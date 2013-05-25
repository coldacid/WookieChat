/*
    The contents of this file are subject to the AROS Public License Version 1.1 
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis, 
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
    for the specific language governing rights and limitations under the License.
*/

void shutdown_my_dcc_recv_socket(void)
{

    if (dcc_conductor->dcc_socket != -1)
    {
        if (FD_ISSET(dcc_conductor->dcc_socket, &read_master))
            shutdown((int) dcc_conductor->dcc_socket, (int) 2);
        if (FD_ISSET(dcc_conductor->dcc_socket, &read_master))
            FD_CLR(dcc_conductor->dcc_socket, &read_master);
        if (FD_ISSET(dcc_conductor->dcc_socket, &write_master))
            FD_CLR(dcc_conductor->dcc_socket, &write_master);
        dcc_conductor->dcc_socket = -1;

        if (dcc_conductor->dcc_file)
        {
            Close(dcc_conductor->dcc_file);
            dcc_conductor->dcc_file = 0;
        }

    }

}

void shutdown_my_dcc_send_socket(void)
{

    if (dcc_send_conductor->dcc_listen_socket != -1)
    {
        if (FD_ISSET(dcc_send_conductor->dcc_listen_socket, &read_master))
            shutdown((int) dcc_send_conductor->dcc_listen_socket, (int) 2);
        if (FD_ISSET(dcc_send_conductor->dcc_listen_socket, &read_master))
            FD_CLR(dcc_send_conductor->dcc_listen_socket, &read_master);
        if (FD_ISSET(dcc_send_conductor->dcc_listen_socket, &write_master))
            FD_CLR(dcc_send_conductor->dcc_listen_socket, &write_master);
        dcc_send_conductor->dcc_listen_socket = -1;

    }

    if (dcc_send_conductor->dcc_socket != -1)
    {
        if (FD_ISSET(dcc_send_conductor->dcc_socket, &read_master))
            shutdown((int) dcc_send_conductor->dcc_socket, (int) 2);
        if (FD_ISSET(dcc_send_conductor->dcc_socket, &read_master))
            FD_CLR(dcc_send_conductor->dcc_socket, &read_master);
        if (FD_ISSET(dcc_send_conductor->dcc_socket, &write_master))
            FD_CLR(dcc_send_conductor->dcc_socket, &write_master);
        dcc_send_conductor->dcc_socket = -1;

    }
}

void dcc_chat_connect()
{

    long i = 1;

#ifdef __amigaos4__
    socklen_t sin_size = sizeof(dcc_chat_conductor->their_addr);
#elif __AROS__
    socklen_t sin_size = sizeof(dcc_chat_conductor->their_addr);
#else
    long sin_size = sizeof(dcc_chat_conductor->their_addr);
#endif

    dcc_chat_conductor->dcc_socket = accept(dcc_chat_conductor->dcc_listen_socket,
            (struct sockaddr *) &dcc_chat_conductor->their_addr, &sin_size);

    IoctlSocket(dcc_chat_conductor->dcc_socket, FIONBIO, (char*) &i);
    IoctlSocket(dcc_chat_conductor->dcc_socket, FIOASYNC, (char*) &i);

    if (!dcc_chat_conductor->str)
    {
        dcc_chat_conductor->str = new char[BUFFERSIZE * 2];
        strcpy(dcc_chat_conductor->str, "");
    }

    if (dcc_chat_conductor->dcc_socket == -1)
    {
        timestamp_2_string();

        sprintf(buffer3, "%s%sDCC%s %s", timestamp, GetCatalogStr(catalog, 217, "["), GetCatalogStr(catalog, 218, "]"),
                GetCatalogStr(catalog, 188, "Unable to establish DCC CHAT Connection"));
        status_conductor->conductor = dcc_chat_conductor->conductor;
        add_text_to_conductor_list(buffer3, 9, ACTIVITY_CHAT);

        if (FD_ISSET(dcc_chat_conductor->dcc_listen_socket, &read_master))
            FD_CLR(dcc_chat_conductor->dcc_listen_socket, &read_master);
    }
    else
    {
        timestamp_2_string();

        dcc_chat_conductor->connected = 1;

        sprintf(buffer3, "%s%sDCC%s %s", timestamp, GetCatalogStr(catalog, 217, "["), GetCatalogStr(catalog, 218, "]"),
                GetCatalogStr(catalog, 189, "DCC CHAT Connection Established"));
        status_conductor->conductor = dcc_chat_conductor->conductor;
        add_text_to_conductor_list(buffer3, 9, ACTIVITY_CHAT);

        if (FD_ISSET(dcc_chat_conductor->dcc_listen_socket, &read_master))
            FD_CLR(dcc_chat_conductor->dcc_listen_socket, &read_master);
        FD_SET(dcc_chat_conductor->dcc_socket, &read_master);
        if (fdmax < dcc_chat_conductor->dcc_socket)
            fdmax = dcc_chat_conductor->dcc_socket;

    }

}

void cleanup_dcc_recv()
{

    dcc_conductor->completed = 1;
    dcc_conductor->connected = 0;

    if (dcc_conductor->dcc_file)
    {
        Close(dcc_conductor->dcc_file);
        dcc_conductor->dcc_file = 0;
    }

    status_conductor = status_root;
    while (status_conductor)
    {
        if (!stricmp(status_conductor->networkname, dcc_conductor->networkname))
            break;
        status_conductor = status_conductor->next;
        if (!status_conductor)
        {
            status_conductor = status_root;
            printf("no valid network name\n");
            break;
        }
    }

    status_conductor->conductor = status_conductor->current_query;
    add_text_to_conductor_list(buffer3, 9, ACTIVITY_CHAT);

    shutdown_my_dcc_recv_socket();

}

void cleanup_dcc_send(int flag_as_completed)
{

    //printf("clean 1\n");

    if (flag_as_completed == FLAG_AS_COMPLETED)
    {
        //printf("clean 2\n");
        //dcc_send_conductor->cancelled=1;
        dcc_send_conductor->completed = 1;
        dcc_send_conductor->connected = 0;
    }

    //dcc_send_conductor->cancelled=1; //attempt to fix incomplete sends

    if (dcc_send_conductor->dcc_file)
    {
        Close(dcc_send_conductor->dcc_file);
        dcc_send_conductor->dcc_file = 0;
    }

    status_conductor = status_root;
    while (status_conductor)
    {
        if (!stricmp(status_conductor->networkname, dcc_send_conductor->networkname))
            break;
        status_conductor = status_conductor->next;
        if (!status_conductor)
        {
            status_conductor = status_root;
            //printf("no valid network name\n");
            break;
        }
    }

    status_conductor->conductor = status_conductor->current_query;
    add_text_to_conductor_list(buffer3, 9, ACTIVITY_CHAT);
    //printf("clean 3\n");

    shutdown_my_dcc_send_socket();

    /*if(dcc_send_conductor->dcc_socket)
     {
     if(FD_ISSET(dcc_send_conductor->dcc_socket, &write_master)) FD_CLR(dcc_send_conductor->dcc_socket, &write_master);
     if(FD_ISSET(dcc_send_conductor->dcc_socket, &read_master)) FD_CLR(dcc_send_conductor->dcc_socket, &read_master);
     if(FD_ISSET(dcc_send_conductor->dcc_socket, &read_master)) shutdown((int)dcc_send_conductor->dcc_socket,(int)2);
     }*/

}

LONG recv_thing;
LONG send_thing;
LONG actualLength;
LONG actual_read;

void dcc_time_to_send(void)
{

    //add_text_to_current_list(buffer3, 9, ACTIVITY_CHAT);
    sprintf(buffer3, "%s%sDCC%s %s %s [%lu bytes] >> %s. ", timestamp, GetCatalogStr(catalog, 217, "["),
            GetCatalogStr(catalog, 218, "]"), GetCatalogStr(catalog, 175, "Finished sending"),
            dcc_send_conductor->filename, dcc_send_conductor->filesize, dcc_send_conductor->nick);

    dcc_time();

    secs = secs - atoi(dcc_send_conductor->timestarted);

    mins = secs / 60;
    hrs = mins / 60;
    days = hrs / 24;
    secs = secs % 60;
    mins = mins % 60;
    hrs = hrs % 24;

    //strcat(buffer3," Sent in ");

    strcat(buffer3, GetCatalogStr(catalog, 190, "Transfer time:"));
    strcat(buffer3, " ");

    if (hrs > 0)
        sprintf(buffer2, "%02lu:%02lu:%02lu", hrs, mins, secs);
    else if (mins > 0)
        sprintf(buffer2, "00:%02lu:%02lu", mins, secs);
    else
        sprintf(buffer2, "00:00:%02lu", secs);

    strcat(buffer3, buffer2);
    //strcat(buffer3,", at an average speed of ");
    strcat(buffer3, ". ");
    strcat(buffer3, GetCatalogStr(catalog, 178, "Average speed: "));
    strcat(buffer3, " ");

    strcat(buffer3, dcc_send_conductor->entry->cps);
    strcat(buffer3, "kB/s");

}

void dcc_time_to_recv(void)
{

    //add_text_to_current_list(buffer3, 9, ACTIVITY_CHAT);

    /*    if(dcc_conductor->filepath[strlen(dcc_conductor->filepath)-1] != '/' && dcc_conductor->filepath[strlen(dcc_conductor->filepath)-1] != ':')
     sprintf(buffer3,"%s%sDCC%s %s %s/%s [%lu bytes] %s %s. ",timestamp,GetCatalogStr(catalog,217,"["),GetCatalogStr(catalog,218,"]"),GetCatalogStr(catalog,176,"Finished recieving"),dcc_conductor->filepath,dcc_conductor->filename,dcc_conductor->filesize,GetCatalogStr(catalog,210,"From"),dcc_conductor->nick);
     else
     sprintf(buffer3,"%s%sDCC%s %s %s%s [%lu bytes] %s %s. ",timestamp,GetCatalogStr(catalog,217,"["),GetCatalogStr(catalog,218,"]"),GetCatalogStr(catalog,176,"Finished recieving"),dcc_conductor->filepath,dcc_conductor->filename,dcc_conductor->filesize,GetCatalogStr(catalog,210,"From"),dcc_conductor->nick);*/

    sprintf(buffer3, "%s%sDCC%s %s %s%s [%lu bytes] %s %s. ", timestamp, GetCatalogStr(catalog, 217, "["),
            GetCatalogStr(catalog, 218, "]"), GetCatalogStr(catalog, 176, "Finished recieving"),
            dcc_conductor->filepath, dcc_conductor->filename, dcc_conductor->filesize,
            GetCatalogStr(catalog, 210, "From"), dcc_conductor->nick);

    //sprintf(buffer3,"%s%sDCC%s %s %s%s [%lu bytes] %s %s. ",timestamp,GetCatalogStr(catalog,217,"["),GetCatalogStr(catalog,218,"]"),GetCatalogStr(catalog,176,"Finished recieving"),dcc_conductor->filepath,dcc_conductor->filename,dcc_conductor->filesize,GetCatalogStr(catalog,210,"From"),dcc_conductor->nick);

    dcc_time();

    secs = secs - atoi(dcc_conductor->timestarted);

    mins = secs / 60;
    hrs = mins / 60;
    days = hrs / 24;
    secs = secs % 60;
    mins = mins % 60;
    hrs = hrs % 24;

    strcat(buffer3, GetCatalogStr(catalog, 190, "Transfer time:"));
    strcat(buffer3, " ");

    if (hrs > 0)
        sprintf(buffer2, "%02lu:%02lu:%02lu", hrs, mins, secs);
    else if (mins > 0)
        sprintf(buffer2, "00:%02lu:%02lu", mins, secs);
    else
        sprintf(buffer2, "00:00:%02lu", secs);

    strcat(buffer3, buffer2);
    strcat(buffer3, ". ");
    strcat(buffer3, GetCatalogStr(catalog, 178, "Average speed: "));
    strcat(buffer3, " ");
    strcat(buffer3, dcc_conductor->entry->cps);
    strcat(buffer3, "kB/s");

}

void accept_dcc(char *b)
{

    DoMethod((Object*) WookieChat->LV_dcc, MUIM_NList_GetEntry, MUIV_NList_GetEntry_Active, &find);
    LONG a = 0;

    dcc_conductor = dcc_root->next;

    if (DEBUG)
        printf("trying to accept\n");

    while (find && dcc_conductor)
    {

        //if(DEBUG) printf("dcc 2\n");

        if (!dcc_conductor->removed)
        {

            //if(DEBUG) printf("dcc 3\n");

            if (find && dcc_conductor->entry)
            {
                if (!stricmp(find->status, dcc_conductor->entry->status)
                        && !stricmp(find->nick, dcc_conductor->entry->nick)
                        && !stricmp(find->filename, dcc_conductor->entry->filename)
                        && !stricmp(find->port, dcc_conductor->entry->port)) //experimental, adding port string to entry struct
                {
                    //if(DEBUG) printf("dcc 4\n");

                    if (dcc_conductor)
                    {

                        if (DEBUG)
                            printf("found the dcc conductor entry\n");
                        //if(dcc_conductor->accepted==TRUE) { printf("aborting accept_dcc()\n"); return; }

                        if (dcc_conductor->entry)
                        {
                            strcpy(dcc_conductor->entry->status, GetCatalogStr(catalog, 201, "Connecting"));
                            strcpy(dcc_conductor->entry->percentage, "0%");
                        }
                        //if(DEBUG) printf("dcc 5\n");

                        DoMethod((Object*) WookieChat->LV_dcc, MUIM_NList_ReplaceSingle, dcc_conductor->entry, a,
                                NOWRAP, ALIGN_LEFT);

                        //if(DEBUG) printf("dcc 6\n");

                        //open file, see if it exists, check filesize for resume option
                        if (b[strlen(b) - 1] != '/' && b[strlen(b) - 1] != ':')
                            sprintf(dcc_conductor->full_filename_with_path, "%s/%s", b, dcc_conductor->filename);
                        else
                            sprintf(dcc_conductor->full_filename_with_path, "%s%s", b, dcc_conductor->filename);
                        BPTR examined_file = Open(dcc_conductor->full_filename_with_path, MODE_READWRITE);

                        strcpy(dcc_conductor->filepath, b);

                        if (b[strlen(b) - 1] != '/' && b[strlen(b) - 1] != ':')
                            strcat(dcc_conductor->filepath, "/");

                        if (!dcc_conductor->completed)
                        {
                            if (DEBUG)
                                printf("examined file for dcc resume:%s\n", dcc_conductor->full_filename_with_path);
                            if (examined_file)
                            {
                                //if(DEBUG) printf("dcc 7\n");

                                struct FileInfoBlock *a_fib;
                                a_fib = (FileInfoBlock*) AllocDosObject(DOS_FIB, NULL);

                                if (ExamineFH(examined_file, a_fib))
                                {
                                    if (a_fib->fib_Size > 0)
                                    {
                                        //printf("file already exists\n");
                                        char c[100], d[100], e[1100];
                                        sprintf(e, "\"%s\" %s", dcc_conductor->full_filename_with_path,
                                                GetCatalogStr(catalog, 202, "already exists!"));
                                        sprintf(c, "%s: %lu bytes",
                                                GetCatalogStr(catalog, 203, "Size of existing file"), a_fib->fib_Size);
                                        sprintf(d, "%s: %lu bytes",
                                                GetCatalogStr(catalog, 204, "Size of file being offered"),
                                                dcc_conductor->filesize);
                                        setmacro((Object*) WookieChat->TX_oldname, MUIA_Text_Contents, e);
                                        setmacro((Object*) WookieChat->TX_oldsize, MUIA_Text_Contents, c);
                                        setmacro((Object*) WookieChat->TX_newsize, MUIA_Text_Contents, d);

                                        dcc_conductor->total_recv = a_fib->fib_Size;

                                        //if(DEBUG) printf("dcc 8\n");

                                        if (my_settings.dcc_recv_auto_accept_if_exists_action == ASK)
                                            setmacro((Object*) WookieChat->WI_dcc_file_exists, MUIA_Window_Open, TRUE);

                                        else if (my_settings.dcc_recv_auto_accept_if_exists_action == OVERWRITE)
                                            DoMethod((Object*) WookieChat->App, MUIM_Application_ReturnID, 71);

                                        else if (my_settings.dcc_recv_auto_accept_if_exists_action == RESUME)
                                            DoMethod((Object*) WookieChat->App, MUIM_Application_ReturnID, 70);

                                        else if (my_settings.dcc_recv_auto_accept_if_exists_action == ABORT)
                                            DoMethod((Object*) WookieChat->App, MUIM_Application_ReturnID, 66);

                                        else if (my_settings.dcc_recv_auto_accept_if_exists_action == RENAME)
                                            DoMethod((Object*) WookieChat->App, MUIM_Application_ReturnID, 60);

                                        else
                                            setmacro((Object*) WookieChat->WI_dcc_file_exists, MUIA_Window_Open, TRUE);

                                        //if(DEBUG) printf("dcc 9\n");

                                    }
                                    else
                                    {
                                        //dcc_conductor->accepted=TRUE;
                                        if (DEBUG)
                                            printf("connect() attempted on dcc socket..\n");

                                        connect(dcc_conductor->dcc_socket, (struct sockaddr*) &dcc_conductor->test,
                                                sizeof(struct sockaddr));
                                        //if(DEBUG) printf("dcc 10\n");
                                    }

                                }
                                Close(examined_file);
                                FreeDosObject(DOS_FIB, a_fib);
                            }
                        }

                        return;
                        break;

                    }
                }

            }

            a++;
        }

        dcc_conductor = dcc_conductor->next;

    }

    //if(DEBUG) printf("dcc 11\n");
    if (DEBUG)
        printf("\n");

}

void dcc_send_data(int a)
{

    if (!dcc_send_conductor)
        return;
    //printf("total_Recv:%d, filesize:%d\n",dcc_send_conductor->total_recv < dcc_send_conductor->filesize);

    if (dcc_send_conductor->total_recv < dcc_send_conductor->filesize)
    {
        //printf("sending data\n");
        recv_thing = recv(dcc_send_conductor->dcc_socket, (i_in) dcc_send_conductor->recv_buffer,
                DCC_RECV_BUFFERSIZE - 1, 0);
        actualLength = Read(dcc_send_conductor->dcc_file, (i_in) dcc_send_conductor->buffer, DCC_RECV_BUFFERSIZE - 1);
        send_thing = send(dcc_send_conductor->dcc_socket, (i_in) dcc_send_conductor->buffer, actualLength, 0);
        if (send_thing != -1)
        {
            dcc_send_conductor->total_recv += send_thing;
        }
        if (!dcc_send_conductor->dcc_file)
            return;

#ifdef __aamigaos4__
        if(send_thing != actualLength) ChangeFilePosition(dcc_send_conductor->dcc_file,dcc_send_conductor->total_recv,OFFSET_BEGINNING);
#else
        if (send_thing != actualLength)
            Seek(dcc_send_conductor->dcc_file, dcc_send_conductor->total_recv, OFFSET_BEGINNING);
#endif

    }

    if (dcc_send_conductor->total_recv >= dcc_send_conductor->filesize)
    {

        dcc_send_conductor->buffer[0] = '\0';

        if (dcc_send_conductor->connected && !dcc_send_conductor->cancelled)
        {

            if (dcc_send_conductor->entry)
            {

                strcpy(dcc_send_conductor->entry->status, GetCatalogStr(catalog, 179, "Completed")); //"Completed");
                strcpy(dcc_send_conductor->entry->timeleft, "N/A");
                sprintf(dcc_send_conductor->entry->bytes_transferred, "%ldkB", dcc_send_conductor->total_recv / 1024);
                float pct = (float) dcc_send_conductor->total_recv / (float) dcc_send_conductor->filesize;
                sprintf(dcc_send_conductor->entry->percentage, "%.0f%%", (float) (pct * 100));
            }

            DoMethod((Object*) WookieChat->LV_send_dcc, MUIM_NList_ReplaceSingle, dcc_send_conductor->entry, a, NOWRAP,
                    ALIGN_LEFT);

            timestamp_2_string();

            sprintf(buffer3, "%s%sDCC%s SEND: %s %s %s %s", timestamp, GetCatalogStr(catalog, 217, "["),
                    GetCatalogStr(catalog, 218, "]"), dcc_send_conductor->filename, GetCatalogStr(catalog, 205, "to"),
                    dcc_send_conductor->nick, GetCatalogStr(catalog, 179, "Completed"));
            dcc_time_to_send();

            cleanup_dcc_send(FLAG_AS_COMPLETED);

        }
    }
    else if (recv_thing == -1 || recv_thing == 0)
    {

        //printf("disconnected:%i\n",Errno());

        if (DEBUG)
            printf("error number %ld\n", Errno());

        if (Errno() == 22 || (Errno() == 32 && recv_thing == -1) || recv_thing == 0)
        {

            //printf("error code reached\n");

            if (dcc_send_conductor->entry)
            {

                strcpy(dcc_send_conductor->entry->status, GetCatalogStr(catalog, 180, "Incomplete"));
                strcpy(dcc_send_conductor->entry->timeleft, "N/A");

                float pct = 0;

                if (dcc_send_conductor->total_recv > 0)
                {
                    sprintf(dcc_send_conductor->entry->bytes_transferred, "%ldkB",
                            dcc_send_conductor->total_recv / 1024);
                    pct = (float) dcc_send_conductor->total_recv / (float) dcc_send_conductor->filesize;
                }
                sprintf(dcc_send_conductor->entry->percentage, "%.0f%%", (float) (pct * 100));

            }

            DoMethod((Object*) WookieChat->LV_send_dcc, MUIM_NList_ReplaceSingle, dcc_send_conductor->entry, a, NOWRAP,
                    ALIGN_LEFT);

            sprintf(buffer3, "%s%sDCC%s SEND: %s %s %s %s", timestamp, GetCatalogStr(catalog, 217, "["),
                    GetCatalogStr(catalog, 218, "]"), dcc_send_conductor->filename, GetCatalogStr(catalog, 205, "to"),
                    dcc_send_conductor->nick, GetCatalogStr(catalog, 180, "Incomplete"));

            cleanup_dcc_send(FLAG_AS_COMPLETED);

        }

    }

}

unsigned long dcc_addr;
unsigned long addr;

int reoffer_dcc(char *networkname, char *string1, char *string2, char *string3, int portnumber)
{

    //string1 == type.. send or chat
    //string2 == nick being offered to
    //string3 == filename including path

    //char *work=new char[50];

    if (my_settings.autogethostid)
        addr = htonl(gethostid());
    else
        addr = htonl(dcc_addr);

    //if(work)
    //{

    char *file_name = new char[300];

    int count_chars = 0;
    int a;

    for (a = strlen(string3); string3[a] != ':' && string3[a] != '/'; a--)
    {
        count_chars++;
    }

    if (string3[strlen(string3) - count_chars] == ':' || string3[strlen(string3) - count_chars] == '/')
        count_chars--;

    for (a = 0; a <= count_chars; a++)
    {
        file_name[a] = string3[strlen(string3) - count_chars + a];
        if (file_name[a] == ' ')
            file_name[a] = '_';
    }
//        file_name[a] == '\0';

    BPTR examined_file = Open(string3, MODE_OLDFILE);

    struct FileInfoBlock *a_fib;
    a_fib = (FileInfoBlock*) AllocDosObject(DOS_FIB, NULL);
    if (!examined_file)
    {
        sprintf(buffer3, "%s%sDCC%s SEND: %s", timestamp, GetCatalogStr(catalog, 217, "["),
                GetCatalogStr(catalog, 218, "]"), GetCatalogStr(catalog, 191, "Error, file doesnt exist"));
        add_text_to_current_list(buffer3, 9, ACTIVITY_CHAT);

        return 0;
    }
    if (ExamineFH(examined_file, a_fib))
    {

        while (status_conductor->next)
        {
            if (!stricmp(status_conductor->networkname, networkname))
                break;
            status_conductor = status_conductor->next;
        }

        dcc_send_conductor->connected = 0;
        dcc_send_conductor->total_recv = 0;
#ifdef __aamigaos4__
        ChangeFilePosition(dcc_send_conductor->dcc_file,0,OFFSET_BEGINNING);
#else
        Seek(dcc_send_conductor->dcc_file, 0, OFFSET_BEGINNING);
#endif

        strcpy(dcc_send_conductor->entry->percentage, "0%");
        strcpy(dcc_send_conductor->entry->status, GetCatalogStr(catalog, 206, "Waiting"));

        //sprintf(sendstuff,"PRIVMSG %s :\001DCC SEND %s %lu %d %u 10870\001\r\n",dcc_send_conductor->entry->nick, dcc_send_conductor->entry->filename, addr, my_settings.current_port, dcc_send_conductor->filesize);
        sprintf(sendstuff, "PRIVMSG %s :\001DCC SEND %s %lu %d %lu\001\r\n", dcc_send_conductor->entry->nick,
                dcc_send_conductor->entry->filename, addr, my_settings.current_port, dcc_send_conductor->filesize);
        send_text(sendstuff);

        dcc_send_conductor = dcc_send_root->next;

        //2005-12-16 change
        //dcc_send_conductor->next=NULL;

        if (DEBUG)
            printf("sendstuff:%s", sendstuff);

        Close(examined_file);
        FreeDosObject(DOS_FIB, a_fib);

    }

    //}

    return 1;
}

int offer_dcc(char *networkname, char *string1, char *string2, char *string3, int portnumber)
{

    //string1 == type.. send or chat
    //string2 == nick being offered to
    //string3 == filename including path

    if (!stricmp(string1, "send"))
    {

        if (my_settings.autogethostid)
            addr = htonl(gethostid()); //lets use the Local IP addess of this machine
        else if (my_settings.ipaddr_use) //lets use the User Specified IP address
        {
#ifdef __MORPHOS__
            struct hostent *he=gethostbyname((UBYTE*)my_settings.ipaddr);
#elif __AROS__
            struct hostent *he = gethostbyname((char*) my_settings.ipaddr);
#else
            struct hostent *he=gethostbyname((i_in)my_settings.ipaddr);
#endif

            struct sockaddr_in wookiechat_in;
            if (he)
            {
                wookiechat_in.sin_addr = *((struct in_addr *) he->h_addr);
                memset(&(wookiechat_in.sin_zero), '\0', 8);
#ifdef __amigaos4__
                //newlib commented out
                //addr = htonl(inet_network(inet_ntoa((struct in_addr)wookiechat_in.sin_addr)));
                addr = htonl(inet_network((char*)Inet_NtoA(wookiechat_in.sin_addr.s_addr)));
#elif __AROS__
                addr = htonl(inet_network((char*) Inet_NtoA(wookiechat_in.sin_addr.s_addr)));
#elif __MORPHOS__
                addr = htonl(inet_network((UBYTE*)Inet_NtoA(wookiechat_in.sin_addr.s_addr)));
#else
                addr = htonl(inet_network((i_in)Inet_NtoA(wookiechat_in.sin_addr.s_addr)));
#endif
            }
            else
            {
                sprintf(buffer3, "%s%sDCC%s Unable to use specified DCC address", timestamp,
                        GetCatalogStr(catalog, 217, "["), GetCatalogStr(catalog, 218, "]"));
                add_text_to_current_list(buffer3, 9, ACTIVITY_CHAT);
                return 0;
            }

        }
        else
            addr = htonl(dcc_addr); //lets use the address automatically fetched from the server upon connection

        char *file_name = new char[300];

        int count_chars = 0;
        int a;

        for (a = strlen(string3); string3[a] != ':' && string3[a] != '/'; a--)
        {
            count_chars++;
        }

        if (string3[strlen(string3) - count_chars] == ':' || string3[strlen(string3) - count_chars] == '/')
            count_chars--;

        for (a = 0; a <= count_chars; a++)
        {
            file_name[a] = string3[strlen(string3) - count_chars + a];
            if (file_name[a] == ' ')
                file_name[a] = '_';
        }
//            file_name[a] == '\0';

        BPTR examined_file = Open(string3, MODE_OLDFILE);

        struct FileInfoBlock *a_fib;
        a_fib = (FileInfoBlock*) AllocDosObject(DOS_FIB, NULL);

        if (!examined_file)
        {
            timestamp_2_string();

            sprintf(buffer3, "%s%sDCC%s SEND: %s", timestamp, GetCatalogStr(catalog, 217, "["),
                    GetCatalogStr(catalog, 218, "]"), GetCatalogStr(catalog, 191, "Error, file doesnt exist"));
            add_text_to_current_list(buffer3, 9, ACTIVITY_CHAT);

            return 0;
        }
        if (ExamineFH(examined_file, a_fib))
        {

            status_conductor = status_current;

            if (my_settings.current_port > atoi(my_settings.end_port))
            {
                my_settings.current_port = atoi(my_settings.start_port);
            }

            if (my_settings.current_port < atoi(my_settings.start_port))
            {
                my_settings.current_port = atoi(my_settings.start_port);
            }

            //10870 is a mirc transfer id string that xchat doesnt like. without it, mirc still seems to work
            //sprintf(sendstuff,"PRIVMSG %s :\001DCC SEND %s %lu %d %lu\001\r\n",string2, file_name, addr, my_settings.current_port, a_fib->fib_Size);

            sprintf(sendstuff, "PRIVMSG %s :\001DCC SEND %s %lu %d %lu\001\r\n", string2, file_name, addr,
                    my_settings.current_port, a_fib->fib_Size);

            create_send_dcc(string2, string3, a_fib->fib_Size, my_settings.current_port);
            //if(DEBUG) printf("DCC STRING TO SEND:%s\n",sendstuff);
            send_text(sendstuff);

            my_settings.current_port++;

            if (DEBUG)
                printf("sendstuff:%s", sendstuff);

            Close(examined_file);

        }

        FreeDosObject(DOS_FIB, a_fib);

    }

    return 1;
}

void create_send_dcc(char *nick, char *string3, int filesize, int portnumber)
{

    //string3 == filename including path

    dcc_send_conductor = dcc_send_root;
    dcc_transfers = 0;

    while (dcc_send_conductor->next)
    {
        if (!dcc_send_conductor->removed)
            dcc_transfers++;
        dcc_send_conductor = dcc_send_conductor->next;
    }

    dcc_root->removed = 1;

    dcc_work = dcc_send_conductor;
    dcc_send_conductor->next = new dcc;
    dcc_send_conductor = dcc_send_conductor->next;
    dcc_send_conductor->next = NULL;
    dcc_send_conductor->previous = dcc_work;

    if (!strcmp(nick, "0") && !strcmp(string3, "0"))
        return;

    strcpy(dcc_send_conductor->networkname, status_current->networkname);

    dcc_send_conductor->dcc_listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (DEBUG)
    {
        if (dcc_send_conductor->dcc_listen_socket == -1)
            printf("unable to create listening socket\nerror number:%li\n", Errno());
        else
            printf("dcc listen socket created, number: %li\n", dcc_send_conductor->dcc_listen_socket);
    }

    if (fdmax < dcc_send_conductor->dcc_listen_socket)
        fdmax = dcc_send_conductor->dcc_listen_socket;
    dcc_send_conductor->dcc_socket = -1;

    // Create a DCC Entry for the DCC WINDOW NLIST then Fill in Values

    dcc_send_conductor->entry = new dcc_entry;

    sprintf(dcc_send_conductor->port, "%i", portnumber);

    dcc_send_conductor->connected = 0;
    dcc_send_conductor->dcc_file = 0;
    dcc_send_conductor->removed = 0;
    dcc_send_conductor->total_recv = 0;
    dcc_send_conductor->filesize = 0;

    strcpy(dcc_send_conductor->entry->port, dcc_send_conductor->port);
    if (nick)
        strcpy(dcc_send_conductor->entry->nick, nick);
    if (nick)
        strcpy(dcc_send_conductor->nick, nick);
    else
        strcpy(dcc_send_conductor->entry->nick, "Unknown?");
    strcpy(dcc_send_conductor->entry->percentage, "0%");
    strcpy(dcc_send_conductor->entry->status, GetCatalogStr(catalog, 206, "Waiting"));

    char *file_name = new char[500];

    int count_chars = 0;
    int a;

    for (a = strlen(string3); string3[a] != ':' && string3[a] != '/'; a--)
    {
        count_chars++;
    }

    if (string3[strlen(string3) - count_chars] == ':' || string3[strlen(string3) - count_chars] == '/')
        count_chars--;

    for (a = 0; a <= count_chars; a++)
    {
        file_name[a] = string3[strlen(string3) - count_chars + a];
        if (file_name[a] == ' ')
            file_name[a] = '_';
    }
//    file_name[a] == '\0';

    if (file_name)
        strcpy(dcc_send_conductor->entry->filename, file_name);
    if (file_name)
        strcpy(dcc_send_conductor->filename, file_name);

    if (string3)
        strcpy(dcc_send_conductor->filepath, string3);

    else
        strcpy(dcc_send_conductor->entry->filename, "Unknown?");
    strcpy(dcc_send_conductor->entry->bytes_transferred, "0");
    strcpy(dcc_send_conductor->entry->cps, "0");
    if (filesize)
        sprintf(dcc_send_conductor->entry->filesize, "%dkB", filesize / 1024);
    if (filesize)
        dcc_send_conductor->filesize = filesize;
    strcpy(dcc_send_conductor->entry->timeleft, "");

    dcc_send_conductor->completed = 0;
    dcc_send_conductor->cancelled = 0;

    memset(&dcc_send_conductor->test, '\0', sizeof(dcc_send_conductor->test));
    dcc_send_conductor->test.sin_family = AF_INET; // host byte order
    dcc_send_conductor->test.sin_addr.s_addr = htonl(INADDR_ANY); // auto-fill with my IP
    dcc_send_conductor->test.sin_port = htons(portnumber); // short, network byte order

    int yes = 1;

    if (setsockopt((int) dcc_send_conductor->dcc_listen_socket, (int) SOL_SOCKET, (int) SO_REUSEADDR, &yes,
            (int) sizeof(yes)) == -1)
    {
        if (DEBUG)
            printf("unable to reuse address\n");

    }

    //#ifdef __amigaos4__
    if (setsockopt((int) dcc_send_conductor->dcc_listen_socket, (int) SOL_SOCKET, (int) SO_REUSEPORT, &yes,
            (int) sizeof(yes)) == -1)
    {
        if (DEBUG)
            printf("unable to reuse port\n");

    }
    //#endif

    if (bind(dcc_send_conductor->dcc_listen_socket, (struct sockaddr *) &dcc_send_conductor->test,
            sizeof(dcc_send_conductor->test)) == -1)
    {
        if (DEBUG)
            printf("unable to bind address to socket, error number:%li\n", Errno());

    }

    if (IoctlSocket(dcc_send_conductor->dcc_listen_socket, FIONBIO, (char*) &yes) < 0)
    {
        if (DEBUG)
            printf("unable to change non-blocking I/O option for socket, error number:%li\n", Errno());
    }

    FD_SET(dcc_send_conductor->dcc_listen_socket, &read_master);

    listen((int) dcc_send_conductor->dcc_listen_socket, (int) 5);

    timestamp_2_string();

    sprintf(buffer3, "%s%sDCC%s SEND: %s %s %s %s %s port %i", timestamp, GetCatalogStr(catalog, 217, "["),
            GetCatalogStr(catalog, 218, "]"), GetCatalogStr(catalog, 192, "Offering file"),
            dcc_send_conductor->filepath, GetCatalogStr(catalog, 194, "to"), dcc_send_conductor->nick,
            GetCatalogStr(catalog, 193, "on"), portnumber);
    add_text_to_current_list(buffer3, 9, ACTIVITY_CHAT);

    if (QUIET_DCC == FALSE)
    {
        getmacro((Object*) WookieChat->WI_dcc2, MUIA_Window_Activate, &string1);
        if (!string1)
        {
            setmacro((Object*) WookieChat->WI_dcc2, MUIA_Window_Activate, FALSE);

            setmacro((Object*) WookieChat->WI_dcc2, MUIA_Window_Open, TRUE); //if the window is already open, dont open it again
            setmacro((Object*) WookieChat->WI_dcc2, MUIA_Window_DefaultObject, WookieChat->LV_send_dcc);
            setmacro((Object*) WookieChat->WI_dcc2, MUIA_Window_ActiveObject, WookieChat->LV_send_dcc);

        }

    }
    DoMethod((Object*) WookieChat->LV_send_dcc, MUIM_NList_InsertSingle, dcc_send_conductor->entry,
            MUIV_NList_Insert_Bottom);

}

void create_recv_dcc_chat(char *nick, char *address, char *port)
{

    if (!stricmp(port, "0"))
        return;

    dcc_chat_conductor = dcc_chat_root;
    //sprintf(buffer3,"$%s",nick);

    while (dcc_chat_conductor) //find already open tab
    {
        if (!stricmp(nick, dcc_chat_conductor->nick) && dcc_chat_conductor->removed == 0)
            break;
        dcc_chat_conductor = dcc_chat_conductor->next;
    }

    if (!dcc_chat_conductor) //if we didnt find a tab already open, create another
    {
        dcc_chat_conductor = dcc_chat_root;

        while (dcc_chat_conductor->next)
        {
            dcc_chat_conductor = dcc_chat_conductor->next;
        }

        dcc_chat_work = dcc_chat_conductor;

        dcc_chat_conductor->next = new dcc_chat;
        dcc_chat_conductor = dcc_chat_conductor->next;
        dcc_chat_conductor->next = NULL;

        dcc_chat_conductor->previous = dcc_chat_work;

        strcpy(dcc_chat_conductor->nick, nick);
        strcpy(dcc_chat_conductor->own_nick, status_conductor->nick);
        sprintf(dcc_chat_conductor->name, "$%s", nick);

        if (strcmp(nick, "0"))
        {
            create_new_tab(dcc_chat_conductor->name, 0, 2);
            dcc_chat_conductor->conductor = status_conductor->conductor;
            dcc_chat_conductor->str = new char[BUFFERSIZE * 2];
            strcpy(dcc_chat_conductor->str, "");

        }
    }
    else
    {
        if (FD_ISSET(dcc_chat_conductor->dcc_socket, &read_master))
            FD_CLR(dcc_chat_conductor->dcc_socket, &read_master);
        if (FD_ISSET(dcc_chat_conductor->dcc_socket, &write_master))
            FD_CLR(dcc_chat_conductor->dcc_socket, &write_master);
    }

    dcc_chat_conductor->dcc_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (fdmax < dcc_chat_conductor->dcc_socket)
        fdmax = dcc_chat_conductor->dcc_socket;

    if (!strcmp(nick, "0"))
        return;

    FD_SET(dcc_chat_conductor->dcc_socket, &read_master);
    FD_SET(dcc_chat_conductor->dcc_socket, &write_master);

    strncpy(dcc_chat_conductor->address, address, 200);
    strncpy(dcc_chat_conductor->port, port, 200);
    dcc_chat_conductor->connected = 0;
    dcc_chat_conductor->removed = 0;

    in_addr test2;
#ifdef __amigaos4__
    test2.s_addr = inet_addr(address);
#elif __AROS__
    test2.s_addr = inet_addr((char*) address);
#elif __MORPHOS__
    test2.s_addr = inet_addr((UBYTE*)address);
#else
    test2.s_addr = inet_addr((u_char*)address);
#endif

    dcc_chat_conductor->test.sin_addr.s_addr = test2.s_addr;
    dcc_chat_conductor->test.sin_port = atoi(port);
    dcc_chat_conductor->test.sin_family = AF_INET;
    memset(&(dcc_chat_conductor->test.sin_zero), '\0', 8);

    long i = 1;

    IoctlSocket(dcc_chat_conductor->dcc_socket, FIONBIO, (char*) &i);
    IoctlSocket(dcc_chat_conductor->dcc_socket, FIOASYNC, (char*) &i);

    connect(dcc_chat_conductor->dcc_socket, (struct sockaddr*) &dcc_chat_conductor->test, sizeof(struct sockaddr));

}

void initiate_outgoing_dcc_chat(char *nick)
{

    if (!stricmp(status_conductor->nick, nick))
        return;

    if (my_settings.autogethostid)
        addr = htonl(gethostid());
    else
        addr = htonl(dcc_addr);

    if (my_settings.current_port > atoi(my_settings.end_port))
        my_settings.current_port = atoi(my_settings.start_port);

    if (my_settings.current_port < atoi(my_settings.start_port))
        my_settings.current_port = atoi(my_settings.start_port);

    dcc_chat_conductor = dcc_chat_root;
    //sprintf(string7,"$%s",nick);

    while (dcc_chat_conductor) //find already open tab
    {
        if (!stricmp(nick, dcc_chat_conductor->nick)/*&& dcc_chat_conductor->removed==0*/)
            break;
        dcc_chat_conductor = dcc_chat_conductor->next;
    }

    if (!dcc_chat_conductor) //if we didnt find a tab already open, create another
    {
        //printf("didnt find already open tab, create another\n");

        dcc_chat_conductor = dcc_chat_root;

        while (dcc_chat_conductor->next)
        {
            dcc_chat_conductor = dcc_chat_conductor->next;
        }

        dcc_chat_work = dcc_chat_conductor;

        dcc_chat_conductor->next = new dcc_chat;
        dcc_chat_conductor = dcc_chat_conductor->next;
        dcc_chat_conductor->next = NULL;

        dcc_chat_conductor->previous = dcc_chat_work;

        strcpy(dcc_chat_conductor->nick, nick);
        strcpy(dcc_chat_conductor->own_nick, status_conductor->nick);
        sprintf(dcc_chat_conductor->name, "$%s", nick);

        if (strcmp(nick, "0"))
        {
            //printf("2didnt find already open tab, create another\n");

            create_new_tab(dcc_chat_conductor->name, 0, 2);
            //dcc_chat_conductor->conductor=status_conductor->conductor;
            dcc_chat_conductor->str = new char[BUFFERSIZE * 2];
            strcpy(dcc_chat_conductor->str, "");
        }
    }

    dcc_chat_conductor->dcc_listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (DEBUG)
    {
        if (dcc_chat_conductor->dcc_listen_socket == -1)
            printf("unable to create listening socket\nerror number:%li\n", Errno());
        else
            printf("dcc listen socket created, number: %li\n", dcc_chat_conductor->dcc_listen_socket);
    }

    if (fdmax < dcc_chat_conductor->dcc_listen_socket)
        fdmax = dcc_chat_conductor->dcc_listen_socket;
    dcc_chat_conductor->dcc_socket = -1;

    memset(&dcc_chat_conductor->test, '\0', sizeof(dcc_chat_conductor->test));
    dcc_chat_conductor->test.sin_family = AF_INET; // host byte order
    dcc_chat_conductor->test.sin_addr.s_addr = htonl(INADDR_ANY); // auto-fill with my IP
    dcc_chat_conductor->test.sin_port = htons(my_settings.current_port); // short, network byte order

    int yes = 1;

    if (setsockopt((int) dcc_chat_conductor->dcc_listen_socket, (int) SOL_SOCKET, (int) SO_REUSEADDR, &yes,
            (int) sizeof(yes)) == -1)
    {
        if (DEBUG)
            printf("unable to reuse address\n");

    }

    if (setsockopt((int) dcc_chat_conductor->dcc_listen_socket, (int) SOL_SOCKET, (int) SO_REUSEPORT, &yes,
            (int) sizeof(yes)) == -1)
    {
        if (DEBUG)
            printf("unable to reuse port\n");

    }

    if (bind(dcc_chat_conductor->dcc_listen_socket, (struct sockaddr *) &dcc_chat_conductor->test,
            sizeof(dcc_chat_conductor->test)) == -1)
    {
        if (DEBUG)
            printf("unable to bind address to socket, error number:%li\n", Errno());

    }

    if (IoctlSocket(dcc_chat_conductor->dcc_listen_socket, FIONBIO, (char*) &yes) < 0)
    {
        if (DEBUG)
            printf("unable to change non-blocking I/O option for socket, error number:%li\n", Errno());
    }

    FD_SET(dcc_chat_conductor->dcc_listen_socket, &read_master);

    listen((int) dcc_chat_conductor->dcc_listen_socket, (int) 5);

    timestamp_2_string();

    sprintf(sendstuff, "PRIVMSG %s :\001DCC CHAT CHAT %lu %d\001\r\n", nick, addr, my_settings.current_port);
    send_text(sendstuff);

    sprintf(buffer3, "%s%sDCC%s %s %s %s %s port %i", timestamp, GetCatalogStr(catalog, 217, "["),
            GetCatalogStr(catalog, 218, "]"), GetCatalogStr(catalog, 195, "Sending DCC CHAT request"),
            GetCatalogStr(catalog, 194, "to"), dcc_chat_conductor->nick, GetCatalogStr(catalog, 193, "on"),
            my_settings.current_port);
    status_conductor->conductor = dcc_chat_conductor->conductor;
    add_text_to_conductor_list(buffer3, 9, ACTIVITY_CHAT);

    my_settings.current_port++;

}

void create_recv_dcc(char *nick, char *filename, char *address, unsigned short port, char *filesize)
{

    if (DEBUG)
        printf("dcc 1\n");

    dcc_conductor = dcc_root;
    dcc_transfers = 0;
    if (DEBUG)
        printf("dcc 2\n");

    while (dcc_conductor->next)
    {
        if (!dcc_conductor->removed)
            dcc_transfers++;
        dcc_conductor = dcc_conductor->next;
    }

    dcc_root->removed = 1;
    dcc_work = dcc_conductor;
    dcc_conductor->next = new dcc;
    dcc_conductor = dcc_conductor->next;
    dcc_conductor->next = NULL;
    dcc_conductor->previous = dcc_work;
    if (DEBUG)
        printf("dcc 3\n");

    if (!strcmp(nick, "0") && !strcmp(filename, "0"))
        return;

    strcpy(dcc_conductor->networkname, status_conductor->networkname);

    dcc_conductor->dcc_socket = socket(AF_INET, SOCK_STREAM, 0);
    dcc_conductor->total_recv = 0;
    if (fdmax < dcc_conductor->dcc_socket)
        fdmax = dcc_conductor->dcc_socket;
    if (DEBUG)
        printf("dcc 4\n");

    //printf("new socket created %i\n",dcc_conductor->dcc_socket);

    FD_SET(dcc_conductor->dcc_socket, &read_master);

    if (DEBUG)
        printf("dcc 5\n");

    // Create a DCC Entry for the DCC WINDOW NLIST then Fill in Values

    dcc_conductor->entry = new dcc_entry;

    if (DEBUG)
        printf("dcc 6\n");

    strcpy(dcc_conductor->address, address);
    sprintf(dcc_conductor->port, "%i", port);
    dcc_conductor->connected = 0;
    dcc_conductor->dcc_file = 0;
    dcc_conductor->removed = 0;

    strcpy(dcc_conductor->entry->port, dcc_conductor->port);
    strcpy(dcc_conductor->entry->status, GetCatalogStr(catalog, 206, "Waiting"));
    if (nick)
        strcpy(dcc_conductor->entry->nick, nick);
    if (nick)
        strcpy(dcc_conductor->nick, nick);
    else
        strcpy(dcc_conductor->entry->nick, "Unknown?");
    strcpy(dcc_conductor->entry->percentage, "0%");
    strcpy(dcc_conductor->full_filename_with_path, "");

    if (filename)
        for (unsigned long count = 0; count < strlen(filename); count++)
        {
            if (filename[count] == ' ')
                filename[count] = '_';
        }

    if (filename)
        strcpy(dcc_conductor->entry->filename, filename);
    if (filename)
        strcpy(dcc_conductor->filename, filename);
    else
        strcpy(dcc_conductor->entry->filename, "Unknown?");
    strcpy(dcc_conductor->entry->bytes_transferred, "0");
    strcpy(dcc_conductor->entry->cps, "0");
    if (filesize)
        sprintf(dcc_conductor->entry->filesize, "%dkB", atoi(filesize) / 1024);
    if (filesize)
        dcc_conductor->filesize = atoi(filesize);
    strcpy(dcc_conductor->entry->timeleft, "");

    dcc_conductor->accepted = FALSE;

    //dcc_conductor->entry->bytes_transferred=0;
    dcc_conductor->completed = 0;
    dcc_conductor->cancelled = 0;

    // This is the Entry position for when we edit the DCC NLIST Info ENTRY
    dcc_conductor->pos = dcc_transfers;

    in_addr test2;
#ifdef __amigaos4__
    test2.s_addr = inet_addr(address);
#elif __AROS__
    test2.s_addr = inet_addr(address);
#elif __MORPHOS__
    test2.s_addr = inet_addr((UBYTE*)address);
#else
    test2.s_addr = inet_addr((u_char*)address);
#endif

    dcc_conductor->test.sin_addr.s_addr = test2.s_addr;
    dcc_conductor->test.sin_port = port;
    dcc_conductor->test.sin_family = AF_INET;
    memset(&(dcc_conductor->test.sin_zero), '\0', 8);

    long i = 1;

    IoctlSocket(dcc_conductor->dcc_socket, FIONBIO, (char*) &i);

    getmacro((Object*) WookieChat->WI_dcc, MUIA_Window_Activate, &string1);
    setmacro((Object*) WookieChat->WI_dcc, MUIA_Window_Activate, FALSE);

    if (!string1)
        setmacro((Object*) WookieChat->WI_dcc, MUIA_Window_Open, TRUE); //if the window is already open, dont open it again
    //or else it will pop to front and activate

    DoMethod((Object*) WookieChat->LV_dcc, MUIM_NList_InsertSingle, dcc_conductor->entry, MUIV_NList_Insert_Bottom);
    if (DEBUG)
        printf("dcc 7\n");

    IoctlSocket(dcc_conductor->dcc_socket, FIOASYNC, (char*) &i);

    setmacro((Object*) WookieChat->LV_dcc, MUIA_NList_Active, MUIV_NList_Active_Bottom);

    getmacro((Object*) WookieChat->PA_dcc_label_0, MUIA_String_Contents, &string1);

    if (DEBUG)
        printf(
                "dcc recv entry created with the following details:\nnick:%s\nfilename:%s\naddress:%s\nport:%d\nfilesize:%s\nTime to accept!\n",
                nick, filename, address, port, filesize);

    if (my_settings.dcc_recv_auto_accept)
        accept_dcc(string1);

}
