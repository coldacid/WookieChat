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
#include "locale.h"

/* Locals */
static char *string1;
static char buffer3[BUFFERSIZE*2];
static int total_charsets;
static char server_nick[128];
static char server_nick2[128];
static char server_nick3[128];
static char server_real_name[256];
static char server_user_name[256];


char * get_server_info(int id)
{
    switch(id)
    {
    case(SI_NICK_1): return server_nick;
    case(SI_NICK_2): return server_nick2;
    case(SI_NICK_3): return server_nick3;
    case(SI_REAL_NAME): return server_real_name;
    case(SI_USER_NAME): return server_user_name;
    default:return NULL;
    }
}

void acquire_connect_details(char *work_buffer)
{

    char the_value[300];
    char parameter[300];

    strcpy(server_name, "");
    strcpy(port_number, "");
    strcpy(server_charset, "");
    strcpy(nick_password, "");
    strcpy(server_password, "");
    strcpy(auto_joins, "");
    strcpy(auto_connect, "");

    use_global = 1;

    string1 = strtok(work_buffer, " ");
    while (string1)
    {
        strcpy(the_value, "");
        int count2, count;

        for (count2 = 0, count = 0; count <= (LONG) strlen(string1); count2++, count++)
        {
            if (string1[count] != '\0' && string1[count] != '=' && string1[count] != ' ')
                parameter[count2] = string1[count];
            else
            {
                count++;
                parameter[count2] = '\0';
                break;
            }
        }
        for (count2 = 0; count <= (LONG) strlen(string1); count2++, count++)
        {
            if (string1[count] != '\0' && string1[count] != ' ')
                the_value[count2] = string1[count];
            else
            {
                count++;
                the_value[count2] = '\0';
                break;
            }
        }

        if (!stricmp(parameter, "SERVER"))
            strcpy(server_name, the_value);
        else if (!stricmp(parameter, "PORT"))
            strcpy(port_number, the_value);
        else if (!stricmp(parameter, "SERVER_CHARSET"))
            strcpy(server_charset, the_value);
        else if (!stricmp(parameter, "NICK_PASSWORD"))
            strcpy(nick_password, the_value);
        else if (!stricmp(parameter, "SERVER_PASSWORD"))
            strcpy(server_password, the_value);
        else if (!stricmp(parameter, "AUTOJOINS"))
            strcpy(auto_joins, the_value);
        else if (!stricmp(parameter, "AUTOCONNECT"))
            strcpy(auto_connect, the_value);
        else if (!stricmp(parameter, "+") || !stricmp(parameter, "Euro"))
            strcpy(server_charset, "ISO-8859-1 + Euro");
        else if (!stricmp(parameter, "NICK1"))
            strcpy(server_nick, the_value);
        else if (!stricmp(parameter, "NICK2"))
            strcpy(server_nick2, the_value);
        else if (!stricmp(parameter, "NICK3"))
            strcpy(server_nick3, the_value);
        else if (!stricmp(parameter, "USERNAME"))
            strcpy(server_user_name, the_value);
        else if (!stricmp(parameter, "REALNAME"))
            strcpy(server_real_name, the_value);
        else if (!stricmp(parameter, "USE_GLOBAL"))
        {
            if (!strcmp(the_value, "1"))
                use_global = 1;
            else
                use_global = 0;
            //printf("the value:%s  use global:%i\n", the_value, use_global);
            //strcpy(use_global,the_value);
        }
        else if (the_value[0] == '\0')
            strcpy(server_name, string1);

        string1 = strtok(NULL, " \n");
    }

}

int connect2server(char *servername, char *port_number, int typedservercommand, int set_connect_script_name,
        int connect_in_current_tab, char *requested_remote_charset, int write_new_passwords_to_this_node)
{
    int count;
    struct hostent *he;

    if (use_global)
    {
        strcpy(status_conductor->nick, nick);
        strcpy(status_conductor->nick2, nick2);
        strcpy(status_conductor->nick3, nick3);
        strcpy(status_conductor->user_name, username);
        strcpy(status_conductor->real_name, realname);

    }
    else
    {
        strcpy(status_conductor->nick, server_nick);
        strcpy(status_conductor->nick2, server_nick2);
        strcpy(status_conductor->nick3, server_nick3);
        strcpy(status_conductor->user_name, server_user_name);
        strcpy(status_conductor->real_name, server_real_name);
    }

    if (!PRO_CHARSETS_ENABLED)
    {
        for (count = 0; remote_charsets[count] != NULL && count < total_charsets; count++)
        {
            if (!stricmp(remote_charsets[count], "UTF-8"))
            {
                //printf("connecting, finding remote charset.. %s \n", remote_charsets[count]);

                status_conductor->remote_charset = count;
                break;
            }
        }

        if (remote_charsets[status_conductor->remote_charset] == NULL)
            status_conductor->remote_charset = local_charset;

    }
    else
    {

        if (requested_remote_charset == NULL)
        {
            status_conductor->remote_charset = local_charset;
            //printf("connect 1\n");
        }
        else
        {
            //printf("connect 2\n");

            for (count = 0; remote_charsets[count] != NULL && count < total_charsets; count++)
            {
                if (!stricmp(remote_charsets[count], requested_remote_charset))
                {
                    status_conductor->remote_charset = count;
                    break;
                }
            }

        }

        if (remote_charsets[status_conductor->remote_charset] == NULL || count == total_charsets)
            status_conductor->remote_charset = local_charset;
    }

    if (connect_in_current_tab == 1)
        status_conductor = status_current;

    status_conductor->quit_requested = FALSE;

    status_conductor->away = FALSE;

    strcpy(status_conductor->chantypes, "#");

    // START OF PREFORM ON CONNECT STUFF
    //
    // the "set_connect_script_name" variable is so that automatically reconnecting to a server after getting
    // disconnected doesnt grab the wrong groupname.. i.e. the currently active entry in the server selection window
    //
    if (set_connect_script_name == 1) //do we want to set the connect script name? YES!!
    {
        struct MUI_NListtree_TreeNode *treenode;
        ULONG empty = DoMethod((Object*) WookieChat->NLT_Servers, MUIM_NListtree_GetNr,
                MUIV_NListtree_GetNr_TreeNode_Active, 0);
        int h = empty;

        while (h != 0)
        {
            treenode = (struct MUI_NListtree_TreeNode *) DoMethod((Object*) WookieChat->NLT_Servers,
                    MUIM_NListtree_GetEntry, MUIV_NListtree_GetEntry_ListNode_Root, h, 0);
            if (!treenode)
                break;
            if (!(strchr((char *)treenode->tn_Name, ' ') > 0))
                break;
            h = h - 1;

        }

        if (treenode)
        {
            if (treenode->tn_Name)
                strcpy(status_conductor->groupname, (char *)treenode->tn_Name);
            if (typedservercommand)
                strcpy(status_conductor->groupname, servername);
        }
        else
            strcpy(status_conductor->groupname, servername);

        //if(treenode->tn_Name) strcpy(status_conductor->groupname,treenode->tn_Name);
        if (typedservercommand)
            strcpy(status_conductor->groupname, servername);
    }
    // END OF PREFORM ON CONNECT STUFF

    strcpy(status_conductor->servername, servername);
    if (strcmp(status_conductor->groupname, "") != 0)
        strcpy(status_conductor->networkname, status_conductor->groupname);

    if (!SocketBase)
    {
        SocketBase = OpenLibrary((_ub_cs) "bsdsocket.library", 0L);
#ifdef __amigaos4__
        ISocket = (struct SocketIFace*) GetInterface((struct Library *)SocketBase, "main", 1, NULL);
#endif

    }

    if (!SocketBase)
    {

		sprintf(buffer3, "%s%s%s%s %s", timestamp, LGS( MSG_OPENING_BRACKET ), LGS( MSG_ERROR ),
				LGS( MSG_CLOSING_BRACKET ),
				LGS( MSG_UNABLE_TO_OPEN_TCPIP_STACK ) );

        add_text_to_current_list(buffer3, 9, ACTIVITY);
        return 0;

    }

    if (DEBUG)
        printf("server name you're trying to connect to is \"%s\"\n", servername);

    if (ZUNE_SYSTEM == TRUE)
        sprintf(status_conductor->root->nlist_tab_title.entry, "         \033c\033E%s", status_conductor->servername);
    else
        sprintf(status_conductor->root->nlist_tab_title.entry, "         :\033c\033E%s", status_conductor->servername);
    status_conductor->root->nlist_tab_title.colour = 23;
    DoMethod((Object*) WookieChat->LV_tabs, MUIM_NList_ReplaceSingle, &status_conductor->root->nlist_tab_title,
            status_conductor->root->nlist_tab_title_number);

#ifdef __MORPHOS__
    if ( !(he=gethostbyname((UBYTE*)servername)) ) // get the host info
#elif __AROS__
    if (!(he = gethostbyname((char*) servername))) // get the host info
#else
    if ( !(he=gethostbyname((i_in)servername)) ) // get the host info
#endif
    {
        timestamp_2_string();
		sprintf(buffer3, "%s%s%s%s %s: gethostbyname() %s.", timestamp, LGS( MSG_OPENING_BRACKET ),
				LGS( MSG_ERROR ), LGS( MSG_CLOSING_BRACKET ), servername, LGS( MSG_DCC_FAILED ));
        add_text_to_current_list(buffer3, 9, ACTIVITY);
        //perror("gethostbyname");
        return 0;
    }

    //int portnumber=atoi(port_number);
    status_conductor->portnumber = atoi(port_number);

    if (!(status_conductor->portnumber))
        status_conductor->portnumber = 6667;

    if (write_new_passwords_to_this_node == 1)
    {
        strcpy(status_conductor->pass, server_password);
        strcpy(status_conductor->nick_pass, nick_password);
        strcpy(status_conductor->auto_joins, auto_joins);
    }

    status_conductor->slapshot_in.sin_family = AF_INET;
    status_conductor->slapshot_in.sin_port = htons(status_conductor->portnumber);
    status_conductor->slapshot_in.sin_addr = *((struct in_addr *) he->h_addr);
    memset(&(status_conductor->slapshot_in.sin_zero), '\0', 8);

    //prepare the socket and conductor to be able to connect() again
    if (status_conductor->connection_active)
    {
        status_conductor->buffer[0] = '\0';
        status_conductor->connection_active = 0;
        status_conductor->servername[0] = '\0';
        status_conductor->networkname[0] = '\0';

		sprintf(buffer3, "%s%sConnect%s %s", timestamp,
					LGS( MSG_OPENING_BRACKET ), LGS( MSG_CLOSING_BRACKET ),
					LGS( MSG_CONNECT_TO_A_NEW_SERVER ) ),

        status_conductor->conductor = status_conductor->root;
        while (status_conductor->conductor)
        {
            if (status_conductor->conductor->LV_channel
                    && (status_conductor->conductor->name[0] == '#'
                            || status_conductor->conductor == status_conductor->root))
            {
                add_text_to_conductor_list(buffer3, 9, ACTIVITY);
            }

            status_conductor->conductor = status_conductor->conductor->next;
        }

        //status_conductor->conductor=status_conductor->current_query;
        //if(status_conductor->conductor->LV_channel) add_text_to_conductor_list(buffer3,9, ACTIVITY);

        FD_CLR(status_conductor->a_socket, &read_master);
        FD_CLR(status_conductor->a_socket, &write_master);

        //shutdown(status_conductor->a_socket,(int)2);

    }

    if (status_conductor->a_socket != -1)
    {
        if (CloseSocket(status_conductor->a_socket) == 0) //if its successful..
        {
            status_conductor->a_socket = -1;

        }
    }
    status_conductor->a_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (fdmax < status_conductor->a_socket)
        fdmax = status_conductor->a_socket;

    //status_conductor->a_socket = socket(AF_INET,SOCK_STREAM,0);

    //if(status_conductor->a_socket != -1) fdmax = status_conductor->a_socket;

    if (status_conductor->a_socket != -1)
    {
        FD_SET(status_conductor->a_socket, &read_master);
        FD_SET(status_conductor->a_socket, &write_master);
    }

    //printf("socket fd:%d\n",status_conductor->a_socket);

    //setup IDENTD server
    if (my_settings.use_ident)
    {
        CloseSocket(status_conductor->ident.listen_socket);
        status_conductor->ident.listen_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (DEBUG)
        {
            if (status_conductor->ident.listen_socket == -1)
                printf("unable to create listening socket\nerror number:%li\n", Errno());
            else
                printf("dcc listen socket created, number: %li\n", status_conductor->ident.listen_socket);
        }

        if (fdmax < status_conductor->ident.listen_socket)
            fdmax = status_conductor->ident.listen_socket;
        //fdmax = status_conductor->ident.listen_socket;
        status_conductor->ident.a_socket = -1;

        memset(&status_conductor->ident.test, '\0', sizeof(status_conductor->ident.test));
        status_conductor->ident.test.sin_family = AF_INET; // host byte order
        status_conductor->ident.test.sin_addr.s_addr = htonl(INADDR_ANY); // auto-fill with my IP
        status_conductor->ident.test.sin_port = htons(113); // short, network byte order

        int yes = 1;

        if (setsockopt((int) status_conductor->ident.listen_socket, (int) SOL_SOCKET, (int) SO_REUSEADDR, &yes,
                (int) sizeof(yes)) == -1)
            if (DEBUG)
                printf("unable to reuse address\n");

        if (setsockopt((int) status_conductor->ident.listen_socket, (int) SOL_SOCKET, (int) SO_REUSEPORT, &yes,
                (int) sizeof(yes)) == -1)
            if (DEBUG)
                printf("unable to reuse port\n");

        if (bind(status_conductor->ident.listen_socket, (struct sockaddr *) &status_conductor->ident.test,
                sizeof(status_conductor->ident.test)) == -1)
            if (DEBUG)
                printf("unable to bind address to socket, error number:%li\n", Errno());

        if (IoctlSocket(status_conductor->ident.listen_socket, FIONBIO, (char*) &yes) < 0)
            if (DEBUG)
                printf("unable to change non-blocking I/O option for socket, error number:%li\n", Errno());

        FD_SET(status_conductor->ident.listen_socket, &read_master);

        listen((int) status_conductor->ident.listen_socket, (int) 5);
    }
    //END of IDENTD setup

    long i = 1;

    IoctlSocket(status_conductor->a_socket, FIONBIO, (char*) &i);

    timestamp_2_string();

    if (status_conductor->retry_number == 0)
    {
        //memset(status_conductor->nick,'\0',30);
        //strcpy(status_conductor->nick,nick);
		sprintf(buffer3, "%s%sConnect%s %s %s (%i)", timestamp,
				LGS( MSG_OPENING_BRACKET ),
				LGS( MSG_CLOSING_BRACKET ),
				LGS( MSG_ATTEMPTING_TO_CONNECT_TO_SERVER ),
				servername,
                status_conductor->portnumber);
        add_text_to_current_list(buffer3, 9, ACTIVITY);
    }
    else
    {
		sprintf(buffer3, "%s%sConnect%s %s %s (%i) %s%i", timestamp,
				LGS( MSG_OPENING_BRACKET ),
				LGS( MSG_CLOSING_BRACKET ),
				LGS( MSG_ATTEMPTING_TO_CONNECT_TO_SERVER ),
				servername,
				status_conductor->portnumber,
				LGS( MSG_RETRY ),
				status_conductor->retry_number);
        add_text_to_conductor_list(buffer3, 9, ACTIVITY);

    }

    strcpy(status_conductor->initial_nick, status_conductor->nick);

    connect(status_conductor->a_socket, (struct sockaddr*) &status_conductor->slapshot_in, sizeof(struct sockaddr));

    return 0;
}

void set_total_charsets(int val)
{
    total_charsets = val;
}
