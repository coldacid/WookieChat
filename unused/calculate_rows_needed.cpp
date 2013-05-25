/*
    The contents of this file are subject to the AROS Public License Version 1.1 
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis, 
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
    for the specific language governing rights and limitations under the License.
*/

long window_width, group_width;

void calculate_how_many_rows_needed()
{

    /*get((Object*)GR_buttons,MUIA_Virtgroup_Width,&group_width);
    get((Object*)WookieChat->WI_main,MUIA_Window_Width,&window_width);

    printf("group_width:%i\n",group_width);
    printf("window_width:%i\n",window_width);

    float divided_result=group_width/window_width;
    float rows_needed=(long)divided_result;

    printf("divided_result:%f\n",divided_result);

    rows_needed++;

    if(rows_needed>1)
    {
        printf("rows_needed rounded up:%f\n",rows_needed);

        //set((Object*)GR_buttons,MUIA_Virtgroup_Width,&group_width);
        //set((Object*)GR_virtual_buttons,MUIA_Group_Rows,(float)rows_needed);

        status_conductor=status_root;

        do
        {
            if(DoMethod((Object*)GR_bottom_group,MUIM_Group_InitChange))
        	{

                if(DoMethod((Object*)GR_buttons,MUIM_Group_InitChange))
            	{

                    if(DoMethod((Object*)status_conductor->GR_server1_buttons,MUIM_Group_InitChange))
                	{

                            set((Object*)status_conductor->GR_server1_buttons,MUIA_Group_Rows,(float)rows_needed);
                            status_conductor=status_conductor->next;
                    }

                	DoMethod((Object*)GR_buttons,MUIM_Group_ExitChange);
                }

                DoMethod((Object*)GR_bottom_group,MUIM_Group_ExitChange);
            }

        }
        while(status_conductor);

    }*/

}



