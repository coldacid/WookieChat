#ifndef LOCALE_H
#define LOCALE_H 1

/*************************************************************************/

#include <exec/types.h>
#include <dos/dos.h>
#include <libraries/locale.h>

#include "WookieChat_strings.h"

#define MSG_ERROR_NOERROR 0

/*
** Prototypes
*/

BOOL   Locale_Open( STRPTR catname, ULONG version, ULONG revision);
void   Locale_Close(void);
STRPTR Locale_GetString(long ID);

/*
** backward compatibly, we should be able to remove
** them at some point of reconstruction
*/

#include "version.h"
#define GCS(stringNum, String) Locale_GetString( stringNum )

/*************************************************************************/

#endif /* LOCALE_H */
