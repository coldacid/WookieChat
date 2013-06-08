/*
    The contents of this file are subject to the AROS Public License Version 1.1 
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis, 
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
    for the specific language governing rights and limitations under the License.
*/

/* ===================================================================
                           Nicklist Functions
   =================================================================== */

#include "includes.h"

#include "intern.h"

/* Locals */
static char windowtitlestring[110];
static struct list_entry *new_entry;

void update_nicks_hostname(char *nick, char *hostname)
{

    //if(DEBUG) printf("new hostname for %s is %s\n",nick,hostname);

    if (!nick || !hostname)
        return;

    for (int count = 0; count < status_conductor->conductor->nicks; count++)
    {

        if (!stricmp(status_conductor->conductor->nicklist[count].name, nick))
        {

            if (!status_conductor->conductor->nicklist[count].hostname)
            {
                status_conductor->conductor->nicklist[count].hostname = malloc(sizeof(char) * HOSTNAME_STRING_SIZE);
                strcpy(status_conductor->conductor->nicklist[count].hostname, "");
            }

            strncpy(status_conductor->conductor->nicklist[count].hostname, hostname, HOSTNAME_STRING_SIZE - 1);
            status_conductor->conductor->nicklist[count].hostname[HOSTNAME_STRING_SIZE] = '\0';

            //if(DEBUG) printf("nicklist hostname is now: %s\n",status_conductor->conductor->nicklist[count].hostname);

            break;
        }

    }

}

void change_window_titlebar_text(void)
{
    if (!status_conductor)
        return;
    if (!status_conductor->conductor)
        return;

    if (status_conductor->conductor == status_current->current_query)
    {
        if (status_conductor->conductor->nicks == 0)
            sprintf(windowtitlestring, "WookieChat - %s %s %s - %s", status_conductor->nick,
                    GCS(177, "connected to"), status_conductor->servername,
                    status_conductor->conductor->name);
        else
            sprintf(windowtitlestring, "WookieChat - %s %s %s - %s [%i %s]", status_conductor->nick,
                    GCS(177, "connected to"), status_conductor->servername,
                    status_conductor->conductor->name, status_conductor->conductor->nicks,
                    GCS(257, "users"));

        setmacro((Object*) WookieChat->WI_main, MUIA_Window_Title, windowtitlestring);
    }

}

void DisplayNicks(void)
{

    //printf("DisplayNicks()\n");
    if (!status_conductor)
        return;
    if (!status_conductor->conductor)
        return;
    if (!status_conductor->conductor->LV_nicklist)
        return;

    if (status_current->current_query == status_conductor->conductor)
        setmacro((Object*) status_conductor->conductor->LV_nicklist, MUIA_NList_Quiet, TRUE);

    if (!new_entry->hostname)
        new_entry->hostname = malloc(sizeof(char) * (HOSTNAME_STRING_SIZE + 1));

    DoMethod((Object*) status_conductor->conductor->LV_nicklist, MUIM_NList_Clear);

    status_conductor->conductor->nicks_ops = 0;

    for (int count1 = 1; count1 <= status_conductor->max_modes; count1++)
    {
        int a = 0;

        while (a < status_conductor->conductor->nicks)
        {
            strcpy(new_entry->name, status_conductor->conductor->nicklist[a].name);
            strcpy(new_entry->modes, status_conductor->conductor->nicklist[a].modes);
            if (!status_conductor->conductor->nicklist[a].hostname)
                status_conductor->conductor->nicklist[a].hostname = malloc(sizeof(char) * (HOSTNAME_STRING_SIZE + 1));
            if (status_conductor->conductor->nicklist[a].hostname)
            {
                strncpy(new_entry->hostname, status_conductor->conductor->nicklist[a].hostname,
                        HOSTNAME_STRING_SIZE - 1);
                new_entry->hostname[HOSTNAME_STRING_SIZE] = '\0';
            }
            else
                strcpy(new_entry->hostname, "");

            if (new_entry)
            {
                if (new_entry->name)
                {
                    //lets loop through the modes supported by the server, and if the user has the mode, lets add them.
                    //so for example, all the channel ops first, then the half ops users second, then the voiced users third, then later
                    //all the regular users with no status/mode
                    if (new_entry->modes[0] == status_conductor->user_modes[count1].symbol[0])
                    {
                        DoMethod((Object*) status_conductor->conductor->LV_nicklist, MUIM_NList_InsertSingle, new_entry,
                                MUIV_NList_Insert_Bottom); //,WRAPCOL0,ALIGN_LEFT);
                    }
                }
            }
            a++;
        }

    }

    //printf("b\n");
    int a = 0;

    while (a < status_conductor->conductor->nicks)
    {

        strcpy(new_entry->name, status_conductor->conductor->nicklist[a].name);
        strcpy(new_entry->modes, status_conductor->conductor->nicklist[a].modes);

        if (status_conductor->conductor->nicklist[a].hostname)
        {
            strncpy(new_entry->hostname, status_conductor->conductor->nicklist[a].hostname, HOSTNAME_STRING_SIZE - 1);
            new_entry->hostname[HOSTNAME_STRING_SIZE] = '\0';

        }
        else
            strcpy(new_entry->hostname, "");

        if (new_entry->name)
        {
            if (new_entry->modes[0] == '@')
                status_conductor->conductor->nicks_ops++;
        }

        if (new_entry->name)
        {
            int result = 0;

            for (int count1 = 1; count1 <= status_conductor->max_modes; count1++)
            {
                if (new_entry->modes[0] == status_conductor->user_modes[count1].symbol[0])
                    result = 1;

            }

            if (result == 0) //if these users do not have an operator/voice/halfops etc status, then they are added to the bottom of our list
                             //but if they do have some sort of status/mode, then they're ignored here, because we already added them at the top
            {
                DoMethod((Object*) status_conductor->conductor->LV_nicklist, MUIM_NList_InsertSingle, new_entry,
                        MUIV_NList_Insert_Bottom, WRAPCOL0, ALIGN_LEFT);
            }
        }
        a++;
    }

    if (status_current->current_query == status_conductor->conductor)
        setmacro((Object*) status_conductor->conductor->LV_nicklist, MUIA_NList_Quiet, FALSE);

    return;

}

int result = 0;

void AddNick(char *nick, char *hostname)
{

    struct list_entry *work_entry3 = malloc(sizeof(struct list_entry));
    work_entry3->hostname = malloc(sizeof(char) * (HOSTNAME_STRING_SIZE + 1));
    strcpy(work_entry3->modes, " ");
    strcpy(work_entry3->name, nick);

    ULONG entries = status_conductor->conductor->nicks;
    unsigned long a = 0, b = 0;

    strncpy(work_entry3->hostname, hostname, HOSTNAME_STRING_SIZE - 1);
    work_entry3->hostname[HOSTNAME_STRING_SIZE] = '\0';

    //in the nicklist, the top entries have ops and voice etc. We want to move past that section into
    //the area of regular users, with no channel status modes.
    while (a < entries && b != 1)
    {
        struct list_entry *work_entry = NULL;
        DoMethod((Object*) status_conductor->conductor->LV_nicklist, MUIM_NList_GetEntry, a, &work_entry);

        result = 0;
        if (!work_entry)
            break;

        for (int count = 1; count <= status_conductor->max_modes; count++)
        {
            if (work_entry->modes[0] == status_conductor->user_modes[count].symbol[0])
            {
                result = 1;
                break;
            }

        }

        if (result == 1)
            a++;
        else
            b = 1;

    }

    while (a != entries)
    {
        struct list_entry *work_entry = NULL;
        DoMethod((Object*) status_conductor->conductor->LV_nicklist, MUIM_NList_GetEntry, a, &work_entry);

        if (work_entry && work_entry3)
        {
            if (stricmp(work_entry3->name, work_entry->name) < 0)
                break;
        }
        else
            break;
        a++;
    }

    DoMethod((Object*) status_conductor->conductor->LV_nicklist, MUIM_NList_InsertSingle, work_entry3, a, WRAPCOL0,
            ALIGN_LEFT);

    change_window_titlebar_text();

    if (work_entry3->hostname)
        free(work_entry3->hostname);
    work_entry3->hostname = NULL;
    free(work_entry3);
    work_entry3 = NULL;

}

int ChangeNick(char *oldnick, char *newnick, char *buffer3)
{

    ULONG entries = status_conductor->conductor->nicks;

    unsigned int a = 0;

    while (a < entries)
    {
        if (!stricmp(oldnick, status_conductor->conductor->nicklist[a].name))
        {
            strcpy(status_conductor->conductor->nicklist[a].name, newnick);
            add_text_to_conductor_list((char*) buffer3, 15, ACTIVITY);
            sort_nicks(1);
            DisplayNicks();

        }

        a++;
    }

    return 0;

}

int ReplaceNicksWithVoiceOps(char *string1, char prefix, int option)
{
    ULONG entries;

    getmacro((Object*) status_conductor->conductor->LV_nicklist, MUIA_NList_Entries, &entries);

    unsigned int a = 0;

    while (a < entries)
    {
        struct list_entry *work_entry = NULL;
        DoMethod((Object*) status_conductor->conductor->LV_nicklist, MUIM_NList_GetEntry, a, &work_entry);

        strcpy(new_entry->modes, work_entry->modes);
        strcpy(new_entry->name, work_entry->name);
        if (!new_entry->hostname)
            new_entry->hostname = malloc(sizeof(char) * (HOSTNAME_STRING_SIZE + 1));
        if (work_entry->hostname)
        {
            strncpy(new_entry->hostname, work_entry->hostname, HOSTNAME_STRING_SIZE - 1);
            new_entry->hostname[HOSTNAME_STRING_SIZE] = '\0';

        }
        else
            strcpy(new_entry->hostname, "");

        if (!string1)
            return 0;

        if (!stricmp(status_conductor->conductor->nicklist[a].name, string1))
        {

            if (option == 0)
            {

                if (status_conductor->conductor->nicklist[a].modes[0] == '@')
                {
                    strcpy(status_conductor->conductor->nicklist[a].modes, " ");
                    status_conductor->conductor->nicks_ops--;
                    update_TX_nicklist();
                }
                //else
                //{

                strcpy(status_conductor->conductor->nicklist[a].modes, " ");
                //}

                //printf("taking away status: %s.%s\n",status_conductor->nick,string1);
                if (!stricmp(status_conductor->nick, string1))
                {
                    status_conductor->conductor->your_current_mode[0] = ' ';
                    status_conductor->conductor->your_current_mode[1] = '\0';
                    update_your_nickname_text_label();
                }

            }

            if (option == 1)
            {
                for (int count = 1; count <= status_conductor->max_modes; count++)
                {
                    if (prefix == status_conductor->user_modes[count].mode[0])
                    {
                        status_conductor->conductor->nicklist[a].modes[0] =
                                status_conductor->user_modes[count].symbol[0];
                        status_conductor->conductor->nicklist[a].modes[1] = '\0';

                        //printf("giving status:%s. %s\n",status_conductor->nick,string1);

                        if (status_conductor->conductor->nicklist[a].modes[0] == '@')
                        {
                            status_conductor->conductor->nicks_ops++;
                            update_TX_nicklist();
                        }
                        //printf("2giving status:%s. %s\n",status_conductor->nick,string1);

                        if (!strcmp(status_conductor->nick, string1))
                        {
                            status_conductor->conductor->your_current_mode[0] =
                                    status_conductor->user_modes[count].symbol[0];
                            status_conductor->conductor->your_current_mode[1] = '\0';
                            update_your_nickname_text_label();
                        }
                        //printf("3giving status:%s. %s\n",status_conductor->nick,string1);

                        break;
                    }

                }
            }

        }

        if (!stricmp(work_entry->name, string1))
        {
            if (option == 0)
            {
                strcpy(new_entry->modes, " ");

            }

            if (option == 1)
            {

                for (int count = 1; count <= status_conductor->max_modes; count++)
                {
                    if (prefix == status_conductor->user_modes[count].mode[0])
                    {
                        new_entry->modes[0] = status_conductor->user_modes[count].symbol[0];
                        new_entry->modes[1] = '\0';
                        break;
                    }
                }

            }

        }

        a++;

    }

    DisplayNicks();

    return 0;

}

int RemoveNick(char *string1)
{
    ULONG entries;

    unsigned int a = 0;
    getmacro((Object*) status_conductor->conductor->LV_nicklist, MUIA_NList_Entries, &entries);

    for (a = 0; a < entries; a++)
    {
        struct list_entry *work_entry = NULL;
        DoMethod((Object*) status_conductor->conductor->LV_nicklist, MUIM_NList_GetEntry, a, &work_entry);

        if (work_entry)
        {
            if (!stricmp(work_entry->name, string1))
            {
                DoMethod((Object*) status_conductor->conductor->LV_nicklist, MUIM_NList_Remove, a);
                break;
            }
        }
        else
        {
            break;

        }

    }

    entries = status_conductor->conductor->nicks;

    for (a = 0; a < entries; a++)
    {
        //printf("2: entries:%i a:%i\n",entries,a);
        if (string1 && status_conductor->conductor->nicklist[a].name)
        {
            if (!stricmp(string1, status_conductor->conductor->nicklist[a].name))
            {
                //printf("2nd level, a=%i\n",a);

                if (status_conductor->conductor->nicklist[a].modes[0] == '@')
                    status_conductor->conductor->nicks_ops--;

                while (a + 1 < entries)
                {
                    strcpy(status_conductor->conductor->nicklist[a].name,
                            status_conductor->conductor->nicklist[a + 1].name);
                    strcpy(status_conductor->conductor->nicklist[a].modes,
                            status_conductor->conductor->nicklist[a + 1].modes);

                    if (status_conductor->conductor->nicklist[a].hostname
                            && status_conductor->conductor->nicklist[a + 1].hostname)
                    {
                        strncpy(status_conductor->conductor->nicklist[a].hostname,
                                status_conductor->conductor->nicklist[a + 1].hostname, HOSTNAME_STRING_SIZE - 1);
                        status_conductor->conductor->nicklist[a].hostname[HOSTNAME_STRING_SIZE] = '\0';
                    }
                    a++;
                }

                strcpy(status_conductor->conductor->nicklist[a].name, "");
                strcpy(status_conductor->conductor->nicklist[a].modes, " ");
                if (status_conductor->conductor->nicklist[a].hostname)
                    strcpy(status_conductor->conductor->nicklist[a].hostname, "");

                status_conductor->conductor->nicks--;

                change_window_titlebar_text();
                update_TX_nicklist();

                return 1;
            }

        }

    }

    update_TX_nicklist();

    change_window_titlebar_text();

    return 0;

}

void nicklist_init()
{
    new_entry = malloc(sizeof(struct list_entry));
    new_entry->hostname = malloc(sizeof(char) * (HOSTNAME_STRING_SIZE + 1));
}

void nicklist_deinit()
{
    free(new_entry->hostname);
    free(new_entry);
}
