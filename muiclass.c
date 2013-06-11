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

#include <clib/alib_protos.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/muimaster.h>
#include <libraries/mui.h>

#include <string.h>

#include "locale.h"
#include "intern.h"
#include "muiclass.h"
#include "muiclass_application.h"
#include "muiclass_windowmain.h"
#include "muiclass_windowquit.h"
#include "muiclass_windowabout.h"
#include "muiclass_windowcolorsettings.h"
#include "muiclass_windowignorelist.h"
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

/* /// MUIClass_Open()
**
*/

/*************************************************************************/

ULONG MUIClass_Open( void )
{
ULONG result;

	if( !(result = MCC_Application_InitClass() ) ) {
		if( !(result = MCC_WindowMain_InitClass() ) ) {
			if( !(result = MCC_WindowAbout_InitClass() ) ) {
				if( !(result = MCC_WindowURLGrabber_InitClass() ) ) {
					if( !(result = MCC_WindowIgnoreList_InitClass() ) ) {
						if( !(result = MCC_WindowColorSettings_InitClass() ) ) {
							if( !(result = MCC_WindowQuit_InitClass() ) ) {
								if( ( application = NewObject( appclasses[ CLASSID_APPLICATION ]->mcc_Class, NULL, TAG_DONE ) ) ) {
									DoMethod( application, MM_APPLICATION_STARTUP );
								}
							}
						}
					}
				}
			}
		}
	}
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

	MCC_WindowAbout_DisposeClass();
	MCC_WindowURLGrabber_DisposeClass();
	MCC_WindowIgnoreList_DisposeClass();
	MCC_WindowColorSettings_DisposeClass();
	MCC_WindowQuit_DisposeClass();
	MCC_WindowMain_DisposeClass();
	MCC_Application_DisposeClass();
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
