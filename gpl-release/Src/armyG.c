// SPDX-License-Identifier: GPL-3.0-or-later
/* This file contains routines allowing user manipulation of units */
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

/*
 * THIS FILE FORMERLY extcmds.c
 *
 * additions made in tut.fi (winter 1987 and spring 1988)
 * by mta@tut.fi and esc@tut.fi
 *
 * These commands have been incorporated into the code; and
 * were totally rewritten for version 5.0 due to the extensive
 * changes to the army and global data structures.
 *                        adb@cs.bu.edu
 */
#define USE_CODES
#define PRINT_CODES
#include "dataG.h"
#include "armyX.h"
#include "cityX.h"
#include "navyX.h"
#include "desigX.h"
#include "mtrlsX.h"
#include "worldX.h"
#include "statusX.h"
#include "dstatusX.h"
#include "keyvalsX.h"

/* COMB_ARMIES -- Merge the second army into the first */
static void
comb_armies PARM_2 (ARMY_PTR, a1_ptr, ARMY_PTR, a2_ptr)
{
  int stat1, stat2;
  int spd1, spd2, splyval, effval;

  /* safety check */
  if (a1_ptr == NULL || a2_ptr == NULL) {
    errormsg("Uh oh, I can't find your army!");
    return;
  }
  if (a1_ptr == a2_ptr) {
    errormsg("Hmm... they seem to be combined already");
    return;
  }
  army_ptr = a1_ptr;

  /* check if the units are the same type */
  if (ARMY_TYPE != a2_ptr->unittype) {
    errormsg("Only troops of the same type may be combined");
    return;
  }

  /* check if the type is a leader */
  if (a_isleader(ARMY_TYPE)) {
    errormsg("Leaders exist only as separate units");
    return;
  }
  if (a_isscout(ARMY_TYPE)) {
    errormsg("Scouts only work alone");
    return;
  }
  if (unit_flight(ARMY_STAT) &&
      !unit_flight(a2_ptr->status)) {
    errormsg("You may not combine a landed unit into a unit that is flying");
    return;
  }

  /* check that they are in the same sector */
  if ((ARMY_XLOC != a2_ptr->xloc) || (ARMY_YLOC != a2_ptr->yloc)) {
    errormsg("The units must be in the same sector to be combined");
    return;
  }

  /* find unit statuses */
  stat1 = real_stat(army_ptr);
  stat2 = real_stat(a2_ptr);

  /* check if the stats are the same */
  if ((is_god == FALSE) &&
      (stat1 != stat2 || nolike_stat(stat1))) {

    /* check if status of the first unit prevents merging */
    if (nocomb_stat(stat1)) {
      sprintf(string, "%s %d has a status of %s, and cannot merge",
	      ainfo_list[ARMY_TYPE].name, ARMY_ID,
	      stat_info[stat1].name);
      errormsg(string);
      return;
    }

    /* check if status of the second unit prevents merging */
    if (nocomb_stat(stat2)) {
      sprintf(string, "%s %d has a status of %s, and cannot merge",
	      ainfo_list[a2_ptr->unittype].name,
	      a2_ptr->armyid, stat_info[stat2].name);
      errormsg(string);
      return;
    }

  }

  /* check spell status */
  if (unit_spelled(ARMY_STAT) != unit_spelled(a2_ptr->status)) {
    errormsg("Both units have to be magically enhanced to be combined");
    return;
  }

  /* assign speed settings */
  spd1 = unit_speed(ARMY_STAT);
  spd2 = unit_speed(a2_ptr->status);

  /* get confirmation */
  if (conq_expert == FALSE) {
    mvprintw(LINES - 1, 0, "Merge %s Unit %d into %s Unit %d? ",
	     ainfo_list[a2_ptr->unittype].name, a2_ptr->armyid,
	     ainfo_list[ARMY_TYPE].name, ARMY_ID);
    if (y_or_n() == FALSE) return;
  }

  /* now combine them */
  if ((spd1 == SPD_STUCK) ||
      ((spd1 < spd2) && (spd2 != SPD_STUCK))) {
    set_speed(ARMY_STAT, spd2);
  }
  if (unit_healed(a2_ptr->status)) {
    set_healed(ARMY_STAT);
  }

  /* check the supply levels */
  if (ARMY_SPLY != a2_ptr->supply) {
    splyval = ((int) ARMY_SPLY * ARMY_SIZE) +
      ((int) a2_ptr->supply * a2_ptr->strength);
    ARMY_SPLY = (uns_char) (splyval / (ARMY_SIZE + a2_ptr->strength));
    AADJSPLY;
  }

  /* adjust the efficiency */
  if (ARMY_MAXEFF != a2_ptr->max_eff) {
    effval = ((int) ARMY_MAXEFF * ARMY_SIZE) +
      ((int) a2_ptr->max_eff * a2_ptr->strength);
    ARMY_MAXEFF = (uns_char) (effval / (ARMY_SIZE + a2_ptr->strength));
    AADJMAXEFF;
  }
  if (ARMY_EFF != a2_ptr->efficiency) {
    effval = ((int) ARMY_EFF * ARMY_SIZE) +
      ((int) a2_ptr->efficiency * a2_ptr->strength);
    ARMY_EFF = (uns_char) (effval / (ARMY_SIZE + a2_ptr->strength));
    AADJEFF;
  }

  /* insert the second unit into the first */
  ARMY_SIZE += a2_ptr->strength;
  AADJSIZE;
  if (a2_ptr->spellpts > 0) {
    if ((int) ARMY_SPTS + (int) a2_ptr->spellpts > 200) {
      ARMY_SPTS = 200;
    } else {
      ARMY_SPTS += a2_ptr->spellpts;
    }
    AADJSPTS;
  }
  if (ARMY_MOVE > a2_ptr->umove) {
    /* reduce movement of unit */
    ARMY_MOVE = a2_ptr->umove;
  }

  /* check if it is grouped */
  if ((unit_status(ARMY_STAT) == ST_GROUPED) &&
      ((army_tptr = armybynum(ARMY_LEAD)) != NULL)) {

    /* set the movement value of a group */
    set_grpmove(ARMY_LEAD, unit_speed(ARMY_STAT), ARMY_MOVE);
    army_ptr = army_tptr;
    GADJMOVE;

  } else if (unit_leading(ARMY_STAT)) {

    /* properly adjust the movement of the group */
    set_grpmove(ARMY_ID, unit_speed(ARMY_STAT), ARMY_MOVE);
    GADJMOVE;

  } else {
    AADJSTAT;
    AADJMOVE;
  }

  /* nuke the second unit */
  army_ptr = a2_ptr;
  ADESTROY;
  dest_army(ARMY_ID);
  army_ptr = NULL;
}

/* CHANGE_ASTATUS -- Adjust the status of an army unit */
static void
change_astatus PARM_2(ARMY_PTR, a1_ptr, int, new_stat)
{
  /* get the army */
  if (a1_ptr == NULL) {
    errormsg("Hmm, I can't seem to find that army anywhere");
    return;
  }

  /* find the army status */
  army_ptr = a1_ptr;

  /* special check for when sieged */
  if (real_stat(army_ptr) == ST_SIEGED) {

    /* check for going onto sortie */
    if (new_stat == ST_ATTACK) {
      bottommsg("Do you wish to launch an irrevocable sortie? ");
      if (y_or_n() == FALSE) {
	return;
      }
      new_stat = ST_SORTIE;
    }

  }

  /* location dependent statuses make armies stay in place */
  if ((is_god == FALSE) && (ARMY_MOVE != 0)) {
    if (nomove_stat(new_stat)) {
      if (ARMY_MOVE > 25) {
	ARMY_MOVE -= 25;
      } else {
	ARMY_MOVE = 0;
      }
    } else {
      if (ARMY_MOVE > 10) {
	ARMY_MOVE -= 10;
      } else {
	ARMY_MOVE = 0;
      }
    }
    if (unit_leading(ARMY_STAT)) {
      set_grpmove(ARMY_ID, unit_speed(ARMY_STAT), ARMY_MOVE);
      GADJMOVE;
    } else {
      AADJMOVE;
    }
  }

  /* first check if it was a grouped unit */
  if ((unit_status(ARMY_STAT) == ST_GROUPED) &&
      ((army_tptr = armybynum(ARMY_LEAD)) != NULL)) {

    /* inherit old info and automatically ungroup */
    set_status(ARMY_STAT, new_stat);
    set_speed(ARMY_STAT, unit_speed(ARMYT_STAT));
    ARMY_LASTX = ARMYT_LASTX;
    ARMY_LASTY = ARMYT_LASTY;
    AADJOLOC;

    /* is the leader still leading? */
    if (check_leader(ARMY_LEAD) == FALSE) {

      /* adjust the leader status; saving army_ptr value */
      ARMY_PTR atmp_ptr = army_ptr;
      army_ptr = army_tptr;
      set_unlead(ARMY_STAT);
      AADJSTAT;
      army_ptr = atmp_ptr;

    }

  } else {
    /* just set the status */
    set_status(ARMY_STAT, new_stat);
  }

  /* check for speed tracking */
  if ((unit_speed(ARMY_STAT) == SPD_STUCK) &&
      !nomove_stat(ARMY_STAT)) {
    set_speed(ARMY_STAT, SPD_SLOW);
  }
  AADJSTAT;
}

/* CHANGE_ASPEED -- Adjust the movement rate of an army unit */
static void
change_aspeed PARM_2(ARMY_PTR, a1_ptr, int, new_speed)
{
  /* get the army */
  if (a1_ptr == NULL) {
    errormsg("Hmm, I can't seem to find that army anywhere");
    return;
  }

  /* find the army status */
  army_ptr = a1_ptr;

  /* adjust the movement potential */
  if ((is_god == FALSE) && (ARMY_MOVE != 0)) {
    if (ARMY_MOVE > 10) {
      ARMY_MOVE -= 10;
    } else {
      ARMY_MOVE = 0;
    }
    if (!unit_leading(ARMY_STAT)) {
      AADJMOVE;
    }
  }
  set_speed(ARMY_STAT, new_speed);
  if (unit_leading(ARMY_STAT)) {
    set_grpmove(ARMY_ID, unit_speed(ARMY_STAT), ARMY_MOVE);
    GADJMOVE;
  } else {
    AADJSTAT;
  }
}

/* REDUCE_ARMY -- Separate out men from the unit */
static void
reduce_army PARM_2(ARMY_PTR, a1_ptr, long, men)
{
  /* validate input */
  if ((army_ptr = a1_ptr) == NULL) {
    return;
  }
  if (men <= 0) {
    errormsg("Huh?  You want to split how many men?");
    return;
  }

  /* check if the unit is not reducable */
  if (a_isleader(ARMY_TYPE)) {
    errormsg("Eh?  Perhaps I should lop an arm off that leader for you?");
    return;
  }
  if (nosplit_stat(ARMY_STAT)) {
    errormsg("That unit is not able to separate");
    return;
  }

  /* check if there are enough men */
  if (ARMY_SIZE == men) {
    errormsg("That is the entire unit!  Why not just renumber it?");
    return;
  }
  if (ARMY_SIZE < men) {
    errormsg("There are not that many troops in the unit");
    return;
  }

  /* check mercenary limits */
  if ((is_god == FALSE) &&
      a_ismerc(ARMY_TYPE)) {
    if ((men < 10) || (ARMY_SIZE - men < 10)) {
      errormsg("The mercenaries refuse to work in such small units");
      return;
    }
  }

  /* build a new unit */
  if ((army_tptr = crt_army(ARMY_TYPE)) == NULL) {
    errormsg("Another one?!  You have too many armies already!");
    return;
  }
  if (a_castspells(ARMY_TYPE) &&
      (ARMY_SPTS > 0)) {
    ARMYT_SPTS = (ARMY_SPTS * men) / ARMY_SIZE;
    ARMY_SPTS -= ARMYT_SPTS;
    AADJSPTS;
  }
  ARMY_SIZE -= men;
  AADJSIZE;
  army_ptr = army_tptr;
  ACREATE;
  ARMY_MOVE = a1_ptr->umove;
  ARMY_LEAD = a1_ptr->leader;
  ARMY_STAT = a1_ptr->status;
  ARMY_SPLY = a1_ptr->supply;
  ARMY_XLOC = a1_ptr->xloc;
  ARMY_LASTX = a1_ptr->lastx;
  ARMY_YLOC = a1_ptr->yloc;
  ARMY_LASTY = a1_ptr->lasty;
  ARMY_EFF = a1_ptr->efficiency;
  ARMY_MAXEFF = a1_ptr->max_eff;
  ARMY_SIZE = men;
  AADJSTAT;
  AADJLEAD;
  AADJSPLY;
  AADJSIZE;
  AADJMAXEFF;
  AADJEFF;
  AADJLOC;
  AADJOLOC;
  AADJMOVE;
  if (ARMY_SPTS > 0) {
    AADJSPTS;
  }
  army_sort(FALSE);
}

/* SPLIT_ARMY -- Separate a specified number of men from the unit */
static void
split_army PARM_1(ARMY_PTR, a1_ptr)
{
  long men;

  /* query amount */
  mvaddstr(LINES - 1, 0, "How many men to split from unit? ");
  clrtoeol();
  refresh();
  men = get_number(FALSE);

  /* check for valid input */
  if (no_input == TRUE) {
    return;
  }

  /* now reduce the unit */
  reduce_army(a1_ptr, men);
}

/* ADD_GROUP -- Add an army to a group */
static void
add_group PARM_2(ARMY_PTR, a1_ptr, int, use_selector)
{
  ARMY_PTR a2_ptr;
  int group, spd1, spd2, stat1, stat2;

  /* check input */
  if (a1_ptr == NULL) return;
  army_ptr = a1_ptr;

  /* check if the stat is adjustable */
  stat1 = real_stat(army_ptr);
  if (nocomb_stat(stat1)) {
    sprintf(string, "Unit %d has a status of %s and cannot be grouped",
	    ARMY_ID, stat_info[unit_status(ARMY_STAT)].name);
    errormsg(string);
    return;
  }
  if (a_isscout(ARMY_STAT)) {
    errormsg("Your scout must work independently");
    return;
  }

  /* query as to what unit will lead */
  if (use_selector) {
    /* set up the selector */
    if (set_select(TRUE, "Select a group leader for this unit") == FALSE) {
      return;
    }
    if ((a2_ptr = get_armyselect()) == NULL) {
      errormsg("You must select a leader unit to group under");
      return;
    }
    group = a2_ptr->armyid;
  } else {
    mvaddstr(LINES - 1, 0, "Group under which leader? ");
    clrtoeol();
    refresh();
    group = get_number(FALSE);
    if (no_input == TRUE) return;

    /* find the leader unit */
    if ((a2_ptr = armybynum(group)) == NULL) {
      errormsg("There is no such unit");
      return;
    }
  }

  /* now check the information */
  if (a1_ptr == a2_ptr) {
    errormsg("How can you group a unit under itself?");
    return;
  }

  /* check if it is a leader */
  if (!a_isleader(a2_ptr->unittype)) {
    errormsg("That unit is not a leader");
    return;
  }

  /* check location */
  if ((ARMY_XLOC != a2_ptr->xloc) || (ARMY_YLOC != a2_ptr->yloc)) {
    errormsg("The unit must group under a leader in the same sector");
    return;
  }
  if (!unit_flight(ARMY_STAT) &&
      unit_flight(a2_ptr->status)) {
    errormsg("You may not group a landed unit under a leader that is flying");
    return;
  }

  /* disallow grouping onto a transport */
  stat2 = unit_status(a2_ptr->status);
  if (onship_stat(stat2)) {
    errormsg("You are unable to join a group on a transport");
    return;
  }
  if (stat2 == ST_GROUPED) {
    errormsg("You may not group under a unit which is already grouped");
    return;
  }

  /* compare speeds */
  spd1 = unit_speed(ARMY_STAT);
  spd2 = unit_speed(a2_ptr->status);

  /* set the speeds */
  if ((spd1 == SPD_STUCK) ||
      ((spd1 < spd2) && (spd2 != SPD_STUCK))) {
    set_speed(ARMY_STAT, spd2);
  }

  /* now do all the fixing */
  if (a2_ptr->umove < ARMY_MOVE) {
    ARMY_MOVE = a2_ptr->umove;
    AADJMOVE;
  } else if (a2_ptr->umove > ARMY_MOVE) {
    a2_ptr->umove = ARMY_MOVE;
  }
  set_status(ARMY_STAT, ST_GROUPED);
  AADJSTAT;
  ARMY_LEAD = group;
  AADJLEAD;
  army_ptr = a2_ptr;
  set_leading(ARMY_STAT);
  AADJSTAT;
  set_grpmove(group, unit_speed(ARMY_STAT), ARMY_MOVE);
  GADJMOVE;
}

/* DISB_ARMY -- Get rid of an army unit */
void
disb_army PARM_1 (ARMY_PTR, a1_ptr)
{
  ITEM_STRUCT supply_items;
  long num_peop = 0;
  int cnt, limited = FALSE, xloc, yloc;

  /* verify the unit */
  if (a1_ptr == NULL) return;
  army_ptr = a1_ptr;

  /* check if the disband is possible */
  if (is_god == FALSE) {
    if (a_isleader(ARMY_TYPE)) {
      errormsg("Your leader doesn't seem to like the idea of suicide");
      return;
    }
    if (a_isnormal(ARMY_TYPE) &&
	!a_anydisband(ARMY_TYPE) &&
	(sct[ARMY_XLOC][ARMY_YLOC].owner != country)) {
      errormsg("Soldiers must disband in their own territory");
      return;
    }
    if ((city_ptr =
	 citybyloc(ntn_ptr, ntn_ptr->capx, ntn_ptr->capy)) == NULL) {
      errormsg("Your nation is to disorganized to issue disband orders");
      return;
    }
  }

  /* question the disband */
  clear_bottom(0);
  if (conq_expert == FALSE) {
    if ((is_god == FALSE) &&
	a_needpay(ARMY_TYPE)) {
      mvprintw(LINES - 4, 0, "Pay %s %d %ld talons to be quiet? ",
	       ainfo_list[ARMY_TYPE].name,
	       ARMY_ID,
	       ainfo_list[ARMY_TYPE].tal_enlist * ARMY_SIZE );
    } else {
      mvprintw(LINES - 2, 0, "Disband your %s #%d? ",
	       ainfo_list[ARMY_TYPE].name, ARMY_ID);
    }
    if (y_or_n() == FALSE) return;
  }

  /* now disband the unit */
  if (is_god == FALSE) {

    /* check for civilian population influx */
    if (a_isnormal(ARMY_TYPE) &&
	!a_anydisband(ARMY_TYPE)) {

      /* try to give back the supplies */
      if ((ARMY_SPLY > 0) &&
	  !a_freesupply(ARMY_TYPE)) {

	/* find the values */
	army_support(army_ptr, &(supply_items.mtrls[0]), ARMY_SPLY);
	limited = real_stat(army_ptr);
	if (limited == ST_SIEGED) {
	  limited = TRUE;
	} else {
	  limited = FALSE;
	}

	/* try to give them back */
	if (send_resources(ARMY_XLOC, ARMY_YLOC, &(supply_items), limited)) {
	  bottommsg("Their supplies will be lost.  Continue? ");
	  if (y_or_n() == FALSE) return;
	  ARMY_SPLY = 0;
	}

      }

      /* the people go into the populous */
      xloc = XREAL;
      yloc = YREAL;
      if (!a_isundead(ARMY_TYPE)) {
	global_int = (PMERCVAL * ARMY_SIZE) / 100;
	if (a_halfmen(ARMY_TYPE)) {
	  global_int /= 2;
	}
	if (global_int < 1) global_int = 1;
	ARMY_SIZE -= global_int;
	AADJMERC;
	xcurs = ARMY_XLOC - xoffset;
	ycurs = ARMY_YLOC - yoffset;

	/* set the population of the sector */
	if ((num_peop = ARMY_SIZE) > 0) {
	  sct[XREAL][YREAL].people += num_peop;
	  SADJPEOP;
	}

      }

      /* now give back the resources */
      if ((ARMY_SPLY > 0) &&
	  !a_freesupply(ARMY_TYPE)) {
	GIVESTART;
	for (cnt = 0; cnt < MTRLS_NUMBER; cnt++) {
	  if ((global_long = supply_items.mtrls[cnt]) > 0) {
	    GIVEMTRLS(cnt);
	  }
	}
	GIVEFINISH(limited);
      }

      /* reset current position */
      xcurs = xloc - xoffset;
      ycurs = yloc - yoffset;

      /* perhaps add some soldiers into the recruiting pool */
      if (!a_halfmen(ARMY_TYPE) &&
	  !a_isundead(ARMY_TYPE) &&
	  (city_ptr = citybyloc(ntn_ptr, ARMY_XLOC, ARMY_YLOC)) != NULL) {
	if (CITY_PEOPLE >= 0) {
	  CITY_PEOPLE += num_peop / 2;
	  CADJPEOP;
	}
      }

    } else if (a_ismerc(ARMY_TYPE)) {
      /* direct increase of the mercenary pool */
      global_int = ARMY_SIZE;
      AADJMERC;
    }

    /* check for pay offs */
    if (a_needpay(ARMY_TYPE)) {
      global_int = MTRLS_TALONS;
      CITY_MTRLS[MTRLS_TALONS] -=
	(ainfo_list[ARMY_TYPE].tal_enlist * ARMY_SIZE);
      CITY_IMTRLS[MTRLS_TALONS] -=
	(ainfo_list[ARMY_TYPE].tal_enlist * ARMY_SIZE);
      if (CITY_IMTRLS[MTRLS_TALONS] < 0) {
	CITY_IMTRLS[MTRLS_TALONS] = 0;
      }
      CADJMTRLS(global_int);
      CADJIMTRLS(global_int);
    }
  }
  ADESTROY;
  dest_army(ARMY_ID);
  army_ptr = NULL;
  hex_recalc();
}

/* SUPPLY_ARMY -- Attempt to set the supply value of an army unit */
int
supply_army PARM_3(ARMY_PTR, a1_ptr, int, level, int, doquery)
{
  ARMY_PTR ahold_ptr = army_ptr;
  ITEM_PTR itest_ptr;
  ITEM_STRUCT supt_cost;
  int oxloc = XREAL, oyloc = YREAL;
  int insectonly = FALSE, cnt, ok_val, diff, hold = TRUE;

  /* check input */
  if (a1_ptr == NULL) {
    errormsg("I can't find that army unit");
    return(hold);
  }
  if ((level < 0) ||
      (level > MAXSUPPLIES)) {
    errormsg("Army units may not carry that many supplies");
    return(hold);
  }

  /* check the difference */
  army_ptr = a1_ptr;
  diff = level - ARMY_SPLY;
  if (diff == 0) {
    /* already set */
    hold = FALSE;
    goto bug_out;
  }

  /* check if they really need them */
  if (a_freesupply(ARMY_TYPE)) {
    sprintf(string, "Your %s %d does not need any supplies",
	    ainfo_list[ARMY_TYPE].name, ARMY_ID);
    hold = FALSE;
    goto bug_out;
  }

  /* now allow it */
  if (is_god == FALSE) {

    /* check if they can get them anywhere */
    if (splyinsect_stat(ARMY_STAT)) {
      insectonly = TRUE;      
    }

    /* check for unwilling give backs */
    if ((diff < 0) &&
	(a_ismonster(ARMY_TYPE) ||
	 a_needpay(ARMY_TYPE))) {
      sprintf(string, "Your %s %d refuses to give back their pay",
	      ainfo_list[ARMY_TYPE].name, ARMY_ID);
      errormsg(string);
      goto bug_out;
    }

    /* get the costs */
    army_support(army_ptr, &(supt_cost.mtrls[0]), abs(diff));

    /* check if we are giving or getting */
    if (diff > 0) {

      /* check for unreachable supplying */
      if (sct[ARMY_XLOC][ARMY_YLOC].owner != country) {
	insectonly = TRUE;      
      }

      /* now check that they are available */
      if ((itest_ptr = find_resources(ARMY_XLOC, ARMY_YLOC, insectonly))
	  == NULL) {
	if (insectonly) {
	  sprintf(string, "%s %d can only get resources from that sector",
		  ainfo_list[ARMY_TYPE].name, ARMY_ID);
	} else {
	  sprintf(string, "%s %d cannot find enough available resources",
		  ainfo_list[ARMY_TYPE].name, ARMY_ID);
	}
	errormsg(string);
	goto bug_out;
      }

      /* now compare the values */
      ok_val = TRUE;
      for (cnt = 0; cnt < MTRLS_NUMBER; cnt++) {
	if (itest_ptr->mtrls[cnt] < supt_cost.mtrls[cnt]) {
	  sprintf(string, "There are not %ld %s for %s %d",
		  (long) supt_cost.mtrls[cnt],
		  mtrls_info[cnt].lname,
		  ainfo_list[ARMY_TYPE].name,
		  ARMY_ID);
	  errormsg(string);
	  ok_val = FALSE;
	}
      }

      /* clean up */
      if (ok_val == FALSE) {
	free(itest_ptr);
	goto bug_out;
      }

      /* query about it */
      if (doquery &&
	  !conq_expert) {
	char tmp_str[LINELTH];
	strcpy(string, "Supplies: ");
	for (cnt = 0; cnt < MTRLS_NUMBER; cnt++) {
	  if (supt_cost.mtrls[cnt] > 0) {
	    sprintf(tmp_str, " %.0f %s",
		    (double) supt_cost.mtrls[cnt],
		    mtrls_info[cnt].lname);
	    strcat(string, tmp_str);
	  }
	}
	strcat(string, "; Continue? ");
	bottommsg(string);
	if (y_or_n() == FALSE) {
	  free(itest_ptr);
	  goto bug_out;
	}
      }

      /* take it away */
      xcurs = ARMY_XLOC - xoffset;
      ycurs = ARMY_YLOC - yoffset;
      TAKESTART;
      for (cnt = 0; cnt < MTRLS_NUMBER; cnt++) {
	if ((global_long = supt_cost.mtrls[cnt]) > 0) {
	  TAKEMTRLS(cnt);
	}
      }
      TAKEFINISH(insectonly);
      xcurs = oxloc - xoffset;
      ycurs = oyloc - yoffset;
      take_resources(ARMY_XLOC, ARMY_YLOC, &supt_cost, insectonly);
      free(itest_ptr);

    } else {

      /* otherwise, just redistribute them in */
      if (send_resources(ARMY_XLOC, ARMY_YLOC, &supt_cost, insectonly)) {
	sprintf(string, "The supplies of %s %d would be lost",
		ainfo_list[ARMY_TYPE].name, ARMY_ID);
	errormsg(string);
	goto bug_out;
      }
      xcurs = ARMY_XLOC - xoffset;
      ycurs = ARMY_YLOC - yoffset;
      GIVESTART;
      for (cnt = 0; cnt < MTRLS_NUMBER; cnt++) {
	if ((global_long = supt_cost.mtrls[cnt]) > 0) {
	  GIVEMTRLS(cnt);
	}
      }
      GIVEFINISH(insectonly);
      xcurs = oxloc - xoffset;
      ycurs = oyloc - yoffset;

    }

  }
  ARMY_SPLY = level;
  AADJSPLY;

  /* reset before bugging out */
  hold = FALSE;
 bug_out:
  army_ptr = ahold_ptr;
  return(hold);
}

/* ASTAT_OK -- Is the army status alright to implement? */
static int
astat_ok PARM_2(int, new_stat, int, verbal)
{
  int old_stat, sown;
  Diplotype dstat;

  /* check the army */
  if (army_ptr == NULL) {
    if (verbal) {
      errormsg("Hmm, I can't seem to find that army anywhere");
    }
    return(FALSE);
  }

  /* find the army status */
  old_stat = real_stat(army_ptr);

  /* scouts stats must stay the same */
  if (a_isscout(ARMY_TYPE)) {
    if (verbal) {
      errormsg("Scouts may not adjust their status");
    }
    return(FALSE);
  } else if ((new_stat == ST_SWEEP) &&
	     a_coverbonus(ARMY_TYPE)) {
    if (verbal) {
      errormsg("Covering units are not capable of sweeping maneuvers");
    }
    return(FALSE);
  }

  /* compare the two */
  if (old_stat == new_stat) {
    if (verbal) {
      errormsg("Excuse me, aren't we doing that already?");
    }
    return(FALSE);
  }
  if (is_god == TRUE) {
    /* god can do anything! */
    return(TRUE);
  }

  /* check for unalterable status */
  if (nochange_stat(old_stat)) {

    /* special cases when sieged */
    if (old_stat == ST_SIEGED) {

      /* check for going onto sortie */
      if (new_stat == ST_ATTACK) {
	/* attack status possible */
	return(TRUE);
      }

      /* may switch out for sortie or rule */
      if ((new_stat != ST_SORTIE) && (new_stat != ST_RESERVE)) {
	if (verbal) {
	  errormsg("A besieged unit may only sortie or go on reserve");
	}
	return(FALSE);
      }

    } else {

      /* the status may not be adjusted */
      if (verbal) {
	sprintf(string, "%s %d has a status of %s and cannot be adjusted",
		ainfo_list[ARMY_TYPE].name, ARMY_ID,
		stat_info[old_stat].name );
	errormsg(string);
      }
      return(FALSE);

    }
  }

  /* quick referencing */
  sct_ptr = &(sct[ARMY_XLOC][ARMY_YLOC]);
  sown = sct_ptr->owner;
  if ((ntn_tptr = world.np[sown]) == NULL) {
    sown = UNOWNED;
    dstat = DIP_NEUTRAL;
  } else {
    dstat = ntn_tptr->dstatus[country] % dstatus_number;
  }

  /* check garrision possibilities */
  if (new_stat == ST_GARRISON) {
    if ((sown == UNOWNED) ||
	((sown != country) && (dstat != DIP_ALLIED)) ||
	(fort_val(ARMY_XLOC, ARMY_YLOC) == 0)) {
      if (verbal) {
	errormsg("We don't have a fortification there to garrison");
      }
      return(FALSE);
    }
  } else if (new_stat == ST_SIEGE) {
    if ((sown == country) ||
	((ntn_ptr->dstatus[sown]) % dstatus_number < DIP_HOSTILE) ||
	(fort_val(ARMY_XLOC, ARMY_YLOC) == 0)) {
      if (verbal) {
	errormsg("You want to lay siege to this sector??");
      }
      return(FALSE);
    }
  } else if (new_stat == ST_RESERVE) {
    if ((sown == UNOWNED) ||
	((sown != country) && (dstat != DIP_ALLIED)) ||
	(fort_val(ARMY_XLOC, ARMY_YLOC) == 0) ||
	(major_desg(sct_ptr->designation) < MAJ_STOCKADE)) {
      if (verbal) {
	errormsg("Reserves are only possible in fortified supply centers");
      }
      return(FALSE);
    }
  } else if ((new_stat == ST_AMBUSH) ||
	     (new_stat == ST_ENGAGE)) {
    if ((sown != country) && (dstat != DIP_ALLIED)) {
      if (verbal) {
	if (new_stat == ST_AMBUSH) {
	  errormsg("You may only set ambushes in territory you control");
	} else {
	  errormsg("You may only engage within territory you control");
	}
      }
      return(FALSE);
    }
  }
  return(TRUE);
}

/* ASPEED_OK -- Is the given army speed possible for the unit */
static int
aspeed_ok PARM_2(int, new_speed, int, verbal)
{
  /* check the army */
  if (army_ptr == NULL) {
    if (verbal) {
      errormsg("Hmm, I can't seem to find that army anywhere");
    }
    return(FALSE);
  }

  /* is it a movable unit? */
  if (nomove_stat(ARMY_STAT)) {
    if (verbal) {
      errormsg("That unit is stopped, its speed is not adjustable");
    }
    return(FALSE);
  }
  if ((ARMY_MOVE < 25) &&
      (unit_speed(ARMY_STAT) > new_speed)) {
    if (verbal) {
      errormsg("The unit has gone too far to reduce speed");
    }
    return(FALSE);
  }
  if (unit_speed(ARMY_STAT) == new_speed) {
    if (verbal) {
      errormsg("The unit is already moving at that speed");
    }
    return(FALSE);
  }
  return(TRUE);
}

/* EXT_ARMYINFO -- Provide a bit of information about the given army unit */
static void
ext_armyinfo PARM_1 (ARMY_PTR, a1_ptr)
{
  ITEM_STRUCT supt_cost;
  int i;
  long str_val;

  /* check input */
  if (a1_ptr == NULL) return;
  army_ptr = a1_ptr;

  /* just provide the information about the unit */
  clear_bottom(0);
  move(LINES - 5, 0);

  /* give the unit description */
  if (a_isleader(ARMY_TYPE)) {
    addstr("Leader");
  } else if (a_ismonster(ARMY_TYPE)) {
    addstr("Monster");
  } else {
    addstr("Army");
  }
  printw(" Unit %d ==>", ARMY_ID);
  printw(" Type: %s;", ainfo_list[ARMY_TYPE].name);
  if (!a_isleader(ARMY_TYPE)) {
    printw(" Size: %ld;", ARMY_SIZE);
  }
  if (a_ismonster(ARMY_TYPE)) {
    str_val = ARMY_SIZE * ainfo_list[ARMY_TYPE].minsth;
  } else {
    str_val = ARMY_SIZE;
  }
  printw(" Strength: %ld %s;", str_val, (str_val == 1)?"man":"men");
  if (a_isleader(ARMY_TYPE) ||
      a_castspells(ARMY_TYPE)) {
    printw(" Power: %d;", ARMY_SPTS);
  }

  /* line two */
  move(LINES - 4, 0);
  printw("  Status: %s;", stat_info[unit_status(ARMY_STAT)].name);
  if (unit_flight(ARMY_STAT)) {
    sprintf(string, "Fly %s",  speedname[unit_speed(ARMY_STAT)]);
  } else {
    strcpy(string, speedname[unit_speed(ARMY_STAT)]);
  }
  printw(" Speed: %s;", string);
  printw(" Move Left: %d%%;", (int) ARMY_MOVE);
  printw(" Move Ability: %d;", army_mvpts(ntn_ptr, army_ptr));

  /* line three */
  move(LINES - 3, 0);
  printw("  Health: %d%%;", (int)ARMY_EFF);
  printw(" Location: [%d,%d];",
	 xloc_relative(ARMY_XLOC),
	 yloc_relative(ARMY_YLOC));
  addstr(" Traits: ");
  i = 0;
  if (unit_leading(ARMY_STAT)) {
    i = 1;
    addstr("leading");
  }
  if (unit_spelled(ARMY_STAT)) {
    if (i == 1) addstr(", ");
    i = 1;
    addstr("spelled");
  }
  if (unit_healed(ARMY_STAT)) {
    if (i == 1) addstr(", ");
    i = 1;
    addstr("healing");
  }
  if (i == 0) {
    addstr("none");
  }
  addstr(";");

  /* line four */
  move(LINES - 2, 0);
  if (a_freesupply(ARMY_TYPE)) {
    printw("  Supply Level: free; Supply Cost: none;");
  } else {
    printw("  Supply Level: %d turns;", (int) ARMY_SPLY);
    addstr(" Supply Cost: ");
    army_support(army_ptr, &(supt_cost.mtrls[0]), 1);
    str_val = 0;
    for (i = 0; i < MTRLS_NUMBER; i++) {
      if (supt_cost.mtrls[i] > 0) {
	if (str_val == 1) addstr(", ");
	str_val = 1;
	printw("%.0f %s", (double) supt_cost.mtrls[i],
	       mtrls_info[i].lname);
      }
    }
    addstr(" per turn;");
  }

  /* done */
  presskey();
}

/* EXT_ARMYCMD -- Perform an extended army operation */
void
ext_armycmd PARM_1 (int, armie)
{
  ARMY_PTR ahold_ptr, a2_ptr;
  static int last_ch;
  int i, x, y;
  int astat, men = 0;

  /* find target army unit */
  if (armie == -1) {
    army_ptr = get_armyselect();
    if ((country == UNOWNED) &&
	get_god(global_int, TRUE)) {
      return;
    }
    if (army_ptr != NULL) {
      goto_army(ARMY_ID);
    }
  } else {
    army_ptr = armybynum(armie);
  }
  if (army_ptr == NULL) {
    errormsg("I could not find that army unit");
    return;
  }

  /* Display first line of possible adjustment commands */
  clear_bottom(0);
  y = LINES - 5;
  x = 1;
  move(y++, x);
  hip_string("? Info, ", 1, TRUE);
  hip_string("+ Combine, ", 1, !nocomb_stat(ARMY_STAT) &&
	     !a_isleader(ARMY_TYPE) && (armie == -1));
  hip_string("Merge, ", 1, !nocomb_stat(ARMY_STAT) &&
	     !a_isleader(ARMY_TYPE));
  hip_string("- Split, ", 1, !a_isleader(ARMY_TYPE));
  hip_string("/ Split 1/2, ", 1, !a_isleader(ARMY_TYPE));
  hip_string("Ungroup, ", 1, (unit_status(ARMY_STAT) == ST_GROUPED));
  hip_string("Group, ", 1, TRUE);

  /* next line of commands */
  move(y++, x);
  hip_string("# Renum, ", 1, TRUE);
  for (i = 0; i < SPD_STUCK; i++) {
    sprintf(string, "%s %s, ", shortspeed[i], speedname[i]);
    hip_string(string, 1, aspeed_ok(i, FALSE));
  }
  hip_string("Disband, ", 1, !a_isleader(ARMY_TYPE));
  hip_string("Supply, ", 1, !a_freesupply(ARMY_TYPE) &&
	     (MAXSUPPLIES > 0));
  hip_string("! Adj Ldr, ", 1, !a_isleader(ARMY_TYPE) &&
	     (unit_status(ARMY_STAT) != ST_GROUPED));

  /* now for the statuses */
  move(y++, x);
  for (i = 0; i < num_statuses; i++) {
    if (usable_stat(i) &&
	aunit_stat(i)) {

      /* check for overruns */
      if (x + strlen(stat_info[i].input) > COLS - 8) {
	addstr(",");
	x = 1;
	move(y++, x);
      }

      /* display the string */
      if (x != 1) {
	addstr(", ");
	x += 2;
      }
      x += strlen(stat_info[i].input) + 2;
      hip_string(stat_info[i].input,
		 stat_info[i].key_char,
		 astat_ok(i, FALSE));

    }
  }

  /* now give the prompt */
  if (armie == -1) {
    mvaddstr(y, 0, "Extended Army Command: ");
  } else {
    mvaddstr(y, 0, "Army Adjustment Command: ");
  }
  refresh();

  switch (last_ch = next_char()) {
  case '?':
    /* the army information function */
    ext_armyinfo(army_ptr);
    break;
  case '!':
    /* check if the troop is being lead */
    if (unit_status(ARMY_STAT) == ST_GROUPED) {
      errormsg("A grouped unit must be lead by its group leader!");
      break;
    }

    /* find out who should lead */
    clear_bottom(0);
    mvaddstr(LINES - 1, 0, "Which leader will this unit follow? ");
    clrtoeol();
    men = get_number(FALSE);
    if (no_input != TRUE) {

      /* validate the leader number */
      if ((army_tptr = armybynum(men)) == NULL) {
	errormsg("There is no such unit");
      } else if (!a_isleader(ARMYT_TYPE)) {
	errormsg("That unit is not a leader!");
      } else {
	ARMY_LEAD = men;
	AADJLEAD;
      }

    }
    break;
  case '-':
    /* split army */
    ahold_ptr = army_ptr;
    split_army(army_ptr);
    if (armie == (-1)) {
      goto_army(ahold_ptr->armyid);
    }
    break;
  case '/':
    /* divide army */
    men = ARMY_SIZE / 2;
    ahold_ptr = army_ptr;
    reduce_army(army_ptr, men);
    if (armie == (-1)) {
      goto_army(ahold_ptr->armyid);
    }
    break;
  case '+':
    /* combine armies */
    if (armie != -1) {
      errormsg("Huh?  I have no idea what you are talking about.");
      break;
    }
    selector += 2;
    a2_ptr = get_armyselect();
    selector -= 2;
    if (a2_ptr == NULL) {
      errormsg("There are no following army units to combine with");
      break;
    }
    ahold_ptr = army_ptr;
    comb_armies(army_ptr, a2_ptr);
    if (armie == (-1)) {
      goto_army(ahold_ptr->armyid);
    }
    break;
  case 'M':
    /* check the basics */
    if (a_isleader(ARMY_TYPE)) {
      errormsg("Leaders exist only as separate units");
      break;
    }
    if (a_isscout(ARMY_TYPE)) {
      errormsg("Scouts only work alone");
      break;
    }

    /* select a unit to merge into */
    if (armie == (-1)) {

      /* set the selector properly */
      if (set_select(TRUE, "Choose an Army Unit to Merge With") == FALSE) {
	break;
      }

      /* now retrieve the information */
      if ((a2_ptr = get_armyselect()) == NULL) {
	errormsg("That is not an Army Unit");
	goto_army(ARMY_ID);
	break;
      }

    } else {

      /* ask for the new unit */
      mvaddstr(LINES - 1, 0, "Merge with what Army Unit number? ");
      clrtoeol();
      refresh();

      /* now get it */
      men = get_number(FALSE);
      if (no_input) break;
      a2_ptr = armybynum(men);

    }

    /* verify it */
    if (a2_ptr == NULL) {
      errormsg("There is no such unit");
      break;
    }
    if (army_ptr == a2_ptr) {
      errormsg("That is the same unit");
      break;
    }
    ahold_ptr = army_ptr;
    comb_armies(army_ptr, a2_ptr);
    if (armie == (-1)) {
      goto_army(ahold_ptr->armyid);
    }
    break;
  case '>':
  case '<':
  case '=':
    /* find which speed it is */
    for (astat = 0; astat < SPD_STUCK; astat++) {
      if (shortspeed[astat][0] == last_ch) break;
    }
    if (astat == SPD_STUCK) {
      errormsg("Invalid Input");
      break;
    }

    /* check if the speed is possible */
    if (aspeed_ok(astat, TRUE)) {
      change_aspeed(army_ptr, astat);
    }
    break;
  case '#':
    /* change the unit identifier */
    mvaddstr(LINES - 1, 0, "Enter a new unit id? ");
    clrtoeol();
    refresh();
    men = get_number(FALSE);
    if (no_input == TRUE) break;

    /* validate the number */
    if (men >= MAX_IDTYPE || men == EMPTY_HOLD) {
      errormsg("That is an invalid unit number.");
      break;
    } else if (armybynum(men) != NULL) {
      errormsg("That number is already in use.");
      break;
    }
    global_int = ARMY_ID;
    army_renum(men);
    AADJID;
    army_sort(FALSE);
    if (armie == -1) goto_army(men);
    break;
  case 'D':
    /* disband a unit */
    disb_army(army_ptr);
    break;
  case 'U':
    /* ungroup a unit */
    if (unit_status(ARMY_STAT) != ST_GROUPED) {
      errormsg("That unit is not in a group");
      break;
    }

    /* now be very sure that it is not on a ship */
    army_tptr = armybynum(ARMY_LEAD);
    if (army_tptr == NULL) {
      set_status(ARMY_STAT, ST_DEFEND);
    } else {
      /* get the real status */
      if (onship_stat(men = group_stat(ARMY_LEAD))) {
	errormsg("Troops must be unloaded from the transport!");
	break;
      }
      set_status(ARMY_STAT, men);
      if (unit_flight(ARMYT_STAT)) {
	set_flight(ARMY_STAT);
      }
      if ((is_god == FALSE) &&
	  (ARMY_MOVE != 0)) {
	if (ARMY_MOVE > 10) {
	  ARMY_MOVE -= 10;
	} else {
	  ARMY_MOVE = 0;
	}
	if (unit_leading(ARMY_STAT)) {
	  set_grpmove(ARMY_ID, unit_speed(ARMY_STAT), ARMY_MOVE);
	  GADJMOVE;
	} else {
	  AADJMOVE;
	}
      }

      /* preserve relocation information */
      ARMY_LASTX = ARMYT_LASTX;
      ARMY_LASTY = ARMYT_LASTY;
      AADJOLOC;

      /* check if the leader is still leading */
      if (check_leader(ARMY_LEAD) == FALSE) {

	/* adjust the leader status; saving army_ptr value */
	a2_ptr = army_ptr;
	army_ptr = army_tptr;
	set_unlead(ARMY_STAT);
	AADJSTAT;
	army_ptr = a2_ptr;

      }
    }
    AADJSTAT;
    break;
  case 'G':
    add_group(army_ptr, (armie == -1));
    break;
  case 'S':
  case 's':
    /* set the supply level of the unit */
    if (MAXSUPPLIES == 0) {
      errormsg("Supply rules are not in effect for this campaign");
      break;
    }
    if (a_freesupply(ARMY_TYPE)) {
      errormsg("That unit does not need any supplies");
      break;
    }

    /* get the value */
    mvprintw(LINES - 1, 0, "Set supply to what value? [default=%d] ",
	     ARMY_SPLY);
    men = get_number(FALSE);

    /* check the input */
    if ((no_input == TRUE) || (men == ARMY_SPLY)) break;
    if (men < 0) {
      errormsg("Wow, negative supplies... why not just kill 'em?");
      break;
    }
    if (men > MAXSUPPLIES) {
      errormsg("They cannot carry that many supplies!");
      break;
    }
    supply_army(army_ptr, men, TRUE);
    break;
  case ' ':
  case '\n':
  case '\r':
    /* peaceful exit on space */
    break;
  default:
    /* army status? */
    for (astat = 0; astat < num_statuses; astat++) {
      if (!usable_stat(astat) ||
	  !aunit_stat(astat)) continue;
      if (stat_info[astat].input[stat_info[astat].key_char - 1] == last_ch)
	break;
    }
    if (astat == num_statuses) {
      errormsg("Huh?  That isn't any command I have in my list.");
      break;
    }

    /* check if the speed is possible */
    if (astat_ok(astat, TRUE)) {
      change_astatus(army_ptr, astat);
    }
    break;
  }
}

/* AUNIT_NUMBERING -- Interface to the army unit autonumbering system */
int
aunit_numbering PARM_0(void)
{
  UNUM_PTR trace_ptr = NULL, page_ptr = NULL;
  char title[LINELTH];
  int x, ltitle, count, forw_page = FALSE, done = FALSE;
  int idnum;

  /* get god's identity */
  if (is_god == TRUE) {
    get_god(-1, FALSE);
  }
  redraw = DRAW_FULL;
  if (ntn_ptr == NULL) {
    strcpy(title, "World Default Army Numbering");
  } else {
    sprintf(title, "Nation %s Army Numbering", ntn_ptr->name);
  }
  ltitle = strlen(title);

  /* keep going until an exit is requested */
  do {

    /* clear appropriately */
    if (redraw == DRAW_FULL) {
      do_redraw();
      redraw = DRAW_DONE;
    } else {
      move(0,0);
      clrtobot();
    }

    /* provide a header */
    standout();
    mvaddstr(1, (COLS - ltitle) / 2, title);
    standend();

    /* align the page pointer */
    if (page_ptr == NULL) {
      if (ntn_ptr == NULL) {
	page_ptr = world.dflt_unum;
      } else {
	page_ptr = ntn_ptr->unum_list;
      }
    }
    count = 3;
    if (forw_page == TRUE) {
      if (trace_ptr == NULL) {
	if (ntn_ptr == NULL) {
	  page_ptr = world.dflt_unum;
	} else {
	  page_ptr = ntn_ptr->unum_list;
	}
      } else {
	page_ptr = trace_ptr;
	mvaddstr(count++, 5, "...more...");
      }
    }

    /* show a page worth of the listings */
    for (trace_ptr = page_ptr;
	 trace_ptr != NULL;
	 trace_ptr = trace_ptr->next) {

      /* check for boundary */
      if (count >= LINES - 7) {
	mvaddstr(count, 5, "...more...");
	break;
      }

      /* display the item */
      switch (trace_ptr->type) {
      case 0:
	mvprintw(count, 3, "Default numbering begins at %d",
		 trace_ptr->number);
	break;
      case 1:
	mvprintw(count, 3, "Armies of Class %s start at %d",
		 ainfo_clist[trace_ptr->ident],
		 trace_ptr->number);
	break;
      case 2:
	mvprintw(count, 3, "%s Armies start at %d",
		 ainfo_list[trace_ptr->ident].name,
		 trace_ptr->number);
	break;
      }

      /* continue */
      count++;

    }

    /* check for empty list */
    if (count == 3) {
      standout();
      if (ntn_ptr == NULL) {
	mvaddstr(count, 3, "No entries:  Global Default Number ==> 1");
      } else {
	mvaddstr(count, 3, "-- global default numbering in use --");
      }
      standend();
    }

    /* give a prompt listing possible commands */
    move(LINES - 4, (COLS - 60) / 2);
    hip_string("Set Default Number, ", 1, TRUE);
    hip_string("Army Class Number, ", 1, TRUE);
    hip_string("Unittype Number", 1, TRUE);
    move(LINES - 3, (COLS - 49) / 2);
    hip_string("Defaults, ", 1, TRUE);
    hip_string("Renumber ALL Units, ", 1, (ntn_ptr != NULL));
    hip_string("Clear Numbering", 1, TRUE);
    move(LINES - 2, (COLS - 27) / 2);
    hip_string("? for Help or ", 1, TRUE);
    hip_string("Q to Exit", 1, TRUE);
    refresh();

    /* get the selection */
    switch (next_char()) {
    case CNTRL_L:
    case CNTRL_R:
      /* full redraw */
      redraw = DRAW_FULL;
      break;
    case 'Q':
    case 'q':
      done = TRUE;
      break;
    case ' ':
    case '\n':
    case '\r':
      forw_page = TRUE;
      break;
    case '?':
      clear_bottom(0);
      mvaddstr(LINES - 5, 3,
  "The army unit numbering system is used to determine what id numbers");
      mvaddstr(LINES - 4, 3,
  "are given to units when they are first created.  When a unit is created");
      mvaddstr(LINES - 3, 3,
  "the type (Militia, Archers, etc.) is checked for a starting number,");
      mvaddstr(LINES - 2, 3,
  "then the type (Mercenary, Scouts, etc.) then the default base number is");
      mvaddstr(LINES - 1, 3,
  "used.  If a nation has no scheme set, the global default will be used.");
      presskey();
      break;
    case 'D':
    case 'd':
      /* set the defaults */
      clear_bottom(1);
      mvaddstr(LINES - 1, 0, "Replace current settings with defaults? ");
      if (y_or_n()) {
	page_ptr = NULL;
	if (ntn_ptr == NULL) {
	  XUNUMDEFAULT;
	  unum_defaults();
	} else {
	  XUNUMCOPY;
	  unum_copydefault();
	}
      }
      break;
    case 'C':
    case 'c':
      /* clear the numbering */
      clear_bottom(1);
      mvaddstr(LINES - 1, 0, "Clear out all of the current settings? ");
      if (y_or_n()) {
	page_ptr = NULL;
	XUNUMRESET;
	resetnumbers();
      }
      break;
    case 'R':
    case 'r':
      /* renumbering of all of the units */
      if (ntn_ptr == NULL) {
	errormsg("You don't have any units to renumber");
	break;
      }
      clear_bottom(1);
      mvaddstr(LINES - 1, 0, "Renumber all of the units in the nation? ");
      if (y_or_n()) {
	page_ptr = NULL;
	XNRENUM;
	army_reorganize();
      }
      break;
    case 'S':
    case 's':
      /* set the base number */
      clear_bottom(0);
      mvaddstr(LINES - 3, 0, "Enter a default starting number? ");
      x = get_number(FALSE);
      if (no_input == TRUE) break;

      /* check the range */
      if ((x <= 0) ||
	  (x > MAX_IDTYPE)) {
	sprintf(string, "A unit number must be between 1 and %d",
		MAX_IDTYPE);
	errormsg(string);
      } else {
	page_ptr = NULL;
	newslotnumber(x, 0, 0);
	global_int = 0;
	global_long = 0;
	XUNUMSLOT;
      }
      break;
    case 'U':
    case 'u':
      /* by the unit type */
      clear_bottom(0);
      mvaddstr(LINES - 3, 0, "Number which Unit Type? ");
      idnum = get_atype();
      if (no_input == TRUE) break;

      /* check the range */
      if (idnum == num_armytypes) {
	errormsg("That is not any army unit type that I know of");
      } else {
	mvprintw(LINES - 2, 0, "Enter a starting number for %s units? ",
		 ainfo_list[idnum].name);
	x = get_number(FALSE);
	if (no_input == TRUE) break;

	/* check the range */
	if ((x <= 0) ||
	    (x > MAX_IDTYPE)) {
	  sprintf(string, "A unit number must be between 1 and %d",
		  MAX_IDTYPE);
	  errormsg(string);
	} else {
	  page_ptr = NULL;
	  newslotnumber(x, idnum, 2);
	  global_long = 2;
	  global_int = idnum;
	  XUNUMSLOT;
	}
      }
      break;
    case 'A':
    case 'a':
      /* by the army class */
      clear_bottom(0);
      mvaddstr(LINES - 3, 0, "Number which Army Class? ");
      idnum = get_aclass();
      if (no_input == TRUE) break;

      /* check the range */
      if (idnum == num_aclasses) {
	errormsg("That is not any army unit type that I know of");
      } else {
	mvprintw(LINES - 2, 0, "Enter a starting number for %s class units? ",
		 ainfo_clist[idnum]);
	x = get_number(FALSE);
	if (no_input == TRUE) break;

	/* check the range */
	if ((x <= 0) ||
	    (x > MAX_IDTYPE)) {
	  sprintf(string, "A unit number must be between 1 and %d",
		  MAX_IDTYPE);
	  errormsg(string);
	} else {
	  page_ptr = NULL;
	  newslotnumber(x, idnum, 1);
	  global_long = 1;
	  global_int = idnum;
	  XUNUMSLOT;
	}
      }
      break;
    default:
      /* huh? */
      errormsg("Unknown command selection");
      break;
    }
  } while (done == FALSE);

  /* finish up */
  redraw = DRAW_FULL;
  if (is_god == TRUE) reset_god();
  return(0);
}

/* UNGROUP_SCOUTS -- Remove any scouts from a group and set to defend */
void
ungroup_scouts PARM_1(int, grpnum)
{
  ARMY_PTR a1_ptr, ahold_ptr = army_ptr;

  /* first get the main leader */
  if ((a1_ptr = armybynum(grpnum)) == NULL) return;

  /* search for the sector with the leader */
  for (army_ptr = ntn_ptr->army_list;
       army_ptr != NULL;
       army_ptr = army_ptr->next) {
    if ((a1_ptr->xloc == army_ptr->xloc) &&
	(a1_ptr->yloc == army_ptr->yloc)) break;
  }

  /* now search for scouts */
  for (; army_ptr != NULL; army_ptr = army_ptr->nrby) {
    if (a_isscout(ARMY_TYPE) &&
	(ARMY_LEAD == grpnum) &&
	(unit_status(ARMY_STAT) == ST_GROUPED)) {
      set_status(ARMY_STAT, ST_DEFEND);
      AADJSTAT;
    }
  }
  army_ptr = ahold_ptr;
}
