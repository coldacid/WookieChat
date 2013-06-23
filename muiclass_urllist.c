/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

/*
** muiclass_urllist.c
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
#include "muiclass_urllist.h"
#include "version.h"

#define LINEBUFFER_SIZEOF 0x2000

/*************************************************************************/

/* /// OM_New()
**
*/

/*************************************************************************/

static ULONG OM_New( struct IClass *cl, Object *obj, struct opSet *msg UNUSED )
{
	return( (ULONG) DoSuperNew( cl, obj,
							MUIA_NList_Title         , TRUE,
							MUIA_CycleChain          , 1,
							MUIA_ObjectID            , OID_URL_LIST,
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
struct URLEntry *ue;

	if( ( ue = msg->entry ) ) {
		*array++ = ue->ue_URL;
	} else {
		*array++ = (STRPTR) LGS( MSG_LV_URL );
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

	DoMethod( obj, MUIM_NList_Clear );
	for( i = 0 ;  ; i++ ) {
		if( ( text = (char *) DoMethod( msg->dataspace, MUIM_Dataspace_Find, OID_URL_LIST + 1 + i ) ) ) {
			DoMethod( obj, MM_URLLIST_ADD,  text );
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
struct URLEntry *ue;
ULONG i;

	for( i = 0 ;  ; i++ ) {
		ue = NULL;
		DoMethod( obj, MUIM_NList_GetEntry, i, &ie );
		if( ue ) {
			DoMethod( msg->dataspace, MUIM_Dataspace_Add, ue->ue_URL, strlen( ue->ue_URL ) + 1, OID_URL_LIST + i + 1 );
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

static ULONG MM_Add( struct IClass *cl, Object *obj, struct MP_URLLIST_ADD *msg )
{
struct URLEntry *ue;
LONG i, entries;

	if( !Strnicmp( msg->URL, (_ub_cs) "http://", 6 ) ||
		!Strnicmp( msg->URL, (_ub_cs) "https://", 6 ) ||
		!Strnicmp( msg->URL, (_ub_cs) "www", 3 ) ) {

		GetAttr( MUIA_NList_Entries, obj, (IPTR*) &entries );

		/* make sure entry is not in list */
		for( i = 0 ; i < entries ; i++ ) {
			ue = NULL;
			DoMethod( obj, MUIM_NList_GetEntry, i, &ue );
			if( ue ) {
				if( !Stricmp( ue->ue_URL, msg->URL ) ) {
                    break;
				}
            }
        }
		if( i == entries ) { /* not in list -> add and save */
			if( ( ue = AllocVec( sizeof( struct URLEntry ), MEMF_ANY|MEMF_CLEAR ) ) ) {
				strncpy( (char *) ue->ue_URL, (char *) msg->URL, URLENTRY_URL_SIZEOF );
				DoMethod( obj, MUIM_NList_InsertSingle, ue, MUIV_NList_Insert_Bottom );
			}
		}
	}
	return( (ULONG) ue );
}
/* \\\ */
/* /// MM_ExportListAsText()
**
** This is not the real settings save. It just allows to export
** the url list in text format.
*/

/*************************************************************************/

static ULONG MM_ExportListAsText( struct IClass *cl, Object *obj, struct MP_URLLIST_EXPORTLISTASTEXT *msg )
{
BPTR handle;
ULONG i;
struct URLEntry *ue;

	if( ( handle = Open( (_ub_cs) msg->Name, MODE_NEWFILE ) ) ) {
		for( i = 0 ;  ; i++ ) {
			ue = NULL;
			DoMethod( obj, MUIM_NList_GetEntry, i, &ue );
			if( ue ) {
				FPuts( handle, (CONST_STRPTR) ue->ue_URL );
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

static ULONG MM_ImportListAsText( struct IClass *cl, Object *obj, struct MP_URLLIST_IMPORTLISTASTEXT *msg )
{
BPTR handle;
char *linebuffer;

	DoMethod( obj, MUIM_NList_Clear );

	if( ( linebuffer = AllocVec( LINEBUFFER_SIZEOF, MEMF_ANY ) ) ) {
		if( ( handle = Open( (_ub_cs) msg->Name, MODE_OLDFILE ) ) ) {
			while( FGets( handle, (STRPTR) linebuffer, LINEBUFFER_SIZEOF - 1 ) ) {
				struct SimpleReadArgsData *srad;
				if( ( srad = SimpleReadArgsParse( "URL/A/F", linebuffer ) ) ) {
					DoMethod( obj, MM_URLLIST_ADD, srad->srad_ArgArray[0] );
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

/* /// MCC_URLList_Dispatcher()
**
*/

/*************************************************************************/

DISPATCHER(MCC_URLList_Dispatcher)
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
		case MM_URLLIST_ADD               : return( MM_Add                           ( cl, obj, (APTR) msg ) );
		case MM_URLLIST_IMPORTLISTASTEXT  : return( MM_ImportListAsText              ( cl, obj, (APTR) msg ) );
		case MM_URLLIST_EXPORTLISTASTEXT  : return( MM_ExportListAsText              ( cl, obj, (APTR) msg ) );
    }
	return( DoSuperMethodA( cl, obj, msg ) );

}
/* \\\ */
/* /// MCC_URLList_InitClass()
**
*/

/*************************************************************************/

ULONG MCC_URLList_InitClass( void )
{
	appclasses[ CLASSID_URLLIST ] = MUI_CreateCustomClass( NULL, (ClassID) MUIC_NList, NULL, 0,  (APTR) ENTRY(MCC_URLList_Dispatcher) );
	return( appclasses[ CLASSID_URLLIST ] ? MSG_ERROR_NOERROR : MSG_ERROR_UNABLETOSETUPMUICLASS );
}
/* \\\ */
/* /// MCC_URLList_DisposeClass()
**
*/

/*************************************************************************/

void MCC_URLList_DisposeClass( void )
{
	if( appclasses[ CLASSID_URLLIST ] ) {
		MUI_DeleteCustomClass( appclasses[ CLASSID_URLLIST ] );
		appclasses[ CLASSID_URLLIST ] = NULL;
    }
}
/* \\\ */




