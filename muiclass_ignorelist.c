/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

/*
** muiclass_ignorelist.c
*/

#include <libraries/mui.h>
#include <prefs/prefhdr.h>
#include <proto/muimaster.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <SDI_hook.h>

#include <string.h>
#include <stdio.h>

#include "system.h"
#include "functions.h"
#include "locale.h"
#include "muiclass.h"
#include "muiclass_windowsettings.h"
#include "muiclass_ignorelist.h"
#include "version.h"

#define LINEBUFFER_SIZEOF 0x2000

/*************************************************************************/

/* /// OM_New()
**
*/

/*************************************************************************/

static ULONG OM_New( struct IClass *cl, Object *obj, struct opSet *msg UNUSED )
{

	debug( "%s (%ld) %s - Class: 0x00009528x Object: 0x00009528x \n", __FILE__, __LINE__, __func__, cl, obj );

	return( (ULONG) DoSuperNew( cl, obj,
							MUIA_NList_Title         , TRUE,
							MUIA_CycleChain          , 1,
							MUIA_NList_Format        , "BAR,BAR,BAR,BAR",
							MUIA_ObjectID            , OID_IGN_LIST,
							TAG_DONE ) );
}
/* \\\ */
/* /// OM_Display()
**
*/

/*************************************************************************/

static ULONG OM_Display( struct IClass *cl, Object *obj, struct MUIP_NList_Display *msg )
{
STRPTR *array = msg->strings;
struct IgnoreEntry *ie;

	if( ( ie = msg->entry ) ) {
		*array++ = (STRPTR) ie->ie_Pattern;
		*array++ = ( ie->ie_Flags & IGNOREENTRYF_TEXT ) ? LGS( MSG_LV_IGNORE ) : LGS( MSG_LV_ALLOW );
		*array++ = ( ie->ie_Flags & IGNOREENTRYF_CTCP ) ? LGS( MSG_LV_IGNORE ) : LGS( MSG_LV_ALLOW );
		*array++ = ( ie->ie_Flags & IGNOREENTRYF_DCC  ) ? LGS( MSG_LV_IGNORE ) : LGS( MSG_LV_ALLOW );
	} else {
		*array++ = (STRPTR) LGS( MSG_LV_PATTERN );
		*array++ = (STRPTR) LGS( MSG_LV_TEXT );
		*array++ = (STRPTR) LGS( MSG_LV_CTCP );
		*array++ = (STRPTR) LGS( MSG_LV_DCC  );
    }
	return( 0 );
}
/* \\\ */
/* /// OM_Destruct()
**
*/

/*************************************************************************/

static ULONG OM_Destruct( struct IClass *cl, Object *obj, struct MUIP_NList_Destruct *msg )
{

	debug( "%s (%ld) %s - Class: 0x00009528x Object: 0x00009528x \n", __FILE__, __LINE__, __func__, cl, obj );

	if( msg->entry ) {
		FreeVec( msg->entry );
    }
	return( 0 );
}
/* \\\ */
#if 0 /* we save as ascii */
/* /// OM_Import()
**
*/

/*************************************************************************/

static ULONG OM_Import( struct IClass *cl, Object *obj, struct MUIP_Import *msg )
{
ULONG i;
char *text;

	debug( "%s (%ld) %s - Class: 0x00009528x Object: 0x00009528x \n", __FILE__, __LINE__, __func__, cl, obj );

	DoMethod( obj, MUIM_NList_Clear );
	for( i = 0 ;  ; i++ ) {
		if( ( text = (char *) DoMethod( msg->dataspace, MUIM_Dataspace_Find, OID_IGN_LIST + 1 + i ) ) ) {
			struct SimpleReadArgsData *srad;
			if( ( srad = SimpleReadArgsParse( "PATTERN/A,TEXT/S,CTCP/S,DCC/S", text ) ) ) {
				DoMethod( obj, MM_IGNORELIST_ADD,  srad->srad_ArgArray[0], ( srad->srad_ArgArray[1] ? IGNOREENTRYF_TEXT : 0 ) | ( srad->srad_ArgArray[2] ? IGNOREENTRYF_CTCP : 0 ) | ( srad->srad_ArgArray[3] ? IGNOREENTRYF_DCC : 0 ) );
				SimpleReadArgsFree( srad );
			}
		} else {
			break;
		}
	}
	return( DoSuperMethodA( cl, obj, (Msg) msg ) );
}
/* \\\ */
/* /// OM_Export()
**
*/

/*************************************************************************/

static ULONG OM_Export( struct IClass *cl, Object *obj, struct MUIP_Import *msg )
{
struct IgnoreEntry *ie;
char buffer[ IGNOREENTRY_PATTERN_SIZEOF + 64 ];
ULONG i;

	debug( "%s (%ld) %s - Class: 0x00009528x Object: 0x00009528x \n", __FILE__, __LINE__, __func__, cl, obj );

	for( i = 0 ;  ; i++ ) {
		ie = NULL;
		DoMethod( obj, MUIM_NList_GetEntry, i, &ie );
		if( ie ) {
			sprintf( buffer, "%s %s%s%s", ie->ie_Pattern,
							(( ie->ie_Flags & IGNOREENTRYF_TEXT ) ? " TEXT" : "" ),
							(( ie->ie_Flags & IGNOREENTRYF_CTCP ) ? " CTCP" : "" ),
							(( ie->ie_Flags & IGNOREENTRYF_DCC  ) ? " DCC"  : "" ) );
			DoMethod( msg->dataspace, MUIM_Dataspace_Add, buffer, strlen( buffer ) + 1, OID_IGN_LIST + i + 1 );
		} else {
			break;
		}
	}
	return( DoSuperMethodA( cl, obj, (Msg) msg ) );
}
/* \\\ */
#endif
/* /// MM_Add()
**
*/

/*************************************************************************/

static ULONG MM_Add( struct IClass *cl, Object *obj, struct MP_IGNORELIST_ADD *msg )
{
struct IgnoreEntry *ie;

	debug( "%s (%ld) %s - Class: 0x00009528x Object: 0x00009528x \n", __FILE__, __LINE__, __func__, cl, obj );

	if( ( ie = AllocVec( sizeof( struct IgnoreEntry ), MEMF_ANY|MEMF_CLEAR ) ) ) {
		ie->ie_Flags = msg->Flags;
		strncpy( ie->ie_Pattern, (char *) ( msg->Pattern ? msg->Pattern : LGS( MSG_MUICLASS_IGNORELIST_DEFAULTPATTERN ) ), IGNOREENTRY_PATTERN_SIZEOF );
		DoMethod( obj, MUIM_NList_InsertSingle, ie, MUIV_NList_Insert_Bottom );
		SetAttrs( obj, MUIA_NList_Active, MUIV_NList_Active_Bottom, TAG_DONE );
	}
	return( (ULONG) ie );
}
/* \\\ */
/* /// MM_ExportListAsText()
**
** This is not the real settings save. It just allows to export
** the ignore list in text format.
*/

/*************************************************************************/

static ULONG MM_ExportListAsText( struct IClass *cl, Object *obj, struct MP_IGNORELIST_EXPORTLISTASTEXT *msg )
{
BPTR handle;
ULONG i;
struct IgnoreEntry *ie;

	debug( "%s (%ld) %s - Class: 0x00009528x Object: 0x00009528x \n", __FILE__, __LINE__, __func__, cl, obj );

	if( ( handle = Open( (_ub_cs) msg->Name, MODE_NEWFILE ) ) ) {
		for( i = 0 ;  ; i++ ) {
			ie = NULL;
			DoMethod( obj, MUIM_NList_GetEntry, i, &ie );
			if( ie ) {
				FPuts( handle, (CONST_STRPTR)     ie->ie_Pattern );
				FPuts( handle, (CONST_STRPTR) ( ( ie->ie_Flags & IGNOREENTRYF_TEXT ) ? " TEXT" : "" ) );
				FPuts( handle, (CONST_STRPTR) ( ( ie->ie_Flags & IGNOREENTRYF_CTCP ) ? " CTCP" : "" ) );
				FPuts( handle, (CONST_STRPTR) ( ( ie->ie_Flags & IGNOREENTRYF_DCC  ) ? " DCC"  : "" ) );
				FPutC( handle,        '\n'  );
			} else {
	            break;
			}
		}
		Close( handle );
    }
	return( 0 );
}
/* \\\ */
/* /// MM_ImportListAsText()
*/


/*************************************************************************/

static ULONG MM_ImportListAsText( struct IClass *cl, Object *obj, struct MP_IGNORELIST_IMPORTLISTASTEXT *msg )
{
BPTR handle;
char *linebuffer;

	debug( "%s (%ld) %s - Class: 0x00009528x Object: 0x00009528x \n", __FILE__, __LINE__, __func__, cl, obj );

	DoMethod( obj, MUIM_NList_Clear );

	if( ( linebuffer = AllocVec( LINEBUFFER_SIZEOF, MEMF_ANY ) ) ) {
		if( ( handle = Open( (_ub_cs) msg->Name, MODE_OLDFILE ) ) ) {
			while( FGets( handle, (STRPTR) linebuffer, LINEBUFFER_SIZEOF - 1 ) ) {
				struct SimpleReadArgsData *srad;
				if( ( srad = SimpleReadArgsParse( "PATTERN/A,TEXT/S,CTCP/S,DCC/S", linebuffer ) ) ) {
					DoMethod( obj, MM_IGNORELIST_ADD, srad->srad_ArgArray[0], ( srad->srad_ArgArray[1] ? IGNOREENTRYF_TEXT : 0 ) | ( srad->srad_ArgArray[2] ? IGNOREENTRYF_CTCP : 0 ) | ( srad->srad_ArgArray[3] ? IGNOREENTRYF_DCC : 0 ) );
					SimpleReadArgsFree( srad );
				}
			}
			Close( handle );
        }
		FreeVec( linebuffer );
    }
	return( 0 );
}
/* \\\ */

/*
** Dispatcher, init and dispose
*/

/* /// MCC_IgnoreList_Dispatcher()
**
*/

/*************************************************************************/

DISPATCHER(MCC_IgnoreList_Dispatcher)
{
    switch (msg->MethodID)
    {
		case OM_NEW                          : return( OM_New                           ( cl, obj, (APTR) msg ) );
		case MUIM_NList_Display              : return( OM_Display                       ( cl, obj, (APTR) msg ) );
		case MUIM_NList_Destruct             : return( OM_Destruct                      ( cl, obj, (APTR) msg ) );
#if 0 /* we save as ascii */
		case MUIM_Import                     : return( OM_Import                        ( cl, obj, (APTR) msg ) );
		case MUIM_Export                     : return( OM_Export                        ( cl, obj, (APTR) msg ) );
#endif
		case MM_IGNORELIST_ADD               : return( MM_Add                           ( cl, obj, (APTR) msg ) );
		case MM_IGNORELIST_IMPORTLISTASTEXT  : return( MM_ImportListAsText              ( cl, obj, (APTR) msg ) );
		case MM_IGNORELIST_EXPORTLISTASTEXT  : return( MM_ExportListAsText              ( cl, obj, (APTR) msg ) );
    }
	return( DoSuperMethodA( cl, obj, msg ) );

}
/* \\\ */
/* /// MCC_IgnoreList_InitClass()
**
*/

/*************************************************************************/

ULONG MCC_IgnoreList_InitClass( void )
{
	appclasses[ CLASSID_IGNORELIST ] = MUI_CreateCustomClass( NULL, (ClassID) MUIC_NList, NULL, 0,  (APTR) ENTRY(MCC_IgnoreList_Dispatcher) );
	return( appclasses[ CLASSID_IGNORELIST ] ? MSG_ERROR_NOERROR : MSG_ERROR_UNABLETOSETUPMUICLASS );
}
/* \\\ */
/* /// MCC_IgnoreList_DisposeClass()
**
*/

/*************************************************************************/

void MCC_IgnoreList_DisposeClass( void )
{
	if( appclasses[ CLASSID_IGNORELIST ] ) {
		MUI_DeleteCustomClass( appclasses[ CLASSID_IGNORELIST ] );
		appclasses[ CLASSID_IGNORELIST ] = NULL;
    }
}
/* \\\ */




