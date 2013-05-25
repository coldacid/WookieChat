#ifndef MEMGUARDIAN_H
#   define MEMGUARDIAN_H

#include <sys/types.h>

#if defined( __amigaos4__ )

#include <exec/types.h>

#if defined( __cplusplus )
extern "C" {
#endif

#ifdef DEBUG

/* Prototypes for wrappers */
	void MG_call_init(void);
	void MG_call_exit(void);

	void * MG_call_malloc(size_t bytes, const char* file, int line);
	void MG_call_free(void *ptr, const char* file, int line);

    APTR MG_call_AllocVec(ULONG byteSize, ULONG requirements, const char* file, int line);
    void MG_call_FreeVec(APTR memoryBlock, const char* file, int line);

    // TODO... Add more wrappers

    //APTR MG_call_AllocAbs(ULONG byteSize, APTR location);
    //APTR Allocate(

//   APTR MG_call_AllocVecPooled();
//   void MG_call_FreeVecPooled();

/* Preprocessor magic */

#	define MG_Init() MG_call_init()
#   define MG_Exit() MG_call_exit()

#	define MG_malloc( bytes ) MG_call_malloc( bytes, __FILE__, __LINE__ )
#	define MG_free( ptr ) MG_call_free( ptr, __FILE__, __LINE__ )

#   define MG_AllocVec( byteSize, requirements ) MG_call_AllocVec( byteSize, requirements, __FILE__, __LINE__ )
#   define MG_FreeVec( memoryBlock ) MG_call_FreeVec( memoryBlock, __FILE__, __LINE__ )

//#   define MG_AllocVecPooled( poolHeader, memSize ) IExec->AllocVecPooled( poolHeader, memSize, __FILE__, __LINE__ )
//#   define MG_FreeVecPooled( poolHeader, memory ) IExec->FreeVecPooled( poolHeader, memory, __FILE__, __LINE__ )

#if defined( __cplusplus )

	void * MG_call_new(size_t bytes, const char* file, int line);
    void MG_call_delete(void* ptr /*, const char* file, int line*/);

	void * MG_call_new_array(size_t bytes, const char* file, int line);
    void MG_call_delete_array(void* ptr /*, const char* file, int line*/);

    void MG_set_file_line( const char* file, int line );

    /* Overload global new & delete operators */
    inline void * operator new(size_t bytes, const char* file, int line)
    {
    	return MG_call_new( bytes, file, line );
    }

    inline void * operator new [] (size_t bytes, const char* file, int line)
    {
    	return MG_call_new_array( bytes, file, line );
    }

    inline void operator delete(void* ptr)
    {
    	MG_call_delete( ptr );
    }

    inline void operator delete [] (void* ptr)
    {
    	MG_call_delete_array( ptr );
    }

/* For consistency */
#define MG_new new
#define MG_delete delete

#define new DEBUG_NEW
#define DEBUG_NEW new(__FILE__, __LINE__)

/* This is a hack. It first sets the file & line, then calls overloaded operator */
#define delete MG_set_file_line(__FILE__, __LINE__); delete

#endif /* __cplusplus */

#else /* DEBUG */

#include <stdlib.h>
#include <proto/exec.h>

/* Normal, non-debug macros */

#   define MG_Init()
# 	define MG_Exit()

#   define MG_malloc( bytes ) malloc( bytes )
#	define MG_free( ptr ) free( ptr )

#   define MG_AllocVec( byteSize, requirements ) IExec->AllocVec( byteSize, requirements )
#   define MG_FreeVec( memoryBlock ) IExec->FreeVec( memoryBlock )

//#   define MG_AllocVecPooled( poolHeader, memSize ) IExec->AllocVecPooled( poolHeader, memSize )
//#   define MG_FreeVecPooled( poolHeader, memory ) IExec->FreeVecPooled( poolHeader, memory )

#if defined( __cplusplus )

#define MG_new new
#define MG_delete delete

#endif /* __cplusplus */

#endif /* DEBUG */


#if defined( __cplusplus )
}
#endif

#endif /* __amigaos4__ */

#endif /* MEMGUARDIAN_H */
