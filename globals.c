/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

#include "includes.h"

#include "intern.h"

struct XYMessage *my_message;
struct XYMessage *incoming_message;

struct ObjApp *WookieChat;
struct Catalog *catalog;
char wookie_folder[400]; //the pathname wookiechat is located in

struct status_window *work_status;
struct status_window *status_root;
struct status_window *status_conductor;
struct status_window *status_current;
struct status_window *status_work;
struct status_window *status_previous;
struct status_window *status_next;

struct query_window *current_query;
struct query_window *work_query;

struct list_entry *work_list_entry;
struct list_entry *work_list_entry2;
struct list_entry *work_entry;

struct Settings my_settings;
struct Settings temp_settings;


ULONG entries;

struct MUI_PenSpec *pendisplay_specs[25];

STRPTR popimage_background;
STRPTR popimage_nicklistbackground;
STRPTR popimage_tabsbackground;

LONG colour=8; //default colour for listview text

char group_name[100];
char server_name[100];
char port_number[10];
char server_password[50];
char auto_joins[100];
char auto_connect[10];
char nick_password[50];
char server_charset[30];
char server_nick[128];
char server_nick2[128];
char server_nick3[128];
char server_real_name[256];
char server_user_name[256];
LONG use_global=1;
char *nick2;
char *nick3;
char *nick;
char username[20];
char realname[100];
int local_charset;
int remote_charset;
char *local_charsets[45];
char *remote_charsets[45];
int total_charsets;

fd_set read_master;       // master file descriptor list
fd_set write_master;      // master file descriptor list
int fdmax=-1;        // maximum file descriptor number

struct hostent *he;

struct dcc_chat *dcc_chat_work;
struct dcc_chat *dcc_chat_root;
struct dcc_chat *dcc_chat_conductor;

struct dcc *dcc_prev;
struct dcc *dcc_next;
struct dcc *dcc_work;
struct dcc *dcc_root;
struct dcc *dcc_conductor;
struct dcc *dcc_send_work;
struct dcc *dcc_send_root;
struct dcc *dcc_send_conductor;

char sendstuff[1500];
struct dcc_entry *find;

struct channel_entry *centry;
ULONG custom_pen_colours[24];
ULONG visible;
ULONG first;

fd_set read_fds;          // temp file descriptor list for select()
fd_set write_fds;         // temp file descriptor list for select()
int queued_messages_total;
BOOL start_reconnect_delay_timer;

struct graphical_smilies_struct graphical_nicklist[3];
char graphical_smiley_themes[10][100];

char string_to_send[BUFFERSIZE];

struct ClockData *clockdata;
char activity[64];
char activity_chat[64];
char activity_highlight[64];
STRPTR text3;
char *channel_display;
char *NewPreParse_NewText;
char server[50];
struct MsgPort *app_process_replyport;
struct MsgPort *send_text_replyport;
struct codeset *cs;
struct SharedList *slist;
BPTR urlgrabber_file;
char urlgrabber_str[2000];
struct codeset *charsets[45];

struct history *work_history;

char listview_format[100];

struct list_entry work_entry4;

char *tabwork2_string; //text before cursor position
char *tabwork3_string; //text after cursor position
char *tabwork4_string; //nick text to complete
char *tabwork5_string; //completed nick text to insert
char *tabwork_string; //original string gadget contents
struct ClockData *clockdata2;
BOOL is_chooser_window_open;
struct DiskObject *dobj; //for the iconified icon
struct MsgPort *arexx_quit_replyport;
