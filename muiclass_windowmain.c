/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

/*
** muiclass_windowmain.c
*/

#include <libraries/mui.h>
#include <prefs/prefhdr.h>
#include <proto/muimaster.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/exec.h>
#include <SDI_hook.h>
#include <string.h>
#include <stdio.h>

#include "locale.h"
#include "muiclass.h"
#include "muiclass_application.h"
#include "muiclass_windowmain.h"
#include "muiclass_windowquit.h"
#include "version.h"
#include "intern.h"

/*************************************************************************/

/*
** gadgets used by this class
*/

enum
{
GID_AREXXPORT = 0,
GID_LAST
};

#define WINDOWTITLE_SIZEOF 0x100
/*
** data used by this class
*/

struct mccdata
{
	Object                *mcc_ClassObjects[ GID_LAST ];
	char                   mcc_WindowTitle[ WINDOWTITLE_SIZEOF + 2 ];
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
			MUIA_Window_Title            , LGS( MSG_MUICLASS_WINDOWMAIN_TITLE ),
			MUIA_Window_ID               , MAKE_ID('M','A','I','N'),
			MUIA_Window_NoMenus	       	 , TRUE,

			WindowContents, VGroup,
						Child, TextObject, MUIA_Text_Contents, VERSION_ABOUT, MUIA_Weight, 100, End,
						Child, TextObject,MUIA_Text_Contents, "\n\033cThanks go out to Guru Meditation for past sponsorship!\n\033cAmiga reseller and Linux consulting\n\033chttp://gurumeditation.se/\n", MUIA_Weight, 100, End,
					 End,
		TAG_DONE ) ) ) {

		struct mccdata *mccdata = INST_DATA( cl, obj );

		CopyMem( &objs[0], &mccdata->mcc_ClassObjects[0], sizeof( mccdata->mcc_ClassObjects));

//		  DoMethod( obj, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, obj, 3, MUIM_Set, MUIA_Window_Open, FALSE );

		sprintf( &mccdata->mcc_WindowTitle[0], (const char *) LGS( MSG_MUICLASS_WINDOWMAIN_TITLE ), VERSION, REVISION );
		SetAttrs( obj, MUIA_Window_Title, mccdata->mcc_WindowTitle, TAG_DONE );

		DoMethod( obj, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, application, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit );

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

/* /// OM_Setup()
**
*/

/*************************************************************************/

static ULONG OM_Setup( struct IClass *cl, Object *obj, Msg *msg )
{
Object *quitwin;
	
	quitwin = (Object *) MUIGetVar( application, MA_APPLICATION_WINDOWQUIT );
	DoMethod( obj, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, quitwin, 3, MUIM_Set, MUIA_Window_Open, TRUE );

	return( DoSuperMethodA( cl, obj,(Msg) msg ) );
}
/* \\\ */

/*
** Dispatcher, init and dispose
*/

/* /// MCC_WindowMain_Dispatcher()
**
*/

/*************************************************************************/

DISPATCHER(MCC_WindowMain_Dispatcher)
{
    switch (msg->MethodID)
    {
		case OM_NEW                          : return( OM_New                           ( cl, obj, (APTR) msg ) );
		case OM_DISPOSE                      : return( OM_Dispose                       ( cl, obj, (APTR) msg ) );

		case MUIM_Window_Setup               : return( OM_Setup                         ( cl, obj, (APTR) msg ) );

    }
	return( DoSuperMethodA( cl, obj, msg ) );

}
/* \\\ */
/* /// MCC_WindowMain_InitClass()
**
*/

/*************************************************************************/

ULONG MCC_WindowMain_InitClass( void )
{
	appclasses[ CLASSID_WINDOWMAIN ] = MUI_CreateCustomClass( NULL, MUIC_Window, NULL, sizeof( struct mccdata ) ,  (APTR) ENTRY(MCC_WindowMain_Dispatcher) );
	return( appclasses[ CLASSID_WINDOWMAIN ] ? MSG_ERROR_NOERROR : MSG_ERROR_UNABLETOSETUPMUICLASS );
}
/* \\\ */
/* /// MCC_WindowMain_DisposeClass()
**
*/

/*************************************************************************/

void MCC_WindowMain_DisposeClass( void )
{
	if( appclasses[ CLASSID_WINDOWMAIN ] ) {
		MUI_DeleteCustomClass( appclasses[ CLASSID_WINDOWMAIN ] );
		appclasses[ CLASSID_WINDOWMAIN ] = NULL;
    }
}
/* \\\ */




