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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <devices/timer.h>
#include <dos/dos.h>
#include <exec/types.h>
#include <exec/memory.h>
#include <exec/ports.h>
#include <exec/io.h>
#include <exec/lists.h>
#include <libraries/dos.h>
#include <libraries/dosextens.h>
#include <libraries/gadtools.h>
#include <libraries/asl.h>
#include <intuition/intuition.h>
#include <devices/clipboard.h>
#include <workbench/workbench.h>
#include <intuition/classusr.h>
#include <graphics/gfxmacros.h>
#include <exec/memory.h>
#include <proto/datatypes.h>
#include <datatypes/soundclass.h>

#ifndef EAD_IS_FILE
#define EAD_IS_FILE(ead)    ((ead)->ed_Type <  0)
#endif

#ifdef __amigaos4__

#include <unistd.h>
#include <intuition/bitmapshare.h>
#include <clib/alib_protos.h>
#include <proto/codesets.h>
//#include <proto/rexxsyslib.h>
#ifndef   EXEC_TYPES_H
#include <exec/types.h>
#endif
#ifndef   REXX_RXSLIB_H
#include <rexx/rxslib.h>
#endif
#ifndef   REXX_REXXIO_H
#include <rexx/rexxio.h>
#endif

#include <proto/rexxsyslib.h>

struct SocketIFace * ISocket;
Library * RexxSysBase = NULL;
RexxSysIFace *IRexxSys;
struct CodesetsIFace  *ICodesets = NULL;

#include <proto/asl.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>
#include <proto/locale.h>
#include <proto/icon.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/filio.h>
#include <arpa/inet.h>
#include <proto/bsdsocket.h>
     // && select_struct.vnum==clipboard_struct.pos+1)
#include <proto/timer.h>
typedef char *p_in;
typedef char *l_in;
typedef char *i_in;
typedef UBYTE *a_in;
typedef char *b_in;
typedef char *c_in;
typedef char **c2_in;
#include "os4.h"

#elif __MORPHOS__
#include <proto/asl.h>
#include <proto/graphics.h>
#include <proto/alib.h>
#include <proto/exec.h>
#include <proto/utility.h>
#include <proto/dos.h>
#include <proto/locale.h>
#include <proto/icon.h>
#include <proto/datatypes.h>
#include <proto/muimaster.h>

#include <proto/codesets.h>

//#include <clib/codesets_protos.h>


//#include <libraries/codesets.h>
#include <proto/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <sys/filio.h>
#include <dos/dostags.h>

typedef char *p_in;
typedef char *l_in;
typedef UBYTE *i_in;
typedef UBYTE *a_in;
typedef char *b_in;
typedef char *c_in;
typedef char **c2_in;

#elif __AROS__

#define MUI_OBSOLETE

extern "C"
{
#include <clib/alib_protos.h>

#include <proto/datatypes.h>
#include <proto/asl.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>
#include <proto/locale.h>
#include <proto/icon.h>

#include <proto/muimaster.h>
#include <proto/socket.h>

#include <dos/dostags.h>

#include <SYS/TYPES.H>
#include <NETDB.H>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <SYS/SOCKET.H>
#include <BSDSOCKET/SOCKETBASETAGS.H>
#include <CLIB/SOCKET_PROTOS.H>
}

typedef char *p_in;
typedef char *l_in;
typedef unsigned int *i_in;
typedef UBYTE *a_in;
typedef char *b_in;
typedef char *c_in;
typedef char **c2_in;

#else
#include <proto/codesets.h>
#include <clib/asl_protos.h>
#include <clib/graphics_protos.h>
#include <clib/alib_protos.h>
#include <clib/exec_protos.h>
#include <clib/utility_protos.h>
#include <clib/dos_protos.h>
#include <clib/locale_protos.h>
#include <clib/icon_protos.h>
#include <inline/alib.h>
#include <inline/muimaster.h>
#include <dos/exall.h>
#include <dos/dostags.h>


typedef char *p_in;
typedef char *l_in;
typedef unsigned char *i_in;
typedef UBYTE *a_in;
typedef char *b_in;
typedef UBYTE *c_in;
typedef UBYTE **c2_in;

extern "C"
{
#include "gg:NETINCLUDE/SYS/TYPES.H"
#include "gg:NETINCLUDE/NETDB.H"
#include "gg:NETINCLUDE/SYS/FILIO.H"
#include "gg:netinclude/sys/ioctl.h"
#include "gg:NETINCLUDE/SYS/SOCKET.H"
#include "gg:NETINCLUDE/BSDSOCKET/SOCKETBASETAGS.H"
#include "gg:NETINCLUDE/CLIB/SOCKET_PROTOS.H"
}

#endif

#include <MUI/NListview_mcc.h>
#include <MUI/NListtree_mcc.h>
#include <MUI/BetterString_mcc.h>

#ifdef __amigaos4__
#include <MUI/NBitmap_mcc.h>
#endif

int main(void)
{

    BPTR examined_file;
    LONG count=1;

    char orig_filename[100];
    strcpy(orig_filename,"thisisafile");

    char filename[100];
    strcpy(filename,"thisisafile");

    while(examined_file = Open(filename,MODE_OLDFILE))
    {
        Close(examined_file);
        count++;
        sprintf(filename,"%s_%li",orig_filename,count);
    }

    if(count!=1)
        sprintf(filename,"%s_%li\n",orig_filename,count);

    printf("new filename:%s\n",filename);

    return 0;
}
