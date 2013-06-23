/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

/*************************************************************************/

/*
** MemoryTracking
*/

/*************************************************************************/

#include <exec/types.h>
#include <exec/semaphores.h>
#include <exec/lists.h>

#include <proto/exec.h>

#define MEMORYPROTECTION_CODE /* avoid patching this code */

#include "memorytracking.h"
#include "system.h"

/*************************************************************************/

#if ENABLE_MEMORYTRACKING
static struct MemoryTracking mt;

/* /// MemoryCreatePool()
*/

/*************************************************************************/

void *MemoryCreatePool( ULONG memflags, ULONG puddlesize, ULONG threshsize )
{
void *pool;
struct PoolHeader *ph;

/* if this is the first pool, we need to init the semaphores */

	if( !( mt.mt_PoolSemaphore.ss_WaitQueue.mlh_Head ) ) {
		InitSemaphore( &mt.mt_PoolSemaphore );
		InitSemaphore( &mt.mt_MemorySemaphore );
		NEWLIST( &mt.mt_PoolHeaderList );
	}

/* Now create a pool, alloc a node and apply the poolheader, so we can track it */

	ObtainSemaphore( &mt.mt_PoolSemaphore );

	if( ( pool = CreatePool( memflags, puddlesize, threshsize ) ) ) {
		if( ( ph = AllocVec( sizeof( struct PoolHeader ), ( MEMF_ANY | MEMF_CLEAR ) ) ) ) {
			NEWLIST( &ph->ph_MemoryList );
			ph->ph_Pool     = pool;
			ph->ph_WallType = (*((ULONG*) pool ) >> 6 ) & 0xff; /* random value from address */
			AddTail( &mt.mt_PoolHeaderList, (struct Node *) ph );
		}
	}
	ReleaseSemaphore( &mt.mt_PoolSemaphore );

/* return to sender */

	return( ph );
}

/* \\\ */
/* /// MemoryDeletePool()
*/

/*************************************************************************/

void MemoryDeletePool( void *pool )
{
struct PoolHeader *ph;
struct PoolMemory *pm, *pmnext;

	ObtainSemaphore( &mt.mt_PoolSemaphore );

	for( ph = (APTR) mt.mt_PoolHeaderList.lh_Head  ; ph->ph_Succ ; ph = ph->ph_Succ ) {
		if( (APTR) ph == (APTR) pool ) { /* it is a pool created by our function */

			ObtainSemaphore( &mt.mt_MemorySemaphore );

			for( pm = (APTR) ph->ph_MemoryList.lh_Head ; ( pmnext = pm->pm_Succ ) ; pm = pmnext ) {
				debug("\n\n%78m*\nMemoryDeletePool:\n\n"
					"  Forgot to free pool memory! Pool: %08lx  Memory: %08lx!\n\n"
					"  Allocated by: '%s()'\n"
					"  Contents: %08lx (Size: %08lx)\n"
					"%512lh\n\n%78m*\n",
					ph, &pm->pm_WallLower[ PM_WALLSIZE ],
					pm->pm_FunctionName,
					&pm->pm_WallLower[ PM_WALLSIZE ], pm->pm_AllocSize,
					&pm->pm_WallLower[ PM_WALLSIZE ]);
				
				MemoryFreeVecPooled( ph, &pm->pm_WallLower[ PM_WALLSIZE ], (char*) __func__ );
	        }
			ReleaseSemaphore( &mt.mt_MemorySemaphore );
			/* now the pool memory list should be empty, so delete real pool */
			DeletePool( ph->ph_Pool );
			Remove( (struct Node *) ph );
			/* free pool header */
			FreeVec( ph );
			ReleaseSemaphore( &mt.mt_PoolSemaphore );
			return;
		}
	}
	debug("\n\n%78m*\nMemoryDeletePool:\n\n  Tried to delete unknown pool: %08lx!\n%78m*\n", ph );
	ReleaseSemaphore( &mt.mt_PoolSemaphore );
}

/* \\\ */
/* /// MemoryAllocVecPooled()
*/

/*************************************************************************/

APTR MemoryAllocVecPooled( APTR ph, ULONG size, char *funcname )
{
struct PoolMemory *pm;
BYTE *wall1;
BYTE *wall2;
ULONG i;

	if( !size ) {
		debug( "\n\n%78m*8\nMemoryAllocPooled:\n\n  Warning zero length alloc!\n%78m*\n" );
	}
	i = PM_PRESIZE + size + PM_WALLSIZE;

	if( ( pm = (APTR) AllocPooled( ( (struct PoolHeader *) ph)->ph_Pool, i ) ) ) {
		pm->pm_PoolHeader         = (struct PoolHeader *) ph;
		pm->pm_MemoryID           = ID_POOL;
		pm->pm_AllocSize          = size;
		pm->pm_RealSize           = i;
		pm->pm_WallType           = ((struct PoolHeader *) ph)->ph_WallType++; /* each block gets different type */
		for( i = 0 ; i < PM_FUNCNAME_SIZEOF - 1 ; i++ ) {
			pm->pm_FunctionName[i] = funcname[ i ];
		}
		/* get wall start */
		wall1 = (BYTE *) pm->pm_WallLower;
		wall2 = wall1 + PM_WALLSIZE + size;

		/* fill wall with wall type */
		for( i = 0 ;i < PM_WALLSIZE ; i++ ) {
			*wall1++ = pm->pm_WallType;
			*wall2++ = pm->pm_WallType;
        }

		ObtainSemaphore( &mt.mt_MemorySemaphore );
		AddTail( &((struct PoolHeader *) ph)->ph_MemoryList, (struct Node *) pm );
		ReleaseSemaphore( &mt.mt_MemorySemaphore );

		pm = (APTR) ((IPTR) pm) + PM_PRESIZE;
	}
	return( pm );
}
/* \\\ */
/* /// MemoryFreeVecPooled()
*/

/*************************************************************************/

void MemoryFreeVecPooled( APTR poolheader, APTR memory, char *funcname )
{
struct PoolMemory *pm, *tmp_pm;
BYTE *wall;
ULONG i, size;

	if( memory ) {
		pm = (APTR) (((IPTR) memory ) - PM_PRESIZE );
		if( pm->pm_MemoryID != ID_POOL ) {
			debug("\n\n%78m*\nMemoryFreePooled:\n\n"
				"  Tried to free unkown memory as pooled: %08lx! (Size: unknown)"
				"\n%40lh\n\n%78m*\n",
			memory,
			memory );
			return; /* we do not free, as this would probably cause a big crash */
		}
/* now check if the PoolHeader is correct */
		if( pm->pm_PoolHeader != poolheader ) {
			debug("\n\n%78m*\nMemoryFreePooled:\n\n"
				"  PoolHeader wrong for memory block: %08lx!\n"
				"  Contents: %08lx (Size: %08lx)\n"
				"%128lh\n\n%78m*\n",
				memory, memory, pm->pm_AllocSize, memory );

			poolheader = pm->pm_PoolHeader;
		}

/* The memory block is confirmed to be pooled type debug.
** If the memory block isn't in list, then it's tried to
** be freed twice!
*/
		ObtainSemaphore( &mt.mt_MemorySemaphore );
		for( tmp_pm = (APTR) pm->pm_PoolHeader->ph_MemoryList.lh_Head ; tmp_pm->pm_Succ ; tmp_pm = tmp_pm->pm_Succ ) {
			if( tmp_pm == pm ) {
                break;
            }
        }
		if( tmp_pm->pm_Succ ) {
			Remove( (struct Node *) pm );
        } else {
			debug("\n\n%78m*\nMemoryFreePooled:\n\n"
				"  Tried to free node twice: %08lx!\n%78m*\n",
				memory );
			ReleaseSemaphore( &mt.mt_MemorySemaphore );
            return;
        }
		ReleaseSemaphore( &mt.mt_MemorySemaphore );

/* Now the memory is confirmed to be pooled and freeable.
** But first it is time to check our memory wall for holes
*/
		wall = (BYTE *) pm->pm_WallLower;

		for( i = 0 ; i < PM_WALLSIZE ; i++ ) {
			if( *wall++ != pm->pm_WallType ) {
				debug("\n\n%78m*\nMemoryFreePooled:\n\n"
					  "	 Wall Hit: Memory: %08lx caused lower wall hit at offset %ld!\n\n"
					  "  Allocated by: '%s()'\n"
					  "  Contents: %08lx (Size: %08lx)\n\n"
					  "%256lh\n"
					  "Wall: %08lx - 128 (Wallsize: %ld)\n%256lh\n\n%78m*\n",
						memory, i,
						pm->pm_FunctionName,
						memory, pm->pm_AllocSize,
						memory,
						&wall[-128], PM_WALLSIZE, &wall[-128] );
                break;
            }
        }

		wall = (BYTE*) ((IPTR) pm ) + PM_PRESIZE + pm->pm_AllocSize;

		for( i = 0 ; i < PM_WALLSIZE ; i++ ) {
			if( *wall++ != pm->pm_WallType ) {
				debug("\n\n%78m*\nMemoryFreePooled:\n\n"
					  "	 Wall Hit: Memory: %08lx caused upper wall hit at offset %ld!\n\n"
					  "  Allocated by: '%s()'\n"
					  "  Contents: %08lx (Size: %08lx)\n\n"
					  "%256lh\n"
					  "Wall: %08lx - 128 (Wallsize: %ld)\n%256lh\n\n%78m*\n",
						memory, i,
						pm->pm_FunctionName,
						memory, pm->pm_AllocSize,
						memory,
						&wall[-128], PM_WALLSIZE, &wall[-128] );
                break;
            }
        }

/* fill memory to cause even more trouble */


		size = pm->pm_RealSize; /* we need the size to free */
		wall = (APTR) pm;

		for( i = 0 ; i < size ; i++ ) {
			*wall++ = 0x00;
		}

/* Finaly dump memory */
		FreePooled( ((struct PoolHeader *) poolheader)->ph_Pool, pm, size );
    }
}
/* \\\ */

/*************************************************************************/

#else /* ENABLE_MEMORYTRACKING */

/* /// MemoryFreeVecPooled()
*/

/*************************************************************************/

void MemoryFreeVecPooled( APTR poolheader, APTR memory )
{
	if( memory ) {
		FreeVecPooled( poolheader, memory );
    }
}
/* \\\ */

#endif /* ENABLE_MEMORYTRACKING */
