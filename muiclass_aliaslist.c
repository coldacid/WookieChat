/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

/*
** muiclass_aliaslist.c
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

#include "functions.h"
#include "system.h"
#include "locale.h"
#include "muiclass.h"
#include "muiclass_windowsettings.h"
#include "muiclass_aliaslist.h"
#include "version.h"

#define LINEBUFFER_SIZEOF 0x2000

/*************************************************************************/

/* /// OM_New()
**
*/

/*************************************************************************/

static ULONG OM_New( struct IClass *cl, Object *obj, struct opSet *msg UNUSED )
{
	debug( "%s (%ld) %s - Class: 0x00007835x Object: 0x00007835x \n", __FILE__, __LINE__, __func__, cl, obj );

	return( (ULONG) DoSuperNew( cl, obj,
							MUIA_NList_Title         , TRUE,
							MUIA_CycleChain          , 1,
							MUIA_NList_Format        , "BAR,",
							MUIA_ObjectID            , OID_ALI_LIST,
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
struct AliasEntry *ae;

	if( ( ae = msg->entry ) ) {
		*array++ = ae->ae_Alias;
		*array++ = ae->ae_Text;
	} else {
		*array++ = (STRPTR) LGS( MSG_LV_ALIAS );
		*array++ = (STRPTR) LGS( MSG_LV_TEXT );
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
		if( ( text = (char *) DoMethod( msg->dataspace, MUIM_Dataspace_Find, OID_ALI_LIST + 1 + i ) ) ) {
			struct SimpleReadArgsData *srad;
			if( ( srad = SimpleReadArgsParse( "ALIAS/A,TEXT/A", text ) ) ) {
				DoMethod( obj, MM_ALIASLIST_ADD, srad->srad_ArgArray[0], srad->srad_ArgArray[1] );
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
struct AliasEntry *ae;
char buffer[ ALIASENTRY_ALIAS_SIZEOF + ALIASENTRY_TEXT_SIZEOF + 64 ];
ULONG i;

	for( i = 0 ;  ; i++ ) {
		ae = NULL;
		DoMethod( obj, MUIM_NList_GetEntry, i, &ae );
		if( ae ) {
			sprintf( buffer, "\"%s\" \"%s\"", ae->ae_Alias, ae->ae_Text );
			DoMethod( msg->dataspace, MUIM_Dataspace_Add, buffer, strlen( buffer ) + 1, OID_ALI_LIST + i + 1 );
		} else {
			break;
		}
	}
	return( DoSuperMethodA( cl, obj, (Msg) msg ) );
}
/* \\\ */
/* /// MM_Add()
**
*/

/*************************************************************************/

static ULONG MM_Add( struct IClass *cl, Object *obj, struct MP_ALIASLIST_ADD *msg )
{
struct AliasEntry *ae;

	if( ( ae = AllocVec( sizeof( struct AliasEntry ), MEMF_ANY|MEMF_CLEAR ) ) ) {
		strncpy( (char *) ae->ae_Alias, (char *) ( msg->Alias ? msg->Alias : LGS( MSG_MUICLASS_ALIASLIST_DEFAULTALIAS ) ), ALIASENTRY_ALIAS_SIZEOF );
		strncpy( (char *) ae->ae_Text , (char *) ( msg->Text  ? msg->Alias : LGS( MSG_MUICLASS_ALIASLIST_DEFAULTTEXT  ) ), ALIASENTRY_TEXT_SIZEOF  );
		DoMethod( obj, MUIM_NList_InsertSingle, ae, MUIV_NList_Insert_Bottom );
		SetAttrs( obj, MUIA_NList_Active, MUIV_NList_Active_Bottom, TAG_DONE );
	}
	return( (ULONG) ae );
}
/* \\\ */
/* /// MM_ExportListAsText()
*/

/*************************************************************************/

static ULONG MM_ExportListAsText( struct IClass *cl, Object *obj, struct MP_ALIASLIST_EXPORTLISTASTEXT *msg )
{
BPTR handle;
ULONG i;
struct AliasEntry *ae;

	if( ( handle = Open( (_ub_cs) msg->Name, MODE_NEWFILE ) ) ) {
		for( i = 0 ;  ; i++ ) {
			ae = NULL;
			DoMethod( obj, MUIM_NList_GetEntry, i, &ae );
			if( ae ) {
				FPuts( handle, (CONST_STRPTR) ae->ae_Alias );
				FPuts( handle, (CONST_STRPTR) " " );
				FPuts( handle, (CONST_STRPTR) ae->ae_Text );
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

static ULONG MM_ImportListAsText( struct IClass *cl, Object *obj, struct MP_ALIASLIST_IMPORTLISTASTEXT *msg )
{
BPTR handle;
char *linebuffer;

	DoMethod( obj, MUIM_NList_Clear );

	if( ( linebuffer = AllocVec( LINEBUFFER_SIZEOF, MEMF_ANY ) ) ) {
		if( ( handle = Open( (_ub_cs) msg->Name, MODE_OLDFILE ) ) ) {
			while( FGets( handle, (STRPTR) linebuffer, LINEBUFFER_SIZEOF - 1 ) ) {
				struct SimpleReadArgsData *srad;
				if( ( srad = SimpleReadArgsParse( "ALIAS/A,NAME/A/F", linebuffer ) ) ) {
					DoMethod( obj, MM_ALIASLIST_ADD,  srad->srad_ArgArray[0], srad->srad_ArgArray[1] );
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

/* /// MCC_AliasList_Dispatcher()
**
*/

/*************************************************************************/

DISPATCHER(MCC_AliasList_Dispatcher)
{
    switch (msg->MethodID)
    {
		case OM_NEW                          : return( OM_New                           ( cl, obj, (APTR) msg ) );
		case MUIM_NList_Display              : return( OM_Display                       ( cl, obj, (APTR) msg ) );
		case MUIM_NList_Destruct             : return( OM_Destruct                      ( cl, obj, (APTR) msg ) );
		case MUIM_Import                     : return( OM_Import                        ( cl, obj, (APTR) msg ) );
		case MUIM_Export                     : return( OM_Export                        ( cl, obj, (APTR) msg ) );
		case MM_ALIASLIST_ADD                : return( MM_Add                           ( cl, obj, (APTR) msg ) );
		case MM_ALIASLIST_IMPORTLISTASTEXT   : return( MM_ImportListAsText              ( cl, obj, (APTR) msg ) );
		case MM_ALIASLIST_EXPORTLISTASTEXT   : return( MM_ExportListAsText              ( cl, obj, (APTR) msg ) );
    }
	return( DoSuperMethodA( cl, obj, msg ) );

}
/* \\\ */
/* /// MCC_AliasList_InitClass()
**
*/

/*************************************************************************/

ULONG MCC_AliasList_InitClass( void )
{
	appclasses[ CLASSID_ALIASLIST ] = MUI_CreateCustomClass( NULL, (ClassID) MUIC_NList, NULL, 0,  (APTR) ENTRY(MCC_AliasList_Dispatcher) );
	return( appclasses[ CLASSID_ALIASLIST ] ? MSG_ERROR_NOERROR : MSG_ERROR_UNABLETOSETUPMUICLASS );
}
/* \\\ */
/* /// MCC_AliasList_DisposeClass()
**
*/

/*************************************************************************/

void MCC_AliasList_DisposeClass( void )
{
	if( appclasses[ CLASSID_ALIASLIST ] ) {
		MUI_DeleteCustomClass( appclasses[ CLASSID_ALIASLIST ] );
		appclasses[ CLASSID_ALIASLIST ] = NULL;
    }
}
/* \\\ */




