// SPDX-License-Identifier: GPL-3.0-or-later
/* This file holds all of the commands to operate over a supply region */
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
#include "dataG.h"
#define USE_CODES
#define PRINT_CODES
#include "armyX.h"
#include "cityX.h"
#include "navyX.h"
#include "butesX.h"
#include "desigX.h"
#include "mtrlsX.h"
#include "worldX.h"
#include "stringX.h"
#include "caravanX.h"

/* ADJUST_REGION -- Affect a change on a region */
void
adjust_region PARM_2(int, x, int, y)
{
  int use_city = TRUE, dvalue, lines, range;
  int failed = 0, total_num = 0, choice, inp_val, value;
  char newnamestr[LINELTH];

  /* check the input */
  if (!XY_ONMAP(x, y)) {
    errormsg("Error:  invalid location in function adjust_region()");
    return;
  }
  sct_ptr = &(sct[x][y]);
  if ((sct_ptr->owner != country) ||
      !IS_CITY(sct_ptr->designation)) {

    /* not using the full potential */
    use_city = FALSE;
    range = 0;
    dvalue = 0;

  } else {

    /* now get the city */
    dvalue = major_desg(sct_ptr->designation);
    if ((city_ptr = citybyloc(ntn_ptr, x, y)) == NULL) {
      sprintf(string, "Error:  could not find your %s in [%d,%d]",
	      maj_dinfo[dvalue].name, x, y);
      errormsg(string);
      return;
    }

    /* get the information */
    range = r10_region(ntn_ptr, city_ptr, sct_ptr);
  }

  /* can now operate on it */
  clear_bottom(0);
  lines = LINES - 5;
  if (use_city) {
    mvprintw(lines++, 0, "%s %s => ", maj_dinfo[dvalue].name, CITY_NAME);
    printw("Supply Range: %d.%d", range / 10, range % 10);
    range /= 10;
  } else {
    mvprintw(lines++, 0, "Nation %s Adjustment Command",
	     ntn_ptr->name);
  }

  /* now display the options */
  if (use_city) {
    move(lines++, 2);
    if (is_god == TRUE) {
      hip_string("Adjust Stores, ", 1, TRUE);
      hip_string("Volunteers, ", 1, TRUE);
    }
    hip_string("Rename, ", 1, TRUE);
    hip_string("Transfer, ", 1, TRUE);
    hip_string("Weighting, ", 1, TRUE);
  }
  move(lines++, 2);
  if (MAXSUPPLIES > 0) {
    hip_string("Supply Armies, ", 1, TRUE);
    hip_string("Caravans, or ", 1, TRUE);
    hip_string("Navies", 1, TRUE);
  }
  move(lines++, 0);
  addstr("Perform which regional command? ");
  refresh();

  /* select it */
  switch (next_char()) {
  case ' ':
  case '\n':
  case '\r':
    /* quick out */
    break;
  case 'R':
  case 'r':
    /* rename the supply center */
    if (use_city == FALSE) {
      errormsg("That was not one of the available options");
      break;
    }
    mvprintw(lines, 0, "Rename your %s to what? ",
	     maj_dinfo[dvalue].name);

    /* get the new name */
    if ((get_string(newnamestr, STR_SPACE, NAMELTH) == 0) ||
	(no_input == TRUE)) {
      break;
    }

    /* check the city name */
    if (((city_tptr = citybyname(newnamestr)) != NULL) &&
	(city_tptr != city_ptr)) {
      errormsg("You have already given that name to something");
      break;
    }

    /* now change it */
    CADJNAME;
    strcpy(CITY_NAME, newnamestr);
    city_sort();
    break;
  case 't':
  case 'T':
    /* transfer items between neighboring cities */
    if (use_city == FALSE) {
      errormsg("That was not one of the available options");
      break;
    }
    if (minor_desg(sct_ptr->designation, MIN_SIEGED)) {
      errormsg("You cannot transfer items out of a city under siege");
      break;
    }
    clear_bottom(4);
    lines = LINES - 4;
    mvprintw(lines++, 0, "Transferring materials from %s to...",
	     CITY_NAME);

    /* now find where it goes */
    mvaddstr(lines++, 0, "Exchange resources with which supply center? ");
    get_string(string, STR_CITY, NAMELTH);
    if (no_input == TRUE) break;
    if ((city_tptr = citybyname(string)) == NULL) {
      errormsg("There is nothing by that name in your nation");
      break;
    }

    /* now verify that it is within range */
    if (city_ptr == city_tptr) {
      errormsg("All of the materials are already there.");
      break;
    } else if ((is_god == FALSE) &&
	!map_within(CITY_XLOC, CITY_YLOC, city_tptr->xloc,
		    city_tptr->yloc, MAX_TRANSFER)) {
      sprintf(string,
	      "Supply centers must be within %d sectors to transfer directly",
	      MAX_TRANSFER);
      errormsg(string);
      break;
    } else if (minor_desg(sct[city_tptr->xloc][city_tptr->yloc].designation,
			  MIN_SIEGED)) {
      errormsg("That city is under siege and cannot be reached");
      break;
    }

    /* now transfer the goods */
    city2xfer(0, city_ptr);
    city2xfer(1, city_tptr);
    xfer_mode();
    xfer2city(0, city_ptr);
    xfer2city(1, city_tptr);
    break;
  case 'v':
  case 'V':
    /* adjust the recruits for the sector */
    if ((use_city == FALSE) ||
	(is_god == FALSE)) {
      errormsg("That was not one of the available options");
      break;
    }
    clear_bottom(0);
    if (is_god == TRUE) {
      long rnum = CITY_PEOPLE;
      if (rnum < 0) {
	mvprintw(LINES - 3, 0,
		 "The supply center has no recruits due to construction for %ld months",
		 - rnum);
      } else {
	mvprintw(LINES - 3, 0,
		 "There are currently %ld volunteers in the supply center",
		 rnum);
      }
      mvprintw(LINES - 2, 0, "Set recruits to what? [%ld] ",
	       rnum);
      rnum = get_number(TRUE);
      if (no_input == TRUE) break;

      /* check ranges */
      if (rnum < -5) {
	mvaddstr(LINES - 1, 0, "Construction cannot take more than 5 months");
	break;
      }
      if (rnum > CITY_PEOPLE) {
	mvprintw(LINES - 1, 0, "There are only %ld people in the sector",
		 CITY_PEOPLE);
	break;
      }
      if ((rnum < 0) &&
	  (CITY_PEOPLE > 0)) {
	mvaddstr(LINES - 1, 0,
		 "Make the supply center be under construction? ");
	if (y_or_n() == FALSE) break;
      } else if ((rnum > 0) &&
		 (CITY_PEOPLE < 0)) {
	mvaddstr(LINES - 1, 0,
		 "End the construction of the sector? ");
	if (y_or_n() == FALSE) break;
      }
      CITY_PEOPLE = rnum;
      CADJPEOP;
    }
    break;
  case 'a':
  case 'A':
    /* adjust items */
    if ((use_city == FALSE) ||
	(is_god == FALSE)) {
      errormsg("That was not one of the available options");
      break;
    }
    clear_bottom(0);
    if (is_god == TRUE) {

      /* build the prompt string */
      clear_bottom(4);
      lines = LINES - 4;
      mvaddstr(lines++, 0, "Adjust:  ");

      /* get the input */
      choice = get_mtrls(NULL);
      if (no_input == TRUE) break;
      if (choice == MTRLS_NUMBER) {
	errormsg("That was not a possible selection");
	break;
      }

      /* negatives okay for talons */
      if (choice == MTRLS_TALONS) {
	inp_val = TRUE;
      } else {
	inp_val = FALSE;
      }

      /* now find out how much */
      mvprintw(lines, 0, "Set %s to what value? [%.0f] ",
	       mtrls_info[choice].lname,
	       (double) CITY_MTRLS[choice]);
      value = get_number(inp_val);
      if (no_input == TRUE) break;

      /* now perform the adjustment */
      CITY_MTRLS[choice] = value;
      CADJMTRLS(choice);

    }
    break;
  case 'w':
  case 'W':
    /* change the supply center weighting value */
    if (use_city == FALSE) {
      errormsg("That was not one of the available options");
      break;
    }
    mvprintw(lines++, 0, "Set the weighting for %s to be what? ",
	     CITY_NAME);

    /* get the value */
    value = get_number(FALSE);
    if (no_input == TRUE) break;

    /* check the settings */
    if (value < dflt_cityweight(country, city_ptr)) {
      sprintf(string, "You may not set the weight below the base value of %d",
	      dflt_cityweight(country, city_ptr));
      errormsg(string);
      break;
    } else if (value > 200) {
      errormsg("You may not set the weighting above 200");
      break;
    }

    /* done */
    CITY_WEIGHT = value;
    CADJWEIGHT;
    set_weights(FALSE);
    break;
  case 'S':
  case 's':
    /* army supplies */
    if (MAXSUPPLIES == 0) {
      errormsg("That was not one of the options I gave you");
      break;
    }

    /* check what level to set the supply at */
    mvprintw(lines++, 0, "Set the national army supply level to what? ");
    clrtoeol();
    inp_val = get_number(FALSE);
    if (no_input == TRUE) break;
    if (inp_val > MAXSUPPLIES) {
      sprintf(string, "Troops may carry only up to %d months of supplies",
	      MAXSUPPLIES);
      errormsg(string);
      break;
    }

    /* now go through all of the army units */
    for (army_ptr = ntn_ptr->army_list;
	 army_ptr != NULL;
	 army_ptr = army_ptr->next) {

      /* check if they need to be supplied */
      if (a_freesupply(ARMY_TYPE)) continue;

      /* now set the value around */
      if (supply_army(army_ptr, inp_val, FALSE)) {
	failed++;
      }
      total_num++;

    }

    /* report */
    if (failed != 0) {
      sprintf(string, "%d out of %d army units supply level unchanged",
	      failed, total_num);
      errormsg(string);
    }
    break;
  case 'n':
  case 'N':
    /* army supplies */
    if (MAXSUPPLIES == 0) {
      errormsg("That was not one of the options I gave you");
      break;
    }

    /* check what level to set the supply at */
    mvprintw(lines++, 0, "Set the national navy supply level to what? ");
    clrtoeol();
    inp_val = get_number(FALSE);
    if (no_input == TRUE) break;
    if (inp_val > 4 * MAXSUPPLIES) {
      sprintf(string, "Navies may carry only up to %d months of supplies",
	      4 * MAXSUPPLIES);
      errormsg(string);
      break;
    }

    /* now go through all of the army units */
    for (navy_ptr = ntn_ptr->navy_list;
	 navy_ptr != NULL;
	 navy_ptr = navy_ptr->next) {

      /* now set the value around */
      if (supply_navy(navy_ptr, inp_val, FALSE)) {
	failed++;
      }
      total_num++;

    }

    /* report */
    if (failed != 0) {
      sprintf(string, "%d out of %d army units supply level unchanged",
	      failed, total_num);
      errormsg(string);
    }
    break;
  case 'C':
  case 'c':
    /* caravan supplies */
    if (MAXSUPPLIES == 0) {
      errormsg("That was not one of the options I gave you");
      break;
    }

    /* check what level to set the supply at */
    mvprintw(lines++, 0, "Set the national caravan supply level to what? ");
    clrtoeol();
    inp_val = get_number(FALSE);
    if (no_input == TRUE) break;
    if (inp_val > 2 * MAXSUPPLIES) {
      sprintf(string, "Caravans may carry only up to %d months of supplies",
	      2 * MAXSUPPLIES);
      errormsg(string);
      break;
    }

    /* now go through all of the army units */
    for (cvn_ptr = ntn_ptr->cvn_list;
	 cvn_ptr != NULL;
	 cvn_ptr = cvn_ptr->next) {

      /* now set the value around */
      if (supply_cvn(cvn_ptr, inp_val, FALSE)) {
	failed++;
      }
      total_num++;

    }

    /* report */
    if (failed != 0) {
      sprintf(string, "%d out of %d caravans supply level unchanged",
	      failed, total_num);
      errormsg(string);
    }
    break;
  default:
    /* complain */
    errormsg("That was not one of the options I gave you");
    break;
  }
}

/* REGION_CMD -- Attempt to send commands to the current supply center */
int
region_cmd PARM_0(void)
{
  /* check if they can operate over a region here */
  if (!XY_ONMAP(XREAL, YREAL) ||
      ((is_god == FALSE) &&
       (sct[XREAL][YREAL].owner != country))) {
    if (is_god == TRUE) {
      errormsg("That sector is not on the map");
    } else {
      errormsg("That sector is not owned by you");
    }
    return(0);
  }

  /* check the sector */
  sct_ptr = &(sct[XREAL][YREAL]);

  /* now take on the nation properties */
  if (is_god == TRUE) {
    if (sct_ptr->owner == UNOWNED) {
      errormsg("This command must by used in owned land");
      return(0);
    }
    if (get_god(sct_ptr->owner, TRUE)) return(0);
  }

  /* try to adjust the supply center */
  adjust_region(XREAL, YREAL);

  /* all done */
  if (is_god == TRUE) reset_god();
  return(0);
}
