// SPDX-License-Identifier: GPL-3.0-or-later
/* Routines to perform the relocation of NPC units */
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
#include "dataA.h"
#include "armyX.h"
#include "moveX.h"
#include "statusX.h"

/* NPC_MOVEARMY -- Relocate an army unit one sector */
int
npc_movearmy PARM_2(int, x, int, y)
{
  int mcost;

  /* check the entered data */
  if ((army_ptr == NULL) ||
      ((ARMY_XLOC == x) && (ARMY_YLOC == y)) ||
      !map_within(ARMY_XLOC, ARMY_YLOC, x, y, 1)) {
    return(FALSE);
  }
  if (unit_flight(ARMY_STAT)) {
    movemode = MOVE_FLYARMY;
  } else {
    movemode = MOVE_ARMY;
  }

  /* check the movement costs */
  if ((mcost = move_cost(x, y, movemode)) < 0) {
    return(FALSE);
  }
  if ((ARMY_MOVE < 100) &&
      (mcost > ARMY_MOVE)) {
    return(FALSE);
  }
  ARMY_MOVE -= mcost;
  ARMY_LASTX = ARMY_XLOC;
  ARMY_LASTY = ARMY_YLOC;
  if (unit_leading(ARMY_STAT)) {
    set_grploc(ARMY_ID, x, y);
    set_grpmove(ARMY_ID, unit_speed(ARMY_STAT), ARMY_MOVE);
  } else {
    ARMY_XLOC = x;
    ARMY_YLOC = y;
  }
  return(TRUE);
}
