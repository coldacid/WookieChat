#include "MemGuardian.h"

#if defined( DEBUG )

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined( __amigaos4__ )

#include <proto/exec.h>

enum {
    /* Memory was recently allocated */
    MS_ALLOCATED,

    /* Memory was recently free'd */
    MS_FREED,

    /* Not used currently */
    MS_PROTECTED
};// MemStatus;

enum {
    /* C */
    AT_MALLOC,

    /* C++ */
    AT_NEW,
    AT_NEW_ARRAY,

    /* Exec library */
    AT_ALLOCABS,
    AT_ALLOCATE,
    AT_ALLOCENTRY,
    AT_ALLOCMEM,
    AT_ALLOCPOOLED,
    AT_ALLOCSIGNAL,
    AT_ALLOCSYSOBJECT,
    AT_ALLOCTRAP,
    AT_ALLOCVEC,
    AT_ALLOCVECPOOLED
};// AllocationType;

#define MG_NAME_BUFFER_SIZE 80

typedef struct
{
    /* For list navigation */
    struct Node _node;

    /* Current status of allocation */
    uint32 _status;

    /* Way of allocation */
    uint32 _type;

    /* Address of allocation */
    void* _address;

    /* File + Line number where allocated */
    char _alloc_file[ MG_NAME_BUFFER_SIZE ] ;

    /* File + Line number where deallocated */
    uint32 _alloc_line;

    char _dealloc_file[ MG_NAME_BUFFER_SIZE ];
    uint32 _dealloc_line;

} MG_allocation;


static struct List MG_list;

static char MG_file[ MG_NAME_BUFFER_SIZE ];
static int MG_line;

/* Avoid starting / finishing MemGuardian twice */
static BOOL bRunning = FALSE;

#define DEBUGOUT printf


/* Work-around function to store info about __FILE__ and __LINE__ when using 'delete' operator */
void MG_set_file_line(const char* file, int line)
{
    strncpy( MG_file, file, MG_NAME_BUFFER_SIZE );
    MG_line = line;
}


void MG_call_init(void)
{
    if ( bRunning )
    {
        DEBUGOUT( "Already Initialized!\n" );
        return;
    }

    bRunning = TRUE;

    DEBUGOUT( "[Mem Guardian] Init\n" );

    IExec->NewList( &MG_list );
}

void MG_call_exit(void)
{
    if ( !bRunning )
    {
        DEBUGOUT( "Already Finished.\n" );
        return;
    }

    bRunning = FALSE;

    DEBUGOUT( "[Mem Guardian] Exit\n" );

    MG_allocation * itr;
    while ( ( itr = (MG_allocation*) IExec->RemTail(&MG_list) ) )
    {
        if ( MS_ALLOCATED == itr->_status )
        {
            DEBUGOUT( "Warning: non-free'd allocation exists: allocated in %s (%ld)\n", itr->_alloc_file, itr->_alloc_line);

            switch ( itr->_type )
            {
            case AT_MALLOC:
                free( itr->_address );
                break;

            case AT_NEW:
                /* operator new was implemented by using malloc() */
                free( itr->_address );
                break;

            case AT_NEW_ARRAY:
                /* operator new [] was implemented by using malloc() */
                free( itr->_address );
                break;

            case AT_ALLOCVEC:
                IExec->FreeVec( itr->_address );
                break;

            default:
                DEBUGOUT( "Unknown allocation method used. Leaking memory.\n" );
            	break;
            }

            itr->_address = NULL;
            itr->_status = MS_FREED;
        }

        /* Remove allocation node */
        IExec->FreeVec( itr );
    }

    IExec->NewList( &MG_list );
}

void * MG_call_malloc(size_t bytes, const char* file, int line)
{
    void* ptr = malloc( bytes );

    if ( ptr )
    {
        DEBUGOUT("Malloc() called in file %s (line %d)\n", file, line);

        MG_allocation * _ptr = (MG_allocation*)IExec->AllocVec( sizeof( MG_allocation ), MEMF_PRIVATE|MEMF_CLEAR );

        if ( _ptr )
        {
            _ptr->_status = MS_ALLOCATED;
            _ptr->_type = AT_MALLOC;
            _ptr->_address = ptr;
            strncpy( _ptr->_alloc_file, file, 80 );
            _ptr->_alloc_line = line;

            IExec->AddTail( &MG_list, (struct Node*) _ptr );
        }
    }

    return ptr;
}

void MG_call_free(void* ptr, const char* file, int line)
{
    BOOL bFound = FALSE;

    MG_allocation * itr = (MG_allocation*) MG_list.lh_Head;
    while ( itr->_node.ln_Succ )
    {
        if ( itr->_address == ptr )
        {
            if ( MS_FREED == itr->_status )
            {
                DEBUGOUT( "Warning: possible attempt to free() already free()'d object in file %s (line %d). Memory was earlier free()'d in file %s (line %ld)\n", file, line, itr->_dealloc_file, itr->_dealloc_line );
            }
            else if ( MS_ALLOCATED == itr->_status )
        	{
            	bFound = TRUE;
        	    if ( AT_MALLOC == itr->_type )
        	    {
        	        free( ptr );
        	        //itr->_address = NULL;
        	        itr->_status = MS_FREED;
                    strncpy( itr->_dealloc_file, file, 80 );
                    itr->_dealloc_line = line;

    				DEBUGOUT("free() called in file %s (line %d)\n", file, line);
        	    }
                else
                {
                    DEBUGOUT("Warning: memory wasn't allocated with malloc() but still trying to free() it.\n");
                    // NOTE: memory remains UNFREED! could be fixed here but unless programmer fixes his bug, freeing in debug version doens't help anyone..
                }
        	}
		}

    	itr = (MG_allocation*) itr->_node.ln_Succ;
    }

    if ( !bFound )
    {
        DEBUGOUT("Warning: couldn't find memory allocation to free() (%p) in file %s (line %d)\n", ptr, file, line);
    }
}


APTR MG_call_AllocVec( ULONG byteSize, ULONG requirements, const char* file, int line )
{
    APTR ptr = IExec->AllocVec( byteSize, requirements );

    if ( ptr )
    {
        DEBUGOUT("AllocVec() called in file %s (line %d)\n", file, line);

        MG_allocation * _ptr = (MG_allocation*) IExec->AllocVec( sizeof( MG_allocation ), MEMF_PRIVATE|MEMF_CLEAR );

        if ( _ptr )
        {
            _ptr->_status = MS_ALLOCATED;
            _ptr->_type = AT_ALLOCVEC;
            _ptr->_address = ptr;
            strncpy( _ptr->_alloc_file, file, 80 );
            _ptr->_alloc_line = line;

            IExec->AddTail( &MG_list, (struct Node*) _ptr );
        }
    }

    return ptr;
}


void MG_call_FreeVec(APTR memoryBlock, const char* file, int line)
{
    BOOL bFound = FALSE;

    MG_allocation * itr = (MG_allocation*) MG_list.lh_Head;
    while ( itr->_node.ln_Succ )
    {
        if ( itr->_address == memoryBlock )
        {
            if ( MS_FREED == itr->_status )
            {
               DEBUGOUT( "Warning: possible attempt to FreeVec() already FreeVec()'d object in file %s (line %d). Memory was earlier FreeVec()'d in file %s (line %ld)\n", file, line, itr->_dealloc_file, itr->_dealloc_line );
            }
            else if ( MS_ALLOCATED == itr->_status )
        	{
	            bFound = TRUE;
        	    if ( AT_ALLOCVEC == itr->_type )
        	    {
        	        IExec->FreeVec( memoryBlock );
        	        //itr->_address = NULL;
        	        itr->_status = MS_FREED;
                    strncpy( itr->_dealloc_file, file, 80 );
                    itr->_dealloc_line = line;

    				DEBUGOUT("FreeVec() called in file %s (line %d)\n", file, line);
        	    }
                else
                {
                    DEBUGOUT("Warning: memory wasn't allocated with AllocVec() but still trying to FreeVec() it.\n");
                    // NOTE: memory remains UNFREED! could be fixed here but unless programmer fixes his bug, freeing in debug version doens't help anyone..
                }
        	}
		}

    	itr = (MG_allocation*) itr->_node.ln_Succ;
    }

    if ( !bFound )
    {
        DEBUGOUT("Warning: couldn't find memory allocation to FreeVec() (%p) in file %s (line %d)\n", memoryBlock, file, line);
	}
}

#if defined( __cplusplus )

void * MG_call_new(size_t bytes, const char* file, int line)
{
    void *ptr = malloc( bytes );

    if ( ptr )
    {
        DEBUGOUT("Operator new called in file %s (line %d)\n", file, line);

        MG_allocation * _ptr = (MG_allocation*) IExec->AllocVec( sizeof( MG_allocation ), MEMF_PRIVATE|MEMF_CLEAR );

        if ( _ptr )
        {
            _ptr->_status = MS_ALLOCATED;
            _ptr->_type = AT_NEW;
            _ptr->_address = ptr;
            strncpy( _ptr->_alloc_file, file, 80 );
            _ptr->_alloc_line = line;

            IExec->AddTail( &MG_list, (struct Node*) _ptr );
        }
    }

    return ptr;
}

void MG_call_delete(void* ptr /*, const char* file, int line*/)
{
    BOOL bFound = FALSE;

    MG_allocation * itr = (MG_allocation*) MG_list.lh_Head;
    while ( itr->_node.ln_Succ )
    {
        if ( itr->_address == ptr )
        {
            if ( MS_FREED == itr->_status )
            {
                DEBUGOUT( "Warning: possible attempt to delete already deleted object in file %s (line %d). Memory was earlier deleted in file %s (line %ld)\n", MG_file, MG_line, itr->_dealloc_file, itr->_dealloc_line );
            }
            else if ( MS_ALLOCATED == itr->_status )
        	{
	            bFound = TRUE;
        	    if ( AT_NEW == itr->_type )
        	    {
                    /* Operator new was implemented by using malloc() ... */
        	        free( ptr );
        	        //itr->_address = NULL;
        	        itr->_status = MS_FREED;
                    strncpy( itr->_dealloc_file, MG_file, 80 );
                    itr->_dealloc_line = MG_line;

    				DEBUGOUT("Operator delete called in file %s (line %d)\n", MG_file, MG_line);
        	    }
                else
                {
                    DEBUGOUT("Warning: memory wasn't allocated with operator new but still trying to delete it.\n");
                    // NOTE: memory remains UNFREED! could be fixed here but unless programmer fixes his bug, freeing in debug version doens't help anyone..
                }
        	}
		}

    	itr = (MG_allocation*) itr->_node.ln_Succ;
    }

    if ( !bFound )
    {
        DEBUGOUT("Warning: couldn't find memory allocation to delete (%p) in file %s (line %d)\n", ptr, MG_file, MG_line);
    }
}


void * MG_call_new_array(size_t bytes, const char* file, int line)
{
    void *ptr = malloc( bytes );

    if ( ptr )
    {
        DEBUGOUT("Operator new [] called in file %s (line %d)\n", file, line);

        MG_allocation * _ptr = (MG_allocation*) IExec->AllocVec( sizeof( MG_allocation ), MEMF_PRIVATE|MEMF_CLEAR );

        if ( _ptr )
        {
            _ptr->_status = MS_ALLOCATED;
            _ptr->_type = AT_NEW;
            _ptr->_address = ptr;
            strncpy( _ptr->_alloc_file, file, 80 );
            _ptr->_alloc_line = line;

            IExec->AddTail( &MG_list, (struct Node*) _ptr );
        }
    }

    return ptr;
}

void MG_call_delete_array(void* ptr /*, const char* file, int line*/)
{
    BOOL bFound = FALSE;

    MG_allocation * itr = (MG_allocation*) MG_list.lh_Head;
    while ( itr->_node.ln_Succ )
    {
        if ( itr->_address == ptr )
        {
            if ( MS_FREED == itr->_status )
            {
                DEBUGOUT( "Warning: possible attempt to delete [] already deleted object in file %s (line %d). Memory was earlier deleted [] in file %s (line %ld)\n", MG_file, MG_line, itr->_dealloc_file, itr->_dealloc_line );
            }
            else if ( MS_ALLOCATED == itr->_status )
        	{
	            bFound = TRUE;
        	    if ( AT_NEW == itr->_type )
        	    {
                    /* Operator new [] was implemented by using malloc() ... */
        	        free( ptr );
        	        //itr->_address = NULL;
        	        itr->_status = MS_FREED;
                    strncpy( itr->_dealloc_file, MG_file, 80 );
                    itr->_dealloc_line = MG_line;

    				DEBUGOUT("Operator delete [] called in file %s (line %d)\n", MG_file, MG_line);
        	    }
                else
                {
                    DEBUGOUT("Warning: memory wasn't allocated with operator new [] but still trying to delete [] it.\n");
                    // NOTE: memory remains UNFREED! could be fixed here but unless programmer fixes his bug, freeing in debug version doens't help anyone..
                }
        	}
		}

    	itr = (MG_allocation*) itr->_node.ln_Succ;
    }

    if ( !bFound )
    {
        DEBUGOUT("Warning: couldn't find memory allocation to delete [] (%p) in file %s (line %d)\n", ptr, MG_file, MG_line);
    }
}

#endif /* __cplusplus */

#endif /* __amigaos4__ */

#endif /* DEBUG */
