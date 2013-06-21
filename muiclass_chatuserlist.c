/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

/*
** muiclass_chatuserlist.c
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

#include "system.h"
#include "locale.h"
#include "muiclass.h"
#include "muiclass_network.h"
#include "muiclass_chatuserlist.h"
#include "version.h"

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
		*msg->strings = (STRPTR) ((struct ChatNick *) msg->entry)->cn_ChatNickEntry->cne_Nick;
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
struct ChatNick *cn;

	if( ( cn = AllocPooled( msg->pool, sizeof( struct ChatNick ) ) ) ) {
		cn->cn_ChatNickEntry = msg->entry;
    }
	return( (IPTR) cn );
}
/* \\\ */
/* /// OM_Destruct()
**
*/

/*************************************************************************/

static ULONG OM_Destruct( struct IClass *cl, Object *obj, struct MUIP_NList_Destruct *msg )
{
	if( msg->entry ) {
		FreePooled( msg->pool, msg->entry, sizeof( struct ChatNick ) );
    }
	return( 0 );
}
/* \\\ */
/* /// OM_Compare()
**
*/

/*************************************************************************/

static ULONG OM_Compare( struct IClass *cl, Object *obj, struct MUIP_NList_Compare *msg )
{
STRPTR nick1 = (STRPTR) ((struct ChatNick *) msg->entry1)->cn_ChatNickEntry->cne_Nick;
STRPTR nick2 = (STRPTR) ((struct ChatNick *) msg->entry2)->cn_ChatNickEntry->cne_Nick;

	return( Stricmp( (CONST_STRPTR) nick1, (CONST_STRPTR) nick2 ) );
}
/* \\\ */

/*
** Dispatcher, init and dispose
*/

/* /// MCC_ChatUserList_Dispatcher()
**
*/

/*************************************************************************/

DISPATCHER(MCC_ChatUserList_Dispatcher)
{
    switch (msg->MethodID)
    {
		case OM_NEW                : return( OM_New       ( cl, obj, (APTR) msg ) );
		case MUIM_NList_Display    : return( OM_Display   ( cl, obj, (APTR) msg ) );
		case MUIM_NList_Destruct   : return( OM_Destruct  ( cl, obj, (APTR) msg ) );
		case MUIM_NList_Construct  : return( OM_Construct ( cl, obj, (APTR) msg ) );
		case MUIM_NList_Compare    : return( OM_Compare   ( cl, obj, (APTR) msg ) );
    }
	return( DoSuperMethodA( cl, obj, msg ) );

}
/* \\\ */
/* /// MCC_ChatUserList_InitClass()
**
*/

/*************************************************************************/

ULONG MCC_ChatUserList_InitClass( void )
{
	appclasses[ CLASSID_CHATUSERLIST ] = MUI_CreateCustomClass( NULL, (ClassID) MUIC_NList, NULL, 0,  (APTR) ENTRY(MCC_ChatUserList_Dispatcher) );
	return( appclasses[ CLASSID_CHATUSERLIST ] ? MSG_ERROR_NOERROR : MSG_ERROR_UNABLETOSETUPMUICLASS );
}
/* \\\ */
/* /// MCC_ChatUserList_DisposeClass()
**
*/

/*************************************************************************/

void MCC_ChatUserList_DisposeClass( void )
{
	if( appclasses[ CLASSID_CHATUSERLIST ] ) {
		MUI_DeleteCustomClass( appclasses[ CLASSID_CHATUSERLIST ] );
		appclasses[ CLASSID_CHATUSERLIST ] = NULL;
    }
}
/* \\\ */




