/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

#include "includes.h"

#include <proto/codesets.h>
#include <proto/muimaster.h>
#include <mui/NList_mcc.h>
#include <mui/NListtree_mcc.h>
#include <mui/BetterString_mcc.h>
#include <libraries/gadtools.h>
#include <libraries/asl.h>

#include "intern.h"
#include "objapp.h"
#include "version.h"
#include "locale.h"
#include "muiclass.h"
#include "muiclass_windowabout.h"
#include "muiclass_windowquit.h"
#include "muiclass_windowurlgrabber.h"
#include "muiclass_windowignorelist.h"


APTR MN1_Hide, MN1_SelectServer, MN1_NewTAB, MN1_NewGTAB,MN1_CloseTAB, MN_ClearAllHistory,MN_ClearHistory,  MN_SaveHistory, MN_MainSettings;
APTR MN_SaveSettings, MN_MUISettings, MN_ColourSettings, MN_windows_dcc2,  MN_windows_dcc, MN_ignorelist, MN_urlgrabber, MN_about, MN_quit, MN_MultiColumnDisplay, MN_Clipboard;
APTR MN_MuteSound;
APTR MN_cut, MN_copy, MN_paste;

APTR GR_string_gadget;
APTR GROUP_ROOT_0;
APTR GR_top;
APTR LV_events;
APTR GR_bottom_group;
APTR GR_buttons;
APTR GR_local_charset;
APTR STR_events_script;
APTR STR_events_args;
APTR CYCLE_events;

/* Locals */

static APTR MNmenuBarLabel2,MN1_Main, MNmenuBarLabel0,MNmenuBarLabel1, MN_Settings, MN_windows, MN_Edit;

static APTR GROUP_ROOT_4, GR_grp_13, LA_servername, GR_grp_11, LA_port, LA_pass, GR_grp_12;
static APTR GROUP_ROOT_5, GR_grp_15, LA_groupname, GR_grp_16, GR_grp_17;
static APTR GR_ban, GR_ban_subgroup, GR_logging_splitup, GR_logging_splitup2;
static APTR LA_space;
static APTR GROUP_ROOT_3, GR_grp_0, LA_nicknames, GR_grp_1, LA_realname, GR_grp_2;
static APTR LA_username, GR_grp_3, REC_label_0, GR_grp_8, GR_grp_7, REC_label_0CC;
static APTR GR_grp_9, GR_grp_6;

// BELOW are colour setting window variables
#if 1
static APTR GROUP_ROOT_6;
static APTR LA_CSW_background;
static APTR LA_CSW_nicklistbackground;
static APTR LA_CSW_join;
static APTR LA_CSW_part;
static APTR LA_CSW_quit;
static APTR LA_CSW_modes;
static APTR LA_CSW_ctcp;
static APTR LA_CSW_actions;
static APTR LA_CSW_normal;
static APTR LA_CSW_info;
static APTR LA_CSW_owntext;
static APTR LA_CSW_highlight;
static APTR LA_CSW_notices;
static APTR LA_CSW_invite; //13
static APTR LA_CSW_kick;    //14
static APTR LA_CSW_nickchange;  //15
static APTR LA_CSW_topic;     //16
static APTR LA_CSW_wallops;    //17
static APTR LA_CSW_activity;
static APTR LA_CSW_chatactivity;
static APTR LA_CSW_highlightactivity;
static APTR LA_CSW_nicklisttext;
static APTR LA_CSW_listviewtabs_background; //22
static APTR LA_CSW_listviewtabs_normaltext; //33
#endif
static APTR DISPLAY_SERVER_WIN_GRP;
static APTR GR_autoconnect;
static APTR GROUP_ROOT_2_ALIASES, GR_alias1, GR_alias2;
static APTR GROUP_ROOT_2_SERVER, GROUP_ROOT_2, GROUP_ROOT_2_SECOND;
static APTR GROUP_ROOT_2_DCC,GROUP_ROOT_2_ACTIONS, obj_auto_open_query_tabs_when_msged_label;
static APTR GR_graphical_smileys;
static APTR obj_autojoin_channels_when_kicked_label, grp_autojoin_channels_when_kicked, GR_grp_32, GR_grp_33;
static APTR grp_dcc_settings1, grp_dcc_settings2,grp_dcc_settings3, grp_dcc_settings4, grp_dcc_settings5;
static APTR GR_grp_28, GR_grp_31, GR_dcc_grp_28,GR_dcc_grp_282, GR_dcc_grp_29,REC_label_6,REC_label_7, GR_grp_29;
static APTR obj_settings1_dcc, obj_settings2_dcc, obj_gethostid_dcc, obj_ipaddr_dcc, obj_port1_dcc, obj_port2_dcc;
static APTR obj_aux21, obj_aux22, obj_aux23, REC_label_5;
static APTR obj_aux17, obj_aux18, obj_aux19, REC_label_4, GR_grp_26;
static APTR obj_aux12, obj_aux13, obj_aux14, obj_aux15, REC_label_3, GR_grp_doubleclickactions;
static APTR obj_aux7, REC_label_2, GR_grp_24, obj_aux8, obj_aux9, obj_aux10, obj_aux11;
static APTR obj_aux2, obj_aux3, REC_label_1, GR_grp_23, obj_aux5, obj_aux6;
static APTR grp_auto_open_query_tabs_when_msged;
static APTR GROUP_ROOT_7, GROUP_ROOT_9;
static APTR GR_send_dcc, GR_dcc, GR_dcc_file_exists;
static APTR GROUP_ROOT_8;
static APTR GROUP_ROOT_10, GR_grp_19, LA_dcc_send_file, GR_grp_30, LA_dcc_send_nick;


static char *CYA_GroupTitleColor[20];
static struct TagItem blank_taglist[] = { {TAG_DONE, 0} };
static char background4[64];


#define string_default  0

#ifdef __AROS__
static IPTR NewObjectAROS( struct IClass *classPtr, UBYTE *classID, ULONG tag1, ... )
{
    AROS_SLOWSTACKTAGS_PRE(tag1)
    retval = (IPTR) NewObject(classPtr, classID, AROS_SLOWSTACKTAGS_ARG(tag1));
    AROS_SLOWSTACKTAGS_POST
}
#endif

#ifdef __AROS__
#define BS_Object NewObjectAROS(get_custom_class(CC_MUIC_BETTERSTRING)->mcc_Class,NULL
#else
#define BS_Object NewObject(get_custom_class(CC_MUIC_BETTERSTRING)->mcc_Class,NULL
#endif

#define CC_MUIC_NLIST           0
#define CC_MUIC_BETTERSTRING    1
#define CC_MUIC_WINDOW          2
#define CC_MUIC_GROUP           3
#define CC_MAX                  4

static struct MUI_CustomClass * classes[CC_MAX];

struct MUI_CustomClass * get_custom_class(ULONG id);

struct ObjApp *CreateApp(void)
{

    struct ObjApp *MBObj;
    struct MUI_CustomClass * nlist = get_custom_class(CC_MUIC_NLIST);
    int tcount = 0;

    if (!(MBObj = (struct ObjApp *) AllocVec(sizeof(struct ObjApp), MEMF_PUBLIC|MEMF_CLEAR)))
        return(NULL);

	static CONST_STRPTR titles2[ MSG_PREFS_EVENT_GROUPTITLE - MSG_PREFS_GENERAL_GROUPTITLE + 2 ];

	titles2[0] = LGS( MSG_PREFS_GENERAL_GROUPTITLE );
	titles2[1] = LGS( MSG_PREFS_GUI_GROUPTITLE );
	titles2[2] = LGS( MSG_PREFS_SERVERS_GROUPTITLE );
	titles2[3] = LGS( MSG_PREFS_LOGGING_GROUPTITLE );
	titles2[4] = LGS( MSG_PREFS_DCC_GROUPTITLE );
	titles2[5] = LGS( MSG_PREFS_SOUNDS_GROUPTITLE );
	titles2[6] = LGS( MSG_PREFS_ALIAS_GROUPTITLE );
	titles2[7] = LGS( MSG_PREFS_NICKLISTBUTTONS_GROUPTITLE );
	titles2[8] = LGS( MSG_PREFS_EVENT_GROUPTITLE );
	titles2[9] = NULL;


        #ifdef __AROS__
        MBObj->LV_tabs = (Object*)NewObjectAROS(nlist->mcc_Class,NULL, //nlist subclass
        #else
        MBObj->LV_tabs = (Object*)NewObject(nlist->mcc_Class,NULL, //nlist subclass
        #endif
        MUIA_Frame, MUIV_Frame_InputList,
        MUIA_NList_ConstructHook, &ConstructLI_channel2_TextHook,
        MUIA_NList_DestructHook, &DestructLI_channel2_TextHook,
        MUIA_NList_DisplayHook, &Display_listviewtabs_TextHook,
        MUIA_NList_AutoCopyToClip, FALSE,
        MUIA_NList_Input, TRUE,
        MUIA_NList_ListBackground, background4,
        MUIA_NList_TypeSelect,MUIV_NList_TypeSelect_Line,
    End;

    MBObj->LV_tabs = (Object*)NListviewObject,
        MUIA_HelpNode, "LV_nicklist",
        MUIA_NListview_Horiz_ScrollBar,MUIV_NListview_HSB_None,
        MUIA_NListview_Vert_ScrollBar,MUIV_NListview_VSB_Always,
        MUIA_Width, 150,
        //MUIA_MaxWidth, 150,
        MUIA_ObjectID, MAKE_ID('L', 'V', '0', '3'),
        MUIA_NListview_NList, MBObj->LV_tabs,
    End;

    //load graphical smilies

	MBObj->LA_local_charset=(Object*)Label( LGS( MSG_LOCAL_CHARSET ));

    STRPTR *array;
    if((array = CodesetsSupportedA(blank_taglist)))
    {
        int i, ii;

        for(i=0, ii=0; array[i] != NULL; i++)
        {
            charsets[i] = CodesetsFindA(array[i],NULL);
            if(!strstr((char *)array[i], "UTF"))
            {
                //local_selectable_charsets[ii] = CodesetsFindA((char*)array[i],NULL);
                local_charsets[ii]=malloc(sizeof(char) * 30);
                strcpy(local_charsets[ii],(char *)array[i]);
                ii++;
                //local_selectable_charsets[ii]=NULL;

            }

            remote_charsets[i]=malloc(sizeof(char) * 30);
            strcpy(remote_charsets[i],(char *)array[i]);

            if(DEBUG) printf("charset:%d %s\n",i,remote_charsets[i]);
        }


            charsets[i]=NULL;
            set_total_charsets(i-1);

            local_charsets[i+1]=NULL;
            remote_charsets[i+1]=NULL;

        //CodesetsFreeA(array, NULL);
    }
    MBObj->CYCLE_local_charset = CycleObject,
       MUIA_Weight,0,
       MUIA_Cycle_Entries, local_charsets,
    End;

    MBObj->WI_change_nick = (Object*)WindowObject,
		MUIA_Window_Title, LGS( MSG_CHANGENICK ),
        MUIA_Window_CloseGadget, TRUE,
        MUIA_Window_ID, MAKE_ID('9', 'z', 'z', 'z'),
        //MUIA_Background, MUII_SHINE,
        WindowContents, GroupObject,
            MUIA_Group_Columns, 2,
			Child,MBObj->LA_change_nick=Label( LGS( MSG_ENTERNEWNICK )),
            Child,MBObj->STR_change_nick=StringObject,MUIA_Frame, MUIV_Frame_String,MUIA_String_Reject," ./",End,
			Child,MBObj->BT_change_nick_change=SimpleButton( LGS( MSG_ENTERNICKCHANGE )),
			Child,MBObj->BT_change_nick_cancel=SimpleButton( LGS( MSG_ENTERNICKCANCEL )),
        End,
    End;

    //dcc send initiate window

	LA_dcc_send_file = (Object*)Label( LGS( MSG_FILE_TO_SEND ));

    MBObj->STR_PA_dcc_send_file = (Object*)String(my_settings.upload_path, 800);

    MBObj->PA_dcc_send_file = (Object*)PopButton(MUII_PopFile);

    MBObj->PA_dcc_send_file = (Object*)PopaslObject,
        MUIA_HelpNode, "PA_dcc_send_file",
        MUIA_Popasl_Type, ASL_FileRequest,
		ASLFR_TitleText, LGS( MSG_DCC_CHOOSE_FILENAME ),
        ASLFR_InitialDrawer,my_settings.upload_path,
        //ASLFR_InitialDrawer,"ram:",
        MUIA_Popstring_String, MBObj->STR_PA_dcc_send_file,
        MUIA_Popstring_Button, MBObj->PA_dcc_send_file,
    End;
    GR_grp_19 = (Object*)GroupObject,
        MUIA_HelpNode, "GR_grp_19",
        MUIA_Group_Columns, 2,
        Child, LA_dcc_send_file,
        Child, MBObj->PA_dcc_send_file,
    End;

	LA_dcc_send_nick = (Object*)Label((char*) LGS( MSG_RECIEVER ));

    MBObj->STR_dcc_send_nick = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_HelpNode, "STR_dcc_send_nick",
        MUIA_String_Contents,"",
    End;
    GR_grp_30= (Object*)GroupObject,
        MUIA_HelpNode, "GR_grp_1",
        MUIA_Group_Columns, 2,
        Child, LA_dcc_send_nick,
        Child, MBObj->STR_dcc_send_nick,
    End;

	MBObj->BT_dcc_send = (Object*)SimpleButton( LGS( MSG_BUTTON_SEND_FILE));

    GROUP_ROOT_10 = (Object*)GroupObject,
        Child, GR_grp_19,
        Child, GR_grp_30,
        Child, MBObj->BT_dcc_send,
    End;
    MBObj->WI_dcc_send = (Object*)WindowObject,
		MUIA_Window_Title, LGS( MSG_DCC_SEND_WINDOW ),
        MUIA_Window_ID, MAKE_ID('9', 'W', 'I', 'N'),
        WindowContents, GROUP_ROOT_10,
    End;

    MBObj->LV_graphical_smileys_preview = (Object*)NListObject,
        MUIA_Weight,100,
        MUIA_NList_ConstructHook, MUIV_NList_ConstructHook_String,
        MUIA_NList_DestructHook, MUIV_NList_DestructHook_String,
        MUIA_NList_TypeSelect,MUIV_NList_TypeSelect_Char,
        MUIA_NList_DisplayHook, &previewdisplay_TextHook,
    End;

    MBObj->LV_graphical_smileys_preview = (Object*)NListviewObject,
        MUIA_HelpNode, "LV_channel",
        MUIA_Weight, 100,
        MUIA_NListview_NList, MBObj->LV_graphical_smileys_preview,
    End;
    MBObj->WI_graphical_smileys_preview = (Object*)WindowObject,
		MUIA_Window_Title, LGS( MSG_GRAPHICAL_SMILEYS_PREVIEW ),
        MUIA_Window_CloseGadget, TRUE,
        MUIA_Window_ID, MAKE_ID('9', 'A', 'I', 'N'),
        WindowContents, MBObj->LV_graphical_smileys_preview,
    End;

    MBObj->GR_graphical_smileys_choose = (Object*)GroupObject,
        MUIA_Group_Columns, 10,
    End;
    MBObj->WI_graphical_smileys_choose = (Object*)WindowObject,
        MUIA_Window_Title, " ",
        MUIA_Window_CloseGadget, TRUE,
        MUIA_Window_ID, MAKE_ID('9', 'A', 'B', 'N'),
        WindowContents, MBObj->GR_graphical_smileys_choose,
    End;




//

    MBObj->LV_send_dcc = (Object*)NListObject,
        MUIA_Frame, MUIV_Frame_InputList,
        MUIA_ObjectID, MAKE_ID('1', 'D', 'C', 'C'),
        MUIA_NList_ConstructHook, &ConstructDCC_TextHook,
        MUIA_NList_DestructHook, &DestructDCC_TextHook,
        MUIA_NList_DisplayHook, &DisplayDCC_send_TextHook,
        MUIA_NList_Exports, MUIV_NList_Exports_ColWidth,
        MUIA_NList_Imports, MUIV_NList_Imports_ColWidth,
        //MUIA_NList_Format,"BAR,BAR,BAR,BAR,BAR,BAR,BAR,",
        MUIA_NList_Format,"WEIGHT=19 BAR,WEIGHT=15 BAR,WEIGHT=55 BAR,WEIGHT=10 BAR,WEIGHT=15 BAR,WEIGHT=15 BAR,WEIGHT=5 BAR,WEIGHT=15",
        //MUIA_NList_Format,"WEIGHT=-1 BAR,WEIGHT=15 BAR,WEIGHT=-1 BAR,WEIGHT=15 BAR,WEIGHT=15 BAR,WEIGHT=15 BAR,WEIGHT=5 BAR,WEIGHT=15",
        MUIA_NList_Title, TRUE,
        #ifndef __AROS__
        //#ifndef __MORPHOS__
        MUIA_NList_ActiveObjectOnClick,TRUE,
        //#endif
        #endif
    End;

    MBObj->LV_send_dcc = (Object*)NListviewObject,
        MUIA_HelpNode, "LV_channel",
        MUIA_Weight, 80,
        MUIA_NListview_NList, MBObj->LV_send_dcc,
    End;

	MBObj->BT_dcc_send_reoffer = (Object*)SimpleButton( LGS( MSG_REOFFER ));
	MBObj->BT_dcc_send_cancel = (Object*)SimpleButton( LGS( MSG_CANCEL_SEND ));
	MBObj->BT_dcc_send_remove = (Object*)SimpleButton( LGS( MSG_REMOVE  ));
	MBObj->BT_dcc_send_new = (Object*)SimpleButton( LGS( MSG_RESUME ));

    GR_send_dcc = (Object*)GroupObject,
        MUIA_Group_Columns, 4,
        Child, MBObj->BT_dcc_send_new,
        Child, MBObj->BT_dcc_send_cancel,
        Child, MBObj->BT_dcc_send_remove,
    End;
	MBObj->BT_dcc_resume = (Object*)SimpleButton( LGS( MSG_RESUME ));
	MBObj->BT_dcc_overwrite = (Object*)SimpleButton( LGS( MSG_OVERWRITE ));
	MBObj->BT_dcc_abort = (Object*)SimpleButton( LGS( MSG_ABORT ));
	MBObj->BT_dcc_rename = (Object*)SimpleButton( LGS( MSG_RENAME ));

    GR_dcc_file_exists = (Object*)GroupObject,
        MUIA_Group_Columns, 4,
        Child, MBObj->BT_dcc_resume,
        Child, MBObj->BT_dcc_overwrite,
        Child, MBObj->BT_dcc_rename,
        Child, MBObj->BT_dcc_abort,
    End;
    MBObj->TX_oldname = (Object*)TextObject,End;
    MBObj->TX_oldsize = (Object*)TextObject,End;
    MBObj->TX_newsize = (Object*)TextObject,End;

    GR_dcc_grp_29 = (Object*)GroupObject,
        MUIA_HelpNode, "GR_dcc_grp_29",
        Child, MBObj->TX_oldname,
        Child, MBObj->TX_oldsize,
        Child, MBObj->TX_newsize,
    End;

    GROUP_ROOT_8 = (Object*)GroupObject,
        Child, GR_dcc_grp_29,
        Child, GR_dcc_file_exists,
    End;
    MBObj->WI_dcc_file_exists = (Object*)WindowObject,
		MUIA_Window_Title, LGS( MSG_ALREADY_EXISTS_WINDOW_TITLE ),
        MUIA_Window_ID, MAKE_ID('6', 'W', 'I', 'N'),
        MUIA_Window_CloseGadget, FALSE,
        MUIA_Window_NoMenus, FALSE,
        WindowContents, GROUP_ROOT_8,
        MUIA_Window_Activate,FALSE,
    End;

	MBObj->LA_upload_path = (Object*)Label2( LGS( MSG_UPLOAD_PATH ));

    MBObj->STR_dcc_PA_label_1 = (Object*)String("",200);
    MBObj->PA_dcc_label_1 = (Object*)PopButton(MUII_PopDrawer);

    MBObj->PA_dcc_label_1 = (Object*)PopaslObject,
        ASLFR_DrawersOnly, TRUE,
		//MUIA_ShortHelp, LGS(39,"Choose a path to save your downloads to"),
        MUIA_Popasl_Type, 0,
        MUIA_Popstring_String, MBObj->STR_dcc_PA_label_1,
        MUIA_Popstring_Button, MBObj->PA_dcc_label_1,
    End;

    GR_dcc_grp_282 = (Object*)GroupObject,
        MUIA_HelpNode, "GR_dcc_grp_28",
        MUIA_Group_Horiz, TRUE,
        Child, MBObj->LA_upload_path,
        Child, MBObj->PA_dcc_label_1,
    End;
	MBObj->LA_download_path = (Object*)Label2( LGS( MSG_DOWNLOAD_PATH ));

    MBObj->STR_dcc_PA_label_0 = (Object*)String("",200);

    MBObj->PA_dcc_label_0 = (Object*)PopButton(MUII_PopDrawer);

    MBObj->PA_dcc_label_0 = (Object*)PopaslObject,
        ASLFR_DrawersOnly, TRUE,
		MUIA_ShortHelp, LGS( MSG_CHOOSE_DOWNLOAD_PATH ),
        MUIA_Popasl_Type, 0,
        MUIA_Popstring_String, MBObj->STR_dcc_PA_label_0,
        MUIA_Popstring_Button, MBObj->PA_dcc_label_0,
    End;
    GR_dcc_grp_28 = (Object*)GroupObject,
        MUIA_HelpNode, "GR_dcc_grp_28",
        MUIA_Group_Horiz, TRUE,
        Child, MBObj->LA_download_path,
        Child, MBObj->PA_dcc_label_0,
    End;

	MBObj->BT_dcc_accept = (Object*)SimpleButton( LGS( MSG_ACCEPT ));
	MBObj->BT_dcc_cancel = (Object*)SimpleButton( LGS( MSG_CANCEL_TRANSFER ));
	MBObj->BT_dcc_remove = (Object*)SimpleButton( LGS( MSG_REMOVE ));
    GR_dcc = (Object*)GroupObject,
        MUIA_Group_Columns, 3,
        Child, MBObj->BT_dcc_accept,
        Child, MBObj->BT_dcc_cancel,
        Child, MBObj->BT_dcc_remove,
    End;

    MBObj->LV_dcc = (Object*)NListObject,
        MUIA_Frame, MUIV_Frame_InputList,
        MUIA_ObjectID, MAKE_ID('2', 'D', 'C', 'C'),
        MUIA_NList_ConstructHook, &ConstructDCC_TextHook,
        MUIA_NList_DestructHook, &DestructDCC_TextHook,
        MUIA_NList_DisplayHook, &DisplayDCC_recv_TextHook,
        //MUIA_NList_Format,"BAR,BAR,BAR,BAR,BAR,BAR,BAR,",
        MUIA_NList_Exports, MUIV_NList_Exports_ColWidth,
        MUIA_NList_Imports, MUIV_NList_Imports_ColWidth,
        //MUIA_NList_Format,"WEIGHT=-1 BAR,WEIGHT=15 BAR,WEIGHT=-1 BAR,WEIGHT=15 BAR,WEIGHT=15 BAR,WEIGHT=15 BAR,WEIGHT=5 BAR,WEIGHT=15",
        MUIA_NList_Format,"WEIGHT=19 BAR,WEIGHT=15 BAR,WEIGHT=55 BAR,WEIGHT=10 BAR,WEIGHT=15 BAR,WEIGHT=15 BAR,WEIGHT=5 BAR,WEIGHT=15",
        MUIA_NList_Title, TRUE,
        #ifndef __AROS__
        MUIA_NList_ActiveObjectOnClick,TRUE,
        #endif
    End;
    MBObj->LV_dcc = (Object*)NListviewObject,
        MUIA_HelpNode, "LV_channel",
        MUIA_Weight, 80,
        MUIA_NListview_NList, MBObj->LV_dcc,
    End;

	MBObj->LA_download_path2 = (Object*) Label2( LGS( MSG_DOWNLOAD_PATH ));
    MBObj->TX_download_path = (Object*)TextObject,
        MUIA_Frame, MUIV_Frame_Text,
        MUIA_Weight, 100,
        MUIA_Text_PreParse, NULL,
        MUIA_Text_Contents, NULL,
    End;
    APTR GR_download_path = (Object*)GroupObject,
        MUIA_Group_Columns, 2,
        Child, MBObj->LA_download_path2,
        Child, MBObj->TX_download_path,
    End;

    GROUP_ROOT_7 = (Object*)GroupObject,
        //MUIA_FrameTitle, "Incoming File Transfers",
        MUIA_Group_Columns, 1,
        Child, GR_download_path,
        Child, MBObj->LV_dcc,
        Child, GR_dcc,
    End;
    GROUP_ROOT_9 = (Object*)GroupObject,
        //MUIA_FrameTitle, "Outgoing File Transfers",
        Child, MBObj->LV_send_dcc,
        Child, GR_send_dcc,
    End;

    MBObj->WI_dcc = (Object*)WindowObject,
		MUIA_Window_Title, LGS( MSG_WINDOW_TITLE_DCC_INCOMING ),
        MUIA_Window_ID, MAKE_ID('5', 'W', 'I', 'N'),
        MUIA_Window_NoMenus, FALSE,
        WindowContents, GROUP_ROOT_7,
        MUIA_Window_Activate,FALSE,
    End;

    MBObj->WI_dcc2 = (Object*)WindowObject,
		MUIA_Window_Title, LGS( MSG_WINDOW_TITLE_DCC_OUTGOING ),
        MUIA_Window_ID, MAKE_ID('8', 'W', 'I', 'N'),
        MUIA_Window_NoMenus, FALSE,
        WindowContents, GROUP_ROOT_9,
        MUIA_Window_Activate,FALSE,
    End;

    //below is the main settings window and objects
	MBObj->RA_nick_completion_style_Label= (Object*)Label2( LGS( MSG_PREFS_NICK_COMPLETION_STYLE ));

    static const char *RA_nick_completion_style_Titles[] =
    {
       "AmIRC", "mIRC", NULL
    };

    MBObj->RA_nick_completion_style = (Object*)CycleObject,
       MUIA_Cycle_Entries, RA_nick_completion_style_Titles,
		MUIA_ShortHelp, LGS( MSG_PREFS_NICK_COMPLETION_STYLE_HELP ),
    End;
    GR_grp_32=(Object*)GroupObject,
        //MUIA_Group_Columns, 2,
        MUIA_Group_Horiz, TRUE,
        //Child, HSpace(0),
        Child, MBObj->RA_nick_completion_style_Label,
        Child, MBObj->RA_nick_completion_style,
        Child, HSpace(0),
    End;
	static CONST_STRPTR RA_GroupTitleColor[6];

	RA_GroupTitleColor[0] = LGS( MSG_OVERWRITE );
	RA_GroupTitleColor[1] = LGS( MSG_RESUME );
	RA_GroupTitleColor[2] = LGS( MSG_ABORT );
	RA_GroupTitleColor[3] = LGS( MSG_ASK );
	RA_GroupTitleColor[4] = LGS( MSG_RENAME );
	RA_GroupTitleColor[5] = NULL;

    MBObj->RA_autoaccept = (Object*)CycleObject,
       MUIA_Cycle_Entries, RA_GroupTitleColor,
    End;

    MBObj->CH_autoaccept = (Object*)CheckMark(FALSE);
    #ifdef __AROS__
    MBObj->LV_trusted_nicks = (Object*)NewObjectAROS(nlist->mcc_Class,NULL,// NListObject,
    #else
    MBObj->LV_trusted_nicks = (Object*)NewObject(nlist->mcc_Class,NULL,// NListObject,
        #endif
        MUIA_Frame, MUIV_Frame_InputList,
        MUIA_NList_ConstructHook, MUIV_NList_ConstructHook_String,
        MUIA_NList_DestructHook, MUIV_NList_DestructHook_String,
        MUIA_NList_ListBackground, popimage_background,
        MUIA_NList_TypeSelect,MUIV_NList_TypeSelect_Char,
    End;
    MBObj->CH_autojoin_channels_when_kicked = (Object*)CheckMark(FALSE);

	obj_autojoin_channels_when_kicked_label = (Object*)Label2( LGS( MSG_PREFS_AUTO_REJOIN ));

    MBObj->CH_auto_open_query_tabs_when_msged = (Object*)CheckMark(FALSE);

	obj_auto_open_query_tabs_when_msged_label = (Object*)Label2( LGS( MSG_PREFS_AUTO_OPEN_TAB ));

    grp_autojoin_channels_when_kicked = (Object*)GroupObject,
        MUIA_Group_Horiz, TRUE,
        MUIA_Weight, 100,
        //Child, HSpace(0),
        Child, MBObj->CH_autojoin_channels_when_kicked,
        Child, obj_autojoin_channels_when_kicked_label,
        Child, HSpace(0),
    End;
    grp_auto_open_query_tabs_when_msged = (Object*)GroupObject,
        MUIA_Group_Horiz, TRUE,
        MUIA_Weight, 100,
        //Child, HSpace(0),
        Child, MBObj->CH_auto_open_query_tabs_when_msged,
        Child, obj_auto_open_query_tabs_when_msged_label,
        Child, HSpace(0),
    End;


    MBObj->STR_label_3 = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_HelpNode, "STR_label_3",
        MUIA_String_Accept, "-0123456789",
        MUIA_String_MaxLen, 15,
    End;

	obj_aux3 = (Object*)Label2( LGS( MSG_PREFS_MAX_LINES_BUFFER ));

    obj_aux2 = (Object*)GroupObject,
        MUIA_Group_Columns, 2,
        Child, obj_aux3,
        Child, MBObj->STR_label_3,
    End;

    REC_label_1 = (Object*)RectangleObject,
        MUIA_Rectangle_HBar, TRUE,
        MUIA_FixHeight, 8,
    End;

    MBObj->CH_label_2 = (Object*)CheckMark(FALSE);

	obj_aux5 = (Object*)Label2( LGS( MSG_PREFS_LOG_CHANNELS ));


    MBObj->CH_label_3 = (Object*)CheckMark(FALSE);

	obj_aux7 = (Object*)Label2( LGS( MSG_PREFS_LOG_PRIVATE ));
    obj_aux6 = (Object*)GroupObject,
        //MUIA_Group_Columns, 2,
        MUIA_Group_Horiz, TRUE,
        MUIA_Weight, 100,
        Child, MBObj->CH_label_3,
        Child, obj_aux7,
        Child, HSpace(0),
    End;

    GR_grp_23 = (Object*)GroupObject,
        MUIA_HelpNode, "GR_grp_23",
		//MUIA_FrameTitle, LGS(20,"Logging"),
        MUIA_Group_Horiz, TRUE,
        MUIA_Weight, 100,
        //Child, HSpace(0),
        Child, MBObj->CH_label_2,
        Child, obj_aux5,
        Child, HSpace(0),
        //Child, MBObj->CH_label_3,
        //Child, obj_aux7,
        //Child, HSpace(0),
    End;
    REC_label_2 = (Object*)RectangleObject,
        MUIA_Rectangle_HBar, TRUE,
        MUIA_FixHeight, 8,
    End;

    MBObj->STR_label_1 = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_HelpNode, "STR_label_1",
        MUIA_String_MaxLen, 800,
    End;
	obj_aux9 = (Object*)Label2( LGS( MSG_DEFAULT_KICK_MESSAGE ));

    obj_aux8 = (Object*)GroupObject,
        MUIA_Group_Columns, 2,
        Child, obj_aux9,
        Child, MBObj->STR_label_1,
    End;

    MBObj->STR_label_2 = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_HelpNode, "STR_label_2",
        MUIA_String_MaxLen, 800,
    End;
	obj_aux11 = (Object*)Label2( LGS( MSG_DEFAULT_BAN_MESSAGE ));

    obj_aux10 = (Object*)GroupObject,
        MUIA_Group_Columns, 2,
        Child, obj_aux11,
        Child, MBObj->STR_label_2,
    End;

    MBObj->STR_label_4 = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_HelpNode, "STR_label_4",
        MUIA_String_MaxLen, 400,
    End;

	obj_aux13 = (Object*)Label2( LGS( MSG_QUIT_MESSAGE ));
    obj_aux12 = (Object*)GroupObject,
        MUIA_Group_Columns, 2,
        Child, obj_aux13,
        Child, MBObj->STR_label_4,
    End;

    MBObj->STR_label_5 = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_HelpNode, "STR_label_5",
        MUIA_String_MaxLen, 400,
    End;

	obj_aux15 = (Object*)Label2( LGS( MSG_PART_MESSAGE ));

    obj_aux14 = (Object*)GroupObject,
        MUIA_Group_Columns, 2,
        Child, obj_aux15,
        Child, MBObj->STR_label_5,
    End;
    GR_grp_24 = (Object*)GroupObject,
        MUIA_HelpNode, "GR_grp_24",
        //MUIA_FrameTitle, "Messages",
        //Child, obj_aux2, //maximum lines to keep in buffer
        Child, obj_aux8,
        Child, obj_aux12,
        Child, obj_aux14,
    End;

    REC_label_3 = (Object*)RectangleObject,
        MUIA_Rectangle_HBar, TRUE,
        MUIA_FixHeight, 8,
    End;

    MBObj->CH_label_4 = (Object*)CheckMark(FALSE);

	obj_aux17 = (Object*)Label2( LGS( MSG_REQUEST_WHOIS ));

    MBObj->CH_label_5 = (Object*)CheckMark(FALSE);

	obj_aux19 = (Object*)Label2( LGS( MSG_OPEN_TAB ));

    obj_aux18 = (Object*)GroupObject,
        MUIA_Group_Horiz, TRUE,
        MUIA_Weight, 100,
        Child, MBObj->CH_label_5,
        Child, obj_aux19,
        Child, HSpace(0),
    End;
    APTR obj_aux182 = (Object*)GroupObject,
        MUIA_Group_Horiz, TRUE,
        MUIA_Weight, 100,
        Child, MBObj->CH_label_4,
        Child, obj_aux17,
        Child, HSpace(0),
    End;



    GR_grp_doubleclickactions = (Object*)GroupObject,
        MUIA_HelpNode, "GR_grp_25",
        MUIA_Weight, 100,
		MUIA_FrameTitle, LGS( MSG_DOUBLECLICK_GROUP_FRAME_TITLE ),
        MUIA_Frame, MUIV_Frame_Group,
        MUIA_Group_Horiz, FALSE,
        Child, obj_aux18,
        Child, obj_aux182,
    End;


    REC_label_4 = (Object*)RectangleObject,
        MUIA_Rectangle_HBar, TRUE,
        MUIA_FixHeight, 8,
    End;

    MBObj->STR_label_0 = (Object*)StringObject,
        MUIA_Weight, 100,
        MUIA_Frame, MUIV_Frame_String,
		MUIA_ShortHelp, LGS( MSG_CUSTOM_TIMESTAMPS ),
        MUIA_HelpNode, "STR_label_0",
        MUIA_String_MaxLen, 50,
    End;
	obj_aux21 = (Object*)Label2( LGS( MSG_TIMESTAMP_FORMAT ));

	MBObj->TX_highlight_label = (Object*)Label2( LGS( MSG_HIGHLIGHT ));

    MBObj->STR_highlight_label = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
		MUIA_ShortHelp, LGS( MSG_HIGHLIGHT_STRINGGADGET_HELP_BUBBLE ),
        MUIA_String_MaxLen, 800,
    End;


    GR_grp_26 = (Object*)GroupObject,
        MUIA_Group_Columns, 2,
		//MUIA_FrameTitle, LGS(64,"Highlight"),
        Child, MBObj->TX_highlight_label,
        Child, MBObj->STR_highlight_label,
    End;


    MBObj->CH_label_0 = (Object*)CheckMark(FALSE);
	obj_aux23 = (Object*)Label2( LGS( MSG_PREFS_USE_TIMESTAMPS ));

    obj_aux22 = (Object*)GroupObject,
		MUIA_FrameTitle, LGS( MSG_TIMESTAMPS_TITLE ),
        MUIA_Group_Horiz, TRUE,
        MUIA_Weight, 100,
        Child, MBObj->CH_label_0,
        Child, obj_aux23,
        Child, HSpace(0),
    End;

    APTR obj_aux222 = (Object*)GroupObject,
		//MUIA_FrameTitle, LGS(66,"Timestamps"),
        MUIA_Group_Horiz, TRUE,
        Child, obj_aux21,
        Child, MBObj->STR_label_0,
    End;


    REC_label_5 = (Object*)RectangleObject,
        MUIA_Rectangle_HBar, TRUE,
        MUIA_FixHeight, 8,
    End;
    MBObj->STR_PA_label_0 = (Object*)String("", 400);

    MBObj->PA_label_0 = (Object*)PopButton(MUII_PopFile);

    MBObj->PA_label_0 = (Object*)PopaslObject,
        MUIA_HelpNode, "PA_label_0",
		MUIA_ShortHelp, LGS( MSG_TEXT_EDITOR_HELP ),
        MUIA_Popstring_String, MBObj->STR_PA_label_0,
        MUIA_Popstring_Button, MBObj->PA_label_0,
    End;

    REC_label_6 = (Object*)RectangleObject,
        MUIA_Rectangle_HBar, TRUE,
        MUIA_FixHeight, 8,
    End;

	MBObj->LA_texteditor = (Object*)Label2( LGS( MSG_TEXT_EDITOR ));
    GR_grp_28 = (Object*)GroupObject,
        MUIA_HelpNode, "GR_grp_28",
        MUIA_Group_Horiz, TRUE,
        //MUIA_FrameTitle, "Text Editor",
        Child, MBObj->LA_texteditor,
        Child, MBObj->PA_label_0,
    End;

	MBObj->PA_browser_label = (Object*)Label2( LGS( MSG_WEB_BROWSER_LABEL ));

    MBObj->PA_browser = (Object*)PopButton(MUII_PopFile);

    MBObj->STR_browser = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
		MUIA_ShortHelp, LGS( MSG_WEB_BROWSER_HELP ),
        MUIA_String_MaxLen, 1000,
    End;

    MBObj->PA_browser = (Object*)PopaslObject,
        MUIA_Popasl_Type, ASL_FileRequest,
		ASLFR_TitleText, LGS( MSG_WEB_BROWSER_REQUESTER ),
        ASLFR_InitialDrawer,"progdir:",
        MUIA_Popstring_String, MBObj->STR_browser,
        MUIA_Popstring_Button, MBObj->PA_browser,
    End;


             //
    GR_grp_31 = (Object*)GroupObject,
        MUIA_HelpNode, "GR_grp_31",
        MUIA_Group_Horiz, TRUE,
        //MUIA_FrameTitle, "Web Browser",
        Child, MBObj->PA_browser_label,
        Child, MBObj->PA_browser,
    End;

	MBObj->BT_label_0 = (Object*)SimpleButton( LGS( MSG_SAVE ));

	MBObj->BT_label_1 = (Object*)SimpleButton( LGS( MSG_USE  ));

	MBObj->BT_label_2 = (Object*)SimpleButton( LGS( MSG_CANCEL ));

	MBObj->BT_label_3 = (Object*)SimpleButton( LGS( MSG_APPLY ));

    GR_grp_29 = (Object*)GroupObject,
        MUIA_HelpNode, "GR_grp_29",
        MUIA_Group_Columns, 4,
        Child, MBObj->BT_label_0,
        Child, MBObj->BT_label_1,
        Child, MBObj->BT_label_3,
        Child, MBObj->BT_label_2,
    End;

	obj_settings1_dcc = (Object*)Label2( LGS( MSG_DCC_AUTO_ACCEPT ));
	obj_settings2_dcc = (Object*)Label2( LGS( MSG_DCC_ACTION ));

	obj_gethostid_dcc = (Object*)Label2( LGS( MSG_DCC_LOCAL_ADDRESS ));
    MBObj->CH_gethostid = (Object*)CheckMark(FALSE);

    MBObj->CH_ipaddr_dcc = (Object*)CheckMark(FALSE);
	obj_ipaddr_dcc = (Object*)Label2( LGS( MSG_PREFS_DCC_ADDRESS ));
    MBObj->STR_ipaddr_dcc = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
		MUIA_ShortHelp, LGS( MSG_PREFS_DCC_ADDRESS_HELP ),
    End;

	obj_port1_dcc = (Object*)Label2( LGS( MSG_PORT_RANGE_START ));
	obj_port2_dcc = (Object*)Label2( LGS( MSG_PORT_RANGE_END ));

    MBObj->STR_port1_dcc = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_String_Reject," ",
		MUIA_ShortHelp, LGS( MSG_PORT_RANGE_START_HELP ),
    End;

    MBObj->STR_port2_dcc = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_String_Reject," ",
		MUIA_ShortHelp, LGS( MSG_PORT_RANGE_END_HELP ),
    End;

    REC_label_7 = (Object*)RectangleObject,
        MUIA_Rectangle_HBar, TRUE,
        MUIA_FixHeight, 8,
    End;

    grp_dcc_settings1 = (Object*)GroupObject,
        //MUIA_Group_Columns, 4,
        MUIA_Group_Horiz, TRUE,
        //Child, HSpace(0),
        Child, MBObj->CH_autoaccept,
        Child, obj_settings1_dcc,
        Child, HSpace(0),
    End;

    grp_dcc_settings2 = (Object*)GroupObject,
        //MUIA_Group_Columns, 1,
        MUIA_Group_Horiz, TRUE,
        //Child, HSpace(0),
        Child, obj_settings2_dcc,
        Child, MBObj->RA_autoaccept,
        Child, HSpace(0),
    End;

    grp_dcc_settings3 = (Object*)GroupObject,
        //MUIA_Group_Columns, 4,
        //Child, HSpace(0),
        MUIA_Group_Horiz, TRUE,
        //Child, HSpace(0),
        Child, MBObj->CH_gethostid,
        Child, obj_gethostid_dcc,
        Child, HSpace(0),
    End;
    grp_dcc_settings4 = (Object*)GroupObject,
        //MUIA_Group_Columns, 5,
        //Child, HSpace(0),
        MUIA_Group_Horiz, TRUE,
        Child, MBObj->CH_ipaddr_dcc,
        Child,obj_ipaddr_dcc,
        Child, MBObj->STR_ipaddr_dcc,
        //Child, HSpace(0),
    End;

    grp_dcc_settings5 = (Object*)GroupObject,
        //MUIA_Group_Columns, 6,
        MUIA_Group_Horiz, TRUE,
        //Child, HSpace(0),
        Child,obj_port1_dcc,
        Child,MBObj->STR_port1_dcc,
        Child,obj_port2_dcc,
        Child,MBObj->STR_port2_dcc,
        //Child, HSpace(0),
    End;


	MBObj->LA_Maximum_Retries = (Object*)Label2( LGS( MSG_MAX_RECONNECT_RETRIES ));

    MBObj->NB_Maximum_Retries = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_Weight, 100,
        MUIA_String_Accept, "0123456789",
    End;

	MBObj->LA_clone_detection = (Object*)Label2( LGS( MSG_MENU_ITEM_CLONE_DETECTION ));
    MBObj->CH_clone_detection = (Object*)CheckMark(FALSE);

    APTR GR_clone_detection = (Object*)GroupObject,
        MUIA_Weight, 100,
        MUIA_Group_Horiz, TRUE,
        Child, MBObj->CH_clone_detection,
        Child, MBObj->LA_clone_detection,
        Child,HSpace(0),
    End;


	MBObj->LA_Reconnect_Delay = (Object*)Label2( LGS( MSG_RECONNECT_DELAY ) );
    MBObj->NB_Reconnect_Delay = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_Weight, 100,
        MUIA_String_Accept, "0123456789",
    End;


    APTR GR_server1 = (Object*)GroupObject,
        MUIA_Group_Horiz, TRUE,
        MUIA_Weight, 100,
        Child,MBObj->LA_Maximum_Retries,    //maximum retrys (0 to turn off)
        Child,MBObj->NB_Maximum_Retries,    //maximum retrys (0 to turn off)
    End;

    APTR GR_server2 = (Object*)GroupObject,
        MUIA_Group_Horiz, TRUE,
        MUIA_Weight, 100,
        Child,MBObj->LA_Reconnect_Delay,   //reconnect delay (in seconds)
        Child,MBObj->NB_Reconnect_Delay,   //reconnect delay (in seconds)
    End;

    GROUP_ROOT_2_SERVER = (Object*)GroupObject,
        MUIA_Weight, 100,
        MUIA_Group_Columns, 1,
        Child, GR_server1,
        Child, GR_server2,
        Child, GR_clone_detection,
        Child, (Object*)GroupObject,
            MUIA_Weight, 100,
            MUIA_Group_Horiz, TRUE,
            Child, MBObj->CH_hide_joins_parts=(Object*)CheckMark(FALSE),
			Child, MBObj->LA_hide_joins_parts=(Object*)Label2( LGS( MSG_HIDEJOINANDPARTMESSAGES )),
            Child, HSpace(0),
        End,
        Child, (Object*)GroupObject,
            MUIA_Weight, 100,
            MUIA_Group_Horiz, TRUE,
            Child, MBObj->CH_hide_channel_mode_gadgets=(Object*)CheckMark(FALSE),
			Child, MBObj->LA_hide_channel_mode_gadgets=(Object*)Label2( LGS( MSG_HIDECHANELMODEBUTTONS )),
            Child, HSpace(0),
        End,
    End;
    //GUI SETTINGS objects
	MBObj->LA_no_server_tabs = (Object*)Label2( LGS( MSG_NO_SERVER_TABS_PREFS ));
    MBObj->CH_no_server_tabs = (Object*)CheckMark(FALSE);

    static CONST_STRPTR CYCLE_tabs_entries[] =
    {
       (CONST_STRPTR)"Listview",
       (CONST_STRPTR)"Buttons",
       NULL,
    };

	CYCLE_tabs_entries[0] = LGS( MSG_CY_LISTVIEW );
	CYCLE_tabs_entries[1] = LGS( MSG_CY_BUTTONS );

	//MBObj->LA_tabs = (Object*)Label2(LGS(296,"Use button-style tabs"));
    //MBObj->CH_tabs = (Object*)CheckMark(TRUE);
	MBObj->LA_tabs = (Object*)Label2( LGS( MSG_METHODOFDISPLAYINGTABS ));

    MBObj->CYCLE_tabs = (Object*)CycleObject,
       MUIA_Weight,0,
       MUIA_Cycle_Entries, CYCLE_tabs_entries,
    End;
    //MBObj->CH_tabs = (Object*)CheckMark(TRUE);



	MBObj->LA_listview_tabs_weight = (Object*)Label2( LGS( MSG_LABEL_LISTVIEW_TABS_WEIGHT ));
    MBObj->NB_listview_tabs_weight = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_Weight, 100,
        MUIA_String_Accept, "0123456789",
    End;

	MBObj->LA_nicklist_horiz_weight = (Object*)Label2( LGS( MSG_PREFS_NICKLIST_HORIZONTAL_WEIGHT ));

    MBObj->NB_nicklist_horiz_weight = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_Weight, 100,
        MUIA_String_Accept, "0123456789",
    End;


    APTR GR_grp_34 = (Object*)GroupObject,
        MUIA_HelpNode, "GR_grp_29",
        //MUIA_Group_Columns, 2,
        MUIA_Group_Horiz, TRUE,
        Child,MBObj->LA_listview_tabs_weight,
        Child,MBObj->NB_listview_tabs_weight,
        //Child,HSpace(0),
        //Child,MBObj->LA_nicklist_horiz_weight,
        //Child,MBObj->NB_nicklist_horiz_weight,
    End;

    APTR GR_grp_342 = (Object*)GroupObject,
        MUIA_HelpNode, "GR_grp_29",
        //MUIA_Group_Columns, 2,
        MUIA_Group_Horiz, TRUE,
        //Child,MBObj->LA_listview_tabs_weight,
        //Child,MBObj->NB_listview_tabs_weight,
        Child,MBObj->LA_nicklist_horiz_weight,
        Child,MBObj->NB_nicklist_horiz_weight,
        //Child,HSpace(0),
    End;
    GR_grp_33 = (Object*)GroupObject,
		MUIA_FrameTitle, LGS( MSG_GROUP_FRAME_TITLE_TAB_PREFERENCES ),
        //MUIA_Frame,MUIV_Frame_Group,
        MUIA_HelpNode, "GR_grp_29",
        MUIA_Group_Horiz, TRUE,
        //MUIA_Group_Columns, 4,
		//MUIA_ShortHelp,LGS(297,"Turn off this option to use Listview-style tabs (the listview is better having a large amount of tabs open, such as 20 or 30)"),
        //Child, HSpace(0),
        Child,MBObj->CYCLE_tabs,
        Child,MBObj->LA_tabs,
        //Child,MBObj->CH_no_server_tabs,
        //Child,MBObj->LA_no_server_tabs,
        Child, HSpace(0),
    End;
    APTR GR_grp_332 = (Object*)GroupObject,
		//MUIA_FrameTitle,LGS(307,"Tab Preferences"),
        //MUIA_Frame,MUIV_Frame_Group,
        MUIA_HelpNode, "GR_grp_29",
        MUIA_Group_Horiz, TRUE,
        Child,MBObj->CH_no_server_tabs,
        Child,MBObj->LA_no_server_tabs,
        Child, HSpace(0),
    End;

	setmacro((Object*)MBObj->LA_tabs,MUIA_ShortHelp, LGS( MSG_PREFS_TOGGLE_BUTTON_TABS_HELP ));
	setmacro((Object*)MBObj->LA_listview_tabs_weight,MUIA_ShortHelp, LGS( MSG_PREFS_HELP_BUBBLE_LISTVIEW_TABS_WEIGHT ));
	setmacro((Object*)MBObj->LA_no_server_tabs,MUIA_ShortHelp, LGS( MSG_PREFS_HELP_BUBBLE_NO_SERVER_TABS ));

	MBObj->LA_graphical_smileys = (Object*)Label2( LGS( MSG_CHECKBOX_ENABLE_GRAPHICAL_SMILEYS ));
    MBObj->CH_graphical_smileys = (Object*)CheckMark(TRUE);

	MBObj->LA_graphical_nicklist = (Object*)Label2( LGS( MSG_PREFS_GRAPHICAL_NICKLIST_CHECKBOX ));
    MBObj->CH_graphical_nicklist = (Object*)CheckMark(TRUE);


	MBObj->RA_graphical_smileys_background_colour_label= (Object*)Label2( LGS( MSG_PREFS_SMILEYS_THEME ));
    //char graphical_smiley_themes[10][100];

    if((tcount = find_themes()) > 0)
    {
        int count2;
        for(count2=0; count2<tcount; count2++)
        {
            CYA_GroupTitleColor[count2]=malloc(sizeof(char) * 100);
            strcpy(CYA_GroupTitleColor[count2],graphical_smiley_themes[count2]);
        }
        CYA_GroupTitleColor[count2]=NULL;

        MBObj->graphical_smileys_set = CycleObject,
         MUIA_Cycle_Entries, CYA_GroupTitleColor,
        End;
    }
    else
    {
        CYA_GroupTitleColor[0]=malloc(sizeof(char) * 100);
        strcpy(CYA_GroupTitleColor[0],"");

        CYA_GroupTitleColor[1]=NULL;

        MBObj->graphical_smileys_set = CycleObject,
         MUIA_Cycle_Entries, CYA_GroupTitleColor,
        End;


    }
    MBObj->BT_graphical_smileys_preview = (Object*)SimpleButton("?");
    setmacro((Object*)MBObj->BT_graphical_smileys_preview,MUIA_Weight,0);

    GR_graphical_smileys=(Object*)GroupObject,
		MUIA_ShortHelp, LGS( MSG_GRAPHICAL_SMILEY_SHORTHELP ),
        MUIA_HelpNode, "GR_grp_31",
        MUIA_Group_Horiz, FALSE,
        MUIA_Group_Horiz, TRUE,
        Child, MBObj->CH_graphical_smileys,
        Child, MBObj->LA_graphical_smileys,
        Child, HSpace(0),
    End;

    APTR GR_graphical_nicklist=(Object*)GroupObject,
		MUIA_ShortHelp, LGS( MSG_GRAPHICAL_SMILEY_SHORTHELP ),
        MUIA_HelpNode, "GR_grp_31",
        MUIA_Group_Horiz, FALSE,
        MUIA_Group_Horiz, TRUE,
        Child, MBObj->CH_graphical_nicklist,
        Child, MBObj->LA_graphical_nicklist,
        Child, HSpace(0),
    End;
	MBObj->LA_column_width_timestamp = (Object*)Label2( LGS( MSG_PREFS_COLUMN_WIDTH_TIMESTAMP ));

    MBObj->NB_column_width_timestamp = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_Weight, 100,
        MUIA_String_Accept, "0123456789",
    End;


	MBObj->LA_column_width_nick = (Object*)Label2( LGS( MSG_PREFS_COLUMN_WIDTH_MIDDLE_NICKS  ));

    MBObj->NB_column_width_nick = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_Weight, 100,
        MUIA_String_Accept, "0123456789",
    End;


	MBObj->LA_column_spacing = (Object*)Label2( LGS( MSG_PREFS_DIVIDER_BAR_SPACE ));

    //MBObj->NB_column_spacing = (Object*)NumericbuttonObject,
    MBObj->NB_column_spacing = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_Weight, 100,
        MUIA_String_Accept, "0123456789",
    End;


	MBObj->LA_remove_brackets = (Object*)Label2( LGS( MSG_PREFS_MULTICOLUMN_NICK_DISPLAY ));
    MBObj->CH_remove_brackets = (Object*)CheckMark(TRUE);

    APTR GR_column_width = (Object*)GroupObject,
        MUIA_Group_Horiz,TRUE,
		MUIA_FrameTitle, LGS( MSG_GROUP_FRAME_TITLE_MULTICOLUMN_DISPLAY_SETTINGS ),
        Child, MBObj->LA_column_width_timestamp,
        Child, MBObj->NB_column_width_timestamp,
        //Child,HSpace(0),
    End;

    APTR GR_column_width2 = (Object*)GroupObject,
        MUIA_Group_Horiz,TRUE,
        //MUIA_Group_Columns,2,
		//MUIA_FrameTitle, LGS(MSG_GROUP_FRAME_TITLE_MULTICOLUMN_DISPLAY_SETTINGS),
        //MUIA_Frame,MUIV_Frame_Group,
        Child, MBObj->LA_column_width_nick,
        Child, MBObj->NB_column_width_nick,
        //Child,HSpace(0),
    End;

    APTR GR_column_width3 = (Object*)GroupObject,
        MUIA_Group_Horiz,TRUE,
        //MUIA_Group_Columns,2,
		//MUIA_FrameTitle, LGS(MSG_GROUP_FRAME_TITLE_MULTICOLUMN_DISPLAY_SETTINGS),
        //MUIA_Frame,MUIV_Frame_Group,
        Child, MBObj->LA_column_spacing,
        Child, MBObj->NB_column_spacing,
        //Child,HSpace(0),
    End;

    APTR GR_column_width4 = (Object*)GroupObject,
        MUIA_Group_Horiz,TRUE,
        //MUIA_Group_Columns,2,
		//MUIA_FrameTitle, LGS(MSG_GROUP_FRAME_TITLE_MULTICOLUMN_DISPLAY_SETTINGS),
        //MUIA_Frame,MUIV_Frame_Group,
        Child, MBObj->CH_remove_brackets,
        Child, MBObj->LA_remove_brackets,
        Child, HSpace(0),
    End;

    APTR GR_smiley_sets = (Object*)GroupObject,
        MUIA_Group_Horiz,TRUE,
        //MUIA_Group_Columns,3,
        Child, MBObj->RA_graphical_smileys_background_colour_label,
        Child, MBObj->graphical_smileys_set,
        Child, MBObj->BT_graphical_smileys_preview,
        Child, HSpace(0),
    End;
    APTR GROUP_ROOT_2_GUI = (Object*)GroupObject,
        Child, obj_aux22,//timestamps
        Child, obj_aux222,//timestamps
        Child, GR_grp_33, //listview tabs or button tabs
        Child, GR_grp_332, //listview tabs or button tabs
        Child, GR_grp_34, //nicklist horizontal weight, and listview tabs vertical weight
        Child, GR_grp_342, //nicklist horizontal weight, and listview tabs vertical weight
        Child, GR_graphical_smileys,
        Child, GR_graphical_nicklist,
        Child, GR_smiley_sets,
        Child, GR_column_width,
        Child, GR_column_width2,
        Child, GR_column_width3,
        Child, GR_column_width4,
        Child, (Object*)GroupObject,
            MUIA_Group_Horiz,TRUE,
            Child, MBObj->CH_user_modes = (Object*)CheckMark(FALSE),
			Child, MBObj->LA_user_modes = (Object*)Label2( LGS( MSG_USERNAMEWHENTALK )),
            Child, HSpace(0),
        End,
        //Child, GR_column_width,
        //Child, GR_remove_brackets,
    End;
    GROUP_ROOT_2_DCC = (Object*)GroupObject,
        Child, grp_dcc_settings1,
        Child, grp_dcc_settings2,
        Child, grp_dcc_settings3,
        Child, grp_dcc_settings4,
        Child, grp_dcc_settings5,
        Child, GR_dcc_grp_28,
        Child, GR_dcc_grp_282,
    End;
    GROUP_ROOT_2 = (Object*)GroupObject,
        MUIA_Weight, 100,
        MUIA_HelpNode, "GR_grp_21",
        Child, grp_autojoin_channels_when_kicked,
        Child, grp_auto_open_query_tabs_when_msged,
        Child, GR_grp_32, //nick completion style group
        Child, GR_grp_24,
        Child, GR_grp_doubleclickactions,
        //Child, obj_aux18,
        Child, GR_grp_26,
        Child, GR_grp_28,
        Child, GR_grp_31,
        //Child, GR_grp_21,
    End;
    // new message sample

	static CONST_STRPTR RA_sound_sample_newmsg_entries[5];

	RA_sound_sample_newmsg_entries[0] = LGS( MSG_NEVER );
	RA_sound_sample_newmsg_entries[1] = LGS( MSG_SOUND_SAMPLE_PLAY_OPTION );
	RA_sound_sample_newmsg_entries[2] = LGS( MSG_ALWAYS );
	RA_sound_sample_newmsg_entries[3] = LGS( MSG_ONLY_WHEN_WINDOW_INACTIVE );
	RA_sound_sample_newmsg_entries[4] = NULL;

	MBObj->LA_sound_sample_newmsg = (Object*)Label2( LGS( MSG_SOUND_SAMPLE_NEW_MSG ));

    MBObj->STR_sound_sample_newmsg = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_ShortHelp, " ",
    End;

	MBObj->PA_sound_sample_newmsg_label = (Object*)Label2( LGS( MSG_PLAY_SAMPLE ));

    MBObj->PA_sound_sample_newmsg = (Object*)PopButton(MUII_PopFile);

    MBObj->PA_sound_sample_newmsg = (Object*)PopaslObject,
        MUIA_Popasl_Type, ASL_FileRequest,
		ASLFR_TitleText, LGS( MSG_CHOOSE_FILENAME ),
        ASLFR_InitialDrawer,"progdir:",
        MUIA_Popstring_String, MBObj->STR_sound_sample_newmsg,
        MUIA_Popstring_Button, MBObj->PA_sound_sample_newmsg,
    End;

    MBObj->RA_sound_sample_newmsg = (Object*)CycleObject,
       MUIA_Cycle_Entries, RA_sound_sample_newmsg_entries,
    End;

    //tabopen action entries

	static CONST_STRPTR RA_sound_sample_tabopen_entries[4];

	RA_sound_sample_tabopen_entries[0] = LGS( MSG_NEVER );
	RA_sound_sample_tabopen_entries[1] = LGS( MSG_ALWAYS );
	RA_sound_sample_tabopen_entries[2] = LGS( MSG_ONLY_WHEN_WINDOW_INACTIVE );
	RA_sound_sample_tabopen_entries[3] = NULL;

	MBObj->LA_sound_sample_tabopen = (Object*)Label2( LGS( MSG_SOUND_SAMPLE_TAB_OPEN ) );

    MBObj->STR_sound_sample_tabopen = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_ShortHelp, " ",
    End;

	MBObj->PA_sound_sample_tabopen_label = (Object*)Label2( LGS( MSG_PLAY_SAMPLE ));

    MBObj->PA_sound_sample_tabopen = (Object*)PopButton(MUII_PopFile);

    MBObj->PA_sound_sample_tabopen = (Object*)PopaslObject,
        MUIA_Popasl_Type, ASL_FileRequest,
		ASLFR_TitleText, LGS( MSG_CHOOSE_FILENAME ),
        ASLFR_InitialDrawer,"progdir:",
        MUIA_Popstring_String, MBObj->STR_sound_sample_tabopen,
        MUIA_Popstring_Button, MBObj->PA_sound_sample_tabopen,
    End;

    MBObj->RA_sound_sample_tabopen = (Object*)CycleObject,
       MUIA_Cycle_Entries, RA_sound_sample_tabopen_entries,
    End;


    //highlight action objects
	static CONST_STRPTR RA_sound_sample_highlight_entries[5];

	RA_sound_sample_highlight_entries[0] = LGS( MSG_NEVER );
	RA_sound_sample_highlight_entries[1] = LGS( MSG_ALWAYS );
	RA_sound_sample_highlight_entries[2] = LGS( MSG_ONLY_WHEN_WINDOW_INACTIVE );
	RA_sound_sample_highlight_entries[3] = LGS( MSG_ONLY_WHEN_TAB_IS_INACTIVE );
	RA_sound_sample_highlight_entries[4] = NULL;

	MBObj->LA_sound_sample_highlight = (Object*)Label2( LGS( MSG_SOUND_SAMPLE_HIGHLIGHT ));

    MBObj->STR_sound_sample_highlight = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_ShortHelp, " ",
    End;

	MBObj->PA_sound_sample_highlight_label = (Object*)Label2( LGS( MSG_PLAY_SAMPLE ));

    MBObj->PA_sound_sample_highlight = (Object*)PopButton(MUII_PopFile);
    MBObj->PA_sound_sample_highlight = (Object*)PopaslObject,
        MUIA_Popasl_Type, ASL_FileRequest,
		ASLFR_TitleText, LGS( MSG_CHOOSE_FILENAME ),
        ASLFR_InitialDrawer,"progdir:",
        MUIA_Popstring_String, MBObj->STR_sound_sample_highlight,
        MUIA_Popstring_Button, MBObj->PA_sound_sample_highlight,
    End;

    MBObj->RA_sound_sample_highlight = (Object*)CycleObject,
       MUIA_Cycle_Entries, RA_sound_sample_highlight_entries,
    End;

	MBObj->LA_samples_path = (Object*) Label2( LGS( MSG_SAMPLES_PATH ));

    MBObj->STR_samples_path = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
		MUIA_ShortHelp, LGS( MSG_SAMPLES_PATH_HELP ),
        MUIA_String_MaxLen, 400,
    End;

    MBObj->PA_samples_path = (Object*)PopButton(MUII_PopDrawer);

    MBObj->PA_samples_path = (Object*)PopaslObject,
        MUIA_Popasl_Type, ASL_FileRequest,
		ASLFR_TitleText, LGS( MSG_CHOOSE_DIR ),
        ASLFR_InitialDrawer,"progdir:",
        MUIA_Popstring_String, MBObj->STR_samples_path,
        MUIA_Popstring_Button, MBObj->PA_samples_path,
    End;

    //external sound player for CTCP SOUND
	MBObj->LA_sound_replayer = (Object*)Label2( LGS( MSG_CHECKBOX_EXTERNAL_PLAYER_FOR_CTCP_SOUND ));
    MBObj->CH_sound_replayer = (Object*)CheckMark(FALSE);
    MBObj->STR_sound_replayer = (Object*)StringObject,MUIA_Frame, MUIV_Frame_String,MUIA_ShortHelp, " ",End;
	MBObj->PA_sound_replayer_label = (Object*)Label2( LGS( MSG_EXTERNAL_PLAYER_FOR_CTCP_SOUND ));
    MBObj->PA_sound_replayer = (Object*)PopButton(MUII_PopFile);
    MBObj->PA_sound_replayer = (Object*)PopaslObject,
        MUIA_Popasl_Type, ASL_FileRequest,
        ASLFR_InitialDrawer,"progdir:",
        MUIA_Popstring_String, MBObj->STR_sound_replayer,
        MUIA_Popstring_Button, MBObj->PA_sound_replayer,
    End;

    APTR LA_space3 =(Object*)TextObject,
        MUIA_Weight, 1,
        MUIA_Text_PreParse, NULL,
        MUIA_Text_Contents, NULL, //"             ",
        MUIA_InnerLeft, 0,
        MUIA_InnerRight, 0,
    End;

    APTR LA_space4 =(Object*)TextObject,
        MUIA_Weight, 1,
        MUIA_Text_PreParse, NULL,
        MUIA_Text_Contents, NULL, //"             ",
        MUIA_InnerLeft, 0,
        MUIA_InnerRight, 0,
    End;

    APTR GR_sound_replayer=(Object*)GroupObject,
        MUIA_Group_Horiz, TRUE,
        MUIA_Group_Columns, 4,
        Child, LA_space4,
        Child, MBObj->LA_sound_replayer,
        Child, MBObj->CH_sound_replayer,
        Child, LA_space3,
    End;

    APTR GR_choose_sound_replayer=(Object*)GroupObject,
        MUIA_Group_Horiz, FALSE,
        MUIA_Group_Columns, 2,
        Child, MBObj->PA_sound_replayer_label,
        Child, MBObj->PA_sound_replayer,
    End;
    APTR GR_ctcp_sound=(Object*)GroupObject,
        MUIA_Group_Horiz, FALSE,
        MUIA_Group_Columns, 1,
        Child, GR_sound_replayer,
        Child, GR_choose_sound_replayer,
    End;

    APTR GR_samples1=(Object*)GroupObject,
        MUIA_Group_Horiz, TRUE,
        MUIA_Frame,MUIV_Frame_Group,
        MUIA_Group_Columns, 2,
        Child, MBObj->LA_sound_sample_tabopen,
        Child, MBObj->PA_sound_sample_tabopen,
        Child, MBObj->PA_sound_sample_tabopen_label,
        Child, MBObj->RA_sound_sample_tabopen,
    End;

    APTR GR_samples2=(Object*)GroupObject,
        MUIA_Group_Horiz, TRUE,
        MUIA_Frame,MUIV_Frame_Group,
        MUIA_Group_Columns, 2,
        Child, MBObj->LA_sound_sample_highlight,
        Child, MBObj->PA_sound_sample_highlight,
        Child, MBObj->PA_sound_sample_highlight_label,
        Child, MBObj->RA_sound_sample_highlight,
    End;

    APTR GR_samples3=(Object*)GroupObject,
        MUIA_Group_Horiz, TRUE,
        MUIA_Frame,MUIV_Frame_Group,
        MUIA_Group_Columns, 2,
        Child, MBObj->LA_sound_sample_newmsg,
        Child, MBObj->PA_sound_sample_newmsg,
        Child, MBObj->PA_sound_sample_newmsg_label,
        Child, MBObj->RA_sound_sample_newmsg,
    End;

    APTR GR_samples_path=(Object*)GroupObject,
        MUIA_Group_Horiz, TRUE,
        MUIA_Group_Columns, 2,
        Child, MBObj->LA_samples_path,
        Child, MBObj->PA_samples_path,
    End;
//HGroup, GroupFrame, Child, HSpace(0), Child, YourOptionGroup, Child, HSpace(0), End,

    APTR GR_samples_prefs=(Object*)GroupObject,
        MUIA_Group_Horiz, TRUE,
        MUIA_Group_Columns, 1,
        Child, GR_samples1,
        Child, GR_samples2,
        Child, GR_samples3,
        Child, GR_samples_path,
    End;

    GROUP_ROOT_2_ACTIONS = (Object*)GroupObject,
        MUIA_Group_Columns, 1,
        Child, GR_samples_prefs,
        Child, GR_ctcp_sound,
        //Child, GR_sound_replayer,
        //Child, GR_choose_sound_replayer,
    End;

    // log reloading gui objects

    MBObj->TX_how_many_lines_to_reload = (Object*)TextObject,
		MUIA_Text_Contents, LGS( MSG_LOGFILE_RELOADING_LABEL ),
    End;
    APTR GR_how_many_lines_to_reload = (Object*)GroupObject,
        MUIA_Group_Horiz, TRUE,
        //Child, HSpace(0),
        Child, MBObj->TX_how_many_lines_to_reload,
        Child, HSpace(0),
    End;

	APTR obj_how_many_lines_to_reload_channel = (Object*)Label2( LGS( MSG_CHANNEL ));

    MBObj->STR_how_many_lines_to_reload_channel = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
		MUIA_ShortHelp, LGS( MSG_CHANNEL_LOG_RELOAD_HELP ),
        MUIA_String_Accept, "0123456789",
        MUIA_String_MaxLen, 4,
    End;
	APTR obj_how_many_lines_to_reload_private = (Object*)Label2( LGS( MSG_PRIVATE));

    MBObj->STR_how_many_lines_to_reload_private = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
		MUIA_ShortHelp, LGS( MSG_PRIVATE_LOG_RELOAD_HELP ),
        MUIA_String_Accept, "0123456789",
        MUIA_String_MaxLen, 4,
    End;

    APTR GR_grp_reload_log2 = (Object*)GroupObject,
        //MUIA_Group_Columns, 4,
        MUIA_Group_Horiz, TRUE,
        //Child, HSpace(0),
        Child, obj_how_many_lines_to_reload_channel,
        Child, MBObj->STR_how_many_lines_to_reload_channel,
        Child, obj_how_many_lines_to_reload_private,
        Child, MBObj->STR_how_many_lines_to_reload_private,
        //Child, HSpace(0),
    End;


    MBObj->CH_split_logs = (Object*)CheckMark(FALSE);
	MBObj->LA_split_logs = (Object*)Label( LGS( MSG_SPLIT_UP_LOGS_PREFS_LABEL ) ),

	MBObj->LA_logsize = (Object*)Label( LGS( MSG_LOGSIZE_PREFS_LABEL ));
    MBObj->STR_logsize = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_HelpNode, "STR_servername",
    End;

    GR_logging_splitup = (Object*)GroupObject,
		MUIA_ShortHelp, LGS( MSG_SPLIT_UP_LOGS_PREFS_HELP ),
        //MUIA_Group_Columns, 2,
        MUIA_Group_Horiz, TRUE,
        Child, MBObj->CH_split_logs,
        Child, MBObj->LA_split_logs,
        Child, HSpace(0),
    End;

    GR_logging_splitup2 = (Object*)GroupObject,
		MUIA_ShortHelp, LGS( MSG_SPLIT_UP_LOGS_PREFS_HELP ),
        MUIA_Group_Columns, 2,
        Child, MBObj->LA_logsize,
        Child, MBObj->STR_logsize,
    End;
    APTR GR_reload = (Object*)GroupObject,
        MUIA_Group_Horiz, FALSE,
        MUIA_Frame, MUIV_Frame_Group,
        Child, GR_how_many_lines_to_reload,
        Child, GR_grp_reload_log2,
    End;


    GROUP_ROOT_2_SECOND = (Object*)GroupObject,
        Child, obj_aux2, //maximum lines to keep in buffer.. feature currently removed due to crashing
        Child, GR_logging_splitup,
        Child, GR_logging_splitup2,
        Child, GR_grp_23,
        Child, obj_aux6,
        Child, GR_reload,
        //Child, GR_how_many_lines_to_reload,
        //Child, GR_grp_reload_log2,
    End;

    MBObj->LV_alias = (Object*) NListObject,
		MUIA_ShortHelp, LGS( MSG_ALIAS_HELP ),
        MUIA_Frame, MUIV_Frame_InputList,
        MUIA_NList_ConstructHook, MUIV_NList_ConstructHook_String,
        MUIA_NList_DestructHook, MUIV_NList_DestructHook_String,
        MUIA_NList_DisplayHook, &Display_alias_TextHook,
        MUIA_NList_AutoCopyToClip, TRUE,
        MUIA_NList_Input,TRUE,
        MUIA_NList_Title, TRUE,
        MUIA_NList_Format,"COL=0 WEIGHT=-1 BAR, COL=1 BAR",
        MUIA_NList_TypeSelect,MUIV_NList_TypeSelect_Line,
    End;


    MBObj->LV_alias = (Object*) NListviewObject,
        MUIA_NListview_Horiz_ScrollBar, MUIV_NListview_HSB_Always,
        MUIA_NListview_NList, MBObj->LV_alias,
    End;

    GR_alias1 = (Object*)GroupObject,
        Child, MBObj->LV_alias,
    End;

	MBObj->BT_add_alias    = (Object*) SimpleButton( LGS( MSG_BUTTON_ADD_ALIAS ) );
	MBObj->BT_remove_alias = (Object*) SimpleButton( LGS( MSG_BUTTON_REMOVE_ALIAS ) );

    GR_alias2 = (Object*)GroupObject,
        MUIA_Group_Columns, 2,
        Child, MBObj->BT_add_alias,
        Child, MBObj->BT_remove_alias,
    End;

    GROUP_ROOT_2_ALIASES = (Object*)GroupObject,
        MUIA_Group_Horiz, FALSE,
        Child, GR_alias2,
        Child, GR_alias1,
        Child, (Object*)GroupObject,
        MUIA_Group_Horiz, TRUE,
        MUIA_Group_Columns, 4,
			Child, Label( LGS( MSG_COMMAND_ALIAS_WINDOW_COLUMN1_TITLE )),
            Child, MBObj->STR_edit_alias_name=(Object*)StringObject,MUIA_Weight, 30, MUIA_Frame, MUIV_Frame_String,MUIA_String_Reject," ",MUIA_String_MaxLen, 50,End,
			Child, Label( LGS( MSG_COMMAND_ALIAS_WINDOW_COLUMN2_TITLE )),
            Child, MBObj->STR_edit_alias_command=(Object*)StringObject,MUIA_Weight, 70, MUIA_Frame, MUIV_Frame_String,MUIA_String_MaxLen, 650,End,
        End,
    End;

    MBObj->LV_user_list_buttons = (Object*)NListObject,
        MUIA_NList_AutoCopyToClip, TRUE,
        MUIA_NList_Input,TRUE,
        MUIA_NList_ConstructHook, MUIV_NList_ConstructHook_String,
        MUIA_NList_DestructHook, MUIV_NList_DestructHook_String,
        MUIA_NList_DisplayHook, &DisplayHook_LV_userlist_buttons,
        MUIA_NList_Title, TRUE,
        MUIA_NList_Format,"COL 0 BAR, COL 1",
        MUIA_NList_TypeSelect,MUIV_NList_TypeSelect_Line,
        MUIA_NList_MultiSelect,MUIV_NList_MultiSelect_Shifted,
    End;

    MBObj->GR_click_user_list_buttons = (Object*)GroupObject,
        MUIA_Group_Columns, 2,
        MUIA_Group_HorizSpacing, 1,
        MUIA_Group_VertSpacing, 1,
    End,

    MBObj->GR_user_list_buttons = (Object*)GroupObject,
        MUIA_Group_Columns, 1,
            Child, GroupObject,
                MUIA_Group_Columns,4,
                MUIA_Group_Horiz,TRUE,
                MUIA_Weight, 100,
				Child, MBObj->BT_user_list_buttons_add      = SimpleButton( LGS( MSG_USERLIST_ADD )),
				Child, MBObj->BT_user_list_buttons_delete   = SimpleButton( LGS( MSG_USERLIST_REMOVE )),
				Child, MBObj->BT_user_list_buttons_move_up  = SimpleButton( LGS( MSG_USERLIST_MOVEUP )),
				Child, MBObj->BT_user_list_buttons_move_down= SimpleButton( LGS( MSG_USERLIST_MOVEDOWN )),
            End,
            Child, GroupObject,
                MUIA_Group_Columns,2,
                MUIA_Group_Horiz,TRUE,
				Child, MBObj->LA_user_list_buttons_help = Label( LGS( MSG_USERLIST_HELP )),
                Child, HSpace(0),
            End,
            Child, MBObj->LV_user_list_buttons2 = (Object*)NListviewObject,
                MUIA_HelpNode, "LV_channel",
                MUIA_Weight, 100,
                MUIA_NListview_NList, MBObj->LV_user_list_buttons,
                MUIA_NListview_Horiz_ScrollBar, MUIV_NListview_HSB_Always,
             End,
            Child, GroupObject,
                MUIA_Group_Columns,6,
                MUIA_Group_Horiz,TRUE,
                MUIA_Weight, 100,
                Child, Label("Name"),
                Child, MBObj->STR_user_list_buttons_name=(Object*)BetterStringObject,
                    MUIA_Weight, 30,
                    MUIA_String_MaxLen, 30,
                    MUIA_Frame, MUIV_Frame_String,
					//MUIA_ShortHelp, LGS(17,"This is the hostname of the user you are currently talking to"),
                End,
                Child, Label("Command"),
                Child, MBObj->STR_user_list_buttons_command=(Object*)BetterStringObject,
                    MUIA_Weight, 70,
                    MUIA_String_MaxLen, 800,
                    MUIA_Frame, MUIV_Frame_String,
					//MUIA_ShortHelp, LGS(17,"This is the hostname of the user you are currently talking to"),
                End,
				Child, MBObj->LA_user_list_buttons_disable=Label( LGS( MSG_USERLIST_HIDEALL ) ),
                Child, MBObj->CH_user_list_buttons_disable=CheckMark(FALSE),
            End,
        End;



    LV_events = (Object*) NListObject,
			MUIA_ShortHelp, LGS( MSG_ALIAS_HELP ),
            MUIA_Frame, MUIV_Frame_InputList,
            MUIA_NList_ConstructHook, MUIV_NList_ConstructHook_String,
            MUIA_NList_DestructHook, MUIV_NList_DestructHook_String,
            MUIA_NList_DisplayHook, &Display_events_TextHook,
            MUIA_NList_AutoCopyToClip, TRUE,
            MUIA_NList_Input,TRUE,
            MUIA_NList_Title, TRUE,
            MUIA_NList_Format,"COL=0 BAR, COL=1 WEIGHT=50 BAR, COL=2 BAR, COL=3",
            MUIA_NList_TypeSelect,MUIV_NList_TypeSelect_Line,
    End;

    LV_events = (Object*) NListviewObject,
        MUIA_NListview_Horiz_ScrollBar, MUIV_NListview_HSB_Always,
        MUIA_NListview_NList, LV_events,
    End;


    static const char *CYCLE_events_entries[] =
    {
       "Never", "Window is Active", "Window is Inactive", "Always", NULL
    };


    APTR GROUP_ROOT_2_EVENTS = (Object*)GroupObject,
        MUIA_Group_Horiz, FALSE,
        /*Child, (Object*)GroupObject,
            MUIA_Group_Columns, 2,
			Child, (Object*)SimpleButton(LGS(800,"Add")),
			Child, (Object*)SimpleButton(LGS(800,"Remove")),
        End, */
        Child, GroupObject,
            MUIA_Group_Columns,2,
            MUIA_Group_Horiz,TRUE,
			Child, (Object*)Label( LGS( MSG_EVENTPLAYHOLDER_HELP )),
            Child, HSpace(0),
        End,
        Child, LV_events,
        Child, (Object*)GroupObject,
        MUIA_Group_Horiz, TRUE,
        MUIA_Group_Columns, 3,
			Child, Label( LGS( MSG_EVENTMODE ) ),
            Child, CYCLE_events=(Object*)CycleObject,
             MUIA_Cycle_Entries, CYCLE_events_entries,
				MUIA_ShortHelp, LGS( MSG_EVENTMODE_HELP ),
            End,
            Child, HSpace(0),
        End,
        Child, (Object*)GroupObject,
        MUIA_Group_Horiz, TRUE,
        MUIA_Group_Columns, 4,
			Child, Label( LGS( MSG_AREXXSCRIPT )),
            Child, STR_events_script= (Object*)PopaslObject,
                MUIA_HelpNode, "PA_dcc_send_file",
                MUIA_Popasl_Type, ASL_FileRequest,
				ASLFR_TitleText,"ARexx Event File..",//LGS(29,"Choose filename to send"),
                ASLFR_InitialDrawer,wookie_folder,
                MUIA_Popstring_String, (Object*)BetterStringObject,
                    MUIA_Weight, 30,
                    MUIA_String_MaxLen, 800,
                    MUIA_Frame, MUIV_Frame_String,
                End,
                MUIA_Popstring_Button, (Object*)PopButton(MUII_PopFile), //MBObj->PA_dcc_send_file,
            End,

			Child, Label( "Text for script"),
            Child, STR_events_args= (Object*)StringObject,MUIA_Weight, 70, MUIA_Frame, MUIV_Frame_String,MUIA_String_MaxLen, 800,End,
        End,
    End;

    MBObj->SETTINGS_Register_Grp = (Object*)RegisterGroup(titles2),
        Child, GROUP_ROOT_2,
        Child, GROUP_ROOT_2_GUI,
        Child, GROUP_ROOT_2_SERVER,
        Child, GROUP_ROOT_2_SECOND,
        Child, GROUP_ROOT_2_DCC,
        Child, GROUP_ROOT_2_ACTIONS,
        Child, GROUP_ROOT_2_ALIASES,
        Child, MBObj->GR_user_list_buttons,
        Child, GROUP_ROOT_2_EVENTS,
    End;

    MBObj->SETTINGS_ROOT = (Object*)GroupObject,
        Child, MBObj->SETTINGS_Register_Grp,
        Child, GR_grp_29,
    End;

    MBObj->WI_mainsettings = (Object*)WindowObject,
		MUIA_Window_Title, LGS( MSG_SETTINGS_WINDOW_TITLE ),
        MUIA_Window_ID, MAKE_ID('4', 'W', 'I', 'N'),
        MUIA_Window_NoMenus, FALSE,
        MUIA_Window_CloseGadget, TRUE,
        MUIA_Window_SizeGadget, TRUE,
        WindowContents, MBObj->SETTINGS_ROOT,
    End;



    //below is the colour settings window objects
#if 1
	#define MSG_WINDOW_TITLE_CHOOSE_A_COLOUR MSG_MUICLASS_SETTINGSCOLOR_CHOOSECOLOR_TITLE
	#define MSG_COLOUR_SETTINGS_LOAD MSG_MUICLASS_SETTINGSCOLOR_LOAD_GAD

	#define MSG_COLOUR_SETTINGS_SAVE MSG_MUICLASS_SETTINGSCOLOR_SAVE_GAD
	#define MSG_COLOUR_SETTINGS_SAVE_AS MSG_MUICLASS_SETTINGSCOLOR_SAVEAS_GAD
	#define MSG_COLOUR_SETTINGS_WINDOW_TITLE MSG_MUICLASS_SETTINGSCOLOR_TITLE

	LA_CSW_background = (Object*)Label( LGS( MSG_COLOUR_SETTINGS_LABEL_BACKGROUND ));
	MBObj->PP_CSW_background = (Object*)MUI_NewObject((char*)MUIC_Popimage,MUIA_Window_Title, LGS( MSG_WINDOW_TITLE_CHOOSE_A_COLOUR ),End;

	LA_CSW_nicklistbackground = (Object*)Label( LGS( MSG_COLOUR_SETTINGS_LABEL_NICKLIST_BACKGROUND ));
	MBObj->PP_CSW_nicklistbackground = (Object*)MUI_NewObject((char*)MUIC_Popimage,MUIA_Window_Title, LGS( MSG_WINDOW_TITLE_CHOOSE_A_COLOUR ),End;

	LA_CSW_listviewtabs_background= (Object*)Label( LGS( MSG_COLOUR_SETTINGS_LABEL_TABS_BACKGROUND ));
	MBObj->PP_CSW_listviewtabs_background = (Object*)MUI_NewObject((char*)MUIC_Popimage,MUIA_Window_Title, LGS( MSG_WINDOW_TITLE_CHOOSE_A_COLOUR ),End;

	LA_CSW_join = (Object*)Label( LGS( MSG_COLOUR_SETTINGS_LABEL_JOIN ));
	MBObj->PP_CSW_join = (Object*)MUI_NewObject((char*)MUIC_Poppen,MUIA_Window_Title, LGS( MSG_WINDOW_TITLE_CHOOSE_A_COLOUR ),End;

	LA_CSW_part = (Object*)Label( LGS( MSG_COLOUR_SETTINGS_LABEL_PART ));
	MBObj->PP_CSW_part = (Object*)MUI_NewObject((char*)MUIC_Poppen,MUIA_Window_Title, LGS( MSG_WINDOW_TITLE_CHOOSE_A_COLOUR ),End;

	LA_CSW_quit = (Object*)Label( LGS( MSG_COLOUR_SETTINGS_LABEL_QUIT ));
	MBObj->PP_CSW_quit = (Object*)MUI_NewObject((char*)MUIC_Poppen,MUIA_Window_Title, LGS( MSG_WINDOW_TITLE_CHOOSE_A_COLOUR ),End;

	LA_CSW_modes = (Object*)Label( LGS( MSG_COLOUR_SETTINGS_LABEL_MODES ));
	MBObj->PP_CSW_modes = (Object*)MUI_NewObject((char*)MUIC_Poppen,MUIA_Window_Title, LGS( MSG_WINDOW_TITLE_CHOOSE_A_COLOUR ),End;

	LA_CSW_ctcp = (Object*)Label( LGS( MSG_COLOUR_SETTINGS_LABEL_CTCP ));
	MBObj->PP_CSW_ctcp = (Object*)MUI_NewObject((char*)MUIC_Poppen,MUIA_Window_Title, LGS( MSG_WINDOW_TITLE_CHOOSE_A_COLOUR ),End;

	LA_CSW_actions = (Object*)Label( LGS( MSG_COLOUR_SETTINGS_LABEL_ACTIONS  ));
	MBObj->PP_CSW_actions = (Object*)MUI_NewObject((char*)MUIC_Poppen,MUIA_Window_Title, LGS( MSG_WINDOW_TITLE_CHOOSE_A_COLOUR ),End;

	LA_CSW_normal = (Object*)Label( LGS( MSG_COLOUR_SETTINGS_LABEL_NORMAL ));
	MBObj->PP_CSW_normal = (Object*)MUI_NewObject((char*)MUIC_Poppen,MUIA_Window_Title, LGS( MSG_WINDOW_TITLE_CHOOSE_A_COLOUR ),End;

	LA_CSW_info = (Object*)Label( LGS( MSG_COLOUR_SETTINGS_LABEL_INFO ));
	MBObj->PP_CSW_info = (Object*)MUI_NewObject((char*)MUIC_Poppen,MUIA_Window_Title, LGS( MSG_WINDOW_TITLE_CHOOSE_A_COLOUR ),End;

	LA_CSW_owntext = (Object*)Label( LGS( MSG_COLOUR_SETTINGS_LABEL_OWN_TEXT ));
	MBObj->PP_CSW_owntext = (Object*)MUI_NewObject((char*)MUIC_Poppen,MUIA_Window_Title, LGS( MSG_WINDOW_TITLE_CHOOSE_A_COLOUR ),End;

	LA_CSW_highlight = (Object*)Label( LGS( MSG_COLOUR_SETTINGS_LABEL_HIGHLIGHT ));
	MBObj->PP_CSW_highlight = (Object*)MUI_NewObject((char*)MUIC_Poppen,MUIA_Window_Title, LGS( MSG_WINDOW_TITLE_CHOOSE_A_COLOUR ),End;

	LA_CSW_invite = (Object*)Label( LGS( MSG_COLOUR_SETTINGS_LABEL_INVITE ));
	MBObj->PP_CSW_invite = (Object*)MUI_NewObject((char*)MUIC_Poppen,MUIA_Window_Title, LGS( MSG_WINDOW_TITLE_CHOOSE_A_COLOUR ),End;

	LA_CSW_kick = (Object*)Label( LGS( MSG_COLOUR_SETTINGS_LABEL_KICK ));
	MBObj->PP_CSW_kick = (Object*)MUI_NewObject((char*)MUIC_Poppen,MUIA_Window_Title, LGS( MSG_WINDOW_TITLE_CHOOSE_A_COLOUR ),End;

	LA_CSW_nickchange = (Object*)Label( LGS( MSG_COLOUR_SETTINGS_LABEL_NICK_CHANGE ));
	MBObj->PP_CSW_nickchange = (Object*)MUI_NewObject((char*)MUIC_Poppen,MUIA_Window_Title, LGS( MSG_WINDOW_TITLE_CHOOSE_A_COLOUR ),End;

	LA_CSW_topic = (Object*)Label( LGS( MSG_COLOUR_SETTINGS_LABEL_TOPIC  ));
	MBObj->PP_CSW_topic = (Object*)MUI_NewObject((char*)MUIC_Poppen,MUIA_Window_Title, LGS( MSG_WINDOW_TITLE_CHOOSE_A_COLOUR ),End;

	LA_CSW_wallops = (Object*)Label( LGS( MSG_COLOUR_SETTINGS_LABEL_WALLOPS ));
	MBObj->PP_CSW_wallops = (Object*)MUI_NewObject((char*)MUIC_Poppen,MUIA_Window_Title, LGS( MSG_WINDOW_TITLE_CHOOSE_A_COLOUR ),End;

	LA_CSW_notices = (Object*)Label( LGS( MSG_COLOUR_SETTINGS_LABEL_NOTICES ));
	MBObj->PP_CSW_notices = (Object*)MUI_NewObject((char*)MUIC_Poppen,MUIA_Window_Title, LGS( MSG_WINDOW_TITLE_CHOOSE_A_COLOUR ),End;

	LA_CSW_activity = (Object*)Label(LGS(MSG_COLOUR_SETTINGS_LABEL_ACTIVITY));
	MBObj->PP_CSW_activity = (Object*)MUI_NewObject((char*)MUIC_Poppen,MUIA_Window_Title, LGS( MSG_WINDOW_TITLE_CHOOSE_A_COLOUR ),End;

	LA_CSW_chatactivity = (Object*)Label( LGS(MSG_COLOUR_SETTINGS_LABEL_CHAT_ACTIVITY));
	MBObj->PP_CSW_chatactivity = (Object*)MUI_NewObject((char*)MUIC_Poppen,MUIA_Window_Title, LGS( MSG_WINDOW_TITLE_CHOOSE_A_COLOUR ),End;

	LA_CSW_highlightactivity = (Object*)Label( LGS(MSG_COLOUR_SETTINGS_LABEL_HIGHLIGHT_ACTIVITY));
	MBObj->PP_CSW_highlightactivity = (Object*)MUI_NewObject((char*)MUIC_Poppen,MUIA_Window_Title, LGS( MSG_WINDOW_TITLE_CHOOSE_A_COLOUR ),End;

	LA_CSW_nicklisttext = (Object*)Label( LGS(MSG_COLOUR_SETTINGS_LABEL_NICKLIST_TEXT));
	MBObj->PP_CSW_nicklisttext = (Object*)MUI_NewObject((char*)MUIC_Poppen,MUIA_Window_Title, LGS( MSG_WINDOW_TITLE_CHOOSE_A_COLOUR ),End;

	LA_CSW_listviewtabs_normaltext = (Object*)Label( LGS(MSG_COLOUR_SETTINGS_LABEL_TAGS_PEN));
	MBObj->PP_CSW_listviewtabs_normaltext = (Object*)MUI_NewObject((char*)MUIC_Poppen,MUIA_Window_Title, LGS( MSG_WINDOW_TITLE_CHOOSE_A_COLOUR ),End;

	MBObj->BT_colour_settings_load = (Object*)SimpleButton( LGS( MSG_COLOUR_SETTINGS_LOAD ));
	MBObj->BT_colour_settings_apply = (Object*)SimpleButton( "Apply");
	MBObj->BT_colour_settings_save = (Object*)SimpleButton( LGS( MSG_COLOUR_SETTINGS_SAVE ));
	MBObj->BT_colour_settings_save_as = (Object*)SimpleButton( LGS( MSG_COLOUR_SETTINGS_SAVE_AS ));


    APTR colour_settings_actions = (Object*)GroupObject,//VirtgroupObject,
        MUIA_Group_Columns, 4,
        Child, MBObj->BT_colour_settings_load,
        Child, MBObj->BT_colour_settings_apply,
        Child, MBObj->BT_colour_settings_save,
        Child, MBObj->BT_colour_settings_save_as,
    End;

    APTR GROUP_ROOT_11 = (Object*)GroupObject,//VirtgroupObject,
        MUIA_Group_Columns, 6,
        Child,    LA_CSW_background,
        Child,    MBObj->PP_CSW_background,
        Child,    LA_CSW_nicklistbackground,
        Child,    MBObj->PP_CSW_nicklistbackground,
        Child,    LA_CSW_listviewtabs_background,
        Child,    MBObj->PP_CSW_listviewtabs_background,
        Child,    LA_CSW_normal,
        Child,    MBObj->PP_CSW_normal,
        Child,    LA_CSW_nicklisttext,
        Child,    MBObj->PP_CSW_nicklisttext,
        Child,    LA_CSW_listviewtabs_normaltext,
        Child,    MBObj->PP_CSW_listviewtabs_normaltext,
        Child,    LA_CSW_join,
        Child,    MBObj->PP_CSW_join,
        Child,    LA_CSW_part,
        Child,    MBObj->PP_CSW_part,
        Child,    LA_CSW_quit,
        Child,    MBObj->PP_CSW_quit,
        Child,    LA_CSW_modes,
        Child,    MBObj->PP_CSW_modes,
        Child,    LA_CSW_ctcp,
        Child,    MBObj->PP_CSW_ctcp,
        Child,    LA_CSW_actions,
        Child,    MBObj->PP_CSW_actions,
        Child,    LA_CSW_info,
        Child,    MBObj->PP_CSW_info,
        Child,    LA_CSW_owntext,
        Child,    MBObj->PP_CSW_owntext,
        Child,    LA_CSW_highlight,
        Child,    MBObj->PP_CSW_highlight,
        Child,    LA_CSW_notices,
        Child,    MBObj->PP_CSW_notices,
        Child,    LA_CSW_invite,
        Child,    MBObj->PP_CSW_invite,
        Child,    LA_CSW_kick,
        Child,    MBObj->PP_CSW_kick,
        Child,    LA_CSW_nickchange,
        Child,    MBObj->PP_CSW_nickchange,
        Child,    LA_CSW_topic,
        Child,    MBObj->PP_CSW_topic,
        Child,    LA_CSW_wallops,
        Child,    MBObj->PP_CSW_wallops,
        Child,    LA_CSW_activity,
        Child,    MBObj->PP_CSW_activity,
        Child,    LA_CSW_chatactivity,
        Child,    MBObj->PP_CSW_chatactivity,
        Child,    LA_CSW_highlightactivity,
        Child,    MBObj->PP_CSW_highlightactivity,
    End;

    GROUP_ROOT_6 = (Object*)GroupObject,//VirtgroupObject,
        MUIA_Group_Columns, 1,
        Child, GROUP_ROOT_11,
        Child, colour_settings_actions,
    End;
    MBObj->WI_colour_settings = (Object*)WindowObject,
		MUIA_Window_Title, LGS( MSG_COLOUR_SETTINGS_WINDOW_TITLE ),
        MUIA_Window_ID, MAKE_ID('3', 'W', 'I', 'N'),
        WindowContents, GROUP_ROOT_6,
    End;
#endif
    //below are menu items

	MBObj->MN_ = MenustripObject,
		Child, MN1_Main = MenuObject, MUIA_Menu_Title, LGS( MSG_MENU_PROJECT_TITLE ),
			Child, MN1_SelectServer       = MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MENU_SELECTSERVER_ITEM ), MUIA_Menuitem_Shortcut, LGS( MSG_MENU_SELECTSERVER_KEY ), End,
			Child, MN1_NewTAB             = MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MENU_NEWSERVERTAB_ITEM ), MUIA_Menuitem_Shortcut, LGS( MSG_MENU_NEWSERVERTAB_KEY ), End,
			Child, MN1_NewGTAB            = MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MENU_NEWGLOBALQUERYTAB_ITEM ), MUIA_Menuitem_Shortcut, LGS( MSG_MENU_NEWGLOBALQUERYTAB_KEY ), End,
			Child, MN1_CloseTAB           = MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MENU_CLOSESERVERTAB_ITEM ), MUIA_Menuitem_Shortcut, LGS( MSG_MENU_CLOSESERVERTAB_KEY ), End,
			Child, MNmenuBarLabel0        = MUI_MakeObject(MUIO_Menuitem, NM_BARLABEL, 0, 0, 0),
			Child, MN1_Hide               = MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MENU_HIDE_ITEM ), MUIA_Menuitem_Shortcut, LGS( MSG_MENU_HIDE_KEY ), End,
			Child, MNmenuBarLabel1        = MUI_MakeObject(MUIO_Menuitem, NM_BARLABEL, 0, 0, 0),
			Child, MN_ClearHistory        = MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MENU_HISTORYCLEAR_ITEM ), End,
			Child, MN_ClearAllHistory     = MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MENU_HISTORYCLEARALL_ITEM ), End,
			Child, MN_SaveHistory         = MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MENU_HISTORYSAVE_ITEM ), End,
			Child, MNmenuBarLabel2        = MUI_MakeObject(MUIO_Menuitem, NM_BARLABEL, 0, 0, 0),
			Child, MN_about               = MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MENU_ABOUT_ITEM ), MUIA_Menuitem_Shortcut, LGS( MSG_MENU_ABOUT_KEY ), End,
			Child, MN_quit                = MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MENU_QUIT_ITEM ), MUIA_Menuitem_Shortcut, LGS( MSG_MENU_QUIT_KEY ), End,
		End,
		Child, MN_Edit = MenuObject, MUIA_Menu_Title, LGS( MSG_MENU_EDIT_TITLE ),
			Child, MN_cut                 = MenuitemObject, MUIA_Menuitem_Title , LGS( MSG_MENU_CUT_ITEM )  , MUIA_Menuitem_Shortcut, LGS( MSG_MENU_CUT_KEY )  , End,
			Child, MN_copy                = MenuitemObject, MUIA_Menuitem_Title , LGS( MSG_MENU_COPY_ITEM ) , MUIA_Menuitem_Shortcut, LGS( MSG_MENU_COPY_KEY ) , End,
			Child, MN_paste               = MenuitemObject, MUIA_Menuitem_Title , LGS( MSG_MENU_PASTE_ITEM ), MUIA_Menuitem_Shortcut, LGS( MSG_MENU_PASTE_KEY ), End,
		End,
		Child, MN_Settings = MenuObject, MUIA_Menu_Title, LGS( MSG_MENU_SETTINGS_TITLE ),
			Child, MN_MainSettings        = MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MENU_SETTINGSMAIN_ITEM ), End,
			Child, MN_ColourSettings      = MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MENU_SETTINGSCOLOR_ITEM ), End,
			Child, MN_MUISettings         = MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MENU_SETTINGSMUI_ITEM ), End,
			Child, MN_MultiColumnDisplay  = MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MENU_MULTICOLUMNCHATDISPLAY_ITEM ), MUIA_Menuitem_Checkit, TRUE, MUIA_Menuitem_Toggle, TRUE, End,
			Child, MN_Clipboard           = MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MENU_COLUMNMARKINGCLIPBOARD_ITEM ), MUIA_Menuitem_Checkit, TRUE, MUIA_Menuitem_Toggle, TRUE, End,
			Child, MN_MuteSound           = MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MENU_MUTEALLSOUNDS_ITEM ), MUIA_Menuitem_Checkit, TRUE, MUIA_Menuitem_Toggle, TRUE, End,
			Child, MN_SaveSettings        = MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MENU_SETTINGSSAVE_ITEM ), End,
		End,
		Child, MN_windows = MenuObject, MUIA_Menu_Title, LGS( MSG_MENU_WINDOWS_TITLE ),
			Child, MN_windows_dcc         = MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MENU_DCCINCOMING_ITEM ), MUIA_Menuitem_Shortcut, LGS( MSG_MENU_DCCINCOMING_KEY ), End,
			Child, MN_windows_dcc2        = MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MENU_DCCOUTGOING_ITEM ), MUIA_Menuitem_Shortcut, LGS( MSG_MENU_DCCOUTGOING_KEY ), End,
			Child, MN_urlgrabber          = MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MENU_URLGRABBER_ITEM  ), MUIA_Menuitem_Shortcut, LGS( MSG_MENU_URLGRABBER_KEY ), End,
			Child, MN_ignorelist          = MenuitemObject, MUIA_Menuitem_Title, LGS( MSG_MENU_IGNORELIST_ITEM  ), MUIA_Menuitem_Shortcut, LGS( MSG_MENU_IGNORELIST_KEY ), End,
		End,
	End;
    //below are edit window stuff



	LA_servername = (Object*)Label( LGS( MSG_SERVER_NAME ));

    MBObj->STR_servername = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_HelpNode, "STR_servername",
        MUIA_String_Reject," ",
    End;

    GR_grp_13 = (Object*)GroupObject,
        MUIA_HelpNode, "GR_grp_13",
        MUIA_Group_Horiz, TRUE,
        Child, LA_servername,
        Child, MBObj->STR_servername,
    End;

	LA_port = (Object*)Label( LGS( MSG_PORT_NUMBER ));
    MBObj->STR_port = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_String_Reject," ",
        MUIA_HelpNode, "STR_port",
    End;

    GR_grp_11 = (Object*)GroupObject,
        MUIA_HelpNode, "GR_grp_11",
        MUIA_Group_Horiz, TRUE,
        Child, LA_port,
        Child, MBObj->STR_port,
    End;

	MBObj->LA_autojoin = (Object*)Label( LGS( MSG_AUTO_JOIN_CHANNELS ));

    MBObj->STR_autojoin = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_String_Reject," ",
        MUIA_String_MaxLen, 400,
    End;

	MBObj->LA_nick_registration_command = (Object*)Label( LGS( MSG_NICK_REGISTER_PASSWORD ));

    MBObj->STR_nick_registration_command = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
		MUIA_ShortHelp, LGS( MSG_NICK_REGISTER_PASSWORD_SHORT_HELP ),
        MUIA_String_Secret, TRUE,
        MUIA_String_Reject," ",
    End;

    APTR GR_grp_autojoin = (Object*)GroupObject,
        MUIA_HelpNode, "GR_grp_17",
        MUIA_Group_Horiz, TRUE,
        MUIA_Group_Columns, 2,
        Child,MBObj->LA_autojoin,
        Child,MBObj->STR_autojoin,
    End;

    APTR GR_grp_nickregistration = (Object*)GroupObject,
        MUIA_HelpNode, "GR_grp_17",
        MUIA_Group_Horiz, TRUE,
        MUIA_Group_Columns, 2,
        Child,MBObj->LA_nick_registration_command,
        Child,MBObj->STR_nick_registration_command,
    End;

	LA_pass = (Object*)Label( LGS( MSG_SERVER_PASSWORD ));

    MBObj->STR_pass = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_HelpNode, "STR_pass",
        MUIA_String_Secret, TRUE,
        MUIA_String_Reject," ",
    End;

    GR_grp_17 = (Object*)GroupObject,
        MUIA_HelpNode, "GR_grp_17",
        MUIA_Group_Horiz, TRUE,
        Child, LA_pass,
        Child, MBObj->STR_pass,
    End;

	MBObj->BT_acceptserver = (Object*)SimpleButton( LGS( MSG_ACCEPT_CHANGES ));

	MBObj->BT_cancelserver = (Object*)SimpleButton( LGS( MSG_CANCEL ));

    GR_grp_12 = (Object*)GroupObject,
        MUIA_HelpNode, "GR_grp_12",
        MUIA_Group_Horiz, TRUE,
        Child, MBObj->BT_acceptserver,
        Child, MBObj->BT_cancelserver,
    End;

    MBObj->CH_autoconnect = (Object*)CheckMark(FALSE);
	MBObj->LA_autoconnect = (Object*)Label( LGS( MSG_AUTO_CONNECT_OPTION ));

    APTR LA_space7 =(Object*)TextObject,
        MUIA_Weight, 1,
        MUIA_Text_PreParse, NULL,
        MUIA_Text_Contents, NULL,
        MUIA_InnerLeft, 0,
        MUIA_InnerRight, 0,
    End;

    GR_autoconnect = (Object*)GroupObject,
        MUIA_Group_Horiz, TRUE,
        Child, MBObj->CH_autoconnect,
        Child, MBObj->LA_autoconnect,
        Child, LA_space7,
        Child, HSpace(0),
        Child, MBObj->CH_use_global = CheckMark(FALSE),
        Child, Label("Use Global user settings"),
        Child, HSpace(0),
    End;

	MBObj->LA_remote_charset=(Object*)Label( LGS( MSG_SERVER_CHARSET ));

    MBObj->CYCLE_remote_charset = CycleObject,
       MUIA_Cycle_Entries, remote_charsets,
    End;

    APTR LA_space6 =(Object*)TextObject,
            MUIA_Weight, 100,
            MUIA_Text_PreParse, NULL,
            MUIA_Text_Contents, NULL,
            MUIA_InnerLeft, 0,
            MUIA_InnerRight, 0,
        End;

    MBObj->GR_charsets = (Object*)GroupObject,
        MUIA_Group_Horiz, TRUE,
        Child,MBObj->LA_remote_charset,
        Child,MBObj->CYCLE_remote_charset,
        Child, LA_space6,
    End;
    APTR GROUP_ROOT_4_COLUMN_1 = (Object*)GroupObject,
        MUIA_Weight, 45,
        Child, GR_grp_13,
        Child, GR_grp_11,
        Child, GR_grp_17,
    End;

    APTR GROUP_ROOT_4_COLUMN_2 = (Object*)GroupObject,
        MUIA_Weight, 55,
        Child, GR_grp_autojoin,
        Child, GR_grp_nickregistration,
        Child, MBObj->GR_charsets,
        //Child, GR_grp_12,
    End;
    GROUP_ROOT_4 = (Object*)GroupObject,
        MUIA_Group_Horiz,FALSE,
        MUIA_Group_Columns,2,
        Child, GROUP_ROOT_4_COLUMN_1,
        Child, GROUP_ROOT_4_COLUMN_2,
    End;

	LA_groupname = (Object*)Label( LGS( MSG_GROUP_NAME ));

	MBObj->BT_perform = (Object*)SimpleButton( LGS( MSG_PERFORM_ON_CONNECT ));

    MBObj->STR_groupname = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_HelpNode, "STR_groupname",
    End;

    GR_grp_15 =(Object*)GroupObject,
        MUIA_HelpNode, "GR_grp_15",
        MUIA_Group_Horiz, TRUE,
        Child, LA_groupname,
        Child, MBObj->STR_groupname,
    End;

	MBObj->BT_acceptgroup =(Object*)SimpleButton( LGS( MSG_ACCEPT ));

	MBObj->BT_cancelgroup =(Object*)SimpleButton( LGS( MSG_CANCEL ));

    GR_grp_16 =(Object*)GroupObject,
        MUIA_HelpNode, "GR_grp_16",
        MUIA_Group_Horiz, TRUE,
        Child, MBObj->BT_acceptgroup,
        Child, MBObj->BT_cancelgroup,
    End;

    GROUP_ROOT_5 =(Object*)GroupObject,
        Child, GR_grp_15,
        Child, GR_grp_16,
    End;
    MBObj->WI_edit_group =(Object*)WindowObject,
		MUIA_Window_Title, LGS( MSG_EDIT_GROUP ),
        MUIA_Window_ID, MAKE_ID('3', 'W', 'I', 'N'),
        MUIA_Window_CloseGadget, FALSE,
        WindowContents, GROUP_ROOT_5,
    End;

// above is edit window stuff

	LA_nicknames =(Object*)Label( LGS( MSG_NICKNAMES ));

    MBObj->STR_nickname1 =(Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_HelpNode, "STR_nickname1",
        MUIA_String_Reject,"\\/#.,?*@!",
    End;

    MBObj->STR_nickname2 =(Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_HelpNode, "STR_nickname2",
        MUIA_String_Reject,"\\/#.,?*@!",
    End;

    MBObj->STR_nickname3 =(Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_HelpNode, "STR_nickname3",
        MUIA_String_Reject,"\\/#.,?*@!",
    End;
    GR_grp_0 =(Object*)GroupObject,
        MUIA_HelpNode, "GR_grp_0",
        MUIA_Group_Horiz, TRUE,
        Child, LA_nicknames,
        Child, MBObj->STR_nickname1,
        Child, MBObj->STR_nickname2,
        Child, MBObj->STR_nickname3,
    End;

	LA_realname =(Object*)Label( LGS( MSG_REAL_NAME ));

    MBObj->STR_realname =(Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_HelpNode, "STR_realname",
    End;




	MBObj->LA_display_server_win=(Object*)Label( LGS( MSG_DISPLAY_SERVER_SELECTION_WINDOW ));
    MBObj->CH_display_server_win=(Object*)CheckMark(FALSE);

    DISPLAY_SERVER_WIN_GRP=(Object*)GroupObject,
        MUIA_Group_Horiz, TRUE,
        //MUIA_Weight,400,
        Child, MBObj->LA_display_server_win,
        Child, MBObj->CH_display_server_win,
    End;

    APTR LA_ =(Object*)TextObject,
            MUIA_Weight, 1,
            MUIA_Text_PreParse, NULL,
            MUIA_Text_Contents, NULL,
            MUIA_InnerLeft, 0,
            MUIA_InnerRight, 0,
        End;
    GR_grp_1 =(Object*)GroupObject,
        MUIA_HelpNode, "GR_grp_1",
        MUIA_Group_Horiz, TRUE,
        Child, DISPLAY_SERVER_WIN_GRP,
        Child, LA_,
    End;

        //Child, MBObj->LA_local_charset,
        //Child, MBObj->CYCLE_local_charset,

    APTR LA_space5 =(Object*)TextObject,
            MUIA_Weight, 100,
            MUIA_Text_PreParse, NULL,
            MUIA_Text_Contents, NULL,
            MUIA_InnerLeft, 0,
            MUIA_InnerRight, 0,
        End;

    GR_local_charset =(Object*)GroupObject,
        MUIA_HelpNode, "GR_grp_1",
        MUIA_Group_Horiz, TRUE,
        MUIA_Group_Columns, 3,
        Child, MBObj->LA_local_charset,
        Child, MBObj->CYCLE_local_charset,
        Child, LA_space5,
    End;

	LA_username =(Object*)Label( LGS( MSG_USER_NAME ));

    MBObj->STR_username =(Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_HelpNode, "STR_username",
    End;


    GR_grp_2 =(Object*)GroupObject,
        MUIA_HelpNode, "GR_grp_2",
        MUIA_Group_Horiz, TRUE,
        Child, LA_username,
        Child, MBObj->STR_username,
        Child, LA_realname,
        Child, MBObj->STR_realname,
        //Child, DISPLAY_SERVER_WIN_GRP,
    End;

    REC_label_0 =(Object*) RectangleObject,
        MUIA_Rectangle_HBar, TRUE,
        MUIA_FixHeight, 8,
    End;

    GR_grp_3 =(Object*)GroupObject,
        MUIA_HelpNode, "GR_grp_3",
        MUIA_Group_Horiz, TRUE,
        Child, REC_label_0,
    End;



    MBObj->NLT_Servers =(Object*) NListtreeObject,
        MUIA_NListtree_ConstructHook, MUIV_NListtree_ConstructHook_String,
        MUIA_NListtree_DestructHook, MUIV_NListtree_DestructHook_String,
        MUIA_NListtree_DisplayHook, &Display_servers_TextHook,
        MUIA_NListtree_Format,"COL=0 WEIGHT=45 BAR, COL=1 WEIGHT=10 BAR, COL=2 WEIGHT=15 BAR, COL=3 WEIGHT=15 BAR",
        MUIA_NListtree_Title, TRUE,
    End;

    if(!PRO_CHARSETS_ENABLED)
        setmacro((Object*)MBObj->NLT_Servers,MUIA_NListtree_Format,"COL=0 WEIGHT=45 BAR, COL=1 WEIGHT=10 BAR, COL=2 WEIGHT=15 BAR");

    MBObj->NLV_Servers=(Object*)NListviewObject,
        MUIA_NListview_Vert_ScrollBar, MUIV_NListview_VSB_Always,
        MUIA_NListview_NList, MBObj->NLT_Servers,
    End;
	if(!MBObj->NLT_Servers) cleanexit((char*) LGS( MSG_GENERAL ));
    GR_grp_8 =(Object*)GroupObject, //VirtgroupObject,
        MUIA_Group_Horiz, TRUE,
        Child, MBObj->NLV_Servers,
        MUIA_HelpNode, "GR_grp_8",
    End;

    REC_label_0CC =(Object*) RectangleObject,
        MUIA_Rectangle_HBar, TRUE,
        MUIA_FixHeight, 8,
    End;

    GR_grp_7 =(Object*)GroupObject,
        MUIA_HelpNode, "GR_grp_7",
        Child, REC_label_0CC,
    End;

	MBObj->BT_addserver =(Object*)SimpleButton( LGS( MSG_NEW_SERVER ));

	MBObj->BT_addgroup =(Object*)SimpleButton( LGS( MSG_NEW_GROUP ));

	MBObj->BT_delete_server =(Object*)SimpleButton( LGS( MSG_DELETE_ENTRY ));

	MBObj->BT_edit =(Object*)SimpleButton( LGS( MSG_EDIT_ENTRY ));

    GR_grp_9 =(Object*)GroupObject,
        MUIA_HelpNode, "GR_grp_9",
        MUIA_Group_Horiz, TRUE,
        Child, MBObj->BT_addserver,
        Child, MBObj->BT_addgroup,
        Child, MBObj->BT_delete_server,
        //Child, MBObj->BT_edit,
        //Child, MBObj->BT_acceptserver,
    End;

	MBObj->BT_connect =(Object*)SimpleButton( LGS( MSG_CONNECT_TO_SERVER ));

	MBObj->BT_connect_tab =(Object*)SimpleButton( LGS( MSG_CONNECT_TO_SERVER_IN_NEW_TAB ));

    GR_grp_6 =(Object*)GroupObject,
        MUIA_HelpNode, "GR_grp_6",
        MUIA_Group_Horiz, TRUE,
        Child, MBObj->BT_connect,
        Child, MBObj->BT_connect_tab,
        Child, MBObj->BT_perform,
    End;


    GROUP_ROOT_3 =(Object*)GroupObject,
        MUIA_Group_HorizSpacing, 2,
        MUIA_Group_VertSpacing, 2,
        Child, GroupObject,
            MUIA_FrameTitle, "Global user settings",
            MUIA_Frame, MUIV_Frame_Group,
            Child, GR_grp_0,
            Child, GR_grp_2,
            Child, GR_grp_1,
            Child, GR_local_charset,
        End,
        Child, GR_grp_8,
        Child, GR_grp_7,
        Child, GR_grp_9,
        Child, GR_grp_6,
        Child, GroupObject,
            MUIA_FrameTitle, "Server settings",
            MUIA_Frame, MUIV_Frame_Group,
            Child, GR_autoconnect,
            Child, MBObj->GR_server_specific_user_prefs = (Object*)GroupObject,
                Child, GroupObject,
                    MUIA_Group_Horiz, TRUE,
					Child, (Object*)Label( LGS( MSG_NICKNAMES )),
                    Child, MBObj->STR_server_nickname1 =(Object*)StringObject,
                        MUIA_Frame, MUIV_Frame_String,
                        MUIA_HelpNode, "STR_nickname1",
                        MUIA_String_Reject,"\\/#.,?*@!",
                    End,
                    Child, MBObj->STR_server_nickname2 =(Object*)StringObject,
                        MUIA_Frame, MUIV_Frame_String,
                        MUIA_HelpNode, "STR_nickname2",
                        MUIA_String_Reject,"\\/#.,?*@!",
                    End,

                    Child, MBObj->STR_server_nickname3 =(Object*)StringObject,
                        MUIA_Frame, MUIV_Frame_String,
                        MUIA_HelpNode, "STR_nickname3",
                        MUIA_String_Reject,"\\/#.,?*@!",
                    End,
                End,
                Child, GroupObject,
                    MUIA_Group_Horiz, TRUE,
					Child, (Object*)Label( LGS( MSG_REAL_NAME )),
                    Child, MBObj->STR_server_username =(Object*)StringObject,
                        MUIA_Frame, MUIV_Frame_String,
                        MUIA_String_Reject," ",
                        MUIA_HelpNode, "STR_username",
                    End,
					Child, (Object*)Label( LGS( MSG_USER_NAME )),
                    Child, MBObj->STR_server_realname =(Object*)StringObject,
                        MUIA_Frame, MUIV_Frame_String,
                        MUIA_String_Reject," ",
                        MUIA_HelpNode, "STR_realname",
                    End,
                End,
            End,
            Child, (Object*) RectangleObject, MUIA_Rectangle_HBar, TRUE, MUIA_FixHeight, 8, End,
            Child, GROUP_ROOT_4,
        End,
    End;

    MBObj->WI_label_2 =(Object*)WindowObject,
		MUIA_Window_Title, LGS( MSG_WINDOW_SELECT_SERVER ),
        MUIA_Window_ID, MAKE_ID('1', 'W', 'I', 'N'),
        WindowContents, GROUP_ROOT_3,
    End;
    GR_top =(Object*)GroupObject,
        MUIA_Group_PageMode,TRUE,
        MUIA_Group_Horiz, TRUE,
        MUIA_HelpNode, "GR_top",
        MUIA_Group_HorizSpacing, 1,
        MUIA_Group_VertSpacing, 1,
        MUIA_Weight,100,
        //Child, MBObj->LV_tabs,
    End;

    if(string_default)
    {
        MBObj->STR_usertext =(Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_HelpNode, "STR_usertext",
        MUIA_String_MaxLen, BUFFERSIZE,
        End;
    }
    else
    {

        MBObj->STR_usertext =(Object*)BS_Object,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_BetterString_StayActive, TRUE,
        MUIA_HelpNode, "STR_usertext",
        MUIA_String_MaxLen, BUFFERSIZE,
        End;

    }

    GR_string_gadget =(Object*)GroupObject,
        MUIA_HelpNode, "GR_string_gadget",
        MUIA_Group_Horiz, TRUE,
        MUIA_Group_HorizSpacing,1,
        //Child, MBObj->TX_nickname = TextObject,MUIA_Text_SetMin, TRUE,MUIA_Text_SetMax, TRUE,/*MUIA_Frame,MUIV_Frame_Text,ButtonFrame,MUIA_Background, MUII_ButtonBack,*/MUIA_Text_Contents,"                     ",MUIA_Weight,0,End,
        Child, MBObj->TX_nickname = TextObject,MUIA_Text_SetMin, TRUE,MUIA_Text_SetMax, TRUE,ButtonFrame,MUIA_Background, MUII_ButtonBack,MUIA_Text_Contents,"                     ",MUIA_Weight,0,
        MUIA_InputMode, MUIV_InputMode_RelVerify,
        End,
        Child, MBObj->STR_usertext,
    End;

    GR_buttons =(Object*) VirtgroupObject,//GroupObject,
        MUIA_HelpNode, "GR_buttons",
        MUIA_Group_Horiz, TRUE,
        MUIA_Group_HorizSpacing, 1,
        MUIA_Group_VertSpacing, 1,
        MUIA_FixHeight, 200,
        //MUIA_Group_Rows,2,
    End;

    LA_space =(Object*)TextObject,
        MUIA_Weight, 1,
        MUIA_Text_PreParse, NULL,
        MUIA_Text_Contents, NULL,
        MUIA_InnerLeft, 0,
        MUIA_InnerRight, 0,
    End;

    GR_bottom_group =(Object*)GroupObject,
        MUIA_HelpNode, "GR_bottom_group",
        MUIA_Group_Horiz, TRUE,
        MUIA_Group_VertSpacing, 1,
        MUIA_Group_HorizSpacing, 1,
        MUIA_Weight,100,
        Child, GR_buttons,
        Child, LA_space,
    End;



    if(!my_settings.use_nlistobject_for_tabs)
    {
        #ifdef __AROS__
        GROUP_ROOT_0 =(Object*)NewObjectAROS(get_custom_class(CC_MUIC_GROUP)->mcc_Class,NULL,
        #else
        GROUP_ROOT_0 =(Object*)NewObject(get_custom_class(CC_MUIC_GROUP)->mcc_Class,NULL,
            #endif
            MUIA_Group_HorizSpacing, 1,
            MUIA_Group_VertSpacing, 1,
            //MUIA_Group_VertSpacing, 2,
            Child, GR_top,
            Child, GR_string_gadget,
            Child, GR_bottom_group,
    End;
    }
    else
    {
        #ifdef __AROS__
        GROUP_ROOT_0 =(Object*)NewObjectAROS(get_custom_class(CC_MUIC_GROUP)->mcc_Class,NULL,
        #else
        GROUP_ROOT_0 =(Object*)NewObject(get_custom_class(CC_MUIC_GROUP)->mcc_Class,NULL,
            #endif
            MUIA_Group_HorizSpacing, 1,
            MUIA_Group_VertSpacing, 1,
            //MUIA_Group_VertSpacing, 2,
            Child, GR_top,
            Child, GR_string_gadget,
        End;
    }

    #ifdef __AROS__
    MBObj->WI_main =(Object*)NewObjectAROS(get_custom_class(CC_MUIC_WINDOW)->mcc_Class,NULL, // (Object*)WindowObject,
    #else
    MBObj->WI_main =(Object*)NewObject(get_custom_class(CC_MUIC_WINDOW)->mcc_Class,NULL, // (Object*)WindowObject,
        #endif
        MUIA_Window_Title, "Wookiechat",
        MUIA_Window_Menustrip, MBObj->MN_,
        MUIA_Window_ID, MAKE_ID('0', 'W', 'I', 'N'),
        WindowContents, GROUP_ROOT_0,
    End;

      if(!MBObj->WI_main) printf("Error creating window object\n");

    #ifdef __AROS__
    MBObj->LV_ban = (Object*)NewObjectAROS(nlist->mcc_Class,NULL,// NListObject,
    #else
    MBObj->LV_ban = (Object*)NewObject(nlist->mcc_Class,NULL,// NListObject,
    #endif
        MUIA_Frame, MUIV_Frame_InputList,
        MUIA_NList_ConstructHook, MUIV_NList_ConstructHook_String,
        MUIA_NList_DestructHook, MUIV_NList_DestructHook_String,
        MUIA_NList_DisplayHook, &DisplayLI_ban_TextHook,
        MUIA_NList_Format, "COL=0 WEIGHT=-1 BAR, COL=1 WEIGHT =-1 BAR, COL=2 BAR",
        MUIA_NList_AutoCopyToClip, TRUE,
        MUIA_NList_TypeSelect,MUIV_NList_TypeSelect_Line,
        MUIA_NList_Input, TRUE,
        MUIA_NList_Title, TRUE,
    End;
    MBObj->LV_ban = (Object*)NListviewObject,
        MUIA_HelpNode, "LV_channel",
        MUIA_Weight, 100,
        MUIA_NListview_NList, MBObj->LV_ban,
    End;

	MBObj->BT_unban = (Object*)SimpleButton( LGS( MSG_UNBAN ));
	MBObj->BT_refreshban = (Object*)SimpleButton( LGS( MSG_REFRESH_BAN_LIST ));

    GR_ban_subgroup = (Object*)GroupObject,
        MUIA_Group_Horiz, TRUE,
        Child, MBObj->BT_unban,
        Child, MBObj->BT_refreshban,
    End;

    GR_ban = (Object*)GroupObject,
        Child, MBObj->LV_ban,
        Child, GR_ban_subgroup,
    End;
    MBObj->WI_ban = (Object*)WindowObject,
        MUIA_Window_Title, "Banlist",
        MUIA_Window_CloseGadget, TRUE,
        MUIA_Window_ID, MAKE_ID('9', '1', 'I', 'N'),
        MUIA_Background, MUII_SHINE,
        WindowContents, GR_ban,
    End;

#ifndef MUIA_Application_UsedClasses
#define MUIA_Application_UsedClasses 0x8042e9a7
#endif

    static STRPTR classlist[] = { (STRPTR)"NList.mcc", (STRPTR)"NListview.mcc", (STRPTR)"NListtree.mcc", (STRPTR)"BetterString.mcc", NULL };
    MBObj->App =(Object*) ApplicationObject,
        MUIA_Application_UsedClasses, classlist,
        MUIA_Application_Author, (char*)"James Carroll",
        MUIA_Application_Base, (char*)"WOOKIECHAT",
        MUIA_Application_Title, (char*)"WookieChat",
        MUIA_Application_Version, (char*)VERSION_MUI,
        MUIA_Application_Copyright, (char*)"James Carroll",
        MUIA_Application_Description, (char*)"IRC Client",
        MUIA_Application_DiskObject, dobj,
        SubWindow, MBObj->WI_main,
        SubWindow, MBObj->WI_label_2,
        SubWindow, MBObj->WI_edit_group,
		SubWindow, MBObj->WI_colour_settings,// = WindowColorSettingsObject, End,
        SubWindow, MBObj->WI_mainsettings,
        SubWindow, MBObj->WI_dcc,  //incoming file transfers
        SubWindow, MBObj->WI_dcc2, //outgoing file transfers
        SubWindow, MBObj->WI_dcc_file_exists,
        SubWindow, MBObj->WI_dcc_send,
		SubWindow, MBObj->WI_about = WindowAboutObject, End,
		SubWindow, MBObj->WI_quit = WindowQuitObject, End,
		SubWindow, MBObj->WI_urlgrabber = WindowURLGrabberObject, End,
        SubWindow, MBObj->WI_ban,
		SubWindow, MBObj->WI_ignore = WindowIgnoreListObject, End,
        //SubWindow, MBObj->WI_edit_alias,
        SubWindow, MBObj->WI_graphical_smileys_preview,
        SubWindow, MBObj->WI_graphical_smileys_choose,
        SubWindow, MBObj->WI_change_nick,
    End;

	/* notifies in here */
	DoMethod((Object*) MN_about     , MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime, (Object*) MBObj->WI_about     , 3, MUIM_Set, MUIA_Window_Open, TRUE );
	DoMethod((Object*) MN_urlgrabber, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime, (Object*) MBObj->WI_urlgrabber, 3, MUIM_Set, MUIA_Window_Open, TRUE );
	DoMethod((Object*) MN_ignorelist, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime, (Object*) MBObj->WI_ignore, 3, MUIM_Set, MUIA_Window_Open, TRUE );
	DoMethod((Object*) MN_ColourSettings, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime, (Object*) MBObj->WI_colour_settings, 3, MUIM_Set, MUIA_Window_Open, TRUE );
	DoMethod((Object*) MN_quit          , MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime, (Object*) MBObj->WI_quit, 3, MUIM_Set, MUIA_Window_Open, TRUE );

    MBObj->smiley_choose_icon=NULL;
    if (!MBObj->App)
    {
        FreeVec(MBObj);
		printf("%s", LGS( MSG_UNABLE_TO_CREATE_APP ));
        return(NULL);
    }

    return(MBObj);
}

BOOL create_custom_classes()
{
    for (int i = 0; i < CC_MAX; i++)
        classes[i] = NULL;

// Create MUI Subclasses
#ifdef __AROS__
    classes[CC_MUIC_BETTERSTRING] = MUI_CreateCustomClass(NULL, (ClassID)MUIC_BetterString, NULL, sizeof(struct InstanceData),
            (APTR) BetterString_Dispatcher);
    if (!classes[CC_MUIC_BETTERSTRING])
		printf("%s\n", LGS( MSG_ERROR_MISSING_MCC_BETTERSTRING ));

    classes[CC_MUIC_NLIST] = MUI_CreateCustomClass(NULL, (ClassID)MUIC_NList, NULL, sizeof(struct MyData), (APTR) NList_Dispatcher);
    if (!classes[CC_MUIC_NLIST])
		printf("%s\n", LGS( MSG_ERROR_MISSING_MCC_NLIST ));

    classes[CC_MUIC_WINDOW] = MUI_CreateCustomClass(NULL, (ClassID)MUIC_Window, NULL, sizeof(struct MyData), (APTR) Window_Dispatcher);
    if (!classes[CC_MUIC_WINDOW])
        printf("Could not create custom class for windows\n");

    classes[CC_MUIC_GROUP] = MUI_CreateCustomClass(NULL, (ClassID)MUIC_Group, NULL, sizeof(struct MyData), (APTR) Group_Dispatcher);
    if (!classes[CC_MUIC_GROUP])
        printf("Could not create custom class for groups\n");

#else
DISPATCHERPROTO(BetterString_Dispatcher);
DISPATCHERPROTO(NList_Dispatcher);
DISPATCHERPROTO(Window_Dispatcher);
DISPATCHERPROTO(Group_Dispatcher);
	classes[CC_MUIC_BETTERSTRING] = MUI_CreateCustomClass(NULL, (ClassID)MUIC_BetterString,NULL,sizeof(struct InstanceData),ENTRY(BetterString_Dispatcher));
	if(!classes[CC_MUIC_BETTERSTRING]) printf("%s\n", LGS( MSG_ERROR_MISSING_MCC_BETTERSTRING ));

    classes[CC_MUIC_NLIST] = MUI_CreateCustomClass(NULL, (ClassID)MUIC_NList,NULL,sizeof(struct MyData),ENTRY(NList_Dispatcher));
	if(!classes[CC_MUIC_NLIST]) printf("%s\n", LGS( MSG_ERROR_MISSING_MCC_NLIST ));

    classes[CC_MUIC_WINDOW] = MUI_CreateCustomClass(NULL, (ClassID)MUIC_Window,NULL,sizeof(struct MyData),ENTRY(Window_Dispatcher));
    if(!classes[CC_MUIC_WINDOW]) printf("Could not create custom class for windows\n");

    classes[CC_MUIC_GROUP] = MUI_CreateCustomClass(NULL, (ClassID)MUIC_Group,NULL,sizeof(struct MyData),ENTRY(Group_Dispatcher));
    if(!classes[CC_MUIC_GROUP]) printf("Could not create custom class for groups\n");
#endif

    for (int i = 0; i < CC_MAX; i++)
        if (!classes[i]) return FALSE;

    return TRUE;
}

void delete_custom_classes()
{
    for (int i = 0; i < CC_MAX; i++)
        if (classes[i])
            MUI_DeleteCustomClass(classes[i]);
}

struct MUI_CustomClass * get_custom_class(ULONG id)
{
    if (id >= CC_MAX)
        return NULL;
    else
        return classes[id];
}

struct MUI_CustomClass * get_nlist_class()
{
    return get_custom_class(CC_MUIC_NLIST);
}
