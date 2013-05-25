/*
    The contents of this file are subject to the AROS Public License Version 1.1 
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis, 
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
    for the specific language governing rights and limitations under the License.
*/

/* ===================================================================
                           Start-Exit Code
   =================================================================== */

void cleanexit(char*);

//#include "MemGuardian.h"
//#include "MemGuardian.c"

void LoadAllLibs(void)
{
    //if(DEBUG) MG_Init();

    background3=new char[64];
    channel_display=new char[100];
    NewPreParse_NewText=new char[100];
    nick2=new char[30];
    nick3=new char[30];
    nick=new char[30];
    nickcolour=new char[40];
    tabwork2_string=new char[900]; //text before cursor position
    tabwork3_string=new char[900]; //text after cursor position
    tabwork4_string=new char[100]; //nick text to complete
    tabwork5_string=new char[100]; //completed nick text to insert
    tabwork_string=new char[900]; //original string gadget contents

    new_entry=new list_entry;
    new_entry->hostname=new char[HOSTNAME_STRING_SIZE+1];
    work_entry4.hostname=new char[HOSTNAME_STRING_SIZE+1];

    fdmax=-1;

    dont_open_colours=FALSE;
    local_charset=0;

    QUIET_DCC=FALSE;
    USE_AREXX=FALSE;
    muted_sound=FALSE;

    o2=NULL;
    o3=NULL;

    for(a=0; a<25; a++) custom_pen_colours[a]=0;
    for(a=0; a<25; a++) pendisplay_specs[a]=new MUI_PenSpec;

    //default values before tooltypes and shell switches are read
    SMALLTABS=0;
    using_a_proxy=FALSE;
    RAW=0;
    my_settings.os3_about_window_gfx=0;



#ifdef __amigaos4__
    DataTypesBase = (Library*)OpenLibrary((c_in)"datatypes.library",0);
    if(!DataTypesBase) cleanexit((char*)"cant open datatypes.library\n");

    AslBase = (Library*)OpenLibrary((c_in)"asl.library",0);
    if(!AslBase) cleanexit((char*)"cant open asl.library\n");

    IconBase = (Library*)OpenLibrary((c_in)"icon.library",0);
    if(!IconBase) cleanexit((char*)"cant open icon.library\n");

    LocaleBase = (Library*)OpenLibrary((c_in)"locale.library",0);
    if(!LocaleBase) cleanexit((char*)"cant open locale.library\n");

    GfxBase = (Library*)OpenLibrary((c_in)"graphics.library",0);
    if(!GfxBase) cleanexit((char*)"cant open graphics.library\n");

    UtilityBase = (Library*)OpenLibrary((c_in)"utility.library",0);
    if(!UtilityBase) cleanexit((char*)"cant open utility.library\n");

    SocketBase = OpenLibrary((c_in)"bsdsocket.library",0L);
    if(!SocketBase) cleanexit((char*)"cant open bsdsocket.library\n");

	MUIMasterBase = OpenLibrary((c_in)"muimaster.library", 0);
    if(!MUIMasterBase) cleanexit((char*)"cant open muimaster.library\n");

    IntuitionBase = OpenLibrary((c_in)"intuition.library",33);
    if(!IntuitionBase) cleanexit((char*)"cant open intuition.library\n");

    RexxSysBase = OpenLibrary((c_in)"rexxsyslib.library",33);
    if(!RexxSysBase) cleanexit((char*)"cant open rexxsyslib.library\n");

    CodesetsBase = OpenLibrary((c_in)CODESETSNAME, CODESETSVER);
    if(!CodesetsBase) cleanexit((char*)"cant open codesets.library\nDownload and install this:\nhttp://aminet.net/util/libs/codesets-6.4.lha\n\n");

    IMUIMaster = (struct MUIMasterIFace*) GetInterface((struct Library *)MUIMasterBase, "main", 1, NULL);
    ISocket = (struct SocketIFace*) GetInterface((struct Library *)SocketBase, "main", 1, NULL);
    IUtility = (struct UtilityIFace*)GetInterface((struct Library *)UtilityBase, "main", 1, NULL);
    IRexxSys = (struct RexxSysIFace*)GetInterface((struct Library *)RexxSysBase, "main", 1, NULL);
    ICodesets = (struct CodesetsIFace*)GetInterface((struct Library *)CodesetsBase, "main", 1, NULL);
    IIntuition = (struct IntuitionIFace*)GetInterface((struct Library *)IntuitionBase, "main", 1, NULL);
    ILocale = (struct LocaleIFace*)GetInterface((struct Library *)LocaleBase, "main", 1, NULL);
    IDataTypes = (struct DataTypesIFace*)GetInterface((struct Library *)DataTypesBase, "main", 1, NULL);
    IIcon = (struct IconIFace*)GetInterface((struct Library *)IconBase, "main", 1, NULL);
    IAsl = (struct AslIFace*)GetInterface((struct Library *)AslBase, "main", 1, NULL);

#elif __MORPHOS__
    AslBase = (Library*)OpenLibrary((c_in)"asl.library",0);
    if(!AslBase) cleanexit((char*)"cant open asl.library\n");

    DataTypesBase = (Library*)OpenLibrary((c_in)"datatypes.library",0);
    if(!DataTypesBase) cleanexit((char*)"cant open datatypes.library\n");

    GfxBase = (struct GfxBase*)OpenLibrary("graphics.library",0);
    if(!GfxBase) cleanexit((char*)"cant open graphics.library\n");

    IconBase = (Library*)OpenLibrary((c_in)"icon.library",0);
    if(!IconBase) cleanexit((char*)"cant open icon.library\n");

    LocaleBase = (Library*)OpenLibrary((c_in)"locale.library",0);
    if(!LocaleBase) cleanexit((char*)"cant open locale.library\n");

    GfxBase = (struct GfxBase*)OpenLibrary((c_in)"graphics.library",0);
    if(!GfxBase) cleanexit((char*)"cant open graphics.library\n");

    UtilityBase = (Library*)OpenLibrary((c_in)"utility.library",0);
    if(!UtilityBase) cleanexit((char*)"cant open utility.library\n");

    SocketBase = OpenLibrary((c_in)"bsdsocket.library",0L);
    if(!SocketBase) cleanexit((char*)"cant open bsdsocket.library\n");

	MUIMasterBase = OpenLibrary((c_in)"muimaster.library", 0);
    if(!MUIMasterBase) cleanexit((char*)"cant open muimaster.library\n");

    IntuitionBase = (struct IntuitionBase*)OpenLibrary((c_in)"intuition.library",33);
    if(!IntuitionBase) cleanexit((char*)"cant open intuition.library\n");

    CodesetsBase = OpenLibrary((c_in)CODESETSNAME, CODESETSVER);
    if(!CodesetsBase) cleanexit((char*)"cant open codesets.library\nDownload and install this:\nhttp://aminet.net/util/libs/codesets-6.4.lha\n\n");
#elif __AROS__
    AslBase = (Library*)OpenLibrary((c_in)"asl.library",0);
    if(!AslBase) cleanexit((char*)"cant open asl.library\n");

    DataTypesBase = (Library*)OpenLibrary((c_in)"datatypes.library",0);
    if(!DataTypesBase) cleanexit((char*)"cant open datatypes.library\n");

    GfxBase = (struct GfxBase*)OpenLibrary("graphics.library",0);
    if(!GfxBase) cleanexit((char*)"cant open graphics.library\n");

    IconBase = (Library*)OpenLibrary((c_in)"icon.library",0);
    if(!IconBase) cleanexit((char*)"cant open icon.library\n");

    LocaleBase = (struct LocaleBase*)OpenLibrary((c_in)"locale.library",0);
    if(!LocaleBase) cleanexit((char*)"cant open locale.library\n");

    GfxBase = (struct GfxBase*)OpenLibrary((c_in)"graphics.library",0);
    if(!GfxBase) cleanexit((char*)"cant open graphics.library\n");

    UtilityBase = (struct UtilityBase*)OpenLibrary((c_in)"utility.library",0);
    if(!UtilityBase) cleanexit((char*)"cant open utility.library\n");

    SocketBase = OpenLibrary((c_in)"bsdsocket.library",0L);
    if(!SocketBase) cleanexit((char*)"cant open bsdsocket.library\n");

	MUIMasterBase = OpenLibrary((c_in)"muimaster.library", 0);
    if(!MUIMasterBase) cleanexit((char*)"cant open muimaster.library\n");

    IntuitionBase = (struct IntuitionBase*)OpenLibrary((c_in)"intuition.library",33);
    if(!IntuitionBase) cleanexit((char*)"cant open intuition.library\n");

    CodesetsBase = OpenLibrary((c_in)CODESETSNAME, CODESETSVER);
    if(!CodesetsBase) cleanexit((char*)"cant open codesets.library\nDownload and install this:\nhttp://aminet.net/util/libs/codesets-6.4.lha\n\n");

#else

    AslBase = (Library*)OpenLibrary((c_in)"asl.library",0);
    if(!AslBase) cleanexit((char*)"cant open asl.library\n");

    DataTypesBase = (Library*)OpenLibrary((c_in)"datatypes.library",0);
    if(!DataTypesBase) cleanexit((char*)"cant open datatypes.library\n");

    IconBase = (Library*)OpenLibrary((c_in)"icon.library",0);
    if(!IconBase) cleanexit((char*)"cant open icon.library\n");

    LocaleBase = (Library*)OpenLibrary((c_in)"locale.library",0);
    if(!LocaleBase) cleanexit((char*)"cant open locale.library\n");

    GfxBase = (Library*)OpenLibrary((c_in)"graphics.library",0);
    if(!GfxBase) cleanexit((char*)"cant open graphics.library\n");

    UtilityBase = (Library*)OpenLibrary((c_in)"utility.library",0);
    if(!UtilityBase) cleanexit((char*)"cant open utility.library\n");

    SocketBase = OpenLibrary((c_in)"bsdsocket.library",0L);
    if(!SocketBase) cleanexit((char*)"cant open bsdsocket.library\n");

	MUIMasterBase = OpenLibrary((c_in)"muimaster.library", 0);
    if(!MUIMasterBase) cleanexit((char*)"cant open muimaster.library\n");

    IntuitionBase = OpenLibrary((c_in)"intuition.library",33);
    if(!IntuitionBase) cleanexit((char*)"cant open intuition.library\n");

    CodesetsBase = OpenLibrary((c_in)CODESETSNAME, CODESETSVER);
    if(!CodesetsBase) cleanexit((char*)"cant open codesets.library\nDownload and install this:\nhttp://aminet.net/util/libs/codesets-6.4.lha\n\n");

#endif


    catalog = OpenCatalog(NULL,
                "WookieChat.catalog",
                OC_BuiltInLanguage, NULL,
                TAG_DONE);

    if(!catalog)
    {

        if(DEBUG) printf("unable to use default language\n");

        catalog = OpenCatalog(NULL,
                "WookieChat.catalog",
                OC_Language, "english",
                TAG_DONE);


        if(!catalog)
        {
            if(DEBUG) printf("unable to load english catalog, using built in strings\n");

        }


    }

    //now that the catalog is open and locale library is open, lets give our context menu titles/items some labels
    
    MenuData1[0].nm_Label =(char*)GetCatalogStr(catalog,219,"Nicklist Options");
    MenuData1[1].nm_Label =(char*)GetCatalogStr(catalog,220,"Whois");
    MenuData1[2].nm_Label =(char*)GetCatalogStr(catalog,221,"Open Query");
    MenuData1[3].nm_Label =(char*)GetCatalogStr(catalog,222,"Open Global Query");
    MenuData1[4].nm_Label =(char*)GetCatalogStr(catalog,223,"CTCP");
    MenuData1[5].nm_Label =(char*)GetCatalogStr(catalog,224,"Ping");
    MenuData1[6].nm_Label =(char*)GetCatalogStr(catalog,225,"Version");
    MenuData1[7].nm_Label =(char*)GetCatalogStr(catalog,226,"Time");
    MenuData1[8].nm_Label =(char*)GetCatalogStr(catalog,227,"Direct Connection");
    MenuData1[9].nm_Label =(char*)GetCatalogStr(catalog,228,"Send file");
    MenuData1[10].nm_Label=(char*)GetCatalogStr(catalog,229,"Chat");
    MenuData1[11].nm_Label=(char*)GetCatalogStr(catalog,230,"Control");
    MenuData1[12].nm_Label=(char*)GetCatalogStr(catalog,231,"Op");
    MenuData1[13].nm_Label=(char*)GetCatalogStr(catalog,232,"DeOp");
    MenuData1[14].nm_Label=(char*)GetCatalogStr(catalog,233,"HalfOp");
    MenuData1[15].nm_Label=(char*)GetCatalogStr(catalog,234,"DeHalfOp");
    MenuData1[16].nm_Label=(char*)GetCatalogStr(catalog,235,"Voice");
    MenuData1[17].nm_Label=(char*)GetCatalogStr(catalog,236,"DeVoice");
    MenuData1[18].nm_Label=(char*)GetCatalogStr(catalog,237,"Kick");
    MenuData1[19].nm_Label=(char*)GetCatalogStr(catalog,238,"Ban");


    dobj = (struct DiskObject*)GetDiskObject((c_in)"PROGDIR:WookieChat_OS4");
    if(!dobj) dobj = (struct DiskObject*)GetDiskObject((c_in)"PROGDIR:WookieChat_OS3");
    if(!dobj) dobj = (struct DiskObject*)GetDiskObject((c_in)"PROGDIR:WookieChat");

    #ifdef __AROS__
    BPTR my_lock;

    if((my_lock=Lock("env:zune",ACCESS_READ)))
    {
        UnLock(my_lock);
        ZUNE_SYSTEM=TRUE;
    }
    else
        ZUNE_SYSTEM=FALSE;
    #endif

            total_smileys=0;


    //lets create some more structures
    new_entry=new list_entry;
    work_entry=new list_entry;
    work_entry3=new list_entry;
    new_entry->hostname=NULL;
    work_entry3->hostname=NULL;

    //zero the socket filedescriptor sets
    FD_ZERO(&read_master);
    FD_ZERO(&read_fds);
    FD_ZERO(&write_fds);
    FD_ZERO(&write_master);

    strcpy(sendstuff, "");

    is_chooser_window_open=FALSE;

    clockdata = new struct ClockData;
    clockdata2 = new struct ClockData;

    if((slist=(struct SharedList*)AllocMem(sizeof(struct SharedList),MEMF_PUBLIC|MEMF_CLEAR)))
    {
        NewList(&slist->sl_List); // initialise the minlist
        InitSemaphore((struct SignalSemaphore*)slist); //initialise the semaphore, it can now be used

    }
    else printf("cant allocate memory for the semaphore structure\n");



}


void cleanexit(char *str)
{
    int GEIT=0;

    if(GEIT) printf("closing\n1\n");

    TimerWait_Close();


    //if(local_charset) CodesetsFreeA(local_charset,blank_taglist);// my_utf8_taglist);
//printf("2\n");
    //printf("1\n");
    if(GEIT) printf("2\n");

    if(USE_AREXX==TRUE)
    {
        //printf("quit 1\n");
    if(GEIT) printf("2a\n");

        if(AREXX_started==TRUE)
        {
    if(GEIT) printf("2b\n");

            //edit
            //printf("quit 1aa\n");

            //cleanup GETLINE
            if(getline_wait==TRUE)
            {
                //printf("quit 1a .. getline equal true\n");

                getline_wait=FALSE;
                wanna_quit=TRUE;

                my_message->xy_Getline = 1;
                my_message->xy_QuitArexx = 1;
                my_message->xy_Msg.mn_Node.ln_Type=NT_MESSAGE;
                my_message->xy_Msg.mn_Length=sizeof(struct XYMessage);
                my_message->xy_Msg.mn_ReplyPort = app_process_replyport;

                //if(SafePutToPort((struct XYMessage*)my_message, "WookieChatAREXX"))
                if(SafePutToPort((struct XYMessage*)my_message, basename))
                {
                    //printf("quit 1ab\n");
                    disable_getline_hook();

                    WaitPort(app_process_replyport);
                    //printf("quit 1ac\n");
                }

            }

            my_message->xy_Getline = 0;
            my_message->xy_QuitArexx = 1;
            my_message->xy_Msg.mn_Node.ln_Type=NT_MESSAGE;
            my_message->xy_Msg.mn_Length=sizeof(struct XYMessage);
            my_message->xy_Msg.mn_ReplyPort = arexx_quit_replyport;

            //if(SafePutToPort((struct XYMessage*)my_message, "WookieChatAREXX_Quit"))
            if(SafePutToPort((struct XYMessage*)my_message, arexxquit_portname))
            {
                //printf("quit 1ad\n");
                WaitPort(arexx_quit_replyport);
                //printf("quit 1ae\n");

            }

            //printf("quit 1af\n");


        }

        //printf("quit 2\n");

    if(GEIT) printf("3\n");

        DeletePort(app_process_replyport);
    if(GEIT) printf("4\n");

        while(GetMsg(send_text_replyport));

    if(GEIT) printf("5\n");

        DeletePort(send_text_replyport);

    if(GEIT) printf("6\n");

        DeletePort(arexx_quit_replyport);


        //printf("quit 3\n");

    }
    //printf("2\n");

    //FreeMem(my_message,sizeof(struct XYMessage));
    //FreeMem(incoming_message,sizeof(struct XYMessage));

    //printf("quit 7\n");

    if(GEIT) printf("7\n");

    if(WookieChat)
    {

        //Now Save all URL's to a text file
        sprintf(file_name,"progdir:urls.txt");
        urlgrabber_file = Open(file_name, MODE_NEWFILE);
        if(urlgrabber_file)
        {
            getmacro((Object*)WookieChat->LV_urlgrabber,MUIA_NList_Entries, &entries);
    //printf("3\n");

            for(unsigned int aaa=0; aaa<entries; aaa++)
            {
                DoMethod((Object*)WookieChat->LV_urlgrabber,MUIM_NList_GetEntry,aaa,&string1);

                if(string1)
                {
                    FPuts(urlgrabber_file,(b_in)string1);
                    FPutC(urlgrabber_file,'\n');

                    if(!strcmp(string1,urlgrabber_str)) break;
                }
            }
            Close(urlgrabber_file);

        }

    //printf("4\n");

    //printf("5\n");

    if(GEIT) printf("8\n");

//printf("3\n");

        status_conductor=status_root;
        while(status_conductor)
        {
            //if(status_conductor->remote_charset) CodesetsFreeA(status_conductor->remote_charset,blank_taglist);// my_utf8_taglist);
    //printf("6\n");
            //if(status_conductor->buffer) delete [] status_conductor->buffer;
    //printf("7\n");
            //if(status_conductor->str) delete [] status_conductor->str;
    //printf("8\n");
            status_conductor=status_conductor->next;
    //printf("9\n");
        }

//printf("4\n");
    if(GEIT) printf("9\n");

        //delete [] NewPreParse_NewText;
        //delete [] channel_display;

    }

    //printf("10\n");

    if(WookieChat)
    {
    if(GEIT) printf("10\n");

    //free up dcc recv linked list
    for(dcc_conductor=dcc_root->next; dcc_conductor; dcc_conductor=dcc_work)
    {
        if(DEBUG) printf("cleaning up one dcc recv, file:%s\n",dcc_conductor->filename);
        dcc_work=dcc_conductor->next;
        if(dcc_conductor)
        {
            //delete dcc_conductor->entry;
            delete dcc_conductor;

         }
    }

    //printf("11\n");

    //free up dcc send linked list
    for(dcc_send_conductor=dcc_send_root->next; dcc_send_conductor; dcc_send_conductor=dcc_send_work)
    {

        dcc_send_work=dcc_send_conductor->next;
        if(dcc_send_conductor)
        {
            //delete dcc_send_conductor->entry;
            delete dcc_send_conductor;

         }
    }
    //printf("12\n");

    //free up dcc chat linked list
    for(dcc_chat_conductor=dcc_chat_root->next; dcc_chat_conductor; dcc_chat_conductor=dcc_chat_work)
    {

        dcc_chat_work=dcc_chat_conductor->next;
        if(dcc_chat_conductor)
        {
            //delete [] dcc_chat_conductor->str;
            delete dcc_chat_conductor;

         }
    }
    //printf("13\n");
    if(GEIT) printf("11\n");

    if(last_clicked_res) delete last_clicked_res; last_clicked_res=NULL;
    if(systime) delete systime; systime=NULL;
    if(systime_reconnect_timer) delete systime_reconnect_timer; systime_reconnect_timer=NULL;

    }

	if(WookieChat)
    {
        status_conductor=status_root;

        setmacro((Object*)WookieChat->WI_main,MUIA_Window_Open, FALSE);
    if(GEIT) printf("12\n");

       // //printf("2\n");
    //printf("14\n");

    //printf("5a\n");
        delete_smiley_objects();
    if(GEIT) printf("13\n");

        while(status_conductor)
        {
            status_conductor->conductor=status_conductor->root;
       // //printf("3\n");

            while(status_conductor->conductor)
            {
       // printf("4\n");

                for(count=0; count<2500; count++)
                {
                    if(status_conductor->conductor->nicklist[count].hostname)
                        delete [] status_conductor->conductor->nicklist[count].hostname;
                }
       // printf("5\n");

                if(DoMethod((Object*)GROUP_ROOT_0,MUIM_Group_InitChange))
                {
                    if(status_conductor->conductor->GR_conductor && (status_conductor->conductor != status_current->current_query))
                    {

                        if(DoMethod((Object*)GR_top,MUIM_Group_InitChange))
                        {

                            if(DoMethod((Object*)status_conductor->conductor->GR_conductor,MUIM_Group_InitChange))
                            {
                                if(DoMethod((Object*)status_conductor->conductor->GR_listviews,MUIM_Group_InitChange))
                                {

                                    getmacro((Object*)status_conductor->conductor->GR_listviews,MUIA_Group_ChildList,&list);
                                    object_state = list->mlh_Head;

                                	while ((member_object = NextObject((Object**)&object_state )) )
                                	{
                                        if(member_object == status_conductor->conductor->GR_nicklist_and_tabs)
                                            DoMethod((Object*)status_conductor->conductor->GR_listviews, OM_REMMEMBER, (Object*)status_conductor->conductor->GR_nicklist_and_tabs);

                                    }

                            		DoMethod((Object*)status_conductor->conductor->GR_listviews,MUIM_Group_ExitChange);

                                }

                        		DoMethod((Object*)status_conductor->conductor->GR_conductor,MUIM_Group_ExitChange);

                            }

                    		DoMethod((Object*)GR_top,MUIM_Group_ExitChange);

                        }
                        //does this belong here?

                        getmacro((Object*)GR_top,MUIA_Group_ChildList,&list);
                        object_state = list->mlh_Head;

                    	while ((member_object = NextObject((Object**) &object_state )) )
                    	{
                            if(member_object == status_conductor->conductor->GR_conductor)
                                DoMethod((Object*)GR_top, OM_REMMEMBER, (Object*)status_conductor->conductor->GR_conductor);
                        }


                    }

            		DoMethod((Object*)GROUP_ROOT_0,MUIM_Group_ExitChange);

                }



                if(status_conductor->conductor->GR_conductor && (status_conductor->conductor != status_current->current_query))
                {
                    MUI_DisposeObject((Object*)status_conductor->conductor->GR_conductor);
                }

       // printf("6\n");

                work_query=status_conductor->conductor->next;
                if(status_conductor->conductor)
                {
                    if(status_conductor->conductor->log_file)
                    {
                        Close(status_conductor->conductor->log_file);
                        status_conductor->conductor->log_file=0;
                    }

                    //attempting to clean up typed history
                    for(status_conductor->conductor->string_conductor=status_conductor->conductor->string_root;
                        status_conductor->conductor->string_conductor;
                        status_conductor->conductor->string_conductor=work_history)
                    {

                        work_history=status_conductor->conductor->string_conductor->next;

                        if(status_conductor->conductor->string_conductor->buffer_history[0]!='\0')
                        {
                            //delete status_conductor->conductor->string_root;
                            delete status_conductor->conductor->string_conductor;
                        }

                    }


                    if(status_conductor->conductor != status_current->current_query)
                    {
                        FreeVec(status_conductor->conductor);
                        status_conductor->conductor=NULL;
                    }
                }

                status_conductor->conductor=work_query;


            }


            work_status=status_conductor->next;
            if(status_conductor && status_conductor != status_current)
            {
                delete status_conductor;
                status_conductor=NULL;
            }
            status_conductor=work_status;



        }
    if(GEIT) printf("14\n");

    //printf("5b\n");

       //printf("7\n");

        exit_delete_smiley_objects();
    if(GEIT) printf("15\n");
        DisposeApp(WookieChat);
    if(GEIT) printf("16\n");


        if(status_current)
        {
            if(status_current->current_query)
            {
                FreeVec(status_current->current_query);
                status_current->current_query=NULL;

                delete status_current;
                status_current=NULL;


            }
        }
    if(GEIT) printf("17\n");

    //printf("8\n");

    }
    //printf("15\n");

    //printf("6\n");

    //if(ngew_entry) delete new_entry; new_entry=NULL;
    //if(work_entry) delete work_entry; work_entry=NULL;
    //if(work_entry3) delete work_entry3; work_entry3=NULL;

    //if(current_query) delete current_query;

        if(o) DisposeDTObject(o);
        if(o2) DisposeDTObject(o2);
        if(o3) DisposeDTObject(o3);

    if(GEIT) printf("18\n");

    //printf("9\n");

    if(mcc) MUI_DeleteCustomClass(mcc);
    if(mcc2) MUI_DeleteCustomClass(mcc2);
    //if(mcc3) MUI_DeleteCustomClass(mcc3);
    if(mcc4) MUI_DeleteCustomClass(mcc4);
    if(mcc5) MUI_DeleteCustomClass(mcc5);
    if(mcc6) MUI_DeleteCustomClass(mcc6);
    //printf("16\n");

    if(GEIT) printf("19\n");
    //printf("8\n");

    //if(tabwork2_string) delete []tabwork2_string; tabwork2_string=NULL;//text before cursor position
    //if(tabwork3_string) delete []tabwork3_string; tabwork3_string=NULL;//text after cursor position
    //if(tabwork4_string) delete []tabwork4_string; tabwork4_string=NULL;//nick text to complete
    //if(tabwork5_string) delete []tabwork5_string; tabwork5_string=NULL;//completed nick text to insert
    //printf("17\n");

    //printf("9\n");

    if(GEIT) printf("20\n");

    //if(res) delete res;
    //printf("10\n");

    if (str) printf("Error: %s\n",str);

    #ifdef __amigaos4__
    DropInterface((struct Interface*)IUtility);
    DropInterface((struct Interface*)IMUIMaster);
    DropInterface((struct Interface*)ISocket);
    DropInterface((struct Interface*)IRexxSys);
    DropInterface((struct Interface*)ICodesets);
    #endif

    if(dobj) FreeDiskObject(dobj);
    if(GEIT) printf("21\n");

    //printf("18\n");

    CloseCatalog(catalog);
    if(GEIT) printf("22\n");

    //printf("11\n");
    
    #ifdef __AROS__
    if(UtilityBase) CloseLibrary((Library*)UtilityBase);
    if(SocketBase) CloseLibrary((Library*)SocketBase);
    if(IntuitionBase) CloseLibrary((Library*)IntuitionBase);
    if(LocaleBase) CloseLibrary((Library*)LocaleBase);
    #else
    if(CodesetsBase) CloseLibrary(CodesetsBase);
    if(UtilityBase) CloseLibrary(UtilityBase);
    if(SocketBase) CloseLibrary(SocketBase);
    if(IntuitionBase) CloseLibrary((Library*)IntuitionBase);
    if(LocaleBase) CloseLibrary(LocaleBase);
    #endif
    
    #ifdef __MORPHOS__
    if(AslBase) CloseLibrary(AslBase);
    if(DataTypesBase) CloseLibrary(DataTypesBase);
    if(GfxBase) CloseLibrary((Library*)GfxBase);
    #endif
    
    if(MUIMasterBase) CloseLibrary(MUIMasterBase);

    if(GEIT) printf("23\n");

    //printf("7\n");

    delete clockdata;
    delete clockdata2;

    delete [] new_entry->hostname;
    delete [] work_entry4.hostname;
    delete [] new_entry2.hostname;

    delete new_entry;


    //SignalCheck_Done();
    //if(DEBUG) MG_Exit();

    exit(0);
}
