/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

/*
** muiclass_buttonlist.c
*/

#define NODEBUG

#include <libraries/mui.h>
#include <prefs/prefhdr.h>
#include <proto/muimaster.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/exec.h>
#include <SDI_hook.h>

#include <string.h>
#include <stdio.h>

#include "functions.h"
#include "system.h"
#include "locale.h"
#include "muiclass.h"
#include "muiclass_windowsettings.h"
#include "muiclass_buttonlist.h"
#include "version.h"

/*************************************************************************/

/* /// OM_New()
**
*/

/*************************************************************************/

static IPTR OM_New( struct IClass *cl, Object *obj, struct opSet *msg UNUSED )
{

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	return( (IPTR) DoSuperNew( cl, obj,
							MUIA_NList_Title         , TRUE,
							MUIA_CycleChain          , 1,
							MUIA_NList_Format        , "BAR,",
							MUIA_ObjectID            , OID_BUT_LIST,
							TAG_DONE ) );
}
/* \\\ */
/* /// OM_Display()
**
*/

/*************************************************************************/

static IPTR OM_Display( struct IClass *cl, Object *obj, struct MUIP_NList_Display *msg )
{
STRPTR *array = msg->strings;
struct ButtonEntry *be;

	if( ( be = msg->entry ) ) {
		*array++ = be->be_Name;
		*array++ = be->be_Command;
	} else {
		*array++ = (STRPTR) LGS( MSG_LV_NAME );
		*array++ = (STRPTR) LGS( MSG_LV_COMMAND );
    }
	return( 0 );
}
/* \\\ */
/* /// OM_Destruct()
**
*/

/*************************************************************************/

static IPTR OM_Destruct( struct IClass *cl, Object *obj, struct MUIP_NList_Destruct *msg )
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

static IPTR OM_Import( struct IClass *cl, Object *obj, struct MUIP_Import *msg )
{
ULONG i;
char *text;

	DoMethod( obj, MUIM_NList_Clear );
	for( i = 0 ;  ; i++ ) {
		if( ( text = (char *) DoMethod( msg->dataspace, MUIM_Dataspace_Find, OID_BUT_LIST + i + 1 ) ) ) {
			struct SimpleReadArgsData *srad;
			if( ( srad = SimpleReadArgsParse( "NAME/A,COMMAND/A", text ) ) ) {
				DoMethod( obj, MM_BUTTONLIST_ADD, srad->srad_ArgArray[0], srad->srad_ArgArray[1] );
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

static IPTR OM_Export( struct IClass *cl, Object *obj, struct MUIP_Import *msg )
{
struct ButtonEntry *be;
char buffer[ BUTTONENTRY_NAME_SIZEOF + BUTTONENTRY_COMMAND_SIZEOF + 64 ];
ULONG i;

	for( i = 0 ;  ; i++ ) {
		be = NULL;
		DoMethod( obj, MUIM_NList_GetEntry, i, &be );
		if( be ) {
			sprintf( buffer, "\"%s\" \"%s\"", be->be_Name, be->be_Command );
			DoMethod( msg->dataspace, MUIM_Dataspace_Add, buffer, strlen( buffer ) + 1, OID_BUT_LIST + i + 1 );
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

static IPTR MM_Add( struct IClass *cl, Object *obj, struct MP_BUTTONLIST_ADD *msg )
{
struct ButtonEntry *be;

	if( ( be = AllocVec( sizeof( struct ButtonEntry ), MEMF_ANY|MEMF_CLEAR ) ) ) {
		strncpy( (char *) be->be_Name    , (char *) ( msg->Name     ? msg->Name    : LGS( MSG_MUICLASS_BUTTONLIST_DEFAULTNAME    ) ), BUTTONENTRY_NAME_SIZEOF );
		strncpy( (char *) be->be_Command , (char *) ( msg->Command  ? msg->Command : LGS( MSG_MUICLASS_BUTTONLIST_DEFAULTCOMMAND ) ), BUTTONENTRY_COMMAND_SIZEOF );
		DoMethod( obj, MUIM_NList_InsertSingle, be, MUIV_NList_Insert_Bottom );
		SetAttrs( obj, MUIA_NList_Active, MUIV_NList_Active_Bottom, TAG_DONE );
	}
	return( (IPTR) be );
}
/* \\\ */


/*
** Dispatcher, init and dispose
*/

/* /// MCC_ButtonList_Dispatcher()
**
*/

/*************************************************************************/

DISPATCHER(MCC_ButtonList_Dispatcher)
{
    switch (msg->MethodID)
    {
		case OM_NEW                          : return( OM_New                           ( cl, obj, (APTR) msg ) );
		case MUIM_NList_Display              : return( OM_Display                       ( cl, obj, (APTR) msg ) );
		case MUIM_NList_Destruct             : return( OM_Destruct                      ( cl, obj, (APTR) msg ) );
		case MUIM_Import                     : return( OM_Import                        ( cl, obj, (APTR) msg ) );
		case MUIM_Export                     : return( OM_Export                        ( cl, obj, (APTR) msg ) );
		case MM_BUTTONLIST_ADD               : return( MM_Add                           ( cl, obj, (APTR) msg ) );
    }
	return( DoSuperMethodA( cl, obj, msg ) );

}
/* \\\ */
/* /// MCC_ButtonList_InitClass()
**
*/

/*************************************************************************/

ULONG MCC_ButtonList_InitClass( void )
{
	appclasses[ CLASSID_BUTTONLIST ] = MUI_CreateCustomClass( NULL, (ClassID) MUIC_NList, NULL, 0,  (APTR) ENTRY(MCC_ButtonList_Dispatcher) );
	return( appclasses[ CLASSID_BUTTONLIST ] ? MSG_ERROR_NOERROR : MSG_ERROR_UNABLETOSETUPMUICLASS );
}
/* \\\ */
/* /// MCC_ButtonList_DisposeClass()
**
*/

/*************************************************************************/

void MCC_ButtonList_DisposeClass( void )
{
	if( appclasses[ CLASSID_BUTTONLIST ] ) {
		MUI_DeleteCustomClass( appclasses[ CLASSID_BUTTONLIST ] );
		appclasses[ CLASSID_BUTTONLIST ] = NULL;
    }
}
/* \\\ */




