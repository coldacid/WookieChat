/*
    The contents of this file are subject to the AROS Public License Version 1.1
    (the "License"); you may not use this file except in compliance with the License.
    You may obtain a copy of the License at http://www.aros.org/license.html

    Software distributed under the License is distributed on an "AS IS" basis,
    WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
    for the specific language governing rights and limitations under the License.
*/

#if defined(__AROS__) || defined(__MORPHOS__)
#define CATCOMP_ARRAY 1
#else
#error Provide locale specific definition for your system
#endif
#define NEW_CATCOMP_ARRAY_IDS
#include "locale.h"
#include "version.h"

#include <proto/locale.h>
#include <proto/exec.h>
#include <SDI_hook.h>

#include "functions.h"

static struct Catalog *locale_catalog = NULL;
struct Locale  *locale_locale;

/*************************************************************************/

/* /// Locale_Open()
**
*/

/*************************************************************************/

BOOL Locale_Open( STRPTR catname, ULONG version, ULONG revision)
{

	if( ( LocaleBase = (APTR) OpenLibrary( (_ub_cs) "locale.library",0 ) ) ) {
#ifdef __amigaos4__
extern struct LocaleIFace      *ILocale;
		if( ( ILocale = (struct LocaleIFace *) GetInterface( LocaleBase, "main", 1, NULL ) ) ) {
#endif
		RemLibrary( (struct Library *) LocaleBase );
		if( ( locale_locale = OpenLocale( NULL ) ) ) {
			if( ( locale_catalog = OpenCatalogA( locale_locale, catname, TAG_DONE ) ) ) {

				if(    locale_catalog->cat_Version  ==  version &&
					   locale_catalog->cat_Revision == revision ) {
					return( TRUE );
				}
				CloseCatalog( locale_catalog );         /* so close catalog */
				locale_catalog = NULL;                  /* and use default (if present) */
			}
		}
    }
	return FALSE;
}
/* \\\ */
/* /// Locale_Close()
**
*/

/*************************************************************************/

void Locale_Close(void)
{
    if( LocaleBase) {
        if( locale_catalog) {
            CloseCatalog(locale_catalog);
            locale_catalog = NULL;
        }
        if( locale_locale) {
            CloseLocale(locale_locale);
            locale_locale = NULL;
        }
		RemLibrary( (struct Library *) LocaleBase ); /* flush our catalog */
#ifdef __amigaos4__
		DropInterface((struct Interface *)ILocale);
#endif
/* flush our catalog */
		RemLibrary( (struct Library *) LocaleBase );

		CloseLibrary( (APTR) LocaleBase );
		LocaleBase = NULL;
    }
}
/* \\\ */
/* /// Locale_GetString()
**
*/

/*************************************************************************/

STRPTR Locale_GetString( long id )
{
#if defined(__AROS__) || defined(__MORPHOS__)
	if (LocaleBase  && locale_catalog != NULL)
    {
        return (STRPTR)GetCatalogStr(locale_catalog, id, CatCompArray[id].cca_Str);
    }
    else
    {
        return CatCompArray[id].cca_Str;
    }
#else
#error Provide locale specific definition for your system
#endif
}
/* \\\ */
/* /// Locale_FormatDate()
**
*/

/* /// FormatDateStream() */

struct FormatDateStream
{
    char    *Target;
    ULONG    TargetSize;
};

/*************************************************************************/

HOOKPROTO( FormatDate_func, ULONG, struct Locale *locale UNUSED, ULONG c )
{
struct FormatDateStream *fs = (struct FormatDateStream*) hook->h_Data;

	if( fs->TargetSize ) {
		*(fs->Target)++ = (char) c;
		fs->TargetSize--;
	}
    return(0);
}
MakeHook( FormatDate_hook, FormatDate_func );

/* \\\ */

/*************************************************************************/

ULONG Locale_FormatDate( char *format, char *target, struct DateStamp *ds, ULONG targetsize )
{
struct FormatDateStream fs;
struct Hook hook;

	CopyMem( &FormatDate_hook, &hook, sizeof( struct Hook )); /* copy hook, to support reentrant */

	fs.Target     = target;
	fs.TargetSize = targetsize;
	hook.h_Data   = &fs;

	if( LocaleBase && locale_locale ) {

		FormatDate( locale_locale, (STRPTR) format, ds, &hook );
	} else {
		*target = 0x00;
	}

	return( targetsize - fs.TargetSize );
}
/* \\\ */

