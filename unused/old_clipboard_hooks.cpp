/*
    The contents of this file are subject to the AROS Public License Version 1.1 
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis, 
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
    for the specific language governing rights and limitations under the License.
*/

/*
#ifdef __amigaos4__
static void Custom_Clipboard_Func(REG(a0, struct Hook *hook),REG(a2, char **array),REG(a1, char *new_entry)) {
//char ** Custom_Clipboard_Func(REG(a0, struct Hook *hook),REG(a2, char **array),REG(a1, char *new_entry)) {
#elif __GNUC__
static void Custom_Clipboard_Func(void) {

  register char **a2 __asm("a2");                         char **array = a2;
  //register struct NList_CopyColumnToClipMessage *a1 __asm("a1");  struct NList_CopyColumnToClipMessage *new_entry = a1;
  register char *a1 __asm("a1");                         char *new_entry = a1;
  register struct Hook *a0 __asm("a0");                   struct Hook *hook = a0;
#endif

    DoMethod((Object*)status_current->current_query->LV_channel,MUIM_NList_GetEntry,array[-1], &buffer3);

    if(array[0]) printf("array[0]:%s\n",array[0]);
    if(new_entry) printf("new_entry:%s\n",new_entry);
    if(array) printf("array[-1]:%i\narray[2]:%i\narray[4]:%i\n",array[-1],array[2],array[4]);
    if(array) printf("array[1]:%i\narray[3]:%i\n",array[1],array[3]);

    //if(array) printf("array[-1]:%i\narray[0]:%i\narray[1]:%i\narray[2]:%i\narray[3]:%i\narray[4]:%i\n",array[-1],array[0],array[1],array[2],array[3],array[4]);

    char **new_array;
    new_array[0]=new char[1000];
    new_array[1]=new char[10];

    int a=(int)array[2],b=0,c=(int)array[4],e=(int)array[1],f=(int)array[3],count=0;


    //if(e != f) return;

    //LONG array2=DoMethod((Object*)status_current->current_query->LV_channel,MUIM_NList_ColumnToCol,array[-1]);
    //printf("converted array[-1]: %i\n",array2);

    if(c==-2 && a==-1)
    {
        for(count=0; count<strlen(array[0])+a; count++) new_array[0][count]=array[0][count+a];
        new_array[0][count]='\0';
        array[0]=new_array[0];

    }
    else if(c==-2 && a!=-1)
    {
        for(count=0; count<strlen(array[0]); count++)
                            new_array[0][count]=array[0][count+a];
        new_array[0][count]='\0';
        array[0]=new_array[0];

        //strcat(array[0],new_array[0]);

    }
    else if(a==-1)
    {
        a=11;
        for(count=0; count<c; count++) new_array[0][count]=array[0][count+a];
        new_array[0][count]='\0';
        array[0]=new_array[0];

    }

    //array[0]=new_array[0];


    /*if(e != f)
    {

        if(e == -2 && f == -1) return;

        if(a != -1) last_a = a;
        if(c != -2) last_c = c;
        return;
    }

    a=last_a;
    c=last_c;

    if(a<10) a=10;
    if(c<1) c=strlen(new_entry);*/


    /*if(a==-1 && c==-2) return;

    if(a==-1) { a=10; c=last_c; }

    last_a=a;

    if(c==-2)
    {
        a=last_a;

    }*/



    //if(a<10) { a=a+10; c=c+10; }

    /*printf("a=%i, c=%i\n",a,c);

    while(a<c)
    {
        new_array[0][b]=new_entry[a];
         a++; b++;
    }

    new_array[0][b]='\0';*/



    //printf("new_array[0][b]:%c\n",new_array[0][b]);
    //printf("new_array[0][b-1]:%c\n",new_array[0][b-1]);
    //printf("new_array[0][b-2]:%c\n",new_array[0][b-2]);

    //sprintf(new_array[1],"%i",strlen(new_array[0]));

    //if(new_array[0][b-1] == '±') new_array[0][b-1] = '\n';

    /*a=0;

    while(a<b)
    {
        if(new_array[0][a] == '±') { printf("found a return\n"); new_array[0][a] = '\n'; }
        a++;

    }*/

    /*for(int a=0; a<strlen(array[0]); a++)
    {
        if(array[0][a]=='±') array[0][a]='\n';

    }*/

    //strcat(array[0],new_array[0]);

    //array[0]=new_array[0];
    //array[1]="5"; //new_array[1];

    //if(new_entry) printf("new_entry:%s\n",new_entry);
    //if(new_array) printf("new_array[0]:%s\n",new_array[0]);




}*/

    DoMethod((Object*)status_current->current_query->LV_channel,MUIM_NList_GetEntry,array[-1], &buffer3);
    if(!buffer3) return;

    if(new_entry) printf("new_entry:%s\n",new_entry);
    //if(array) printf("array[-1]:%i\narray[1]:%i\narray[2]:%i\n",array[-1],array[1],array[2]);
    //if(array) printf("array[-1]:%i\narray[0]:%i\narray[1]:%i\narray[2]:%i\narray[3]:%i\narray[4]:%i\n",array[-1],array[0],array[1],array[2],array[3],array[4]);

    char **new_array;
    new_array[0]=new char[500];
    new_array[1]=new char[10];

    int a=(int)array[1],b=0,c=(int)array[2];
    printf("a=%i, c=%i\n",a,c);

    while(a<c)
    {
        new_array[0][b]=new_entry[a];
         a++; b++;
    }

    new_array[0][b]='\0';


    //printf("new_array[0][b]:%c\n",new_array[0][b]);
    printf("new_array[0][b-1]:%c\n",new_array[0][b-1]);
    printf("new_array[0][b-2]:%c\n",new_array[0][b-2]);

    sprintf(new_array[1],"%i",strlen(new_array[0]));


    //if(new_array[0][b-1] != '±')
    /*if(1)
    {
        printf("line ends with terminator\n");

        while(1)
        {
            if(new_array[0][b] == '\n') { printf("found a return\n"); new_array[0][b] = ' '; }

            if(b==0) break;
            b=b-1;

        }

    }*/


    array[0]=new_array[0];
    //array[0]="testing";
    array[1]="5"; //new_array[1];

    //if(new_entry) printf("new_entry:%s\n",new_entry);
    if(new_array) printf("new_array[0]:%s\n",new_array[0]);

    //return new_array;


