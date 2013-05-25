/*
    The contents of this file are subject to the AROS Public License Version 1.1 
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis, 
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
    for the specific language governing rights and limitations under the License.
*/

/* ===================================================================
                            IRC Functions
   =================================================================== */


void SWhoIs(char *selnick)
{
	sprintf(sendstuff, "WHOIS %s\r\n", selnick);
    //send_current(sendstuff);
    send_text(sendstuff);

}	
	

void SQuery(char *selnick)
{
	if(selnick) create_new_query_window(selnick, 1, 2);
}


void ChangeMyNick(char *mynewnick)
{

	sprintf(sendstuff, "NICK %s\r\n", mynewnick);
    //send_current(sendstuff);
    send_text(sendstuff);

}


void SJoin(char *selchannel)
{
	sprintf(sendstuff, "JOIN %s\r\n", selchannel);
    send_text(sendstuff);
}


void SMsg(char *selnick, char *message)
{
	sprintf(sendstuff, "PRIVMSG %s :%s\r\n", selnick, message);
		
    send_text(sendstuff);
    //send_current(sendstuff);
	
	sprintf(buffer3,"%s-> *%s* %s",timestamp,selnick, message);

	add_text_to_conductor_list((char*)buffer3, 10, ACTIVITY_CHAT);
	//add_text_to_current_list((char*)buffer3, 10, ACTIVITY_CHAT);

		
}


void SNames(char *selchannel)
{
	sprintf(sendstuff, "names %s\r\n", selchannel);
		
    status_conductor->conductor=status_conductor->root;
    while( stricmp((p_in)status_conductor->conductor->name, (p_in)selchannel) )
    {
        status_conductor->conductor=status_conductor->conductor->next;
        if(!status_conductor->conductor) break;
    }

    if(status_conductor->conductor)
    {
        DoMethod((Object*)status_conductor->conductor->LV_nicklist,MUIM_NList_Clear);
    	status_conductor->conductor->nicks=0;

		
    	sprintf(buffer3,"%s** %s %s",timestamp, GetCatalogStr(catalog,207,"Updating Names list for channel"),selchannel);
    	add_text_to_conductor_list((char*)buffer3, 9, ACTIVITY_CHAT);
   	}

    send_text(sendstuff);

		
}


void SNotice(char *selnick, char *message)
{
	sprintf(sendstuff, "NOTICE %s :%s\r\n", selnick, message);
		
    send_text(sendstuff);
    //send_current(sendstuff);
	
	sprintf(buffer3,"%s-> -%s- %s",timestamp, selnick, message);

	add_text_to_conductor_list((char*)buffer3, 12, ACTIVITY_CHAT);
	//add_text_to_current_list((char*)buffer3, 12, ACTIVITY_CHAT);
   		
}


void ChangeTopic(char *newtopic)
{
	if(newtopic) sprintf(sendstuff, "TOPIC %s :%s\r\n", status_conductor->current_query->name, newtopic);
	else sprintf(sendstuff,"TOPIC %s\r\n",status_conductor->current_query->name);
		
    send_text(sendstuff);

}


void SPart(char *message)
{


    if(message)
    {

        if(!strcmp(message,""))
            sprintf(sendstuff,"PART %s :%s\r\n",status_current->current_query->name,my_settings.part_msg);
        else
            sprintf(sendstuff,"PART %s :%s\r\n",status_current->current_query->name,message);

    }
    else
        sprintf(sendstuff,"PART %s :%s\r\n",status_current->current_query->name,my_settings.part_msg);


	/*if(!stricmp(message,"") && my_settings.part_msg )
    {
        strcpy(message, my_settings.part_msg);
    }

    if(!stricmp(message,""))
        sprintf(sendstuff,"PART %s\r\n",status_current->current_query->name);
    else
        sprintf(sendstuff,"PART %s :%s\r\n",status_current->current_query->name,message);
     */

    send_text(sendstuff);
    CloseQueryWindow();


}


void SQuit(char *message)
{
	if(!stricmp(message,"")) strcpy(message, my_settings.quit_msg);
    else strcpy(message,"Client exiting");
    sprintf(sendstuff, "QUIT :%s\r\n", message);

    status_conductor->quit_requested=TRUE;
		
    send_text(sendstuff);
}


void SAction(char *message)
{		
	if(message)
    {
        sprintf(sendstuff,"PRIVMSG %s :\001ACTION %s\001\r\n",status_conductor->current_query->name, message);

        //send_current(sendstuff);
        send_text(sendstuff);
	
    	sprintf(buffer3,"%s* %s %s",timestamp, status_current->nick,message);
    	//add_text_to_current_list((char*)buffer3, 10, ACTIVITY_CHAT);
    	add_text_to_conductor_list((char*)buffer3, 10, ACTIVITY_CHAT);
    }
}



//old, reliable, working close query window.. but doesnt reorder linked list when tabs close in the middle
//or free up memory
/*void CloseQueryWindow(void)
{


    status_conductor=status_current;

    if(status_conductor->current_query==status_root->root) return; // Make it impossible to close the root tab

    if(status_conductor->current_query->name[0]=='$')
    {
        dcc_chat_conductor=dcc_chat_root->next;
        while(dcc_chat_conductor)
        {
            if(dcc_chat_conductor->conductor==status_conductor->current_query)
            {
                dcc_chat_conductor->removed=1; //yes its now removed
                shutdown((int)dcc_chat_conductor->dcc_socket,(int)2);
                break;
            }
            dcc_chat_conductor=dcc_chat_conductor->next;

        }

    }

	strcpy(status_conductor->current_query->name,"");

    query_window *status_work=status_conductor->current_query->previous;


    if(DoMethod((Object*)GR_bottom_group,MUIM_Group_InitChange))
	{
        if(DoMethod((Object*)status_conductor->GR_server1_buttons,MUIM_Group_InitChange))
    	{
            DoMethod((Object*)status_conductor->GR_server1_buttons, OM_REMMEMBER, (Object*)status_conductor->current_query->BT_querybutton);
            MUI_DisposeObject((Object*)status_conductor->current_query->BT_querybutton);
            status_conductor->current_query->BT_querybutton=NULL;

            //if(status_work->BT_querybutton)
            if(status_work)
            {
                if(status_work->BT_querybutton)
                {
                    setmacro(status_work->BT_querybutton, MUIA_Selected, TRUE);
                    setmacro(status_work->BT_querybutton,MUIA_Text_PreParse, "\033c\0333");
                }
                else
                {

                    setmacro(status_conductor->root->BT_querybutton, MUIA_Selected, TRUE);
                    setmacro(status_conductor->root->BT_querybutton,MUIA_Text_PreParse, "\033c\0333");

                }
    		}
            else
            {
               setmacro(status_conductor->root->BT_querybutton, MUIA_Selected, TRUE);
               setmacro(status_conductor->root->BT_querybutton,MUIA_Text_PreParse, "\033c\0333");

            }
            DoMethod((Object*)status_conductor->GR_server1_buttons,MUIM_Group_ExitChange);
        }

        if(status_conductor->current_query==status_conductor->root)
        {

            //remove all buttons
            //
            for(status_conductor->conductor=status_conductor->root; status_conductor->conductor; status_conductor->conductor=status_conductor->conductor->next)
            {
                DoMethod((Object*)status_conductor->GR_server1_buttons, OM_REMMEMBER, (Object*)status_conductor->conductor->BT_querybutton);
                MUI_DisposeObject((Object*)status_conductor->conductor->BT_querybutton);
                status_conductor->conductor->BT_querybutton=NULL;
            }
            //
            //

            DoMethod((Object*)GR_bottom_group, OM_REMMEMBER, (Object*)status_conductor->GR_server1_buttons);
            MUI_DisposeObject((Object*)status_conductor->GR_server1_buttons);
            status_conductor->GR_server1_buttons=NULL;

            if(my_settings.quit_msg) sprintf(buffer3,"QUIT :%s\r\n",my_settings.quit_msg);
                else sprintf(buffer3,"QUIT :Client Exiting\r\n");

            shutdown((int)status_conductor->a_socket,(int)2);
            FD_CLR(status_conductor->a_socket, &read_master);
        }

    	DoMethod((Object*)GR_bottom_group,MUIM_Group_ExitChange);

    }


    // Add the new group and buttons now that the closed ones have been removed

        if(DoMethod((Object*)GR_top,MUIM_Group_InitChange))
        {
            DoMethod((Object*)GR_top, OM_REMMEMBER, (Object*)status_conductor->current_query->GR_conductor);

            //MUI_DisposeObject((Object*)status_conductor->current_query->GR_conductor);
            //status_conductor->current_query->GR_conductor=NULL;
            //status_conductor->current_query->LV_channel=NULL;

            if(status_conductor->current_query->log_file)
            {
                Close(status_conductor->current_query->log_file);
                status_conductor->current_query->log_file=0;
            }


            //if(status_work->BT_querybutton)
            if(status_work)
            {
                if(status_work->BT_querybutton)
                {
                    DoMethod((Object*)GR_top, OM_ADDMEMBER, (Object*)status_work->GR_conductor);
                    status_conductor->current_query=status_work;

                }
                else
                {
                    DoMethod((Object*)GR_top, OM_ADDMEMBER, (Object*)status_conductor->root->GR_conductor);
                    status_conductor->current_query=status_conductor->root;


                }

    		}
            else
            {
                DoMethod((Object*)GR_top, OM_ADDMEMBER, (Object*)status_conductor->root->GR_conductor);
                status_conductor->current_query=status_conductor->root;

            }
            DoMethod((Object*)GR_top,MUIM_Group_ExitChange);
        }

    status_conductor->previous_query=status_conductor->root;

    calculate_how_many_rows_needed();


}*/



query_window *status_work;
query_window *status_previous, *status_next;

void CloseQueryWindow(void)
{

    int count_root_tabs=0;

    status_conductor=status_current;

    //count the servers
    for(status_conductor=status_root; status_conductor; status_conductor=status_conductor->next)
    {
        for(status_conductor->conductor=status_conductor->root; status_conductor->conductor;
            status_conductor->conductor=status_conductor->conductor->next)
        {
            if(status_conductor->conductor->BT_querybutton) { count_root_tabs++; break; }

        }

    }

    status_conductor=status_current;
    status_conductor->conductor=status_conductor->current_query;

    count=0;

    //if we've only got one server group, and we've closed the only channel here, we
    //want to leave that tab open for when we join another channel
    if(status_conductor->current_query->name[0]=='#')
    {
        for(status_conductor->conductor=status_conductor->root; status_conductor->conductor; status_conductor->conductor=status_conductor->conductor->next)
        {
            if(status_conductor->conductor->BT_querybutton) count++;
        }

        status_conductor->conductor=status_conductor->current_query;

        if(count==1)
        {
            strcpy(status_conductor->conductor->name," ");
            strcpy(status_conductor->conductor->displayed_name,"(none)");
            setmacro((Object*)status_conductor->conductor->BT_querybutton,MUIA_Text_Contents,status_conductor->conductor->displayed_name);

            sprintf(buffer3,"%s%s%s%s %s",timestamp,GetCatalogStr(catalog,217,"«"),GetCatalogStr(catalog,165,"Part"),GetCatalogStr(catalog,218,"»"),status_conductor->nick);
        	add_text_to_current_list((char*)buffer3, 3, ACTIVITY_CHAT);

        }
    }


    //Are we closing a DCC CHAT tab?
    if(status_conductor->current_query->name[0]=='$')
    {
        dcc_chat_conductor=dcc_chat_root->next;
        while(dcc_chat_conductor)
        {
            if(dcc_chat_conductor->conductor==status_conductor->current_query)
            {
                dcc_chat_conductor->removed=1; //yes its now removed
                shutdown((int)dcc_chat_conductor->dcc_socket,(int)2);
                dcc_chat_conductor->conductor=NULL;
                break;
            }
            dcc_chat_conductor=dcc_chat_conductor->next;

        }

    }

	strcpy(status_conductor->current_query->name," ");

    status_work=status_conductor->current_query->previous;

    for(count=0,status_conductor->conductor=status_conductor->root; status_conductor->conductor; status_conductor->conductor=status_conductor->conductor->next)
    {
        if(status_conductor->conductor->BT_querybutton) count++;

    }

    if(count==1 && count_root_tabs==1) return;


    //if(ZUNE_SYSTEM==TRUE) setmacro((Object*)James->WI_main,MUIA_Window_Open, FALSE);

    if(DoMethod((Object*)GR_bottom_group,MUIM_Group_InitChange))
	{
        //if(DoMethod((Object*)status_conductor->GR_server1_buttons,MUIM_Group_InitChange))
    	{

            DoMethod((Object*)James->LV_tabs,MUIM_NList_Remove,status_conductor->current_query->nlist_tab_number);
            if(DEBUG) printf("closing %ld\n",status_conductor->current_query->nlist_tab_number);
            DoMethod((Object*)status_conductor->GR_server1_buttons, OM_REMMEMBER, (Object*)status_conductor->current_query->BT_querybutton);
            MUI_DisposeObject((Object*)status_conductor->current_query->BT_querybutton);
            status_conductor->current_query->BT_querybutton=NULL;
            status_conductor->current_query->removed=1;


            strcpy(status_conductor->current_query->name," ");
            strcpy(status_conductor->current_query->displayed_name,"(none)");

            //DoMethod((Object*)status_conductor->GR_server1_buttons,MUIM_Group_ExitChange);
        }

        if(count==1)
        {
            if(DEBUG) printf("remove all buttons\n");

            //remove all buttons
            for(status_conductor->conductor=status_conductor->root; status_conductor->conductor; status_conductor->conductor=status_conductor->conductor->next)
            {
                if(status_conductor->conductor->removed==0) DoMethod((Object*)James->LV_tabs,MUIM_NList_Remove,status_conductor->conductor->nlist_tab_number);

                DoMethod((Object*)status_conductor->GR_server1_buttons, OM_REMMEMBER, (Object*)status_conductor->conductor->BT_querybutton);
                MUI_DisposeObject((Object*)status_conductor->conductor->BT_querybutton);
                status_conductor->conductor->BT_querybutton=NULL;
                status_conductor->conductor->removed=1;

            }

            DoMethod((Object*)GR_bottom_group, OM_REMMEMBER, (Object*)status_conductor->GR_server1_buttons);
            MUI_DisposeObject((Object*)status_conductor->GR_server1_buttons);
            status_conductor->GR_server1_buttons=NULL;

            if(my_settings.quit_msg) sprintf(buffer3,"QUIT :%s\r\n",my_settings.quit_msg);
                else sprintf(buffer3,"QUIT :Client Exiting\r\n");

            shutdown((int)status_conductor->a_socket,(int)2);
            FD_CLR(status_conductor->a_socket, &read_master);
        }

    	DoMethod((Object*)GR_bottom_group,MUIM_Group_ExitChange);

    }


    // Add the new group and buttons now that the closed ones have been removed

    if(DoMethod((Object*)GR_top,MUIM_Group_InitChange))
    {

        //if(DoMethod((Object*)status_conductor->current_query->GR_listviews,MUIM_Group_InitChange))
        {

            //removing tab and freeing up nlist objects
            DoMethod((Object*)GR_top, OM_REMMEMBER, (Object*)status_conductor->current_query->LV_channel);
            MUI_DisposeObject((Object*)status_conductor->current_query->LV_channel);
            status_conductor->current_query->LV_channel=NULL;

            //if(DoMethod((Object*)status_conductor->current_query->GR_nicklist_and_tabs,MUIM_Group_InitChange))
            {
                DoMethod((Object*)status_conductor->current_query->GR_nicklist_and_tabs, OM_REMMEMBER, (Object*)status_conductor->current_query->LV_nicklist);
                MUI_DisposeObject((Object*)status_conductor->current_query->LV_nicklist);
                status_conductor->current_query->LV_nicklist=NULL;

                if(my_settings.use_nlistobject_for_tabs)
                    DoMethod((Object*)status_conductor->current_query->GR_nicklist_and_tabs, OM_REMMEMBER, (Object*)status_conductor->current_query->GR_tabs);

                //DoMethod((Object*)status_conductor->current_query->GR_nicklist_and_tabs,MUIM_Group_ExitChange);

            }

            //DoMethod((Object*)status_conductor->current_query->GR_listviews,MUIM_Group_ExitChange);

        }


        DoMethod((Object*)GR_top, OM_REMMEMBER, (Object*)status_conductor->current_query->GR_conductor);
        MUI_DisposeObject((Object*)status_conductor->current_query->GR_conductor);
        status_conductor->current_query->GR_conductor=NULL;

        //is the logfile open?
        if(status_conductor->current_query->log_file)
        {
            Close(status_conductor->current_query->log_file);
            status_conductor->current_query->log_file=0;
        }

        //free up nicklist struct memory!
        for(count=0; count<2500; count++)
        {
            if(status_conductor->current_query->nicklist[count].hostname)
            {
                delete [] status_conductor->current_query->nicklist[count].hostname;
                status_conductor->current_query->nicklist[count].hostname=NULL;
            }
        }

        //count how many tabs there are open
        for(count2=0, status_conductor->conductor=status_conductor->root;
            status_conductor->conductor;
            status_conductor->conductor=status_conductor->conductor->next)
            {
                if(status_conductor->conductor->removed==0) count2++;

            }

        //is the tab we're closing the left most tab, AND the only tab?
        for(count=0, status_conductor->conductor=status_conductor->root;
            status_conductor->conductor;
            status_conductor->conductor=status_conductor->conductor->next, count++)
            {
                if(status_conductor->conductor == status_conductor->current_query && count==0) break;
            }

        if(status_conductor->conductor)
        {
            if(status_conductor->conductor == status_conductor->current_query && count==0)
            {

                if(DEBUG) printf("closing root tab\n");
                if(status_conductor->conductor->next)
                {

                    if(DEBUG) printf("closing root tab2\n");
                    status_conductor->conductor=status_conductor->conductor->next;
                    status_conductor->root=status_conductor->conductor;
                    status_conductor->root->previous=NULL;

                }

                if(DEBUG) printf("root tabs count: %d\n",count2);

                if(count2>=1)
                {
                    if(DEBUG) printf("freeing root tab string gadget struct\n");

                    //attempting to clean up typed history
                    for(status_conductor->current_query->string_conductor=status_conductor->current_query->string_root;
                        status_conductor->current_query->string_conductor;
                        status_conductor->current_query->string_conductor=work_history)
                    {

                        work_history=status_conductor->current_query->string_conductor->next;

                        if(status_conductor->current_query->string_conductor->buffer_history[0]!='\0')
                        {
                            delete status_conductor->current_query->string_conductor;
                        }

                    }

                    

                    FreeVec(status_conductor->current_query);
                    status_conductor->current_query=NULL;
                }

            }
        }
        else //we want to close a tab, remove its entry in the linked list, and pretend it never existed!
        {

            if(DEBUG) printf("closing regular tab\n");

            for(status_conductor->conductor=status_conductor->root; status_conductor->conductor;
                status_conductor->conductor=status_conductor->conductor->next)
            {
                    if(status_conductor->conductor->next == status_conductor->current_query) break;
            }

            if(status_conductor->conductor)
            {
                status_previous=status_conductor->conductor;
                status_conductor->conductor=status_conductor->conductor->next;
                status_next=status_conductor->conductor->next;
                status_previous->next=status_next;
                status_conductor->conductor->previous=status_previous;
            }

            //attempting to clean up typed history
            for(status_conductor->current_query->string_conductor=status_conductor->current_query->string_root;
                status_conductor->current_query->string_conductor;
                status_conductor->current_query->string_conductor=work_history)
            {

                work_history=status_conductor->current_query->string_conductor->next;

                if(status_conductor->current_query->string_conductor->buffer_history[0]!='\0')
                {
                    delete status_conductor->current_query->string_conductor;
                }

            }


            FreeVec(status_conductor->current_query);
            status_conductor->current_query=NULL;


        }

        for(count=1,status_conductor=status_root;
            status_conductor && status_conductor!=status_current;
            status_conductor=status_conductor->next,count++);

        status_conductor=status_current;


        //our linked list has been changed, a node has been removed. so now, lets give new
        //"tab switching" id codes to every node.
        for(status_conductor->conductor=status_conductor->root,status_conductor->iv=(count*100+1);
            status_conductor->conductor;
            status_conductor->conductor=status_conductor->conductor->next)
            {
                if(status_conductor->conductor)
                {

                    status_conductor->iv++;

                    if(status_conductor->conductor->BT_querybutton)
                    {
                        DoMethod((Object*)status_conductor->conductor->BT_querybutton,MUIM_KillNotify,MUIA_Selected);
                        DoMethod((Object*)status_conductor->conductor->BT_querybutton,MUIM_Notify,MUIA_Selected,TRUE,(Object*)James->App,2,MUIM_Application_ReturnID,status_conductor->iv);
                    }
                }
            }


        work_status=status_conductor;

        //give every LISTVIEW TAB entry a number so we can change it
        for(count=0,status_conductor=status_root; status_conductor; status_conductor=status_conductor->next)
        {
            for(status_conductor->conductor=status_conductor->root; status_conductor->conductor;
                status_conductor->conductor=status_conductor->conductor->next)
                {
                    if(status_conductor->conductor && status_conductor->conductor->removed==0)
                    {

                        status_conductor->conductor->nlist_tab_number=count;
                        if(DEBUG) printf("closing irc tab, new entry number %d\n",count);
                        count++;
                    }
            }
        }
        status_conductor=work_status;


        for(status_conductor->conductor=status_conductor->root; status_conductor->conductor->next;
            status_conductor->conductor=status_conductor->conductor->next);

        for(; status_conductor->conductor;
            status_conductor->conductor=status_conductor->conductor->previous)
        {
            if(status_conductor->conductor)
            {
                if(status_conductor->conductor->removed==0)
                {
                    if(status_conductor->conductor->LV_channel)
                    {

                        add_tabs_to_nicklist_group();

                        DoMethod((Object*)GR_top, OM_ADDMEMBER, (Object*)status_conductor->conductor->GR_conductor);
                        setmacro(status_conductor->conductor->BT_querybutton, MUIA_Selected, TRUE);
                        setmacro(status_conductor->conductor->BT_querybutton,MUIA_Text_PreParse, "\033c\0333");
                        status_conductor->current_query=status_conductor->conductor;
                        status_current=status_conductor;
                        count=1000;

                        break;
                    }
                }
            }
        }


        if(!status_conductor->conductor)
        {

            for(status_conductor=status_root; status_conductor; status_conductor=status_conductor->next)
            {
                for(status_conductor->conductor=status_conductor->root; status_conductor->conductor;
                    status_conductor->conductor=status_conductor->conductor->next)
                {
                    if(status_conductor->conductor->LV_channel)
                    {
                        DoMethod((Object*)GR_top, OM_ADDMEMBER, (Object*)status_conductor->conductor->GR_conductor);
                        setmacro(status_conductor->conductor->BT_querybutton, MUIA_Selected, TRUE);
                        setmacro(status_conductor->conductor->BT_querybutton,MUIA_Text_PreParse, "\033c\0333");
                        status_conductor->current_query=status_conductor->conductor;
                        status_current=status_conductor;
                        count=1000;
                        break;
                    }
                }

                if(count==1000) break;

            }


        }

        DoMethod((Object*)GR_top,MUIM_Group_ExitChange);
    }

    /*if(ZUNE_SYSTEM==TRUE)
    {

        dont_open_colours=TRUE;
        setmacro((Object*)James->WI_main,MUIA_Window_Open, TRUE);
        dont_open_colours=FALSE;
        setmacro((Object*)James->WI_main,MUIA_Window_Activate, TRUE);

    } */



}

//be able to close the root server tab if theres more than 1 server group
/*void CloseQueryWindow(void)
{

    int count_root_tabs=0;

    status_conductor=status_current;

    for(status_conductor=status_root; status_conductor; status_conductor=status_conductor->next)
    {
        for(status_conductor->conductor=status_conductor->root; status_conductor->conductor;
            status_conductor->conductor=status_conductor->conductor->next)
        {
            if(status_conductor->conductor->BT_querybutton) { count_root_tabs++; break; }

        }

    }


    status_conductor=status_current;
    status_conductor->conductor=status_conductor->current_query;


    count=0;

    if(status_conductor->current_query->name[0]=='#')
    {
        for(status_conductor->conductor=status_conductor->root; status_conductor->conductor; status_conductor->conductor=status_conductor->conductor->next)
        {

            if(status_conductor->conductor->BT_querybutton) count++;

        }

        status_conductor->conductor=status_conductor->current_query;

        if(count==1)
        {

            strcpy(status_conductor->conductor->name," ");
            strcpy(status_conductor->conductor->displayed_name,"(none)");
            setmacro((Object*)status_conductor->conductor->BT_querybutton,MUIA_Text_Contents,status_conductor->conductor->displayed_name);

            sprintf(buffer3,"%s%s%s%s %s",timestamp,GetCatalogStr(catalog,217,"«"),GetCatalogStr(catalog,165,"Part"),GetCatalogStr(catalog,218,"»"),status_conductor->nick);
        	add_text_to_current_list((char*)buffer3, 3, ACTIVITY_CHAT);

        }
    }


    if(status_conductor->current_query->name[0]=='$')
    {
        dcc_chat_conductor=dcc_chat_root->next;
        while(dcc_chat_conductor)
        {
            if(dcc_chat_conductor->conductor==status_conductor->current_query)
            {
                dcc_chat_conductor->removed=1; //yes its now removed
                shutdown((int)dcc_chat_conductor->dcc_socket,(int)2);
                break;
            }
            dcc_chat_conductor=dcc_chat_conductor->next;

        }

    }

	strcpy(status_conductor->current_query->name," ");

    query_window *status_work;

    status_work=status_conductor->current_query->next;

    for(count=0,status_conductor->conductor=status_conductor->root; status_conductor->conductor; status_conductor->conductor=status_conductor->conductor->next)
    {
        if(status_conductor->conductor->BT_querybutton) count++;

    }


    if(count==1 && count_root_tabs==1) return;

    if(DoMethod((Object*)GR_bottom_group,MUIM_Group_InitChange))
	{
        if(DoMethod((Object*)status_conductor->GR_server1_buttons,MUIM_Group_InitChange))
    	{
            DoMethod((Object*)status_conductor->GR_server1_buttons, OM_REMMEMBER, (Object*)status_conductor->current_query->BT_querybutton);
            MUI_DisposeObject((Object*)status_conductor->current_query->BT_querybutton);
            status_conductor->current_query->BT_querybutton=NULL;


            strcpy(status_conductor->current_query->name," ");
            strcpy(status_conductor->current_query->displayed_name,"(none)");

            if(status_work)
            {
                if(status_work->BT_querybutton)
                {

                    //printf("work1\n");

                    setmacro(status_work->BT_querybutton, MUIA_Selected, TRUE);
                    setmacro(status_work->BT_querybutton,MUIA_Text_PreParse, "\033c\0333");
                }
                else
                {
                    //printf("root1\n");
                    setmacro(status_conductor->root->BT_querybutton, MUIA_Selected, TRUE);
                    setmacro(status_conductor->root->BT_querybutton,MUIA_Text_PreParse, "\033c\0333");

                }
    		}
            else
            {
                    //printf("root2\n");

                //for(status_conductor=status_root; status_conductor; status_conductor=status_conductor->next)
                //{

                    for(status_conductor->conductor=status_conductor->root;
                        status_conductor->conductor; status_conductor->conductor=status_conductor->conductor->next)
                        {

                            if(status_conductor->conductor->BT_querybutton)
                            {
                                setmacro(status_conductor->conductor->BT_querybutton, MUIA_Selected, TRUE);
                                setmacro(status_conductor->conductor->BT_querybutton,MUIA_Text_PreParse, "\033c\0333");

                                break;
                            }

                        }

                    //if(status_conductor->conductor) break;

                //}

            }
            DoMethod((Object*)status_conductor->GR_server1_buttons,MUIM_Group_ExitChange);
        }


        //if(status_conductor->current_query==status_conductor->root)
        //if(count==1)
        if(count==1) // && count_root_tabs>=2)
        {
            //printf("removing now, count:%i, root_tabs:%i\n",count,count_root_tabs);

            //remove all buttons
            for(status_conductor->conductor=status_conductor->root; status_conductor->conductor; status_conductor->conductor=status_conductor->conductor->next)
            {
                DoMethod((Object*)status_conductor->GR_server1_buttons, OM_REMMEMBER, (Object*)status_conductor->conductor->BT_querybutton);
                MUI_DisposeObject((Object*)status_conductor->conductor->BT_querybutton);
                status_conductor->conductor->BT_querybutton=NULL;
            }

            DoMethod((Object*)GR_bottom_group, OM_REMMEMBER, (Object*)status_conductor->GR_server1_buttons);
            MUI_DisposeObject((Object*)status_conductor->GR_server1_buttons);
            status_conductor->GR_server1_buttons=NULL;

            if(my_settings.quit_msg) sprintf(buffer3,"QUIT :%s\r\n",my_settings.quit_msg);
                else sprintf(buffer3,"QUIT :Client Exiting\r\n");

            shutdown((int)status_conductor->a_socket,(int)2);
            FD_CLR(status_conductor->a_socket, &read_master);
        }

    	DoMethod((Object*)GR_bottom_group,MUIM_Group_ExitChange);

    }


    // Add the new group and buttons now that the closed ones have been removed

    if(DoMethod((Object*)GR_top,MUIM_Group_InitChange))
    {
        DoMethod((Object*)GR_top, OM_REMMEMBER, (Object*)status_conductor->current_query->GR_conductor);

        //MUI_DisposeObject((Object*)status_conductor->current_query->GR_conductor);
        //status_conductor->current_query->GR_conductor=NULL;
        //status_conductor->current_query->LV_channel=NULL;

        if(status_conductor->current_query->log_file)
        {
            Close(status_conductor->current_query->log_file);
            status_conductor->current_query->log_file=0;
        }

        if(status_work)
        {
            if(status_work->BT_querybutton)
            {
                DoMethod((Object*)GR_top, OM_ADDMEMBER, (Object*)status_work->GR_conductor);
                status_conductor->current_query=status_work;
                //printf("1\n");
            }
            else
            {

                DoMethod((Object*)GR_top, OM_ADDMEMBER, (Object*)status_conductor->root->GR_conductor);
                status_conductor->current_query=status_conductor->root;
                //printf("2\n");


            }

		}
        else
        {
            DoMethod((Object*)GR_top, OM_ADDMEMBER, (Object*)status_conductor->root->GR_conductor);
            status_conductor->current_query=status_conductor->root;
                //printf("3\n");

        }
        DoMethod((Object*)GR_top,MUIM_Group_ExitChange);
    }

    status_conductor->previous_query=status_conductor->root;



}*/




void SQuote(char *message)
{
	sprintf(sendstuff,"%s\r\n", message);
		
    send_text(sendstuff);
		
}						


void SPing(char *string1)
{
    ping_time();

    sprintf(sendstuff,"PRIVMSG %s :\001PING %s\001\r\n",string1,pingtimestamp);
    send_text(sendstuff);

    timestamp_2_string();
    sprintf(buffer3,"%s%sCTCP%s %s PING %s %s",timestamp,GetCatalogStr(catalog,217,"«"),GetCatalogStr(catalog,218,"»"),GetCatalogStr(catalog,213,"Sending CTCP"),GetCatalogStr(catalog,205,"to"), string1);

	add_text_to_current_list((char*)buffer3, 6, ACTIVITY_CHAT);


}
