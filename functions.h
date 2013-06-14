/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

#ifndef FUNCTIONS_H
#define FUNCTIONS_H 1

/*************************************************************************/

#include <exec/types.h>
#include <workbench/icon.h>
#include <workbench/startup.h>

#include <string.h>

#include <proto/utility.h>
#include <proto/exec.h>
#include <proto/dos.h>

/*************************************************************************/

/*
** DOS Functions
*/

void Dos_ShowFailure( void );

/*
** Icon
*/

struct DiskObject *Icon_GetPutDiskObject( struct DiskObject *diskobj );
BOOL               Icon_ToolTypeGetBool( struct DiskObject *o, STRPTR tooltype, BOOL defvalue );
long               Icon_ToolTypeGetInteger( struct DiskObject *o, STRPTR tooltype, long defvalue );
STRPTR             Icon_ToolTypeGetString( struct DiskObject *o, STRPTR tooltype, STRPTR deftooltype );

/*
** WBMessage
*/

extern struct WBStartup *wbmessage;

void WBMessage_Get  ( void );
void WBMessage_Reply( void );

/*
** SimpleArgumentParsing
*/

#define SIMPLEREADARGS_ARGNUM 32

struct SimpleReadArgsData {
	struct RDArgs *srad_rda;
	struct RDArgs *srad_rd;
	IPTR           srad_ArgArray[ SIMPLEREADARGS_ARGNUM ];
	char           srad_Buffer[1];
};

struct SimpleReadArgsData *SimpleReadArgsParse( char *templatestring, char *parsestring );
void                       SimpleReadArgsFree( struct SimpleReadArgsData *srad );

/*************************************************************************/

#endif /* MUICLASS_H */

