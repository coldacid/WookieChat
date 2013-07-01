/*
**  MemoryTracking V4.0
**
** 	(C)1991 - 2013 by Guido Mersmann
**
**  This file turns any AllocVec() call into AllocVecPooled().
**  Inbetween, when memory tracking itself is enabled it, all
**  allocations will be traced. Missaligned or false memory blocks
**  and even hits on the upper/lower boundary will be traced
**  and dumped as error on standard debug output.
**
**  DISTRIBUTION
**
**  The contents of this file is considered to be PD and free to
**  use in any kind of software as long as the original credits
**  stay intact.
**
*/

#ifndef MEMORYTRACKING_H
#define MEMORYTRACKING_H 1

/*************************************************************************/

#include <exec/types.h>
#include <exec/semaphores.h>
#include <exec/lists.h>

#include <proto/exec.h> /* required for patching */

#ifndef offsetof
 #ifdef __builtin_offsetof
  #define offsetof(type, member)  __builtin_offsetof(type, member)
 #endif
#endif

extern void *memorypool;

/* structures only needed when tracking is enabled */ 
#if ENABLE_MEMORYTRACKING

/*************************************************************************/

/*
** memory tracking base structure
*/

struct MemoryTracking {
	struct List    	       	   mt_PoolHeaderList;
	struct SignalSemaphore     mt_PoolSemaphore;
	struct SignalSemaphore     mt_MemorySemaphore;
	BYTE                       mt_WallType;
};

/*************************************************************************/

/*
** pool header structure
*/

struct PoolHeader {
	struct PoolHeader *ph_Succ;
	struct PoolHeader *ph_Pred;
	void              *ph_Pool;
	struct List        ph_MemoryList;
	BYTE               ph_WallType;
};

#define PM_WALLSIZE 8182
#define PM_FUNCNAME_SIZEOF 32
#define ID_POOL MAKE_ID('P','O','O','L')	/* identifies pooled memory */

/*************************************************************************/

/*
** pool header structure
*/

#pragma pack(1) /* we need alignment of 4 for compatibly */
struct PoolMemory {
	struct PoolMemory *pm_Succ;
	struct PoolMemory *pm_Pred;
	struct PoolHeader *pm_PoolHeader;
	IPTR               pm_CheckSum;     /* pointer on PMID avoids  */
	IPTR               pm_MemoryID;     /* ID to identify this structure */
	ULONG              pm_AllocSize;    /* size requested by application */
	ULONG              pm_RealSize;     /* size incl. walls */
	char               pm_FunctionName[ PM_FUNCNAME_SIZEOF ];
	BYTE               pm_WallType;     /* random wall value */
	BYTE               pm_RESERVED[1];  /* pad bytes */
	BYTE               pm_WallLower[ PM_WALLSIZE ];
};
#pragma pack() /* we need alignment of 4 for compatibly */

#define PM_PRESIZE offsetof( struct PoolMemory, pm_WallLower[ PM_WALLSIZE ] )

/*************************************************************************/

/*
** prototypes
*/

#ifndef MEMORYPROTECTION_CODE
 #undef AllocVec
 #define AllocVec(size,requirements) MemoryAllocVecPooled( memorypool, size, (char*) __func__)
 #undef FreeVec
 #define FreeVec(mb)                 MemoryFreeVecPooled ( memorypool, (APTR) mb, (char*) __func__ )
#endif  /* MEMORYPROTECTION_CODE */

void *MemoryCreatePool( ULONG memflags, ULONG puddlesize, ULONG threshsize );
void  MemoryDeletePool( APTR poolheader );

void *MemoryAllocVecPooled( APTR poolheader, ULONG size, char *funcname );
void  MemoryFreeVecPooled ( APTR poolheader, APTR memory, char *funcname );

/*
** Macros for direct usage
*/

#define Memory_AllocPooled(pool,size)   MemoryAllocVecPooled(pool,size, (char*) __func__)
#define Memory_FreePooled(pool,memory)  MemoryFreeVecPooled(pool,memory, (char*) __func__)
#define Memory_CreatePool  MemoryCreatePool
#define Memory_DeletePool  MemoryDeletePool

#else /* ENABLE_MEMORYTRACKING == 0 */

/*************************************************************************/

/*
** prototypes
*/

#define MemoryCreatePool( memflags, puddlesize, threshsize ) CreatePool( memflags, puddlesize, threshsize )
#define MemoryDeletePool( poolheader )                       DeletePool( poolheader )

void  MemoryFreeVecPooled( APTR poolheader, APTR memory );

/*
** For alloc the code is the same, but the free function
** provides a check for zero test, so it needs to be called
** instead of FreeVecPooled directly
*/

#ifndef MEMORYPROTECTION_CODE
 #define MemoryAllocVecPooled AllocVecPooled
 #undef  AllocVec
 #define AllocVec(ms,requirements)  AllocVecPooled( memorypool, ms )
 #undef  FreeVec
 #define FreeVec(mb)                MemoryFreeVecPooled( memorypool, (APTR) mb )

 #define Memory_AllocPooled(pool,size)   MemoryAllocVecPooled(pool,size)
 #define Memory_FreePooled(pool,memory)  MemoryFreeVecPooled(pool,memory)
 #define Memory_CreatePool  MemoryCreatePool
 #define Memory_DeletePool  MemoryDeletePool

#endif  /* MEMORYPROTECTION_CODE */
#endif /* ENABLE_MEMORYTRACKING */




/*************************************************************************/

#endif /* MEMORYTRACKING_H */

 
