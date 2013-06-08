/*
    The contents of this file are subject to the AROS Public License Version 1.1 
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis, 
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
    for the specific language governing rights and limitations under the License.
*/

/* ===================================================================
                        Process Incoming
   =================================================================== */
#include "includes.h"

#include <clib/alib_protos.h>
#include <proto/codesets.h>

#include "intern.h"
#include "version.h"
#include "objapp.h"
#include "audio.h"
#include "locale.h"

struct list_entry new_entry2;

/* Locals */

struct SharedList
{
    struct SignalSemaphore sl_Semaphore;
    struct List sl_List;
};

#define MAX_CLONE_FIELDS    25
#define WORK_BUFFER_SIZE    800
#define STRING_BUFFERSIZE   800
static char *pch3;
static char work_incoming[STRING_BUFFERSIZE * 2];
static int is_bold;
static char clone_results[MAX_CLONE_FIELDS][900];
static char rawservername[100];
static int count_clones;
static int nick_length;
static char urlvisit_str[1][2000];
static char number_of_lines_unread[20];
static char string11[900];
static char *string4;
static char *string5;
static char string8[900];
static char string9[900];
static char *string2;
static char *string3;
static char string10[900];
static char string7[900];
static char *string1;
static char work_buffer[900];
static char buffer2[BUFFERSIZE*2];
static char buffer3[BUFFERSIZE*2];
static char file_name[800];
static struct SharedList slist;
static char NewPreParse_NewText[100];
static char channel_display[100];
static char activity[64];
static char activity_chat[64];
static char activity_highlight[64];
static char sendstuff[1500];

#ifdef __AROS__
struct TagItem my_incoming_charset3_taglist[] =
{
    { CSA_Source, (IPTR) buffer2 },
    { CSA_CodesetFamily, CSV_CodesetFamily_Latin },
    { CSA_FallbackToDefault, TRUE },
    { TAG_DONE, 0 }
};
#else
struct TagItem my_incoming_charset3_taglist[] =
{
    {   CSA_Source, (ULONG)buffer2},
    {   CSA_CodesetFamily, CSV_CodesetFamily_Latin},
    {   CSA_FallbackToDefault, TRUE},
    {   TAG_DONE,0}
};
#endif

char *work;
char mode_work[1000];

char orig_time_mins[3];
char orig_time_secs[3];

enum
{
    PRIVMSG = 1, CTCP, DCC
};

BOOL found_clones;
BOOL play_tab_open_sample = FALSE;

#define MAXIMUM_UNREAD_LINES_VALUE 999

void add_unread_lines_num(void)
{

    if (status_conductor->conductor->number_of_lines_unread < MAXIMUM_UNREAD_LINES_VALUE)
    {
        status_conductor->conductor->number_of_lines_unread++;
        //listview style tabs
        sprintf(number_of_lines_unread, " %d", status_conductor->conductor->number_of_lines_unread);
        strcat(status_conductor->conductor->nlist_tab.entry, number_of_lines_unread);
        //button style tabs
        sprintf(channel_display, "%s%s %d", NewPreParse_NewText, status_conductor->conductor->displayed_name,
                status_conductor->conductor->number_of_lines_unread);

    }
    else
    {
        //listview style tabs
        sprintf(number_of_lines_unread, " %d+", status_conductor->conductor->number_of_lines_unread);
        strcat(status_conductor->conductor->nlist_tab.entry, number_of_lines_unread);
        //button style tabs
        sprintf(channel_display, "%s%s %d+", NewPreParse_NewText, status_conductor->conductor->displayed_name,
                status_conductor->conductor->number_of_lines_unread);

        //do not let this value get too high
        status_conductor->conductor->number_of_lines_unread = MAXIMUM_UNREAD_LINES_VALUE;

    }

}

void onjoin_clone_detection(char *nick, char *hostname)
{
    found_clones = FALSE;

    if (nick && hostname)
    {

        strcpy(string10, hostname);
        strtok(string10, "@");
        string1 = strtok(NULL, "");

        if (!string1)
            return;

		sprintf(buffer3, "%s%s%s%s %s %s: %s ", timestamp,
				LGS( MSG_OPENING_BRACKET ),
				LGS( MSG_CLONES ),
				LGS( MSG_CLOSING_BRACKET ),
				LGS( MSG_CLONE_DETECTED ),
				string1, nick );

        for (int count = 0; count < status_conductor->conductor->nicks; count++)
        {

            if (status_conductor->conductor->nicklist[count].hostname)
            {
                strcpy(string11, status_conductor->conductor->nicklist[count].hostname);
                strtok(string11, "@");
                string2 = strtok(NULL, "");

                if (string1 && string2)
                {
                    //printf("hostname %d: %s comparing to %s\n",count,string2, string1);
                    if (!strcmp(string2, string1))
                    {

                        strcat(buffer3, status_conductor->conductor->nicklist[count].name);
                        strcat(buffer3, " ");
                        found_clones = TRUE;

                    }
                }
            }

        }

        if (found_clones == TRUE)
            add_text_to_conductor_list((char*) buffer3, 9, ACTIVITY);

    }

}

int wildcmp(const char *wild, const char *string)
{
    const char *cp = NULL, *mp = NULL;

    while ((*string) && (*wild != '*'))
    {
        if ((*wild != *string) && (*wild != '?'))
        {
            return 0;
        }
        wild++;
        string++;
    }

    while (*string)
    {
        if (*wild == '*')
        {
            if (!*++wild)
            {
                return 1;
            }
            mp = wild;
            cp = string + 1;
        }
        else if ((*wild == *string) || (*wild == '?'))
        {
            wild++;
            string++;
        }
        else
        {
            wild = mp;
            string = cp++;
        }
    }

    while (*wild == '*')
    {
        wild++;
    }
    return !*wild;
}

BOOL ignore_privmsg;
BOOL ignore_ctcp;
BOOL ignore_dcc;

BOOL user_being_ignored(char *nick, char *hostname, int type)
{
    //if(DEBUG) printf("searching for this ignore:%s\n",string10);
    //printf("searching for this ignore:%s\n",string10);
    //printf("searching for this ignore:%s!%s\n",nick,hostname);

    if (type == PRIVMSG)
        ignore_privmsg = FALSE;
    else if (type == CTCP)
        ignore_ctcp = FALSE;
    else if (type == DCC)
        ignore_dcc = FALSE;

    char string7[800];

    strcpy(string7, nick);
    for (unsigned count3 = 0; count3 < strlen(string7); count3++)
        string7[count3] = ToLower(string7[count3]);
    //for(unsigned count3=0; count3<strlen(nick); count3++) nick[count3]=ToLower(nick[count3]);
    for (unsigned count3 = 0; count3 < strlen(hostname); count3++)
        hostname[count3] = ToLower(hostname[count3]);

    sprintf(string11, "%s!%s", string7, hostname);
    //if(DEBUG) printf("searching for this ignore:%s\n",string11);

    for (int count = 0; count < my_settings.totalignores; count++)
    {
        //if(DEBUG) printf("ignoremask entry:%s\n",my_settings.ignorelist[count].ignoremask);
        if (wildcmp(my_settings.ignorelist[count].ignoremask, string11))
        {
            //printf("found hit\n");

            if (my_settings.ignorelist[count].ignore_privmsg == TRUE && type == PRIVMSG)
            {
                ignore_privmsg = TRUE;
                //if(DEBUG) printf("ignoring privmsgs, ");
                return TRUE;
            }

            if (my_settings.ignorelist[count].ignore_ctcp == TRUE && type == CTCP)
            {
                ignore_ctcp = TRUE;
                //if(DEBUG) printf("ignoring ctcp, ");
                return TRUE;
            }
            if (my_settings.ignorelist[count].ignore_dcc == TRUE && type == DCC)
            {
                ignore_dcc = TRUE;
                //if(DEBUG) printf("ignoring dcc\n");
                return TRUE;
            }

        }

    }

    return FALSE;

}

char * doubleclick_url_action(char *buffer1, int clicked_char_number, int column)
{

    //if(DEBUG) printf("doubleclick\n");

    //selectup_code();

    //when you doubleclick, it eats the event. the subclasses listen to "left click mouseup and mousedown"
    //when it doesnt recieve the "up click", it pauses indefinately. which is why we set the BOOL variable below
    user_is_trying_to_select_text = FALSE;

    if (clicked_char_number == -1)
        return NULL;

    unsigned int aa = 0;
    unsigned int bb = 0;
    int num_url_string = 0;

    if (DEBUG)
        printf("buffer1:%s\n", buffer1);
    if (DEBUG)
        printf("clicked_char_number:%d\n", clicked_char_number);
    if (DEBUG)
        printf("bb:%d\n", bb);

    while (bb < strlen(buffer1))
    {
        for (aa = 0; bb < strlen(buffer1); aa++, bb++)
        {
            urlvisit_str[num_url_string][aa] = buffer1[bb];

            if (urlvisit_str[num_url_string][aa] == ' ' || urlvisit_str[num_url_string][aa] == ')'
                    || urlvisit_str[num_url_string][aa] == '\'')
            {
                urlvisit_str[num_url_string][aa] = '\0';
                bb++;

                break;
            }

            //if(urlvisit_str[num_url_string][aa]==':' && aa==0) aa--;
        }

        urlvisit_str[num_url_string][aa] = '\0';

        //if(DEBUG) printf("urlvisit_str:%s\n",urlvisit_str);
        //if(DEBUG) printf("column:%d\n",column);

        if (strncmp(urlvisit_str[num_url_string], "http://", 6) == 0
                || strncmp(urlvisit_str[num_url_string], "https://", 6) == 0
                || strncmp(urlvisit_str[num_url_string], "www", 2) == 0)
        {

            if (status_current->current_query->name[0] == '*' && column == 2)
            {
                //if(DEBUG) printf("trying url action 1\n");

                if ((bb == (ULONG) clicked_char_number || bb > (ULONG) clicked_char_number)
                        && strcspn(urlvisit_str[num_url_string], status_current->chantypes) != 0) // urlvisit_str[num_url_string][0] != '#')
                {
                    if (DEBUG)
                        printf("url action 1\n");
                    return urlvisit_str[num_url_string];
                }
            }
            else if (status_current->current_query->name[0] != '*')
            {
                //if(DEBUG) printf("trying url action 2\n");

                if ((bb + 8 == (ULONG) clicked_char_number || bb + 8 > (ULONG) clicked_char_number)
                        && strcspn(urlvisit_str[num_url_string], status_current->chantypes) != 0) //urlvisit_str[num_url_string][0] != '#')
                {
                    if (DEBUG)
                        printf("url action 2\n");
                    return urlvisit_str[num_url_string];
                }
            }

        }
        else if (strcspn(urlvisit_str[num_url_string], status_current->chantypes) == 0)
        {
            if (status_current->current_query->name[0] == '*' && column == 0)
            {
                //if(DEBUG) printf("trying join action 1\n");

                if (bb + 8 == (ULONG) clicked_char_number || bb + 8 > (ULONG) clicked_char_number)
                {
                    if (DEBUG)
                        printf("join action 1\n");

                    sprintf(buffer3, "JOIN %s\r\n", urlvisit_str[num_url_string]);
                    send_current(buffer3);
                    return NULL;
                }
            }
            else if (status_current->current_query->name[0] != '*')
            {
                //if(DEBUG) printf("trying join action 2\n");

                if (bb + 8 == (ULONG) clicked_char_number || bb + 8 > (ULONG) clicked_char_number)
                {
                    if (DEBUG)
                        printf("join action 2\n");

                    sprintf(buffer3, "JOIN %s\r\n", urlvisit_str[num_url_string]);
                    send_current(buffer3);
                    return NULL;
                }
            }

        }

    }
    //if(DEBUG) printf("end doubleclick\n");
    return NULL;

}

void search_for_url_then_add(char *buffer1)
{

    if (!my_settings.urlgrabber)
        return; //is the url grabber switched off?

    unsigned int aa = 0, bb = 0, aaa = 0;

    while (bb < strlen(buffer1))
    {
        ULONG entries;
        for (aa = 0; bb < strlen(buffer1); aa++, bb++)
        {
            urlgrabber_str[aa] = buffer1[bb];

            if (urlgrabber_str[aa] == ' ' || urlgrabber_str[aa] == ')' || urlgrabber_str[aa] == '\'')
            {
                urlgrabber_str[aa] = '\0';
                bb++;
                break;
            }
        }

        urlgrabber_str[aa] = '\0';

        getmacro((Object*) WookieChat->LV_urlgrabber, MUIA_NList_Entries, &entries);

        if (strncmp(urlgrabber_str, "http://", 6) == 0 || strncmp(urlgrabber_str, "https://", 6) == 0
                || strncmp(urlgrabber_str, "www", 2) == 0) // || strchr(urlgrabber_str,'@'))
        {

            for (aaa = 0; aaa < entries; aaa++)
            {
                DoMethod((Object*) WookieChat->LV_urlgrabber, MUIM_NList_GetEntry, aaa, &string1);

                if (string1)
                {
                    if (!strcmp(string1, urlgrabber_str))
                        break;
                }

            }

            if (aaa == entries)
            {
                DoMethod((Object*) WookieChat->LV_urlgrabber, MUIM_NList_InsertSingle, urlgrabber_str,
                        MUIV_NList_Insert_Top);

                //now save all url's to a text file
                sprintf(file_name, "progdir:urls.txt");
                urlgrabber_file = Open((_s_cs)file_name, MODE_NEWFILE);
                if (urlgrabber_file)
                {
                    getmacro((Object*) WookieChat->LV_urlgrabber, MUIA_NList_Entries, &entries);

                    for (aaa = 0; aaa < entries; aaa++)
                    {
                        DoMethod((Object*) WookieChat->LV_urlgrabber, MUIM_NList_GetEntry, aaa, &string1);

                        if (string1)
                        {
                            FPuts(urlgrabber_file, (l_in) string1);
                            FPutC(urlgrabber_file, '\n');

                            if (!strcmp(string1, urlgrabber_str))
                                break;
                        }
                    }
                    Close(urlgrabber_file);
                }

            }

        }

    }

}

int add_text_to_current_list(char *buffer1, LONG colour, int activitylevel)
{

    status_conductor->conductor = status_conductor->current_query;
    add_text_to_conductor_list(buffer1, colour, activitylevel);

    return 0;
}

int add_text_to_conductor_list(char *buffer1, LONG colour, int activitylevel)
{

    //struct channel_entry *centry=(struct channel_entry*) AllocMem(sizeof(struct channel_entry),MEMF_ANY);

    ObtainSemaphore(&slist.sl_Semaphore);

#ifdef __AROS__
    strcpy(centry->entry, "        :");
#else
    strcpy(centry->entry,"          ");
#endif

    centry->colour = colour;

    if (colour == 10)
    {
        struct status_window *status_work = status_conductor;
        sprintf(sendstuff, "away :\r\n");

        for (status_conductor = status_root; status_conductor; status_conductor = status_conductor->next)
        {
            if (status_conductor->away == TRUE && status_conductor->connection_active)
            {
                struct query_window *work_query = status_conductor->conductor;

                if (status_conductor == status_current)
                    status_conductor->conductor = status_conductor->current_query;
                else
                    status_conductor->conductor = status_conductor->root;

                send_text(sendstuff);
                status_conductor->away = FALSE;
                status_conductor->conductor = work_query;
            }
        }

        status_conductor = status_work;
    }

    if (colour == 10)
    {
        status_conductor->away = FALSE;
    }
    int a = 0;
    int b = 0;

    do
    {
        if (buffer1[a] == '\003')
        {
            a++;
            if ((buffer1[a] - '0' >= 0 && buffer1[a] - '0' <= 9) && (buffer1[a] - '0' != 1 && buffer1[a] - '0' != 0))
            {
                a++;
            }
            else if (buffer1[a] - '0' == 0)
            {
                a++;
                if (buffer1[a] - '0' >= 0 && buffer1[a] - '0' <= 9)
                    a++;
            }
            else if (buffer1[a] - '0' == 1)
            {
                a++;
                if (buffer1[a] - '0' >= 0 && buffer1[a] - '0' <= 5)
                    a++;
            }

            if (buffer1[a] == ',')
            {
                a++;

                if ((buffer1[a] - '0' >= 0 && buffer1[a] - '0' <= 9)
                        && (buffer1[a] - '0' != 1 && buffer1[a] - '0' != 0))
                {
                    a++;
                }
                else if (buffer1[a] - '0' == 0)
                {
                    a++;
                    if (buffer1[a] - '0' >= 0 && buffer1[a] - '0' <= 9)
                        a++;
                }
                else if (buffer1[a] - '0' == 1)
                {
                    a++;
                    if (buffer1[a] - '0' >= 0 && buffer1[a] - '0' <= 5)
                        a++;
                }
            }

        }
        else
        {
            buffer1[b] = buffer1[a];
            a++;
            b++;
        }

    } while (buffer1[a] != '\0');

    buffer1[b] = '\0';

    strcpy(work_incoming, "");

    a = 0, b = 0;
    is_bold = 0;

    while (buffer1[a])
    {
        if (buffer1[a] == '\002')
        {
            if (is_bold)
            {
                work_incoming[b++] = '\033';
                work_incoming[b++] = 'n';
                is_bold = 0;
            }
            else
            {
                work_incoming[b++] = '\033';
                work_incoming[b++] = 'b';
                is_bold = 1;
            }
        }
        else
        {
            work_incoming[b++] = buffer1[a];
        }
        a++;
    }

    work_incoming[b] = '\0';

    strcpy(buffer1, work_incoming);

    a = 0, b = 0;
    int is_underline = 0;

    //insert underline codes
    while (buffer1[a])
    {
        if (buffer1[a] == '\037')
        {
            if (is_underline)
            {
                work_incoming[b++] = '\033';
                work_incoming[b++] = 'n';
                is_underline = 0;
            }
            else
            {
                work_incoming[b++] = '\033';
                work_incoming[b++] = 'u';
                is_underline = 1;
            }
        }
        else
        {
            work_incoming[b++] = buffer1[a];
        }
        a++;
    }

    work_incoming[b] = '\0';

    strcat(centry->entry, work_incoming);

    if (!status_conductor)
    {
        ReleaseSemaphore(&slist.sl_Semaphore);
        return 0;
    }
    if (!status_conductor->GR_server1_buttons)
    {
        ReleaseSemaphore(&slist.sl_Semaphore);
        return 0;
    }
    if (!status_conductor->conductor)
    {
        ReleaseSemaphore(&slist.sl_Semaphore);
        return 0;
    }
    if (!status_conductor->conductor->LV_channel)
    {
        ReleaseSemaphore(&slist.sl_Semaphore);
        return 0;
    }

    if (!status_conductor)
    {
        return 0;
    }
    if (!status_conductor->GR_server1_buttons)
    {
        return 0;
    }
    if (!status_conductor->conductor)
    {
        return 0;
    }
    if (!status_conductor->conductor->LV_channel)
    {
        return 0;
    }

    insert_graphical_smilies();

#ifdef __amigaos4__
    if(user_is_trying_to_select_text==TRUE)
    {
        if(status_conductor->conductor->waiting_to_be_displayed_count>=49)
        {
            work_status=status_conductor;
            struct query_window *work_query=status_conductor->conductor;
            selectup_code();
            status_conductor=work_status;
            status_conductor->conductor=work_query;
        }
        else
        {
            strcpy(status_conductor->conductor->waiting_to_be_displayed_buffer[status_conductor->conductor->waiting_to_be_displayed_count].entry,centry->entry);
            status_conductor->conductor->waiting_to_be_displayed_buffer[status_conductor->conductor->waiting_to_be_displayed_count].colour=centry->colour;
            status_conductor->conductor->waiting_to_be_displayed_count++;
        }
    }
    else
#endif
    {
        ULONG visible;

        search_for_url_then_add(work_incoming);

        getmacro((Object*) status_conductor->conductor->LV_channel, MUIA_NList_Visible, &visible);

        if (status_conductor->conductor == status_current->current_query)
        {

            if (visible > 0)
            {
                ULONG first;
                ULONG entries;
                getmacro((Object*) status_conductor->conductor->LV_channel, MUIA_NList_Entries, &entries);
                getmacro((Object*) status_conductor->conductor->LV_channel, MUIA_NList_First, &first);
                DoMethod((Object*) status_conductor->conductor->LV_channel, MUIM_NList_InsertSingleWrap, centry,
                        MUIV_NList_Insert_Bottom, WRAPCOL0, ALIGN_LEFT);
                if ((first + visible) >= (entries - 1))
                    DoMethod((Object*) status_conductor->conductor->LV_channel, MUIM_NList_Jump,
                            MUIV_NList_Jump_Bottom);

            }
            else
            {
                DoMethod((Object*) status_conductor->conductor->LV_channel, MUIM_NList_InsertSingleWrap, centry,
                        MUIV_NList_Insert_Bottom, WRAPCOL0, ALIGN_LEFT);
                DoMethod((Object*) status_conductor->conductor->LV_channel, MUIM_NList_Jump, MUIV_NList_Jump_Bottom);

            }
        }
        else
        {
            DoMethod((Object*) status_conductor->conductor->LV_channel, MUIM_NList_InsertSingleWrap, centry,
                    MUIV_NList_Insert_Bottom, WRAPCOL0, ALIGN_LEFT);

        }

#ifndef __AROS__
#if 0
        if(activitylevel == 2 || activitylevel == 3)
        {

            if(iconified_and_new_text==0)
            {
				setmacro((Object*)WookieChat->App,MUIA_Application_IconifyTitle, LGS(MSG_ICONIFY_TEXT_NEW_MESSAGES_WAITING));
                iconified_and_new_text=1;

            }

        }
#endif
#endif

        if (status_conductor->conductor != current_query)
        {

            if (activitylevel == 2 || activitylevel == 3)
            {

                if (activitylevel == 2 && activitylevel >= status_conductor->conductor->activity)
                {
                    sprintf(NewPreParse_NewText, "\033c\033I[%s] ", activity_chat);
                    status_conductor->conductor->activity = 2;
                    status_conductor->conductor->nlist_tab.colour = 19;

                }
                else if (activitylevel == 3 && activitylevel >= status_conductor->conductor->activity)
                {

                    sprintf(NewPreParse_NewText, "\033c\033I[%s] ", activity_highlight);
                    status_conductor->conductor->activity = 3;
                    status_conductor->conductor->nlist_tab.colour = 20;

                }

                if (ZUNE_SYSTEM == TRUE)
                    strcpy(status_conductor->conductor->nlist_tab.entry, "         ");
                else
                    strcpy(status_conductor->conductor->nlist_tab.entry, "         :");
                strcat(status_conductor->conductor->nlist_tab.entry, status_conductor->conductor->displayed_name);

                add_unread_lines_num();

#ifndef __AROS__
#if 0
                if(iconified_and_new_text==0)
                {
                    setmacro((Object*)WookieChat->App,MUIA_Application_IconifyTitle,"New Messages Waiting");
                    iconified_and_new_text=1;
                }
#endif
#endif

                setmacro(status_conductor->conductor->BT_querybutton, MUIA_Text_Contents, channel_display);

                DoMethod((Object*) WookieChat->LV_tabs, MUIM_NList_ReplaceSingle,
                        &status_conductor->conductor->nlist_tab, status_conductor->conductor->nlist_tab_number, 0, 0);

            }
            else if (activitylevel == 1 && activitylevel > status_conductor->conductor->activity)
            {
                //the button
                sprintf(NewPreParse_NewText, "\033c\033I[%s] ", activity);
                status_conductor->conductor->activity = 1;
                sprintf(channel_display, "%s%s", NewPreParse_NewText, status_conductor->conductor->displayed_name);
                setmacro(status_conductor->conductor->BT_querybutton, MUIA_Text_Contents, channel_display);

                //the nlist entry
                if (ZUNE_SYSTEM == TRUE)
                    strcpy(status_conductor->conductor->nlist_tab.entry, "         ");
                else
                    strcpy(status_conductor->conductor->nlist_tab.entry, "         :");
                status_conductor->conductor->nlist_tab.colour = 18;
                strcat(status_conductor->conductor->nlist_tab.entry, status_conductor->conductor->displayed_name);

                DoMethod((Object*) WookieChat->LV_tabs, MUIM_NList_ReplaceSingle,
                        &status_conductor->conductor->nlist_tab, status_conductor->conductor->nlist_tab_number, 0, 0);

            }

        }

    }

    if (!status_conductor->conductor->log_file
            && ((strcspn(status_conductor->conductor->name, status_conductor->chantypes) == 0
                    && my_settings.log_channels) || my_settings.log_private))
    {
        if (wookie_folder[strlen(wookie_folder) - 1] == ':' || wookie_folder[strlen(wookie_folder) - 1] == '/')
            sprintf(file_name, "%slogs/%s_%s", wookie_folder, status_conductor->networkname,
                    status_conductor->conductor->name);
        else
            sprintf(file_name, "%s/logs/%s_%s", wookie_folder, status_conductor->networkname,
                    status_conductor->conductor->name);
        status_conductor->conductor->log_file = Open((_s_cs)file_name, MODE_READWRITE);
        if (status_conductor->conductor->log_file)
        {
            char buffer_text[800];
            struct timeval systime;
            struct ClockData clockdata;
            //get the date and current time for logging stamps and renaming
            get_sys_time(&systime); // Get current system time
#ifdef __amigaos4__
            Amiga2Date(systime.tv_sec, &clockdata);
#else
            Amiga2Date(systime.tv_secs, &clockdata);
#endif

            timestamp_2_string();

			sprintf(buffer_text, "* * %s %d-%d-%d, at %s:%s * *", LGS( MSG_LOGGING_STARTED_AT ),
                    clockdata.year, clockdata.month, clockdata.mday, timestamp_hrs, timestamp_mins);

            //if the filesize is over our max logfile size, lets rename it and create a new logfile!
            //but only do this if the user has configured wookie to do it
            if (my_settings.splitup_logfiles == 1)
            {

                struct FileInfoBlock a_fib;
                if (ExamineFH(status_conductor->conductor->log_file, &a_fib))
                {
                    if (a_fib.fib_Size > (ULONG) my_settings.max_log_size * 1024)
                    {
                        sprintf(string7, "%s_%d-%d-%d", file_name, clockdata.year, clockdata.month, clockdata.mday);
                        Rename((_s_cs)file_name, (_s_cs)string7);
                        Close(status_conductor->conductor->log_file);
                        status_conductor->conductor->log_file = Open((_s_cs)file_name, MODE_READWRITE);

                    }

                }
            }
            if (status_conductor->conductor->log_file)
            {
                //take us to the end of the file now
#ifdef __aamigaos4__
                ChangeFilePosition(status_conductor->conductor->log_file,0,OFFSET_END);
#else
                Seek(status_conductor->conductor->log_file, 0, OFFSET_END);
#endif

                //write the log header
                FPuts(status_conductor->conductor->log_file, (l_in) buffer_text);
                FPutC(status_conductor->conductor->log_file, '\n');
#ifdef __amigaos4__
                FFlush(status_conductor->conductor->log_file);
#endif

            }

        }

    }

    //write new text to the logfile
    if (status_conductor->conductor->log_file
            && ((strcspn(status_conductor->conductor->name, status_conductor->chantypes) == 0
                    && my_settings.log_channels)
                    || (strcspn(status_conductor->conductor->name, status_conductor->chantypes) != 0
                            && my_settings.log_private)))
    {
        FPuts(status_conductor->conductor->log_file, (l_in) work_incoming);
        FPutC(status_conductor->conductor->log_file, '\n');
#ifdef __amigaos4__
        FFlush(status_conductor->conductor->log_file);
#endif
    }

    ReleaseSemaphore(&slist.sl_Semaphore);

    return 0;

}

int add_text_to_channels_query(char *buffer1, LONG colour, int activitylevel)
{

    strcpy(centry->entry, "         :");

    centry->colour = colour;

    int a = 0;
    int b = 0;

    strcpy(work_incoming, "");

    a = 0, b = 0;
    is_bold = 0;

    strcat(centry->entry, buffer1);

    if (!status_conductor)
        return 0;
    if (!status_conductor->conductor)
        return 0;

    if (!status_conductor->conductor->LV_channel)
        return 0;

    DoMethod((Object*) status_conductor->conductor->LV_channel, MUIM_NList_InsertSingle, centry,
            MUIV_NList_Insert_Bottom);

    if (status_conductor->conductor != current_query)
    {
        if (activitylevel == 3 && activitylevel > status_conductor->conductor->activity)
        {
            sprintf(NewPreParse_NewText, "\033c\033I[%s] ", activity_highlight);
            status_conductor->conductor->activity = 3;
            if (status_conductor->conductor == status_conductor->root)
            {
                if (!stricmp(status_conductor->networkname, ""))
                    sprintf(channel_display, "%s%s", NewPreParse_NewText, status_conductor->root->name);
                else
                    sprintf(channel_display, "%s%s", NewPreParse_NewText, status_conductor->networkname);
            }
            else
                sprintf(channel_display, "%s%s", NewPreParse_NewText, status_conductor->conductor->displayed_name);

            setmacro(status_conductor->conductor->BT_querybutton, MUIA_Text_Contents, channel_display);
        }
        else if (activitylevel == 2 && activitylevel > status_conductor->conductor->activity)
        {
            sprintf(NewPreParse_NewText, "\033c\033I[%s] ", activity_chat);
            status_conductor->conductor->activity = 2;

            if (status_conductor->conductor == status_conductor->root)
            {
                if (!stricmp(status_conductor->networkname, ""))
                    sprintf(channel_display, "%s%s", NewPreParse_NewText, status_conductor->root->name);
                else
                    sprintf(channel_display, "%s%s", NewPreParse_NewText, status_conductor->networkname);
            }
            else
                sprintf(channel_display, "%s%s", NewPreParse_NewText, status_conductor->conductor->displayed_name);

            setmacro(status_conductor->conductor->BT_querybutton, MUIA_Text_Contents, channel_display);
        }
        else if (activitylevel == 1 && activitylevel > status_conductor->conductor->activity)
        {
            sprintf(NewPreParse_NewText, "\033c\033I[%s] ", activity);
            status_conductor->conductor->activity = 1;

            if (status_conductor->conductor == status_conductor->root)
            {
                if (!stricmp(status_conductor->networkname, ""))
                    sprintf(channel_display, "%s%s", NewPreParse_NewText, status_conductor->root->name);
                else
                    sprintf(channel_display, "%s%s", NewPreParse_NewText, status_conductor->networkname);
            }
            else
                sprintf(channel_display, "%s%s", NewPreParse_NewText, status_conductor->conductor->displayed_name);

            setmacro(status_conductor->conductor->BT_querybutton, MUIA_Text_Contents, channel_display);

        }

    }

    return 0;

}

char james[BUFFERSIZE];

//#ifndef __AROS__
UTF8 utf8_buffer[1600];

BOOL FindUTF8Chars(char *buffer2)
{
    memcpy(utf8_buffer, buffer2, strlen(buffer2));
    for (unsigned long count = 0; count < strlen(buffer2); count++)
    {
        //printf("%c-%d-",utf8_buffer[count],utf8_buffer[count]);
        if (utf8_buffer[count] >= 194 && utf8_buffer[count] < 223)
            return TRUE;
    }

    return FALSE;

}
//#endif

void process_incoming()
{
    int colour = 1;

    /* insert /r's where there are none */

    unsigned int a = 0, b = 0;

    while (1)
    {

        if (status_conductor->str[b] == '\r' && status_conductor->str[b + 1] == '\n')
        {
            james[a] = status_conductor->str[b];
            a++;
            b++;
            james[a] = status_conductor->str[b];
            a++;
            b++;

        }
        else if (status_conductor->str[b] == '\n') // && status_conductor->str[b+1] == '\n')
        {
            james[a] = '\r';
            a++;
            james[a] = status_conductor->str[b];
            a++;
            b++;

        }
        else if (status_conductor->str[b] == '\0')
        {
            james[a] = '\0';
            break;
        }
        else
        {
            james[a] = status_conductor->str[b];
            a++;
            b++;

        }

    }

    strcpy(status_conductor->str, james);

    /* find number of character in a string */
    char * pch = strstr(status_conductor->str, "\r\n");
    if (pch)
    {
        STRPTR text3 = NULL;
        /* make the default colour the "normal" text pen */

        colour = 8;

        /* copy chars leading up to searched char to a string */

        strncpy(buffer2, status_conductor->str, pch - status_conductor->str);
        buffer2[pch - status_conductor->str] = '\0';

        char *incoming_1, *incoming_2, *incoming_3;

        if (RAW)
            printf("before conversion %s\n", buffer2);

        if ((cs = CodesetsFindBestA(my_incoming_charset3_taglist)))
        {
            if (FindUTF8Chars(buffer2))
            {
                if (DEBUG)
                    printf("codesets 1\n");

                if (!text3)
                {
#ifdef __AROS__
                    struct TagItem my_incoming_charset4_taglist[] =
                    {
                    { CSA_Source, (IPTR) buffer2 },
                    { CSA_SourceCodeset, (IPTR) remote_charsets[0] }, /*{CSA_DestCodeset, (ULONG)local_charsets[local_charset]},*/
                    { TAG_DONE, 0 } };

#else
                    struct TagItem my_incoming_charset4_taglist[] =
                    {
                        {   CSA_Source, (ULONG)buffer2},
                        {   CSA_SourceCodeset, (ULONG)remote_charsets[0]}, /*{CSA_DestCodeset, (ULONG)local_charsets[local_charset]},*/
                        {   TAG_DONE, 0}};
#endif

                    text3 = CodesetsUTF8ToStrA(my_incoming_charset4_taglist);

                    if (DEBUG)
                        printf("codesets 2\n");

                }
            }
            else
            {
                if (DEBUG)
                    printf("codesets 3\n");

                text3 = NULL;
            }
        }
        else
        {
#ifdef __AROS__
            struct TagItem my_incoming_charset4_taglist[] =
            {
            { CSA_Source, (IPTR) buffer2 },
            { CSA_SourceCodeset, (IPTR) remote_charsets[status_conductor->remote_charset] },
            { CSA_DestCodeset, (IPTR) local_charsets[local_charset] },
            { TAG_DONE, 0 } };
#else
            struct TagItem my_incoming_charset4_taglist[] =
            {
                {   CSA_Source, (ULONG)buffer2},
                {   CSA_SourceCodeset, (ULONG)remote_charsets[status_conductor->remote_charset]},
                {   CSA_DestCodeset, (ULONG)local_charsets[local_charset]},
                {   TAG_DONE, 0}};
#endif

            if (1) //if(status_conductor->remote_charset==0)
            {
                text3 = CodesetsUTF8ToStrA(my_incoming_charset4_taglist);

                if (DEBUG)
                    printf("codesets 4\n");

            }
            else
            {
                if (DEBUG)
                    printf("codesets 5\n");

                text3 = NULL;

            }
        }

        if (text3)
            strcpy(buffer2, (char *)text3);

        if (text3)
            CodesetsFreeA(text3, NULL);
        text3 = NULL;

        if (RAW)
            printf("%s\n", buffer2);

        incoming_1 = strtok(buffer2, " ");
        incoming_2 = strtok(NULL, " ");
        incoming_3 = strtok(NULL, "");
        if (!(incoming_1))
        {
            incoming_1 = malloc(sizeof(char));
            strcpy(incoming_1, "");
        }
        if (!(incoming_2))
        {
            incoming_2 = malloc(sizeof(char));
            strcpy(incoming_2, "");
        }
        if (!(incoming_3))
        {
            incoming_3 = malloc(sizeof(char));
            strcpy(incoming_3, "");
        }

        if (DEBUG && incoming_3)
            printf("1:%s, 2:%s, 3:%s\n", incoming_1, incoming_2, incoming_3);
        //else if(RAW) printf("%s %s %s\n",incoming_1, incoming_2, incoming_3);

        if (USE_AREXX == TRUE)
        {
            struct XYMessage *incoming_message = NULL;
            while ((incoming_message = (struct XYMessage*) GetMsg(send_text_replyport)))
            {
                if (incoming_message->xy_Sendtext == 1)
                {
                    //work_status=status_conductor;
                    //printf("pincoming: arexx wants to perform this command:%s",string_to_send2);
                    process_outgoing(string_to_send2, 0);
                    strcpy(string_to_send2, "");
                    //printf("got the send text signal!\n");
                    //printf("replied to send text signal!\n");
                    //status_conductor=work_status;

                }
                ReplyMsg((struct Message*) incoming_message);
                Delay(1);

            }

            if (incoming_3)
                sprintf(status_conductor->last_incoming_line_unparsed, "%s %s %s", incoming_1, incoming_2, incoming_3);
            else
                sprintf(status_conductor->last_incoming_line_unparsed, "%s %s", incoming_1, incoming_2);

            arexx_wait_for_getline_to_be_false();
        }

        timestamp_2_string();

        if (!status_conductor->conductor)
            status_conductor->conductor = status_conductor->root;

        if (!strcmp(incoming_1, "PING"))
        {
            //disable server pings so that we can get disconnected
            //if(status_conductor->filter_swear_words)
            {
                sprintf(sendstuff, "PONG %s\r\n", incoming_2);
                send_text(sendstuff);
                status_conductor->waiting_for_ping = 0;
                //printf("1:%s, 2:%s, 3:%s\n",incoming_1, incoming_2, incoming_3);
                //printf("Sending:PONG %s\r\n", incoming_2);
            }
        }
        else if (!strcmp(incoming_2, "PONG"))
        {
            status_conductor->waiting_for_ping = 0;

        }

        else if (!strcmp(incoming_2, "482"))
        {
            string1 = strtok(incoming_3, " ");
            string2 = strtok(NULL, "");

            if (string1 && string2)
            {
                if (!stricmp(string1, status_conductor->nick))
					sprintf(buffer3, "%s%s%s%s %s", timestamp, LGS( MSG_OPENING_BRACKET ),
							LGS( MSG_ERROR ),  LGS( MSG_CLOSING_BRACKET ), string2);
                else
					sprintf(buffer3, "%s%s%s%s%s %s", timestamp, LGS( MSG_OPENING_BRACKET ),
							LGS( MSG_ERROR ),  LGS( MSG_CLOSING_BRACKET ), string1, string2);

                status_conductor->conductor = status_conductor->current_query;

                add_text_to_current_list((char*) buffer3, colour, ACTIVITY);
            }

        }
        else if (!strcmp(incoming_2, "001"))
        {
            char server[50];
            //status_conductor->quit_requested=FALSE;

            int a = 0;
            while (incoming_1[a + 1] != ' ')
            {
                server[a] = incoming_1[a + 1];
                a++;
            }
            server[a] = '\0';

            strcpy(status_conductor->servername, server);

            if (strcmp(status_conductor->groupname, "") != 0)
            {
                strcpy(status_conductor->networkname, status_conductor->groupname);

                //if(my_settings.no_server_tabs==0) strcpy(status_conductor->root->displayed_name, status_conductor->networkname);

                //update the Server's tabs group Label in our Listview-style Tabs
                if (ZUNE_SYSTEM == TRUE)
                    sprintf(status_conductor->root->nlist_tab_title.entry, "         \033c\033E%s",
                            status_conductor->networkname);
                else
                    sprintf(status_conductor->root->nlist_tab_title.entry, "         :\033c\033E%s",
                            status_conductor->networkname);
                status_conductor->root->nlist_tab_title.colour = 23;
                DoMethod((Object*) WookieChat->LV_tabs, MUIM_NList_ReplaceSingle,
                        &status_conductor->root->nlist_tab_title, status_conductor->root->nlist_tab_title_number);
            }
            else
            {
                //if(my_settings.no_server_tabs==0) strcpy(status_conductor->root->displayed_name,server);

            }

            if (my_settings.no_server_tabs == 0)
                strcpy(status_conductor->root->displayed_name, "Status");

            if (!SMALLTABS && strlen(status_conductor->root->displayed_name) > 11)
            {
                status_conductor->root->displayed_name[10] = '.';
                status_conductor->root->displayed_name[11] = '.';
                status_conductor->root->displayed_name[12] = '\0';
            }

            //update the network name in the appropriate gui objects
            setmacro(status_conductor->root->BT_querybutton, MUIA_Text_Contents, status_conductor->networkname);
            if (status_conductor->networkname[0] == '\0')
                strcpy(status_conductor->networkname, status_conductor->servername);

            if (!strpbrk(status_conductor->root->nlist_tab.entry, "#&"))
            {
                if (ZUNE_SYSTEM == TRUE)
                    strcpy(status_conductor->root->nlist_tab.entry, "         ");
                else
                    strcpy(status_conductor->root->nlist_tab.entry, "         :");

                strcat(status_conductor->root->nlist_tab.entry, status_conductor->root->displayed_name);
                DoMethod((Object*) WookieChat->LV_tabs, MUIM_NList_ReplaceSingle, &status_conductor->root->nlist_tab,
                        status_conductor->root->nlist_tab_number, 0, 0);
            }

            change_window_titlebar_text();

            strcpy(status_conductor->shortserver, server);

            if (!SMALLTABS && my_settings.use_nlistobject_for_tabs == 0)
            {
                status_conductor->shortserver[9] = '.';
                status_conductor->shortserver[10] = '.';
                status_conductor->shortserver[11] = '\0';
            }

            setmacro(status_conductor->root->BT_querybutton, MUIA_Text_Contents, status_conductor->shortserver);

			sprintf(buffer3, "%s%s%s%s %s %s %s", timestamp, LGS( MSG_OPENING_BRACKET ),
							LGS( MSG_SERVER ), LGS( MSG_CLOSING_BRACKET ), incoming_1, incoming_2, incoming_3 );

            status_conductor->conductor = status_conductor->root;
            add_text_to_conductor_list((char*) buffer3, colour, ACTIVITY);

            if (!USING_A_PROXY)
            {
                string1 = strtok(incoming_3, " ");

                string2 = strtok(NULL, "!");

                if (string2)
                {
                    if (strtok(NULL, "@"))
                    {
                        string3 = strtok(NULL, "");
                        if (string3)
                        {
                            //if(strchr(string3,'.'))
                            {
                                //find out your IP address from the hostname given when you connect, for use in DCC transfers
#ifdef __MORPHOS__
                                struct hostent *he=gethostbyname((UBYTE*)string3);
#elif __AROS__
                                struct hostent *he = gethostbyname((char*) string3);
#else
                                struct hostent *he=gethostbyname((i_in)string3);
#endif
                                if (he)
                                {
                                    struct sockaddr_in wookiechat_in;
                                    wookiechat_in.sin_addr = *((struct in_addr *) he->h_addr);
                                    memset(&(wookiechat_in.sin_zero), '\0', 8);
#ifdef __amigaos4__
                                    //newlib compile
                                    //dcc_addr = inet_network(inet_ntoa((struct in_addr)wookiechat_in.sin_addr));
                                    dcc_addr = inet_network((char*)Inet_NtoA(wookiechat_in.sin_addr.s_addr));
#elif __AROS__
                                    dcc_addr = inet_network((char*) Inet_NtoA(wookiechat_in.sin_addr.s_addr));
#elif __MORPHOS__
                                    dcc_addr = inet_network((UBYTE*)Inet_NtoA(wookiechat_in.sin_addr.s_addr));
#else
                                    dcc_addr = inet_network((i_in)Inet_NtoA(wookiechat_in.sin_addr.s_addr));
#endif
                                }
                                else
                                {
                                    sprintf(buffer3,
                                            "%s%sDCC%s Unable to use obtain own address for use in DCC connections",
											timestamp,
											LGS( MSG_OPENING_BRACKET ),
											LGS( MSG_CLOSING_BRACKET ) );
                                    add_text_to_current_list(buffer3, 9, ACTIVITY);
                                }

                            }
                        }
                    }
                }
            }
            else
                dcc_addr = 0;

            //this is the built in Nickserv login/registering system in WookieChat
            if (status_conductor->nick_pass[0] != '\0')
            {
                sprintf(buffer3, "PRIVMSG nickserv :identify %s\r\n", status_conductor->nick_pass);
                send_text(buffer3);
            }

            //this is the built in Nickserv login/registering system in WookieChat
            if (status_conductor->auto_joins[0] != '\0')
            {
                sprintf(buffer3, "JOIN :%s\r\n", status_conductor->auto_joins);
                send_text(buffer3);
            }

            char work_buffer[WORK_BUFFER_SIZE];

            if (strcmp(status_conductor->groupname, "") != 0)
            {

                sprintf(work_buffer, "progdir:connectscripts/%s", status_conductor->groupname);

                BPTR newbptr_file = Open((_s_cs)work_buffer, MODE_OLDFILE);

                if (newbptr_file)
                {
                    char *len2 = (char*) FGets(newbptr_file, (STRPTR) work_buffer, WORK_BUFFER_SIZE);

                    if (work_buffer[strlen(work_buffer) - 1] == '\n')
                        work_buffer[strlen(work_buffer) - 1] = '\0';

                    unsigned int a = 0;

                    while (len2 && a < 20)
                    {
                        strcat(work_buffer, "\r\n");

                        if (stricmp(work_buffer, "\r\n"))
                        {
                            int count;
                            //string1 == your typed command
                            //string4 == your typed stuff after command
                            //string2 == command alias keyword
                            //string3 == command alias stuff after command

                            for (count = 1; count <= my_settings.number_of_command_aliases; count++)
                            {
                                strcpy(string7, work_buffer);
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

                                        if (string9[0] != ';')
                                            process_outgoing(string9, 0);
                                        break;
                                    }

                                }
                            }

                            if (count > my_settings.number_of_command_aliases)
                            {
                                if (work_buffer[0] != ';')
                                    process_outgoing(work_buffer, 0);
                            }
                        }

                        len2 = (char*) FGets(newbptr_file, (STRPTR) work_buffer, WORK_BUFFER_SIZE);

                        if (work_buffer[strlen(work_buffer) - 1] == '\n')
                            work_buffer[strlen(work_buffer) - 1] = '\0';

                        a++;
                    }

                    Close(newbptr_file);
                }
            }

            //rejoin channels that have open tabs!!
            for (status_conductor->conductor = status_conductor->root; status_conductor->conductor;
                    status_conductor->conductor = status_conductor->conductor->next)
            {
                if (strcspn(status_conductor->conductor->name, status_conductor->chantypes) == 0)
                {
                    if (!stricmp(status_conductor->conductor->keyword, ""))
                        sprintf(buffer3, "JOIN %s\r\n", status_conductor->conductor->name);
                    else
                        sprintf(buffer3, "JOIN %s %s\r\n", status_conductor->conductor->name,
                                status_conductor->conductor->keyword);
                    send_text(buffer3);
                }
            }

        }
        else if (!strcmp(incoming_2, "WALLOPS"))
        {

            //1::prism!Alchemy@Admin.z0r.org, 2:WALLOPS, 3::OPERWALL - Test message, this is for deevlopment purposes please ign clicked on 0

            //string7 == nick/sender
            //string9 == actual message

            a = 1, b = 0;

            while (incoming_1[a] != '!' && incoming_1[a] != ' ' && incoming_1[a] != '\0')
            {
                string7[b] = incoming_1[a];
                a++;
                b++;
            }
            string7[b] = '\0';

            b = 0;

            if (incoming_3[0] == ':')
                a = 1;
            else
                a = 0;

            while (incoming_3[a] != '\0')
            {
                string9[b] = incoming_3[a];
                b++;
                a++;
            }
            string9[b] = '\0';

            //if(string7 && string9)
            {
                sprintf(buffer3, "%s-%s- %s", timestamp, string7, string9);
                add_text_to_current_list((char*) buffer3, 17, ACTIVITY);
                //notice colour is 12
            }

        }
        else if (!strcmp(incoming_2, "INVITE"))
        {
            timestamp_2_string();

            string1 = strtok(incoming_3, ":"); //nick who invited person

            string2 = strtok(NULL, ""); //channel you're being invited to

            for (a = 1; a < strlen(incoming_1); a++)
            {
                string7[a - 1] = incoming_1[a];
                if (string7[a - 1] == '!')
                {
                    string7[a - 1] = '\0';
                    break;
                }
            }

            if (string1 && string2)
            {
				sprintf(buffer3, "%s%s%s%s %s%s %s %s %s", timestamp,
						LGS( MSG_OPENING_BRACKET ),
						LGS( MSG_INVITE ),
						LGS( MSG_CLOSING_BRACKET ),
						string1,
						LGS( MSG_HAS_BEEN_INVITED_TO_CHANNEL ),
						string2,
						LGS( MSG_INVITED_BY_USER ),
						string7);

                add_text_to_conductor_list((char*) buffer3, 14, ACTIVITY);

                if ((my_settings.events[INVITE].use_when == 1 && is_window_active())
                        || (my_settings.events[INVITE].use_when == 2 && !is_window_active())
                        || my_settings.events[INVITE].use_when == 3)
                {
					sprintf(buffer3, "%s%s%s %s%s %s %s %s",
						LGS( MSG_OPENING_BRACKET ),
						LGS( MSG_INVITE ),
						LGS( MSG_CLOSING_BRACKET ),
						string1,
						LGS( MSG_HAS_BEEN_INVITED_TO_CHANNEL ),
						string2,
						LGS( MSG_INVITED_BY_USER ),
						string7);

                    strcpy(target_nick, string1);
                    create_arexx_event_string(my_settings.events[INVITE].arexx_script,
                            my_settings.events[INVITE].arexx_script2, buffer3);
                    Execute((_s_cs)(_s_cs)event_string, 0, 0);
                }

            }

        }
        else if (!strcmp(incoming_2, "005"))
        {

            status_conductor->retry_number = 0;

            string1 = strtok(incoming_3, " ");
            string2 = strtok(NULL, "");

            if (string1 && string2)
            {
                if (!stricmp(string1, status_conductor->nick))
					sprintf(buffer3, "%s%s%s%s %s", timestamp, LGS( MSG_OPENING_BRACKET ),
							LGS( MSG_SERVER ), LGS( MSG_CLOSING_BRACKET ), string2);
                else
					sprintf(buffer3, "%s%s%s%s %s %s", timestamp, LGS( MSG_OPENING_BRACKET ),
							LGS( MSG_SERVER ), LGS( MSG_CLOSING_BRACKET ), string1, string2);

                add_text_to_conductor_list((char*) buffer3, colour, ACTIVITY);

                if (stricmp(string1, status_conductor->nick))
                    strcpy(status_conductor->nick, string1);
                update_your_nickname_text_label();

            }

            char *work;
            char *nick_length_char = malloc(sizeof(char) * 3);

            work = strstr(string2, "NICKLEN=");
            if (work)
            {
                int a1 = 8, b1 = 0;

                while (work[a1] != ' ')
                {
                    nick_length_char[b1] = work[a1];
                    a1++;
                    b1++;
                }
                nick_length_char[b1] = '\0';

                nick_length = atoi(nick_length_char);
                status_conductor->nick[nick_length] = '\0';
                char *string1;
                char *string2;

                strcpy(buffer3, incoming_3);

                string1 = strtok(buffer3, " ");
                string2 = strtok(NULL, "");

                if (string1 && string2)
                {
                    if (!strcmp(string1, status_conductor->nick))
						sprintf(buffer3, "%s%s%s%s %s", timestamp, LGS( MSG_OPENING_BRACKET ),
							LGS( MSG_SERVER ), LGS( MSG_CLOSING_BRACKET ), string2);
                    else
						sprintf(buffer3, "%s%s%s%s %s %s", timestamp, LGS( MSG_OPENING_BRACKET ),
							LGS( MSG_SERVER ), LGS( MSG_CLOSING_BRACKET ), string1,
                                string2);

                    if (!status_conductor->conductor)
                        status_conductor->conductor = status_conductor->root;

                    add_text_to_conductor_list((char*) buffer3, colour, ACTIVITY);

                    change_window_titlebar_text();
                }

            }

            work = strstr(string2, "NETWORK=");
            if (work)
            {
                int a1 = 8, b1 = 0;

                while (work[a1] != ' ')
                {
                    status_conductor->networkname[b1] = work[a1];
                    a1++;
                    b1++;
                }
                status_conductor->networkname[b1] = '\0';

                if (status_conductor->networkname[0] != '\0')
                {
                    sprintf(string7, "\033I[2:00000000,00000000,00000000] %s", status_conductor->networkname);
                } //keep the proper network name in the button instead of "status"

                setmacro(status_conductor->root->BT_querybutton, MUIA_Text_Contents, string7);

                if (status_conductor->networkname[0] == '\0')
                    strcpy(status_conductor->networkname, status_conductor->servername);

                if (!strpbrk(status_conductor->root->nlist_tab.entry, "#&"))
                {

                    if (my_settings.no_server_tabs == 0 && my_settings.use_nlistobject_for_tabs == 0)
                        strcpy(status_conductor->root->displayed_name, status_conductor->networkname);

                    //strcpy(status_conductor->root->nlist_tab.entry,"         :");
                    //strcat(status_conductor->root->nlist_tab.entry,status_conductor->root->displayed_name);
                    //DoMethod((Object*)WookieChat->LV_tabs,MUIM_NList_ReplaceSingle,&status_conductor->root->nlist_tab,status_conductor->root->nlist_tab_number,0,0);
                }

            }

            strcpy(status_conductor->chantypes, "#");
            status_conductor->chantypes_total = 1;

            work = strstr(string2, "CHANTYPES=");
            if (work)
            {
                //strncpy(status_conductor->chantypes,work,15);
                //printf("chantypes for this network=\"%s\"\n",status_conductor->chantypes);
                strcpy(work_buffer, work);
                strtok(work_buffer, "=");
                string1 = strtok(NULL, " \n");
                if (string1)
                {
                    strcpy(status_conductor->chantypes, string1);
                    status_conductor->chantypes_total = strlen(string1);
                    //printf("chantypes for this network=%s total=%d\n",status_conductor->chantypes,status_conductor->chantypes_total);
                }
            }

            work = strstr(string2, "PREFIX=");
            if (work)
            {
                if (strchr(work, '('))
                {
                    strcpy(work_buffer, work);
                    strtok(work_buffer, "(");
                    string1 = strtok(NULL, ")");
                    string2 = strtok(NULL, " \r\n");

                    for (unsigned int count = 1; count <= strlen(string1) && count < 15; count++)
                    {
                        status_conductor->user_modes[count].mode[0] = string1[count - 1];
                        status_conductor->user_modes[count].symbol[0] = string2[count - 1];
                        status_conductor->max_modes = count;
                        status_conductor->user_modes[count + 1].mode[0] = ' ';
                        status_conductor->user_modes[count + 1].symbol[0] = ' ';
                        status_conductor->user_modes[count + 1].mode[1] = '\0';
                        status_conductor->user_modes[count + 1].symbol[1] = '\0';

                    }
                }

            }

            free(nick_length_char);

        }
        else if (!strcmp(incoming_2, "352")) // result from the /WHO command
        {
            //sample of what it looks like raw
            //3:jahcman #test ~jahc 1f5b825b.164642ae.xnet.co.nz SE2.AmigaWorld.Net

            strtok(incoming_3, " ");
            string1 = strtok(NULL, " "); //channel
            string2 = strtok(NULL, " "); //userid
            string3 = strtok(NULL, " "); //hostname
            strtok(NULL, " "); //servername
            string4 = strtok(NULL, " "); //nick being who'd

            if (string1 && string2 && string3 && string4)
            {

                //printf("string1:%s string2:%s string3:%s string4:%s\n",string1,string2,string3,string4);

                sprintf(string10, "%s@%s", string2, string3);
                for (status_conductor->conductor = status_conductor->root; status_conductor->conductor;
                        status_conductor->conductor = status_conductor->conductor->next)
                {
                    if (!stricmp(string1, status_conductor->conductor->name))
                    {

                        update_nicks_hostname(string4, string10);

                        break;

                    }

                }

            }

        }
        else if (!strcmp(incoming_2, "315")) // End of /WHO List
        {
            //raw text for incoming end of /who list:
            //2:315, 3:jahcman #AmigaWorld :End of /WHO list.

            strtok(incoming_3, " ");
            string1 = strtok(NULL, " "); //channelname

            //lets find clones!
            if (string1)
            {
                int count3 = 0;
                count_clones = 0;

                for (a = 0; a < 15; a++)
                    strcpy(clone_results[a], "");

                for (status_conductor->conductor = status_conductor->root; status_conductor->conductor;
                        status_conductor->conductor = status_conductor->conductor->next)
                {
                    if (!stricmp(status_conductor->conductor->name, string1))
                        break;
                }

                if (status_conductor->conductor)
                {

                    for (int count2 = 0; count2 < status_conductor->conductor->nicks; count2++)
                    {

                        //found_clones=FALSE;
                        count3 = 0;

                        if (status_conductor->conductor->nicklist[count2].hostname)
                        {
                            strcpy(string10, status_conductor->conductor->nicklist[count2].hostname);
                            strtok(string10, "@");
                            string1 = strtok(NULL, "");

                            if (!string1)
                                break;

							sprintf(buffer3, "%s%s%s%s %s %s: ", timestamp, LGS( MSG_OPENING_BRACKET ),
									LGS( MSG_CLONES ), LGS( MSG_CLOSING_BRACKET ),
									LGS( MSG_CLONE_DETECTED ), string1 );

                            for (int count = 0; count < status_conductor->conductor->nicks; count++)
                            {

                                if (status_conductor->conductor->nicklist[count].hostname)
                                {
                                    strcpy(string11, status_conductor->conductor->nicklist[count].hostname);
                                    strtok(string11, "@");
                                    string2 = strtok(NULL, "");

                                    if (string1 && string2)
                                    {
                                        if (!strcmp(string2, string1))
                                        {
                                            strcat(buffer3, status_conductor->conductor->nicklist[count].name);
                                            strcat(buffer3, " ");
                                            //found_clones=TRUE;
                                            count3++;

                                            //printf("found hostname %d: %s comparing to %s\n",count,string2, string1);

                                        }
                                    }
                                }

                            }

                        }

                        if (count3 > 1)
                        {
                            if (count_clones == MAX_CLONE_FIELDS)
                                break;

                            for (a = 0; a < MAX_CLONE_FIELDS; a++)
                            {
                                if (!stricmp(clone_results[a], buffer3))
                                    break;
                            }

                            if (a >= (ULONG) count_clones)
                            {
                                strncpy(clone_results[count_clones], buffer3, 900 - 1);
                                clone_results[count_clones][900] = '\0';
                                count_clones++;
                            }

                        }
                    }

                    for (int a = 0; a < MAX_CLONE_FIELDS && a < count_clones; a++)
                    {
                        add_text_to_conductor_list((char*) clone_results[a], 9, ACTIVITY);
                    }

                    count_clones = 0;
                }

            }

        }
        else if (!strcmp(incoming_2, "615")) //display MODE information about a user
        {
//1:NO.AmigaWorld.Net, 2:615, 3:jahcman jahcman :is using modes +i +

            strtok(incoming_3, " ");
            string1 = strtok(NULL, ":"); //users name
            if (string1)
            {
                string2 = strtok(NULL, "");
            } //is using modes (whatever)

            if (string1 && string2)
            {
				sprintf(buffer3, "%s%sMode%s %s%s", timestamp, LGS( MSG_OPENING_BRACKET ),
									LGS( MSG_CLOSING_BRACKET ), string1, string2);

                add_text_to_conductor_list((char*) buffer3, 5, ACTIVITY);
            }

        }

        else if (!strcmp(incoming_2, "301") || !strcmp(incoming_2, "307") || !strcmp(incoming_2, "311")
                || !strcmp(incoming_2, "312") || !strcmp(incoming_2, "313") || !strcmp(incoming_2, "317")
                || !strcmp(incoming_2, "318") || !strcmp(incoming_2, "319") || !strcmp(incoming_2, "310")
                || !strcmp(incoming_2, "378") || !strcmp(incoming_2, "616") || !strcmp(incoming_2, "614")) // WHOIS RESULT DISPLAY
        {
            char *work1;
            strcpy(string7, incoming_3);

            strtok(incoming_3, " ");
            work1 = strtok(NULL, " "); //nick being whos'd
            string1 = strtok(NULL, "");

            if (!strcmp(incoming_2, "301"))
            {
				sprintf(buffer3, "%s%sWhois%s %s User is away %s", timestamp,
						LGS( MSG_CLOSING_BRACKET ), work1, string1);

            }
            else if (!strcmp(incoming_2, "317")) //seconds idle, signon time
            {
                int count;

                for (count = 0; count < (LONG) strlen(string7) && string7[count] != ' '; count++)
                    ;
                count++;
                for (; count < (LONG) strlen(string7) && string7[count] != ' '; count++)
                    ;
                count++;

                int count2 = count;

                for (count = 0; count2 < (LONG) strlen(string7); count++, count2++)
                {
                    string10[count] = string7[count2];
                    if (string10[count] == ' ')
                    {
                        string10[count] = '\0';
                        count++;
                        count2++;
                        break;
                    }
                }

                for (count = 0; count2 < (LONG) strlen(string7); count++, count2++)
                {
                    string11[count] = string7[count2];
                    if (string11[count] == ' ')
                    {
                        string11[count] = '\0';
                        count++;
                        break;
                    }

                }

                //if(string10 && string11)
                {
                    ULONG secs, mins, hrs, days;
                    time_t rawtime = atoi(string11);
                    struct tm *timeinfo;
                    timeinfo = localtime(&rawtime);

                    secs = atoi(string10);
                    mins = secs / 60;
                    hrs = mins / 60;
                    days = hrs / 24;
                    secs = secs % 60;
                    mins = mins % 60;
                    hrs = hrs % 24; // Compute days, hours, etc.

                    if (days > 0)
                        sprintf(string10, "%ld days, %ld hrs, %ld mins %ld secs", days, hrs, mins, secs);
                    else if (hrs > 0)
                        sprintf(string10, "%ld hrs, %ld mins, %ld secs", hrs, mins, secs);
                    else if (mins > 0)
                        sprintf(string10, "%ld mins, %ld secs", mins, secs);
                    else
                        sprintf(string10, "%ld secs", secs);

                    sprintf(buffer3, "%s%sWhois%s Idle: %s, Signed on at: %s", timestamp,
							LGS( MSG_OPENING_BRACKET ),
							LGS( MSG_CLOSING_BRACKET ),
							string10,
                            asctime(timeinfo));

                    //theres an \n or \r  at the end of this string, get rid of it
                    buffer3[strlen(buffer3) - 1] = '\0';
                }

            }
            else
            {

                if (string1)
					sprintf(buffer3, "%s%sWhois%s %s %s", timestamp, LGS( MSG_OPENING_BRACKET ),
							LGS( MSG_CLOSING_BRACKET ), work1, string1);
                else
					sprintf(buffer3, "%s%sWhois%s %s", timestamp, LGS( MSG_OPENING_BRACKET ),
							LGS( MSG_CLOSING_BRACKET ), work1);

            }

            //add_text_to_conductor_list(buffer3, 9, ACTIVITY);

            if (string1)
            {
                if (!(!strcmp(string1, status_conductor->current_query->away_message) && !strcmp(incoming_2, "301")))
                    add_text_to_current_list(buffer3, 9, ACTIVITY);

                if (!strcmp(incoming_2, "301"))
                    strcpy(status_conductor->current_query->away_message, string1);
            }

            if (!strcmp(incoming_2, "311"))
            {
                string2 = strtok(string1, " "); //whois's persons username
                string3 = strtok(NULL, " "); //whois's persons hostname

                if (string2 && string3)
                {
                    //status_conductor=status_current;
                    //status_conductor->conductor=current_query;
                    if (status_conductor->conductor)
                    {
                        sprintf(string10, "%s@%s", string2, string3);
                        update_nicks_hostname(work1, string10);
                        //printf("work1:%s string10:%s\n",work1,string10);
                    }
                }

            }

        }
        else if (!strcmp(incoming_2, "305") || !strcmp(incoming_2, "306")) // away messages
        {
            strtok(incoming_3, ":");
            string1 = strtok(NULL, "");
            if (string1)
            {
				sprintf(buffer3, "%s%sAway%s %s", timestamp, LGS( MSG_OPENING_BRACKET ),
						LGS( MSG_CLOSING_BRACKET ), string1);

                add_text_to_conductor_list(buffer3, 9, ACTIVITY);
            }
            if (!strcmp(incoming_2, "305"))
                status_conductor->away = FALSE;
            else if (!strcmp(incoming_2, "306"))
                status_conductor->away = TRUE;

        }
        else if (!strcmp(incoming_2, "367")) //an incoming banmask for a channel
        {
            strtok(incoming_3, " ");
            strtok(NULL, " ");
            string1 = strtok(NULL, " "); //ban mask
            string2 = strtok(NULL, " "); //banned by
            string3 = strtok(NULL, ""); //date and time ban was placed

            if (string1 && string2 && string3)
            {
                sprintf(buffer3, "%s %s %s", string1, string2, string3);
                DoMethod((Object*) WookieChat->LV_ban, MUIM_NList_InsertSingle, buffer3, MUIV_NList_Insert_Bottom,
                        ALIGN_LEFT);

            }

        }
        else if (!strcmp(incoming_2, "368")) //end of banmask list
        {

            // do nothing!

        }
        else if (!strcmp(incoming_2, "341")) //message telling you that you have just invited someone to a channel
        {
            timestamp_2_string();

            strtok(incoming_3, " ");
            string1 = strtok(NULL, " "); //nick who invited you

            string2 = strtok(NULL, ""); //channel you're being invited to

            if (string1 && string2)
            {
				sprintf(buffer3, "%s%sInvite%s %s %s %s %s", timestamp, LGS( MSG_OPENING_BRACKET ),
						LGS( MSG_CLOSING_BRACKET ),
						LGS( MSG_YOU_HAVE_INVITED ),
						string1,
						LGS( MSG_TO_CHANNEL ),
						string2 );
                add_text_to_conductor_list((char*) buffer3, 14, ACTIVITY);
            }

        }
        else if (!strcmp(incoming_2, "321")) //a list of channels on this network is about to be sent to us
        {

            for (a = 0; a != strlen(incoming_1) + 1; a++)
                rawservername[a] = incoming_1[a + 1]; //servername

            status_conductor = status_root;
            do
            {
                if (!status_conductor)
                    break;
                if (!stricmp(status_conductor->servername, rawservername))
                    break;
                status_conductor = status_conductor->next;

            } while (1);

            status_conductor->conductor = status_conductor->root;
            do
            {
                if (!status_conductor->conductor)
                    break;
                if (!stricmp(status_conductor->conductor->name, "*Channel_List"))
                    break;
                status_conductor->conductor = status_conductor->conductor->next;

            } while (1);

            if (!status_conductor->conductor)
                create_new_tab((char*) "*Channel_List", 1, 2); //open a private query tab to display channel names in
            setmacro((Object*) status_current->current_query->LV_channel, MUIA_NList_DisplayHook,
                    &Display_channellist_TextHook);
            setmacro((Object*) status_current->current_query->LV_channel, MUIA_NList_Format,
                    "COL=0 WEIGHT=-1 BAR, COL=1 WEIGHT=-1 BAR, COL=2");

            strtok(incoming_3, " ");
            string1 = strtok(NULL, "");

            //if(string1) add_text_to_conductor_list(string1, 9, ACTIVITY);
            if (string1)
                add_text_to_channels_query(string1, 9, ACTIVITY);

        }
        else if (!strcmp(incoming_2, "322")) //channel list
        {

            for (a = 0; a != strlen(incoming_1) + 1; a++)
                rawservername[a] = incoming_1[a + 1]; //servername

            status_conductor = status_root;
            do
            {
                if (!status_conductor)
                    break;
                if (!stricmp(status_conductor->servername, rawservername))
                    break;
                status_conductor = status_conductor->next;

            } while (1);

            if (status_conductor)
            {
                status_conductor->conductor = status_conductor->root;

                do
                {
                    if (!status_conductor->conductor)
                        break;
                    if (!stricmp(status_conductor->conductor->name, "*Channel_List"))
                        break;
                    status_conductor->conductor = status_conductor->conductor->next;

                } while (1);

                if (status_conductor->conductor)
                {
                    strtok(incoming_3, " ");
                    string1 = strtok(NULL, "");

                    if (string1)
                    {

                        //add_text_to_conductor_list(string1, 9, ACTIVITY);

                        add_text_to_channels_query(string1, 8, ACTIVITY);

                    }
                }

            }

        }
        else if (!strcmp(incoming_2, "323")) //Server has finished sending list of channels
        {

        }
        else if (!strcmp(incoming_2, "324"))
        {
            strtok(incoming_3, " ");
            char *channel;
            channel = strtok(NULL, " ");
            char *modes;
            modes = strtok(NULL, " ");
            char *mode_params;
            mode_params = strtok(NULL, "\0");
            char *work;
            work = strtok(mode_params, " ");
            int a = 0;

            status_conductor->conductor = status_conductor->root;
            while (stricmp(status_conductor->conductor->name, channel))
            {
                status_conductor->conductor = status_conductor->conductor->next;
                if (!status_conductor->conductor)
                    break;
            }

            //if(DEBUG) printf("getting modes for channel..\n");
            if (status_conductor->conductor)
            {
                while (modes[a])
                {
                    char prefix = modes[a];

                    if (prefix == 'n')
                    {
                        //DoMethod((Object*)status_conductor->conductor->BT_mode_N,MUIM_KillNotify,MUIA_Selected);
                        if (status_conductor->conductor == status_current->current_query)
                        {
                            DoMethod((Object*) status_conductor->conductor->BT_mode_N, MUIM_KillNotify, MUIA_Selected);

                            setmacro((Object*) status_conductor->conductor->BT_mode_N, MUIA_Selected, TRUE);

                            DoMethod((Object*) status_conductor->conductor->BT_mode_N, MUIM_Notify, MUIA_Selected, TRUE,
                                    (Object*) WookieChat->App, 2, MUIM_Application_ReturnID, 11);
                            DoMethod((Object*) status_conductor->conductor->BT_mode_N, MUIM_Notify, MUIA_Selected,
                                    FALSE, (Object*) WookieChat->App, 2, MUIM_Application_ReturnID, 21);

                        } //DoMethod((Object*)status_conductor->conductor->BT_mode_N,MUIM_Notify,MUIA_Selected,TRUE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,11);
                          //DoMethod((Object*)status_conductor->conductor->BT_mode_N,MUIM_Notify,MUIA_Selected,FALSE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,21);
                        status_conductor->conductor->mode_N = 1;

                    }

                    if (prefix == 't')
                    {
                        if (status_conductor->conductor == status_current->current_query)
                        {
                            //DoMethod((Object*)status_conductor->conductor->BT_mode_N,MUIM_KillNotify,MUIA_Selected);
                            DoMethod((Object*) status_conductor->conductor->BT_mode_T, MUIM_KillNotify, MUIA_Selected);

                            setmacro((Object*) status_conductor->conductor->BT_mode_T, MUIA_Selected, TRUE);

                            if (status_current->current_query->BT_mode_T)
                                DoMethod((Object*) status_current->current_query->BT_mode_T, MUIM_Notify, MUIA_Selected,
                                        TRUE, (Object*) WookieChat->App, 2, MUIM_Application_ReturnID, 10);
                            if (status_current->current_query->BT_mode_T)
                                DoMethod((Object*) status_current->current_query->BT_mode_T, MUIM_Notify, MUIA_Selected,
                                        FALSE, (Object*) WookieChat->App, 2, MUIM_Application_ReturnID, 20);

                        } //DoMethod((Object*)status_conductor->conductor->BT_mode_T,MUIM_Notify,MUIA_Selected,TRUE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,10);
                          //DoMethod((Object*)status_conductor->conductor->BT_mode_T,MUIM_Notify,MUIA_Selected,FALSE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,20);
                        status_conductor->conductor->mode_T = 1;
                    }

                    if (prefix == 's')
                    {

                        //DoMethod((Object*)status_conductor->conductor->BT_mode_S,MUIM_KillNotify,MUIA_Selected);
                        if (status_conductor->conductor == status_current->current_query)
                        {
                            DoMethod((Object*) status_conductor->conductor->BT_mode_S, MUIM_KillNotify, MUIA_Selected);

                            setmacro((Object*) status_conductor->conductor->BT_mode_S, MUIA_Selected, TRUE);

                            DoMethod((Object*) status_conductor->conductor->BT_mode_S, MUIM_Notify, MUIA_Selected, TRUE,
                                    (Object*) WookieChat->App, 2, MUIM_Application_ReturnID, 12);
                            DoMethod((Object*) status_conductor->conductor->BT_mode_S, MUIM_Notify, MUIA_Selected,
                                    FALSE, (Object*) WookieChat->App, 2, MUIM_Application_ReturnID, 22);
                        } //DoMethod((Object*)status_conductor->conductor->BT_mode_S,MUIM_Notify,MUIA_Selected,TRUE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,12);
                          //DoMethod((Object*)status_conductor->conductor->BT_mode_S,MUIM_Notify,MUIA_Selected,FALSE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,22);
                        status_conductor->conductor->mode_S = 1;
                    }

                    if (prefix == 'i')
                    {

                        //DoMethod((Object*)status_conductor->conductor->BT_mode_I,MUIM_KillNotify,MUIA_Selected);
                        if (status_conductor->conductor == status_current->current_query)
                        {
                            DoMethod((Object*) status_conductor->conductor->BT_mode_I, MUIM_KillNotify, MUIA_Selected);

                            setmacro((Object*) status_conductor->conductor->BT_mode_I, MUIA_Selected, TRUE);

                            DoMethod((Object*) status_conductor->conductor->BT_mode_I, MUIM_Notify, MUIA_Selected, TRUE,
                                    (Object*) WookieChat->App, 2, MUIM_Application_ReturnID, 13);
                            DoMethod((Object*) status_conductor->conductor->BT_mode_I, MUIM_Notify, MUIA_Selected,
                                    FALSE, (Object*) WookieChat->App, 2, MUIM_Application_ReturnID, 23);
                        } //DoMethod((Object*)status_conductor->conductor->BT_mode_I,MUIM_Notify,MUIA_Selected,TRUE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,13);
                          //DoMethod((Object*)status_conductor->conductor->BT_mode_I,MUIM_Notify,MUIA_Selected,FALSE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,23);
                        status_conductor->conductor->mode_I = 1;
                    }

                    if (prefix == 'p')
                    {
                        //DoMethod((Object*)status_conductor->conductor->BT_mode_P,MUIM_KillNotify,MUIA_Selected);
                        if (status_conductor->conductor == status_current->current_query)
                        {
                            DoMethod((Object*) status_conductor->conductor->BT_mode_P, MUIM_KillNotify, MUIA_Selected);

                            setmacro((Object*) status_conductor->conductor->BT_mode_P, MUIA_Selected, TRUE);

                            DoMethod((Object*) status_conductor->conductor->BT_mode_P, MUIM_Notify, MUIA_Selected, TRUE,
                                    (Object*) WookieChat->App, 2, MUIM_Application_ReturnID, 14);
                            DoMethod((Object*) status_conductor->conductor->BT_mode_P, MUIM_Notify, MUIA_Selected,
                                    FALSE, (Object*) WookieChat->App, 2, MUIM_Application_ReturnID, 24);

                        } //DoMethod((Object*)status_conductor->conductor->BT_mode_P,MUIM_Notify,MUIA_Selected,TRUE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,14);
                          //DoMethod((Object*)status_conductor->conductor->BT_mode_P,MUIM_Notify,MUIA_Selected,FALSE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,24);
                        status_conductor->conductor->mode_P = 1;
                    }

                    if (prefix == 'm')
                    {
                        //DoMethod((Object*)status_conductor->conductor->BT_mode_M,MUIM_KillNotify,MUIA_Selected);
                        if (status_conductor->conductor == status_current->current_query)
                        {
                            DoMethod((Object*) status_conductor->conductor->BT_mode_M, MUIM_KillNotify, MUIA_Selected);

                            setmacro((Object*) status_conductor->conductor->BT_mode_M, MUIA_Selected, TRUE);

                            DoMethod((Object*) status_conductor->conductor->BT_mode_M, MUIM_Notify, MUIA_Selected, TRUE,
                                    (Object*) WookieChat->App, 2, MUIM_Application_ReturnID, 15);
                            DoMethod((Object*) status_conductor->conductor->BT_mode_M, MUIM_Notify, MUIA_Selected,
                                    FALSE, (Object*) WookieChat->App, 2, MUIM_Application_ReturnID, 25);
                        } //DoMethod((Object*)status_conductor->conductor->BT_mode_M,MUIM_Notify,MUIA_Selected,TRUE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,15);
                          //DoMethod((Object*)status_conductor->conductor->BT_mode_M,MUIM_Notify,MUIA_Selected,FALSE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,25);
                        status_conductor->conductor->mode_M = 1;
                    }

                    if (prefix == 'k')
                    {
                        //DoMethod((Object*)status_conductor->conductor->BT_mode_K,MUIM_KillNotify,MUIA_Selected);
                        //DoMethod((Object*)status_conductor->conductor->STR_keyword,MUIM_KillNotify,MUIA_String_Acknowledge);
                        if (status_conductor->conductor == status_current->current_query)
                        {
                            DoMethod((Object*) status_conductor->conductor->BT_mode_K, MUIM_KillNotify, MUIA_Selected);
                            DoMethod((Object*) status_conductor->conductor->STR_keyword, MUIM_KillNotify,
                                    MUIA_String_Acknowledge);

                            setmacro((Object*) status_conductor->conductor->BT_mode_K, MUIA_Selected, TRUE);
                            setmacro((Object*) status_conductor->conductor->STR_keyword, MUIA_String_Contents, work);

                            DoMethod((Object*) status_conductor->conductor->BT_mode_K, MUIM_Notify, MUIA_Selected, TRUE,
                                    (Object*) WookieChat->App, 2, MUIM_Application_ReturnID, 17);
                            DoMethod((Object*) status_conductor->conductor->BT_mode_K, MUIM_Notify, MUIA_Selected,
                                    FALSE, (Object*) WookieChat->App, 2, MUIM_Application_ReturnID, 28);
                            DoMethod((Object*) status_conductor->conductor->STR_keyword, MUIM_Notify,
                                    MUIA_String_Acknowledge, MUIV_EveryTime, (Object*) WookieChat->App, 2,
                                    MUIM_Application_ReturnID, 17);

                        } //DoMethod((Object*)status_conductor->conductor->BT_mode_K,MUIM_Notify,MUIA_Selected,TRUE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,17);
                          //DoMethod((Object*)status_conductor->conductor->BT_mode_K,MUIM_Notify,MUIA_Selected,FALSE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,28);
                          //DoMethod((Object*)status_conductor->conductor->STR_keyword,MUIM_Notify,MUIA_String_Acknowledge, MUIV_EveryTime, (Object*)WookieChat->App,2,MUIM_Application_ReturnID,17);
                        status_conductor->conductor->mode_K = 1;
                        strcpy(status_conductor->conductor->keyword, work);
                    }

                    if (prefix == 'l')
                    {
                        //DoMethod((Object*)status_conductor->conductor->BT_mode_L,MUIM_KillNotify,MUIA_Selected);
                        //DoMethod((Object*)status_conductor->conductor->STR_limit,MUIM_KillNotify,MUIA_String_Acknowledge);
                        if (status_conductor->conductor == status_current->current_query)
                        {
                            DoMethod((Object*) status_conductor->conductor->BT_mode_L, MUIM_KillNotify, MUIA_Selected);
                            DoMethod((Object*) status_conductor->conductor->STR_limit, MUIM_KillNotify,
                                    MUIA_String_Acknowledge);

                            setmacro((Object*) status_conductor->conductor->BT_mode_L, MUIA_Selected, TRUE);
                            setmacro((Object*) status_conductor->conductor->STR_limit, MUIA_String_Contents, work);

                            DoMethod((Object*) status_conductor->conductor->BT_mode_L, MUIM_Notify, MUIA_Selected, TRUE,
                                    (Object*) WookieChat->App, 2, MUIM_Application_ReturnID, 16);
                            DoMethod((Object*) status_conductor->conductor->BT_mode_L, MUIM_Notify, MUIA_Selected,
                                    FALSE, (Object*) WookieChat->App, 2, MUIM_Application_ReturnID, 27);
                            DoMethod((Object*) status_conductor->conductor->STR_limit, MUIM_Notify,
                                    MUIA_String_Acknowledge, MUIV_EveryTime, (Object*) WookieChat->App, 2,
                                    MUIM_Application_ReturnID, 16);

                        } //DoMethod((Object*)status_conductor->conductor->BT_mode_L,MUIM_Notify,MUIA_Selected,TRUE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,16);
                          //DoMethod((Object*)status_conductor->conductor->BT_mode_L,MUIM_Notify,MUIA_Selected,FALSE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,27);
                          //DoMethod((Object*)status_conductor->conductor->STR_limit,MUIM_Notify,MUIA_String_Acknowledge, MUIV_EveryTime, (Object*)WookieChat->App,2,MUIM_Application_ReturnID,16);
                        status_conductor->conductor->mode_L = 1;
                        strcpy(status_conductor->conductor->limit, work);
                    }

                    //printf("finding modes\n");
                    /*
                     if(prefix == 'n')
                     {
                     //DoMethod((Object*)status_conductor->conductor->BT_mode_N,MUIM_KillNotify,MUIA_Selected);
                     if(status_conductor->conductor==status_current->current_query)
                     { if(status_conductor->conductor->BT_mode_N) setmacro((Object*)status_conductor->conductor->BT_mode_N,MUIA_Selected, TRUE); }
                     //DoMethod((Object*)status_conductor->conductor->BT_mode_N,MUIM_Notify,MUIA_Selected,TRUE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,11);
                     status_conductor->conductor->mode_N=1;
                     }

                     if(prefix == 't')
                     {
                     //DoMethod((Object*)status_conductor->conductor->BT_mode_T,MUIM_KillNotify,MUIA_Selected);
                     if(status_conductor->conductor==status_current->current_query)
                     {
                     if(status_conductor->conductor->BT_mode_T) setmacro((Object*)status_conductor->conductor->BT_mode_T,MUIA_Selected, TRUE);
                     }
                     //DoMethod((Object*)status_conductor->conductor->BT_mode_T,MUIM_Notify,MUIA_Selected,TRUE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,10);
                     status_conductor->conductor->mode_T=1;
                     }

                     if(prefix == 's')
                     {
                     //DoMethod((Object*)status_conductor->conductor->BT_mode_S,MUIM_KillNotify,MUIA_Selected);
                     if(status_conductor->conductor==status_current->current_query)
                     { if(status_conductor->conductor->BT_mode_S) setmacro((Object*)status_conductor->conductor->BT_mode_S,MUIA_Selected, TRUE); }
                     //DoMethod((Object*)status_conductor->conductor->BT_mode_S,MUIM_Notify,MUIA_Selected,TRUE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,12);
                     //DoMethod((Object*)status_conductor->conductor->BT_mode_S,MUIM_Notify,MUIA_Selected,FALSE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,22);
                     status_conductor->conductor->mode_S=1;
                     }

                     if(prefix == 'i')
                     {
                     //DoMethod((Object*)status_conductor->conductor->BT_mode_I,MUIM_KillNotify,MUIA_Selected);
                     if(status_conductor->conductor==status_current->current_query)
                     {
                     if(status_conductor->conductor->BT_mode_I) setmacro((Object*)status_conductor->conductor->BT_mode_I,MUIA_Selected, TRUE);
                     }//DoMethod((Object*)status_conductor->conductor->BT_mode_I,MUIM_Notify,MUIA_Selected,TRUE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,13);
                     //DoMethod((Object*)status_conductor->conductor->BT_mode_I,MUIM_Notify,MUIA_Selected,FALSE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,23);
                     status_conductor->conductor->mode_I=1;
                     }

                     if(prefix == 'p')
                     {
                     //DoMethod((Object*)status_conductor->conductor->BT_mode_P,MUIM_KillNotify,MUIA_Selected);
                     if(status_conductor->conductor==status_current->current_query)
                     {
                     if(status_conductor->conductor->BT_mode_P) setmacro((Object*)status_conductor->conductor->BT_mode_P,MUIA_Selected, TRUE);
                     }//DoMethod((Object*)status_conductor->conductor->BT_mode_P,MUIM_Notify,MUIA_Selected,TRUE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,14);
                     //DoMethod((Object*)status_conductor->conductor->BT_mode_P,MUIM_Notify,MUIA_Selected,FALSE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,24);
                     status_conductor->conductor->mode_P=1;
                     }

                     if(prefix == 'm')
                     {
                     //DoMethod((Object*)status_conductor->conductor->BT_mode_M,MUIM_KillNotify,MUIA_Selected);
                     if(status_conductor->conductor==status_current->current_query)
                     {
                     if(status_conductor->conductor->BT_mode_M) setmacro((Object*)status_conductor->conductor->BT_mode_M,MUIA_Selected, TRUE);
                     }//DoMethod((Object*)status_conductor->conductor->BT_mode_M,MUIM_Notify,MUIA_Selected,TRUE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,15);
                     //DoMethod((Object*)status_conductor->conductor->BT_mode_M,MUIM_Notify,MUIA_Selected,FALSE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,25);
                     status_conductor->conductor->mode_M=1;
                     }

                     if(prefix == 'k')
                     {
                     //DoMethod((Object*)status_conductor->conductor->BT_mode_K,MUIM_KillNotify,MUIA_Selected);
                     //DoMethod((Object*)status_conductor->conductor->STR_keyword,MUIM_KillNotify,MUIA_String_Acknowledge);
                     if(status_conductor->conductor==status_current->current_query)
                     {if(status_conductor->conductor->BT_mode_K) setmacro((Object*)status_conductor->conductor->BT_mode_K,MUIA_Selected, TRUE);
                     setmacro((Object*)status_conductor->conductor->STR_keyword,MUIA_String_Contents, work);
                     }//DoMethod((Object*)status_conductor->conductor->BT_mode_K,MUIM_Notify,MUIA_Selected,TRUE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,17);
                     //DoMethod((Object*)status_conductor->conductor->BT_mode_K,MUIM_Notify,MUIA_Selected,FALSE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,28);
                     //DoMethod((Object*)status_conductor->conductor->STR_keyword,MUIM_Notify,MUIA_String_Acknowledge, MUIV_EveryTime, (Object*)WookieChat->App,2,MUIM_Application_ReturnID,17);
                     status_conductor->conductor->mode_K=1;
                     strcpy(status_conductor->conductor->keyword,work);

                     }

                     if(prefix == 'l')
                     {
                     //DoMethod((Object*)status_conductor->conductor->BT_mode_L,MUIM_KillNotify,MUIA_Selected);
                     //DoMethod((Object*)status_conductor->conductor->STR_limit,MUIM_KillNotify,MUIA_String_Acknowledge);
                     if(status_conductor->conductor==status_current->current_query)
                     { if(status_conductor->conductor->BT_mode_L) setmacro((Object*)status_conductor->conductor->BT_mode_L,MUIA_Selected, TRUE);
                     setmacro((Object*)status_conductor->conductor->STR_limit,MUIA_String_Contents, work);
                     }//DoMethod((Object*)status_conductor->conductor->BT_mode_L,MUIM_Notify,MUIA_Selected,TRUE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,16);
                     //DoMethod((Object*)status_conductor->conductor->BT_mode_L,MUIM_Notify,MUIA_Selected,FALSE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,27);
                     //DoMethod((Object*)status_conductor->conductor->STR_limit,MUIM_Notify,MUIA_String_Acknowledge, MUIV_EveryTime, (Object*)WookieChat->App,2,MUIM_Application_ReturnID,16);

                     status_conductor->conductor->mode_L=1;
                     strcpy(status_conductor->conductor->limit,work);
                     }                */

                    a++;

                }
            }

        }
        else if (!strcmp(incoming_2, "329")) // stuff after getting channel modes
        {
            if (status_conductor->conductor == status_current->current_query)
                setup_notifys();

        }
        else if (!strcmp(incoming_2, "376")) // END OF MOTD
        {

        }
        else if (!strcmp(incoming_2, "353")) // NAMES list
        {

            if (status_conductor->max_modes <= 0)
            {
                status_conductor->user_modes[1].mode[0] = 'o';
                status_conductor->user_modes[1].mode[1] = '\0';
                status_conductor->user_modes[1].symbol[0] = '@';
                status_conductor->user_modes[1].symbol[1] = '\0';

                status_conductor->user_modes[2].mode[0] = 'h';
                status_conductor->user_modes[2].mode[1] = '\0';
                status_conductor->user_modes[2].symbol[0] = '%';
                status_conductor->user_modes[2].symbol[1] = '\0';

                status_conductor->user_modes[3].mode[0] = 'v';
                status_conductor->user_modes[3].mode[1] = '\0';
                status_conductor->user_modes[3].symbol[0] = '+';
                status_conductor->user_modes[3].symbol[1] = '\0';

                status_conductor->max_modes = 3;
            }

            int a = 0, b = 0, result = 0;

            strcpy(string7, "");
            strcpy(string8, "");

            while (strcspn(incoming_3, status_conductor->chantypes) != (ULONG) a)
                a++;

            //while(incoming_3[a] != '#' && incoming_3[a] != '&') a++;

            while (incoming_3[a] != ' ')
            {
                string7[b] = incoming_3[a];
                a++;
                b++;
            }
            string7[b] = '\0';
            a = a + 2;
            b = 0;

            while (incoming_3[a] != '\0')
            {
                if (incoming_3[a] != '\r' && incoming_3[a] != '\n')
                    string8[b] = incoming_3[a];
                a++;
                b++;
            }

            string8[b] = '\0';

            for (status_conductor->conductor = status_conductor->root; status_conductor->conductor;
                    status_conductor->conductor = status_conductor->conductor->next)
            {
                if (!stricmp(string7, status_conductor->conductor->name))
                    break;
            }

            //we're not joined to the channel we're getting a names list from, so lets display it in the root tab
            if (!status_conductor->conductor)
            {
                status_conductor->conductor = status_conductor->root;

				sprintf(buffer3, "%s%sNames%s %s %s:", timestamp,
						LGS( MSG_OPENING_BRACKET ),
						LGS( MSG_CLOSING_BRACKET ),
						LGS( MSG_USERS_IN_CHANNEL ),
						string7 );
                add_text_to_current_list(buffer3, 9, ACTIVITY);

				sprintf(buffer3, "%s%sNames%s %s:", timestamp,
						LGS( MSG_OPENING_BRACKET ),
						LGS( MSG_CLOSING_BRACKET ),
						string8 );
                add_text_to_current_list(buffer3, 9, ACTIVITY);

            }
            else //we're in the channel we're getting the list from, so lets update the nicklist structure with their names
            {

				sprintf(buffer3, "%s%sNames%s %s %s:", timestamp,
						LGS( MSG_OPENING_BRACKET ),
						LGS( MSG_CLOSING_BRACKET ),
						LGS( MSG_USERS_IN_CHANNEL ),
						string7 );
                add_text_to_conductor_list(buffer3, 9, ACTIVITY);

				sprintf(buffer3, "%s%sNames%s %s:", timestamp,
						LGS( MSG_OPENING_BRACKET ),
						LGS( MSG_CLOSING_BRACKET ),
						string8 );
                add_text_to_conductor_list(buffer3, 9, ACTIVITY);

                DoMethod((Object*) status_conductor->conductor->LV_channel, MUIM_NList_Jump, MUIV_NList_Jump_Bottom);

                if (stricmp(status_conductor->conductor->name, string7))
                {
                    status_conductor->conductor = status_conductor->root;
                    while (stricmp(status_conductor->conductor->name, string7))
                    {
                        status_conductor->conductor = status_conductor->conductor->next;
                        if (!status_conductor->conductor)
                            break;
                    }
                }

                work = strtok(string8, " ");

                if (work)
                {
                    result = 0;
                    for (int count = 1; count <= status_conductor->max_modes; count++)
                    {
                        if (work[0] == status_conductor->user_modes[count].symbol[0])
                            result = 1;

                    }
                    if (result)
                    {
                        mode_work[0] = work[0];
                        mode_work[1] = '\0';
                        a = 0;

                        while (work[a + 1])
                        {
                            work[a] = work[a + 1];
                            a++;
                        }

                        work[a] = '\0';

                    }
                    else // to try and fix the crash on /op bug
                    {
                        mode_work[0] = ' ';
                        mode_work[1] = '\0';
                    }
                }

                int count;
                if (status_conductor->conductor)
                {
                    if (status_conductor->conductor->nicks)
                        count = status_conductor->conductor->nicks - 1;
                    else
                        count = -1;
                }

                while (work)
                {

                    mode_work[1] = '\0';

                    if (status_conductor->conductor)
                    {
                        strcpy(status_conductor->conductor->nicklist[++count].modes, mode_work);
                        strcpy(status_conductor->conductor->nicklist[count].name, work);

                        status_conductor->conductor->nicks++;

                        if (!strcmp(status_conductor->nick, status_conductor->conductor->nicklist[count].name))
                        {
                            status_conductor->conductor->your_current_mode[0] = mode_work[0];
                            status_conductor->conductor->your_current_mode[1] = '\0';
                            //printf("shitty\n");

                        }

                    }

                    work = strtok(NULL, " ");
                    if (!work)
                        break;

                    result = 0;

                    for (int count4 = 1; count4 <= status_conductor->max_modes; count4++)
                    {
                        if (work[0] == status_conductor->user_modes[count4].symbol[0])
                            result = 1;

                    }

                    if (result == 1)
                    {
                        mode_work[0] = work[0];
                        mode_work[1] = '\0';
                        a = 0;

                        while (work[a + 1])
                        {
                            work[a] = work[a + 1];
                            a++;
                        }

                        work[a] = '\0';
                        /*
                         if(!strcmp(status_conductor->nick,status_conductor->conductor->nicklist[count].name))
                         {
                         status_conductor->conductor->your_current_mode[0]=mode_work[0];
                         status_conductor->conductor->your_current_mode[1]='\0';
                         printf("shit\n");

                         }     */

                    }
                    else // to try and fix the crash on /op bug
                    {
                        mode_work[0] = ' ';
                        mode_work[1] = '\0';

                        /*if(!strcmp(status_conductor->nick,status_conductor->conductor->nicklist[count].name))
                         {
                         printf("fuck\n");

                         status_conductor->conductor->your_current_mode[0]=mode_work[0];
                         status_conductor->conductor->your_current_mode[1]='\0';
                         } */

                    }

                }
            }

        }
        else if (!strcmp(incoming_2, "332")) // Topic
        {
            //string7=nick
            //string8=channel name
            //string9=topic

            int a = 0, b = 0;
            while (incoming_3[b] != ' ')
            {
                string7[a] = incoming_3[b];
                a++;
                b++;
            }
            string7[a] = '\0';
            b++;
            a = 0;

            while (incoming_3[b] != ' ')
            {
                string8[a] = incoming_3[b];
                a++;
                b++;
            }
            string8[a] = '\0';
            b = b + 2;
            a = 0;

            while (incoming_3[b] != '\0')
            {
                string9[a] = incoming_3[b];
                a++;
                b++;
            }
            string9[a] = '\0';

            status_conductor->conductor = status_conductor->root;
            while (stricmp(status_conductor->conductor->name, string8))
            {
                status_conductor->conductor = status_conductor->conductor->next;
                if (!status_conductor->conductor)
                    break;
            }

			sprintf(buffer3, "%s%sTopic%s %s", timestamp,
						LGS( MSG_OPENING_BRACKET ),
						LGS( MSG_CLOSING_BRACKET ),
						string9 );

            if (status_conductor->conductor)
            {
                DoMethod((Object*) status_conductor->conductor->STR_topic, MUIM_KillNotify, MUIA_String_Acknowledge);

                setmacro((Object*) status_conductor->conductor->STR_topic, MUIA_String_Contents, string9);
                setmacro((Object*) status_conductor->conductor->STR_topic, MUIA_String_BufferPos, 0);

                DoMethod((Object*) status_conductor->conductor->STR_topic, MUIM_Notify, MUIA_String_Acknowledge,
                        MUIV_EveryTime, (Object*) WookieChat->App, 2, MUIM_Application_ReturnID, 29);

                add_text_to_conductor_list((char*) buffer3, 16, ACTIVITY);

                strcpy(status_conductor->conductor->topic, string9);
                setmacro((Object*) status_conductor->conductor->STR_topic, MUIA_ShortHelp,
                        &status_conductor->conductor->topic);

            }

        }
        else if (!strcmp(incoming_2, "333")) // Topic set by
        {
            //string7=nick
            //string8=channel name
            //string9=topic changed by this nick
            //string10=time changed

            int a = 0, b = 0;
            while (incoming_3[b] != ' ')
            {
                string7[a] = incoming_3[b];
                a++;
                b++;
            }
            string7[a] = '\0';
            b++;
            a = 0;

            while (incoming_3[b] != ' ')
            {
                string8[a] = incoming_3[b];
                a++;
                b++;
            }
            string8[a] = '\0';
            b++;
            a = 0;

            while (incoming_3[b] != ' ')
            {
                string9[a] = incoming_3[b];
                a++;
                b++;
            }
            string9[a] = '\0';
            b++;
            a = 0;

            while (incoming_3[b] != '\0')
            {
                string10[a] = incoming_3[b];
                a++;
                b++;
            }
            string10[a] = '\0';

            time_t rawtime = atoi(string10);
            struct tm *timeinfo;
            timeinfo = localtime(&rawtime);

			sprintf(buffer3, "%s%sTopic%s %s %s %s %s", timestamp,
					LGS( MSG_OPENING_BRACKET ),
					LGS( MSG_CLOSING_BRACKET ),
					LGS( MSG_TOPIC_SET_BY ),
					string9,
					LGS( MSG_TOPIC_DATE ),
					asctime(timeinfo));

            buffer3[strlen(buffer3) - 1] = '\0';

            status_conductor->conductor = status_conductor->root;
            while (stricmp(status_conductor->conductor->name, string8))
            {
                status_conductor->conductor = status_conductor->conductor->next;
                if (!status_conductor->conductor)
                    break;
            }

            if (status_conductor->conductor)
                add_text_to_conductor_list((char*) buffer3, 16, ACTIVITY);
            else
                add_text_to_current_list((char*) buffer3, 16, ACTIVITY);

        }
        else if (!strcmp(incoming_2, "366")) // End of names list message
        {

            sort_nicks(1);

            //setmacro((Object*)status_conductor->conductor->LV_nicklist,MUIA_NList_Quiet, TRUE);
            DisplayNicks();
            //setmacro((Object*)status_conductor->conductor->LV_nicklist,MUIA_NList_Quiet, FALSE);
            change_window_titlebar_text();
            update_TX_nicklist();
            update_your_nickname_text_label();

        }
        else if (!strcmp(incoming_2, "433") && incoming_3[0] == '*') // Nick in use msg when connecting to server
        {
            char newnick[100];

            if (!stricmp(status_conductor->nick, status_conductor->initial_nick)
                    && status_conductor->nick2[0] != '\0')
            {
                strcpy(newnick, status_conductor->nick2);
            }

            else if (!stricmp(status_conductor->nick, status_conductor->nick2)
                    && status_conductor->nick3[0] != '\0')
            {
                strcpy(newnick, status_conductor->nick3);
            }

            else if (!stricmp(status_conductor->nick, status_conductor->nick3))
            {
                strcpy(newnick, "^");
                strcat(newnick, status_conductor->nick);

            }
            else
            {
                strcpy(newnick, "^");
                strcat(newnick, status_conductor->nick);

            }

			sprintf(buffer3, "%s%sNick%s Nick %s %s %s", timestamp,
					LGS( MSG_OPENING_BRACKET ),
					LGS( MSG_CLOSING_BRACKET ),
					status_conductor->nick,
					LGS( MSG_NICK_IN_USE_RETRYING_WITH ),
					newnick);

            add_text_to_current_list((char*) buffer3, 9, ACTIVITY);

            strcpy(status_conductor->nick, newnick);

            sprintf(sendstuff, "NICK %s\r\n", newnick); //status_conductor->nick);

            send_text(sendstuff);

            change_window_titlebar_text();
            update_your_nickname_text_label();

        }
        else if (!strcmp(incoming_2, "JOIN"))
        {
            int a = 1, b = 0;
            //string7=nick
            //string8=hostname
            //string9=channelname

            while (incoming_1[a] != '!')
            {
                string7[b] = incoming_1[a];
                a++;
                b++;
            }
            string7[b] = '\0';
            b = 0;
            a++;

            while (incoming_1[a] != '\0')
            {
                string8[b] = incoming_1[a];
                a++;
                b++;
            }
            string8[b] = '\0';
            a = 0;
            b = 0;

            if (incoming_3[b] == ':')
                b = 1;

            while (incoming_3[a] != '\0' && incoming_3[a] != ' ')
            {
                string9[a] = incoming_3[b];
                a++;
                b++;
            }
            string9[a] = '\0';

			sprintf(buffer3, "%s%s%s%s %s (%s)", timestamp,
					LGS( MSG_OPENING_BRACKET ),
					LGS( MSG_JOIN ),
					LGS( MSG_CLOSING_BRACKET ),
					string7, string8);

            //have we opened any channels yet? if not, lets use the root tab
            if (my_settings.no_server_tabs || (!my_settings.no_server_tabs && status_conductor->root->server_tab == 0))
            {
                for (status_conductor->conductor = status_conductor->root; status_conductor->conductor;
                        status_conductor->conductor = status_conductor->conductor->next)
                {
                    if (strcspn(status_conductor->conductor->name, status_conductor->chantypes) == 0)
                        break;
                    if (status_conductor->conductor->name[0] == '#')
                        break;
                    if (status_conductor->conductor->name[0] == '!')
                        break;

                }

            }

            BOOL using_root_tab_with_disabled_server_tabs = FALSE;

            if (!status_conductor->conductor)
            {
                //join the channel in the server tab node

                if (!status_conductor->conductor)
                {
                    using_root_tab_with_disabled_server_tabs = TRUE;

                    for (status_conductor->conductor = status_conductor->root; status_conductor->conductor;
                            status_conductor->conductor = status_conductor->conductor->next)
                    {
                        //if(status_conductor->conductor->BT_querybutton && status_conductor->conductor->LV_nicklist && status_conductor->conductor->name[0]!='#') break;
                        if (status_conductor->conductor->BT_querybutton && status_conductor->conductor->LV_nicklist
                                && strcspn(status_conductor->conductor->name, status_conductor->chantypes) != 0)
                            break;

                    }

                }

                if (!status_conductor->conductor)
                    status_conductor->conductor = status_conductor->root;

                if (status_conductor->conductor->BT_querybutton)
                {

                    strcpy(status_conductor->conductor->name, string9);
                    strcpy(status_conductor->conductor->displayed_name, string9);

                    strcpy(status_conductor->conductor->nlist_tab.entry, "         :");
                    status_conductor->conductor->nlist_tab.colour = 23;
                    strcat(status_conductor->conductor->nlist_tab.entry, string9);

                    DoMethod((Object*) WookieChat->LV_tabs, MUIM_NList_ReplaceSingle,
                            &status_conductor->conductor->nlist_tab, status_conductor->conductor->nlist_tab_number, 0,
                            0);

                    if (!SMALLTABS && my_settings.use_nlistobject_for_tabs == 0)
                    {
                        if (strlen(status_conductor->conductor->displayed_name) > 11)
                        {
                            status_conductor->conductor->displayed_name[10] = '.';
                            status_conductor->conductor->displayed_name[11] = '.';
                            status_conductor->conductor->displayed_name[12] = '\0';
                        }
                    }

                    status_conductor->conductor->activity = 0;

                    sprintf(string7, "\033I[2:00000000,00000000,00000000] %s",
                            status_conductor->conductor->displayed_name);
                    if (status_conductor->conductor->BT_querybutton)
                    {
                        setmacro((Object*) status_conductor->conductor->BT_querybutton, MUIA_Text_Contents, string7);
                    }

                    sprintf(sendstuff, "mode %s\r\n", string9);
                    send_text(sendstuff);

                    if (my_settings.clone_detection)
                    {
                        sprintf(sendstuff, "who %s\r\n", string9);
                        send_text(sendstuff);
                    }

                    //display the last few lines of logfile
                    display_last_few_lines_of_logfile_conductor();

                    // set the logfile to zero so we know its not open yet
                    status_conductor->conductor->log_file = 0;

                    if (my_settings.show_joins_parts)
                        add_text_to_conductor_list((char*) buffer3, 2, ACTIVITY);

                    DoMethod((Object*) status_conductor->conductor->LV_channel, MUIM_NList_Jump,
                            MUIV_NList_Jump_Bottom);

                    DoMethod((Object*) status_conductor->conductor->LV_nicklist, MUIM_NList_Clear);
                    status_conductor->conductor->nicks = 0;

                    if (status_conductor == status_current)
                    {
                        if (DoMethod((Object*) GROUP_ROOT_0, MUIM_Group_InitChange))
                        {
                            if (DoMethod((Object*) GR_top, MUIM_Group_InitChange))
                            {

                                activate_tab_button();

                                DoMethod((Object*) GR_top, MUIM_Group_ExitChange);

                            }

                            DoMethod((Object*) GROUP_ROOT_0, MUIM_Group_ExitChange);

                        }
                    }
                    /*if(my_settings.use_nlistobject_for_tabs)
                     {

                     setmacro((Object*)status_conductor->conductor->GR_nicklist_and_tabs, MUIA_ShowMe, TRUE);


                     }
                     else
                     {
                     //if(status_conductor->conductor->name[0]!='#' && status_conductor->conductor->name[0]!='&')
                     if(strcspn(status_conductor->conductor->name,status_conductor->chantypes)!=0)
                     setmacro((Object*)status_conductor->conductor->GR_nicklist_and_tabs, MUIA_ShowMe, FALSE);
                     else
                     setmacro((Object*)status_conductor->conductor->GR_nicklist_and_tabs, MUIA_ShowMe, TRUE);

                     } */

                    //end
                }
                else
                {

                    if ((!my_settings.no_server_tabs && status_conductor->root->server_tab == 0)
                            || my_settings.no_server_tabs)
                    {
                        status_conductor->conductor = status_conductor->root;
                    }
                    else
                    {
                        status_conductor->conductor = status_conductor->root->next;
                    }

                    while (stricmp(status_conductor->conductor->name, string9))
                    {
                        status_conductor->conductor = status_conductor->conductor->next;
                        if (!status_conductor->conductor)
                            break;
                    }

                    if (!stricmp(status_conductor->nick, string7))
                    {
                        if (!status_conductor->conductor)
                        {
                            create_new_tab(string9, 1, 1);

                            //strcpy(status_conductor->str, pch+2);
                            //return;

                            sprintf(sendstuff, "mode %s\r\n", string9);
                            send_text(sendstuff);

                            if (my_settings.clone_detection)
                            {
                                sprintf(sendstuff, "who %s\r\n", string9);
                                send_text(sendstuff);
                            }
                            //2008-07-21 experimental

                        }
                        else
							sprintf(buffer3, "%s%s%s%s %s %s..", timestamp,
									LGS( MSG_OPENING_BRACKET ),
									LGS( MSG_JOIN ),
									LGS( MSG_CLOSING_BRACKET ),
									LGS( MSG_AUTO_REJOINING ),
									string9);

                        add_text_to_conductor_list((char*) buffer3, 2, ACTIVITY);

                        DoMethod((Object*) status_conductor->conductor->LV_nicklist, MUIM_NList_Clear);
                        status_conductor->conductor->nicks = 0;

                        status_conductor->conductor->your_current_mode[0] = ' ';
                        status_conductor->conductor->your_current_mode[1] = '\0';
                        update_your_nickname_text_label();

                    }

                }

            }
            else
            {

                status_conductor->conductor = status_conductor->root;
                while (stricmp(status_conductor->conductor->name, string9))
                {
                    status_conductor->conductor = status_conductor->conductor->next;
                    if (!status_conductor->conductor)
                        break;
                }

                if (!stricmp(status_conductor->nick, string7))
                {
                    if (!status_conductor->conductor)
                    {
                        create_new_tab(string9, 1, 1);

                        //experimental 2008-07-21
                        //strcpy(status_conductor->str, pch+2);
                        //return;

                        sprintf(sendstuff, "mode %s\r\n", string9);
                        send_text(sendstuff);

                        if (my_settings.clone_detection)
                        {
                            sprintf(sendstuff, "who %s\r\n", string9);
                            send_text(sendstuff);
                        }

                    }
                    else
						sprintf(buffer3, "%s%s%s%s %s %s..", timestamp,
							LGS( MSG_OPENING_BRACKET ),
							LGS( MSG_JOIN ),
							LGS( MSG_CLOSING_BRACKET ),
							LGS( MSG_AUTO_REJOINING ),
							string9);

                    add_text_to_conductor_list((char*) buffer3, 2, ACTIVITY);

                    if (status_conductor->conductor)
                    {
                        if (status_conductor->conductor->LV_nicklist)
                        {
                            if (strcspn(status_conductor->conductor->name, status_conductor->chantypes) == 0)
                            {
                                DoMethod((Object*) status_conductor->conductor->LV_nicklist, MUIM_NList_Clear);
                                status_conductor->conductor->nicks = 0;
                            }
                        }
                    }

                }
                else
                {

                    if (status_conductor->conductor)
                    {
                        if (my_settings.show_joins_parts)
                            add_text_to_conductor_list((char*) buffer3, 2, ACTIVITY);

                        if ((my_settings.events[JOIN].use_when == 1 && is_window_active())
                                || (my_settings.events[JOIN].use_when == 2 && !is_window_active())
                                || my_settings.events[JOIN].use_when == 3)
                        {
                            strcpy(target_nick, string7);
                            create_arexx_event_string(my_settings.events[JOIN].arexx_script,
                                    my_settings.events[JOIN].arexx_script2, buffer3);
                            Execute((_s_cs)event_string, 0, 0);
                        }

                        if (my_settings.clone_detection)
                        {
                            onjoin_clone_detection(string7, string8);
                        }

                        strcpy(new_entry2.modes, "");
                        strcpy(new_entry2.name, string7);
                        if (!new_entry2.hostname)
                            new_entry2.hostname = malloc(sizeof(char) * HOSTNAME_STRING_SIZE);
                        strcpy(new_entry2.hostname, string8);

                        strcpy(status_conductor->conductor->nicklist[status_conductor->conductor->nicks].name,
                                new_entry2.name);
                        strcpy(status_conductor->conductor->nicklist[status_conductor->conductor->nicks].modes, "");

                        if (!status_conductor->conductor->nicklist[status_conductor->conductor->nicks].hostname)
                            status_conductor->conductor->nicklist[status_conductor->conductor->nicks].hostname =
                                    malloc(sizeof(char) * HOSTNAME_STRING_SIZE);

                        strcpy(status_conductor->conductor->nicklist[status_conductor->conductor->nicks].hostname,
                                new_entry2.hostname);

                        status_conductor->conductor->nicks++;

                        sort_nicks(1);

                        setmacro((Object*) status_conductor->conductor->LV_nicklist, MUIA_NList_Quiet, TRUE);

                        AddNick(string7, string8);
                        update_TX_nicklist();

                        setmacro((Object*) status_conductor->conductor->LV_nicklist, MUIA_NList_Quiet, FALSE);

                        //lets put the joiners nick back in his private tabs' nicklist struct, if we're currently
                        //talking to him in private
                        status_conductor->conductor = status_conductor->root;
                        while (stricmp(status_conductor->conductor->name, string7))
                        {
                            status_conductor->conductor = status_conductor->conductor->next;
                            if (!status_conductor->conductor)
                                break;
                        }
                        if (status_conductor->conductor && status_conductor->conductor->nicks == 0)
                        {
                            strcpy(status_conductor->conductor->nicklist[0].name, string7);

                            if (!status_conductor->conductor->nicklist[status_conductor->conductor->nicks].hostname)
                                status_conductor->conductor->nicklist[status_conductor->conductor->nicks].hostname =
                                        malloc(sizeof(char) * HOSTNAME_STRING_SIZE);
                            strcpy(status_conductor->conductor->nicklist[0].hostname, string8);

                            status_conductor->conductor->nicks = 1;

							sprintf(buffer3, "%s%s%s%s %s %s", timestamp,
									LGS( MSG_OPENING_BRACKET ),
									LGS( MSG_JOIN ),
									LGS( MSG_CLOSING_BRACKET ),
									string7,
									LGS( MSG_USER_HAS_RETURNED_TO_IRC ) );
                            add_text_to_conductor_list((char*) buffer3, 2, ACTIVITY);
                        }
                    }
                }
            }
        }
        else if (!strcmp(incoming_2, "KICK"))
        {
            //string1=nick
            //string8=hostname
            //string3=channelname
            //string4=nick being kicked
            //string5=kick reason

            int a = 1, b = 0;

            while (incoming_1[a] != '!' && incoming_1[a] != ' ')
            {
                string7[b] = incoming_1[a];
                a++;
                b++;
            }
            string7[b] = '\0';
            b = 0;
            a++;

            while (incoming_1[a] != '\0')
            {
                string8[b] = incoming_1[a];
                a++;
                b++;
            }
            string8[b] = '\0';
            a = 0, b = 0;

            string3 = strtok(incoming_3, " ");
            string4 = strtok(NULL, " ");
            string5 = strtok(NULL, "");

            if (string3 && string4)
            {

                if (string5)
                {
                    string5[a] = '(';
					sprintf(buffer3, "%s%sKick%s %s %s %s %s)", timestamp,
							LGS( MSG_OPENING_BRACKET ),
							LGS( MSG_CLOSING_BRACKET ),
							string4,
							LGS( MSG_USER_WAS_KICKED_BY ),
							string7,
							string5);
                }
                else
                {
					sprintf(buffer3, "%s%sKick%s %s %s %s", timestamp,
							LGS( MSG_OPENING_BRACKET ),
							LGS( MSG_CLOSING_BRACKET ),
							string4,
							LGS( MSG_USER_WAS_KICKED_BY ),
                            string7);

                }

                status_conductor->conductor = status_conductor->root;
                while (stricmp(status_conductor->conductor->name, string3))
                {
                    status_conductor->conductor = status_conductor->conductor->next;
                    if (!status_conductor->conductor)
                        break;
                }
                if (status_conductor->conductor)
                {

                    if (!stricmp(string4, status_conductor->nick))
                    {
						sprintf(buffer3, "%s%sKick%s %s %s %s)", timestamp,
								LGS( MSG_OPENING_BRACKET ),
								LGS( MSG_CLOSING_BRACKET ),
								LGS( MSG_YOU_WERE_KICKED_BY ),
                                string7, string5);

                        add_text_to_conductor_list((char*) buffer3, 14, ACTIVITY);

                        if (my_settings.auto_rejoin_on_kick)
                        {
                            //sprintf(sendstuff,"JOIN %s\r\n",string3);
                            if (!stricmp(status_conductor->conductor->keyword, ""))
                                sprintf(sendstuff, "JOIN %s\r\n", status_conductor->conductor->name);
                            else
                                sprintf(sendstuff, "JOIN %s %s\r\n", status_conductor->conductor->name,
                                        status_conductor->conductor->keyword);

                            DoMethod((Object*) status_conductor->conductor->LV_nicklist, MUIM_NList_Clear);
                            status_conductor->conductor->nicks = 0;
                            send_text(sendstuff);
                        }
                    }
                    else
                    {
                        add_text_to_conductor_list((char*) buffer3, 14, ACTIVITY);
                        RemoveNick(string4);

                    }

                    if ((my_settings.events[KICK].use_when == 1 && is_window_active())
                            || (my_settings.events[KICK].use_when == 2 && !is_window_active())
                            || my_settings.events[KICK].use_when == 3)
                    {

                        if (string5)
                        {
                            string5[a] = '(';
							sprintf(buffer3, "%sKick%s %s %s %s %s)",
									LGS( MSG_OPENING_BRACKET ),
									LGS( MSG_CLOSING_BRACKET ),
									string4,
									LGS( MSG_USER_WAS_KICKED_BY ),
									string7, string5);
                        }
                        else
							sprintf(buffer3, "%sKick%s %s %s %s",
									LGS( MSG_OPENING_BRACKET ),
									LGS( MSG_CLOSING_BRACKET ),
									string4,
									LGS( MSG_USER_WAS_KICKED_BY ),
									string7);

                        if (!stricmp(string4, status_conductor->nick))
							sprintf(buffer3, "%sKick%s %s %s %s)",
									LGS( MSG_OPENING_BRACKET ),
									LGS( MSG_CLOSING_BRACKET ),
									LGS( MSG_YOU_WERE_KICKED_BY ),
                                    string7, string5);

                        strcpy(target_nick, string7);
                        create_arexx_event_string(my_settings.events[KICK].arexx_script,
                                my_settings.events[KICK].arexx_script2, buffer3);
                        Execute((_s_cs)event_string, 0, 0);
                    }

                }

            }

        }
        else if (!strcmp(incoming_2, "NOTICE"))
        {

            //string7 =nick
            //string8 =hostname
            //string9 =sender
            //string10 =notice text

            int a = 1, b = 0;
            colour = 12;

            while (incoming_1[a] != '!' && incoming_1[a] != ' ')
            {
                string7[b] = incoming_1[a];
                a++;
                b++;
            }
            string7[b] = '\0';
            b = 0;
            a++;

            while (incoming_1[a] != '\0')
            {
                string8[b] = incoming_1[a];
                a++;
                b++;
            }
            string8[b] = '\0';
            a = 0;
            b = 0;

            while (incoming_3[a] != ' ')
            {
                string9[b] = incoming_3[a];
                b++;
                a++;
            }
            string9[b] = '\0';
            a = a + 2;
            b = 0;

            while (incoming_3[a] != '\0')
            {
                string10[b] = incoming_3[a];
                a++;
                b++;
            }
            string10[b] = '\0';

            //TEST TO SEE IF WE'RE IGNORING THIS PERSON IN ANY YWAY
            user_being_ignored(string9, string8, PRIVMSG);

            if (!ignore_privmsg)
            {

                sprintf(buffer3, "%s-%s- %s", timestamp, string7, string10);
                if (!stricmp(status_conductor->conductor->name, string9))
                    sprintf(buffer3, "%s-%s:%s- %s", timestamp, string9, string7, string10);

                //update_nicks_hostname(string1, string8);

                if (strstr(string10, "\001PING") && string10[strlen(string10) - 1] == '\001')
                {

                    orig_time_mins[0] = string10[6];
                    orig_time_mins[1] = string10[7];
                    orig_time_mins[2] = '\0';

                    orig_time_secs[0] = string10[8];
                    orig_time_secs[1] = string10[9];
                    orig_time_secs[2] = '\0';

                    ping_time();

                    a = atoi(orig_time_mins);
                    b = atoi(orig_time_secs);

                    int aa = atoi(pingtimestamp_mins);
                    int bb = atoi(pingtimestamp_secs);

                    a = a * 60; //turn orig mins into secs
                    aa = aa * 60; //turn current mins into secs
                    a = a + b; //original time, mins plus secs
                    aa = aa + bb; //current time, mins plus secs

                    a = aa - a; //current time minus original time

                    if (a == 1)
                        sprintf(buffer3, "%s%sPing%s PING Reply %s %s, %i second", timestamp,
								LGS( MSG_OPENING_BRACKET ),
								LGS( MSG_CLOSING_BRACKET ),
								LGS( MSG_FROM ),
								string7, a);
                    else
                        sprintf(buffer3, "%s%sPing%s PING Reply %s %s, %i seconds", timestamp,
								LGS( MSG_OPENING_BRACKET ),
								LGS( MSG_CLOSING_BRACKET ),
								LGS( MSG_FROM ),
								string7, a);
                    colour = 6;

                }
                else if ((strstr(string10, "\001VERSION") || strstr(string10, "\001version"))
                        && string10[strlen(string10) - 1] == '\001')
                {
                    strtok(string10, " ");
                    string5 = strtok(NULL, "");

                    if (string5)
                    {
                        string5[strlen(string5) - 1] = '\0';
                        sprintf(buffer3, "%s%sVersion%s %s VERSION Reply: %s", timestamp,
								LGS( MSG_OPENING_BRACKET ),
								LGS( MSG_CLOSING_BRACKET ),
								string7,
								string5);
                        colour = 6;
                    }

                }
                else if ((strstr(string10, "\001TIME") || strstr(string10, "\001time"))
                        && string10[strlen(string10) - 1] == '\001')
                {
                    strtok(string10, " ");
                    string5 = strtok(NULL, "");

                    if (string5)
                    {
                        string5[strlen(string5) - 1] = '\0';
						sprintf(buffer3, "%s%sTime%s %s TIME Reply: %s", timestamp,
								LGS( MSG_OPENING_BRACKET ),
								LGS( MSG_CLOSING_BRACKET ),
								string7,
								string5);
                        colour = 6;
                    }
                }

                if (strcspn(string9, status_conductor->chantypes) == 0)
                {
                    for (status_conductor->conductor = status_conductor->root; status_conductor->conductor;
                            status_conductor->conductor = status_conductor->conductor->next)
                    {

                        if (!stricmp(status_conductor->conductor->name, string9))
                        {
                            add_text_to_conductor_list((char*) buffer3, colour, ACTIVITY_CHAT);
                            break;
                        }
                    }

                    if (!status_conductor->conductor)
                        add_text_to_current_list((char*) buffer3, colour, ACTIVITY_CHAT);
                }
                else
                {
                    if (status_conductor != status_current)
                    {
                        status_conductor->conductor = status_conductor->root;
                        add_text_to_conductor_list((char*) buffer3, colour, ACTIVITY_CHAT);
                    }
                    else
                        add_text_to_current_list((char*) buffer3, colour, ACTIVITY_CHAT);
                }

                if ((my_settings.events[NOTICE].use_when == 1 && is_window_active())
                        || (my_settings.events[NOTICE].use_when == 2 && !is_window_active())
                        || my_settings.events[NOTICE].use_when == 3)
                {
                    strcpy(buffer3, buffer3 + strlen(timestamp));
                    strcpy(target_nick, string7);
                    create_arexx_event_string(my_settings.events[NOTICE].arexx_script,
                            my_settings.events[NOTICE].arexx_script2, buffer3);
                    Execute((_s_cs)event_string, 0, 0);
                }

            }

        }
        else if (!strcmp(incoming_2, "MODE"))
        {

            //string1=nick
            //string2=hostname
            //string3=channelname
            //string4=mode info
            //string5=possible list of nicks to edit

            string1 = strtok(incoming_1, "!");
            string1[0] = ' ';
            string2 = strtok(NULL, "\0");
            string3 = strtok(incoming_3, " ");
            string4 = strtok(NULL, " ");
            string5 = strtok(NULL, "");

            if ((my_settings.events[MODE_CHANGE].use_when == 1 && is_window_active())
                    || (my_settings.events[MODE_CHANGE].use_when == 2 && !is_window_active())
                    || my_settings.events[MODE_CHANGE].use_when == 3)
            {

                if (string5)
					sprintf(buffer3, "%sMode%s%s %s: %s %s",
							LGS( MSG_OPENING_BRACKET ),
							LGS( MSG_CLOSING_BRACKET ),
							string1,
							LGS( MSG_SETS_MODE ),
							string4,
							string5);
                else
					sprintf(buffer3, "%sMode%s%s %s: %s",
							LGS( MSG_OPENING_BRACKET ),
							LGS( MSG_CLOSING_BRACKET ),
							string1,
							LGS( MSG_SETS_MODE ),
                            string4);

                strcpy(target_nick, string1);
                create_arexx_event_string(my_settings.events[MODE_CHANGE].arexx_script,
                        my_settings.events[MODE_CHANGE].arexx_script2, buffer3);
                Execute((_s_cs)event_string, 0, 0);
            }

            if (string5)
				sprintf(buffer3, "%s%sMode%s%s %s: %s %s", timestamp,
						LGS( MSG_OPENING_BRACKET ),
						LGS( MSG_CLOSING_BRACKET ),
						string1,
						LGS( MSG_SETS_MODE ),
						string4,
                        string5);
            else
				sprintf(buffer3, "%s%sMode%s%s %s: %s", timestamp,
						LGS( MSG_OPENING_BRACKET ),
						LGS( MSG_CLOSING_BRACKET ),
						string1,
						LGS( MSG_SETS_MODE ),
						string4);

            //if(DEBUG) printf("changing modes..\n");

            //update_nicks_hostname(string1, string2);

            if (string1 && string3 && string4)
            //if(string1 && string2 && string3 && string4)
            {

                int a = 0;
                status_conductor->conductor = status_conductor->root;

                while (stricmp(status_conductor->conductor->name, string3))
                {
                    status_conductor->conductor = status_conductor->conductor->next;
                    if (!status_conductor->conductor)
                        break;
                }

                char *work;
                char prefix;

                if (string5)
                {
                    work = strtok(string5, " ");
                }
                a = 0;
                int option = 0;

                if (status_conductor->conductor) // && string5)
                {
                    while (string4[a] != '\0')
                    {
                        prefix = string4[a];

                        if (string4[a] == '+' || option == 1)
                        {
                            if (string4[a] == '+')
                                a++;
                            if (string4[a + 1] != '-')
                                option = 1;
                            else
                                option = 0;

                            for (int count = 1; count <= status_conductor->max_modes; count++)
                            {
                                if (string4[a] == status_conductor->user_modes[count].mode[0])
                                {
                                    ReplaceNicksWithVoiceOps(work, string4[a], 1);
                                    break;
                                }
                            }

                            if (string4[a] == 'n')
                            {
                                //DoMethod((Object*)status_conductor->conductor->BT_mode_N,MUIM_KillNotify,MUIA_Selected);
                                if (status_conductor->conductor == status_current->current_query)
                                {
                                    DoMethod((Object*) status_conductor->conductor->BT_mode_N, MUIM_KillNotify,
                                            MUIA_Selected);

                                    setmacro((Object*) status_conductor->conductor->BT_mode_N, MUIA_Selected, TRUE);

                                    DoMethod((Object*) status_conductor->conductor->BT_mode_N, MUIM_Notify,
                                            MUIA_Selected, TRUE, (Object*) WookieChat->App, 2,
                                            MUIM_Application_ReturnID, 11);
                                    DoMethod((Object*) status_conductor->conductor->BT_mode_N, MUIM_Notify,
                                            MUIA_Selected, FALSE, (Object*) WookieChat->App, 2,
                                            MUIM_Application_ReturnID, 21);

                                } //DoMethod((Object*)status_conductor->conductor->BT_mode_N,MUIM_Notify,MUIA_Selected,TRUE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,11);
                                  //DoMethod((Object*)status_conductor->conductor->BT_mode_N,MUIM_Notify,MUIA_Selected,FALSE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,21);
                                status_conductor->conductor->mode_N = 1;

                            }

                            if (string4[a] == 't')
                            {
                                if (status_conductor->conductor == status_current->current_query)
                                {
                                    //DoMethod((Object*)status_conductor->conductor->BT_mode_N,MUIM_KillNotify,MUIA_Selected);
                                    DoMethod((Object*) status_conductor->conductor->BT_mode_T, MUIM_KillNotify,
                                            MUIA_Selected);
                                    setmacro((Object*) status_conductor->conductor->BT_mode_T, MUIA_Selected, TRUE);

                                    if (status_current->current_query->BT_mode_T)
                                        DoMethod((Object*) status_current->current_query->BT_mode_T, MUIM_Notify,
                                                MUIA_Selected, TRUE, (Object*) WookieChat->App, 2,
                                                MUIM_Application_ReturnID, 10);
                                    if (status_current->current_query->BT_mode_T)
                                        DoMethod((Object*) status_current->current_query->BT_mode_T, MUIM_Notify,
                                                MUIA_Selected, FALSE, (Object*) WookieChat->App, 2,
                                                MUIM_Application_ReturnID, 20);

                                } //DoMethod((Object*)status_conductor->conductor->BT_mode_T,MUIM_Notify,MUIA_Selected,TRUE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,10);
                                  //DoMethod((Object*)status_conductor->conductor->BT_mode_T,MUIM_Notify,MUIA_Selected,FALSE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,20);
                                status_conductor->conductor->mode_T = 1;
                            }

                            if (string4[a] == 's')
                            {

                                //DoMethod((Object*)status_conductor->conductor->BT_mode_S,MUIM_KillNotify,MUIA_Selected);
                                if (status_conductor->conductor == status_current->current_query)
                                {
                                    DoMethod((Object*) status_conductor->conductor->BT_mode_S, MUIM_KillNotify,
                                            MUIA_Selected);

                                    setmacro((Object*) status_conductor->conductor->BT_mode_S, MUIA_Selected, TRUE);

                                    DoMethod((Object*) status_conductor->conductor->BT_mode_S, MUIM_Notify,
                                            MUIA_Selected, TRUE, (Object*) WookieChat->App, 2,
                                            MUIM_Application_ReturnID, 12);
                                    DoMethod((Object*) status_conductor->conductor->BT_mode_S, MUIM_Notify,
                                            MUIA_Selected, FALSE, (Object*) WookieChat->App, 2,
                                            MUIM_Application_ReturnID, 22);
                                } //DoMethod((Object*)status_conductor->conductor->BT_mode_S,MUIM_Notify,MUIA_Selected,TRUE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,12);
                                  //DoMethod((Object*)status_conductor->conductor->BT_mode_S,MUIM_Notify,MUIA_Selected,FALSE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,22);
                                status_conductor->conductor->mode_S = 1;
                            }

                            if (string4[a] == 'i')
                            {

                                //DoMethod((Object*)status_conductor->conductor->BT_mode_I,MUIM_KillNotify,MUIA_Selected);
                                if (status_conductor->conductor == status_current->current_query)
                                {
                                    DoMethod((Object*) status_conductor->conductor->BT_mode_I, MUIM_KillNotify,
                                            MUIA_Selected);

                                    setmacro((Object*) status_conductor->conductor->BT_mode_I, MUIA_Selected, TRUE);

                                    DoMethod((Object*) status_conductor->conductor->BT_mode_I, MUIM_Notify,
                                            MUIA_Selected, TRUE, (Object*) WookieChat->App, 2,
                                            MUIM_Application_ReturnID, 13);
                                    DoMethod((Object*) status_conductor->conductor->BT_mode_I, MUIM_Notify,
                                            MUIA_Selected, FALSE, (Object*) WookieChat->App, 2,
                                            MUIM_Application_ReturnID, 23);
                                } //DoMethod((Object*)status_conductor->conductor->BT_mode_I,MUIM_Notify,MUIA_Selected,TRUE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,13);
                                  //DoMethod((Object*)status_conductor->conductor->BT_mode_I,MUIM_Notify,MUIA_Selected,FALSE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,23);
                                status_conductor->conductor->mode_I = 1;
                            }

                            if (string4[a] == 'p')
                            {
                                //DoMethod((Object*)status_conductor->conductor->BT_mode_P,MUIM_KillNotify,MUIA_Selected);
                                if (status_conductor->conductor == status_current->current_query)
                                {
                                    DoMethod((Object*) status_conductor->conductor->BT_mode_P, MUIM_KillNotify,
                                            MUIA_Selected);

                                    setmacro((Object*) status_conductor->conductor->BT_mode_P, MUIA_Selected, TRUE);

                                    DoMethod((Object*) status_conductor->conductor->BT_mode_P, MUIM_Notify,
                                            MUIA_Selected, TRUE, (Object*) WookieChat->App, 2,
                                            MUIM_Application_ReturnID, 14);
                                    DoMethod((Object*) status_conductor->conductor->BT_mode_P, MUIM_Notify,
                                            MUIA_Selected, FALSE, (Object*) WookieChat->App, 2,
                                            MUIM_Application_ReturnID, 24);

                                } //DoMethod((Object*)status_conductor->conductor->BT_mode_P,MUIM_Notify,MUIA_Selected,TRUE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,14);
                                  //DoMethod((Object*)status_conductor->conductor->BT_mode_P,MUIM_Notify,MUIA_Selected,FALSE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,24);
                                status_conductor->conductor->mode_P = 1;
                            }

                            if (string4[a] == 'm')
                            {
                                //DoMethod((Object*)status_conductor->conductor->BT_mode_M,MUIM_KillNotify,MUIA_Selected);
                                if (status_conductor->conductor == status_current->current_query)
                                {
                                    DoMethod((Object*) status_conductor->conductor->BT_mode_M, MUIM_KillNotify,
                                            MUIA_Selected);

                                    setmacro((Object*) status_conductor->conductor->BT_mode_M, MUIA_Selected, TRUE);

                                    DoMethod((Object*) status_conductor->conductor->BT_mode_M, MUIM_Notify,
                                            MUIA_Selected, TRUE, (Object*) WookieChat->App, 2,
                                            MUIM_Application_ReturnID, 15);
                                    DoMethod((Object*) status_conductor->conductor->BT_mode_M, MUIM_Notify,
                                            MUIA_Selected, FALSE, (Object*) WookieChat->App, 2,
                                            MUIM_Application_ReturnID, 25);
                                } //DoMethod((Object*)status_conductor->conductor->BT_mode_M,MUIM_Notify,MUIA_Selected,TRUE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,15);
                                  //DoMethod((Object*)status_conductor->conductor->BT_mode_M,MUIM_Notify,MUIA_Selected,FALSE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,25);
                                status_conductor->conductor->mode_M = 1;
                            }

                            if (string4[a] == 'k')
                            {
                                //DoMethod((Object*)status_conductor->conductor->BT_mode_K,MUIM_KillNotify,MUIA_Selected);
                                //DoMethod((Object*)status_conductor->conductor->STR_keyword,MUIM_KillNotify,MUIA_String_Acknowledge);
                                if (status_conductor->conductor == status_current->current_query)
                                {
                                    DoMethod((Object*) status_conductor->conductor->BT_mode_K, MUIM_KillNotify,
                                            MUIA_Selected);
                                    DoMethod((Object*) status_conductor->conductor->STR_keyword, MUIM_KillNotify,
                                            MUIA_String_Acknowledge);

                                    setmacro((Object*) status_conductor->conductor->BT_mode_K, MUIA_Selected, TRUE);
                                    setmacro((Object*) status_conductor->conductor->STR_keyword, MUIA_String_Contents,
                                            work);

                                    DoMethod((Object*) status_conductor->conductor->BT_mode_K, MUIM_Notify,
                                            MUIA_Selected, TRUE, (Object*) WookieChat->App, 2,
                                            MUIM_Application_ReturnID, 17);
                                    DoMethod((Object*) status_conductor->conductor->BT_mode_K, MUIM_Notify,
                                            MUIA_Selected, FALSE, (Object*) WookieChat->App, 2,
                                            MUIM_Application_ReturnID, 28);
                                    DoMethod((Object*) status_conductor->conductor->STR_keyword, MUIM_Notify,
                                            MUIA_String_Acknowledge, MUIV_EveryTime, (Object*) WookieChat->App, 2,
                                            MUIM_Application_ReturnID, 17);

                                } //DoMethod((Object*)status_conductor->conductor->BT_mode_K,MUIM_Notify,MUIA_Selected,TRUE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,17);
                                  //DoMethod((Object*)status_conductor->conductor->BT_mode_K,MUIM_Notify,MUIA_Selected,FALSE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,28);
                                  //DoMethod((Object*)status_conductor->conductor->STR_keyword,MUIM_Notify,MUIA_String_Acknowledge, MUIV_EveryTime, (Object*)WookieChat->App,2,MUIM_Application_ReturnID,17);
                                status_conductor->conductor->mode_K = 1;
                                strcpy(status_conductor->conductor->keyword, work);
                            }

                            if (string4[a] == 'l')
                            {
                                //DoMethod((Object*)status_conductor->conductor->BT_mode_L,MUIM_KillNotify,MUIA_Selected);
                                //DoMethod((Object*)status_conductor->conductor->STR_limit,MUIM_KillNotify,MUIA_String_Acknowledge);
                                if (status_conductor->conductor == status_current->current_query)
                                {
                                    DoMethod((Object*) status_conductor->conductor->BT_mode_L, MUIM_KillNotify,
                                            MUIA_Selected);
                                    DoMethod((Object*) status_conductor->conductor->STR_limit, MUIM_KillNotify,
                                            MUIA_String_Acknowledge);

                                    setmacro((Object*) status_conductor->conductor->BT_mode_L, MUIA_Selected, TRUE);
                                    setmacro((Object*) status_conductor->conductor->STR_limit, MUIA_String_Contents,
                                            work);

                                    DoMethod((Object*) status_conductor->conductor->BT_mode_L, MUIM_Notify,
                                            MUIA_Selected, TRUE, (Object*) WookieChat->App, 2,
                                            MUIM_Application_ReturnID, 16);
                                    DoMethod((Object*) status_conductor->conductor->BT_mode_L, MUIM_Notify,
                                            MUIA_Selected, FALSE, (Object*) WookieChat->App, 2,
                                            MUIM_Application_ReturnID, 27);
                                    DoMethod((Object*) status_conductor->conductor->STR_limit, MUIM_Notify,
                                            MUIA_String_Acknowledge, MUIV_EveryTime, (Object*) WookieChat->App, 2,
                                            MUIM_Application_ReturnID, 16);

                                } //DoMethod((Object*)status_conductor->conductor->BT_mode_L,MUIM_Notify,MUIA_Selected,TRUE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,16);
                                  //DoMethod((Object*)status_conductor->conductor->BT_mode_L,MUIM_Notify,MUIA_Selected,FALSE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,27);
                                  //DoMethod((Object*)status_conductor->conductor->STR_limit,MUIM_Notify,MUIA_String_Acknowledge, MUIV_EveryTime, (Object*)WookieChat->App,2,MUIM_Application_ReturnID,16);
                                status_conductor->conductor->mode_L = 1;
                                strcpy(status_conductor->conductor->limit, work);
                            }

                        }
                        else if (string4[a] == '-' || option == 0)
                        {
                            if (string4[a] == '-')
                                a++;
                            if (string4[a + 1] != '-')
                                option = 0;
                            else
                                option = 1;

                            for (int count = 1; count <= status_conductor->max_modes; count++)
                            {
                                if (string4[a] == status_conductor->user_modes[count].mode[0])
                                {
                                    ReplaceNicksWithVoiceOps(work, string4[a], 0);
                                    break;
                                }
                            }

                            if (string4[a] == 'n')
                            {
                                //DoMethod((Object*)status_conductor->conductor->BT_mode_N,MUIM_KillNotify,MUIA_Selected);
                                if (status_conductor->conductor == status_current->current_query)
                                {
                                    setmacro((Object*) status_conductor->conductor->BT_mode_N, MUIA_Selected, FALSE);
                                } //DoMethod((Object*)status_conductor->conductor->BT_mode_N,MUIM_Notify,MUIA_Selected,TRUE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,11);
                                  //DoMethod((Object*)status_conductor->conductor->BT_mode_N,MUIM_Notify,MUIA_Selected,FALSE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,21);

                                status_conductor->conductor->mode_N = 0;

                            }

                            if (string4[a] == 't')
                            {
                                //DoMethod((Object*)status_conductor->conductor->BT_mode_T,MUIM_KillNotify,MUIA_Selected);
                                if (status_conductor->conductor == status_current->current_query)
                                {
                                    setmacro((Object*) status_conductor->conductor->BT_mode_T, MUIA_Selected, FALSE);
                                } //DoMethod((Object*)status_conductor->conductor->BT_mode_T,MUIM_Notify,MUIA_Selected,TRUE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,10);
                                  //DoMethod((Object*)status_conductor->conductor->BT_mode_T,MUIM_Notify,MUIA_Selected,FALSE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,20);

                                status_conductor->conductor->mode_T = 0;
                            }
                            if (string4[a] == 's')
                            {
                                //DoMethod((Object*)status_conductor->conductor->BT_mode_S,MUIM_KillNotify,MUIA_Selected);
                                if (status_conductor->conductor == status_current->current_query)
                                {
                                    setmacro((Object*) status_conductor->conductor->BT_mode_S, MUIA_Selected, FALSE);
                                } //DoMethod((Object*)status_conductor->conductor->BT_mode_S,MUIM_Notify,MUIA_Selected,TRUE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,12);
                                  //DoMethod((Object*)status_conductor->conductor->BT_mode_S,MUIM_Notify,MUIA_Selected,FALSE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,22);
                                status_conductor->conductor->mode_S = 0;
                            }

                            if (string4[a] == 'i')
                            {

                                //DoMethod((Object*)status_conductor->conductor->BT_mode_I,MUIM_KillNotify,MUIA_Selected);
                                if (status_conductor->conductor == status_current->current_query)
                                {
                                    setmacro((Object*) status_conductor->conductor->BT_mode_I, MUIA_Selected, FALSE);
                                } //DoMethod((Object*)status_conductor->conductor->BT_mode_I,MUIM_Notify,MUIA_Selected,TRUE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,13);
                                  //DoMethod((Object*)status_conductor->conductor->BT_mode_I,MUIM_Notify,MUIA_Selected,FALSE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,23);
                                status_conductor->conductor->mode_I = 0;
                            }

                            if (string4[a] == 'p')
                            {
                                //DoMethod((Object*)status_conductor->conductor->BT_mode_P,MUIM_KillNotify,MUIA_Selected);
                                if (status_conductor->conductor == status_current->current_query)
                                {
                                    setmacro((Object*) status_conductor->conductor->BT_mode_P, MUIA_Selected, FALSE);
                                } //DoMethod((Object*)status_conductor->conductor->BT_mode_P,MUIM_Notify,MUIA_Selected,TRUE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,14);
                                  //DoMethod((Object*)status_conductor->conductor->BT_mode_P,MUIM_Notify,MUIA_Selected,FALSE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,24);
                                status_conductor->conductor->mode_P = 0;
                            }

                            if (string4[a] == 'm')
                            {
                                //DoMethod((Object*)status_conductor->conductor->BT_mode_M,MUIM_KillNotify,MUIA_Selected);
                                if (status_conductor->conductor == status_current->current_query)
                                {
                                    setmacro((Object*) status_conductor->conductor->BT_mode_M, MUIA_Selected, FALSE);
                                } //DoMethod((Object*)status_conductor->conductor->BT_mode_M,MUIM_Notify,MUIA_Selected,TRUE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,15);
                                  //DoMethod((Object*)status_conductor->conductor->BT_mode_M,MUIM_Notify,MUIA_Selected,FALSE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,25);
                                status_conductor->conductor->mode_M = 0;
                            }

                            if (string4[a] == 'k')
                            {
                                //DoMethod((Object*)status_conductor->conductor->BT_mode_K,MUIM_KillNotify,MUIA_Selected);
                                //DoMethod((Object*)status_conductor->conductor->STR_keyword,MUIM_KillNotify,MUIA_String_Acknowledge);
                                if (status_conductor->conductor == status_current->current_query)
                                {
                                    setmacro((Object*) status_conductor->conductor->BT_mode_K, MUIA_Selected, FALSE);
                                    setmacro((Object*) status_conductor->conductor->STR_keyword, MUIA_String_Contents,
                                            work);
                                } //DoMethod((Object*)status_conductor->conductor->BT_mode_K,MUIM_Notify,MUIA_Selected,TRUE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,17);
                                  //DoMethod((Object*)status_conductor->conductor->BT_mode_K,MUIM_Notify,MUIA_Selected,FALSE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,28);
                                  //DoMethod((Object*)status_conductor->conductor->STR_keyword,MUIM_Notify,MUIA_String_Acknowledge, MUIV_EveryTime, (Object*)WookieChat->App,2,MUIM_Application_ReturnID,17);
                                status_conductor->conductor->mode_K = 0;
                                strcpy(status_conductor->conductor->keyword, "");
                            }

                            if (string4[a] == 'l')
                            {
                                //DoMethod((Object*)status_conductor->conductor->BT_mode_L,MUIM_KillNotify,MUIA_Selected);
                                //DoMethod((Object*)status_conductor->conductor->STR_limit,MUIM_KillNotify,MUIA_String_Acknowledge);
                                if (status_conductor->conductor == status_current->current_query)
                                {
                                    setmacro((Object*) status_conductor->conductor->BT_mode_L, MUIA_Selected, FALSE);
                                    setmacro((Object*) status_conductor->conductor->STR_limit, MUIA_String_Contents,
                                            work);
                                } //DoMethod((Object*)status_conductor->conductor->BT_mode_L,MUIM_Notify,MUIA_Selected,TRUE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,16);
                                  //DoMethod((Object*)status_conductor->conductor->BT_mode_L,MUIM_Notify,MUIA_Selected,FALSE,(Object*)WookieChat->App,2,MUIM_Application_ReturnID,27);
                                  //DoMethod((Object*)status_conductor->conductor->STR_limit,MUIM_Notify,MUIA_String_Acknowledge, MUIV_EveryTime, (Object*)WookieChat->App,2,MUIM_Application_ReturnID,16);
                                status_conductor->conductor->mode_L = 0;
                                strcpy(status_conductor->conductor->limit, "");
                            }

                        }

                        a++;

                        work = strtok(NULL, " ");

                    }

                    add_text_to_conductor_list((char*) buffer3, 5, ACTIVITY);

                }
                else
                {
                    string4[0] = '\0';
                    //string5[0]='\0';

                }
            }
        }
        else if (!strcmp(incoming_2, "PART"))
        {
            int a = 1, b = 0;

            //string1=nick
            //string8=hostname
            //string3=channelname
            //string4=part message

            while (incoming_1[a] != '!' && incoming_1[a] != '\0')
            {
                string7[b] = incoming_1[a];
                a++;
                b++;
            }
            string7[b] = '\0';
            b = 0;
            a++;

            while (incoming_1[a] != '\0')
            {
                string8[b] = incoming_1[a];
                a++;
                b++;
            }
            string8[b] = '\0';
            a = 0;
            b = 0;

            string3 = strtok(incoming_3, " ");
            string4 = strtok(NULL, "");

            //MrsCushla!~cushy1@122-57-133-7.jetstream.xtra.co.nz PART :#jahc2
            //printf("1part string3:%s\n",string3);

            if (string3[0] == ':')
            {
                strcpy(string3, string3 + 1);
                //printf("2part string3:%s\n",string3);
            }

            if (string4)
            {
                if (string4[0] == ':')
                {
                    string4[0] = '(';
					sprintf(buffer3, "%s%s%s%s %s (%s) %s)", timestamp,
							LGS( MSG_OPENING_BRACKET ),
							LGS( MSG_PART ),
							LGS( MSG_CLOSING_BRACKET ),
							string7,
							string8,
                            string4);
                }
                else
					sprintf(buffer3, "%s%s%s%s %s (%s) (%s)", timestamp,
							LGS( MSG_OPENING_BRACKET ),
							LGS( MSG_PART ),
							LGS( MSG_CLOSING_BRACKET ),
							string7,
							string8,
                            string4);
            }
            else
				sprintf(buffer3, "%s%s%s%s %s (%s)", timestamp,
						LGS( MSG_OPENING_BRACKET ),
						LGS( MSG_PART ),
						LGS( MSG_CLOSING_BRACKET ),
						string7,
						string8);

            status_conductor->previous_query = status_conductor->root;
            status_conductor->conductor = status_conductor->root;

            if (string3)
            {
                while (stricmp(status_conductor->conductor->name, string3))
                {
                    status_conductor->conductor = status_conductor->conductor->next;
                    if (!status_conductor->conductor)
                        break;
                }

                if (status_conductor->conductor && string7 != status_conductor->nick)
                {
                    if (my_settings.show_joins_parts)
                        add_text_to_conductor_list((char*) buffer3, 3, ACTIVITY);

                    if ((my_settings.events[PART].use_when == 1 && is_window_active())
                            || (my_settings.events[PART].use_when == 2 && !is_window_active())
                            || my_settings.events[PART].use_when == 3)
                    {
                        strcpy(target_nick, string7);
                        create_arexx_event_string(my_settings.events[PART].arexx_script,
                                my_settings.events[PART].arexx_script2, buffer3);
                        Execute((_s_cs)event_string, 0, 0);
                    }

                    RemoveNick(string7);
                }

                status_conductor->conductor = status_conductor->root;
            }

        }
        else if (!strcmp(incoming_2, "QUIT"))
        {
            int a = 1, b = 0;
            //string7=nick
            //string8=hostname
            //string9=quit message

            while (incoming_1[a] != '!')
            {
                string7[b] = incoming_1[a];
                a++;
                b++;
            }
            string7[b] = '\0';
            b = 0;
            a++;

            while (incoming_1[a] != '\0')
            {
                string8[b] = incoming_1[a];
                a++;
                b++;
            }
            string8[b] = '\0';
            a = 0;

            while (incoming_3[a] != '\0')
            {
                string9[a] = incoming_3[a + 1];
                a++;
            }
            string9[a] = '\0';

            status_conductor->conductor = status_conductor->root;

			sprintf(buffer3, "%s%s%s%s %s (%s) (%s)", timestamp,
					LGS( MSG_OPENING_BRACKET ),
					LGS( MSG_QUIT ),
					LGS( MSG_CLOSING_BRACKET ),
					string7,
					string8,
					string9);
            if ((my_settings.events[QUIT].use_when == 1 && is_window_active())
                    || (my_settings.events[QUIT].use_when == 2 && !is_window_active())
                    || my_settings.events[QUIT].use_when == 3)
            {
                strcpy(target_nick, string7);
                create_arexx_event_string(my_settings.events[QUIT].arexx_script,
                        my_settings.events[QUIT].arexx_script2, buffer3);
                Execute((_s_cs)event_string, 0, 0);
            }

            while (status_conductor->conductor)
            {
                //if(string7)
                {

                    //if(status_conductor->conductor->name[0]=='#' || status_conductor->conductor->name[0]=='&')
                    if (strcspn(status_conductor->conductor->name, status_conductor->chantypes) == 0)
                    {
						sprintf(buffer3, "%s%s%s%s %s (%s) (%s)", timestamp,
								LGS( MSG_OPENING_BRACKET ),
								LGS( MSG_QUIT ),
								LGS( MSG_CLOSING_BRACKET ),
								string7,
								string8,
                                string9);
                        if (RemoveNick(string7))
                            add_text_to_conductor_list((char*) buffer3, 4, ACTIVITY);

                    }
                    else
                    {
						sprintf(buffer3, "%s%s%s%s %s %s.. (%s)", timestamp,
								LGS( MSG_OPENING_BRACKET ),
								LGS( MSG_QUIT ),
								LGS( MSG_CLOSING_BRACKET ),
								string7,
								LGS( MSG_USER_HAS_QUIT_IRC ),
								string9);
                        //RemoveNick(string7);

                        //if(strcspn(status_conductor->conductor->name,status_conductor->chantypes)>0)
                        //    add_text_to_conductor_list((char*)buffer3, 4, ACTIVITY);

                        if (RemoveNick(string7))
                            add_text_to_conductor_list((char*) buffer3, 4, ACTIVITY);
                    }

                }

                status_conductor->conductor = status_conductor->conductor->next;
                if (!status_conductor->conductor)
                    break;
            }

            if (!stricmp(string7, nick))
            {
                ChangeMyNick(nick);

                //this is the built in Nickserv login/registering system in WookieChat
                if (status_conductor->nick_pass[0] != '\0')
                {
                    sprintf(buffer3, "PRIVMSG nickserv :identify %s\r\n", status_conductor->nick_pass);
                    send_text(buffer3);
                }

            }

        }
        else if (!strcmp(incoming_2, "NICK"))
        {
            int a = 1, b = 0;
            //string7=original nick
            //string8=hostname
            //string9=new nick

            while (incoming_1[a] != '!')
            {
                string7[b] = incoming_1[a];
                a++;
                b++;
            }
            string7[b] = '\0';
            b = 0;
            a++;

            while (incoming_1[a] != '\0')
            {
                string8[b] = incoming_1[a];
                a++;
                b++;
            }
            string8[b] = '\0';
            a = 0;

            if (incoming_3[a] == ':')
            {
                while (incoming_3[a] != '\0')
                {
                    string9[a] = incoming_3[a + 1];
                    a++;
                }
                string9[a] = '\0';
            }
            else
            {
                while (incoming_3[a] != '\0')
                {
                    string9[a] = incoming_3[a];
                    a++;
                }
                string9[a] = '\0';

            }

			sprintf(buffer3, "%s%sNick%s %s %s %s", timestamp,
					LGS( MSG_OPENING_BRACKET ),
					LGS( MSG_CLOSING_BRACKET ),
					string7,
					LGS( MSG_IS_NOW_KNOWN_AS ),
					string9);
            if (!stricmp(string7, status_conductor->nick))
            {
                strcpy(status_conductor->nick, string9);

                update_your_nickname_text_label();

                change_window_titlebar_text();

            }

            status_conductor->conductor = status_conductor->root;
            while (status_conductor->conductor)
            {
                update_nicks_hostname(string7, string8);

                if (status_conductor->conductor->LV_nicklist)
                    ChangeNick(string7, string9, buffer3);

                if (status_conductor->conductor->BT_querybutton
                        && !stricmp(status_conductor->conductor->name, string7))
                {
                    if (status_conductor->conductor->activity == 0)
                        sprintf(string10, "\033c\033I[2:00000000,00000000,00000000] %s", string9);
                    else if (status_conductor->conductor->activity == 3)
                        sprintf(string10, "\033c\033I[%s] %s", activity_highlight, string9);
                    else if (status_conductor->conductor->activity == 2)
                        sprintf(string10, "\033c\033I[%s] %s", activity_chat, string9);
                    else if (status_conductor->conductor->activity == 1)
                        sprintf(string10, "\033c\033I[%s] %s", activity, string9);

                    setmacro(status_conductor->conductor->BT_querybutton, MUIA_Text_Contents, string10);

                    //set(status_conductor->conductor->BT_querybutton,MUIA_Text_Contents,string9);
                    strcpy(status_conductor->conductor->name, string9);

                    strcpy(status_conductor->conductor->nlist_tab.entry, "         :");
                    //status_conductor->conductor->nlist_tab.colour=NLIST_NORMAL_PEN;
                    strcat(status_conductor->conductor->nlist_tab.entry, status_conductor->conductor->name);
                    DoMethod((Object*) WookieChat->LV_tabs, MUIM_NList_ReplaceSingle,
                            &status_conductor->conductor->nlist_tab, status_conductor->conductor->nlist_tab_number, 0,
                            0);

                    //shorten the name to avoid MUI glitches
                    strcpy(status_conductor->conductor->displayed_name, string9);
                    //removed while testing small tabs
                    if (!SMALLTABS && my_settings.use_nlistobject_for_tabs == 0)
                    {
                        if (strlen(status_conductor->conductor->displayed_name) > 11)
                        {
                            status_conductor->conductor->displayed_name[10] = '.';
                            status_conductor->conductor->displayed_name[11] = '.';
                            status_conductor->conductor->displayed_name[12] = '\0';
                        }
                    }

                }
                status_conductor->conductor = status_conductor->conductor->next;
            }

        }
        else if (!strcmp(incoming_2, "PRIVMSG"))
        {
            //string7=nick
            //string8=hostname
            //string9=channel or nick this message came from
            //string10=message text

            int a = 1, b = 0;

            while (incoming_1[a] != '!' && incoming_1[a] != '\0')
            {
                string7[b] = incoming_1[a];
                a++;
                b++;
            }
            string7[b] = '\0';
            b = 0;
            a++;

            if (incoming_1[a - 1] != '\0')
            {
                while (incoming_1[a] != '\0')
                {
                    string8[b] = incoming_1[a];
                    a++;
                    b++;
                }
                string8[b] = '\0';
                a = 0;
                b = 0;
            }
            else
                strcpy(string8, "");

            while (incoming_3[a] != ' ')
            {
                string9[b] = incoming_3[a];
                b++;
                a++;
            }
            string9[b] = '\0';
            a = a + 2;
            b = 0;

            //while(incoming_3[a] != '\0')
            //{ string10[b]=incoming_3[a]; a++; b++; } string10[b]='\0';

            while (incoming_3[a] != '\0' && a < 400)
            {
                string10[b] = incoming_3[a];
                a++;
                b++;
            }
            string10[b] = '\0';

            strcpy(target_nick, string7);

            //TEST TO SEE IF WE'RE IGNORING THIS PERSON IN ANY YWAY
            user_being_ignored(string7, string8, PRIVMSG);
            user_being_ignored(string7, string8, CTCP);
            user_being_ignored(string7, string8, DCC);

            status_conductor->conductor = status_conductor->root;
            BOOL channel_message = FALSE;
            BOOL isAction = FALSE;

            if (!stricmp(string9, status_conductor->nick))
            {

                while (stricmp(status_conductor->conductor->name, string7))
                {
                    status_conductor->conductor = status_conductor->conductor->next;
                    if (!status_conductor->conductor)
                        break;
                }

                if (!status_conductor->conductor)
                {

                    status_conductor->conductor = status_conductor->root;
                    while (status_conductor->conductor->name[0] != '-')
                    {
                        status_conductor->conductor = status_conductor->conductor->next;
                        if (!status_conductor->conductor)
                            break;
                    }

                }

            }
            else //if(strcspn(string9,status_conductor->chantypes)==0)
            {
                channel_message = TRUE;

                while (stricmp(status_conductor->conductor->name, string9))
                {
                    status_conductor->conductor = status_conductor->conductor->next;
                    if (!status_conductor->conductor)
                    {
                        break;
                    }
                }

            }

            char modes[10];
            strcpy(modes, "");

            if (status_conductor->conductor && my_settings.user_modes_beside_nicks)
            {
                for (int count = 0; count < status_conductor->conductor->nicks; count++)
                {
                    if (!stricmp(status_conductor->conductor->nicklist[count].name, string7))
                    {
                        strcpy(modes, status_conductor->conductor->nicklist[count].modes);
                        if (modes[0] == ' ')
                            strcpy(modes, "");
                        break;

                    }

                }
            }

            /*
             if(status_conductor->conductor) sprintf(buffer3,"%s<%s%s> %s",timestamp,modes,string7, string10);
             else if(!status_conductor->conductor && my_settings.open_query_on_privmsg==0) sprintf(buffer3,"%s*%s* %s",timestamp,string7, string10);
             else if(!status_conductor->conductor && my_settings.open_query_on_privmsg==1) sprintf(buffer3,"%s<%s%s> %s",timestamp,modes,string7, string10);
             */

            if (status_conductor->conductor)
                sprintf(buffer3, "%s<%s%s> %s", timestamp, modes, target_nick, string10);
            else if (!status_conductor->conductor && my_settings.open_query_on_privmsg == 0)
                sprintf(buffer3, "%s*%s* %s", timestamp, target_nick, string10);
            else if (!status_conductor->conductor && my_settings.open_query_on_privmsg == 1)
                sprintf(buffer3, "%s<%s%s> %s", timestamp, modes, target_nick, string10);

            if (!ignore_privmsg)
            {

                //if(!status_conductor->conductor && my_settings.open_query_on_privmsg==1 && strstr(string10,"\001")==NULL && string9[0] != '#')
                if (!status_conductor->conductor && my_settings.open_query_on_privmsg == 1
                        && strstr(string10, "\001") == NULL && strcspn(string9, status_conductor->chantypes) != 0)
                {
                    char work[100];
                    strcpy(work, string7);

                    create_new_tab(string7, 0, 2);

                    strcpy(string7, work);

                    play_tab_open_sample = TRUE;

                    //play a sound if configured to do so
                    getmacro((Object*) WookieChat->WI_main, MUIA_Window_Activate, &string1);

                    if ((!string1 && my_settings.play_sound_when_tabopen == 2)
                            || my_settings.play_sound_when_tabopen == 1)
                    {
                        play_sample(SAMPLE_TABOPEN);
                    }

                    setmacro((Object*) status_conductor->conductor->STR_topic, MUIA_String_Contents, string8);

                    //experimental 2008-07-21
                    //strcpy(status_conductor->str, pch+2);
                    //return;

                }
                if (strstr(string10, "\001ACTION") && string10[strlen(string10) - 1] == '\001')
                {

                    isAction = TRUE;
                    char work[100];
                    strcpy(work, string7);

                    strcpy(target_nick, string7);

                    if (!status_conductor->conductor && my_settings.open_query_on_privmsg == 1
                            && strcspn(string9, status_conductor->chantypes) != 0) //string9[0] != '#')
                        create_new_tab(string7, 0, 2);

                    strcpy(string7, work);

                    strtok(string10, " ");

                    string5 = strtok(NULL, "");
                    if (!(string5))
                    {
                        string5 = malloc(sizeof(char));
                        strcpy(string5, "");
                    }

                    string5[strlen(string5) - 1] = '\0';

                    if (status_conductor->conductor)
                        sprintf(buffer3, "%s* %s %s", timestamp, string7, string5);
                    else
                        sprintf(buffer3, "%s*%s* * %s %s", timestamp, string7, string7, string5);

                    colour = 7;

                    if ((my_settings.events[CHANNEL_MESSAGE].use_when == 1 && is_window_active())
                            || (my_settings.events[CHANNEL_MESSAGE].use_when == 2 && !is_window_active())
                            || my_settings.events[CHANNEL_MESSAGE].use_when == 3)
                    {
                        char backup[800];
                        strcpy(backup, buffer3);

                        sprintf(buffer3, "* %s %s", string7, string5);

                        strcpy(target_nick, string7);
                        create_arexx_event_string(my_settings.events[CHANNEL_MESSAGE].arexx_script,
                                my_settings.events[CHANNEL_MESSAGE].arexx_script2, buffer3);
                        Execute((_s_cs)event_string, 0, 0);

                        strcpy(buffer3, backup);
                        channel_message = TRUE;

                    }

                }
                else if ((my_settings.events[CHANNEL_MESSAGE].use_when == 1 && is_window_active())
                        || (my_settings.events[CHANNEL_MESSAGE].use_when == 2 && !is_window_active())
                        || my_settings.events[CHANNEL_MESSAGE].use_when == 3)
                {
                    if (channel_message == TRUE)
                    {
                        char backup[800];
                        strcpy(backup, buffer3);

                        sprintf(buffer3, "<%s> %s", string7, string10);

                        strcpy(target_nick, string7);
                        create_arexx_event_string(my_settings.events[CHANNEL_MESSAGE].arexx_script,
                                my_settings.events[CHANNEL_MESSAGE].arexx_script2, buffer3);
                        Execute((_s_cs)event_string, 0, 0);

                        strcpy(buffer3, backup);

                    }
                }

            }
            // CTCP SUPPORT
            if (!ignore_ctcp)
            {
                BOOL is_it_a_ctcp = FALSE;

                if (strstr(string10, "\001SOUND") && string10[strlen(string10) - 1] == '\001')
                {
                    strtok(string10, " ");

                    string5 = strtok(NULL, "\001");
                    if (!(string5))
                    {
                        string5 = malloc(sizeof(char));
                        strcpy(string5, "");
                    }
                    string5[strlen(string5)] = '\0';

					sprintf(buffer3, "%s%sSound%s %s %s %s", timestamp,
							LGS( MSG_OPENING_BRACKET ),
							LGS( MSG_CLOSING_BRACKET ),
							string7,
							LGS( MSG_PLAYS ),
							string5);

                    play_external_sound_replayer(string5);

                    colour = 6;
                    is_it_a_ctcp = TRUE;
                    channel_message = TRUE;

                }

                else if ((strstr(string10, "\001VERSION") || strstr(string10, "\001version"))
                        && string10[strlen(string10) - 1] == '\001')
                {

                    //if(string9[0]=='#')
                    if (strcspn(string9, status_conductor->chantypes) == 0)

                        sprintf(buffer3, "%s%sCTCP%s CTCP VERSION %s %s %s", timestamp,
								LGS( MSG_OPENING_BRACKET ),
								LGS( MSG_CLOSING_BRACKET ),
								string9,
								LGS( MSG_FROM ),
								string7);
                    else
						sprintf(buffer3, "%s%sCTCP%s CTCP VERSION %s %s", timestamp,
								LGS( MSG_OPENING_BRACKET ),
								LGS( MSG_CLOSING_BRACKET ),
								LGS( MSG_FROM ),
								string7);
                    sprintf(sendstuff, "NOTICE %s :\001VERSION %s \001\r\n", string7, VERSION_CTCP);
                    colour = 6;

                    send_text(sendstuff);
                    is_it_a_ctcp = TRUE;
                    channel_message = TRUE;

                }
                else if ((strstr(string10, "\001PING") || strstr(string10, "\001ping"))
                        && string10[strlen(string10) - 1] == '\001')
                {

                    //if(string9[0]=='#')
                    if (strcspn(string9, status_conductor->chantypes) == 0)
						sprintf(buffer3, "%s%sCTCP%s CTCP PING %s %s %s", timestamp,
								LGS( MSG_OPENING_BRACKET ),
								LGS( MSG_CLOSING_BRACKET ),
								string9,
								LGS( MSG_FROM ),
                                string7);
                    else
						sprintf(buffer3, "%s%sCTCP%s CTCP PING %s %s", timestamp,
								LGS( MSG_OPENING_BRACKET ),
								LGS( MSG_CLOSING_BRACKET ),
								LGS( MSG_FROM ),
								string7);
                    sprintf(sendstuff, "NOTICE %s :%s\r\n", string7, string10);
                    colour = 6;

                    send_text(sendstuff);
                    is_it_a_ctcp = TRUE;
                    channel_message = TRUE;

                }
                else if ((strstr(string10, "\001TIME") || strstr(string10, "\001time"))
                        && string10[strlen(string10) - 1] == '\001')
                {
                    struct timeval systime;
                    struct ClockData clockdata;
                    if (strcspn(string9, status_conductor->chantypes) == 0)
						sprintf(buffer3, "%s%sCTCP%s CTCP TIME %s %s %s", timestamp,
								LGS( MSG_OPENING_BRACKET ),
								LGS( MSG_CLOSING_BRACKET ),
								string9,
								LGS( MSG_FROM ),
                                string7);
                    else
						sprintf(buffer3, "%s%sCTCP%s CTCP TIME %s %s", timestamp,
								LGS( MSG_OPENING_BRACKET ),
								LGS( MSG_CLOSING_BRACKET ),
								LGS( MSG_FROM ),
								string7);

                    get_sys_time(&systime); // Get current system time
#ifdef __amigaos4__
                    Amiga2Date(systime.tv_sec, &clockdata);
#else
                    Amiga2Date(systime.tv_secs, &clockdata);
#endif

                    timestamp_2_string();

                    char month[10];
                    switch (clockdata.month)
                    {
                    case 1:
                        strcpy(month, "Jan");
                        break;
                    case 2:
                        strcpy(month, "Feb");
                        break;
                    case 3:
                        strcpy(month, "Mar");
                        break;
                    case 4:
                        strcpy(month, "Apr");
                        break;
                    case 5:
                        strcpy(month, "May");
                        break;
                    case 6:
                        strcpy(month, "Jun");
                        break;
                    case 7:
                        strcpy(month, "Jul");
                        break;
                    case 8:
                        strcpy(month, "Aug");
                        break;
                    case 9:
                        strcpy(month, "Sep");
                        break;
                    case 10:
                        strcpy(month, "Oct");
                        break;
                    case 11:
                        strcpy(month, "Nov");
                        break;
                    case 12:
                        strcpy(month, "Dec");
                        break;

                    }

                    sprintf(sendstuff, "NOTICE %s :\001TIME %d-%s-%d %s:%s\001\r\n", string7, clockdata.mday, month,
                            clockdata.year, timestamp_hrs, timestamp_mins);

                    colour = 6;

                    send_text(sendstuff);
                    is_it_a_ctcp = TRUE;
                    channel_message = TRUE;

                }
                else if (string10[0] == '\001' && string10[strlen(string10) - 1] == '\001'
                        && !(string10[1] == 'D' && string10[2] == 'C' && string10[3] == 'C' && string10[4] == ' '))
                {
                    string10[0] = ' ';
                    string10[strlen(string10) - 1] = ' ';
					sprintf(buffer3, "%s%sCTCP%s UNKNOWN CTCP%s%s %s", timestamp,
							LGS( MSG_OPENING_BRACKET ),
							LGS( MSG_CLOSING_BRACKET ),
							string10,
							LGS( MSG_FROM ),
							string7);
                    colour = 6;
                    is_it_a_ctcp = TRUE;
                    channel_message = TRUE;

                }

                if ((my_settings.events[CTCP_REQUEST].use_when == 1 && is_window_active())
                        || (my_settings.events[CTCP_REQUEST].use_when == 2 && !is_window_active())
                        || my_settings.events[CTCP_REQUEST].use_when == 3)
                {
                    if (is_it_a_ctcp == TRUE)
                    {
                        char backup[800];
                        strcpy(backup, buffer3);

                        strcpy(buffer3, buffer3 + strlen(timestamp));
                        strcpy(target_nick, string7);
                        create_arexx_event_string(my_settings.events[CTCP_REQUEST].arexx_script,
                                my_settings.events[CTCP_REQUEST].arexx_script2, buffer3);
                        Execute((_s_cs)event_string, 0, 0);

                        strcpy(buffer3, backup);

                    }
                }

            }

            if (!ignore_dcc)
            {
                // DCC SUPPORT
                if (strstr(string10, "\001DCC RESUME") && string10[strlen(string10) - 1] == '\001') //outgoing dcc resume
                {
                    strtok(string10, " ");
                    strtok(NULL, " "); //move strtok position past "DCC RESUME"
                    string1 = strtok(NULL, " "); //filename
                    string2 = strtok(NULL, " "); //port number
                    string3 = strtok(NULL, "\001 "); //file size at recievers end

                    if (string1 && string2 && string3)
                    {

                        colour = 9;

                        sprintf(sendstuff, "PRIVMSG %s :\001DCC ACCEPT %s %s %s\001\r\n", string7, string1, string2,
                                string3);
                        send_text(sendstuff);

                        //if(DEBUG) printf("sendstuff:%s",sendstuff);

                        if (string1[0] == '\"')
                        {
                            int count;

                            for (count = 1; count < (LONG) strlen(string1); count++)
                            {
                                string1[count - 1] = string1[count];
                            }

                            string1[count - 2] = '\0';

                        }

                        dcc_send_conductor = dcc_send_root->next;
                        while (dcc_send_conductor)
                        {
                            if (!dcc_send_conductor->cancelled && !dcc_send_conductor->removed
                                    && !stricmp(dcc_send_conductor->filename, string1))
                            {
                                //if(DEBUG) printf("pincoming dcc_send_conductor->filename:%s\n",dcc_send_conductor->filename);

                                dcc_send_conductor->total_recv = atoi(string3); //start sending from this file position
#ifdef __aamigaos4__
                                if(dcc_send_conductor->dcc_file) ChangeFilePosition(dcc_send_conductor->dcc_file,dcc_send_conductor->total_recv,OFFSET_BEGINNING);
#else
                                if (dcc_send_conductor->dcc_file)
                                    Seek(dcc_send_conductor->dcc_file, dcc_send_conductor->total_recv,
                                            OFFSET_BEGINNING);
#endif
                                sprintf(buffer3, "%s%sDCC%s %s %s >> %s @ %s bytes", timestamp,
										LGS( MSG_OPENING_BRACKET ),
										LGS( MSG_CLOSING_BRACKET ),
										LGS( MSG_RESUME_FILE ),
										dcc_send_conductor->filename,
										dcc_send_conductor->nick,
										string3);

                                if ((my_settings.events[DCC_RECV_OFFERED].use_when == 1 && is_window_active())
                                        || (my_settings.events[DCC_RECV_OFFERED].use_when == 2 && !is_window_active())
                                        || my_settings.events[DCC_RECV_OFFERED].use_when == 3)
                                {
                                    strcpy(target_nick, string7);

                                    create_arexx_event_string(my_settings.events[DCC_RECV_OFFERED].arexx_script,
                                            my_settings.events[DCC_RECV_OFFERED].arexx_script2, buffer3);
                                    Execute((_s_cs)event_string, 0, 0);
                                }

                            }
                            dcc_send_conductor = dcc_send_conductor->next;

                        }

                    }

                    channel_message = TRUE;

                    //PRIVMSG User2 :DCC ACCEPT filename port position
                }
                else if (strstr(string10, "\001DCC ACCEPT") && string10[strlen(string10) - 1] == '\001') //incoming dcc resume
                {

                    //if(DEBUG) printf("dcc accept\n");
                    char *work1;

                    if (strchr(string10, '\"'))
                    {
                        strtok(string10, " ");
                        strtok(NULL, "\""); //move strtok position past "DCC ACCEPT"
                        work1 = strtok(NULL, "\"");
                        for (int count = 0; count < (LONG) strlen(work1); count++)
                        {
                            if (work1[count] == ' ')
                                work1[count] = '_';
                        }

                    }
                    else
                    {
                        strtok(string10, " ");
                        strtok(NULL, " "); //move strtok position past "DCC ACCEPT"
                        work1 = strtok(NULL, " ");

                    }

                    if (work1)
                    {

                        int a, b;

                        for (a = 0, b = 0; work1[a] != '\0';)
                        {
                            if (work1[b] != '\"')
                            {
                                work1[a] = work1[b];
                                a++;
                                b++;
                            }
                            else
                                b++;
                        }

                        work1[b] = '\0';

                        dcc_conductor = dcc_root->next;
                        while (dcc_conductor)
                        {
                            //if(DEBUG) printf("dccnick %s\n",dcc_conductor->nick);
                            //if(DEBUG) printf("string7 %s\n", string7);
                            //if(DEBUG) printf("dccfilename %s\n",dcc_conductor->filename);
                            //if(DEBUG) printf("work1 %s\n",work1);

                            if (!dcc_conductor->removed && !stricmp(dcc_conductor->nick, string7)
                                    && !stricmp(dcc_conductor->filename, work1))
                            {
                                connect(dcc_conductor->dcc_socket, (struct sockaddr*) &dcc_conductor->test,
                                        sizeof(struct sockaddr));
                                colour = 9;
								sprintf(buffer3, "%s%sDCC%s %s %s %s %s", timestamp,
										LGS( MSG_OPENING_BRACKET ),
										LGS( MSG_CLOSING_BRACKET ),
										LGS( MSG_ATTEMPTING_TO_RESUME ),
										dcc_conductor->filename,
										LGS( MSG_FROM ),
										dcc_conductor->nick); //,dcc_conductor->address,dcc_conductor->port);
                            }
                            dcc_conductor = dcc_conductor->next;

                        }
                    }

                    channel_message = TRUE;

                }
                else if (strstr(string10, "\001DCC CHAT") && string10[strlen(string10) - 1] == '\001')
                {

                    char *work1, *work2, *work3;

                    strtok(string10, " ");
                    strtok(NULL, " "); //move strtok position past "DCC CHAT"

                    work1 = strtok(NULL, " "); //the string "chat".. ignore this
                    work2 = strtok(NULL, " "); //address
                    work3 = strtok(NULL, " "); //port number
                    if (strlen(work3) >= 200)
                        work3[200] = '\0';

                    if (work1 && work2 && work3)
                    {
                        if (!stricmp(work3, "0") || !stricmp(work2, "0"))
                        {
                            colour = 9;
							sprintf(buffer3, "%s%sDCC%s%s - %s", timestamp,
									LGS( MSG_OPENING_BRACKET ),
									LGS( MSG_CLOSING_BRACKET ),
									string7,
									LGS( MSG_UNABLE_TO_CONNECT ) );
                        }
                        else
                        {
                            create_recv_dcc_chat(string7, work2, work3);
                            colour = 9;
							sprintf(buffer3, "%s%sDCC%s%s - %s", timestamp,
									LGS( MSG_OPENING_BRACKET ),
									LGS( MSG_CLOSING_BRACKET ),
									string7,
									LGS( MSG_ATTEMPT_TO_ACCEPT ) );
                        }
                    }

                    channel_message = TRUE;

                }
                else if (strstr(string10, "\001DCC SEND") && string10[strlen(string10) - 1] == '\001')
                {
                    char *work7, *work1, *work2, *work3;
                    unsigned short work6; //port number after being converted

                    if (DEBUG)
                        printf(
                                "incoming file transfer detected! now chopping up details into individual strings, for sending to our create_recv_dcc() function..\n");

                    if (!strchr(string10, '"'))
                    {
                        strtok(string10, " ");
                        strtok(NULL, " "); //move strtok position past "DCC SEND"

                        work1 = strtok(NULL, " "); //filename
                        work2 = strtok(NULL, " "); //address
                        work3 = strtok(NULL, " "); //port number
                        work7 = strtok(NULL, "\r\n\001 "); //filesize
                    }
                    else
                    {
                        strtok(string10, " ");
                        strtok(NULL, "\""); //move strtok position past "DCC SEND"

                        work1 = strtok(NULL, "\""); //filename
                        work2 = strtok(NULL, " "); //address
                        work3 = strtok(NULL, " "); //port number
                        work7 = strtok(NULL, "\r\n\001 "); //filesize

                    }
                    if (work1 && work2 && work7)
                    {
                        if (work3)
                            work6 = atoi(work3); //port number
                        else
                            work6 = 2000;

                        create_recv_dcc(string7, work1, work2, work6, work7);

                        sprintf(buffer3, "%s%sDCC%s SEND: %s %s %s port %i", timestamp,
								LGS( MSG_OPENING_BRACKET ),
								LGS( MSG_CLOSING_BRACKET ),
								work1,
								LGS( MSG_OFFERED_FROM ),
								string7,
								work6);
                        colour = 9;

                        if ((my_settings.events[DCC_RECV_OFFERED].use_when == 1 && is_window_active())
                                || (my_settings.events[DCC_RECV_OFFERED].use_when == 2 && !is_window_active())
                                || my_settings.events[DCC_RECV_OFFERED].use_when == 3)
                        {
                            strcpy(target_nick, string7);

                            create_arexx_event_string(my_settings.events[DCC_RECV_OFFERED].arexx_script,
                                    my_settings.events[DCC_RECV_OFFERED].arexx_script2, buffer3);
                            Execute((_s_cs)event_string, 0, 0);
                        }

                    }
                    channel_message = TRUE;

                }
            }

            if (!(ignore_ctcp && (strstr(string10, "\001"))) && !(ignore_privmsg && (colour == 8 || colour == 7))
                    && !(ignore_dcc && colour == 9))

            {

                if (status_conductor->conductor)
                {
                    if (strcspn(string7, status_conductor->chantypes) != 0)
                        update_nicks_hostname(string7, string8);

                    if (strcspn(string9, status_conductor->chantypes) != 0 && play_tab_open_sample == FALSE)
                    {
                        getmacro((Object*) WookieChat->WI_main, MUIA_Window_Activate, &string1);

                        if ((!string1 && my_settings.play_sound_when_newmsg == 3)
                                || my_settings.play_sound_when_newmsg == 2
                                || (my_settings.play_sound_when_newmsg == 1
                                        && (status_conductor->conductor != status_current->current_query))
                                || (!string1 && my_settings.play_sound_when_newmsg == 1))
                        {
                            play_sample(SAMPLE_NEWMSG);
                        }

                    }

                    if (colour == 7 && search_for_highlight_words(string5, buffer3))
                        add_text_to_conductor_list((char*) buffer3, 11, ACTIVITY_HIGHLIGHT);
                    else if (search_for_highlight_words(string10, buffer3))
                        add_text_to_conductor_list((char*) buffer3, 11, ACTIVITY_HIGHLIGHT);
                    else
                        add_text_to_conductor_list((char*) buffer3, colour, ACTIVITY_CHAT);

                    if ((colour == 7 && search_for_highlight_words(string5, buffer3))
                            || (search_for_highlight_words(string10, buffer3)))
                    {
                        channel_message = TRUE;
                        if (status_conductor->conductor)
                        {
                            if (isAction == TRUE) // strstr(string10,"\001ACTION") ) // && string10[strlen(string10)-1] == '\001' )
                            {
                                sprintf(buffer3, "<%s> %s", target_nick, string5);
                            }
                            else
                            {
                                sprintf(buffer3, "<%s> %s", target_nick, string10);

                            }
                        }

                        create_arexx_event_string(my_settings.events[HIGHLIGHT].arexx_script,
                                my_settings.events[HIGHLIGHT].arexx_script2, buffer3);
                        if ((my_settings.events[HIGHLIGHT].use_when == 1 && is_window_active())
                                || (my_settings.events[HIGHLIGHT].use_when == 2 && !is_window_active())
                                || my_settings.events[HIGHLIGHT].use_when == 3)
                            Execute((_s_cs)event_string, 0, 0);

                    }

                }
                else
                {

                    if (strcspn(string9, status_conductor->chantypes) != 0 && play_tab_open_sample == FALSE)
                    {
                        getmacro((Object*) WookieChat->WI_main, MUIA_Window_Activate, &string1);

                        if ((!string1 && my_settings.play_sound_when_newmsg == 3)
                                || my_settings.play_sound_when_newmsg == 2
                                || (!string1 && my_settings.play_sound_when_newmsg == 1))
                        {
                            play_sample(SAMPLE_NEWMSG);
                        }

                    }

                    if (colour == 7 && search_for_highlight_words(string5, buffer3))
                        add_text_to_current_list((char*) buffer3, colour, ACTIVITY_HIGHLIGHT);
                    else if (search_for_highlight_words(string10, buffer3))
                        add_text_to_current_list((char*) buffer3, colour, ACTIVITY_HIGHLIGHT);
                    else
                        add_text_to_current_list((char*) buffer3, colour, ACTIVITY_CHAT);

                    if ((colour == 7 && search_for_highlight_words(string5, buffer3))
                            || (search_for_highlight_words(string10, buffer3)))
                    {
                        channel_message = TRUE;
                        //if(status_conductor->conductor) sprintf(buffer3,"<%s> %s",target_nick, string10);

                        if (isAction == TRUE) // strstr(string10,"\001ACTION") ) //&& string10[strlen(string10)-1] == '\001' )
                        {
                            sprintf(buffer3, "<%s> %s", target_nick, string5);
                        }
                        else
                        {
                            sprintf(buffer3, "<%s> %s", target_nick, string10);

                        }

                        create_arexx_event_string(my_settings.events[HIGHLIGHT].arexx_script,
                                my_settings.events[HIGHLIGHT].arexx_script2, buffer3);
                        if ((my_settings.events[HIGHLIGHT].use_when == 1 && is_window_active())
                                || (my_settings.events[HIGHLIGHT].use_when == 2 && !is_window_active())
                                || my_settings.events[HIGHLIGHT].use_when == 3)
                            Execute((_s_cs)event_string, 0, 0);
                    }

                }
            }

            if ((my_settings.events[PRIVATE_MESSAGE].use_when == 1 && is_window_active())
                    || (my_settings.events[PRIVATE_MESSAGE].use_when == 2 && !is_window_active())
                    || my_settings.events[PRIVATE_MESSAGE].use_when == 3)
            {

                if (channel_message == FALSE)
                {
                    char backup[800];
                    strcpy(backup, buffer3);

                    //if( strstr(string10,"\001ACTION") && string10[strlen(string10)-1] == '\001' )
                    if (isAction == TRUE)
                    {
                        sprintf(buffer3, "<%s> %s", target_nick, string5);
                    }
                    else
                    {
                        sprintf(buffer3, "<%s> %s", target_nick, string10);

                    }

                    create_arexx_event_string(my_settings.events[PRIVATE_MESSAGE].arexx_script,
                            my_settings.events[PRIVATE_MESSAGE].arexx_script2, buffer3);
                    Execute((_s_cs)event_string, 0, 0);

                    strcpy(buffer3, backup);

                }
            }

            colour = 8;
            play_tab_open_sample = FALSE;

        }
        else if (!strcmp(incoming_2, "TOPIC")) //changing the topic
        {
            //string1=nick
            //string8=hostname
            //string9=channelname
            //string10=new topic

            int a = 1, b = 0;

            while (incoming_1[a] != '!' && incoming_1[a] != ' ')
            {
                string7[b] = incoming_1[a];
                a++;
                b++;
            }
            string7[b] = '\0';
            b = 0;
            a++;

            while (incoming_1[a] != '\0')
            {
                string8[b] = incoming_1[a];
                a++;
                b++;
            }
            string8[b] = '\0';
            a = 0;
            b = 0;

            while (incoming_3[a] != ' ')
            {
                string9[b] = incoming_3[a];
                b++;
                a++;
            }
            string9[b] = '\0';
            a = a + 2;
            b = 0;

            while (incoming_3[a] != '\0')
            {
                string10[b] = incoming_3[a];
                a++;
                b++;
            }
            string10[b] = '\0';

            //update_nicks_hostname(string1, string8);

			sprintf(buffer3, "%s%sTopic%s %s %s '%s'", timestamp,
					LGS( MSG_OPENING_BRACKET ),
					LGS( MSG_CLOSING_BRACKET ),
					string7,
					LGS( MSG_TOPIC_CHANGED ),
                    string10);

            status_conductor->conductor = status_conductor->root;
            while (stricmp(status_conductor->conductor->name, string9))
            {
                status_conductor->conductor = status_conductor->conductor->next;
                if (!status_conductor->conductor)
                    break;
            }

            if (status_conductor->conductor)
            {
                add_text_to_conductor_list((char*) buffer3, 16, ACTIVITY);

                // change topic string gadget to new topic
                DoMethod((Object*) status_conductor->conductor->STR_topic, MUIM_KillNotify, MUIA_String_Acknowledge);
                setmacro((Object*) status_conductor->conductor->STR_topic, MUIA_String_Contents, string10);
                setmacro((Object*) status_conductor->conductor->STR_topic, MUIA_String_BufferPos, 0);
                DoMethod((Object*) status_conductor->conductor->STR_topic, MUIM_Notify, MUIA_String_Acknowledge,
                        MUIV_EveryTime, (Object*) WookieChat->App, 2, MUIM_Application_ReturnID, 29);

                strcpy(status_conductor->conductor->topic, string10);
                setmacro((Object*) status_conductor->conductor->STR_topic, MUIA_ShortHelp,
                        &status_conductor->conductor->topic);

            }
            else
            {
                add_text_to_current_list((char*) buffer3, 16, ACTIVITY);
            }

        }
        else if (!strcmp(incoming_2, "421") || !strcmp(incoming_2, "401") || !strcmp(incoming_2, "404")) //401 tried to send text to an unknown nick
        { //404 cant send to channel

            string1 = strtok(incoming_3, " ");
            string2 = strtok(NULL, "");

            if (string2)
            {
				sprintf(buffer3, "%s%s%s%s %s", timestamp,
						LGS( MSG_OPENING_BRACKET ),
						LGS( MSG_ERROR ),
						LGS( MSG_CLOSING_BRACKET ),
						string2);
                add_text_to_current_list((char*) buffer3, 9, ACTIVITY);
            }

        }
        else
        {

            string1 = strtok(incoming_3, " ");
            string2 = strtok(NULL, "");

            if (string1 && string2)
            {
                if (!stricmp(string1, status_conductor->nick))
					sprintf(buffer3, "%s%s%s%s %s", timestamp,
							LGS( MSG_OPENING_BRACKET ),
							LGS( MSG_SERVER ),
							LGS( MSG_CLOSING_BRACKET ),
							string2);
                else
					sprintf(buffer3, "%s%s%s%s %s %s", timestamp,
							LGS( MSG_OPENING_BRACKET ),
							LGS( MSG_SERVER ),
							LGS( MSG_CLOSING_BRACKET ),
							string1,
							string2);

                //status_conductor->conductor=status_conductor->root;
                for (status_conductor->conductor = status_conductor->root; status_conductor->conductor;
                        status_conductor->conductor = status_conductor->conductor->next)
                {
                    if (status_conductor->conductor->BT_querybutton)
                    {

                        if (status_conductor == status_current && !status_conductor->root->server_tab) //stricmp(status_conductor->root->displayed_name,"Status"))
                        {
                            add_text_to_current_list((char*) buffer3, colour, ACTIVITY);
                        }
                        else
                        {
                            add_text_to_conductor_list((char*) buffer3, colour, ACTIVITY);
                        }

                        char *find;
                        if ((find = strstr(buffer3, "/QUOTE PASS")))
                        {
                            strtok(find, " ");
                            string1 = strtok(NULL, " ");
                            string2 = strtok(NULL, " \r\n");

                            if (string2)
                            {
                                sprintf(sendstuff, "%s %s\r\n", string1, string2);
                                send_text(sendstuff);

                                sprintf(buffer3, "%s%s%s%s Automatically sending: %s", timestamp,
										LGS( MSG_OPENING_BRACKET ),
										LGS( MSG_SERVER ),
										LGS( MSG_CLOSING_BRACKET ),
										sendstuff);

                                if (status_conductor == status_current && !status_conductor->root->server_tab) //stricmp(status_conductor->root->displayed_name,"Status"))
                                {
                                    add_text_to_current_list((char*) buffer3, colour, ACTIVITY);
                                }
                                else
                                {
                                    add_text_to_conductor_list((char*) buffer3, colour, ACTIVITY);
                                }

                            }
                        }
                        //    /QUOTE PASS 39089

                        break;
                    }
                }

            }

            if (string1 && !strcmp(incoming_2, "251"))
            {

                if (stricmp(string1, status_conductor->nick))
                    strcpy(status_conductor->nick, string1);

            }
        }

        if (RECENTLY_CREATED_A_TAB == TRUE)
        {
            if (status_current)
            {
                if (status_current->current_query)
                {
                    setmacro((Object*) WookieChat->LV_tabs, MUIA_NList_Active,
                            status_current->current_query->nlist_tab_number);
                    sort_linked_list();
                }
            }

            RECENTLY_CREATED_A_TAB = FALSE;
        }

        // remove characters leading up to found character

        strcpy(status_conductor->str, pch + 2);

        process_incoming();

    }

}

void process_dcc_chat_incoming()
{

    /* make the default colour the "normal" text pen */
    LONG colour = 8;

    /* insert /r's where there are none */
    pch3 = strstr(dcc_chat_conductor->str, "\n");
    //pch3=strstr(dcc_chat_conductor->buffer, "\n");
    if (pch3)
    {

        /* copy chars leading up to searched char to a string */

        strncpy(buffer2, dcc_chat_conductor->str, pch3 - dcc_chat_conductor->str);
        buffer2[pch3 - dcc_chat_conductor->str] = '\0';

        //strncpy(buffer2,dcc_chat_conductor->buffer, pch3-dcc_chat_conductor->buffer);
        //buffer2[pch3-dcc_chat_conductor->buffer]='\0';

        buffer2[600] = '\0';
        timestamp_2_string();

        sprintf(buffer3, "%s<%s> %s", timestamp, dcc_chat_conductor->nick, buffer2);
        //printf("dcc chat pincoming:%s\n",buffer3);

        status_conductor->conductor = dcc_chat_conductor->conductor;
        add_text_to_conductor_list(buffer3, colour, ACTIVITY_CHAT);

        // remove characters leading up to found character

        strcpy(dcc_chat_conductor->str, pch3 + 1);
        //strcpy(dcc_chat_conductor->buffer, pch3+1);

        process_dcc_chat_incoming();

    }

}

void set_activity(const char * act, const char * actchat, const char * acthlight)
{
    memset(activity,'\0',63); strcpy(activity," "); strcat(activity, act);
    activity[0]='2'; activity[1]=':';
    memset(activity_chat,'\0',63); strcpy(activity_chat," "); strcat(activity_chat, actchat);
    activity_chat[0]='2'; activity_chat[1]=':';
    memset(activity_highlight,'\0',63); strcpy(activity_highlight," "); strcat(activity_highlight, acthlight);
    activity_highlight[0]='2'; activity_highlight[1]=':';
}

void pincoming_init()
{
    NewList(&slist.sl_List); // initialise the minlist
    InitSemaphore(&slist.sl_Semaphore); //initialise the semaphore, it can now be used
}
