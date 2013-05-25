/*
    The contents of this file are subject to the AROS Public License Version 1.1 
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis, 
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
    for the specific language governing rights and limitations under the License.
*/

#ifndef MAKE_ID
#define MAKE_ID(a,b,c,d) ((ULONG) (a)<<24 | (ULONG) (b)<<16 | (ULONG) (c)<<8 | (ULONG) (d))
#endif


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <dos/dos.h>
#include <exec/types.h>
#include <exec/memory.h>
#include <exec/ports.h>
#include <exec/io.h>
#include <libraries/dos.h>
#include <libraries/dosextens.h>
#include <libraries/gadtools.h>
#include <libraries/asl.h>
#include <libraries/mui.h>
#include <devices/clipboard.h>
#include <workbench/workbench.h>
#include <intuition/intuition.h>
#include <intuition/classusr.h>
#include <graphics/gfxmacros.h>

#ifdef __GNUC__
#include <proto/alib.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/gadtools.h>
#include <proto/asl.h>
#include <clib/muimaster_protos.h>
#endif

#ifdef __SASC
#include <clib/alib_protos.h>
#include <clib/exec_protos.h>
#include <clib/dos_protos.h>
#include <clib/icon_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>
#include <clib/gadtools_protos.h>
#include <clib/utility_protos.h>
#include <clib/asl_protos.h>
#define REG(x) register __ ## x
#define ASM    __asm
#define SAVEDS __saveds
#include <pragmas/exec_sysbase_pragmas.h>
#include <pragmas/dos_pragmas.h>
#include <pragmas/icon_pragmas.h>
#include <pragmas/graphics_pragmas.h>
#include <pragmas/intuition_pragmas.h>
#include <pragmas/gadtools_pragmas.h>
#include <pragmas/utility_pragmas.h>
#include <pragmas/asl_pragmas.h>
#include <pragmas/muimaster_pragmas.h>
extern struct Library *SysBase,*IntuitionBase,*UtilityBase,*GfxBase,*DOSBase,*IconBase;
#endif

extern struct Library *MUIMasterBase;

#include <MUI/NListview_mcc.h>
//#include <MUI/NFloattext_mcc.h>



#ifdef __SASC
#include <proto/muimaster.h>
#endif


/*extern struct MUI_CustomClass *NLI_Class;

extern struct MUI_CustomClass *NLI_Create(void);
extern void NLI_Delete(void);*/

//#define MUIA_NLIMG_Spec 0x9d51ffff



#include <inline/exec.h>
#include <libraries/mui.h>
#include <exec/memory.h>

struct ObjApp
{
	APTR	App;
	APTR	WI_main;
	APTR	STR_usertext;
	APTR	BT_query;
	APTR	BT_queryCC;
};





/* GUI */

void DisposeApp(struct ObjApp * MBObj)
{
	MUI_DisposeObject((Object*)MBObj->App);
	FreeVec(MBObj);
}

APTR	GROUP_ROOT_0, GR_top;
APTR	GR_string_gadget, GR_bottom_group, GR_buttons, GR_server1_buttons;
APTR	LA_space;

struct ObjApp *CreateApp(void)
{
	struct ObjApp *MBObj;

	if (!(MBObj = (struct ObjApp *) AllocVec(sizeof(struct ObjApp), MEMF_PUBLIC|MEMF_CLEAR)))
		return(NULL);


	GR_top = GroupObject,
		MUIA_HelpNode, "GR_top",
		MUIA_Group_HorizSpacing, 0,
		MUIA_Group_VertSpacing, 0,
		//Child, GR_conductor,
	End;

	if(string_default)
    {
    	MBObj->STR_usertext = StringObject,
		//MUIA_Textinput_Multiline, FALSE,
		MUIA_Frame, MUIV_Frame_String,
		MUIA_HelpNode, "STR_usertext",
		MUIA_String_MaxLen, 800,
    	End;
    }
    else
    {
        MBObj->STR_usertext = BetterStringObject,
		//MUIA_Textinput_Multiline, FALSE,
		MUIA_Frame, MUIV_Frame_String,
		MUIA_HelpNode, "STR_usertext",
		//MUIA_String_MaxLen, 800,
    	End;
    }

	GR_string_gadget = GroupObject,
		MUIA_HelpNode, "GR_string_gadget",
		Child, MBObj->STR_usertext,
	End;


	GR_server1_buttons = GroupObject,
		MUIA_HelpNode, "GR_server1_buttons",
		MUIA_Group_Horiz, TRUE,
		MUIA_Group_HorizSpacing, 2,
	End;

	GR_buttons = GroupObject,
		MUIA_HelpNode, "GR_buttons",
		MUIA_Group_Horiz, TRUE,
		MUIA_Group_HorizSpacing, 2,
		Child, GR_server1_buttons,
	End;

	LA_space = TextObject,
		MUIA_Weight, 1,
		MUIA_Text_PreParse, "\033r",
		MUIA_Text_Contents, "",
		MUIA_InnerLeft, 0,
		MUIA_InnerRight, 0,
	End;


	GR_bottom_group = GroupObject,
		MUIA_HelpNode, "GR_bottom_group",
		MUIA_Group_Horiz, TRUE,
		MUIA_Group_HorizSpacing, 2,
		Child, GR_buttons,
		Child, LA_space,
	End;

	GROUP_ROOT_0 = GroupObject,
		MUIA_Group_HorizSpacing, 0,
		MUIA_Group_VertSpacing, 2,
		Child, GR_top,
		Child, GR_string_gadget,
		Child, GR_bottom_group,
	End;

	MBObj->WI_main = WindowObject,
		MUIA_Window_Title, "Wookiechat",
		MUIA_Window_ID, MAKE_ID('0', 'W', 'I', 'N'),
		WindowContents, GROUP_ROOT_0,
	End;

	MBObj->App = ApplicationObject,
		MUIA_Application_Author, "NONE",
		MUIA_Application_Base, "NONE",
		MUIA_Application_Title, "NONE",
		MUIA_Application_Version, "$VER: NONE XX.XX (XX.XX.XX)",
		MUIA_Application_Copyright, "NOBODY",
		MUIA_Application_Description, "NONE",
		SubWindow, MBObj->WI_main,
	End;



	if (!MBObj->App)
	{
		FreeVec(MBObj);
		return(NULL);
	}



	return(MBObj);
}
