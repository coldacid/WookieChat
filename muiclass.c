/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

/*
** muiclass.c
*/

#define MUI_OBSOLETE
#include <clib/alib_protos.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/muimaster.h>
#include <libraries/mui.h>
#include <intuition/intuition.h>
#include <utility/utility.h>
#include <stdarg.h>
#include <string.h>

#include "intern.h"
#include "locale.h"
#include "muiclass.h"
#include "muiclass_aliaslist.h"
#include "muiclass_application.h"
#include "muiclass_buttonlist.h"
#include "muiclass_channel.h"
#include "muiclass_channellist.h"
#include "muiclass_connectedlist.h"
#include "muiclass_eventlist.h"
#include "muiclass_ignorelist.h"
#include "muiclass_nicklist.h"
#include "muiclass_serverlist.h"
#include "muiclass_settingsalias.h"
#include "muiclass_settingsbutton.h"
#include "muiclass_settingscolor.h"
#include "muiclass_settingsdcc.h"
#include "muiclass_settingsevent.h"
#include "muiclass_settingsgeneral.h"
#include "muiclass_settingsgui.h"
#include "muiclass_settingslog.h"
#include "muiclass_settingsserver.h"
#include "muiclass_settingssound.h"
#include "muiclass_urllist.h"
#include "muiclass_userlist.h"
#include "muiclass_windowabout.h"
#include "muiclass_windowignorelist.h"
#include "muiclass_windowmain.h"
#include "muiclass_windowquit.h"
#include "muiclass_windowsettings.h"
#include "muiclass_windowurlgrabber.h"

#ifndef MUIA_Text_HiIndex
 #define MUIA_Text_HiIndex 0x804214f5
#endif

/*
** GUI Globals
*/

struct MUI_CustomClass *appclasses[ CLASSID_LAST ];
Object *application;

/*************************************************************************/

static char TAB_FIXWIDTH[] = "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW";

/* /// MUIClass_Open()
**
*/

/*************************************************************************/

ULONG MUIClass_Open( void )
{
ULONG result;

if( !(result = MCC_AliasList_InitClass() ) ) {
if( !(result = MCC_Application_InitClass() ) ) {
if( !(result = MCC_ButtonList_InitClass() ) ) {
if( !(result = MCC_ChannelList_InitClass() ) ) {
if( !(result = MCC_Channel_InitClass() ) ) {
if( !(result = MCC_ConnectedList_InitClass() ) ) {
if( !(result = MCC_EventList_InitClass() ) ) {
if( !(result = MCC_IgnoreList_InitClass() ) ) {
if( !(result = MCC_NickList_InitClass() ) ) {
if( !(result = MCC_ServerList_InitClass() ) ) {
if( !(result = MCC_SettingsAlias_InitClass() ) ) {
if( !(result = MCC_SettingsButton_InitClass() ) ) {
if( !(result = MCC_SettingsColor_InitClass() ) ) {
if( !(result = MCC_SettingsDCC_InitClass() ) ) {
if( !(result = MCC_SettingsEvent_InitClass() ) ) {
if( !(result = MCC_SettingsGeneral_InitClass() ) ) {
if( !(result = MCC_SettingsGUI_InitClass() ) ) {
if( !(result = MCC_SettingsLog_InitClass() ) ) {
if( !(result = MCC_SettingsSound_InitClass() ) ) {
if( !(result = MCC_SettingsServer_InitClass() ) ) {
if( !(result = MCC_URLList_InitClass() ) ) {
if( !(result = MCC_UserList_InitClass() ) ) {
if( !(result = MCC_WindowAbout_InitClass() ) ) {
if( !(result = MCC_WindowIgnoreList_InitClass() ) ) {
if( !(result = MCC_WindowMain_InitClass() ) ) {
if( !(result = MCC_WindowQuit_InitClass() ) ) {
if( !(result = MCC_WindowSettings_InitClass() ) ) {
if( !(result = MCC_WindowURLGrabber_InitClass() ) ) {
if( ( application = NewObject( appclasses[ CLASSID_APPLICATION ]->mcc_Class, NULL, TAG_DONE ) ) ) {
	DoMethod( application, MM_APPLICATION_STARTUP );
} else {
	result = MSG_ERROR_UNABLETOSETUPMUICLASS;
}
}}}}}}}}}}}}}}}}}}}}}}}}}}}}
   return( result );
}
/* \\\ */
/* /// MUIClass_Close()
**
*/

/*************************************************************************/

void MUIClass_Close( void )
{
	if( application ) {
		MUI_DisposeObject( application );
		application = NULL;
    }

	MCC_AliasList_DisposeClass();
	MCC_Application_DisposeClass();
	MCC_ButtonList_DisposeClass();
	MCC_ChannelList_DisposeClass();
	MCC_Channel_DisposeClass();
	MCC_ConnectedList_DisposeClass();
	MCC_EventList_DisposeClass();
	MCC_IgnoreList_DisposeClass();
	MCC_NickList_DisposeClass();
	MCC_ServerList_DisposeClass();
	MCC_SettingsAlias_DisposeClass();
	MCC_SettingsButton_DisposeClass();
	MCC_SettingsColor_DisposeClass();
	MCC_SettingsDCC_DisposeClass();
	MCC_SettingsEvent_DisposeClass();
	MCC_SettingsGeneral_DisposeClass();
	MCC_SettingsGUI_DisposeClass();
	MCC_SettingsLog_DisposeClass();
	MCC_SettingsSound_DisposeClass();
	MCC_SettingsServer_DisposeClass();
	MCC_URLList_DisposeClass();
	MCC_UserList_DisposeClass();
	MCC_WindowAbout_DisposeClass();
	MCC_WindowIgnoreList_DisposeClass();
	MCC_WindowMain_DisposeClass();
	MCC_WindowQuit_DisposeClass();
	MCC_WindowSettings_DisposeClass();
	MCC_WindowURLGrabber_DisposeClass();
}
/* \\\ */

/* /// MUIGetVar()
**
*/

/*************************************************************************/

ULONG MUIGetVar( Object *obj, ULONG attr )
{
ULONG var = 0;
	GetAttr( attr, obj, (void*) &var );
	return ( var );
}
/* \\\ */
/* /// MUIInitStringArray()
**
*/

/*************************************************************************/

void MUIInitStringArray( STRPTR array[], ULONG first, ULONG last )
{
ULONG i;

	for( i = first ; i < last + 1 ; i++ ) {
		array[ i - first ] = LGS( i );
    }
	array[ last - first + 1 ] = NULL;
}
/* \\\ */

/* /// MUIGetUnderScore()
*/

/*************************************************************************/

TEXT MUIGetUnderScore( ULONG text)
{
	if (text)
	{
		char *c;
		if( (c = strchr( (const char *) LGS( text ),'_')) )
		{
			return(ToLower(*(c+1)));
		}
	}
	return(0);
}
/* \\\ */
/* /// MUIGetKeyLocale()
**
*/

/*************************************************************************/

char MUIGetKeyLocale( ULONG text )
{
long i = 0;
STRPTR name = LGS( text );

	while( name[ i ] ) {
		if( name[ i++ ] == '_') {
			return( ToLower( name[ i ] ) );
        }
    }
	return( 0x00 );
}

/* \\\ */

/* /// GUI_GetKeyLocaleUpper()
**
*/

/*************************************************************************/

char MUIGetKeyLocaleUpper( ULONG text )
{
long i = 0;
STRPTR name = LGS( text );

	while( name[ i ] ) {
		if( name[ i++ ] == '_') {
			return( ToUpper( name[ i ] ) );
        }
    }
	return( 0x00 );
}
/* \\\ */
/* /// MUICreatePoppen()
**
** NOTE: Label must be followed by bubble help!
*/

/*************************************************************************/

APTR MUICreatePoppen( ULONG text, ULONG poptitle )
{
APTR poppen;

	return( poppen = MUI_NewObject(MUIC_Poppen,
										MUIA_ControlChar    , MUIGetUnderScore( text ),
										MUIA_CycleChain     , 1,
										MUIA_ShortHelp      , LGS( text + 1 ), /* HELP is always behind label in catalog */
	                                    InnerSpacing(0, 0),
										MUIA_Window_Title   , LGS( poptitle ),
										MUIA_FixWidth       , 14,
										MUIA_FixHeight      , 14,
										MUIA_Pendisplay_Spec, "r00000000,00000000,00000000",
										TAG_DONE ) );
}
/* \\\ */
/* /// MUICreateButton()
**
** NOTE: Label must be followed by bubble help!
*/

/*************************************************************************/

APTR MUICreateButton( ULONG text )
{
	return( MUI_NewObject(MUIC_Text,
										MUIA_Frame                              , MUIV_Frame_Button,
										MUIA_Font                               , MUIV_Font_Button,
										MUIA_Background                         , MUII_ButtonBack,
										MUIA_ControlChar                        , MUIGetUnderScore( text ),
										MUIA_CycleChain                         , 1,
										MUIA_ShortHelp                          , LGS( text+1),
										MUIA_Text_Contents                      , LGS(text),
										MUIA_Text_PreParse                      , "\33c", /* center gadget text */
										MUIA_Text_HiChar                        , MUIGetUnderScore( text ),
										MUIA_Text_HiIndex						, '_',
										MUIA_InputMode                          , MUIV_InputMode_RelVerify,
										TAG_DONE ) );
}
/* \\\ */
/* /// MUICreateSmallButton()
**
** NOTE: Label must be followed by bubble help!
*/

/*************************************************************************/

APTR MUICreateSmallButton( ULONG text )
{
	return( MUI_NewObject(MUIC_Text,
										MUIA_Frame                              , MUIV_Frame_Button,
										MUIA_Font                               , MUIV_Font_Button,
										MUIA_Background                         , MUII_ButtonBack,
										MUIA_ControlChar                        , MUIGetUnderScore( text ),
										MUIA_CycleChain                         , 1,
										MUIA_ShortHelp                          , LGS( text+1),
										MUIA_Text_Contents                      , LGS(text),
										MUIA_Text_PreParse                      , "\33c", /* center gadget text */
										MUIA_Text_HiChar                        , MUIGetUnderScore( text ),
										MUIA_Text_HiIndex						, '_',
										MUIA_InputMode                          , MUIV_InputMode_RelVerify,
										MUIA_HorizWeight, 0,
										MUIA_VertWeight, 0,
										TAG_DONE ) );
}
/* \\\ */
/* /// MUICreateLabel()
**
** Just like MUI label(), but with locale id as label
** text, MUIA_DoubleBuffer set and bubble help support.
**
** NOTE: Label must be followed by bubble help!
*/

/*************************************************************************/

APTR MUICreateLabel( ULONG text )
{
	return( TextObject,
			MUIA_Text_Contents    , LGS( text ),
			MUIA_Weight           , 0,
			MUIA_InnerLeft        , 0,
			MUIA_InnerRight       , 0,
			MUIA_FramePhantomHoriz, TRUE,
			MUIA_ShortHelp        , LGS( text+1), /* HELP is always behind label in catalog */
			MUIA_Text_HiIndex , '_',
            MUIA_Text_PreParse, "\33r",
			End );
}
/* \\\ */
/* /// MUICreateLabelLeft()
**
** Just like MUI label(), but with locale id as label
** text, MUIA_DoubleBuffer set and bubble help support.
**
** NOTE: Label must be followed by bubble help!
*/

/*************************************************************************/

APTR MUICreateLabelLeft( ULONG text )
{
	return( TextObject,
			MUIA_Text_Contents    , LGS( text ),
			MUIA_Weight           , 0,
			MUIA_InnerLeft        , 0,
			MUIA_InnerRight       , 0,
			MUIA_FramePhantomHoriz, TRUE,
			MUIA_ShortHelp        , LGS( text+1), /* HELP is always behind label in catalog */
			MUIA_Text_HiIndex , '_',
			MUIA_Text_PreParse, "\33l",
			End );
}
/* \\\ */
/* /// MUICreateCheckbox()
**
** NOTE: Label must be followed by bubble help!
*/

/*************************************************************************/

APTR MUICreateCheckbox( ULONG text, ULONG defstate )
{
	return( MUI_NewObject( MUIC_Image,   MUIA_Frame               , MUIV_Frame_ImageButton,
											MUIA_Background       , MUII_ButtonBack,
											MUIA_ControlChar      , MUIGetUnderScore( text ),
											MUIA_CycleChain       , 1,
											MUIA_ShortHelp        , LGS( text + 1 ), /* HELP is always behind label in catalog */
											MUIA_Image_FreeVert   , TRUE,
											MUIA_InputMode        , MUIV_InputMode_Toggle,
											MUIA_Image_Spec       , MUII_CheckMark,
											MUIA_Selected         , defstate,
											MUIA_ShowSelState     , FALSE,
											TAG_DONE) );

}
/* \\\ */
/* /// MUICreateCycle()
**
*/

/*************************************************************************/

APTR MUICreateCycle( ULONG text, APTR labels, ULONG first, ULONG last )
{

    if( first || last ) {
		MUIInitStringArray( labels, first, last );
    }

	return( MUI_NewObject( MUIC_Cycle, MUIA_Frame         , MUIV_Frame_Button,
                                        MUIA_Font          , MUIV_Font_Button,
										MUIA_ControlChar   , MUIGetKeyLocale( text ),
                                        MUIA_CycleChain    , 1,
										MUIA_ShortHelp     , (ULONG) LGS( text + 1 ),
										MUIA_Cycle_Entries , (ULONG) labels,
										TAG_DONE ) );
}
/* \\\ */
/* /// MUICreateSlider()
**
*/

/***************************************************************************/

APTR MUICreateSlider( ULONG text, ULONG def, ULONG min, ULONG max, ULONG format, ULONG fix )
{
ULONG length = strlen( TAB_FIXWIDTH );

	fix = length - fix;
	if( fix > length ) {
		fix = 0;
	}

	return( SliderObject,
					MUIA_ControlChar                            , LGS( text ),
					MUIA_Slider_Level                           , def,
					MUIA_Slider_Max                             , max,
					MUIA_Slider_Min                             , min,
					(format ? MUIA_Numeric_Format : TAG_IGNORE ), ( format ? (ULONG) LGS( format ) : (ULONG) 0 ),
					MUIA_ShortHelp                              , (ULONG) LGS( text + 1 ),
					(fix    ? MUIA_FixWidthTxt    : TAG_IGNORE ), &TAB_FIXWIDTH[ fix ],
					MUIA_CycleChain                             , 1,
					End );
}
/* \\\ */
/* /// MUICreateInteger()
**
*/

/*************************************************************************/

char *WIDTHTXTARRAY = "000000000";

APTR MUICreateInteger( ULONG text, ULONG maxchars )
{
	return( MUI_NewObject( MUIC_String, MUIA_Frame           , MUIV_Frame_String,
										MUIA_ControlChar     , MUIGetKeyLocale( text),
                                        MUIA_CycleChain      , 1,
										MUIA_ShortHelp       , LGS( text + 1 ),
                                        MUIA_String_MaxLen   , maxchars,
                                        MUIA_String_Accept   , "0123456789",
										MUIA_FixWidthTxt     , &WIDTHTXTARRAY[ strlen( (char *) WIDTHTXTARRAY ) - maxchars ],
										TAG_DONE ) );
}
/* \\\ */

/* /// MUICreateString()
**
** NOTE: Label must be followed by bubble help!
*/

APTR MUICreateString( ULONG text, ULONG maxchars )
{

	return( MUI_NewObject( MUIC_String,
							MUIA_Frame             , MUIV_Frame_String,
							MUIA_ControlChar       , MUIGetUnderScore( text ),
							MUIA_CycleChain        , 1,
							MUIA_ShortHelp         , LGS( text+1), /* HELP is always behind label in catalog */
							MUIA_String_MaxLen     , maxchars,
							TAG_DONE )
	);
}
/* \\\ */
/* /// MUICreateStringFixed()
**
** NOTE: Label must be followed by bubble help!
*/


APTR MUICreateStringFixed( ULONG text, ULONG maxchars )
{
ULONG size, length;

	length = strlen( TAB_FIXWIDTH );
	size = length - maxchars;
	if( size > length ) {
		size = 0;
	}

	return( MUI_NewObject( MUIC_String,
							MUIA_Frame             , MUIV_Frame_String,
							MUIA_ControlChar       , MUIGetUnderScore( text ),
							MUIA_CycleChain        , 1,
							MUIA_ShortHelp         , LGS( text+1), /* HELP is always behind label in catalog */
							MUIA_String_MaxLen     , maxchars,
							MUIA_FixWidthTxt       , &TAB_FIXWIDTH[ size ],
							TAG_DONE )
	);
}
/* \\\ */
/* /// MUICreatePopASL()
**
*/

/*************************************************************************/

APTR MUICreatePopASL ( ULONG text, ULONG maxchars, ULONG poptype, Object **strobj, struct TagItem *taglist )
{
Object *obj, *dummy;

/* now create the gadget */

	if( ( obj = MUI_NewObject( MUIC_Popasl,
                                         MUIA_CycleChain       , 1,
										 MUIA_ShortHelp , (ULONG) Locale_GetString( text + 1 ),
										 MUIA_Popstring_String , (ULONG) (*strobj = MUI_NewObject( MUIC_String, MUIA_Frame, MUIV_Frame_String,
																 MUIA_ControlChar, MUIGetKeyLocale( text ),
                                                                 MUIA_String_MaxLen, maxchars,
																 TAG_DONE ) ),
										 MUIA_Popstring_Button , (ULONG) ( dummy = PopButton(poptype) ),

										 taglist ? TAG_MORE : TAG_DONE, (ULONG) taglist ) ) ) {

		SetAttrs( dummy, MUIA_ControlChar, MUIGetKeyLocaleUpper( text ), MUIA_CycleChain, 1, TAG_DONE );
    }
	return( obj );
}
/* \\\ */

/* /// MUIDataspaceImportPoppen()
**
*/

/*************************************************************************/

struct MUI_PenSpec *MUIDataspaceImportPoppen( Object *poppen, Object *dataspace, ULONG objectid )
{
struct MUI_PenSpec *penspec;

	if( ( penspec = (struct MUI_PenSpec *) DoMethod( dataspace, MUIM_Dataspace_Find, objectid ) ) ) {
		SetAttrs( poppen, MUIA_Pendisplay_Spec, penspec, TAG_DONE );
	}
	return( penspec );
}
/* \\\ */
/* /// MUIDataspaceExportPoppen()
**
*/

/*************************************************************************/

void MUIDataspaceExportPoppen( Object *poppen, Object *dataspace, ULONG objectid )
{
struct MUI_PenSpec *penspec;

	if( ( penspec = (struct MUI_PenSpec *) MUIGetVar( poppen, MUIA_Pendisplay_Spec ) ) ) {
		DoMethod( dataspace, MUIM_Dataspace_Add, penspec, sizeof( struct MUI_PenSpec ), objectid );
	}
}
/* \\\ */


#ifdef __AROS__
IPTR NewObjectAROS( struct IClass *classPtr, UBYTE *classID, ULONG tag1, ... )
{
    AROS_SLOWSTACKTAGS_PRE(tag1)
    retval = (IPTR) NewObject(classPtr, classID, AROS_SLOWSTACKTAGS_ARG(tag1));
    AROS_SLOWSTACKTAGS_POST
}
IPTR DoSuperNew(struct IClass *cl, Object *obj, ULONG tag1, ...)
{
    AROS_SLOWSTACKTAGS_PRE(tag1)
    retval = (IPTR)DoSuperMethod(cl, obj, OM_NEW, AROS_SLOWSTACKTAGS_ARG(tag1));
    AROS_SLOWSTACKTAGS_POST
}
#endif


#ifdef __MORPHOS__
#ifdef ENABLE_GEITDEBUG
#define DEBUG
#include "debug.h"
#include "debug.c"
#undef DEBUG
#undef REG
#undef REGARGS
#undef STDARGS
#undef FAR
#undef INLINE
#else
#undef debug
void debug() {}
#endif /* ENABLE_GEITDEBUG */
#endif /* __MORPHOS__ */
