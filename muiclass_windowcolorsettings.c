/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

/*
** muiclass_windowcolorsettings.c
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
#include <mui/NList_mcc.h>
#include <mui/NListview_mcc.h>

#include "locale.h"
#include "muiclass.h"
#include "muiclass_windowcolorsettings.h"
#include "version.h"
#include "intern.h"

/*************************************************************************/

#define LINEBUFFER_SIZEOF 0x2000
#define DEFAULT_COLORSETTINGSDIR "PROGDIR:Colours/"
#define DEFAULT_COLORSETTINGS_NAME DEFAULT_COLORSETTINGSDIR "default.txt"

/*
** gadgets used by this class
*/

enum
{
GID_BACKGROUND = 0,
GID_NICKLISTBG,
GID_TABLISTBG,
GID_NORMAL,
GID_NICKLISTTX,
GID_TABSPEN,
GID_JOIN,
GID_PART,
GID_QUIT,
GID_MODES,
GID_CTCP,
GID_ACTIONS,
GID_INFO,
GID_OWNTEXT,
GID_HIGHLIGHT,
GID_NOTICES,
GID_INVITE,
GID_KICK,
GID_NICKCHANGE,
GID_TOPIC,
GID_WALLOPS,
GID_ACTIVITY,
GID_CHATACTIVITY,
GID_HIGHLIGHTACTIVITY,
GID_LOAD,
GID_SAVE,
GID_SAVEAS,
GID_LAST
};

#define GID_LASTCOLOR  GID_HIGHLIGHTACTIVITY
#define GID_FIRSTCOLOR GID_BACKGROUND
#define COLOR_NUMBEROF ( GID_HIGHLIGHTACTIVITY - GID_BACKGROUND )

/*
** data used by this class
*/

#define AREXXPORT_SIZEOF 0x100

struct mccdata
{
	Object                *mcc_ClassObjects[ GID_LAST ];
	struct MUI_PenSpec    *mcc_PenDisplaySpecs[ COLOR_NUMBEROF ];
	STRPTR popimage_background;
	STRPTR popimage_nicklistbackground;
	STRPTR popimage_tabsbackground;

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
			MUIA_Window_Title            , LGS( MSG_MUICLASS_WINDOWCOLORSETTINGS_TITLE ),
			MUIA_Window_ID               , MAKE_ID('C','O','L','S'),
			MUIA_Window_NoMenus	       	 , TRUE,

			WindowContents, VGroup,
				Child, ColGroup(6),
					Child, MUICreateLabel( MSG_MUICLASS_WINDOWCOLORSETTINGS_BACKGROUND_GAD ),
					Child, objs[ GID_BACKGROUND        ] = MUICreatePoppen( MSG_MUICLASS_WINDOWCOLORSETTINGS_BACKGROUND_GAD       , MSG_MUICLASS_WINDOWCOLORSETTINGS_CHOOSECOLOR_TITLE ),
					Child, MUICreateLabel( MSG_MUICLASS_WINDOWCOLORSETTINGS_NICKLISTBG_GAD ),
					Child, objs[ GID_NICKLISTBG        ] = MUICreatePoppen( MSG_MUICLASS_WINDOWCOLORSETTINGS_NICKLISTBG_GAD       , MSG_MUICLASS_WINDOWCOLORSETTINGS_CHOOSECOLOR_TITLE ),
					Child, MUICreateLabel( MSG_MUICLASS_WINDOWCOLORSETTINGS_TABLISTBG_GAD ),
					Child, objs[ GID_TABLISTBG         ] = MUICreatePoppen( MSG_MUICLASS_WINDOWCOLORSETTINGS_TABLISTBG_GAD        , MSG_MUICLASS_WINDOWCOLORSETTINGS_CHOOSECOLOR_TITLE ),

					Child, MUICreateLabel( MSG_MUICLASS_WINDOWCOLORSETTINGS_NORMAL_GAD ),
					Child, objs[ GID_NORMAL            ] = MUICreatePoppen( MSG_MUICLASS_WINDOWCOLORSETTINGS_NORMAL_GAD           , MSG_MUICLASS_WINDOWCOLORSETTINGS_CHOOSECOLOR_TITLE ),
					Child, MUICreateLabel( MSG_MUICLASS_WINDOWCOLORSETTINGS_NICKLISTTX_GAD ),
					Child, objs[ GID_NICKLISTTX        ] = MUICreatePoppen( MSG_MUICLASS_WINDOWCOLORSETTINGS_NICKLISTTX_GAD       , MSG_MUICLASS_WINDOWCOLORSETTINGS_CHOOSECOLOR_TITLE ),
					Child, MUICreateLabel( MSG_MUICLASS_WINDOWCOLORSETTINGS_TABSPEN_GAD ),
					Child, objs[ GID_TABSPEN           ] = MUICreatePoppen( MSG_MUICLASS_WINDOWCOLORSETTINGS_TABSPEN_GAD          , MSG_MUICLASS_WINDOWCOLORSETTINGS_CHOOSECOLOR_TITLE ),

					Child, MUICreateLabel( MSG_MUICLASS_WINDOWCOLORSETTINGS_JOIN_GAD ),
					Child, objs[ GID_JOIN              ] = MUICreatePoppen( MSG_MUICLASS_WINDOWCOLORSETTINGS_JOIN_GAD             , MSG_MUICLASS_WINDOWCOLORSETTINGS_CHOOSECOLOR_TITLE ),
					Child, MUICreateLabel( MSG_MUICLASS_WINDOWCOLORSETTINGS_PART_GAD ),
					Child, objs[ GID_PART              ] = MUICreatePoppen( MSG_MUICLASS_WINDOWCOLORSETTINGS_PART_GAD             , MSG_MUICLASS_WINDOWCOLORSETTINGS_CHOOSECOLOR_TITLE ),
					Child, MUICreateLabel( MSG_MUICLASS_WINDOWCOLORSETTINGS_QUIT_GAD ),
					Child, objs[ GID_QUIT              ] = MUICreatePoppen( MSG_MUICLASS_WINDOWCOLORSETTINGS_QUIT_GAD             , MSG_MUICLASS_WINDOWCOLORSETTINGS_CHOOSECOLOR_TITLE ),

					Child, MUICreateLabel( MSG_MUICLASS_WINDOWCOLORSETTINGS_MODES_GAD ),
					Child, objs[ GID_MODES             ] = MUICreatePoppen( MSG_MUICLASS_WINDOWCOLORSETTINGS_MODES_GAD            , MSG_MUICLASS_WINDOWCOLORSETTINGS_CHOOSECOLOR_TITLE ),
					Child, MUICreateLabel( MSG_MUICLASS_WINDOWCOLORSETTINGS_CTCP_GAD ),
					Child, objs[ GID_CTCP              ] = MUICreatePoppen( MSG_MUICLASS_WINDOWCOLORSETTINGS_CTCP_GAD             , MSG_MUICLASS_WINDOWCOLORSETTINGS_CHOOSECOLOR_TITLE ),
					Child, MUICreateLabel( MSG_MUICLASS_WINDOWCOLORSETTINGS_ACTIONS_GAD ),
					Child, objs[ GID_ACTIONS           ] = MUICreatePoppen( MSG_MUICLASS_WINDOWCOLORSETTINGS_ACTIONS_GAD          , MSG_MUICLASS_WINDOWCOLORSETTINGS_CHOOSECOLOR_TITLE ),

					Child, MUICreateLabel( MSG_MUICLASS_WINDOWCOLORSETTINGS_INFO_GAD ),
					Child, objs[ GID_INFO              ] = MUICreatePoppen( MSG_MUICLASS_WINDOWCOLORSETTINGS_INFO_GAD             , MSG_MUICLASS_WINDOWCOLORSETTINGS_CHOOSECOLOR_TITLE ),
					Child, MUICreateLabel( MSG_MUICLASS_WINDOWCOLORSETTINGS_OWNTEXT_GAD ),
					Child, objs[ GID_OWNTEXT           ] = MUICreatePoppen( MSG_MUICLASS_WINDOWCOLORSETTINGS_OWNTEXT_GAD          , MSG_MUICLASS_WINDOWCOLORSETTINGS_CHOOSECOLOR_TITLE ),
					Child, MUICreateLabel( MSG_MUICLASS_WINDOWCOLORSETTINGS_HIGHLIGHT_GAD ),
					Child, objs[ GID_HIGHLIGHT         ] = MUICreatePoppen( MSG_MUICLASS_WINDOWCOLORSETTINGS_HIGHLIGHT_GAD        , MSG_MUICLASS_WINDOWCOLORSETTINGS_CHOOSECOLOR_TITLE ),

					Child, MUICreateLabel( MSG_MUICLASS_WINDOWCOLORSETTINGS_NOTICES_GAD ),
					Child, objs[ GID_NOTICES           ] = MUICreatePoppen( MSG_MUICLASS_WINDOWCOLORSETTINGS_NOTICES_GAD          , MSG_MUICLASS_WINDOWCOLORSETTINGS_CHOOSECOLOR_TITLE ),
					Child, MUICreateLabel( MSG_MUICLASS_WINDOWCOLORSETTINGS_INVITE_GAD ),
					Child, objs[ GID_INVITE            ] = MUICreatePoppen( MSG_MUICLASS_WINDOWCOLORSETTINGS_INVITE_GAD           , MSG_MUICLASS_WINDOWCOLORSETTINGS_CHOOSECOLOR_TITLE ),
					Child, MUICreateLabel( MSG_MUICLASS_WINDOWCOLORSETTINGS_KICK_GAD ),
					Child, objs[ GID_KICK              ] = MUICreatePoppen( MSG_MUICLASS_WINDOWCOLORSETTINGS_KICK_GAD             , MSG_MUICLASS_WINDOWCOLORSETTINGS_CHOOSECOLOR_TITLE ),

					Child, MUICreateLabel( MSG_MUICLASS_WINDOWCOLORSETTINGS_NICKCHANGE_GAD ),
					Child, objs[ GID_NICKCHANGE        ] = MUICreatePoppen( MSG_MUICLASS_WINDOWCOLORSETTINGS_NICKCHANGE_GAD       , MSG_MUICLASS_WINDOWCOLORSETTINGS_CHOOSECOLOR_TITLE ),
					Child, MUICreateLabel( MSG_MUICLASS_WINDOWCOLORSETTINGS_TOPIC_GAD ),
					Child, objs[ GID_TOPIC             ] = MUICreatePoppen( MSG_MUICLASS_WINDOWCOLORSETTINGS_TOPIC_GAD            , MSG_MUICLASS_WINDOWCOLORSETTINGS_CHOOSECOLOR_TITLE ),
					Child, MUICreateLabel( MSG_MUICLASS_WINDOWCOLORSETTINGS_WALLOPS_GAD ),
					Child, objs[ GID_WALLOPS           ] = MUICreatePoppen( MSG_MUICLASS_WINDOWCOLORSETTINGS_WALLOPS_GAD          , MSG_MUICLASS_WINDOWCOLORSETTINGS_CHOOSECOLOR_TITLE ),

					Child, MUICreateLabel( MSG_MUICLASS_WINDOWCOLORSETTINGS_ACTIVITY_GAD ),
					Child, objs[ GID_ACTIVITY          ] = MUICreatePoppen( MSG_MUICLASS_WINDOWCOLORSETTINGS_ACTIVITY_GAD         , MSG_MUICLASS_WINDOWCOLORSETTINGS_CHOOSECOLOR_TITLE ),
					Child, MUICreateLabel( MSG_MUICLASS_WINDOWCOLORSETTINGS_CHATACTIVITY_GAD ),
					Child, objs[ GID_CHATACTIVITY      ] = MUICreatePoppen( MSG_MUICLASS_WINDOWCOLORSETTINGS_CHATACTIVITY_GAD     ,  MSG_MUICLASS_WINDOWCOLORSETTINGS_CHOOSECOLOR_TITLE ),
					Child, MUICreateLabel( MSG_MUICLASS_WINDOWCOLORSETTINGS_HIGHLIGHTACTIVITY_GAD ),
					Child, objs[ GID_HIGHLIGHTACTIVITY ] = MUICreatePoppen( MSG_MUICLASS_WINDOWCOLORSETTINGS_HIGHLIGHTACTIVITY_GAD,  MSG_MUICLASS_WINDOWCOLORSETTINGS_CHOOSECOLOR_TITLE ),
				End,
				Child, HGroup,
					Child, objs[ GID_LOAD   ] = MUICreateButton( MSG_MUICLASS_WINDOWCOLORSETTINGS_LOAD_GAD   ),
					Child, objs[ GID_SAVE   ] = MUICreateButton( MSG_MUICLASS_WINDOWCOLORSETTINGS_SAVE_GAD   ),
					Child, objs[ GID_SAVEAS ] = MUICreateButton( MSG_MUICLASS_WINDOWCOLORSETTINGS_SAVEAS_GAD ),
				End,
			End,
		TAG_DONE ) ) ) {

		struct mccdata *mccdata = INST_DATA( cl, obj );

		CopyMem( &objs[0], &mccdata->mcc_ClassObjects[0], sizeof( mccdata->mcc_ClassObjects));

		DoMethod( obj, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, obj, 3, MUIM_Set, MUIA_Window_Open, FALSE );

		DoMethod( obj, MM_WINDOWCOLORSETTINGS_LOADCOLORS, DEFAULT_COLORSETTINGS_NAME );

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

/* /// MCC_SaveColors()
**
*/

/*************************************************************************/

ULONG MM_SaveColors( struct IClass *cl, Object *obj, struct MP_WINDOWCOLORSETTINGS_SAVECOLORS *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
BPTR handle;
ULONG i;
STRPTR penspec;

	DoMethod( obj, MM_WINDOWCOLORSETTINGS_CREATEDIR );

	if( ( handle = Open( msg->Name, MODE_NEWFILE ) ) ) {
		for( i = 0 ; i < COLOR_NUMBEROF ; i++ ) {
			penspec = (STRPTR) MUIGetVar( mccdata->mcc_ClassObjects[ GID_FIRSTCOLOR + i ], MUIA_Pendisplay_Spec );
			FPuts( handle, (l_in) penspec );
			FPuts( handle, (l_in) "\n" );
		}
		Close( handle );
	}
	return( 0 );
}
/* \\\ */
/* /// MCC_LoadColors()
**
*/

/*************************************************************************/

ULONG MM_LoadColors( struct IClass *cl, Object *obj, struct MP_WINDOWCOLORSETTINGS_LOADCOLORS *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
BPTR handle;
ULONG i, length;
char *linebuffer;

	DoMethod( obj, MM_WINDOWCOLORSETTINGS_CREATEDIR );

	if( ( linebuffer = AllocVec( LINEBUFFER_SIZEOF, MEMF_ANY ) ) ) {

		if( ( handle = Open( msg->Name, MODE_OLDFILE ) ) ) {
			i = 0;
			while( FGets( handle, (STRPTR) linebuffer, LINEBUFFER_SIZEOF - 1 ) ) {
				if( ( length = strlen( linebuffer ) ) ) {
					char *tmp = &linebuffer[ --length ];
					while( length && ( ( *tmp == 0x0a ) || ( *tmp == 0x0d ) ) ) {
						*tmp-- = 0x00;
						length--;
					}
					SetAttrs( mccdata->mcc_ClassObjects[ GID_FIRSTCOLOR + i ], MUIA_Pendisplay_Spec, linebuffer, TAG_DONE );
					i++;
				}
			}
			Close( handle );
		}
		FreeVec( linebuffer );
	}

	//DoMethod( obj, MM_WINDOWCOLORSETTINGS_UPDATECOLORS );

	return( 0 );
}
/* \\\ */
/* /// MCC_CreateDir()
**
*/

/*************************************************************************/

ULONG MM_CreateDir( struct IClass *cl, Object *obj, Msg *msg )
{
BPTR lock;

	if( ( lock = CreateDir( (_s_cs) DEFAULT_COLORSETTINGSDIR ) ) ) {
		UnLock( lock );
	}
	return( 0 );
}
/* \\\ */


/* /// MCC_UpdateColors()
**
*/

/*************************************************************************/

ULONG MM_UpdateColors( struct IClass *cl, Object *obj, Msg *msg )
{
struct mccdata *mccdata = INST_DATA( cl, obj );
ULONG i;
STRPTR penspec;

	for( i = 0 ; i < COLOR_NUMBEROF ; i++ ) {

		penspec = (STRPTR) MUIGetVar( mccdata->mcc_ClassObjects[ GID_FIRSTCOLOR + i ], MUIA_Pendisplay_Spec );

		switch( GID_FIRSTCOLOR + i ) {
#if 1
			case GID_NICKLISTBG:
				if( status_current->current_query->LV_nicklist ) {
					SetAttrs( status_current->current_query->LV_nicklist, MUIA_NList_ListBackground, penspec, TAG_DONE );
				}
				break;
			case GID_TABLISTBG:
				if( WookieChat->LV_tabs ) {
					SetAttrs( WookieChat->LV_tabs, MUIA_NList_ListBackground, penspec, TAG_DONE );
				}
				break;
#endif
			case GID_BACKGROUND:
				if( status_current->current_query->LV_channel ) {
					SetAttrs( status_current->current_query->LV_channel, MUIA_NList_ListBackground, penspec, TAG_DONE );
				}
				break;
		}
	}
	DoMethod( status_current->current_query->LV_channel, MUIM_NList_Redraw, MUIV_NList_Redraw_All );

	return( 0 );
}
/* \\\ */

/*
** Dispatcher, init and dispose
*/

/* /// MCC_WindowColorSettings_Dispatcher()
**
*/

/*************************************************************************/

DISPATCHER(MCC_WindowColorSettings_Dispatcher)
{
    switch (msg->MethodID)
    {
		case OM_NEW                             : return( OM_New                           ( cl, obj, (APTR) msg ) );
		case OM_DISPOSE                         : return( OM_Dispose                       ( cl, obj, (APTR) msg ) );

		case MM_WINDOWCOLORSETTINGS_CREATEDIR   : return( MM_CreateDir                     ( cl, obj, (APTR) msg ) );
		case MM_WINDOWCOLORSETTINGS_LOADCOLORS  : return( MM_LoadColors                    ( cl, obj, (APTR) msg ) );
		case MM_WINDOWCOLORSETTINGS_SAVECOLORS  : return( MM_SaveColors                    ( cl, obj, (APTR) msg ) );

		case MM_WINDOWCOLORSETTINGS_UPDATECOLORS: return( MM_UpdateColors                  ( cl, obj, (APTR) msg ) );
	}
	return( DoSuperMethodA( cl, obj, msg ) );

}
/* \\\ */
/* /// MCC_WindowColorSettings_InitClass()
**
*/

/*************************************************************************/

ULONG MCC_WindowColorSettings_InitClass( void )
{
	appclasses[ CLASSID_WINDOWCOLORSETTINGS ] = MUI_CreateCustomClass( NULL, MUIC_Window, NULL, sizeof( struct mccdata ) ,  (APTR) ENTRY(MCC_WindowColorSettings_Dispatcher) );
	return( appclasses[ CLASSID_WINDOWCOLORSETTINGS ] ? MSG_ERROR_NOERROR : MSG_ERROR_UNABLETOSETUPMUICLASS );
}
/* \\\ */
/* /// MCC_WindowColorSettings_DisposeClass()
**
*/

/*************************************************************************/

void MCC_WindowColorSettings_DisposeClass( void )
{
	if( appclasses[ CLASSID_WINDOWCOLORSETTINGS ] ) {
		MUI_DeleteCustomClass( appclasses[ CLASSID_WINDOWCOLORSETTINGS ] );
		appclasses[ CLASSID_WINDOWCOLORSETTINGS ] = NULL;
    }
}
/* \\\ */



