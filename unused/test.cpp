/*
    The contents of this file are subject to the AROS Public License Version 1.1 
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis, 
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
    for the specific language governing rights and limitations under the License.
*/

#include <iostream.h>

struct test
{
    char *str;
    char *buffer;

} *status_conductor;

int main(void)
{
    status_conductor=new test;
    status_conductor->str=new char[8000];
    status_conductor->buffer=new char[BUFFERSIZE];

    printf("deleting\n");
    delete [] status_conductor->buffer;
    printf("buffer deleted\n");
    delete [] status_conductor->str;
    printf("string deleted\n");

    return;

}
