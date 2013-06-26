/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

/*
** muiclass_channellist.c
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

#include "system.h"
#include "locale.h"
#include "muiclass.h"
#include "muiclass_channellist.h"
#include "muiclass_serverlist.h"
#include "version.h"

/*************************************************************************/

/* /// OM_New()
**
*/

/*************************************************************************/

static ULONG OM_New( struct IClass *cl, Object *obj, struct opSet *msg UNUSED )
{

	debug( "%s (%ld) %s - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	return( (ULONG) (obj = (Object *) DoSuperNew( cl, obj,
													MUIA_NList_Title         , TRUE,
													MUIA_CycleChain          , 1,
													MUIA_NList_Format        , "BAR,",
													TAG_DONE ) ) );
}
/* \\\ */
/* /// OM_Display()
**
*/

/*************************************************************************/

static ULONG OM_Display( struct IClass *cl, Object *obj, struct MUIP_NList_Display *msg )
{
STRPTR *array = msg->strings;
struct ChannelEntry *ce;

	if( ( ce = msg->entry ) ) {
		*array++ = (STRPTR) ce->ce_Name;
		*array++ = (STRPTR) ce->ce_Password;
	} else {
		*array++ = (STRPTR) LGS( MSG_LV_CHANNEL  );
		*array++ = (STRPTR) LGS( MSG_LV_PASSWORD );
    }
	return( 0 );
}
/* \\\ */
/* /// MM_Add()
**
*/

/*************************************************************************/

static ULONG MM_Add( struct IClass *cl, Object *obj, struct MP_CHANNELLIST_ADD *msg )
{
struct ChannelEntry *ce;

	if( ( ce = AllocVec( sizeof( struct ChannelEntry ), MEMF_ANY|MEMF_CLEAR ) ) ) {
		strncpy( (char *) ce->ce_Name    , (char *) ( msg->Channel  ? msg->Channel  : LGS( MSG_MUICLASS_CHANNELLIST_DEFAULTNAME )), CHANNELENTRY_CHANNEL_SIZEOF     );
		strncpy( (char *) ce->ce_Password, (char *) ( msg->Password ? msg->Password : (STRPTR) "" ), CHANNELENTRY_PASSWORD_SIZEOF  );
		if( msg->ServerEntry ) {
			AddTail( &((struct ServerEntry *) msg->ServerEntry)->se_ChannelList, (struct Node *) ce );
		}
	}
	return( (ULONG) ce );
}
/* \\\ */
/* /// MM_Remove()
**
*/

/*************************************************************************/

static ULONG MM_Remove( struct IClass *cl, Object *obj, struct MP_CHANNELLIST_REMOVE *msg )
{
struct ChannelEntry *ce;

	if( ( ce = msg->ChannelEntry ) ) {
		if( ce->ce_Succ && ce->ce_Pred ) {
			Remove( (struct Node *) ce );
		}
		FreeVec( ce );
	}
	return( 0 );
}
/* \\\ */

/*
** Dispatcher, init and dispose
*/

/* /// MCC_ChannelList_Dispatcher()
**
*/

/*************************************************************************/

DISPATCHER(MCC_ChannelList_Dispatcher)
{
    switch (msg->MethodID)
    {
		case OM_NEW                          : return( OM_New                           ( cl, obj, (APTR) msg ) );
		case MUIM_NList_Display              : return( OM_Display                       ( cl, obj, (APTR) msg ) );

		case MM_CHANNELLIST_ADD              : return( MM_Add                           ( cl, obj, (APTR) msg ) );
		case MM_CHANNELLIST_REMOVE           : return( MM_Remove                        ( cl, obj, (APTR) msg ) );
    }
	return( DoSuperMethodA( cl, obj, msg ) );

}
/* \\\ */
/* /// MCC_ChannelList_InitClass()
**
*/

/*************************************************************************/

ULONG MCC_ChannelList_InitClass( void )
{
	appclasses[ CLASSID_CHANNELLIST ] = MUI_CreateCustomClass( NULL, (ClassID) MUIC_NList, NULL, 0, (APTR) ENTRY(MCC_ChannelList_Dispatcher) );
	return( appclasses[ CLASSID_CHANNELLIST ] ? MSG_ERROR_NOERROR : MSG_ERROR_UNABLETOSETUPMUICLASS );
}
/* \\\ */
/* /// MCC_ChannelList_DisposeClass()
**
*/

/*************************************************************************/

void MCC_ChannelList_DisposeClass( void )
{
	if( appclasses[ CLASSID_CHANNELLIST ] ) {
		MUI_DeleteCustomClass( appclasses[ CLASSID_CHANNELLIST ] );
		appclasses[ CLASSID_CHANNELLIST ] = NULL;
    }
}
/* \\\ */




