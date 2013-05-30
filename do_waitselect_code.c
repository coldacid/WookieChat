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

/* Locals */
static char uhostname[50] = "58.28.146.45"; // Default Hostname
static char uservername[50] = "uk.amigaworld.net"; // Default Server of the User
static int len;

int do_waitselect_code(void)
{

    //WaitSelect() sockets are are ready for reading
    for (int i = 0; i <= fdmax; i++)
    {

        if (FD_ISSET(i, &read_fds))
        {
            //printf("wait select\n");
            //if(DEBUG) printf("read fd\n");

            status_conductor = status_root;

            while (i != status_conductor->a_socket && i != status_conductor->ident.a_socket
                    && i != status_conductor->ident.listen_socket)
            {
                status_conductor = status_conductor->next;

                if (!status_conductor)
                {
                    status_conductor = status_root;
                    break;
                }

            }

            if (i == status_conductor->a_socket)
            {

                /*recv_thing=1;
                 while(recv_thing>0)
                 {
                 count=BUFFERSIZE-strlen(status_conductor->str)-1;
                 recv_thing=recv(status_conductor->a_socket, (i_in)status_conductor->buffer, count, 0);

                 if(recv_thing != -1) //append text to the end of our buffer
                 {
                 status_conductor->buffer[recv_thing]='\0';
                 strcat(status_conductor->str,status_conductor->buffer);
                 }
                 if(recv_thing == -1 && Errno() == 35) //we're not disconnected, but theres nothing left to read from the socket
                 {
                 recv_thing=1; break;
                 }
                 if(strlen(status_conductor->str) >= BUFFERSIZE-100) { recv_thing=1; break; }
                 }*/

                recv_thing = -1;
                count = BUFFERSIZE - strlen(status_conductor->str);
                if (count > 0)
                    recv_thing = recv(status_conductor->a_socket, (i_in) status_conductor->buffer, count - 1, 0);
                if (recv_thing != -1)
                {
                    status_conductor->buffer[recv_thing] = '\0';
                    strcat(status_conductor->str, status_conductor->buffer);
                }

                if ((status_conductor->connection_active == 1 && recv_thing == 0)
                        || (status_conductor->connection_active == 1 && (recv_thing == -1 && Errno() == 32))) //broken pipe
                {

                    status_conductor->buffer[0] = '\0';
                    status_conductor->connection_active = 0;
                    //status_conductor->pass[0]='\0';
                    //status_conductor->retry_number=0;

                    for (status_conductor->conductor = status_conductor->root; status_conductor->conductor;
                            status_conductor->conductor = status_conductor->conductor->next)
                    {
                        if (!status_conductor->connection_active
                                && status_conductor->conductor->queued_messages[0] != NULL)
                        {
                            for (count = 0;
                                    count <= status_conductor->conductor->queued_messages_total
                                            && count < MAX_QUEUED_MESSAGES - 1; count++)
                            {
                                if (status_conductor->conductor->queued_messages[count])
                                {
                                    free(status_conductor->conductor->queued_messages[count]);
                                    status_conductor->conductor->queued_messages[count] = NULL;

                                }

                            }

                            queued_messages_total = queued_messages_total
                                    - status_conductor->conductor->queued_messages_total;

                            status_conductor->conductor->queued_messages_total = 0;

                            if (DEBUG)
                                printf("tab queued messages left %d\n",
                                        status_conductor->conductor->queued_messages_total);
                            if (DEBUG)
                                printf("total queued messages left %d\n", queued_messages_total);

                        }

                    }

                    if ((my_settings.events[DISCONNECT].use_when == 1 && is_window_active())
                            || (my_settings.events[DISCONNECT].use_when == 2 && !is_window_active())
                            || my_settings.events[DISCONNECT].use_when == 3)
                    {
                        strcpy(target_nick, "");
                        sprintf(buffer3, "%sServer%s %s", GCS(catalog, 217, "["),
                                GCS(catalog, 218, "]"), GCS(catalog, 183, "Disconnected"));

                        create_arexx_event_string(my_settings.events[DISCONNECT].arexx_script,
                                my_settings.events[DISCONNECT].arexx_script2);
                        Execute((_s_cs)event_string, 0, 0);
                    }

                    sprintf(buffer3, "%s%sServer%s %s", timestamp, GCS(catalog, 217, "["),
                            GCS(catalog, 218, "]"), GCS(catalog, 183, "Disconnected"));
                    status_conductor->conductor = status_conductor->root;
                    while (status_conductor->conductor)
                    {
                        if (status_conductor->conductor->LV_channel)
                        {
                            if (status_conductor->conductor->name[0] == '#'
                                    || status_conductor->conductor == status_conductor->root)
                                add_text_to_conductor_list(buffer3, 9, ACTIVITY);
                        }
                        status_conductor->conductor = status_conductor->conductor->next;
                    }

                    if (FD_ISSET(status_conductor->a_socket, &read_master))
                        FD_CLR(status_conductor->a_socket, &read_master);

                    if (status_conductor->quit_requested == FALSE && status_conductor->retry_number == 0)
                    {
                        start_reconnect_delay_timer = TRUE;

                        status_conductor->trying_to_reconnect = TRUE;
                        automatically_reconnect_server(RECONNECT_STRAIGHT_AWAY);
                        //printf("reconnect straight away\n");
                    }
                    else if (status_conductor->quit_requested == FALSE && status_conductor->retry_number > 0)
                    {
                        start_reconnect_delay_timer = TRUE;

                        status_conductor->trying_to_reconnect = TRUE;
                        automatically_reconnect_server(START_DELAY);
                        //printf("reconnect delayed\n");
                    }
                    else
                    {

                        status_conductor->servername[0] = '\0';
                        status_conductor->networkname[0] = '\0';
                        status_conductor->trying_to_reconnect = FALSE;
                        //printf("reconnect failed\n");
                    }

                    break;

                }
                else if (!status_conductor->connection_active && recv_thing > 0)
                {
                    status_conductor->connection_active = 1;
                    start_reconnect_delay_timer = FALSE;
                    status_conductor->trying_to_reconnect = FALSE;

                    sprintf(buffer3, "%s%sServer%s %s", timestamp, GCS(catalog, 217, "["),
                            GCS(catalog, 218, "]"), GCS(catalog, 182, "Connected"));
                    if (DEBUG)
                        printf("connected!!\n");

                    add_text_to_conductor_list(buffer3, 9, ACTIVITY);

                    if (status_conductor->user_name[0] == '\0')
                        strcpy(status_conductor->user_name, "mrnobody");
                    if (uhostname[0] == '\0')
                        strcpy(uhostname, "test");
                    if (uservername[0] == '\0')
                        strcpy(uservername, "test2");
                    if (status_conductor->real_name[0] == '\0')
                        strcpy(status_conductor->real_name, "mr nobody");

                    if (status_conductor->pass[0] == '\0')
                    {
                        sprintf(sendstuff, "NICK %s\r\nUSER %s %s %s :%s\r\n", status_conductor->nick,
                                status_conductor->user_name, uhostname, uservername, status_conductor->real_name);
                    }
                    else
                    {

                        sprintf(sendstuff, "PASS %s\r\nNICK %s\r\nUSER %s %s %s :%s\r\n", status_conductor->pass,
                                status_conductor->nick, status_conductor->user_name, uhostname, uservername,
                                status_conductor->real_name);
                    }
                    send_text(sendstuff);

                    if ((my_settings.events[CONNECT].use_when == 1 && is_window_active())
                            || (my_settings.events[CONNECT].use_when == 2 && !is_window_active())
                            || my_settings.events[CONNECT].use_when == 3)
                    {
                        sprintf(buffer3, "%sServer%s %s", GCS(catalog, 217, "["),
                                GCS(catalog, 218, "]"), GCS(catalog, 182, "Connected"));

                        strcpy(target_nick, "");
                        create_arexx_event_string(my_settings.events[CONNECT].arexx_script,
                                my_settings.events[CONNECT].arexx_script2);
                        Execute((_s_cs)event_string, 0, 0);
                    }

                    break;

                }
                else if (recv_thing == 0)
                {
                    sprintf(buffer3, "%s%sConnect%s %s", timestamp, GCS(catalog, 217, "["),
                            GCS(catalog, 218, "]"), GCS(catalog, 368, "Unable to connect"));
                    add_text_to_conductor_list(buffer3, 9, ACTIVITY);
                    if (status_conductor)
                    {
                        if (FD_ISSET(status_conductor->a_socket, &write_master))
                            FD_CLR(status_conductor->a_socket, &write_master);
                        if (FD_ISSET(status_conductor->a_socket, &read_master))
                            FD_CLR(status_conductor->a_socket, &read_master);
                    }
                }

                if (recv_thing == -1)
                    break;

                if (status_conductor->str)
                {
                    if (strcmp(status_conductor->str, "") != 0)
                        process_incoming();
                }

                break;

            }
            else if (i == status_conductor->ident.listen_socket)
            {
                //printf("1\n");
                if (status_conductor->ident.listen_socket != -1)
                {
#ifdef __amigaos4__
                    socklen_t sin_size = sizeof(status_conductor->ident.their_addr);
#elif __AROS__
                    socklen_t sin_size = sizeof(status_conductor->ident.their_addr);
#else
                    long sin_size = sizeof(status_conductor->ident.their_addr);
#endif

                    status_conductor->ident.a_socket = accept(status_conductor->ident.listen_socket,
                            (struct sockaddr *) &status_conductor->ident.their_addr, &sin_size);
                    if (DEBUG)
                        printf("trying to accept connection for ident\n");

                    if (FD_ISSET(status_conductor->ident.listen_socket, &read_master))
                        FD_CLR(status_conductor->ident.listen_socket, &read_master);

                    status_conductor->ident.listen_socket = -1;

                    if (status_conductor->ident.a_socket != -1)
                    {
                        if (DEBUG)
                            printf("ident connection successful!\n");
                        FD_SET(status_conductor->ident.a_socket, &read_master);
                        if (fdmax < status_conductor->ident.a_socket)
                            fdmax = status_conductor->ident.a_socket;
                    }
                }

            }
            else if (i == status_conductor->ident.a_socket)
            {
                //printf("ident socket read\n");
                //printf("2\n");

                if (status_conductor->ident.a_socket != -1)
                {
                    recv_thing = 0;

                    recv_thing = recv(status_conductor->ident.a_socket, (i_in) status_conductor->ident.buffer,
                            BUFFERSIZE - 1, 0);
                    if (recv_thing != -1)
                        status_conductor->ident.buffer[recv_thing] = '\0';

                    if (recv_thing == -1)
                        break;
                    string1 = strtok(status_conductor->ident.buffer, "\r\n");

                    if (string1)
                    {

                        if (DEBUG)
                            printf("incoming ident text:%s\n", status_conductor->ident.buffer);

                        sprintf(string10, "%s : USERID : AMIGAOS : %s\r\n", string1, username);
                        if (DEBUG)
                            printf("now attempting to send the following ident string:%s\n", string10);

                        int total = 0; // how many bytes we've sent
                        int bytesleft = strlen(string10); // how many we have left to send
                        int n;
                        int length = strlen(string10);

                        while (total < length)
                        {
                            n = send(status_conductor->ident.a_socket, (i_in) string10 + total, bytesleft, 0);
                            if (n == -1)
                            {
                                break;
                            }
                            total += n;
                            bytesleft -= n;
                        }

                        if (FD_ISSET(status_conductor->ident.a_socket, &read_master))
                            FD_CLR(status_conductor->ident.a_socket, &read_master);

                        shutdown(status_conductor->ident.a_socket, (int) 2);
                        //CloseSocket(status_conductor->ident.a_socket);

                    }

                }
                else
                {
                    if (FD_ISSET(status_conductor->ident.a_socket, &read_master))
                        FD_CLR(status_conductor->ident.a_socket, &read_master);
                    shutdown(status_conductor->ident.a_socket, (int) 2);

                    //CloseSocket(status_conductor->ident.a_socket);
                }

            }

            //DCC RECIEVE CODE

            dcc_conductor = dcc_root->next; //the root node contains nothing, so start at root->next
            a = 0;

            while (dcc_conductor)
            {

                if (i == dcc_conductor->dcc_socket)
                    break;

                if (!dcc_conductor->removed)
                {
                    a++;
                }

                dcc_conductor = dcc_conductor->next;

                if (!dcc_conductor)
                    break;

            }

            if (dcc_conductor)
            {

                //printf("4\n");

                if (i == dcc_conductor->dcc_socket)
                {

                    if (DEBUG)
                        printf("dcc recv socket has been hit!\n");

                    recv_thing = 0;

                    if (!dcc_conductor->connected)
                    {
                        //printf("opening %s, dcc_socket:%i\n",dcc_conductor->filename, dcc_conductor->dcc_socket);
                        if (DEBUG)
                            printf("dcc recv socket: not connected yet, so lets open the file..\n");

                        //if(dcc_conductor->filename)
                        if (stricmp(dcc_conductor->full_filename_with_path, ""))
                        {
                            if (DEBUG)
                                printf("dcc recv socket: filename okay?\n");

                            dcc_conductor->connected = 1;

                            //dcc_conductor->dcc_file=0;
                            if (dcc_conductor->dcc_file)
                            {
                                Close(dcc_conductor->dcc_file);
                                dcc_conductor->dcc_file = 0;
                            }

                            if (!dcc_conductor->total_recv)
                            {
                                dcc_conductor->dcc_file = Open((_s_cs)dcc_conductor->full_filename_with_path, MODE_NEWFILE);

                            }
                            else
                            {
                                dcc_conductor->dcc_file = Open((_s_cs)dcc_conductor->full_filename_with_path, MODE_READWRITE);

                            }

                            if (dcc_conductor->dcc_file)
                            {
                                if (DEBUG)
                                    printf("dcc recv socket: save file has been opened successfully..\n");

                                Close(dcc_conductor->dcc_file);
#ifdef __MORPHOS__
                                sprintf(buffer2,"%s <%s> @ %s",dcc_conductor->nick,(i_in)Inet_NtoA(inet_addr((UBYTE*)dcc_conductor->address)),dcc_conductor->networkname);
#elif __AROS__
                                sprintf(buffer2, "%s <%s> @ %s", dcc_conductor->nick,
                                        (i_in) Inet_NtoA(inet_addr((const char*) dcc_conductor->address)),
                                        dcc_conductor->networkname);
#else
                                sprintf(buffer2,"%s <%s> @ %s",dcc_conductor->nick,(i_in)Inet_NtoA(inet_addr((i_in)dcc_conductor->address)),dcc_conductor->networkname);
#endif

#ifndef __AROS__
                                SetComment(dcc_conductor->full_filename_with_path,buffer2);
#endif

                                dcc_conductor->dcc_file = Open((_s_cs)dcc_conductor->full_filename_with_path, MODE_READWRITE);
#ifdef __amigaos4__
                                if(dcc_conductor->dcc_file) ChangeFilePosition(dcc_conductor->dcc_file,0,OFFSET_END);
#else
                                if (dcc_conductor->dcc_file)
                                    Seek(dcc_conductor->dcc_file, 0, OFFSET_END);
#endif
                            }

                            dcc_time();

                            strcpy(dcc_conductor->timestarted, dcctimestamp);

                            if (dcc_conductor->dcc_file == 0)
                            {
                                printf("%s\n",
                                        GCS(catalog, 185, "unable to create new file for incoming transfer"));
                                //if(dcc_conductor->dcc_file) Close(dcc_conductor->dcc_file);
                            }

                        }

                        if (DEBUG)
                            printf("dcc recv socket: replacing File Transfer entry\n");

                        if (dcc_conductor->entry)
                        {
                            strcpy(dcc_conductor->entry->status, (char *)GCS(catalog, 148, "Transferring"));
                            strcpy(dcc_conductor->entry->percentage, "0%");
                        }

                        DoMethod((Object*) WookieChat->LV_dcc, MUIM_NList_ReplaceSingle, dcc_conductor->entry, a,
                                NOWRAP, ALIGN_LEFT);

                    }

                    recv_thing = recv(dcc_conductor->dcc_socket, (i_in) dcc_conductor->buffer, DCC_RECV_BUFFERSIZE - 1,
                            0);

                    if (recv_thing == 0)
                    {

                        if (DEBUG)
                            printf(
                                    "dcc recv socket: connection has been closed somehow for some reason. could be an error, could be completed\n");

                        dcc_conductor->buffer[0] = '\0';

                        if (dcc_conductor->connected && !dcc_conductor->cancelled)
                        {

                            if (dcc_conductor->total_recv >= dcc_conductor->filesize)
                            {
                                if (dcc_conductor->entry)
                                {

                                    strcpy(dcc_conductor->entry->status, (char *)GCS(catalog, 179, "Complete")); //"Completed");
                                    strcpy(dcc_conductor->entry->timeleft, "N/A"); //Done");
                                    sprintf(dcc_conductor->entry->bytes_transferred, "%luKB",
                                            dcc_conductor->total_recv / 1024);
                                    float pct = (float) dcc_conductor->total_recv / (float) dcc_conductor->filesize;
                                    sprintf(dcc_conductor->entry->percentage, "%.0f%%", (float) (pct * 100));
                                }

                                DoMethod((Object*) WookieChat->LV_dcc, MUIM_NList_ReplaceSingle, dcc_conductor->entry,
                                        a, NOWRAP, ALIGN_LEFT);

                                dcc_time_to_recv();

                            }
                            else
                            {
                                if (dcc_conductor->entry)
                                {

                                    strcpy(dcc_conductor->entry->status, (char *)GCS(catalog, 180, "Incomplete")); //"Incomplete");
                                    strcpy(dcc_conductor->entry->timeleft, "N/A");
                                    float pct = 0;
                                    if (dcc_conductor->total_recv > 0)
                                    {
                                        sprintf(dcc_conductor->entry->bytes_transferred, "%luKB",
                                                dcc_conductor->total_recv / 1024);
                                        pct = (float) dcc_conductor->total_recv / (float) dcc_conductor->filesize;
                                    }
                                    sprintf(dcc_conductor->entry->percentage, "%.0f%%", (float) (pct * 100));
                                }

                                DoMethod((Object*) WookieChat->LV_dcc, MUIM_NList_ReplaceSingle, dcc_conductor->entry,
                                        a, NOWRAP, ALIGN_LEFT);

                                sprintf(buffer3, "%s%sDCC%s SEND: %s %s %s %s", timestamp,
                                        GCS(catalog, 217, "["), GCS(catalog, 218, "]"),
                                        dcc_conductor->filename, GCS(catalog, 173, "offered from"),
                                        dcc_conductor->nick, GCS(catalog, 180, "Incomplete"));

                                if (dcc_conductor->dcc_file)
                                {
                                    Close(dcc_conductor->dcc_file);
                                    dcc_conductor->dcc_file = 0;
                                }
                            }

                            cleanup_dcc_recv();

                            dcc_conductor->completed = 1;

                        }
                        else if (dcc_conductor->cancelled && !strcmp(dcc_conductor->entry->status, "Removed"))
                        {

                            //do nothing
                            dcc_conductor->connected = 0;

                            if (dcc_conductor->dcc_file)
                            {
                                Close(dcc_conductor->dcc_file);
                                dcc_conductor->dcc_file = 0;
                            }
                        }
                        else if (dcc_conductor->cancelled)
                        {

                            if (dcc_conductor->entry)
                            {
                                strcpy(dcc_conductor->entry->status, (char *)GCS(catalog, 181, "Cancelled"));
                                float pct = 0;

                                if (dcc_conductor->total_recv > 0)
                                {
                                    sprintf(dcc_conductor->entry->bytes_transferred, "%luKB",
                                            dcc_conductor->total_recv / 1024);
                                    pct = (float) dcc_conductor->total_recv / (float) dcc_conductor->filesize;
                                }
                                sprintf(dcc_conductor->entry->percentage, "%.0f%%", (float) (pct * 100));
                            }

                            DoMethod((Object*) WookieChat->LV_dcc, MUIM_NList_ReplaceSingle, dcc_conductor->entry, a,
                                    NOWRAP, ALIGN_LEFT);
                            sprintf(buffer3, "%s%sDCC%s SEND: %s %s %s *%s*", timestamp,
                                    GCS(catalog, 217, "["), GCS(catalog, 218, "]"),
                                    dcc_conductor->filename, GCS(catalog, 173, "offered from"),
                                    dcc_conductor->nick, GCS(catalog, 181, "Cancelled"));

                            //dcc_conductor->connected=0;

                            cleanup_dcc_recv();

                        }
                        else
                        {

                            if (dcc_conductor->entry)
                            {
                                strcpy(dcc_conductor->entry->status, (char *)GCS(catalog, 184, "Failed"));
                                float pct = 0;

                                if (dcc_conductor->total_recv > 0)
                                {

                                    sprintf(dcc_conductor->entry->bytes_transferred, "%luKB",
                                            dcc_conductor->total_recv / 1024);
                                    pct = (float) dcc_conductor->total_recv / (float) dcc_conductor->filesize;
                                }
                                sprintf(dcc_conductor->entry->percentage, "%.0f%%", (float) (pct * 100));
                            }

                            DoMethod((Object*) WookieChat->LV_dcc, MUIM_NList_ReplaceSingle, dcc_conductor->entry, a,
                                    NOWRAP, ALIGN_LEFT);
                            sprintf(buffer3, "%s%sDCC%s SEND: %s %s %s %s", timestamp, GCS(catalog, 217, "["),
                                    GCS(catalog, 218, "]"), dcc_conductor->filename,
                                    GCS(catalog, 173, "offered from"), dcc_conductor->nick,
                                    GCS(catalog, 184, "Failed"));

                            //2006-10-04
                            //dcc_conductor->connected=0;

                            cleanup_dcc_recv();

                        }

                        //28th august 2006 change

                        if (dcc_conductor->dcc_file)
                        {
                            Close(dcc_conductor->dcc_file);
                            dcc_conductor->dcc_file = 0;
                        }
                        shutdown_my_dcc_recv_socket();

                        break;
                    }

                    if (recv_thing != -1)
                    {
                        dcc_conductor->total_recv += recv_thing;
                        dcc_conductor->buffer[recv_thing] = '\0';
                        if (DEBUG)
                            printf("dcc recv socket: received %lu bytes so far..\n", dcc_conductor->total_recv);

#ifdef __amigaos4__
                        long pos = htonl(dcc_conductor->total_recv);
                        len=send(dcc_conductor->dcc_socket,(i_in) &pos, 4, 0);
#elif __AROS__
                        long pos = htonl(dcc_conductor->total_recv);
                        len = send(dcc_conductor->dcc_socket, (i_in) &pos, 4, 0);
#else
                        ULONG pos = htonl(dcc_conductor->total_recv);
                        len=send(dcc_conductor->dcc_socket,(i_in) &pos, 4, 0);
#endif
                    }

                    if (recv_thing > 0)
                    {
#ifdef __amigaos4__
                        if(dcc_conductor->dcc_file) Write(dcc_conductor->dcc_file, dcc_conductor->buffer, recv_thing);
#else
                        if (dcc_conductor->dcc_file)
                            Write(dcc_conductor->dcc_file, dcc_conductor->buffer, recv_thing);
#endif
                    }

                    break; //2006-01-10 ..experimental break to improve performance
                }

            }

            //DCC SEND CODE

            dcc_send_conductor = dcc_send_root->next; //the root node contains nothing, so start at root->next
            a = 0;
            while (dcc_send_conductor)
            {

                if (i == dcc_send_conductor->dcc_socket)
                    break;

                if (!dcc_send_conductor->removed)
                {
                    a++;
                }

                dcc_send_conductor = dcc_send_conductor->next;

                if (!dcc_send_conductor)
                    break;

            }

            if (dcc_send_conductor)
            {

                //printf("3\n");

                if (i == dcc_send_conductor->dcc_socket)
                {

                    if (dcc_send_conductor->connected)
                        dcc_send_data(a);

                    //printf("dcc_send_conductor\n");

                    //break; //2006-01-10 ..experimental break to improve performance

                }

            }

            //DCC SEND LISTEN CODE

            dcc_send_conductor = dcc_send_root->next; //the root node contains nothing, so start at root->next
            a = 0;
            while (dcc_send_conductor)
            {

                if (i == dcc_send_conductor->dcc_listen_socket)
                    break;

                if (!dcc_send_conductor->removed)
                {
                    a++;
                }

                dcc_send_conductor = dcc_send_conductor->next;

                if (!dcc_send_conductor)
                    break;

            }

            if (dcc_send_conductor)
            {

                if (i == dcc_send_conductor->dcc_listen_socket)
                {
                    //printf("dcc_send_conductor listen\n");

                    status_conductor = status_root;
                    while (status_conductor)
                    {
                        if (!stricmp(status_conductor->networkname, dcc_send_conductor->networkname))
                            break;
                        status_conductor = status_conductor->next;
                        if (!status_conductor)
                        {
                            status_conductor = status_root;
                            break;
                        }
                    }

                    status_conductor->conductor = status_conductor->current_query;

                    if (!dcc_send_conductor->connected
                            && (dcc_send_conductor->total_recv != dcc_send_conductor->filesize))
                    {
                        if (dcc_send_conductor->filename)
                        {

                            dcc_send_conductor->connected = 1;

#ifdef __aamigaos4__
                            if(dcc_send_conductor->filepath && !dcc_send_conductor->dcc_file) dcc_send_conductor->dcc_file = Open(dcc_send_conductor->filepath,MODE_OLDFILE);
                            if(dcc_send_conductor->dcc_file) ChangeFilePosition(dcc_send_conductor->dcc_file,dcc_send_conductor->total_recv,OFFSET_BEGINNING);
#else
                            if (dcc_send_conductor->filepath && !dcc_send_conductor->dcc_file)
                                dcc_send_conductor->dcc_file = Open((_s_cs)dcc_send_conductor->filepath, MODE_OLDFILE);
                            if (dcc_send_conductor->dcc_file)
                                Seek(dcc_send_conductor->dcc_file, dcc_send_conductor->total_recv, OFFSET_BEGINNING);
#endif

                            dcc_time();
                            strcpy(dcc_send_conductor->timestarted, dcctimestamp);

                            if (dcc_send_conductor->dcc_file == 0)
                            {
                                sprintf(buffer3, "%s%sDCC%s %s", timestamp, GCS(catalog, 217, "["),
                                        GCS(catalog, 218, "]"),
                                        GCS(catalog, 186, "unable to open requested file"));
                                //add_text_to_current_list(buffer3,9, ACTIVITY_CHAT);
                                add_text_to_conductor_list(buffer3, 9, ACTIVITY_CHAT);

                            }
                        }

                        if (dcc_send_conductor->entry)
                        {

                            strcpy(dcc_send_conductor->entry->status, (char *)GCS(catalog, 148, "Transferring")); //"Transferring");
                            strcpy(dcc_send_conductor->entry->percentage, "0%");
                        }

                        DoMethod((Object*) WookieChat->LV_send_dcc, MUIM_NList_ReplaceSingle, dcc_send_conductor->entry,
                                a, NOWRAP, ALIGN_LEFT);

#ifdef __amigaos4__
                        socklen_t sin_size = sizeof(dcc_send_conductor->their_addr);
#elif __AROS__
                        socklen_t sin_size = sizeof(dcc_send_conductor->their_addr);
#else
                        long sin_size = sizeof(dcc_send_conductor->their_addr);
#endif

                        dcc_send_conductor->dcc_socket = accept(dcc_send_conductor->dcc_listen_socket,
                                (struct sockaddr *) &dcc_send_conductor->their_addr, &sin_size);

                        if (dcc_send_conductor->dcc_socket == -1)
                        {
                            timestamp_2_string();

                            sprintf(buffer3, "%s[DCC%s %s %s %s %s error number:%li", timestamp,
                                    GCS(catalog, 218, "]"), dcc_send_conductor->filename,
                                    GCS(catalog, 205, "to"), dcc_send_conductor->nick,
                                    GCS(catalog, 184, "failed"), Errno());
                            //add_text_to_current_list(buffer3,9, ACTIVITY_CHAT);
                            add_text_to_conductor_list(buffer3, 9, ACTIVITY_CHAT);

                            if (FD_ISSET(dcc_send_conductor->dcc_listen_socket, &read_master))
                                FD_CLR(dcc_send_conductor->dcc_listen_socket, &read_master);
                        }
                        else
                        {
                            timestamp_2_string();

                            if (dcc_send_conductor->entry
                                    && (dcc_send_conductor->total_recv != dcc_send_conductor->filesize))
                            {

                                strcpy(dcc_send_conductor->entry->status, (char *)GCS(catalog, 148, "Transferring")); //"Transferring");
                                strcpy(dcc_send_conductor->entry->timeleft, "");
                                sprintf(dcc_send_conductor->entry->bytes_transferred, "0KB");
                                sprintf(dcc_send_conductor->entry->percentage, "0%%");
                            }

                            if (dcc_send_conductor->total_recv == 0)
                            {
                                sprintf(buffer3, "%s%sDCC%s SEND: %s %s %s %s", timestamp,
                                        GCS(catalog, 217, "["), GCS(catalog, 218, "]"),
                                        GCS(catalog, 187, "Started file transfer:"),
                                        dcc_send_conductor->filename, GCS(catalog, 205, "to"),
                                        dcc_send_conductor->nick);
                                //add_text_to_current_list(buffer3,9, ACTIVITY_CHAT);
                                add_text_to_conductor_list(buffer3, 9, ACTIVITY_CHAT);

                                if ((my_settings.events[DCC_SEND_FINISHED].use_when == 1 && is_window_active())
                                        || (my_settings.events[DCC_SEND_FINISHED].use_when == 2 && !is_window_active())
                                        || my_settings.events[DCC_SEND_FINISHED].use_when == 3)
                                {
                                    strcpy(target_nick, "");
                                    create_arexx_event_string(my_settings.dcc_send_finished.arexx_script,
                                            my_settings.dcc_send_finished.arexx_script2);
                                    Execute((_s_cs)event_string, 0, 0);
                                }

                            }

                            if (FD_ISSET(dcc_send_conductor->dcc_listen_socket, &read_master))
                                FD_CLR(dcc_send_conductor->dcc_listen_socket, &read_master);
                            FD_SET(dcc_send_conductor->dcc_socket, &read_master);
                            if (fdmax < dcc_send_conductor->dcc_socket)
                                fdmax = dcc_send_conductor->dcc_socket;

                            actualLength = Read(dcc_send_conductor->dcc_file, (i_in) dcc_send_conductor->buffer, 1024);
                            send_thing = send(dcc_send_conductor->dcc_socket, (i_in) dcc_send_conductor->buffer,
                                    actualLength, 0);
                            if (send_thing != -1)
                            {
                                dcc_send_conductor->total_recv += send_thing;
                            }

#ifdef __aamigaos4__
                            if(send_thing != actualLength) ChangeFilePosition(dcc_send_conductor->dcc_file,dcc_send_conductor->total_recv,OFFSET_BEGINNING);
#else
                            if (send_thing != actualLength)
                                Seek(dcc_send_conductor->dcc_file, dcc_send_conductor->total_recv, OFFSET_BEGINNING);
#endif

                            if (dcc_send_conductor->total_recv == dcc_send_conductor->filesize)
                            {

                                dcc_send_conductor->buffer[0] = '\0';

                                //remove bottom
                                if (dcc_send_conductor->connected && !dcc_send_conductor->cancelled)
                                {

                                    if (dcc_send_conductor->entry)
                                    {

                                        strcpy(dcc_send_conductor->entry->status,
                                                (char *)GCS(catalog, 179, "Complete")); //"Completed");
                                        strcpy(dcc_send_conductor->entry->timeleft, "N/A"); //Done");
                                        sprintf(dcc_send_conductor->entry->bytes_transferred, "%luKB",
                                                dcc_send_conductor->total_recv / 1024);
                                        float pct = (float) dcc_send_conductor->total_recv
                                                / (float) dcc_send_conductor->filesize;
                                        sprintf(dcc_send_conductor->entry->percentage, "%.0f%%", (float) (pct * 100));
                                    }

                                    DoMethod((Object*) WookieChat->LV_send_dcc, MUIM_NList_ReplaceSingle,
                                            dcc_send_conductor->entry, a, NOWRAP, ALIGN_LEFT);

                                    //sprintf(buffer3,"%s%sDCC%s SEND: %s to %s complete",timestamp,GCS(catalog,217,"["),GCS(catalog,218,"]"),dcc_send_conductor->filename,dcc_send_conductor->nick);

                                    //commented out the bottom part to see if small files can be labelled "complete" instead of stuck on "transffering"
                                    //dcc_time_to_send();
                                    //dcc_send_conductor->completed=1;
                                    //dcc_send_conductor->connected=0;

                                    cleanup_dcc_send(DONT_FLAG_AS_COMPLETED);

                                }

                            }
                            else if (send_thing == -1)
                            {

                                if (Errno() == 57 || Errno() == 34 || Errno() == 32) //Error, not connected!!
                                {

                                    dcc_send_conductor->buffer[0] = '\0';

                                    if (dcc_send_conductor->connected)
                                    {

                                        if (dcc_send_conductor->entry)
                                        {

                                            strcpy(dcc_send_conductor->entry->status,
                                                    (char *)GCS(catalog, 180, "Incomplete")); //"Incomplete");
                                            strcpy(dcc_send_conductor->entry->timeleft, "N/A"); //Done");
                                            float pct = 0;

                                            if (dcc_send_conductor->total_recv > 0)
                                            {
                                                sprintf(dcc_send_conductor->entry->bytes_transferred, "%luKB",
                                                        dcc_send_conductor->total_recv / 1024);
                                                pct = (float) dcc_send_conductor->total_recv
                                                        / (float) dcc_send_conductor->filesize;
                                            }
                                            sprintf(dcc_send_conductor->entry->percentage, "%.0f%%",
                                                    (float) (pct * 100));
                                        }

                                        DoMethod((Object*) WookieChat->LV_send_dcc, MUIM_NList_ReplaceSingle,
                                                dcc_send_conductor->entry, a, NOWRAP, ALIGN_LEFT);

                                        sprintf(buffer3, "%s%sDCC%s SEND: %s %s %s %s", timestamp,
                                                GCS(catalog, 217, "["), GCS(catalog, 218, "]"),
                                                dcc_send_conductor->filename, GCS(catalog, 205, "to"),
                                                dcc_send_conductor->nick, GCS(catalog, 180, "Incomplete"));

                                        if ((my_settings.events[DCC_SEND_FINISHED].use_when == 1 && is_window_active())
                                                || (my_settings.events[DCC_SEND_FINISHED].use_when == 2
                                                        && !is_window_active())
                                                || my_settings.events[DCC_SEND_FINISHED].use_when == 3)
                                        {
                                            strcpy(target_nick, "");
                                            create_arexx_event_string(my_settings.dcc_send_finished.arexx_script,
                                                    my_settings.dcc_send_finished.arexx_script2);
                                            Execute((_s_cs)event_string, 0, 0);
                                        }

                                        cleanup_dcc_send(FLAG_AS_COMPLETED);

                                    }
                                }

                            }

                        }

                    }

                    //break;

                }

            }

            dcc_chat_conductor = dcc_chat_root->next; //the root node contains nothing, so start at root->next

            while (dcc_chat_conductor)
            {

                if (i == dcc_chat_conductor->dcc_socket || i == dcc_chat_conductor->dcc_listen_socket)
                    break;
                dcc_chat_conductor = dcc_chat_conductor->next;
                if (!dcc_chat_conductor)
                    break;

            }

            if (dcc_chat_conductor)
            {
                if (DEBUG)
                    printf("dcc chat conductor being processed\n");

                if (i == dcc_chat_conductor->dcc_socket)
                {
                    recv_thing = 0;
                    //recv_thing=recv(dcc_chat_conductor->dcc_socket, (i_in)dcc_chat_conductor->buffer, BUFFERSIZE-1, 0);
                    recv_thing = recv(dcc_chat_conductor->dcc_socket, (i_in) dcc_chat_conductor->recv_buffer,
                            BUFFERSIZE - 1, 0);
                    if (recv_thing == 0)
                    {

                        dcc_chat_conductor->recv_buffer[0] = '\0';

                        if (dcc_chat_conductor->connected)
                        {
                            dcc_chat_conductor->connected = 0;
                            strcpy(dcc_chat_conductor->name, "");

                            sprintf(buffer3, "%s%sDCC%s DCC CHAT %s", timestamp, GCS(catalog, 217, "["),
                                    GCS(catalog, 218, "]"), GCS(catalog, 183, "Disconnected"));
                            status_conductor->conductor = dcc_chat_conductor->conductor;
                            add_text_to_conductor_list(buffer3, 9, ACTIVITY_CHAT);
                            if (FD_ISSET(dcc_chat_conductor->dcc_socket, &read_master))
                                FD_CLR(dcc_chat_conductor->dcc_socket, &read_master);

                            //remove this entry out of the DCC CHAT linked list
                            if (dcc_chat_conductor->next)
                            {
                                dcc_chat_work = dcc_chat_conductor->previous;
                                dcc_chat_work->next = dcc_chat_conductor->next;
                                if (dcc_chat_conductor)
                                    free(dcc_chat_conductor);

                            }
                            else
                            {
                                dcc_chat_work = dcc_chat_conductor->previous;
                                dcc_chat_work->next = NULL;
                                if (dcc_chat_conductor)
                                    free(dcc_chat_conductor);
                                dcc_chat_conductor = NULL;
                            }

                        }

                    }
                    else if (recv_thing > 0)
                    {

                        //dcc_chat_conductor->buffer[recv_thing]='\0';
                        dcc_chat_conductor->recv_buffer[recv_thing] = '\0';

                        if (!dcc_chat_conductor->connected)
                        {
                            dcc_chat_conductor->connected = 1;
                            sprintf(buffer3, "%s%sDCC%s DCC CHAT %s", timestamp, GCS(catalog, 217, "["),
                                    GCS(catalog, 218, "]"), GCS(catalog, 182, "Connected"));
                            status_conductor->conductor = dcc_chat_conductor->conductor;
                            add_text_to_conductor_list(buffer3, 9, ACTIVITY_CHAT);
                            strcpy(dcc_chat_conductor->buffer, "");
                            //strcpy(dcc_chat_conductor->str,"");

                        }
                        //if(strcmp(dcc_chat_conductor->buffer,"") != 0)
                        if (strcmp(dcc_chat_conductor->recv_buffer, "") != 0)
                        {
                            strcat(dcc_chat_conductor->str, dcc_chat_conductor->recv_buffer);
                            //dcc_chat_conductor->str=dcc_chat_conductor->buffer;

                            //pch3=strstr(dcc_chat_conductor->str,"\n");

                            //strcat(dcc_chat_conductor->buffer,dcc_chat_conductor->recv_buffer);
                            //pch3=strstr(dcc_chat_conductor->buffer,"\n");
                            //if(pch3) process_dcc_chat_incoming();

                            process_dcc_chat_incoming();

                        }
                    }
                }
                else if (i == dcc_chat_conductor->dcc_listen_socket)
                {

                    dcc_chat_connect();

                }

            }

        }
        if (FD_ISSET(i, &write_fds))
        {

            //if(DEBUG) printf("write fd\n");

            status_conductor = status_root;

            while (i != status_conductor->a_socket)
            {
                status_conductor = status_conductor->next;

                if (!status_conductor)
                {
                    status_conductor = status_root;
                    break;
                }

            }

            if (i == status_conductor->a_socket)
            {

                if (!status_conductor->connection_active)
                {

                    status_conductor->connection_active = 1;

                    if (DEBUG)
                        printf("connected!!\n");

                    sprintf(buffer3, "%s%sServer%s %s", timestamp, GCS(catalog, 217, "["),
                            GCS(catalog, 218, "]"), GCS(catalog, 182, "Connected"));

                    add_text_to_conductor_list(buffer3, 9, ACTIVITY);

                    if (status_conductor->user_name[0] == '\0')
                        strcpy(status_conductor->user_name, "mrnobody");
                    if (uhostname[0] == '\0')
                        strcpy(uhostname, "test");
                    if (uservername[0] == '\0')
                        strcpy(uservername, "test2");
                    if (status_conductor->real_name[0] == '\0')
                        strcpy(status_conductor->real_name, "mr nobody");

                    if (status_conductor->pass[0] == '\0')
                    {
                        sprintf(sendstuff, "NICK %s\r\nUSER %s %s %s :%s\r\n", status_conductor->nick,
                                status_conductor->user_name, uhostname, uservername, status_conductor->real_name);
                    }
                    else
                    {

                        sprintf(sendstuff, "PASS %s\r\nNICK %s\r\nUSER %s %s %s :%s\r\n", status_conductor->pass,
                                status_conductor->nick, status_conductor->user_name, uhostname, uservername,
                                status_conductor->real_name);
                    }
                    send_text(sendstuff);

                    if ((my_settings.events[CONNECT].use_when == 1 && is_window_active())
                            || (my_settings.events[CONNECT].use_when == 2 && !is_window_active())
                            || my_settings.events[CONNECT].use_when == 3)
                    {
                        sprintf(buffer3, "%sServer%s %s", GCS(catalog, 217, "["),
                                GCS(catalog, 218, "]"), GCS(catalog, 182, "Connected"));

                        strcpy(target_nick, "");
                        create_arexx_event_string(my_settings.events[CONNECT].arexx_script,
                                my_settings.events[CONNECT].arexx_script2);
                        Execute((_s_cs)event_string, 0, 0);
                    }

                    break;

                }

                if (FD_ISSET(status_conductor->a_socket, &write_master))
                    FD_CLR(status_conductor->a_socket, &write_master);
                //printf("1\n");
            }

            dcc_chat_conductor = dcc_chat_root->next; //the root node contains nothing, so start at root->next

            while (dcc_chat_conductor)
            {
                if (i == dcc_chat_conductor->dcc_socket)
                    break;
                dcc_chat_conductor = dcc_chat_conductor->next;
                if (!dcc_chat_conductor)
                    break;

            }

            if (dcc_chat_conductor)
            {
                if (DEBUG)
                    printf("dcc chat conductor being processed\n");

                if (i == dcc_chat_conductor->dcc_socket)
                {

                    if (!dcc_chat_conductor->connected)
                    {

                        dcc_chat_conductor->connected = 1;

                        sprintf(buffer3, "%s%sDCC%s %s", timestamp, GCS(catalog, 217, "["),
                                GCS(catalog, 218, "]"),
                                GCS(catalog, 189, "DCC CHAT connection established"));
                        status_conductor->conductor = dcc_chat_conductor->conductor;
                        add_text_to_conductor_list(buffer3, 9, ACTIVITY);
                        //strcpy(dcc_chat_conductor->str,"");

                    }

                    if (dcc_chat_conductor->dcc_listen_socket)
                    {
                        //    if(FD_ISSET(dcc_chat_conductor->dcc_listen_socket, &read_master)) FD_CLR(dcc_chat_conductor->dcc_listen_socket, &read_master);
                    }
                    if (FD_ISSET(dcc_chat_conductor->dcc_socket, &write_master))
                        FD_CLR(dcc_chat_conductor->dcc_socket, &write_master);

                }
            }

        }

    }

    return 0;

}
