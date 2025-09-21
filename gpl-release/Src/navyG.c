// SPDX-License-Identifier: GPL-3.0-or-later
/* This file contains routines allowing user manipulation of naval units */
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
#define USE_CODES
#define PRINT_CODES
#include "dataG.h"
#include "armyX.h"
#include "cityX.h"
#include "navyX.h"
#include "desigX.h"
#include "mtrlsX.h"
#include "worldX.h"
#include "elevegX.h"
#include "statusX.h"
#include "caravanX.h"

/* CHANGE_NSPEED -- Adjust the movement rate of a navy unit */
static void
change_nspeed PARM_2(NAVY_PTR, n1_ptr, int, new_speed)
{
  /* get the navy */
  if (n1_ptr == NULL) {
    errormsg("Hmm, I can't seem to find that fleet anywhere");
    return;
  }

  /* find the navy */
  navy_ptr = n1_ptr;

  /* adjust the movement potential */
  if ((is_god == FALSE) && (NAVY_MOVE != 0)) {
    if (NAVY_MOVE > 10) {
      NAVY_MOVE -= 10;
    } else {
      NAVY_MOVE = 0;
    }
    NADJMOVE;
  }
  set_speed(NAVY_STAT, new_speed);
  NADJSTAT;
}

/* DISB_NAVY -- Get rid of a naval unit */
void
disb_navy PARM_1 (NAVY_PTR, n1_ptr)
{
  ITEM_STRUCT supply_items;
  NAVY_PTR nhold_ptr = navy_ptr;
  long num_peop;
  int i, cnt, limited = FALSE, xloc, yloc;

  /* check input */
  if (n1_ptr == NULL) {
    errormsg("Invalid input to function disb_navy()");
    return;
  }

  /* check disbanding */
  if ((is_god == FALSE) &&
      (sct[n1_ptr->xloc][n1_ptr->yloc].owner != country)) {
    errormsg("Fleets must disband in their own territory");
    return;
  }
  if (navy_carrying(n1_ptr)) {
    errormsg("Everything must be unloaded prior to fleet disbanding");
    return;
  }

  /* question the disband */
  if (conq_expert == FALSE) {
    mvprintw(LINES - 1, 0, "Disband your naval fleet #%d? ",
	     n1_ptr->navyid);
    if (y_or_n() == FALSE) return;
  }

  /* now disband the unit */
  navy_ptr = n1_ptr;
  if (is_god == FALSE) {

    /* try to give back the supplies */
    if (NAVY_SPLY > 0) {

      /* find the values */
      navy_support(navy_ptr, &(supply_items.mtrls[0]), NAVY_SPLY);
      limited = FALSE;

      /* try to give them back */
      if (send_resources(NAVY_XLOC, NAVY_YLOC, &(supply_items), limited)) {
	bottommsg("The fleet supplies will be lost.  Continue? ");
	if (y_or_n() == FALSE) return;
	NAVY_SPLY = 0;
      }

    }

    /* set dispersal point */
    xloc = XREAL;
    yloc = YREAL;
    xcurs = NAVY_XLOC - xoffset;
    ycurs = NAVY_YLOC - yoffset;

    /* disperse the crew */
    num_peop = 0L;
    for (i = 0; i < NSHP_NUMBER; i++) {
      num_peop += (long) NAVY_CREW *
	navy_holds(navy_ptr, i);
    }
    num_peop += (long) NAVY_PEOP * navy_holds(navy_ptr, NSHP_GALLEYS);
    sct[NAVY_XLOC][NAVY_YLOC].people += num_peop;
    SADJPEOP;

    /* now give back the resources */
    if (NAVY_SPLY > 0) {
      GIVESTART;
      /* maybe add wood to sector storage? UNIMPLEMENTED */
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

    /* perhaps add some back into the recruiting pool */
    if ((city_ptr = citybyloc(ntn_ptr, NAVY_XLOC, NAVY_YLOC)) != NULL) {
      if (CITY_PEOPLE >= 0) {
	CITY_PEOPLE += num_peop / 4;
	CADJPEOP;
      }
    }
  }

  /* do it */
  NDESTROY;
  dest_navy(NAVY_ID);
  hex_recalc();
  navy_ptr = nhold_ptr;
}

/* COMB_NAVIES -- Merge the second navy into the first */
static void
comb_navies PARM_2 (NAVY_PTR, n1_ptr, NAVY_PTR, n2_ptr)
{
  long lvalue;
  int stat1, stat2;
  int spd1, spd2;
  int i, j, err_add = FALSE;
  int old_ships[NSHP_NUMBER];
  int new_ships[NSHP_NUMBER];

  /* safety check */
  if ((n1_ptr == NULL) || (n2_ptr == NULL)) {
    errormsg("Uh oh, I can't find your navy!");
    return;
  }
  if (n1_ptr == n2_ptr) {
    errormsg("Hmm... they seem to be combined already");
    return;
  }
  navy_ptr = n1_ptr;

  /* check that they are in the same sector */
  if ((NAVY_XLOC != n2_ptr->xloc) || (NAVY_YLOC != n2_ptr->yloc)) {
    errormsg("The fleets must be in the same sector to be combined");
    return;
  }

  /* find unit statuses */
  stat1 = unit_status(NAVY_STAT);
  stat2 = unit_status(n2_ptr->status);
  if (nocomb_stat(stat1)) {
    sprintf(string, "Fleet %d is on %s and cannot be merged",
	    NAVY_ID, stat_info[stat1].name);
    errormsg(string);
    return;
  }
  if (nocomb_stat(stat2)) {
    sprintf(string, "Fleet %d is on %s and cannot be merged",
	    n2_ptr->navyid, stat_info[stat2].name);
    errormsg(string);
    return;
  }

  /* check spell status */
  if (unit_spelled(NAVY_STAT) != unit_spelled(n2_ptr->status)) {
    errormsg("Both fleets have to be magically enhanced to be combined");
    return;
  }

  /* check cargo */
  if ((NAVY_ARMY != EMPTY_HOLD) && (n2_ptr->armynum != EMPTY_HOLD)) {
    errormsg("Both ships are carrying troops; One must be unloaded to merge");
    return;
  }

  /* check cargo */
  if ((NAVY_CVN != EMPTY_HOLD) && (n2_ptr->cvnnum != EMPTY_HOLD)) {
    errormsg("Both ships are carrying wagons; One must be unloaded to merge");
    return;
  }

  /* assign speed settings */
  spd1 = unit_speed(NAVY_STAT);
  spd2 = unit_speed(n2_ptr->status);

  /* confirm it */
  if (conq_expert == FALSE) {
    mvprintw(LINES - 1, 0, "Merge Naval Fleet %d into Fleet %d? ",
	     n2_ptr->navyid, NAVY_ID);
    if (y_or_n() == FALSE) return;
  }

  /* merge the second unit into the first */
  for (j = 0; j < NSHP_NUMBER; j++) {
    old_ships[j] = NAVY_SHIPS[j];
    new_ships[j] = old_ships[j];
    for (i = N_LIGHT; i <= N_HEAVY; i++) {
      if ((new_ships[j] =
	   navy_addships(new_ships[j], i,
			 N_CNTSHIPS(n2_ptr->ships[j], i))) == (-1)) {
	err_add = TRUE;
      }
      if (err_add) {
	sprintf(string, "There are too many %s %s for them to combine",
		shipsize[i], ninfo_list[j].name);
	errormsg(string);
	return;
      }
    }
  }

  /* now go through and save the adjustments */
  for (global_int = 0; global_int < NSHP_NUMBER; global_int++) {
    NAVY_SHIPS[global_int] = new_ships[global_int];
    NADJSHIP;
  }

  /* adjust the speeds */
  if ((spd1 != spd2) && (is_god == TRUE)) {

    /* compare and adjust speeds */
    if (spd1 > spd2) {

      if (NAVY_MOVE > 10) {
	NAVY_MOVE -= 10;
      } else {
	NAVY_MOVE = 0;
      }
      set_speed(NAVY_STAT, spd2);
      NADJSTAT;

    } else {
      
      if (n2_ptr->umove > 10) {
	n2_ptr->umove -= 10;
      } else {
	n2_ptr->umove = 0;
      }

    }
  }

  /* fix movement */
  if (NAVY_MOVE > n2_ptr->umove) {

    /* reduce movement of unit */
    NAVY_MOVE = n2_ptr->umove;

  }
  NADJMOVE;

  /* calculate combinationals */
  i = j = 0;
  for (global_int = 0; global_int < NSHP_NUMBER; global_int++) {
    i += cargo_holds(old_ships[global_int]);
    j += cargo_holds(new_ships[global_int]);
  }
  if (NAVY_CREW != n2_ptr->crew) {
    lvalue = i * NAVY_CREW + (j - i) * n2_ptr->crew;
    NAVY_CREW = (uns_char) (lvalue / j);
    NADJCREW;
  }
  if (NAVY_SPLY != n2_ptr->supply) {
    lvalue = i * NAVY_SPLY + (j - i) * n2_ptr->supply;
    NAVY_SPLY = (uns_char) (lvalue / j);
    NADJSPLY;
  }
  if (NAVY_PEOP != n2_ptr->people) {
    i = cargo_holds(old_ships[NSHP_GALLEYS]);
    j = cargo_holds(new_ships[NSHP_GALLEYS]);
    lvalue = i * NAVY_PEOP + (j - i) * n2_ptr->people;
    NAVY_PEOP = (uns_char) (lvalue / j);
    NADJPEOP;
  }
  if (n2_ptr->armynum != EMPTY_HOLD) {
    NAVY_ARMY = n2_ptr->armynum;
    NADJARMY;
  }
  if (n2_ptr->cvnnum != EMPTY_HOLD) {
    NAVY_CVN = n2_ptr->cvnnum;
    NADJCVN;
  }

  /* merge the two into one */
  for (i = 0; i < MTRLS_NUMBER; i++) {
    if (n2_ptr->mtrls[i] != 0) {
      NAVY_MTRLS[i] += n2_ptr->mtrls[i];
      NMTRLS(i);
    }
  }

  /* nuke the second unit */
  navy_ptr = n2_ptr;
  NDESTROY;
  dest_navy(NAVY_ID);
  navy_ptr = NULL;
}

/* CHANGE_NAVYSTATUS -- Adjust the status of a navy unit */
static void
change_nstatus PARM_2(NAVY_PTR, n1_ptr, int, new_stat)
{
  /* get the navy */
  if (n1_ptr == NULL) {
    errormsg("Hmm, I can't seem to find that navy anywhere");
  }

  /* find the navy status */
  navy_ptr = n1_ptr;
  set_status(NAVY_STAT, new_stat);
  NADJSTAT;
  if ((is_god == FALSE) && (NAVY_MOVE != 0)) {
    if (NAVY_MOVE > 10) {
      NAVY_MOVE -= 10;
    } else {
      NAVY_MOVE = 0;
    }
    NADJMOVE;
  }
}

/* SPLIT_NAVY -- Create a new naval unit with the ships specified */
static void
split_navy PARM_1(NAVY_PTR, n1_ptr)
{
  int i, j, k, maxval = 0, number, was_asked = FALSE;
  int new_ships[NSHP_NUMBER];
  int old_ships[NSHP_NUMBER];

  /* validate input */
  if ((navy_ptr = n1_ptr) == NULL) {
    return;
  }

  /* check the status */
  if (nosplit_stat(NAVY_STAT)) {
    errormsg("That unit is for sale and may not be broken down");
    return;
  }

  /* go and find out what to split */
  clear_bottom(0);
  for (j = 0; j < NSHP_NUMBER; j++) {
    old_ships[j] = NAVY_SHIPS[j];
    new_ships[j] = 0;
    if ((NAVY_ARMY != EMPTY_HOLD) &&
	(j == NSHP_WARSHIPS)) continue;
    if ((NAVY_CVN != EMPTY_HOLD) &&
	(j == NSHP_BARGES)) continue;
    for (i = N_LIGHT; i <= N_HEAVY; i++) {

      /* check if a split is possible */
      if ((maxval = NAVY_CNTSHIPS(j, i)) == 0) continue;

      /* ask how many to split */
      was_asked = TRUE;
    ask_split_amt:
      mvprintw(LINES - 2, 0, "Split how many %s %s? [max %d] ",
	       shipsize[i], ninfo_list[j].name, maxval);
      clrtoeol();
      refresh();
      number = get_number(FALSE);
      if ((no_input == TRUE) || (number == 0)) continue;
      if (number > maxval) {
	errormsg("There are not that many available");
	goto ask_split_amt;
      }

      /* separate them out */
      new_ships[j] = navy_addships(new_ships[j], i, number);
      old_ships[j] = navy_subships(old_ships[j], i, number);

    }
  }

  /* check the results */
  number = 0;
  for (i = 0; i < NSHP_NUMBER; i++) {
    number |= old_ships[i];
  }
  if (number == 0) {
    errormsg("You have left no ships in the original naval unit");
    return;
  }
  number = 0;
  for (i = 0; i < NSHP_NUMBER; i++) {
    number |= new_ships[i];
  }
  if (number == 0) {
    if (was_asked) {
      errormsg("You have not separated out any ships");
    } else {
      errormsg("You are not able to separate out ships within this fleet");
    }
    return;
  }

  /* fix old unit */
  if ((navy_tptr = crt_navy()) == NULL) {
    errormsg("Woops!  Looks like you have too many fleets already");
    return;
  }
  for (global_int = 0; global_int < NSHP_NUMBER; global_int++) {
    NAVY_SHIPS[global_int] = old_ships[global_int];
    NADJSHIP;
  }
  if (NAVY_PEOP != 0) {
    if (new_ships[NSHP_GALLEYS] != 0) {
      NAVYT_PEOP = NAVY_PEOP;
      navy_ptr = navy_tptr;
      NADJPEOP;
      navy_ptr = n1_ptr;
    }
    if (old_ships[NSHP_GALLEYS] == 0) {
      NAVY_PEOP = 0;
      NADJPEOP;
    }
  }

  /* distribute good appropriately */
  if (new_ships[NSHP_MERCHANTS] != 0) {

    /* find the number of merchant holds */
    i = cargo_holds(old_ships[NSHP_MERCHANTS]);
    j = cargo_holds(new_ships[NSHP_MERCHANTS]);
    for (k = 0; k < MTRLS_NUMBER; k++) {

      /* change 'em */
      if (NAVY_MTRLS[k] != 0) {
	NAVYT_MTRLS[k] = (i * NAVY_MTRLS[k]) / (i + j);
	NAVY_MTRLS[k] = (j * NAVY_MTRLS[k]) / (i + j);

	/* record it */
	NMTRLS(k);
      }
    }
  }

  /* create the new navy */
  navy_ptr = navy_tptr;
  NCREATE;
  for (global_int = 0; global_int < NSHP_NUMBER; global_int++) {
    NAVY_SHIPS[global_int] = new_ships[global_int];
    NADJSHIP;
    NAVY_EFF[global_int] = n1_ptr->efficiency[global_int];
    NADJEFF(global_int);
  }
  NAVY_MOVE = n1_ptr->umove;
  NAVY_STAT = n1_ptr->status;
  NAVY_CREW = n1_ptr->crew;
  NAVY_SPLY = n1_ptr->supply;
  NAVY_XLOC = n1_ptr->xloc;
  NAVY_LASTX = n1_ptr->lastx;
  NAVY_YLOC = n1_ptr->yloc;
  NAVY_LASTY = n1_ptr->lasty;
  NADJSTAT;
  NADJCREW;
  NADJSPLY;
  NADJLOC;
  NADJOLOC;
  NADJMOVE;
  for (k = 0; k < MTRLS_NUMBER; k++) {
    if (NAVY_MTRLS[k] != 0) {
      NMTRLS(k);
    }
  }
  navy_sort();
}

/* SEPARATE_NAVY -- Separate out a specified ship class from the fleet */
static void
separate_navy PARM_1(NAVY_PTR, n1_ptr)
{
  int i, info[NSHP_NUMBER], type, count;
  char qstr[LINELTH];

  if ((navy_ptr = n1_ptr) == NULL) {
    errormsg("I seem to have misplaced that fleet somewhere");
    return;
  }
  type = NSHP_NUMBER;
  for (i = 0; i < NSHP_NUMBER; i++) {
    if ((info[i] = NAVY_SHIPS[i]) == 0) {
      type--;
    }
  }
  if (type == 1) {
    errormsg("You only have one type of ship in that fleet");
    return;
  }

  /* show what can be separated */
  clear_bottom(0);
  count = 0;
  mvaddstr(LINES - 2, 0, "Separate All ");
  for (i = 0; i < NSHP_NUMBER; i++) {
    if (info[i] != 0) {

      /* display the class */
      printw("(%c)%s", ninfo_list[i].name[0],
	     &(ninfo_list[i].name[1]));
      qstr[count++] = ninfo_list[i].name[0];

      /* decrement and check separation */
      type--;
      if (type == 1) {
	addstr("or ");
      } else if (type > 0) {
	addstr(", ");
      }

    }
  }
  addstr("? ");
  qstr[count] = '\0';
  clrtoeol();
  refresh();

  /* get the input */
  i = one_char(qstr, TRUE);
  if (no_input == TRUE) {
    return;
  }
  for (type = 0; type < NSHP_NUMBER; type++) {
    if (i == ninfo_list[type].name[0]) break;
  }
  if (type == NSHP_NUMBER) {
    errormsg("I couldn't find your choice among my list of possibilities");
    return;
  }

  /* check for valid input */
  if (no_input == TRUE) {
    return;
  }
  if (info[type] == 0) {
    sprintf(string, "There are no %s in this fleet",
	    ninfo_list[type].name);
    errormsg(string);
    return;
  }

  /* now create a new unit */
  if ((navy_tptr = crt_navy()) == NULL) {
    errormsg("You already have too many naval units");
    return;
  }
  navy_ptr = navy_tptr;
  NCREATE;
  navy_ptr = n1_ptr;

  /* separate out the ships */
  global_int = type;
  NAVY_SHIPS[global_int] = 0;
  NADJSHIP;
  NAVYT_EFF[type] = NAVY_EFF[type];
  NAVY_EFF[type] = 0;
  NADJEFF(type);
  switch (type) {
  case NSHP_WARSHIPS:
    if (NAVY_ARMY != 0) {
      NAVYT_ARMY = NAVY_ARMY;
      NAVY_ARMY = EMPTY_HOLD;
      NADJARMY;
      navy_ptr = navy_tptr;
      NADJARMY;
    }
    break;
  case NSHP_BARGES:
    if (NAVY_CVN != 0) {
      NAVYT_CVN = NAVY_CVN;
      NAVY_CVN = EMPTY_HOLD;
      NADJCVN;
      navy_ptr = navy_tptr;
      NADJCVN;
    }
    break;
  case NSHP_MERCHANTS:
    for (i = 0; i < MTRLS_NUMBER; i++) {
      if (NAVY_MTRLS[i] != 0) {
	NAVYT_MTRLS[i] = NAVY_MTRLS[i];
	NAVY_MTRLS[i] = (itemtype) 0;
	NMTRLS(i);
      }
    }
    break;
  case NSHP_GALLEYS:
    if (NAVY_PEOP != 0) {
      NAVYT_PEOP = NAVY_PEOP;
      NAVY_PEOP = 0;
      NADJPEOP;
      navy_ptr = navy_tptr;
      NADJPEOP;
    }
    break;
  }
  navy_ptr = navy_tptr;
  NAVY_SHIPS[global_int] = info[global_int];
  NADJSHIP;
  NADJEFF(type);
  NAVY_MOVE = n1_ptr->umove;
  NADJMOVE;
  NAVY_STAT = n1_ptr->status;
  NADJSTAT;
  NAVY_CREW = n1_ptr->crew;
  NADJCREW;
  NAVY_SPLY = n1_ptr->supply;
  NADJSPLY;
  NAVY_XLOC = n1_ptr->xloc;
  NAVY_YLOC = n1_ptr->yloc;
  NADJLOC;
  NAVY_LASTX = n1_ptr->lastx;
  NAVY_LASTY = n1_ptr->lasty;
  NADJOLOC;
  for (i = 0; i < MTRLS_NUMBER; i++) {
    if (NAVY_MTRLS[i] != 0) {
      NMTRLS(i);
    }
  }
}

/* SUPPLY_NAVY -- Attempt to set the supply value of a navy unit */
int
supply_navy PARM_3(NAVY_PTR, n1_ptr, int, level, int, doquery)
{
  NAVY_PTR nhold_ptr = navy_ptr;
  ITEM_PTR itest_ptr;
  ITEM_STRUCT supt_cost;
  int oxloc = XREAL, oyloc = YREAL;
  int cnt, ok_val, diff, hold = TRUE;
  int insectonly = FALSE;

  /* check input */
  if (n1_ptr == NULL) {
    errormsg("I can't find that navy unit");
    return(hold);
  }
  if ((level < 0) ||
      (level > MAXSUPPLIES * 4)) {
    errormsg("Navy units may not carry that many supplies");
    return(hold);
  }

  /* check the difference */
  navy_ptr = n1_ptr;
  diff = level - NAVY_SPLY;
  if (diff == 0) {
    /* already set */
    hold = FALSE;
    goto bug_out;
  }
#ifndef MAYGIVEBACK
  if (diff < 0) {
    /* can't do this */
    sprintf(string, "Fleet %d refuses to return their pay",
	    NAVY_ID);
    errormsg(string);
    goto bug_out;
  }
#endif /* MAYGIVEBACK */

  /* now allow it */
  if (is_god == FALSE) {

    /* check for supply anywhere */
    if (sct[NAVY_XLOC][NAVY_YLOC].altitude == ELE_WATER) {
      insectonly = TRUE;
    }

    /* get the costs */
    navy_support(navy_ptr, &(supt_cost.mtrls[0]), abs(diff));

    /* check if we are giving or getting */
    if (diff > 0) {

      /* check for unreachable supplying */
      if (sct[NAVY_XLOC][NAVY_YLOC].owner != country) {
	insectonly = TRUE;      
      }

      /* now check that they are available */
      if ((itest_ptr = find_resources(NAVY_XLOC, NAVY_YLOC,
				      insectonly))
	  == NULL) {
	if (insectonly) {
	  sprintf(string, "Fleet %d can only be supplied from that sector",
		  NAVY_ID);
	} else {
	  sprintf(string, "Fleet %d cannot find enough available resources",
		  NAVY_ID);
	}
	errormsg(string);
	goto bug_out;
      }

      /* now compare the values */
      ok_val = TRUE;
      for (cnt = 0; cnt < MTRLS_NUMBER; cnt++) {
	if (itest_ptr->mtrls[cnt] < supt_cost.mtrls[cnt]) {
	  sprintf(string, "There are not %ld %s for fleet %d",
		  (long) supt_cost.mtrls[cnt],
		  mtrls_info[cnt].lname, NAVY_ID);
	  errormsg(string);
	  ok_val = FALSE;
	}
      }

      /* clean up */
      if (ok_val == FALSE) {
	free(itest_ptr);
	goto bug_out;
      }

      /* ask the user? */
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

      /* consume the resources */
      xcurs = NAVY_XLOC - xoffset;
      ycurs = NAVY_YLOC - yoffset;
      TAKESTART;
      for (cnt = 0; cnt < MTRLS_NUMBER; cnt++) {
	if ((global_long = supt_cost.mtrls[cnt]) > 0) {
	  TAKEMTRLS(cnt);
	}
      }
      TAKEFINISH(insectonly);
      xcurs = oxloc - xoffset;
      ycurs = oyloc - yoffset;
      take_resources(NAVY_XLOC, NAVY_YLOC, &supt_cost, insectonly);
      free(itest_ptr);
#ifdef MAYGIVEBACK
    } else {

      /* otherwise, just redistribute them in */
      if (send_resources(NAVY_XLOC, NAVY_YLOC, &supt_cost, insectonly)) {
	sprintf(string, "The supplies of fleet %d would be lost",
		NAVY_ID);
	errormsg(string);
	goto bug_out;
      }
      xcurs = NAVY_XLOC - xoffset;
      ycurs = NAVY_YLOC - yoffset;
      GIVESTART;
      for (cnt = 0; cnt < MTRLS_NUMBER; cnt++) {
	if ((global_long = supt_cost.mtrls[cnt]) > 0) {
	  GIVEMTRLS(cnt);
	}
      }
      GIVEFINISH(insectonly);
      xcurs = oxloc - xoffset;
      ycurs = oyloc - yoffset;
#endif /* MAYGIVEBACK */
    }
  }
  NAVY_SPLY = level;
  NADJSPLY;

  /* reset before bugging out */
  hold = FALSE;
 bug_out:
  navy_ptr = nhold_ptr;
  return(hold);
}

/* NAVY_REPAIR -- Attempt to repair a navy */
void
navy_repair PARM_1(NAVY_PTR, n1_ptr)
{
  itemtype cost_mtrls[MTRLS_NUMBER];
  int i, okval = TRUE;

  /* begin */
  if (n1_ptr == NULL) {
    errormsg("I could not find that fleet");
    return;
  }
  navy_ptr = n1_ptr;

  /* now check for a city in the current sector */
  if (is_god == FALSE) {

    /* are we in a supply center */
    if (((city_ptr = citybyloc(ntn_ptr, NAVY_XLOC, NAVY_YLOC)) == NULL) ||
	!minor_desg(sct[NAVY_XLOC][NAVY_YLOC].designation,
		    MIN_HARBOR)) {
      errormsg("A fleet needs to be in a harbor to be repaired");
      return;
    }

    /* now find the naval repair costs */
    navy_redocosts(navy_ptr, &(cost_mtrls[0]));

    /* check if it is supportable */
    for (i = 0; i < MTRLS_NUMBER; i++) {
      if (CITY_MTRLS[i] < cost_mtrls[i]) {
	okval = FALSE;
	sprintf(string, "The supply center doesn't have %.0f %s",
		(double) cost_mtrls[i], mtrls_info[i].lname);
	errormsg(string);
      }
    }
    if (okval == FALSE) return;

    /* get the okay */
    if (conq_expert == FALSE) {
      mvaddstr(LINES - 1, 0, "Cost: ");
      for (i = 0; i < MTRLS_NUMBER; i++) {
	if (cost_mtrls[i] > 0) {
	  if (i > 0) {
	    addstr(", ");
	  }
	  printw("%0.f %s", (double) cost_mtrls[i],
		 mtrls_info[i].lname);
	}
      }
      addstr("; Continue? ");
      if (y_or_n() == FALSE) {
	return;
      }
    }

    /* spend the totals */
    for (i = 0; i < MTRLS_NUMBER; i++) {
      if (cost_mtrls[i] > 0) {
	CITY_MTRLS[i] -= cost_mtrls[i];
	CADJMTRLS(i);
	CITY_IMTRLS[i] -= cost_mtrls[i];
	if (CITY_IMTRLS[i] < 0) {
	  CITY_IMTRLS[i] = 0;
	}
	CADJIMTRLS(i);
      }
    }

  }

  /* now "repair" it */
  for (i = 0; i < NSHP_NUMBER; i++) {
    NAVY_EFF[i] = 100;
    NADJEFF(i);
  }

  /* has to complete the repairs */
  if (is_god == FALSE) {
    set_status(NAVY_STAT, ST_REPAIR);
    NADJSTAT;
    if (NAVY_MOVE > 0) {
      NAVY_MOVE = 0;
      NADJMOVE;
    }
  }
}

/* NSTAT_OK -- Is the naval status okay? */
static int
nstat_ok PARM_2(int, new_stat, int, verbal)
{
  int old_stat;

  /* check the unit */
  if (navy_ptr == NULL) {
    if (verbal) {
      errormsg("Hmm, I can't seem to find that navy anywhere");
    }
    return(FALSE);
  }

  /* find the army status */
  old_stat = unit_status(NAVY_STAT);

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

  /* check for unalterable statuses */
  if (nochange_stat(old_stat)) {
    if (verbal) {
      errormsg("That fleet is unable to change their status");
    }
    return(FALSE);
  }
  return(TRUE);
}

/* NSPEED_OK -- Is the given navy speed possible for the unit */
static int
nspeed_ok PARM_2(int, new_speed, int, verbal)
{
  /* check the navy */
  if (navy_ptr == NULL) {
    if (verbal) {
      errormsg("Hmm, I can't seem to find that fleet anywhere");
    }
    return(FALSE);
  }

  /* is it a movable unit? */
  if (nomove_stat(NAVY_STAT)) {
    if (verbal) {
      errormsg("That unit is stopped, its speed is not adjustable");
    }
    return(FALSE);
  }
  if ((NAVY_MOVE < 25) &&
      (unit_speed(NAVY_STAT) < new_speed)) {
    if (verbal) {
      errormsg("That fleet has gone too far to increase speed");
    }
    return(FALSE);
  }
  if (unit_speed(NAVY_STAT) == new_speed) {
    if (verbal) {
      errormsg("The fleet is already moving at that speed");
    }
    return(FALSE);
  }
  return(TRUE);
}

/* EXT_NAVYINFO -- Provide extended information about the naval fleet */
void
ext_navyinfo PARM_1(NAVY_PTR, n1_ptr)
{
  ITEM_STRUCT supt_cost;
  long lval;
  int ind, i, j, holds = 0;

  /* check input */
  if (n1_ptr == NULL) return;
  navy_ptr = n1_ptr;

  /* now clear the bottom area */
  clear_bottom(0);

  /* first line of information */
  move(LINES - 5, 0);
  printw("Fleet %d ==> ", NAVY_ID);
  ind = 0;
  for (i = 0; i < NSHP_NUMBER; i++) {
    holds += navy_holds(navy_ptr, i);
    if (NAVY_SHIPS[i] != 0) {
      if (ind == 1) addstr(", ");
      ind = 1;
      printw("%s:", ninfo_list[i].name);
      for (j = 0; j <= N_HEAVY; j++) {
	if (j != 0) addch('/');
	printw("%d", (int) N_CNTSHIPS(NAVY_SHIPS[i], j));
      }
    }
  }
  addstr(";");

  /* second line of information */
  move(LINES - 4, 0);
  printw("  Status: %s;", stat_info[unit_status(NAVY_STAT)].name);
  printw(" Speed: %s;", speedname[unit_speed(NAVY_STAT)]);
  printw(" Move Left: %d%%;", (int) NAVY_MOVE);
  printw(" Move Ability: %d;", navy_mvpts(ntn_ptr, navy_ptr));

  /* line three */
  move(LINES - 3, 0);
  addstr("  Eff: ");
  ind = 0;
  for (i = 0; i < NSHP_NUMBER; i++) {
    if (NAVY_SHIPS[i] != 0) {
      if (ind == 1) addstr(", ");
      ind = 1;
      printw("%s%d%%", ninfo_list[i].sname, (int) NAVY_EFF[i]);
    }
  }
  addstr(";");
  printw(" Tot Crew: %d;", (int) NAVY_CREW * holds);
  printw(" Locale: [%d,%d];",
	 xloc_relative(NAVY_XLOC),
	 yloc_relative(NAVY_YLOC));

  /* describe the cargo */
  move(LINES - 2, 0);
  addstr("  Cargo: ");
  ind = 0;
  if (NAVY_PEOP != 0) {
    ind = 1;
    lval = (long) NAVY_PEOP * navy_holds(navy_ptr, NSHP_GALLEYS);
    printw("%ld %s", lval, (lval == 1)?"person":"people");
  }
  if ((army_tptr = armybynum(NAVY_ARMY)) != NULL) {
    if (ind == 1) addstr(", ");
    ind = 1;
    if (a_isleader(ARMYT_TYPE) &&
	unit_leading(ARMYT_STAT)) {
      printw("Group #%d", ARMYT_ID);
    } else {
      printw("%s #%d", ainfo_list[ARMYT_TYPE].name, ARMYT_ID);
    }
  }
  if ((cvn_tptr = cvnbynum(NAVY_CVN)) != NULL) {
    if (ind == 1) addstr(", ");
    ind = 1;
    printw("Cvn #%d", CVNT_ID);
  }
  for (i = 0; i < MTRLS_NUMBER; i++) {
    if (NAVY_MTRLS[i] > 0) {
      if (ind == 1) addstr(", ");
      ind = 1;
      printw("%.0f %s", (double) NAVY_MTRLS[i],
	     mtrls_info[i].lname);
    }
  }
  if (ind == 0) addstr("none");
  addstr(";");

  /* line five */
  move(LINES - 1, 0);
  printw("  Supply Level: %d turns;", (int) NAVY_SPLY);
  addstr(" Supply Cost: ");
  navy_support(navy_ptr, &(supt_cost.mtrls[0]), 1);
  ind = 0;
  for (i = 0; i < MTRLS_NUMBER; i++) {
    if (supt_cost.mtrls[i] > 0) {
      if (ind == 1) addstr(", ");
      ind = 1;
      printw("%.0f %s", (double) supt_cost.mtrls[i],
	     mtrls_info[i].lname);
    }
  }
  addstr(";");

  /* finish hit */
  presskey();
}

/* EXT_NAVYCMD -- Perform an extended navy operation */
void
ext_navycmd PARM_1 (int, navie)
{
  NAVY_PTR nhold_ptr, n2_ptr;
  int i, x, y, last_ch, need_repairs = FALSE;
  int nstat = -1, men;

  /* find target navy unit */
  if (navie == -1) {
    navy_ptr = get_navyselect();
    if ((country == UNOWNED) &&
	get_god(global_int, TRUE)) {
      return;
    }
    if (navy_ptr != NULL) {
      goto_navy(NAVY_ID);
    }
  } else {
    navy_ptr = navybynum(navie);
  }
  if (navy_ptr == NULL) {
    errormsg("I could not find that naval unit");
    return;
  }

  /* Display possible adjustment commands */
  y = LINES - 5;
  x = 1;
  clear_bottom(0);
  move(y++, x);
  hip_string("? Info, ", 1, TRUE);
  hip_string("+ Combine, ", 1, (navie == -1) &&
	     !nocomb_stat(NAVY_STAT));
  hip_string("Merge, ", 1, !nocomb_stat(NAVY_STAT));
  hip_string("- Split Navy, ", 1, TRUE);
  hip_string("/ Separate Types, ", 1, TRUE);

  /* next line of commands */
  move(y++, x);
  hip_string("# Renumber, ", 1, TRUE);
  for (i = 0; i < SPD_STUCK; i++) {
    sprintf(string, "%s %s, ", shortspeed[i], speedname[i]);
    hip_string(string, 1, nspeed_ok(i, FALSE));
  }
  hip_string("Disband, ", 1, TRUE);
  hip_string("Supply,", 1, (MAXSUPPLIES > 0) &&
	     (NAVY_SPLY < MAXSUPPLIES * 4));

  /* third line of options */
  move(y++, x);
  for (i = 0; i < NSHP_NUMBER; i++) {
    if ((NAVY_SHIPS[i] != 0) &&
	(NAVY_EFF[i] < 100)) {
      need_repairs = TRUE;
      break;
    }
  }
  hip_string("Repair, ", 1, (need_repairs == TRUE) &&
	     (citybyloc(ntn_ptr, NAVY_XLOC, NAVY_YLOC) != (CITY_PTR)NULL));
  hip_string("Tfer Cargo", 1, TRUE);
  x += 20;
  for (i = 0; i < num_statuses; i++) {
    if (usable_stat(i) &&
	nunit_stat(i)) {

      /* check for overruns */
      if (x + strlen(stat_info[i].input) > COLS - 8) {
	addstr(",");
	x = 1;
	move(y++, x);
      }

      /* display the string */
      if (x > 1) {
	addstr(", ");
	x += 2;
      }
      x += strlen(stat_info[i].input) + 2;
      hip_string(stat_info[i].input,
		 stat_info[i].key_char,
		 nstat_ok(i, FALSE));
    }
  }

  if (navie == -1) {
    mvaddstr(y, 0, "Extended Navy Command: ");
  } else {
    mvaddstr(y, 0, "Naval Adjustment Command: ");
  }
  refresh();

  switch((last_ch = next_char())) {
  case '?':
    ext_navyinfo(navy_ptr);
    break;
  case '-':
    /* split navy */
    nhold_ptr = navy_ptr;
    split_navy(navy_ptr);
    if (navie == (-1)) {
      goto_navy(nhold_ptr->navyid);
    }
    break;
  case '/':
    /* divide navy */
    nhold_ptr = navy_ptr;
    separate_navy(navy_ptr);
    if (navie == (-1)) {
      goto_navy(nhold_ptr->navyid);
    }
    break;
  case '+':
    /* combine navies */
    if (navie != -1) {
      errormsg("Huh?  I have no idea what you are talking about.");
      break;
    }
    selector += 2;
    n2_ptr = get_navyselect();
    selector -= 2;
    if (n2_ptr == NULL) {
      errormsg("There are no following naval units to combine with");
      break;
    }
    nhold_ptr = navy_ptr;
    comb_navies(navy_ptr, n2_ptr);
    goto_navy(nhold_ptr->navyid);
    break;
  case 'M':
    /* select a unit to merge into */
    if (navie == -1) {

      /* set the selector properly */
      if (set_select(TRUE, "Choose a Naval Unit to Merge With") == FALSE) {
	break;
      }

      /* now retrieve the information */
      if ((n2_ptr = get_navyselect()) == NULL) {
	errormsg("That is not a Naval Unit");
	goto_navy(NAVY_ID);
	break;
      }

    } else {

      /* get it by number */
      mvaddstr(LINES - 1, 0, "Merge with what naval unit? ");
      clrtoeol();
      refresh();
      men = get_number(FALSE);
      if (no_input) break;
      n2_ptr = navybynum(men);

    }

    /* confirm it */
    if (n2_ptr == NULL) {
      errormsg("There is no such unit");
      break;
    }
    if (navy_ptr == n2_ptr) {
      errormsg("That is the same unit");
      break;
    }
    nhold_ptr = navy_ptr;
    comb_navies(navy_ptr, n2_ptr);
    if (navie == (-1)) {
      goto_navy(nhold_ptr->navyid);
    }
    break;
  case '<':
  case '=':
  case '>':
    /* find which speed it is */
    for (nstat = 0; nstat < SPD_STUCK; nstat++) {
      if (shortspeed[nstat][0] == last_ch) break;
    }
    if (nstat == SPD_STUCK) {
      errormsg("Invalid Input");
      break;
    }

    /* check if the speed is possible */
    if (nspeed_ok(nstat, TRUE)) {
      change_nspeed(navy_ptr, nstat);
    }
    break;
  case 'D':
    /* destroy the unit */
    disb_navy(navy_ptr);
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
    } else if (navybynum(men) != NULL) {
      errormsg("That number is already in use.");
      break;
    }
    global_int = NAVY_ID;
    navy_renum(men);
    NADJID;
    navy_sort();
    if (navie == -1) goto_navy(men);
    break;
  case 'r':
  case 'R':
    if (need_repairs == FALSE) {
      errormsg("That fleet does not need to be repaired");
      break;
    }
    navy_repair(navy_ptr);
    break;
  case 's':
  case 'S':
    /* set the supply level of the unit */
    if (MAXSUPPLIES == 0) {
      errormsg("Supplying is not in effect for this campaign");
      break;
    }

    /* now get it and set it */
    mvprintw(LINES - 1, 0, "Set supply to what value? [default=%d] ",
	     NAVY_SPLY);
    men = get_number(FALSE);

    /* check the input */
    if ((no_input == TRUE) || (men == NAVY_SPLY)) break;
    if (men < 0) {
      errormsg("Wow, negative supplies... why not just destroy it?");
      break;
    }
    if (men > MAXSUPPLIES * 4) {
      errormsg("They cannot carry that many supplies!");
      break;
    }

    /* now do it */
    supply_navy(navy_ptr, men, TRUE);
    break;
  case 't':
  case 'T':
    /* load/unload fleet */
    navy_transport(navy_ptr);
    break;
  case ' ':
  case '\n':
  case '\r':
    /* peaceful exit on space */
    break;
  default:
    /* naval status */
    for (nstat = 0; nstat < num_statuses; nstat++) {
      if (!usable_stat(nstat) ||
	  !nunit_stat(nstat)) continue;
      if (stat_info[nstat].input[stat_info[nstat].key_char - 1] == last_ch)
	break;
    }
    if (nstat == num_statuses) {
      errormsg("Huh?  That isn't any command I have in my list.");
      break;
    }

    /* check if the speed is possible */
    if (nstat_ok(nstat, TRUE)) {
      change_nstatus(navy_ptr, nstat);
    }
    break;
  }
}

/* NAVY_TRANSPORT -- Load/Unload the given naval unit */
void
navy_transport PARM_1(NAVY_PTR, n1_ptr)
{
  NAVY_PTR n2_ptr;
  CITY_PTR c2_ptr;
  CVN_PTR v2_ptr;
  int use_selector = TRUE, do_sector = FALSE;
  int x, y, idnum, set_god = FALSE;

  /* check for getting the current selection */
  if (n1_ptr == NULL) {
    use_selector = TRUE;
    n1_ptr = get_navyselect();
    if (country == UNOWNED) {
      if (get_god(global_int, TRUE)) {
	return;
      }
      set_god = TRUE;
    }
  }
  navy_ptr = n1_ptr;

  /* valid input? */
  if (navy_ptr == NULL) {
    errormsg("I could not find any naval fleet");
    if (set_god) reset_god();
    return;
  }

  /* check if the sector is in the wrong position */
  x = NAVY_XLOC;
  y = NAVY_YLOC;
  if (!XY_ONMAP(x, y)) {
    errormsg("You may not load or unload in that sector");
    if (set_god) reset_god();
    return;
  }

  /* bottom cleaning */
  clear_bottom(0);
  mvprintw(LINES - 3, 0, "Transferring items from fleet %d to...",
	   NAVY_ID);
  
  /* prepare the initial navy */
  navy2xfer(0, navy_ptr);

  /* find out what can be done */
  c2_ptr = citybyloc(ntn_ptr, x, y);
  for (v2_ptr = ntn_ptr->cvn_list;
       v2_ptr != NULL;
       v2_ptr = v2_ptr->next) {
    if ((v2_ptr->xloc == x) &&
	(v2_ptr->yloc == y)) {
      break;
    }
  }
  for (n2_ptr = ntn_ptr->navy_list;
       n2_ptr != NULL;
       n2_ptr = n2_ptr->next) {
    if (n2_ptr == navy_ptr) continue;
    if ((n2_ptr->xloc == x) &&
	(n2_ptr->yloc == y)) {
      break;
    }
  }

  /* any takers? */
  if ((n2_ptr == NULL) &&
      (c2_ptr == NULL) &&
      (v2_ptr == NULL)) {
    if (sct[x][y].altitude != ELE_WATER) {
      do_sector = TRUE;
    } else {
      errormsg("There is nothing in the sector to exchange with");
      if (set_god) reset_god();
      return;
    }
  }

  /* check if the city gets it */
  if ((v2_ptr != NULL) ||
      (n2_ptr != NULL)) {

    /* determine what selection is desired */
    if (use_selector) {

      /* is the city? */
      if (c2_ptr != NULL) {
	mvprintw(LINES - 2, 0, "Exchange materials with %s?", c2_ptr->name);
	if (y_or_n() == FALSE) {
	  c2_ptr = NULL;
	} else {
	  n2_ptr = NULL;
	  v2_ptr = NULL;
	}
      } else if (sct[x][y].altitude != ELE_WATER) {
	mvprintw(LINES - 2, 0, "Transfer materials within the sector? ");
	if (y_or_n()) {
	  do_sector = TRUE;
	  n2_ptr = NULL;
	  v2_ptr = NULL;
	}
      }

      /* must be a navy or caravan */
      if ((c2_ptr == NULL) &&
	  (do_sector == FALSE)) {
	if (set_select(TRUE,
		       "Choose a Navy or Caravan to exchange with")
	    == FALSE) {
	  if (set_god == TRUE) {
	    reset_god();
	  }
	  return;
	}
	if ((n2_ptr = get_navyselect()) != NULL) {
	  v2_ptr = NULL;
	  if (n2_ptr == navy_ptr) {
	    errormsg("That is the same fleet!");
	    if (set_god == TRUE) {
	      reset_god();
	    }
	    return;
	  }
	} else if ((v2_ptr = get_cvnselect()) == NULL) {
	  errormsg("That wasn't a fleet or a caravan!");
	  if (set_god == TRUE) {
	    reset_god();
	  }
	  return;
	}
      }
    } else {
      /* select which ever */
      mvaddstr(LINES - 2, 0, "Exchange goods with a ");
      hip_string("Navy, ", 1, TRUE);
      hip_string("Caravan, or the ", 1, TRUE);
      hip_string("Sector? ", 1, TRUE);
      refresh();
      switch (next_char()) {
      case 'S':
      case 's':
	/* the sector */
	if ((c2_ptr == NULL) &&
	    (sct[x][y].altitude == ELE_WATER)) {
	  errormsg("Your fleet cannot exchange within this sector");
	  if (set_god == TRUE) {
	    reset_god();
	  }
	  return;
	}
	if (c2_ptr == NULL) {
	  do_sector = TRUE;
	}
	n2_ptr = NULL;
	v2_ptr = NULL;
	break;
      case 'N':
      case 'n':
	/* naval selection */
	c2_ptr = NULL;
	v2_ptr = NULL;
	mvaddstr(LINES - 1, 0, "Exchange with which fleet number? ");
	idnum = get_number(FALSE);
	if (no_input == TRUE) {
	  if (set_god == TRUE) reset_god();
	  return;
	}
	if ((n2_ptr = navybynum(idnum)) == NULL) {
	  errormsg("I couldn't find that navy");
	  if (set_god == TRUE) reset_god();
	  return;
	} else if (n2_ptr == navy_ptr) {
	  errormsg("That is the same fleet!");
	  if (set_god == TRUE) reset_god();
	  return;
	} else if ((n2_ptr->xloc != x) ||
		   (n2_ptr->yloc != y)) {
	  errormsg("That fleet is not in the same sector!");
	  if (set_god == TRUE) reset_god();
	  return;
	}
	break;
      case 'C':
      case 'c':
	/* caravan selection */
	c2_ptr = NULL;
	n2_ptr = NULL;
	mvaddstr(LINES - 1, 0, "Exchange with which caravan number? ");
	idnum = get_number(FALSE);
	if (no_input == TRUE) {
	  if (set_god == TRUE) reset_god();
	  return;
	}
	if ((v2_ptr = cvnbynum(idnum)) == NULL) {
	  errormsg("I couldn't find that caravan");
	  if (set_god == TRUE) reset_god();
	  return;
	} else if ((v2_ptr->xloc != x) ||
		   (v2_ptr->yloc != y)) {
	  errormsg("That caravan is not in the same sector!");
	  if (set_god == TRUE) reset_god();
	  return;
	}
	break;
      }
    }
  }

  /* prepare the second site */
  if (n2_ptr != NULL) {

    /* compare the moves */
    if ((NAVY_MOVE != n2_ptr->umove) &&
	(conq_expert == FALSE)) {
      bottommsg("These units will have to match movement.  Continue? ");
      if (y_or_n() == FALSE) {
	if (set_god == TRUE) reset_god();
	return;
      }
    }

    /* must get the minimum movement */
    if (NAVY_MOVE < n2_ptr->umove) {
      n2_ptr->umove = NAVY_MOVE;
      navy_ptr = n2_ptr;
      NADJMOVE;
      navy_ptr = n1_ptr;
    } else if (NAVY_MOVE > n2_ptr->umove) {
      NAVY_MOVE = n2_ptr->umove;
      NADJMOVE;
    }
    navy2xfer(1, n2_ptr);

  } else if (c2_ptr != NULL) {

    /* just convert */
    city2xfer(1, c2_ptr);

  } else if (do_sector) {

    /* just convert */
    sct2xfer(1, x, y);

  } else if (v2_ptr != NULL) {

    /* check potentials */
    if ((NAVY_MOVE != v2_ptr->umove) &&
	(conq_expert == FALSE)) {
      bottommsg("These units will have to match movement.  Continue? ");
      if (y_or_n() == FALSE) {
	if (set_god == TRUE) reset_god();
	return;
      }
    }

    /* convert between the movement potentials */
    if (NAVY_MOVE < v2_ptr->umove) {
      v2_ptr->umove = NAVY_MOVE;
      cvn_ptr = v2_ptr;
      VADJMOVE;
    } else if (NAVY_MOVE > v2_ptr->umove) {
      NAVY_MOVE = v2_ptr->umove;
      NADJMOVE;
    }
    cvn2xfer(1, v2_ptr);

  } else {

    /* oops */
    if (set_god == TRUE) reset_god();
    return;

  }

  /* do the transfer */
  xfer_mode();

  /* return things to their proper place */
  xfer2navy(0, navy_ptr);
  if (n2_ptr != NULL) {
    xfer2navy(1, n2_ptr);
  } else if (c2_ptr != NULL) {
    xfer2city(1, c2_ptr);
  } else if (do_sector) {
    xfer2sct(1, x, y);
  } else if (v2_ptr != NULL) {
    xfer2cvn(1, v2_ptr);
  }
  if (set_god == TRUE) {
    reset_god();
  }
}
