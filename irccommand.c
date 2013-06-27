/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

//#define NODEBUG

/*
** irccommand.c
*/

#include <libraries/mui.h>
#include <proto/muimaster.h>
#include <proto/intuition.h>
#include <proto/utility.h>
#include <proto/exec.h>
#include <SDI_hook.h>

#include <string.h>
#include <stdio.h>

#include "system.h"
#include "functions.h"
#include "irccommand.h"
#include "locale.h"
#include "muiclass.h"
#include "muiclass_application.h"
#include "muiclass_windowsettings.h"
#include "muiclass_windowchat.h"
#include "muiclass_network.h"
#include "muiclass_chatlog.h"
#include "muiclass_settingscolor.h"
#include "version.h"

/*************************************************************************/

/* /// IRCCMD_PrivMsg()
**
*/

/*************************************************************************/

static ULONG IRCCMD_PrivMsg( Object *obj, struct Server *s, struct ServerMessageParse *smp )
{
	if( strstr( smp->smp_Message, "\001ACTION" ) && smp->smp_Message[ strlen( smp->smp_Message ) - 1 ] == '\001' ) {
		smp->smp_Pen = PEN_LOGACTION;

		sprintf( &smp->smp_MessageBuffer[ strlen( smp->smp_MessageBuffer ) ],
				"* %s%s", smp->smp_FromNick, &smp->smp_Message[ strlen( "\001ACTION" ) ]
			);
	} else {
		smp->smp_Pen = PEN_LOGPRIVMSG;
		sprintf( &smp->smp_MessageBuffer[ strlen( smp->smp_MessageBuffer ) ],
				"<%s> %s", smp->smp_FromNick, smp->smp_Message
			);
	}

	return( 0 );
}
/* \\\ */
/* /// IRCCMD_Notice()
**
*/

/*************************************************************************/

static ULONG IRCCMD_Notice( Object *obj, struct Server *s, struct ServerMessageParse *smp )
{
	smp->smp_Pen = PEN_LOGNOTICE;
	sprintf( &smp->smp_MessageBuffer[ strlen( smp->smp_MessageBuffer ) ],
		"[%s] %s %s", LGS( MSG_MUICLASS_NETWORK_NOTICE ),
						smp->smp_FromNick,
						smp->smp_Message
		);

	return( 0 );
}
/* \\\ */
/* /// IRCCMD_Ping()
**
*/

/*************************************************************************/

static ULONG IRCCMD_Ping( Object *obj, struct Server *s, struct ServerMessageParse *smp )
{
	DoMethod( obj, MM_NETWORK_SERVERMESSAGESEND, s, "PONG" );

	return( IRCCMD_RESULTF_IGNOREMESSAGE );
}
/* \\\ */
/* /// IRCCMD_GenericServer()
**
*/

/*************************************************************************/

ULONG IRCCMD_GenericServer( Object *obj, struct Server *s, struct ServerMessageParse *smp )
{
	sprintf( &smp->smp_MessageBuffer[ strlen( smp->smp_MessageBuffer ) ],
		"[%s] %s", LGS( MSG_MUICLASS_NETWORK_SERVER ),
						smp->smp_StringStorage );

	return( 0 );
}
/* \\\ */
/* /// IRCCMD_Join()
**
*/

/*************************************************************************/

static ULONG IRCCMD_Join( Object *obj, struct Server *s, struct ServerMessageParse *smp )
{
struct Channel *c;

	if( ( c = (APTR) DoMethod( obj, MM_NETWORK_SERVERFINDCHANNEL, s, smp->smp_Channel ) ) ) {
		DoMethod( _app(obj), MM_APPLICATION_CHANNELADD, c );
	}

	smp->smp_Pen = PEN_LOGJOIN;
	sprintf( &smp->smp_MessageBuffer[ strlen( smp->smp_MessageBuffer ) ],
		"[%s] %s (%s@%s) %s",  LGS( MSG_MUICLASS_NETWORK_JOIN ),
							smp->smp_FromNick,
							smp->smp_FromUserID,
							smp->smp_FromHost,
							smp->smp_Message
		);

	return( 0 );
}
/* \\\ */
/* /// IRCCMD_Part()
**
*/

/*************************************************************************/

static ULONG IRCCMD_Part( Object *obj, struct Server *s, struct ServerMessageParse *smp )
{
struct Channel *c;

	if( ( c = (APTR) DoMethod( obj, MM_NETWORK_SERVERFINDCHANNEL, s, smp->smp_Channel ) ) ) {
		DoMethod( _app(obj), MM_APPLICATION_CHANNELREMOVE, c );
	}

	smp->smp_Pen = PEN_LOGPART;
	sprintf( &smp->smp_MessageBuffer[ strlen( smp->smp_MessageBuffer ) ],
		"[%s] %s (%s@%s) %s",  LGS( MSG_MUICLASS_NETWORK_PART ),
							smp->smp_FromNick,
							smp->smp_FromUserID,
							smp->smp_FromHost,
							smp->smp_Message
		);

	return( 0 );
}
/* \\\ */
/* /// IRCCMD_Quit()
**
*/

/*************************************************************************/

static ULONG IRCCMD_Quit( Object *obj, struct Server *s, struct ServerMessageParse *smp )
{
	smp->smp_Pen = PEN_LOGQUIT;
	sprintf( &smp->smp_MessageBuffer[ strlen( smp->smp_MessageBuffer ) ],
		"[%s] %s (%s@%s) %s",  LGS( MSG_MUICLASS_NETWORK_QUIT ),
							smp->smp_FromNick,
							smp->smp_FromUserID,
							smp->smp_FromHost,
							smp->smp_Message
		);

	return( 0 );
}
/* \\\ */
/* /// IRCCMD_ChannelWebSiteIs()
**
*/

/*************************************************************************/

static ULONG IRCCMD_ChannelWebSiteIs( Object *obj, struct Server *s, struct ServerMessageParse *smp )
{
struct Channel *c;

	c = (APTR) DoMethod( obj, MM_NETWORK_SERVERFINDCHANNEL, s, smp->smp_Channel );

	smp->smp_Pen = PEN_LOGINFO;
	sprintf( &smp->smp_MessageBuffer[ strlen( smp->smp_MessageBuffer ) ], (char *)
						LGS( MSG_MUICLASS_NETWORK_CHANNELWEBSITE_FORMAT  ),
							LGS( MSG_MUICLASS_NETWORK_SERVER ),
							smp->smp_Channel,
							smp->smp_Message
		);

	return( 0 );
}
/* \\\ */
/* /// IRCCMD_TopicNotSet()
**
*/

/*************************************************************************/

static ULONG IRCCMD_TopicNotSet( Object *obj, struct Server *s, struct ServerMessageParse *smp )
{
struct Channel *c;
char *msg = (char*) LGS( MSG_MUICLASS_NETWORK_TOPICNOTSET );

	if( ( c = (APTR) DoMethod( obj, MM_NETWORK_SERVERFINDCHANNEL, s, smp->smp_Channel ) ) ) {
		if( c->c_Topic ) {
			FreeVec( c->c_Topic );
		}
		if( ( c->c_Topic = AllocVec( strlen( msg ), MEMF_ANY ) ) ) {
			strcpy( c->c_Topic, msg );
		}
		DoMethod( _app(obj), MM_APPLICATION_CHANNELCHANGETOPIC, c );
	}

	smp->smp_Pen = PEN_LOGTOPIC;
	sprintf( &smp->smp_MessageBuffer[ strlen( smp->smp_MessageBuffer ) ], (char*)
						LGS( MSG_MUICLASS_NETWORK_TOPICNOTSET_FORMAT ),
							LGS( MSG_MUICLASS_NETWORK_TOPIC ),
							msg
		);


	return( 0 );
}
/* \\\ */
/* /// IRCCMD_Topic()
**
*/

/*************************************************************************/

static ULONG IRCCMD_Topic( Object *obj, struct Server *s, struct ServerMessageParse *smp )
{
struct Channel *c;

	if( ( c = (APTR) DoMethod( obj, MM_NETWORK_SERVERFINDCHANNEL, s, smp->smp_Channel ) ) ) {
		if( c->c_Topic ) {
			FreeVec( c->c_Topic );
		}
		if( ( c->c_Topic = AllocVec( strlen( smp->smp_Message ) + 1, MEMF_ANY ) ) ) {
			strcpy( c->c_Topic, smp->smp_Message );
		}
		DoMethod( _app(obj), MM_APPLICATION_CHANNELCHANGETOPIC, c );
	}

	smp->smp_Pen = PEN_LOGTOPIC;
	sprintf( &smp->smp_MessageBuffer[ strlen( smp->smp_MessageBuffer ) ], (char*)
						LGS( MSG_MUICLASS_NETWORK_TOPIC_FORMAT ),
							LGS( MSG_MUICLASS_NETWORK_TOPIC ),
							smp->smp_Message
		);

	return( 0 );
}
/* \\\ */
/* /// IRCCMD_TopicSetBy()
**
*/

/*************************************************************************/

static ULONG IRCCMD_TopicSetBy( Object *obj, struct Server *s, struct ServerMessageParse *smp )
{
	smp->smp_Pen = PEN_LOGTOPIC;
	sprintf( &smp->smp_MessageBuffer[ strlen( smp->smp_MessageBuffer ) ], (char *)
						LGS( MSG_MUICLASS_NETWORK_TOPICSETBY_FORMAT ),
							LGS( MSG_MUICLASS_NETWORK_TOPIC ),
							smp->smp_FromNick,
							smp->smp_FromUserID,
							smp->smp_FromHost,
							smp->smp_Date
		);

	return( 0 );
}
/* \\\ */
/* /// IRCCMD_NameReply()
**
*/

/*************************************************************************/

static ULONG IRCCMD_NameReply( Object *obj, struct Server *s, struct ServerMessageParse *smp )
{
struct SimpleReadArgsData *srad;
struct Channel *c;
struct ChannelNickEntry *cne;
struct ChatLogEntry *cle;
char *nick, *bufferstart;

	if( ( c = (APTR) DoMethod( obj, MM_NETWORK_SERVERFINDCHANNEL, s, smp->smp_Channel ) ) ) {
		if( ( srad = SimpleReadArgsParse( "NICK/M/A", smp->smp_Message ) ) ) {
			char **array = (APTR) srad->srad_ArgArray[0];
			while( ( nick = *array++ ) ) {
				if( ( cne = (APTR) DoMethod( obj, MM_NETWORK_CHATNICKENTRYALLOC, c, nick ) ) ) {
					DoMethod( _app(obj), MM_APPLICATION_CHANNELNICKADD, c, cne );
				}
			}
			SimpleReadArgsFree( srad );
		}
		if( !( c->c_Flags & CHANNELF_NAMESLIST ) ) {
			bufferstart	= &smp->smp_MessageBuffer[ strlen( smp->smp_MessageBuffer ) ];
			sprintf( bufferstart,
				"[%s] %s %s:", LGS( MSG_MUICLASS_NETWORK_NAMES ),
							LGS( MSG_MUICLASS_NETWORK_USERSINCHANNEL ),
							smp->smp_Channel
							);

			if( ( cle = (APTR) DoMethod( obj, MM_NETWORK_CHATLOGENTRYADD, s, smp->smp_Channel, PEN_LOGNOTICE, smp->smp_MessageBuffer ) ) ) {
				DoMethod( _app(obj), MM_APPLICATION_MESSAGERECEIVED, cle );
			}
			c->c_Flags |= CHANNELF_NAMESLIST; /* make sure header is only shown once */
		}

		smp->smp_Pen = PEN_LOGNOTICE;
		sprintf( bufferstart,
			"[%s] %s", LGS( MSG_MUICLASS_NETWORK_NAMES ),
					smp->smp_Message
			);
		
		return( 0 );
	}
	return( IRCCMD_RESULTF_IGNOREMESSAGE );
}
/* \\\ */
/* /// IRCCMD_EndOfNames()
**
*/

/*************************************************************************/

static ULONG IRCCMD_EndOfNames( Object *obj, struct Server *s, struct ServerMessageParse *smp )
{
struct Channel *c;

	if( ( c = (APTR) DoMethod( obj, MM_NETWORK_SERVERFINDCHANNEL, s, smp->smp_Channel ) ) ) {
		c->c_Flags &= ~CHANNELF_NAMESLIST; /* we are done */
	}

	return( IRCCMD_RESULTF_IGNOREMESSAGE );
}
/* \\\ */
/* /// IRCCMD_MOTDStart()
**
*/

/*************************************************************************/

static ULONG IRCCMD_MOTDStart( Object *obj, struct Server *s, struct ServerMessageParse *smp )
{
struct Channel *c;

	if( ( c = (APTR) DoMethod( obj, MM_NETWORK_SERVERFINDCHANNEL, s, NULL ) ) ) {
		c->c_Flags |= CHANNELF_MESSAGEOFTHEDAY;

		smp->smp_Pen = PEN_LOGNOTICE;
		sprintf( &smp->smp_MessageBuffer[ strlen( smp->smp_MessageBuffer ) ],
			"[%s] %s", LGS( MSG_MUICLASS_NETWORK_SERVER ),
								smp->smp_Message
			);
		return( 0 );
	}
	return( IRCCMD_RESULTF_IGNOREMESSAGE );
}
/* \\\ */
/* /// IRCCMD_MOTD()
**
*/

/*************************************************************************/

static ULONG IRCCMD_MOTD( Object *obj, struct Server *s, struct ServerMessageParse *smp )
{
struct Channel *c;

	if( ( c = (APTR) DoMethod( obj, MM_NETWORK_SERVERFINDCHANNEL, s, NULL ) ) ) {
		if( ( c->c_Flags & CHANNELF_MESSAGEOFTHEDAY ) ) {
			smp->smp_Pen = PEN_LOGNOTICE;
			sprintf( &smp->smp_MessageBuffer[ strlen( smp->smp_MessageBuffer ) ],
				"[%s] %s", LGS( MSG_MUICLASS_NETWORK_SERVER ),
									smp->smp_Message
				);
				return( 0 );
		}
	}
	return( IRCCMD_RESULTF_IGNOREMESSAGE );
}
/* \\\ */
/* /// IRCCMD_MOTDEnd()
**
*/

/*************************************************************************/

static ULONG IRCCMD_MOTDEnd( Object *obj, struct Server *s, struct ServerMessageParse *smp )
{
struct Channel *c;

	if( ( c = (APTR) DoMethod( obj, MM_NETWORK_SERVERFINDCHANNEL, s, NULL ) ) ) {
		c->c_Flags &= ~CHANNELF_MESSAGEOFTHEDAY;
	}
	return( IRCCMD_RESULTF_IGNOREMESSAGE );
}
/* \\\ */
/* /// IRCCMD_ChannelModeIs()
**
*/

/*************************************************************************/

static ULONG IRCCMD_ChannelModeIs( Object *obj, struct Server *s, struct ServerMessageParse *smp )
{

	debug("channel mode is '%s'\n",smp->smp_StringStorage );
	smp->smp_Pen = PEN_LOGMODE;
	sprintf( &smp->smp_MessageBuffer[ strlen( smp->smp_MessageBuffer ) ],
		"[%s] %s",  LGS( MSG_MUICLASS_NETWORK_SERVER ),
							smp->smp_StringStorage );

	return( 0 );
}
/* \\\ */

/* /// IRCCommands
*/

/*************************************************************************/

struct IRCCommands TAB_IRCCOMMANDS[] =
{
	{ "PRIVMSG",    "C",     IRCCMD_PrivMsg                         },
	{ "NOTICE",     "N",     IRCCMD_Notice                          },
	{ "PING",       "",      IRCCMD_Ping                            },
	{ "JOIN",       "C",     IRCCMD_Join                            },
	{ "PART",       "C",     IRCCMD_Part                            },
	{ "QUIT",       "",      IRCCMD_Quit                            },
	{ "328",        "NC",    IRCCMD_ChannelWebSiteIs          },
	{ "331",        "NC",    IRCCMD_TopicNotSet               },
	{ "332",        "NC",    IRCCMD_Topic                     },
	{ "333",        "NCFD",  IRCCMD_TopicSetBy                },
	{ "353",        "N-C",   IRCCMD_NameReply                 },
	{ "366",        "NC",    IRCCMD_EndOfNames                },
	{ "375",        "NC",    IRCCMD_MOTDStart                 },
	{ "372",        "NC",    IRCCMD_MOTD                      },
	{ "376",        "NC",    IRCCMD_MOTDEnd                   },
	{ "324",        "NC",    IRCCMD_ChannelModeIs             },
	{ NULL, NULL, NULL },
};
/* \\\ */

