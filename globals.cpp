/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

#include "intern.h"

struct XYMessage *my_message;
struct XYMessage *incoming_message;

struct ObjApp *WookieChat;
struct Catalog *catalog;
BOOL USE_AREXX;
char wookie_folder[400]; //the pathname wookiechat is located in
char work_buffer[900];
char *string1;

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
struct list_entry *work_entry = new list_entry;

struct Settings my_settings;
struct Settings temp_settings;

char buffer2[BUFFERSIZE*2];
char buffer3[BUFFERSIZE*2];

ULONG entries;
BOOL QUIET_DCC;

struct timeval *systime;
// "reconnection on abnormal disconnection" delay timer
struct timeval *systime_reconnect_timer;

struct MUI_PenSpec *pendisplay_specs[25];

STRPTR popimage_background;
STRPTR popimage_nicklistbackground;
STRPTR popimage_tabsbackground;

char background2[64];
char file_name[800];
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

char timestamp[12];

fd_set read_master;       // master file descriptor list
fd_set write_master;      // master file descriptor list
int fdmax=-1;        // maximum file descriptor number

BOOL ZUNE_SYSTEM;
BOOL Pro_Charsets_Enabled=FALSE;

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

ULONG days;
ULONG hrs;
ULONG secs;
ULONG mins;
char sendstuff[1500];
struct dcc_entry *find=new dcc_entry;

int a;
int how_many_lines=15;
char buffer_text[800];
struct channel_entry *centry=new channel_entry;
char new_array[20];
ULONG custom_pen_colours[24];
ULONG visible;
ULONG first;
