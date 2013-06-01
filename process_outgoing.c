/*
    The contents of this file are subject to the AROS Public License Version 1.1 
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis, 
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
    for the specific language governing rights and limitations under the License.
*/

/* ===================================================================
                             Chat Input
   =================================================================== */

#include "includes.h"

#include "intern.h"
#include "objapp.h"
#include "audio.h"

/* Locals */
static char *pch2;
static char new_filtered_string1[550];
static char buffer3[BUFFERSIZE*2];
static char file_name[800];
#ifndef __AROS__
static STRPTR charset_convert_1;
static STRPTR charset_convert_2;
#endif

void censor_swear_words(char *string1)
{

    strcpy(new_filtered_string1, string1);

    for (unsigned int count = 0; count < strlen(string1); count++)
        string1[count] = ToLower(string1[count]);

    pch2 = strstr(string1, "shit");

    int where_to_put_stars[500];
    int where_to_put_stars_count = 0;

    while (pch2)
    {
        //where_to_put_stars[where_to_put_stars_count++]=pch2-string1+1;
        where_to_put_stars[where_to_put_stars_count++] = pch2 - string1 + 2;

        //string1[pch2-string1+1]='*';
        string1[pch2 - string1 + 2] = '*';

        pch2 = strstr(pch2, "shit");

    }

    pch2 = strstr(string1, "fuck");

    while (pch2)
    {
        where_to_put_stars[where_to_put_stars_count++] = pch2 - string1 + 1;
        //where_to_put_stars[where_to_put_stars_count++]=pch2-string1+2;

        string1[pch2 - string1 + 1] = '*';
        //string1[pch2-string1+2]='*';

        pch2 = strstr(pch2, "fuck");

    }

    pch2 = strstr(string1, "cunt");

    while (pch2)
    {
        where_to_put_stars[where_to_put_stars_count++] = pch2 - string1 + 1;
        //where_to_put_stars[where_to_put_stars_count++]=pch2-string1+2;

        string1[pch2 - string1 + 1] = '*';
        //string1[pch2-string1+2]='*';

        pch2 = strstr(pch2, "cunt");

    }

    pch2 = strstr(string1, "wank");

    while (pch2)
    {
        where_to_put_stars[where_to_put_stars_count++] = pch2 - string1 + 1;
        //where_to_put_stars[where_to_put_stars_count++]=pch2-string1+2;

        string1[pch2 - string1 + 1] = '*';
        //string1[pch2-string1+4]='*';

        pch2 = strstr(pch2, "wanker");

    }

    pch2 = strstr(string1, "dick");

    while (pch2)
    {
        where_to_put_stars[where_to_put_stars_count++] = pch2 - string1 + 1;

        string1[pch2 - string1 + 1] = '*';

        pch2 = strstr(pch2, "dick");

    }

    pch2 = strstr(string1, "bastard");

    while (pch2)
    {
        where_to_put_stars[where_to_put_stars_count++] = pch2 - string1 + 1;
        //where_to_put_stars[where_to_put_stars_count++]=pch2-string1+2;
        //where_to_put_stars[where_to_put_stars_count++]=pch2-string1+4;

        string1[pch2 - string1 + 1] = '*';
        //string1[pch2-string1+2]='*';
        //string1[pch2-string1+4]='*';

        pch2 = strstr(pch2, "bastard");

    }

    for (int count = 0; count < where_to_put_stars_count; count++)
    {
        new_filtered_string1[where_to_put_stars[count]] = '*';

    }

    strcpy(string1, new_filtered_string1);

}

void process_outgoing(char *string123, int usestringgadget)
{

    //printf("outgoing:%s\n",string123);
    colour = 8;

    if (usestringgadget)
    {
        status_conductor = status_current;

        status_conductor->current_query->string_conductor = status_conductor->current_query->string_root;

        while (status_conductor->current_query->string_conductor->next)
        {

            status_conductor->current_query->string_conductor = status_conductor->current_query->string_conductor->next;

        }

        if (strcmp(string123, ""))
        {

            strcpy(status_conductor->current_query->string_conductor->buffer_history, string123);

            struct history *work_history = status_conductor->current_query->string_conductor;
            status_conductor->current_query->string_conductor->next = malloc(sizeof(struct history));
            status_conductor->current_query->string_conductor = status_conductor->current_query->string_conductor->next;
            status_conductor->current_query->string_conductor->previous = work_history;

            status_conductor->current_query->string_conductor->next = NULL;
            status_conductor->current_query->string_conductor->buffer_history[0] = '\0';

        }

        status_conductor = status_current;
        status_conductor->conductor = status_current->current_query;
    }

    timestamp_2_string();

    char *pch;
    char *string1;
    char *string2;
    char *string3;

    if (string123[0] == ' ')
        pch = strtok(string123, "\r\n");
    else
        pch = strtok(string123, " \r\n");
    if (!(pch))
    {
        pch = malloc(sizeof(char));
        strcpy(pch, "");
    }

    if (!stricmp(pch, "/whois"))
    {
        string1 = strtok(NULL, "");

        if (string1)
            SWhoIs(string1);
        else
        {
            sprintf(buffer3, "%s%s%s%s /whois %s", timestamp, GCS(catalog, 217, "["),
                    GCS(catalog, 149, "Syntax"), GCS(catalog, 218, "]"),
                    GCS(catalog, 242, "<nick>"));

            add_text_to_conductor_list(buffer3, 9, ACTIVITY);
        }

    }
    else if (!stricmp(pch, "/activate"))
    {

        setmacro((Object*) WookieChat->WI_main, MUIA_Window_Open, TRUE);
        setmacro((Object*) WookieChat->WI_main, MUIA_Window_Activate, TRUE);
        DoMethod((Object*) WookieChat->WI_main, MUIM_Window_ScreenToFront);

    }
    else if (!stricmp(pch, "/catalog"))
    {
        string1 = strtok(NULL, " ");

        if (string1)
        {

            CloseCatalog(catalog);

            printf("loading catalog for language %s\n", string1);

            catalog = OpenCatalog(NULL, (loc_in)"WookieChat.catalog", OC_Language, string1, TAG_DONE);

            if (!catalog)
            {
                printf("unable to use default language\n");

                catalog = OpenCatalog(NULL, (loc_in)"WookieChat.catalog", OC_Language, "english", TAG_DONE);

                if (!catalog)
                    printf("unable to load english catalog, using built in strings\n");

            }
        }

    }
    else if (!stricmp(pch, "/invite"))
    {
        string1 = strtok(NULL, " "); //nick to invite
        string2 = strtok(NULL, " "); //channel to invite to

        timestamp_2_string();

        if (string1 && string2)
        {
            sprintf(buffer3, "INVITE %s :%s\r\n", string1, string2);
            send_text(buffer3);

            //sprintf(buffer3,"* Inviting %s to %s",string1,string2);
            //add_text_to_current_list(buffer3, 15, ACTIVITY);

        }
        else if (string1)
        {
            sprintf(buffer3, "INVITE %s :%s\r\n", string1, status_conductor->conductor->name); //current_query->name);
            send_text(buffer3);
            //sprintf(buffer3,"* Inviting %s to %s",string1,current_query->name);
            //add_text_to_current_list(buffer3, 15, ACTIVITY);

        }
        else
        {
            sprintf(buffer3, "%s%s%s%s /invite %s %s", timestamp, GCS(catalog, 217, "["),
                    GCS(catalog, 149, "Syntax"), GCS(catalog, 218, "]"),
                    GCS(catalog, 242, "<nick>"), GCS(catalog, 241, "<channel_name>"));
            add_text_to_conductor_list(buffer3, 9, ACTIVITY);
        }

    }
    else if (!stricmp(pch, "/dcc"))
    {
        string1 = strtok(NULL, " "); //send or chat?
        string2 = strtok(NULL, " "); //nick
        string3 = strtok(NULL, " "); //argument, filename or "chat"
        if (string1)
        {
            if (!stricmp(string1, "send"))
            {

                if (string3)
                    setmacro((Object*) WookieChat->PA_dcc_send_file, MUIA_String_Contents, string3);

                if (string2)
                    setmacro((Object*) WookieChat->STR_dcc_send_nick, MUIA_String_Contents, string2);

                if (string2 && string3)
                {
                    char work1[5];
                    strcpy(work1, "SEND");
                    offer_dcc(status_conductor->networkname, work1, string2, string3, 1025);
                }
                else
                    setmacro((Object*) WookieChat->WI_dcc_send, MUIA_Window_Open, TRUE);

            }
            if (!stricmp(string1, "chat"))
            {
                if (string2)
                {
                    initiate_outgoing_dcc_chat(string2);

                }

            }
        }
        else
        {
            sprintf(buffer3, "%s%s%s%s /dcc send %s %s", timestamp, GCS(catalog, 217, "["),
                    GCS(catalog, 149, "Syntax"), GCS(catalog, 218, "]"),
                    GCS(catalog, 242, "<nick>"), GCS(catalog, 255, "<filename>"));
            add_text_to_conductor_list(buffer3, 9, ACTIVITY);

            sprintf(buffer3, "%s%s%s%s /dcc chat %s", timestamp, GCS(catalog, 217, "["),
                    GCS(catalog, 149, "Syntax"), GCS(catalog, 218, "]"),
                    GCS(catalog, 242, "<nick>"));
            add_text_to_conductor_list(buffer3, 9, ACTIVITY);

        }

    }
    else if (!stricmp(pch, "/clip") || !stricmp(pch, "/clipboard"))
    {

        if (my_settings.which_clipboard_style == NORMAL)
        {
            my_settings.which_clipboard_style = COLUMNS;
            sprintf(buffer3, "%s%s%s%s Changing style to: Column marking", timestamp, GCS(catalog, 217, "["),
                    GCS(catalog, 800, "Clipboard"), GCS(catalog, 218, "]"));
            add_text_to_current_list(buffer3, 9, ACTIVITY);
            setmacro((Object*) MN_Clipboard, MUIA_Menuitem_Checked, TRUE);
        }
        else
        {
            my_settings.which_clipboard_style = NORMAL;
            sprintf(buffer3, "%s%s%s%s Changing style to: Normal", timestamp, GCS(catalog, 217, "["),
                    GCS(catalog, 800, "Clipboard"), GCS(catalog, 218, "]"));
            add_text_to_current_list(buffer3, 9, ACTIVITY);
            setmacro((Object*) MN_Clipboard, MUIA_Menuitem_Checked, FALSE);

        }
    }
    else if (!stricmp(pch, "/sound"))
    {

        string1 = strtok(NULL, " ");
        string2 = strtok(NULL, "");

        if (string1)
        {

            send_text((char*) "PRIVMSG ");

            if (string2)
                send_text(string2);
            else
                send_text(status_conductor->conductor->name);

            send_text((char*) " :\001SOUND ");
            send_text(string1);
            send_text((char*) "\001\r\n");

            if (string2)
                sprintf(buffer3, "%s%sSound%s %s %s to %s", timestamp, GCS(catalog, 217, "["),
                        GCS(catalog, 218, "]"), GCS(catalog, 212, "playing"), string1, string2);
            else
                sprintf(buffer3, "%s%sSound%s %s %s", timestamp, GCS(catalog, 217, "["),
                        GCS(catalog, 218, "]"), GCS(catalog, 212, "playing"), string1);

            add_text_to_conductor_list(buffer3, 6, ACTIVITY);

            play_external_sound_replayer(string1);

        }
        else
        {
            sprintf(buffer3, "%s%s%s%s /sound %s %s", timestamp, GCS(catalog, 217, "["),
                    GCS(catalog, 149, "Syntax"), GCS(catalog, 218, "]"),
                    GCS(catalog, 244, "<sound_sample_name>"), GCS(catalog, 242, "<nick>"));

            add_text_to_conductor_list(buffer3, 9, ACTIVITY);
        }

    }
    else if (!stricmp(pch, "/ctcp"))
    {
        string1 = strtok(NULL, " ");
        string2 = strtok(NULL, "");

        if (string1 && string2)
        {

            if (!stricmp(string2, "PING") && string1)
                SPing(string1);
            else
            {
                send_text((char*) "PRIVMSG ");
                send_text(string1);
                send_text((char*) " :\001");
                for (unsigned int a = 0; a < strlen(string2) + 1; a++)
                    string2[a] = ToUpper(string2[a]);
                send_text(string2);
                send_text((char*) "\001\r\n");

                sprintf(buffer3, "%s%sCTCP%s %s %s %s %s", timestamp, GCS(catalog, 217, "["),
                        GCS(catalog, 218, "]"), GCS(catalog, 213, "Sending CTCP"), string2,
                        GCS(catalog, 205, "to"), string1);
                add_text_to_conductor_list(buffer3, 6, ACTIVITY);

            }

        }
        else
        {
            sprintf(buffer3, "%s%s%s%s /ctcp %s %s", timestamp, GCS(catalog, 217, "["),
                    GCS(catalog, 149, "Syntax"), GCS(catalog, 218, "]"),
                    GCS(catalog, 242, "<nick>"), GCS(catalog, 243, "<command>"));

            add_text_to_conductor_list(buffer3, 9, ACTIVITY);
        }
    }
    else if (!stricmp(pch, "/clearchans"))
    {

        for (status_conductor = status_root; status_conductor; status_conductor = status_conductor->next)
        {
            for (status_conductor->conductor = status_conductor->root; status_conductor->conductor;
                    status_conductor->conductor = status_conductor->conductor->next)
            {
                if (!status_conductor->conductor->removed)
                {
                    if (status_conductor->conductor->LV_channel
                            && strcspn(status_conductor->conductor->name, status_conductor->chantypes) == 0)
                    {
                        status_conductor->conductor->entries_count = 0;

                        DoMethod((Object*) status_conductor->conductor->LV_channel, MUIM_NList_Clear);
                    }
                }
            }

        }

        status_conductor = status_current;
        status_conductor->conductor = status_conductor->current_query;
    }
    else if (!stricmp(pch, "/flush") || !stricmp(pch, "/clear"))
    {
        current_query->entries_count = 0;

        DoMethod((Object*) current_query->LV_channel, MUIM_NList_Clear);
    }
    else if (!stricmp(pch, "/op"))
    {

        string1 = strtok(NULL, ", ");

        if (string1)
        {
            //sprintf(sendstuff,"mode %s +o %s\r\n",current_query->name, string1);
            sprintf(sendstuff, "mode %s +o %s\r\n", status_conductor->conductor->name, string1);
            send_text(sendstuff);
        }
        else
        {
            sprintf(buffer3, "%s%s%s%s /op %s", timestamp, GCS(catalog, 217, "["),
                    GCS(catalog, 149, "Syntax"), GCS(catalog, 218, "]"),
                    GCS(catalog, 242, "<nick>"));
            add_text_to_conductor_list(buffer3, 9, ACTIVITY);
        }
    }
    else if (!stricmp(pch, "/deop"))
    {

        string1 = strtok(NULL, ", ");

        if (string1)
        {
            sprintf(sendstuff, "mode %s -o %s\r\n", status_conductor->conductor->name, string1);
            send_text(sendstuff);
        }
        else
        {
            sprintf(buffer3, "%s%s%s%s /deop %s", timestamp, GCS(catalog, 217, "["),
                    GCS(catalog, 149, "Syntax"), GCS(catalog, 218, "]"),
                    GCS(catalog, 242, "<nick>"));
            add_text_to_conductor_list(buffer3, 9, ACTIVITY);
        }
    }
    else if (!stricmp(pch, "/ban"))
    {

        string1 = strtok(NULL, " ");
        if (string1)
        {

            sprintf(sendstuff, "mode %s +b %s\r\n", status_conductor->conductor->name, string1);
            send_text(sendstuff);

        }
        else
        {
            sprintf(buffer3, "%s%s%s%s /ban %s", timestamp, GCS(catalog, 217, "["),
                    GCS(catalog, 149, "Syntax"), GCS(catalog, 218, "]"),
                    GCS(catalog, 245, "<ban_mask>"));
            add_text_to_conductor_list(buffer3, 9, ACTIVITY);
        }
    }
    else if (!stricmp(pch, "/unban"))
    {

        string1 = strtok(NULL, " ");
        if (string1)
        {

            sprintf(sendstuff, "mode %s -b %s\r\n", status_conductor->conductor->name, string1);
            send_text(sendstuff);

        }
        else
        {
            sprintf(buffer3, "%s%s%s%s /unban %s", timestamp, GCS(catalog, 217, "["),
                    GCS(catalog, 149, "Syntax"), GCS(catalog, 218, "]"),
                    GCS(catalog, 245, "<ban_mask>"));

            add_text_to_conductor_list(buffer3, 9, ACTIVITY);
        }
    }
    else if (!stricmp(pch, "/kick"))
    {

        string1 = strtok(NULL, " ");
        string2 = strtok(NULL, "");

        if (string1)
        {
            if (string2)
                sprintf(sendstuff, "kick %s %s :%s\r\n", status_current->current_query->name, string1, string2);
            else
                sprintf(sendstuff, "kick %s %s :%s\r\n", status_current->current_query->name, string1,
                        my_settings.default_kickmsg);

            send_text(sendstuff);
        }
        else
        {
            sprintf(buffer3, "%s%s%s%s /kick %s %s", timestamp, GCS(catalog, 217, "["),
                    GCS(catalog, 149, "Syntax"), GCS(catalog, 218, "]"),
                    GCS(catalog, 242, "<nick>"), GCS(catalog, 246, "<message>"));

            add_text_to_conductor_list(buffer3, 9, ACTIVITY);
        }
    }
    else if (!stricmp(pch, "/cycle"))
    {
        //add_text_to_current_list("* Cycling..",9);

        sprintf(buffer3, "PART %s :Cycling..\r\n", current_query->name);
        send_text(buffer3);
        sprintf(buffer3, "JOIN %s\r\n", current_query->name);
        send_text(buffer3);

    }
    else if (!stricmp(pch, "/a"))
    {

        strcpy(buffer3, "[12:32:02] <sundown> Anyone seen jahc? where is he!! jahc is such a tosser");

        char *test_search = malloc(sizeof(char) * 10);
        strcpy(test_search, "jahc");

        search_for_highlight_words(test_search, buffer3);

        //sort_linked_list();

    }
    else if (!stricmp(pch, "/b"))
    {


        int recv_thing = recv(status_conductor->a_socket, (i_in) status_conductor->buffer, 512, 0);

        if (recv_thing != -1)
        {
            status_conductor->buffer[recv_thing] = '\0';
            strcat(status_conductor->str, status_conductor->buffer);
        }

        if (recv_thing > 0)
        {
            sprintf(buffer3, "%s%s%s%s recv() text: \"%s\"", timestamp, GCS(catalog, 217, "["),
                    GCS(catalog, 149, "Syntax"), GCS(catalog, 218, "]"), status_conductor->str);
        }
        else
        {
            sprintf(buffer3, "%s%s%s%s No text to recv(): \"%s\"", timestamp, GCS(catalog, 217, "["),
                    GCS(catalog, 149, "Syntax"), GCS(catalog, 218, "]"), status_conductor->str);
        }

        add_text_to_conductor_list(buffer3, 9, ACTIVITY);

    }

    else if (!stricmp(pch, "/away"))
    {
        string1 = strtok(NULL, "");
        //global status changes (set all networks away at once),

        if (string1)
            sprintf(sendstuff, "away :%s\r\n", string1);
        else
            sprintf(sendstuff, "away :\r\n");

        for (status_conductor = status_root; status_conductor; status_conductor = status_conductor->next)
        {
            if (status_conductor == status_current)
                status_conductor->conductor = status_conductor->current_query;
            else
                status_conductor->conductor = status_conductor->root;

            status_conductor->away = TRUE;

            if (status_conductor->connection_active)
                send_text(sendstuff);
        }

        status_conductor = status_current;

    }

    else if (!stricmp(pch, "/ping"))
    {
        string1 = strtok(NULL, "");

        if (string1)
            SPing(string1);
        else
        {
            sprintf(buffer3, "%s%s%s%s /ping %s", timestamp, GCS(catalog, 217, "["),
                    GCS(catalog, 149, "Syntax"), GCS(catalog, 218, "]"),
                    GCS(catalog, 242, "<nick>"));
            add_text_to_conductor_list(buffer3, 9, ACTIVITY);

        }
    }
    else if (!stricmp(pch, "/tab"))
    {
        create_new_status(0);
    }
    else if (!stricmp(pch, "/version"))
    {
        string1 = strtok(NULL, "");

        if (string1)
        {
            sprintf(sendstuff, "PRIVMSG %s :\001VERSION\001\r\n", string1);
            sprintf(buffer3, "%s-> [%s] VERSION", timestamp, string1);

            send_text(sendstuff);
            add_text_to_conductor_list(buffer3, 6, ACTIVITY);
        }
        else
        {

            sprintf(buffer3, "%s%s%s%s /version %s", timestamp, GCS(catalog, 217, "["),
                    GCS(catalog, 149, "Syntax"), GCS(catalog, 218, "]"),
                    GCS(catalog, 242, "<nick>"));
            add_text_to_conductor_list(buffer3, 9, ACTIVITY);
        }

    }
    else if (!stricmp(pch, "/time"))
    {
        string1 = strtok(NULL, "");
        if (string1)
        {
            sprintf(sendstuff, "PRIVMSG %s :\001TIME\001\r\n", string1);
            sprintf(buffer3, "%s-> [%s] TIME", timestamp, string1);

            send_text(sendstuff);
            add_text_to_conductor_list(buffer3, 6, ACTIVITY);

        }
        else
        {
            sprintf(buffer3, "%s%s%s%s /time %s", timestamp, GCS(catalog, 217, "["),
                    GCS(catalog, 149, "Syntax"), GCS(catalog, 218, "]"),
                    GCS(catalog, 242, "<nick>"));

            add_text_to_conductor_list(buffer3, 9, ACTIVITY);

        }

    }

    else if (!stricmp(pch, "/settings"))
    {
        DoMethod((Object*) WookieChat->App, MUIM_Application_OpenConfigWindow, 0);
    }
    else if (!(stricmp(pch, "/query")))
    {
        string1 = strtok(NULL, " ");
        string2 = strtok(NULL, "");

        if (string1)
        {
            SQuery(string1);
        }
        else
        {
            sprintf(buffer3, "%s%s%s%s /query %s", timestamp, GCS(catalog, 217, "["),
                    GCS(catalog, 149, "Syntax"), GCS(catalog, 218, "]"),
                    GCS(catalog, 242, "<nick>"));
            add_text_to_conductor_list(buffer3, 9, ACTIVITY);

        }
        if (string1 && string2)
        {
            if (string2)
            {
                /*
                 #ifndef __AROS__
                 struct TagItem my_send_charset1_taglist[] = { {CSA_Source, (ULONG)string2 },
                 {CSA_SourceCodeset, (ULONG)charsets[local_charset]}, {CSA_DestCodeset, (ULONG)charsets[status_conductor->remote_charset]}, {TAG_DONE, 0} };
                 charset_convert_1 = CodesetsConvertStrA(my_send_charset1_taglist);

                 if(charset_convert_1)
                 {
                 sprintf(string_to_send,"PRIVMSG %s :%s\r\n",(char*)string1, (char*)charset_convert_1);
                 sprintf(buffer3,"%s<%s> %s",timestamp,status_conductor->nick, (char*)charset_convert_1);
                 CodesetsFreeA(charset_convert_1,NULL);
                 }
                 else
                 {
                 sprintf(string_to_send,"PRIVMSG %s :%s\r\n",(char*)string1, (char*)string2);
                 sprintf(buffer3,"%s<%s> %s",timestamp,status_conductor->nick, string2);

                 }
                 #else
                 sprintf(string_to_send,"PRIVMSG %s :%s\r\n",(char*)string1, (char*)string2);
                 sprintf(buffer3,"%s<%s> %s",timestamp,status_conductor->nick, string2);
                 #endif
                 */

                sprintf(string_to_send, "PRIVMSG %s :%s\r\n", (char*) string1, (char*) string2);
                sprintf(buffer3, "%s<%s> %s", timestamp, status_conductor->nick, string2);

                send_text(string_to_send);
                add_text_to_conductor_list((char*) buffer3, 10, ACTIVITY);

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

    }

    else if (!(stricmp(pch, "/nick")))
    {
        string1 = strtok(NULL, "");

        if (string1)
            ChangeMyNick(string1);
        else
        {
            sprintf(buffer3, "%s%s%s%s /nick %s", timestamp, GCS(catalog, 217, "["),
                    GCS(catalog, 149, "Syntax"), GCS(catalog, 218, "]"),
                    GCS(catalog, 247, "<new nick>"));

            add_text_to_conductor_list(buffer3, 9, ACTIVITY);
        }
    }

    else if (!(stricmp(pch, "/j")) || !(stricmp(pch, "/join")))
    {
        string1 = strtok(NULL, "");
        if (string1)
        {
            /*if(string1[0]!='#') sprintf(string7,"#%s",string1);
             else strcpy(string7,string1);
             */

            /*if(strcspn(string1,status_conductor->chantypes)!=0) sprintf(string7,"#%s",string1);
             else strcpy(string7,string1);
             */

            if (string1)
                SJoin(string1);
        }
        //if(string1) SJoin(string1);
        else
        {
            sprintf(buffer3, "%s%s%s%s /join %s", timestamp, GCS(catalog, 217, "["),
                    GCS(catalog, 149, "Syntax"), GCS(catalog, 218, "]"),
                    GCS(catalog, 241, "<channel_name>"));

            add_text_to_conductor_list(buffer3, 9, ACTIVITY);
        }
    }

    else if (!(stricmp(pch, "/m")) || !(stricmp(pch, "/msg")))
    {
        string1 = strtok(NULL, " "); //nick
        string2 = strtok(NULL, ""); //message
        if (string1 && string2)
        {
            if (!stricmp(string1, "."))
                SMsg(status_conductor->last_msg_nick, string2);
            else
                SMsg(string1, string2);

        }
        else
        {
            sprintf(buffer3, "%s%s%s%s /msg %s %s", timestamp, GCS(catalog, 217, "["),
                    GCS(catalog, 149, "Syntax"), GCS(catalog, 218, "]"),
                    GCS(catalog, 242, "<nick>"), GCS(catalog, 246, "<message>"));
            add_text_to_conductor_list(buffer3, 9, ACTIVITY);
        }
    }

    else if (!(stricmp(pch, "/names")))
    {
        string1 = strtok(NULL, " ");
        if (string1)
            SNames(string1);
        else
            SNames(status_current->current_query->name);

        /*{

         sprintf(buffer3,"%s%s%s%s /names %s",timestamp,GCS(catalog,217,"["),GCS(catalog,149,"Syntax"),GCS(catalog,218,"]"),GCS(catalog,241,"<channel_name>"));

         add_text_to_conductor_list(buffer3,9, ACTIVITY);
         } */

    }
    else if (!(stricmp(pch, "/notice")))
    {
        string1 = strtok(NULL, " ");
        string2 = strtok(NULL, "");

        if (string1 && string2)
        {
            if (!stricmp(string1, "."))
                SNotice(status_conductor->last_notice_nick, string2);
            else
                SNotice(string1, string2);

        }
        else
        {
            sprintf(buffer3, "%s%s%s%s /notice %s %s", timestamp, GCS(catalog, 217, "["),
                    GCS(catalog, 149, "Syntax"), GCS(catalog, 218, "]"),
                    GCS(catalog, 254, "<nick or channel>"), GCS(catalog, 246, "<message>"));

            add_text_to_conductor_list(buffer3, 9, ACTIVITY);
        }
    }
    else if (!(stricmp(pch, "/topic")))
    {
        string1 = strtok(NULL, "");

        if (string1)
            ChangeTopic(string1);
        else
        {
            sprintf(string_to_send, "TOPIC %s\r\n", status_conductor->conductor->name);
            send_text(string_to_send);

        }
    }
    else if (!(stricmp(pch, "/p")) || !(stricmp(pch, "/part")))
    {
        string1 = strtok(NULL, "");
        //if(status_conductor->conductor->name[0]=='#') SPart(string1);
        if (strcspn(status_conductor->conductor->name, status_conductor->chantypes) == 0)
            SPart(string1);
        else
            close_tab();

        /*status_conductor=status_current;
         if(status_current->current_query->name[0]=='#') SPart(string1);
         else close_tab();
         */
    }
    else if (!(stricmp(pch, "/quit")))
    {
        string1 = strtok(NULL, "");
        if (!(string1))
        {
            string1 = malloc(sizeof(char));
            strcpy(string1, "");
        }

        SQuit(string1);

    }

    else if (!(stricmp(pch, "/me")))
    {
        string1 = strtok(NULL, "");

        if (string1)
            SAction(string1);
        else
        {
            sprintf(buffer3, "%s%s%s%s /me %s", timestamp, GCS(catalog, 217, "["),
                    GCS(catalog, 149, "Syntax"), GCS(catalog, 218, "]"),
                    GCS(catalog, 256, "<action>"));

            add_text_to_conductor_list(buffer3, 9, ACTIVITY);
        }
    }

    else if (!(stricmp(pch, "/server")))
    {

        char *servername = malloc(sizeof(char) * 50);
        servername = strtok(NULL, " ");

        char *portnumber = malloc(sizeof(char) * 7);
        portnumber = strtok(NULL, "");

        if (!portnumber)
        {
            portnumber = malloc(sizeof(char) * 7);
            strcpy(portnumber, "6667");
        }

        if (servername)
        {
            strcpy(server_name, "");
            strcpy(port_number, "");
            strcpy(server_charset, "");
            strcpy(nick_password, "");
            strcpy(server_password, "");
            strcpy(auto_joins, "");
            strcpy(auto_connect, "");

            connect2server(servername, portnumber, 1, 1, CONNECT_IN_CURRENT_TAB, NULL, 0);

        }
        else
        {
            sprintf(buffer3, "%s%s%s%s /server %s %s", timestamp, GCS(catalog, 217, "["),
                    GCS(catalog, 149, "Syntax"), GCS(catalog, 218, "]"),
                    GCS(catalog, 249, "<servername>"), GCS(catalog, 250, "<port>"));

            add_text_to_conductor_list(buffer3, 9, ACTIVITY);
        }

    }

    else if (!(stricmp(pch, "/close")))
    {
        close_tab();
    }
    else if (!(stricmp(pch, "/filter")))
    {
        status_conductor->filter_swear_words = 1 - status_conductor->filter_swear_words;
        if (status_conductor->filter_swear_words == 1)
        {
            sprintf(buffer3, "%s%s%s%s %s", timestamp, GCS(catalog, 217, "["),
                    GCS(catalog, 251, "SwearFilter"), GCS(catalog, 218, "]"),
                    GCS(catalog, 252, "Enabled"));

            add_text_to_conductor_list(buffer3, 9, ACTIVITY);
        }
        else
        {
            sprintf(buffer3, "%s%s%s%s %s", timestamp, GCS(catalog, 217, "["),
                    GCS(catalog, 251, "SwearFilter"), GCS(catalog, 218, "]"),
                    GCS(catalog, 253, "Disabled"));

            add_text_to_conductor_list(buffer3, 9, ACTIVITY);
        }

    }

    else if (!(stricmp(pch, "/quote")) || !(stricmp(pch, "/raw")))
    {
        string1 = strtok(NULL, "");

        if (string1)
            SQuote(string1);

    }
    else if (!(stricmp(pch, "/playing")) || !(stricmp(pch, "/p"))
            || !(stricmp(pch, "/tunenet")))
    {

        char work_buffer[400];
        char work_buffer2[100];
        //memset(work_buffer,'\0',399);
        //memset(work_buffer2,'\0',99);

        BPTR tunenet_status = Open((_s_cs)"env:tunenet/status", MODE_OLDFILE);
        BPTR tunenet_playing = Open((_s_cs)"env:tunenet/playing", MODE_OLDFILE);
        if (tunenet_playing && tunenet_status)
        {

            if (!FGets(tunenet_playing, (STRPTR)work_buffer, 300))
                strcpy(work_buffer, "(none)");

            if (!FGets(tunenet_status, (STRPTR)work_buffer2, 300))
                strcpy(work_buffer2, "(none)");

            sprintf(string_to_send, "PRIVMSG %s :[%s] %s\r\n", status_conductor->conductor->name, work_buffer2,
                    work_buffer);
            sprintf(buffer3, "%s<%s> [%s] %s", timestamp, status_conductor->nick, work_buffer2, work_buffer);

            add_text_to_conductor_list(buffer3, 10, ACTIVITY);
            send_text(string_to_send);

        }

        if (tunenet_status)
            Close(tunenet_status);
        if (tunenet_playing)
            Close(tunenet_playing);

    }
    else if (!(stricmp(pch, "/say")))
    {

        string1 = strtok(NULL, "\0");
        if (!(string1))
        {
            string1 = malloc(sizeof(char));
            strcpy(string1, "");
        }

        if (!status_conductor)
            status_conductor = status_current;
        /*if(status_conductor->filter_swear_words==1)
         {
         censor_swear_words(pch);      //do the first word
         censor_swear_words(string1);  //then do the rest of the sentence
         } */

        if (!(strcmp("Status", status_conductor->conductor->name)))
        {
            sprintf(buffer3, "%s %s", timestamp, string1);
            sprintf(string_to_send, "%s\r\n", string1);
        }
        else
        {
            sprintf(string_to_send, "PRIVMSG %s :%s\r\n", status_conductor->conductor->name, string1);
            sprintf(buffer3, "%s<%s> %s", timestamp, status_conductor->nick, string1);

        }

        add_text_to_conductor_list(buffer3, 10, ACTIVITY);
        //printf("string to send:%s",string_to_send);

        send_text(string_to_send);

        //send_text(string_to_send);

    }
    else if (!(stricmp(pch, "/rx")))
    {
        string1 = strtok(NULL, "\0");
        if (string1)
        {

            sprintf(file_name, "run >nil: rx %s", string1);
            if (DEBUG)
                printf("%s\n", file_name);

            BPTR arexx_dir_lock = Lock((_s_cs)"progdir:arexx_scripts", SHARED_LOCK);
            if (arexx_dir_lock)
            {
#ifdef __AROS__
                struct TagItem my_arexx_command_taglist[] =
                {
                    { NP_CurrentDir, (IPTR) arexx_dir_lock },
                    { TAG_DONE, (IPTR) 0 }
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

        }
    }
    else if (!(stricmp(pch, "/echo")))
    {

        string1 = strtok(NULL, " ");
        string2 = strtok(NULL, "\0");

        if (string1 && string2)
        {
            sprintf(buffer3, "%s%s", timestamp, string2);
            add_text_to_conductor_list(buffer3, atoi(string1), ACTIVITY);
        }
        else if (string1)
        {
            sprintf(buffer3, "%s%s", timestamp, string1);
            add_text_to_conductor_list(buffer3, 9, ACTIVITY);
        }

    }

    else if (!(stricmp(pch, "/sayall")))
    {

        string1 = strtok(NULL, "\0");
        if (!(string1))
        {
            string1 = malloc(sizeof(char));
            strcpy(string1, "");
        }

        for (status_conductor = status_root; status_conductor; status_conductor = status_conductor->next)
        {
            for (status_conductor->conductor = status_conductor->root; status_conductor->conductor;
                    status_conductor->conductor = status_conductor->conductor->next)
            {
                //if(status_conductor->conductor->name[0]=='#')
                if (strcspn(status_conductor->conductor->name, status_conductor->chantypes) == 0)
                {
                    sprintf(string_to_send, "PRIVMSG %s :%s\r\n", status_conductor->conductor->name, string1);
                    sprintf(buffer3, "%s<%s> %s", timestamp, status_conductor->nick, string1);
                    add_text_to_conductor_list(buffer3, 10, ACTIVITY);
                    send_text(string_to_send);

                }
            }
            //status_conductor->conductor=status_conductor->root;

        }

        status_conductor = status_current;
        status_conductor->conductor = status_conductor->root;

    }

    else if (pch[0] == '/' && pch[1] != '/')
    {
        string1 = malloc(sizeof(char) * 400);
        int a = 0;
        for (a = 0; pch[a] != '\0'; a++)
        {

            string1[a] = pch[a + 1];

        }

        string2 = strtok(NULL, "\0");

        if (string2)
        {
            sprintf(buffer3, "%s %s\r\n", string1, string2);
            send_text(buffer3);
        }
        else if (string1)
        {
            sprintf(buffer3, "%s\r\n", string1);
            send_text(buffer3);

        }
    }
    else if (!(pch))
    {

    }
    else if (status_conductor->current_query->name[0] == '$')
    {
        dcc_chat_conductor = dcc_chat_root->next;
        while (dcc_chat_conductor)
        {
            //if(dcc_chat_conductor->conductor==status_conductor->conductor)
            if (dcc_chat_conductor->conductor)
            {
                if (!stricmp(dcc_chat_conductor->conductor->name, status_conductor->conductor->name))
                {
                    string1 = strtok(NULL, "\0");
                    if (string1)
                    {
                        sprintf(buffer3, "%s<%s> %s %s", timestamp, dcc_chat_conductor->own_nick, pch, string1);
                        sprintf(string_to_send, "%s %s\r\n", pch, string1);
                    }
                    else
                    {
                        sprintf(buffer3, "%s<%s> %s", timestamp, dcc_chat_conductor->own_nick, pch);
                        sprintf(string_to_send, "%s\r\n", pch);

                    }

                    add_text_to_conductor_list(buffer3, 10, ACTIVITY);
                    send_dcc_chat(string_to_send);
                    break;

                }
            }
            dcc_chat_conductor = dcc_chat_conductor->next;

        }

    }
    else
    {

        string1 = strtok(NULL, "\0");

        if (status_conductor->filter_swear_words == 1)
        {
            censor_swear_words(pch); //do the first word
            if (string1)
                censor_swear_words(string1); //then do the rest of the sentence
        }

        if (!(strcmp("Status", status_conductor->conductor->name)))
        {
            if (string1)
            {
                sprintf(buffer3, "%s%s %s", timestamp, pch, string1);
                sprintf(string_to_send, "%s %s\r\n", pch, string1);
            }
            else
            {
                sprintf(buffer3, "%s%s", timestamp, pch);
                sprintf(string_to_send, "%s\r\n", pch);

            }

        }
        else
        {
            char modes[10];
            strcpy(modes, "");

            if (status_conductor->conductor && my_settings.user_modes_beside_nicks)
            {
                for (int count = 0; count < status_conductor->conductor->nicks; count++)
                {
                    if (!stricmp(status_conductor->conductor->nicklist[count].name, status_conductor->nick))
                    {
                        strcpy(modes, status_conductor->conductor->nicklist[count].modes);
                        if (modes[0] == ' ')
                            strcpy(modes, "");
                        break;
                    }
                }
            }

            if (string1 && pch)
            {

                sprintf(string_to_send, "PRIVMSG %s :%s %s\r\n", status_conductor->conductor->name, (char*) pch,
                        (char*) string1);
                sprintf(buffer3, "%s<%s%s> %s %s", timestamp, modes, status_conductor->nick, (char*) pch,
                        (char*) string1);
            }
            else if (pch)
            {
                sprintf(string_to_send, "PRIVMSG %s :%s\r\n", status_conductor->conductor->name, (char*) pch);
                sprintf(buffer3, "%s<%s%s> %s", timestamp, modes, status_conductor->nick, (char*) pch);
            }

        }

        add_text_to_conductor_list(buffer3, 10, ACTIVITY);

        send_text(string_to_send);

    }

    if (usestringgadget)
    {
        setmacro((Object*) WookieChat->STR_usertext, MUIA_String_Contents, "");
        setmacro((Object*) WookieChat->WI_main, MUIA_Window_ActiveObject, (Object*) WookieChat->STR_usertext);
    }

}
