/*
    The contents of this file are subject to the AROS Public License Version 1.1 
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis, 
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License 
    for the specific language governing rights and limitations under the License.
*/

#include "includes.h"

#include <clib/alib_protos.h>

#include "intern.h"

ULONG timer_signal;

char timestamp[12];
char timestamp_hrs[4];
char timestamp_mins[4];
char pingtimestamp_mins[4];
char pingtimestamp_secs[4];

/* Locals */
static BOOL Timer1_WaitCheck;
static BOOL Timer2_WaitCheck;
static BOOL Timer4_WaitCheck;
static BOOL Timer5_WaitCheck;

static struct MsgPort *TimerMP;
static struct timerequest *TimerIO;
// second timer device variables for the 2 second delays
static struct MsgPort *Timer2MP;
static struct timerequest *Timer2IO;
//check if its midnight yet, if it is then inform the user in all open tabs
struct MsgPort *Timer4MP;
struct timerequest *Timer4IO;
// signal us when its time to send another line of our paste
struct MsgPort *Timer5MP;
struct timerequest *Timer5IO;

BOOL TimerWait_Close(void);

BOOL TimerWait_Open(void)
{
    LONG error;
    Timer1_WaitCheck = FALSE;
    Timer2_WaitCheck = FALSE;
    Timer4_WaitCheck = FALSE;
    Timer5_WaitCheck = FALSE;

    Timer1_WaitCheck = FALSE;
    if ((TimerMP = CreatePort(0, 0)))
    {
        Timer2_WaitCheck = FALSE;

        if ((Timer2MP = CreatePort(0, 0)))
        {
            Timer4_WaitCheck = FALSE;

            if ((Timer4MP = CreatePort(0, 0)))
            {
                Timer5_WaitCheck = FALSE;

                if ((Timer5MP = CreatePort(0, 0)))
                {
                    if ((Timer5IO = (struct timerequest *) CreateIORequest(Timer5MP, sizeof(struct timerequest))))
                    {
                        if (!(error = OpenDevice((_ub_cs) TIMERNAME, UNIT_WAITUNTIL, (struct IORequest *) Timer5IO, 0L)))
                        {
                            Timer5_WaitCheck = TRUE;

                            Timer5IO->tr_node.io_Message.mn_Node.ln_Type = NT_REPLYMSG;

                            if ((Timer4IO = (struct timerequest *) CreateIORequest(Timer4MP, sizeof(struct timerequest))))
                            {

                                if (!(error = OpenDevice((_ub_cs) TIMERNAME, UNIT_WAITUNTIL,
                                        (struct IORequest *) Timer4IO, 0L)))
                                {
                                    Timer4_WaitCheck = TRUE;

                                    Timer4IO->tr_node.io_Message.mn_Node.ln_Type = NT_REPLYMSG;
                                    Timer4IO->tr_node.io_Command = TR_ADDREQUEST;

                                    if ((Timer2IO = (struct timerequest *) CreateIORequest(Timer2MP,
                                            sizeof(struct timerequest))) != NULL)
                                    {
                                        if (!(error = OpenDevice((_ub_cs) TIMERNAME, UNIT_WAITUNTIL,
                                                (struct IORequest *) Timer2IO, 0L)))
                                        {
                                            Timer2_WaitCheck = TRUE;
                                            timer_signal = 1L << Timer2MP->mp_SigBit;

                                            Timer2IO->tr_node.io_Message.mn_Node.ln_Type = NT_REPLYMSG;
                                            Timer2IO->tr_node.io_Command = TR_ADDREQUEST;

                                            if ((TimerIO = (struct timerequest *) CreateIORequest(TimerMP,
                                                    sizeof(struct timerequest))) != NULL)
                                            {
                                                if (!(error = OpenDevice((_ub_cs) TIMERNAME, UNIT_VBLANK,
                                                        (struct IORequest *) TimerIO, 0L)))
                                                {
                                                    Timer1_WaitCheck = TRUE;
                                                    TimerIO->tr_node.io_Message.mn_Node.ln_Type = NT_REPLYMSG;

                                                    TimerIO->tr_node.io_Command = TR_GETSYSTIME;
                                                }
                                            }

                                        }
                                    }
                                }
                            }
                        }
                    }
                }

            }
        }

    }

    if (Timer2_WaitCheck)
    {
        struct timeval systime;
        get_sys_time(&systime); // Get current system time
#ifdef __amigaos4__
        Timer2IO->tr_time.tv_sec = systime.tv_sec+2;
        Timer2IO->tr_time.tv_usec = systime.tv_usec;
#else
        Timer2IO->tr_time.tv_secs = systime.tv_secs + 2;
        Timer2IO->tr_time.tv_micro = systime.tv_micro;
#endif
        SendIO((struct IORequest *) Timer2IO); // Get the results
    }

    if (Timer4_WaitCheck)
    {
        struct timeval systime;
        get_sys_time(&systime); // Get current system time
#ifdef __amigaos4__
        Amiga2Date(systime.tv_sec, clockdata);
#else
        Amiga2Date(systime.tv_secs, clockdata);
#endif
        timestamp_2_string();
        clockdata2->sec = 0;
        clockdata2->min = 59;
        clockdata2->hour = 23;
        clockdata2->mday = clockdata->mday;
        clockdata2->month = clockdata->month;
        clockdata2->year = clockdata->year;
        clockdata2->wday = clockdata->wday;
#ifdef __amigaos4__
        Timer4IO->tr_time.tv_sec = Date2Amiga(clockdata2)+60;
        Timer4IO->tr_time.tv_usec = 100;
#else
        Timer4IO->tr_time.tv_secs = Date2Amiga(clockdata2) + 60;
        Timer4IO->tr_time.tv_micro = 100;
#endif
        SendIO((struct IORequest *) Timer4IO); // Get the results
    }

    if (!Timer1_WaitCheck)
    { /*TimerWait_Close();*/
        return FALSE;
    }
    else
        return TRUE;

}

BOOL TimerWait_Close(void)
{
    if (Timer2_WaitCheck)
    {
        if (Timer2IO)
        {
            if (!CheckIO((struct IORequest *) Timer2IO))
            {
                AbortIO((struct IORequest *) Timer2IO);
                WaitIO((struct IORequest *) Timer2IO);
            }
        }

        if (Timer2IO)
        {
            CloseDevice((struct IORequest *) Timer2IO);
            DeleteIORequest((struct IORequest*) Timer2IO);
        }
        if (Timer2MP)
            DeletePort(Timer2MP);

        if (Timer1_WaitCheck)
        {
            if (TimerIO)
            {
                if (!CheckIO((struct IORequest *) TimerIO))
                {
                    AbortIO((struct IORequest *) TimerIO);
                    WaitIO((struct IORequest *) TimerIO);
                }
            }

            if (TimerIO)
            {
                CloseDevice((struct IORequest *) TimerIO);
                DeleteIORequest((struct IORequest*) TimerIO);
            }
            //while(GetMsg(TimerMP));
            if (TimerMP)
                DeletePort(TimerMP);

            if (Timer4_WaitCheck)
            {
                if (!CheckIO((struct IORequest *) Timer4IO))
                {
                    AbortIO((struct IORequest *) Timer4IO);
                    WaitIO((struct IORequest *) Timer4IO);
                }

                CloseDevice((struct IORequest *) Timer4IO);
                DeleteIORequest((struct IORequest*) Timer4IO);

                //while(GetMsg(Timer4MP));
                if (Timer4MP)
                    DeletePort(Timer4MP);

                if (Timer5_WaitCheck)
                {
                    if (!CheckIO((struct IORequest *) Timer5IO))
                    {
                        AbortIO((struct IORequest *) Timer5IO);
                        WaitIO((struct IORequest *) Timer5IO);
                    }

                    CloseDevice((struct IORequest *) Timer5IO);
                    DeleteIORequest((struct IORequest*) Timer5IO);

                    //while(GetMsg(Timer5MP));
                    if (Timer5MP)
                        DeletePort(Timer5MP);

                }

            }
        }
    }
    return TRUE;
}

void timestamp_2_string()
{
    ULONG mics, secs, mins, hrs, days;
    char timestamp_secs[4];
    DoIO((struct IORequest *) TimerIO); // Get the results
#ifdef __amigaos4__
            mics=TimerIO->tr_time.tv_usec; secs=TimerIO->tr_time.tv_sec;
#else
    mics = TimerIO->tr_time.tv_micro;
    secs = TimerIO->tr_time.tv_secs;
#endif
    mins = secs / 60;
    hrs = mins / 60;
    days = hrs / 24;
    secs = secs % 60;
    mins = mins % 60;
    hrs = hrs % 24; // Compute days, hours, etc.
    if (hrs < 10)
        sprintf(timestamp_hrs, "0%ld", hrs);
    else
        sprintf(timestamp_hrs, "%ld", hrs);
    if (mins < 10)
        sprintf(timestamp_mins, "0%ld", mins);
    else
        sprintf(timestamp_mins, "%ld", mins);
    if (secs < 10)
        sprintf(timestamp_secs, "0%ld", secs);
    else
        sprintf(timestamp_secs, "%ld", secs);

    if (my_settings.timestamp_config)
    {
        int a = 0;
        int b = 0;

        memset(timestamp, '\0', 12);

        while (my_settings.time_stamp_string[a] != '\0')
        {
            if (my_settings.time_stamp_string[a] == '%')
            {
                a++;
                if (my_settings.time_stamp_string[a] == 'h' || my_settings.time_stamp_string[a] == 'H')
                    strcat(timestamp, timestamp_hrs);

                else if (my_settings.time_stamp_string[a] == 'm' || my_settings.time_stamp_string[a] == 'M')
                    strcat(timestamp, timestamp_mins);

                else if (my_settings.time_stamp_string[a] == 's' || my_settings.time_stamp_string[a] == 'S')
                    strcat(timestamp, timestamp_secs);

                b++;

            }
            else
            {
                timestamp[b] = my_settings.time_stamp_string[a];
            }

            a++;
            b++;
        }

        timestamp[b] = ' ';
        b++;
        timestamp[b] = '\0';

    }
    else
        strcpy(timestamp, "");
}

char * ping_time()
{
    ULONG mics, secs, mins, hrs, days;
    static char pingtimestamp[12];
    char pingtimestamp_hrs[4];
    DoIO((struct IORequest *) TimerIO); // Get the results
#ifdef __amigaos4__
            mics=TimerIO->tr_time.tv_usec; secs=TimerIO->tr_time.tv_sec;
#else
    mics = TimerIO->tr_time.tv_micro;
    secs = TimerIO->tr_time.tv_secs;
#endif
    mins = secs / 60;
    hrs = mins / 60;
    days = hrs / 24;
    secs = secs % 60;
    mins = mins % 60;
    hrs = hrs % 24; // Compute days, hours, etc
    if (hrs < 10)
        sprintf(pingtimestamp_hrs, "0%ld", hrs);
    else
        sprintf(pingtimestamp_hrs, "%ld", hrs);
    if (mins < 10)
        sprintf(pingtimestamp_mins, "0%ld", mins);
    else
        sprintf(pingtimestamp_mins, "%ld", mins);
    if (secs < 10)
        sprintf(pingtimestamp_secs, "0%ld", secs);
    else
        sprintf(pingtimestamp_secs, "%ld", secs);
    sprintf(pingtimestamp, "%s%s", pingtimestamp_mins, pingtimestamp_secs);

    return pingtimestamp; /* Yes, return internal state */
}

char * dcc_time()
{
    ULONG mics, secs;
    static char dcctimestamp[12];
    DoIO((struct IORequest *) TimerIO); // Get the results
#ifdef __amigaos4__
    mics = TimerIO->tr_time.tv_usec; secs=TimerIO->tr_time.tv_sec;
#else
    mics = TimerIO->tr_time.tv_micro;
    secs = TimerIO->tr_time.tv_secs;
#endif
    sprintf(dcctimestamp, "%lu", secs);
    return dcctimestamp; /* Yes, return internal state */
}

struct timeval get_sys_time(struct timeval *tv)
{

    DoIO((struct IORequest *) TimerIO); // Get the results
#ifdef __amigaos4__
    tv->tv_sec=TimerIO->tr_time.tv_sec;
    tv->tv_usec=TimerIO->tr_time.tv_usec;

#else
    tv->tv_secs = TimerIO->tr_time.tv_secs;
    tv->tv_micro = TimerIO->tr_time.tv_micro;
#endif

    return *tv;
}

void init_2s_delay()
{
    // Get current system then wait 2 seconds before signalling again
    struct timeval systime;
    get_sys_time(&systime);

#ifdef __amigaos4__
    Timer2IO->tr_time.tv_sec = systime.tv_sec+2;
    Timer2IO->tr_time.tv_usec = systime.tv_usec;
#else
    Timer2IO->tr_time.tv_secs = systime.tv_secs + 2;
    Timer2IO->tr_time.tv_micro = systime.tv_micro;
#endif

    SendIO((struct IORequest *) Timer2IO);
}

struct Message * getmsg_2s_delay()
{
    return GetMsg(Timer2MP);
}

void init_paste_pause_delay()
{
    if (CheckIO((struct IORequest *) Timer5IO))
    {
        struct timeval *systime;
        get_sys_time(systime); // Get current system time
        Timer5IO->tr_node.io_Command = TR_ADDREQUEST;

#ifdef __amigaos4__
        Timer5IO->tr_time.tv_sec = systime.tv_sec+my_settings.paste_delay_seconds; //QUEUED_MESSAGES_DELAY_IN_SECONDS;
        Timer5IO->tr_time.tv_usec = systime.tv_usec+my_settings.paste_delay_microseconds;//QUEUED_MESSAGES_DELAY_IN_MICROSECONDS;
#else
        Timer5IO->tr_time.tv_secs = systime->tv_secs + my_settings.paste_delay_seconds; //QUEUED_MESSAGES_DELAY_IN_SECONDS;
        Timer5IO->tr_time.tv_micro = systime->tv_micro + my_settings.paste_delay_microseconds; //QUEUED_MESSAGES_DELAY_IN_MICROSECONDS;
#endif

        SendIO((struct IORequest *) Timer5IO); // signal us when its time to send another line of our paste
    }
}

struct Message * getmsg_paste_pause_delay()
{
    return GetMsg(Timer5MP);
}

struct Message * getmsg_midnight_wait()
{
    return GetMsg(Timer4MP);
}

void init_midnight_wait()
{
    struct timeval systime;
    get_sys_time(&systime); // Get current system time
#ifdef __amigaos4__
    Amiga2Date(systime.tv_sec, clockdata);
#else
    Amiga2Date(systime.tv_secs, clockdata);
#endif

#ifdef __amigaos4__
    Timer4IO->tr_time.tv_sec = systime.tv_sec+(60*60*24);
    Timer4IO->tr_time.tv_usec = 0;
#else
    Timer4IO->tr_time.tv_secs = systime.tv_secs + (60 * 60 * 24);
    Timer4IO->tr_time.tv_micro = 0;
#endif
    SendIO((struct IORequest *) Timer4IO); // Get the results
}
