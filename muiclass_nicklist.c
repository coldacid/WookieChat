/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

/*
** muiclass_nicklist.c
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
#include "muiclass_nicklist.h"
#include "version.h"

/*************************************************************************/

/* /// OM_New()
**
*/

/*************************************************************************/

static ULONG OM_New( struct IClass *cl, Object *obj, struct opSet *msg UNUSED )
{

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
struct NickEntry *ne;

	if( ( ne = msg->entry ) ) {
		*array++ = (STRPTR) ne->ne_Nick;
		*array++ = (STRPTR) ne->ne_Password;
	} else {
		*array++ = (STRPTR) LGS( MSG_LV_NICK     );
		*array++ = (STRPTR) LGS( MSG_LV_PASSWORD );
    }
	return( 0 );
}
/* \\\ */

/*
** Dispatcher, init and dispose
*/

/* /// MCC_NickList_Dispatcher()
**
*/

/*************************************************************************/

DISPATCHER(MCC_NickList_Dispatcher)
{
    switch (msg->MethodID)
    {
		case OM_NEW                          : return( OM_New                           ( cl, obj, (APTR) msg ) );
		case MUIM_NList_Display              : return( OM_Display                       ( cl, obj, (APTR) msg ) );

/* application specific methods */

    }
	return( DoSuperMethodA( cl, obj, msg ) );

}
/* \\\ */
/* /// MCC_NickList_InitClass()
**
*/

/*************************************************************************/

ULONG MCC_NickList_InitClass( void )
{
	appclasses[ CLASSID_NICKLIST ] = MUI_CreateCustomClass( NULL, (ClassID) MUIC_NList, NULL, 0 ,  (APTR) ENTRY(MCC_NickList_Dispatcher) );
	return( appclasses[ CLASSID_NICKLIST ] ? MSG_ERROR_NOERROR : MSG_ERROR_UNABLETOSETUPMUICLASS );
}
/* \\\ */
/* /// MCC_NickList_DisposeClass()
**
*/

/*************************************************************************/

void MCC_NickList_DisposeClass( void )
{
	if( appclasses[ CLASSID_NICKLIST ] ) {
		MUI_DeleteCustomClass( appclasses[ CLASSID_NICKLIST ] );
		appclasses[ CLASSID_NICKLIST ] = NULL;
    }
}
/* \\\ */




