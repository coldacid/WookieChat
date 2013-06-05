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



struct Settings my_settings;










char server_name[100];
char port_number[10];
char server_password[50];
char auto_joins[100];
char auto_connect[10];
char nick_password[50];
char server_charset[30];

LONG use_global=1;


int local_charset;
char *local_charsets[45];
char *remote_charsets[45];


fd_set read_master;       // master file descriptor list
fd_set write_master;      // master file descriptor list
int fdmax=-1;        // maximum file descriptor number




struct dcc_chat *dcc_chat_root;
struct dcc_chat *dcc_chat_conductor;



struct dcc *dcc_root;
struct dcc *dcc_conductor;

struct dcc *dcc_send_root;
struct dcc *dcc_send_conductor;




struct channel_entry *centry;
ULONG custom_pen_colours[24];



fd_set read_fds;          // temp file descriptor list for select()
fd_set write_fds;         // temp file descriptor list for select()
int queued_messages_total;









struct MsgPort *app_process_replyport;
struct MsgPort *send_text_replyport;
struct codeset *cs;

BPTR urlgrabber_file;
char urlgrabber_str[2000];
struct codeset *charsets[45];

char listview_format[100];

struct MsgPort *arexx_quit_replyport;
