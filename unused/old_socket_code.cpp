/*
    The contents of this file are subject to the AROS Public License Version 1.1 
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis, 
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
    for the specific language governing rights and limitations under the License.
*/

        /*if(waitsignals & socket_signal)
		{


            while((ev_sock=GetSocketEvents(&ev_type)) != -1)
            {


                //if(DEBUG) printf("ev_sock:%i\n",ev_sock);

                //if(!ev_sock) break;

                LONG a=0;


                //BELOW IS THE DCC SEND SOCKET CODE

                dcc_send_conductor=dcc_send_root->next;

                if(dcc_send_conductor)
                {
                    while(ev_sock != dcc_send_conductor->dcc_listen_socket)
                    {


                        printf("listen socket search\n");

                        if(!dcc_send_conductor->removed)
                        {
                            a++;
                        }

                        dcc_send_conductor=dcc_send_conductor->next;

                        if(!dcc_send_conductor) break;



                    }
                }

                if(dcc_send_conductor && (ev_sock == dcc_send_conductor->dcc_listen_socket))
                {

                    //printf("ev_type:%s\n",ev_type);

                    if(ev_type & FD_READ)
                    {

                        int sin_size = sizeof(struct sockaddr_in);


                        dcc_send_conductor->dcc_socket = accept(dcc_send_conductor->dcc_listen_socket, (struct sockaddr *)&dcc_send_conductor->their_addr, &sin_size);
                        if(dcc_send_conductor->dcc_socket == -1)
                            printf("unable to accept\nerror number:%i\n",Errno());



                    }
                    if(ev_type & FD_CONNECT)
                    {

                        printf("dcc send: connect\n");
                    }

        			if(ev_type & FD_ACCEPT)
                    {

                        printf("accepted\n");


                        #ifdef __amigaos4__
                        int sin_size = sizeof(struct sockaddr_in);
                        #else
                        long sin_size = sizeof(struct sockaddr_in);
                        #endif


                        dcc_send_conductor->dcc_socket = accept(dcc_send_conductor->dcc_listen_socket, (struct sockaddr *)&dcc_send_conductor->their_addr, &sin_size);
                        Seek(dcc_send_conductor->dcc_file,0,OFFSET_BEGINNING);
                        actual_read = Read(dcc_send_conductor->dcc_file, dcc_send_conductor->outgoing_buffer, BUFFERSIZE);
                        int len=0;
                        len=send(dcc_send_conductor->dcc_socket, (i_in)dcc_send_conductor->outgoing_buffer, actual_read, 0);
                        printf("len:%i\n",len);

                    }



                }


                //BELOW IS THE DCC RECIEVE SOCKET CODE


                a=0;
                dcc_conductor=dcc_root->next;

                if(dcc_conductor)
                {
                    while(ev_sock != dcc_conductor->dcc_socket)
                    {

                        if(!dcc_conductor->removed)
                        {
                            a++;
                        }

                        dcc_conductor=dcc_conductor->next;

                        if(!dcc_conductor) break;



                    }
                }

                if(dcc_conductor && (ev_sock == dcc_conductor->dcc_socket))
                {

    			if(ev_type & FD_CONNECT)
                {

                    //printf("ev_sock:%i\n",ev_sock);

                    if(dcc_conductor->filename)
                    {
                        char *a=new char[200];
                        char *b=new char[200];

                        get((Object*)James->PA_dcc_label_0,MUIA_String_Contents,&b);

                        if(b[strlen(b)-1] != '/' && b[strlen(b)-1] != ':')
                            sprintf(a,"%s/%s",b,dcc_conductor->filename);
                        else
                            sprintf(a,"%s%s",b,dcc_conductor->filename);

                        dcc_conductor->connected=1;

                        if(!dcc_conductor->total_recv)
                        {

                            #ifdef __amigaos4__
                            if(USE_F) { if(a) dcc_conductor->dcc_file = FOpen((UBYTE*)a,MODE_NEWFILE,BUFFERSIZE-1);
                                        if(dcc_conductor->dcc_file) FSeek(dcc_conductor->dcc_file,0,OFFSET_END);
                            }
                            else { if(a) dcc_conductor->dcc_file = Open(a,MODE_NEWFILE);
                                if(dcc_conductor->dcc_file) Seek(dcc_conductor->dcc_file,0,OFFSET_END);
                            }
                            #else
                            if(a) dcc_conductor->dcc_file = Open(a,MODE_NEWFILE);
                                if(dcc_conductor->dcc_file) Seek(dcc_conductor->dcc_file,0,OFFSET_END);
                            #endif
                        }
                        else
                        {


                            #ifdef __amigaos4__
                            if(USE_F) { if(a) dcc_conductor->dcc_file = FOpen((UBYTE*)a,MODE_READWRITE,BUFFERSIZE-1);
                                        if(dcc_conductor->dcc_file) FSeek(dcc_conductor->dcc_file,0,OFFSET_END);
                            }
                            else { if(a) dcc_conductor->dcc_file = Open(a,MODE_READWRITE);
                                if(dcc_conductor->dcc_file) Seek(dcc_conductor->dcc_file,0,OFFSET_END);
                            }
                            #else
                            if(a) dcc_conductor->dcc_file = Open(a,MODE_READWRITE);
                                if(dcc_conductor->dcc_file) Seek(dcc_conductor->dcc_file,0,OFFSET_END);
                            #endif


                        }
                        dcc_time();
                        strcpy(dcc_conductor->timestarted,dcctimestamp);

                        if(dcc_conductor->dcc_file==0)
                        {
                            printf("unable to create new file for incoming transfer\n");
                            if(dcc_conductor->dcc_file) Close(dcc_conductor->dcc_file);
                        }
                    }

                    if(dcc_conductor->entry)
                    {
                        strcpy(dcc_conductor->entry->status,"Transferring");
                        strcpy(dcc_conductor->entry->percentage,"0%");
                    }

                    //DoMethod((Object*)James->LV_dcc,MUIM_NList_ReplaceSingle,dcc_conductor->entry,dcc_conductor->pos,NOWRAP,ALIGN_LEFT);
                    DoMethod((Object*)James->LV_dcc,MUIM_NList_ReplaceSingle,dcc_conductor->entry,a,NOWRAP,ALIGN_LEFT);


                }


    			if(ev_type & FD_READ)
                {
                        //printf("read\n");

                        recv_thing=0;

                        while(recv_thing != -1)
                        {


                            recv_thing=recv(dcc_conductor->dcc_socket, (i_in)dcc_conductor->buffer, BUFFERSIZE-1, 0);

                            if(recv_thing == -1)
                            {
                                dcc_conductor->buffer[0]='\0';
                                //if(Errno() != 35) continue;
                                break;
                            }

                            if(recv_thing == 0) break;

                            dcc_conductor->buffer[recv_thing]='\0';
                            dcc_conductor->total_recv+=recv_thing;

                            #ifdef __amigaos4__
                            uint32 pos = htonl(dcc_conductor->total_recv);
                            len=send(dcc_conductor->dcc_socket,(i_in) &pos, 4, 0);
                            #else
                            ULONG pos = htonl(dcc_conductor->total_recv);
                            len=send(dcc_conductor->dcc_socket,(i_in) &pos, 4, 0);
                            #endif


                            #ifdef __amigaos4__
                            if(USE_F)
                            {
                                if(dcc_conductor->dcc_file) FWrite(dcc_conductor->dcc_file, dcc_conductor->buffer, 1, recv_thing); //strlen(dcc_conductor->buffer));
                            }
                            else
                            {
                                if(dcc_conductor->dcc_file) Write(dcc_conductor->dcc_file, dcc_conductor->buffer, recv_thing);
                            }
                            #else
                            if(dcc_conductor->dcc_file) Write(dcc_conductor->dcc_file, dcc_conductor->buffer, recv_thing);
                            #endif



                        }



                }



    			if(ev_type & FD_CLOSE)
                {

                    if(dcc_conductor->connected && !dcc_conductor->cancelled)
                    {

                        if(dcc_conductor->entry)
                        {
                            strcpy(dcc_conductor->entry->status,"Completed");
                            strcpy(dcc_conductor->entry->timeleft,"Done");
                            sprintf(dcc_conductor->entry->bytes_transferred,"%dKB",dcc_conductor->total_recv/1024);
                            float pct = (float) dcc_conductor->total_recv / (float) dcc_conductor->filesize;
                            sprintf(dcc_conductor->entry->percentage,"%.0f%%",(float)(pct*100));
                        }

                        //DoMethod((Object*)James->LV_dcc,MUIM_NList_ReplaceSingle,dcc_conductor->entry,dcc_conductor->pos);
                        DoMethod((Object*)James->LV_dcc,MUIM_NList_ReplaceSingle,dcc_conductor->entry,a);


                        sprintf(buffer3,"%s* DCC SEND: %s from %s complete",timestamp,dcc_conductor->filename,dcc_conductor->nick);


                        dcc_conductor->completed=1;
                        dcc_conductor->connected=0;

                        #ifdef __amigaos4__
                        if(dcc_conductor->dcc_file && USE_F) FClose(dcc_conductor->dcc_file);
                        else if(dcc_conductor->dcc_file) Close(dcc_conductor->dcc_file);
                        #else
                        if(dcc_conductor->dcc_file) Close(dcc_conductor->dcc_file);
                        #endif

                                status_conductor=status_root;
                                while(status_conductor)
                                {
                                    if(!stricmp(status_conductor->networkname,dcc_conductor->networkname)) break;
                                    status_conductor=status_conductor->next;
                                    if(!status_conductor)
                                    {
                                        status_conductor=status_root;
                                        break;
                                    }
                                }

        				add_text_to_conductor_list((char*)buffer3, 9, ACTIVITY_CHAT);

                        //status_conductor=status_current;
        				//add_text_to_current_list((char*)buffer3, 9, ACTIVITY_CHAT);



                    }
                    else if(dcc_conductor->cancelled && !strcmp(dcc_conductor->entry->status,"Removed"))
                    {
                        //do nothing
                        dcc_conductor->connected=0;

                        #ifdef __amigaos4__
                        if(dcc_conductor->dcc_file && USE_F) FClose(dcc_conductor->dcc_file);
                        else if(dcc_conductor->dcc_file) Close(dcc_conductor->dcc_file);
                        #else
                        if(dcc_conductor->dcc_file) Close(dcc_conductor->dcc_file);
                        #endif


                    }
                    else if(dcc_conductor->cancelled)
                    {

                        if(dcc_conductor->entry)
                        {
                            strcpy(dcc_conductor->entry->status,"Cancelled");
                            sprintf(dcc_conductor->entry->bytes_transferred,"%dKB",dcc_conductor->total_recv/1024);
                            float pct = (float) dcc_conductor->total_recv / (float) dcc_conductor->filesize;
                            sprintf(dcc_conductor->entry->percentage,"%.0f%%",(float)(pct*100));
                        }

                        #ifdef __amigaos4__
                        if(dcc_conductor->dcc_file && USE_F) FClose(dcc_conductor->dcc_file);
                        else if(dcc_conductor->dcc_file) Close(dcc_conductor->dcc_file);
                        #else
                        if(dcc_conductor->dcc_file) Close(dcc_conductor->dcc_file);
                        #endif




                        //DoMethod((Object*)James->LV_dcc,MUIM_NList_ReplaceSingle,dcc_conductor->entry,dcc_conductor->pos);
                        DoMethod((Object*)James->LV_dcc,MUIM_NList_ReplaceSingle,dcc_conductor->entry,a);

                        sprintf(buffer3,"%s* DCC SEND: %s from %s *cancelled*",timestamp,dcc_conductor->filename,dcc_conductor->nick);

                        dcc_conductor->connected=0;

                                status_conductor=status_root;
                                while(status_conductor)
                                {
                                    if(!stricmp(status_conductor->networkname,dcc_conductor->networkname)) break;
                                    status_conductor=status_conductor->next;
                                    if(!status_conductor)
                                    {
                                        status_conductor=status_root;
                                        break;
                                    }
                                }

        				add_text_to_conductor_list((char*)buffer3, 9, ACTIVITY_CHAT);

                        //status_conductor=status_current;
        				//add_text_to_current_list((char*)buffer3, 9, ACTIVITY_CHAT);


                    }
                    else
                    {

                        if(dcc_conductor->entry)
                        {
                            strcpy(dcc_conductor->entry->status,"Failed");
                            sprintf(dcc_conductor->entry->bytes_transferred,"%dKB",dcc_conductor->total_recv/1024);
                            float pct = (float) dcc_conductor->total_recv / (float) dcc_conductor->filesize;
                            sprintf(dcc_conductor->entry->percentage,"%.0f%%",(float)(pct*100));
                        }


                        //DoMethod((Object*)James->LV_dcc,MUIM_NList_ReplaceSingle,dcc_conductor->entry,dcc_conductor->pos);
                        DoMethod((Object*)James->LV_dcc,MUIM_NList_ReplaceSingle,dcc_conductor->entry,a);

                        sprintf(buffer3,"%s* DCC SEND: %s from %s has failed",timestamp,dcc_conductor->filename,dcc_conductor->nick);

                        dcc_conductor->connected=0;
                    }




                }



                }



                status_conductor=status_root;


                while(ev_sock != status_conductor->a_socket)
                {
                    status_conductor=status_conductor->next;

                    if(!status_conductor)
                    {
                        status_conductor=status_root;
                        break;
                    }

                }


                if(status_conductor && ev_sock == status_conductor->a_socket)
                {


                if(!status_conductor) break;


    			if(ev_type & FD_READ)
                {

                    //printf("b\n");

                    recv_thing=0;

                    while( recv_thing != -1)
                    {
                        recv_thing=recv(status_conductor->a_socket, (i_in)status_conductor->buffer, BUFFERSIZE-1, 0);
                        if(recv_thing != -1) status_conductor->buffer[recv_thing]='\0';

                        if(recv_thing == -1)
                        {
                            status_conductor->buffer[0]='\0';
                		    break;
                        }

            			strcat(status_conductor->str,status_conductor->buffer);

                        if(recv_thing == 0) break;

                        status_conductor->buffer[recv_thing]='\0';
                        //status_conductor->buffer[0]='\0';



                        if(strcmp(status_conductor->buffer,"") != 0)
            			process_incoming();
                        else break;



                    }


                }

                if(ev_type & FD_CLOSE)
                {
                    status_conductor->connection_active=0;
                    status_conductor->pass[0]='\0';
                    status_conductor->servername[0]='\0';
                    status_conductor->networkname[0]='\0';

                    sprintf(buffer3, "* Disconnected from server");
                    status_conductor->conductor=status_conductor->root;
                    while(status_conductor->conductor)
                    {
                        if(status_conductor->conductor->LV_channel) add_text_to_conductor_list(buffer3,9, ACTIVITY_CHAT);
                        status_conductor->conductor=status_conductor->conductor->next;
                    }
                }
    			if(ev_type & FD_CONNECT)
                {
                    status_conductor->connection_active=1;

                    sprintf(buffer3, "Connected.");

                    add_text_to_conductor_list(buffer3,9, ACTIVITY_CHAT);


                    if(status_conductor->pass[0] == '\0')
                    {
                        sprintf(sendstuff, "NICK %s\r\nUSER %s %s %s :%s\r\n", nick, username, uhostname, uservername, realname);
                        //printf("no server password to send\n");
                    }
                    else
                    {
                        //printf("pass2:%s\n",status_conductor->pass);

                        sprintf(sendstuff, "PASS %s\r\nNICK %s\r\nUSER %s %s %s :%s\r\n", status_conductor->pass,nick, username, uhostname, uservername, realname);
                        //printf("sending server password\n");
                    }
                    //sprintf(sendstuff, "NICK %s\r\nUSER %s %s %s :%s\r\n", nick, username, uhostname, uservername, realname);
                    send_text(sendstuff);



                }
                }




    		}

        }*/

