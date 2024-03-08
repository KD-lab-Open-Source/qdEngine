/*
  Copyright (c) 1990-2005 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2004-May-22 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, both of these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
/* Only the Windows DLL is currently supported */
#ifndef _ZIPAPI_H
#define _ZIPAPI_H

#include "zip.h"

#ifdef WIN32
#   ifndef PATH_MAX
#      define PATH_MAX 260
#   endif
#else
#   ifndef PATH_MAX
#      define PATH_MAX 128
#   endif
#endif

#if defined(WINDLL) || defined(API)
#include <windows.h>
/* Porting definations between Win 3.1x and Win32 */
#ifdef WIN32
#  define far
#  define _far
#  define __far
#  define near
#  define _near
#  define __near
#endif

/*---------------------------------------------------------------------------
    Prototypes for public Zip API (DLL) functions.
  ---------------------------------------------------------------------------*/

#define ZPVER_LEN    sizeof(ZpVer)
/* These defines are set to zero for now, until OS/2 comes out
   with a dll.
 */
#define D2_MAJORVER 0
#define D2_MINORVER 0
#define D2_PATCHLEVEL 0

/* intended to be a private struct: */
typedef struct _zip_ver {
    uch major;              /* e.g., integer 5 */
    uch minor;              /* e.g., 2 */
    uch patchlevel;         /* e.g., 0 */
    uch not_used;
} _zip_version_type;

typedef struct _ZpVer {
    ulg structlen;          /* length of the struct being passed */
    ulg flag;               /* bit 0: is_beta   bit 1: uses_zlib */
    char betalevel[10];     /* e.g., "g BETA" or "" */
    char date[20];          /* e.g., "4 Sep 95" (beta) or "4 September 1995" */
    char zlib_version[10];  /* e.g., "0.95" or NULL */
    _zip_version_type zip;
    _zip_version_type os2dll;
    _zip_version_type windll;
} ZpVer;

#  ifndef EXPENTRY
#    define EXPENTRY WINAPI
#  endif

#ifndef DEFINED_ONCE
#define DEFINED_ONCE
typedef int (WINAPI DLLPRNT) (LPSTR, unsigned long);
typedef int (WINAPI DLLPASSWORD) (LPSTR, int, LPCSTR, LPCSTR);
typedef int (WINAPI DLLSERVICE) (LPCSTR, unsigned long);
#endif
typedef int (WINAPI DLLCOMMENT)(LPSTR);

/* Structures */

typedef struct {        /* zip options */
LPSTR Date;             /* Date to include after */
LPSTR szRootDir;        /* Directory to use as base for zipping */
LPSTR szTempDir;        /* Temporary directory used during zipping */
BOOL fTemp;             /* Use temporary directory '-b' during zipping */
BOOL fSuffix;           /* include suffixes (not implemented) */
BOOL fEncrypt;          /* encrypt files */
BOOL fSystem;           /* include system and hidden files */
BOOL fVolume;           /* Include volume label */
BOOL fExtra;            /* Exclude extra attributes */
BOOL fNoDirEntries;     /* Do not add directory entries */
BOOL fExcludeDate;      /* Exclude files newer than specified date */
BOOL fIncludeDate;      /* Include only files newer than specified date */
BOOL fVerbose;          /* Mention oddities in zip file structure */
BOOL fQuiet;            /* Quiet operation */
BOOL fCRLF_LF;          /* Translate CR/LF to LF */
BOOL fLF_CRLF;          /* Translate LF to CR/LF */
BOOL fJunkDir;          /* Junk directory names */
BOOL fGrow;             /* Allow appending to a zip file */
BOOL fForce;            /* Make entries using DOS names (k for Katz) */
BOOL fMove;             /* Delete files added or updated in zip file */
BOOL fDeleteEntries;    /* Delete files from zip file */
BOOL fUpdate;           /* Update zip file--overwrite only if newer */
BOOL fFreshen;          /* Freshen zip file--overwrite only */
BOOL fJunkSFX;          /* Junk SFX prefix */
BOOL fLatestTime;       /* Set zip file time to time of latest file in it */
BOOL fComment;          /* Put comment in zip file */
BOOL fOffsets;          /* Update archive offsets for SFX files */
BOOL fPrivilege;        /* Use privileges (WIN32 only) */
BOOL fEncryption;       /* TRUE if encryption supported, else FALSE.
                           this is a read only flag */
int  fRecurse;          /* Recurse into subdirectories. 1 => -r, 2 => -R */
int  fRepair;           /* Repair archive. 1 => -F, 2 => -FF */
char fLevel;            /* Compression level (0 - 9) */
} ZPOPT, _far *LPZPOPT;

typedef struct {
int  argc;              /* Count of files to zip */
LPSTR lpszZipFN;        /* name of archive to create/update */
char **FNV;             /* array of file names to zip up */
} ZCL, _far *LPZCL;

typedef struct {
DLLPRNT *print;
DLLCOMMENT *comment;
DLLPASSWORD *password;
DLLSERVICE *ServiceApplication;
} ZIPUSERFUNCTIONS, far * LPZIPUSERFUNCTIONS;

extern LPZIPUSERFUNCTIONS lpZipUserFunctions;

void  EXPENTRY ZpVersion(ZpVer far *);
int   EXPENTRY ZpInit(LPZIPUSERFUNCTIONS lpZipUserFunc);
BOOL  EXPENTRY ZpSetOptions(LPZPOPT Opts);
ZPOPT EXPENTRY ZpGetOptions(void);
int   EXPENTRY ZpArchive(ZCL C);

#if defined(ZIPLIB) || defined(COM_OBJECT)
#   define ydays zp_ydays
#endif



/* Functions not yet supported */
#if 0
int      EXPENTRY ZpMain            (int argc, char **argv);
int      EXPENTRY ZpAltMain         (int argc, char **argv, ZpInit *init);
#endif
#endif /* WINDLL? || API? */

#endif /* _ZIPAPI_H */
