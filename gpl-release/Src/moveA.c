// SPDX-License-Identifier: GPL-3.0-or-later
/* Routines to perform the relocation of NPC units */
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
