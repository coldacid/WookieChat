/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

#include <exec/types.h>
#include <exec/ports.h>
#include <dos/dos.h>
#include <intuition/intuition.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <netdb.h>

#define MUI_OBSOLETE
#include <libraries/mui.h>

#include <libraries/gadtools.h>


#define MAX_QUEUED_MESSAGES         500
#define BUFFERSIZE                  30000
#define MAX_EVENTS                  16
#define DCC_RECV_BUFFERSIZE         10000

/*
 * ub   - UBYTE *
 * s    - STRPTR
 * cs   - CONST_STRTR
 * _3.1_AROSMOS4.x
 */

#ifdef __amigaos4__
typedef char *          c_in;
typedef char *          i_in;
typedef CONST_STRPTR    l_in;
typedef CONST_STRPTR    loc_in;
typedef CONST_STRPTR    _ub_cs;
typedef CONST_STRPTR    _s_cs;
#elif __MORPHOS__
typedef char *          c_in;
typedef UBYTE *         i_in;
typedef CONST_STRPTR    l_in;
typedef STRPTR          loc_in;
typedef CONST_STRPTR    _ub_cs;
typedef CONST_STRPTR    _s_cs;
#elif __AROS__
typedef char *          c_in;
typedef unsigned int *  i_in;
typedef CONST_STRPTR    l_in;
typedef CONST_STRPTR    loc_in;
typedef CONST_STRPTR    _ub_cs;
typedef CONST_STRPTR    _s_cs;
#else
typedef UBYTE *         c_in;
typedef unsigned char * i_in;
typedef STRPTR          l_in;
typedef STRPTR          loc_in;
typedef UBYTE *         _ub_cs;
typedef STRPTR          _s_cs;
#endif

struct XYMessage
{
    struct Message xy_Msg;
    int xy_Getline;
    int xy_QuitArexx;
    int xy_Sendtext;
};

struct ObjApp
{
    APTR    GROUP_ROOT_0, GROUP_ROOT_1, GR_top;

    APTR WI_change_nick;
    APTR LA_change_nick;
    APTR STR_change_nick;
    APTR BT_change_nick_change;
    APTR BT_change_nick_cancel;

    APTR GR_server_specific_user_prefs;
    APTR GR_click_user_list_buttons;
    APTR GR_user_list_buttons;
    APTR LV_user_list_buttons;
    APTR LV_user_list_buttons2;
    APTR STR_user_list_buttons_name;
    APTR STR_user_list_buttons_command;
    APTR LA_user_list_buttons_help;

    APTR LA_user_list_buttons_disable;
    APTR CH_user_list_buttons_disable;

    APTR LA_hide_channel_mode_gadgets;
    APTR CH_hide_channel_mode_gadgets;

    APTR LA_hide_joins_parts;
    APTR CH_hide_joins_parts;

    APTR BT_user_list_buttons_add;
    APTR BT_user_list_buttons_delete;
    APTR BT_user_list_buttons_move_up;
    APTR BT_user_list_buttons_move_down;

    //APTR LA_use_utf8;
    APTR LA_local_charset;
    APTR LA_remote_charset;

    APTR    App;
    APTR    WI_main;

    APTR    LV_tabs;

    APTR    WI_graphical_smileys_choose;
    APTR    GR_graphical_smileys_choose;

    APTR    smiley_choose_icon;

    APTR    WI_graphical_smileys_preview;
    APTR    LV_graphical_smileys_preview;
    APTR    BT_graphical_smileys_preview;
    //quit requester
    APTR    WI_quit;
    APTR    BT_quit_yes;
    APTR    BT_quit_no;

    //about window objects
    APTR    WI_about;
    APTR    TX_about;
    APTR    TX_about2;
    APTR    TX_about3;
    APTR    TX_about_old;
    APTR    wookiechat_img_obj;
    APTR    guru_meditation_img_obj;
    APTR    SETTINGS_ROOT;
    
    //This window pops up if you try to recieve a file that already exists
    APTR    WI_dcc_file_exists;
    APTR    TX_oldname;
    APTR    TX_newname;
    APTR    TX_oldsize;
    APTR    TX_newsize;
    APTR    TX_olddate;
    APTR    BT_dcc_resume;
    APTR    BT_dcc_overwrite;
    APTR    BT_dcc_rename;
    APTR    BT_dcc_abort;

    //DCC WINDOW BUTTONS AND OBJECTS
    APTR    WI_dcc;
    APTR    WI_dcc2;
    APTR    LV_dcc;
    APTR    BT_dcc_accept;
    APTR    BT_dcc_cancel;
    APTR    BT_dcc_remove;
    APTR    STR_dcc_PA_label_0;
    APTR    PA_dcc_label_0;
    APTR    DCC_Register_Grp;
    APTR    LV_send_dcc;
    APTR    BT_dcc_send_new;
    APTR    BT_dcc_send_reoffer;
    APTR    BT_dcc_send_cancel;
    APTR    BT_dcc_send_remove;
    APTR    LA_download_path;
    APTR    MN_;
    APTR    LA_upload_path;
    APTR    STR_dcc_PA_label_1;
    APTR    PA_dcc_label_1;
    APTR    LA_download_path2;
    APTR    TX_download_path;


    APTR    STR_usertext;
    APTR    TX_nickname;

    //server selection window
    APTR    WI_label_2;
    APTR    STR_nickname1;
    APTR    STR_nickname2;
    APTR    STR_nickname3;
    APTR    STR_realname;
    APTR    STR_username;
    
    APTR    STR_server_nickname1;
    APTR    STR_server_nickname2;
    APTR    STR_server_nickname3;
    APTR    STR_server_realname;
    APTR    STR_server_username;

    APTR    BT_addserver;
    APTR    BT_addgroup;
    APTR    BT_delete_server;
    APTR    BT_edit;
    APTR    BT_connect;
    APTR    BT_connect_tab;
    APTR    BT_perform;
    APTR    NLT_Servers;
    APTR    NLV_Servers;
    APTR    CH_display_server_win;
    APTR    LA_display_server_win;
    APTR CH_use_global;


   //edit window stuff follows
    APTR    WI_edit_server;
    APTR    STR_servername;
    APTR    STR_port;
    APTR    STR_pass;
    APTR    STR_autojoin;
    APTR    STR_nick_registration_command;
    APTR    LA_autojoin;
    APTR    LA_nick_registration_command;
    APTR    BT_acceptserver;
    APTR    BT_cancelserver;
    APTR    WI_edit_group;
    APTR    STR_groupname;
    APTR    BT_acceptgroup;
    APTR    BT_cancelgroup;

    APTR    GR_charsets;
    APTR    CYCLE_local_charset;
    APTR    CYCLE_remote_charset;
    //APTR      CH_use_utf8;

    APTR    CH_charset;

    APTR    CH_autoconnect;
    APTR    LA_autoconnect;

   //colour settings window prefs
    APTR    WI_colour_settings;
    APTR    PP_CSW_background;
    APTR    PP_CSW_nicklistbackground;
    APTR    PP_CSW_join;
    APTR    PP_CSW_part;
    APTR    PP_CSW_quit;
    APTR    PP_CSW_modes;
    APTR    PP_CSW_ctcp;
    APTR    PP_CSW_actions;
    APTR    PP_CSW_normal;
    APTR    PP_CSW_info;
    APTR    PP_CSW_owntext;
    APTR    PP_CSW_highlight;
    APTR    PP_CSW_notices;
    APTR    PP_CSW_invite; //13
    APTR    PP_CSW_kick;    //14
    APTR    PP_CSW_nickchange;  //15
    APTR    PP_CSW_topic;     //16
    APTR    PP_CSW_wallops;    //17
    APTR    PP_CSW_activity;     //18
    APTR    PP_CSW_chatactivity;  //19
    APTR    PP_CSW_highlightactivity; //20
    APTR    PP_CSW_nicklisttext; //21
    APTR    PP_CSW_listviewtabs_background; //22
    APTR    PP_CSW_listviewtabs_normaltext; //33

    APTR    BT_colour_settings_load;
    APTR    BT_colour_settings_apply;
    APTR    BT_colour_settings_save;
    APTR    BT_colour_settings_save_as;


   //main settings window
    APTR    SETTINGS_Register_Grp;
    APTR    WI_mainsettings;
    //APTR      CH_label_6;
    //APTR      CH_label_1;
    
    APTR     CH_autojoin_channels_when_kicked;
    APTR     CH_auto_open_query_tabs_when_msged;

    APTR    STR_label_3;
    APTR    CH_label_2;
    APTR    CH_label_3;
    APTR    STR_label_1;
    APTR    STR_label_2;
    APTR    STR_label_4;
    APTR    STR_label_5;
    APTR    CH_label_4;
    APTR    CH_label_5;
    APTR    STR_label_0;
    APTR    CH_label_0;
    APTR    PA_label_0;
    APTR    STR_PA_label_0;
    APTR    BT_label_0;
    APTR    BT_label_1;
    APTR    BT_label_2;
    APTR    BT_label_3;
    APTR    TX_highlight_label;
    APTR    STR_highlight_label;
    APTR    TX_how_many_lines_to_reload;
    APTR    STR_how_many_lines_to_reload_channel;
    APTR    STR_how_many_lines_to_reload_private;
    APTR    LA_texteditor;
    APTR    PA_browser_label;
    APTR    PA_browser;
    APTR    STR_browser;
    APTR    RA_nick_completion_style_Label;
    APTR    RA_nick_completion_style;

    APTR    LA_tabs;
    APTR    CYCLE_tabs;
    //APTR    CH_tabs;

    //gui settings tab
    APTR    LA_column_width_timestamp;
    APTR    NB_column_width_timestamp;
    APTR    LA_column_width_nick;
    APTR    NB_column_width_nick;
    APTR    LA_column_spacing;
    APTR    NB_column_spacing;
    APTR    LA_graphical_smileys;
    APTR    CH_graphical_smileys;
    APTR    RA_graphical_smileys_background_colour_label;
    APTR    RA_graphical_smileys_background_colour;
    APTR    RA_graphical_smileys;
    APTR    graphical_smileys_set; //cycle gadget
    APTR    LA_graphical_nicklist;
    APTR    CH_graphical_nicklist;
    APTR    LA_listview_tabs_weight;
    APTR    NB_listview_tabs_weight;
    //logging register group
    APTR    LA_split_logs;
    APTR    CH_split_logs;
    APTR    LA_logsize;
    APTR    STR_logsize;
    APTR    LA_remove_brackets;
    APTR    CH_remove_brackets;
    APTR    LA_nicklist_horiz_weight;
    APTR    NB_nicklist_horiz_weight;

    APTR    LA_user_modes;
    APTR    CH_user_modes;


    //SERVER register group for the prefs window
    APTR    LA_reconnect_to_same_server;
    APTR    CH_reconnect_to_same_server;
    APTR    LA_Maximum_Retries;    //maximum retrys (0 to turn off)
    APTR    NB_Maximum_Retries;    //maximum retrys (0 to turn off)
    APTR    LA_Reconnect_Delay;   //reconnect delay (in seconds)
    APTR    NB_Reconnect_Delay;   //reconnect delay (in seconds)
    APTR    LA_no_server_tabs;
    APTR    CH_no_server_tabs;

    APTR    LA_clone_detection;
    APTR    CH_clone_detection;



    //dcc part of the settings window
    APTR    CH_autoaccept;
    APTR    LV_trusted_nicks;
    APTR    RA_autoaccept;
    APTR    CH_gethostid;
    APTR    CH_ipaddr_dcc;
    APTR    STR_ipaddr_dcc;
    APTR    STR_port1_dcc;
    APTR    STR_port2_dcc;

    //events part of settings window
    APTR    PA_sound_sample_player_label;
    APTR    PA_sound_sample_player;
    APTR    LA_sound_sample_player;
    APTR    STR_sound_sample_player;

    APTR    PA_sound_sample_newmsg_label;
    APTR    PA_sound_sample_newmsg;
    APTR    LA_sound_sample_newmsg;
    APTR    STR_sound_sample_newmsg;
    APTR    RA_sound_sample_newmsg;

    APTR    PA_sound_sample_tabopen_label;
    APTR    PA_sound_sample_tabopen;
    APTR    LA_sound_sample_tabopen;
    APTR    STR_sound_sample_tabopen;
    APTR    RA_sound_sample_tabopen;

    APTR    PA_sound_sample_highlight_label;
    APTR    PA_sound_sample_highlight;
    APTR    LA_sound_sample_highlight;
    APTR    STR_sound_sample_highlight;
    APTR    RA_sound_sample_highlight;
    APTR    LA_samples_path;
    APTR    STR_samples_path;
    APTR    PA_samples_path;

    APTR    LA_sound_replayer;
    APTR    CH_sound_replayer;
    APTR    STR_sound_replayer;
    APTR    PA_sound_replayer_label;
    APTR    PA_sound_replayer;

    //aliases register group in settings window
    APTR    LV_alias;
    APTR    BT_add_alias;
    APTR    BT_remove_alias;
    //APTR    BT_edit_alias;
    //APTR    STR_add_alias;
    APTR    STR_edit_alias_name;
    APTR    STR_edit_alias_command;
    
    //edit alias window
    /*APTR    WI_edit_alias;
    APTR    BT_edit_alias_accept;
    APTR    BT_edit_alias_cancel;
    APTR    STR_edit_alias;
      */

    //initiate dcc send window
    APTR    WI_dcc_send;
    APTR    PA_dcc_send_file;
    APTR    STR_PA_dcc_send_file;
    APTR    STR_dcc_send_nick;
    APTR    BT_dcc_send;

    //url grabber window
    APTR    WI_urlgrabber;
    APTR    LV_urlgrabber;
    APTR    BT_urlgrabber_clear;
    APTR    BT_urlgrabber_clearall;

    //ignore list window
    APTR    WI_ignore;
    APTR    LV_ignore;
    APTR    BT_ignore_remove;
    APTR    BT_ignore_add;
    APTR    BT_ignore_edit;
    APTR    WI_addignore;
    APTR    BT_addignore;
    APTR    STR_addignore;
    APTR    TX_addignore;
    APTR    LA_addignore_privmsg;
    APTR    LA_addignore_ctcp;
    APTR    LA_addignore_dcc;
    APTR    CH_addignore_privmsg;
    APTR    CH_addignore_ctcp;
    APTR    CH_addignore_dcc;
    APTR    LA_ignore_hostmask;
    APTR    LA_ignore_actions;

    APTR    WI_ban;
    APTR    LV_ban;
    APTR    BT_unban;
    APTR    BT_refreshban;
};

struct ident_struct
{
    long a_socket;
    long listen_socket;
    struct sockaddr_in test;
    struct sockaddr_in their_addr;
    char buffer[2000];
};

struct channel_entry
{
    char entry[900];
    char Display_entry[900];
    LONG colour;

    //experimental
    char display_hook_breakup[700];
    char display_hook_column1[70];
    char display_hook_column2[600];
    char display_hook_column3[650];
    char *display_string1;
    //end exeperimental

};

struct list_entry
{
    char modes[4];
    char name[32];
    char *hostname;
};

struct history
{
    char buffer_history[800];
    struct history *next;
    struct history *previous;
};

struct query_window
{
    char your_current_mode[15];
    BOOL jump_to_bottom;
    struct channel_entry nlist_tab_title;
    struct channel_entry nlist_tab;
    LONG   nlist_tab_number;
    LONG   nlist_tab_title_number;

    LONG   page_number;
    char nicklist_info[100];

    struct channel_entry waiting_to_be_displayed_buffer[50];
    int waiting_to_be_displayed_count;
    BPTR log_file;

    APTR    GR_tabs;
    APTR GR_conductor, GR_channel_modes, GR_listviews, GR_listviews_sub;
    APTR    GR_mode_gadgets_sub_group;

    APTR    BT_mode_X;
    APTR    STR_topic;
    APTR    BT_mode_T;
    APTR    BT_mode_N;
    APTR    BT_mode_S;
    APTR    BT_mode_I;
    APTR    BT_mode_P;
    APTR    BT_mode_M;
    APTR    BT_mode_B;
    APTR    BT_mode_K;
    APTR    STR_keyword;
    APTR    BT_mode_L;
    APTR    STR_limit;
    APTR    LV_channel;
    APTR    TX_nicklist;
    APTR    LV_nicklist;
    APTR    strip;
    APTR    strip2;
    APTR    GR_nicklist_and_tabs;

    BOOL attached;
    BOOL deleted;
    APTR    BT_querybutton;
    char name[60];
    char displayed_name[60];

    struct query_window *previous;
    char topic[800];
    struct query_window *next;
    int chanquery;
    struct history *string_root;
    struct history *string_conductor;
    int nicks;
    int nicks_ops;
    struct list_entry nicklist[2500];
    int mode_T;
    int mode_N;
    int mode_S;
    int mode_I;
    int mode_P;
    int mode_M;
    int mode_K;
    int mode_L;

    LONG entries_count;

    char keyword[500];
    char limit[100];
    char away_message[600];

    int activity;
    int removed;
    int number_of_lines_unread;
    APTR balance_object;
    int iv;
    int server_tab;
    char *queued_messages[MAX_QUEUED_MESSAGES+1];

    int queued_messages_total;
};

struct status_window
{
    int socket_number;
    int connection_active;
    APTR GR_server1_buttons;
    APTR my_hvspace;

    int iv;

    long a_socket;
    struct ident_struct ident;

    char *str;
    char *buffer;

    struct query_window *root, *conductor, *current_query, *previous_query;
    struct status_window *next;
    struct status_window *previous;
    char *nick;
    char pass[100];
    char groupname[100];
    char servername[100];
    char shortserver[50];
    char networkname[100];
    char last_msg_nick[40];
    char last_notice_nick[40];

    char nick_pass[100];
    char auto_joins[100];

    struct irc_user_modes_struct
    {
        char mode[5];
        char symbol[5];

    } user_modes[15];

    char nick_string[70];
    int max_modes;

    int filter_swear_words;
    int portnumber;

    BOOL quit_requested;
    BOOL trying_to_reconnect;

    int retry_number;

    char last_incoming_line_unparsed[800];
    BOOL away;

    //int utf8;

    int remote_charset;

    char chantypes[15];
    int chantypes_total;

    struct hostent *he;
    int waiting_for_ping;

    struct sockaddr_in slapshot_in;

    char nick1[128];
    char nick2[128];
    char nick3[128];
    char real_name[256];
    char user_name[256];

    char initial_nick[128];

};

struct AREXX_Menu
{

    APTR MN_MenuItem;
    char MenuItem_String[300];
    char MenuItem_FullFilename[300];
    LONG return_id;
};

struct ignore_list
{
    char ignoremask[100];
    BOOL ignore_privmsg;
    BOOL ignore_dcc;
    BOOL ignore_ctcp;

};

struct event_settings
{
    BOOL enabled;
    BOOL when_inactive;
    char arexx_script[800];
    char arexx_script2[800];

};

struct events_settings_struct
{
    int use_when;
    char arexx_script[800];
    char arexx_script2[800];
};

struct Settings
{
    int paste_delay_seconds;
    int paste_delay_microseconds;
    int auto_rejoin_on_kick;
    int open_query_on_privmsg;
    LONG max_lines_for_buffers;
    int log_channels;
    int log_private;
    char default_kickmsg[500];
    char quit_msg[800];
    char part_msg[800];

    //when doubleclicking on a nick in the nick list do you want to..
    int request_whois_info;
    int open_private_tab;

    char time_stamp_string[20];
    int timestamp_config;
    char text_editor[250];
    char browser[250];

    char upload_path[300];
    char download_path[300];

    int dcc_recv_auto_accept;
    int dcc_recv_auto_accept_if_exists_action; // 1 == overwrite
                                               // 2 == resume
                                               // 3 == abort
                                               // 4 == ask
    int autogethostid;
    char ipaddr[100];
    int ipaddr_use;

    char start_port[100];
    char end_port[100];
    int current_port;

    int how_many_lines_to_reload_channel;
    int how_many_lines_to_reload_private;

    char highlight[800];

    char samples_path[800];
    char sound_sample_player[800];
    char sound_sample_tabopen[800];
    char sound_sample_newmsg[800];
    int play_sound_when_tabopen;  // 0 == never
                                  // 1 == always
                                  // 2 == only when window is inactive
    int play_sound_when_newmsg;   // 0 == never
                                  // 1 == only when private tab isnt selected
                                  // 2 == only when window is inactive

    char sound_sample_highlight[800];
    int play_sound_when_highlight; // 0 == never
                                   // 1 == always
                                   // 2 == only when window is inactive

    char command_alias[100][600];
    int number_of_command_aliases;
    int use_column_display;
    int nick_completion;

    int timestamp_column_width;
    int nick_column_width;
    int urlgrabber;

    //automatically "reconnect to server upon non requested disconnection" prefs
    int reconnect_to_same_server;
    int Maximum_Retries;
    int Reconnect_Delay;



    struct ignore_list ignorelist[100];
    int totalignores;

    int display_server_window_on_startup;

    int clone_detection;

    int max_log_size;
    int splitup_logfiles;
    int no_server_tabs;

    int use_nlistobject_for_tabs;

    int os3_about_window_gfx;

    int use_external_sound_replayer;
    char external_sound_replayer[800];
    int use_ident;
    int graphical_smilies;
    int graphical_smilies_background;
    int which_clipboard_style;
    int graphical_nicklist;
    int delta;

    int listview_tabs_weight;

    int remove_brackets;
    char default_colours_theme[800];

    char dcc_recv_format[200];

    char dcc_send_format[200];

    //setmacro((Object*)status_current->current_query->GR_nicklist_and_tabs,MUIA_Weight,my_settings.nicklist_weight);
    LONG nicklist_horizontal_weight;

    int sort_tabs_alphabetically;

    int show_joins_parts;
    int hide_user_list_buttons;
    int hide_channel_mode_buttons;
    int user_modes_beside_nicks;

    struct event_settings dcc_send_finished;
    struct event_settings dcc_recv_offered;
    struct event_settings dcc_recv_finished;

    struct events_settings_struct events[MAX_EVENTS];

};

struct dcc_chat
{
    struct query_window *conductor;
    char name[150];
    char networkname[100];
    char address[200];
    char port[200];
    char nick[150];
    char own_nick[150];
    char recv_buffer[BUFFERSIZE];
    char buffer[BUFFERSIZE];
    char *str;
    long dcc_socket;
    long dcc_listen_socket;
    struct dcc_chat *next;
    struct dcc_chat *previous;
    int connected;
    int removed;
    struct sockaddr_in test;
    struct sockaddr_in their_addr;
};

struct dcc_entry
{
    char status[50];
    char nick[90];
    char filename[200];
    char cps[20];
    char bytes_transferred[20];
    char filesize[20];
    char percentage[10];
    char timeleft[50];
    char port[20];
};

//dcc send/recieve transfer structure
struct dcc
{
    BOOL accepted;
    char networkname[100];
    LONG pos;
    char address[50];
    char port[20];
    char nick[150];
    char filename[800];
    char filepath[800];
    char full_filename_with_path[1000];
    char recv_buffer[DCC_RECV_BUFFERSIZE];
    char buffer[DCC_RECV_BUFFERSIZE];
    long dcc_socket;
    long dcc_listen_socket;
    struct dcc_entry *entry;
    #ifdef __AROS__
    IPTR total_recv;
    IPTR start_recv; //not used yet
    IPTR filesize;
    #else
    ULONG total_recv;
    ULONG start_recv; //not used yet
    ULONG filesize;
    #endif

    struct dcc *next;
    struct dcc *previous;
    BPTR dcc_file;
    int completed;
    int connected;
    int removed;
    int cancelled;
    struct sockaddr_in test;
    struct sockaddr_in their_addr;
    char timestarted[1000];
};

enum
{
    HIGHLIGHT = 0,
    CONNECT,
    DISCONNECT,
    JOIN,
    PART,
    QUIT,
    DCC_RECV_OFFERED,
    DCC_RECV_FINISHED,
    DCC_SEND_FINISHED,
    CHANNEL_MESSAGE,
    PRIVATE_MESSAGE,
    NOTICE,
    CTCP_REQUEST,
    KICK,
    MODE_CHANGE,
    INVITE
};

enum
{
    COLUMNS = 1, NORMAL
};

enum
{
    AMIRC_STYLE = 0, MIRC_STYLE
};

#define AREXX_MENU_VALUES           500000
#define START_DELAY                 2
#define RECONNECT_STRAIGHT_AWAY     1
#define DONT_FLAG_AS_COMPLETED      0


#define ACTIVITY                    1
#define ACTIVITY_CHAT               2
#define ACTIVITY_HIGHLIGHT          3

#define FLAG_AS_COMPLETED           1

#define OVERWRITE                   0
#define RESUME                      1
#define ABORT                       2
#define ASK                         3
#define RENAME                      4

#define GRAPHICAL_SMILEY_VALUES     500900

#define HOSTNAME_STRING_SIZE        85

#define CONNECT_IN_CURRENT_TAB      1

#define SECOND_SET_OF_RETURNIDS     400000

#ifndef EAD_IS_FILE
#define EAD_IS_FILE(ead)            ((ead)->ed_Type <  0)
#endif

#define setmacro(obj,attr,value) SetAttrs(obj,attr,value,TAG_DONE)
#ifdef __AROS__
#define getmacro(obj,attr,store) GetAttr(attr,obj,(IPTR *)store)
#else
#define getmacro(obj,attr,store) GetAttr(attr,obj,(ULONG *)store)
#endif

#ifndef MUIC_DTPIC
#define MUIC_DTPIC "Dtpic.mui"
#endif

#ifndef MUIA_Dtpic_Name
#define MUIA_Dtpic_Name 0x80423d72
#endif

#ifndef __AROS__

#ifndef MUIA_Application_IconifyTitle
#define MUIA_Application_IconifyTitle 0x80422cb8 /* V18 isg STRPTR            */
#endif

#ifndef MUIA_Imagedisplay_Spec
#define MUIA_Imagedisplay_Spec 0x8042a547
#endif

#ifndef MUIA_Window_DisableKeys
#define MUIA_Window_DisableKeys 0x80424c36
#endif

#endif



/* globals.c */
extern struct ObjApp *WookieChat;
extern char wookie_folder[400]; //the pathname wookiechat is located in
extern struct status_window *work_status;
extern struct status_window *status_root;
extern struct status_window *status_conductor;
extern struct status_window *status_current;
extern struct query_window *current_query;
extern struct Settings my_settings;
extern char server_name[100];
extern char port_number[10];
extern char server_password[50];
extern char auto_joins[100];
extern char auto_connect[10];
extern char nick_password[50];
extern char server_charset[30];
extern LONG use_global;
extern int local_charset;
extern char *local_charsets[45];
extern char *remote_charsets[45];
extern fd_set read_master;
extern fd_set write_master;
extern int fdmax;
extern struct dcc_chat *dcc_chat_root;
extern struct dcc_chat *dcc_chat_conductor;
extern struct dcc *dcc_root;
extern struct dcc *dcc_conductor;
extern struct dcc *dcc_send_root;
extern struct dcc *dcc_send_conductor;
extern struct channel_entry *centry;
extern ULONG custom_pen_colours[24];
extern fd_set read_fds;
extern fd_set write_fds;
extern int queued_messages_total;
extern struct codeset *cs;
extern BPTR urlgrabber_file;
extern char urlgrabber_str[2000];
extern struct codeset *charsets[45];
extern char listview_format[100];
extern struct MsgPort *send_text_replyport;

/* arexx_hooks.c */
extern char string_to_send2[800];
extern ULONG arexx_wants_to_send_signal;

int arexx_add_scripts_to_menu();
void arexx_setup_messageports();
void arexx_wait_for_getline_to_be_false();
void arexx_deinit_messageports();
void arexx_create_process();
char * arexx_get_menu_item_string(int pos);

/* main.c */
extern BOOL DEBUG;
extern BOOL SMALLTABS;
extern BOOL RAW;
extern BOOL GEIT;
extern BOOL GEIT2;
extern BOOL GEIT3;
extern BOOL NEWDEBUG;
extern BOOL RECENTLY_CREATED_A_TAB;
extern BOOL ZUNE_SYSTEM;
extern BOOL QUIET_DCC;
extern BOOL USE_AREXX;
extern BOOL USING_A_PROXY;
extern BOOL PRO_CHARSETS_ENABLED;

extern BOOL start_reconnect_delay_timer;

void send_text(char*);
void send_current(char*);
void set_channel_clipboard_hook();

/* process_outgoing.c */
void process_outgoing(char*, int);

/* coloursettings.c */
extern struct MUI_PenSpec *pendisplay_specs[25];
extern STRPTR popimage_background;
extern STRPTR popimage_nicklistbackground;
extern STRPTR popimage_tabsbackground;
void save_colours();
void get_colours();
void set_colours();
void load_colours(char*);

/* subclasses.c */
struct InstanceData
{
    struct MUI_EventHandlerNode ehnode; /* input event handler*/
};

struct MyData
{
    struct MUI_PenSpec penspec;
    LONG pen;
    BOOL penchange;
    int requestchange;
};

extern struct Hook ConstructDCC_TextHook;
extern struct Hook DestructDCC_TextHook;
extern struct Hook DisplayDCC_recv_TextHook;
extern struct Hook DisplayDCC_send_TextHook;
extern LONG position;
void setup_background_colours();
ULONG BetterString_Dispatcher(Class *cl, Object *obj, Msg msg);
ULONG NList_Dispatcher(Class *cl, Object *obj, Msg msg);
ULONG Window_Dispatcher(Class *cl, Object *obj, Msg msg);
ULONG Group_Dispatcher(Class *cl, Object *obj, Msg msg);

/* connect2server.c */
#define SI_NICK_1       0
#define SI_NICK_2       1
#define SI_NICK_3       2
#define SI_REAL_NAME    3
#define SI_USER_NAME    4
#define SI_MAX          5
int connect2server(char *servername, char *port_number, int typedservercommand, int set_connect_script_name,
        int connect_in_current_tab, char *requested_remote_charset, int write_new_passwords_to_this_node);
void acquire_connect_details(char*);
void set_total_charsets(int val);
char * get_server_info(int id);

/* pincoming.c */
extern struct list_entry new_entry2;
int add_text_to_conductor_list(char*, LONG, int);
int add_text_to_current_list(char*, LONG, int);
void process_incoming();
void process_dcc_chat_incoming();
char *doubleclick_url_action(char*, int, int);
void set_activity(const char * act, const char * actchat, const char * acthightlight);
void pincoming_init();

/* dcc.c */
extern LONG recv_thing;
extern LONG send_thing;
extern LONG actualLength;
extern unsigned long dcc_addr;
void cleanup_dcc_recv();
void cleanup_dcc_send(int flag_as_completed);
void dcc_time_to_recv();
void dcc_send_data(int a);
void dcc_chat_connect();
void create_recv_dcc(char *nick, char *filename, char *address, unsigned short port, char *filesize);
void accept_dcc(char *b);
void create_send_dcc(char *nick, char *string3, int filesize, int portnumber);
void shutdown_my_dcc_recv_socket();
void create_recv_dcc_chat(char *, char *, char *);
int offer_dcc(char*,char*,char*,char*,int);
void initiate_outgoing_dcc_chat(char*);
void send_dcc_chat(char*);
void shutdown_my_dcc_send_socket();



/* tabs_create_close.c */
int create_new_tab(char *name, int show_now, int query_type);
void close_tab();
int create_new_status(int first);
int give_each_tab_a_listview_number_for_switching_tabs();

/* graphical_smilies.c */
#define MAXIMUM_SMILEYS         35
#define MAXIMUM_SMILEY_ASCII    30

struct smilies_struct
{
    char filename[100];
    char ascii[MAXIMUM_SMILEY_ASCII][16];
    char remove_ascii[16];
    int ascii_total;
    APTR icon;
    APTR choose_icon;
};

extern int total_smileys;
extern int preview_total_smileys;
extern struct smilies_struct smilies[MAXIMUM_SMILEYS];
extern struct smilies_struct preview_smilies[MAXIMUM_SMILEYS];
extern char graphical_smiley_themes[10][100];
void insert_graphical_smilies();
void delete_smiley_objects();
void delete_preview_smiley_objects();
void exit_delete_smiley_objects();
char * convert_graphical_smilies_2_text(char*);
int free_graphical_smilies(struct query_window*);
int use_graphical_smilies(struct query_window*);
int load_graphical_smilies();
int find_themes();

/* events_arexx.c */
extern char event_string[900];
extern char target_nick[60];
void create_arexx_event_string(char *arexx_event_to_use, char *arexx_arguments, char *buffer3);
BOOL is_window_active();

/* auto_reconnect_server.c */
ULONG get_reconnect_secs();
void automatically_reconnect_server(int);

/* hooks.c */
extern char display_hook_breakup2[900];
extern char *graphical_string1;
extern struct Hook ConstructLI_channel_TextHook;
extern struct Hook DestructLI_channel_TextHook;
extern struct Hook ConstructLI_channel2_TextHook;
extern struct Hook DestructLI_channel2_TextHook;
extern struct Hook DisplayLI_channel_TextHook;
extern struct Hook Custom_Clipboard2_Hook;
extern struct Hook DisplayLI2_channel_TextHook;
extern struct Hook ConstructLI_TextHook;
extern struct Hook DisplayLI_TextHook;
extern struct Hook DestructLI_TextHook;
extern struct Hook Display_listviewtabs_TextHook;
extern struct Hook Display_ignore_TextHook;
extern struct Hook previewdisplay_TextHook;
extern struct Hook Display_alias_TextHook;
extern struct Hook DisplayHook_LV_userlist_buttons;
extern struct Hook Display_events_TextHook;
extern struct Hook Display_servers_TextHook;
extern struct Hook DisplayLI_ban_TextHook;
extern struct Hook Display_channellist_TextHook;

/* nicklist.c */
void update_nicks_hostname(char *nick, char *hostname);
void AddNick(char *nick, char *hostname);
int RemoveNick(char *string1);
int ReplaceNicksWithVoiceOps(char *string1, char prefix, int option);
int ChangeNick(char *oldnick, char *newnick, char *buffer3);
void change_window_titlebar_text();
void DisplayNicks();
void nicklist_init();
void nicklist_deinit();

/* sortnicks.c */
int sort_nicks(int);
void sort_linked_list();

/* tabs_change.c */
int update_TX_nicklist();
int update_your_nickname_text_label();
void activate_tab_button();
void setup_notifys();
void check_column_size();
void set_column_size();
int switch_between_tabs_with_keys(int);
int switch_between_tabs_with_nlisttabclick(LONG result, BOOL saving_prefs);
void check_if_at_bottom();
void kill_my_notifys();
void remove_tab_listview();
void switch_between_tabs(int);
void add_tabs_to_nicklist_group();



/* highlight_search.c */
int search_for_highlight_words(char *, char *);

/* irc.c */
void ChangeMyNick(char *mynewnick);
void SWhoIs(char *selnick);
void SQuery(char *selnick);
void SJoin(char *selchannel);
void SMsg(char *selnick, char *message);
void SNames(char *selchannel);
void SNotice(char *selnick, char *massage);
void SPart(char *message);
void SQuit(char *message);
void SPing(char *message);
void SAction(char *message);
void SQuote(char *message);
void ChangeTopic(char *newtopic);


/* display_logfile_lines.c */
int display_last_few_lines_of_logfile_conductor();

/* subclasses.c */
extern BOOL user_is_trying_to_select_text;

/* save_settings.c */
#define MAX_BUTTONS             30
#define USERLIST_NAME_SIZE      100
#define USERLIST_COMMAND_SIZE   800
#define DEFAULT_CHARSET         "ISO-8859-15"


struct user_list_buttons_array
{
    int num;
    APTR BT_click;
    char name[USERLIST_NAME_SIZE];
    char command[USERLIST_COMMAND_SIZE];

};
extern struct user_list_buttons_array buttons[MAX_BUTTONS + 1];
extern char nick2[30];
extern char nick3[30];
extern char nick[30];
extern char username[20];
extern char realname[100];

void set_settings();
void load_settings();
int load_user_list_buttons_config();
int create_user_list_buttons();
int load_events_settings();
void save_settings();
int save_events_settings();
void get_events_settings();
void load_ignore_list();
void save_ignore_list();
void save_nick_settings();
void save_colours_choice();
void load_colours_choice();
void retrieve_settings();
void load_nick_settings();


/* startexit.c */
extern struct DiskObject *dobj;
extern struct NewMenu MenuData1[];
void cleanexit(char*);
void LoadAllLibs();

/* timers.c */
extern ULONG timer_signal;
extern char timestamp[12];
extern char timestamp_hrs[4];
extern char timestamp_mins[4];
void timestamp_2_string();
extern char pingtimestamp_mins[4];
extern char pingtimestamp_secs[4];
char * ping_time();
char * dcc_time();
struct timeval get_sys_time(struct timeval *tv);
BOOL TimerWait_Close();
BOOL TimerWait_Open();
void init_2s_delay();
struct Message * getmsg_2s_delay();
void init_paste_pause_delay();
struct Message * getmsg_paste_pause_delay();
void init_midnight_wait();
struct Message * getmsg_midnight_wait();

/* do_waitselect_code.c */
int do_waitselect_code();

/* closeserverselectwin.c */
void close_server_select_window();

/* locale.c */
CONST_STRPTR GCS(ULONG stringNum, char * defaultString);
void locale_opencatalog(char * language);
void locale_openbuiltincatalog();
void locale_closecatalog();
