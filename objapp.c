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
#include <mui/NListtree_mcc.h>
#include <mui/BetterString_mcc.h>
#include <libraries/gadtools.h>
#include <libraries/asl.h>

#include "intern.h"
#include "objapp.h"
#include "version_info.h"

APTR MN1_Hide, MN1_SelectServer, MN1_NewTAB, MN1_NewGTAB,MN1_CloseTAB, MN_ClearAllHistory,MN_ClearHistory,  MN_SaveHistory, MN_MainSettings;
APTR MN_SaveSettings, MN_MUISettings, MN_ColourSettings, MN_windows_dcc2,  MN_windows_dcc, MN_urlgrabber, MN_ignorelist, MN_about, MN_quit, MN_MultiColumnDisplay, MN_Clipboard;
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
static int AMIX=0;

static APTR MNmenuBarLabel2,MN1_Main, MNmenuBarLabel0,MNmenuBarLabel1, MN_Settings, MN_windows, MN_Edit;

static APTR GROUP_ROOT_4, GR_grp_13, LA_servername, GR_grp_11, LA_port, LA_pass, GR_grp_12;
static APTR GROUP_ROOT_5, GR_grp_15, LA_groupname, GR_grp_16, GR_grp_17;
static APTR GR_ban, GR_ban_subgroup, GR_logging_splitup, GR_logging_splitup2;
static APTR LA_space;
static APTR GROUP_ROOT_3, GR_grp_0, LA_nicknames, GR_grp_1, LA_realname, GR_grp_2;
static APTR LA_username, GR_grp_3, REC_label_0, GR_grp_8, GR_grp_7, REC_label_0CC;
static APTR GR_grp_9, GR_grp_6;

// BELOW are colour setting window variables
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

static APTR DISPLAY_SERVER_WIN_GRP;
static APTR IGNORE_ROOT, IGNORE_GRP1, GR_autoconnect;
static APTR GROUP_ROOT_2_ALIASES, GR_alias1, GR_alias2;
static APTR URLGRABBER_ROOT, URLGRABBER_GRP1, ABOUT_ROOT, GROUP_ROOT_2_SERVER, GROUP_ROOT_2, GROUP_ROOT_2_SECOND;
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
static APTR QUIT_ROOT, quit_label;
static APTR ADDIGNORE_ROOT, ADDIGNORE_GRP1;
static APTR about_group1, about_group2, about_group3, about_group4;
static APTR PICTURE_LOGO;


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

if(AMIX) printf("2\n");
    static CONST_STRPTR titles2[] =
    {
        (CONST_STRPTR)"General",
        (CONST_STRPTR)"GUI",
        (CONST_STRPTR)"Servers",
        (CONST_STRPTR)"Logging",
        (CONST_STRPTR)"DCC",
        (CONST_STRPTR)"Sounds",
        (CONST_STRPTR)"Aliases",
        (CONST_STRPTR)"Nick List Buttons",
        (CONST_STRPTR)"Events",
        NULL
    };

    titles2[0] = GCS(catalog,19,"General");
    titles2[2] = GCS(catalog,260,"Servers");
    titles2[3] = GCS(catalog,20,"Logging");
    titles2[5] = GCS(catalog,21,"Sounds");
    titles2[6] = GCS(catalog,22,"Aliases");
    titles2[7] = GCS(catalog,374,"Nick List Buttons");
    titles2[8] = GCS(catalog,800,"Events");

if(AMIX) printf("3\n");
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

if(AMIX) printf("4\n");

    char about_text[2000];
    strcpy(about_text,VERSION_ABOUT);

    MBObj->TX_about = (Object*)TextObject,
        MUIA_Text_Contents,about_text,
        MUIA_Weight, 100,
    End;


    MBObj->TX_about2 = (Object*)TextObject,
        MUIA_Text_Contents,"\033cNew sponsor coming soon..",
        MUIA_Weight, 100,
    End;

    MBObj->TX_about_old = (Object*)TextObject,
        MUIA_Text_Contents,"\n\033cThanks go out to Guru Meditation for past sponsorship!\n\033cAmiga reseller and Linux consulting\n\033chttp://gurumeditation.se/\n",
        MUIA_Weight, 100,
    End;

    MBObj->TX_about3 = (Object*)TextObject,
        MUIA_Text_Contents,GCS(catalog,333,"AREXX port: "),
        MUIA_Weight, 100,
    End;
if(AMIX) printf("5\n");
#if defined(__amigaos4__) || defined(__MORPHOS__) || defined(__AROS__)

    about_group1 = GroupObject,
        MUIA_FixHeight, 78,
        //Child, PICTURE_LOGO = (Object*)MUI_NewObject((char*)MUIC_DTPIC, MUIA_Dtpic_Name, "wookiechat_logo.gif", End,
        Child, PICTURE_LOGO = (Object*)MUI_NewObject((char*)MUIC_DTPIC, MUIA_Dtpic_Name, "progdir:wookiechat_logo.gif", End,
        MUIA_Weight,100,

    End;

    about_group2 = GroupObject,
        Child, MBObj->TX_about,
        MUIA_Weight,100,                                                                                                                               \
    End;

    APTR about_group5 = GroupObject,
        Child, MBObj->TX_about_old,
        MUIA_Weight,100,
    End;


    about_group3 = GroupObject,
        MUIA_FixHeight, 114,
        Child, MUI_NewObject((char*)MUIC_DTPIC, MUIA_Dtpic_Name, "progdir:avatar2.jpg", End,
        //Child, MUI_NewObject((char*)MUIC_DTPIC, MUIA_Dtpic_Name, "avatar2.jpg", End,
        MUIA_Weight,100,
    End;


    about_group4 = GroupObject,
        Child, MBObj->TX_about2,
        MUIA_Weight,100,
    End;

    ABOUT_ROOT = GroupObject,
        MUIA_Background, "2:ffffffff,ffffffff,ffffffff",
        Child, about_group1,
        Child, about_group2,//amix sponsorship text
        Child, about_group3,//amix new logo
        Child, about_group4,
        Child, about_group5, //thanks go to Guru Meditation
        Child, MBObj->TX_about3,
    End;
if(AMIX) printf("6\n");
#else

    about_group2 = (Object*)GroupObject,
        Child, MBObj->TX_about,
        MUIA_Weight,100,

    End;

    about_group4 = (Object*)GroupObject,
        Child, MBObj->TX_about2,
        MUIA_Weight,100,

    End;
if(AMIX) printf("7\n");
    if(my_settings.os3_about_window_gfx)
    {

        about_group1 = GroupObject,
            MUIA_FixHeight, 78,
            Child, MUI_NewObject((char*)MUIC_DTPIC, MUIA_Dtpic_Name, "progdir:wookiechat_logo.gif", End,
            MUIA_Weight,100,

        End;

        about_group3 = GroupObject,
            MUIA_FixHeight, 114,
            Child, MUI_NewObject((char*)MUIC_DTPIC, MUIA_Dtpic_Name, "progdir:avatar2.jpg", End,
            MUIA_Weight,100,
        End;

        ABOUT_ROOT = (Object*)GroupObject,
            MUIA_Background, "2:ffffffff,ffffffff,ffffffff",
            Child, about_group1,
            Child, about_group2,
            Child, about_group3,
            Child, about_group4,
            Child, MBObj->TX_about3,
        End;


    }
    else
    {

        ABOUT_ROOT = (Object*)GroupObject,
            MUIA_Background, "2:ffffffff,ffffffff,ffffffff",
            Child, about_group2,
            Child, about_group4,
            Child, MBObj->TX_about3,
        End;

    }

#endif

if(AMIX) printf("8\n");
    //load graphical smilies

    MBObj->LA_local_charset=(Object*)Label(GCS(catalog,328,"Local Charset:"));

    STRPTR *array;
if(AMIX) printf("9\n");
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
            total_charsets=i-1;

            local_charsets[i+1]=NULL;
            remote_charsets[i+1]=NULL;

        //CodesetsFreeA(array, NULL);
    }
if(AMIX) printf("10\n");
    MBObj->CYCLE_local_charset = CycleObject,
       MUIA_Weight,0,
       MUIA_Cycle_Entries, local_charsets,
    End;

    MBObj->LV_urlgrabber = (Object*) NListObject,
        MUIA_Frame, MUIV_Frame_InputList,
        MUIA_NList_ConstructHook, MUIV_NList_ConstructHook_String,
        MUIA_NList_DestructHook, MUIV_NList_DestructHook_String,
        MUIA_NList_AutoCopyToClip, TRUE,
        MUIA_NList_Input, TRUE,
        MUIA_NList_MultiSelect,MUIV_NList_MultiSelect_Shifted,
        MUIA_NList_TypeSelect,MUIV_NList_TypeSelect_Line,
        #ifndef __AROS__
        //#ifndef __MORPHOS__
        MUIA_NList_ActiveObjectOnClick,TRUE,
        //#endif
        #endif
    End;

    MBObj->LV_urlgrabber = (Object*)NListviewObject,
        MUIA_Weight, 100,
        MUIA_NListview_NList, MBObj->LV_urlgrabber,
    End;
if(AMIX) printf("11\n");
    MBObj->BT_urlgrabber_clear=(Object*)SimpleButton(GCS(catalog,23,"Remove URL(s) From List"));
    MBObj->BT_urlgrabber_clearall=(Object*)SimpleButton(GCS(catalog,24,"Clear All"));
if(AMIX) printf("12\n");
    URLGRABBER_GRP1 =(Object*)GroupObject,
        MUIA_Group_Horiz, TRUE,
        Child, MBObj->BT_urlgrabber_clear,
        Child, MBObj->BT_urlgrabber_clearall,
    End;

    URLGRABBER_ROOT = (Object*)GroupObject,
        MUIA_Group_Horiz, FALSE,
        Child, MBObj->LV_urlgrabber,
        Child, URLGRABBER_GRP1,
    End;
if(AMIX) printf("13\n");
    MBObj->WI_urlgrabber = (Object*)WindowObject,
        MUIA_Window_Title, GCS(catalog,25,"URL Grabber - Double Click to Open"),
        MUIA_Window_CloseGadget, TRUE,
        MUIA_Window_ID, MAKE_ID('7', '1', '1', 'N'),
        MUIA_Background, MUII_SHINE,
        WindowContents, URLGRABBER_ROOT,
        //MUIA_Window_ActiveObject, MBObj->LV_urlgrabber,
        //MUIA_Window_DefaultObject, WookieChat->LV_urlgrabber,
    End;


    MBObj->STR_addignore = (Object*)StringObject,
        MUIA_Weight, 80,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_String_MaxLen, 600,
        End;

    MBObj->BT_addignore = (Object*)TextObject,
        ButtonFrame,
        MUIA_Text_Contents,"blah", //GCS(catalog,812,"blah"),
        MUIA_Text_PreParse, "\033c",
        MUIA_InputMode, MUIV_InputMode_RelVerify,
        //MUIA_InputMode, MUIV_InputMode_Toggle,
    End;

    MBObj->LA_addignore_privmsg = (Object*)Label2(GCS(catalog,265,"Text"));
    MBObj->CH_addignore_privmsg = (Object*)CheckMark(FALSE);

    MBObj->LA_addignore_ctcp = (Object*)Label2("CTCP");
    MBObj->CH_addignore_ctcp = (Object*)CheckMark(FALSE);


    MBObj->LA_addignore_dcc = (Object*)Label2("DCC");
    MBObj->CH_addignore_dcc = (Object*)CheckMark(FALSE);

    MBObj->TX_addignore =(Object*)TextObject,
        MUIA_Weight, 100,
        MUIA_Text_PreParse, NULL,
        MUIA_Text_Contents, GCS(catalog,270, "Hostmarks look like this: nick!userid@hostname\ni.e.«Join» jahc (~jahc00@jahc.Amigaworld.Net)\njahc is the nick, jahc00 is the userid, and jahc.Amigaworld.Net is the hostname\nTo ignore this person, you could insert an ignore entry of: jahc*!*@*\nThis will ignore all people with a nick starting with the letters jahc.\nThe use of wildcards makes this ignore command very powerful.\nThings like this are possible: guru*editatio*!*userid@*.se\nor if you want to keep it simple, you could just use: roberto*"),
        MUIA_InnerLeft, 0,
        MUIA_InnerRight, 0,
    End;
if(AMIX) printf("14\n");
    MBObj->LA_ignore_hostmask=(Object*)Label(GCS(catalog,271,"Ignore entry:"));
    MBObj->LA_ignore_actions=(Object*)Label(GCS(catalog,272,"Which events to ignore:"));


    ADDIGNORE_GRP1 = (Object*)GroupObject,
        MUIA_Group_Horiz, TRUE,
        Child, MBObj->LA_ignore_hostmask,
        Child, MBObj->STR_addignore,
        Child, MBObj->LA_addignore_privmsg,
        Child, MBObj->CH_addignore_privmsg,
        Child, MBObj->LA_addignore_ctcp,
        Child, MBObj->CH_addignore_ctcp,
        Child, MBObj->LA_addignore_dcc,
        Child, MBObj->CH_addignore_dcc,
    End;

if(AMIX) printf("15\n");
    ADDIGNORE_ROOT = (Object*)GroupObject,
        MUIA_Group_Horiz, FALSE,
        Child, ADDIGNORE_GRP1,
        Child, MBObj->BT_addignore,
        Child, MBObj->TX_addignore,
    End;

    MBObj->WI_addignore = (Object*)WindowObject,
        MUIA_Window_Title, "jahc was here", //this window serves two purposes, "add ignore", and "edit ignore",
        MUIA_Window_CloseGadget, TRUE,
        MUIA_Window_ID, MAKE_ID('7', '2', 'I', 'N'),
        MUIA_Background, MUII_SHINE,
        WindowContents, ADDIGNORE_ROOT,
    End;
if(AMIX) printf("16\n");
        //MUIA_Background, MUII_BACKGROUND,

    MBObj->BT_ignore_add=(Object*)SimpleButton(GCS(catalog,273,"Add"));
    MBObj->BT_ignore_edit=(Object*)SimpleButton(GCS(catalog,274,"Edit"));
    MBObj->BT_ignore_remove=(Object*)SimpleButton(GCS(catalog,275,"Remove"));

    MBObj->LV_ignore = (Object*) NListObject,
        MUIA_Frame, MUIV_Frame_InputList,
        MUIA_NList_ConstructHook, MUIV_NList_ConstructHook_String,
        MUIA_NList_DestructHook,  MUIV_NList_DestructHook_String,
        MUIA_NList_DisplayHook, &Display_ignore_TextHook,
        MUIA_NList_AutoCopyToClip, TRUE,
        MUIA_NList_Input,TRUE,
        MUIA_NList_Title, TRUE,
        MUIA_NList_Format,"COL=0 WEIGHT=80 BAR, COL=1 WEIGHT=-1 BAR, COL=2 WEIGHT=-1 BAR, COL=3 WEIGHT=-1 BAR",
        MUIA_NList_MultiSelect,MUIV_NList_MultiSelect_Shifted,
        MUIA_NList_TypeSelect,MUIV_NList_TypeSelect_Line,
    End;

    MBObj->LV_ignore = (Object*)NListviewObject,
        MUIA_Weight, 100,
        MUIA_NListview_NList, MBObj->LV_ignore,
    End;
if(AMIX) printf("17\n");
    IGNORE_GRP1 = (Object*)GroupObject,
        MUIA_Group_Horiz, TRUE,
        Child, MBObj->BT_ignore_add,
        Child, MBObj->BT_ignore_edit,
        Child, MBObj->BT_ignore_remove,
    End;

    IGNORE_ROOT = (Object*)GroupObject,
        MUIA_Group_Horiz, FALSE,
        Child, MBObj->LV_ignore,
        Child, IGNORE_GRP1,
    End;
if(AMIX) printf("18\n");
    MBObj->WI_ignore = (Object*)WindowObject,
        MUIA_Window_Title, GCS(catalog,276,"Ignore List"),
        MUIA_Window_CloseGadget, TRUE,
        MUIA_Window_ID, MAKE_ID('7', '1', 'I', 'N'),
        MUIA_Background, MUII_SHINE,
        WindowContents, IGNORE_ROOT,
    End;
if(AMIX) printf("19\n");
    //end of ignore stuff

    MBObj->WI_about = (Object*)WindowObject,
        MUIA_Window_Title, GCS(catalog,26,"About"),
        MUIA_Window_CloseGadget, TRUE,
        MUIA_Window_ID, MAKE_ID('7', 'W', 'I', 'N'),
        MUIA_Background, MUII_SHINE,
        WindowContents, ABOUT_ROOT,
    End;

    // quit yes/no window

    quit_label = (Object*)Label(GCS(catalog,60,"Quit Program?"));

    MBObj->BT_quit_yes = (Object*)SimpleButton(GCS(catalog,27,"Yes"));
    MBObj->BT_quit_no = (Object*)SimpleButton(GCS(catalog,28,"No"));

    QUIT_ROOT = (Object*)GroupObject,
        MUIA_Group_Columns, 3,
        Child, quit_label,
        Child, MBObj->BT_quit_yes,
        Child, MBObj->BT_quit_no,
    End;
if(AMIX) printf("20\n");
    MBObj->WI_quit = (Object*)WindowObject,
        MUIA_Window_Title, GCS(catalog,60,"Quit Program?"), //"Quit",
        MUIA_Window_CloseGadget, FALSE,
        MUIA_Window_ID, MAKE_ID('9', 'W', 'I', 'N'),
        MUIA_Background, MUII_SHINE,
        WindowContents, QUIT_ROOT,
    End;


    MBObj->WI_change_nick = (Object*)WindowObject,
        MUIA_Window_Title, GCS(catalog,800,"Change nick"), //"Quit",
        MUIA_Window_CloseGadget, TRUE,
        MUIA_Window_ID, MAKE_ID('9', 'z', 'z', 'z'),
        //MUIA_Background, MUII_SHINE,
        WindowContents, GroupObject,
            MUIA_Group_Columns, 2,
            Child,MBObj->LA_change_nick=Label(GCS(catalog,800,"Enter new nick")),
            Child,MBObj->STR_change_nick=StringObject,MUIA_Frame, MUIV_Frame_String,MUIA_String_Reject," ./",End,
            Child,MBObj->BT_change_nick_change=SimpleButton(GCS(catalog,800,"Change")),
            Child,MBObj->BT_change_nick_cancel=SimpleButton(GCS(catalog,800,"Cancel")),
        End,
    End;

    //dcc send initiate window

    LA_dcc_send_file = (Object*)Label(GCS(catalog,61,"File to send"));

    MBObj->STR_PA_dcc_send_file = (Object*)String(my_settings.upload_path, 800);

    MBObj->PA_dcc_send_file = (Object*)PopButton(MUII_PopFile);

    MBObj->PA_dcc_send_file = (Object*)PopaslObject,
        MUIA_HelpNode, "PA_dcc_send_file",
        MUIA_Popasl_Type, ASL_FileRequest,
        ASLFR_TitleText,GCS(catalog,29,"Choose filename to send"),
        ASLFR_InitialDrawer,my_settings.upload_path,
        //ASLFR_InitialDrawer,"ram:",
        MUIA_Popstring_String, MBObj->STR_PA_dcc_send_file,
        MUIA_Popstring_Button, MBObj->PA_dcc_send_file,
    End;
if(AMIX) printf("21\n");
    GR_grp_19 = (Object*)GroupObject,
        MUIA_HelpNode, "GR_grp_19",
        MUIA_Group_Columns, 2,
        Child, LA_dcc_send_file,
        Child, MBObj->PA_dcc_send_file,
    End;

    LA_dcc_send_nick = (Object*)Label((char*)GCS(catalog,240,"Reciever"));

    MBObj->STR_dcc_send_nick = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_HelpNode, "STR_dcc_send_nick",
        MUIA_String_Contents,"",
    End;
if(AMIX) printf("22\n");
    GR_grp_30= (Object*)GroupObject,
        MUIA_HelpNode, "GR_grp_1",
        MUIA_Group_Columns, 2,
        Child, LA_dcc_send_nick,
        Child, MBObj->STR_dcc_send_nick,
    End;

    MBObj->BT_dcc_send = (Object*)SimpleButton(GCS(catalog,30,"Send File"));

    GROUP_ROOT_10 = (Object*)GroupObject,
        Child, GR_grp_19,
        Child, GR_grp_30,
        Child, MBObj->BT_dcc_send,
    End;
if(AMIX) printf("34\n");
    MBObj->WI_dcc_send = (Object*)WindowObject,
        MUIA_Window_Title, GCS(catalog,31,"DCC Send File"),
        MUIA_Window_ID, MAKE_ID('9', 'W', 'I', 'N'),
        WindowContents, GROUP_ROOT_10,
    End;

if(AMIX) printf("35\n");
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
if(AMIX) printf("36\n");
    MBObj->WI_graphical_smileys_preview = (Object*)WindowObject,
        MUIA_Window_Title, GCS(catalog,320,"Graphical Smileys Preview"),
        MUIA_Window_CloseGadget, TRUE,
        MUIA_Window_ID, MAKE_ID('9', 'A', 'I', 'N'),
        WindowContents, MBObj->LV_graphical_smileys_preview,
    End;

    MBObj->GR_graphical_smileys_choose = (Object*)GroupObject,
        MUIA_Group_Columns, 10,
    End;
if(AMIX) printf("37\n");
    MBObj->WI_graphical_smileys_choose = (Object*)WindowObject,
        MUIA_Window_Title, " ",
        MUIA_Window_CloseGadget, TRUE,
        MUIA_Window_ID, MAKE_ID('9', 'A', 'B', 'N'),
        WindowContents, MBObj->GR_graphical_smileys_choose,
    End;


if(AMIX) printf("38\n");


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

if(AMIX) printf("39\n");
    MBObj->BT_dcc_send_reoffer = (Object*)SimpleButton(GCS(catalog,32,"Reoffer file"));
    MBObj->BT_dcc_send_cancel = (Object*)SimpleButton(GCS(catalog,33,"Cancel Send"));
    MBObj->BT_dcc_send_remove = (Object*)SimpleButton(GCS(catalog,34,"Remove"));
    MBObj->BT_dcc_send_new = (Object*)SimpleButton(GCS(catalog,30,"Send File"));

    GR_send_dcc = (Object*)GroupObject,
        MUIA_Group_Columns, 4,
        Child, MBObj->BT_dcc_send_new,
        Child, MBObj->BT_dcc_send_cancel,
        Child, MBObj->BT_dcc_send_remove,
    End;
if(AMIX) printf("40\n");
    MBObj->BT_dcc_resume = (Object*)SimpleButton(GCS(catalog,35,"Resume"));
    MBObj->BT_dcc_overwrite = (Object*)SimpleButton(GCS(catalog,36,"Overwrite"));
    MBObj->BT_dcc_abort = (Object*)SimpleButton(GCS(catalog,37,"Abort"));
    MBObj->BT_dcc_rename = (Object*)SimpleButton(GCS(catalog,369,"Rename"));

    GR_dcc_file_exists = (Object*)GroupObject,
        MUIA_Group_Columns, 4,
        Child, MBObj->BT_dcc_resume,
        Child, MBObj->BT_dcc_overwrite,
        Child, MBObj->BT_dcc_rename,
        Child, MBObj->BT_dcc_abort,
    End;
if(AMIX) printf("41\n");
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
if(AMIX) printf("42\n");
    MBObj->WI_dcc_file_exists = (Object*)WindowObject,
        MUIA_Window_Title, GCS(catalog,38,"Incoming File already exists"),
        MUIA_Window_ID, MAKE_ID('6', 'W', 'I', 'N'),
        MUIA_Window_CloseGadget, FALSE,
        MUIA_Window_NoMenus, FALSE,
        WindowContents, GROUP_ROOT_8,
        MUIA_Window_Activate,FALSE,
    End;

    MBObj->LA_upload_path = (Object*)Label2(GCS(catalog, 371,"Upload Path"));

    MBObj->STR_dcc_PA_label_1 = (Object*)String("",200);
if(AMIX) printf("43\n");
    MBObj->PA_dcc_label_1 = (Object*)PopButton(MUII_PopDrawer);

    MBObj->PA_dcc_label_1 = (Object*)PopaslObject,
        ASLFR_DrawersOnly, TRUE,
        //MUIA_ShortHelp, GCS(catalog,39,"Choose a path to save your downloads to"),
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
if(AMIX) printf("44\n");
    MBObj->LA_download_path = (Object*)Label2(GCS(catalog, 305,"Download Path"));

    MBObj->STR_dcc_PA_label_0 = (Object*)String("",200);

    MBObj->PA_dcc_label_0 = (Object*)PopButton(MUII_PopDrawer);

    MBObj->PA_dcc_label_0 = (Object*)PopaslObject,
        ASLFR_DrawersOnly, TRUE,
        MUIA_ShortHelp, GCS(catalog,39,"Choose a path to save your downloads to"),
        MUIA_Popasl_Type, 0,
        MUIA_Popstring_String, MBObj->STR_dcc_PA_label_0,
        MUIA_Popstring_Button, MBObj->PA_dcc_label_0,
    End;
if(AMIX) printf("45\n");
    GR_dcc_grp_28 = (Object*)GroupObject,
        MUIA_HelpNode, "GR_dcc_grp_28",
        MUIA_Group_Horiz, TRUE,
        Child, MBObj->LA_download_path,
        Child, MBObj->PA_dcc_label_0,
    End;

    MBObj->BT_dcc_accept = (Object*)SimpleButton(GCS(catalog,40,"Accept"));
    MBObj->BT_dcc_cancel = (Object*)SimpleButton(GCS(catalog,41,"Cancel"));
    MBObj->BT_dcc_remove = (Object*)SimpleButton(GCS(catalog,34,"Remove"));
if(AMIX) printf("46\n");
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
if(AMIX) printf("47\n");
    MBObj->LV_dcc = (Object*)NListviewObject,
        MUIA_HelpNode, "LV_channel",
        MUIA_Weight, 80,
        MUIA_NListview_NList, MBObj->LV_dcc,
    End;

    MBObj->LA_download_path2 = (Object*)Label2(GCS(catalog, 305,"Download Path"));
    MBObj->TX_download_path = (Object*)TextObject,
        MUIA_Frame, MUIV_Frame_Text,
        MUIA_Weight, 100,
        MUIA_Text_PreParse, NULL,
        MUIA_Text_Contents, NULL,
    End;
if(AMIX) printf("48\n");
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
if(AMIX) printf("49\n");
    GROUP_ROOT_9 = (Object*)GroupObject,
        //MUIA_FrameTitle, "Outgoing File Transfers",
        Child, MBObj->LV_send_dcc,
        Child, GR_send_dcc,
    End;

    MBObj->WI_dcc = (Object*)WindowObject,
        MUIA_Window_Title, GCS(catalog,62,"Incoming File Transfers"),
        MUIA_Window_ID, MAKE_ID('5', 'W', 'I', 'N'),
        MUIA_Window_NoMenus, FALSE,
        WindowContents, GROUP_ROOT_7,
        MUIA_Window_Activate,FALSE,
    End;

    MBObj->WI_dcc2 = (Object*)WindowObject,
        MUIA_Window_Title, GCS(catalog,63,"Outgoing DCC File Transfers"),
        MUIA_Window_ID, MAKE_ID('8', 'W', 'I', 'N'),
        MUIA_Window_NoMenus, FALSE,
        WindowContents, GROUP_ROOT_9,
        MUIA_Window_Activate,FALSE,
    End;

    //below is the main settings window and objects
if(AMIX) printf("50\n");
    MBObj->RA_nick_completion_style_Label= (Object*)Label2(GCS(catalog,42,"Nick Completion Style"));

    static const char *RA_nick_completion_style_Titles[] =
    {
       "AmIRC", "mIRC", NULL
    };

    MBObj->RA_nick_completion_style = (Object*)CycleObject,
       MUIA_Cycle_Entries, RA_nick_completion_style_Titles,
        MUIA_ShortHelp, GCS(catalog,43,"AmIRC style puts a colon and a space\nafter users nicks if used at the beginning\nof a line, or just a space afterwards\nanywhere else.\nmIRC style: No colon\nor spaces are used, only the nick is\ndisplayed"),
    End;
if(AMIX) printf("51\n");
    GR_grp_32=(Object*)GroupObject,
        //MUIA_Group_Columns, 2,
        MUIA_Group_Horiz, TRUE,
        //Child, HSpace(0),
        Child, MBObj->RA_nick_completion_style_Label,
        Child, MBObj->RA_nick_completion_style,
        Child, HSpace(0),
    End;
if(AMIX) printf("52\n");
    static CONST_STRPTR RA_GroupTitleColor[] =
    {
       //"Overwrite", "Resume", "Abort", "Ask", NULL
       (CONST_STRPTR)"Overwrite",
       (CONST_STRPTR)"Resume",
       (CONST_STRPTR)"Abort",
       (CONST_STRPTR)"Ask",
       (CONST_STRPTR)"Rename",
       NULL
    };

    RA_GroupTitleColor[0] = GCS(catalog,36,"Overwrite");
    RA_GroupTitleColor[1] = GCS(catalog,35,"Resume");
    RA_GroupTitleColor[2] = GCS(catalog,37,"Abort");
    RA_GroupTitleColor[3] = GCS(catalog,44,"Ask");
    RA_GroupTitleColor[4] = GCS(catalog,369,"Rename");

if(AMIX) printf("53\n");
    MBObj->RA_autoaccept = (Object*)CycleObject,
       MUIA_Cycle_Entries, RA_GroupTitleColor,
    End;

    MBObj->CH_autoaccept = (Object*)CheckMark(FALSE);
if(AMIX) printf("54\n");
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
if(AMIX) printf("55\n");
    MBObj->CH_autojoin_channels_when_kicked = (Object*)CheckMark(FALSE);

    obj_autojoin_channels_when_kicked_label = (Object*)Label2(GCS(catalog,45,"Auto-rejoin channels when kicked"));

    MBObj->CH_auto_open_query_tabs_when_msged = (Object*)CheckMark(FALSE);

    obj_auto_open_query_tabs_when_msged_label = (Object*)Label2(GCS(catalog,46,"Auto-open private tabs when messaged"));

    grp_autojoin_channels_when_kicked = (Object*)GroupObject,
        MUIA_Group_Horiz, TRUE,
        MUIA_Weight, 100,
        //Child, HSpace(0),
        Child, MBObj->CH_autojoin_channels_when_kicked,
        Child, obj_autojoin_channels_when_kicked_label,
        Child, HSpace(0),
    End;
if(AMIX) printf("56\n");
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

    obj_aux3 = (Object*)Label2(GCS(catalog,47,"Maximum number of lines to keep in buffer"));

    obj_aux2 = (Object*)GroupObject,
        MUIA_Group_Columns, 2,
        Child, obj_aux3,
        Child, MBObj->STR_label_3,
    End;

if(AMIX) printf("57\n");
    REC_label_1 = (Object*)RectangleObject,
        MUIA_Rectangle_HBar, TRUE,
        MUIA_FixHeight, 8,
    End;

    MBObj->CH_label_2 = (Object*)CheckMark(FALSE);

    obj_aux5 = (Object*)Label2(GCS(catalog,48,"Log channels"));


    MBObj->CH_label_3 = (Object*)CheckMark(FALSE);

    obj_aux7 = (Object*)Label2(GCS(catalog,49,"Log private chats"));
if(AMIX) printf("58\n");
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
        //MUIA_FrameTitle, GCS(catalog,20,"Logging"),
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
if(AMIX) printf("59\n");
    REC_label_2 = (Object*)RectangleObject,
        MUIA_Rectangle_HBar, TRUE,
        MUIA_FixHeight, 8,
    End;

    MBObj->STR_label_1 = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_HelpNode, "STR_label_1",
        MUIA_String_MaxLen, 800,
    End;
if(AMIX) printf("60\n");
    obj_aux9 = (Object*)Label2(GCS(catalog,50,"Default Kick Message"));

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
if(AMIX) printf("61\n");
    obj_aux11 = (Object*)Label2(GCS(catalog,51,"Default Ban Message"));

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

    obj_aux13 = (Object*)Label2(GCS(catalog,52,"Quit Message"));
if(AMIX) printf("62\n");
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

    obj_aux15 = (Object*)Label2(GCS(catalog,53,"Part Message"));

    obj_aux14 = (Object*)GroupObject,
        MUIA_Group_Columns, 2,
        Child, obj_aux15,
        Child, MBObj->STR_label_5,
    End;
if(AMIX) printf("63\n");
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

if(AMIX) printf("64\n");
    MBObj->CH_label_4 = (Object*)CheckMark(FALSE);

    obj_aux17 = (Object*)Label2(GCS(catalog,54,"Request Whois information"));

    MBObj->CH_label_5 = (Object*)CheckMark(FALSE);

    obj_aux19 = (Object*)Label2(GCS(catalog,55,"Open private tab"));

    obj_aux18 = (Object*)GroupObject,
        MUIA_Group_Horiz, TRUE,
        MUIA_Weight, 100,
        Child, MBObj->CH_label_5,
        Child, obj_aux19,
        Child, HSpace(0),
    End;
if(AMIX) printf("65\n");
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
        MUIA_FrameTitle, GCS(catalog,56,"Nicklist doubleclick action(s)"),
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
        MUIA_ShortHelp,GCS(catalog,57,"Customise your timestamps! use %h for the hour, %m for the minute, and %s for the second. This is useful to see what time an event happened (such as a user joining, or speaking)"),
        MUIA_HelpNode, "STR_label_0",
        MUIA_String_MaxLen, 50,
    End;
if(AMIX) printf("66\n");
    obj_aux21 = (Object*)Label2(GCS(catalog,58,"Timestamp Format"));

    MBObj->TX_highlight_label = (Object*)Label2(GCS(catalog,59,"Words to Highlight"));

    MBObj->STR_highlight_label = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_ShortHelp,GCS(catalog,258,"Enter words here for WookieChat to watch for. Lines of text will appear in the highlight colour if one of these words is found. Enter the words you want to highlight, separated by spaces"),
        MUIA_String_MaxLen, 800,
    End;


    GR_grp_26 = (Object*)GroupObject,
        MUIA_Group_Columns, 2,
        //MUIA_FrameTitle, GCS(catalog,64,"Highlight"),
        Child, MBObj->TX_highlight_label,
        Child, MBObj->STR_highlight_label,
    End;


    MBObj->CH_label_0 = (Object*)CheckMark(FALSE);
if(AMIX) printf("67\n");
    obj_aux23 = (Object*)Label2(GCS(catalog,65,"Use timestamps"));

    obj_aux22 = (Object*)GroupObject,
        MUIA_FrameTitle, GCS(catalog,66,"Timestamps"),
        MUIA_Group_Horiz, TRUE,
        MUIA_Weight, 100,
        Child, MBObj->CH_label_0,
        Child, obj_aux23,
        Child, HSpace(0),
    End;

    APTR obj_aux222 = (Object*)GroupObject,
        //MUIA_FrameTitle, GCS(catalog,66,"Timestamps"),
        MUIA_Group_Horiz, TRUE,
        Child, obj_aux21,
        Child, MBObj->STR_label_0,
    End;


    REC_label_5 = (Object*)RectangleObject,
        MUIA_Rectangle_HBar, TRUE,
        MUIA_FixHeight, 8,
    End;
if(AMIX) printf("68\n");
    MBObj->STR_PA_label_0 = (Object*)String("", 400);

    MBObj->PA_label_0 = (Object*)PopButton(MUII_PopFile);

    MBObj->PA_label_0 = (Object*)PopaslObject,
        MUIA_HelpNode, "PA_label_0",
        MUIA_ShortHelp, GCS(catalog,67,"This text editor is used when you edit your \"Perform on Connect\" scripts (from the Server Selection window)"),
        MUIA_Popstring_String, MBObj->STR_PA_label_0,
        MUIA_Popstring_Button, MBObj->PA_label_0,
    End;

    REC_label_6 = (Object*)RectangleObject,
        MUIA_Rectangle_HBar, TRUE,
        MUIA_FixHeight, 8,
    End;

    MBObj->LA_texteditor = (Object*)Label2(GCS(catalog,68,"Text Editor"));
if(AMIX) printf("69\n");
    GR_grp_28 = (Object*)GroupObject,
        MUIA_HelpNode, "GR_grp_28",
        MUIA_Group_Horiz, TRUE,
        //MUIA_FrameTitle, "Text Editor",
        Child, MBObj->LA_texteditor,
        Child, MBObj->PA_label_0,
    End;

    MBObj->PA_browser_label = (Object*)Label2(GCS(catalog,69,"Web Browser"));

    MBObj->PA_browser = (Object*)PopButton(MUII_PopFile);

    MBObj->STR_browser = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_ShortHelp, GCS(catalog,70,"Choose a web browser to use with WookieChat's \"URL Grabber\" Window. Double clicking an address there will load in this chosen browser"),
        MUIA_String_MaxLen, 1000,
    End;

    MBObj->PA_browser = (Object*)PopaslObject,
        MUIA_Popasl_Type, ASL_FileRequest,
        ASLFR_TitleText,GCS(catalog,71,"Choose browser to use"),
        ASLFR_InitialDrawer,"progdir:",
        MUIA_Popstring_String, MBObj->STR_browser,
        MUIA_Popstring_Button, MBObj->PA_browser,
    End;
if(AMIX) printf("70\n");


             //
    GR_grp_31 = (Object*)GroupObject,
        MUIA_HelpNode, "GR_grp_31",
        MUIA_Group_Horiz, TRUE,
        //MUIA_FrameTitle, "Web Browser",
        Child, MBObj->PA_browser_label,
        Child, MBObj->PA_browser,
    End;

    MBObj->BT_label_0 = (Object*)SimpleButton(GCS(catalog,72,"Save"));

    MBObj->BT_label_1 = (Object*)SimpleButton(GCS(catalog,73,"Use"));

    MBObj->BT_label_2 = (Object*)SimpleButton(GCS(catalog,74,"Cancel"));

    MBObj->BT_label_3 = (Object*)SimpleButton(GCS(catalog,361,"Apply"));

    GR_grp_29 = (Object*)GroupObject,
        MUIA_HelpNode, "GR_grp_29",
        MUIA_Group_Columns, 4,
        Child, MBObj->BT_label_0,
        Child, MBObj->BT_label_1,
        Child, MBObj->BT_label_3,
        Child, MBObj->BT_label_2,
    End;
if(AMIX) printf("71\n");

    obj_settings1_dcc = (Object*)Label2(GCS(catalog,75,"Auto accept incoming DCC File Transfers"));
    obj_settings2_dcc = (Object*)Label2(GCS(catalog,76,"Action to take if file exists"));

    obj_gethostid_dcc = (Object*)Label2(GCS(catalog,77,"Use local address for DCC connections"));
    MBObj->CH_gethostid = (Object*)CheckMark(FALSE);

    MBObj->CH_ipaddr_dcc = (Object*)CheckMark(FALSE);
    obj_ipaddr_dcc = (Object*)Label2(GCS(catalog,294,"Specify DCC address?"));
    MBObj->STR_ipaddr_dcc = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_ShortHelp, GCS(catalog,295,"WookieChat attempts to obtain your address automatically for outgoing DCC connections. If this address is incorrect, you can specify it here"),
    End;
if(AMIX) printf("72\n");

    obj_port1_dcc = (Object*)Label2(GCS(catalog,78,"DCC port range from "));
    obj_port2_dcc = (Object*)Label2(GCS(catalog,79," to "));

    MBObj->STR_port1_dcc = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_String_Reject," ",
        MUIA_ShortHelp, GCS(catalog,80,"This is the start of the port range for incoming DCC connections. This is configurable so you can choose which ports to use and open up on any firewall you might have"),
    End;

    MBObj->STR_port2_dcc = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_String_Reject," ",
        MUIA_ShortHelp, GCS(catalog,81,"This is the end of the port range for incoming DCC connections. This is configurable so you can choose which ports to use and open up on any firewall you might have"),
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
if(AMIX) printf("74\n");
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


    MBObj->LA_Maximum_Retries = (Object*)Label2(GCS(catalog,261,"Maximum attempts to reconnect after non-requested disconnection"));

    MBObj->NB_Maximum_Retries = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_Weight, 100,
        MUIA_String_Accept, "0123456789",
    End;

    MBObj->LA_clone_detection = (Object*)Label2(GCS(catalog,285,"Clone detection"));
    MBObj->CH_clone_detection = (Object*)CheckMark(FALSE);

    APTR GR_clone_detection = (Object*)GroupObject,
        MUIA_Weight, 100,
        MUIA_Group_Horiz, TRUE,
        Child, MBObj->CH_clone_detection,
        Child, MBObj->LA_clone_detection,
        Child,HSpace(0),
    End;


if(AMIX) printf("75\n");
    MBObj->LA_Reconnect_Delay = (Object*)Label2(GCS(catalog,262,"Delay (in seconds) in between reconnection attempts"));
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
            Child, MBObj->LA_hide_joins_parts=(Object*)Label2(GCS(catalog,383,"Hide Join and Part messages in Channels")),
            Child, HSpace(0),
        End,
        Child, (Object*)GroupObject,
            MUIA_Weight, 100,
            MUIA_Group_Horiz, TRUE,
            Child, MBObj->CH_hide_channel_mode_gadgets=(Object*)CheckMark(FALSE),
            Child, MBObj->LA_hide_channel_mode_gadgets=(Object*)Label2(GCS(catalog,800,"Hide the channel mode buttons at the top of the display")),
            Child, HSpace(0),
        End,
    End;
if(AMIX) printf("76\n");
    //GUI SETTINGS objects
    MBObj->LA_no_server_tabs = (Object*)Label2(GCS(catalog,292,"Disable server tabs"));
    MBObj->CH_no_server_tabs = (Object*)CheckMark(FALSE);

    static CONST_STRPTR CYCLE_tabs_entries[] =
    {
       (CONST_STRPTR)"Listview",
       (CONST_STRPTR)"Buttons",
       NULL,
    };

    CYCLE_tabs_entries[0] = GCS(catalog,375,"Listview");
    CYCLE_tabs_entries[1] = GCS(catalog,376,"Buttons");

    //MBObj->LA_tabs = (Object*)Label2(GCS(catalog,296,"Use button-style tabs"));
    //MBObj->CH_tabs = (Object*)CheckMark(TRUE);
    MBObj->LA_tabs = (Object*)Label2(GCS(catalog,377,"Method of displaying Tabs for channels and private messages"));
    MBObj->CYCLE_tabs = (Object*)CycleObject,
       MUIA_Weight,0,
       MUIA_Cycle_Entries, CYCLE_tabs_entries,
    End;
    //MBObj->CH_tabs = (Object*)CheckMark(TRUE);



    MBObj->LA_listview_tabs_weight = (Object*)Label2(GCS(catalog,306,"Listview tabs vertical weight"));
    MBObj->NB_listview_tabs_weight = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_Weight, 100,
        MUIA_String_Accept, "0123456789",
    End;

if(AMIX) printf("77\n");
    MBObj->LA_nicklist_horiz_weight = (Object*)Label2(GCS(catalog,321,"Nicklist horizontal weight"));

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
if(AMIX) printf("78\n");
    GR_grp_33 = (Object*)GroupObject,
        MUIA_FrameTitle,GCS(catalog,307,"Tab Preferences"),
        //MUIA_Frame,MUIV_Frame_Group,
        MUIA_HelpNode, "GR_grp_29",
        MUIA_Group_Horiz, TRUE,
        //MUIA_Group_Columns, 4,
        //MUIA_ShortHelp,GCS(catalog,297,"Turn off this option to use Listview-style tabs (the listview is better having a large amount of tabs open, such as 20 or 30)"),
        //Child, HSpace(0),
        Child,MBObj->CYCLE_tabs,
        Child,MBObj->LA_tabs,
        //Child,MBObj->CH_no_server_tabs,
        //Child,MBObj->LA_no_server_tabs,
        Child, HSpace(0),
    End;
if(AMIX) printf("79\n");
    APTR GR_grp_332 = (Object*)GroupObject,
        //MUIA_FrameTitle,GCS(catalog,307,"Tab Preferences"),
        //MUIA_Frame,MUIV_Frame_Group,
        MUIA_HelpNode, "GR_grp_29",
        MUIA_Group_Horiz, TRUE,
        Child,MBObj->CH_no_server_tabs,
        Child,MBObj->LA_no_server_tabs,
        Child, HSpace(0),
    End;

if(AMIX) printf("80\n");
    setmacro((Object*)MBObj->LA_tabs,MUIA_ShortHelp,GCS(catalog,297,"Turn off this option to use Listview-style tabs (the listview is better having a large amount of tabs open, such as 20 or 30)"));
    setmacro((Object*)MBObj->LA_listview_tabs_weight,MUIA_ShortHelp,GCS(catalog,308,"If you have disabled button style tabs, you can configure the size of the \"tabs listview\""));
    setmacro((Object*)MBObj->LA_no_server_tabs,MUIA_ShortHelp,GCS(catalog,309,"You can turn off keeping a dedicated tab for server messages to conserve memory and valuable screen space"));
if(AMIX) printf("81\n");

    MBObj->LA_graphical_smileys = (Object*)Label2(GCS(catalog,300,"Enable Graphical Smileys"));
    MBObj->CH_graphical_smileys = (Object*)CheckMark(TRUE);

    MBObj->LA_graphical_nicklist = (Object*)Label2(GCS(catalog,310,"Enable Graphical Nicklist"));
    MBObj->CH_graphical_nicklist = (Object*)CheckMark(TRUE);


    MBObj->RA_graphical_smileys_background_colour_label= (Object*)Label2(GCS(catalog,322,"Smileys theme"));
if(AMIX) printf("82\n");
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
if(AMIX) printf("84\n");
    MBObj->BT_graphical_smileys_preview = (Object*)SimpleButton("?");
    setmacro((Object*)MBObj->BT_graphical_smileys_preview,MUIA_Weight,0);

    GR_graphical_smileys=(Object*)GroupObject,
        MUIA_ShortHelp, GCS(catalog,302,"This option will display common ASCII \"smiley faces\" such as :) and :D in aa graphical way.\n(This feature is reliant on picture datatypes)"),
        MUIA_HelpNode, "GR_grp_31",
        MUIA_Group_Horiz, FALSE,
        MUIA_Group_Horiz, TRUE,
        Child, MBObj->CH_graphical_smileys,
        Child, MBObj->LA_graphical_smileys,
        Child, HSpace(0),
    End;

    APTR GR_graphical_nicklist=(Object*)GroupObject,
        MUIA_ShortHelp, GCS(catalog,302,"This option will display common ASCII \"smiley faces\" such as :) and :D in aa graphical way.\n(This feature is reliant on picture datatypes)"),
        MUIA_HelpNode, "GR_grp_31",
        MUIA_Group_Horiz, FALSE,
        MUIA_Group_Horiz, TRUE,
        Child, MBObj->CH_graphical_nicklist,
        Child, MBObj->LA_graphical_nicklist,
        Child, HSpace(0),
    End;
if(AMIX) printf("85\n");
    MBObj->LA_column_width_timestamp = (Object*)Label2(GCS(catalog,312,"Column width (in Pixels) for timestamps"));

    MBObj->NB_column_width_timestamp = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_Weight, 100,
        MUIA_String_Accept, "0123456789",
    End;


    MBObj->LA_column_width_nick = (Object*)Label2(GCS(catalog,313,"Column width (in Pixels) for the middle/nicks column"));

    MBObj->NB_column_width_nick = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_Weight, 100,
        MUIA_String_Accept, "0123456789",
    End;


    MBObj->LA_column_spacing = (Object*)Label2(GCS(catalog,314,"Amount of space surrounding columns divider bar"));

    //MBObj->NB_column_spacing = (Object*)NumericbuttonObject,
    MBObj->NB_column_spacing = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_Weight, 100,
        MUIA_String_Accept, "0123456789",
    End;


    MBObj->LA_remove_brackets = (Object*)Label2(GCS(catalog,315,"Remove < and > from around nicks"));
    MBObj->CH_remove_brackets = (Object*)CheckMark(TRUE);

    APTR GR_column_width = (Object*)GroupObject,
        MUIA_Group_Horiz,TRUE,
        MUIA_FrameTitle, GCS(catalog,316,"Multicolumn display settings"),
        Child, MBObj->LA_column_width_timestamp,
        Child, MBObj->NB_column_width_timestamp,
        //Child,HSpace(0),
    End;

    APTR GR_column_width2 = (Object*)GroupObject,
        MUIA_Group_Horiz,TRUE,
        //MUIA_Group_Columns,2,
        //MUIA_FrameTitle, GCS(catalog,316,"Multicolumn display settings"),
        //MUIA_Frame,MUIV_Frame_Group,
        Child, MBObj->LA_column_width_nick,
        Child, MBObj->NB_column_width_nick,
        //Child,HSpace(0),
    End;

    APTR GR_column_width3 = (Object*)GroupObject,
        MUIA_Group_Horiz,TRUE,
        //MUIA_Group_Columns,2,
        //MUIA_FrameTitle, GCS(catalog,316,"Multicolumn display settings"),
        //MUIA_Frame,MUIV_Frame_Group,
        Child, MBObj->LA_column_spacing,
        Child, MBObj->NB_column_spacing,
        //Child,HSpace(0),
    End;

    APTR GR_column_width4 = (Object*)GroupObject,
        MUIA_Group_Horiz,TRUE,
        //MUIA_Group_Columns,2,
        //MUIA_FrameTitle, GCS(catalog,316,"Multicolumn display settings"),
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
if(AMIX) printf("86\n");
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
            Child, MBObj->LA_user_modes = (Object*)Label2(GCS(catalog,800,"Display usermodes beside a persons nick when they talk")),
            Child, HSpace(0),
        End,
        //Child, GR_column_width,
        //Child, GR_remove_brackets,
    End;
if(AMIX) printf("87\n");
    GROUP_ROOT_2_DCC = (Object*)GroupObject,
        Child, grp_dcc_settings1,
        Child, grp_dcc_settings2,
        Child, grp_dcc_settings3,
        Child, grp_dcc_settings4,
        Child, grp_dcc_settings5,
        Child, GR_dcc_grp_28,
        Child, GR_dcc_grp_282,
    End;
if(AMIX) printf("88\n");
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
if(AMIX) printf("89\n");
    // new message sample

    static CONST_STRPTR RA_sound_sample_newmsg_entries[] =
    {
       (CONST_STRPTR)"Never",
       (CONST_STRPTR)"When private tab is not selected",
       (CONST_STRPTR)"Always",
       (CONST_STRPTR)"Only when Window is inactive",
       NULL,
    };

    RA_sound_sample_newmsg_entries[0] = GCS(catalog,82,"Never");
    RA_sound_sample_newmsg_entries[1] = GCS(catalog,367,"When private tab is not selected");
    RA_sound_sample_newmsg_entries[2] = GCS(catalog,83,"Always");
    RA_sound_sample_newmsg_entries[3] = GCS(catalog,84,"Only when Window is inactive");

if(AMIX) printf("90\n");
    MBObj->LA_sound_sample_newmsg = (Object*)Label2(GCS(catalog,366,"Sample to play when a new PRIVMSG arrives"));

    MBObj->STR_sound_sample_newmsg = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_ShortHelp, " ",
    End;

    MBObj->PA_sound_sample_newmsg_label = (Object*)Label2(GCS(catalog,86,"Play sample"));

    MBObj->PA_sound_sample_newmsg = (Object*)PopButton(MUII_PopFile);

    MBObj->PA_sound_sample_newmsg = (Object*)PopaslObject,
        MUIA_Popasl_Type, ASL_FileRequest,
        ASLFR_TitleText,GCS(catalog,87,"Choose filename"),
        ASLFR_InitialDrawer,"progdir:",
        MUIA_Popstring_String, MBObj->STR_sound_sample_newmsg,
        MUIA_Popstring_Button, MBObj->PA_sound_sample_newmsg,
    End;

    MBObj->RA_sound_sample_newmsg = (Object*)CycleObject,
       MUIA_Cycle_Entries, RA_sound_sample_newmsg_entries,
    End;

    //tabopen action entries

    static CONST_STRPTR RA_sound_sample_tabopen_entries[] =
    {
       (CONST_STRPTR)"Never",
       (CONST_STRPTR)"Always",
       (CONST_STRPTR)"Only when Window is inactive",
       NULL,
    };

    RA_sound_sample_tabopen_entries[0] = GCS(catalog,82,"Never");
    RA_sound_sample_tabopen_entries[1] = GCS(catalog,83,"Always");
    RA_sound_sample_tabopen_entries[2] = GCS(catalog,84,"Only when Window is inactive");

if(AMIX) printf("91\n");
    MBObj->LA_sound_sample_tabopen = (Object*)Label2(GCS(catalog,85,"Sample to play when a tab opens"));

    MBObj->STR_sound_sample_tabopen = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_ShortHelp, " ",
    End;

    MBObj->PA_sound_sample_tabopen_label = (Object*)Label2(GCS(catalog,86,"Play sample"));

    MBObj->PA_sound_sample_tabopen = (Object*)PopButton(MUII_PopFile);

    MBObj->PA_sound_sample_tabopen = (Object*)PopaslObject,
        MUIA_Popasl_Type, ASL_FileRequest,
        ASLFR_TitleText,GCS(catalog,87,"Choose filename"),
        ASLFR_InitialDrawer,"progdir:",
        MUIA_Popstring_String, MBObj->STR_sound_sample_tabopen,
        MUIA_Popstring_Button, MBObj->PA_sound_sample_tabopen,
    End;

    MBObj->RA_sound_sample_tabopen = (Object*)CycleObject,
       MUIA_Cycle_Entries, RA_sound_sample_tabopen_entries,
    End;


    //highlight action objects
if(AMIX) printf("92\n");
    static CONST_STRPTR RA_sound_sample_highlight_entries[] =
    {
       //"Never", "Always", "Only when Window is inactive", NULL
       (CONST_STRPTR)"Never",
       (CONST_STRPTR)"Always",
       (CONST_STRPTR)"Only when Window is inactive",
       (CONST_STRPTR)"Only when Tab is inactive",
       NULL
    };

    RA_sound_sample_highlight_entries[0] = GCS(catalog,82,"Never");
    RA_sound_sample_highlight_entries[1] = GCS(catalog,83,"Always");
    RA_sound_sample_highlight_entries[2] = GCS(catalog,84,"Only when Window is inactive");
    RA_sound_sample_highlight_entries[3] = GCS(catalog,373,"Only when Tab is inactive");

    MBObj->LA_sound_sample_highlight = (Object*)Label2(GCS(catalog,88,"Sample to play on highlight"));

    MBObj->STR_sound_sample_highlight = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_ShortHelp, " ",
    End;

    MBObj->PA_sound_sample_highlight_label = (Object*)Label2(GCS(catalog,86,"Play sample"));

    MBObj->PA_sound_sample_highlight = (Object*)PopButton(MUII_PopFile);
if(AMIX) printf("93\n");
    MBObj->PA_sound_sample_highlight = (Object*)PopaslObject,
        MUIA_Popasl_Type, ASL_FileRequest,
        ASLFR_TitleText,GCS(catalog,87,"Choose filename"),
        ASLFR_InitialDrawer,"progdir:",
        MUIA_Popstring_String, MBObj->STR_sound_sample_highlight,
        MUIA_Popstring_Button, MBObj->PA_sound_sample_highlight,
    End;

    MBObj->RA_sound_sample_highlight = (Object*)CycleObject,
       MUIA_Cycle_Entries, RA_sound_sample_highlight_entries,
    End;

    MBObj->LA_samples_path = (Object*)Label2(GCS(catalog,89,"Samples Path for CTCP SOUND"));

    MBObj->STR_samples_path = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_ShortHelp, GCS(catalog,90,"When you recieve a CTCP SOUND request, WookieChat will\nlook in this path for sound samples"),
        MUIA_String_MaxLen, 400,
    End;

    MBObj->PA_samples_path = (Object*)PopButton(MUII_PopDrawer);

    MBObj->PA_samples_path = (Object*)PopaslObject,
        MUIA_Popasl_Type, ASL_FileRequest,
        ASLFR_TitleText,GCS(catalog,91,"Choose directory"),
        ASLFR_InitialDrawer,"progdir:",
        MUIA_Popstring_String, MBObj->STR_samples_path,
        MUIA_Popstring_Button, MBObj->PA_samples_path,
    End;

if(AMIX) printf("94\n");
    //external sound player for CTCP SOUND
    MBObj->LA_sound_replayer = (Object*)Label2(GCS(catalog,303,"Use External Replayer For CTCP SOUND"));
    MBObj->CH_sound_replayer = (Object*)CheckMark(FALSE);
    MBObj->STR_sound_replayer = (Object*)StringObject,MUIA_Frame, MUIV_Frame_String,MUIA_ShortHelp, " ",End;
    MBObj->PA_sound_replayer_label = (Object*)Label2(GCS(catalog,304,"Sound player"));
    MBObj->PA_sound_replayer = (Object*)PopButton(MUII_PopFile);
    MBObj->PA_sound_replayer = (Object*)PopaslObject,
        MUIA_Popasl_Type, ASL_FileRequest,
        //ASLFR_TitleText,GCS(catalog,87,"Choose Sound Player"),
        ASLFR_InitialDrawer,"progdir:",
        MUIA_Popstring_String, MBObj->STR_sound_replayer,
        MUIA_Popstring_Button, MBObj->PA_sound_replayer,
    End;

if(AMIX) printf("95\n");
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
if(AMIX) printf("96\n");
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
if(AMIX) printf("97\n");
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
        MUIA_Text_Contents, GCS(catalog,92,"# of lines to display from the end of logfiles when opening a tab\n(Put 0 to disable this feature)"),
    End;
if(AMIX) printf("98\n");
    APTR GR_how_many_lines_to_reload = (Object*)GroupObject,
        MUIA_Group_Horiz, TRUE,
        //Child, HSpace(0),
        Child, MBObj->TX_how_many_lines_to_reload,
        Child, HSpace(0),
    End;

    APTR obj_how_many_lines_to_reload_channel = (Object*)Label2(GCS(catalog,93,"channel"));

    MBObj->STR_how_many_lines_to_reload_channel = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_ShortHelp, GCS(catalog,94,"How many lines to reload from the end of log files when you join a channel"),
        MUIA_String_Accept, "0123456789",
        MUIA_String_MaxLen, 4,
    End;
if(AMIX) printf("99\n");
    APTR obj_how_many_lines_to_reload_private = (Object*)Label2(GCS(catalog,95,"private"));

    MBObj->STR_how_many_lines_to_reload_private = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_ShortHelp, GCS(catalog,96,"How many lines to reload from the end of log files when you open a private query tab to another user"),
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

    if(AMIX) printf("100\n");

    MBObj->CH_split_logs = (Object*)CheckMark(FALSE);
    MBObj->LA_split_logs = (Object*)Label(GCS(catalog,289,"Split up logfiles into smaller chunks"));

    MBObj->LA_logsize = (Object*)Label(GCS(catalog,290,"If the above is enabled, choose a log file size (in kbytes)"));
    MBObj->STR_logsize = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_HelpNode, "STR_servername",
    End;

    GR_logging_splitup = (Object*)GroupObject,
        MUIA_ShortHelp, GCS(catalog,291,"When a tab is opened, the logfile will be examined. If it is over a certain size, then it will be renamed with the date appended to the end, and WookieChat will resume logging in a new empty logfile. This is implemented for performance reasons. It can take a few seconds to reload large logfiles that are a few MB in size."),
        //MUIA_Group_Columns, 2,
        MUIA_Group_Horiz, TRUE,
        Child, MBObj->CH_split_logs,
        Child, MBObj->LA_split_logs,
        Child, HSpace(0),
    End;

    GR_logging_splitup2 = (Object*)GroupObject,
        MUIA_ShortHelp, GCS(catalog,291,"When a tab is opened, the logfile will be examined. If it is over a certain size, then it will be renamed with the date appended to the end, and WookieChat will resume logging in a new empty logfile. This is implemented for performance reasons, because it can sometimes take a few seconds to reload large logfiles that are a few MB in size."),
        MUIA_Group_Columns, 2,
        Child, MBObj->LA_logsize,
        Child, MBObj->STR_logsize,
    End;
if(AMIX) printf("101\n");
    APTR GR_reload = (Object*)GroupObject,
        MUIA_Group_Horiz, FALSE,
        MUIA_Frame, MUIV_Frame_Group,
        Child, GR_how_many_lines_to_reload,
        Child, GR_grp_reload_log2,
    End;


if(AMIX) printf("102\n");
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
        MUIA_ShortHelp, GCS(catalog,98,"Enter an alias name, then the command you want it to use\nE.g. With this line added to the alias list..\n/j /join\n..then the \"/j\" command will be replaced with \"/join\""),
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

    MBObj->BT_add_alias = (Object*)SimpleButton(GCS(catalog,99,"Add alias"));
    MBObj->BT_remove_alias = (Object*)SimpleButton(GCS(catalog,100,"Remove alias"));

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
            Child, Label(GCS(catalog,280,"Alias")),
            Child, MBObj->STR_edit_alias_name=(Object*)StringObject,MUIA_Weight, 30, MUIA_Frame, MUIV_Frame_String,MUIA_String_Reject," ",MUIA_String_MaxLen, 50,End,
            Child, Label(GCS(catalog,281,"Command")),
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
                Child, MBObj->BT_user_list_buttons_add=SimpleButton(GCS(catalog,378,"Add New")),
                Child, MBObj->BT_user_list_buttons_delete=SimpleButton(GCS(catalog,379,"Delete")),
                Child, MBObj->BT_user_list_buttons_move_up=SimpleButton(GCS(catalog,380,"Move up")),
                Child, MBObj->BT_user_list_buttons_move_down=SimpleButton(GCS(catalog,381,"Move down")),
            End,
            Child, GroupObject,
                MUIA_Group_Columns,2,
                MUIA_Group_Horiz,TRUE,
                Child, MBObj->LA_user_list_buttons_help=Label(GCS(catalog,382,"\033lCodes:\n%s = selected nick, %h = hostname of the selected nick,\n%c = current channel, %n = your own nick")),
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
                    //MUIA_ShortHelp, GCS(catalog,17,"This is the hostname of the user you are currently talking to"),
                End,
                Child, Label("Command"),
                Child, MBObj->STR_user_list_buttons_command=(Object*)BetterStringObject,
                    MUIA_Weight, 70,
                    MUIA_String_MaxLen, 800,
                    MUIA_Frame, MUIV_Frame_String,
                    //MUIA_ShortHelp, GCS(catalog,17,"This is the hostname of the user you are currently talking to"),
                End,
                Child, MBObj->LA_user_list_buttons_disable=Label(GCS(catalog,384,"Hide all from the display")),
                Child, MBObj->CH_user_list_buttons_disable=CheckMark(FALSE),
            End,
        End;



    LV_events = (Object*) NListObject,
            MUIA_ShortHelp, GCS(catalog,98,"Enter an alias name, then the command you want it to use\nE.g. With this line added to the alias list..\n/j /join\n..then the \"/j\" command will be replaced with \"/join\""),
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
            Child, (Object*)SimpleButton(GCS(catalog,800,"Add")),
            Child, (Object*)SimpleButton(GCS(catalog,800,"Remove")),
        End, */
        Child, GroupObject,
            MUIA_Group_Columns,2,
            MUIA_Group_Horiz,TRUE,
            Child, (Object*)Label(GCS(catalog,800,"\033lCodes:\n%b = text being displayed in the window, %t = timestamp,\n%g = network name, %s = servername, %c = channel, %n = own nick, %m = nick")),
            Child, HSpace(0),
        End,
        Child, LV_events,
        Child, (Object*)GroupObject,
        MUIA_Group_Horiz, TRUE,
        MUIA_Group_Columns, 3,
            Child, Label("Use when:"),
            Child, CYCLE_events=(Object*)CycleObject,
             MUIA_Cycle_Entries, CYCLE_events_entries,
                MUIA_ShortHelp, GCS(catalog,800,"Choose when you want to execute this ARexx script"),
            End,
            Child, HSpace(0),
        End,
        Child, (Object*)GroupObject,
        MUIA_Group_Horiz, TRUE,
        MUIA_Group_Columns, 4,
            Child, Label(GCS(catalog,800,"ARexx Script")),
            Child, STR_events_script= (Object*)PopaslObject,
                MUIA_HelpNode, "PA_dcc_send_file",
                MUIA_Popasl_Type, ASL_FileRequest,
                ASLFR_TitleText,"ARexx Event File..",//GCS(catalog,29,"Choose filename to send"),
                ASLFR_InitialDrawer,wookie_folder,
                MUIA_Popstring_String, (Object*)BetterStringObject,
                    MUIA_Weight, 30,
                    MUIA_String_MaxLen, 800,
                    MUIA_Frame, MUIV_Frame_String,
                End,
                MUIA_Popstring_Button, (Object*)PopButton(MUII_PopFile), //MBObj->PA_dcc_send_file,
            End,

            Child, Label(GCS(catalog,800,"Text for script")),
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

if(AMIX) printf("105\n");
    MBObj->SETTINGS_ROOT = (Object*)GroupObject,
        Child, MBObj->SETTINGS_Register_Grp,
        Child, GR_grp_29,
    End;

    MBObj->WI_mainsettings = (Object*)WindowObject,
        MUIA_Window_Title, GCS(catalog,101,"Settings"),
        MUIA_Window_ID, MAKE_ID('4', 'W', 'I', 'N'),
        MUIA_Window_NoMenus, FALSE,
        MUIA_Window_CloseGadget, TRUE,
        MUIA_Window_SizeGadget, TRUE,
        WindowContents, MBObj->SETTINGS_ROOT,
    End;
if(AMIX) printf("106\n");



    //below is the colour settings window objects
    LA_CSW_background = (Object*)Label(GCS(catalog,335,"background"));
    MBObj->PP_CSW_background = (Object*)MUI_NewObject((char*)MUIC_Popimage,MUIA_Window_Title, GCS(catalog,360,"Choose Colour"),End;

    LA_CSW_nicklistbackground = (Object*)Label(GCS(catalog,336,"nicklistbackground"));
    MBObj->PP_CSW_nicklistbackground = (Object*)MUI_NewObject((char*)MUIC_Popimage,MUIA_Window_Title, GCS(catalog,360,"Choose Colour"),End;

    LA_CSW_listviewtabs_background= (Object*)Label(GCS(catalog,357,"tabs background"));
    MBObj->PP_CSW_listviewtabs_background = (Object*)MUI_NewObject((char*)MUIC_Popimage,MUIA_Window_Title, GCS(catalog,360,"Choose Colour"),End;


    LA_CSW_join = (Object*)Label(GCS(catalog,337,"join"));
    MBObj->PP_CSW_join = (Object*)MUI_NewObject((char*)MUIC_Poppen,MUIA_Window_Title, GCS(catalog,360,"Choose Colour"),End;

    LA_CSW_part = (Object*)Label(GCS(catalog,338,"part"));
    MBObj->PP_CSW_part = (Object*)MUI_NewObject((char*)MUIC_Poppen,MUIA_Window_Title, GCS(catalog,360,"Choose Colour"),End;

    LA_CSW_quit = (Object*)Label(GCS(catalog,339,"quit"));
    MBObj->PP_CSW_quit = (Object*)MUI_NewObject((char*)MUIC_Poppen,MUIA_Window_Title, GCS(catalog,360,"Choose Colour"),End;

    LA_CSW_modes = (Object*)Label(GCS(catalog,340,"modes"));
    MBObj->PP_CSW_modes = (Object*)MUI_NewObject((char*)MUIC_Poppen,MUIA_Window_Title, GCS(catalog,360,"Choose Colour"),End;

    LA_CSW_ctcp = (Object*)Label(GCS(catalog,341,"ctcp"));
    MBObj->PP_CSW_ctcp = (Object*)MUI_NewObject((char*)MUIC_Poppen,MUIA_Window_Title, GCS(catalog,360,"Choose Colour"),End;

    LA_CSW_actions = (Object*)Label(GCS(catalog,342,"actions"));
    MBObj->PP_CSW_actions = (Object*)MUI_NewObject((char*)MUIC_Poppen,MUIA_Window_Title, GCS(catalog,360,"Choose Colour"),End;

    LA_CSW_normal = (Object*)Label(GCS(catalog,343,"normal"));
    MBObj->PP_CSW_normal = (Object*)MUI_NewObject((char*)MUIC_Poppen,MUIA_Window_Title, GCS(catalog,360,"Choose Colour"),End;

    LA_CSW_info = (Object*)Label(GCS(catalog,344,"info"));
    MBObj->PP_CSW_info = (Object*)MUI_NewObject((char*)MUIC_Poppen,MUIA_Window_Title, GCS(catalog,360,"Choose Colour"),End;

    LA_CSW_owntext = (Object*)Label(GCS(catalog,345,"owntext"));
    MBObj->PP_CSW_owntext = (Object*)MUI_NewObject((char*)MUIC_Poppen,MUIA_Window_Title, GCS(catalog,360,"Choose Colour"),End;

    LA_CSW_highlight = (Object*)Label(GCS(catalog,346,"highlight"));
    MBObj->PP_CSW_highlight = (Object*)MUI_NewObject((char*)MUIC_Poppen,MUIA_Window_Title, GCS(catalog,360,"Choose Colour"),End;

    LA_CSW_invite = (Object*)Label(GCS(catalog,347,"invite"));
    MBObj->PP_CSW_invite = (Object*)MUI_NewObject((char*)MUIC_Poppen,MUIA_Window_Title, GCS(catalog,360,"Choose Colour"),End;

    LA_CSW_kick = (Object*)Label(GCS(catalog,348,"kick"));
    MBObj->PP_CSW_kick = (Object*)MUI_NewObject((char*)MUIC_Poppen,MUIA_Window_Title, GCS(catalog,360,"Choose Colour"),End;

    LA_CSW_nickchange = (Object*)Label(GCS(catalog,349,"nickchange"));
    MBObj->PP_CSW_nickchange = (Object*)MUI_NewObject((char*)MUIC_Poppen,MUIA_Window_Title, GCS(catalog,360,"Choose Colour"),End;

    LA_CSW_topic = (Object*)Label(GCS(catalog,350,"topic"));
    MBObj->PP_CSW_topic = (Object*)MUI_NewObject((char*)MUIC_Poppen,MUIA_Window_Title, GCS(catalog,360,"Choose Colour"),End;

    LA_CSW_wallops = (Object*)Label(GCS(catalog,351,"wallops"));
    MBObj->PP_CSW_wallops = (Object*)MUI_NewObject((char*)MUIC_Poppen,MUIA_Window_Title, GCS(catalog,360,"Choose Colour"),End;

    LA_CSW_notices = (Object*)Label(GCS(catalog,352,"notices"));
    MBObj->PP_CSW_notices = (Object*)MUI_NewObject((char*)MUIC_Poppen,MUIA_Window_Title, GCS(catalog,360,"Choose Colour"),End;

    LA_CSW_activity = (Object*)Label(GCS(catalog,353,"activity"));
    MBObj->PP_CSW_activity = (Object*)MUI_NewObject((char*)MUIC_Poppen,MUIA_Window_Title, GCS(catalog,360,"Choose Colour"),End;

    LA_CSW_chatactivity = (Object*)Label(GCS(catalog,354,"chatactivity"));
    MBObj->PP_CSW_chatactivity = (Object*)MUI_NewObject((char*)MUIC_Poppen,MUIA_Window_Title, GCS(catalog,360,"Choose Colour"),End;

    LA_CSW_highlightactivity = (Object*)Label(GCS(catalog,355,"highlightactivity"));
    MBObj->PP_CSW_highlightactivity = (Object*)MUI_NewObject((char*)MUIC_Poppen,MUIA_Window_Title, GCS(catalog,360,"Choose Colour"),End;

    LA_CSW_nicklisttext = (Object*)Label(GCS(catalog,356,"nicklisttext"));
    MBObj->PP_CSW_nicklisttext = (Object*)MUI_NewObject((char*)MUIC_Poppen,MUIA_Window_Title, GCS(catalog,360,"Choose Colour"),End;

    LA_CSW_listviewtabs_normaltext = (Object*)Label(GCS(catalog,358,"tabs pen"));
    MBObj->PP_CSW_listviewtabs_normaltext = (Object*)MUI_NewObject((char*)MUIC_Poppen,MUIA_Window_Title, GCS(catalog,360,"Choose Colour"),End;

if(AMIX) printf("107\n");
    MBObj->BT_colour_settings_load = (Object*)SimpleButton(GCS(catalog,323,"Load"));
    MBObj->BT_colour_settings_apply = (Object*)SimpleButton(GCS(catalog,324,"Apply"));
    MBObj->BT_colour_settings_save = (Object*)SimpleButton(GCS(catalog,325,"Save"));
    MBObj->BT_colour_settings_save_as = (Object*)SimpleButton(GCS(catalog,326,"Save As..."));


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
if(AMIX) printf("108\n");
    MBObj->WI_colour_settings = (Object*)WindowObject,
        MUIA_Window_Title, GCS(catalog,102,"Colour Settings"),
        MUIA_Window_ID, MAKE_ID('3', 'W', 'I', 'N'),
        WindowContents, GROUP_ROOT_6,
    End;
if(AMIX) printf("109\n");
    //below are menu items
    MN1_SelectServer = (Object*)MenuitemObject,
        MUIA_Menuitem_Title, GCS(catalog,103,"Select server"),
        MUIA_Menuitem_Shortcut, "S",
    End;

    MN1_CloseTAB = (Object*)MenuitemObject,
        MUIA_Menuitem_Title, GCS(catalog,104,"Close current tab"),
        MUIA_Menuitem_Shortcut, "Z",
    End;

    MNmenuBarLabel2 = (Object*)MUI_MakeObject(MUIO_Menuitem, NM_BARLABEL, 0, 0, 0);

    MN1_Hide = (Object*)MenuitemObject,
        MUIA_Menuitem_Title, GCS(catalog,293,"Hide"),
        MUIA_Menuitem_Shortcut, "H",
    End;

    MN1_NewTAB = (Object*)MenuitemObject,
        MUIA_Menuitem_Title, GCS(catalog,105,"New server tab"),
        MUIA_Menuitem_Shortcut, "T",
    End;

    MN1_NewGTAB = (Object*)MenuitemObject,
        MUIA_Menuitem_Title, GCS(catalog,106,"Open global query tab"),
    End;

    MNmenuBarLabel0 = (Object*)MUI_MakeObject(MUIO_Menuitem, NM_BARLABEL, 0, 0, 0);

    MNmenuBarLabel1 = (Object*)MUI_MakeObject(MUIO_Menuitem, NM_BARLABEL, 0, 0, 0);


    MN_about = (Object*)MenuitemObject,
        MUIA_Menuitem_Title, GCS(catalog,26,"About"),
    End;

    MN_quit = (Object*)MenuitemObject,
        MUIA_Menuitem_Title, GCS(catalog,107,"Quit"),
        MUIA_Menuitem_Shortcut, "Q",
    End;

    MN_ClearHistory = (Object*)MenuitemObject,
        MUIA_Menuitem_Title, GCS(catalog,108,"Clear buffer"),
    End;

    MN_ClearAllHistory = (Object*)MenuitemObject,
        MUIA_Menuitem_Title, GCS(catalog,334,"Clear all buffers"),
    End;

    MN_SaveHistory = (Object*)MenuitemObject,
        MUIA_Menuitem_Title, GCS(catalog,109,"Save buffer as"),
    End;

    MN1_Main = (Object*)MenuObject,
        MUIA_Menu_Title, GCS(catalog,110,"Main"),
        MUIA_Family_Child, MN1_SelectServer,
        MUIA_Family_Child, MN1_NewTAB,
        MUIA_Family_Child, MN1_NewGTAB,
        MUIA_Family_Child, MN1_CloseTAB,
        MUIA_Family_Child, MNmenuBarLabel0,
        MUIA_Family_Child, MN1_Hide,
        MUIA_Family_Child, MNmenuBarLabel2,
        MUIA_Family_Child, MN_ClearHistory,
        MUIA_Family_Child, MN_ClearAllHistory,
        MUIA_Family_Child, MN_SaveHistory,
        MUIA_Family_Child, MNmenuBarLabel1,
        MUIA_Family_Child, MN_about,
        MUIA_Family_Child, MN_quit,
    End;
if(AMIX) printf("110\n");
    MN_MainSettings  = (Object*)MenuitemObject,
        MUIA_Menuitem_Title, GCS(catalog,111,"Main Settings..."),
        //MUIA_Menuitem_Shortcut, "S",
    End;

    MN_ColourSettings = (Object*)MenuitemObject,
        MUIA_Menuitem_Title, GCS(catalog,102,"Colour Settings..."),
    End;

    MN_MUISettings = (Object*)MenuitemObject,
        MUIA_Menuitem_Title, GCS(catalog,112,"MUI Settings..."),
        //MUIA_Menuitem_Shortcut, "S",
    End;

    MN_MultiColumnDisplay  = (Object*)MenuitemObject,
        MUIA_Menuitem_Title, GCS(catalog,317,"Multi-column chat display"),
        MUIA_Menuitem_Checkit, TRUE,
        MUIA_Menuitem_Toggle, TRUE,
        //MUIA_Menuitem_Shortcut, "S",
    End;

    MN_Clipboard = (Object*)MenuitemObject,
        MUIA_Menuitem_Title, GCS(catalog,318,"Column-marking Clipboard"),
        MUIA_Menuitem_Checkit, TRUE,
        MUIA_Menuitem_Toggle, TRUE,
    End;


    MN_SaveSettings = (Object*)MenuitemObject,
        MUIA_Menuitem_Title, GCS(catalog,288,"Save settings"),
    End;


    MN_MuteSound = (Object*)MenuitemObject,
        MUIA_Menuitem_Title, GCS(catalog,372,"Mute all sound"),
        MUIA_Menuitem_Checkit, TRUE,
        MUIA_Menuitem_Toggle, TRUE,
    End;

    MN_Edit = (Object*)MenuObject,
        MUIA_Menu_Title, GCS(catalog,800,"Edit"),
        MUIA_Family_Child, MN_cut  = (Object*)MenuitemObject,MUIA_Menuitem_Title, GCS(catalog,800,"Cut"),MUIA_Menuitem_Shortcut, "X", End,
        MUIA_Family_Child, MN_copy = (Object*)MenuitemObject,MUIA_Menuitem_Title, GCS(catalog,800,"Copy"),MUIA_Menuitem_Shortcut, "C", End,
        MUIA_Family_Child, MN_paste = (Object*)MenuitemObject,MUIA_Menuitem_Title, GCS(catalog,800,"Paste"),MUIA_Menuitem_Shortcut, "V", End,
    End;

    MN_Settings = (Object*)MenuObject,
        MUIA_Menu_Title, GCS(catalog,101,"Settings"),
        MUIA_Family_Child, MN_MainSettings,
        MUIA_Family_Child, MN_ColourSettings,
        MUIA_Family_Child, MN_MUISettings,
        MUIA_Family_Child, MN_MultiColumnDisplay,
        MUIA_Family_Child, MN_Clipboard,
        MUIA_Family_Child, MN_MuteSound,
        MUIA_Family_Child, MN_SaveSettings,
    End;

    MN_windows_dcc = (Object*)MenuitemObject,
        MUIA_Menuitem_Title, GCS(catalog,62,"Incoming DCC File Transfers..."),
        MUIA_Menuitem_Shortcut, "D",
    End;

    MN_windows_dcc2 = (Object*)MenuitemObject,
        MUIA_Menuitem_Title, GCS(catalog,63,"Outgoing DCC File Transfers..."),
        MUIA_Menuitem_Shortcut, "R",
    End;

    MN_urlgrabber = (Object*)MenuitemObject,
        MUIA_Menuitem_Title, GCS(catalog,114,"URL Grabber..."),
        MUIA_Menuitem_Shortcut, "U",
    End;

    MN_ignorelist = (Object*)MenuitemObject,
        MUIA_Menuitem_Title, GCS(catalog,276,"Ignore List..."),
        MUIA_Menuitem_Shortcut, "I",
    End;

    MN_windows = (Object*)MenuObject,
        MUIA_Menu_Title, GCS(catalog,214,"Windows"),
        MUIA_Family_Child, MN_windows_dcc,
        MUIA_Family_Child, MN_windows_dcc2,
        MUIA_Family_Child, MN_urlgrabber,
        MUIA_Family_Child, MN_ignorelist,
    End;

    MBObj->MN_ = (Object*)MenustripObject,
        MUIA_Family_Child, MN1_Main,
        MUIA_Family_Child, MN_Edit,
        MUIA_Family_Child, MN_Settings,
        MUIA_Family_Child, MN_windows,
        //MUIA_Family_Child, MN1_AREXX,
    End;

    //below are edit window stuff

    LA_servername = (Object*)Label(GCS(catalog,115,"Servername"));

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

    LA_port = (Object*)Label(GCS(catalog,116,"Port number"));
if(AMIX) printf("111\n");
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

    MBObj->LA_autojoin = (Object*)Label(GCS(catalog,362,"Automatically join these channels"));

    MBObj->STR_autojoin = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_String_Reject," ",
        MUIA_String_MaxLen, 400,
    End;

    MBObj->LA_nick_registration_command = (Object*)Label(GCS(catalog,363,"Nick registration password"));

    MBObj->STR_nick_registration_command = (Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_ShortHelp, GCS(catalog,364,"This option will only work with a registered nick on a network with a Nickserv service"),
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

    LA_pass = (Object*)Label(GCS(catalog,117,"Server password"));

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

    MBObj->BT_acceptserver = (Object*)SimpleButton(GCS(catalog,365,"Accept Changes"));

    MBObj->BT_cancelserver = (Object*)SimpleButton(GCS(catalog,74,"Cancel"));

    GR_grp_12 = (Object*)GroupObject,
        MUIA_HelpNode, "GR_grp_12",
        MUIA_Group_Horiz, TRUE,
        Child, MBObj->BT_acceptserver,
        Child, MBObj->BT_cancelserver,
    End;

    MBObj->CH_autoconnect = (Object*)CheckMark(FALSE);
    MBObj->LA_autoconnect = (Object*)Label(GCS(catalog,277,"Automatically connect here when WookieChat is started"));

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

    MBObj->LA_remote_charset=(Object*)Label(GCS(catalog,327,"Server Charset"));

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
if(AMIX) printf("112\n");
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
if(AMIX) printf("113\n");
    GROUP_ROOT_4 = (Object*)GroupObject,
        MUIA_Group_Horiz,FALSE,
        MUIA_Group_Columns,2,
        Child, GROUP_ROOT_4_COLUMN_1,
        Child, GROUP_ROOT_4_COLUMN_2,
    End;

if(AMIX) printf("114\n");
    LA_groupname = (Object*)Label(GCS(catalog,119,"Group name"));

    MBObj->BT_perform = (Object*)SimpleButton(GCS(catalog,120,"Perform on connect"));

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

    MBObj->BT_acceptgroup =(Object*)SimpleButton(GCS(catalog,40,"Accept"));

    MBObj->BT_cancelgroup =(Object*)SimpleButton(GCS(catalog,74,"Cancel"));

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
if(AMIX) printf("115\n");
    MBObj->WI_edit_group =(Object*)WindowObject,
        MUIA_Window_Title, GCS(catalog,121,"Edit Group"),
        MUIA_Window_ID, MAKE_ID('3', 'W', 'I', 'N'),
        MUIA_Window_CloseGadget, FALSE,
        WindowContents, GROUP_ROOT_5,
    End;

// above is edit window stuff

    LA_nicknames =(Object*)Label(GCS(catalog,216,"Nick Names"));

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
if(AMIX) printf("116\n");
    GR_grp_0 =(Object*)GroupObject,
        MUIA_HelpNode, "GR_grp_0",
        MUIA_Group_Horiz, TRUE,
        Child, LA_nicknames,
        Child, MBObj->STR_nickname1,
        Child, MBObj->STR_nickname2,
        Child, MBObj->STR_nickname3,
    End;

    LA_realname =(Object*)Label(GCS(catalog,122,"Real Name"));

    MBObj->STR_realname =(Object*)StringObject,
        MUIA_Frame, MUIV_Frame_String,
        MUIA_HelpNode, "STR_realname",
    End;




    MBObj->LA_display_server_win=(Object*)Label(GCS(catalog,278,"Display Server Window upon startup:"));
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
if(AMIX) printf("117\n");
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
if(AMIX) printf("118\n");

    LA_username =(Object*)Label(GCS(catalog,123,"User Name"));

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
if(AMIX) printf("119\n");
    if(!MBObj->NLT_Servers) cleanexit((char*)GCS(catalog,19,"Please download and install the NListtree and NList MUI Classes from aminet\n"));
if(AMIX) printf("120\n");
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

    MBObj->BT_addserver =(Object*)SimpleButton(GCS(catalog,124,"New Server"));

    MBObj->BT_addgroup =(Object*)SimpleButton(GCS(catalog,125,"New Group"));

    MBObj->BT_delete_server =(Object*)SimpleButton(GCS(catalog,127,"Delete Entry"));

    MBObj->BT_edit =(Object*)SimpleButton(GCS(catalog,126,"Edit Entry"));

    GR_grp_9 =(Object*)GroupObject,
        MUIA_HelpNode, "GR_grp_9",
        MUIA_Group_Horiz, TRUE,
        Child, MBObj->BT_addserver,
        Child, MBObj->BT_addgroup,
        Child, MBObj->BT_delete_server,
        //Child, MBObj->BT_edit,
        //Child, MBObj->BT_acceptserver,
    End;

    MBObj->BT_connect =(Object*)SimpleButton(GCS(catalog,128,"Connect to server"));

    MBObj->BT_connect_tab =(Object*)SimpleButton(GCS(catalog,129,"Connect in new tab"));

    GR_grp_6 =(Object*)GroupObject,
        MUIA_HelpNode, "GR_grp_6",
        MUIA_Group_Horiz, TRUE,
        Child, MBObj->BT_connect,
        Child, MBObj->BT_connect_tab,
        Child, MBObj->BT_perform,
    End;

if(AMIX) printf("123\n");

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
                    Child, (Object*)Label(GCS(catalog,216,"Nick Names")),
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
                    Child, (Object*)Label(GCS(catalog,123,"User Name")),
                    Child, MBObj->STR_server_username =(Object*)StringObject,
                        MUIA_Frame, MUIV_Frame_String,
                        MUIA_String_Reject," ",
                        MUIA_HelpNode, "STR_username",
                    End,
                    Child, (Object*)Label(GCS(catalog,122,"Real Name")),
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

if(AMIX) printf("124\n");
    MBObj->WI_label_2 =(Object*)WindowObject,
        MUIA_Window_Title, GCS(catalog,130,"Select a server"),
        MUIA_Window_ID, MAKE_ID('1', 'W', 'I', 'N'),
        WindowContents, GROUP_ROOT_3,
    End;
if(AMIX) printf("125\n");
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
if(AMIX) printf("126\n");

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

if(AMIX) printf("127\n");


    if(!my_settings.use_nlistobject_for_tabs)
    {
        if(AMIX) printf("127a\n");
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
        if(AMIX) printf("127b\n");
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
if(AMIX) printf("128\n");

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

if(AMIX) printf("129\n");
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
if(AMIX) printf("130\n");
    MBObj->LV_ban = (Object*)NListviewObject,
        MUIA_HelpNode, "LV_channel",
        MUIA_Weight, 100,
        MUIA_NListview_NList, MBObj->LV_ban,
    End;

    MBObj->BT_unban = (Object*)SimpleButton(GCS(catalog,131,"Unban"));
    MBObj->BT_refreshban = (Object*)SimpleButton(GCS(catalog,132,"Refresh List"));

    GR_ban_subgroup = (Object*)GroupObject,
        MUIA_Group_Horiz, TRUE,
        Child, MBObj->BT_unban,
        Child, MBObj->BT_refreshban,
    End;

    GR_ban = (Object*)GroupObject,
        Child, MBObj->LV_ban,
        Child, GR_ban_subgroup,
    End;
if(AMIX) printf("131\n");
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

//if(AMIX) printf("132\n");
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
        SubWindow, MBObj->WI_colour_settings,
        SubWindow, MBObj->WI_mainsettings,
        SubWindow, MBObj->WI_dcc,  //incoming file transfers
        SubWindow, MBObj->WI_dcc2, //outgoing file transfers
        SubWindow, MBObj->WI_dcc_file_exists,
        SubWindow, MBObj->WI_dcc_send,
        SubWindow, MBObj->WI_about,
        SubWindow, MBObj->WI_quit,
        SubWindow, MBObj->WI_urlgrabber,
        SubWindow, MBObj->WI_ban,
        SubWindow, MBObj->WI_ignore,
        SubWindow, MBObj->WI_addignore,
        //SubWindow, MBObj->WI_edit_alias,
        SubWindow, MBObj->WI_graphical_smileys_preview,
        SubWindow, MBObj->WI_graphical_smileys_choose,
        SubWindow, MBObj->WI_change_nick,
    End;


    MBObj->smiley_choose_icon=NULL;
    //if(AMIX) printf("133\n");
    if (!MBObj->App)
    {
        //if(AMIX) printf("134.. THE END..\n");
        FreeVec(MBObj);
        printf("%s",GCS(catalog,133,"unable to create app\n"));
        return(NULL);
    }

//if(AMIX) printf(" THE END.. \n");
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
        printf("%s\n", GCS(catalog, 137, "Please download and install the BetterString.mcc MUI Class"));

    classes[CC_MUIC_NLIST] = MUI_CreateCustomClass(NULL, (ClassID)MUIC_NList, NULL, sizeof(struct MyData), (APTR) NList_Dispatcher);
    if (!classes[CC_MUIC_NLIST])
        printf("%s\n", GCS(catalog, 138, "Please download and install the NList.mcc MUI Class"));

    classes[CC_MUIC_WINDOW] = MUI_CreateCustomClass(NULL, (ClassID)MUIC_Window, NULL, sizeof(struct MyData), (APTR) Window_Dispatcher);
    if (!classes[CC_MUIC_WINDOW])
        printf("Could not create custom class for windows\n");

    classes[CC_MUIC_GROUP] = MUI_CreateCustomClass(NULL, (ClassID)MUIC_Group, NULL, sizeof(struct MyData), (APTR) Group_Dispatcher);
    if (!classes[CC_MUIC_GROUP])
        printf("Could not create custom class for groups\n");

#else
    classes[CC_MUIC_BETTERSTRING] = MUI_CreateCustomClass(NULL, (ClassID)MUIC_BetterString,NULL,sizeof(struct InstanceData),ENTRY(BetterString_Dispatcher));
    if(!classes[CC_MUIC_BETTERSTRING]) printf("%s\n",GCS(catalog,137,"Please download and install the BetterString.mcc MUI Class"));

    classes[CC_MUIC_NLIST] = MUI_CreateCustomClass(NULL, (ClassID)MUIC_NList,NULL,sizeof(struct MyData),ENTRY(NList_Dispatcher));
    if(!classes[CC_MUIC_NLIST]) printf("%s\n",GCS(catalog,138,"Please download and install the NList.mcc MUI Class"));

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
