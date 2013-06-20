/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

/*
** muiclass_chatchannellist.c
*/

#include <libraries/mui.h>
#include <intuition/classusr.h>
#include <prefs/prefhdr.h>
#include <proto/muimaster.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/exec.h>
#include <SDI_hook.h>

#include <mui/NList_mcc.h>
#include <mui/NListview_mcc.h>
#include <string.h>

#include "system.h"
#include "locale.h"
#include "muiclass.h"
#include "muiclass_network.h"
#include "muiclass_chatchannellist.h"
#include "version.h"

/*************************************************************************/

/*
** gadgets used by this class
*/

enum
{
GID_CHATLOG = 0,
GID_CHATUSERLIST,
GID_LAST
};

/*
** data used by this class
*/

struct mccdata
{
	Object                *mcc_ClassObjects[ GID_LAST ];
};

/*************************************************************************/

/* /// OM_New()
**
*/

/*************************************************************************/

static ULONG OM_New( struct IClass *cl, Object *obj, struct opSet *msg UNUSED )
{
	if( (obj = (Object *)DoSuperNew( cl, obj, TAG_DONE ) ) ) {

		//struct mccdata *mccdata = INST_DATA( cl, obj );

		return( (ULONG) obj );
    }
	return( (ULONG) NULL );
}
/* \\\ */
/* /// OM_Dispose()
**
*/

/*************************************************************************/

static ULONG OM_Dispose( struct IClass *cl, Object *obj, Msg msg )
{
//struct mccdata *mccdata = INST_DATA( cl, obj );

	return( DoSuperMethodA( cl, obj, msg ) );
}
/* \\\ */
/* /// OM_Display()
**
*/

/*************************************************************************/

static ULONG OM_Display( struct IClass *cl, Object *obj, struct MUIP_NList_Display *msg )
{
struct ChatChannel *cc;
STRPTR *array = msg->strings;

	if( ( cc = msg->entry ) ) {
		*array = (STRPTR) cc->cc_Channel->c_Name;
	}
	return( 0 );
}
/* \\\ */
/* /// OM_Construct()
**
*/

/*************************************************************************/

static ULONG OM_Construct( struct IClass *cl, Object *obj, struct MUIP_NList_Construct *msg )
{
struct ChatChannel *cc;

	if( ( cc = AllocPooled( msg->pool, sizeof( struct ChatChannel ) ) ) ) {
		cc->cc_Channel = msg->entry;
    }
	return( (IPTR) cc );
}
/* \\\ */
/* /// OM_Destruct()
**
*/

/*************************************************************************/

static ULONG OM_Destruct( struct IClass *cl, Object *obj, struct MUIP_NList_Destruct *msg )
{
	if( msg->entry ) {
		FreePooled( msg->pool, msg->entry, sizeof( struct ChatChannel ) );
    }
	return( 0 );
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
			case MM_CHATCHANNELLIST_OBJECTCHATLOG:
				mccdata->mcc_ClassObjects[ GID_CHATLOG ] = (APTR) tidata;
				break;
			case MM_CHATCHANNELLIST_OBJECTCHATUSERLIST:
				mccdata->mcc_ClassObjects[ GID_CHATUSERLIST ] = (APTR) tidata;
				break;
		}
    }
	return( DoSuperMethodA( cl, obj,(Msg) msg ) );
}
/* \\\ */

/* /// MM_Remove()
**
*/

/*************************************************************************/

static ULONG MM_Remove( struct IClass *cl, Object *obj, struct MP_CHATCHANNELLIST_REMOVE *msg )
{
struct ChatChannel *cc;
ULONG i;

//	  DoMethod( mccdata->mcc_ClassObjects[ GID_CONNECTEDBUTTONS ], MM_CONNECTEDBUTTONS_REMOVE, msg->Channel );

	for( i = 0 ;  ; i++ ) {
		cc = NULL;
		DoMethod( obj, MUIM_NList_GetEntry, i, &cc );
		if( cc ) {
			if( cc->cc_Channel == msg->Channel ) {
				DoMethod( obj, MUIM_NList_Remove, i );
				break;
			}
		} else {
			break;
		}
	}
	return( 0 );
}
/* \\\ */
/* /// MM_Add()
**
*/

/*************************************************************************/

static ULONG MM_Add( struct IClass *cl, Object *obj, struct MP_CHATCHANNELLIST_ADD *msg )
{
struct ChatChannel *cc;
ULONG i;

debug("list channel add\n");

	/* only add, if not already in list */
	for( i = 0 ;  ; i++ ) {
		cc = NULL;
		DoMethod( obj, MUIM_NList_GetEntry, i, &cc );
		if( cc ) {
			if( cc->cc_Channel == msg->Channel ) {
				return( 0 );
			}
		} else {
			break;
		}
	}

debug("list channel adding\n");

//	  DoMethod( mccdata->mcc_ClassObjects[ GID_CONNECTEDBUTTONS ], MM_CONNECTEDBUTTONS_ADD, msg->Channel );

	DoMethod( obj, MUIM_NList_InsertSingle, msg->Channel, MUIV_NList_Insert_Bottom );

	return( 0 );
}
/* \\\ */

/* /// MM_MessageReceived()
**
*/

/*************************************************************************/

static ULONG MM_MessageReceived( struct IClass *cl, Object *obj, struct MP_CHATCHANNELLIST_MESSAGERECEIVED *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );

	DoMethod( mccdata->mcc_ClassObjects[ GID_CHATLOG ], MUIM_NList_InsertSingleWrap, msg->ChatLogEntry, MUIV_NList_Insert_Bottom, WRAPCOL0, ALIGN_LEFT );

	return( 0 );
}
/* \\\ */

/*
** Dispatcher, init and dispose
*/

/* /// MCC_ChatChannelList_Dispatcher()
**
*/

/*************************************************************************/

DISPATCHER(MCC_ChatChannelList_Dispatcher)
{
    switch (msg->MethodID)
    {
		case OM_NEW                              : return( OM_New                 ( cl, obj, (APTR) msg ) );
		case OM_DISPOSE                          : return( OM_Dispose             ( cl, obj, (APTR) msg ) );
		case OM_SET                              : return( OM_Set                 ( cl, obj, (APTR) msg ) );

		case MUIM_NList_Display                  : return( OM_Display             ( cl, obj, (APTR) msg ) );
		case MUIM_NList_Destruct                 : return( OM_Destruct            ( cl, obj, (APTR) msg ) );
		case MUIM_NList_Construct                : return( OM_Construct           ( cl, obj, (APTR) msg ) );

		case MM_CHATCHANNELLIST_ADD              : return( MM_Add                 ( cl, obj, (APTR) msg ) );
		case MM_CHATCHANNELLIST_REMOVE           : return( MM_Remove              ( cl, obj, (APTR) msg ) );

		case MM_CHATCHANNELLIST_MESSAGERECEIVED  : return( MM_MessageReceived     ( cl, obj, (APTR) msg ) );
    }
	return( DoSuperMethodA( cl, obj, msg ) );

}
/* \\\ */
/* /// MCC_ChatChannelList_InitClass()
**
*/

/*************************************************************************/

ULONG MCC_ChatChannelList_InitClass( void )
{
	appclasses[ CLASSID_CHATCHANNELLIST ] = MUI_CreateCustomClass( NULL, (ClassID) MUIC_NList, NULL, sizeof( struct mccdata ) ,  (APTR) ENTRY(MCC_ChatChannelList_Dispatcher) );
	return( appclasses[ CLASSID_CHATCHANNELLIST ] ? MSG_ERROR_NOERROR : MSG_ERROR_UNABLETOSETUPMUICLASS );
}
/* \\\ */
/* /// MCC_ChatChannelList_DisposeClass()
**
*/

/*************************************************************************/

void MCC_ChatChannelList_DisposeClass( void )
{
	if( appclasses[ CLASSID_CHATCHANNELLIST ] ) {
		MUI_DeleteCustomClass( appclasses[ CLASSID_CHATCHANNELLIST ] );
		appclasses[ CLASSID_CHATCHANNELLIST ] = NULL;
    }
}
/* \\\ */




