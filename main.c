/*
    The contents of this file are subject to the AROS Public License Version 1.1 
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis, 
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
    for the specific language governing rights and limitations under the License.
*/


#define USE_F 0
#define SECONDS_TO_WAIT_BEFORE_PING_SERVER 60*2
#define DONT_CONNECT_IN_CURRENT_TAB 0


#ifdef __amigaos4__
#define DEPRECATED
#endif

#include "includes.h"

#include <clib/alib_protos.h>
#include <proto/asl.h>
#include <proto/codesets.h>
#include <proto/icon.h>
#include <proto/muimaster.h>

#include <mui/NListtree_mcc.h>
#include <mui/BetterString_mcc.h>

#include "functions.h"
#include "muiclass.h"
#include "muiclass_application.h"
#include "muiclass_network.h"
#include "version.h"
#include "intern.h"
#include "objapp.h"
#include "audio.h"
#include "locale.h"

/* Global flags */
BOOL NEWDEBUG               = FALSE;
BOOL DEBUG                  = FALSE;
BOOL SMALLTABS              = FALSE;
BOOL RAW                    = FALSE;
BOOL ALTERNATIVE_CLIPBOARD  = FALSE;
BOOL GEIT                   = FALSE;
BOOL GEIT2                  = FALSE;
BOOL GEIT3                  = FALSE;
BOOL RECENTLY_CREATED_A_TAB = FALSE;
BOOL ZUNE_SYSTEM            = FALSE;
BOOL QUIET_DCC              = FALSE;
BOOL USE_AREXX              = FALSE;
BOOL USING_A_PROXY          = FALSE;
BOOL PRO_CHARSETS_ENABLED   = FALSE;

BOOL start_reconnect_delay_timer;

/* Locals */
static int delay_b4_ping_server_count=0;
static char ban_window_title[200];
static BOOL aslresult;
static struct FileRequester *filerequester;
static char old_alias_entry[800];
static char new_filename[1000];
static char orig_filename[1000];
static char filename[1000];
static char wookie_dir[400]; //the pathname wookiechat is located in
static ULONG iconified_and_new_text;
static char string11[900];
static char string8[900];
static char string9[900];
static char string7[900];
static char file_name[800];
static BOOL is_chooser_window_open = FALSE;
static char sendstuff[1500];
static char group_name[100];
static char *text;
static char buffer3[BUFFERSIZE*2];
static char *string1;
static char *string4;
static char *string5;
static char *string2;
static char *string3;
static struct Settings temp_settings;

/* /// OLD MAIN ->>>
*/

/*************************************************************************/

int main(int argc, char *argv[])
{

	SysBase = (APTR) (*((struct ExecBase **) 4 ) );


    //SignalCheck_Init();

    LoadAllLibs();

#if ENABLE_NEWWOOKIECODE
	WBMessage_Get();
#else
    if (argc == 0)
    {
        wbmessage = (struct WBStartup *) argv;
        int i = 0;

        for (i = 0, wbarg = wbmessage->sm_ArgList; i < wbmessage->sm_NumArgs; i++, wbarg++)
        {
            if (!wbarg)
                break;
            if (!ParseToolTypes(wbarg))
                break;

        }
    }
#endif

    if (argc >= 2)
    {
        for (int count = 1; count < argc; count++)
        {
            if (!stricmp(argv[count], "geit"))
                GEIT = TRUE;
            if (!stricmp(argv[count], "geit2"))
                GEIT2 = TRUE;
            if (!stricmp(argv[count], "geit3"))
                GEIT3 = TRUE;
            if (!stricmp(argv[count], "smalltabs"))
                SMALLTABS = TRUE;
            if (!stricmp(argv[count], "debug"))
                DEBUG = TRUE;
            if (!stricmp(argv[count], "raw"))
                RAW = TRUE;
            if (!stricmp(argv[count], "proxy"))
                USING_A_PROXY = TRUE;
            if (!stricmp(argv[count], "nozune"))
                ZUNE_SYSTEM = FALSE;
            if (!stricmp(argv[count], "zune"))
                ZUNE_SYSTEM = TRUE;
            if (!stricmp(argv[count], "altclipboard"))
                ALTERNATIVE_CLIPBOARD = TRUE;
            if (!stricmp(argv[count], "aboutgfx"))
                my_settings.os3_about_window_gfx = 1;
            if (!stricmp(argv[count], "arexx"))
                USE_AREXX = TRUE;
        }
    }

    struct Window *my_window;

// Setup some MsgPort's if we're going to use AREXX
    arexx_setup_messageports();
//

    NameFromLock(GetProgramDir(), (STRPTR) wookie_folder, 300);
    if (DEBUG)
        printf("%s\r\n", wookie_folder);

    if (wookie_folder[strlen(wookie_folder) - 1] == ':' || wookie_folder[strlen(wookie_folder) - 1] == '/')
        sprintf(wookie_dir, "%s", wookie_folder);
    else
        sprintf(wookie_dir, "%s/", wookie_folder);

// Set AppPaths ENV variable for AmiUpdate for OS4 compiled Wookies
#ifdef __amigaos4__
    urlgrabber_file = Open((_s_cs)"Envarc:AppPaths/WookieChat", MODE_NEWFILE);
    if(urlgrabber_file)
    {
        FPuts(urlgrabber_file,(l_in)wookie_folder);
        Close(urlgrabber_file);
    }
    urlgrabber_file=0;

    urlgrabber_file = Open((_s_cs)"Env:AppPaths/WookieChat", MODE_NEWFILE);
    if(urlgrabber_file)
    {
        FPuts(urlgrabber_file,(l_in)wookie_folder);
        Close(urlgrabber_file);
    }
    urlgrabber_file=0; //NULL;
#endif
//

// Create progdir:logs/ if it doesnt exist
    if (wookie_folder[strlen(wookie_folder) - 1] == ':' || wookie_folder[strlen(wookie_folder) - 1] == '/')
        sprintf(file_name, "%slogs", wookie_folder);
    else
        sprintf(file_name, "%s/logs", wookie_folder);
    BPTR does_dir_exist = Open((_s_cs)file_name, MODE_OLDFILE);
    if (!does_dir_exist)
    {
        BPTR created_dir_lock = CreateDir((_s_cs)file_name);
        UnLock(created_dir_lock);
    }
    else
    {
        Close(does_dir_exist);
    }
//

// Create Timers
    if (!TimerWait_Open())
        cleanexit((char*) "cant create timers\n");

    if (!create_custom_classes())
		cleanexit((char*) LGS( MSG_ERROR_MISSING ) );
//

    WookieChat = CreateApp();
    if (!WookieChat)
		cleanexit((char*) LGS( MSG_UNABLE_TO_CREATE_APP ) );

    arexx_add_scripts_to_menu();

// Create AREXX Process
    arexx_create_process();
//

// Loading settings for colours, nicks, graphical smileys, and create our initial Server tab
    load_settings();

    load_colours_choice();
    load_colours(my_settings.default_colours_theme);

    status_current = NULL;
    current_query = NULL;

    status_conductor = NULL;

    load_graphical_smilies();

    create_new_status(1);

    //strcpy(status_conductor->nick,"(nick)");

    //status_conductor->your_current_mode[0]==' ';
    //setmacro((Object*)WookieChat->TX_nickname,MUIA_Text_Contents,status_conductor->nick_string);

    //load_graphical_smilies();

    set_settings();

    retrieve_settings();
    set_settings();

    load_user_list_buttons_config();
    create_user_list_buttons();

    load_events_settings();

    //use_graphical_smilies(status_current->current_query);

    load_nick_settings();
    update_your_nickname_text_label();

    //setmacro((Object*)WookieChat->TX_nickname,MUIA_Text_Contents,status_conductor->nick);

#ifdef __amigaos4__
    struct MUI_EventHandlerNode ehnode;
    ehnode.ehn_Object = (Object*)WookieChat->WI_main;
    ehnode.ehn_Class = mcc4->mcc_Class;
    ehnode.ehn_Events = IDCMP_MOUSEBUTTONS;
    DoMethod((Object*)WookieChat->WI_main,MUIM_Window_AddEventHandler,&ehnode);
#endif

    DoMethod((Object*) WookieChat->WI_main, MUIM_Notify, MUIA_Window_Activate, TRUE, WookieChat->WI_main, 3, MUIM_Set,
            MUIA_Window_ActiveObject, (Object*) WookieChat->STR_usertext);
    //DoMethod((Object*)WookieChat->WI_main,MUIM_Notify,MUIA_Selected,TRUE,WookieChat->WI_main,3,MUIM_Set,MUIA_Window_ActiveObject,(Object*)WookieChat->STR_usertext);

    iconified_and_new_text = 1;

#ifndef __AROS__
    DoMethod((Object*)WookieChat->App,MUIM_Notify,MUIA_Application_Iconified, TRUE, WookieChat->App, 3, MUIM_WriteLong, 0, &iconified_and_new_text);
    DoMethod((Object*)WookieChat->App,MUIM_Notify,MUIA_Application_Iconified, FALSE, WookieChat->App,3, MUIM_WriteLong, 1, &iconified_and_new_text);
    DoMethod((Object*)WookieChat->App,MUIM_Notify,MUIA_Application_Iconified, FALSE, WookieChat->App,3, MUIM_Set, MUIA_Application_IconifyTitle, "Wookiechat");
#endif

    setmacro((Object*)WookieChat->WI_main, MUIA_Window_ActiveObject, (Object*)WookieChat->STR_usertext);

    setmacro((Object*)WookieChat->WI_main, MUIA_Window_DefaultObject, (Object*)WookieChat->STR_usertext);

    //disable TAB so we can use it for nick completion
    setmacro((Object*)WookieChat->WI_main, MUIA_Window_DisableKeys, (1<<MUIKEY_GADGET_NEXT) | (1<<MUIKEY_GADGET_PREV));


    if (status_conductor->conductor->LV_nicklist)
        setmacro((Object*)status_conductor->conductor->LV_nicklist, MUIA_NList_ListBackground,
                popimage_nicklistbackground);
    setmacro((Object*)status_conductor->conductor->LV_channel, MUIA_NList_ListBackground, popimage_background);

// Set GUI cycle chains for when using the TAB key
    DoMethod((Object *) WookieChat->WI_mainsettings, MUIM_Window_SetCycleChain, WookieChat->SETTINGS_Register_Grp,
            WookieChat->WI_mainsettings, WookieChat->CH_autojoin_channels_when_kicked,
            WookieChat->CH_auto_open_query_tabs_when_msged, WookieChat->STR_label_3, WookieChat->CH_label_2,
            WookieChat->CH_label_3, WookieChat->STR_label_1, WookieChat->STR_label_2, WookieChat->STR_label_4,
            WookieChat->STR_label_5, WookieChat->CH_label_4, WookieChat->CH_label_5, WookieChat->STR_label_0,
            WookieChat->CH_label_0, WookieChat->STR_PA_label_0, WookieChat->BT_label_0, WookieChat->BT_label_1,
            WookieChat->BT_label_3, WookieChat->BT_label_2, WookieChat->STR_highlight_label,
            WookieChat->STR_how_many_lines_to_reload_channel, WookieChat->STR_how_many_lines_to_reload_private,
            WookieChat->STR_browser, WookieChat->RA_nick_completion_style, WookieChat->CH_autoaccept,
            WookieChat->LV_trusted_nicks, WookieChat->RA_autoaccept, WookieChat->CH_gethostid,
            WookieChat->STR_port1_dcc, WookieChat->STR_port2_dcc, WookieChat->STR_sound_sample_tabopen,
            WookieChat->RA_sound_sample_tabopen, WookieChat->STR_sound_sample_highlight,
            WookieChat->RA_sound_sample_highlight, WookieChat->STR_samples_path, WookieChat->LV_alias,
            WookieChat->BT_add_alias, WookieChat->BT_remove_alias,
            //WookieChat->STR_add_alias,
            NULL);

    DoMethod((Object *) WookieChat->WI_label_2, MUIM_Window_SetCycleChain, WookieChat->STR_nickname1,
            WookieChat->STR_nickname2, WookieChat->STR_nickname3, WookieChat->STR_realname, WookieChat->STR_username,
            WookieChat->BT_addserver, WookieChat->BT_addgroup, WookieChat->BT_delete_server,
            //WookieChat->BT_edit,
            WookieChat->BT_connect, WookieChat->BT_connect_tab, WookieChat->BT_perform, WookieChat->NLT_Servers, NULL);
//

// Setup notifications for all our GUI objects
    //DoMethod((Object*)PICTURE_LOGO,MUIM_Notify,MUIA_Pressed,TRUE,(Object*)WookieChat->WI_colour_settings,3,MUIM_Set,MUIA_Window_Open,TRUE);

    DoMethod((Object *) WookieChat->LV_tabs, MUIM_Notify, MUIA_NList_EntryClick, MUIV_EveryTime,
            (Object*) WookieChat->App, 2, MUIM_Application_ReturnID, 92);

    DoMethod((Object *) WookieChat->NLT_Servers, MUIM_Notify, MUIA_NList_DoubleClick, TRUE, (Object*) WookieChat->App,
            2, MUIM_Application_ReturnID, 49);
    DoMethod((Object *) WookieChat->NLT_Servers, MUIM_Notify, MUIA_NListtree_Active, MUIV_EveryTime,
            (Object*) WookieChat->App, 2, MUIM_Application_ReturnID, 53);
    DoMethod((Object*) WookieChat->WI_mainsettings, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
            (Object*) WookieChat->App, 2, MUIM_Application_ReturnID, 7);
	DoMethod((Object*) WookieChat->WI_main, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, (Object*) WookieChat->WI_quit,
			3, MUIM_Set, MUIA_Window_Open, TRUE );

    DoMethod((Object *) MN_MultiColumnDisplay, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
            (Object*) WookieChat->App, 2, MUIM_Application_ReturnID, 97);
    DoMethod((Object *) MN_Clipboard, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime, (Object*) WookieChat->App, 2,
            MUIM_Application_ReturnID, 94);
    DoMethod((Object *) MN_SaveSettings, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime, (Object*) WookieChat->App,
            2, MUIM_Application_ReturnID, 5);

    DoMethod((Object *) MN_MuteSound, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime, (Object*) WookieChat->App, 2,
            MUIM_Application_ReturnID, SECOND_SET_OF_RETURNIDS + 1);
    DoMethod((Object*) WookieChat->WI_label_2, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, (Object*) WookieChat->App,
            2, MUIM_Application_ReturnID, 2);

    DoMethod((Object*) WookieChat->WI_colour_settings, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
            (Object*) WookieChat->WI_colour_settings, 3, MUIM_Set, MUIA_Window_Open, FALSE);
    DoMethod((Object*) MN_ColourSettings, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
            (Object*) WookieChat->WI_colour_settings, 3, MUIM_Set, MUIA_Window_Open, TRUE);
    DoMethod((Object*) WookieChat->BT_colour_settings_load, MUIM_Notify, MUIA_Pressed, FALSE, (Object*) WookieChat->App,
            2, MUIM_Application_ReturnID, 4);
    DoMethod((Object*) WookieChat->BT_colour_settings_save, MUIM_Notify, MUIA_Pressed, FALSE, (Object*) WookieChat->App,
            2, MUIM_Application_ReturnID, 3);
    DoMethod((Object*) WookieChat->BT_colour_settings_save_as, MUIM_Notify, MUIA_Pressed, FALSE,
            (Object*) WookieChat->App, 2, MUIM_Application_ReturnID, 93);
    DoMethod((Object*) WookieChat->BT_colour_settings_apply, MUIM_Notify, MUIA_Pressed, FALSE,
            (Object*) WookieChat->App, 2, MUIM_Application_ReturnID, 30);

    DoMethod((Object*) WookieChat->BT_graphical_smileys_preview, MUIM_Notify, MUIA_Pressed, FALSE,
            (Object*) WookieChat->App, 2, MUIM_Application_ReturnID, 99);
    DoMethod((Object*) WookieChat->WI_graphical_smileys_preview, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
            (Object*) WookieChat->WI_graphical_smileys_preview, 3, MUIM_Set, MUIA_Window_Open, FALSE);
    DoMethod((Object*) WookieChat->WI_graphical_smileys_choose, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
            (Object*) WookieChat->App, 2, MUIM_Application_ReturnID, 95);

    DoMethod((Object*) WookieChat->WI_dcc, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, (Object*) WookieChat->App, 2,
            MUIM_Application_ReturnID, 68);
    DoMethod((Object*) WookieChat->WI_dcc2, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, (Object*) WookieChat->WI_dcc2,
            3, MUIM_Set, MUIA_Window_Open, FALSE);

    DoMethod((Object*) WookieChat->STR_usertext, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
            (Object*) WookieChat->App, 2, MUIM_Application_ReturnID, 1);

    DoMethod((Object*) WookieChat->BT_addserver, MUIM_Notify, MUIA_Pressed, FALSE, (Object*) WookieChat->App, 2,
            MUIM_Application_ReturnID, 50);
    DoMethod((Object*) WookieChat->BT_addgroup, MUIM_Notify, MUIA_Selected, FALSE, (Object*) WookieChat->App, 2,
            MUIM_Application_ReturnID, 51);
    DoMethod((Object*) WookieChat->BT_delete_server, MUIM_Notify, MUIA_Pressed, FALSE, (Object*) WookieChat->App, 2,
            MUIM_Application_ReturnID, 52);
    DoMethod((Object*) WookieChat->BT_cancelgroup, MUIM_Notify, MUIA_Pressed, FALSE, (Object*) WookieChat->App, 2,
            MUIM_Application_ReturnID, 54);
    DoMethod((Object*) WookieChat->BT_acceptgroup, MUIM_Notify, MUIA_Pressed, FALSE, (Object*) WookieChat->App, 2,
            MUIM_Application_ReturnID, 55);
    DoMethod((Object*) WookieChat->BT_perform, MUIM_Notify, MUIA_Pressed, FALSE, (Object*) WookieChat->App, 2,
            MUIM_Application_ReturnID, 64);
    DoMethod((Object*) WookieChat->BT_cancelserver, MUIM_Notify, MUIA_Pressed, FALSE, (Object*) WookieChat->App, 2,
            MUIM_Application_ReturnID, 56);
    //DoMethod((Object*)WookieChat->BT_acceptserver,MUIM_Notify,MUIA_Pressed,FALSE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,57);
    DoMethod((Object*) WookieChat->BT_connect, MUIM_Notify, MUIA_Pressed, FALSE, (Object*) WookieChat->App, 2,
            MUIM_Application_ReturnID, 58);
    DoMethod((Object*) WookieChat->BT_connect_tab, MUIM_Notify, MUIA_Pressed, FALSE, (Object*) WookieChat->App, 2,
            MUIM_Application_ReturnID, 59);

    DoMethod((Object*) WookieChat->BT_add_alias, MUIM_Notify, MUIA_Pressed, FALSE, (Object*) WookieChat->App, 2,
            MUIM_Application_ReturnID, 81);
    DoMethod((Object*) WookieChat->BT_remove_alias, MUIM_Notify, MUIA_Pressed, FALSE, (Object*) WookieChat->App, 2,
            MUIM_Application_ReturnID, 82);
    //DoMethod((Object*)WookieChat->BT_edit_alias,MUIM_Notify,MUIA_Pressed,FALSE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,89);
    //DoMethod((Object*)WookieChat->BT_edit_alias_accept,MUIM_Notify,MUIA_Pressed,FALSE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,91);
    //DoMethod((Object*)WookieChat->BT_edit_alias_cancel,MUIM_Notify,MUIA_Pressed,FALSE,(Object*)WookieChat->WI_edit_alias,3,MUIM_Set,MUIA_Window_Open,FALSE);

    DoMethod((Object*) WookieChat->STR_edit_alias_name, MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
            (Object*) WookieChat->App, 2, MUIM_Application_ReturnID, 91);
    DoMethod((Object*) WookieChat->STR_edit_alias_command, MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
            (Object*) WookieChat->App, 2, MUIM_Application_ReturnID, 91);

    //edit server details in the Server Selection window
    DoMethod((Object*) WookieChat->CH_autoconnect, MUIM_Notify, MUIA_Selected, TRUE, (Object*) WookieChat->App, 2,
            MUIM_Application_ReturnID, 57);
    DoMethod((Object*) WookieChat->CH_autoconnect, MUIM_Notify, MUIA_Selected, FALSE, (Object*) WookieChat->App, 2,
            MUIM_Application_ReturnID, 57);

    DoMethod((Object*) WookieChat->CH_use_global, MUIM_Notify, MUIA_Selected, TRUE, (Object*) WookieChat->App, 2,
            MUIM_Application_ReturnID, 57);
    DoMethod((Object*) WookieChat->CH_use_global, MUIM_Notify, MUIA_Selected, FALSE, (Object*) WookieChat->App, 2,
            MUIM_Application_ReturnID, 57);

    if (ZUNE_SYSTEM == TRUE)
    {
        DoMethod((Object*) WookieChat->STR_servername, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
                (Object*) WookieChat->App, 2, MUIM_Application_ReturnID, 57);
        DoMethod((Object*) WookieChat->STR_port, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
                (Object*) WookieChat->App, 2, MUIM_Application_ReturnID, 57);
        DoMethod((Object*) WookieChat->STR_pass, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
                (Object*) WookieChat->App, 2, MUIM_Application_ReturnID, 57);
        DoMethod((Object*) WookieChat->STR_autojoin, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
                (Object*) WookieChat->App, 2, MUIM_Application_ReturnID, 57);
        DoMethod((Object*) WookieChat->STR_nick_registration_command, MUIM_Notify, MUIA_String_Acknowledge,
                MUIV_EveryTime, (Object*) WookieChat->App, 2, MUIM_Application_ReturnID, 57);
        DoMethod((Object*) WookieChat->STR_server_realname, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
                (Object*) WookieChat->App, 2, MUIM_Application_ReturnID, 57);
        DoMethod((Object*) WookieChat->STR_server_username, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
                (Object*) WookieChat->App, 2, MUIM_Application_ReturnID, 57);
        DoMethod((Object*) WookieChat->STR_server_nickname1, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
                (Object*) WookieChat->App, 2, MUIM_Application_ReturnID, 57);
        DoMethod((Object*) WookieChat->STR_server_nickname2, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
                (Object*) WookieChat->App, 2, MUIM_Application_ReturnID, 57);
        DoMethod((Object*) WookieChat->STR_server_nickname3, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
                (Object*) WookieChat->App, 2, MUIM_Application_ReturnID, 57);
    }
    else
    {
        DoMethod((Object*) WookieChat->STR_servername, MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
                (Object*) WookieChat->App, 2, MUIM_Application_ReturnID, 57);
        DoMethod((Object*) WookieChat->STR_port, MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
                (Object*) WookieChat->App, 2, MUIM_Application_ReturnID, 57);
        DoMethod((Object*) WookieChat->STR_pass, MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
                (Object*) WookieChat->App, 2, MUIM_Application_ReturnID, 57);
        DoMethod((Object*) WookieChat->STR_autojoin, MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
                (Object*) WookieChat->App, 2, MUIM_Application_ReturnID, 57);
        DoMethod((Object*) WookieChat->STR_nick_registration_command, MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
                (Object*) WookieChat->App, 2, MUIM_Application_ReturnID, 57);
        DoMethod((Object*) WookieChat->STR_server_realname, MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
                (Object*) WookieChat->App, 2, MUIM_Application_ReturnID, 57);
        DoMethod((Object*) WookieChat->STR_server_username, MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
                (Object*) WookieChat->App, 2, MUIM_Application_ReturnID, 57);
        DoMethod((Object*) WookieChat->STR_server_nickname1, MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
                (Object*) WookieChat->App, 2, MUIM_Application_ReturnID, 57);
        DoMethod((Object*) WookieChat->STR_server_nickname2, MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
                (Object*) WookieChat->App, 2, MUIM_Application_ReturnID, 57);
        DoMethod((Object*) WookieChat->STR_server_nickname3, MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
                (Object*) WookieChat->App, 2, MUIM_Application_ReturnID, 57);

    }

    DoMethod((Object*) WookieChat->CYCLE_remote_charset, MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime,
            (Object*) WookieChat->App, 2, MUIM_Application_ReturnID, 57);
    if (!PRO_CHARSETS_ENABLED)
    {
        setmacro(WookieChat->GR_charsets, MUIA_ShowMe, FALSE);
        setmacro(GR_local_charset, MUIA_ShowMe, FALSE);
    }

    //DoMethod((Object*)WookieChat->STR_user_list_buttons_name,MUIM_Notify,MUIA_String_Contents, MUIV_EveryTime,(Object*)WookieChat->App, 2, MUIM_Application_ReturnID,SECOND_SET_OF_RETURNIDS+2);
    //DoMethod((Object*)WookieChat->STR_user_list_buttons_command,MUIM_Notify,MUIA_String_Contents, MUIV_EveryTime,(Object*)WookieChat->App, 2, MUIM_Application_ReturnID,SECOND_SET_OF_RETURNIDS+2);
    DoMethod((Object*) WookieChat->STR_user_list_buttons_name, MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
            (Object*) WookieChat->App, 2, MUIM_Application_ReturnID, SECOND_SET_OF_RETURNIDS + 2);
    DoMethod((Object*) WookieChat->STR_user_list_buttons_command, MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
            (Object*) WookieChat->App, 2, MUIM_Application_ReturnID, SECOND_SET_OF_RETURNIDS + 2);
    DoMethod((Object*) WookieChat->LV_user_list_buttons, MUIM_Notify, MUIA_NList_EntryClick, MUIV_EveryTime,
            (Object*) WookieChat->App, 2, MUIM_Application_ReturnID, SECOND_SET_OF_RETURNIDS + 3);
    DoMethod((Object*) WookieChat->BT_user_list_buttons_add, MUIM_Notify, MUIA_Pressed, FALSE,
            (Object*) WookieChat->App, 2, MUIM_Application_ReturnID, SECOND_SET_OF_RETURNIDS + 4);
    DoMethod((Object*) WookieChat->BT_user_list_buttons_delete, MUIM_Notify, MUIA_Pressed, FALSE,
            (Object*) WookieChat->App, 2, MUIM_Application_ReturnID, SECOND_SET_OF_RETURNIDS + 5);

    DoMethod((Object*) WookieChat->BT_user_list_buttons_move_up, MUIM_Notify, MUIA_Pressed, FALSE,
            (Object*) WookieChat->App, 2, MUIM_Application_ReturnID, SECOND_SET_OF_RETURNIDS + 6);
    DoMethod((Object*) WookieChat->BT_user_list_buttons_move_down, MUIM_Notify, MUIA_Pressed, FALSE,
            (Object*) WookieChat->App, 2, MUIM_Application_ReturnID, SECOND_SET_OF_RETURNIDS + 7);

    DoMethod((Object*) LV_events, MUIM_Notify, MUIA_NList_EntryClick, MUIV_EveryTime, (Object*) WookieChat->App, 2,
            MUIM_Application_ReturnID, SECOND_SET_OF_RETURNIDS + 9);
    DoMethod((Object*) STR_events_script, MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime, (Object*) WookieChat->App,
            2, MUIM_Application_ReturnID, SECOND_SET_OF_RETURNIDS + 10);
    DoMethod((Object*) STR_events_args, MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime, (Object*) WookieChat->App, 2,
            MUIM_Application_ReturnID, SECOND_SET_OF_RETURNIDS + 10);
    DoMethod((Object*) CYCLE_events, MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime, (Object*) WookieChat->App, 2,
            MUIM_Application_ReturnID, SECOND_SET_OF_RETURNIDS + 10);

    DoMethod((Object*) WookieChat->TX_nickname, MUIM_Notify, MUIA_Pressed, FALSE, (Object*) WookieChat->WI_change_nick,
            3, MUIM_Set, MUIA_Window_Open, TRUE);
    DoMethod((Object*) WookieChat->TX_nickname, MUIM_Notify, MUIA_Pressed, FALSE, (Object*) WookieChat->WI_change_nick,
            3, MUIM_Set, MUIA_Window_ActiveObject, (Object*) WookieChat->STR_change_nick);

    DoMethod((Object*) WookieChat->BT_change_nick_cancel, MUIM_Notify, MUIA_Pressed, FALSE,
            (Object*) WookieChat->WI_change_nick, 3, MUIM_Set, MUIA_Window_Open, FALSE);
    DoMethod((Object*) WookieChat->BT_change_nick_change, MUIM_Notify, MUIA_Pressed, FALSE, (Object*) WookieChat->App,
            2, MUIM_Application_ReturnID, SECOND_SET_OF_RETURNIDS + 8);
    DoMethod((Object*) WookieChat->STR_change_nick, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
            (Object*) WookieChat->App, 2, MUIM_Application_ReturnID, SECOND_SET_OF_RETURNIDS + 8);
    DoMethod((Object*) WookieChat->WI_change_nick, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
            (Object*) WookieChat->WI_change_nick, 3, MUIM_Set, MUIA_Window_Open, FALSE);
    //setmacro((Object*)WookieChat->WI_change_nick,MUIA_Window_ActiveObject,(Object*)WookieChat->STR_change_nick);

    DoMethod((Object *) WookieChat->LV_alias, MUIM_Notify, MUIA_NList_EntryClick, MUIV_EveryTime,
            (Object*) WookieChat->App, 2, MUIM_Application_ReturnID, 89);

    /*
     getmacro((Object*)WookieChat->CH_autoconnect,MUIA_Selected,&my_auto_connect);
     getmacro((Object*)WookieChat->STR_servername,MUIA_String_Contents,&string3);
     getmacro((Object*)WookieChat->STR_port,MUIA_String_Contents,&string2);
     getmacro((Object*)WookieChat->STR_pass,MUIA_String_Contents,&string4);
     getmacro((Object*)WookieChat->STR_autojoin,MUIA_String_Contents,&auto_join);
     getmacro((Object*)WookieChat->STR_nick_registration_command,MUIA_String_Contents,&nick_pass);
     getmacro((Object*)WookieChat->CYCLE_remote_charset,MUIA_Cycle_Active,&cycle_server_charset);

     */

    //main settings window buttons for save, use and cancel
    DoMethod((Object*) WookieChat->BT_label_0, MUIM_Notify, MUIA_Pressed, FALSE, (Object*) WookieChat->App, 2,
            MUIM_Application_ReturnID, 5);
    DoMethod((Object*) WookieChat->BT_label_1, MUIM_Notify, MUIA_Pressed, FALSE, (Object*) WookieChat->App, 2,
            MUIM_Application_ReturnID, 6);
    DoMethod((Object*) WookieChat->BT_label_2, MUIM_Notify, MUIA_Pressed, FALSE, (Object*) WookieChat->App, 2,
            MUIM_Application_ReturnID, 7);
    DoMethod((Object*) WookieChat->BT_label_3, MUIM_Notify, MUIA_Pressed, FALSE, (Object*) WookieChat->App, 2,
            MUIM_Application_ReturnID, 31);

    //dcc window button notification
    DoMethod((Object*) WookieChat->BT_dcc_accept, MUIM_Notify, MUIA_Pressed, FALSE, (Object*) WookieChat->App, 2,
            MUIM_Application_ReturnID, 65);
    DoMethod((Object*) WookieChat->BT_dcc_cancel, MUIM_Notify, MUIA_Pressed, FALSE, (Object*) WookieChat->App, 2,
            MUIM_Application_ReturnID, 66);
    DoMethod((Object*) WookieChat->BT_dcc_remove, MUIM_Notify, MUIA_Pressed, FALSE, (Object*) WookieChat->App, 2,
            MUIM_Application_ReturnID, 67);

    DoMethod((Object*) WookieChat->BT_dcc_send_cancel, MUIM_Notify, MUIA_Pressed, FALSE, (Object*) WookieChat->App, 2,
            MUIM_Application_ReturnID, 75);
    DoMethod((Object*) WookieChat->BT_dcc_send_remove, MUIM_Notify, MUIA_Pressed, FALSE, (Object*) WookieChat->App, 2,
            MUIM_Application_ReturnID, 76);
    DoMethod((Object*) WookieChat->BT_dcc_send_reoffer, MUIM_Notify, MUIA_Pressed, FALSE, (Object*) WookieChat->App, 2,
            MUIM_Application_ReturnID, 79);
    DoMethod((Object*) WookieChat->BT_dcc_send_new, MUIM_Notify, MUIA_Pressed, FALSE, (Object*) WookieChat->WI_dcc_send,
            3, MUIM_Set, MUIA_Window_Open, TRUE);

    DoMethod((Object*) WookieChat->BT_dcc_send, MUIM_Notify, MUIA_Pressed, FALSE, (Object*) WookieChat->App, 2,
            MUIM_Application_ReturnID, 78);
    DoMethod((Object*) WookieChat->WI_dcc_send, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
            (Object*) WookieChat->WI_dcc_send, 3, MUIM_Set, MUIA_Window_Open, FALSE);

    //dcc exists window buttone notification
    DoMethod((Object*) WookieChat->BT_dcc_resume, MUIM_Notify, MUIA_Pressed, FALSE, (Object*) WookieChat->App, 2,
            MUIM_Application_ReturnID, 70);
    DoMethod((Object*) WookieChat->BT_dcc_overwrite, MUIM_Notify, MUIA_Pressed, FALSE, (Object*) WookieChat->App, 2,
            MUIM_Application_ReturnID, 71);
    DoMethod((Object*) WookieChat->BT_dcc_abort, MUIM_Notify, MUIA_Pressed, FALSE,
            (Object*) WookieChat->WI_dcc_file_exists, 2, MUIM_Set, MUIA_Window_Open, FALSE);
    DoMethod((Object*) WookieChat->BT_dcc_rename, MUIM_Notify, MUIA_Pressed, FALSE, (Object*) WookieChat->App, 2,
            MUIM_Application_ReturnID, 100);

    //ban window close gadget
    DoMethod((Object*) WookieChat->WI_ban, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, (Object*) WookieChat->WI_ban, 3,
            MUIM_Set, MUIA_Window_Open, FALSE);
    DoMethod((Object*) WookieChat->BT_unban, MUIM_Notify, MUIA_Selected, FALSE, (Object*) WookieChat->App, 2,
            MUIM_Application_ReturnID, 19);
    DoMethod((Object*) WookieChat->BT_refreshban, MUIM_Notify, MUIA_Selected, FALSE, (Object*) WookieChat->App, 2,
            MUIM_Application_ReturnID, 18);

    // Menu notifys!
    //DoMethod((Object*)MN1_SelectServer, MUIM_Notify, MUIA_Menuitem_Trigger,MUIV_EveryTime,(Object*)WookieChat->App,2,MUIM_Application_ReturnID, 60);
    DoMethod((Object*) MN1_SelectServer, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
            (Object*) WookieChat->WI_label_2, 3, MUIM_Set, MUIA_Window_Open, TRUE);

    DoMethod((Object*) MN1_NewTAB, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime, (Object*) WookieChat->App, 2,
            MUIM_Application_ReturnID, 61);
    DoMethod((Object*) MN1_CloseTAB, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime, (Object*) WookieChat->App, 2,
            MUIM_Application_ReturnID, 73);
    DoMethod((Object*) MN_ClearHistory, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime, (Object*) WookieChat->App,
            2, MUIM_Application_ReturnID, 62);
    DoMethod((Object*) MN_ClearAllHistory, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime,
            (Object*) WookieChat->App, 2, MUIM_Application_ReturnID, 72);
    DoMethod((Object*) MN_SaveHistory, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime, (Object*) WookieChat->App, 2,
            MUIM_Application_ReturnID, 74);
    DoMethod((Object*) MN_MUISettings, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime, (Object*) WookieChat->App, 2,
            MUIM_Application_ReturnID, 63);
    DoMethod((Object*) MN_MainSettings, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime, (Object*) WookieChat->App,
            2, MUIM_Application_ReturnID, 8);
    DoMethod((Object*) MN_windows_dcc, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime, (Object*) WookieChat->App, 2,
            MUIM_Application_ReturnID, 69);
    DoMethod((Object*) MN_windows_dcc2, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime, (Object*) WookieChat->App,
            2, MUIM_Application_ReturnID, 80);
    DoMethod((Object*) MN1_NewGTAB, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime, (Object*) WookieChat->App, 2,
            MUIM_Application_ReturnID, 77);
    DoMethod((Object*) MN1_Hide, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime, (Object*) WookieChat->App, 3,
            MUIM_Set, MUIA_Application_Iconified, TRUE);

    DoMethod((Object*) MN_cut, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime, (Object*) WookieChat->App, 2,
            MUIM_Application_ReturnID, SECOND_SET_OF_RETURNIDS + 11);
    DoMethod((Object*) MN_copy, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime, (Object*) WookieChat->App, 2,
            MUIM_Application_ReturnID, SECOND_SET_OF_RETURNIDS + 12);
    DoMethod((Object*) MN_paste, MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime, (Object*) WookieChat->App, 2,
            MUIM_Application_ReturnID, SECOND_SET_OF_RETURNIDS + 13);

// Setup DCC CHAT SEND AND RECV linked lists and structures

    //setup dcc chat recieve linked list
    dcc_chat_root = malloc(sizeof(struct dcc_chat));
    dcc_chat_root->next = NULL;
    dcc_chat_conductor = malloc(sizeof(struct dcc_chat));
    dcc_chat_conductor->next = NULL;
    create_recv_dcc_chat((char*) "0", (char*) "0", (char*) "0");
    dcc_chat_conductor->dcc_socket = -1;
    dcc_chat_root = dcc_chat_conductor;

    //setup dcc recieve linked list
    dcc_root = malloc(sizeof(struct dcc));
    dcc_root->next = NULL;
    dcc_conductor = malloc(sizeof(struct dcc));
    dcc_conductor->previous = NULL;
    dcc_conductor->next = NULL;
    create_recv_dcc((char*) "0", (char*) "0", (char*) "0", 0, (char*) "0");
    dcc_conductor->dcc_socket = -1;
    dcc_root = dcc_conductor;

    //setup dcc recieve linked list
    dcc_send_root = malloc(sizeof(struct dcc));
    dcc_send_root->next = NULL;
    dcc_send_conductor = malloc(sizeof(struct dcc));
    dcc_send_conductor->next = NULL;
    dcc_send_conductor->previous = NULL;
    create_send_dcc((char*) "0", (char*) "0", 0, 0);
    dcc_send_conductor->dcc_socket = -1;
    dcc_send_conductor->dcc_listen_socket = -1;
    dcc_send_root = dcc_send_conductor;
//
#if !ENABLE_NEWWOOKIECODE
    setmacro((Object*)WookieChat->WI_main, MUIA_Window_Open, TRUE);
#endif
// Open our servers.txt file and update the Server Selection window
    read_list_of_servers();

    struct test_tree
    {
        struct MUI_NListtree_TreeNode *t;
        struct test_tree *next;

    }*tree_conductor, *tree_root;

    tree_root = malloc(sizeof(struct test_tree));
    tree_conductor = malloc(sizeof(struct test_tree));
    tree_conductor->next = NULL;

    //tree_conductor->t=(struct MUI_NListtree_TreeNode *)DoMethod((Object*)WookieChat->NLT_Servers, MUIM_NListtree_Insert, "Servers", NULL,
    //    MUIV_NListtree_Insert_ListNode_Root, MUIV_NListtree_Insert_PrevNode_Tail, TNF_LIST|TNF_OPEN);

    tree_root = tree_conductor;

    char *work_buffer = malloc(sizeof(char) * 2000);
    char *work_buffer2 = malloc(sizeof(char) * 2000);
    char *work1 = malloc(sizeof(char) * 100);
    char *len2; //variable used for file access

    BPTR newbptr_file = Open((_s_cs)"progdir:servers.txt", MODE_OLDFILE);
    int running3 = 1;

    while (running3)
    {
        if (!newbptr_file)
            break;

        len2 = (char*) FGets(newbptr_file, (STRPTR) work_buffer, 600);
        work_buffer[strlen(work_buffer) - 1] = '\0';

        if (stricmp(work_buffer, "WOOKIECHAT_SERVERS_FILE_2"))
        {

            strcpy(work_buffer2, work_buffer);
            strtok(work_buffer2, " ");
            work1 = strtok(NULL, " ");

            if (!len2)
                running3 = 0;

            else if (!work1)
            {
                tree_conductor->next = malloc(sizeof(struct test_tree));
                tree_conductor = tree_conductor->next;
                tree_conductor->next = NULL;

                tree_conductor->t = (struct MUI_NListtree_TreeNode *) DoMethod((Object*) WookieChat->NLT_Servers,
                        MUIM_NListtree_Insert, work_buffer, NULL, MUIV_NListtree_Insert_ListNode_Root,
                        MUIV_NListtree_Insert_PrevNode_Tail, TNF_LIST);
            }
            else
            {
                DoMethod((Object*) WookieChat->NLT_Servers, MUIM_NListtree_Insert, work_buffer, NULL, tree_conductor->t,
                        MUIV_NListtree_Insert_PrevNode_Tail, NULL);

            }
        }

    }

    Close(newbptr_file);

    my_settings.current_port = atoi(my_settings.start_port);

    if (my_settings.display_server_window_on_startup)
        setmacro((Object*)WookieChat->WI_label_2, MUIA_Window_Open, TRUE);

    setmacro((Object*)WookieChat->WI_main, MUIA_Window_ActiveObject, (Object*)WookieChat->STR_usertext);
//

    {
// Startup connects
        struct MUI_NListtree_TreeNode *treenode;
        ULONG count_nodes = DoMethod((Object*) WookieChat->NLT_Servers, MUIM_NListtree_GetNr, 0,
                MUIV_NListtree_GetNr_Flag_CountAll);

        for (int count = 0, connecting = 0; count < (LONG) count_nodes; count++)
        {
            treenode = (struct MUI_NListtree_TreeNode *) DoMethod((Object*) WookieChat->NLT_Servers,
                    MUIM_NListtree_GetEntry, MUIV_NListtree_GetEntry_ListNode_Root, count, 0);

            if (treenode)
            {
                strcpy(work_buffer, (char *)treenode->tn_Name);

                acquire_connect_details(work_buffer);

                if (port_number[0] != '\0')
                {

					if (!stricmp(auto_connect, "Yes") || !stricmp(auto_connect, (char *) LGS( MSG_YES ))) /* geit FIXME: NO LOCALE SETTINGS */
                    {
                        if (!connecting)
                        {
                            connect2server(server_name, port_number, 0, 0, CONNECT_IN_CURRENT_TAB, server_charset, 1);
                            strcpy(status_conductor->groupname, group_name);
                            connecting = 1;
                            //printf("connecting1.. to %s\n",group_name);

                        }
                        else
                        {
                            create_new_status(0);

                            connect2server(server_name, port_number, 0, 0, CONNECT_IN_CURRENT_TAB, server_charset, 1);
                            strcpy(status_conductor->groupname, group_name);
                            //printf("connecting2.. to %s\n",group_name);

                        }
                    }

                }
                else
                {

                    if (port_number[0] == '\0')
                    {
                        strcpy(group_name, server_name);
                    }

                }

            }

        }
//
    }

    BOOL running = TRUE;
    ULONG signal, waitsignals;
    LONG result, e;
    int select_result;
    char *test[1000];
    char *test2 = malloc(sizeof(char) * 462);

    while (running)
    {
#if ENABLE_NEWWOOKIECODE
		result = DoMethod( application, MUIM_Application_Input, &signal);
#else
        result = DoMethod((Object*) WookieChat->App, MUIM_Application_Input, &signal);
#endif
        if (result)
        {
			if (result == 1)
            {
                char * string123 = NULL;
// user has typed some text and press enter
                getmacro((Object*)WookieChat->STR_usertext, MUIA_String_Contents, &string123);
                int c = 0;
                //BOOL start;

                if (string123)
                {

                    test[c] = strtok(string123, "\r\n\0");
                    c++;
                    while ((test[c] = strtok(NULL, "\r\n\0")) != NULL)
                    {
                        c++;
                        if (c >= 1000)
                            break;
                    }
                    int b = c;
                    c = 0;

                    //if(b==1) start=TRUE; else start=FALSE;

                    if (status_current)
                        status_conductor = status_current;
                    else
                        status_conductor = status_root;

                    while (c < b)
                    {
                        //printf("b:%d c:%d\n",b,c);

                        if (test[c])
                        {

                            int b = 0;
                            int e = strlen(test[c]);

                            if (strlen(test[c]) > 450)
                            {

                                while (1)
                                {
                                    int a = 0;
                                    int count;

                                    do
                                    {
                                        test2[a] = test[c][b];
                                        a++;
                                        b++;
                                    } while (test2[a - 1] != '\0' && a < 450 && b < e);
                                    test2[a] = '\0';

                                    for (count = 1; count <= my_settings.number_of_command_aliases; count++)
                                    {
                                        strcpy(string7, test2);
                                        string1 = strtok(string7, " ");
                                        string4 = strtok(NULL, "");

                                        strcpy(string8, my_settings.command_alias[count - 1]);
                                        string2 = strtok(string8, " ");
                                        string3 = strtok(NULL, "");

                                        if (string1 && string2 && string3)
                                        {
                                            if (!stricmp(string1, string2))
                                            {
                                                if (string4)
                                                    sprintf(string9, "%s %s", string3, string4);
                                                else
                                                    sprintf(string9, "%s", string3);

                                                {
                                                    if (status_current->current_query->name[0] == '$')
                                                    {
                                                        process_outgoing(string9, 1);
                                                    }
                                                    else if (status_current->current_query->queued_messages_total
                                                            < MAX_QUEUED_MESSAGES)
                                                    {
                                                        if (status_current->current_query->queued_messages[status_current->current_query->queued_messages_total]
                                                                == NULL)
                                                        {
                                                            status_current->current_query->queued_messages[status_current->current_query->queued_messages_total] =
                                                                    malloc(sizeof(char) * 460);
                                                            strcpy(
                                                                    status_current->current_query->queued_messages[status_current->current_query->queued_messages_total],
                                                                    string9);
                                                        }
                                                        status_current->current_query->queued_messages_total++;
                                                        queued_messages_total++;
                                                    }
                                                }
                                                count = 0;
                                                break;
                                            }

                                        }
                                    }

                                    {
                                        if (count > my_settings.number_of_command_aliases)
                                        {

                                            if (status_current->current_query->name[0] == '$')
                                            {
                                                process_outgoing(test2, 1);
                                            }
                                            else if (status_current->current_query->queued_messages_total
                                                    < MAX_QUEUED_MESSAGES)
                                            {
                                                if (status_current->current_query->queued_messages[status_current->current_query->queued_messages_total]
                                                        == NULL)
                                                {
                                                    status_current->current_query->queued_messages[status_current->current_query->queued_messages_total] =
                                                            malloc(sizeof(char) * 460);
                                                    strcpy(
                                                            status_current->current_query->queued_messages[status_current->current_query->queued_messages_total],
                                                            test2);
                                                }
                                                status_current->current_query->queued_messages_total++;
                                                queued_messages_total++;
                                            }
                                        }
                                    }
                                    if (b >= e)
                                        break;

                                }
                            }
                            else
                            {
                                int count;
                                for (count = 1; count <= my_settings.number_of_command_aliases; count++)
                                {
                                    strcpy(string7, test[c]);
                                    string1 = strtok(string7, " ");
                                    string4 = strtok(NULL, "");

                                    strcpy(string8, my_settings.command_alias[count - 1]);
                                    string2 = strtok(string8, " ");
                                    string3 = strtok(NULL, "");

                                    if (string1 && string2 && string3)
                                    {
                                        if (!stricmp(string1, string2))
                                        {
                                            if (string4)
                                                sprintf(string9, "%s %s", string3, string4);
                                            else
                                                sprintf(string9, "%s", string3);

                                            if ((status_current->current_query->queued_messages_total == 0 && c == 0)
                                                    || status_current->current_query->name[0] == '$')
                                            {
                                                process_outgoing(string9, 1);
                                            }
                                            else
                                            {
                                                if (status_current->current_query->queued_messages_total
                                                        < MAX_QUEUED_MESSAGES)
                                                {
                                                    if (status_current->current_query->queued_messages[status_current->current_query->queued_messages_total]
                                                            == NULL)
                                                    {
                                                        status_current->current_query->queued_messages[status_current->current_query->queued_messages_total] =
                                                                malloc(sizeof(char) * 460);
                                                        strcpy(
                                                                status_current->current_query->queued_messages[status_current->current_query->queued_messages_total],
                                                                string9);
                                                    }
                                                    status_current->current_query->queued_messages_total++;
                                                    queued_messages_total++;
                                                }
                                            }
                                            count = 0;
                                            break;
                                        }
                                    }
                                }

                                if ((status_current->current_query->queued_messages_total == 0 && c == 0)
                                        || status_current->current_query->name[0] == '$')
                                {
                                    if (count > my_settings.number_of_command_aliases)
                                        process_outgoing(test[c], 1);
                                }
                                else
                                {
                                    if (count > my_settings.number_of_command_aliases)
                                    {
                                        if (status_current->current_query->queued_messages_total < MAX_QUEUED_MESSAGES)
                                        {
                                            if (status_current->current_query->queued_messages[status_current->current_query->queued_messages_total]
                                                    == NULL)
                                            {
                                                status_current->current_query->queued_messages[status_current->current_query->queued_messages_total] =
                                                        malloc(sizeof(char) * 460);
                                                strcpy(
                                                        status_current->current_query->queued_messages[status_current->current_query->queued_messages_total],
                                                        test[c]);
                                            }
                                            status_current->current_query->queued_messages_total++;
                                            queued_messages_total++;
                                        }
                                    }
                                }
                            }
                        }
                        c++;

                    }
                }

                setmacro((Object*)WookieChat->STR_usertext, MUIA_String_Contents, "");
                setmacro((Object*)WookieChat->WI_main, MUIA_Window_ActiveObject, (Object*)WookieChat->STR_usertext);
                if (queued_messages_total > 0)
                    init_paste_pause_delay();

//
            }
            else if (result == 2)
            {
                save_nick_settings();
                close_server_select_window();
            }
            else if (result == 3) //save current theme
            {
// save current theme
                save_colours();
                set_colours();
                save_colours_choice();

                //printf("saving colours to file.. %s\n",my_settings.default_colours_theme);

                setmacro((Object*)WookieChat->WI_main, MUIA_Window_Open, FALSE);
                setmacro((Object*)WookieChat->WI_main, MUIA_Window_Open, TRUE);
                setmacro((Object*)WookieChat->WI_colour_settings, MUIA_Window_Open, TRUE);
//
            }
            else if (result == 93) //save current colours theme as...
            {
// save current colours theme as...
                //printf("requested saving colours as..\n");

                getmacro((Object*)WookieChat->WI_main, MUIA_Window_Window, &my_window);

#ifdef __amigaos4__
                filerequester=(struct FileRequester*)AllocAslRequestTags(ASL_FileRequest,ASLFR_Window,my_window,ASLFR_PopToFront,TRUE,/*ASLFR_TitleText,"Open Theme",*/ASLFR_InitialDrawer,"progdir:colours",TAG_DONE, NULL);
#elif __MORPHOS__
                filerequester=(struct FileRequester*)AllocAslRequestTags(ASL_FileRequest,/*ASLFR_TitleText,"Open Theme",ASLFR_InitialDrawer,"progdir:colours",*/TAG_DONE, NULL);
#else
                filerequester = (struct FileRequester*) AllocAslRequestTags(ASL_FileRequest,/*ASLFR_TitleText,"Open Theme",*/
                        ASLFR_InitialDrawer, "progdir:colours", TAG_DONE, NULL);
#endif

                aslresult = AslRequest(filerequester, NULL);
                if (aslresult)
                {

                    strcpy(work_buffer, (char *)filerequester->fr_Drawer);
                    if (work_buffer[strlen(work_buffer) - 1] != ':')
                        strcat(work_buffer, "/");
                    strcat(work_buffer, (char *)filerequester->fr_File);

                    strcpy(my_settings.default_colours_theme, work_buffer);

                    //printf("saving as colours to file..%s\n",work_buffer);

                    save_colours();
                    set_colours();
                    save_colours_choice();

                    setmacro((Object*)WookieChat->WI_main, MUIA_Window_Open, FALSE);
                    setmacro((Object*)WookieChat->WI_main, MUIA_Window_Open, TRUE);
                    setmacro((Object*)WookieChat->WI_colour_settings, MUIA_Window_Open, TRUE);

                }

                FreeAslRequest(filerequester);

//
            }
            else if (result == 4) //load colour settings
            {
// load colour settings

                getmacro((Object*)WookieChat->WI_main, MUIA_Window_Window, &my_window);

#ifdef __amigaos4__
                filerequester=(struct FileRequester*)AllocAslRequestTags(ASL_FileRequest,ASLFR_Window,my_window,ASLFR_PopToFront,TRUE,/*ASLFR_TitleText,"Open Theme",*/ASLFR_DoMultiSelect,TRUE,ASLFR_InitialDrawer,"progdir:colours",TAG_DONE, NULL);
#elif __MORPHOS__
                filerequester=(struct FileRequester*)AllocAslRequestTags(ASL_FileRequest,/*ASLFR_TitleText,"Open Theme",ASLFR_DoMultiSelect,TRUE,ASLFR_InitialDrawer,"progdir:colours",*/TAG_DONE, NULL);
#else
                filerequester = (struct FileRequester*) AllocAslRequestTags(ASL_FileRequest,/*ASLFR_TitleText,"Open Theme",*/
                        ASLFR_DoMultiSelect, TRUE, ASLFR_InitialDrawer, "progdir:colours", TAG_DONE, NULL);
#endif

                aslresult = AslRequest(filerequester, NULL);
                if (aslresult)
                {

                    strcpy(work_buffer, (char *)filerequester->fr_Drawer);
                    if (work_buffer[strlen(work_buffer) - 1] != ':')
                        strcat(work_buffer, "/");
                    strcat(work_buffer, (char *)filerequester->fr_File);

                    load_colours(work_buffer);
                    set_colours();

                    setmacro((Object*)WookieChat->WI_main, MUIA_Window_Open, FALSE);
                    setmacro((Object*)WookieChat->WI_main, MUIA_Window_Open, TRUE);
                    setmacro((Object*)WookieChat->WI_colour_settings, MUIA_Window_Open, TRUE);

                }

                FreeAslRequest(filerequester);

//
            }
            else if (result == 30) //apply colour settings to the display, but dont save or load
            {
// apply colour settings to the display, but dont save or load

                get_colours();
                setmacro((Object*)WookieChat->WI_main, MUIA_Window_Open, FALSE);
                setmacro((Object*)WookieChat->WI_main, MUIA_Window_Open, TRUE);
                setmacro((Object*)WookieChat->WI_colour_settings, MUIA_Window_Open, TRUE);

//
            }
            else if (result == 5) //save settings
            {
// save settings
                retrieve_settings();
                free_graphical_smilies(status_current->current_query);
                set_settings();
                create_user_list_buttons();

                delete_smiley_objects();
                load_graphical_smilies();
                use_graphical_smilies(status_current->current_query);

                save_settings();

                get_events_settings();
                save_events_settings();

                save_nick_settings();

                setmacro((Object*)WookieChat->WI_mainsettings, MUIA_Window_Open, FALSE);

                switch_between_tabs_with_nlisttabclick(status_current->current_query->nlist_tab_number, 1);
                //change_display(status_current->current_query->nlist_tab_number);

//
            }
            else if (result == 6) //use settings
            {
//use settings
                get_events_settings();

                retrieve_settings();
                free_graphical_smilies(status_current->current_query);
                set_settings();
                create_user_list_buttons();

                delete_smiley_objects();
                load_graphical_smilies();
                use_graphical_smilies(status_current->current_query);

                setmacro((Object*)WookieChat->WI_mainsettings, MUIA_Window_Open, FALSE);

                switch_between_tabs_with_nlisttabclick(status_current->current_query->nlist_tab_number, 1);
                //change_display(status_current->current_query->nlist_tab_number);

//
            }
            else if (result == 31) //apply settings
            {
// apply settings
                get_events_settings();

                retrieve_settings();
                free_graphical_smilies(status_current->current_query);
                set_settings();
                create_user_list_buttons();

                delete_smiley_objects();
                load_graphical_smilies();
                use_graphical_smilies(status_current->current_query);

                switch_between_tabs_with_nlisttabclick(status_current->current_query->nlist_tab_number, 1);
                //change_display(status_current->current_query->nlist_tab_number);

//
            }
            else if (result == 7) //cancel and discard settings
            {
// cancel and discard settings
                copy_undo_buffer_to_settings();
                //load_user_list_buttons_config();

                setmacro((Object*)WookieChat->WI_mainsettings, MUIA_Window_Open, FALSE);
//
            }
            else if (result == 8) //open main settings window
            {
//open main settings window
                copy_settings_to_undo_buffer();
                setmacro((Object*)WookieChat->WI_mainsettings, MUIA_Window_Open, TRUE);
//
            }
            else if (result == 9)
            {
// user has double clicked on an entry in the nicklist
                LONG a;
                struct list_entry *work_entry = malloc(sizeof(struct list_entry));

                getmacro((Object*)status_current->current_query->LV_nicklist, MUIA_NList_DoubleClick, &a);
                DoMethod((Object*) status_current->current_query->LV_nicklist, MUIM_NList_GetEntry, a, &work_entry);

                status_conductor = status_current;
                if (my_settings.open_private_tab)
                    SQuery(work_entry->name);
                if (my_settings.request_whois_info)
                    SWhoIs(work_entry->name);
                status_conductor = status_current;
                status_conductor->conductor = status_conductor->current_query;
                //DisplayNicks();

                if (RECENTLY_CREATED_A_TAB == TRUE)
                {
                    if (status_current)
                    {
                        if (status_current->current_query)
                        {
                            setmacro((Object*)WookieChat->LV_tabs, MUIA_NList_Active,
                                    status_current->current_query->nlist_tab_number);
                            sort_linked_list();
                        }
                    }

                    RECENTLY_CREATED_A_TAB = FALSE;
                }

//
            }
            else if (result == 50) //add new server
            {
// add a new server to the Server Selection window
                DoMethod(
                        (Object*) WookieChat->NLT_Servers,
                        MUIM_NListtree_Insert,
                        "SERVER=server-rename_and_move_me PORT=6667 AUTOCONNECT=No SERVER_CHARSET=UTF-8 SERVER_PASSWORD= NICK_PASSWORD= AUTOJOINS= REALNAME= USERNAME= NICK1= NICK2= NICK3= USE_GLOBAL=1",
                        NULL, MUIV_NListtree_Insert_ListNode_ActiveFallback, MUIV_NListtree_Insert_PrevNode_Active,
                        MUIV_NListtree_Insert_Flag_Active);
//
            }
            else if (result == 51) //add new group
            {
// add a new server group to the Server Selection window
                //DoMethod((Object*)WookieChat->NLT_Servers, MUIM_NListtree_Insert,
                //"NewGroup-EditMe",
                //NULL, tree_root->t, MUIV_NListtree_Insert_PrevNode_Tail, MUIV_NListtree_Insert_Flag_Active|TNF_LIST|TNF_OPEN);

                tree_conductor->next = malloc(sizeof(struct test_tree));
                tree_conductor = tree_conductor->next;
                tree_conductor->next = NULL;

                tree_conductor->t = (struct MUI_NListtree_TreeNode *) DoMethod((Object*) WookieChat->NLT_Servers,
                        MUIM_NListtree_Insert, "NewGroup-Editme", NULL, MUIV_NListtree_Insert_ListNode_Root,
                        MUIV_NListtree_Insert_PrevNode_Tail, TNF_LIST);

//
            }
            else if (result == 52) //remove server or group entry
            {
                DoMethod((Object*) WookieChat->NLT_Servers, MUIM_NListtree_Remove,
                        MUIV_NListtree_Remove_ListNode_Active, MUIV_NListtree_Remove_TreeNode_Active, 0);
            }
            else if (result == 53) //Edit Group or Server entries in the server selection windows nlisttree
            {
// Edit Group or Server entries in the server selection windows nlisttree

                struct MUI_NListtree_TreeNode *treenode = (struct MUI_NListtree_TreeNode *) DoMethod(
                        (Object*) WookieChat->NLT_Servers, MUIM_NListtree_GetEntry,
                        MUIV_NListtree_GetEntry_ListNode_Active, MUIV_NListtree_GetEntry_Position_Active, 0);

                char work_buffer[2000];

                if (treenode)
                {
                    //copy all the prefs for this server to a temporary buffer
                    strcpy(work_buffer, (char *)treenode->tn_Name);

                    //copy all the individual prefs to individual strings, etc.. we'll copy these strings into the GUI later
                    acquire_connect_details(work_buffer);

                    if (port_number[0] == '\0') // edit a group
                    {

                        setmacro((Object*)WookieChat->STR_servername, MUIA_String_Contents, server_name);
                        setmacro((Object*)WookieChat->CH_autoconnect, MUIA_Selected, FALSE);
                        setmacro((Object*)WookieChat->STR_port, MUIA_String_Contents, NULL);
                        setmacro((Object*)WookieChat->STR_pass, MUIA_String_Contents, NULL);
                        setmacro((Object*)WookieChat->STR_autojoin, MUIA_String_Contents, NULL);
                        setmacro((Object*)WookieChat->STR_nick_registration_command, MUIA_String_Contents, NULL);
                        setmacro((Object*)WookieChat->STR_server_realname, MUIA_String_Contents, NULL);
                        setmacro((Object*)WookieChat->STR_server_username, MUIA_String_Contents, NULL);
                        setmacro((Object*)WookieChat->STR_server_nickname1, MUIA_String_Contents, NULL);
                        setmacro((Object*)WookieChat->STR_server_nickname2, MUIA_String_Contents, NULL);
                        setmacro((Object*)WookieChat->STR_server_nickname3, MUIA_String_Contents, NULL);

                        setmacro((Object*)WookieChat->STR_servername, MUIA_String_BufferPos, 0);
                        setmacro((Object*)WookieChat->STR_port, MUIA_String_BufferPos, 0);
                        setmacro((Object*)WookieChat->STR_pass, MUIA_String_BufferPos, 0);
                        setmacro((Object*)WookieChat->STR_autojoin, MUIA_String_BufferPos, 0);
                        setmacro((Object*)WookieChat->STR_nick_registration_command, MUIA_String_BufferPos, 0);

                        setmacro((Object*)WookieChat->CH_autoconnect, MUIA_Disabled, TRUE);
                        setmacro((Object*)WookieChat->STR_port, MUIA_Disabled, TRUE);
                        setmacro((Object*)WookieChat->STR_pass, MUIA_Disabled, TRUE);
                        setmacro((Object*)WookieChat->STR_autojoin, MUIA_Disabled, TRUE);
                        setmacro((Object*)WookieChat->STR_nick_registration_command, MUIA_Disabled, TRUE);

                        setmacro((Object*)WookieChat->STR_server_realname, MUIA_Disabled, TRUE);
                        setmacro((Object*)WookieChat->STR_server_username, MUIA_Disabled, TRUE);
                        setmacro((Object*)WookieChat->STR_server_nickname1, MUIA_Disabled, TRUE);
                        setmacro((Object*)WookieChat->STR_server_nickname2, MUIA_Disabled, TRUE);
                        setmacro((Object*)WookieChat->STR_server_nickname3, MUIA_Disabled, TRUE);

                    }
                    else //else edit a server
                    {
                        int count2, count;
                        //setmacro((Object*)WookieChat->WI_edit_server,MUIA_Window_Open, TRUE);
                        //setmacro((Object*)WookieChat->GR_server_specific_user_prefs, MUIA_ShowMe, FALSE);

                        setmacro((Object*)WookieChat->STR_servername, MUIA_String_Contents, server_name);
                        setmacro((Object*)WookieChat->STR_port, MUIA_String_Contents, port_number);
                        setmacro((Object*)WookieChat->STR_pass, MUIA_String_Contents, server_password);
                        setmacro((Object*)WookieChat->CH_autoconnect, MUIA_Selected, auto_connect);
                        setmacro((Object*)WookieChat->STR_autojoin, MUIA_String_Contents, auto_joins);
                        setmacro((Object*)WookieChat->STR_nick_registration_command, MUIA_String_Contents,
                                nick_password);

                        setmacro((Object*)WookieChat->CH_use_global, MUIA_Selected, use_global);

                        //fill in the GUI with values we've extracted from the line we've pulled from the Server window
                        setmacro((Object*)WookieChat->STR_server_realname, MUIA_String_Contents, get_server_info(SI_REAL_NAME));
                        setmacro((Object*)WookieChat->STR_server_username, MUIA_String_Contents, get_server_info(SI_USER_NAME));
                        setmacro((Object*)WookieChat->STR_server_nickname1, MUIA_String_Contents, get_server_info(SI_NICK_1));
                        setmacro((Object*)WookieChat->STR_server_nickname2, MUIA_String_Contents, get_server_info(SI_NICK_2));
                        setmacro((Object*)WookieChat->STR_server_nickname3, MUIA_String_Contents, get_server_info(SI_NICK_3));

                        //we're editing a server now, so make all these fields editable .. (they're not editable when you edit a group)
                        setmacro((Object*)WookieChat->CH_autoconnect, MUIA_Disabled, FALSE);
                        setmacro((Object*)WookieChat->STR_port, MUIA_Disabled, FALSE);
                        setmacro((Object*)WookieChat->STR_pass, MUIA_Disabled, FALSE);
                        setmacro((Object*)WookieChat->STR_autojoin, MUIA_Disabled, FALSE);
                        setmacro((Object*)WookieChat->STR_nick_registration_command, MUIA_Disabled, FALSE);

                        if (use_global == 1) //disable the Server specified preferences, and we'll use global preferences when we connect
                        {
                            setmacro((Object*)WookieChat->STR_server_realname, MUIA_Disabled, TRUE);
                            setmacro((Object*)WookieChat->STR_server_username, MUIA_Disabled, TRUE);
                            setmacro((Object*)WookieChat->STR_server_nickname1, MUIA_Disabled, TRUE);
                            setmacro((Object*)WookieChat->STR_server_nickname2, MUIA_Disabled, TRUE);
                            setmacro((Object*)WookieChat->STR_server_nickname3, MUIA_Disabled, TRUE);

                        }
                        else
                        {
                            setmacro((Object*)WookieChat->STR_server_realname, MUIA_Disabled, FALSE);
                            setmacro((Object*)WookieChat->STR_server_username, MUIA_Disabled, FALSE);
                            setmacro((Object*)WookieChat->STR_server_nickname1, MUIA_Disabled, FALSE);
                            setmacro((Object*)WookieChat->STR_server_nickname2, MUIA_Disabled, FALSE);
                            setmacro((Object*)WookieChat->STR_server_nickname3, MUIA_Disabled, FALSE);

                        }

                        //position the cursor at the beginning of each string gadget
                        setmacro((Object*)WookieChat->STR_servername, MUIA_String_BufferPos, 0);
                        setmacro((Object*)WookieChat->STR_port, MUIA_String_BufferPos, 0);
                        setmacro((Object*)WookieChat->STR_pass, MUIA_String_BufferPos, 0);
                        setmacro((Object*)WookieChat->STR_autojoin, MUIA_String_BufferPos, 0);
                        setmacro((Object*)WookieChat->STR_nick_registration_command, MUIA_String_BufferPos, 0);

						if (!stricmp(auto_connect, "Yes") || !stricmp(auto_connect, (char *) LGS( MSG_YES ) )) /* geit FIXME: NO TRANSLATIONS IN SETTINGS */
                            setmacro((Object*)WookieChat->CH_autoconnect, MUIA_Selected, TRUE);
                        else
                            setmacro((Object*)WookieChat->CH_autoconnect, MUIA_Selected, FALSE);

                        for (count = 0; remote_charsets[count] != NULL; count++)
                        {
                            if (!stricmp(remote_charsets[count], DEFAULT_CHARSET))
                                count2 = count;

                            if (!stricmp(remote_charsets[count], server_charset))
                            {
                                setmacro((Object*)WookieChat->CYCLE_remote_charset, MUIA_Cycle_Active, count);
                                break;

                            }
                        }
                        if (remote_charsets[count] == NULL)
                            setmacro((Object*)WookieChat->CYCLE_remote_charset, MUIA_Cycle_Active, count2);

                    }
                }
//
            }
            else if (result == 54) //discard group edit changes
            {
                setmacro((Object*)WookieChat->WI_edit_group, MUIA_Window_Open, FALSE);
            }
            else if (result == 55) //accept group edit change
            {
// Accept group edit change
                char *a;
                getmacro((Object*)WookieChat->STR_groupname, MUIA_String_Contents, &a);

                DoMethod((Object*) WookieChat->NLT_Servers, MUIM_NListtree_Rename,
                        MUIV_NListtree_Rename_TreeNode_Active, a, 0);
                setmacro((Object*)WookieChat->WI_edit_group, MUIA_Window_Open, FALSE);
//
            }
            else if (result == 56) //discard server edit changes
            {
                setmacro((Object*)WookieChat->WI_edit_server, MUIA_Window_Open, FALSE);
            }
            else if (result == 57) //accept server edit change
            {
// Accept server edit change

                char work_buffer3[1000];
                char *nick_pass, *auto_join;
                LONG *my_auto_connect;
                int cycle_server_charset;
                char *realname, *username, *nick, *nick2, *nick3;

                getmacro((Object*)WookieChat->CH_autoconnect, MUIA_Selected, &my_auto_connect);
                getmacro((Object*)WookieChat->STR_servername, MUIA_String_Contents, &string3);
                getmacro((Object*)WookieChat->STR_port, MUIA_String_Contents, &string2);
                getmacro((Object*)WookieChat->STR_pass, MUIA_String_Contents, &string4);
                getmacro((Object*)WookieChat->STR_autojoin, MUIA_String_Contents, &auto_join);
                getmacro((Object*)WookieChat->STR_nick_registration_command, MUIA_String_Contents, &nick_pass);
                getmacro((Object*)WookieChat->CYCLE_remote_charset, MUIA_Cycle_Active, &cycle_server_charset);
                getmacro((Object*)WookieChat->STR_server_realname, MUIA_String_Contents, &realname);
                getmacro((Object*)WookieChat->STR_server_username, MUIA_String_Contents, &username);
                getmacro((Object*)WookieChat->STR_server_nickname1, MUIA_String_Contents, &nick);
                getmacro((Object*)WookieChat->STR_server_nickname2, MUIA_String_Contents, &nick2);
                getmacro((Object*)WookieChat->STR_server_nickname3, MUIA_String_Contents, &nick3);
                getmacro((Object*)WookieChat->CH_use_global, MUIA_Selected, &use_global);

                //printf("use global: %i\n", use_global);

                //LONG use_global2;
                //getmacro((Object*)WookieChat->CH_use_global,MUIA_Selected,&use_global2);

                //printf("use global2: %i\n", use_global2);

                //TEMPLATE FOR BLANK SERVER
                //"SERVER=server-rename_and_move_me PORT=6667 AUTOCONNECT=No SERVER_CHARSET=UTF-8 SERVER_PASSWORD= NICK_PASSWORD= AUTOJOINS= REALNAME= USERNAME= NICK1= NICK2= NICK3=",NULL,

                //if its a group change, lets only change the group details
                if (string2[0] == '\0')
                {
                    strcpy(work_buffer3, string3);

                }
                else //its a server entry
                {
                    if (string3)
                    {
                        strcpy(work_buffer3, "SERVER=");
                        strcat(work_buffer3, string3);

                        if (string2)
                        {
                            strcat(work_buffer3, " PORT=");
                            strcat(work_buffer3, string2);
                        }
                    }

                    strcat(work_buffer3, " AUTOCONNECT=");

                    if (my_auto_connect)
						strcat(work_buffer3, (char *) "Yes"); /*fixed local saving here */
                    else
						strcat(work_buffer3, (char *) "No" ); /* fixed local saving here */

                    strcat(work_buffer3, " SERVER_CHARSET=");
                    //#ifdef __AROS__
                    //strcat(work_buffer3,"0");
                    //#else
                    //strcat(work_buffer3,local_charsets[cycle_server_charset]);
                    strcat(work_buffer3, remote_charsets[cycle_server_charset]);
                    //#endif

                    if (string4)
                    {
                        if (!strstr(string4, " "))
                        {
                            strcat(work_buffer3, " SERVER_PASSWORD=");
                            strcat(work_buffer3, string4);
                        }
                        else
                        {
                            strcat(work_buffer3, " SERVER_PASSWORD=");
                            string1 = strtok(string4, " ");
                            strcat(work_buffer3, string1);

                        }
                    }
                    else
                        strcat(work_buffer3, " SERVER_PASSWORD=");

                    if (nick_pass)
                    {
                        if (!strstr(nick_pass, " "))
                        {
                            strcat(work_buffer3, " NICK_PASSWORD=");
                            strcat(work_buffer3, nick_pass);
                        }
                        else
                        {
                            strcat(work_buffer3, " NICK_PASSWORD=");
                            string1 = strtok(nick_pass, " ");
                            strcat(work_buffer3, string1);
                        }

                    }
                    else
                        strcat(work_buffer3, " NICK_PASSWORD=");

                    if (auto_join)
                    {
                        if (!strstr(auto_join, " "))
                        {
                            strcat(work_buffer3, " AUTOJOINS=");
                            strcat(work_buffer3, auto_join);
                        }
                        else
                        {
                            strcat(work_buffer3, " AUTOJOINS=");
                            string1 = strtok(auto_join, " ");
                            strcat(work_buffer3, string1);

                        }
                    }
                    else
                        strcat(work_buffer3, " AUTOJOINS=");

                    strcat(work_buffer3, " USERNAME=");
                    if (username)
                    {
                        strcat(work_buffer3, username);
                    }

                    strcat(work_buffer3, " REALNAME=");
                    if (realname)
                    {
                        strcat(work_buffer3, realname);
                    }

                    strcat(work_buffer3, " NICK1=");
                    if (nick)
                    {
                        strcat(work_buffer3, nick);
                    }

                    strcat(work_buffer3, " NICK2=");
                    if (nick2)
                    {
                        strcat(work_buffer3, nick2);
                    }

                    strcat(work_buffer3, " NICK3=");
                    if (nick3)
                    {
                        strcat(work_buffer3, nick3);
                    }

                    strcat(work_buffer3, " USE_GLOBAL=");
                    if (use_global == TRUE)
                    {
                        strcat(work_buffer3, "1");
                    }
                    else
                    {
                        strcat(work_buffer3, "0");
                    }

                }
#ifndef __AROS__
                struct MUI_NListtree_TreeNode *treenode = (struct MUI_NListtree_TreeNode *) DoMethod(
                        (Object*) WookieChat->NLT_Servers, MUIM_NListtree_GetEntry,
                        MUIV_NListtree_GetEntry_ListNode_Active, MUIV_NListtree_GetEntry_Position_Active, 0);
#endif

                if (DEBUG)
                    printf("new server entry %s\n", work_buffer3);
#ifndef __AROS__
                if(treenode)
#endif
                {

                    DoMethod((Object*) WookieChat->NLT_Servers, MUIM_NListtree_Rename,
                            MUIV_NListtree_Rename_TreeNode_Active, work_buffer3, 0);
                }

                if (use_global == TRUE)
                {
                    setmacro((Object*)WookieChat->STR_server_realname, MUIA_Disabled, TRUE);
                    setmacro((Object*)WookieChat->STR_server_username, MUIA_Disabled, TRUE);
                    setmacro((Object*)WookieChat->STR_server_nickname1, MUIA_Disabled, TRUE);
                    setmacro((Object*)WookieChat->STR_server_nickname2, MUIA_Disabled, TRUE);
                    setmacro((Object*)WookieChat->STR_server_nickname3, MUIA_Disabled, TRUE);

                }
                else
                {
                    setmacro((Object*)WookieChat->STR_server_realname, MUIA_Disabled, FALSE);
                    setmacro((Object*)WookieChat->STR_server_username, MUIA_Disabled, FALSE);
                    setmacro((Object*)WookieChat->STR_server_nickname1, MUIA_Disabled, FALSE);
                    setmacro((Object*)WookieChat->STR_server_nickname2, MUIA_Disabled, FALSE);
                    setmacro((Object*)WookieChat->STR_server_nickname3, MUIA_Disabled, FALSE);

                }
//
            }
            else if (result == 49) // connect to server, automatically choose in current tab or new tab
            {
// Connect to server, automatically choose in current tab or new tab

                save_nick_settings();

                struct MUI_NListtree_TreeNode *treenode = (struct MUI_NListtree_TreeNode *) DoMethod(
                        (Object*) WookieChat->NLT_Servers, MUIM_NListtree_GetEntry,
                        MUIV_NListtree_GetEntry_ListNode_Active, MUIV_NListtree_GetEntry_Position_Active, 0);

                if (treenode)
                {

                    char work_buffer[800];
                    char *work;
                    if (treenode->tn_Name)
                        strcpy(work_buffer, (char *)treenode->tn_Name);

                    strtok(work_buffer, " ");
                    work = strtok(NULL, " ");

                    if (work)
                    {

                        int a = 1;

                        status_conductor = status_root;

                        while (status_conductor)
                        {
                            if (status_conductor->connection_active == 0 && status_conductor->GR_server1_buttons)
                            {
                                a = 0;
                                break;
                            }

                            status_conductor = status_conductor->next;
                        }

                        if (a == 0)
                            DoMethod((Object*) WookieChat->App, MUIM_Application_ReturnID, 58);
                        else
                        {
                            DoMethod((Object*) WookieChat->App, MUIM_Application_ReturnID, 59);
                        }

                    }
                }
//
            }
            else if (result == 58) // connect to server
            {
// Connect to server

                save_nick_settings();

                struct MUI_NListtree_TreeNode *treenode = (struct MUI_NListtree_TreeNode *) DoMethod(
                        (Object*) WookieChat->NLT_Servers, MUIM_NListtree_GetEntry,
                        MUIV_NListtree_GetEntry_ListNode_Active, MUIV_NListtree_GetEntry_Position_Active, 0);

                if (treenode)
                {
                    char work_buffer[600];

                    if (treenode->tn_Name)
                        strcpy(work_buffer, (char *)treenode->tn_Name);

                    acquire_connect_details(work_buffer);

                    close_server_select_window();

                    status_conductor = status_current; // 2004-10-13 change to make connects
                    status_conductor->conductor = status_conductor->root; // connect in the right tab

                    status_conductor->quit_requested = TRUE;
                    status_conductor->trying_to_reconnect = FALSE;
                    status_conductor->retry_number = 0;
                    start_reconnect_delay_timer = FALSE;

                    connect2server(server_name, port_number, 0, 1, CONNECT_IN_CURRENT_TAB, server_charset, 1);

                }
//
            }
            else if (result == 59) // connect to server in a new tab
            {
// Connect to server in a new tab
                save_nick_settings();

                struct MUI_NListtree_TreeNode *treenode = (struct MUI_NListtree_TreeNode *) DoMethod(
                        (Object*) WookieChat->NLT_Servers, MUIM_NListtree_GetEntry,
                        MUIV_NListtree_GetEntry_ListNode_Active, MUIV_NListtree_GetEntry_Position_Active, 0);

                if (treenode)
                {
                    char work_buffer[600];

                    if (treenode->tn_Name)
                        strcpy(work_buffer, (char *)treenode->tn_Name);

                    create_new_status(0);

                    acquire_connect_details(work_buffer);

                    status_conductor->quit_requested = TRUE;
                    status_conductor->trying_to_reconnect = FALSE;
                    status_conductor->retry_number = 0;
                    start_reconnect_delay_timer = FALSE;

                    connect2server(server_name, port_number, 0, 1, CONNECT_IN_CURRENT_TAB, server_charset, 1);

                    close_server_select_window();

                }
//
            }
            else if (result == 60) // auto-rename the incoming dcc file transfer
            {
                LONG a = 0;
                struct dcc_entry *find = NULL;
                DoMethod((Object*) WookieChat->LV_dcc, MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, &find);

                dcc_conductor = dcc_root->next;

                while (dcc_conductor && find)
                {

                    if (!dcc_conductor->removed)
                    {
                        if (find && dcc_conductor->entry)
                        {
                            if ( //!stricmp(find->status,dcc_conductor->entry->status) &&
                            !stricmp(find->nick, dcc_conductor->entry->nick)
                                    && !stricmp(find->filename, dcc_conductor->entry->filename)
                                    && !stricmp(find->port, dcc_conductor->entry->port)) //experimental, adding port string to entry struct
                            {

                                if (dcc_conductor->entry)
                                {
                                    if (DEBUG)
                                        printf("auto renaming incoming file\n");

                                    int count = 1;

                                    getmacro((Object*)WookieChat->PA_dcc_label_0, MUIA_String_Contents, &string5);

                                    if (string5)
                                        strcpy(dcc_conductor->filepath, string5);
                                    else
                                        strcpy(dcc_conductor->filepath, "");

                                    //printf("path:%s\n",dcc_conductor->filepath);

                                    strcpy(string11, dcc_conductor->filename);

                                    //printf("string11:%s\n",string11);

                                    if (dcc_conductor->filepath[strlen(dcc_conductor->filepath) - 1] != '/'
                                            && dcc_conductor->filepath[strlen(dcc_conductor->filepath) - 1] != ':')
                                        sprintf(dcc_conductor->full_filename_with_path, "%s/%s",
                                                dcc_conductor->filepath, dcc_conductor->filename);
                                    else
                                        sprintf(dcc_conductor->full_filename_with_path, "%s%s", dcc_conductor->filepath,
                                                dcc_conductor->filename);

                                    strcpy(orig_filename, dcc_conductor->full_filename_with_path);

                                    strcpy(filename, dcc_conductor->full_filename_with_path);

                                    strcpy(new_filename, string11);

                                    while ((newbptr_file = Open((_s_cs)filename, MODE_OLDFILE)))
                                    {
                                        SetCurrentDirName((_s_cs)dcc_conductor->filepath);

                                        Close(newbptr_file);
                                        count++;
                                        sprintf(filename, "%s_%d", orig_filename, count);
                                        sprintf(new_filename, "%s_%d", string11, count);

                                    }

                                    if (count != 1)
                                        sprintf(filename, "%s_%i", orig_filename, count);

                                    //printf("new filename:%s\n",filename);
                                    strncpy(dcc_conductor->filename, new_filename, 499);
                                    dcc_conductor->filename[499] = '\0';

                                    strncpy(dcc_conductor->entry->filename, new_filename, 499);
                                    dcc_conductor->entry->filename[499] = '\0';

                                    //printf("dcc_conductor->filename:%s\n",dcc_conductor->filename);

                                    strcpy(dcc_conductor->full_filename_with_path, filename);

                                    //printf("dcc_conductor->full_filename_with_path:%s\n",dcc_conductor->full_filename_with_path);

                                    dcc_conductor->total_recv = 0;

									strcpy(dcc_conductor->entry->status, (char *) LGS( MSG_CONNECTING ));
                                    strcpy(dcc_conductor->entry->percentage, "0%");

                                    DoMethod((Object*) WookieChat->LV_dcc, MUIM_NList_ReplaceSingle,
                                            dcc_conductor->entry, a, NOWRAP, ALIGN_LEFT);

                                    //dcc_conductor->accepted=TRUE;

                                    connect(dcc_conductor->dcc_socket, (struct sockaddr*) &dcc_conductor->test,
                                            sizeof(struct sockaddr));

                                }

                                setmacro((Object*)WookieChat->WI_dcc_file_exists, MUIA_Window_Open, FALSE);
                                dcc_conductor = dcc_root->next;

                                break;

                            }

                            a++;

                        }
                    }

                    dcc_conductor = dcc_conductor->next;

                }

            }
            else if (result == 61) //open new tab
            {
                create_new_status(0);
            }
            else if (result == 62) //clear history
            {
                current_query->entries_count = 0;
                DoMethod((Object*) current_query->LV_channel, MUIM_NList_Clear);
            }
            else if (result == 63) //open mui settings
            {
                DoMethod((Object*) WookieChat->App, MUIM_Application_OpenConfigWindow, 0);
            }
            else if (result == 64) // perform on connect edit
            {
// Edit perform on connect Script
                struct MUI_NListtree_TreeNode *treenode = (struct MUI_NListtree_TreeNode *) DoMethod(
                        (Object*) WookieChat->NLT_Servers, MUIM_NListtree_GetEntry,
                        MUIV_NListtree_GetEntry_ListNode_Active, MUIV_NListtree_GetEntry_Position_Active, 0);

                if (treenode)
                {
                    strcpy(work_buffer, (char *)treenode->tn_Name);
                    strtok(work_buffer, " ");
                    char *work = strtok(NULL, "");

                    if (work)
                    {

                        treenode = (struct MUI_NListtree_TreeNode *) DoMethod((Object*) WookieChat->NLT_Servers,
                                MUIM_NListtree_GetEntry, MUIV_NListtree_GetEntry_ListNode_Active,
                                MUIV_NListtree_GetEntry_Position_Head, 0);
                        treenode = (struct MUI_NListtree_TreeNode *) DoMethod((Object*) WookieChat->NLT_Servers,
                                MUIM_NListtree_GetEntry, treenode, MUIV_NListtree_GetEntry_Position_Parent, 0);

                    }

                    if (wookie_folder[strlen(wookie_folder) - 1] == ':')
                        sprintf(file_name, "run >nil: %s %sconnectscripts/%s", my_settings.text_editor, wookie_folder,
                                treenode->tn_Name);
                    else
                        sprintf(file_name, "run >nil: %s %s/connectscripts/%s", my_settings.text_editor, wookie_folder,
                                treenode->tn_Name);

#ifdef __amigaos4__
                    SystemTags((_s_cs)file_name,TAG_DONE);
#else
                    Execute((_s_cs)file_name, 0, 0);
#endif
                }
//
            }
            else if (result == 65) // DCC Accept Button Pressed
            {
                getmacro((Object*)WookieChat->PA_dcc_label_0, MUIA_String_Contents, &string1);

                accept_dcc(string1);
            }
            else if (result == 66) // DCC Cancel Button Pressed
            {
// DCC Cancel Button Pressed
                LONG a = 0;
                struct dcc_entry *find = NULL;
                DoMethod((Object*) WookieChat->LV_dcc, MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, &find);

                dcc_conductor = dcc_root->next;

                while (dcc_conductor && find)
                {

                    if (!dcc_conductor->removed)
                    {

                        if (find && dcc_conductor->entry)
                        {
                            if ( //!stricmp(find->status,dcc_conductor->entry->status) &&
                            !stricmp(find->nick, dcc_conductor->entry->nick)
                                    && !stricmp(find->filename, dcc_conductor->entry->filename)
                                    && !stricmp(find->port, dcc_conductor->entry->port)) //experimental, adding port string to entry struct
                            {

                                if (dcc_conductor->entry)
                                {
									strcpy(dcc_conductor->entry->status, (char *) LGS( MSG_CANCELLED ) );
                                    strcpy(dcc_conductor->entry->percentage, "N/A");

                                    dcc_conductor->cancelled = 1;

                                    //if(dcc_conductor->completed!=1 && dcc_conductor->connected!=0)
                                    //{

#ifdef __amigaos4__
                                    if(dcc_conductor->dcc_file) Close(dcc_conductor->dcc_file);
                                    dcc_conductor->dcc_file=0;
#else
                                    if (dcc_conductor->dcc_file)
                                        Close(dcc_conductor->dcc_file);
                                    dcc_conductor->dcc_file = 0;
#endif

                                    shutdown_my_dcc_recv_socket();

                                    //}
                                    DoMethod((Object*) WookieChat->LV_dcc, MUIM_NList_ReplaceSingle,
                                            dcc_conductor->entry, a, NOWRAP, ALIGN_LEFT);
                                }

                                break;

                            }

                        }
                        a++;
                    }

                    dcc_conductor = dcc_conductor->next;

                }
//
            }
            else if (result == 67) // DCC Remove From List Button Pressed
            {
// DCC Remove From List Button Pressed
                struct dcc_entry *find = NULL;
                DoMethod((Object*) WookieChat->LV_dcc, MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, &find);
                LONG a = 0;

                dcc_conductor = dcc_root->next;

                while (find && dcc_conductor)
                {

                    if (find && dcc_conductor->entry && !dcc_conductor->removed)
                    {
                        if ( //!stricmp(find->status,dcc_conductor->entry->status) &&
                        !stricmp(find->nick, dcc_conductor->entry->nick)
                                && !stricmp(find->filename, dcc_conductor->entry->filename)
                                && !stricmp(find->port, dcc_conductor->entry->port)) //experimental, adding port string to entry struct
                        {
                            struct dcc *dcc_prev;
                            struct dcc *dcc_next;
                            shutdown_my_dcc_recv_socket();

                            dcc_conductor->cancelled = 1;
                            dcc_conductor->removed = 1;

                            dcc_prev = dcc_conductor->previous;
                            dcc_next = dcc_conductor->next;

                            if (DEBUG)
                                printf("\ndeleting file:%s\n", dcc_conductor->filename);

                            DoMethod((Object*) WookieChat->LV_dcc, MUIM_NList_Remove, a);

                            if (dcc_prev)
                            {
                                if (dcc_next)
                                {
                                    if (DEBUG)
                                        printf("dcc_conductor->next isnt false\n");
                                    dcc_prev->next = dcc_next;
                                    dcc_next->previous = dcc_prev;
                                    free(dcc_conductor);
                                    dcc_conductor = NULL;
                                }
                                else
                                {
                                    if (DEBUG)
                                        printf("dcc_conductor->next IS false\n");
                                    free(dcc_conductor);
                                    dcc_prev->next = NULL;
                                    dcc_conductor = NULL;
                                }
                            }

                            for (dcc_conductor = dcc_root->next; dcc_conductor; dcc_conductor = dcc_conductor->next)
                            {
                                if (DEBUG)
                                    printf("the list forwards, file:%s\n", dcc_conductor->filename);

                            }

                            /*for(dcc_conductor=dcc_root->next; dcc_conductor->next; dcc_conductor=dcc_conductor->next);

                             for(; dcc_conductor->previous; dcc_conductor=dcc_conductor->previous)
                             {
                             if(DEBUG) printf("the list backwards, file:%s\n",dcc_conductor->filename);

                             } */

                            break;

                        }

                        a++;

                    }

                    dcc_conductor = dcc_conductor->next;

                }

                dcc_conductor = dcc_root->next;
                a = 0;
//
            }
            else if (result == 68) // close the DCC window
            {
                setmacro((Object*)WookieChat->WI_dcc, MUIA_Window_Open, FALSE);

            }
            else if (result == 69) // open the incoming DCC window
            {
// Open the incoming DCC SEND window
                setmacro((Object*)WookieChat->WI_dcc, MUIA_Window_Activate, FALSE);
                setmacro((Object*)WookieChat->WI_dcc, MUIA_Window_Open, TRUE);
#ifdef __AROS__
                setmacro((Object*)WookieChat->WI_dcc, MUIA_Window_DefaultObject, WookieChat->LV_dcc);
                setmacro((Object*)WookieChat->WI_dcc, MUIA_Window_ActiveObject, WookieChat->LV_dcc);
#endif
//
            }
            else if (result == 70) //user has selected to RESUME a DCC transfer
            {
// User has selected to RESUME a DCC transfer
                struct dcc_entry *find = NULL;
                DoMethod((Object*) WookieChat->LV_dcc, MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, &find);

                dcc_conductor = dcc_root->next;
                //printf("resume\n");

                while (find && dcc_conductor)
                {
                    //printf("resume2\n");

                    if (!dcc_conductor->removed)
                    {

                        if (find && dcc_conductor->entry)
                        {
                            if (!stricmp(find->status, dcc_conductor->entry->status)
                                    && !stricmp(find->nick, dcc_conductor->entry->nick)
                                    && !stricmp(find->filename, dcc_conductor->entry->filename)) // &&
                            //!stricmp(find->port,dcc_conductor->entry->port) )
                            {

                                status_conductor = status_root;
                                while (status_conductor)
                                {
                                    if (!stricmp(status_conductor->networkname, dcc_conductor->networkname))
                                        break;
                                    status_conductor = status_conductor->next;
                                    if (!status_conductor)
                                    {
                                        status_conductor = status_root;
                                        break;
                                    }
                                }

                                sprintf(sendstuff, "PRIVMSG %s :\001DCC RESUME \"%s\" %d %lu\001\r\n",
                                        dcc_conductor->nick, dcc_conductor->filename, atoi(dcc_conductor->port),
                                        dcc_conductor->total_recv);
                                send_text(sendstuff);

                                break;
                            }

                        }
                    }

                    dcc_conductor = dcc_conductor->next;

                }

                setmacro((Object*)WookieChat->WI_dcc_file_exists, MUIA_Window_Open, FALSE);

//
            }
            else if (result == 71) //user has selected to OVERWRITE existing file in a DCC transfer
            {
// User has selected to OVERWRITE existing file in a DCC transfer
                struct dcc_entry *find = NULL;
                DoMethod((Object*) WookieChat->LV_dcc, MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, &find);

                dcc_conductor = dcc_root->next;

                while (find && dcc_conductor)
                {

                    if (!dcc_conductor->removed)
                    {

                        if (find && dcc_conductor->entry)
                        {
                            if ( //!stricmp(find->status,dcc_conductor->entry->status) &&
                            !stricmp(find->nick, dcc_conductor->entry->nick)
                                    && !stricmp(find->filename, dcc_conductor->entry->filename)
                                    && !stricmp(find->port, dcc_conductor->entry->port)) //experimental, adding port string to entry struct
                            {

                                dcc_conductor->total_recv = 0;
                                connect(dcc_conductor->dcc_socket, (struct sockaddr*) &dcc_conductor->test,
                                        sizeof(struct sockaddr));

                                break;
                            }

                        }
                    }

                    dcc_conductor = dcc_conductor->next;

                }

                setmacro((Object*)WookieChat->WI_dcc_file_exists, MUIA_Window_Open, FALSE);
//
            }
            else if (result == 72) //clear all history
            {
// Clear all history

                for (status_conductor = status_root; status_conductor; status_conductor = status_conductor->next)
                {
                    for (status_conductor->conductor = status_conductor->root; status_conductor->conductor;
                            status_conductor->conductor = status_conductor->conductor->next)
                    {
                        if (!status_conductor->conductor->removed)
                        {
                            if (status_conductor->conductor->LV_channel)
                            {
                                status_conductor->conductor->entries_count = 0;

                                DoMethod((Object*) status_conductor->conductor->LV_channel, MUIM_NList_Clear);
                            }
                        }
                    }

                }

                status_conductor = status_current;
                status_conductor->conductor = status_conductor->current_query;
//
            }
            else if (result == 73) //user selected Close TAB from the pull down menu
            {
                status_conductor = status_current;
                if (strcspn(status_current->current_query->name, status_conductor->chantypes) == 0)
                    SPart(NULL);
                else
                    close_tab();
                //if(status_current->current_query->name[0]=='#') SPart(NULL); else close_tab();
            }
            else if (result == 74) //user selected save history menu item
            {
// User selected save history menu item

                status_conductor = status_current;

				//filerequester=(struct FileRequester*)AllocAslRequestTags(ASL_FileRequest,ASLFR_PopToFront,TRUE,ASLFR_TitleText,LGS( MSG_CHOOSE_FILENAME ),ASLFR_DoMultiSelect,TRUE,ASLFR_InitialDrawer,"progdir:",TAG_DONE, NULL);
                getmacro((Object*)WookieChat->WI_main, MUIA_Window_Window, &my_window);

#ifdef __amigaos4__
				filerequester=(struct FileRequester * ) AllocAslRequestTags( ASL_FileRequest, ASLFR_Window, my_window,
																							ASLFR_PopToFront,TRUE,
																							ASLFR_TitleText, LGS( MSG_CHOOSE_FILENAME ),
																							ASLFR_DoMultiSelect, TRUE,
																							ASLFR_InitialDrawer, "progdir:",
																							TAG_DONE );
#elif __MORPHOS__
				filerequester=(struct FileRequester*) AllocAslRequestTags( ASL_FileRequest, ASLFR_TitleText, LGS( MSG_CHOOSE_FILENAME ),
																								ASLFR_DoMultiSelect, TRUE,
																								ASLFR_InitialDrawer, "progdir:",
																								TAG_DONE );
#else
				filerequester = (struct FileRequester*) AllocAslRequestTags(ASL_FileRequest,
																							ASLFR_TitleText, LGS( MSG_CHOOSE_FILENAME ),
																							ASLFR_DoMultiSelect, TRUE,
																							ASLFR_InitialDrawer, "progdir:",
																							TAG_DONE );
#endif

                aslresult = AslRequest(filerequester, NULL);
                if (aslresult)
                {

                    strcpy(work_buffer, (char *)filerequester->fr_Drawer);
                    if (work_buffer[strlen(work_buffer) - 1] != ':')
                        strcat(work_buffer, "/");
                    strcat(work_buffer, (char *)filerequester->fr_File);

                    newbptr_file = Open((_s_cs)work_buffer, MODE_NEWFILE);

                    if (newbptr_file)
                    {
                        ULONG entries;
                        getmacro((Object*)status_conductor->current_query->LV_channel, MUIA_NList_Entries, &entries);

						sprintf(buffer3, "%s%sBuffer%s %s %s", timestamp,
								LGS( MSG_OPENING_BRACKET ),
								LGS( MSG_PART ),
								LGS( MSG_SAVING_BUFFER_TO ),
                                work_buffer);
                        add_text_to_current_list(buffer3, 9, ACTIVITY_CHAT);

                        char work_buffer[800];
                        char work_buffer2[800];
                        strcpy(work_buffer2, "");

                        for (unsigned int a = 0; a < entries; a++)
                        {
                            strcpy(work_buffer, "");

                            DoMethod((Object*) status_conductor->current_query->LV_channel, MUIM_NList_GetEntry, a,
                                    &string2);

                            if (string2)
                                strcpy(work_buffer, string2);

                            if (strcmp(work_buffer, work_buffer2))
                            {
                                //if(DEBUG) printf("work_buffer:%s\n",work_buffer);
                                strcpy(work_buffer2, string2);

                                strcpy(work_buffer, convert_graphical_smilies_2_text(work_buffer));

                                strtok(work_buffer, "]");
                                string1 = strtok(NULL, "");

                                if (string1)
                                {
                                    //if(DEBUG) printf("string1:%s\n",string1);
                                    if (strcmp(string1, ""))
                                    {
                                        if (FPuts(newbptr_file, (l_in) string1) != 0)
                                            break;
                                        FPutC(newbptr_file, '\n');
                                    }
                                }
                            }

                        }
                        Close(newbptr_file);

                    }

                }
                FreeAslRequest(filerequester);
//
            }

            else if (result == 75) //user wants to CANCEL an outgoing file transfer
            {
// User wants to CANCEL an outgoing file transfer

                LONG a = 0;
                struct dcc_entry *find = NULL;
                DoMethod((Object*) WookieChat->LV_send_dcc, MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, &find);

                dcc_send_conductor = dcc_send_root->next;

                while (dcc_send_conductor && find)
                {

                    if (!dcc_send_conductor->removed)
                    {

                        if (find && dcc_send_conductor->entry)
                        {
                            if ( //!stricmp(find->status,dcc_send_conductor->entry->status) &&
                            !stricmp(find->nick, dcc_send_conductor->entry->nick)
                                    && !stricmp(find->filename, dcc_send_conductor->entry->filename)
                                    && !stricmp(find->port, dcc_send_conductor->entry->port))
                            {

                                if (dcc_send_conductor->entry)
                                {
									strcpy(dcc_send_conductor->entry->status, (char *) LGS( MSG_CANCELLED ) );
                                    strcpy(dcc_send_conductor->entry->percentage, "N/A");
                                    dcc_send_conductor->cancelled = 1;
                                    //if(dcc_send_conductor->completed!=1 && dcc_send_conductor->connected!=0)
                                    //{

                                    shutdown_my_dcc_send_socket();

                                    //}
                                    DoMethod((Object*) WookieChat->LV_send_dcc, MUIM_NList_ReplaceSingle,
                                            dcc_send_conductor->entry, a, NOWRAP, ALIGN_LEFT);
                                }

                                break;

                            }

                        }
                        a++;
                    }

                    dcc_send_conductor = dcc_send_conductor->next;

                }
//
            }
            else if (result == 76) // Remove DCC SEND From List
            {
// Remove DCC SEND From List
                struct dcc_entry *find = NULL;
                DoMethod((Object*) WookieChat->LV_send_dcc, MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, &find);
                LONG a = 0;

                dcc_send_conductor = dcc_send_root->next;

                while (find && dcc_send_conductor)
                {

                    if (find && dcc_send_conductor->entry && !dcc_send_conductor->removed)
                    {
                        if ( //!stricmp(find->status,dcc_send_conductor->entry->status) &&
                        !stricmp(find->nick, dcc_send_conductor->entry->nick)
                                && !stricmp(find->filename, dcc_send_conductor->entry->filename)
                                && !stricmp(find->port, dcc_send_conductor->entry->port))
                        {

                            struct dcc *dcc_prev;
                            struct dcc *dcc_next;
                            shutdown_my_dcc_send_socket();

                            dcc_send_conductor->cancelled = 1;
                            dcc_send_conductor->removed = 1;

                            dcc_prev = dcc_send_conductor->previous;
                            dcc_next = dcc_send_conductor->next;

                            if (DEBUG)
                                printf("\ndeleting file:%s\n", dcc_send_conductor->filename);

                            DoMethod((Object*) WookieChat->LV_send_dcc, MUIM_NList_Remove, a);

                            if (dcc_prev)
                            {
                                if (dcc_next)
                                {
                                    if (DEBUG)
                                        printf("dcc_send_conductor->next isnt false\n");
                                    dcc_prev->next = dcc_next;
                                    dcc_next->previous = dcc_prev;
                                    free(dcc_send_conductor);
                                    dcc_send_conductor = NULL;
                                }
                                else
                                {
                                    if (DEBUG)
                                        printf("dcc_send_conductor->next IS false\n");
                                    free(dcc_send_conductor);
                                    dcc_prev->next = NULL;
                                    dcc_send_conductor = NULL;
                                }
                            }

                            for (dcc_send_conductor = dcc_send_root->next; dcc_send_conductor; dcc_send_conductor =
                                    dcc_send_conductor->next)
                            {
                                if (DEBUG)
                                    printf("the list forwards, file:%s\n", dcc_send_conductor->filename);

                            }

                            break;

                        }

                        a++;

                    }

                    dcc_send_conductor = dcc_send_conductor->next;

                }

                dcc_send_conductor = dcc_send_root->next;
                a = 0;
//
            }
            else if (result == 77) // Open a private messages tab, all private messages will go here
            {
                status_conductor = status_current;
                SQuery((char*) "-global");
                if (RECENTLY_CREATED_A_TAB == TRUE)
                {
                    if (status_current)
                    {
                        if (status_current->current_query)
                        {
                            setmacro((Object*)WookieChat->LV_tabs, MUIA_NList_Active,
                                    status_current->current_query->nlist_tab_number);
                            sort_linked_list();
                        }
                    }

                    RECENTLY_CREATED_A_TAB = FALSE;
                }
            }
            else if (result == 78) // DCC SEND button clicked, to initiate DCC SEND
            {
// DCC SEND button clicked, to initiate DCC SEND
                timestamp_2_string();

                getmacro((Object*)WookieChat->PA_dcc_send_file, MUIA_String_Contents, &string1);
                getmacro((Object*)WookieChat->STR_dcc_send_nick, MUIA_String_Contents, &string2);

                if (!strcmp(string1, "") && !strcmp(string2, ""))
                {
                    sprintf(
                            buffer3,
                            "%s%sDCC%s %s",
                            timestamp,
							LGS( MSG_OPENING_BRACKET ),
							LGS( MSG_CLOSING_BRACKET ),
							LGS( MSG_DCC_SEND_MESSAGE ) );
                    add_text_to_current_list(buffer3, 9, ACTIVITY_CHAT);
                }
                else if (!strcmp(string1, ""))
                {
					sprintf(buffer3, "%s%sDCC%s %s", timestamp,
							LGS( MSG_OPENING_BRACKET ),
							LGS( MSG_CLOSING_BRACKET ),
							LGS( MSG_DCC_SEND_MESSAGE2 ) );
                    add_text_to_current_list(buffer3, 9, ACTIVITY_CHAT);
                }
                else if (!strcmp(string2, ""))
                {
					sprintf(buffer3, "%s%sDCC%s %s", timestamp,
							LGS( MSG_OPENING_BRACKET ),
							LGS( MSG_CLOSING_BRACKET ),
							LGS( MSG_DCC_SEND_MESSAGE3 ) );
                    add_text_to_current_list(buffer3, 9, ACTIVITY_CHAT);
                }
                else if (string1 && string2)
                {

                    char work1[6];
                    strcpy(work1, "SEND");
                    offer_dcc(status_current->networkname, work1, string2, string1, 1025);
                    setmacro((Object*)WookieChat->WI_dcc_send, MUIA_Window_Open, FALSE);

                }
//
            }
            else if (result == 79) //reoffer dcc transfer
            {
// Reoffer DCC SEND transfer
                struct dcc_entry *find = NULL;
                DoMethod((Object*) WookieChat->LV_send_dcc, MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, &find);
                LONG a = 0;

                dcc_send_conductor = dcc_send_root->next;

                while (find && dcc_send_conductor)
                {

                    if (find && dcc_send_conductor->entry && !dcc_send_conductor->removed)
                    {
                        if ( //!stricmp(find->status,dcc_send_conductor->entry->status) &&
                        !stricmp(find->nick, dcc_send_conductor->entry->nick)
                                && !stricmp(find->filename, dcc_send_conductor->entry->filename)
                                && !stricmp(find->port, dcc_send_conductor->entry->port))
                        {

                            dcc_send_conductor = dcc_send_root;
                            while (dcc_send_conductor)
                            {
                                if (!stricmp(status_conductor->networkname, dcc_send_conductor->networkname))
                                {
                                    offer_dcc(dcc_send_conductor->networkname, (char*) "SEND", dcc_send_conductor->nick,
                                            dcc_send_conductor->filepath, atoi(dcc_send_conductor->port));

                                    break;

                                }
                                dcc_send_conductor = dcc_send_conductor->next;
                                if (!dcc_send_conductor)
                                {
                                    printf("no valid network name\n");
                                    break;
                                }
                            }

                            //reoffer_dcc(dcc_send_conductor->networkname, "SEND", dcc_send_conductor->nick, dcc_send_conductor->filepath,atoi(dcc_send_conductor->port));

                        }

                        a++;

                    }

                    dcc_send_conductor = dcc_send_conductor->next;

                }

                dcc_send_conductor = dcc_send_root->next;
                a = 0;
//
            }
            else if (result == 80) // open the outgoing DCC window
            {
// Open the outgoing DCC SEND window
                setmacro((Object*)WookieChat->WI_dcc2, MUIA_Window_Activate, FALSE);
                setmacro((Object*)WookieChat->WI_dcc2, MUIA_Window_Open, TRUE);
#ifdef __AROS__
                setmacro((Object*)WookieChat->WI_dcc2, MUIA_Window_DefaultObject, WookieChat->LV_send_dcc);
                setmacro((Object*)WookieChat->WI_dcc2, MUIA_Window_ActiveObject, WookieChat->LV_send_dcc);
#endif
//
            }
            else if (result == 81) //add an alias
            {
                //getmacro((Object*)WookieChat->STR_add_alias,MUIA_String_Contents,&work_buffer);
                sprintf(work_buffer, "alias command");
                DoMethod((Object*) WookieChat->LV_alias, MUIM_NList_InsertSingle, work_buffer,
                        MUIV_NList_Insert_Bottom);
            }
            else if (result == 82) //remove an alias
            {
                DoMethod((Object*) WookieChat->LV_alias, MUIM_NList_Remove, MUIV_NList_Remove_Selected);
            }
            else if (result == 89) //edit an alias entry in LV_alias listview
            {
// Edit an alias entry in LV_alias listview
                DoMethod((Object*) WookieChat->LV_alias, MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, &string1);
                if (string1)
                {
                    strcpy(work_buffer, string1);
                    string1 = strtok(work_buffer, " ");
                    string2 = strtok(NULL, "");
                    if (string1 && string2)
                    {
                        setmacro((Object*)WookieChat->STR_edit_alias_name, MUIA_String_Contents, string1);
                        setmacro((Object*)WookieChat->STR_edit_alias_command, MUIA_String_Contents, string2);
                    }
                    else if (string1)
                    {
                        setmacro((Object*)WookieChat->STR_edit_alias_name, MUIA_String_Contents, string1);
                        setmacro((Object*)WookieChat->STR_edit_alias_command, MUIA_String_Contents, NULL);
                    }

                    /*setmacro((Object*)WookieChat->STR_edit_alias,MUIA_String_Contents, string1);
                     setmacro((Object*)WookieChat->WI_edit_alias,MUIA_Window_Open, TRUE);
                     setmacro((Object*)WookieChat->WI_edit_alias,MUIA_Window_ActiveObject,(Object*)WookieChat->STR_edit_alias);
                     strcpy(old_alias_entry, string1);*/
                }
//
            }
			else if (result == MUIV_Application_ReturnID_Quit) //user selected "yes" to quit
            {
// user selected yes to quit
                running = FALSE;
                status_conductor = status_root;

                if (my_settings.quit_msg)
                    sprintf(buffer3, "QUIT :%s\r\n", my_settings.quit_msg);
                else
                    sprintf(buffer3, "QUIT :Client Exiting\r\n");

                while (status_conductor)
                {
                    if (status_conductor->connection_active)
                        send_text(buffer3);
                    status_conductor = status_conductor->next;

                }
                //if(DEBUG) printf("quitting..\n");

                if (running == FALSE)
                    break;
//
            }
            else if (result == 91) //accept the alias edit change
            {
// Accept the alias edit change

                DoMethod((Object*) WookieChat->LV_alias, MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, &string1);
                if (string1)
                    strcpy(old_alias_entry, string1);

                getmacro((Object*)WookieChat->STR_edit_alias_name, MUIA_String_Contents, &string1);
                getmacro((Object*)WookieChat->STR_edit_alias_command, MUIA_String_Contents, &string2);

                for (int count = 0;; count++)
                {

                    DoMethod((Object*) WookieChat->LV_alias, MUIM_NList_GetEntry, count, &string3);

                    if (string3)
                    {
                        if (string1 && string2)
                            sprintf(work_buffer, "%s %s", string1, string2);
                        else if (string1)
                            sprintf(work_buffer, "%s empty", string1);

                        if (!stricmp(old_alias_entry, string3))
                            DoMethod((Object*) WookieChat->LV_alias, MUIM_NList_ReplaceSingle, work_buffer, count, NULL,
                                    ALIGN_LEFT);
                        //printf("old entry:%s size:%d\nstring3:%s size:%d\n",old_alias_entry,strlen(old_alias_entry),string3,strlen(string3));

                    }

                    if (!string3)
                        break;

                }

                //setmacro((Object*)WookieChat->WI_edit_alias,MUIA_Window_Open, FALSE);
//
            }
            else if (result == 92) //nlist tab click
            {
                getmacro((Object*)WookieChat->LV_tabs, MUIA_NList_EntryClick, &e);
                switch_between_tabs_with_nlisttabclick(e, 0);
            }
            else if (result == 94) //toggle column marking clipboard
            {
// Toggle between the column marking style clipboard and the built in nlist one
                if (my_settings.which_clipboard_style == COLUMNS)
                    my_settings.which_clipboard_style = NORMAL;
                else
                    my_settings.which_clipboard_style = COLUMNS;
                set_channel_clipboard_hook();
//
            }
            else if (result == 95) //open or close the graphical smiley chooser window
            {
// Open or close the graphical smiley chooser window
                if (is_chooser_window_open == TRUE)
                {
                    setmacro((Object*)WookieChat->WI_graphical_smileys_choose, MUIA_Window_Open, FALSE);
                    is_chooser_window_open = FALSE;
                }
                else
                {
                    setmacro((Object*)WookieChat->WI_graphical_smileys_choose, MUIA_Window_Open, TRUE);
                    is_chooser_window_open = TRUE;
                }
//
            }
            else if (result == 96) //double click in LV_channel. Lets see if its a URL we can visit!
            {
// double click in LV_channel. Lets see if its a URL we can visit!
                struct MUI_NList_TestPos_Result last_clicked_res;
                DoMethod((Object*) status_current->current_query->LV_channel, MUIM_NList_TestPos, MUI_MAXMAX,
                        MUI_MAXMAX, &last_clicked_res);

                DoMethod((Object*) status_current->current_query->LV_channel, MUIM_NList_GetEntry,
                        last_clicked_res.entry, &string1);

                if (string1)
                {
                    strcpy(work_buffer, string1);

                    if (DEBUG)
                        printf("double clicked buffer:%s\n", work_buffer);

                    //strtok(work_buffer,"]:");
                    //string1=strtok(NULL,"");
                    //undo the above for old working double click code
                    string1 = strtok(work_buffer, "");

                    if (string1)
                        string1 = doubleclick_url_action(string1, last_clicked_res.char_number,
                                last_clicked_res.column);
                    if (string1)
                    {
                        if (!strcmp(my_settings.browser, ""))
                        {
                            sprintf(
                                    buffer3,
                                    "%s%s%s%s %s",
                                    timestamp,
									LGS( MSG_OPENING_BRACKET ),
									LGS( MSG_ERROR ),
									LGS( MSG_CLOSING_BRACKET ),
									LGS( MSG_NO_BROWSER_SELECTED ) );

                            status_conductor = status_current;
                            status_conductor->conductor = status_conductor->current_query;

                            add_text_to_conductor_list(buffer3, 9, ACTIVITY_CHAT);

                        }
                        else
                        {

                            if (DEBUG)
                                printf("end result, now visiting %s\n", string1);

                            sprintf(file_name, "run >nil: %s \"%s\"", my_settings.browser, string1);

#ifdef __amigaos4__
                            SystemTags((_s_cs)file_name,TAG_DONE);
#else
                            Execute((_s_cs)file_name, 0, 0);
#endif

//[13:20:14] <jahc> look at this picture of NeRP http://tinyurl.com/2d9z3a
//[13:23:22] <spotUP> released some mins ago: http://www.os4depot.net/index.php?function=showfile&file=demo/music/planethively.lha

                        }
                    }

                }



//
            }
            else if (result == 97)
            {
// Toggle between single column and multicolumn chat displays
                if (my_settings.use_column_display == 1)
                {
                    setmacro((Object*)status_current->current_query->LV_channel, MUIA_NList_DisplayHook,
                            &DisplayLI_channel_TextHook);
                    setmacro((Object*)status_current->current_query->LV_channel, MUIA_NList_Format, "");
                    my_settings.use_column_display = 0;
                    setmacro((Object*)MN_MultiColumnDisplay, MUIA_Menuitem_Checked, FALSE);
                }
                else
                {
                    setmacro((Object*)status_current->current_query->LV_channel, MUIA_NList_DisplayHook,
                            &DisplayLI2_channel_TextHook);
                    setmacro((Object*)status_current->current_query->LV_channel, MUIA_NList_Format, listview_format);
                    my_settings.use_column_display = 1;
                    setmacro((Object*)MN_MultiColumnDisplay, MUIA_Menuitem_Checked, TRUE);
                }
//
            }
            else if (result == 99) //open the "graphical smileys theme" preview window
            {
// open the "graphical smileys theme" preview window

                setmacro((Object*)WookieChat->WI_graphical_smileys_preview, MUIA_Window_Open, TRUE);
                DoMethod((Object*) WookieChat->LV_graphical_smileys_preview, MUIM_NList_Clear);

                LONG preview_number;
                getmacro((Object*)WookieChat->graphical_smileys_set, MUIA_Cycle_Active, &preview_number);

                sprintf(work_buffer, "progdir:smilies/%s", graphical_smiley_themes[preview_number]);
                newbptr_file = Open((_s_cs)work_buffer, MODE_OLDFILE);

                if (newbptr_file)
                {

                    delete_preview_smiley_objects();

                    preview_total_smileys = 0;

                    while (FGets(newbptr_file, (STRPTR) work_buffer, 900))
                    {
                        if (work_buffer[0] != '#')
                        {
                            preview_total_smileys++;
                            //printf("line %s",work_buffer);
                            string1 = strtok(work_buffer, " ");
                            if (string1)
                            {
                                sprintf(preview_smilies[preview_total_smileys].filename, "%s", string1);

#ifdef __amigaos4__
                                preview_smilies[preview_total_smileys].icon=(Object*)NBitmapFile((char*)preview_smilies[preview_total_smileys].filename);
#else
                                preview_smilies[preview_total_smileys].icon=(Object*)MUI_NewObject((char*)MUIC_DTPIC, MUIA_Dtpic_Name, preview_smilies[preview_total_smileys].filename, End;
#endif

                                        if (preview_smilies[preview_total_smileys].icon)
                                            DoMethod((Object*) WookieChat->LV_graphical_smileys_preview,
                                                    MUIM_NList_UseImage, preview_smilies[preview_total_smileys].icon,
                                                    preview_total_smileys, 0L);

                                    }
                                    preview_smilies[preview_total_smileys].ascii_total = 0;
                                    string1 = strtok(NULL, "\n ");
                                    while (string1)
                                    {
                                        sprintf(
                                                preview_smilies[preview_total_smileys].ascii[preview_smilies[preview_total_smileys].ascii_total],
                                                "%s ", string1);
                                        if (preview_smilies[preview_total_smileys].ascii_total == 0)
                                            sprintf(preview_smilies[preview_total_smileys].remove_ascii, "%s", string1);
                                        preview_smilies[preview_total_smileys].ascii_total++;
                                        string1 = strtok(NULL, "\n ");
                                        if (preview_smilies[preview_total_smileys].ascii_total >= MAXIMUM_SMILEY_ASCII)
                                        {
                                            break;
                                        }

                                    }
                                    if (preview_total_smileys >= MAXIMUM_SMILEYS)
                                        break;
                                }
                            }

                            Close(newbptr_file);

                        }

                        for (int count4 = 1; count4 <= preview_total_smileys && count4 <= MAXIMUM_SMILEYS; count4++)
                        {
                            sprintf(work_buffer, "\033o[%lu] ", count4);

#ifdef __AROS__
                            strcpy(centry->entry, "        :");
#else
                            strcpy(centry->entry,"          ");
#endif

                            centry->colour = 8;

                            for (int count3 = 0;
                                    count3 < preview_smilies[count4].ascii_total && count3 <= MAXIMUM_SMILEY_ASCII;
                                    count3++)
                            {
                                strcat(work_buffer, preview_smilies[count4].ascii[count3]);

                            }
                            strcat(centry->entry, work_buffer);

                            DoMethod((Object*) WookieChat->LV_graphical_smileys_preview, MUIM_NList_InsertSingle,
                                    centry, MUIV_NList_Insert_Bottom);

                        }

//
                    }
                    else if (result == 100) //rename the incoming dcc transfer because it already exists
                    {
                        LONG a = 0;
                        struct dcc_entry *find = NULL;
                        DoMethod((Object*) WookieChat->LV_dcc, MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, &find);

                        dcc_conductor = dcc_root->next;

                        while (dcc_conductor && find)
                        {

                            if (!dcc_conductor->removed)
                            {
                                if (find && dcc_conductor->entry)
                                {
                                    if ( //!stricmp(find->status,dcc_conductor->entry->status) &&
                                    !stricmp(find->nick, dcc_conductor->entry->nick)
                                            && !stricmp(find->filename, dcc_conductor->entry->filename)
                                            && !stricmp(find->port, dcc_conductor->entry->port)) //experimental, adding port string to entry struct
                                    {

                                        if (dcc_conductor->entry)
                                        {
                                            //printf("renaming incoming file\n");

                                            getmacro((Object*)WookieChat->WI_main, MUIA_Window_Window, &my_window);

#ifdef __amigaos4__
                                            filerequester=(struct FileRequester*)AllocAslRequestTags(ASL_FileRequest,ASLFR_Window,my_window,ASLFR_PopToFront,TRUE,ASLFR_DoPatterns, TRUE,ASLFR_DoMultiSelect,FALSE,ASLFR_InitialDrawer,my_settings.download_path,ASLFR_InitialFile,dcc_conductor->entry->filename,TAG_DONE, NULL);
#elif __MORPHOS__
                                            filerequester=(struct FileRequester*)AllocAslRequestTags(ASL_FileRequest,ASLFR_Window,my_window,ASLFR_PopToFront,TRUE,ASLFR_DoPatterns, TRUE,ASLFR_DoMultiSelect,FALSE,ASLFR_InitialDrawer,my_settings.download_path,ASLFR_InitialFile,dcc_conductor->entry->filename,TAG_DONE, NULL);
#else
                                            filerequester = (struct FileRequester*) AllocAslRequestTags(ASL_FileRequest,
                                                    ASLFR_DoPatterns, TRUE, ASLFR_DoMultiSelect, FALSE,
                                                    ASLFR_InitialDrawer, my_settings.download_path, ASLFR_InitialFile,
                                                    dcc_conductor->entry->filename, TAG_DONE, NULL);
#endif

                                            aslresult = AslRequest(filerequester, NULL);
                                            if (aslresult)
                                            {
                                                //printf("aslrequest successful\n");
                                                //printf("renaming incoming file 2\n");

                                                strncpy(dcc_conductor->filename, (char *)filerequester->fr_File, 499);
                                                dcc_conductor->filename[499] = '\0';
                                                strncpy(dcc_conductor->filepath, (char *)filerequester->fr_Drawer, 499);
                                                dcc_conductor->filepath[499] = '\0';
                                                strncpy(dcc_conductor->entry->filename, (char *)filerequester->fr_File, 499);
                                                dcc_conductor->entry->filename[499] = '\0';

                                                DoMethod((Object*) WookieChat->LV_dcc, MUIM_NList_ReplaceSingle,
                                                        dcc_conductor->entry, a, NOWRAP, ALIGN_LEFT);
                                                dcc_conductor->total_recv = 0;

                                                accept_dcc((char *)filerequester->fr_Drawer);

                                            }
                                        }

                                        setmacro((Object*)WookieChat->WI_dcc_file_exists, MUIA_Window_Open, FALSE);

                                        break;

                                    }

                                    a++;

                                }
                            }

                            dcc_conductor = dcc_conductor->next;

                        }

                    }
                    else if (result >= SECOND_SET_OF_RETURNIDS && result < AREXX_MENU_VALUES)
                    {
                        result = result - SECOND_SET_OF_RETURNIDS;

                        if (result == 1)
                        {
                            if (is_sound_muted() == FALSE)
                            {
                                setmacro((Object*)MN_MuteSound, MUIA_Menuitem_Checked, TRUE);
                                set_sound_muted(TRUE);
                            }
                            else
                            {
                                setmacro((Object*)MN_MuteSound, MUIA_Menuitem_Checked, FALSE);
                                set_sound_muted(FALSE);
                            }

                        }
                        else if (result == 2) //user is typing in the User List Buttons config
                        {
                            getmacro((Object*)WookieChat->STR_user_list_buttons_name, MUIA_String_Contents, &string1);
                            getmacro((Object*)WookieChat->STR_user_list_buttons_command, MUIA_String_Contents,
                                    &string2);

                            if (string1)
                            {
//#define USERLIST_NAME_SIZE 40
//#define USERLIST_COMMAND_SIZE 800
                                ULONG entries;
                                stccpy(work_buffer, string1, USERLIST_NAME_SIZE);
                                strcat(work_buffer, "\033");
                                //sprintf(work_buffer,"%s\033",string1);
                                if (string2)
                                {
                                    strcat(work_buffer, string2);
                                }

                                //printf("String1:%s string2:%s\n",string1,string2);

                                DoMethod((Object*) WookieChat->LV_user_list_buttons, MUIM_NList_GetEntry,
                                        MUIV_NList_GetEntry_Active, &string3);
                                getmacro((Object*)WookieChat->LV_user_list_buttons, MUIA_NList_Entries, &entries);
                                if (string3)
                                {
                                    for (unsigned int count = 0; count <= entries; count++)
                                    {
                                        DoMethod((Object*) WookieChat->LV_user_list_buttons, MUIM_NList_GetEntry, count,
                                                &string4);
                                        if (string4)
                                        {
                                            //printf("string4:%s\n",string4);

                                            if (!stricmp(string3, string4))
                                            {
                                                DoMethod((Object*) WookieChat->LV_user_list_buttons,
                                                        MUIM_NList_ReplaceSingle, work_buffer, count, NOWRAP,
                                                        ALIGN_LEFT);
                                                break;
                                            }
                                        }

                                    }
                                }

                            }

                        }
                        else if (result == 3) //user clicked on an entry in the User List Buttons config
                        {

                            DoMethod((Object*) WookieChat->LV_user_list_buttons, MUIM_NList_GetEntry,
                                    MUIV_NList_GetEntry_Active, &string1);
                            if (string1)
                            {
                                strcpy(work_buffer, string1);
                                string1 = strtok(work_buffer, "\033");
                                string2 = strtok(NULL, "");

                                if (string1)
                                    setmacro((Object*)WookieChat->STR_user_list_buttons_name, MUIA_String_Contents,
                                            string1);
                                else
                                    setmacro((Object*)WookieChat->STR_user_list_buttons_name, MUIA_String_Contents, "");
                                if (string2)
                                    setmacro((Object*)WookieChat->STR_user_list_buttons_command, MUIA_String_Contents,
                                            string2);
                                else
                                    setmacro((Object*)WookieChat->STR_user_list_buttons_command, MUIA_String_Contents,
                                            "");

                            }

                        }

                        else if (result == 4) //User List Buttons config: Add New Entry
                        {
                            ULONG entries;
                            getmacro((Object*)WookieChat->LV_user_list_buttons, MUIA_NList_Entries, &entries);
                            if (entries < MAX_BUTTONS)
                            {
                                strcpy(work_buffer, "NEW\033*EDIT ME*");
                                DoMethod((Object*) WookieChat->LV_user_list_buttons, MUIM_NList_InsertSingle,
                                        work_buffer, MUIV_NList_Insert_Bottom);
                            }
                        }
                        else if (result == 5) //User List Buttons config: Delete Entry
                        {

                            DoMethod((Object*) WookieChat->LV_user_list_buttons, MUIM_NList_Remove,
                                    MUIV_NList_Remove_Active);

                        }
                        else if (result == 6) //User List Buttons config: Move Entry Up
                        {
                            ULONG entries;
                            DoMethod((Object*) WookieChat->LV_user_list_buttons, MUIM_NList_GetEntry,
                                    MUIV_NList_GetEntry_Active, &string3);
                            getmacro((Object*)WookieChat->LV_user_list_buttons, MUIA_NList_Entries, &entries);
                            if (string3)
                            {
                                for (unsigned int count = 0; count <= entries; count++)
                                {
                                    DoMethod((Object*) WookieChat->LV_user_list_buttons, MUIM_NList_GetEntry, count,
                                            &string4);
                                    if (string4)
                                    {
                                        if (!stricmp(string3, string4))
                                        {
                                            if (count == 0)
                                                break;
                                            DoMethod((Object*) WookieChat->LV_user_list_buttons, MUIM_NList_Exchange,
                                                    count, count - 1);
                                            break;
                                        }
                                    }

                                }
                            }

                        }
                        else if (result == 7) //User List Buttons config: Move Entry Down
                        {
                            ULONG entries;
                            DoMethod((Object*) WookieChat->LV_user_list_buttons, MUIM_NList_GetEntry,
                                    MUIV_NList_GetEntry_Active, &string3);
                            getmacro((Object*)WookieChat->LV_user_list_buttons, MUIA_NList_Entries, &entries);
                            if (string3)
                            {
                                for (unsigned int count = 0; count <= entries; count++)
                                {
                                    DoMethod((Object*) WookieChat->LV_user_list_buttons, MUIM_NList_GetEntry, count,
                                            &string4);
                                    if (string4)
                                    {
                                        if (!stricmp(string3, string4))
                                        {
                                            if (count == entries)
                                                break;
                                            DoMethod((Object*) WookieChat->LV_user_list_buttons, MUIM_NList_Exchange,
                                                    count, count + 1);
                                            break;
                                        }
                                    }

                                }
                            }

                        }
                        else if (result == 8) //Change nick window: user has clicked Change
                        {

                            getmacro((Object*)WookieChat->STR_change_nick, MUIA_String_Contents, &string1);
                            if (string1)
                            {
                                status_conductor = status_current;
                                if (stricmp(string1, ""))
                                    ChangeMyNick(string1);
                            }
                            setmacro((Object*)WookieChat->WI_change_nick, MUIA_Window_Open, FALSE);
                            //setmacro((Object*)WookieChat->WI_change_nick,MUIA_Window_ActiveObject,(Object*)WookieChat->STR_change_nick);

                        }
                        else if (result == 9) //user clicked on an entry in the EVENTS config
                        {

                            DoMethod((Object*) LV_events, MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, &string1);
                            if (string1)
                            {
                                strcpy(work_buffer, string1);

                                strtok(work_buffer, "\033");
                                string1 = strtok(NULL, " ");
                                string2 = strtok(NULL, "\033");
                                string3 = strtok(NULL, "");

                                if (string1)
                                    setmacro((Object*)CYCLE_events, MUIA_Cycle_Active, atoi(string1));
                                else
                                    setmacro((Object*)CYCLE_events, MUIA_Cycle_Active, 0);

                                if (string2)
                                    setmacro((Object*)STR_events_script, MUIA_String_Contents, string2);
                                else
                                    setmacro((Object*)STR_events_script, MUIA_String_Contents, "");

                                if (string3)
                                    setmacro((Object*)STR_events_args, MUIA_String_Contents, string3);
                                else
                                    setmacro((Object*)STR_events_args, MUIA_String_Contents, "");

                            }

                        }
                        else if (result == 10) //user is typing in the EVENTS config
                        {
                            getmacro((Object*)STR_events_script, MUIA_String_Contents, &string1);
                            getmacro((Object*)STR_events_args, MUIA_String_Contents, &string2);

                            ULONG active_event;
                            getmacro((Object*)CYCLE_events, MUIA_Cycle_Active, &active_event);

                            char insert_string2[800];

                            if (string1)
                            {
                                ULONG entries;
                                sprintf(work_buffer, "%lu %s\033", active_event, string1);

                                if (string2)
                                {
                                    strcat(work_buffer, string2);
                                }

                                //printf("String1:%s string2:%s\n",string1,string2);

                                DoMethod((Object*) LV_events, MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active,
                                        &string3);
                                getmacro((Object*)LV_events, MUIA_NList_Entries, &entries);
                                if (string3)
                                {
                                    for (unsigned int count = 0; count <= entries; count++)
                                    {
                                        DoMethod((Object*) LV_events, MUIM_NList_GetEntry, count, &string4);
                                        if (string4)
                                        {
                                            //printf("string4:%s\n",string4);

                                            if (!stricmp(string3, string4))
                                            {
                                                if (count == 0)
                                                    strcpy(insert_string2, "Highlight\033");
                                                else if (count == 1)
                                                    strcpy(insert_string2, "Connect\033");
                                                else if (count == 2)
                                                    strcpy(insert_string2, "Disconnect\033");
                                                else if (count == 3)
                                                    strcpy(insert_string2, "Join\033");
                                                else if (count == 4)
                                                    strcpy(insert_string2, "Part\033");
                                                else if (count == 5)
                                                    strcpy(insert_string2, "Quit\033");
                                                else if (count == 6)
                                                    strcpy(insert_string2, "DCC RECV Offered\033");
                                                else if (count == 7)
                                                    strcpy(insert_string2, "DCC RECV Finished\033");
                                                else if (count == 8)
                                                    strcpy(insert_string2, "DCC SEND Finished\033");
                                                else if (count == 9)
                                                    strcpy(insert_string2, "Channel Message\033");
                                                else if (count == 10)
                                                    strcpy(insert_string2, "Private Message\033");
                                                else if (count == 11)
                                                    strcpy(insert_string2, "Notice\033");
                                                else if (count == 12)
                                                    strcpy(insert_string2, "CTCP Request\033");
                                                else if (count == 13)
                                                    strcpy(insert_string2, "Kick\033");
                                                else if (count == 14)
                                                    strcpy(insert_string2, "Mode Change\033");
                                                else if (count == 15)
                                                    strcpy(insert_string2, "Invite\033");

                                                else
                                                    strcpy(insert_string2, "(blank)\033");

                                                strcat(insert_string2, work_buffer);

                                                DoMethod((Object*) LV_events, MUIM_NList_ReplaceSingle, insert_string2,
                                                        count, NOWRAP, ALIGN_LEFT);
                                                break;
                                            }
                                        }

                                    }
                                }

                            }

                        }
                        else if (result == 11) //Edit Menu Item, cut
                        {
                            if (DEBUG)
                                printf("cut\n");
                            //DoMethod((Object*)WookieChat->STR_usertext, MUIM_BetterString_DoAction, MUIV_BetterString_DoAction_Cut);

                            Object *active;
                            getmacro((Object*)WookieChat->WI_main, MUIA_Window_ActiveObject, &active);
                            if (active)
                                DoMethod((Object*) active, MUIM_BetterString_DoAction, MUIV_BetterString_DoAction_Cut);

                        }
                        else if (result == 12) //Edit Menu Item, copy
                        {
                            if (DEBUG)
                                printf("copy\n");
                            //DoMethod((Object*)WookieChat->STR_usertext, MUIM_BetterString_DoAction, MUIV_BetterString_DoAction_Copy);

                            Object *active;
                            getmacro((Object*)WookieChat->WI_main, MUIA_Window_ActiveObject, &active);
                            if (active)
                                DoMethod((Object*) active, MUIM_BetterString_DoAction, MUIV_BetterString_DoAction_Copy);

                        }
                        else if (result == 13) //Edit Menu Item, paste
                        {
                            if (DEBUG)
                                printf("paste\n");
                            Object *active;
                            getmacro((Object*)WookieChat->WI_main, MUIA_Window_ActiveObject, &active);
                            if (active)
                                DoMethod((Object*) active, MUIM_BetterString_DoAction,
                                        MUIV_BetterString_DoAction_Paste);

                            /*getmacro(_win(WookieChat->STR_usertext), MUIA_Window_ActiveObject,&active);
                             if(active) DoMethod((Object*)WookieChat->STR_usertext, MUIM_BetterString_DoAction, MUIV_BetterString_DoAction_Paste);

                             getmacro(_win(status_current->current_query->STR_topic), MUIA_Window_ActiveObject,&active);
                             if(active) DoMethod((Object*)status_current->current_query->STR_topic, MUIM_BetterString_DoAction, MUIV_BetterString_DoAction_Paste);

                             getmacro(_win(status_current->current_query->STR_keyword), MUIA_Window_ActiveObject,&active);
                             if(active) DoMethod((Object*)status_current->current_query->STR_keyword, MUIM_BetterString_DoAction, MUIV_BetterString_DoAction_Paste);
                             */

                            /*getmacro((Object*)WookieChat->STR_usertext,MUIA_Window_ActiveObject,&string1);
                             if(string1)
                             {
                             DoMethod((Object*)WookieChat->STR_usertext, MUIM_BetterString_DoAction, MUIV_BetterString_DoAction_Paste);
                             }
                             else if(status_current->current_query)
                             {
                             getmacro((Object*)status_current->current_query->STR_topic,MUIA_Window_ActiveObject,&string1);
                             if(string1)
                             {
                             DoMethod((Object*)status_current->current_query->STR_topic, MUIM_BetterString_DoAction, MUIV_BetterString_DoAction_Paste);
                             }
                             else
                             {
                             getmacro((Object*)status_current->current_query->STR_keyword,MUIA_Window_ActiveObject,&string1);
                             if(string1)
                             {
                             DoMethod((Object*)status_current->current_query->STR_keyword, MUIM_BetterString_DoAction, MUIV_BetterString_DoAction_Paste);
                             }
                             }
                             }*/
                        }
                        else if (result >= 100) //User List Button: CLicked!
                        {
                            result = result - 100;
                            //printf("result == %d\n",result);
                            char work_buffer[1000];
                            char work_buffer2[1000];

                            //printf("1work_buffer:%s\nbutton command:%s\n",work_buffer,buttons[result].command);

                            if (strstr(buttons[result].command, "%s") || strstr(buttons[result].command, "%h")) // if this code is selected, then we want to do a command on each selected nick
                            {

                                for (LONG id = MUIV_NList_NextSelected_Start;;)
                                {
                                    struct list_entry *work_entry = NULL;

                                    if (buttons[result].command[0] != '/')
                                        strcpy(work_buffer, "/");
                                    else
                                        strcpy(work_buffer, "");
                                    strcat(work_buffer, buttons[result].command);
                                    strcat(work_buffer, "\n");

                                    status_conductor = status_current;
                                    status_conductor->conductor = status_current->current_query;

                                    DoMethod((Object*) status_conductor->conductor->LV_nicklist,
                                            MUIM_NList_NextSelected, &id);

                                    DoMethod((Object*) status_conductor->conductor->LV_nicklist, MUIM_NList_GetEntry,
                                            id, &work_entry);
                                    if (id == MUIV_NList_NextSelected_End)
                                    {
                                        break;
                                    }
                                    //if(!work_entry) break;

                                    strcpy(work_buffer2, "");
                                    work_buffer[strlen(work_buffer) - 1] = '\0';
                                    //printf("2work_buffer:%s\nbutton command:%s\n",work_buffer,buttons[result].command+1);

                                    for (unsigned int count = 0, count2 = 0; count < strlen(work_buffer);
                                            count++, count2++)
                                    {
                                        work_buffer2[count2] = '\0';

                                        if (work_buffer[count] == '%')
                                        {
                                            count++;

                                            if (work_buffer[count] == '%')
                                            {
                                                work_buffer2[count2] = '%';
                                            }
                                            else if (work_buffer[count] == 's') //selected nick
                                            {
                                                strcat(work_buffer2, work_entry->name);
                                                count2 = strlen(work_buffer2) - 1;
                                            }
                                            else if (work_buffer[count] == 'h') //hostname selected nick
                                            {
                                                for (int count5 = 0; count5 < status_conductor->conductor->nicks;
                                                        count5++)
                                                {

                                                    if (!stricmp(status_conductor->conductor->nicklist[count5].name,
                                                            work_entry->name))
                                                    {

                                                        if (status_conductor->conductor->nicklist[count5].hostname)
                                                        {
                                                            strcat(
                                                                    work_buffer2,
                                                                    status_conductor->conductor->nicklist[count5].hostname);

                                                            count2 = strlen(work_buffer2) - 1;

                                                        }
                                                        break;
                                                    }
                                                }
                                            }

                                            else if (work_buffer[count] == 'c') //current channel
                                            {
                                                strcat(work_buffer2, status_conductor->conductor->name);
                                                count2 = strlen(work_buffer2) - 1;
                                            }
                                            else if (work_buffer[count] == 'n') //own nick nick
                                            {
                                                strcat(work_buffer2, status_conductor->nick);
                                                count2 = strlen(work_buffer2) - 1;
                                            }
                                        }
                                        else
                                        {
                                            work_buffer2[count2] = work_buffer[count];

                                        }
                                        work_buffer2[count2 + 1] = '\0';

                                    }

                                    process_outgoing(work_buffer2, 0);

                                    if (RECENTLY_CREATED_A_TAB == TRUE)
                                    {
                                        if (status_current)
                                        {
                                            if (status_current->current_query)
                                            {
                                                setmacro((Object*)WookieChat->LV_tabs, MUIA_NList_Active,
                                                        status_current->current_query->nlist_tab_number);
                                                sort_linked_list();
                                            }
                                        }

                                        RECENTLY_CREATED_A_TAB = FALSE;
                                    }

                                }
                            }
                            else //if we're not doing this action on every selected nick.. we'll just do it once
                            {
                                if (buttons[result].command[0] != '/')
                                    strcpy(work_buffer, "/");
                                else
                                    strcpy(work_buffer, "");
                                strcat(work_buffer, buttons[result].command);
                                strcat(work_buffer, "\n");

                                status_conductor = status_current;
                                status_conductor->conductor = status_current->current_query;

                                strcpy(work_buffer2, "");
                                work_buffer[strlen(work_buffer) - 1] = '\0';

                                for (unsigned int count = 0, count2 = 0; count < strlen(work_buffer); count++, count2++)
                                {
                                    work_buffer2[count2] = '\0';

                                    if (work_buffer[count] == '%')
                                    {
                                        count++;

                                        if (work_buffer[count] == '%')
                                        {
                                            work_buffer2[count2] = '%';
                                        }
#if 0
                                        // FIXME: This code was accessing global work_entry of unknow state!
                                        else if (work_buffer[count] == 's') //selected nick
                                        {
                                            strcat(work_buffer2, work_entry->name);
                                            count2 = strlen(work_buffer2) - 1;
                                        }
#endif
                                        else if (work_buffer[count] == 'c') //current channel
                                        {
                                            strcat(work_buffer2, status_conductor->conductor->name);
                                            count2 = strlen(work_buffer2) - 1;
                                        }
                                        else if (work_buffer[count] == 'n') //own nick nick
                                        {
                                            strcat(work_buffer2, status_conductor->nick);
                                            count2 = strlen(work_buffer2) - 1;
                                        }
                                    }
                                    else
                                    {
                                        work_buffer2[count2] = work_buffer[count];

                                    }
                                    work_buffer2[count2 + 1] = '\0';

                                }

                                //printf("2sending command:%s\n",work_buffer2);
                                process_outgoing(work_buffer2, 0);

                            }
                            result = 0;
                        }

                    }
                    else if (result > 100 && result < AREXX_MENU_VALUES)
                    {
                        switch_between_tabs(result);
                    }
                    else if (result == AREXX_MENU_VALUES) //rescan arexx scripts directory and add them to the arexx scripts pulldown menu
                    {
                        arexx_add_scripts_to_menu();
                    }
                    else if (result > AREXX_MENU_VALUES && result < GRAPHICAL_SMILEY_VALUES)
                    {
// Execute an AREXX script
                        int count = result - AREXX_MENU_VALUES;

                        sprintf(file_name, "run rx %s", arexx_get_menu_item_string(count));
                        if (DEBUG)
                            printf("%s\n", file_name);

                        BPTR arexx_dir_lock = Lock((_s_cs)"progdir:arexx_scripts", SHARED_LOCK);
                        if (arexx_dir_lock)
                        {
#ifdef __AROS__
                            struct TagItem my_arexx_command_taglist[] =
                            {
                                { NP_CurrentDir, (IPTR) arexx_dir_lock },
                                { TAG_DONE, 0 }
                            };
#else
                            struct TagItem my_arexx_command_taglist[] =
                            {
                                {   NP_CurrentDir,(ULONG)arexx_dir_lock},
                                {   TAG_DONE, (ULONG)0}
                            };
#endif
                            SystemTagList((_s_cs)file_name, my_arexx_command_taglist);
                        }
//
                    }
                    else if (result > GRAPHICAL_SMILEY_VALUES)
                    {
// Insert a graphical smiley ascii into the chat entry string gadget
                        char tabwork3_string[900]; //text after cursor position
                        char tabwork2_string[900]; //text before cursor position
                        char tabwork_string[900]; //original string gadget contents
                        char * ptr;
                        int count = result - GRAPHICAL_SMILEY_VALUES;

                        setmacro((Object*)WookieChat->WI_graphical_smileys_choose, MUIA_Window_Open, FALSE);
                        is_chooser_window_open = FALSE;

                        getmacro((Object*)WookieChat->STR_usertext, MUIA_String_BufferPos, &position);
                        getmacro((Object*)WookieChat->STR_usertext, MUIA_String_Contents, &ptr);

                        int a = 0;
                        int b = 0;

                        strncpy(tabwork_string, ptr, 900);

                        if (position != 0)
                        {
                            do
                            {
                                tabwork2_string[b] = tabwork_string[a];
                                b++;
                                a++;
                            } while (a <= (LONG) position);
                            b = b - 1;
                            tabwork2_string[b] = '\0';
                        }
                        else
                        {
                            b = 0;
                            tabwork2_string[b] = '\0';
                        }
                        a = position;
                        b = 0;

                        if (position != (LONG) strlen(tabwork_string))
                        {
                            do
                            {
                                tabwork3_string[b] = tabwork_string[a];
                                b++;
                                a++;
                            } while (a <= (LONG) strlen(tabwork_string));
                        }

                        tabwork3_string[b] = '\0';

                        sprintf(tabwork_string, "%s%s %s", tabwork2_string, smilies[count].remove_ascii,
                                tabwork3_string);
                        setmacro((Object*)WookieChat->STR_usertext, MUIA_String_Contents, tabwork_string);
                        setmacro((Object*)WookieChat->STR_usertext, MUIA_String_BufferPos,
                                position+strlen(smilies[count].ascii[0]));

                        setmacro((Object*)WookieChat->WI_main, MUIA_Window_ActiveObject,
                                (Object*)WookieChat->STR_usertext);
//

                    }

                    //MODE GADGET STUFF
// Mode gadgets have been clicked. Lets send the commands to the server now
                    else if (status_current)
                    {
                        status_conductor = status_current;

                        if (status_conductor && current_query)
                        {
                            status_conductor->conductor = status_conductor->current_query; //current_query;

                            if (result == 10) //mode t button hit
                            {
                                if (!status_conductor->conductor->mode_T)
                                {
                                    sprintf(sendstuff, "mode %s +t\r\n", status_conductor->conductor->name);
                                    send_text(sendstuff);
                                }
                            }

                            else if (result == 11) //mode n button hit
                            {
                                if (!status_conductor->conductor->mode_N)
                                {
                                    sprintf(sendstuff, "mode %s +n\r\n", status_conductor->conductor->name);
                                    send_text(sendstuff);
                                }
                            }

                            else if (result == 12) //mode s button hit
                            {
                                if (!status_conductor->conductor->mode_S)
                                {
                                    sprintf(sendstuff, "mode %s +s\r\n", status_conductor->conductor->name);
                                    send_text(sendstuff);
                                }
                            }

                            else if (result == 13) //mode i button hit
                            {
                                if (!status_conductor->conductor->mode_I)
                                {
                                    sprintf(sendstuff, "mode %s +i\r\n", status_conductor->conductor->name);
                                    send_text(sendstuff);
                                }
                            }

                            else if (result == 14) //mode p button hit
                            {
                                if (!status_conductor->conductor->mode_P)
                                {
                                    sprintf(sendstuff, "mode %s +p\r\n", status_conductor->conductor->name);
                                    send_text(sendstuff);
                                }
                            }

                            else if (result == 15) //mode m button hit
                            {
                                if (!status_conductor->conductor->mode_M)
                                {
                                    sprintf(sendstuff, "mode %s +m\r\n", status_conductor->conductor->name);
                                    send_text(sendstuff);
                                }
                            }

                            else if (result == 16) //toggle limit mode gadget hit
                            {
                                if (!status_conductor->conductor->mode_L)
                                {
                                    STRPTR work;
                                    getmacro((Object*)status_conductor->conductor->STR_limit, MUIA_String_Contents,
                                            &work);
                                    sprintf(sendstuff, "mode %s +l %s\r\n", status_conductor->conductor->name, work);
                                    send_text(sendstuff);
                                }
                            }

                            else if (result == 17) //toggle keyword mode gadget hit
                            {
                                if (!status_conductor->conductor->mode_K)
                                {

                                    STRPTR work;
                                    getmacro((Object*)status_conductor->conductor->STR_keyword, MUIA_String_Contents,
                                            &work);
                                    sprintf(sendstuff, "mode %s +k %s\r\n", status_conductor->conductor->name, work);
                                    send_text(sendstuff);
                                }
                            }
                            else if (result == 18) //get list of banmasks for current channel
                            {
                                sprintf(sendstuff, "mode %s b\r\n", status_conductor->conductor->name);
                                send_text(sendstuff);

                                DoMethod((Object*) WookieChat->LV_ban, MUIM_NList_Clear);
                                setmacro((Object*)WookieChat->WI_ban, MUIA_Window_Open, TRUE);

                                sprintf(ban_window_title, "%s %s",
										LGS( MSG_WINDOW_TITLE_BANMASKS ),
                                        status_conductor->conductor->name);

                                setmacro((Object*)WookieChat->WI_ban, MUIA_Window_Title, ban_window_title);

                            }
                            else if (result == 19) //"unban" button clicked for the banmask window
                            {

                                DoMethod((Object*) WookieChat->LV_ban, MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active,
                                        &string1);
                                string2 = strtok(string1, " ");
                                if (string2)
                                {
                                    sprintf(sendstuff, "mode %s -b %s\n", status_conductor->conductor->name, string2);
                                    send_text(sendstuff);

                                    sprintf(buffer3, "%s%sMode%s %s %s %s %s", timestamp,
											LGS( MSG_OPENING_BRACKET ),
											LGS( MSG_REMOVING_BAN ),
											LGS( MSG_CLOSING_BRACKET ),
											string2,
											LGS( MSG_REMOVING_BAN_FOR_CHANNEL ),
                                            status_conductor->conductor->name);
                                    add_text_to_current_list(buffer3, 5, ACTIVITY_CHAT);

                                }

                            }
                            else if (result == 20) //mode t button hit
                            {
                                if (status_conductor->conductor->mode_T)
                                {
                                    status_conductor->conductor->mode_T = 0;
                                    sprintf(sendstuff, "mode %s -t\r\n", status_conductor->conductor->name);
                                    send_text(sendstuff);
                                }
                            }

                            else if (result == 21) //mode n button hit
                            {
                                if (status_conductor->conductor->mode_N)
                                {
                                    status_conductor->conductor->mode_N = 0;
                                    sprintf(sendstuff, "mode %s -n\r\n", status_conductor->conductor->name);
                                    send_text(sendstuff);
                                }
                            }

                            else if (result == 22) //mode s button hit
                            {
                                if (status_conductor->conductor->mode_S)
                                {
                                    status_conductor->conductor->mode_S = 0;
                                    sprintf(sendstuff, "mode %s -s\r\n", status_conductor->conductor->name);
                                    send_text(sendstuff);
                                }
                            }

                            else if (result == 23) //mode i button hit
                            {
                                if (status_conductor->conductor->mode_I)
                                {
                                    status_conductor->conductor->mode_I = 0;
                                    sprintf(sendstuff, "mode %s -i\r\n", status_conductor->conductor->name);
                                    send_text(sendstuff);
                                }
                            }

                            else if (result == 24) //mode p button hit
                            {
                                if (status_conductor->conductor->mode_P)
                                {
                                    status_conductor->conductor->mode_P = 0;
                                    sprintf(sendstuff, "mode %s -p\r\n", status_conductor->conductor->name);
                                    send_text(sendstuff);
                                }
                            }

                            else if (result == 25) //mode m button hit
                            {
                                if (status_conductor->conductor->mode_M)
                                {
                                    status_conductor->conductor->mode_M = 0;
                                    sprintf(sendstuff, "mode %s -m\r\n", status_conductor->conductor->name);
                                    send_text(sendstuff);
                                }
                            }

                            else if (result == 27) //toggle limit mode gadget
                            {
                                if (DEBUG)
                                    printf("1:trying to remove limit mode\n");
                                if (status_conductor->conductor->mode_L)
                                {
                                    status_conductor->conductor->mode_L = 0;
                                    if (DEBUG)
                                        printf("2:trying to remove limit mode\n");
                                    sprintf(sendstuff, "mode %s -l\r\n", status_conductor->conductor->name);
                                    send_text(sendstuff);
                                }
                            }

                            else if (result == 28) //toggle keyword mode gadget
                            {
                                if (status_conductor->conductor->mode_K)
                                {
                                    status_conductor->conductor->mode_K = 0;
                                    STRPTR work;
                                    getmacro((Object*)status_conductor->conductor->STR_keyword, MUIA_String_Contents,
                                            &work);
                                    sprintf(sendstuff, "mode %s -k %s\r\n", status_conductor->conductor->name, work);
                                    send_text(sendstuff);
                                }
                            }

                            else if (result == 29) //set topic
                            {
                                STRPTR work;
                                getmacro((Object*)status_conductor->conductor->STR_topic, MUIA_String_Contents, &work);
                                sprintf(sendstuff, "topic %s :%s\r\n", status_conductor->conductor->name, work);
                                send_text(sendstuff);
                                //printf("work:%s\n",work);
                            }
                            result = 0;
                        }

                    }
//

                }

                waitsignals = signal | SIGBREAKF_CTRL_C | timer_signal | arexx_wants_to_send_signal;

                read_fds = read_master;
                write_fds = write_master;
                select_result = 0;

                if (running && signal)
					select_result = WaitSelect(fdmax + 1, &read_fds, &write_fds, NULL, NULL, &waitsignals);

                if (waitsignals & arexx_wants_to_send_signal) //lets send some text
                {
                    if (USE_AREXX == TRUE)
                    {
                        struct XYMessage *incoming_message = NULL;
                        while ((incoming_message = (struct XYMessage*) GetMsg(send_text_replyport)))
                        {
                            if (incoming_message->xy_Sendtext == 1)
                            {
                                if (DEBUG)
                                    printf("send 2 : recieving - command:%s\n", string_to_send2);
                                process_outgoing(string_to_send2, 0);
                                strcpy(string_to_send2, "");
                                //printf("got the send text signal!\n");

                                ReplyMsg((struct Message*) incoming_message);
                                Delay(1);
                                //printf("replied to send text signal!\n");

                            }

                        }
                    }
                }
                if (waitsignals & timer_signal)
                {
                    struct timeval systime;
// Update the dcc transfers window entries every 2 seconds!

                    while (getmsg_2s_delay())
                    {

                        dcc_conductor = dcc_root->next;

                        LONG a = 0;

                        while (dcc_conductor)
                        {

                            if (!dcc_conductor->removed && !dcc_conductor->completed)
                            {
                                if (dcc_conductor->entry)
                                {
                                    ULONG mins, hrs;
                                    //Amount Transferred
                                    if (dcc_conductor->total_recv > 0)
                                        sprintf(dcc_conductor->entry->bytes_transferred, "%lukB",
                                                dcc_conductor->total_recv / 1024);
                                    else
                                        sprintf(dcc_conductor->entry->bytes_transferred, "0kB");

                                    //Percentage
                                    float pct;
                                    if (dcc_conductor->total_recv > 0 && dcc_conductor->filesize > 0)
                                        pct = (float) dcc_conductor->total_recv / (float) dcc_conductor->filesize;
                                    else
                                        pct = 1;

                                    sprintf(dcc_conductor->entry->percentage, "%.0f%%", (float) (pct * 100));

                                    //Speed
                                    ULONG orig_time = atol(dcc_conductor->timestarted);
                                    ULONG current_time = atol(dcc_time());
                                    current_time = current_time - orig_time;
                                    float cps;
                                    if (dcc_conductor->total_recv > 0 && current_time > 0)
                                        cps = (float) dcc_conductor->total_recv / (float) current_time;
                                    else
                                        cps = 1;

                                    sprintf(dcc_conductor->entry->cps, "%.1f", cps / 1024);

                                    //Time Left
                                    ULONG bytes_left = dcc_conductor->filesize - dcc_conductor->total_recv;
                                    ULONG seconds_left = 1;

                                    if (bytes_left > 0 && dcc_conductor->total_recv > 0 && current_time > 0)
                                    {
                                        //    seconds_left = (bytes_left/1024) / ((dcc_conductor->total_recv / current_time)/1024);

                                        ULONG work1 = (bytes_left / 1024);
                                        ULONG work2 = ((dcc_conductor->total_recv / current_time) / 1024);

                                        if (work1 && work2)
                                            seconds_left = work1 / work2;
                                    }

                                    mins = seconds_left / 60;
                                    hrs = mins / 60;
                                    seconds_left = seconds_left % 60;
                                    mins = mins % 60; // hrs=hrs%24;

                                    if (hrs > 0)
                                        sprintf(dcc_conductor->entry->timeleft, "%02lu:%02lu:%02lu", hrs, mins,
                                                seconds_left);
                                    else if (mins > 0)
                                        sprintf(dcc_conductor->entry->timeleft, "00:%02lu:%02lu", mins, seconds_left);
                                    else
                                        sprintf(dcc_conductor->entry->timeleft, "00:00:%02lu", seconds_left);

                                    if (!stricmp(dcc_conductor->entry->status,
											(char *) LGS( MSG_TRANSFERRING )))
                                        DoMethod((Object*) WookieChat->LV_dcc, MUIM_NList_ReplaceSingle,
                                                dcc_conductor->entry, a, NOWRAP, ALIGN_LEFT);

                                }

                            }

                            if (!dcc_conductor->removed)
                                a++;

                            dcc_conductor = dcc_conductor->next;
                        }

                        //outgoing transfers
                        a = 0;
                        dcc_send_conductor = dcc_send_root->next;

                        while (dcc_send_conductor)
                        {

                            if (!dcc_send_conductor->removed && !dcc_send_conductor->completed)
                            {
                                if (dcc_send_conductor->entry)
                                {
                                    ULONG mins, hrs;
                                    //Amount Transferred
                                    if (dcc_send_conductor->total_recv > 0)
                                        sprintf(dcc_send_conductor->entry->bytes_transferred, "%lukB",
                                                dcc_send_conductor->total_recv / 1024);
                                    else
                                        sprintf(dcc_send_conductor->entry->bytes_transferred, "0kB");

                                    //Percentage
                                    float pct;
                                    if (dcc_send_conductor->total_recv > 0 && dcc_send_conductor->filesize > 0)
                                        pct = (float) dcc_send_conductor->total_recv
                                                / (float) dcc_send_conductor->filesize;
                                    else
                                        pct = 0;

                                    sprintf(dcc_send_conductor->entry->percentage, "%.0f%%", (float) (pct * 100));

                                    //Speed
                                    ULONG orig_time = atol(dcc_send_conductor->timestarted);
                                    ULONG current_time = atol(dcc_time());
                                    current_time = current_time - orig_time;
                                    //float cps = (float) dcc_send_conductor->total_recv / (float) current_time;
                                    float cps;
                                    if (dcc_send_conductor->total_recv > 0 && current_time > 0)
                                        cps = (float) dcc_send_conductor->total_recv / (float) current_time;
                                    else
                                        cps = 1;

                                    sprintf(dcc_send_conductor->entry->cps, "%.1f", cps / 1024);

                                    //Time Left
                                    ULONG bytes_left = dcc_send_conductor->filesize - dcc_send_conductor->total_recv;

                                    ULONG seconds_left = 0;

                                    //if(bytes_left>0 && dcc_send_conductor->total_recv>0 && current_time>0)
                                    //    seconds_left = (bytes_left/1024) / ((dcc_send_conductor->total_recv / current_time)/1024);
                                    if (bytes_left > 0 && dcc_send_conductor->total_recv > 0 && current_time > 0)
                                    {

                                        ULONG work1 = (bytes_left / 1024);
                                        ULONG work2 = ((dcc_send_conductor->total_recv / current_time) / 1024);

                                        if (work1 && work2)
                                            seconds_left = work1 / work2;
                                    }

                                    mins = seconds_left / 60;
                                    hrs = mins / 60;
                                    seconds_left = seconds_left % 60;
                                    mins = mins % 60; // hrs=hrs%24;
                                    if (hrs > 0)
                                        sprintf(dcc_send_conductor->entry->timeleft, "%02lu:%02lu:%02lu", hrs, mins,
                                                seconds_left);
                                    else if (mins > 0)
                                        sprintf(dcc_send_conductor->entry->timeleft, "00:%02lu:%02lu", mins,
                                                seconds_left);
                                    else
                                        sprintf(dcc_send_conductor->entry->timeleft, "00:00:%02lu", seconds_left);

                                    if (!stricmp(dcc_send_conductor->entry->status,
											(char *) LGS( MSG_TRANSFERRING )))
                                        DoMethod((Object*) WookieChat->LV_send_dcc, MUIM_NList_ReplaceSingle,
                                                dcc_send_conductor->entry, a, NOWRAP, ALIGN_LEFT);

                                }

                            }

                            if (!dcc_send_conductor->removed)
                            {
                                if (dcc_send_conductor->connected)
                                    dcc_send_data(a);
                                //dcc_send_data(a);
                                a++;
                            }

                            dcc_send_conductor = dcc_send_conductor->next;
                        }

                        init_2s_delay();

                        //to detect if our connection has been lost, we will ping the server every few minutes
                        delay_b4_ping_server_count++;
                        if (delay_b4_ping_server_count >= SECONDS_TO_WAIT_BEFORE_PING_SERVER)
                        {

                            for (status_conductor = status_root; status_conductor;
                                    status_conductor = status_conductor->next)
                            {
                                if (status_conductor->connection_active) // && !status_conductor->waiting_for_ping)
                                {
                                    char string_to_send[512];
                                    sprintf(string_to_send, "PING %s\r\n", status_conductor->servername);
                                    send_text(string_to_send);
                                    delay_b4_ping_server_count = 0;
                                    status_conductor->waiting_for_ping = 1;
                                    if (DEBUG)
                                        printf("sending ping to server..\n");
                                }

                            }
                        }

                    }

                    get_sys_time(&systime); // Get current system time

                    // Check every 2 minutes if we're still connected. Auto reconnect on on unwanted disconnection
#ifdef __amigaos4__
                    if(start_reconnect_delay_timer==TRUE && get_reconnect_secs() <= systime.tv_sec)
#else
                    if (start_reconnect_delay_timer == TRUE && get_reconnect_secs() <= systime.tv_secs)
#endif
                    {

                        start_reconnect_delay_timer = FALSE;

                        for (status_conductor = status_root; status_conductor;
                                status_conductor = status_conductor->next)
                        {

                            if (status_conductor->connection_active == 0 && status_conductor->quit_requested == FALSE
                                    && status_conductor->trying_to_reconnect == TRUE)
                            {

                                status_conductor->conductor = status_conductor->current_query;
                                status_conductor->retry_number++;

                                if (status_conductor->retry_number < my_settings.Maximum_Retries + 1)
                                {

                                    sprintf(string7, "%i", status_conductor->portnumber);
                                    connect2server(status_conductor->servername, string7, 1, 0,
                                            DONT_CONNECT_IN_CURRENT_TAB, NULL, 0);
                                    start_reconnect_delay_timer = TRUE;
                                    status_conductor->trying_to_reconnect = TRUE;

                                }
                                else
                                {
                                    if (my_settings.Maximum_Retries > 0)
                                    {
                                        sprintf(buffer3, "%s%sConnect%s %s", timestamp,
												LGS( MSG_OPENING_BRACKET ),
												LGS( MSG_CLOSING_BRACKET ),
												LGS( MSG_UNABLE_TO_RECONNECT_TO_SERVER ) );
                                        add_text_to_conductor_list(buffer3, 9, ACTIVITY_CHAT);

                                        if (status_conductor->a_socket != -1)
                                        {
                                            if (FD_ISSET(status_conductor->a_socket, &read_master))
                                                FD_CLR(status_conductor->a_socket, &read_master);
                                            if (FD_ISSET(status_conductor->a_socket, &write_master))
                                                FD_CLR(status_conductor->a_socket, &write_master);
                                        }
                                    }
                                }

                            }

                        }

                        if (start_reconnect_delay_timer)
                            automatically_reconnect_server(START_DELAY);
                        status_conductor = status_root;

                    }
//
                }

                timestamp_2_string();

                //process the queued messages, send one out every second.
                if (getmsg_paste_pause_delay())
                {
                    for (status_conductor = status_root; status_conductor; status_conductor = status_conductor->next)
                    {
                        for (status_conductor->conductor = status_conductor->root; status_conductor->conductor;
                                status_conductor->conductor = status_conductor->conductor->next)
                        {
                            if (status_conductor->conductor->queued_messages[0] != NULL)
                            {
                                process_outgoing(status_conductor->conductor->queued_messages[0], 0);
                                int count;
                                for (count = 0;count <= status_conductor->conductor->queued_messages_total
                                     && count < MAX_QUEUED_MESSAGES - 1; count++)
                                {
                                    if (status_conductor->conductor->queued_messages[count] != NULL
                                            && status_conductor->conductor->queued_messages[count + 1] != NULL)
                                    {
                                        strcpy(status_conductor->conductor->queued_messages[count],
                                                status_conductor->conductor->queued_messages[count + 1]);
                                    }
                                }
                                status_conductor->conductor->queued_messages_total--;
                                queued_messages_total--;
                                count--;

                                if (DEBUG)
                                    printf("tab queued messages left %d\n",
                                            status_conductor->conductor->queued_messages_total);
                                if (DEBUG)
                                    printf("total queued messages left %d\n", queued_messages_total);

                                for (count--; count < MAX_QUEUED_MESSAGES - 1; count++)
                                {
                                    if (status_conductor->conductor->queued_messages[count])
                                    {
                                        free(status_conductor->conductor->queued_messages[count]);
                                        status_conductor->conductor->queued_messages[count] = NULL;

                                    }
                                }

                            }
                        }

                        status_conductor->conductor = status_conductor->root;

                    }

                    status_conductor = status_current;
                    status_conductor->conductor = status_conductor->root;

                    if (queued_messages_total > 0)
                        init_paste_pause_delay();

                }
                if (getmsg_midnight_wait())
                {
                    struct timeval systime;
                    struct ClockData clockdata;
                    get_sys_time(&systime); // Get current system time
#ifdef __amigaos4__
                    Amiga2Date(systime.tv_sec, &clockdata);
#else
                    Amiga2Date(systime.tv_secs, &clockdata);
#endif
                    timestamp_2_string();

					sprintf(buffer3, "%s%s%s%s * * %d-%d-%d * *", timestamp,
							LGS( MSG_OPENING_BRACKET ),
							LGS( MSG_COLOUR_SETTINGS_LABEL_INFO ),
							LGS( MSG_CLOSING_BRACKET ),
							clockdata.year,
							clockdata.month,
							clockdata.mday); //,timestamp_hrs,timestamp_mins);

                    for (status_conductor = status_root; status_conductor; status_conductor = status_conductor->next)
                    {
                        for (status_conductor->conductor = status_conductor->root; status_conductor->conductor;
                                status_conductor->conductor = status_conductor->conductor->next)
                        {
                            add_text_to_conductor_list(buffer3, 9, 0);

                        }
                        status_conductor->conductor = status_conductor->root;

                    }

                    status_conductor = status_current;
                    status_conductor->conductor = status_conductor->root;

                    init_midnight_wait();
                }

                if (select_result > 0)
                    do_waitselect_code(); //iterate through all the sockets and read/write to them
            }

            free(test2);

            cleanexit(NULL);

            free(work_buffer);

            return 0;

        }
/* \\\ */


size_t stccpy ( char	   * dest, const char * src, size_t	      n)
{
    char * ptr = dest;

    while (n>1 && *src)
    {
        *ptr = *src;
	ptr ++;
	src ++;
	n--;
    }

    *ptr++ = '\0';

    return (ptr-dest);
}

void copy_settings_to_undo_buffer()
{
    temp_settings = my_settings;
}

void copy_undo_buffer_to_settings()
{
    my_settings = temp_settings;
    set_settings();
}

UTF8 *utf8 = NULL;
STRPTR buf[1024];

void send_text(char *text2)
{

    if (DEBUG)
        printf("Wants to send:%s\n", text2);

    if (!status_conductor)
        return;

    if (utf8)
    {
        if ((char*) utf8 != (char*) buf)
            CodesetsFreeA(utf8, NULL);
        utf8 = NULL;
    }

    //if((utf8 = CodesetsUTF8Create(CSA_Source,  text2, CSA_Dest, buf, CSA_DestLen, sizeof(buf), TAG_DONE)))
#ifdef __AROS__
    struct TagItem utf8_conv_taglist[] =
    {
    { CSA_Source, (STACKIPTR) text2 },
    { CSA_Dest, (STACKIPTR) buf },
    { CSA_DestLen, (IPTR) sizeof(buf) },
    { TAG_DONE, 0 } };
#else
    struct TagItem utf8_conv_taglist[] =
    {
        {   CSA_Source, (ULONG)text2},
        {   CSA_Dest,(ULONG)buf},
        {   CSA_DestLen, sizeof(buf)},
        {   TAG_DONE,0}};
#endif

    if ((utf8 = CodesetsUTF8CreateA(utf8_conv_taglist)))
    {

        text = (char*) utf8;

    }
    else
    {
        text = text2;
    }

    if (!SocketBase)
        return;
    timestamp_2_string();
    if (!status_conductor->connection_active)
    {
		sprintf(buffer3, "%s%s%s%s %s", timestamp,
				LGS( MSG_OPENING_BRACKET ),
				LGS( MSG_ERROR ),
				LGS( MSG_CLOSING_BRACKET ),
				LGS( MSG_NOT_CONNECTED ));
        add_text_to_conductor_list(buffer3, 9, ACTIVITY_CHAT);
        return;
    }

    int n;

    //if(DEBUG) printf("Sending:%s\n",text);

    if (status_conductor->connection_active == 1)
    {

        int total = 0; // how many bytes we've sent
        int bytesleft = strlen(text); // how many we have left to send
        int length = strlen(text);

        //test for write connectivity
        /*struct timeval t;
         t.tv_sec = 0;
         t.tv_usec = 500000;
         fd_set rd;
         int32 canread = 1, i = 0;
         FD_ZERO(&rd);
         FD_SET(status_conductor->a_socket, &rd);
         canread = WaitSelect(status_conductor->a_socket + 1, NULL, &rd, NULL, &t, NULL);
         if(canread == 0) // && !FD_ISSET(status_conductor->a_socket, &rd))
         {
		 sprintf(buffer3,"%s%s%s%s Unable to write text to socket: error number: %li",timestamp,LGS(catalog,217,"["),LGS(catalog,0,"Error"),LGS(catalog,218,"]"),Errno());
         add_text_to_current_list(buffer3,9, ACTIVITY_CHAT);
         return;
         } */
        //test to write connectivity end
        while (total < length)
        {
            n = send(status_conductor->a_socket, (i_in) text + total, bytesleft, 0);
            if (n == -1)
            {
                break;
            }
            total += n;
            bytesleft -= n;

        }

        if (DEBUG)
            printf("Sent:%s\n", text);

        if (n == -1)
        {

            if (DEBUG)
                printf("error:%li\nunable to send:%s", Errno(), text);

            sprintf(buffer3, "%s%s%s%s Unable to send text: error number: %li", timestamp,
					LGS( MSG_OPENING_BRACKET ),
					LGS( MSG_ERROR ),
					LGS( MSG_CLOSING_BRACKET ),
					Errno());
            if (status_conductor->conductor->LV_channel)
                add_text_to_conductor_list(buffer3, 9, ACTIVITY_CHAT);

            if (Errno() == 57 || Errno() == 32) //Error, not connected!!
            {

                if (status_conductor->connection_active == 1)
                {
					sprintf(buffer3, "%s%s%s%s %s", timestamp,
							LGS( MSG_OPENING_BRACKET ),
							LGS( MSG_ERROR ),
							LGS( MSG_CLOSING_BRACKET ),
							LGS( MSG_NOT_CONNECTED ) );

                    status_conductor->connection_active = 0;
                    status_conductor->pass[0] = '\0';
                    status_conductor->servername[0] = '\0';
                    status_conductor->networkname[0] = '\0';

                    status_conductor->conductor = status_conductor->root;
                    while (status_conductor->conductor)
                    {
                        if (status_conductor->conductor->LV_channel)
                            add_text_to_conductor_list(buffer3, 9, ACTIVITY_CHAT);
                        status_conductor->conductor = status_conductor->conductor->next;
                    }

                }
            }

        }
    }

}

void send_dcc_chat(char *text)
{
    if (!SocketBase)
        return;
    timestamp_2_string();

    if (!dcc_chat_conductor->connected)
    {
		sprintf(buffer3, "%s%s%s%s %s", timestamp,
				LGS( MSG_OPENING_BRACKET ),
				LGS( MSG_ERROR ),
				LGS( MSG_CLOSING_BRACKET ),
				LGS( MSG_NOT_CONNECTED_TO_USER ) );
        add_text_to_conductor_list(buffer3, 9, ACTIVITY_CHAT);
        return;
    }

    int n;

    if (dcc_chat_conductor->connected)
    {
        int total = 0; // how many bytes we've sent
        int bytesleft = strlen(text); // how many we have left to send
        int length = strlen(text);

        while (total < length)
        {
            n = send(dcc_chat_conductor->dcc_socket, (i_in) text + total, bytesleft, 0);
            if (n == -1)
            {
                break;
            }
            total += n;
            bytesleft -= n;
        }

    }
    else
    {
        if (DEBUG)
            printf("not connected to dcc chat\n");
    }

    if (n == -1)
    {

        if (DEBUG)
            printf("error:%li\nunable to send:%s", Errno(), text);
        if (Errno() == 57 || Errno() == 32) //Error, not connected!!
        {

            if (dcc_chat_conductor->connected == 1)
            {
				sprintf(buffer3, "%s%s%s%s %s", timestamp,
						LGS( MSG_OPENING_BRACKET ),
						LGS( MSG_ERROR ),
						LGS( MSG_CLOSING_BRACKET ),
						LGS( MSG_NOT_CONNECTED_TO_USER ) );
                if (dcc_chat_conductor->conductor->LV_channel)
                    add_text_to_conductor_list(buffer3, 9, ACTIVITY_CHAT);
                dcc_chat_conductor->connected = 0;
            }

        }

    }

}

void send_current(char *text2)
{
    if (!status_conductor)
        return;
    if (!SocketBase)
        return;
    timestamp_2_string();

    if (DEBUG)
        printf("sending:%s\n", text2);

    //struct TagItem my_send_charset1_taglist[] = { {CSA_Source, (ULONG)text2 }, {CSA_SourceCodeset, (ULONG)charsets[local_charset]}, {CSA_DestCodeset, (ULONG)charsets[status_current->remote_charset]}, {TAG_DONE, 0} };
    /*struct TagItem my_send_charset1_taglist[] = { {CSA_Source, (ULONG)text2 }, {CSA_SourceCodeset, (ULONG)local_charsets[local_charset]}, {CSA_DestCodeset, (ULONG)remote_charsets[status_current->remote_charset]}, {TAG_DONE, 0} };
     text = CodesetsConvertStrA(my_send_charset1_taglist);
     if(!text)
     {
     printf("failed conversion when sending to a server..\n");
     text=text2;
     }
     else
     {

     if(!strcmp(text,"") && strlen(text2) > 0)
     {
     if(DEBUG)
     {
     printf("text field is blank..\n");
     printf("original text field wasnt blank..\n");
     printf("lets resend the original..\n");
     }
     text=text2;
     }

     }*/
    text = text2;

    if (!status_current->connection_active)
    {
		sprintf(buffer3, "%s%s%s%s %s", timestamp,
				LGS( MSG_OPENING_BRACKET ),
				LGS( MSG_ERROR ),
				LGS( MSG_CLOSING_BRACKET ),
				LGS( MSG_NOT_CONNECTED ) );

        add_text_to_current_list(buffer3, 9, ACTIVITY_CHAT);
        return;
    }

    //if(DEBUG) printf("Sending1:%s\n",text);
    int n;

    if (status_current->connection_active)
    {
        int total = 0; // how many bytes we've sent
        int bytesleft = strlen(text); // how many we have left to send

        int length = strlen(text);

        //test for write connectivity
        /*struct timeval t;
         t.tv_sec = 0;
         t.tv_usec = 500000;
         fd_set rd;
         int32 canread = 1, i = 0;
         FD_ZERO(&rd);
         FD_SET(status_current->a_socket, &rd);
         canread = WaitSelect(status_current->a_socket + 1, NULL, &rd, NULL, &t, NULL);
         if(canread == 0) // && !FD_ISSET(status_current->a_socket, &rd))
         {
		 sprintf(buffer3,"%s%s%s%s Unable to write text to socket: error number: %li",timestamp,LGS(catalog,217,"["),LGS(catalog,0,"Error"),LGS(catalog,218,"]"),Errno());
         add_text_to_current_list(buffer3,9, ACTIVITY_CHAT);
         return;
         } */
        //test to write connectivity end
        while (total < length)
        {
            n = send(status_current->a_socket, (i_in) text + total, bytesleft, 0);
            if (n == -1)
            {
                break;
            }
            total += n;
            bytesleft -= n;
        }

        if (n == -1)
        {

            if (DEBUG)
                printf("error:%li\nunable to send:%s", Errno(), text);
            sprintf(buffer3, "%s%s%s%s Unable to send text: error number: %li", timestamp,
					LGS( MSG_OPENING_BRACKET ),
					LGS( MSG_ERROR ),
					LGS( MSG_CLOSING_BRACKET ),
					Errno());
            if (status_current->conductor->LV_channel)
                add_text_to_current_list(buffer3, 9, ACTIVITY_CHAT);

            if (Errno() == 57 || Errno() == 32) //Error, not connected!!
            {

                if (status_current->connection_active == 1)
                {

					sprintf(buffer3, "%s%s%s%s %s", timestamp,
							LGS( MSG_OPENING_BRACKET ),
							LGS( MSG_ERROR ),
							LGS( MSG_CLOSING_BRACKET ),
							LGS( MSG_NOT_CONNECTED ) );

                    status_current->conductor = status_current->root;
                    while (status_current->conductor)
                    {
                        if (status_current->conductor->LV_channel)
                            add_text_to_current_list(buffer3, 9, ACTIVITY_CHAT);
                        status_current->conductor = status_conductor->conductor->next;
                    }

                    status_current->connection_active = 0;
                    status_current->pass[0] = '\0';
                    status_current->servername[0] = '\0';
                    status_current->networkname[0] = '\0';
                }

            }

        }

    }
    else if (DEBUG)
        printf("no connection\n");
}

struct WBArg *wbarg;

#ifdef __amigaos4__
typedef CONST_STRPTR c1_in;
typedef STRPTR *c2_in;
#elif __MORPHOS__
typedef STRPTR c1_in;
typedef STRPTR *c2_in;
#elif __AROS__
typedef const STRPTR c1_in;
typedef const STRPTR *c2_in;
#else
typedef UBYTE *c1_in;
typedef UBYTE **c2_in;
#endif

BOOL ParseToolTypes(struct WBArg *wbarg)
{
    struct DiskObject *dobj;
    c2_in toolarray;
    char *s;
    BOOL success = FALSE;

    if (!wbarg->wa_Name)
        return 0;

	if ((*wbarg->wa_Name) && (dobj = GetDiskObject((c1_in) (wbarg->wa_Name))))
    {
        toolarray = /*(char **)*/ dobj->do_ToolTypes;
        if ((s = (char *) FindToolType((c2_in) toolarray, (c1_in) "SMALLTABS")))
        {
            if (!stricmp(s, "true"))
                SMALLTABS = TRUE;
        }
        if ((s = (char *) FindToolType((c2_in) toolarray, (c1_in) "DEBUG")))
        {
            //if(!stricmp(s,"true")) DEBUG=1;
        }
        if ((s = (char *) FindToolType((c2_in) toolarray, (c1_in) "RAW")))
        {
            if (!stricmp(s, "true"))
                RAW = TRUE;
        }
        if ((s = (char *) FindToolType((c2_in) toolarray, (c1_in) "PROXY")))
        {
            if (!stricmp(s, "true"))
                USING_A_PROXY = TRUE;
        }
        if ((s = (char *) FindToolType((c2_in) toolarray, (c1_in) "NOZUNE")))
        {
            if (!stricmp(s, "true"))
                ZUNE_SYSTEM = FALSE;

        }
        if ((s = (char *) FindToolType((c2_in) toolarray, (c1_in) "ZUNE")))
        {
            if (!stricmp(s, "true"))
                ZUNE_SYSTEM = TRUE;
            else
                ZUNE_SYSTEM = FALSE;

        }
        if ((s = (char *) FindToolType((c2_in) toolarray, (c1_in) "ALTCLIPBOARD")))
        {
            if (!stricmp(s, "true"))
                ALTERNATIVE_CLIPBOARD = TRUE;

        }

        if ((s = (char *) FindToolType((c2_in) toolarray, (c1_in) "ABOUTGFX")))
        {
            if (!stricmp(s, "true"))
                my_settings.os3_about_window_gfx = 1;
        }
        if ((s = (char *) FindToolType((c2_in) toolarray, (c1_in) "GEIT")))
        {
            if (!stricmp(s, "true"))
                GEIT = TRUE;
        }
        if ((s = (char *) FindToolType((c2_in) toolarray, (c1_in) "AREXX")))
        {
            if (!stricmp(s, "true"))
                USE_AREXX = TRUE;
            else
                USE_AREXX = FALSE;
        }

        /* Free the diskobject we got */
        FreeDiskObject(dobj);
        success = TRUE;
    }
    return (success);
}

void set_channel_clipboard_hook(void)
{

    if (!status_current)
        return;
    if (!status_current->current_query)
        return;
    if (!status_current->current_query->LV_channel)
        return;

    if (my_settings.which_clipboard_style == COLUMNS || ALTERNATIVE_CLIPBOARD == 1)
    {
        setmacro((Object*)status_current->current_query->LV_channel, MUIA_NList_DragColOnly, 1);
        setmacro((Object*)MN_Clipboard, MUIA_Menuitem_Checked, TRUE);
        setmacro((Object*)status_current->current_query->LV_channel, MUIA_NList_CopyEntryToClipHook,
                &Custom_Clipboard2_Hook);
        //lets use multicolumn hook

    }
    else
    {
        setmacro((Object*)status_current->current_query->LV_channel, MUIA_NList_DragColOnly, -1);
        setmacro((Object*)MN_Clipboard, MUIA_Menuitem_Checked, FALSE);
        setmacro((Object*)status_current->current_query->LV_channel, MUIA_NList_CopyEntryToClipHook, NULL);
        //lets use built in clipboard hook

    }

}

void read_list_of_servers(void)
{
    char *work_buffer=malloc(sizeof(char) * 1000);
    char *work_buffer2=malloc(sizeof(char) * 1000);
    char *work1=malloc(sizeof(char) * 100);
    char *len2; //variable used for file access
    char work_buffer3[600];
    BPTR newbptr_file = Open((_s_cs)"progdir:servers.txt",MODE_OLDFILE);

    int running3=1;
    len2=(char*)FGets(newbptr_file,(STRPTR)work_buffer,600);
    if(len2)
    {
        if(stricmp(work_buffer,"WOOKIECHAT_SERVERS_FILE_2\n"))
        {
            printf("Old servers.txt file detected!\nRenaming servers.txt to servers2.txt\nNow generating a new servers.txt in the new format\n(Keep servers2.txt as a backup in case something goes wrong!\n");

            Rename((_s_cs)"progdir:servers.txt",(_s_cs)"progdir:servers2.txt");

            BPTR new_servers_file = Open((_s_cs)"progdir:servers.txt",MODE_NEWFILE);
            if(new_servers_file)
            {
                FPuts(new_servers_file,(l_in)"WOOKIECHAT_SERVERS_FILE_2\n");

                FPuts(new_servers_file,(l_in)work_buffer);

                while(running3)
                {
                    //read the line from the servers file
                    len2=(char*)FGets(newbptr_file,(STRPTR)work_buffer,600);

                    if(!len2) { running3=0; }

                    if(DEBUG) printf("running3 = %d\n",running3);


                    //work_buffer[strlen(work_buffer)]='\0';

                    strcpy(work_buffer2,work_buffer);
                    strtok(work_buffer2," ");
                    work1=strtok(NULL," \n");

                    //its a group name! lets create the initial group entry
                    if(!work1)
                    {
                        //if(!first) FPuts(new_servers_file,(char *)"ENDGROUP\n");
                        //first=FALSE;

                        //FPuts(new_servers_file,(char *)"GROUP");
                        FPuts(new_servers_file,(l_in)work_buffer);
                        //FPuts(new_servers_file,(char *)"\n");
                        //printf("putting group name:%s",work_buffer);

                    }
                    else
                    {
                        strcpy(work_buffer2,work_buffer);
                        //printf("1 work_buffer :%s",work_buffer);
                        //printf("1 work_buffer2:%s",work_buffer2);

                        string3=strtok(work_buffer2," "); //servername
                        string2=strtok(NULL,"\n ");     //port number
                        string1=strtok(NULL,"\n ");    //connect automatically, yes or not?
                        string5=strtok(NULL,"\n ");     //charset
                        string4=strtok(NULL,"\n ");   //password

                        if(string3)
                        {
                            sprintf(work_buffer3,"SERVER=%s ",string3);
                            FPuts(new_servers_file,(l_in)work_buffer3);

                            if(string2)
                            {
                                sprintf(work_buffer3,"PORT=%s",string2);
                                FPuts(new_servers_file,(l_in)work_buffer3);

                            }
                        }

                        if(string1)
                            sprintf(work_buffer3," AUTOCONNECT=%s",string1);
                        else
                            sprintf(work_buffer3," AUTOCONNECT=");

                        FPuts(new_servers_file,(l_in)work_buffer3);

                        if(string5)
                            sprintf(work_buffer3," SERVER_CHARSET=%s",string5);
                        else
                            sprintf(work_buffer3," SERVER_CHARSET=");

                        FPuts(new_servers_file,(l_in)work_buffer3);

                        if(string4)
                            sprintf(work_buffer3," SERVER_PASSWORD=%s",string4);
                        else
                            sprintf(work_buffer3," SERVER_PASSWORD=");
                        FPuts(new_servers_file,(l_in)work_buffer3);


                        sprintf(work_buffer3," AUTOJOINS=");
                        FPuts(new_servers_file,(l_in)work_buffer3);

                        FPuts(new_servers_file,(l_in)"\n");
                    }
                }
                Close(new_servers_file);
            }
        }
    }

    Close(newbptr_file);


    return;
}

static char *stack_cookie __attribute__((used)) = (char*) "$STACK: 550000";

#ifdef __MORPHOS__
int __stack = 550000;
#endif

#ifdef __amigaos4__
const char *__stdiowin = "NIL:";
#endif

/*************************************************************************/


