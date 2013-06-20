/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

/*
** muiclass_chatlog.c
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
#include "muiclass_chatlog.h"
#include "muiclass_network.h"
#include "version.h"

/*************************************************************************/


/*
** gadgets used by this class
*/

enum
{
GID_AREXXPORT = 0,
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
Object *objs[ GID_LAST ];

	if( (obj = (Object *)DoSuperNew( cl, obj,
		TAG_DONE ) ) ) {

		struct mccdata *mccdata = INST_DATA( cl, obj );

		CopyMem( &objs[0], &mccdata->mcc_ClassObjects[0], sizeof( mccdata->mcc_ClassObjects));

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
struct ChatLogEntry *cle;
STRPTR *array = msg->strings;

	if( ( cle = msg->entry ) ) {
		*array = (STRPTR) cle->cle_Message;
	}
	return( 0 );
}
/* \\\ */

/*
** Dispatcher, init and dispose
*/

/* /// MCC_ChatLog_Dispatcher()
**
*/

/*************************************************************************/

DISPATCHER(MCC_ChatLog_Dispatcher)
{
    switch (msg->MethodID)
    {
		case OM_NEW                          : return( OM_New        ( cl, obj, (APTR) msg ) );
		case OM_DISPOSE                      : return( OM_Dispose    ( cl, obj, (APTR) msg ) );
		case MUIM_NList_Display              : return( OM_Display    ( cl, obj, (APTR) msg ) );
    }
	return( DoSuperMethodA( cl, obj, msg ) );

}
/* \\\ */
/* /// MCC_ChatLog_InitClass()
**
*/

/*************************************************************************/

ULONG MCC_ChatLog_InitClass( void )
{
	appclasses[ CLASSID_CHATLOG ] = MUI_CreateCustomClass( NULL, (ClassID) MUIC_NList, NULL, sizeof( struct mccdata ) ,  (APTR) ENTRY(MCC_ChatLog_Dispatcher) );
	return( appclasses[ CLASSID_CHATLOG ] ? MSG_ERROR_NOERROR : MSG_ERROR_UNABLETOSETUPMUICLASS );
}
/* \\\ */
/* /// MCC_ChatLog_DisposeClass()
**
*/

/*************************************************************************/

void MCC_ChatLog_DisposeClass( void )
{
	if( appclasses[ CLASSID_CHATLOG ] ) {
		MUI_DeleteCustomClass( appclasses[ CLASSID_CHATLOG ] );
		appclasses[ CLASSID_CHATLOG ] = NULL;
    }
}
/* \\\ */




