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
#include "muiclass_chatchannellist.h"

/*************************************************************************/

/* /// OM_New()
**
*/

/*************************************************************************/

static ULONG OM_New( struct IClass *cl, Object *obj, struct opSet *msg UNUSED )
{
	return( (IPTR) DoSuperNew( cl, obj, TAG_DONE ) );
}
/* \\\ */
/* /// OM_Display()
**
*/

/*************************************************************************/

static ULONG OM_Display( struct IClass *cl, Object *obj, struct MUIP_NList_Display *msg )
{
	if( ( msg->entry ) ) {
		*msg->strings = (STRPTR) ((struct ChatChannel *) msg->entry)->cc_Channel->c_Name;
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

/*
** Dispatcher, init and dispose
*/

/* /// MCC_ChatChannelList_Dispatcher()
**
*/

/*************************************************************************/

DISPATCHER(MCC_ChatChannelList_Dispatcher)
{
	switch (msg->MethodID) {
		case OM_NEW                : return( OM_New       ( cl, obj, (APTR) msg ) );
		case MUIM_NList_Display    : return( OM_Display   ( cl, obj, (APTR) msg ) );
		case MUIM_NList_Destruct   : return( OM_Destruct  ( cl, obj, (APTR) msg ) );
		case MUIM_NList_Construct  : return( OM_Construct ( cl, obj, (APTR) msg ) );
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
	appclasses[ CLASSID_CHATCHANNELLIST ] = MUI_CreateCustomClass( NULL, (ClassID) MUIC_NList, NULL, 0,  (APTR) ENTRY(MCC_ChatChannelList_Dispatcher) );
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




