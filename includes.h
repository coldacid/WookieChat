/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/locale.h>
#include <proto/dos.h>
#include <proto/utility.h>

#define MUI_OBSOLETE
#include <libraries/mui.h>
#include <mui/NListview_mcc.h>

#ifndef __AROS__
#include <SDI_compiler.h>
#include <SDI_hook.h>
#endif

#ifdef __amigaos4__
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/filio.h>
#include <arpa/inet.h>
#include <proto/bsdsocket.h>
#include "os4.h"
#elif __MORPHOS__
#include <proto/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/filio.h>
#include <netdb.h>
#elif __AROS__
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <bsdsocket/socketbasetags.h>
#include <proto/socket.h>
#else
#include "gg:netinclude/sys/types.h"
#include "gg:netinclude/netdb.h"
#include "gg:netinclude/sys/filio.h"
#include "gg:netinclude/sys/ioctl.h"
#include "gg:netinclude/sys/socket.h"
#include "gg:netinclude/bsdsocket/socketbasetags.h"
#include "gg:netinclude/clib/socket_protos.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
