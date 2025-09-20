// SPDX-License-Identifier: GPL-3.0-or-later
/* routines to perform the action of monster units and nations */
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
#include "cityX.h"
#include "navyX.h"
#include "desigX.h"
#include "mtrlsX.h"
#include "worldX.h"
#include "elevegX.h"
#include "statusX.h"

/* MONSTER_GROWTH -- Expand the military and the "treasures" */
void
monster_growth PARM_0(void)
{
  int i, j, rate_inc, rate_merc;

  /* set quickie references */
  if (ntn_ptr == NULL) return;
  rate_merc = PMERCMONST;
  if (rate_merc < 0) rate_merc = 0;
  rate_inc = PMONSTER - rate_merc;
  rate_merc = (GROWTH * rate_merc) / 10;
  rate_inc = (GROWTH * rate_inc) / 10;
  if ((rate_merc == 0) && (rate_inc == 0)) return;

  /* go through all of the armies */
  for (army_ptr = ntn_ptr->army_list;
       army_ptr != NULL;
       army_ptr = army_ptr->next) {
    /* figure the type */
    if (a_ismonster(ARMY_TYPE)) {
      /* monsters will only increase in size if there is more than one */
      if ((ARMY_SIZE > 1) &&
	  (ARMY_SIZE * rate_inc > rand_val(1200))) {
	ARMY_SIZE++;
      }
    } else if (!a_isleader(ARMY_TYPE)) {
      MERCMEN += (ARMY_SIZE * rate_merc) / 1200;
      ARMY_SIZE += (ARMY_SIZE * rate_inc) / 1200;
    }
  }

  /* adjust all of the navies */
  for (navy_ptr = ntn_ptr->navy_list;
       navy_ptr != NULL;
       navy_ptr = navy_ptr->next) {
    for (i = 0; i <= N_HEAVY; i++) {
      j = (int) N_CNTSHIPS(NAVY_SHIPS[NSHP_WARSHIPS], i);
      if (j * rate_inc > rand_val(1200)) {
	NAVY_SHIPS[NSHP_WARSHIPS] =
	  navy_addships(NAVY_SHIPS[NSHP_WARSHIPS], i, 1);
      }
    }
  }

  /* now adjust the raw materials slightly */
  for (city_ptr = ntn_ptr->city_list;
       city_ptr != NULL;
       city_ptr = city_ptr->next) {
    for (i = 0; i < MTRLS_NUMBER; i++) {
      if (CITY_MTRLS[i] > 0)  {
	/* slower increase for prizes */
	CITY_MTRLS[i] += (CITY_MTRLS[i] * rate_inc) / 2400;
      }
    }
  }
}

/* variables to keep track of sector types for relocation */
static int veg_type, ele_type;

/* EV_TEST -- Return true if the elevation and vegetation is right */
static int
ev_test PARM_2(int, x, int, y)
{
  int ele;
  if (((ele = sct[x][y].altitude) != ELE_WATER) &&
      (ele != ELE_PEAK) &&
      (major_desg(sct[x][y].designation) != MAJ_WALL)) {
    if ((sct[x][y].owner == country) ||
	(((ele_type == -1) ||
	  (ele_type != ele)) &&
	 ((veg_type == -1) ||
	  (veg_type == sct[x][y].vegetation)))) {
      return(TRUE);
    }
  }
  return (FALSE);
}

/* MN_CNT_TYPE -- Count how many possible sectors can be entered */
static void
mn_cnt_type PARM_2(int, x, int, y)
{
  /* is it a match? */
  if (ev_test(x, y)) {
    global_int++;
  }
}

/* MN_MOVE_IT -- Now, relocate when the correct one is found */
static void
mn_move_it PARM_2(int, x, int, y)
{
  if ((global_int > 0) &&
      (ev_test(x, y))) {
    if (--global_int == 0) {
      ARMY_XLOC = x;
      ARMY_YLOC = y;
    }
  }
}

/* MONSTER_MOVE_ARMY -- Relocate current army unit one sector radius */
void
monster_move_army PARM_2(int, vtype, int, etype)
{
  /* set the search mechanisms */
  veg_type = vtype;
  ele_type = etype;
  global_int = 0;
  map_loop(ARMY_XLOC, ARMY_YLOC, 1, mn_cnt_type);
  if (global_int > 0) {
    if (global_int > 1) global_int = rand_val(global_int) + 1;
    map_loop(ARMY_XLOC, ARMY_YLOC, 1, mn_move_it);
  }
}

/* UPD_LIZARDS -- Update the lizards */
void
upd_lizards PARM_0(void)
{
  /* start with the basics */
  monster_growth();

  /* go through all of the lizard troops */
  for (army_ptr = ntn_ptr->army_list;
       army_ptr != NULL;
       army_ptr = army_ptr->next) {
    /* find the current sector */
    if (!XY_INMAP(ARMY_XLOC, ARMY_YLOC)) continue;
    sct_ptr = &sct[ARMY_XLOC][ARMY_YLOC];

    /* make sure that garrisons are secured */
    if (a_isarcher(ARMY_TYPE) ||
	(ARMY_SIZE < 200)) {
      if (sct_ptr->owner == country) {
	if (fort_val(ARMY_XLOC, ARMY_YLOC)) {
	  set_status(ARMY_STAT, ST_GARRISON);
	} else {
	  monster_move_army(VEG_LT_VEG, -1);
	  set_status(ARMY_STAT, ST_DEFEND);
	}
      } else if (rand_val(2) == 0) {
	set_status(ARMY_STAT, ST_ATTACK);
      } else {
	monster_move_army(VEG_LT_VEG, -1);
	set_status(ARMY_STAT, ST_DEFEND);
      }
    } else {
      if (sct_ptr->owner == country) {
	/* go roaming */
	set_status(ARMY_STAT, ST_DEFEND);
	monster_move_army(VEG_SWAMP, -1);
      } else if (sct_ptr->owner == UNOWNED) {
	set_status(ARMY_STAT, ST_ATTACK);
      } else {
	if (rand_val(3)) {
	  /* go roaming */
	  set_status(ARMY_STAT, ST_DEFEND);
	  monster_move_army(VEG_SWAMP, -1);
	} else {
	  set_status(ARMY_STAT, ST_SWEEP);
	}
      }
    }
  }
}

/* UPD_SAVAGES -- Update the savages */
void
upd_savages PARM_0(void)
{
  /* start with the basics */
  monster_growth();

  /* go through all of the savages */
  for (army_ptr = ntn_ptr->army_list;
       army_ptr != NULL;
       army_ptr = army_ptr->next) {
    /* find the current sector */
    if (!XY_INMAP(ARMY_XLOC, ARMY_YLOC)) continue;
    sct_ptr = &sct[ARMY_XLOC][ARMY_YLOC];

    /* go roaming */
    set_status(ARMY_STAT, ST_ATTACK);
    if (rand_val(3)) {
      monster_move_army(-1, -1);
    }
  }
}

/* UPD_NOMADS -- The nomadic tribes */
void
upd_nomads PARM_0(void)
{
  /* start with the basics */
  monster_growth();

  /* go through all of the nomads */
  for (army_ptr = ntn_ptr->army_list;
       army_ptr != NULL;
       army_ptr = army_ptr->next) {
    /* find the current sector */
    if (!XY_INMAP(ARMY_XLOC, ARMY_YLOC)) continue;
    sct_ptr = &sct[ARMY_XLOC][ARMY_YLOC];

    /* go roaming */
    set_status(ARMY_STAT, ST_SWEEP);
    monster_move_army(-1, ELE_MOUNTAIN);
  }
}

/* UPD_PIRATES -- The vile cutthroats and marauders */
void
upd_pirates PARM_0(void)
{
  /* start with the basics */
  monster_growth();
}
