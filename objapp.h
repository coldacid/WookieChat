/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

#include <exec/types.h>

extern APTR MN1_Hide;
extern APTR MN1_SelectServer, MN1_NewTAB, MN1_NewGTAB,MN1_CloseTAB;
extern APTR MN_ClearAllHistory,MN_ClearHistory,  MN_SaveHistory, MN_MainSettings;
extern APTR MN_SaveSettings, MN_MUISettings;
extern APTR MN_ColourSettings, MN_windows_dcc2,  MN_windows_dcc, MN_urlgrabber, MN_ignorelist, MN_about, MN_quit, MN_MultiColumnDisplay, MN_Clipboard;
extern APTR MN_MuteSound;
extern APTR MN_cut, MN_copy, MN_paste;

extern APTR GR_string_gadget;
extern APTR GROUP_ROOT_0;
extern APTR GR_top;
extern APTR LV_events;
extern APTR GR_bottom_group;
extern APTR GR_buttons;
extern APTR GR_local_charset;
extern APTR STR_events_script;
extern APTR STR_events_args;
extern APTR CYCLE_events;


struct ObjApp *CreateApp();

BOOL create_custom_classes();
void delete_custom_classes();

struct MUI_CustomClass * get_nlist_class();
