/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

/*
** functions.c
*/

#include <workbench/icon.h>
#include <workbench/startup.h>

#include <proto/icon.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/utility.h>

#include "locale.h"
#include "intern.h"
#include "version.h"
#include "functions.h"

/***************************************************************************/

/*
** DOS Functions
*/

/* /// Dos_ShowFailure()
**
*/

/***************************************************************************/

void Dos_ShowFailure( void )
{
	PrintFault( IoErr(), (CONST_STRPTR) APPLICATIONNAME );
}
/* \\\ */

/*
** Icon
*/

/* /// Icon_GetPutDiskObject()
**
*/

/***************************************************************************/

struct DiskObject *Icon_GetPutDiskObject( struct DiskObject *diskobj )
{
#define ds_SIZEOF 0x200
char *ds[ds_SIZEOF];
STRPTR name;
BPTR lock;
struct DiskObject *result = 0L;
struct WBArg *wbarg;

    if( wbmessage ) {

        wbarg = wbmessage->sm_ArgList;

        lock = (BPTR) wbarg->wa_Lock;
		name = (STRPTR) wbarg->wa_Name;

        if( wbmessage->sm_NumArgs >= 2) {
            wbarg++;

			if( ( name = (STRPTR) wbarg->wa_Name ) ) {
                lock = (BPTR) wbarg->wa_Lock;
				name = (STRPTR) wbarg->wa_Name;
            }
        }
		if( !name ) {
			return( NULL );
        }

    } else {

		if( GetProgramName( (STRPTR) ds, ds_SIZEOF ) ) {

			name = FilePart( (APTR) &ds );
            lock = GetProgramDir();

		} else {
			return( NULL );
		}
    }
	lock = CurrentDir( lock );
	if( !diskobj ) {
		result = GetDiskObjectNew( name );
	} else {
		result  = (APTR)(ULONG) PutDiskObject( name, diskobj );
	}
	CurrentDir( lock );

	return( result );
}
/* \\\ */
/* /// Icon_ToolTypeGetBool()
**
*/

/***************************************************************************/

BOOL Icon_ToolTypeGetBool( struct DiskObject *o, STRPTR tooltype, BOOL defvalue )
{
STRPTR toolstring;

	if( o ) {
#if defined(__MORPHOS__)
		if( ( toolstring = (STRPTR) FindToolType( (CONST STRPTR *) o->do_ToolTypes, (CONST STRPTR) tooltype ) ) ) {
#elif defined( __amigaos4__)
		if( ( toolstring = (STRPTR) FindToolType( (STRPTR *) o->do_ToolTypes, (CONST_STRPTR) tooltype ) ) ) {
#else
		if( ( toolstring = (STRPTR) FindToolType( (const STRPTR *) o->do_ToolTypes, (const STRPTR) tooltype ) ) ) {
#endif
			return( ( (!Stricmp( toolstring, (STRPTR) "YES" ) ) || ( !Stricmp( toolstring, (STRPTR) "ON" ) ) || (!Stricmp( toolstring, (STRPTR) "TRUE" ) ) ) ? TRUE : FALSE );
		}
	}
	return( defvalue );
}
/* \\\ */
/* /// Icon_ToolTypeGetInteger()
**
*/

/***************************************************************************/
long Icon_ToolTypeGetInteger( struct DiskObject *o, STRPTR tooltype, long defvalue )
{

STRPTR toolstring;
LONG value;

	if( o ) {
#if defined(__MORPHOS__)
		if( ( toolstring = (STRPTR) FindToolType( (CONST STRPTR *) o->do_ToolTypes, (CONST STRPTR) tooltype ) ) ) {
#elif defined( __amigaos4__)
		if( ( toolstring = (STRPTR) FindToolType( (STRPTR *) o->do_ToolTypes, (CONST_STRPTR) tooltype ) ) ) {
#else
		if( ( toolstring = (STRPTR) FindToolType( (const STRPTR *) o->do_ToolTypes, (const STRPTR) tooltype ) ) ) {
#endif
			if( -1 != StrToLong( toolstring, &value ) ) {
				return( value );
			}
		}
	}
	return( defvalue );

}
/* \\\ */
/* /// Icon_ToolTypeGetString()
**
*/

/***************************************************************************/

STRPTR Icon_ToolTypeGetString( struct DiskObject *o, STRPTR tooltype, STRPTR deftooltype )
{
STRPTR result = NULL;

	if( o ) {
#if defined(__MORPHOS__)
		result =  (STRPTR) FindToolType( (CONST STRPTR *) o->do_ToolTypes, (CONST STRPTR) tooltype );
#elif defined( __amigaos4__)
		result =  (STRPTR) FindToolType( (STRPTR *) o->do_ToolTypes, (CONST_STRPTR) tooltype );
#else
		result =  (STRPTR) FindToolType( (const STRPTR *) o->do_ToolTypes, (const STRPTR) tooltype );
#endif
	}
    if( !result) {
		return( deftooltype );
    } else {
		return( result );
    }
}
/* \\\ */

/*
** WBMessage
*/

/* /// WBMessage_Data
**
*/

/*************************************************************************/

struct WBStartup *wbmessage;

/* \\\ */
/* /// WBMessage_Get()
**
*/

/*************************************************************************/

void WBMessage_Get(void)
{
struct Process *process;

	process = (struct Process *) FindTask( NULL );

	if( !( process->pr_CLI ) ) {
		WaitPort( &( process->pr_MsgPort ) );

		wbmessage = (struct WBStartup *) GetMsg( &( process->pr_MsgPort ) );

    }
}
/* \\\ */
/* /// WBMessage_Reply()
**
*/

/*************************************************************************/

void WBMessage_Reply( void )
{
    if( wbmessage) {
        Forbid();
		ReplyMsg( (struct Message *) wbmessage );
    }
}
/* \\\ */

/*
** SimpleArgumentParsing
*/

/* /// SimpleReadArgsParse()
**
*/

/*************************************************************************/


struct SimpleReadArgsData *SimpleReadArgsParse( char *templatestring, char *parsestring )
{
struct SimpleReadArgsData *srad;
ULONG length;

	if( ( length = strlen( parsestring ) ) ) {
		if( ( srad = AllocVec( sizeof( struct SimpleReadArgsData ) + length + 4, MEMF_ANY|MEMF_CLEAR ) ) ) {
			if( ( srad->srad_rda = (struct RDArgs *) AllocDosObject( DOS_RDARGS, NULL ) ) ) {
				strcpy( &srad->srad_Buffer[ 0 ], parsestring );
				srad->srad_Buffer[ length++ ] = 0x0a;
				srad->srad_Buffer[ length++ ] = 0x00;
				srad->srad_rda->RDA_Source.CS_Length = length;
				srad->srad_rda->RDA_Source.CS_CurChr = 0;
				srad->srad_rda->RDA_Source.CS_Buffer = (STRPTR) &srad->srad_Buffer[ 0 ];

				if( ( srad->srad_rd = ReadArgs( (CONST_STRPTR) templatestring, (LONG*) &srad->srad_ArgArray[ 0 ], srad->srad_rda ) ) ) {
					return( srad );
				}
			}
			SimpleReadArgsFree( srad );
		}
	}
	return( NULL );
}
/* \\\ */
/* /// SimpleReadArgsFree()
**
*/

/*************************************************************************/

void SimpleReadArgsFree( struct SimpleReadArgsData *srad )
{
	if( srad ) {
		if( srad->srad_rd ) {
			FreeArgs( srad->srad_rd );
		}
		if( srad->srad_rda ) {
			FreeDosObject( DOS_RDARGS, srad->srad_rda );
		}
		FreeVec( srad );
	}
}
/* \\\ */

