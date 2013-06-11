/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

/*
** muiclass_application.c
*/

#include <libraries/mui.h>
#include <prefs/prefhdr.h>
#include <proto/muimaster.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/icon.h>
#include <SDI_hook.h>
#include <string.h>

#include "intern.h"
#include "locale.h"
#include "functions.h"
#include "muiclass.h"
#include "muiclass_application.h"
#include "muiclass_windowmain.h"
#include "muiclass_windowquit.h"
#include "muiclass_windowabout.h"
#include "muiclass_windowcolorsettings.h"
#include "muiclass_windowignorelist.h"
#include "muiclass_windowurlgrabber.h"
#include "version.h"

/*************************************************************************/

#define READARGS_TEMPLATE "ICONIFY/S"

enum{
READARGS_ICONIFY = 0,
READARGS_LAST
};

/*
** objects used by this class
*/

enum
{
WID_MAIN = 0,
WID_QUIT,
WID_ABOUT,
WID_COLORSETTINGS,
WID_IGNORELIST,
WID_URLGRABBER,
GID_LAST
};

/*
** data used by this class
*/

struct mccdata
{
	Object                *mcc_ClassObjects[ GID_LAST ];
	struct DiskObject     *mcc_DiskObject;
	LONG                   mcc_ReadargsArray[ READARGS_LAST ];

};

/*************************************************************************/

/* /// OM_New()
**
*/


static STRPTR classlist[] = { (STRPTR)"NList.mcc", (STRPTR)"NListview.mcc", (STRPTR)"NListtree.mcc", (STRPTR)"BetterString.mcc", NULL };

/*************************************************************************/

static ULONG OM_New( struct IClass *cl, Object *obj, struct opSet *msg UNUSED )
{
Object *objs[ GID_LAST ];

	if( ( obj = (Object *)DoSuperNew( cl, obj,
			        MUIA_Application_UsedClasses, classlist,
					MUIA_Application_Author     , (char*) AUTHORNAME,
					MUIA_Application_Base       , (char*) APPLICATIONNAME "_NEW",
					MUIA_Application_Title      , (char*) APPLICATIONNAME "_NEW",
					MUIA_Application_Version    , (char*) VERSION_MUI,
					MUIA_Application_Copyright  , (char*) VERSION_COPYRIGHT,
					MUIA_Application_Description, (char*) LGS( MSG_MUICLASS_APPLICATION_DESCRIPTION ),
					MUIA_Application_Window     , objs[ WID_MAIN          ] = WindowMainObject, End,
					MUIA_Application_Window     , objs[ WID_QUIT          ] = WindowQuitObject, End,
					MUIA_Application_Window     , objs[ WID_ABOUT         ] = WindowAboutObject, End,
					MUIA_Application_Window     , objs[ WID_COLORSETTINGS ] = WindowColorSettingsObject, End,
					MUIA_Application_Window     , objs[ WID_IGNORELIST    ] = WindowIgnoreListObject, End,
					MUIA_Application_Window     , objs[ WID_URLGRABBER    ] = WindowURLGrabberObject, End,

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
struct mccdata *mccdata = INST_DATA( cl, obj );

	SetAttrs( obj, MUIA_Application_DiskObject, 0, TAG_DONE );
	if( mccdata->mcc_DiskObject ) {
		FreeDiskObject( mccdata->mcc_DiskObject );
	}
	return( DoSuperMethodA( cl, obj, msg ) );
}
/* \\\ */
/* /// OM_Get()
**
*/

/*************************************************************************/

static ULONG OM_Get(struct IClass *cl, Object *obj, struct opGet *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );

//	  debug( SOURCENAME ": OM_Get( IClass: 0x%08lx, Object: 0x%08lx, MSG: 0x%08lx)\n", cl, obj, msg );

	switch( msg->opg_AttrID ) {
		case MA_APPLICATION_WINDOWMAIN          : *msg->opg_Storage = (ULONG) mccdata->mcc_ClassObjects[ WID_MAIN          ] ; return( TRUE );
		case MA_APPLICATION_WINDOWQUIT          : *msg->opg_Storage = (ULONG) mccdata->mcc_ClassObjects[ WID_QUIT          ] ; return( TRUE );
		case MA_APPLICATION_WINDOWABOUT         : *msg->opg_Storage = (ULONG) mccdata->mcc_ClassObjects[ WID_ABOUT         ] ; return( TRUE );
		case MA_APPLICATION_WINDOWCOLORSETTINGS : *msg->opg_Storage = (ULONG) mccdata->mcc_ClassObjects[ WID_COLORSETTINGS ] ; return( TRUE );
		case MA_APPLICATION_WINDOWIGNORELIST    : *msg->opg_Storage = (ULONG) mccdata->mcc_ClassObjects[ WID_IGNORELIST    ] ; return( TRUE );
		case MA_APPLICATION_WINDOWURLGRABBER    : *msg->opg_Storage = (ULONG) mccdata->mcc_ClassObjects[ WID_URLGRABBER    ] ; return( TRUE );
		default: return( DoSuperMethodA( cl, obj, (Msg) msg ) );
    }
}
/* \\\ */

/* /// MM_Startup()
**
*/

/*************************************************************************/

static ULONG MM_Startup( struct IClass *cl, Object *obj, Msg msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );

	mccdata->mcc_DiskObject	= Icon_GetPutDiskObject( NULL );
	if( mccdata->mcc_DiskObject ) {
		SetAttrs( obj, MUIA_Application_DiskObject, mccdata->mcc_DiskObject, TAG_DONE );
	}

	if( !wbmessage ) {
		struct RDArgs *readargs;
		if( !( readargs = ReadArgs( (CONST_STRPTR) READARGS_TEMPLATE, mccdata->mcc_ReadargsArray, NULL ) ) ) {
			Dos_ShowFailure();
		} else {
			/* insert arg handling */
			if( readargs ) {
				FreeArgs( readargs );
			}
		}
	}

#if ENABLE_NEWWOOKIECODE
	SetAttrs( mccdata->mcc_ClassObjects[ WID_MAIN ], MUIA_Window_Open, TRUE, TAG_DONE );
#endif

	return( 0 );
}
/* \\\ */


/*
** Dispatcher, init and dispose
*/

/* /// MCC_Application_Dispatcher()
**
*/

/*************************************************************************/

DISPATCHER(MCC_Application_Dispatcher)
{
    switch (msg->MethodID)
    {
		case OM_NEW                          : return( OM_New                           ( cl, obj, (APTR) msg ) );
		case OM_DISPOSE                      : return( OM_Dispose                       ( cl, obj, (APTR) msg ) );

		case OM_GET                          : return( OM_Get                           ( cl, obj, (APTR) msg ) );

		case MM_APPLICATION_STARTUP          : return( MM_Startup                       ( cl, obj, (APTR) msg ) );
    }
	return( DoSuperMethodA( cl, obj, msg ) );

}
/* \\\ */
/* /// MCC_Application_InitClass()
**
*/

/*************************************************************************/

ULONG MCC_Application_InitClass( void )
{
	appclasses[ CLASSID_APPLICATION ] = MUI_CreateCustomClass( NULL, MUIC_Application, NULL, sizeof( struct mccdata ) ,  (APTR) ENTRY(MCC_Application_Dispatcher) );
	return( appclasses[ CLASSID_APPLICATION ] ? MSG_ERROR_NOERROR : MSG_ERROR_UNABLETOSETUPMUICLASS );
}
/* \\\ */
/* /// MCC_Application_DisposeClass()
**
*/

/*************************************************************************/

void MCC_Application_DisposeClass( void )
{
	if( appclasses[ CLASSID_APPLICATION ] ) {
		MUI_DeleteCustomClass( appclasses[ CLASSID_APPLICATION ] );
		appclasses[ CLASSID_APPLICATION ] = NULL;
    }
}
/* \\\ */




