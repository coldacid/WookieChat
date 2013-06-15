/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

/*
** muiclass_serverlist.c
*/

#include <libraries/mui.h>
#include <prefs/prefhdr.h>
#include <proto/muimaster.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <SDI_hook.h>

#include <mui/NList_mcc.h>
#include <mui/NListview_mcc.h>
#include <string.h>
#include <stdio.h>

#include "locale.h"
#include "muiclass.h"
#include "muiclass_windowsettings.h"
#include "muiclass_serverlist.h"
#include "muiclass_channellist.h"
#include "muiclass_nicklist.h"
#include "version.h"

/*************************************************************************/

/*
** gadgets used by this class
*/

enum
{
GID_NICKLIST = 0,
GID_CHANNELLIST,
GID_LAST
};

/*
** data used by this class
*/

struct mccdata
{
	Object                *mcc_ClassObjects[ GID_LAST ];
};

/* /// OM_New()
**
*/

/*************************************************************************/

static ULONG OM_New( struct IClass *cl, Object *obj, struct opSet *msg UNUSED )
{
	return( (ULONG) (obj = (Object *) DoSuperNew( cl, obj,
													MUIA_NList_Title         , TRUE,
													MUIA_CycleChain          , 1,
													MUIA_NList_Format        , "BAR,BAR,BAR,BAR",
													TAG_DONE ) ) );

}
/* \\\ */
/* /// OM_Set()
**
*/

/*************************************************************************/

static ULONG OM_Set( struct IClass *cl, Object *obj, struct opSet *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
struct TagItem *tag;
struct TagItem *tstate;

	for( tstate = msg->ops_AttrList ; ( tag = NextTagItem( &tstate ) ) ; ) {
		ULONG tidata = tag->ti_Data;
        switch( tag->ti_Tag ) {
			case MA_SERVERLIST_CHANNELLISTOBJ:
				mccdata->mcc_ClassObjects[ GID_CHANNELLIST ] = (Object *) tidata;
				break;
			case MA_SERVERLIST_NICKLISTOBJ:
				mccdata->mcc_ClassObjects[ GID_NICKLIST    ] = (Object *) tidata;
				break;
		}
    }
	return( DoSuperMethodA( cl, obj,(Msg) msg ) );
}
/* \\\ */

/* /// OM_Display()
**
*/

/*************************************************************************/

static ULONG OM_Display( struct IClass *cl, Object *obj, struct MUIP_NList_Display *msg )
{
STRPTR *array = msg->strings;
struct ServerEntry *ee;

	if( ( ee = msg->entry ) ) {
		*array++ = (STRPTR) "ss";
		*array++ = (STRPTR) "ss";
		*array++ = (STRPTR) "ss";
		*array++ = (STRPTR) "ss";
	} else {
		*array++ = (STRPTR) LGS( MSG_LV_SERVER  );
		*array++ = (STRPTR) LGS( MSG_LV_PORT    );
		*array++ = (STRPTR) LGS( MSG_LV_CONNECT );
		*array++ = (STRPTR) LGS( MSG_LV_CHARSET );
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
struct mccdata *mccdata = INST_DATA( cl, obj );
struct ServerEntry *se;
struct ChannelEntry *ce, *tce;
struct NickEntry *ne, *tne;
ULONG i;

	if( ( se = msg->entry ) ) {
		while( ( ce = (APTR) se->se_ChannelList.lh_Head )->ce_Succ ) {
			for( i = 0 ; ; i++ ) {
				tce = NULL;
				DoMethod( mccdata->mcc_ClassObjects[ GID_CHANNELLIST ], MUIM_NList_GetEntry, i, &tce );
				if( tce && ( tce == ce ) ) {
					/* if channel is in list, remove or we will display trashed data */
					DoMethod( mccdata->mcc_ClassObjects[ GID_CHANNELLIST ], MUIM_NList_Remove, i );
					break;
				}
			}
			DoMethod( mccdata->mcc_ClassObjects[ GID_CHANNELLIST ], MM_CHANNELLIST_REMOVE, ce );
		}
		while( ( ne = (APTR) se->se_NickList.lh_Head )->ne_Succ ) {
			for( i = 0 ; ; i++ ) {
				tne = NULL;
				DoMethod( mccdata->mcc_ClassObjects[ GID_NICKLIST ], MUIM_NList_GetEntry, i, &tne );
				if( tne && ( tne == ne ) ) {
					/* if channel is in list, remove or we will display trashed data */
					DoMethod( mccdata->mcc_ClassObjects[ GID_CHANNELLIST ], MUIM_NList_Remove, i );
					break;
				}
			}
			DoMethod( mccdata->mcc_ClassObjects[ GID_CHANNELLIST ], MM_CHANNELLIST_REMOVE, ne );
		}
		FreeMem( se, sizeof( struct ServerEntry ) );
    }
	return( 0 );
}
/* \\\ */
#if 0
/* /// OM_Import()
**
*/

/*************************************************************************/

static ULONG OM_Import( struct IClass *cl, Object *obj, struct MUIP_Import *msg )
{
struct ServerEntry *ee;
ULONG i;
char *text, *buffer;

	for( i = 0 ;  ; i++ ) {
		ee = NULL;
		DoMethod( obj, MUIM_NList_GetEntry, i, &ee );
		if( ee ) {
			if( ( text = (char *) DoMethod( msg->dataspace, MUIM_Dataspace_Find, OID_SVR_LIST + ee->ee_Type + 1 ) ) ) {
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
struct ServerEntry *ee;
char buffer[ SERVERENTRY_SCRIPT_SIZEOF + SERVERENTRY_TEXT_SIZEOF + 64 ];
ULONG i;

	for( i = 0 ;  ; i++ ) {
		ee = NULL;
		DoMethod( obj, MUIM_NList_GetEntry, i, &ee );
		if( ee ) {
			sprintf( buffer, "SERVER \"%s\" PORT %ld CHARSET \"%s\" SPASSWORD \"%s\" NPASSWORD \"%s\" JOIN ", ee->ee_Mode, ee->ee_Script, ee->ee_Text );
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

static ULONG MM_Add( struct IClass *cl, Object *obj, struct MP_SERVERLIST_ADD *msg )
{
struct ServerEntry *ee;
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
		ee = AllocMem( sizeof( struct ServerEntry ), MEMF_ANY|MEMF_CLEAR );
		newitem++;
	}
	if( ee ) {
		/* fill / update entry */
		ee->ee_Type = msg->Type;
		ee->ee_Mode = msg->Mode;
		strncpy( (char *) ee->ee_Script  , (char *) ( msg->Script ? msg->Script : (STRPTR) "" ), SERVERENTRY_SCRIPT_SIZEOF );
		strncpy( (char *) ee->ee_Text    , (char *) ( msg->Text   ? msg->Text   : (STRPTR) "" ), SERVERENTRY_TEXT_SIZEOF   );
		if( newitem ) { /* only add if entry is new */
			DoMethod( obj, MUIM_NList_InsertSingle, ee, MUIV_NList_Insert_Bottom );
		}
	}
	return( (ULONG) ee );
}
/* \\\ */
#endif

/* /// MM_Add()
**
*/

/*************************************************************************/

static ULONG MM_Add( struct IClass *cl, Object *obj, struct MP_SERVERLIST_ADD *msg )
{
struct ServerEntry *se;

	if( ( se = AllocVec( sizeof( struct ServerEntry ), MEMF_ANY|MEMF_CLEAR ) ) ) {
		NEWLIST( &se->se_ChannelList );
		NEWLIST( &se->se_NickList );
		strncpy( (char *) se->se_Name    , (char *)   msg->Name   ,                                       SERVERENTRY_NAME_SIZEOF     );
		strncpy( (char *) se->se_Address , (char *)   msg->Address,                                       SERVERENTRY_ADDRESS_SIZEOF  );
		strncpy( (char *) se->se_Password, (char *) ( msg->Password ? msg->Password : (STRPTR) ""      ), SERVERENTRY_PASSWORD_SIZEOF );
		strncpy( (char *) se->se_Charset , (char *) ( msg->Charset  ? msg->Charset  : (STRPTR) "UTF-8" ), SERVERENTRY_CHARSET_SIZEOF  );
		se->se_Port  = msg->Port;
		se->se_Flags = msg->Flags;
	}
	return( (ULONG) se );
}
/* \\\ */


/*
** Dispatcher, init and dispose
*/

/* /// MCC_ServerList_Dispatcher()
**
*/

/*************************************************************************/

DISPATCHER(MCC_ServerList_Dispatcher)
{
    switch (msg->MethodID)
    {
		case OM_NEW                          : return( OM_New                           ( cl, obj, (APTR) msg ) );
		case OM_SET                          : return( OM_Set                           ( cl, obj, (APTR) msg ) );
		case MUIM_NList_Display              : return( OM_Display                       ( cl, obj, (APTR) msg ) );
		case MUIM_NList_Destruct             : return( OM_Destruct                      ( cl, obj, (APTR) msg ) );
#if 0
		case MUIM_Import                     : return( OM_Import                        ( cl, obj, (APTR) msg ) );
		case MUIM_Export                     : return( OM_Export                        ( cl, obj, (APTR) msg ) );
#endif
		case MM_SERVERLIST_ADD               : return( MM_Add                           ( cl, obj, (APTR) msg ) );
    }
	return( DoSuperMethodA( cl, obj, msg ) );

}
/* \\\ */
/* /// MCC_ServerList_InitClass()
**
*/

/*************************************************************************/

ULONG MCC_ServerList_InitClass( void )
{
	appclasses[ CLASSID_SERVERLIST ] = MUI_CreateCustomClass( NULL, (ClassID) MUIC_NList, NULL, sizeof( struct mccdata ), (APTR) ENTRY(MCC_ServerList_Dispatcher) );
	return( appclasses[ CLASSID_SERVERLIST ] ? MSG_ERROR_NOERROR : MSG_ERROR_UNABLETOSETUPMUICLASS );
}
/* \\\ */
/* /// MCC_ServerList_DisposeClass()
**
*/

/*************************************************************************/

void MCC_ServerList_DisposeClass( void )
{
	if( appclasses[ CLASSID_SERVERLIST ] ) {
		MUI_DeleteCustomClass( appclasses[ CLASSID_SERVERLIST ] );
		appclasses[ CLASSID_SERVERLIST ] = NULL;
    }
}
/* \\\ */




