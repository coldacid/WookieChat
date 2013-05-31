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

/* Locals */
static int last_line = 999999;
static int last_pos1 = 999;
static int last_pos2 = 999;
static int first_pos1 = 0;
static char string22[800];
static struct MUI_NList_GetEntryInfo clipboard_struct;
static struct MUI_NList_GetSelectInfo select_struct;
static int last_clipboardline = 999;
static char new_array3[20];
static char *string2;
static char *string3;
static char *string1;
static char work_buffer[900];

#ifdef __amigaos4__
void Custom_Clipboard2_Func(REG(a0, struct Hook *hook),REG(a2, char **array),REG(a1, char *new_entry)) {
#elif __MORPHOS__
void Custom_Clipboard2_Func(REG(a0, struct Hook *hook),REG(a2, char **array),REG(a1, char *new_entry)) {
#elif __AROS__
void Custom_Clipboard2_Func(struct Hook *hook,char **array,char *new_entry) {
#elif __GNUC__
void Custom_Clipboard2_Func(void) {

  register char **a2 __asm("a2");                         char **array = a2;
  register char *a1 __asm("a1");                          char *new_entry = a1;
  register struct Hook *a0 __asm("a0");                   struct Hook *hook = a0;
#endif


    if(DEBUG) printf("\nCLIPBOARD START..\n");

    clipboard_struct.pos=(long)array[-1];

    DoMethod((Object*)status_current->current_query->LV_channel,MUIM_NList_GetEntryInfo,&clipboard_struct);

    if(DEBUG) printf("pos (IMPORTANT):%lu\n",clipboard_struct.pos);
    if(DEBUG) printf("clipboard_line:%lu\n",clipboard_struct.line);
    if(DEBUG) printf("entry_pos:%lu\n",clipboard_struct.entry_pos);
    if(DEBUG) printf("charpos:%lu\n",clipboard_struct.charpos);
    if(DEBUG) printf("charlen:%lu\n",clipboard_struct.charlen);
    if(DEBUG) printf("entry:%s\n",(char*)clipboard_struct.entry);
    if(DEBUG) printf("wrapcol:%lu\n",clipboard_struct.wrapcol);

//struct MUI_NList_GetEntryInfo
//  LONG pos;             /* num of entry you want info about */
//  LONG line;            /* real line number */
//  LONG entry_pos;       /* entry num of returned entry ptr */
//  APTR entry;           /* entry pointer */
//  LONG wrapcol;         /* NOWRAP, WRAPCOLx, or WRAPPED|WRAPCOLx */
//  LONG charpos;         /* start char number in string (unused if NOWRAP) */
//  LONG charlen;         /* string lenght (unused if NOWRAP) */

    DoMethod((Object*)status_current->current_query->LV_channel,MUIM_NList_GetSelectInfo,&select_struct);

    if(DEBUG) printf("start:%lu\n",select_struct.start);
    if(DEBUG) printf("end:%lu\n",select_struct.end);
    if(DEBUG) printf("startpos:%lu\n",select_struct.start_pos);
    if(DEBUG) printf("endpos:%lu\n",select_struct.end_pos);
    if(DEBUG) printf("startcol:%lu\n",select_struct.start_column);
    if(DEBUG) printf("endcol:%lu\n",select_struct.end_column);
    if(DEBUG) printf("vstart:%lu\n",select_struct.vstart);
    if(DEBUG) printf("vend:%lu\n",select_struct.vend);
    if(DEBUG) printf("vnum:%lu\n",select_struct.vnum);

//  LONG start;        /* num of first selected *REAL* entry/line (first of wrapped from which start is issued) */
//  LONG end;          /* num of last selected *REAL* entry/line (first of wrapped from which start is issued) */
//  LONG num;          /* not used */
//  LONG start_column; /* column of start of selection in 'start' entry */
//  LONG end_column;   /* column of end of selection in 'end' entry */
//  LONG start_pos;    /* char pos of start of selection in 'start_column' entry */
//  LONG end_pos;      /* char pos of end of selection in 'end_column' entry */
//  LONG vstart;       /* num of first visually selected entry */
//  LONG vend;         /* num of last visually selected entry */
//  LONG vnum;         /* number of visually selected entries */


    #ifdef __AROS__
    long pos1=(long)array[1];
    //int work_pos1=(int)array[1];
    long pos2=(long)array[2];
    #else
    int pos1=(int)array[1];
    //int work_pos1=(int)array[1];
    int pos2=(int)array[2];

    #endif

    pos1=select_struct.start_pos;
    pos2=select_struct.end_pos;
    int work_pos1=pos1;
    if(!which_clipboard_style() && clipboard_struct.pos == select_struct.vstart)
    {
        first_pos1=pos1;
        last_line=9999;
        last_pos1=999;
        last_pos2=999;
    }

    if(DEBUG) printf("first_pos1:%i\n",first_pos1);
    if(DEBUG) printf("work_pos1:%i\n",work_pos1);
    if(DEBUG) printf("pos1:%i\n",pos1);
    if(DEBUG) printf("pos2:%i\n",pos2);


    strcpy(work_buffer,new_entry);

    strtok(work_buffer,"]");
    string1=strtok(NULL," ");  //timestamp
    string2=strtok(NULL," ");  //2nd column
    string3=strtok(NULL,"");   //the rest

    //we dont want to do the actions on the same entry twice
    if(last_line==clipboard_struct.line && last_pos1==pos1 && last_pos2==pos2) { array[0]=(char*)""; return; };
    if(!string1 || !string2) { array[0]=(char*)""; return; }


    if(!string2) { string2=malloc(sizeof(char)); strcpy(string2,""); }
    if(!string3) { string3=malloc(sizeof(char)); strcpy(string3,"");}

    //we dont want to do the actions on the same entry twice
    last_line=clipboard_struct.line;
    last_pos1=pos1;
    last_pos2=pos2;
    last_clipboardline=clipboard_struct.line;


    if(select_struct.start_column<=select_struct.end_column)
    {
        if(select_struct.start_column==0)
        {

            if(string1)
            {
                if(pos1==-1)
                {


                    if(DEBUG) printf("1a\n");
                    pos1=10;
                }
                else
                {
                    if(clipboard_struct.pos == select_struct.vstart)
                    {

                        if(DEBUG) printf("1b\n");
                        {
                            if(which_clipboard_style() && clipboard_struct.pos > select_struct.vstart) pos1=0;
                            if(DEBUG) printf("1b changing pos1 to %d\n",pos1);
                        }
                    }
                    else
                    {

                        if(DEBUG) printf("1bb\n");
                        {
                            if(!which_clipboard_style() && clipboard_struct.pos > select_struct.vstart) pos1=10;
                            if(DEBUG) printf("1bb changing pos1 to %d\n",pos1);
                        }


                    }
                }
            }

            if(which_clipboard_style() && clipboard_struct.pos > select_struct.vstart) { pos1=10;
                        if(DEBUG) printf("1b normal changing pos1 to %d\n",pos1);                         }
            else if(!which_clipboard_style() && clipboard_struct.pos > select_struct.vstart) { pos1=first_pos1;
                        if(DEBUG)  printf("1b columns changing pos1 to %d\n",pos1);                        }


        }
        else if(select_struct.start_column==1)
        {

            if(string1)
            {
                if(clipboard_struct.pos == select_struct.vstart)
                {
                    if(pos1 != -1)
                    {
                        if(!which_clipboard_style())
                        {
                            //we dont want to copy from the start of the selected text
                            //pos1=select_struct.start_pos+strlen(string1)-1;

                            //we want to copy from the beginning of the column
                            pos1=strlen(string1)+11;
                            if(DEBUG) printf("2aaa\n");
                            first_pos1=10;

                            if(DEBUG) printf("2a\n");
                        }
                        else
                        {
                            pos1=select_struct.start_pos+strlen(string1)-1;
                            if(DEBUG) printf("2aa\n");
                        }

                    }
                    else
                    {
                        pos1=strlen(string1)+11;
                        if(DEBUG) printf("2aaa\n");
                    }

                }
                else
                {
                    if(which_clipboard_style() && clipboard_struct.pos > select_struct.vstart) pos1=10;
                    if(DEBUG) printf("2b\n");

                }
            }


            if(which_clipboard_style() && clipboard_struct.pos > select_struct.vstart) { pos1=10;
                        if(DEBUG)  printf("2 normal changing pos1 to %d\n",pos1);                         }
            if(!which_clipboard_style() && clipboard_struct.pos > select_struct.vstart)
            {
                    pos1=strlen(string1)+11; //strlen(string2)+4;

                if(DEBUG)  printf("2 columns changing pos1 to %d\n",pos1);
            }

        }
        else if(select_struct.start_column==2)
        {

            if(string1 && string2)
            {
                if(pos1==-1)
                {
                    if(DEBUG) printf("3a\n");
                    pos1=strlen(string1)+strlen(string2);
                }
                else
                {

                    if(clipboard_struct.pos==select_struct.vstart)
                    {

                        if(!which_clipboard_style())
                        {
                            pos1=pos1+strlen(string1)+strlen(string2);
                            if(DEBUG) printf("3b\n");
                        }
                        else
                        {
                            pos1=pos1+strlen(string1)+strlen(string2);
                            if(DEBUG) printf("3bb\n");
                        }

                    }
                    else
                    {
                        pos1=12; //strlen(string1)+strlen(string2);
                        if(DEBUG) printf("3bbb\n");

                    }
                }

            }

            if(which_clipboard_style() && clipboard_struct.pos > select_struct.vstart) { pos1=10;
                        if(DEBUG)  printf("3 normal changing pos1 to %d\n",pos1);                         }
            if(!which_clipboard_style() && clipboard_struct.pos > select_struct.vstart) { pos1=first_pos1+strlen(string1)+strlen(string2);
                        if(DEBUG)  printf("3 columns changing pos1 to %d\n",pos1);                         }



        }

        if(select_struct.end_column==0)
        {

            if(pos2==-2)
            {
                if(string3)
                {
                if(DEBUG) printf("4a\n");
                    pos2=strlen(string1)+strlen(string2)+strlen(string3)+select_struct.start_pos+2;
                }
                else
                {
                    pos2=strlen(string1)+strlen(string2)+select_struct.start_pos+2;
                if(DEBUG) printf("4aa\n");
                }

            }
            else
            {
                if(clipboard_struct.pos < select_struct.vend && clipboard_struct.pos > select_struct.vstart)
                {
                    pos2=10+strlen(string1)+strlen(string2)+strlen(string3)+2;

                    if(DEBUG) printf("4b\n");

                }
                else if(which_clipboard_style() && clipboard_struct.pos == select_struct.vend)
                {

                    if(DEBUG) printf("4bb\n");

                }
                else if(!which_clipboard_style() && clipboard_struct.pos == select_struct.vend)
                {

                    if(DEBUG) printf("4bbb\n");
                    pos2=pos2;
                }

                else if(clipboard_struct.pos == select_struct.vstart)
                {
                    if(DEBUG) printf("4bbbb\n");

                    pos2=10+strlen(string1)+strlen(string2)+strlen(string3)+2;

                }

                if(DEBUG) printf("changing 4 pos2 to %d\n",pos2);

            }

        }
        else if(select_struct.end_column==1)
        {

            if(pos2==-2 || clipboard_struct.pos == select_struct.vend)// && clipboard_struct.pos > select_struct.vstart)
            {

                if(pos2==-2)
                {
                    //not sure if the below works
                    pos2=10+strlen(string1)+strlen(string2)+strlen(string3)+2;
                    if(DEBUG) printf("5a\n");

                }
                else if(work_pos1==-1)
                {
                    pos2=strlen(string1)+strlen(string2)+12;
                    if(DEBUG) printf("5aa\n");

                }
                else
                {
                    pos2=9+select_struct.end_pos;
                    if(DEBUG) printf("5aaa\n");
                }
                if(DEBUG) printf("after 5 pos2:%d\n",pos2);


            }
            else if(pos2==-2 || clipboard_struct.pos < select_struct.vend)// && clipboard_struct.pos > select_struct.vstart)
            {

                if(pos2==-1)
                {

                    pos2=strlen(string1)+strlen(string2)+work_pos1+10+2;
                    if(DEBUG) printf("5b pos2:%d\n",pos2);

                }
                else
                {
                    if(work_pos1 != -1)
                    {
                        if(clipboard_struct.pos == select_struct.vstart)
                        {
                            pos2=10+strlen(string1)+strlen(string2)+strlen(string3);
                            if(DEBUG) printf("5bb pos2:%d\n",pos2);
                        }
                        else
                        {
                            pos2=pos2+pos1+2;
                            if(DEBUG) printf("5bbb pos2:%d\n",pos2);
                        }
                    }
                    else
                    {
                        pos2=10+strlen(string1)+strlen(string2)+strlen(string3)+12;
                        if(DEBUG) printf("5bbbb pos2:%d\n",pos2);

                    }

                }
            }


        }
        else if(select_struct.end_column==2)
        {

            if(pos2==-2 || clipboard_struct.pos < select_struct.vend) // && clipboard_struct.pos != select_struct.vend-1)
            {
                if(string3)
                {

                    if(which_clipboard_style() && clipboard_struct.pos == select_struct.vstart)
                    {
                        pos2=strlen(string1)+strlen(string2)+strlen(string3)+pos1+2;
                        if(DEBUG) printf("6a\n");

                    }
                    else
                    {
                        //does the below work??
                        //pos2=strlen(string1)+strlen(string2)+strlen(string3)+pos1+2;
                        if(pos2!=-2)
                        {
                            pos2=strlen(string1)+strlen(string2)+strlen(string3)+10+2;
                            //pos2=pos2+strlen(string1)+2;
                            if(DEBUG) printf("6aa\n");
                        }
                        else
                        {
                            pos2=strlen(string2)+strlen(string3)+strlen(string1)+10+2;
                            if(DEBUG) printf("6aaa\n");
                        }
                    }
                }
                else
                {
                    if(DEBUG) printf("6ab\n");
                    pos2=strlen(string1)+strlen(string2)+select_struct.start_pos+2;
                }
            }
            else
            {
                if(DEBUG) printf("6b\n");
                if(!string1 || !string2) { array[0]=(char*)""; return; }
                pos2=pos2+strlen(string1)+strlen(string2);

            }
            //its a middle entry, lets copy the whole bloody thing
            if(which_clipboard_style() && clipboard_struct.pos > select_struct.vstart && clipboard_struct.pos < select_struct.vend)
            {
                if(DEBUG) printf("6 changing normal mode pos2..\n");
                pos2=strlen(string1)+strlen(string2)+strlen(string3)+10+2; 
            }
            else if(!which_clipboard_style() && clipboard_struct.pos > select_struct.vstart && clipboard_struct.pos < select_struct.vend)
            {
                if(DEBUG) printf("6 changing column mode pos2..\n");
                pos2=strlen(string1)+strlen(string2)+strlen(string3)+10+2; 
            }

                    if(DEBUG) printf("pos2:%d\n",pos2);

        }

    }
    else
    {
        // special copy, if the end column is before the start column
        // if !(select_struct.start_column<=select_struct.end_column)

        //this is the first entry
        if(clipboard_struct.pos==select_struct.vstart)
        {

            if(select_struct.start_column==0)
            {

                if(string1)
                {
                    if(pos1==-1)
                    {
                        if(DEBUG) printf("11a\n");
                        pos1=10;
                    }
                    else
                    {
                        if(DEBUG) printf("11b\n");
                        pos1=pos1;

                    }
                }


            }
            else if(select_struct.start_column==1)
            {

                if(string1)
                {
                    if(pos1==-1)
                    {
                        if(DEBUG) printf("22a\n");
                        pos1=strlen(string1)+strlen(string2);

                    }
                    else
                    {
                        if(DEBUG) printf("22b\n");
                        pos1=pos1+strlen(string1)-1;

                    }
                }



            }
            else if(select_struct.start_column==2)
            {

                if(string1 && string2)
                {
                    if(pos1==-1)
                    {
                        if(DEBUG) printf("33a\n");
                        pos1=strlen(string1)+strlen(string2);
                    }
                    else
                    {
                        if(DEBUG) printf("33b\n");
                        pos1=pos1+strlen(string1)+strlen(string2);

                    }

                }

            }

            {

                if(string3)
                {
                    if(DEBUG) printf("9a\n");
                    pos2=strlen(string1)+strlen(string2)+strlen(string3)+select_struct.start_pos+2;

                }
                else
                {
                    if(DEBUG) printf("9b\n");
                    pos2=strlen(string1)+strlen(string2)+select_struct.start_pos+2;
                }

            }

        }
        //this is the last entry
        else if(clipboard_struct.pos==select_struct.vend)
        {

            pos1=10;

            if(pos2==-2)
            {

                if(DEBUG) printf("14a\n");
                pos2=strlen(string1)+strlen(string2)+strlen(string3)+8;

            }
            else
            {
                if(DEBUG) printf("14b\n");
                pos2=1;

            }


        }
        //else if its an entry in the middle, we'll copy the whole bloody thing!!
        else
        {
            pos1=10;
            if(DEBUG) printf("33c\n");

            if(string3)
            {
                if(DEBUG) printf("44a\n");
                pos2=strlen(string1)+strlen(string2)+strlen(string3)+10+2;

            }
            else
            {

                if(DEBUG) printf("44b\n");
                pos2=strlen(string1)+strlen(string2)+10+2;

            }
        }

    }


    if(DEBUG) printf("modified pos1:%i\n",pos1);
    if(DEBUG) printf("modified pos2:%i\n",pos2);

    if(pos2<pos1) { if(DEBUG) printf("pos2 is less than pos1\n"); pos2=pos1+1; }


    strncpy(string22,new_entry+pos1,pos2-pos1);
    string22[pos2-pos1]='\0';

    if(clipboard_struct.pos!=select_struct.vend) strcat(string22,"\n");

    strcpy(string22,convert_graphical_smilies_2_text(string22));

    if(DEBUG) printf("string22:%s\n",string22);

    array[0]=string22;
    array[1]=(char*)strlen(string22);

}

#ifdef __MORPHOS__
struct Hook Custom_Clipboard2_Hook = { { NULL,NULL }, (HOOKFUNC)HookEntry, (HOOKFUNC)Custom_Clipboard2_Func, NULL};
#elif __AROS__
struct Hook Custom_Clipboard2_Hook = { { NULL,NULL }, (IPTR(*)())Custom_Clipboard2_Func, NULL,NULL };
#else
struct Hook Custom_Clipboard2_Hook = { { NULL,NULL }, (ULONG(*)())Custom_Clipboard2_Func, NULL,NULL };
#endif



#ifdef __amigaos4__
struct list_entry * ConstructLI_TextFunc(REG(a0, struct Hook *hook),REG(a2, Object *obj),REG(a1, struct list_entry *new_entry)) {
#elif __MORPHOS__
struct list_entry * ConstructLI_TextFunc(REG(a0, struct Hook *hook),REG(a2, Object *obj),REG(a1, struct list_entry *new_entry)) {
#elif __AROS__
struct list_entry * ConstructLI_TextFunc(struct Hook *hook,Object *obj,struct list_entry *new_entry) {
#elif __GNUC__
struct list_entry * ConstructLI_TextFunc(void) {

  register Object *a2 __asm("a2");                        Object *obj = a2;
  register struct list_entry *a1 __asm("a1");             struct list_entry *new_entry = a1;
  register struct Hook *a0 __asm("a0");                   struct Hook *hook = a0;
#endif

    struct list_entry *work_entry = (struct list_entry*) AllocMem(sizeof(struct list_entry),MEMF_ANY);

    *work_entry=*new_entry;
    work_entry->hostname=NULL;
    //return new_entry;
    
    return work_entry;
}

#ifdef __MORPHOS__
struct Hook ConstructLI_TextHook = { { NULL, NULL }, (HOOKFUNC)HookEntry, (HOOKFUNC)ConstructLI_TextFunc, NULL };
#elif __AROS__
struct Hook ConstructLI_TextHook = { { NULL,NULL }, (IPTR(*)())ConstructLI_TextFunc, NULL,NULL };
#else
struct Hook ConstructLI_TextHook = { { NULL,NULL }, (ULONG(*)())ConstructLI_TextFunc, NULL,NULL };
#endif

#ifdef __amigaos4__
static void DestructLI_TextFunc(REG(a0, struct Hook *hook),REG(a2, Object *obj),REG(a1, struct list_entry *new_entry)) {
#elif __MORPHOS__
static void DestructLI_TextFunc(REG(a0, struct Hook *hook),REG(a2, Object *obj),REG(a1, struct list_entry *new_entry)) {
#elif __AROS__
static void DestructLI_TextFunc(struct Hook *hook,Object *obj,struct list_entry *new_entry) {
#elif __GNUC__
static void DestructLI_TextFunc(void)
{

 register Object *a2 __asm("a2");                        Object *obj = a2;
  register struct list_entry *a1 __asm("a1");             struct list_entry *new_entry = a1;
  register struct Hook *a0 __asm("a0");                   struct Hook *hook = a0;
#endif


    if (new_entry)
    {
        FreeMem((struct list_entry*) new_entry, sizeof(struct list_entry));
        new_entry=NULL;
    }
}

#ifdef __MORPHOS__
struct Hook DestructLI_TextHook = { { NULL, NULL }, (HOOKFUNC)HookEntry, (HOOKFUNC)DestructLI_TextFunc, NULL };
#elif __AROS__
struct Hook DestructLI_TextHook = { { NULL,NULL },(IPTR(*)())DestructLI_TextFunc, NULL,NULL };
#else
struct Hook DestructLI_TextHook = { { NULL,NULL },(ULONG(*)())DestructLI_TextFunc, NULL,NULL };
#endif


char ignore_work[500];
char *ignore_token;

  #ifdef __amigaos4__
static void Display_ignore_TextFunc(REG(a0, struct Hook *hook), REG(a2, char **array),REG(a1, char *a_entry)) {
#elif __MORPHOS__
static void Display_ignore_TextFunc(REG(a0, struct Hook *hook), REG(a2, char **array),REG(a1, char *a_entry)) {
#elif __AROS__
static void Display_ignore_TextFunc(struct Hook *hook, char **array, char *a_entry) {
  #elif __GNUC__
static void Display_ignore_TextFunc(void)
{

  register char **a2 __asm("a2");                      char **array = a2;
  register char *a1 __asm("a1");                char *a_entry = a1;
  register struct Hook *a0 __asm("a0");                struct Hook *hook = a0;
#endif


    if(a_entry)
    {
        strcpy(ignore_work,a_entry);

        ignore_token=strtok(ignore_work," ");
        if(ignore_token) array[0]=ignore_token;

        ignore_token=strtok(NULL," ");
        if(ignore_token) array[1]=ignore_token;
        else array[1]=(char*)"";

        ignore_token=strtok(NULL," ");
        if(ignore_token) array[2]=ignore_token;
        else array[2]=(char*)"";

        ignore_token=strtok(NULL," ");
        if(ignore_token) array[3]=ignore_token;
        else array[3]=(char*)"";

    }
    else
    {

        array[0]= (char*)GCS(catalog,264,"Ignore Mask");
        array[1]= (char*)GCS(catalog,265,"Text");
        array[2]= (char*)"CTCP";
        array[3]= (char*)"DCC";

    }

}


#ifdef __MORPHOS__
struct Hook Display_ignore_TextHook = { { NULL, NULL }, (HOOKFUNC)HookEntry, (HOOKFUNC)Display_ignore_TextFunc, NULL };
#elif __AROS__
struct Hook Display_ignore_TextHook = { { NULL,NULL },(IPTR(*)())Display_ignore_TextFunc, NULL,NULL };
#else
struct Hook Display_ignore_TextHook = { { NULL,NULL },(ULONG(*)())Display_ignore_TextFunc, NULL,NULL };
#endif

// channel list display

char channellist_work[1200];
char channellist_work2[150];
char channellist_work3[100];
char channellist_work4[900];
char channellist_back[900];
char *channellist_token;

#ifdef __amigaos4__
static void Display_channellist_TextFunc(REG(a0, struct Hook *hook), REG(a2, char **array),REG(a1, struct channel_entry *a_entry)) {
#elif __MORPHOS__
static void Display_channellist_TextFunc(REG(a0, struct Hook *hook), REG(a2, char **array),REG(a1, struct channel_entry *a_entry)) {
#elif __AROS__
static void Display_channellist_TextFunc(struct Hook *hook, char **array,struct channel_entry *a_entry) {
#elif __GNUC__
static void Display_channellist_TextFunc(void)
{

  register char **a2 __asm("a2");                      char **array = a2;
  register struct channel_entry *a1 __asm("a1");        struct channel_entry *a_entry = a1;
  register struct Hook *a0 __asm("a0");                struct Hook *hook = a0;
#endif


    if(a_entry)
    {
        char new_array[20];
        sprintf(new_array,"\033P[%ld]",custom_pen_colours[a_entry->colour]);

        strcpy(channellist_work,a_entry->entry);
        strtok(channellist_work,":");

        channellist_token=strtok(NULL," ");
        if(channellist_token)
        {
            sprintf(channellist_work2,"%s%s",new_array, channellist_token);
            array[0]=channellist_work2;

        }
        else array[0]=(char*)"";


        channellist_token=strtok(NULL,": ");

        if(channellist_token)
        {
            sprintf(channellist_work3,"%s%s",new_array, channellist_token);
            array[1]=channellist_work3;

        }
        else array[1]=(char*)"";

        channellist_token=strtok(NULL,"");
        if(channellist_token)
        {
            if(channellist_token[0]==':')
            {
                for(int count=0; count<(LONG)strlen(channellist_token); count++)
                {    channellist_token[count]=channellist_token[count+1]; }
            }
            sprintf(channellist_work4,"%s%s",new_array, channellist_token);
            array[2]=channellist_work4;

        }
        else array[2]=(char*)"";


    }

}


#ifdef __MORPHOS__
struct Hook Display_channellist_TextHook = { { NULL, NULL }, (HOOKFUNC)HookEntry, (HOOKFUNC)Display_channellist_TextFunc, NULL };
#elif __AROS__
struct Hook Display_channellist_TextHook = { { NULL,NULL },(IPTR(*)())Display_channellist_TextFunc, NULL,NULL };
#else
struct Hook Display_channellist_TextHook = { { NULL,NULL },(ULONG(*)())Display_channellist_TextFunc, NULL,NULL };
#endif

//alias listview

char alias_work[800];
char *alias_token;

#ifdef __amigaos4__
static void Display_alias_TextFunc(REG(a0, struct Hook *hook), REG(a2, char **array),REG(a1, char *a_entry)) {
#elif __MORPHOS__
static void Display_alias_TextFunc(REG(a0, struct Hook *hook), REG(a2, char **array),REG(a1, char *a_entry)) {
#elif __AROS__
static void Display_alias_TextFunc(struct Hook *hook,char **array,char *a_entry) {
#elif __GNUC__
static void Display_alias_TextFunc(void)
{

  register char **a2 __asm("a2");                      char **array = a2;
  register char *a1 __asm("a1");                char *a_entry = a1;
  register struct Hook *a0 __asm("a0");                struct Hook *hook = a0;
#endif


    if(a_entry)
    {
        strcpy(alias_work,a_entry);

        alias_token=strtok(alias_work," ");
        if(alias_token) array[0]=alias_token;

        alias_token=strtok(NULL,"");
        if(alias_token) array[1]=alias_token;
        else array[1]=(char*)"";


    }
    else
    {

        array[0]= (char*)GCS(catalog,280,"Alias");
        array[1]= (char*)GCS(catalog,281,"Command");

    }

}


#ifdef __MORPHOS__
struct Hook Display_alias_TextHook = { { NULL, NULL }, (HOOKFUNC)HookEntry, (HOOKFUNC)Display_alias_TextFunc, NULL };
#elif __AROS__
struct Hook Display_alias_TextHook = { { NULL,NULL },(IPTR(*)())Display_alias_TextFunc, NULL,NULL };
#else
struct Hook Display_alias_TextHook = { { NULL,NULL },(ULONG(*)())Display_alias_TextFunc, NULL,NULL };
#endif


char events_work[800];
char *events_token;

#ifdef __amigaos4__
static void Display_events_TextFunc(REG(a0, struct Hook *hook), REG(a2, char **array),REG(a1, char *a_entry)) {
#elif __MORPHOS__
static void Display_events_TextFunc(REG(a0, struct Hook *hook), REG(a2, char **array),REG(a1, char *a_entry)) {
#elif __AROS__
static void Display_events_TextFunc(struct Hook *hook,char **array,char *a_entry) {
#elif __GNUC__
static void Display_events_TextFunc(void)
{

  register char **a2 __asm("a2");                      char **array = a2;
  register char *a1 __asm("a1");                char *a_entry = a1;
  register struct Hook *a0 __asm("a0");                struct Hook *hook = a0;
#endif


    if(a_entry)
    {
        strcpy(events_work,a_entry);

        events_token=strtok(events_work,"\033");
        if(events_token) array[0]=events_token;

        events_token=strtok(NULL," ");
        if(events_token)
        {
            if(atoi(events_token)==0) array[1]=(char*)"Never";
            if(atoi(events_token)==1) array[1]=(char*)"Window Active";
            if(atoi(events_token)==2) array[1]=(char*)"Window Inactive";
            if(atoi(events_token)==3) array[1]=(char*)"Always";

        }

        events_token=strtok(NULL,"\033");
        if(events_token)
        {
            array[2]=events_token;

            events_token=strtok(NULL,"");
            if(events_token) array[3]=events_token;
            else array[3]=(char*)"";

        }
        else
        {
            array[2]=(char*)"";
            array[3]=(char*)"";

        }

        /*events_token=strtok(NULL,"\033");
        if(events_token) array[2]=events_token;
        else array[2]=(char*)"";

        events_token=strtok(NULL,"");
        if(events_token) array[3]=events_token;
        else array[3]=(char*)"";
          */

    }
    else
    {

        array[0]= (char*)GCS(catalog,800,"Event");
        array[1]= (char*)GCS(catalog,800,"Use when");
        array[2]= (char*)GCS(catalog,800,"ARexx script");
        array[3]= (char*)GCS(catalog,800,"Text to send to script");

    }

}


#ifdef __MORPHOS__
struct Hook Display_events_TextHook = { { NULL, NULL }, (HOOKFUNC)HookEntry, (HOOKFUNC)Display_events_TextFunc, NULL };
#elif __AROS__
struct Hook Display_events_TextHook = { { NULL,NULL },(IPTR(*)())Display_events_TextFunc, NULL,NULL };
#else
struct Hook Display_events_TextHook = { { NULL,NULL },(ULONG(*)())Display_events_TextFunc, NULL,NULL };
#endif


//

char servers_work[500];
char *servers_token1;
char *servers_token2;
char *servers_token3;
char *servers_token4;

#ifdef __amigaos4__
static void Display_servers_TextFunc(REG(a0, struct Hook *hook), REG(a2, char **array),REG(a1, struct MUIP_NListtree_DisplayMessage *a_entry)) {
#elif __MORPHOS__
static void Display_servers_TextFunc(REG(a0, struct Hook *hook), REG(a2, char **array),REG(a1, struct MUIP_NListtree_DisplayMessage *a_entry)) {
#elif __AROS__
static void Display_servers_TextFunc(struct Hook *hook, char **array,struct MUIP_NListtree_DisplayMessage *a_entry) {
#elif __GNUC__
static void Display_servers_TextFunc(void)
{

  register char **a2 __asm("a2");                      char **array = a2;
  register struct MUIP_NListtree_DisplayMessage  *a1 __asm("a1");      struct MUIP_NListtree_DisplayMessage *a_entry = a1;
  register struct Hook *a0 __asm("a0");                struct Hook *hook = a0;
#endif


    if(a_entry->TreeNode != NULL)
    {
        strcpy(servers_work,(char *)a_entry->TreeNode->tn_Name);

        acquire_connect_details(servers_work);

        a_entry->Array[0]=(STRPTR)server_name;
        a_entry->Array[1]=(STRPTR)port_number;
        a_entry->Array[2]=(STRPTR)auto_connect;
        if(Pro_Charsets_Enabled==TRUE)
            a_entry->Array[3]=(STRPTR)server_charset;


    }
    else
    {

        a_entry->Array[0]= (STRPTR)GCS(catalog,266,"Servers");
        a_entry->Array[1]= (STRPTR)GCS(catalog,267,"Port");
        a_entry->Array[2]= (STRPTR)GCS(catalog,268,"Auto-Connect");
        if(Pro_Charsets_Enabled==TRUE)
            a_entry->Array[3]= (STRPTR)GCS(catalog,329,"Charset");


    }

}


#ifdef __MORPHOS__
struct Hook Display_servers_TextHook = { { NULL, NULL }, (HOOKFUNC)HookEntry, (HOOKFUNC)Display_servers_TextFunc, NULL };
#elif __AROS__
struct Hook Display_servers_TextHook = { { NULL,NULL },(IPTR(*)())Display_servers_TextFunc, NULL,NULL };
#else
struct Hook Display_servers_TextHook = { { NULL,NULL },(ULONG(*)())Display_servers_TextFunc, NULL,NULL };
#endif

/* *********************************************** */

//multi column display hook for channels
char display_hook_breakup[900];
char display_hook_breakup2[900];
char display_hook_column1[900];
char display_hook_column2[900];
char display_hook_column3[900];
char *display_string1;

//display hook for ban window
#ifdef __amigaos4__
static void DisplayLI_ban_TextFunc(REG(a0, struct Hook *hook), REG(a2, char **array),REG(a1, char *a_entry)) {
#elif __MORPHOS__
static void DisplayLI_ban_TextFunc(REG(a0, struct Hook *hook), REG(a2, char **array),REG(a1, char *a_entry)) {
#elif __AROS__
static void DisplayLI_ban_TextFunc(struct Hook *hook, char **array, char *a_entry) {
#elif __GNUC__
static void DisplayLI_ban_TextFunc(void)
{

  register char **a2 __asm("a2");                       char **array = a2;
  register char *a1 __asm("a1");                        char *a_entry = a1;
  register struct Hook *a0 __asm("a0");                 struct Hook *hook = a0;
#endif

    if(a_entry)
    {

        strcpy(display_hook_breakup, a_entry);
        display_string1=strtok(display_hook_breakup," ");
        if(display_string1) strcpy(display_hook_column1, display_string1);

        display_string1=strtok(NULL," ");
        if(display_string1) strcpy(display_hook_column2, display_string1);

        display_string1=strtok(NULL,"");
        if(display_string1)
        {
            time_t rawtime=atoi(display_string1);
            struct tm *timeinfo;
            timeinfo=localtime(&rawtime);

            strcpy(display_hook_column3, asctime(timeinfo));
        }

        array[0]=display_hook_column1;
        array[1]=display_hook_column2;
        array[2]=display_hook_column3;

    }
    else
    {
        array[0]=(char*)GCS(catalog,1,"Ban mask");
        array[1]=(char*)GCS(catalog,2,"Set by");
        array[2]=(char*)GCS(catalog,3,"Date placed");

    }

}


#ifdef __MORPHOS__
struct Hook DisplayLI_ban_TextHook = { { NULL, NULL }, (HOOKFUNC)HookEntry, (HOOKFUNC)DisplayLI_ban_TextFunc, NULL };
#elif __AROS__
struct Hook DisplayLI_ban_TextHook = { { NULL,NULL },(IPTR(*)())DisplayLI_ban_TextFunc, NULL,NULL };
#else
struct Hook DisplayLI_ban_TextHook = { { NULL,NULL },(ULONG(*)())DisplayLI_ban_TextFunc, NULL,NULL };
#endif



char *userlist_work1, *userlist_work2;
char userlist_buffer[1500];


//display hook for ban window
#ifdef __amigaos4__
static void DisplayLV_userlist_buttons_TextFunc(REG(a0, struct Hook *hook), REG(a2, char **array),REG(a1, char *a_entry)) {
#elif __MORPHOS__
static void DisplayLV_userlist_buttons_TextFunc(REG(a0, struct Hook *hook), REG(a2, char **array),REG(a1, char *a_entry)) {
#elif __AROS__
static void DisplayLV_userlist_buttons_TextFunc(struct Hook *hook, char **array, char *a_entry) {
#elif __GNUC__
static void DisplayLV_userlist_buttons_TextFunc(void)
{

  register char **a2 __asm("a2");                       char **array = a2;
  register char *a1 __asm("a1");                        char *a_entry = a1;
  register struct Hook *a0 __asm("a0");                 struct Hook *hook = a0;
#endif

    if(a_entry)
    {
        strcpy(userlist_buffer,a_entry);
        userlist_work1=strtok(userlist_buffer,"\033");
        userlist_work2=strtok(NULL,"");

        if(userlist_work1)
            array[0]=(char*)userlist_work1;
        else
            array[0]=(char*)"";

        if(userlist_work2)
            array[1]=(char*)userlist_work2;
        else
            array[1]=(char*)"";
    }
    else
    {
        array[0]=(char*)GCS(catalog,800,"Name");
        array[1]=(char*)GCS(catalog,800,"Command");

    }

}


#ifdef __MORPHOS__
struct Hook DisplayHook_LV_userlist_buttons = { { NULL, NULL }, (HOOKFUNC)HookEntry, (HOOKFUNC)DisplayLV_userlist_buttons_TextFunc, NULL };
#elif __AROS__
struct Hook DisplayHook_LV_userlist_buttons = { { NULL,NULL },(IPTR(*)())DisplayLV_userlist_buttons_TextFunc, NULL,NULL };
#else
struct Hook DisplayHook_LV_userlist_buttons = { { NULL,NULL },(ULONG(*)())DisplayLV_userlist_buttons_TextFunc, NULL,NULL };
#endif

 

#ifdef __amigaos4__
static void DisplayLI2_channel_TextFunc(REG(a0, struct Hook *hook), REG(a2, char **array),REG(a1, struct channel_entry *a_entry)) {
#elif __MORPHOS__
static void DisplayLI2_channel_TextFunc(REG(a0, struct Hook *hook), REG(a2, char **array),REG(a1, struct channel_entry *a_entry)) {
#elif __AROS__
static void DisplayLI2_channel_TextFunc(struct Hook *hook, char **array, struct channel_entry *a_entry) {
#elif __GNUC__
static void DisplayLI2_channel_TextFunc(void)
{

  register char **a2 __asm("a2");                      char **array = a2;
  register struct channel_entry *a1 __asm("a1");        struct channel_entry *a_entry = a1;
  register struct Hook *a0 __asm("a0");                 struct Hook *hook = a0;
#endif

    //if(!status_conductor) return;
    //if(!status_conductor->conductor) return;
    //if(!status_current) return;
    //if(!status_current->current_query) return;
    //char new_array[512];
    
    //if(status_conductor->conductor!=status_current->current_query) return;
    //if(!array) return;
    
    if(a_entry != NULL)
    {
        
        if((int)a_entry->colour < 0) return;
        
        if(a_entry->colour)
        {

            if(a_entry->colour >= 0 && a_entry->colour <= 23)
            {
                char new_array[20];
                sprintf(new_array,"\033P[%ld]",custom_pen_colours[a_entry->colour]);

                if(strlen(new_array) == 5)
                {
                    new_array[4]=' ';
                    new_array[5]=' ';
                    new_array[6]=' ';
                    new_array[7]=' ';
                    new_array[8]=' ';
                    new_array[9]=']';
                    new_array[10]='\0';

                }
                else if(strlen(new_array) == 6)
                {
                    new_array[5]=' ';
                    new_array[6]=' ';
                    new_array[7]=' ';
                    new_array[8]=' ';
                    new_array[9]=']';
                    new_array[10]='\0';


                }
                else if(strlen(new_array) == 7)
                {
                    new_array[6]=' ';
                    new_array[7]=' ';
                    new_array[8]=' ';
                    new_array[9]=']';
                    new_array[10]='\0';

                }

                strncpy(a_entry->entry,new_array,strlen(new_array));
                //stccpy(a_entry->Display_entry,a_entry->entry,600);
                //strncpy(a_entry->Display_entry,new_array,strlen(new_array));


                //strcpy(a_entry->display_hook_breakup, a_entry->Display_entry);
                strcpy(a_entry->display_hook_breakup, a_entry->entry);

                strtok(a_entry->display_hook_breakup,"]");

                strcpy(a_entry->display_hook_column1,"");

                a_entry->display_string1=strtok(NULL," ");
                if(a_entry->display_string1)
                {
                    sprintf(a_entry->display_hook_column1,"%s%s",new_array,a_entry->display_string1);

                }
                else
                {
                    sprintf(a_entry->display_hook_column1,"%s ",new_array);

                }

                array[2]=a_entry->display_hook_column1;

                sprintf(a_entry->display_hook_column2,"\033r\%s",new_array);
                a_entry->display_string1=strtok(NULL," ");
                if(a_entry->display_string1)
                {
                    if(my_settings.remove_brackets)
                    {
                        if(a_entry->display_string1[0]=='<' && a_entry->display_string1[strlen(a_entry->display_string1)-1]=='>')
                        {
                            int count;
                            for(count=0; count<(LONG)strlen(a_entry->display_string1)-1; count++)
                                a_entry->display_string1[count]=a_entry->display_string1[count+1];
                            a_entry->display_string1[count-1]='\0';
                        }
                    }

                    strcat(a_entry->display_hook_column2,a_entry->display_string1);

                }
                array[1]=a_entry->display_hook_column2;

                sprintf(a_entry->display_hook_column3,"\033P%s",new_array);
                a_entry->display_string1=strtok(NULL,"");
                if(a_entry->display_string1) strcat(a_entry->display_hook_column3,a_entry->display_string1);
                array[0]=a_entry->display_hook_column3;

                /*
                strcpy(display_hook_breakup, a_entry->entry);

                strtok(display_hook_breakup,"]");

                strcpy(display_hook_column1,"");

                display_string1=strtok(NULL," ");
                if(display_string1)
                {
                    sprintf(display_hook_column1,"%s%s",new_array,display_string1);

                }
                else
                {
                    sprintf(display_hook_column1,"%s ",new_array);

                }

                array[2]=display_hook_column1;

                sprintf(display_hook_column2,"\033r\%s",new_array);
                display_string1=strtok(NULL," ");
                if(display_string1)
                {
                    if(my_settings.remove_brackets)
                    {
                        if(display_string1[0]=='<' && display_string1[strlen(display_string1)-1]=='>')
                        {
                            for(count=0; count<(LONG)strlen(display_string1)-1; count++)
                                display_string1[count]=display_string1[count+1];
                            display_string1[count-1]='\0';
                        }
                    }

                    strcat(display_hook_column2,display_string1);

                }
                array[1]=display_hook_column2;

                sprintf(display_hook_column3,"\033P%s",new_array);
                display_string1=strtok(NULL,"");
                if(display_string1) strcat(display_hook_column3,display_string1);
                array[0]=display_hook_column3;
                  */


            }
        }
        else
        {
            char new_array[20];
            sprintf(new_array,"\033P[%ld]",custom_pen_colours[1]);

            if(strlen(new_array) == 5)
            {
                new_array[4]=' ';
                new_array[5]=' ';
                new_array[6]=' ';
                new_array[7]=' ';
                new_array[8]=' ';
                new_array[9]=']';
                new_array[10]='\0';

            }
            else if(strlen(new_array) == 6)
            {
                new_array[5]=' ';
                new_array[6]=' ';
                new_array[7]=' ';
                new_array[8]=' ';
                new_array[9]=']';
                new_array[10]='\0';


            }
            else if(strlen(new_array) == 7)
            {
                new_array[6]=' ';
                new_array[7]=' ';
                new_array[8]=' ';
                new_array[9]=']';
                new_array[10]='\0';

            }

            strncpy(a_entry->entry,new_array,strlen(new_array));
            //stccpy(a_entry->Display_entry,a_entry->entry,600);
            //strncpy(a_entry->Display_entry,new_array,strlen(new_array));

            //array[0]=(char*)a_entry->Display_entry;

            array[0]=a_entry->entry;
            array[1]=(char*)"no_colour";
            array[2]=(char*)"";


        }

    }

}

#ifdef __MORPHOS__
struct Hook DisplayLI2_channel_TextHook = { { NULL, NULL }, (HOOKFUNC)HookEntry, (HOOKFUNC)DisplayLI2_channel_TextFunc, NULL };
#elif __AROS__
struct Hook DisplayLI2_channel_TextHook = { { NULL,NULL },(IPTR(*)())DisplayLI2_channel_TextFunc, NULL,NULL };
#else
struct Hook DisplayLI2_channel_TextHook = { { NULL,NULL },(ULONG(*)())DisplayLI2_channel_TextFunc, NULL,NULL };
#endif

//single column displayhook for channels
LONG pen_work;
char *graphical_string1;

#ifdef __amigaos4__
static void DisplayLI_channel_TextFunc(REG(a0, struct Hook *hook), REG(a2, char **array),REG(a1, struct channel_entry *a_entry)) {
#elif __MORPHOS__
static void DisplayLI_channel_TextFunc(REG(a0, struct Hook *hook), REG(a2, char **array),REG(a1, struct channel_entry *a_entry)) {
#elif __AROS__
static void DisplayLI_channel_TextFunc(struct Hook *hook, char **array, struct channel_entry *a_entry) {
#elif __GNUC__
static void DisplayLI_channel_TextFunc(void)
{

  register char **a2 __asm("a2");                      char **array = a2;
  register struct channel_entry *a1 __asm("a1");        struct channel_entry *a_entry = a1;
  register struct Hook *a0 __asm("a0");                 struct Hook *hook = a0;
#endif

    if(a_entry)
    {
        char new_array[20];
        //copy a colour code to the start of our line, this will make our black and white
        //text appear in colour. Theres a 10 character gap at the start of each Entry
        //for us to put our colour code
        if(a_entry->colour >= 0 && a_entry->colour <= 24)
            sprintf(new_array,"\033P[%ld]",(LONG)custom_pen_colours[a_entry->colour]);
        else
            sprintf(new_array,"\033P[%ld]",custom_pen_colours[1]);


        //lets pad out the blank space in our 10 character colour code
        if(strlen(new_array) == 5)
        {
            new_array[4]=' ';
            new_array[5]=' ';
            new_array[6]=' ';
            new_array[7]=' ';
            new_array[8]=' ';
            new_array[9]=']';
            new_array[10]='\0';

        }
        else if(strlen(new_array) == 6)
        {
            new_array[5]=' ';
            new_array[6]=' ';
            new_array[7]=' ';
            new_array[8]=' ';
            new_array[9]=']';
            new_array[10]='\0';


        }
        else if(strlen(new_array) == 7)
        {
            new_array[6]=' ';
            new_array[7]=' ';
            new_array[8]=' ';
            new_array[9]=']';
            new_array[10]='\0';

        }


        //put the new colour code over the start of our Entry
        stccpy(a_entry->Display_entry,a_entry->entry,600);
        strncpy(a_entry->Display_entry,new_array,strlen(new_array));

        array[0]=(char*)a_entry->Display_entry;
    }
    else array[0]=(char*)"";

}

#ifdef __MORPHOS__
struct Hook DisplayLI_channel_TextHook = { { NULL, NULL }, (HOOKFUNC)HookEntry, (HOOKFUNC)DisplayLI_channel_TextFunc, NULL };
#elif __AROS__
struct Hook DisplayLI_channel_TextHook = { { NULL,NULL },(IPTR(*)())DisplayLI_channel_TextFunc, NULL,NULL };
#else
struct Hook DisplayLI_channel_TextHook = { { NULL,NULL },(ULONG(*)())DisplayLI_channel_TextFunc, NULL,NULL };
#endif


#ifdef __amigaos4__
static void Display_listviewtabs_TextFunc(REG(a0, struct Hook *hook), REG(a2, char **array),REG(a1, struct channel_entry *a_entry)) {
#elif __MORPHOS__
static void Display_listviewtabs_TextFunc(REG(a0, struct Hook *hook), REG(a2, char **array),REG(a1, struct channel_entry *a_entry)) {
#elif __AROS__
static void Display_listviewtabs_TextFunc(struct Hook *hook, char **array, struct channel_entry *a_entry) {
#elif __GNUC__
static void Display_listviewtabs_TextFunc(void)
{

  register char **a2 __asm("a2");                      char **array = a2;
  register struct channel_entry *a1 __asm("a1");        struct channel_entry *a_entry = a1;
  register struct Hook *a0 __asm("a0");                 struct Hook *hook = a0;
#endif

    if(a_entry)
    {
        char new_array[20];

        //copy a colour code to the start of our line, this will make our black and white
        //text appear in colour. Theres a 10 character gap at the start of each Entry
        //for us to put our colour code
        if(a_entry->colour >= 0 && a_entry->colour <= 24)
            sprintf(new_array,"\033P[%ld]",(LONG)custom_pen_colours[a_entry->colour]);
        else
            sprintf(new_array,"\033P[%ld]",custom_pen_colours[1]);


        //lets pad out the blank space in our 10 character colour code
        if(strlen(new_array) == 5)
        {
            new_array[4]=' ';
            new_array[5]=' ';
            new_array[6]=' ';
            new_array[7]=' ';
            new_array[8]=' ';
            new_array[9]=']';
            new_array[10]='\0';

        }
        else if(strlen(new_array) == 6)
        {
            new_array[5]=' ';
            new_array[6]=' ';
            new_array[7]=' ';
            new_array[8]=' ';
            new_array[9]=']';
            new_array[10]='\0';


        }
        else if(strlen(new_array) == 7)
        {
            new_array[6]=' ';
            new_array[7]=' ';
            new_array[8]=' ';
            new_array[9]=']';
            new_array[10]='\0';

        }


        //put the new colour code over the start of our Entry
        stccpy(a_entry->Display_entry,a_entry->entry,600);
        strncpy(a_entry->Display_entry,new_array,strlen(new_array));

        array[0]=(char*)a_entry->Display_entry;
    }
    else array[0]=(char*)"";

}

#ifdef __MORPHOS__
struct Hook Display_listviewtabs_TextHook = { { NULL, NULL }, (HOOKFUNC)HookEntry, (HOOKFUNC)Display_listviewtabs_TextFunc, NULL };
#elif __AROS__
struct Hook Display_listviewtabs_TextHook = { { NULL,NULL },(IPTR(*)())Display_listviewtabs_TextFunc, NULL,NULL };
#else
struct Hook Display_listviewtabs_TextHook = { { NULL,NULL },(ULONG(*)())Display_listviewtabs_TextFunc, NULL,NULL };
#endif


char tabs_display_hook[100];
char tabs_display_hook2[100];
char tabs_display_hook3[100];
char *tabs_string1, *tabs_string2, *tabs_token;

#ifdef __amigaos4__
static void DisplayLI_tabs_TextFunc(REG(a0, struct Hook *hook), REG(a2, char **array),REG(a1, struct channel_entry *a_entry)) {
#elif __MORPHOS__
static void DisplayLI_tabs_TextFunc(REG(a0, struct Hook *hook), REG(a2, char **array),REG(a1, struct channel_entry *a_entry)) {
#elif __AROS__
static void DisplayLI_tabs_TextFunc(struct Hook *hook, char **array, struct channel_entry *a_entry) {
#elif __GNUC__
static void DisplayLI_tabs_TextFunc(void)
{

  register char **a2 __asm("a2");                      char **array = a2;
  register struct channel_entry *a1 __asm("a1");        struct channel_entry *a_entry = a1;
  register struct Hook *a0 __asm("a0");                 struct Hook *hook = a0;
#endif

    if(a_entry)
    {
        char new_array[20];
        if(a_entry->colour >= 0 && a_entry->colour <= 24)
            sprintf(new_array,"\033P[%ld]",(LONG)custom_pen_colours[a_entry->colour]);
        else
            sprintf(new_array,"\033P[%ld]",custom_pen_colours[1]);

        if(strlen(new_array) == 5)
        {
            new_array[4]=' ';
            new_array[5]=' ';
            new_array[6]=' ';
            new_array[7]=' ';
            new_array[8]=' ';
            new_array[9]=']';
            new_array[10]='\0';

        }
        else if(strlen(new_array) == 6)
        {
            new_array[5]=' ';
            new_array[6]=' ';
            new_array[7]=' ';
            new_array[8]=' ';
            new_array[9]=']';
            new_array[10]='\0';


        }
        else if(strlen(new_array) == 7)
        {
            new_array[6]=' ';
            new_array[7]=' ';
            new_array[8]=' ';
            new_array[9]=']';
            new_array[10]='\0';

        }

        strcpy(tabs_display_hook,a_entry->entry);
        strncpy(tabs_display_hook,new_array,strlen(new_array));

        strtok(tabs_display_hook,"]");
        tabs_string1=strtok(NULL," ");
        tabs_string2=strtok(NULL,"");

        strcpy(tabs_display_hook2,"         ]");
        strcat(tabs_display_hook2,tabs_string1);
        strncpy(tabs_display_hook2,new_array,strlen(new_array));

        if(tabs_string2)
        {
            strcpy(tabs_display_hook3,"         ]");
            strcat(tabs_display_hook3,tabs_string2);
            strncpy(tabs_display_hook3,new_array,strlen(new_array));
        }

        array[0]=tabs_display_hook2; //tabs_string1;
        if(tabs_string2) array[1]=tabs_display_hook3;
        else array[1]=(char*)"";

    }

}

#ifdef __MORPHOS__
struct Hook DisplayLI_tabs_TextHook = { { NULL, NULL }, (HOOKFUNC)HookEntry, (HOOKFUNC)DisplayLI_tabs_TextFunc, NULL };
#elif __AROS__
struct Hook DisplayLI_tabs_TextHook = { { NULL,NULL },(IPTR(*)())DisplayLI_tabs_TextFunc, NULL,NULL };
#else
struct Hook DisplayLI_tabs_TextHook = { { NULL,NULL },(ULONG(*)())DisplayLI_tabs_TextFunc, NULL,NULL };
#endif

// CONSTRUCTER FOR CHANNEL LISTVIEWS

#ifdef __amigaos4__
struct channel_entry * ConstructLI_channel2_TextFunc(REG(a0, struct Hook *hook),REG(a2, Object *obj),REG(a1, struct channel_entry *anew_entry)) {
#elif __MORPHOS__
struct channel_entry * ConstructLI_channel2_TextFunc(REG(a0, struct Hook *hook),REG(a2, Object *obj),REG(a1, struct channel_entry *anew_entry)) {
#elif __AROS__
struct channel_entry * ConstructLI_channel2_TextFunc(struct Hook *hook, Object *obj, struct channel_entry *anew_entry) {
#elif __GNUC__
struct channel_entry * ConstructLI_channel2_TextFunc(void) {

  register Object *a2 __asm("a2");                        Object *obj = a2;
  register struct channel_entry *a1 __asm("a1");             struct channel_entry *anew_entry = a1;
  register struct Hook *a0 __asm("a0");                   struct Hook *hook = a0;
#endif

    struct channel_entry *work_entry = (struct channel_entry*) AllocMem(sizeof(struct channel_entry),MEMF_ANY);
    *work_entry=*anew_entry;
    return work_entry;

}


#ifdef __MORPHOS__
struct Hook ConstructLI_channel2_TextHook = { { NULL, NULL }, (HOOKFUNC)HookEntry, (HOOKFUNC)ConstructLI_channel2_TextFunc, NULL };
#elif __AROS__
struct Hook ConstructLI_channel2_TextHook = { { NULL,NULL }, (IPTR(*)())ConstructLI_channel2_TextFunc, NULL,NULL };
#else
struct Hook ConstructLI_channel2_TextHook = { { NULL,NULL }, (ULONG(*)())ConstructLI_channel2_TextFunc, NULL,NULL };
#endif

//DESTRUCTER FOR CHANNEL LISTVIEWS

#ifdef __amigaos4__
static void DestructLI_channel2_TextFunc(REG(a0, struct Hook *hook),REG(a2, Object *obj),REG(a1, struct channel_entry *new_entry)) {
#elif __MORPHOS__
static void DestructLI_channel2_TextFunc(REG(a0, struct Hook *hook),REG(a2, Object *obj),REG(a1, struct channel_entry *new_entry)) {
#elif __AROS__
static void DestructLI_channel2_TextFunc(struct Hook *hook, Object *obj, struct channel_entry *new_entry) {
#elif __GNUC__
static void DestructLI_channel2_TextFunc(void)
{

 register Object *a2 __asm("a2");                        Object *obj = a2;
  register struct channel_entry *a1 __asm("a1");             struct channel_entry *new_entry = a1;
  register struct Hook *a0 __asm("a0");                   struct Hook *hook = a0;
#endif

  if (new_entry)
  {
        FreeMem((struct channel_entry*) new_entry, sizeof(new_entry));
        new_entry=NULL;

    }
}

#ifdef __MORPHOS__
struct Hook DestructLI_channel2_TextHook = { { NULL, NULL }, (HOOKFUNC)HookEntry, (HOOKFUNC)DestructLI_channel2_TextFunc, NULL };
#elif __AROS__
struct Hook DestructLI_channel2_TextHook = { { NULL,NULL },(IPTR(*)())DestructLI_channel2_TextFunc, NULL,NULL };
#else
struct Hook DestructLI_channel2_TextHook = { { NULL,NULL },(ULONG(*)())DestructLI_channel2_TextFunc, NULL,NULL };
#endif


// CONSTRUCTER FOR CHANNEL LISTVIEWS

#ifdef __amigaos4__
struct channel_entry * ConstructLI_channel_TextFunc(REG(a0, struct Hook *hook),REG(a2, Object *obj),REG(a1, struct channel_entry *anew_entry)) {
#elif __MORPHOS__
struct channel_entry * ConstructLI_channel_TextFunc(REG(a0, struct Hook *hook),REG(a2, Object *obj),REG(a1, struct channel_entry *anew_entry)) {
#elif __AROS__
struct channel_entry * ConstructLI_channel_TextFunc(struct Hook *hook, Object *obj, struct channel_entry *anew_entry) {
#elif __GNUC__
struct channel_entry * ConstructLI_channel_TextFunc(void) {

  register Object *a2 __asm("a2");                        Object *obj = a2;
  register struct channel_entry *a1 __asm("a1");             struct channel_entry *anew_entry = a1;
  register struct Hook *a0 __asm("a0");                   struct Hook *hook = a0;
#endif

    struct channel_entry *work_entry = (struct channel_entry*) AllocMem(sizeof(struct channel_entry),MEMF_ANY);
    *work_entry=*anew_entry;
    return work_entry;

}


#ifdef __MORPHOS__
struct Hook ConstructLI_channel_TextHook = { { NULL, NULL }, (HOOKFUNC)HookEntry, (HOOKFUNC)ConstructLI_channel_TextFunc, NULL };
#elif __AROS__
struct Hook ConstructLI_channel_TextHook = { { NULL,NULL }, (IPTR(*)())ConstructLI_channel_TextFunc, NULL,NULL };
#else
struct Hook ConstructLI_channel_TextHook = { { NULL,NULL }, (ULONG(*)())ConstructLI_channel_TextFunc, NULL,NULL };
#endif

//DESTRUCTER FOR CHANNEL LISTVIEWS

#ifdef __amigaos4__
static void DestructLI_channel_TextFunc(REG(a0, struct Hook *hook),REG(a2, Object *obj),REG(a1, struct channel_entry *new_entry)) {
#elif __MORPHOS__
static void DestructLI_channel_TextFunc(REG(a0, struct Hook *hook),REG(a2, Object *obj),REG(a1, struct channel_entry *new_entry)) {
#elif __AROS__
static void DestructLI_channel_TextFunc(struct Hook *hook, Object *obj, struct channel_entry *new_entry) {
#elif __GNUC__
static void DestructLI_channel_TextFunc(void)
{

 register Object *a2 __asm("a2");                        Object *obj = a2;
  register struct channel_entry *a1 __asm("a1");             struct channel_entry *new_entry = a1;
  register struct Hook *a0 __asm("a0");                   struct Hook *hook = a0;
#endif

  if (new_entry)
  {
        FreeMem((struct channel_entry*) new_entry, sizeof(new_entry));
        new_entry=NULL;

    }
}


#ifdef __MORPHOS__
struct Hook DestructLI_channel_TextHook = { { NULL, NULL }, (HOOKFUNC)HookEntry, (HOOKFUNC)DestructLI_channel_TextFunc, NULL };
#elif __AROS__
struct Hook DestructLI_channel_TextHook = { { NULL,NULL },(IPTR(*)())DestructLI_channel_TextFunc, NULL,NULL };
#else
struct Hook DestructLI_channel_TextHook = { { NULL,NULL },(ULONG(*)())DestructLI_channel_TextFunc, NULL,NULL };
#endif
/* *********************************************** */

char mode_gfx[7];

#ifdef __amigaos4__
static void previewdisplay_TextFunc(REG(a0, struct Hook *hook), REG(a2, char **array),REG(a1, struct channel_entry  *a_entry)) {
#elif __MORPHOS__
static void previewdisplay_TextFunc(REG(a0, struct Hook *hook), REG(a2, char **array),REG(a1, struct channel_entry  *a_entry)) {
#elif __AROS__
static void previewdisplay_TextFunc(struct Hook *hook, char **array, struct channel_entry *a_entry) {
#elif __GNUC__
static void previewdisplay_TextFunc(void)
{

  register char **a2 __asm("a2");                      char **array = a2;
  register struct channel_entry  *a1 __asm("a1");                       struct channel_entry  *a_entry = a1;
  register struct Hook *a0 __asm("a0");                 struct Hook *hook = a0;
#endif

    if(a_entry)
    {
        /*if(a_entry->colour >= 0 && a_entry->colour <= 24)
            sprintf(new_array3,"\033P[%ld]",(LONG)custom_pen_colours[a_entry->colour]);
        else*/
            sprintf(new_array3,"\033P[%ld]",custom_pen_colours[8]);

        strncpy(a_entry->entry,new_array3,strlen(new_array3));

        array[0]=a_entry->entry;

    }

}

#ifdef __MORPHOS__
struct Hook previewdisplay_TextHook = { { NULL, NULL }, (HOOKFUNC)HookEntry, (HOOKFUNC)previewdisplay_TextFunc, NULL };
#elif __AROS__
struct Hook previewdisplay_TextHook = { { NULL,NULL },(IPTR(*)())previewdisplay_TextFunc, NULL,NULL };
#else
struct Hook previewdisplay_TextHook = { { NULL,NULL },(ULONG(*)())previewdisplay_TextFunc, NULL,NULL };
#endif


#ifdef __amigaos4__
static void DisplayLI_TextFunc(REG(a0, struct Hook *hook), REG(a2, char **array),REG(a1, struct list_entry *a_entry)) {
#elif __MORPHOS__
static void DisplayLI_TextFunc(REG(a0, struct Hook *hook), REG(a2, char **array),REG(a1, struct list_entry *a_entry)) {
#elif __AROS__
static void DisplayLI_TextFunc(struct Hook *hook, char **array, struct list_entry *a_entry) {
#elif __GNUC__
static void DisplayLI_TextFunc(void)
{

  register char **a2 __asm("a2");                      char **array = a2;
  register struct list_entry *a1 __asm("a1");             struct list_entry *a_entry = a1;
  register struct Hook *a0 __asm("a0");                 struct Hook *hook = a0;
#endif

    if(a_entry)
    {
        if(my_settings.graphical_nicklist)
        {

            if(a_entry->modes[0]=='@')
                strcpy(mode_gfx,"\033o[0]");

            else if(a_entry->modes[0]=='%')
                strcpy(mode_gfx,"\033o[1]");

            else if(a_entry->modes[0]=='+')
                strcpy(mode_gfx,"\033o[2]");
            else strcpy(mode_gfx,a_entry->modes);

            /*if(a_entry->modes[0]=='@')
                strcpy(mode_gfx,"\033o[0]");

            else if(a_entry->modes[0]=='%')
                strcpy(mode_gfx,"\033o[1]");

            else if(a_entry->modes[0]=='+')
                strcpy(mode_gfx,"\033o[2]");
            else strcpy(mode_gfx,a_entry->modes);
            */
        }
        else strcpy(mode_gfx,a_entry->modes);
        array[0] = mode_gfx; //a_entry->modes;
        array[1] = a_entry->name;
    }

}

#ifdef __MORPHOS__
struct Hook DisplayLI_TextHook = { { NULL, NULL }, (HOOKFUNC)HookEntry, (HOOKFUNC)DisplayLI_TextFunc, NULL };
#elif __AROS__
struct Hook DisplayLI_TextHook = { { NULL,NULL },(IPTR(*)())DisplayLI_TextFunc, NULL,NULL };
#else
struct Hook DisplayLI_TextHook = { { NULL,NULL },(ULONG(*)())DisplayLI_TextFunc, NULL,NULL };
#endif


/* *********************************************** */

#ifdef __amigaos4__
static void DisplayTREE_TextFunc(  REG(a0, struct IClass *cl),  REG(a2, char **array),  REG(a1, struct MUIP_NListtree_DisplayMessage *a_entry)) {
#elif __MORPHOS__
static void DisplayTREE_TextFunc(  REG(a0, struct IClass *cl),  REG(a2, char **array),  REG(a1, struct MUIP_NListtree_DisplayMessage *a_entry)) {
#elif __AROS__
static void DisplayTREE_TextFunc(  struct IClass *cl, char **array, struct MUIP_NListtree_DisplayMessage *a_entry) {
#elif __GNUC__
static void DisplayTREE_TextFunc(void)
{

  register char **a2 __asm("a2");                      char **array = a2;
  register MUIP_NListtree_DisplayMessage *a1 __asm("a1");    struct MUIP_NListtree_DisplayMessage *a_entry = a1;
  register struct Hook *a0 __asm("a0");                 struct Hook *hook = a0;
  #endif

    if(a_entry)
    {
        char *work;

        work=strtok((char *)a_entry->Array[0]," ");
        work=strtok(NULL," ");
        strcpy((char *)a_entry->Array[0],work);

    }

}


#ifdef __MORPHOS__
struct Hook DisplayTREE_TextHook = { { NULL, NULL }, (HOOKFUNC)HookEntry, (HOOKFUNC)DisplayTREE_TextFunc, NULL };
#elif __AROS__
struct Hook DisplayTREE_TextHook = { { NULL,NULL },(IPTR(*)())DisplayTREE_TextFunc, NULL,NULL };
#else
struct Hook DisplayTREE_TextHook = { { NULL,NULL },(ULONG(*)())DisplayTREE_TextFunc, NULL,NULL };
#endif
