/*
    The contents of this file are subject to the AROS Public License Version 1.1 
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis, 
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
    for the specific language governing rights and limitations under the License.
*/

#include "includes.h"

#include <mui/NListtree_mcc.h>

#include "intern.h"

void close_server_select_window()
{

    int a = 0;
    int abc = 1;

    BPTR save_file = Open((_s_cs)"progdir:servers.txt", MODE_NEWFILE);

    FPuts(save_file, (l_in) "WOOKIECHAT_SERVERS_FILE_2\n");

    do
    {
        struct MUI_NListtree_TreeNode *treenode = (struct MUI_NListtree_TreeNode *) DoMethod(
                (Object*) WookieChat->NLT_Servers, MUIM_NListtree_GetEntry, MUIV_NListtree_GetEntry_ListNode_Root, a,
                0);
        a++;

        if (!treenode)
            abc = 0;
        else
        {

            FPuts(save_file, (l_in) treenode->tn_Name);
            FPuts(save_file, (l_in) "\n");

        }

    } while (abc);

    Close(save_file);

    setmacro((Object*) WookieChat->WI_label_2, MUIA_Window_Open, FALSE);

}

