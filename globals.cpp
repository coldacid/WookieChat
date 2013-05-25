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

ULONG entries;
BOOL QUIET_DCC;
