/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

#ifndef IRCCOMMANDS_H
#define IRCCOMMANDS_H 1

/*************************************************************************/

#include <exec/types.h>
#include <intuition/intuition.h>
#include <utility/utility.h>

#include "muiclass_network.h"

/*************************************************************************/

struct IRCCommands {
	char *CMD_Name;
	char *CMD_ArgPattern;
	ULONG (* CMD_Function)( Object *obj, struct Server *Server, struct ServerMessageParse *ServerMessageParse );
};

extern struct IRCCommands TAB_IRCCOMMANDS[];

#define IRCCMD_RESULTF_IGNOREMESSAGE 1


/*
** Prototypes
*/

ULONG IRCCMD_GenericServer( Object *obj, struct Server *s, struct ServerMessageParse *smp );

/*************************************************************************/

#endif /* MUICLASS_H */

