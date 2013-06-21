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
#include <intuition/intuition.h>
#include <exec/ports.h>
#include <devices/timer.h>

#include <proto/icon.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/utility.h>
#include <proto/muimaster.h>
#include <proto/intuition.h>
#include <proto/locale.h>

#include "system.h"
#include "locale.h"
#include "version.h"
#include "functions.h"

/***************************************************************************/

/*
** intuition
*/

/* /// ShowRequester()
**
*/

/***************************************************************************/

ULONG ShowRequester( ULONG message, ULONG gadgets, IPTR *args )
{
	if( MUIMasterBase ) {
		return( MUI_RequestA( NULL, NULL, 0, (char *) LGS( MSG_REQUESTER_TITLE ), (char *) LGS( gadgets ), (char *) LGS( message ), args ) );
	} else {
		if( IntuitionBase ) {
			struct EasyStruct es = { sizeof( struct EasyStruct ), 0, LGS( MSG_REQUESTER_TITLE ), LGS( message ), LGS( gadgets ) };
			return( EasyRequestArgs( NULL, &es, NULL, args ) );
		} else {
			char *str, *tmp;
			if( ( str = AllocVec( strlen( (char *) LGS( message ) ), MEMF_ANY ) ) ) {
				strcpy( str, (char *) LGS( message ) );
				while( ( tmp = strchr( str, '\n' ) ) ) {
					*tmp = 0x20; /* replace \n by space */
				}
				VPrintf( (CONST_STRPTR) str, args );
				FreeVec( str );
			}
		}
	}
	return( 0 );
}
/* \\\ */

/*
** exec functions
*/

#ifdef __AROS__
struct ExecBase         *SysBase;
struct LocaleBase       *LocaleBase;
struct Library          *MUIMasterBase;
struct IntuitionBase    *IntuitionBase;
struct Library          *DOSBase;
struct Library          *AslBase;
struct Library          *IconBase;
struct GfxBase          *GfxBase;
struct UtilityBase      *UtilityBase;
struct Library          *SocketBase;
struct Library          *DataTypesBase;
struct Library          *CodesetsBase;
#elif __amigaos4__
struct Library          *SysBase;
struct Library          *LocaleBase;
struct Library          *MUIMasterBase;
struct Library          *IntuitionBase;
struct Library          *DOSBase;
struct Library          *AslBase;
struct Library          *IconBase;
struct Library          *GfxBase;
struct Library          *UtilityBase;
struct Library          *SocketBase;
struct Library          *DataTypesBase;
struct Library          *CodesetsBase;
struct LocaleIFace      *ILocale;
struct MUIMasterIFace   *IMUIMaster;
struct IntuitionIFace   *IIntuition;
struct AslIFace         *IAsl;
struct IconIFace        *IIcon;
struct GraphicsIFace    *IGraphics;
struct UtilityIFace     *IUtility;
struct SocketIFace      *ISocket;
struct DataTypesIFace   *IDataTypes;
struct CodesetsIFace    *ICodesets;
#elif __MORPHOS__
struct ExecBase         *SysBase;
struct Library          *LocaleBase;
struct Library          *MUIMasterBase;
struct IntuitionBase    *IntuitionBase;
struct DosLibrary       *DOSBase;
struct Library          *AslBase;
struct Library          *IconBase;
struct GfxBase          *GfxBase;
struct Library          *UtilityBase;
struct Library          *SocketBase;
struct Library          *DataTypesBase;
struct Library          *CodesetsBase;
#else
struct ExecBase         *SysBase;
struct Library          *LocaleBase;
struct Library          *MUIMasterBase;
struct Library          *IntuitionBase;
struct DosLibrary       *DOSBase;
struct Library          *AslBase;
struct Library          *IconBase;
struct Library          *GfxBase;
struct Library          *UtilityBase;
struct Library          *SocketBase;
struct Library          *DataTypesBase;
struct Library          *CodesetsBase;
#endif

static struct LibraryData LibraryArray[] = {
	LIBMACRO( "locale.library"           ,  0, LIBFLAGSF_OPTIONAL, LocaleBase      , ILocale    ), /* first to get localised error output */
	LIBMACRO( "intuition.library"        , 33, 0                 , IntuitionBase   , IIntuition ), /* second to deliver requesters */
	LIBMACRO( "muimaster.library"        ,  0, 0                 , MUIMasterBase   , IMUIMaster ), /* third for mui requesters */
	LIBMACRO( "dos.library"              ,  0, 0                 , DOSBase         , IDos       ), /* now all other stuff */
	LIBMACRO( "asl.library"              ,  0, 0                 , AslBase         , IAsl       ),
	LIBMACRO( "icon.library"             ,  0, 0                 , IconBase        , IIcon      ),
	LIBMACRO( "graphics.library"         ,  0, 0                 , GfxBase         , IGfx       ),
	LIBMACRO( "utility.library"          ,  0, 0                 , UtilityBase     , IUtility   ),
	LIBMACRO( "bsdsocket.library"        ,  0, LIBFLAGSF_OPTIONAL, SocketBase      , ISocket    ),
	LIBMACRO( "datatypes.library"        , 43, 0                 , DataTypesBase   , IDataTypes ),
	LIBMACRO( "codesets.library"         ,  6, 0                 , CodesetsBase    , ICodesets  ),
    LIBMACROEND
};

/* /// Libraries_Open()
**
*/

/*************************************************************************/

BOOL Libraries_Open(void)
{

ULONG i = 0;
BOOL result = FALSE;

	while( LibraryArray[ i ].Name ) {
		if( ( (*(LibraryArray[ i ].LibBase)) = OpenLibrary( (_ub_cs) LibraryArray[ i ].Name, LibraryArray[ i ].Version ) ) ) {
#ifdef __amigaos4__
			if( ((*(LibraryArray[ i ].IBase)) = (APTR) GetInterface( (*(LibraryArray[ i ].LibBase)), "main", 1, NULL ) ) ) {
				i++;
				continue;
			} else {
				CloseLibrary( (APTR) (*(LibraryArray[i].IBase)) );
				(*(LibraryArray[ i ].IBase)) = NULL;
			}
#else
			i++;
			continue;
#endif
		}
		if( !( LibraryArray[ i ].Flags & LIBFLAGSF_OPTIONAL ) ) {
			IPTR args[2];
			args[0] = (IPTR) LibraryArray[ i ].Name;
			args[1] = (IPTR) LibraryArray[ i ].Version;
			ShowRequester( MSG_ERROR_UNABLETOOPENLIBRARY, MSG_REQUESTER_OK_GADGETS, &args[0] );
            result = TRUE;
            break;
		}
		i++;
	}
	return( result );
}
/* \\\ Libraries_Open */
/* /// Libraries_Close()
**
*/

/*************************************************************************/

void Libraries_Close(void)
{
ULONG i = 0;

	 while( LibraryArray[ i ].Name ) {
#ifdef __amigaos4__
		if( (*(LibraryArray[i].IBase)) ) {
			DropInterface( (APTR) (*(LibraryArray[ i ].IBase)) );
			//(*(LibraryArray[i].IBase)) = NULL; /* removed because of startup.o */
        }
#endif
		if( (*(LibraryArray[i].LibBase)) ) {
			CloseLibrary( (struct Library *) (*(LibraryArray[i].LibBase)) );
			//(*(LibraryArray[i].LibBase)) = NULL; /* removed because of startup.o */
        }
        i++;
    }
}
/* \\\ Libraries_Close */

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

/*
** Timer Device
*/

struct Library *TimerBase;

/* /// DeviceTimer_Open()
**
** Open timer.device and add timer handler
*/

/*************************************************************************/

ULONG DeviceTimer_Open( struct Device_Timer *dt )
{
ULONG result;

	result = MSG_ERROR_UNABLETOCREATEMSGPORT;
	if( ( dt->MsgPort = CreateMsgPort() ) ) {
		result = MSG_ERROR_UNABLETOCREATEIOREQUEST;
		if( ( dt->IORequest = CreateIORequest( dt->MsgPort, sizeof( struct timerequest ) ) ) ) {
			result = MSG_ERROR_UNABLETOOPENTIMERDEVICE;
			if( !OpenDevice( (CONST_STRPTR) "timer.device", UNIT_MICROHZ, (struct IORequest *) dt->IORequest, 0L ) ) {

				TimerBase = (APTR) dt->IORequest->tr_node.io_Device;

				dt->IORequest->tr_node.io_Message.mn_Node.ln_Type = NT_REPLYMSG;
				dt->SignalMask = ( 1<<dt->MsgPort->mp_SigBit );
				dt->OpenCheck = 1;
				result = MSG_ERROR_NOERROR;
	        }
	    }
	}
	return( result );
}
/* \\\ */
/* /// DeviceTimer_Close()
**
** Close timer.device and remove timer handler
*/

/*************************************************************************/

void DeviceTimer_Close( struct Device_Timer *dt )
{
	if( dt->OpenCheck ) {

		while( !CheckIO( &dt->IORequest->tr_node) ) {
			AbortIO( &dt->IORequest->tr_node );
			WaitIO( &dt->IORequest->tr_node );  /* wait for outstanding Timer */
        }
		CloseDevice( &dt->IORequest->tr_node );
		dt->OpenCheck = 0;
    }

	if( dt->IORequest ) {
		DeleteIORequest( &dt->IORequest->tr_node );
		dt->IORequest = NULL;
    }

	if( dt->MsgPort ) {
		DeleteMsgPort( dt->MsgPort );
		dt->MsgPort = NULL;
    }
}
/* \\\ */
/* /// DeviceTimer_SendRequest()
**
*/

/*************************************************************************/

void DeviceTimer_SendRequest( struct Device_Timer *dt )
{
	if( !CheckIO( &dt->IORequest->tr_node ) ) {
		WaitIO( &dt->IORequest->tr_node );
	}
	dt->IORequest->tr_node.io_Command = TR_ADDREQUEST;

	dt->IORequest->tr_time.tv_secs    = 1;
	dt->IORequest->tr_time.tv_micro   = 0;

	SendIO( &dt->IORequest->tr_node );
}
/* \\\ */

/*
** Exec Lists
*/

/* /// List_GetListFromNode()
**
*/

/*************************************************************************/

APTR List_GetListFromNode( APTR snode )
{
struct Node *node;

	if( snode ) {
        for( node = snode ; node->ln_Succ ; node = node->ln_Succ ) {}
		return( (APTR) ( (( (IPTR) node )) - ((IPTR) sizeof(IPTR)) ) );
    } else {
		return( NULL );
    }
}
/* \\\ */

