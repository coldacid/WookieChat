/*
    The contents of this file are subject to the AROS Public License Version 1.1 
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis, 
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
    for the specific language governing rights and limitations under the License.
*/

#include "includes.h"

#include <proto/muimaster.h>

#include "intern.h"
#include "objapp.h"

/* Locals */
static char output_string[800];
static char string11[900];
static char *string2;
static char *string1;

int load_user_list_buttons_config(void);
int save_user_list_buttons_config(void);
int create_user_list_buttons(void);

void save_colours_choice()
{

    strcpy(file_name, "progdir:colours/colours_theme.txt");
    BPTR save_file = Open((_s_cs)file_name, MODE_NEWFILE);

    if (save_file)
    {

        FPuts(save_file, (l_in) my_settings.default_colours_theme);

        Close(save_file);
    }

}

void load_colours_choice()
{

    strcpy(file_name, "progdir:colours/colours_theme.txt");
    BPTR save_file = Open((_s_cs)file_name, MODE_OLDFILE);

    if (save_file)
    {

        if (FGets(save_file, (STRPTR) my_settings.default_colours_theme, 800))
        {
            if (DEBUG)
                printf("loaded default colour theme:%s\n", my_settings.default_colours_theme);
        }
        else
            strcpy(my_settings.default_colours_theme, "progdir:colours/default.txt");
        Close(save_file);

    }
    else
        strcpy(my_settings.default_colours_theme, "progdir:colours/default.txt");

}

void save_ignore_list()
{
    BPTR save_file;

    char *work_buffer;

    getmacro((Object*) WookieChat->LV_ignore, MUIA_NList_Entries, &entries);

    strcpy(file_name, "progdir:ignorelist.txt");
    save_file = Open((_s_cs)file_name, MODE_NEWFILE);

    if (save_file)
    {
        my_settings.totalignores = 0;

        for (unsigned int count = 0; count < entries && count < 100; count++)
        {
            DoMethod((Object*) WookieChat->LV_ignore, MUIM_NList_GetEntry, count, &work_buffer);
            if (work_buffer)
            {
                sprintf(output_string, "%s\n", work_buffer);
                FPuts(save_file, (l_in) output_string);

            }
            else
                break;

            //strcpy(my_settings.ignorelist[count].ignoremask,work_buffer);

            strcpy(string11, work_buffer);

            //string1=strtok(work_buffer," "); //move past the ignore mask, get to the options
            string1 = strtok(string11, " "); //move past the ignore mask, get to the options
            if (string1)
            {

                strcpy(my_settings.ignorelist[count].ignoremask, string1);

            }

            string1 = strtok(NULL, " ");
            if (string1)
            {
                if (!stricmp(string1, "ignore"))
                    my_settings.ignorelist[count].ignore_privmsg = TRUE;
                else
                    my_settings.ignorelist[count].ignore_privmsg = FALSE;

            }

            string1 = strtok(NULL, " ");
            if (string1)
            {
                if (!stricmp(string1, "ignore"))
                    my_settings.ignorelist[count].ignore_ctcp = TRUE;
                else
                    my_settings.ignorelist[count].ignore_ctcp = FALSE;
            }

            string1 = strtok(NULL, " ");
            if (string1)
            {
                if (!stricmp(string1, "ignore"))
                    my_settings.ignorelist[count].ignore_dcc = TRUE;
                else
                    my_settings.ignorelist[count].ignore_dcc = FALSE;
            }

            my_settings.totalignores++;

            if (my_settings.totalignores == 100)
                break;

        }

        Close(save_file);
    }

}

void load_ignore_list()
{
    //load and display the ignore masks and options

    DoMethod((Object*) WookieChat->LV_ignore, MUIM_NList_Clear);

    strcpy(file_name, "progdir:ignorelist.txt");
    BPTR ignore_file = Open((_s_cs)file_name, MODE_OLDFILE);
    if (ignore_file)
    {
        while (FGets(ignore_file, (STRPTR) urlgrabber_str, 2000))
        {
            urlgrabber_str[strlen(urlgrabber_str) - 1] = '\0';
            for (unsigned int count = 0; count < strlen(urlgrabber_str); count++)
                urlgrabber_str[count] = ToLower(urlgrabber_str[count]);

            DoMethod((Object*) WookieChat->LV_ignore, MUIM_NList_InsertSingle, urlgrabber_str,
                    MUIV_NList_Insert_Bottom);
        }

        Close(ignore_file);
    }

}

void save_nick_settings()
{
    char *a;
    int b;

    BPTR save_file = Open((_s_cs)"progdir:nick_settings.txt", MODE_NEWFILE);

    char output_string[800];

    getmacro((Object*) WookieChat->STR_nickname1, MUIA_String_Contents, &a);
    sprintf(output_string, "NICK1 %s\n", a);
    FPuts(save_file, (l_in) output_string);
    strcpy(nick, a);

    getmacro((Object*) WookieChat->STR_nickname2, MUIA_String_Contents, &a);
    sprintf(output_string, "NICK2 %s\n", a);
    FPuts(save_file, (l_in) output_string);
    strcpy(nick2, a);

    getmacro((Object*) WookieChat->STR_nickname3, MUIA_String_Contents, &a);
    sprintf(output_string, "NICK3 %s\n", a);
    FPuts(save_file, (l_in) output_string);
    strcpy(nick3, a);

    getmacro((Object*) WookieChat->STR_realname, MUIA_String_Contents, &a);
    sprintf(output_string, "REALNAME %s\n", a);
    FPuts(save_file, (l_in) output_string);
    strcpy(realname, a);

    getmacro((Object*) WookieChat->STR_username, MUIA_String_Contents, &a);
    sprintf(output_string, "USERNAME %s\n", a);
    FPuts(save_file, (l_in) output_string);
    strcpy(username, a);

    getmacro((Object*) WookieChat->CH_display_server_win, MUIA_Selected, &b);
    sprintf(output_string, "DISPLAYSERVERWIN %i\n", b);
    FPuts(save_file, (l_in) output_string);
    my_settings.display_server_window_on_startup = b;

    getmacro((Object*) WookieChat->CYCLE_local_charset, MUIA_Cycle_Active, &b);
    sprintf(output_string, "CHARSET %i\n", b);
    FPuts(save_file, (l_in) output_string);
    local_charset = b;

    Close(save_file);
}

void save_settings()
{
    char *work_buffer = malloc(sizeof(char) * 800);

    BPTR save_file = Open((_s_cs)"progdir:settings.txt", MODE_NEWFILE);

    //nick settings are now stored in progdir:nick_settings.txt

    sprintf(output_string,
            "#the next two time values are added together for the paste delay (seconds + microseconds)\n");
    FPuts(save_file, (l_in) output_string);
    sprintf(output_string, "PASTE_DELAY_SECONDS %i\n", my_settings.paste_delay_seconds);
    FPuts(save_file, (l_in) output_string);
    sprintf(output_string, "PASTE_DELAY_MICROSECONDS %i\n", my_settings.paste_delay_microseconds);
    FPuts(save_file, (l_in) output_string);

    sprintf(
            output_string,
            "#for the setting below, choose 1 to sort tabs alphabetically when they open, ot 0 to NOT sort tabs alphabetically\n");
    FPuts(save_file, (l_in) output_string);
    sprintf(output_string, "SORT_TABS_ALPHABETICALLY %i\n", my_settings.sort_tabs_alphabetically);
    FPuts(save_file, (l_in) output_string);

    //sprintf(output_string,"#for the setting below, choose -1 for autoresizing columns, or a positive number for how many characters wide\n");
    //FPuts(save_file,(l_in)output_string);
    sprintf(output_string, "TIMESTAMP_COLUMN_WIDTH %i\n", my_settings.timestamp_column_width);
    FPuts(save_file, (l_in) output_string);

    //sprintf(output_string,"#for the setting below, choose -1 for autoresizing columns, or a positive number for how many characters wide\n");
    //FPuts(save_file,(l_in)output_string);
    sprintf(output_string, "NICK_COLUMN_WIDTH %i\n", my_settings.nick_column_width);
    FPuts(save_file, (l_in) output_string);

    //sprintf(output_string,"#for the setting below, choose 1 for yes, 0 for no\n");
    //FPuts(save_file,(l_in)output_string);
    sprintf(output_string, "USE_EXTERNAL_SOUND_REPLAYER %i\n", my_settings.use_external_sound_replayer);
    FPuts(save_file, (l_in) output_string);

    //sprintf(output_string,"#for the setting below, choose a Sound replayer with full path. I.e.: dh1:sound/play16/play16\n");
    //FPuts(save_file,(l_in)output_string);
    sprintf(output_string, "EXTERNAL_SOUND_REPLAYER %s\n", my_settings.external_sound_replayer);
    FPuts(save_file, (l_in) output_string);

    sprintf(output_string, "REMOVE_BRACKETS %i\n", my_settings.remove_brackets);
    FPuts(save_file, (l_in) output_string);

    sprintf(output_string, "USE_IDENT_SERVER %i\n", my_settings.use_ident);
    FPuts(save_file, (l_in) output_string);

    sprintf(output_string, "BUTTON_TABS %i\n", my_settings.use_nlistobject_for_tabs);
    FPuts(save_file, (l_in) output_string);

    sprintf(output_string, "USE_COLUMN_DISPLAY %i\n", my_settings.use_column_display);
    FPuts(save_file, (l_in) output_string);

    sprintf(output_string, "ENABLE_CLONE_DETECTION %i\n", my_settings.clone_detection);
    FPuts(save_file, (l_in) output_string);

    sprintf(output_string, "COLUMN_STYLE_CLIPBOARD %i\n", my_settings.which_clipboard_style);
    FPuts(save_file, (l_in) output_string);

    sprintf(output_string, "AUTO_REJOIN %i\n", my_settings.auto_rejoin_on_kick);
    FPuts(save_file, (l_in) output_string);

    sprintf(output_string, "AUTO_OPEN_TABS %i\n", my_settings.open_query_on_privmsg);
    FPuts(save_file, (l_in) output_string);

    sprintf(output_string, "BUFFER_SIZE %ld\n", my_settings.max_lines_for_buffers);
    FPuts(save_file, (l_in) output_string);

    sprintf(output_string, "LOG_CHANNELS %i\n", my_settings.log_channels);
    FPuts(save_file, (l_in) output_string);

    sprintf(output_string, "LOG_PRIVATE %i\n", my_settings.log_private);
    FPuts(save_file, (l_in) output_string);

    sprintf(output_string, "KICK_MSG %s\n", my_settings.default_kickmsg);
    FPuts(save_file, (l_in) output_string);

    sprintf(output_string, "SAMPLES_PATH %s\n", my_settings.samples_path);
    FPuts(save_file, (l_in) output_string);

    sprintf(output_string, "QUIT_MSG %s\n", my_settings.quit_msg);
    FPuts(save_file, (l_in) output_string);

    sprintf(output_string, "PART_MSG %s\n", my_settings.part_msg);
    FPuts(save_file, (l_in) output_string);

    sprintf(output_string, "NICKLIST_WHOIS %i\n", my_settings.request_whois_info);
    FPuts(save_file, (l_in) output_string);

    sprintf(output_string, "NICKLIST_QUERY %i\n", my_settings.open_private_tab);
    FPuts(save_file, (l_in) output_string);

    sprintf(output_string, "TIME_STAMP_STRING %s\n", my_settings.time_stamp_string);
    FPuts(save_file, (l_in) output_string);

    sprintf(output_string, "TIME_STAMP %i\n", my_settings.timestamp_config);
    FPuts(save_file, (l_in) output_string);

    sprintf(output_string, "TEXT_EDITOR %s\n", my_settings.text_editor);
    FPuts(save_file, (l_in) output_string);

    sprintf(output_string, "BROWSER %s\n", my_settings.browser);
    FPuts(save_file, (l_in) output_string);

    if (my_settings.download_path[0] == '\0')
        sprintf(output_string, "DOWNLOAD_PATH %s\n", wookie_folder);
    else
        sprintf(output_string, "DOWNLOAD_PATH %s\n", my_settings.download_path);
    FPuts(save_file, (l_in) output_string);

    sprintf(output_string, "UPLOAD_PATH %s\n", my_settings.upload_path);
    FPuts(save_file, (l_in) output_string);

    sprintf(output_string, "DCC_AUTO_ACCEPT %i\n", my_settings.dcc_recv_auto_accept);
    FPuts(save_file, (l_in) output_string);

    sprintf(output_string, "DCC_AUTO_IF_EXISTS %i\n", my_settings.dcc_recv_auto_accept_if_exists_action);
    FPuts(save_file, (l_in) output_string);

    sprintf(output_string, "DCC_GET_IP %i\n", my_settings.autogethostid);
    FPuts(save_file, (l_in) output_string);

    sprintf(output_string, "IPADDR %s\n", my_settings.ipaddr);
    FPuts(save_file, (l_in) output_string);

    sprintf(output_string, "IPADDR_USE %i\n", my_settings.ipaddr_use);
    FPuts(save_file, (l_in) output_string);

    sprintf(output_string, "DCC_START_PORT %s\n", my_settings.start_port);
    FPuts(save_file, (l_in) output_string);

    sprintf(output_string, "DCC_END_PORT %s\n", my_settings.end_port);
    FPuts(save_file, (l_in) output_string);

    sprintf(output_string, "RELOAD_CHAN %i\n", my_settings.how_many_lines_to_reload_channel);
    FPuts(save_file, (l_in) output_string);

    sprintf(output_string, "RELOAD_PRIVATE %i\n", my_settings.how_many_lines_to_reload_private);
    FPuts(save_file, (l_in) output_string);

    sprintf(output_string, "HIGHLIGHT %s\n", my_settings.highlight);
    FPuts(save_file, (l_in) output_string);

    sprintf(output_string, "SAMPLE_TABOPEN %s\n", my_settings.sound_sample_tabopen);
    FPuts(save_file, (l_in) output_string);

    sprintf(output_string, "SAMPLE_NEWMSG %s\n", my_settings.sound_sample_newmsg);
    FPuts(save_file, (l_in) output_string);

    sprintf(output_string, "SAMPLE_HIGHLIGHT %s\n", my_settings.sound_sample_highlight);
    FPuts(save_file, (l_in) output_string);

    sprintf(output_string, "PLAY_SOUND_WHEN_NEWMSG %i\n", my_settings.play_sound_when_newmsg);
    FPuts(save_file, (l_in) output_string);

    sprintf(output_string, "PLAY_SOUND_WHEN_TABOPEN %i\n", my_settings.play_sound_when_tabopen);
    FPuts(save_file, (l_in) output_string);

    sprintf(output_string, "PLAY_SOUND_WHEN_HIGHLIGHT %i\n", my_settings.play_sound_when_highlight);
    FPuts(save_file, (l_in) output_string);

    sprintf(output_string, "NICK_COMPLETION_STYLE %i\n", my_settings.nick_completion);
    FPuts(save_file, (l_in) output_string);

    sprintf(output_string, "URLGRABBER %i\n", my_settings.urlgrabber);
    FPuts(save_file, (l_in) output_string);

    sprintf(output_string, "RECONNECT_TO_SAME_SERVER %i\n", my_settings.reconnect_to_same_server);
    FPuts(save_file, (l_in) output_string);

    sprintf(output_string, "RECONNECT_MAX_RETRIES %i\n", my_settings.Maximum_Retries);
    FPuts(save_file, (l_in) output_string);

    sprintf(output_string, "RECONNECT_DELAY %i\n", my_settings.Reconnect_Delay);
    FPuts(save_file, (l_in) output_string);

    sprintf(output_string, "SPLITUP_LOGFILES %i\n", my_settings.splitup_logfiles);
    FPuts(save_file, (l_in) output_string);

    sprintf(output_string, "MAX_LOGFILES_SIZE %i\n", my_settings.max_log_size);
    FPuts(save_file, (l_in) output_string);

    sprintf(output_string, "NO_SERVER_TABS %i\n", my_settings.no_server_tabs);
    FPuts(save_file, (l_in) output_string);

    sprintf(output_string, "GRAPHICAL_SMILIES %i\n", my_settings.graphical_smilies);
    FPuts(save_file, (l_in) output_string);

    sprintf(output_string, "GRAPHICAL_SMILIES_BACKGROUND_COLOUR %i\n", my_settings.graphical_smilies_background);
    FPuts(save_file, (l_in) output_string);

    sprintf(output_string, "GRAPHICAL_NICKLIST %i\n", my_settings.graphical_nicklist);
    FPuts(save_file, (l_in) output_string);

    //sprintf(output_string,"#DELTA HELP1 - Enter a number of pixels for the padding in between the separater bar in multicolunned mode.\n");
    //FPuts(save_file,(l_in)output_string);
    //sprintf(output_string,"#DELTA HELP2 - Note: This power_user function is only read on startup! USE NUMBERS FOR THIS VALUE ONLY\n");
    //FPuts(save_file,(l_in)output_string);
    sprintf(output_string, "DELTA %i\n", my_settings.delta);
    FPuts(save_file, (l_in) output_string);

    sprintf(output_string, "LISTVIEW_TABS_WEIGHT %i\n", my_settings.listview_tabs_weight);
    FPuts(save_file, (l_in) output_string);

    sprintf(output_string, "NICKLIST_WEIGHT %lu\n", my_settings.nicklist_horizontal_weight);
    FPuts(save_file, (l_in) output_string);

    sprintf(output_string, "SHOW_JOINS_PARTS %d\n", my_settings.show_joins_parts);
    FPuts(save_file, (l_in) output_string);

    sprintf(output_string, "HIDE_USER_LIST_BUTTONS %i\n", my_settings.hide_user_list_buttons);
    FPuts(save_file, (l_in) output_string);

    sprintf(output_string, "HIDE_CHANNEL_MODE_BUTTONS %i\n", my_settings.hide_channel_mode_buttons);
    FPuts(save_file, (l_in) output_string);

    sprintf(output_string, "USER_MODES_BESIDE_NICKS %i\n", my_settings.user_modes_beside_nicks);
    FPuts(save_file, (l_in) output_string);

    /*
     sprintf(output_string,"DCC_RECV_FORMAT %s\n",my_settings.dcc_recv_format);
     FPuts(save_file,(l_in)output_string);

     sprintf(output_string,"DCC_SEND_FORMAT %s\n",my_settings.dcc_send_format);
     FPuts(save_file,(l_in)output_string);
     */

    //save all entries in the LV_alias nlist
    getmacro((Object*) WookieChat->LV_alias, MUIA_NList_Entries, &entries);

    for (unsigned int count = 0; count < entries; count++)
    {
        DoMethod((Object*) WookieChat->LV_alias, MUIM_NList_GetEntry, count, &work_buffer);
        strcpy(my_settings.command_alias[count], work_buffer);
        sprintf(output_string, "ALIAS %s\n", work_buffer);
        FPuts(save_file, (l_in) output_string);
        my_settings.number_of_command_aliases = count + 1;

    }

    Close(save_file);

    save_user_list_buttons_config();

}

void retrieve_settings()
{
    LONG a;
    char *b = malloc(sizeof(char) * 800);
    char *work_buffer = malloc(sizeof(char) * 800);

    getmacro((Object*) WookieChat->STR_nickname1, MUIA_String_Contents, &b);
    if (b)
        strcpy(nick, b);

    getmacro((Object*) WookieChat->STR_nickname2, MUIA_String_Contents, &b);
    if (b)
        strcpy(nick2, b);

    getmacro((Object*) WookieChat->STR_nickname3, MUIA_String_Contents, &b);
    if (b)
        strcpy(nick3, b);

    getmacro((Object*) WookieChat->CH_display_server_win, MUIA_Selected, &a);
    if (a)
        my_settings.display_server_window_on_startup = 1;
    else
        my_settings.display_server_window_on_startup = 0;

    getmacro((Object*) WookieChat->STR_how_many_lines_to_reload_channel, MUIA_String_Contents, &b);
    my_settings.how_many_lines_to_reload_channel = atoi(b);

    getmacro((Object*) WookieChat->STR_how_many_lines_to_reload_private, MUIA_String_Contents, &b);
    my_settings.how_many_lines_to_reload_private = atoi(b);

    getmacro((Object*) WookieChat->STR_highlight_label, MUIA_String_Contents, &b);
    if (b)
    {
        strcpy(my_settings.highlight, b);

        //convert string to lowercase
        for (unsigned int a = 0; a != strlen(my_settings.highlight) + 1; a++)
            my_settings.highlight[a] = ToLower(my_settings.highlight[a]);
        //set((Object*)WookieChat->STR_highlight_label,MUIA_String_Contents,&my_settings.highlight);

    }

    getmacro((Object*) WookieChat->CH_user_list_buttons_disable, MUIA_Selected, &a);
    if (a)
        my_settings.hide_user_list_buttons = 1;
    else
        my_settings.hide_user_list_buttons = 0;

    getmacro((Object*) WookieChat->CH_clone_detection, MUIA_Selected, &a);
    if (a)
        my_settings.clone_detection = 1;
    else
        my_settings.clone_detection = 0;

    getmacro((Object*) WookieChat->CH_remove_brackets, MUIA_Selected, &a);
    if (a)
        my_settings.remove_brackets = 1;
    else
        my_settings.remove_brackets = 0;

    //getmacro((Object*)WookieChat->CH_tabs,MUIA_Selected,&a);
    getmacro((Object*) WookieChat->CYCLE_tabs, MUIA_Cycle_Active, &a);
    if (a)
        my_settings.use_nlistobject_for_tabs = 0;
    else
        my_settings.use_nlistobject_for_tabs = 1;

    getmacro((Object*) WookieChat->CH_autoaccept, MUIA_Selected, &a);
    if (a)
        my_settings.dcc_recv_auto_accept = 1;
    else
        my_settings.dcc_recv_auto_accept = 0;

    getmacro((Object*) WookieChat->CH_graphical_nicklist, MUIA_Selected, &a);
    if (a)
        my_settings.graphical_nicklist = 1;
    else
        my_settings.graphical_nicklist = 0;

    getmacro((Object*) WookieChat->CH_graphical_smileys, MUIA_Selected, &a);
    if (a)
        my_settings.graphical_smilies = 1;
    else
        my_settings.graphical_smilies = 0;

    //getmacro((Object*)WookieChat->RA_graphical_smileys_background_colour,MUIA_Radio_Active,&a);
    getmacro((Object*) WookieChat->graphical_smileys_set, MUIA_Cycle_Active, &a);
    my_settings.graphical_smilies_background = (LONG) a;

    getmacro((Object*) WookieChat->RA_autoaccept, MUIA_Cycle_Active, &a);
    my_settings.dcc_recv_auto_accept_if_exists_action = a;

    getmacro((Object*) WookieChat->CH_autojoin_channels_when_kicked, MUIA_Selected, &a);
    if (a)
        my_settings.auto_rejoin_on_kick = 1;
    else
        my_settings.auto_rejoin_on_kick = 0;

    getmacro((Object*) WookieChat->CH_auto_open_query_tabs_when_msged, MUIA_Selected, &a);
    if (a)
        my_settings.open_query_on_privmsg = 1;
    else
        my_settings.open_query_on_privmsg = 0;

    getmacro((Object*) WookieChat->STR_label_3, MUIA_String_Contents, &b);
    my_settings.max_lines_for_buffers = atoi(b);

    getmacro((Object*) WookieChat->CH_label_2, MUIA_Selected, &a);
    if (a)
        my_settings.log_channels = 1;
    else
        my_settings.log_channels = 0;

    getmacro((Object*) WookieChat->CH_label_3, MUIA_Selected, &a);
    if (a)
        my_settings.log_private = 1;
    else
        my_settings.log_private = 0;

    getmacro((Object*) WookieChat->STR_samples_path, MUIA_String_Contents, &b);
    if (b)
        strcpy(my_settings.samples_path, b);

    getmacro((Object*) WookieChat->STR_sound_sample_tabopen, MUIA_String_Contents, &b);
    if (b)
        strcpy(my_settings.sound_sample_tabopen, b);

    getmacro((Object*) WookieChat->STR_sound_sample_newmsg, MUIA_String_Contents, &b);
    if (b)
        strcpy(my_settings.sound_sample_newmsg, b);

    getmacro((Object*) WookieChat->STR_sound_sample_highlight, MUIA_String_Contents, &b);
    if (b)
        strcpy(my_settings.sound_sample_highlight, b);
#ifndef __AROS__
    reload_sample(my_settings.sound_sample_highlight, SAMPLE_HIGHLIGHT);
#endif

    getmacro((Object*) WookieChat->RA_sound_sample_newmsg, MUIA_Cycle_Active, &a);
    //getmacro((Object*)WookieChat->RA_sound_sample_newmsg,MUIA_Radio_Active,&a);
    my_settings.play_sound_when_newmsg = a;
#ifndef __AROS__
    reload_sample(my_settings.sound_sample_newmsg, SAMPLE_NEWMSG);
#endif

    //getmacro((Object*)WookieChat->RA_sound_sample_tabopen,MUIA_Radio_Active,&a);
    getmacro((Object*) WookieChat->RA_sound_sample_tabopen, MUIA_Cycle_Active, &a);
    my_settings.play_sound_when_tabopen = a;
#ifndef __AROS__
    reload_sample(my_settings.sound_sample_tabopen, SAMPLE_TABOPEN);
#endif

    //getmacro((Object*)WookieChat->RA_sound_sample_highlight,MUIA_Radio_Active,&a);
    getmacro((Object*) WookieChat->RA_sound_sample_highlight, MUIA_Cycle_Active, &a);
    my_settings.play_sound_when_highlight = a;

    getmacro((Object*) WookieChat->STR_label_1, MUIA_String_Contents, &b);
    if (b)
        strcpy(my_settings.default_kickmsg, b);

    getmacro((Object*) WookieChat->STR_label_4, MUIA_String_Contents, &b);
    if (b)
        strcpy(my_settings.quit_msg, b);

    getmacro((Object*) WookieChat->STR_label_5, MUIA_String_Contents, &b);
    if (b)
        strcpy(my_settings.part_msg, b);

    getmacro((Object*) WookieChat->CH_label_4, MUIA_Selected, &a);
    if (a)
        my_settings.request_whois_info = 1;
    else
        my_settings.request_whois_info = 0;

    getmacro((Object*) WookieChat->CH_label_5, MUIA_Selected, &a);
    if (a)
        my_settings.open_private_tab = 1;
    else
        my_settings.open_private_tab = 0;

    getmacro((Object*) WookieChat->STR_label_0, MUIA_String_Contents, &b);
    if (b)
        strcpy(my_settings.time_stamp_string, b);

    getmacro((Object*) WookieChat->CH_label_0, MUIA_Selected, &a);
    if (a)
        my_settings.timestamp_config = 1;
    else
        my_settings.timestamp_config = 0;

    getmacro((Object*) WookieChat->PA_label_0, MUIA_String_Contents, &b);
    if (b)
        strcpy(my_settings.text_editor, b);

    getmacro((Object*) WookieChat->PA_browser, MUIA_String_Contents, &b);
    if (b)
        strcpy(my_settings.browser, b);

    getmacro((Object*) WookieChat->PA_dcc_label_0, MUIA_String_Contents, &b);
    if (b)
        strcpy(my_settings.download_path, b);

    getmacro((Object*) WookieChat->PA_dcc_label_1, MUIA_String_Contents, &b);
    if (b)
        strcpy(my_settings.upload_path, b);

    getmacro((Object*) WookieChat->CH_gethostid, MUIA_Selected, &a);
    if (a)
        my_settings.autogethostid = 1;
    else
        my_settings.autogethostid = 0;

    getmacro((Object*) WookieChat->CH_ipaddr_dcc, MUIA_Selected, &a);
    if (a)
        my_settings.ipaddr_use = 1;
    else
        my_settings.ipaddr_use = 0;

    getmacro((Object*) WookieChat->STR_ipaddr_dcc, MUIA_String_Contents, &b);
    if (b)
        strcpy(my_settings.ipaddr, b);

    getmacro((Object*) WookieChat->STR_port1_dcc, MUIA_String_Contents, &b);
    if (b)
        strcpy(my_settings.start_port, b);

    getmacro((Object*) WookieChat->STR_port2_dcc, MUIA_String_Contents, &b);
    if (b)
        strcpy(my_settings.end_port, b);

    getmacro((Object*) WookieChat->RA_nick_completion_style, MUIA_Cycle_Active, &a);
    my_settings.nick_completion = a;

    getmacro((Object*) WookieChat->CH_reconnect_to_same_server, MUIA_Selected, &a);
    if (a)
        my_settings.reconnect_to_same_server = 1;
    else
        my_settings.reconnect_to_same_server = 0;

    //getmacro((Object*)WookieChat->NB_Maximum_Retries,MUIA_Numeric_Value,&a);
    getmacro((Object*) WookieChat->NB_Maximum_Retries, MUIA_String_Contents, &b);
    my_settings.Maximum_Retries = (LONG) atoi(b);

    //getmacro((Object*)WookieChat->NB_Reconnect_Delay,MUIA_Numeric_Value,&a);
    getmacro((Object*) WookieChat->NB_Reconnect_Delay, MUIA_String_Contents, &b);
    my_settings.Reconnect_Delay = (LONG) atoi(b);

    //getmacro((Object*)WookieChat->NB_column_spacing,MUIA_Numeric_Value,&a);
    //my_settings.delta=(LONG)a;
    getmacro((Object*) WookieChat->NB_column_spacing, MUIA_String_Contents, &b);
    my_settings.delta = (LONG) atoi(b);

    //getmacro((Object*)WookieChat->NB_listview_tabs_weight,MUIA_Numeric_Value,&a);
    //my_settings.listview_tabs_weight=(LONG)a;

    getmacro((Object*) WookieChat->NB_listview_tabs_weight, MUIA_String_Contents, &b);
    my_settings.listview_tabs_weight = (LONG) atoi(b);

    //getmacro((Object*)WookieChat->NB_nicklist_horiz_weight,MUIA_Numeric_Value,&a);
    //my_settings.nicklist_horizontal_weight=(LONG)a;

    getmacro((Object*) WookieChat->NB_nicklist_horiz_weight, MUIA_String_Contents, &b);
    my_settings.nicklist_horizontal_weight = (LONG) atoi(b);

    check_column_size();
    set_column_size();

    //getmacro((Object*)WookieChat->NB_column_width_nick,MUIA_Numeric_Value,&a);
    //my_settings.nick_column_width=(LONG)a;

    //getmacro((Object*)WookieChat->NB_column_width_timestamp,MUIA_Numeric_Value,&a);
    //my_settings.timestamp_column_width=(LONG)a;

    getmacro((Object*) WookieChat->NB_column_width_nick, MUIA_String_Contents, &b);
    my_settings.nick_column_width = (LONG) atoi(b);

    getmacro((Object*) WookieChat->NB_column_width_timestamp, MUIA_String_Contents, &b);
    my_settings.timestamp_column_width = (LONG) atoi(b);

    getmacro((Object*) WookieChat->CH_split_logs, MUIA_Selected, &a);
    if (a)
        my_settings.splitup_logfiles = 1;
    else
        my_settings.splitup_logfiles = 0;

    getmacro((Object*) WookieChat->STR_logsize, MUIA_String_Contents, &b);
    my_settings.max_log_size = atoi(b);

    getmacro((Object*) WookieChat->CH_no_server_tabs, MUIA_Selected, &a);
    if (a)
        my_settings.no_server_tabs = 1;
    else
        my_settings.no_server_tabs = 0;

    getmacro((Object*) WookieChat->CH_sound_replayer, MUIA_Selected, &a);
    if (a)
        my_settings.use_external_sound_replayer = 1;
    else
        my_settings.use_external_sound_replayer = 0;

    getmacro((Object*) WookieChat->PA_sound_replayer, MUIA_String_Contents, &b);
    if (b)
        strcpy(my_settings.external_sound_replayer, b);

    getmacro((Object*) WookieChat->CH_hide_joins_parts, MUIA_Selected, &a);
    if (a)
        my_settings.show_joins_parts = 0;
    else
        my_settings.show_joins_parts = 1;

    getmacro((Object*) WookieChat->LV_alias, MUIA_NList_Entries, &entries);

    getmacro((Object*) WookieChat->CH_hide_channel_mode_gadgets, MUIA_Selected, &a);
    if (a)
        my_settings.hide_channel_mode_buttons = 1;
    else
        my_settings.hide_channel_mode_buttons = 0;

    getmacro((Object*) WookieChat->CH_user_modes, MUIA_Selected, &a);
    if (a)
        my_settings.user_modes_beside_nicks = 1;
    else
        my_settings.user_modes_beside_nicks = 0;

    for (unsigned int count = 0; count < entries; count++)
    {
        DoMethod((Object*) WookieChat->LV_alias, MUIM_NList_GetEntry, count, &work_buffer);
        strcpy(my_settings.command_alias[count], work_buffer);
        my_settings.number_of_command_aliases = count + 1;

    }

}

#define SECONDARY_CHARSET "US-ASCII"

void load_nick_settings()
{

    for (count = 0; local_charsets[count] != NULL; count++)
    {
        if (!stricmp(local_charsets[count], DEFAULT_CHARSET))
        {
            setmacro((Object*) WookieChat->CYCLE_local_charset, MUIA_Cycle_Active, count);
            local_charset = count;
            break;
        }
    }

    if (local_charsets[count] == NULL)
    {
        for (count = 0; local_charsets[count] != NULL; count++)
        {
            if (!stricmp(local_charsets[count], SECONDARY_CHARSET))
            {
                setmacro((Object*) WookieChat->CYCLE_local_charset, MUIA_Cycle_Active, count);
                local_charset = count;
                break;
            }
        }
    }
    //if(local_charsets[count]==NULL) local_charset=0;

    if (strstr(local_charsets[count], "UTF"))
    {

        for (count = 0; local_charsets[count] != NULL; count++)
        {
            if (!stricmp(local_charsets[count], DEFAULT_CHARSET))
            {
                setmacro((Object*) WookieChat->CYCLE_local_charset, MUIA_Cycle_Active, count);
                local_charset = count;
                break;
            }
        }

        if (local_charsets[count] == NULL)
        {
            for (count = 0; local_charsets[count] != NULL; count++)
            {
                if (!stricmp(local_charsets[count], SECONDARY_CHARSET))
                {
                    setmacro((Object*) WookieChat->CYCLE_local_charset, MUIA_Cycle_Active, count);
                    local_charset = count;
                    break;
                }
            }
        }

    }

    //these arent nick related, but this is a good place to setup the
    //ignore list in the "ignore list" data structure for the first time

    load_ignore_list();
    save_ignore_list();

    char *work_buffer = malloc(sizeof(char) * 800);
    //STRPTR work_buffer;
    char *len2; //variable used for file access

    BPTR newbptr_file = Open((_s_cs)"progdir:nick_settings.txt", MODE_OLDFILE);
    if (!newbptr_file)
        return;

    while (1)
    {

        len2 = (char*) FGets(newbptr_file, (STRPTR) work_buffer, 800);
        if (!len2)
            break;

        char *which_setting = strtok(work_buffer, " ");
        char *setting_value = strtok(NULL, "\n");

        if (!stricmp(which_setting, "NICK1"))
        {
            setmacro((Object*) WookieChat->STR_nickname1, MUIA_String_Contents, setting_value);
            if (setting_value)
                strcpy(nick, setting_value);

        }
        else if (!stricmp(which_setting, "NICK2"))
        {
            setmacro((Object*) WookieChat->STR_nickname2, MUIA_String_Contents, setting_value);
            if (setting_value)
                strcpy(nick2, setting_value);

        }
        else if (!stricmp(which_setting, "NICK3"))
        {
            setmacro((Object*) WookieChat->STR_nickname3, MUIA_String_Contents, setting_value);
            if (setting_value)
                strcpy(nick3, setting_value);

        }
        else if (!stricmp(which_setting, "REALNAME"))
        {
            setmacro((Object*) WookieChat->STR_realname, MUIA_String_Contents, setting_value);
            if (setting_value)
                strcpy(realname, setting_value);
        }
        else if (!stricmp(which_setting, "USERNAME"))
        {
            setmacro((Object*) WookieChat->STR_username, MUIA_String_Contents, setting_value);
            if (setting_value)
                strcpy(username, setting_value);

        }
        else if (!stricmp(which_setting, "DISPLAYSERVERWIN"))
        {
            if (setting_value)
            {

                setmacro((Object*) WookieChat->CH_display_server_win, MUIA_Selected, atoi(setting_value));
                my_settings.display_server_window_on_startup = atoi(setting_value);

            }

        }
        else if (!stricmp(which_setting, "CHARSET"))
        {
            if (setting_value)
            {
                if (Pro_Charsets_Enabled == FALSE)
                {
                }
                else
                {
                    setmacro((Object*) WookieChat->CYCLE_local_charset, MUIA_Cycle_Active, atoi(setting_value));
                    local_charset = atoi(setting_value);

                    if (strstr(local_charsets[local_charset], "UTF"))
                    {

                        for (count = 0; local_charsets[count] != NULL; count++)
                        {
                            if (!stricmp(local_charsets[count], DEFAULT_CHARSET))
                            {
                                setmacro((Object*) WookieChat->CYCLE_local_charset, MUIA_Cycle_Active, count);
                                local_charset = count;
                                break;
                            }
                        }

                        if (local_charsets[count] == NULL)
                        {
                            for (count = 0; local_charsets[count] != NULL; count++)
                            {
                                if (!stricmp(local_charsets[count], SECONDARY_CHARSET))
                                {
                                    setmacro((Object*) WookieChat->CYCLE_local_charset, MUIA_Cycle_Active, count);
                                    local_charset = count;
                                    break;
                                }
                            }
                        }

                    }
                }
            }

        }
    }

    if (newbptr_file)
        Close(newbptr_file);

    free(work_buffer);
    if (len2)
        free(len2);

}

void set_settings()
{

    //if(DoMethod((Object*)GROUP_ROOT_0,MUIM_Group_InitChange))
    {

        char work_buffer[1000];

        if (my_settings.use_nlistobject_for_tabs)
        {
            setmacro((Object*) WookieChat->CYCLE_tabs, MUIA_Cycle_Active, 0);
        }
        else
        {
            setmacro((Object*) WookieChat->CYCLE_tabs, MUIA_Cycle_Active, 1);
        }

        if (my_settings.use_column_display == 1)
            setmacro((Object*) MN_MultiColumnDisplay, MUIA_Menuitem_Checked, TRUE);
        else
            setmacro((Object*) MN_MultiColumnDisplay, MUIA_Menuitem_Checked, FALSE);

        if (my_settings.clone_detection == 1)
            setmacro((Object*) WookieChat->CH_clone_detection, MUIA_Selected, TRUE);
        else
            setmacro((Object*) WookieChat->CH_clone_detection, MUIA_Selected, FALSE);

        if (my_settings.hide_user_list_buttons == 1)
            setmacro((Object*) WookieChat->CH_user_list_buttons_disable, MUIA_Selected, TRUE);
        else
            setmacro((Object*) WookieChat->CH_user_list_buttons_disable, MUIA_Selected, FALSE);

        set_channel_clipboard_hook();

        DoMethod((Object*) WookieChat->LV_alias, MUIM_NList_Clear);

        for (int count = 0; count < my_settings.number_of_command_aliases; count++)
        {
            DoMethod((Object*) WookieChat->LV_alias, MUIM_NList_InsertSingle, my_settings.command_alias[count],
                    MUIV_NList_Insert_Bottom);

        }

        sprintf(work_buffer, "%i", my_settings.how_many_lines_to_reload_channel);
        setmacro((Object*) WookieChat->STR_how_many_lines_to_reload_channel, MUIA_String_Contents, work_buffer);

        sprintf(work_buffer, "%i", my_settings.how_many_lines_to_reload_private);
        setmacro((Object*) WookieChat->STR_how_many_lines_to_reload_private, MUIA_String_Contents, work_buffer);

        setmacro((Object*) WookieChat->STR_port1_dcc, MUIA_String_Contents, my_settings.start_port);

        setmacro((Object*) WookieChat->STR_port2_dcc, MUIA_String_Contents, my_settings.end_port);

        setmacro((Object*) WookieChat->STR_highlight_label, MUIA_String_Contents, my_settings.highlight);

        setmacro((Object*) WookieChat->STR_ipaddr_dcc, MUIA_String_Contents, my_settings.ipaddr);

        if (my_settings.ipaddr_use)
            setmacro((Object*) WookieChat->CH_ipaddr_dcc, MUIA_Selected, TRUE);
        else
            setmacro((Object*) WookieChat->CH_ipaddr_dcc, MUIA_Selected, FALSE);

        if (my_settings.autogethostid)
            setmacro((Object*) WookieChat->CH_gethostid, MUIA_Selected, TRUE);
        else
            setmacro((Object*) WookieChat->CH_gethostid, MUIA_Selected, FALSE);

        if (my_settings.dcc_recv_auto_accept)
            setmacro((Object*) WookieChat->CH_autoaccept, MUIA_Selected, TRUE);
        else
            setmacro((Object*) WookieChat->CH_autoaccept, MUIA_Selected, FALSE);

        setmacro((Object*) WookieChat->STR_sound_sample_tabopen, MUIA_String_Contents,
                my_settings.sound_sample_tabopen);

        setmacro((Object*) WookieChat->STR_sound_sample_newmsg, MUIA_String_Contents, my_settings.sound_sample_newmsg);

        setmacro((Object*) WookieChat->STR_sound_sample_highlight, MUIA_String_Contents,
                my_settings.sound_sample_highlight);

        setmacro((Object*) WookieChat->RA_sound_sample_tabopen, MUIA_Cycle_Active, my_settings.play_sound_when_tabopen);
        setmacro((Object*) WookieChat->RA_sound_sample_newmsg, MUIA_Cycle_Active, my_settings.play_sound_when_newmsg);
        setmacro((Object*) WookieChat->RA_sound_sample_highlight, MUIA_Cycle_Active,
                my_settings.play_sound_when_highlight);

        setmacro((Object*) WookieChat->RA_autoaccept, MUIA_Cycle_Active,
                my_settings.dcc_recv_auto_accept_if_exists_action);

        if (my_settings.auto_rejoin_on_kick)
            setmacro((Object*) WookieChat->CH_autojoin_channels_when_kicked, MUIA_Selected, TRUE);
        else
            setmacro((Object*) WookieChat->CH_autojoin_channels_when_kicked, MUIA_Selected, FALSE);

        if (my_settings.open_query_on_privmsg)
            setmacro((Object*) WookieChat->CH_auto_open_query_tabs_when_msged, MUIA_Selected, TRUE);
        else
            setmacro((Object*) WookieChat->CH_auto_open_query_tabs_when_msged, MUIA_Selected, FALSE);

        setmacro((Object*) WookieChat->STR_label_1, MUIA_String_Contents, my_settings.default_kickmsg);

        sprintf(work_buffer, "%ld", my_settings.max_lines_for_buffers);
        setmacro((Object*) WookieChat->STR_label_3, MUIA_String_Contents, work_buffer);

        if (my_settings.log_channels)
            setmacro((Object*) WookieChat->CH_label_2, MUIA_Selected, TRUE);
        else
            setmacro((Object*) WookieChat->CH_label_2, MUIA_Selected, FALSE);

        if (my_settings.log_private)
            setmacro((Object*) WookieChat->CH_label_3, MUIA_Selected, TRUE);
        else
            setmacro((Object*) WookieChat->CH_label_3, MUIA_Selected, FALSE);

        setmacro((Object*) WookieChat->STR_label_5, MUIA_String_Contents, my_settings.part_msg);

        setmacro((Object*) WookieChat->STR_label_4, MUIA_String_Contents, my_settings.quit_msg);
        setmacro((Object*) WookieChat->STR_samples_path, MUIA_String_Contents, my_settings.samples_path);

        setmacro((Object*) WookieChat->STR_label_0, MUIA_String_Contents, my_settings.time_stamp_string);

        if (my_settings.request_whois_info)
            setmacro((Object*) WookieChat->CH_label_4, MUIA_Selected, TRUE);
        else
            setmacro((Object*) WookieChat->CH_label_4, MUIA_Selected, FALSE);

        if (my_settings.open_private_tab)
            setmacro((Object*) WookieChat->CH_label_5, MUIA_Selected, TRUE);
        else
            setmacro((Object*) WookieChat->CH_label_5, MUIA_Selected, FALSE);

        if (my_settings.timestamp_config)
            setmacro((Object*) WookieChat->CH_label_0, MUIA_Selected, TRUE);
        else
            setmacro((Object*) WookieChat->CH_label_0, MUIA_Selected, FALSE);

        setmacro((Object*) WookieChat->PA_label_0, MUIA_String_Contents, my_settings.text_editor);

        setmacro((Object*) WookieChat->PA_browser, MUIA_String_Contents, my_settings.browser);

        setmacro((Object*) WookieChat->PA_dcc_label_0, MUIA_String_Contents, my_settings.download_path);
        setmacro((Object*) WookieChat->TX_download_path, MUIA_Text_Contents, my_settings.download_path);

        setmacro((Object*) WookieChat->PA_dcc_label_1, MUIA_String_Contents, my_settings.upload_path);
        setmacro((Object*) WookieChat->STR_PA_dcc_send_file, MUIA_String_Contents, my_settings.upload_path);

        setmacro((Object*) WookieChat->RA_nick_completion_style, MUIA_Cycle_Active, my_settings.nick_completion);

        if (my_settings.reconnect_to_same_server)
            setmacro((Object*) WookieChat->CH_reconnect_to_same_server, MUIA_Selected, TRUE);
        else
            setmacro((Object*) WookieChat->CH_reconnect_to_same_server, MUIA_Selected, FALSE);

        if (my_settings.remove_brackets)
            setmacro((Object*) WookieChat->CH_remove_brackets, MUIA_Selected, TRUE);
        else
            setmacro((Object*) WookieChat->CH_remove_brackets, MUIA_Selected, FALSE);

        sprintf(work_buffer, "%i", my_settings.Maximum_Retries);
        setmacro((Object*) WookieChat->NB_Maximum_Retries, MUIA_String_Contents, work_buffer);

        sprintf(work_buffer, "%i", my_settings.Reconnect_Delay);
        setmacro((Object*) WookieChat->NB_Reconnect_Delay, MUIA_String_Contents, work_buffer);

        setmacro((Object*) WookieChat->CH_no_server_tabs, MUIA_Selected, my_settings.no_server_tabs);

        sprintf(work_buffer, "%i", my_settings.max_log_size);
        setmacro((Object*) WookieChat->STR_logsize, MUIA_String_Contents, work_buffer);

        if (my_settings.splitup_logfiles)
            setmacro((Object*) WookieChat->CH_split_logs, MUIA_Selected, TRUE);
        else
            setmacro((Object*) WookieChat->CH_split_logs, MUIA_Selected, FALSE);

        if (my_settings.graphical_smilies)
            setmacro((Object*) WookieChat->CH_graphical_smileys, MUIA_Selected, TRUE);
        else
            setmacro((Object*) WookieChat->CH_graphical_smileys, MUIA_Selected, FALSE);

        if (my_settings.graphical_nicklist)
            setmacro((Object*) WookieChat->CH_graphical_nicklist, MUIA_Selected, TRUE);
        else
            setmacro((Object*) WookieChat->CH_graphical_nicklist, MUIA_Selected, FALSE);

        setmacro((Object*) WookieChat->graphical_smileys_set, MUIA_Cycle_Active,
                my_settings.graphical_smilies_background);

        if (my_settings.use_external_sound_replayer == 1)
            setmacro((Object*) WookieChat->CH_sound_replayer, MUIA_Selected, TRUE);
        else
            setmacro((Object*) WookieChat->CH_sound_replayer, MUIA_Selected, FALSE);

        setmacro((Object*) WookieChat->PA_sound_replayer, MUIA_String_Contents, my_settings.external_sound_replayer);

        sprintf(work_buffer, "%i", my_settings.listview_tabs_weight);
        setmacro((Object*) WookieChat->NB_listview_tabs_weight, MUIA_String_Contents, work_buffer); //MUIA_Numeric_Value,my_settings.listview_tabs_weight);

        sprintf(work_buffer, "%ld", my_settings.nicklist_horizontal_weight);
        setmacro((Object*) WookieChat->NB_nicklist_horiz_weight, MUIA_String_Contents, work_buffer); //MUIA_Numeric_Value,my_settings.nicklist_horizontal_weight);

        sprintf(work_buffer, "%i", my_settings.delta);

        setmacro((Object*) WookieChat->NB_column_spacing, MUIA_String_Contents, work_buffer); //MUIA_Numeric_Value,my_settings.delta);

        sprintf(work_buffer, "%i", my_settings.timestamp_column_width);
        setmacro((Object*) WookieChat->NB_column_width_timestamp, MUIA_String_Contents, work_buffer);

        sprintf(work_buffer, "%i", my_settings.nick_column_width);
        setmacro((Object*) WookieChat->NB_column_width_nick, MUIA_String_Contents, work_buffer);

        sprintf(listview_format, "COL=2 PIXWIDTH=%i, COL=1 DELTA=%i PIXWIDTH=%i BAR, DELTA=%i COL=0",
                my_settings.timestamp_column_width, my_settings.delta, my_settings.nick_column_width,
                my_settings.delta);

        if (status_current->current_query && my_settings.use_column_display == 1)
            setmacro((Object*) status_current->current_query->LV_channel, MUIA_NList_Format, listview_format);

        if (!my_settings.show_joins_parts)
            setmacro((Object*) WookieChat->CH_hide_joins_parts, MUIA_Selected, TRUE);
        else
            setmacro((Object*) WookieChat->CH_hide_joins_parts, MUIA_Selected, FALSE);

        if (my_settings.graphical_smilies)
            setmacro((Object*) WookieChat->smiley_choose_icon, MUIA_ShowMe, TRUE);
        else
            setmacro((Object*) WookieChat->smiley_choose_icon, MUIA_ShowMe, FALSE);

        if (my_settings.hide_channel_mode_buttons)
            setmacro((Object*) WookieChat->CH_hide_channel_mode_gadgets, MUIA_Selected, TRUE);
        else
            setmacro((Object*) WookieChat->CH_hide_channel_mode_gadgets, MUIA_Selected, FALSE);

        if (my_settings.user_modes_beside_nicks)
            setmacro((Object*) WookieChat->CH_user_modes, MUIA_Selected, TRUE);
        else
            setmacro((Object*) WookieChat->CH_user_modes, MUIA_Selected, FALSE);

        //create_user_list_buttons();

        //DoMethod((Object*)GROUP_ROOT_0,MUIM_Group_ExitChange);
    }

}

void load_settings()
{

    char *work_buffer = malloc(sizeof(char) * 800);
    char *len2; //variable used for file access

    BPTR newbptr_file = Open((_s_cs)"progdir:settings.txt", MODE_OLDFILE);
    //default values
    my_settings.sort_tabs_alphabetically = 1;
    my_settings.totalignores = 0;
    my_settings.number_of_command_aliases = 0;
    my_settings.use_column_display = 0;
    my_settings.urlgrabber = 1;
    my_settings.clone_detection = 0;
    my_settings.use_nlistobject_for_tabs = 0;
    my_settings.use_external_sound_replayer = 0;
    my_settings.use_ident = 0;
    my_settings.which_clipboard_style = NORMAL;
    my_settings.delta = 8;
    my_settings.listview_tabs_weight = 50;
    my_settings.nicklist_horizontal_weight = 160;
    my_settings.timestamp_column_width = 90;
    my_settings.nick_column_width = 150;
    my_settings.paste_delay_seconds = 1;
    my_settings.paste_delay_microseconds = 0;
    my_settings.show_joins_parts = 1;
    my_settings.hide_user_list_buttons = 0;
    my_settings.hide_channel_mode_buttons = 0;
    my_settings.user_modes_beside_nicks = 0;

    //strcpy(my_settings.dcc_recv_format,"WEIGHT=19 BAR,WEIGHT=15 BAR,WEIGHT=55 BAR,WEIGHT=10 BAR,WEIGHT=15 BAR,WEIGHT=15 BAR,WEIGHT=5 BAR,WEIGHT=15");
    //strcpy(my_settings.dcc_send_format,"WEIGHT=19 BAR,WEIGHT=15 BAR,WEIGHT=55 BAR,WEIGHT=10 BAR,WEIGHT=15 BAR,WEIGHT=15 BAR,WEIGHT=5 BAR,WEIGHT=15");

    //strcpy(my_settings.dcc_recv_format,"BAR,BAR,BAR,BAR,BAR,BAR,BAR,");
    //strcpy(my_settings.dcc_send_format,"BAR,BAR,BAR,BAR,BAR,BAR,BAR,");

    my_settings.remove_brackets = 1;
    setmacro((Object*) WookieChat->CH_remove_brackets, MUIA_Selected, TRUE);

    my_settings.graphical_nicklist = 1;
    setmacro((Object*) WookieChat->CH_graphical_nicklist, MUIA_Selected, TRUE);

    my_settings.graphical_smilies = 1;
    setmacro((Object*) WookieChat->CH_graphical_smileys, MUIA_Selected, TRUE);

    my_settings.graphical_smilies_background = 1;
    setmacro((Object*) WookieChat->graphical_smileys_set, MUIA_Cycle_Active, my_settings.graphical_smilies_background);

    my_settings.no_server_tabs = 0;
    setmacro((Object*) WookieChat->CH_no_server_tabs, MUIA_Selected, FALSE);

    my_settings.splitup_logfiles = 0;
    setmacro((Object*) WookieChat->CH_split_logs, MUIA_Selected, FALSE);

    my_settings.max_log_size = 400;
    setmacro((Object*) WookieChat->STR_logsize, MUIA_String_Contents, "400");

    my_settings.display_server_window_on_startup = 1;
    setmacro((Object*) WookieChat->CH_display_server_win, MUIA_Selected, TRUE);

    my_settings.nick_completion = MIRC_STYLE;
    setmacro((Object*) WookieChat->RA_nick_completion_style, MUIA_Cycle_Active, my_settings.nick_completion);

    setmacro((Object*) WookieChat->STR_sound_sample_tabopen, MUIA_String_Contents, "progdir:sounds/eagh");
    strcpy(my_settings.sound_sample_tabopen, "progdir:sounds/eagh");

    setmacro((Object*) WookieChat->STR_sound_sample_newmsg, MUIA_String_Contents, "progdir:sounds/beep");
    strcpy(my_settings.sound_sample_newmsg, "progdir:sounds/beep");

    setmacro((Object*) WookieChat->STR_sound_sample_highlight, MUIA_String_Contents, "progdir:sounds/explall.iff");
    strcpy(my_settings.sound_sample_highlight, "progdir:sounds/explall.iff");

    setmacro((Object*) WookieChat->STR_samples_path, MUIA_String_Contents, "progdir:sounds/");
    strcpy(my_settings.samples_path, "progdir:sounds/");

    setmacro((Object*) WookieChat->STR_how_many_lines_to_reload_channel, MUIA_String_Contents, "15");
    my_settings.how_many_lines_to_reload_channel = 15;

    setmacro((Object*) WookieChat->STR_how_many_lines_to_reload_private, MUIA_String_Contents, "15");
    my_settings.how_many_lines_to_reload_private = 15;

    setmacro((Object*) WookieChat->STR_port1_dcc, MUIA_String_Contents, "1025");
    strcpy(my_settings.start_port, "1025");

    setmacro((Object*) WookieChat->STR_port2_dcc, MUIA_String_Contents, "1035");
    strcpy(my_settings.end_port, "1035");

    my_settings.auto_rejoin_on_kick = 1;
    if (my_settings.auto_rejoin_on_kick)
        setmacro((Object*) WookieChat->CH_autojoin_channels_when_kicked, MUIA_Selected, TRUE);

    my_settings.open_query_on_privmsg = 1;
    if (my_settings.open_query_on_privmsg)
        setmacro((Object*) WookieChat->CH_auto_open_query_tabs_when_msged, MUIA_Selected, TRUE);

    setmacro((Object*) WookieChat->STR_label_3, MUIA_String_Contents, "300");
    my_settings.max_lines_for_buffers = 300;

    setmacro((Object*) WookieChat->STR_label_0, MUIA_String_Contents, "[%h:%m:%s]");
    strcpy(my_settings.time_stamp_string, "[%h:%m:%s]");

    my_settings.request_whois_info = 1;
    if (my_settings.request_whois_info)
        setmacro((Object*) WookieChat->CH_label_4, MUIA_Selected, TRUE);

    my_settings.open_private_tab = 1;
    if (my_settings.open_private_tab)
        setmacro((Object*) WookieChat->CH_label_5, MUIA_Selected, TRUE);

    my_settings.timestamp_config = 1;
    if (my_settings.timestamp_config)
        setmacro((Object*) WookieChat->CH_label_0, MUIA_Selected, TRUE);

    setmacro((Object*) WookieChat->PA_label_0, MUIA_String_Contents, "c:ed");
    strcpy(my_settings.text_editor, "c:ed");

    setmacro((Object*) WookieChat->PA_browser, MUIA_String_Contents, "");
    strcpy(my_settings.browser, "");

    DoMethod((Object*) WookieChat->LV_alias, MUIM_NList_Clear);

    my_settings.reconnect_to_same_server = 1;
    setmacro((Object*) WookieChat->CH_reconnect_to_same_server, MUIA_Selected, TRUE);

    /*my_settings.Maximum_Retries=7;
     setmacro((Object*)WookieChat->NB_Maximum_Retries,MUIA_Numeric_Value,my_settings.Maximum_Retries);

     my_settings.Reconnect_Delay=30;
     setmacro((Object*)WookieChat->NB_Reconnect_Delay,MUIA_Numeric_Value,my_settings.Reconnect_Delay);
     */
    my_settings.Maximum_Retries = 7;
    sprintf(work_buffer, "%i", my_settings.Maximum_Retries);
    setmacro((Object*) WookieChat->NB_Maximum_Retries, MUIA_String_Contents, work_buffer);

    my_settings.Reconnect_Delay = 10;
    sprintf(work_buffer, "%i", my_settings.Reconnect_Delay);
    setmacro((Object*) WookieChat->NB_Reconnect_Delay, MUIA_String_Contents, work_buffer);

    my_settings.ipaddr_use = 0;
    setmacro((Object*) WookieChat->CH_ipaddr_dcc, MUIA_Selected, FALSE);

    //end of default values
    if (!newbptr_file)
        return;

    while (1)
    {

        len2 = (char*) FGets(newbptr_file, (STRPTR) work_buffer, 800);
        if (!len2)
            break;

        char *which_setting = strtok(work_buffer, " ");
        char *setting_value = strtok(NULL, "\n");

        if (!stricmp(which_setting, "NICK1"))
        {
            setmacro((Object*) WookieChat->STR_nickname1, MUIA_String_Contents, setting_value);
            if (setting_value)
                strcpy(nick, setting_value);

        }
        else if (!stricmp(which_setting, "NICK2"))
        {
            setmacro((Object*) WookieChat->STR_nickname2, MUIA_String_Contents, setting_value);
            if (setting_value)
                strcpy(nick2, setting_value);

        }
        else if (!stricmp(which_setting, "NICK3"))
        {
            setmacro((Object*) WookieChat->STR_nickname3, MUIA_String_Contents, setting_value);
            if (setting_value)
                strcpy(nick3, setting_value);

        }
        else if (!stricmp(which_setting, "REALNAME"))
        {
            setmacro((Object*) WookieChat->STR_realname, MUIA_String_Contents, setting_value);
            if (setting_value)
                strcpy(realname, setting_value);
        }
        else if (!stricmp(which_setting, "USERNAME"))
        {
            setmacro((Object*) WookieChat->STR_username, MUIA_String_Contents, setting_value);
            if (setting_value)
                strcpy(username, setting_value);

        }
        else if (!stricmp(which_setting, "SORT_TABS_ALPHABETICALLY"))
        {
            if (setting_value)
            {
                my_settings.sort_tabs_alphabetically = atoi(setting_value);

            }
            else
                my_settings.sort_tabs_alphabetically = 1;

        }
        else if (!stricmp(which_setting, "URLGRABBER"))
        {
            if (setting_value)
            {
                my_settings.urlgrabber = atoi(setting_value);

            }
            else
                my_settings.urlgrabber = 1;

        }
        else if (!stricmp(which_setting, "USE_EXTERNAL_SOUND_REPLAYER"))
        {
            if (setting_value)
            {
                my_settings.use_external_sound_replayer = atoi(setting_value);

            }
            else
                my_settings.use_external_sound_replayer = 0;

        }
        else if (!stricmp(which_setting, "EXTERNAL_SOUND_REPLAYER"))
        {
            if (setting_value)
                strcpy(my_settings.external_sound_replayer, setting_value);
            else
                strcpy(my_settings.external_sound_replayer, "Unconfigured_please_choose_a_player");

        }
        else if (!stricmp(which_setting, "TIMESTAMP_COLUMN_WIDTH"))
        {
            if (setting_value)
                my_settings.timestamp_column_width = atoi(setting_value);

            if (my_settings.timestamp_column_width == -1)
                my_settings.timestamp_column_width = 90;

        }
        else if (!stricmp(which_setting, "NICK_COLUMN_WIDTH"))
        {
            if (setting_value)
                my_settings.nick_column_width = atoi(setting_value);

            if (my_settings.nick_column_width == -1)
                my_settings.nick_column_width = 150;

        }
        else if (!stricmp(which_setting, "ENABLE_CLONE_DETECTION"))
        {
            if (atoi(setting_value) == 1)
            {
                my_settings.clone_detection = 1;
            }
            else
            {
                my_settings.clone_detection = 0;
            }

        }
        else if (!stricmp(which_setting, "COLUMN_STYLE_CLIPBOARD"))
        {
            if (atoi(setting_value) == 1)
            {
                my_settings.which_clipboard_style = COLUMNS;
            }
            else
            {
                my_settings.which_clipboard_style = NORMAL;
            }
        }
        else if (!stricmp(which_setting, "USE_IDENT_SERVER"))
        {
            if (atoi(setting_value) == 1)
                my_settings.use_ident = 1;
            else
                my_settings.use_ident = 0;

        }
        else if (!stricmp(which_setting, "BUTTON_TABS"))
        {
            if (atoi(setting_value) == 1)
                my_settings.use_nlistobject_for_tabs = 1;
            else
                my_settings.use_nlistobject_for_tabs = 0;

        }
        else if (!stricmp(which_setting, "REMOVE_BRACKETS"))
        {

            if (atoi(setting_value) == 1)
                my_settings.remove_brackets = 1;
            else
                my_settings.remove_brackets = 0;

        }
        else if (!stricmp(which_setting, "USE_COLUMN_DISPLAY"))
        {
            if (atoi(setting_value) == 1)
            {
                my_settings.use_column_display = 1;
            }
            else
            {
                my_settings.use_column_display = 0;
            }

        }
        else if (!stricmp(which_setting, "ALIAS"))
        {
            DoMethod((Object*) WookieChat->LV_alias, MUIM_NList_InsertSingle, setting_value, MUIV_NList_Insert_Bottom);
            if (setting_value)
            {
                strcpy(my_settings.command_alias[my_settings.number_of_command_aliases], setting_value);
                my_settings.number_of_command_aliases++;
            }

        }
        else if (!stricmp(which_setting, "RELOAD_CHAN"))
        {
            //set((Object*)WookieChat->STR_how_many_lines_to_reload_channel,MUIA_String_Contents,setting_value);
            if (setting_value)
                my_settings.how_many_lines_to_reload_channel = atoi(setting_value);
            else
                my_settings.how_many_lines_to_reload_channel = 15;

        }
        else if (!stricmp(which_setting, "RELOAD_PRIVATE"))
        {
            //set((Object*)WookieChat->STR_how_many_lines_to_reload_private,MUIA_String_Contents,setting_value);
            if (setting_value)
                my_settings.how_many_lines_to_reload_private = atoi(setting_value);
            else
                my_settings.how_many_lines_to_reload_private = 15;
        }
        else if (!stricmp(which_setting, "DCC_START_PORT"))
        {
            //set((Object*)WookieChat->STR_port1_dcc,MUIA_String_Contents,setting_value);
            if (setting_value)
                strcpy(my_settings.start_port, setting_value);
            else
                strcpy(my_settings.start_port, "1025");

        }
        else if (!stricmp(which_setting, "DCC_END_PORT"))
        {
            //set((Object*)WookieChat->STR_port2_dcc,MUIA_String_Contents,setting_value);
            if (setting_value)
                strcpy(my_settings.end_port, setting_value);
            else
                strcpy(my_settings.end_port, "1035");

        }
        else if (!stricmp(which_setting, "HIGHLIGHT"))
        {
            //set((Object*)WookieChat->STR_highlight_label,MUIA_String_Contents,setting_value);
            if (setting_value)
                strcpy(my_settings.highlight, setting_value);

        }
        else if (!stricmp(which_setting, "IPADDR_USE"))
        {
            my_settings.ipaddr_use = atoi(setting_value);

        }
        else if (!stricmp(which_setting, "IPADDR"))
        {
            if (setting_value)
                strcpy(my_settings.ipaddr, setting_value);
            else
                strcpy(my_settings.ipaddr, "");

        }
        else if (!stricmp(which_setting, "DCC_GET_IP"))
        {
            my_settings.autogethostid = atoi(setting_value);
            //if(my_settings.autogethostid) setmacro((Object*)WookieChat->CH_gethostid,MUIA_Selected,TRUE);
            //    else setmacro((Object*)WookieChat->CH_gethostid,MUIA_Selected,FALSE);

        }
        else if (!stricmp(which_setting, "DCC_AUTO_ACCEPT"))
        {
            my_settings.dcc_recv_auto_accept = atoi(setting_value);
            //if(my_settings.dcc_recv_auto_accept) setmacro((Object*)WookieChat->CH_autoaccept,MUIA_Selected,TRUE);
            //  else setmacro((Object*)WookieChat->CH_autoaccept,MUIA_Selected,FALSE);

        }
        else if (!stricmp(which_setting, "SAMPLE_TABOPEN"))
        {
            if (setting_value)
                strcpy(my_settings.sound_sample_tabopen, setting_value);

        }
        else if (!stricmp(which_setting, "SAMPLE_NEWMSG"))
        {
            if (setting_value)
                strcpy(my_settings.sound_sample_newmsg, setting_value);

        }

        else if (!stricmp(which_setting, "SAMPLE_HIGHLIGHT"))
        {
            //set((Object*)WookieChat->STR_sound_sample_highlight,MUIA_String_Contents,setting_value);
            if (setting_value)
                strcpy(my_settings.sound_sample_highlight, setting_value);

        }
        else if (!stricmp(which_setting, "PLAY_SOUND_WHEN_TABOPEN"))
        {
            my_settings.play_sound_when_tabopen = atoi(setting_value);

        }
        else if (!stricmp(which_setting, "PLAY_SOUND_WHEN_NEWMSG"))
        {
            my_settings.play_sound_when_newmsg = atoi(setting_value);

        }

        else if (!stricmp(which_setting, "PLAY_SOUND_WHEN_HIGHLIGHT"))
        {
            my_settings.play_sound_when_highlight = atoi(setting_value);
            //set((Object*)WookieChat->RA_sound_sample_highlight,MUIA_Radio_Active,my_settings.play_sound_when_highlight);

        }

        else if (!stricmp(which_setting, "DCC_AUTO_IF_EXISTS"))
        {
            my_settings.dcc_recv_auto_accept_if_exists_action = atoi(setting_value);
            //set((Object*)WookieChat->RA_autoaccept,MUIA_Radio_Active,my_settings.dcc_recv_auto_accept_if_exists_action);

        }
        else if (!stricmp(which_setting, "AUTO_REJOIN"))
        {
            my_settings.auto_rejoin_on_kick = atoi(setting_value);
            //if(my_settings.auto_rejoin_on_kick) setmacro((Object*)WookieChat->CH_autojoin_channels_when_kicked,MUIA_Selected,TRUE);
            //  else setmacro((Object*)WookieChat->CH_autojoin_channels_when_kicked,MUIA_Selected,FALSE);

        }
        else if (!stricmp(which_setting, "AUTO_OPEN_TABS"))
        {
            my_settings.open_query_on_privmsg = atoi(setting_value);
            //if(my_settings.open_query_on_privmsg) setmacro((Object*)WookieChat->CH_auto_open_query_tabs_when_msged,MUIA_Selected,TRUE);
            //  else setmacro((Object*)WookieChat->CH_auto_open_query_tabs_when_msged,MUIA_Selected,FALSE);

        }
        else if (!stricmp(which_setting, "KICK_MSG"))
        {

            if (setting_value)
            {
                //set((Object*)WookieChat->STR_label_1,MUIA_String_Contents,setting_value);
                strcpy(my_settings.default_kickmsg, setting_value);

            }
        }
        else if (!stricmp(which_setting, "BUFFER_SIZE"))
        {
            if (setting_value)
            {
                //set((Object*)WookieChat->STR_label_3,MUIA_String_Contents,setting_value);
                my_settings.max_lines_for_buffers = atoi(setting_value);

            }

        }
        else if (!stricmp(which_setting, "LOG_CHANNELS"))
        {
            if (setting_value)
            {
                my_settings.log_channels = atoi(setting_value);

            }

            //if(my_settings.log_channels) setmacro((Object*)WookieChat->CH_label_2,MUIA_Selected,TRUE);
            //    else setmacro((Object*)WookieChat->CH_label_2,MUIA_Selected,FALSE);

        }

        else if (!stricmp(which_setting, "LOG_PRIVATE"))
        {
            if (setting_value)
            {
                my_settings.log_private = atoi(setting_value);

            }
            //if(my_settings.log_private) setmacro((Object*)WookieChat->CH_label_3,MUIA_Selected,TRUE);
            //    else setmacro((Object*)WookieChat->CH_label_3,MUIA_Selected,FALSE);

        }

        else if (!stricmp(which_setting, "PART_MSG"))
        {
            if (setting_value)
            {
                //set((Object*)WookieChat->STR_label_5,MUIA_String_Contents,setting_value);
                strcpy(my_settings.part_msg, setting_value);

            }

        }

        else if (!stricmp(which_setting, "QUIT_MSG"))
        {
            if (setting_value)
            {
                //set((Object*)WookieChat->STR_label_4,MUIA_String_Contents,setting_value);
                strcpy(my_settings.quit_msg, setting_value);

            }

        }
        else if (!stricmp(which_setting, "SAMPLES_PATH"))
        {
            if (setting_value)
            {
                //set((Object*)WookieChat->STR_samples_path,MUIA_String_Contents,setting_value);
                strcpy(my_settings.samples_path, setting_value);

            }

        }
        else if (!stricmp(which_setting, "TIME_STAMP_STRING"))
        {
            if (setting_value)
            {
                //set((Object*)WookieChat->STR_label_0,MUIA_String_Contents,setting_value);
                strcpy(my_settings.time_stamp_string, setting_value);

            }

        }
        else if (!stricmp(which_setting, "NICKLIST_WHOIS"))
        {
            if (setting_value)
            {
                my_settings.request_whois_info = atoi(setting_value);

            }
            //if(my_settings.request_whois_info) setmacro((Object*)WookieChat->CH_label_4,MUIA_Selected,TRUE);
            //    else setmacro((Object*)WookieChat->CH_label_4,MUIA_Selected,FALSE);

        }
        else if (!stricmp(which_setting, "NICKLIST_QUERY"))
        {
            if (setting_value)
            {
                my_settings.open_private_tab = atoi(setting_value);

            }
            //if(my_settings.open_private_tab) setmacro((Object*)WookieChat->CH_label_5,MUIA_Selected,TRUE);
            //    else setmacro((Object*)WookieChat->CH_label_5,MUIA_Selected,FALSE);

        }
        else if (!stricmp(which_setting, "TIME_STAMP"))
        {

            if (setting_value)
            {
                my_settings.timestamp_config = atoi(setting_value);

                //if(my_settings.timestamp_config) setmacro((Object*)WookieChat->CH_label_0,MUIA_Selected,TRUE);
                //    else setmacro((Object*)WookieChat->CH_label_0,MUIA_Selected,FALSE);

            }

        }
        else if (!stricmp(which_setting, "TEXT_EDITOR"))
        {
            if (setting_value)
            {
                //set((Object*)WookieChat->PA_label_0,MUIA_String_Contents,setting_value);
                strcpy(my_settings.text_editor, setting_value);

            }

        }
        else if (!stricmp(which_setting, "BROWSER"))
        {
            if (setting_value)
            {
                //set((Object*)WookieChat->PA_browser,MUIA_String_Contents,setting_value);
                strcpy(my_settings.browser, setting_value);

            }

        }
        else if (!stricmp(which_setting, "DOWNLOAD_PATH"))
        {
            if (setting_value)
            {
                //set((Object*)WookieChat->PA_dcc_label_0,MUIA_String_Contents,setting_value);
                strcpy(my_settings.download_path, setting_value);

            }
            else
            {
                //strcpy(my_settings.download_path,"RAM:");
                strcpy(my_settings.download_path, wookie_folder);
            }

        }
        else if (!stricmp(which_setting, "UPLOAD_PATH"))
        {
            if (setting_value)
            {
                //set((Object*)WookieChat->PA_dcc_label_0,MUIA_String_Contents,setting_value);
                strcpy(my_settings.upload_path, setting_value);

            }

        }

        else if (!stricmp(which_setting, "NICK_COMPLETION_STYLE"))
        {
            if (setting_value)
                my_settings.nick_completion = atoi(setting_value);
            //set((Object*)WookieChat->RA_nick_completion_style,MUIA_Radio_Active,my_settings.nick_completion);

        }
        else if (!stricmp(which_setting, "RECONNECT_TO_SAME_SERVER"))
        {

            my_settings.reconnect_to_same_server = atoi(setting_value);

            //if(my_settings.reconnect_to_same_server)
            //    setmacro((Object*)WookieChat->CH_reconnect_to_same_server,MUIA_Selected,TRUE);
            //else
            //    setmacro((Object*)WookieChat->CH_reconnect_to_same_server,MUIA_Selected,FALSE);

        }
        else if (!stricmp(which_setting, "RECONNECT_MAX_RETRIES"))
        {
            my_settings.Maximum_Retries = atoi(setting_value);

            //set((Object*)WookieChat->NB_Maximum_Retries,MUIA_Numeric_Value,my_settings.Maximum_Retries);

        }
        else if (!stricmp(which_setting, "RECONNECT_DELAY"))
        {

            my_settings.Reconnect_Delay = atoi(setting_value);

            //set((Object*)WookieChat->NB_Reconnect_Delay,MUIA_Numeric_Value,my_settings.Reconnect_Delay);

        }
        else if (!stricmp(which_setting, "SPLITUP_LOGFILES"))
        {

            my_settings.splitup_logfiles = atoi(setting_value);

        }
        else if (!stricmp(which_setting, "MAX_LOGFILES_SIZE"))
        {

            my_settings.max_log_size = atoi(setting_value);

        }
        else if (!stricmp(which_setting, "NO_SERVER_TABS"))
        {

            my_settings.no_server_tabs = atoi(setting_value);

        }
        else if (!stricmp(which_setting, "GRAPHICAL_SMILIES"))
        {

            my_settings.graphical_smilies = atoi(setting_value);

        }
        else if (!stricmp(which_setting, "GRAPHICAL_SMILIES_BACKGROUND_COLOUR"))
        {

            my_settings.graphical_smilies_background = atoi(setting_value);

        }
        else if (!stricmp(which_setting, "DELTA"))
        {
            if (setting_value)
                my_settings.delta = atoi(setting_value);
        }
        else if (!stricmp(which_setting, "GRAPHICAL_NICKLIST"))
        {

            my_settings.graphical_nicklist = atoi(setting_value);

        }
        else if (!stricmp(which_setting, "LISTVIEW_TABS_WEIGHT"))
        {

            my_settings.listview_tabs_weight = atoi(setting_value);

        }
        else if (!stricmp(which_setting, "NICKLIST_WEIGHT"))
        {

            my_settings.nicklist_horizontal_weight = atoi(setting_value);

        }
        else if (!stricmp(which_setting, "PASTE_DELAY_SECONDS"))
        {

            my_settings.paste_delay_seconds = atoi(setting_value);

        }
        else if (!stricmp(which_setting, "PASTE_DELAY_MICROSECONDS"))
        {

            my_settings.paste_delay_microseconds = atoi(setting_value);
            if (my_settings.paste_delay_microseconds < 250000)
                my_settings.paste_delay_microseconds = 250000;

        }
        else if (!stricmp(which_setting, "SHOW_JOINS_PARTS"))
        {

            if (atoi(setting_value))
                my_settings.show_joins_parts = 1;
            else
                my_settings.show_joins_parts = 0;

        }
        else if (!stricmp(which_setting, "HIDE_USER_LIST_BUTTONS"))
        {

            if (atoi(setting_value))
                my_settings.hide_user_list_buttons = 1;
            else
                my_settings.hide_user_list_buttons = 0;

        }
        else if (!stricmp(which_setting, "HIDE_CHANNEL_MODE_BUTTONS"))
        {

            if (atoi(setting_value))
                my_settings.hide_channel_mode_buttons = 1;
            else
                my_settings.hide_channel_mode_buttons = 0;

        }
        else if (!stricmp(which_setting, "USER_MODES_BESIDE_NICKS"))
        {

            if (atoi(setting_value))
                my_settings.user_modes_beside_nicks = 1;
            else
                my_settings.user_modes_beside_nicks = 0;

        }
    }

    if (newbptr_file)
        Close(newbptr_file);

    sprintf(listview_format, "COL=2 PIXWIDTH=%i, COL=1 DELTA=%i PIXWIDTH=%i BAR, DELTA=%i COL=0",
            my_settings.timestamp_column_width, my_settings.delta, my_settings.nick_column_width, my_settings.delta);
}

struct user_list_buttons_array buttons[MAX_BUTTONS + 1];

#define USER_LIST_BUTTONS_FILE "progdir:user_list_buttons.txt"

#define EVENTS_SETTINGS_FILE "progdir:events.txt"

void get_events_settings(void)
{

    getmacro((Object*) LV_events, MUIA_NList_Entries, &entries);
    char *work_buffer;
    char *string1;
    char breakup_buffer[800];

    for (unsigned int count = 0; count < MAX_EVENTS; count++)
    {
        if (DEBUG)
            printf("retrieving event %d..\n", count);

        DoMethod((Object*) LV_events, MUIM_NList_GetEntry, count, &work_buffer);
        if (work_buffer)
        {
            stccpy(breakup_buffer, work_buffer, 800);

            strtok(breakup_buffer, "\033");

            if ((string1 = strtok(NULL, " ")))
                my_settings.events[count].use_when = atoi(string1);

            if ((string1 = strtok(NULL, "\033")))
                stccpy(my_settings.events[count].arexx_script, string1, 800);

            if ((string1 = strtok(NULL, "\n")))
                stccpy(my_settings.events[count].arexx_script2, string1, 800);

        }
        else
            break;
    }
}

int save_events_settings(void)
{

    BPTR save_file = Open((_s_cs)EVENTS_SETTINGS_FILE, MODE_NEWFILE);
    if (!save_file)
        return 0;

    getmacro((Object*) LV_events, MUIA_NList_Entries, &entries);
    char *work_buffer;
    char *string1;
    char breakup_buffer[800];

    for (unsigned int count = 0; count < MAX_EVENTS; count++)
    {
        if (DEBUG)
            printf("saving event %d..\n", count);

        DoMethod((Object*) LV_events, MUIM_NList_GetEntry, count, &work_buffer);
        if (work_buffer)
        {
            stccpy(breakup_buffer, work_buffer, 800);

            strtok(breakup_buffer, "\033");
            if ((string1 = strtok(NULL, "")))
            {
                sprintf(output_string, "%s\n", string1);
                FPuts(save_file, (l_in) output_string);
            }
        }
        else
            break;
    }
    strcpy(output_string, "\n");
    FPuts(save_file, (l_in) output_string);

    if (DEBUG)
        printf("finished saving events, closing save file..\n");

    Close(save_file);

    return 1;

}

int load_events_settings(void)
{

    BPTR newbptr_file = Open((_s_cs)EVENTS_SETTINGS_FILE, MODE_READWRITE);
    if (!newbptr_file)
        return 0;

    char work_buffer[800];
    char insert_string[800];
    char insert_string2[800];

    for (int count = 0; count < MAX_EVENTS; count++)
    {

        if (FGets(newbptr_file, (STRPTR) work_buffer, 800))
        {
            if ((string1 = strtok(work_buffer, " ")))
                my_settings.events[count].use_when = atoi(string1);

            if ((string1 = strtok(NULL, "\033")))
                stccpy(my_settings.events[count].arexx_script, string1, 800);

            if ((string1 = strtok(NULL, "\n")))
                stccpy(my_settings.events[count].arexx_script2, string1, 800);
        }
        else
        {
            my_settings.events[count].use_when = 0;

            strcpy(my_settings.events[count].arexx_script, "");

            strcpy(my_settings.events[count].arexx_script2, "");
        }

        sprintf(insert_string, "%d %s\033%s", my_settings.events[count].use_when,
                my_settings.events[count].arexx_script, my_settings.events[count].arexx_script2);

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

        strcat(insert_string2, insert_string);

        DoMethod((Object*) LV_events, MUIM_NList_InsertSingle, insert_string2, MUIV_NList_Insert_Bottom);

        //else break;

    }

    Close(newbptr_file);

    return 0;

}

int load_user_list_buttons_config(void)
{

    BPTR newbptr_file = Open((_s_cs)USER_LIST_BUTTONS_FILE, MODE_OLDFILE);
    if (!newbptr_file)
        return 0;

    char work_buffer[800];

    char name[800];
    char command[800];
    int count = 0;

    for (;;)
    {
        if (FGets(newbptr_file, (STRPTR) work_buffer, 200))
        {
            stccpy(name, work_buffer, USERLIST_NAME_SIZE - 1);

        }
        else
            break;

        if (FGets(newbptr_file, (STRPTR) work_buffer, 800))
        {
            stccpy(command, work_buffer, USERLIST_COMMAND_SIZE - 1);

        }
        else
            break;

        if (strlen(name) == 1)
            break;
        if (strlen(command) == 1)
            break;

        name[strlen(name) - 1] = '\0';
        command[strlen(command) - 1] = '\0';

        if (count > MAX_BUTTONS - 1)
            break;

        //printf("%d name:%s\n%d command:%s\n",count,name,count,command);
        sprintf(work_buffer, "%s\033%s", name, command);
        DoMethod((Object*) WookieChat->LV_user_list_buttons, MUIM_NList_InsertSingle, work_buffer,
                MUIV_NList_Insert_Bottom);

        buttons[count].num = count + SECOND_SET_OF_RETURNIDS + 100;
        strcpy(buttons[count].name, name);
        strcpy(buttons[count].command, command);
        buttons[count].BT_click = NULL;
        //buttons[count].BT_click=SimpleButton(buttons[count].name);
        count++;

    }

    Close(newbptr_file);

    return 0;

}

int create_user_list_buttons(void)
{
    //if(DoMethod((Object*)WookieChat->GR_click_user_list_buttons,MUIM_Group_InitChange))
    {

        char *work_buffer;
        char work_buffer2[1000];

        getmacro((Object*) WookieChat->LV_user_list_buttons, MUIA_NList_Entries, &entries);

        for (unsigned int count = 0; count < entries && count < 100; count++)
        {
            DoMethod((Object*) WookieChat->LV_user_list_buttons, MUIM_NList_GetEntry, count, &work_buffer);
            if (work_buffer)
            {
                strcpy(work_buffer2, work_buffer);
                string1 = strtok(work_buffer2, "\033");
                string2 = strtok(NULL, "");
                if (string2)
                {
                    buttons[count].num = count + SECOND_SET_OF_RETURNIDS + 100;
                    strcpy(buttons[count].name, string1);
                    strcpy(buttons[count].command, string2);

                }
                else
                    break;

            }
            else
                break;
        }

        getmacro((Object*) WookieChat->GR_click_user_list_buttons, MUIA_Group_ChildList, &list);
        object_state = list->mlh_Head;
        while ((member_object = NextObject((Object**) &object_state)))
        {
            DoMethod((Object*) WookieChat->GR_click_user_list_buttons, OM_REMMEMBER, (Object*) member_object);
            MUI_DisposeObject((Object*) member_object);
            //member_object=NULL;
        }

        getmacro((Object*) WookieChat->LV_user_list_buttons, MUIA_NList_Entries, &entries);

        for (unsigned int count2 = 0; count2 < entries && count2 < MAX_BUTTONS; count2++)
        {
            buttons[count2].BT_click = SimpleButton(buttons[count2].name);
            setmacro((Object*) buttons[count2].BT_click, MUIA_Text_SetMin, FALSE);
            DoMethod((Object*) WookieChat->GR_click_user_list_buttons, OM_ADDMEMBER,
                    (Object*) buttons[count2].BT_click);
            DoMethod((Object*) buttons[count2].BT_click, MUIM_Notify, MUIA_Pressed, FALSE, (Object*) WookieChat->App, 2,
                    MUIM_Application_ReturnID, buttons[count2].num);

        }

        if (entries == 0)
        {
            //printf("no userlist buttons config to load!\n");
            buttons[0].BT_click = SimpleButton("(empty)");
            //buttons[1].BT_click=SimpleButton("(empty)");

            DoMethod((Object*) WookieChat->GR_click_user_list_buttons, OM_ADDMEMBER, (Object*) buttons[0].BT_click);
            //DoMethod((Object*)WookieChat->GR_click_user_list_buttons, OM_ADDMEMBER, (Object*)buttons[1].BT_click);
            //DoMethod((Object*)buttons[count2].BT_click,MUIM_Notify,MUIA_Pressed,FALSE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,buttons[count2].num);

        }

        //DoMethod((Object*)WookieChat->GR_click_user_list_buttons,MUIM_Group_ExitChange);

    }

    return 0;

}

int get_user_list_buttons_config(void)
{

    return 0;

}

int save_user_list_buttons_config(void)
{

    BPTR save_file;
    if (DEBUG)
        printf("saving user list buttins config..\n");

    char *work_buffer;
    char work_buffer2[1000];

    getmacro((Object*) WookieChat->LV_user_list_buttons, MUIA_NList_Entries, &entries);

    strcpy(file_name, USER_LIST_BUTTONS_FILE);
    save_file = Open((_s_cs)file_name, MODE_NEWFILE);

    if (save_file)
    {
        if (DEBUG)
            printf("save file has been opened..\n");

        for (unsigned int count = 0; count < entries && count < MAX_BUTTONS; count++)
        {
            if (DEBUG)
                printf("saving entry %d..\n", count);

            DoMethod((Object*) WookieChat->LV_user_list_buttons, MUIM_NList_GetEntry, count, &work_buffer);
            if (work_buffer)
            {
                strcpy(work_buffer2, work_buffer);
                string1 = strtok(work_buffer2, "\033");
                string2 = strtok(NULL, "");
                if (string2)
                {
                    sprintf(output_string, "%s\n%s\n", string1, string2);
                    FPuts(save_file, (l_in) output_string);

                }
                //sprintf(output_string,"%s\n",work_buffer);
                //FPuts(save_file,(l_in)output_string);

            }
            else
                break;
        }
        strcpy(output_string, "\n");
        FPuts(save_file, (l_in) output_string);
        if (DEBUG)
            printf("finished saving, closing save file..\n");

        Close(save_file);
        if (DEBUG)
            printf("finished saving, file closed!\n");

    }
    return 0;

}
