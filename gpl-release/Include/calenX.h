// SPDX-License-Identifier: GPL-3.0-or-later
/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/* This file lists the various calender definitions for conquer */

#define YEAR(x)		((int)((x+11)/12))	/* value of the game year */
#define MONTH(x)	((x-1)%12)		/* numeric montly value */
#define PMONTH(x)	monthstr[(x-1)%12]	/* printable month value */
#define SEASON(x)	(MONTH(x)/3)		/* numeric season value */
#define PSEASON(x)	seasonstr[MONTH(x)/3]	/* printable season value */
#define SPRING_SEASON	0
#define SUMMER_SEASON	1
#define FALL_SEASON	2
#define WINTER_SEASON  	3

/* Variables */
extern char *seasonstr[], *monthstr[];
