/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/* This file contains definitions for used when adding new users */

/* User Addition Definitions */
#define AU_PEOPLE	0
#define AU_TREASURY	1
#define AU_LOCATE	2
#define AU_SOLDIERS	3
#define AU_ATTACK	4
#define AU_DEFEND	5
#define AU_REPRO	6
#define AU_MOVEMENT	7
#define AU_MAGIC	8
#define AU_LEADERS	9
#define AU_RAWGOODS	10
/* how many selections there are AU_NUMBER in dataX.h */

/* values for wood, jewels and metal (food is in AU_values array) */
#define AU_WOOD		15000L
#define AU_JEWELS	15000L
#define AU_METALS	15000L

/* location defines */
#define AU_OOPS		0
#define AU_RANDOM	1
#define AU_FAIR		2
#define AU_GOOD		3
#define AU_EXCELLENT	4

/* direction defines */
#define ADDITION	0
#define SUBTRACTION	1
