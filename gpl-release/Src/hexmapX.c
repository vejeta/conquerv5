// SPDX-License-Identifier: GPL-3.0-or-later
/* This file contains hexmap routines relevent to all of conquer */
/*
 * Conquer - Classic Multi-Player Strategy Game
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
#include "dataX.h"

/* MAP_LOOP -- Perform a loop of range of N around given sector;
               The provided function is called for each sector found */
void
map_loop PARM_4(int, xcent, int, ycent, int, range, FNCV2, perform_func)
{
  register int xloc, yloc;
  int xend, xreal, ystart, yend, ycount;

  /* calculate the horizontal range */
  xend = xcent + range;

  /* now cruise the horizontal */
  for (xloc = xcent - range; xloc <= xend; xloc++) {

    /* find the range for the vertical */
    if (world.hexmap) {
      ycount = 2 * range - abs(xcent - xloc);
      ystart = ycent - (ycount + (xcent + 1) % 2) / 2;
      yend = ystart + ycount;
    } else {
      ystart = ycent - range;
      yend = ycent + range;
    }

    /* find the proper x location */
    xreal = (xloc + MAPX) % MAPX;

    /* cruise the vertical */
    for (yloc = ystart; yloc <= yend; yloc++) {

      /* if it is on the map... do the work */
      if (Y_ONMAP(yloc)) {

	/* call it */
	(*perform_func)(xreal, yloc);

      }

    }

  }
}

/* MAP_WITHIN -- Are the two locations within the given range? */
int
map_within PARM_5(int, xa, int, ya, int, xb, int, yb, int, range)
{
  int hold = FALSE, xdiff, ydiff, aydiff;

  /* find some info */
  xdiff = xa - xb;
  xdiff = abs(xdiff);
  ydiff = ya - yb;
  aydiff = abs(ydiff);

  /* find the horizontal difference */
  xdiff = xdiff % MAPX;
  if (xdiff > MAPX - xdiff) {
    xdiff = MAPX - xdiff;
  }

  /* depends on map type */
  if (world.hexmap) {

    /* translate */
    if ((xa % 2) == 1) {
      ydiff = -ydiff;
    }

    /* is it within this number of moves? */
    if ((aydiff <= range) &&
	(xdiff <= range)) {

      /* find central cone */
      if (aydiff <= range - 3) {
	/* always fine in here */
	hold = TRUE;
      } else if (xdiff <= (range - aydiff) * 2 + (ydiff >= 0)) {
	/* a bit tricky, but okay here now as well */
	hold = TRUE;
      }
    }

  } else {

    /* is it within range? */
    if ((aydiff <= range) &&
	(xdiff <= range)) {
      hold = TRUE;
    }

  }

  /* all done */
  return(hold);
}
