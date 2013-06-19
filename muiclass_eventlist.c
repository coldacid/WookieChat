/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

/*
** muiclass_eventlist.c
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
#include <stdio.h>

#include "system.h"
#include "functions.h"
#include "locale.h"
#include "muiclass.h"
#include "muiclass_windowsettings.h"
#include "muiclass_eventlist.h"
#include "version.h"

/*************************************************************************/

/* /// OM_New()
**
*/

/*************************************************************************/

static ULONG OM_New( struct IClass *cl, Object *obj, struct opSet *msg UNUSED )
{
	if( ( obj =  DoSuperNew( cl, obj,
							MUIA_NList_Title         , TRUE,
							MUIA_CycleChain          , 1,
							MUIA_NList_Format        , "BAR,BAR,BAR,BAR",
							MUIA_ObjectID            , OID_EVT_LIST,
							TAG_DONE ) ) ) {
		ULONG i;
		for( i = 0 ; i < EVENTTYPE_LAST ; i++ ) {
			DoMethod( obj, MM_EVENTLIST_ADD, i, 0, NULL, NULL );
		}
	}
	return( (ULONG) obj );
}
/* \\\ */
/* /// OM_Display()
**
*/

/*************************************************************************/

static ULONG OM_Display( struct IClass *cl, Object *obj, struct MUIP_NList_Display *msg )
{
STRPTR *array = msg->strings;
struct EventEntry *ee;

	if( ( ee = msg->entry ) ) {
		*array++ = LGS( MSG_MUICLASS_EVENTLIST_TYPECONNECT + ee->ee_Type );
		*array++ = LGS( MSG_MUICLASS_EVENTLIST_MODENEVER   + ee->ee_Mode );
		*array++ = (STRPTR) ee->ee_Script;
		*array++ = (STRPTR) ee->ee_Text;
	} else {
		*array++ = (STRPTR) LGS( MSG_LV_EVENT  );
		*array++ = (STRPTR) LGS( MSG_LV_MODE   );
		*array++ = (STRPTR) LGS( MSG_LV_SCRIPT );
		*array++ = (STRPTR) LGS( MSG_LV_TEXT   );
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
		FreeMem( msg->entry, sizeof( struct EventEntry ) );
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
struct EventEntry *ee;
ULONG i;
char *text;

	for( i = 0 ;  ; i++ ) {
		ee = NULL;
		DoMethod( obj, MUIM_NList_GetEntry, i, &ee );
		if( ee ) {
			if( ( text = (char *) DoMethod( msg->dataspace, MUIM_Dataspace_Find, OID_EVT_LIST + ee->ee_Type + 1 ) ) ) {
				struct SimpleReadArgsData *srad;
				if( ( srad = SimpleReadArgsParse( "MODE/N/A,SCRIPT/A,TEXT/A", text ) ) ) {
					DoMethod( obj, MM_EVENTLIST_ADD, ee->ee_Type, *((ULONG*) srad->srad_ArgArray[0]), srad->srad_ArgArray[1], srad->srad_ArgArray[2] );
					SimpleReadArgsFree( srad );
				}
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
struct EventEntry *ee;
char buffer[ EVENTENTRY_SCRIPT_SIZEOF + EVENTENTRY_TEXT_SIZEOF + 64 ];
ULONG i;

	for( i = 0 ;  ; i++ ) {
		ee = NULL;
		DoMethod( obj, MUIM_NList_GetEntry, i, &ee );
		if( ee ) {
			sprintf( buffer, "%ld \"%s\" \"%s\"", ee->ee_Mode, ee->ee_Script, ee->ee_Text );
			DoMethod( msg->dataspace, MUIM_Dataspace_Add, buffer, strlen( buffer ) + 1, OID_EVT_LIST + ee->ee_Type + 1 );
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

static ULONG MM_Add( struct IClass *cl, Object *obj, struct MP_EVENTLIST_ADD *msg )
{
struct EventEntry *ee;
ULONG i, newitem = 0;

	/* old item in list ? */
	for( i = 0 ;  ; i++ ) {
		ee = NULL;
		DoMethod( obj, MUIM_NList_GetEntry, i, &ee );
		if( ee ) {
			if( ee->ee_Type == msg->Type ) {
				break;
			}
		} else {
			break;
		}
	}
	/* no item found? Then alloc new entry */
	if( !ee ) {
		ee = AllocMem( sizeof( struct EventEntry ), MEMF_ANY|MEMF_CLEAR );
		newitem++;
	}
	if( ee ) {
		/* fill / update entry */
		ee->ee_Type = msg->Type;
		ee->ee_Mode = msg->Mode;
		strncpy( (char *) ee->ee_Script  , (char *) ( msg->Script ? msg->Script : (STRPTR) "" ), EVENTENTRY_SCRIPT_SIZEOF );
		strncpy( (char *) ee->ee_Text    , (char *) ( msg->Text   ? msg->Text   : (STRPTR) "" ), EVENTENTRY_TEXT_SIZEOF   );
		if( newitem ) { /* only add if entry is new */
			DoMethod( obj, MUIM_NList_InsertSingle, ee, MUIV_NList_Insert_Bottom );
		}
	}
	return( (ULONG) ee );
}
/* \\\ */

/*
** Dispatcher, init and dispose
*/

/* /// MCC_EventList_Dispatcher()
**
*/

/*************************************************************************/

DISPATCHER(MCC_EventList_Dispatcher)
{
    switch (msg->MethodID)
    {
		case OM_NEW                          : return( OM_New                           ( cl, obj, (APTR) msg ) );
		case MUIM_NList_Display              : return( OM_Display                       ( cl, obj, (APTR) msg ) );
		case MUIM_NList_Destruct             : return( OM_Destruct                      ( cl, obj, (APTR) msg ) );
		case MUIM_Import                     : return( OM_Import                        ( cl, obj, (APTR) msg ) );
		case MUIM_Export                     : return( OM_Export                        ( cl, obj, (APTR) msg ) );
		case MM_EVENTLIST_ADD               : return( MM_Add                           ( cl, obj, (APTR) msg ) );
    }
	return( DoSuperMethodA( cl, obj, msg ) );

}
/* \\\ */
/* /// MCC_EventList_InitClass()
**
*/

/*************************************************************************/

ULONG MCC_EventList_InitClass( void )
{
	appclasses[ CLASSID_EVENTLIST ] = MUI_CreateCustomClass( NULL, (ClassID) MUIC_NList, NULL, 0,  (APTR) ENTRY(MCC_EventList_Dispatcher) );
	return( appclasses[ CLASSID_EVENTLIST ] ? MSG_ERROR_NOERROR : MSG_ERROR_UNABLETOSETUPMUICLASS );
}
/* \\\ */
/* /// MCC_EventList_DisposeClass()
**
*/

/*************************************************************************/

void MCC_EventList_DisposeClass( void )
{
	if( appclasses[ CLASSID_EVENTLIST ] ) {
		MUI_DeleteCustomClass( appclasses[ CLASSID_EVENTLIST ] );
		appclasses[ CLASSID_EVENTLIST ] = NULL;
    }
}
/* \\\ */

