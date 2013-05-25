/*
    The contents of this file are subject to the AROS Public License Version 1.1 
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis, 
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
    for the specific language governing rights and limitations under the License.
*/

#include <stdio.h>
#include <string.h>

char work_buffer2[800];
char work_buffer[800];


int main(void)
{

strcpy(work_buffer,"kick %s get out!\n");

                        for(int count=0,count2=0; count<strlen(work_buffer); count++, count2++)
                        {
                            //work_buffer2[count2]='\0';

                            if(work_buffer[count]=='%')
                            {
                                count++;

                                if(work_buffer[count]=='%')
                                {
                                    work_buffer2[count2]='%';
                                }
                                else if(work_buffer[count]=='s') //selected nick
                                {
                                    strcat(work_buffer2,"JAHC");
                                    count2=strlen(work_buffer2)-1;
                                }
                                else if(work_buffer[count]=='c') //current channel
                                {
                                    strcat(work_buffer2,"CHANNEL");
                                    count2=strlen(work_buffer2)-1;
                                }
                                else if(work_buffer[count]=='n') //own nick nick
                                {
                                    strcat(work_buffer2,"ME");
                                    count2=strlen(work_buffer2)-1;
                                }
                            /*
                                else if(work_buffer[count]=='s') //selected nick
                                {
                                    strcat(work_buffer2,"test");
                                    count2=strlen(work_buffer2)-1;
                                }*/
                            }
                            else
                            {
                                work_buffer2[count2]=work_buffer[count];

                            }
                                work_buffer2[count2+1]='\0';

                        }

                        printf("sending command:%s\n",work_buffer2);

                return 0;
    }
