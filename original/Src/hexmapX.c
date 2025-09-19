/* This file contains hexmap routines relevent to all of conquer */
/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * A good deal of time and effort has gone into the writing of this
 * code and it is our hope that you respect this.  We give permission
 * to alter the code, but not to redistribute modified versions of the
 * code without our explicit permission.  If you do modify the code,
 * please document the changes made, and send us a copy, so that all
 * people may have it.  The code, to the best of our knowledge, works
 * well, but there will probably always be a need for bug fixes and
 * improvements.  We disclaim any responsibility for the codes'
 * actions.  [Use at your own risk].  This notice is just our way of
 * saying, "Happy gaming!", while making an effort to not get sued in
 * the process.
 *                           Ed Barlow, Adam Bryant
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
