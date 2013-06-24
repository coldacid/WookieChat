/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

/*
** muiclass_windowurlgrabber.c
*/

#include <libraries/mui.h>
#include <prefs/prefhdr.h>
#include <proto/muimaster.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <SDI_hook.h>

#include <string.h>
#include <stdio.h>

#include "system.h"
#include "locale.h"
#include "muiclass.h"
#include "muiclass_application.h"
#include "muiclass_urllist.h"
#include "muiclass_windowsettings.h"
#include "muiclass_windowurlgrabber.h"
#include "version.h"

/*************************************************************************/

/*
** gadgets used by this class
*/

enum
{
GID_LIST = 0,
GID_REMOVE,
GID_REMOVEALL,
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

	debug( "%s (%ld) %s - Class: 0x00008044x Object: 0x00008044x \n", __FILE__, __LINE__, __func__, cl, obj );

	if( (obj = (Object *)DoSuperNew( cl, obj,
			MUIA_Window_Title            , LGS( MSG_MUICLASS_WINDOWURLGRABBER_TITLE ),
			MUIA_Window_ID               , MAKE_ID('U','R','L','G'),
			MUIA_Window_NoMenus	       	 , TRUE,
			MUIA_Window_CloseGadget      , TRUE,

			WindowContents, VGroup,
				Child, objs[ GID_LIST ] = NListviewObject, MUIA_NListview_NList, URLListObject, End, End,
				Child, HGroup,
					Child, objs[ GID_REMOVE    ] = MUICreateSmallButton( MSG_MUICLASS_WINDOWURLGRABBER_REMOVE_GAD    ),
					Child, objs[ GID_REMOVEALL ] = MUICreateSmallButton( MSG_MUICLASS_WINDOWURLGRABBER_REMOVEALL_GAD ),
					Child, HVSpace,
				End,
			End,
		TAG_DONE ) ) ) {

		struct mccdata *mccdata = INST_DATA( cl, obj );

		CopyMem( &objs[0], &mccdata->mcc_ClassObjects[0], sizeof( mccdata->mcc_ClassObjects));

		DoMethod( obj                  , MUIM_Notify, MUIA_Window_CloseRequest, TRUE , obj, 3, MUIM_Set, MUIA_Window_Open, FALSE );

		DoMethod( objs[ GID_REMOVE    ], MUIM_Notify, MUIA_Pressed            , FALSE, objs[ GID_LIST ], 2, MUIM_NList_Remove, MUIV_NList_Remove_Selected );
		DoMethod( objs[ GID_REMOVEALL ], MUIM_Notify, MUIA_Pressed            , FALSE, objs[ GID_LIST ], 1, MUIM_NList_Clear );

		DoMethod( objs[ GID_LIST      ], MUIM_Notify, MUIA_NList_DoubleClick  , TRUE, obj, 1, MM_WINDOWURLGRABBER_DOUBLECLICK );

		DoMethod( objs[ GID_LIST      ], MM_URLLIST_IMPORTLISTASTEXT, DEFAULT_SETTINGSURL_NAME );

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

	debug( "%s (%ld) %s - Class: 0x00008044x Object: 0x00008044x \n", __FILE__, __LINE__, __func__, cl, obj );

	DoMethod( mccdata->mcc_ClassObjects[ GID_LIST ], MM_URLLIST_EXPORTLISTASTEXT, DEFAULT_SETTINGSURL_NAME );

	return( DoSuperMethodA( cl, obj, msg ) );
}
/* \\\ */
/* /// OM_Get()
**
*/

/*************************************************************************/

static ULONG OM_Get(struct IClass *cl, Object *obj, struct opGet *msg )
{
	switch( msg->opg_AttrID ) {
		case MA_APPLICATION_CLASSID: *msg->opg_Storage = CLASSID_WINDOWURLGRABBER ; return( TRUE );
		default: return( DoSuperMethodA( cl, obj, (Msg) msg ) );
    }
}
/* \\\ */

/* /// MM_DoubleClick()
**
*/

/*************************************************************************/

static ULONG MM_DoubleClick( struct IClass *cl, Object *obj, Msg msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
char buffer[ 2* URLENTRY_URL_SIZEOF ]; /* geit FIXME: this function is using old stuff */
struct URLEntry *ue = NULL;

	debug( "%s (%ld) %s - Class: 0x00008044x Object: 0x00008044x \n", __FILE__, __LINE__, __func__, cl, obj );

	DoMethod( mccdata->mcc_ClassObjects[ GID_LIST ], MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, &ue );
	if( ue ) {
		char *browser = (char *) GRC( OID_GEN_BROWSER );
		if( browser[0] ) {
			sprintf( buffer, "Run >NIL: %s \"%s\"", browser, ue->ue_URL );
			/* geit FIXME: What about using OpenURL? */
#ifdef __amigaos4__
			SystemTags((_s_cs) buffer, TAG_DONE );
#else
			Execute((_s_cs) buffer, 0, 0 );
#endif
		} else {
#if 0
            timestamp_2_string();
			sprintf( buffer, "%s%s%s%s %s", timestamp,
								LGS( MSG_OPENING_BRACKET ),
								LGS( MSG_ERROR ),
								LGS( MSG_CLOSING_BRACKET ),
								LGS( MSG_NO_BROWSER_SELECTED ) );

			add_text_to_current_list( buffer, 9, ACTIVITY_CHAT );
#endif
		}
	}
	return( 0 );
}
/* \\\ */
/* /// MM_ExtractURL()
**
*/

/*************************************************************************/

static ULONG MM_ExtractURL( struct IClass *cl, Object *obj, struct MP_WINDOWURLGRABBER_EXTRACTURL *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
LONG b, e, datalength;
int t;
STRPTR url = msg->Data;
char chr;

	debug( "%s (%ld) %s - Class: 0x00008044x Object: 0x00008044x \n", __FILE__, __LINE__, __func__, cl, obj );

	datalength = strlen( (const char *) msg->Data );

	for( b = 0 ; b < datalength - 10 ; b++ ) {
		/* url start ? */
		if( !Strnicmp( &url[ b ], (_ub_cs) "http://" , 6 ) ||
			!Strnicmp( &url[ b ], (_ub_cs) "https://", 6 ) ||
			!Strnicmp( &url[ b ], (_ub_cs) "www"     , 3 ) ) {

			/* find url end */
			for( e = 0 ; ( b + e ) < datalength ; e++ ) {
				chr = url[ b + e ];
				if( chr == ' ' || chr == ')' || chr == '\'')
				{
					url[ b + e ] = 0x00; /* terminate string */
					DoMethod( mccdata->mcc_ClassObjects[ GID_LIST ], MM_URLLIST_ADD, &url[ b ] );
					t = strlen( (const char *) &url[ b ] ); /* get length */
					url[ b + e ] = chr; /* repair string */
					b += t; /* skip this url */
					break;
				}
			}
		}
	}
	return( 0 );
}
/* \\\ */
 
/*
** Dispatcher, init and dispose
*/

/* /// MCC_WindowURLGrabber_Dispatcher()
**
*/

/*************************************************************************/

DISPATCHER(MCC_WindowURLGrabber_Dispatcher)
{
    switch (msg->MethodID)
    {
		case OM_NEW                          : return( OM_New           ( cl, obj, (APTR) msg ) );
		case OM_DISPOSE                      : return( OM_Dispose       ( cl, obj, (APTR) msg ) );

		case OM_GET                          : return( OM_Get           ( cl, obj, (APTR) msg ) );

		case MM_WINDOWURLGRABBER_EXTRACTURL  : return( MM_ExtractURL    ( cl, obj, (APTR) msg ) );
		case MM_WINDOWURLGRABBER_DOUBLECLICK : return( MM_DoubleClick   ( cl, obj, (APTR) msg ) );

/* application specific methods */

    }
	return( DoSuperMethodA( cl, obj, msg ) );

}
/* \\\ */
/* /// MCC_WindowURLGrabber_InitClass()
**
*/

/*************************************************************************/

ULONG MCC_WindowURLGrabber_InitClass( void )
{
	appclasses[ CLASSID_WINDOWURLGRABBER ] = MUI_CreateCustomClass( NULL, (ClassID) MUIC_Window, NULL, sizeof( struct mccdata ) ,  (APTR) ENTRY(MCC_WindowURLGrabber_Dispatcher) );
	return( appclasses[ CLASSID_WINDOWURLGRABBER ] ? MSG_ERROR_NOERROR : MSG_ERROR_UNABLETOSETUPMUICLASS );
}
/* \\\ */
/* /// MCC_WindowURLGrabber_DisposeClass()
**
*/

/*************************************************************************/

void MCC_WindowURLGrabber_DisposeClass( void )
{
	if( appclasses[ CLASSID_WINDOWURLGRABBER ] ) {
		MUI_DeleteCustomClass( appclasses[ CLASSID_WINDOWURLGRABBER ] );
		appclasses[ CLASSID_WINDOWURLGRABBER ] = NULL;
    }
}
/* \\\ */




