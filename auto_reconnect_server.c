/*
    The contents of this file are subject to the AROS Public License Version 1.1 
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis, 
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
    for the specific language governing rights and limitations under the License.
*/

//this function is called when my waitselect loop detects unwanted disconnection..
//wookie then waits the user specified amount of time, then attempts to reconnect.
//in main() is some code which waits for this timer signal. if it hasnt finished
//reconnecting by that time, it will send another timer signal.

#include "intern.h"

void automatically_reconnect_server(int action)
{

    //status_conductor->trying_to_reconnect=TRUE;

    get_sys_time(systime); // Get current system time

    if (action == START_DELAY)
    {

#ifdef __amigaos4__
        systime_reconnect_timer->tv_sec = systime->tv_sec+my_settings.Reconnect_Delay;
        systime_reconnect_timer->tv_usec = systime->tv_usec;
#else
        systime_reconnect_timer->tv_secs = systime->tv_secs + my_settings.Reconnect_Delay;
        systime_reconnect_timer->tv_micro = systime->tv_micro;
#endif

    }
    else if (action == RECONNECT_STRAIGHT_AWAY)
    {

#ifdef __amigaos4__
        systime_reconnect_timer->tv_sec = systime->tv_sec;
        systime_reconnect_timer->tv_usec = systime->tv_usec;
#else
        systime_reconnect_timer->tv_secs = systime->tv_secs;
        systime_reconnect_timer->tv_micro = systime->tv_micro;
#endif

    }

}

