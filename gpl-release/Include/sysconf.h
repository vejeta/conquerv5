// SPDX-License-Identifier: GPL-3.0-or-later
/* This file contains the listing of system prototypes */
/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * See the file header.h for the full copyright notice.
 *
 * THIS FILE SHOULD NOT NEED MODIFYING
 *
 * If any changes should be necessary, please send such changes 
 * along with the OS, and machine type, to https://github.com/vejeta/conquerv5/issues
 */

/* stupid stuff for SGI systems */
#ifdef SGI
/* I hate this... why don't they just check for __STDC__? */
#ifdef __STDC__
#define __EXTENSIONS__
#endif /* __STDC__ */
#endif /* SGI */

/* standard list of definitions */
#define CRYPT		/* have crypt(): DO NOT CHANGE IF DATA IS IN PLACE */
#define STDLIB		/* system has the <stdlib.h> file in /usr/include */
#define UNISTD		/* system has the <unistd.h> file in /usr/include */
#define MKDIR		/* system has the mkdir() function in the library */
#define LRAND48		/* system has the lrand48() function available */
#define RANDOM		/* system has random(), #if LRAND48, lrand48() used */
#define FILELOCK	/* system has the "BSD" flock() function */
#define SEARCH_PATHENV	/* provide support for the PATH environment variable */

/* other options which are set by system specific sections */
/* #define WINCH_HANDLER */	/* system has the SIG_WINCH system in place */
/* #define REGEXP */		/* system has the re_exec() function */
/* #define NO_PERROR */		/* their is no perror() function available */
/* #define GETDTABLESIZE */	/* the getdtablesize() function is on system */
/* #define SETPRIORITY */	/* the setpriority() function is available */
/* #define SETREUID */		/* the setreuid() function is available */
/* #define SWITCHID */		/* switch uids when appropriate */
/* #define BZERO */		/* has the BSD bzero() memory clearing */
/* #define CUSERID */		/* the cuserid() function is on the system */
/* #define UNAME */		/* the uname() function for host name */
/* #define STRCHR */		/* the strchr() not index() is used */
/* #define MALLOCH */		/* system has <malloc.h> in /usr/include */
/* #define MEMORYH */		/* system has <memory.h> in /usr/include */
/* #define STRINGSH */		/* system uses <strings.h> not <string.h> */
/* #define SYS_SIGNAL */	/* system has signal.h in the sys subdir */
/* #define SIZET_FREAD */	/* system declares fread() as size_t */
/* #define DCLR_A */		/* declare first set of functions */
/* #define DCLR_B */		/* declare second set of definitions */
/* #define DCLR_C */		/* declare third set of definitions */
/* #define DCLR_D */		/* declare fourth set of definitions */

/* system specific definitions */

/* MACH OS?  At least on NeXT machines */
#ifdef MACHOS
#undef LRAND48
#define SYS_SIGNAL
#define BZERO
#define SETPRIORITY
#define DCLR_A
#endif /* MACHOS */

/* Sun OS 4.1.x system definitions */
#ifdef SUN41
#define WINCH_HANDLER
#define REGEXP
#define SETREUID
#define CUSERID
#define GETDTABLESIZE
#define SETPRIORITY
#define BZERO
#define MALLOCH
#define MEMORYH
#define STRINGSH
#define DCLR_C
#define DCLR_D
#endif /* SUN41 */

/* system V R3 definition */
#ifdef SYSV3
#define MEMORYH
#define MALLOCH
#define UNAME
#define STRCHR
#define DCLR_C
#endif /* SYSV3 */

/* system V R4 definition */
#ifdef SYSV4
#define MEMORYH
#define MALLOCH
#define SWITCHID
#define SETPRIORITY
#define UNAME
#define STRCHR
#define DCLR_C
#endif /* SYSV3 */

/* for Ultrix sites */
#ifdef ULTRIX
#define BZERO
#define SETPRIORITY
#define SIZET_FREAD
#define DCLR_D
#endif /* ULTRIX */

/* for HPUX sites */
#ifdef HPUX
#undef FILELOCK
#define REGEXP
#define NO_PERROR
#define SIZET_FREAD
#endif /* HPUX */

/* for other BSD systems */
#ifdef BSD
#undef UNISTD
#undef STDLIB
#undef LRAND48
#define WINCH_HANDLER
#define SETREUID
#define GETDTABLESIZE
#define REGEXP
#define BZERO
#define SETPRIORITY
#define DCLR_A
#define DCLR_B
#define DCLR_C
#define DCLR_D
#endif /* BSD */

/* AIX machines */
#ifdef AIX
#define MALLOCH
#define SETPRIORITY
#define SIZET_FREAD
#endif /* AIX */

/* for AIX 370 machines */
#ifdef AIX370
#define MALLOCH
#define SETPRIORITY
#define AIX
#endif /* AIX370 */

/* now, if LOCKF is uncommented, we need FILELOCK */
#ifdef LOCKF
#ifndef FILELOCK
#define FILELOCK
#endif /* FILELOCK */
#endif /* LOCKF */

/* must have SWITCHID if SETREUID */
#ifdef SETREUID
#ifndef SWITCHID
#define SWITCHID
#endif /* SWITCHID */
#endif /* SETREUID */

/* now include the system header files */
#include <curses.h>
#ifndef stdin		/* stdio.h should be included by curses.h */
#include <stdio.h>
#endif /* stdin */
#include <ctype.h>
#include <sys/time.h>
#ifdef __STDC__
#ifdef STDLIB
#include <stdlib.h>
#endif /* STDLIB */
#endif /* __STDC__ */
#ifdef UNISTD
#include <unistd.h>
#endif /* UNISTD */

#ifdef STRINGSH
#include <strings.h>
#else
#include <string.h>
#endif /* STRINGSH */
#ifndef VMS
#include <sys/types.h>
#include <sys/stat.h>
#else
#include <types.h>
#include <stat.h>
#endif /* VAXC */
#ifdef MALLOCH
#include <malloc.h>
#endif /* MALLOCH */

/* System Curses Definitions */
/*extern int printw(), mvprintw(), waddch(), waddstr(), wrefresh(), wmove(); */
/*extern int wclear(), wclrtobot(), wclrtoeol(), wstandout(), wstandend();*/
/*extern int wgetch(), gtty(), stty(), ioctl(), endwin();*/
/*extern int winsertln(), scroll();*/

/* Standard System Definitions */
extern char *getpass();
extern void perror();
extern int fclose(), pclose(), system();
extern int fputs(), getopt();
#ifndef VAXC
#ifndef ULTRIX
extern long time();
#endif /* ULTRIX */
#endif /* VAXC */

/* System Dependent Definitions */

/* Things not properly declared on BSD systems */
#ifdef DCLR_A
/*extern int getuid(), geteuid(), sleep(), umask();*/
#endif /* DCLR_A */
#ifdef DCLR_B
extern int exit(), abort();
extern int setuid(), chdir(), fprintf(), free(), unlink(), stat();
extern int sscanf(), close(), link(), mkdir();
extern char *malloc(), *getenv();
#endif /* DCLR_B */

#ifdef GETDTABLESIZE
extern int getdtablesize();
#endif /* GETDTABLESIZE */

#ifdef __mips__
#ifndef mips
#define mips
#endif /* mips */
#endif /* __mips__ */
#ifdef __LANGUAGE_C__
#ifndef LANGUAGE_C
#define LANGUAGE_C
#endif /* LANGUAGE_C */
#endif /* __LANGUAGE_C__ */

#ifndef AIX
#ifndef SYSV4
/*extern int open();*/
#endif /* SYSV4 */
#endif /* AIX */

#ifdef HPUX
extern int __flsbuf(),__filbuf();
extern void scrollok();
extern void crmode(),nocrmode(),echo(),noecho();
#else /* HPUX */
extern int _flsbuf(), _filbuf();
#endif /* HPUX */

#ifdef DCLR_C
/* Ultrix and AIX already have these defined? */
extern int scanf(), sscanf(), fscanf(), printf(), fprintf();
#endif /* DCLR_C */

#ifdef DCLR_D
/* the system timing stuff */
extern int setitimer(), gethostname();
#ifdef SETPRIORITY
extern int setpriority();
#endif /* SETPRIORITY */
#endif /* DCLR_D */

/* Index/strchr weirdness */
#ifdef STRCHR
#define index(s,c)	strchr(s,c)
#endif /* STRCHR */

/* check the ctype "function"s */
#ifndef toupper
extern int toupper();
#endif /* toupper */
#ifndef isprint
extern int isprint();
#endif /* isprint */
#ifndef isdigit
extern int isdigit();
#endif /* isdigit */
#ifndef islower
extern int islower();
#endif /* islower */
#ifndef isalpha
extern int isalpha();
#endif /* isalpha */
#ifndef isspace
extern int isspace();
#endif /* isspace */

/* the memory function and sprintf weirdness */
#ifdef MEMORYH
/* grab the system definitions */
#include <memory.h>
#else
#ifndef ULTRIX
#ifdef BSD
#ifdef MACHOS
extern char *sprintf();
#endif /* MACHOS */
#else
#ifndef AIX
extern int sprintf();
#endif /* AIX */
#endif /* BSD */
#endif /* ULTRIX */
#endif /* MALLOCH */

#ifdef BZERO
extern void bzero();
#endif /* BSD */

/* so, VAX C doesn't like unlink or uid stuff, huh? */
#ifdef VAXC
extern char *cuserid();
extern int rename(), remove(), sleep();
#ifndef SIZET_FREAD
#define SIZET_FREAD
#endif
#define unlink remove
#else
#endif /* VAXC */

#ifdef SETREUID
extern int setreuid();
#endif /* SETREUID */

#ifdef SIZET_FREAD
extern size_t fread(), fwrite();
#else
#ifndef HPUX
#ifndef MACHOS
/*extern int fread(), fwrite();*/
#endif /* MACHOS */
#endif /* HPUX */
#endif /* SIZET_FREAD */

/* Optional routine usage */

/* Let's be paranoid */
#ifdef CRYPT
extern char *crypt();
#endif /* CRYPT */

/* Randomizer */
#ifdef LRAND48
extern long lrand48();
extern void srand48();
#else
#ifdef RANDOM
extern long random();
extern void srandom();
#endif /* RANDOM */
#endif /* LRAND48 */

/* File locking */
#ifdef FILELOCK
#ifdef LOCKF
extern int lockf();
#else
extern int flock();
#endif /* LOCKF */
#endif /* FILELOCK */

/* Regular expression handlers */
#ifdef REGEXP
extern char *re_comp();
extern int re_exec();
#endif /* REGEXP */

/* The user listing stuff */
#ifdef LISTUSERS

#ifndef _UNISTD_H
extern ssize_t read(int, void *, size_t);
extern ssize_t write(int, const void *, size_t);
#endif

#ifndef _UNISTD_H
extern char *ttyname(int);
#endif

#endif /* LISTUSERS */

/* Finally, the data customizations */

/* Data Definitions */
//#ifndef PLATFORM_MACOS
typedef unsigned short int uns_short;
typedef unsigned char uns_char;
typedef uns_char ntntype;
//#endif /* PLATFORM_MACOS */

#ifdef DOUBLE_ITEMS
typedef double itemtype;
#define BIGITEM	BIGDOUBLE
#else
typedef long itemtype;
#define BIGITEM	BIGINT
#endif /* DOUBLE_ITEMS */

#ifdef MANY_UNITS
typedef uns_short idtype;
#else
typedef uns_char idtype;
#endif /* MANY_UNITS */

#ifdef HUGE_MAP
typedef uns_short maptype;
#else
typedef uns_char maptype;
#endif /* HUGE_MAP */

/* Make sure that the number of bits per byte is set */
#ifndef NBBY
#define NBBY	8	/* should be set by system, otherwise assume 8 */
#endif /* NBBY */

/* Data Constants */
#define MAX_MAPTYPE	((1L<<(sizeof(maptype) * NBBY)) - 1L)
#define MAX_IDTYPE	((1L<<(sizeof(idtype) * NBBY)) - 1L)
#define MAX_NTNTYPE	((1L<<(sizeof(ntntype) * NBBY)) - 1L)
#define TIME_DEAD 3600	/* ifndef FILELOCK how old a dead file must be	*/
