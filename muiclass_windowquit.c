/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

/*
** muiclass_windowquit.c
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
#include "muiclass_application.h"
#include "muiclass_windowquit.h"
#include "version.h"

/*************************************************************************/

/*
** gadgets used by this class
*/

enum
{
GID_OK = 0,
GID_CANCEL,
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

static IPTR OM_New( struct IClass *cl, Object *obj, struct opSet *msg UNUSED )
{
Object *objs[ GID_LAST ];

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	if( ( obj = (Object *) DoSuperNew( cl, obj,
			MUIA_Window_Title            , LGS( MSG_REQUESTER_TITLE ),
			MUIA_Window_ID               , MAKE_ID('Q','U','I','T'),
			MUIA_Window_CloseGadget      , FALSE,
			MUIA_Window_NoMenus	       	 , TRUE,
			MUIA_Background              , MUII_SHINE,

			WindowContents, VGroup,
				Child, Label( LGS( MSG_MUICLASS_WINDOWQUIT_QUIT_TEXT )),
				Child, HGroup,
					Child, objs[ GID_OK     ] = MUICreateButton( MSG_MUICLASS_WINDOWQUIT_OK_GAD     ),
					Child, HVSpace,
					Child, objs[ GID_CANCEL ] = MUICreateButton( MSG_MUICLASS_WINDOWQUIT_CANCEL_GAD ),
				End,
			End,
		TAG_DONE ) ) ) {

		struct mccdata *mccdata = INST_DATA( cl, obj );

		CopyMem( &objs[0], &mccdata->mcc_ClassObjects[0], sizeof( mccdata->mcc_ClassObjects));

		DoMethod( mccdata->mcc_ClassObjects[ GID_CANCEL ], MUIM_Notify, MUIA_Pressed, FALSE, obj, 3, MUIM_Set, MUIA_Window_Open, FALSE );
		DoMethod( mccdata->mcc_ClassObjects[ GID_OK     ], MUIM_Notify, MUIA_Pressed, FALSE, MUIV_Notify_Application, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit );

		return( (IPTR) obj );
    }
	return( (IPTR) NULL );
}
/* \\\ */
/* /// OM_Dispose()
**
*/

/*************************************************************************/

static IPTR OM_Dispose( struct IClass *cl, Object *obj, Msg msg )
{
//struct mccdata *mccdata = INST_DATA( cl, obj );

	debug( "%s (%ld) %s() - Class: 0x%08lx Object: 0x%08lx \n", __FILE__, __LINE__, __func__, cl, obj );

	return( DoSuperMethodA( cl, obj, msg ) );
}
/* \\\ */
/* /// OM_Get()
**
*/

/*************************************************************************/

static IPTR OM_Get(struct IClass *cl, Object *obj, struct opGet *msg )
{
	switch( msg->opg_AttrID ) {
		case MA_APPLICATION_CLASSID: *msg->opg_Storage = CLASSID_WINDOWQUIT ; return( TRUE );
		default: return( DoSuperMethodA( cl, obj, (Msg) msg ) );
    }
}
/* \\\ */

/*
** Dispatcher, init and dispose
*/

/* /// MCC_WindowQuit_Dispatcher()
**
*/

/*************************************************************************/

DISPATCHER(MCC_WindowQuit_Dispatcher)
{
    switch (msg->MethodID)
    {
		case OM_NEW                          : return( OM_New      ( cl, obj, (APTR) msg ) );
		case OM_DISPOSE                      : return( OM_Dispose  ( cl, obj, (APTR) msg ) );
		case OM_GET                          : return( OM_Get      ( cl, obj, (APTR) msg ) );
    }
	return( DoSuperMethodA( cl, obj, msg ) );

}
/* \\\ */
/* /// MCC_WindowQuit_InitClass()
**
*/

/*************************************************************************/

ULONG MCC_WindowQuit_InitClass( void )
{
	appclasses[ CLASSID_WINDOWQUIT ] = (struct MUI_CustomClass *) MUI_CreateCustomClass( NULL, (ClassID) MUIC_Window, NULL, sizeof( struct mccdata ) ,  (APTR) ENTRY(MCC_WindowQuit_Dispatcher) );
	return( appclasses[ CLASSID_WINDOWQUIT ] ? MSG_ERROR_NOERROR : MSG_ERROR_UNABLETOSETUPMUICLASS );
}
/* \\\ */
/* /// MCC_WindowQuit_DisposeClass()
**
*/

/*************************************************************************/

void MCC_WindowQuit_DisposeClass( void )
{
	if( appclasses[ CLASSID_WINDOWQUIT ] ) {
		MUI_DeleteCustomClass( appclasses[ CLASSID_WINDOWQUIT ] );
		appclasses[ CLASSID_WINDOWQUIT ] = NULL;
    }
}
/* \\\ */




