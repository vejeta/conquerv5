// SPDX-License-Identifier: GPL-3.0-or-later
/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/* This file lists the options for the various display modes */
#include "displayX.h"

/* Structure for screen information */
typedef struct s_screen {
  int xshift;		/* characters shifted when going horizonally */
  int yshift;		/* characters shifted when going vertically */
  int xsize;		/* width of one sector on the display */
  int ysize;		/* height of one sector on the display */
  int oddlift;		/* y offset for odd columns */
  int has_border;	/* is there a border on this setting? */
} SCREEN_STRUCT, *SCREEN_PTR;

/* the screen definitions */
#define SCREEN_RIGHT	20
#define SCREEN_MIDDLE	50
#define SCREEN_BOTTOM	5
#define SCREEN_EXTBTM	12
#define SCREEN_BTMLIM	(conq_infomode?SCREEN_EXTBTM:SCREEN_BOTTOM)
#define SCREEN_RGTLIM	(conq_infomode?SCREEN_MIDDLE:(SCREEN_RIGHT+1))
#define SCREEN_X_SIZE	(COLS - (SCREEN_RGTLIM + 1))
#define SCREEN_Y_SIZE	(LINES - SCREEN_BTMLIM)

/* the number of base display modes */
#define DMODE_NUMBER	21

/* change view options */
#define CV_CONTOUR	0
#define CV_DESIGNATION	1
#define CV_VEGETATION	2
#define CV_NUMBER	3

/* Sector information storage */
#define SSTAT_TROOPS	0x01
#define SSTAT_UNMOVED	0x02
#define SSTAT_MOVABLE	0x04
#define SSTAT_SCOUTS	0x08

/* sector status schtick */
#define SECTSTAT(x,y)	sectstat[(x) + (y) * MAPX]
#define TROOPLOCS(x,y)	trooplocs[(x) + (y) * MAPX]

/* quickie references */
#define has_troops(x,y)	(SECTSTAT(x,y) & SSTAT_TROOPS)
#define has_unmoved(x,y)	(SECTSTAT(x,y) & SSTAT_UNMOVED)
#define has_movable(x,y)	(SECTSTAT(x,y) & SSTAT_MOVABLE)
#define has_scouts(x,y)	(SECTSTAT(x,y) & SSTAT_SCOUTS)

/* quickie settings */
#define set_has_troops(x,y)	SECTSTAT(x,y) |= SSTAT_TROOPS
#define set_has_unmoved(x,y)	SECTSTAT(x,y) |= SSTAT_UNMOVED
#define set_has_movable(x,y)	SECTSTAT(x,y) |= SSTAT_MOVABLE
#define set_has_scouts(x,y)	SECTSTAT(x,y) |= SSTAT_SCOUTS

/* data information */
extern int zoom_level;
extern DISPLAY_STRUCT base_modes[DMODE_NUMBER];
extern SCREEN_STRUCT hexmap_screen[ZOOM_NUMBER];
extern SCREEN_STRUCT rectmap_screen[ZOOM_NUMBER];
extern SCREEN_PTR curmap_screen;
