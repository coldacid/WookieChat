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
#include <SDI_hook.h>

#include <mui/NList_mcc.h>
#include <mui/NListview_mcc.h>
#include <string.h>

#include "locale.h"
#include "muiclass.h"
#include "muiclass_windowsettings.h"
#include "muiclass_aliaslist.h"
#include "version.h"

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
							MUIA_NList_Format        , "BAR,",
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
		FreeMem( msg->entry, sizeof( struct AliasEntry ) );
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
char alias[ ALIASENTRY_ALIAS_SIZEOF + ALIASENTRY_TEXT_SIZEOF + 4 ];
char *text;

	DoMethod( obj, MUIM_NList_Clear );
	for( i = 0 ;  ; i++ ) {
		if( ( text = (char *) DoMethod( msg->dataspace, MUIM_Dataspace_Find, OID_ALI_LIST + i ) ) ) {
			strncpy( alias, text, ALIASENTRY_ALIAS_SIZEOF + ALIASENTRY_TEXT_SIZEOF + 1 );
			if( ( text = strchr( alias, (char) 0x20 ) ) ) {
				*text++ = 0x00;
				DoMethod( obj, MM_ALIASLIST_ADD, alias, text );
			} else {
				DoMethod( obj, MM_ALIASLIST_ADD, alias, (char *) "<none>" );
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
char buffer[ ALIASENTRY_ALIAS_SIZEOF + ALIASENTRY_TEXT_SIZEOF + 4 ];
ULONG i;

	for( i = 0 ;  ; i++ ) {
		ae = NULL;
		DoMethod( obj, MUIM_NList_GetEntry, i, &ae );
		if( ae ) {
			strncpy( buffer, (char *) ae->ae_Alias, ALIASENTRY_ALIAS_SIZEOF );
			strcat(  buffer, (char *) " " );
			strncat( buffer, (char *) ae->ae_Text, ALIASENTRY_TEXT_SIZEOF );
			DoMethod( msg->dataspace, MUIM_Dataspace_Add, buffer, strlen( buffer ) + 1, OID_ALI_LIST + i );
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

	if( ( ae = AllocMem( sizeof( struct AliasEntry ), MEMF_ANY|MEMF_CLEAR ) ) ) {
		strncpy( (char *) ae->ae_Alias, (char *) msg->Alias, ALIASENTRY_ALIAS_SIZEOF );
		strncpy( (char *) ae->ae_Text , (char *) msg->Text , ALIASENTRY_TEXT_SIZEOF );
		DoMethod( obj, MUIM_NList_InsertSingle, ae, MUIV_NList_Insert_Bottom );
		SetAttrs( obj, MUIA_NList_Active, MUIV_NList_Active_Bottom, TAG_DONE );
	}
	return( (ULONG) ae );
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




