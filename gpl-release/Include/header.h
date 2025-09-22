// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Conquer - Classic Multi-Player Strategy Game - Main Header File
 *
 * Originally Copyright (C) 1987-1992 Ed Barlow <barlowedward@hotmail.com>
 * Originally Copyright (C) 1987-1992 Adam Bryant <adb@usa.com>
 * Copyright (C) 2025 Juan Manuel Méndez Rey <vejeta@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Relicensed to GPL v3+ with explicit permission from original authors.
 * For relicensing documentation, see RELICENSING-PERMISSIONS.md
 */


#ifndef CONQUER_HEADER_H
#define CONQUER_HEADER_H

#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include <errno.h>
#include <signal.h>

/* Modern C standard support with fallbacks */
#ifdef __STDC_VERSION__
    #if __STDC_VERSION__ >= 199901L
        #include <stdint.h>
        #include <stdbool.h>
        #define HAVE_C99 1
    #endif
#endif

#ifndef HAVE_C99
    #ifndef __cplusplus
        #ifndef bool
            typedef int bool;
            #define true 1
            #define false 0
        #endif
    #endif
#endif

/* Cross-platform compatibility */
#if defined(__APPLE__) && defined(__MACH__)
    #define PLATFORM_MACOS 1
    #define PLATFORM_UNIX 1
#elif defined(__linux__)
    #define PLATFORM_LINUX 1
    #define PLATFORM_UNIX 1
#elif defined(_WIN32)
    #define PLATFORM_WINDOWS 1
#endif

#ifdef PLATFORM_UNIX
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <fcntl.h>
#endif

#define VERSION "Version 5.0b"

/* Super User Information - LOGIN can be overridden by Makefile */
#define OWNER "God"
#ifndef LOGIN
#define LOGIN "jmy99"
#endif

/* PRIMARY ADJUSTMENTS - preserved exactly from original v5 */
#define COMPRESS            /* use compression when storing the data file */
#ifdef COMPRESS
    #define COMP_NAME "compress"    /* file compression program */
    #define COMP_READ "zcat"        /* how to read compressed files */
    #define COMP_SFX ".Z"           /* compressed file suffix */
#endif

#define MANY_UNITS          /* allow more than 255 army, navy, cvan units */
#define HUGE_MAP            /* allow really huge ( > 256x256 ) map sizes */
#define ABSMAXNTN 100       /* maximum number of nations in the world */

/* System Dependencies */
#define DEFAULT_UMASK 077
#define CUSTOM_UMASK  066
#define FULLWRITE_UMASK 000
#define ALLOW_EDIT_FORK
#define DO_TIME_CHECK

#ifdef PLATFORM_UNIX
    #define SYSMAIL
    #ifdef PLATFORM_MACOS
        #define SPOOLDIR "/var/mail/"
    #else
        #define SPOOLDIR "/usr/spool/mail/"
    #endif
#endif

#define LOCKF
#define REMOVE_NAME "/bin/rm -f"
#define MAIL_TIME 60
#define TIMETOBOOT 120
#define TMP_FILE_FMT "/tmp/%s.%d"
#define TMP_DIR "/tmp"
#define ENV_EDITOR "EDITOR"
#define DEFAULT_EDITOR "/usr/bin/vi"
#define NULL_INPUT "/dev/null"
#define NULL_OUTPUT "/dev/null"

/* Optional Configuration Definitions */
#define SECURITY
#define LISTUSERS
#define REMAKE
#define CHEAT
#define SPEW
#define RUNSTOP
#define UPDATESLEEP 30

/* Parameter Settings - preserved exactly from original */
#define NRAND_DIGITS 9
#define MIN_WORLD_SIZE 24
#define MAXFORTVAL 24
#define MAXCHARITY 25
#define MAXTAX 20
#define MAXNEWS 12
#define PLEADER_EXP 100
#define LONGTRIP 100
#define SPEEDUP_COST 1
#define PSTARVE 25
#define PCOLLAPSE 10
#define PDISBAND 10
#define PVOLUNTEERS 20
#define PBARNICLES 2
#define LATESTART 2
#define MIN_GAIN_STR 250
#define PROB_GAIN_STR 1500

/* Range Values */
#define LANDSEE 2
#define NAVYSEE 1
#define ARMYSEE 2
#define CVNSEE 1
#define PRTZONE 3
#define MEETNTN 2
#define NAVYRANGE 3
#define VISRANGE 4

/* Pager Settings */
#define MAX_FILE_LINES 5000
#define D_PAGEOFF 0
#define D_PAGETAB 8

/* Sector Specifications */
#define DESFOOD 4
#define TOOMANYPEOPLE 5000L
#define ABSMAXPEOPLE 50000L
#define MILLSIZE 500L
#define TOOMUCHMINED 50000L

/* Cost Specifications */
#define MOVECOST 20L
#define PEOPLE_MCOST 50L
#define NAVYMAINT 4000L
#define CVNMAINT 1000L
#define FORTCOST 1000L
#define CARAVANCOST 5000L
#define CARAVANWOOD 400L

/* Base numeric settings */
#define TAKESECTOR 75
#define TAKE_RATIO 7
#define BASE_TAKEPCT 10
#define MAXLOSS 60
#define FINDPERCENT 1
#define LOAD_CITYCOST 4
#define LOAD_LANDCOST 12

/* Unit definitions */
#define MAXNAVYCREW 100
#define MAXCVNCREW 30
#define NAVY_HOLD 100000L
#define CVN_HOLD 50000L
#define ARMYUNITCOST 500L
#define NAVYUNITCOST 1000L
#define CVNUNITCOST 500L
#define WAGONS_IN_CVN 10

/* NPC Behavior Settings */
#define CITYLIMIT 5L
#define CITYPERCENT 20L
#define MILRATIO 8L
#define MILINCAP 8L
#define MILINCITY 10L
#define NPCTOOFAR 15
#define METALORE 7L

/* Modern utility macros */
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#ifdef DEBUG
    #define DEBUG_PRINT(fmt, ...) \
        fprintf(stderr, "[DEBUG] %s:%d: " fmt "\n", __func__, __LINE__, ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(fmt, ...)
#endif

#endif /* CONQUER_HEADER_H */
