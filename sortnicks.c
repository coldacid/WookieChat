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

#define SORT    1

int sort_nicks(int a)
{
    if (SORT)
    {
        unsigned int flipped;
        unsigned int indx = 1;
        unsigned int indx2 = 0;

        entries = status_conductor->conductor->nicks;

        if (entries <= 1)
            return 0;

        //SORT EVERYTHING ALPHABETICALLY
        do
        {
            flipped = 0;

            for (indx2 = entries - 1; indx2 >= indx; --indx2)
            {

                if (stricmp((p_in) status_conductor->conductor->nicklist[indx2].name,
                        (p_in) status_conductor->conductor->nicklist[indx2 - 1].name) < 0)
                {

                    strcpy(work_entry4.name, status_conductor->conductor->nicklist[indx2 - 1].name);
                    strcpy(work_entry4.modes, status_conductor->conductor->nicklist[indx2 - 1].modes);

                    strcpy(status_conductor->conductor->nicklist[indx2 - 1].name,
                            status_conductor->conductor->nicklist[indx2].name);
                    strcpy(status_conductor->conductor->nicklist[indx2 - 1].modes,
                            status_conductor->conductor->nicklist[indx2].modes);

                    strcpy(status_conductor->conductor->nicklist[indx2].name, work_entry4.name);
                    strcpy(status_conductor->conductor->nicklist[indx2].modes, work_entry4.modes);

                    flipped = 1;
                }
            }

        } while ((++indx < entries) && flipped);

    }

    return 0;

}

int num_nodes;
struct query_window *prev = 0, *next = 0, *head, *current = 0;

void sort_linked_list(void)
{
    if (my_settings.sort_tabs_alphabetically == 0)
        return;

    if (DoMethod((Object*) status_conductor->GR_server1_buttons, MUIM_Group_InitChange))
    {

        for (num_nodes = 0, status_conductor->conductor = status_conductor->root; status_conductor->conductor;
                status_conductor->conductor = status_conductor->conductor->next)
        {
            num_nodes++;
            DoMethod((Object*) status_conductor->GR_server1_buttons, OM_REMMEMBER,
                    (Object*) status_conductor->conductor->BT_querybutton);

        }

        //current ptr is used to point the current node in the list
        //prev ptr is used to point the previous node of current node
        //next ptr is used to point the next node of current node
        int i, j;

        //Initially current node and previous node should be the same(first) node in the list
        if (my_settings.no_server_tabs == 0)
        {
            num_nodes--;
            head = status_conductor->root->next;
        }
        else
            head = status_conductor->root;

        current = head;
        prev = head;
        if (!head)
            return;

        next = head->next;
        for (i = 0; i < num_nodes - 1; i++)
        {
            for (j = 0; j < num_nodes - i - 1; j++)
            {

                if (stricmp(current->name, next->name) > 0)
                {
                    current->next = next->next;
                    next->next = current;

                    //test
                    next->previous = current->previous;

                    if (current == head)
                    {
                        if (my_settings.no_server_tabs == 0)
                            status_conductor->root->next = next;
                        else
                            status_conductor->root = next;

                        head = next;
                        prev = next;
                        //head=next; prev=NULL;
                    }
                    else
                    {
                        //prev->next=next;prev=next;

                        prev->next = next; //prev->previous=next->previous;
                    }
                    if (next != NULL) //check whether final node is reached
                        next = current->next;
                }
                else //just moved each node ptr by one position
                {
                    prev = current;
                    current = next;
                    next = current->next;

                    if (next)
                        next->previous = prev;
                }

            }
            //For next iteration make the initial settings again
            current = head;
            prev = head;
            //prev=current->previous;
            next = current->next;
        }

        for (status_conductor->conductor = status_conductor->root; status_conductor->conductor;)
        {
            work_query = status_conductor->conductor;
            status_conductor->conductor = status_conductor->conductor->next;

            if (status_conductor->conductor)
                status_conductor->conductor->previous = work_query; //status_conductor->conductor;

            //status_conductor->conductor=status_conductor->conductor->next
        }

        /*while(current!=NULL)
         {
         current->previous=current;
         current=current->next;

         } */

        //For display the sorted numbers
        if (DEBUG)
        {
            current = status_conductor->root;
            while (current != NULL)
            {
                printf("linked list sort->%s\n", current->name);
                current = current->next;
            }

            current = status_conductor->root;
            while (current->next != NULL)
                current = current->next;

            while (current != NULL)
            {
                printf("backwards linked list sort->%s\n", current->name);
                current = current->previous;
            }

        }

        work_status = status_conductor;

        for (count = 1, status_conductor = status_root; status_conductor && status_conductor != work_status; //status_conductor!=status_current;
                status_conductor = status_conductor->next, count++)
            ;

        status_conductor = work_status;

        //our linked list has been changed, a node has been removed. so now, lets give new
        //"tab switching" id codes to every node.
        for (status_conductor->conductor = status_conductor->root, status_conductor->iv = (count * 100 + 1);
                status_conductor->conductor; status_conductor->conductor = status_conductor->conductor->next)
        {
            if (status_conductor->conductor)
            {
                status_conductor->iv++;
                status_conductor->conductor->iv = status_conductor->iv;
                if (status_conductor->conductor->BT_querybutton)
                {
                    DoMethod((Object*) status_conductor->conductor->BT_querybutton, MUIM_KillNotify, MUIA_Selected);

                    DoMethod((Object*) status_conductor->GR_server1_buttons, OM_ADDMEMBER,
                            (Object*) status_conductor->conductor->BT_querybutton);

                    DoMethod((Object*) status_conductor->conductor->BT_querybutton, MUIM_Notify, MUIA_Selected, TRUE,
                            (Object*) WookieChat->App, 2, MUIM_Application_ReturnID, status_conductor->iv);
                }
            }
        }

        DoMethod((Object*) status_conductor->GR_server1_buttons, MUIM_Group_ExitChange);

    }

    give_each_tab_a_listview_number_for_switching_tabs();

    //make sure our dcc chat windows are directed to the right place.
    //and also replace our entries in our listview for the "listview-style tabs"
    for (count = 0, status_conductor = status_root; status_conductor; status_conductor = status_conductor->next)
    {
        for (status_conductor->conductor = status_conductor->root; status_conductor->conductor;
                status_conductor->conductor = status_conductor->conductor->next)
        {
            if (status_conductor->conductor->name[0] == '$')
            {
                //if(DEBUG) printf("count:%d equals a dcc chat tab!\n",count);
                for (dcc_chat_conductor = dcc_chat_root; dcc_chat_conductor;
                        dcc_chat_conductor = dcc_chat_conductor->next)
                {
                    if (dcc_chat_conductor)
                    {
                        if (!stricmp(dcc_chat_conductor->name, status_conductor->conductor->name))
                        {
                            dcc_chat_conductor->conductor = status_conductor->conductor;
                            //if(DEBUG) printf("count:%d new dcc chat tab at\n",count);
                        }

                    }
                }

            }

            if (status_conductor->conductor && status_conductor->conductor->removed == 0)
            {
                DoMethod((Object*) WookieChat->LV_tabs, MUIM_NList_ReplaceSingle,
                        &status_conductor->conductor->nlist_tab, status_conductor->conductor->nlist_tab_number, 0, 0);

            }
        }
    }

    setmacro((Object*) WookieChat->LV_tabs, MUIA_NList_Active, status_current->current_query->nlist_tab_number);

    status_conductor = work_status;

    for (status_conductor->conductor = status_conductor->root; status_conductor->conductor->next;
            status_conductor->conductor = status_conductor->conductor->next)
        ;
}
