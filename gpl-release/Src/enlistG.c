// SPDX-License-Identifier: GPL-3.0-or-later
/* routines to create armies, navies, and caravans */
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
#include "itemX.h"
#include "navyX.h"
#include "desigX.h"
#include "magicX.h"
#include "mtrlsX.h"
#include "worldX.h"
#include "elevegX.h"
#include "activeX.h"
#include "hlightX.h"
#include "statusX.h"
#include "caravanX.h"

/* markers for food / civilian limitations */
static int crew_limit;
static int food_limit;

/* UPGRADE_ARMY -- Upgrade the army unit */
static void
upgrade_army PARM_0(void)
{
  itemtype cost_mtrls[MTRLS_NUMBER];
  int i, choice;
  long men;

  /* check the selection */
  if ((army_ptr = get_armyselect()) == NULL) {
    errormsg("I couldn't find your army unit");
    return;
  }

  /* initialize */
  for (i = 0; i < MTRLS_NUMBER; i++) {
    cost_mtrls[i] = (itemtype) 0;
  }

  /* get the selection */
  if ((choice = enter_unittype(2, "Upgrade", "You cannot upgrade here"))
      == num_armytypes) {
    return;
  }

  /* query them */
  set_upgcosts(ntn_ptr, army_ptr, &(cost_mtrls[0]), choice);
  if (conq_expert == FALSE) {
    char bufstr[LINELTH];
    sprintf(bufstr, "Cost to upgrade:");
    for (i = 0; i < MTRLS_NUMBER; i++) {
      if (cost_mtrls[i] > 0) {
	sprintf(string, " %.0f %s", (double) cost_mtrls[i],
		mtrls_info[i].name);
	strcat(bufstr, string);
      }
    }
    strcat(bufstr, "; Continue? ");
    bottommsg(bufstr);
    if (y_or_n() == FALSE) {
      return;
    }

  }

  /* warn them */
  if ((conq_expert == FALSE) &&
      (is_god == FALSE) &&
      a_halfmen(ARMY_TYPE)) {
    bottommsg("Only half of the men are willing to be retrained. Continue? ");
    if (y_or_n() == FALSE) {
      return;
    }
  }

  /* now upgrade it! */
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

  /* adjust for half recruits */
  if ((is_god == FALSE) &&
      a_halfmen(ARMY_TYPE)) {
    men = ARMY_SIZE;
    ARMY_SIZE++;
    ARMY_SIZE /= 2;
    men -= ARMY_SIZE;
    if (men > 0) {
      sct[XREAL][YREAL].people += men;
      SADJPEOP;
    }
  }

  /* change it */
  ARMY_TYPE = choice;
  AADJTYPE;
  if ((is_god == FALSE) && (ARMY_MOVE != 0)) {
    ARMY_MOVE = 0;
    AADJMOVE;
  }
}

/* REMOTE_ENLIST -- Enlist a spy class unit from another population */
static void
remote_enlist PARM_0(void)
{
  itemtype cost_mtrls[MTRLS_NUMBER];
  int i, count, choice, okval;
  char bufstr[LINELTH];

  /* initialize */
  for (i = 0; i < MTRLS_NUMBER; i++) {
    cost_mtrls[i] = (itemtype) 0;
  }

  /* get the type */
  if ((choice = enter_unittype(3, "Recruit",
			       "You were unable to recruit agents here"))
      == num_armytypes) {
    return;
  }

  /* now test expenses */
  if (is_god == FALSE) {

    /* determine the expenses */
    set_armycosts(ntn_ptr, &(cost_mtrls[0]), choice, 1);

    /* check it */
    okval = TRUE;
    for (count = 0; count < MTRLS_NUMBER; count++) {
      if (ITEM_MTRLS[count] < cost_mtrls[count]) {
	sprintf(string, "You do not have %.0f %s available",
		(double) cost_mtrls[count],
		mtrls_info[count].lname);
	errormsg(string);
	okval = FALSE;
      }
    }
    if (okval == FALSE) return;

    /* query them */
    if (conq_expert == FALSE) {
      sprintf(bufstr, "Cost (%ld %s):", 1, ainfo_list[choice].name);
      for (count = 0; count < MTRLS_NUMBER; count++) {
	if (cost_mtrls[count] > 0) {
	  sprintf(string, " %.0f %s",
		  (double) cost_mtrls[count],
		  mtrls_info[count].name);
	  strcat(bufstr, string);
	}
      }
      strcat(bufstr, "; Continue? ");
      bottommsg(bufstr);
      if (y_or_n() == FALSE) {
	return;
      }
    }
  }

  /* check it */
  if ((army_ptr = crt_army(choice)) == NULL) {
    errormsg("You have too many army units");
    return;
  }

  /* set starting location */
  ACREATE;
  ARMY_YLOC = YREAL;
  ARMY_XLOC = XREAL;
  AADJLOC;
  sprintf(string, "Creating new army number %d", ARMY_ID);
  bottommsg(string);
  sleep(2);

  /* set status */
  set_speed(ARMY_STAT, SPD_NORMAL);
  set_status(ARMY_STAT, ST_DEFEND);
  set_healed(ARMY_STAT);
  ARMY_SIZE = 1;
  AADJSIZE;
  ARMY_SPLY = conq_supply_level;
  AADJSPLY;
  ARMY_MOVE = 0;
  AADJMOVE;
  ARMY_EFF = 95;
  AADJEFF;
  ARMY_MAXEFF = 100;
  AADJMAXEFF;
  AADJSTAT;
}

/* ENLIST_ARMY -- Enlist an army unit in the current sector */
static void
enlist_army PARM_0(void)
{
  itemtype cost_mtrls[MTRLS_NUMBER];
  int i, combine = FALSE, count, choice, effval;
  int start_stat = ST_DEFEND, okval;
  char bufstr[LINELTH];
  long unitsize;

  /* initialize */
  for (i = 0; i < MTRLS_NUMBER; i++) {
    cost_mtrls[i] = (itemtype) 0;
  }

  /* get the type */
  if ((choice = enter_unittype(0, "Enlist", "You are unable to enlist here"))
      == num_armytypes) {
    return;
  }

  /* find out how many */
  clear_bottom(0);
  if (a_isscout(choice)) {

    /* must be a single man */
    unitsize = 1;
    effval = 95;

  } else {

    /* special handling of mercenaries */
    if (a_ismerc(choice)) {

      /* can't get any mercs within a city under siege */
      effval = 90;
      if ((is_god == FALSE) &&
	  minor_desg(sct[XREAL][YREAL].designation, MIN_SIEGED)) {
	errormsg("The mercenary guild refuses hire while under siege");
	return;
      }

      /* get a specialized merc unit */
      if ((choice = enter_unittype(1, "Hire",
				   "You can't hire any mercs here"))
	  == num_armytypes) {
	return;
      }

    } else if (a_isundead(choice)) {
      effval = 100;
    } else {
      effval = 75;
    }
    
    /* find out how many */
    mvprintw(LINES - 2, 0, "%s how many %s? ",
	     a_ismerc(choice) ? "Hire" : "Enlist",
	     ainfo_list[choice].name);
    if (is_god == FALSE) {
      if (a_ismerc(choice)) {
	printw("[10 - %d] ", max_numunit(ntn_ptr, city_ptr, choice, 0));
      } else {
	printw("[max %d] ", max_numunit(ntn_ptr, city_ptr, choice, 0));
      }
    }
    clrtoeol();
    unitsize = get_number(FALSE);

    /* check limits */
    if ((no_input == TRUE) || (unitsize == 0)) {
      return;
    }
    if (is_god == FALSE) {
      if (a_ismerc(choice)) {
	if (unitsize < 10) {
	  errormsg("You could not find any mercenary groups that small");
	  return;
	}
	if (unitsize + conq_mercsused > MERCMEN / (2 * MAXNTN)) {
	  errormsg("You could not find enough mercenaries for hire");
	  return;
	}
      } else if ((!a_halfmen(choice) &&
		  (unitsize > min(CITY_PEOPLE,
				  sct[XREAL][YREAL].people))) ||
		 (unitsize > 2 * min(CITY_PEOPLE,
				     sct[XREAL][YREAL].people))) {
	errormsg("There are not that many recruits available");
	return;
      }
    }
  }

  /* now test expenses */
  if (is_god == FALSE) {

    /* determine the expenses */
    set_armycosts(ntn_ptr, &(cost_mtrls[0]), choice, unitsize);

    /* check it */
    okval = TRUE;
    for (count = 0; count < MTRLS_NUMBER; count++) {
      if (CITY_MTRLS[count] < cost_mtrls[count]) {
	sprintf(string, "You do not have %.0f %s available",
		(double) cost_mtrls[count],
		mtrls_info[count].lname);
	errormsg(string);
	okval = FALSE;
      }
    }
    if (okval == FALSE) return;

    /* query them */
    if (conq_expert == FALSE) {
      sprintf(bufstr, "Cost (%ld %s):",
	      unitsize, ainfo_list[choice].name);
      for (count = 0; count < MTRLS_NUMBER; count++) {
	if (cost_mtrls[count] > 0) {
	  sprintf(string, " %.0f %s",
		  (double) cost_mtrls[count],
		  mtrls_info[count].name);
	  strcat(bufstr, string);
	}
      }
      strcat(bufstr, "; Continue? ");
      bottommsg(bufstr);
      if (y_or_n() == FALSE) {
	return;
      }
    }
  }

  /* find new unit? */
  if ((is_god == FALSE) &&
      !a_isscout(choice)) {
    /* check current unit */
    if (((army_ptr = get_armyselect()) != NULL) &&
	(ARMY_TYPE == choice)) {
      /* check status */
      i = unit_status(ARMY_STAT);
      count = FALSE;
      if ((i == ST_GROUPED) &&
	  ((army_tptr = armybynum(ARMY_LEAD)) != NULL)) {
	i = unit_status(ARMYT_STAT);
      }
      if ((i != ST_SIEGED) &&
	  (nocomb_stat(i))) {
	count = TRUE;
      }

      /* query if needed */
      if (count == FALSE) {
	mvprintw(LINES - 3, 0, "Combine with unit number %d?", ARMY_ID);
	clrtoeol();
	refresh();
	if (y_or_n()) {
	  combine = TRUE;
	}
      }

    }
  }

  /* to combine or not to combine? */
  if (combine == FALSE) {
    /* check it */
    if ((army_ptr = crt_army(choice)) == NULL) {
      errormsg("You have too many army units");
      return;
    }

    /* set starting location */
    ACREATE;
    ARMY_YLOC = YREAL;
    ARMY_XLOC = XREAL;
    AADJLOC;
    sprintf(string, "Creating new army number %d", ARMY_ID);
    bottommsg(string);
    sleep(2);

    /* set status */
    if (is_god != TRUE) {
      if (minor_desg(sct_ptr->designation, MIN_SIEGED)) {
	start_stat = ST_SIEGED;
	set_speed(ARMY_STAT, SPD_SLOW);
      } else {
	if (!a_isscout(choice)) {
	  start_stat = ST_GARRISON;
	  set_speed(ARMY_STAT, SPD_SLOW);
	} else {
	  set_speed(ARMY_STAT, SPD_NORMAL);
	  start_stat = ST_DEFEND;
	}
      }
    }
    set_status(ARMY_STAT, start_stat);
  } else {
    sprintf(string, "Joining with army number %d", ARMY_ID);
    bottommsg(string);
    sleep(2);
  }

  /* now add it in; with proper level of supplies */
  set_healed(ARMY_STAT);
  if (ARMY_SPLY != conq_supply_level) {
    i = ARMY_SIZE * ARMY_SPLY + unitsize * conq_supply_level;
    i = (int) (i / (double) (ARMY_SIZE + unitsize) + 0.5);
    ARMY_SPLY = i;
    AADJSPLY;
  }
  if (ARMY_EFF != effval) {
    i = ((int) ARMY_EFF * ARMY_SIZE) + ((int) effval * unitsize);
    ARMY_EFF = (uns_char) (i / (ARMY_SIZE + unitsize));
    AADJEFF;
  }
  if (ARMY_MAXEFF != 100) {
    i = ((int) ARMY_MAXEFF * ARMY_SIZE) + ((int) 100 * unitsize);
    ARMY_MAXEFF = (uns_char) (i / (ARMY_SIZE + unitsize));
    AADJMAXEFF;
  }
  if (ARMY_SIZE == 0) {
    set_speed(ARMY_STAT, SPD_SLOW);
  }
  ARMY_SIZE += unitsize;
  AADJSIZE;
  ARMY_MOVE = 0;
  if (unit_status(ARMY_STAT) == ST_GROUPED) {
    ARMY_PTR ahold_ptr = army_ptr;
    if ((army_ptr = armybynum(ARMY_LEAD)) != NULL) {
      set_grpmove(ARMY_ID, SPD_SLOW, ARMY_MOVE);
      GADJMOVE;
    }
    army_ptr = ahold_ptr;
  } else {
    AADJMOVE;
    AADJSTAT;
  }    

  /* record costs and adjust units */
  if (is_god == FALSE) {
    for (count = 0; count < MTRLS_NUMBER; count++) {
      if (cost_mtrls[count] > 0) {
	CITY_MTRLS[count] -= cost_mtrls[count];
	CADJMTRLS(count);
	CITY_IMTRLS[count] -= cost_mtrls[count];
	if (CITY_IMTRLS[count] < 0) {
	  CITY_IMTRLS[count] = 0;
	}
	CADJIMTRLS(count);
      }
    }
    if (a_ismerc(ARMY_TYPE)) {
      conq_mercsused += unitsize;
      global_int = -unitsize;
      AADJMERC;
    } else {
      sct_ptr->people -= unitsize;
      SADJPEOP;
      if (a_halfmen(ARMY_TYPE)) {
	unitsize++;
	unitsize /= 2;
      }
      CITY_PEOPLE -= unitsize;
      CADJPEOP;
    }
  }

  /* perform the sort */
  army_sort(FALSE);
  hex_recalc();
}

/* ENLIST_NAVY -- Enlist a navy unit in the current sector */
static void
enlist_navy PARM_0(void)
{
  int i = 0, j, desg, number, type, size, okval;
  itemtype spt_talons, cost_mtrls[MTRLS_NUMBER];
  char qstr[LINELTH];
  long lvalue;

  /* must be in a harbor... or god */
  for (j = 0; j < MTRLS_NUMBER; j++) {
    cost_mtrls[j] = (itemtype) 0;
  }
  crew_limit = MAXNAVYCREW;
  food_limit = 4 * conq_supply_level;
  clear_bottom(0);

  /* check the type */
  mvaddstr(LINES - 4, 0, "Build ");
  for (i = 0; i < NSHP_NUMBER; i++) {
    if (i + 1 == NSHP_NUMBER) {
      addstr(" or ");
    } else if (i > 0) {
      addstr(", ");
    }
    hip_string(ninfo_list[i].name, 1, TRUE);
    qstr[i] = ninfo_list[i].name[0];
  }
  qstr[i] = '\0';
  addstr("? ");

  /* now get the input */
  refresh();
  i = one_char(qstr, TRUE);
  if (no_input == TRUE) {
    return;
  }
  for (type = 0; type < NSHP_NUMBER; type++) {
    if (i == ninfo_list[type].name[0]) break;
  }
  if (type == NSHP_NUMBER) {
    errormsg("I couldn't seem to locate your selection");
    return;
  }

  /* check the information */
  desg = major_desg(sct_ptr->designation);
  if ((is_god == TRUE) || (desg > MAJ_TOWN)) {
    size = N_HEAVY;
  } else if (desg == MAJ_TOWN) {
    size = N_MEDIUM;
  } else if (desg == MAJ_STOCKADE) {
    size = N_LIGHT;
  } else {
    errormsg("You may not build ships here... how did you get here?");
    return;
  }

  /* check the size */
  if (size == N_LIGHT) {
    mvaddstr(LINES - 3, 0, "You may only build light ships in this sector");
  } else {

    /* give an intro */
    mvaddstr(LINES - 3, 0, "Build ");
    for (i = 0; i <= N_HEAVY; i++) {
      if (i == size) addstr(" or ");
      else if (i != 0) addstr(", ");
      hip_string(shipsize[i], 1, (i <= size));
    }
    printw(" %s?", ninfo_list[type].name);
    refresh();

    /* now get the input and check it */
    number = next_char();
    if ((number == ' ') || (number == '\n') || (number == '\r'))
      return;
    if (islower(number)) number = toupper(number);
    for (i = 0; i <= N_HEAVY; i++) {
      if (shipsize[i][0] == number) break;
    }
    if (i > N_HEAVY) {
      errormsg("That was not among your choices");
      return;
    }
    if (i > size) {
      errormsg("This port is not capable of building that size ship");
      return;
    }
    size = i;
  }

  /* now find out how many */
  mvprintw(LINES - 2, 0, "Build how many %s %s? ", shipsize[size],
	   ninfo_list[type].name);
  refresh();
  number = get_number(FALSE);
  if ((no_input == TRUE) || (number == 0)) return;

  /* check it */
  if (number > N_MASK) {
    errormsg("You may not build that many ships");
    return;
  }

  /* now find out where it goes */
  navy_ptr = NULL;
  if (is_god == TRUE) {

    /* get any naval unit */
    if (ntn_ptr->navy_list != NULL) {
      mvaddstr(LINES -1, 0, "Add to what fleet? [none] ");
      i = get_number(FALSE);
      if (no_input != TRUE) {
	if ((navy_ptr = navybynum(i)) == NULL) {
	  errormsg("There is no such fleet in that nation");
	  return;
	}

      }
    }

  } else {

    /* check the crew */
    lvalue = (size + 1) * number * MAXNAVYCREW;
    if ((CITY_PEOPLE < lvalue) ||
	(sct_ptr->people < lvalue)) {
      lvalue = min(CITY_PEOPLE, sct_ptr->people);
      if (lvalue / ((size + 1) * number) <= 0) {
	sprintf(string, "There are no volunteers for your crew");
	errormsg(string);
	return;
      } else {
	lvalue /= ((size + 1) * number);
	sprintf(string, "You could only get %ld volunteers per hold; Cont?",
		lvalue);
	bottommsg(string);
	if (y_or_n() == FALSE) return;
      }
    } else {
      lvalue = MAXNAVYCREW;
    }
    crew_limit = lvalue;

    /* check for pricing */
    cost_mtrls[MTRLS_TALONS] =
      ninfo_list[type].taloncost * (size + 1) * number;
    cost_mtrls[MTRLS_WOOD] =
      ninfo_list[type].woodcost * (size + 1) * number;
    food_limit = crew_limit * (size + 1) * number;
    cost_mtrls[MTRLS_FOOD] =
      4 * conq_supply_level * food_limit;

    /* check for reductions */
    sct_cost_adjust(0, sct_ptr, &(cost_mtrls[0]));
    mgk_cost_adjust(2, &(cost_mtrls[0]));

    /* now for the expenses */
    okval = TRUE;
    for (j = 0; j < MTRLS_NUMBER; j++) {
      if (CITY_MTRLS[j] < cost_mtrls[j]) {
	if (j != MTRLS_FOOD) {
	  sprintf(string, "You do not have %.0f %s available",
		  (double) cost_mtrls[j],
		  mtrls_info[j].lname);
	  errormsg(string);
	  okval = FALSE;
	} else {
	  sprintf(string,
		  "There is only enough food for %.0f supplies; Continue?",
		  (double) CITY_MTRLS[j] / food_limit);
	  bottommsg(string);
	  if (y_or_n() == FALSE) return;
	  food_limit = CITY_MTRLS[j] / food_limit;
	  cost_mtrls[j] = food_limit * crew_limit * (size + 1) * number;
	}
      } else if (j == MTRLS_FOOD) {
	food_limit = 4 * conq_supply_level;
      }
    }
    if (okval == FALSE) return;

    /* now add in the costs for that amount of supplies */
    j = MTRLS_TALONS;
    spt_talons = food_limit * NAVYMAINT * (size + 1) * number;
    if (CITY_MTRLS[j] < cost_mtrls[j] + spt_talons) {
      food_limit = (CITY_MTRLS[j] - cost_mtrls[j]) /
	(NAVYMAINT * (size + 1) * number);
      sprintf(string,
	      "There is only enough talons for %d supplies; Continue?",
	      food_limit);
      bottommsg(string);
      if (y_or_n() == FALSE) return;
      spt_talons = food_limit * NAVYMAINT * (size + 1) * number;
    }
    cost_mtrls[j] += spt_talons;
    cost_mtrls[MTRLS_FOOD] = food_limit * crew_limit * (size + 1) * number;

    /* query them */
    if (conq_expert == FALSE) {
      mvprintw(LINES - 2, 0, "Building %d %s %s...",
	       number, shipsize[size], ninfo_list[type].name);
      clrtoeol();
      sprintf(string,
	      "Cost: %d Recruits %.0f Talons %.0f Wood %.0f Food; Continue?",
	      crew_limit * (size + 1) * number,
	      (double) cost_mtrls[MTRLS_TALONS],
	      (double) cost_mtrls[MTRLS_WOOD],
	      (double) cost_mtrls[MTRLS_FOOD]);
      bottommsg(string);
      if (y_or_n() == FALSE) {
	return;
      }
    }

    /* check for combining */
    if ((navy_ptr = get_navyselect()) != NULL) {
      mvprintw(LINES - 1, 0, "Combine with fleet %d?", NAVY_ID);
      clrtoeol();
      refresh();
      if (y_or_n() == FALSE) {
	navy_ptr = NULL;
      }
    }

  }

  /* create or check the unit */
  if (navy_ptr == NULL) {

    /* create a new fleet */
    if ((navy_ptr = crt_navy()) == NULL) {
      errormsg("There are too many fleets in the nation");
      return;
    }
    NCREATE;
    NAVY_XLOC = XREAL;
    NAVY_YLOC = YREAL;
    NADJLOC;
    NAVY_STAT = ST_SUPPLY;
    sprintf(string, "Creating new naval fleet #%d...", NAVY_ID);
    bottommsg(string);
    sleep(2);

  } else {

    /* check size */
    i = NAVY_CNTSHIPS(type, size);
    if (number + i > N_MASK) {
      errormsg("You can not have that many of one ship type in a fleet");
      return;
    }

    /* check status */
    if (nocomb_stat(NAVY_STAT)) {
      errormsg("The status of that ship prevent merging with others");
      return;
    }
  }

  /* no movement */
  if (NAVY_MOVE != 0) {
    NAVY_MOVE = 0;
    NADJMOVE;
  }
  set_speed(NAVY_STAT, SPD_SLOW);
  NADJSTAT;

  /* now add the unit */
  global_int = type;
  NAVY_SHIPS[type] = navy_addships(NAVY_SHIPS[type], size, number);
  NADJSHIP;

  /* check the rest */
  i = 0;
  for (j = 0 ; j < NSHP_NUMBER; j++) {
    i += navy_holds(navy_ptr, j);
  }
  j = (size + 1) * number;
  if (NAVY_CREW != crew_limit) {
    lvalue = (i - j) * (long) NAVY_CREW + j * (long) crew_limit;
    NAVY_CREW = (uns_char) (lvalue / i);
    NADJCREW;
  }
  if (NAVY_SPLY != food_limit) {
    lvalue = (i - j) * NAVY_SPLY + j * food_limit;
    NAVY_SPLY = (uns_char) (lvalue / (double) i + 0.5);
    NADJSPLY;
  }
  if ((type == 1) && (NAVY_PEOP != 0)) {
    i = navy_holds(navy_ptr, NSHP_GALLEYS);
    lvalue = (i - j) * NAVY_PEOP;
    NAVY_PEOP = (uns_char) (lvalue / i);
    NADJPEOP;
  }

  /* record costs */
  if (is_god == FALSE) {

    /* take the goods */
    for (j = 0; j < MTRLS_NUMBER; j++) {
      if (cost_mtrls[j] > 0) {
	CITY_MTRLS[j] -= cost_mtrls[j];
	CADJMTRLS(j);
	CITY_IMTRLS[j] -= cost_mtrls[j];
	if (CITY_IMTRLS[j] < 0) {
	  CITY_IMTRLS[j] = 0;
	}
	CADJIMTRLS(j);
      }
    }

    /* now take the people */
    lvalue = (size + 1) * number * crew_limit;
    sct_ptr->people -= lvalue;
    SADJPEOP;
    CITY_PEOPLE -= lvalue;
    CADJPEOP;

  }
  hex_recalc();
}

/* ENLIST_CVN -- Enlist a carvan in the current sector */
static void
enlist_cvn PARM_0(void)
{
  int i, number, okval;
  itemtype spt_talons, cost_mtrls[MTRLS_NUMBER];
  long lvalue;

  /* ask how many wagons to build */
  clear_bottom(0);
  for (i = 0; i < MTRLS_NUMBER; i++) {
    cost_mtrls[i] = (itemtype) 0;
  }
  crew_limit = MAXCVNCREW;
  food_limit = 2 * conq_supply_level;
  mvprintw(LINES - 3, 0, "Build how many sets of %d wagons? ",
	   WAGONS_IN_CVN);
  number = get_number(FALSE);
  if ((no_input == TRUE) || (number == 0)) return;

  /* check if it is the all mighty */
  if (is_god == FALSE) {

    /* check the crew */
    lvalue = number * MAXCVNCREW;
    if ((CITY_PEOPLE < lvalue) ||
	(sct_ptr->people < lvalue)) {
      lvalue = min(sct_ptr->people, CITY_PEOPLE);
      if (lvalue / number <= 0) {
	sprintf(string, "There are no volunteers for your crew");
	errormsg(string);
	return;
      } else {
	lvalue /= number;
	sprintf(string, "You could only get %ld volunteers per caravan; Cont?",
		lvalue);
	bottommsg(string);
	if (y_or_n() == FALSE) return;
      }
    } else {
      lvalue = MAXCVNCREW;
    }
    crew_limit = lvalue;

    /* check the price */
    cost_mtrls[MTRLS_TALONS] = CARAVANCOST * number;
    cost_mtrls[MTRLS_WOOD] = CARAVANWOOD * number;
    food_limit = crew_limit * number;
    cost_mtrls[MTRLS_FOOD] = food_limit * 2 * conq_supply_level;

    /* check adjustments */
    sct_cost_adjust(0, sct_ptr, &(cost_mtrls[0]));
    mgk_cost_adjust(0, &(cost_mtrls[0]));


    /* now for the expenses */
    okval = TRUE;
    for (i = 0; i < MTRLS_NUMBER; i++) {
      if (CITY_MTRLS[i] < cost_mtrls[i]) {
	if (i != MTRLS_FOOD) {
	  sprintf(string, "You do not have %.0f %s available",
		  (double) cost_mtrls[i],
		  mtrls_info[i].lname);
	  errormsg(string);
	  okval = FALSE;
	} else {
	  sprintf(string,
		  "There is only enough food for %.0f supplies; Continue?",
		  (double) CITY_MTRLS[i] / food_limit);
	  bottommsg(string);
	  if (y_or_n() == FALSE) return;
	  food_limit = CITY_MTRLS[i] / food_limit;
	  cost_mtrls[i] = food_limit * crew_limit * number;
	}
      } else if (i == MTRLS_FOOD) {
	food_limit = 2 * conq_supply_level;
      }
    }
    if (okval == FALSE) return;

    /* now add in the costs for that amount of supplies */
    i = MTRLS_TALONS;
    spt_talons = food_limit * CVNMAINT * number;
    if (CITY_MTRLS[i] < cost_mtrls[i] + spt_talons) {
      food_limit = (CITY_MTRLS[i] - cost_mtrls[i]) /
	(CVNMAINT * number);
      sprintf(string,
	      "There is only enough talons for %d supplies; Continue?",
	      food_limit);
      bottommsg(string);
      if (y_or_n() == FALSE) return;
      spt_talons = food_limit * CVNMAINT * number;
    }
    cost_mtrls[i] += spt_talons;
    cost_mtrls[MTRLS_FOOD] = food_limit * crew_limit * number;

    /* query them */
    if (conq_expert == FALSE) {
      mvprintw(LINES - 1, 0, "Building %d Wagons...",
	       number * WAGONS_IN_CVN);
      clrtoeol();
      sprintf(string,
	      "Cost: %d Recruits %.0f Talons %.0f Wood %.0f Food; Continue?",
	      crew_limit * number,
	      (double) cost_mtrls[MTRLS_TALONS],
	      (double) cost_mtrls[MTRLS_WOOD],
	      (double) cost_mtrls[MTRLS_FOOD]);
      bottommsg(string);
      if (y_or_n() == FALSE) {
	return;
      }
    }

    /* check for combining */
    if ((cvn_ptr = get_cvnselect()) != NULL) {
      mvprintw(LINES - 1, 0, "Combine with caravan %d?", CVN_ID);
      clrtoeol();
      if (y_or_n() == FALSE) {
	cvn_ptr = NULL;
      }
    } else {
      cvn_ptr = NULL;
    }

  } else {

    /* can add to any caravan */
    cvn_ptr = NULL;
    if (ntn_ptr->cvn_list != NULL) {

      mvaddstr(LINES - 1, 0, "Add to what carvan? [none] ");
      i = get_number(FALSE);
      if (no_input == FALSE) {
	if ((cvn_ptr = cvnbynum(i)) == NULL) {
	  errormsg("There is no such carvan in that nation");
	  return;
	}
      }
    }

  }

  /* build a new one */
  if (cvn_ptr == NULL) {
    if ((cvn_ptr = crt_cvn()) == NULL) {
      errormsg("There are too many caravans in the nation");
      return;
    }
    VCREATE;
    CVN_XLOC = XREAL;
    CVN_YLOC = YREAL;
    VADJLOC;
    if (minor_desg(sct_ptr->designation, MIN_SIEGED)) {
      CVN_STAT = ST_SIEGED;
    } else {
      CVN_STAT = ST_SUPPLY;
    }
    sprintf(string, "Creating new caravan #%d...", CVN_ID);
    set_speed(CVN_STAT, SPD_SLOW);
    bottommsg(string);
    sleep(2);

  }

  /* no movement */
  if (CVN_MOVE != 0) {
    CVN_MOVE = 0;
    VADJMOVE;
  }
  VADJSTAT;

  /* check crew */
  if (CVN_CREW != crew_limit) {
    lvalue = (long) CVN_CREW * CVN_SIZE + number * crew_limit;
    CVN_CREW = (uns_char) (lvalue / (CVN_SIZE + number));
    VADJCREW;
  }
  if (CVN_PEOP != 0) {
    lvalue = (long) CVN_PEOP * CVN_SIZE;
    CVN_PEOP = (uns_char) (lvalue / (CVN_SIZE + number));
    VADJPEOP;
  }
  if (CVN_SPLY != food_limit) {
    lvalue = (long) CVN_SPLY * CVN_SIZE + number * food_limit;
    CVN_SPLY = (uns_char) (lvalue / (double) (CVN_SIZE + number) + 0.5);
    VADJSPLY;
  }

  /* add wagons */
  CVN_SIZE += number;
  VADJSIZE;

  /* record costs */
  if (is_god == FALSE) {

    /* take the goods */
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

    /* now take the people */
    lvalue = number * crew_limit;
    sct_ptr->people -= lvalue;
    SADJPEOP;
    CITY_PEOPLE -= lvalue;
    CADJPEOP;

  }
  hex_recalc();
}

/* ENLIST -- Select whether to enlist armies, navies or caravans */
int
enlist PARM_0(void)
{
  ARMY_PTR a1_ptr = NULL;
  int slct_type = 0;

  /* check if they may enlist here */
  if (!XY_ONMAP(XREAL, YREAL) ||
      ((is_god == FALSE) &&
       (sct[XREAL][YREAL].owner != country) &&
       (VIS_CHECK(XREAL, YREAL) == HS_NOSEE))) {

    if (is_god == TRUE)
      errormsg("That sector is off the map");
    else errormsg("You cannot enlist troops in that sector");
    return(0);

  }

  /* quickie */
  sct_ptr = &(sct[XREAL][YREAL]);

  if (is_god == FALSE) {

    /* check sector */
    if (!IS_CITY(sct_ptr->designation)) {
      errormsg("It is not possible to recruit in this sector");
      return(0);
    }
    a1_ptr = get_armyselect();

  } else {

    /* now become a nation */
    if (get_god(-1, TRUE)) {
      return(0);
    }
    city_ptr = NULL;
    a1_ptr = NULL;

  }

  /* get city information */
  if ((is_god == TRUE) ||
      ((city_ptr = citybyloc(ntn_ptr, XREAL, YREAL)) != NULL)) {

    /* check people */
    if ((is_god == FALSE) && (CITY_PEOPLE < 0)) {
      errormsg("You may not recruit while the sector is under construction");
    } else {

      /* first check for unit upgrading */
      if ((a1_ptr != NULL) &&
	  may_upgrade(ntn_ptr, city_ptr, a1_ptr->unittype,
		      a1_ptr->strength)) {

	/* do they want to? */
	sprintf(string, "Do you wish to upgrade your %s %d? ",
		ainfo_list[a1_ptr->unittype].name,
		a1_ptr->armyid);
	bottommsg(string);
	if (y_or_n()) {
	  slct_type = 1;
	  upgrade_army();
	  if (is_god == TRUE) {
	    reset_god();
	  }
	  return(0);
	}

      }

      /* now perform the recruiting */
      clear_bottom(0);
      mvaddstr(LINES - 4, 0, "Enlist an ");
      hip_string("Army Unit", 1, TRUE);
      if ((is_god == TRUE) || IS_HARBOR(sct_ptr->designation)) {
	hip_string(", a Caravan, or a ", 5, TRUE);
	hip_string("Naval Unit? ", 1, TRUE);
      } else {
	hip_string(" or a Caravan? ", 7, TRUE);
      }

      /* get the input */
      refresh();
      switch(next_char()) {
      case ' ':
      case '\r':
      case '\n':
	/* out and about */
	break;
      case 'a':
      case 'A':
	/* do the army */
	slct_type = 1;
	enlist_army();
	break;
      case 'c':
      case 'C':
	/* do the caravan */
	slct_type = 2;
	enlist_cvn();
	break;
      case 'n':
      case 'N':
	/* possibly do the navy */
	if ((is_god == TRUE) || IS_HARBOR(sct_ptr->designation)) {
	  slct_type = 3;
	  enlist_navy();
	  break;
	}
      default:
	/* that ain't right */
	errormsg("That was not among your choices");
	break;
      }

    }

  } else if ((city_ptr = citybyloc(world.np[sct_ptr->owner],
				   XREAL, YREAL)) != NULL) {
    /* check for available resources */
    if ((item_ptr = find_resources(city_ptr->xloc, city_ptr->yloc, FALSE))
	!= NULL) {
      slct_type = 1;
      remote_enlist();
      free(item_ptr);
    } else {
      errormsg("You do not have any materials near enough to that center");
    }

  } else {
    /* woops */
    errormsg("That city seems to have fallen off the map");
  }

  /* take leave of this place*/
  if (is_god == TRUE) {
    reset_god();
  }
  hex_recalc();

  /* now switch to the appropriate unit */
  switch (slct_type) {
  case 1:
    if (army_ptr != NULL) {
      goto_army(ARMY_ID);
    }
    break;
  case 2:
    if (cvn_ptr != NULL) {
      goto_army(CVN_ID);
    }
    break;
  case 3:
    if (navy_ptr != NULL) {
      goto_army(NAVY_ID);
    }
    break;
  }
  if (is_god == TRUE) return(0);
  return(MOVECOST);
}
