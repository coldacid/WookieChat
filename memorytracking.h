/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

#ifndef MEMORYTRACKING_H
#define MEMORYTRACKING_H 1

/*************************************************************************/

#include <exec/types.h>
#include <exec/semaphores.h>
#include <exec/lists.h>

#include <proto/exec.h> /* required for patching */

#ifndef offsetof
#define offsetof(type, member)  __builtin_offsetof(type, member)
#endif

extern void *mempool;

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

#pragma pack(4) /* we need alignment of 4 for compatibly */
struct PoolMemory {
	struct PoolMemory *pm_Succ;
	struct PoolMemory *pm_Pred;
	struct PoolHeader *pm_PoolHeader;
	IPTR               pm_MemoryID;     /* ID to identify this structure */
	IPTR               pm_CheckSum;     /* pointer on PMID avoids  */
	ULONG              pm_AllocSize;    /* size requested by application */
	ULONG              pm_RealSize;     /* size incl. walls */
	char               pm_FunctionName[ PM_FUNCNAME_SIZEOF ];
	BYTE               pm_WallType;     /* random wall value */
	BYTE               pm_RESERVED[3];  /* pad bytes */
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
 #define AllocVec(size,requirements) MemoryAllocVecPooled(mempool,size, (char*) __func__)
 #undef FreeVec
 #define FreeVec(mb)   MemoryFreeVecPooled( mempool, (APTR) mb, (char*) __func__ )
#endif  /* MEMORYPROTECTION_CODE */

void *MemoryCreatePool( ULONG memflags, ULONG puddlesize, ULONG threshsize );
void  MemoryDeletePool( APTR poolheader );

void *MemoryAllocVecPooled( APTR poolheader, ULONG size, char *funcname );
void  MemoryFreeVecPooled( APTR poolheader, APTR memory, char *funcname );

#else /* ENABLE_MEMORYTRACKING */

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
 #undef FreeVec
 #define FreeVec(mb)   MemoryFreeVecPooled( mempool, (APTR) mb )
#endif  /* MEMORYPROTECTION_CODE */

#endif /* ENABLE_MEMORYTRACKING */

/*************************************************************************/

#endif /* MEMORYTRACKING_H */

 
