// SPDX-License-Identifier: GPL-3.0-or-later
/* This file holds editing functions for information displays */
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
#define USE_CODES
#define PRINT_CODES
#include "dataG.h"
#include "armyX.h"
#include "cityX.h"
#include "infoG.h"
#include "navyX.h"
#include "mtrlsX.h"
#include "worldX.h"
#include "activeX.h"
#include "statusX.h"
#include "stringX.h"
#include "caravanX.h"
#include "dstatusX.h"

/* access the variables in infoG.c */
extern ARMY_PTR army_Pptr, army_Iptr;
extern NAVY_PTR navy_Pptr, navy_Iptr;
extern CITY_PTR city_Pptr, city_Iptr;
extern CVN_PTR cvn_Pptr, cvn_Iptr;
extern ITEM_PTR item_Pptr, item_Iptr;
extern NTN_PTR ntn_Pptr, ntn_Iptr;
extern int cntryI, cntryP;

/* GOD_CVNEDIT -- Adjust a caravan in god mode */
void
god_cvnedit PARM_2(int, line, int, cvnnum)
{
  long lval;
  int value, holdnum, xloc, yloc;

  /* verify the unit */
  if ((cvn_ptr = cvnbynum(cvnnum)) == NULL) {
    errormsg("There is no such caravan");
    return;
  }

  /* provide the options */
  mvaddstr(line++, 0, "Super User: ");
  hip_string("! Extcmds, ", 1, TRUE);
  hip_string("Relocate, ", 1, TRUE);
  hip_string("Adj Speed, ", 1, TRUE);
  hip_string("Size or ", 1, TRUE);
  hip_string("Efficiency,", 1, TRUE);
  move(line++, 3);
  hip_string("Change Status, ", 1, TRUE);
  hip_string("Mtrls, ", 1, TRUE);
  hip_string("Move Pts., or ", 6, TRUE);
  hip_string("CreW.  Your choice? ", 4, TRUE);
  refresh();

  /* parse input */
  switch (next_char()) {
  case ' ':
  case '\r':
  case '\n':
    /* quiet exit */
    break;
  case '!':
    /* call the normal routine */
    if (cvn_Pptr != NULL) {
      holdnum = cvn_Pptr->cvnid;
      cvn_Iptr = cvn_Pptr->next;
    } else {
      holdnum = -1;
      cvn_Iptr = NULL;
    }
    ext_cvncmd(cvnnum);

    if (cvnbynum(holdnum) == NULL) {
      if (cvn_Iptr == NULL) {
	cvn_Pptr = ntn_ptr->cvn_list;
      } else {
	cvn_Pptr = cvn_Iptr;
      }
    }
    break;
  case 'a':
  case 'A':
    /* is it possible */
    if (nomove_stat(CVN_STAT)) {
      errormsg("The speed of the unit is not adjustable");
      break;
    }

    /* find a new speed */
    clear_bottom(0);
    line = LINES - 4;
    mvaddstr(line, 0, "Set speed to what? ");
    value = get_speed();

    /* verify it */
    if (no_input == TRUE) break;
    if ((value < SPD_MINIMUM) || (value >= SPD_MINIMUM + SPD_NUMBER)) {
      errormsg("That is not a valid speed");
      break;
    }
    if (value >= SPD_STUCK) {
      errormsg("That speed is not possible for that status");
      break;
    }

    /* now set the speed */
    set_speed(CVN_STAT, value);
    VADJSTAT;
    break;
  case 'm':
  case 'M':
    /* change the materials */
    clear_bottom(0);
    line = LINES - 4;
    mvaddstr(line++, 0, "Adjust:  ");
    for (holdnum = 0; holdnum < MTRLS_NUMBER; holdnum++) {
      if (holdnum > 0) addstr(", ");
      addstr(mtrls_info[holdnum].name);
      string[holdnum] = mtrls_info[holdnum].name[0];
    }
    addstr("? ");
    string[holdnum] = '\0';

    /* get the input */
    xloc = one_char(string, TRUE);
    if (no_input == TRUE) break;
    for (holdnum = 0; holdnum < MTRLS_NUMBER; holdnum++) {
      if (xloc == mtrls_info[holdnum].name[0]) break;
    }
    if (holdnum == MTRLS_NUMBER) {
      errormsg("That was not a possible selection");
      break;
    }

    /* now find out how much */
    mvprintw(line, 0, "Set %s to what value? [%.0f] ",
	     mtrls_info[holdnum].lname,
	     (double) CVN_MTRLS[holdnum]);
    lval = get_number(FALSE);
    if (no_input == TRUE) break;

    /* now perform the adjustment */
    CVN_MTRLS[holdnum] = lval;
    VMTRLS(holdnum);
    break;
  case 'r':
  case 'R':
    /* get a new x and y location */
    clear_bottom(0);
    line = LINES - 4;
    xloc = CVN_XLOC;
    yloc = CVN_YLOC;
  getxloc:
    mvprintw(line, 0, "Move to what X sector? [default=%d] ", xloc);
    clrtoeol();
    value = get_number(FALSE);
    if (no_input == TRUE) {
      value = xloc;
    } else if (value < 0 || value >= MAPX) {
      errormsg("That X sector is off of the map");
      goto getxloc;
    }
    CVN_XLOC = value;
  getyloc:
    mvprintw(line + 1, 0, "Move to what Y sector? [default=%d] ", yloc);
    clrtoeol();
    value = get_number(FALSE);
    if (no_input == TRUE) {
      value = yloc;
    } else if (value < 0 || value >= MAPY) {
      errormsg("That Y sector is not on the map");
      goto getyloc;
    }
    CVN_YLOC = value;
    VADJLOC;
    mvaddstr(line + 2, 0, "Adjust Old Location? ");
    if (y_or_n()) {
      /* get a new x and y location */
      clear_bottom(0);
      xloc = CVN_LASTX;
      yloc = CVN_LASTY;
    getoxloc:
      mvprintw(line, 0, "Set to what X sector? [default=%d] ", xloc);
      clrtoeol();
      value = get_number(FALSE);
      if (no_input == TRUE) {
	value = xloc;
      } else if (value < 0 || value >= MAPX) {
	errormsg("That X sector is off of the map");
	goto getoxloc;
      }
      CVN_LASTX = value;
    getoyloc:
      mvprintw(line + 1, 0, "Set to what Y sector? [default=%d] ", yloc);
      clrtoeol();
      value = get_number(FALSE);
      if (no_input == TRUE) {
	value = yloc;
      } else if (value < 0 || value >= MAPY) {
	errormsg("That Y sector is not on the map");
	goto getoyloc;
      }
      CVN_LASTY = value;
      VADJOLOC;
    }
    break;
  case 'c':
  case 'C':
    /* adjust the status of the unit */
    clear_bottom(0);
    line = LINES - 4;
    mvaddstr(line, 0, "Set status to what? ");
    refresh();
    value = get_status();

    /* check it */
    if (no_input == TRUE) break;
    if (value < 0 || value >= num_statuses) {
      errormsg("There is no such status");
      break;
    }
    if (!vunit_stat(value)) {
      errormsg("That status is not possible for a caravan unit");
      break;
    }

    /* now set the status */
    set_status(CVN_STAT, value);
    VADJSTAT;
    break;
  case 's':
  case 'S':
    /* adjust the size in the unit */
    clear_bottom(0);
    line = LINES - 4;
    holdnum = CVN_SIZE;
    mvprintw(line, 0, "How many units of %d wagons in this caravan? [%d] ",
	     WAGONS_IN_CVN, holdnum * WAGONS_IN_CVN);
    clrtoeol();
    value = get_number(FALSE);
    if ((no_input == TRUE) || (value == holdnum)) break;;
    if (value > 255) {
      errormsg("That is too large a caravan");
      break;
    }
    CVN_SIZE = value;
    VADJSIZE;
    break;
  case 'p':
  case 'P':
    clear_bottom(0);
    line = LINES - 4;
    mvprintw(line, 0, "How many move points in this unit? [default=%d] ",
	     CVN_MOVE);
    refresh();
    value = get_number(FALSE);

    /* now verify it */
    if (no_input == TRUE) break;
    if (value < 0) {
      errormsg("I doubt that negative movement would be of much use");
      break;
    }
    if (value > 200) {
      errormsg("Isn't that a little extreme?");
      break;
    }
    CVN_MOVE = value;
    VADJMOVE;
    break;
  case 'e':
  case 'E':
    clear_bottom(0);
    line = LINES - 4;
    mvprintw(line, 0, "Set what efficiency percentage? [%d%%] ",
	     CVN_EFF);
    value = get_number(FALSE);
    if (no_input == TRUE) break;

    /* check range */
    if ((value < 0) || (value > 100)) {
      errormsg("The efficiency must be between 0 and 100 percent");
    } else {
      CVN_EFF = value;
      VADJEFF;
    }
    break;
  case 'w':
  case 'W':
    /* adjust the crew */
    clear_bottom(0);
    line = LINES - 4;
    mvprintw(line, 0, "What is the crew percentage? [default=%d] ",
	     (CVN_CREW * 100) / MAXCVNCREW);
    value = get_number(FALSE);
    if (no_input == TRUE) break;

    /* check it */
    if (value == CVN_CREW * 100 / MAXCVNCREW) break;
    if (value > 100) {
      errormsg("That is over the maximum percentage");
      break;
    }
    value = (value * MAXCVNCREW) / 100;
    CVN_CREW = value;
    VADJCREW;
    break;
  default:
    errormsg("That was not one of the possible choices");
    break;
  }
}

/* GOD_NAVYEDIT -- Adjust a navy in god mode */
void
god_navyedit PARM_2(int, line, int, navynum)
{
  long lval;
  int i, value, holdnum = 0, xloc, yloc;

  /* verify the unit */
  if ((navy_ptr = navybynum(navynum)) == NULL) {
    errormsg("There is no such fleet");
    return;
  }

  /* provide the options */
  mvaddstr(line++, 0, "Super User: ");
  hip_string("! Extcmds, ", 1, TRUE);
  hip_string("Relocate, ", 1, TRUE);
  hip_string("Adj Speed or ", 1, TRUE);
  hip_string("Ships, ", 1, TRUE);
  hip_string("Efficiency,", 1, TRUE);
  move(line++, 3);
  hip_string("Change Status, ", 1, TRUE);
  hip_string("Mtrls, ", 1, TRUE);
  hip_string("Move Pts or ", 6, TRUE);
  hip_string("or CreW.  Your choice? ", 7, TRUE);
  refresh();

  /* parse input */
  switch (next_char()) {
  case ' ':
  case '\r':
  case '\n':
    /* quiet exit */
    break;
  case '!':
    /* call the normal routine */
    if (navy_Pptr != NULL) {
      holdnum = navy_Pptr->navyid;
      navy_Iptr = navy_Pptr->next;
    } else {
      holdnum = -1;
      navy_Iptr = NULL;
    }
    ext_navycmd(navynum);

    if (navybynum(holdnum) == NULL) {
      if (navy_Iptr == NULL) {
	navy_Pptr = ntn_ptr->navy_list;
      } else {
	navy_Pptr = navy_Iptr;
      }
    }
    break;
  case 'a':
  case 'A':
    /* is it possible */
    if (nomove_stat(NAVY_STAT)) {
      errormsg("The speed is not adjustable for this unit");
      break;
    }

    /* find a new speed */
    clear_bottom(0);
    line = LINES - 4;
    mvaddstr(line, 0, "Set speed to what? ");
    value = get_speed();

    /* verify it */
    if (no_input == TRUE) break;
    if ((value < SPD_MINIMUM) || (value >= SPD_MINIMUM + SPD_NUMBER)) {
      errormsg("That is not a valid speed");
      break;
    }
    if (value >= SPD_STUCK) {
      errormsg("That speed is not possible for this unit");
      break;
    }

    /* now set the speed */
    set_speed(NAVY_STAT, value);
    NADJSTAT;
    break;
  case 'm':
  case 'M':
    /* change the materials */
    clear_bottom(4);
    line = LINES - 4;
    mvaddstr(line++, 0, "Adjust:  ");
    for (holdnum = 0; holdnum < MTRLS_NUMBER; holdnum++) {
      if (holdnum > 0) addstr(", ");
      addstr(mtrls_info[holdnum].name);
      string[holdnum] = mtrls_info[holdnum].name[0];
    }
    addstr("? ");
    string[holdnum] = '\0';

    /* get the input */
    xloc = one_char(string, TRUE);
    if (no_input == TRUE) break;
    for (holdnum = 0; holdnum < MTRLS_NUMBER; holdnum++) {
      if (xloc == mtrls_info[holdnum].name[0]) break;
    }
    if (holdnum == MTRLS_NUMBER) {
      errormsg("That was not a possible selection");
      break;
    }

    /* now find out how much */
    mvprintw(line, 0, "Set %s to what value? [%.0f] ",
	     mtrls_info[holdnum].lname,
	     (double) NAVY_MTRLS[holdnum]);
    lval = get_number(FALSE);
    if (no_input == TRUE) break;

    /* now perform the adjustment */
    NAVY_MTRLS[holdnum] = lval;
    NMTRLS(holdnum);
    break;
  case 'r':
  case 'R':
    /* get a new x and y location */
    clear_bottom(0);
    line = LINES - 4;
    xloc = NAVY_XLOC;
    yloc = NAVY_YLOC;
  getxloc:
    mvprintw(line, 0, "Move to what X sector? [default=%d] ", xloc);
    clrtoeol();
    value = get_number(FALSE);
    if (no_input == TRUE) {
      value = xloc;
    } else if (value < 0 || value >= MAPX) {
      errormsg("That X sector is off of the map");
      goto getxloc;
    }
    NAVY_XLOC = value;
  getyloc:
    mvprintw(line + 1, 0, "Move to what Y sector? [default=%d] ", yloc);
    clrtoeol();
    value = get_number(FALSE);
    if (no_input == TRUE) {
      value = yloc;
    } else if (value < 0 || value >= MAPY) {
      errormsg("That Y sector is not on the map");
      goto getyloc;
    }
    NAVY_YLOC = value;
    NADJLOC;
    mvaddstr(line + 2, 0, "Adjust Old Location? ");
    if (y_or_n()) {
      clear_bottom(0);
      /* get a new x and y location */
      xloc = NAVY_LASTX;
      yloc = NAVY_LASTY;
    getoxloc:
      mvprintw(line, 0, "Set to what X sector? [default=%d] ", xloc);
      clrtoeol();
      value = get_number(FALSE);
      if (no_input == TRUE) {
	value = xloc;
      } else if (value < 0 || value >= MAPX) {
	errormsg("That X sector is off of the map");
	goto getoxloc;
      }
      NAVY_LASTX = value;
    getoyloc:
      mvprintw(line + 1, 0, "Set to what Y sector? [default=%d] ", yloc);
      clrtoeol();
      value = get_number(FALSE);
      if (no_input == TRUE) {
	value = yloc;
      } else if (value < 0 || value >= MAPY) {
	errormsg("That Y sector is not on the map");
	goto getoyloc;
      }
      NAVY_LASTY = value;
      NADJOLOC;
    }
    break;
  case 'c':
  case 'C':
    /* adjust the status of the unit */
    clear_bottom(0);
    line = LINES - 4;
    mvaddstr(line, 0, "Set status to what? ");
    refresh();
    value = get_status();

    /* check it */
    if (no_input == TRUE) break;
    if (value < 0 || value >= num_statuses) {
      errormsg("There is no such status");
      break;
    }
    if (!nunit_stat(value)) {
      errormsg("That status is not possible for a naval unit");
      break;
    }

    /* now set the status */
    set_status(NAVY_STAT, value);
    NADJSTAT;
    break;
  case 's':
  case 'S':
    /* adjust the ships in the unit */
    clear_bottom(0);
    line = LINES - 4;
    for (global_int = 0; global_int < NSHP_NUMBER; global_int++) {
      for (i = N_LIGHT; i <= N_HEAVY; i++) {

	/* find the current amount */
	holdnum = NAVY_CNTSHIPS(global_int, i);

	/* find the new amount */
	mvprintw(line, 0, "How many %s %s in this fleet? [%d] ",
		 shipsize[i], ninfo_list[global_int].name,
		 holdnum);
	clrtoeol();
	value = get_number(FALSE);
	if ((no_input == TRUE) || (value == holdnum)) continue;
	if (value > N_MASK) {
	  errormsg("Fleets may not have that many of that ship type");
	  i--;
	  continue;
	}

	/* change it */
	NAVY_SHIPS[global_int] =
	  navy_addships(NAVY_SHIPS[global_int], i, value - holdnum);
	
      }
      NADJSHIP;
    }
    break;
  case 'e':
  case 'E':
    clear_bottom(0);
    line = LINES - 4;
    mvprintw(line, 0, "Set what efficiency percentage? [%d%%] ",
	     NAVY_EFF);
    value = get_number(FALSE);
    if (no_input == TRUE) break;

    /* check range */
    if ((value < 0) || (value > 100)) {
      errormsg("The efficiency must be between 0 and 100 percent");
    } else {
      NAVY_EFF[0] = value;
      NADJEFF(0);
    }
    break;
  case 'w':
  case 'W':
    /* adjust the crew */
    clear_bottom(0);
    line = LINES - 4;
    mvprintw(line, 0, "What is the crew percentage? [default=%d] ",
	     (NAVY_CREW * 100) / MAXNAVYCREW);
    value = get_number(FALSE);
    if (no_input == TRUE) break;

    /* check it */
    if (value == NAVY_CREW * 100 / MAXNAVYCREW) break;
    if (value > 100) {
      errormsg("That is over the maximum percentage");
      break;
    }
    value = (value * MAXNAVYCREW) / 100;
    NAVY_CREW = value;
    NADJCREW;
    break;
  case 'p':
  case 'P':
    clear_bottom(0);
    line = LINES - 4;
    mvprintw(line, 0, "How many move points in this unit? [default=%d] ",
	     NAVY_MOVE);
    refresh();
    value = get_number(FALSE);

    /* now verify it */
    if (no_input == TRUE) break;
    if (value < 0) {
      errormsg("I doubt that negative movement would be of much use");
      break;
    }
    if (value > 200) {
      errormsg("Isn't that a little extreme?");
      break;
    }
    NAVY_MOVE = value;
    NADJMOVE;
    break;
  default:
    errormsg("That was not one of the possible choices");
    break;
  }
}

/* GOD_ARMYEDIT -- Adjust an army in god mode */
void
god_armyedit PARM_3 (int, line, int, choice, int, armynum)
{
  long value;
  int xloc, yloc;

  /* verify the unit */
  if ((army_ptr = armybynum(armynum)) == NULL) {
    errormsg("There is no such unit");
    return;
  }

  /* now, provide the options */
  mvaddstr(line++, 0, "Super User: ");
  hip_string("! Extcmds, ", 1, TRUE);
  hip_string("Relocate, ", 1, TRUE);
  hip_string("Adj Spd, ", 1, TRUE);
  if (a_isleader(ARMY_TYPE)) {
    hip_string("Health, ", 1, TRUE);
    hip_string("SpelL Pts,", 5, TRUE);
  } else {
    hip_string("Efncy, ", 1, TRUE);
    hip_string("Max Eff, ", 1, TRUE);
    hip_string("SuppLy, ", 5, TRUE);
  }
  move(line++, 3);
  hip_string("Change Status, ", 1, TRUE);
  hip_string("Size, ", 1, TRUE);
  hip_string("Move Pts, or ", 6, TRUE);
  hip_string("Type; Your choice? ", 1, TRUE);
  refresh();

  /* parse input */
  switch (next_char()) {
  case ' ':
  case '\r':
  case '\n':
    /* quiet exit */
    break;
  case '!':
    /* call the normal routine */
    if ((army_Pptr != NULL) && (armynum == army_Pptr->armyid)) {
      value = TRUE;
      if (choice == INFO_ARMY) {
	army_Iptr = army_Pptr->next;
      } else {
	army_Iptr = army_Pptr->nrby;
      }
    } else {
      value = FALSE;
    }
    ext_armycmd(armynum);
    if ((value == TRUE) && (armybynum(armynum) == NULL)) {
      if (army_Iptr == NULL) {
	army_Pptr = ntn_ptr->army_list;
	if (choice == INFO_GRPARM) {
	  for (;
	       army_Pptr != NULL;
	       army_Pptr = army_Pptr->next) {
	    if ((army_Pptr->xloc == XREAL) &&
		(army_Pptr->yloc == YREAL)) break;
	  }
	}
      } else {
	army_Pptr = army_Iptr;
      }
    }
    break;
  case 'a':
  case 'A':
    /* find a new speed */
    clear_bottom(0);
    line = LINES - 4;
    mvaddstr(line, 0, "Set speed to what? ");
    refresh();
    value = get_speed();

    /* verify it */
    if (no_input == TRUE) break;
    if ((value < SPD_MINIMUM) || (value >= SPD_MINIMUM + SPD_NUMBER)) {
      errormsg("That is not a valid speed");
      break;
    }
    if (nomove_stat(ARMY_STAT)) {
      if (value < SPD_STUCK) {
	errormsg("That speed is not possible for this unit");
	break;
      }
      if (value == SPD_STUCK) {
	value = SPD_SLOW;
      } else {
	value = SPD_NORMAL;
      }
    } else {
      if (value >= SPD_STUCK) {
	errormsg("That speed is not possible for this unit");
	break;
      }
    }

    /* now set the speed */
    if (unit_leading(ARMY_STAT)) {
      set_grpmove(armynum, value, ARMY_MOVE);
      GADJMOVE;
    } else {
      set_speed(ARMY_STAT, value);
      AADJSTAT;
    }
    break;
  case 'l':
  case 'L':
    /* set the supply or spell points */
    clear_bottom(0);
    line = LINES - 4;
    if (a_isleader(ARMY_TYPE) ||
	a_castspells(ARMY_TYPE)) {
      mvprintw(line, 0, "Set the spell points to what? [%d] ",
	       ARMY_SPTS);
    } else {
      mvprintw(line, 0, "Set the supply value to what? [%d] ",
	       ARMY_SPLY);
    }
    clrtoeol();

    /* get the input */
    value = get_number(FALSE);
    if (no_input == TRUE) break;
    if (a_isleader(ARMY_TYPE) ||
	a_castspells(ARMY_TYPE)) {
      if (value > 200) {
	errormsg("That value is a little high");
      } else {
	ARMY_SPTS = (uns_char) value;
	AADJSPTS;
      }
    } else if (value > MAXSUPPLIES) {
      errormsg("That is over their supply limit");
    } else {
      ARMY_SPLY = (uns_char) value;
      AADJSPLY;
    }
    break;
  case 'm':
  case 'M':
    /* is it possible? */
    if (a_isleader(ARMY_TYPE)) {
      errormsg("Leaders maximum efficiency is always 100%");
      break;
    }

    /* adjust the maximum efficiency of the unit */
    clear_bottom(0);
    line = LINES - 4;
    mvprintw(line, 0, "Set the maximum efficiency to what? [%d%%] ",
	     ARMY_MAXEFF);
    clrtoeol();

    /* get and check the input */
    value = get_number(FALSE);
    if (no_input == TRUE) break;
    if ((value < 50) || (value > 150)) {
      errormsg("The maximum efficiency must be between 50 and 150");
    } else {
      ARMY_MAXEFF = value;
      AADJMAXEFF;
    }
    break;
  case 'e':
  case 'E':
  case 'h':
  case 'H':
    /* set the efficiency for the unit */
    clear_bottom(0);
    line = LINES - 4;
    if (a_isleader(ARMY_TYPE)) {
      mvprintw(line, 0, "Set the health of the leader to what? [%d%%] ",
	       ARMY_EFF);
    } else {
      mvprintw(line, 0, "Set the efficiency of the unit to what? [%d%%] ",
	       ARMY_EFF);
    }
    clrtoeol();

    /* get and check the input */
    value = get_number(FALSE);
    if (no_input == TRUE) break;
    if ((value < 0) || (value > ARMY_MAXEFF)) {
      sprintf(string, "The efficiency must be between 0 and %d percent",
	      ARMY_MAXEFF);
      errormsg(string);
    } else {
      ARMY_EFF = value;
      AADJEFF;
    }
    break;
  case 'r':
  case 'R':
    /* get a new x and y location */
    clear_bottom(0);
    line = LINES - 4;
    xloc = ARMY_XLOC;
    yloc = ARMY_YLOC;
  getxloc:
    mvprintw(line, 0, "Move to what X sector? [default=%d] ", xloc);
    clrtoeol();
    value = get_number(FALSE);
    if (no_input == TRUE) {
      value = xloc;
    } else if (value < 0 || value >= MAPX) {
      errormsg("That X sector is off of the map");
      goto getxloc;
    }
    ARMY_XLOC = value;
  getyloc:
    mvprintw(line + 1, 0, "Move to what Y sector? [default=%d] ", yloc);
    clrtoeol();
    value = get_number(FALSE);
    if (no_input == TRUE) {
      value = yloc;
    } else if (value < 0 || value >= MAPY) {
      errormsg("That Y sector is not on the map");
      goto getyloc;
    }
    ARMY_YLOC = value;
    AADJLOC;
    army_sort(FALSE);
    mvaddstr(line + 2, 0, "Adjust Old Location? ");
    if (y_or_n()) {
      clear_bottom(0);
      /* get a new x and y location */
      xloc = ARMY_LASTX;
      yloc = ARMY_LASTY;
    getoxloc:
      mvprintw(line, 0, "Set to what X sector? [default=%d] ", xloc);
      clrtoeol();
      value = get_number(FALSE);
      if (no_input == TRUE) {
	value = xloc;
      } else if (value < 0 || value >= MAPX) {
	errormsg("That X sector is off of the map");
	goto getoxloc;
      }
      ARMY_LASTX = value;
    getoyloc:
      mvprintw(line + 1, 0, "Set to what Y sector? [default=%d] ", yloc);
      clrtoeol();
      value = get_number(FALSE);
      if (no_input == TRUE) {
	value = yloc;
      } else if (value < 0 || value >= MAPY) {
	errormsg("That Y sector is not on the map");
	goto getoyloc;
      }
      ARMY_LASTY = value;
      AADJOLOC;
    }
    break;
  case 'c':
  case 'C':
    /* adjust the status of the unit */
    clear_bottom(0);
    line = LINES - 4;
    mvaddstr(line, 0, "Set status to what? ");
    refresh();
    value = get_status();

    /* check it */
    if (no_input == TRUE) break;
    if (value < 0 || value >= num_statuses) {
      errormsg("There is no such status");
      break;
    }
    if (!aunit_stat(value)) {
      errormsg("That status is not possible for an army unit");
      break;
    }

    /* now set the status */
    set_status(ARMY_STAT, value);
    AADJSTAT;
    break;
  case 's':
  case 'S':
    /* adjust the men in the unit */
    clear_bottom(0);
    line = LINES - 4;
    mvprintw(line, 0, "What is the new unit strength? [default=%d] ",
	     ARMY_SIZE);
    refresh();
    value = get_number(FALSE);

    /* check the input */
    if (no_input == TRUE) break;
    if (value < 0) {
      errormsg("Huh?  Trying to venture into the unknown?");
      break;
    }
    ARMY_SIZE = value;
    AADJSIZE;
    break;
  case 'p':
  case 'P':
    /* movement percentage available to the unit */
    clear_bottom(0);
    line = LINES - 4;
    mvprintw(line, 0, "How much movement potential in this unit? [dflt=%d] ",
	     ARMY_MOVE);
    refresh();
    value = get_number(FALSE);

    /* now verify it */
    if (no_input == TRUE) break;
    if (value < 0) {
      errormsg("I doubt that negative movement would be of much use");
      break;
    }
    if (value > 150) {
      errormsg("Isn't that a little extreme?");
      break;
    }
    if (value > 75 && nomove_stat(ARMY_STAT)) {
      errormsg("The status of the unit prevents that much movement");
      break;
    } else if (onship_stat(ARMY_STAT) &&
	       (value != 0)) {
      errormsg("Onboard units have no movement potential available");
      break;
    }
    ARMY_MOVE = value;
    AADJMOVE;
    break;
  case 't':
  case 'T':
    /* set the unit type */
    clear_bottom(0);
    line = LINES - 4;
    mvaddstr(line, 0, "What type should this unit become? ");
    refresh();
    value = get_atype();

    if (no_input == TRUE) break;
    if (value < 0 || value >= num_armytypes) {
      errormsg("There is no such army unit type");
      break;
    }
    ARMY_TYPE = value;
    AADJTYPE;
    break;
  default:
    errormsg("That was not one of the possible choices");
    break;
  }
}

/* EDIT_INFO -- Allow the user to alter some of the information */
void
edit_info PARM_2 (int, line, int, choice)
{
  long value;
  int i, number, holdnum, ship_info;

  /* clean up prior message */
  move(line, 0);
  clrtoeol();
  move(line + 1, 0);
  clrtoeol();
  move(line + 2, 0);
  clrtoeol();

  switch (choice) {
  case INFO_ARMY:
  case INFO_GRPARM:
    /* allow modification of army information */
    mvaddstr(line++, 0, "Options; ");
    hip_string("Adjust Unit, ", 1, TRUE);
    if (is_god == TRUE) {
      hip_string("Create Unit, ", 1, TRUE);
    }
    hip_string("Disband Unit: ", 1, TRUE);
    refresh();

    /* find out what needs doing */
    switch (next_char()) {
    case ' ':
    case '\n':
    case '\r':
      /* quick return on these keys */
      break;
    case 'a':
    case 'A':
      /* allow unit adjustment */
      mvaddstr(line++, 0, "Adjust what unit? ");
      refresh();
      value = get_number(FALSE);
      if (armybynum(value) == NULL) {
	errormsg("There is no such army unit");
      } else {
	if (is_god == TRUE) {
	  /* let god do the talking */
	  god_armyedit(line, choice, value);
	} else {
	  /* normal user interface */
	  if (army_Pptr != NULL) {
	    holdnum = army_Pptr->armyid;
	    if (choice == INFO_ARMY)
	      army_Iptr = army_Pptr->next;
	    else army_Iptr = army_Pptr->nrby;
	  } else {
	    holdnum = -1;
	    army_Iptr = NULL;
	  }
	  ext_armycmd(value);

	  /* check if it is still there */
	  if (armybynum(holdnum) == NULL) {
	    if (army_Iptr == NULL) {
	      army_Pptr = ntn_ptr->army_list;
	      if (choice == INFO_GRPARM) {
		for (;
		     army_Pptr != NULL;
		     army_Pptr = army_Pptr->next) {
		  if ((army_Pptr->xloc == XREAL) &&
		      (army_Pptr->yloc == YREAL)) break;
		}
	      }
	    } else {
	      army_Pptr = army_Iptr;
	    }
	  }
	    
	}
      }
      break;
    case 'd':
    case 'D':
      /* nuke one unit */
      mvaddstr(line++, 0, "Disband which unit? ");
      refresh();
      value = get_number(FALSE);

      /* verify unit */
      if ((army_ptr = armybynum(value)) == NULL) {
	errormsg("There is no such unit!");
	break;
      }

      /* check disbanding */
      if ((choice == INFO_GRPARM)
	  && ((ARMY_XLOC != XREAL) || (ARMY_YLOC != YREAL))) {
	errormsg("That unit is not among those in the current sector");
	break;
      }

      /* be sure to fix page information */
      if (army_Pptr == army_ptr) {
	if (choice == INFO_ARMY) {
	  army_Pptr = army_Pptr->next;
	} else {
	  army_Pptr = army_Pptr->nrby;
	}
	if (army_Pptr != NULL) {
	  army_Pptr = ntn_ptr->army_list;
	  if (choice == INFO_GRPARM) {
	    for (;
		 army_Pptr != NULL;
		 army_Pptr = army_Pptr->next) {
	      if ((army_Pptr->xloc == XREAL) &&
		  (army_Pptr->yloc == YREAL)) break;
	    }
	  }
	  if (army_Pptr == army_ptr) army_Pptr = NULL;
	}
	holdnum = ARMY_ID;
      } else {
	holdnum = -1;
      }

      /* now perform the disbanding */
      disb_army(army_ptr);

      /* is it still there */
      if ((holdnum != -1) &&
	  ((army_ptr = armybynum(holdnum)) != NULL)) {
	army_Pptr = army_ptr;
      }
      break;
    case 'c':
    case 'C':
      /* create a new unit */
      if (is_god == TRUE) {

	/* find the unit type */
	mvaddstr(line++, 0, "Create what type of unit? ");
	refresh();
	i = get_atype();
	if (no_input == TRUE) {
	  break;
	}

	/* check the result */
	if (i == num_armytypes) {
	  errormsg("There is no such unit type");
	} else {
	  if ((army_ptr = crt_army(i)) == NULL) {
	    errormsg("Could not create the new unit");
	  } else {
	    ACREATE;
	    if (a_isleader(i)) {
	      value = ainfo_list[i].minsth;
	    } else if (a_ismonster(i)) {
	      value = 1;
	    } else {
	      value = TAKESECTOR;
	    }
	    mvprintw(line, 0, "How strong is the unit? [default=%d] ", value);
	    number = get_number(FALSE);

	    /* check input */
	    if (no_input == TRUE) {
	      number = value;
	    }
	    ARMY_SIZE = number;
	    AADJSIZE;
	    ARMY_LASTX = ARMY_XLOC = XREAL;
	    ARMY_LASTY = ARMY_YLOC = YREAL;
	    AADJLOC;
	    AADJOLOC;
	    sprintf(string, "New %s Unit %d Created",
		    ainfo_list[ARMY_TYPE].name, ARMY_ID);
	    errormsg(string);
	    army_sort(FALSE);

	    /* now adjust the page pointer if needed */
	    if (army_Pptr == NULL) {
	      army_Pptr = army_ptr;
	    }
	  }
	}
	break;
      }
    default:
      /* invalid command */
      errormsg("That is not one of the available options!");
      break;
    }
    break;
  case INFO_NAVY:
    /* allow modification of navy information */
    mvaddstr(line++, 0, "Options; ");
    hip_string("Adjust Navy, ", 1, TRUE);
    if (is_god == TRUE) {
      hip_string("Create Navy, ", 1, TRUE);
    }
    hip_string("Disband Navy: ", 1, TRUE);
    refresh();

    /* find out what needs doing */
    switch (next_char()) {
    case ' ':
    case '\n':
    case '\r':
      /* quick return on these keys */
      break;
    case 'a':
    case 'A':
      /* allow navy adjustment */
      mvaddstr(line++, 0, "Adjust what fleet? ");
      value = get_number(FALSE);
      if (no_input == TRUE) break;
      if ((navybynum(value)) == NULL) {
	errormsg("There is no such navy");
      } else {
	if (is_god == TRUE) {
	  /* let god do the talking */
	  god_navyedit(line, value);
	} else {
	  /* normal user interface */
	  if (navy_Pptr != NULL) {
	    holdnum = navy_Pptr->navyid;
	    navy_Iptr = navy_Pptr->next;
	  } else {
	    holdnum = -1;
	    navy_Iptr = NULL;
	  }
	  ext_navycmd(value);

	  /* check if it is still there */
	  if (navybynum(holdnum) == NULL) {
	    if (navy_Iptr == NULL) navy_Pptr = ntn_ptr->navy_list;
	    else navy_Pptr = navy_Iptr;
	  }

	}
      }
      break;
    case 'd':
    case 'D':
      /* disband a navy */
      mvaddstr(line++, 0, "Disband which fleet? ");
      value = get_number(FALSE);
      if (no_input == TRUE) break;

      /* verify unit */
      if ((navy_ptr = navybynum(value)) == NULL) {
	errormsg("There is no such fleet!");
	break;
      }

      /* be sure to fix page information */
      if (navy_Pptr == navy_ptr) {
	navy_Pptr = navy_Pptr->next;
	if (navy_Pptr == NULL)
	  navy_Pptr = ntn_ptr->navy_list;
      }
      disb_navy(navy_ptr);
      break;
    case 'c':
    case 'C':
      /* create a new navy */
      if (is_god == TRUE) {

	if ((navy_ptr = crt_navy()) == NULL) {
	  errormsg("Could not create the new naval unit");
	} else {
	  NCREATE;
	  for (global_int = 0; global_int < NSHP_NUMBER; global_int++) {

	    ship_info = 0;
	    for (i = N_LIGHT; i <= N_HEAVY; i++) {

	    ship_number:
	      mvprintw(line, 0, "Have how many %s %s? ",
		       shipsize[i], ninfo_list[global_int].name);
	      clrtoeol();
	      number = get_number(FALSE);
	      if (no_input == TRUE) continue;
	      if (number > N_MASK) {
		errormsg("You may not have that many in a single naval unit");
		goto ship_number;
	      }
	      ship_info = navy_addships(ship_info, i, number);
	    }

	    NAVY_SHIPS[global_int] = ship_info;
	    NADJSHIP;
	  }

	  NAVY_LASTX = NAVY_XLOC = XREAL;
	  NAVY_LASTY = NAVY_YLOC = YREAL;
	  NADJLOC;
	  NADJOLOC;
	  NAVY_CREW = MAXCVNCREW;
	  NADJCREW;
	  sprintf(string, "New Naval Fleet %d Created", NAVY_ID);
	  errormsg(string);

	  /* now adjust the page pointer if needed */
	  if (navy_Pptr == NULL) {
	    navy_Pptr = navy_ptr;
	  }
	}
	break;
      }
    default:
      /* I don't know nothing! */
      errormsg("That isn't any of the options I told you about!");
      break;
    }
    break;
  case INFO_CVN:
    /* allow modification of caravan information */
    mvaddstr(line++, 0, "Options; ");
    hip_string("Adjust Caravan, ", 1, TRUE);
    if (is_god == TRUE) {
      hip_string("Create Caravan, ", 1, TRUE);
    }
    hip_string("Disband Caravan: ", 1, TRUE);
    refresh();

    /* find out what needs doing */
    switch (next_char()) {
    case ' ':
    case '\n':
    case '\r':
      /* quick return on these keys */
      break;
    case 'a':
    case 'A':
      /* allow caravan adjustment */
      mvaddstr(line++, 0, "Adjust what caravan? ");
      value = get_number(FALSE);
      if (no_input == TRUE) break;
      if ((cvnbynum(value)) == NULL) {
	errormsg("There is no such caravan");
      } else {
	if (is_god == TRUE) {
	  /* let god do the talking */
	  god_cvnedit(line, value);
	} else {
	  /* normal user interface */
	  if (cvn_Pptr != NULL) {
	    holdnum = cvn_Pptr->cvnid;
	    cvn_Iptr = cvn_Pptr->next;
	  } else {
	    holdnum = -1;
	    cvn_Iptr = NULL;
	  }
	  ext_cvncmd(value);

	  /* check if it is still there */
	  if (cvnbynum(holdnum) == NULL) {
	    if (cvn_Iptr == NULL) cvn_Pptr = ntn_ptr->cvn_list;
	    else cvn_Pptr = cvn_Iptr;
	  }

	}
      }
      break;
    case 'd':
    case 'D':
      /* disband a caravan */
      mvaddstr(line++, 0, "Disband which caravan? ");
      value = get_number(FALSE);
      if (no_input == TRUE) break;

      /* verify unit */
      if ((cvn_ptr = cvnbynum(value)) == NULL) {
	errormsg("There is no such caravan!");
	break;
      }

      /* be sure to fix page information */
      if (cvn_Pptr == cvn_ptr) {
	cvn_Pptr = cvn_Pptr->next;
	if (cvn_Pptr == NULL)
	  cvn_Pptr = ntn_ptr->cvn_list;
      }
      disb_cvn(cvn_ptr);
      break;
    case 'c':
    case 'C':
      /* create a new caravan */
      if (is_god == TRUE) {

	/* start it */
	if ((cvn_ptr = crt_cvn()) == NULL) {
	  errormsg("Could not create the new caravan");
	} else {
	  VCREATE;
	  number = 0;
	cvn_newsize:
	  while (number == 0) {
	    mvprintw(line, 0, "How many units of %d wagons in the caravan? ",
		     WAGONS_IN_CVN);
	    clrtoeol();
	    number = get_number(FALSE);
	    if ((no_input == TRUE) || (number == 0)) {
	      errormsg("You must have some wagons in it!");
	      number = 0;
	      continue;
	    }
	  }
	  if (number > 255) {
	    errormsg("Isn't that a little much?");
	    number = 0;
	    goto cvn_newsize;
	  }
	  CVN_SIZE = number;
	  VADJSIZE;

	  CVN_LASTX = CVN_XLOC = XREAL;
	  CVN_LASTY = CVN_YLOC = YREAL;
	  VADJLOC;
	  VADJOLOC;
	  CVN_CREW = MAXNAVYCREW;
	  VADJCREW;
	  sprintf(string, "New Caravan %d Created", CVN_ID);
	  errormsg(string);

	  /* now adjust the page pointer if needed */
	  if (cvn_Pptr == NULL) {
	    cvn_Pptr = cvn_ptr;
	  }
	}
	break;
      }
    default:
      /* unknown command */
      errormsg("That is not among the options I gave you!");
      break;
    }
    break;
  case INFO_CITY:
    /* allow minor alterations of city information */
    mvaddstr(line++, 0, "Manipulate which city? ");
    refresh();
    get_string(string, STR_CITY, NAMELTH);
    if (no_input == TRUE) break;
    if ((city_ptr = citybyname(string)) == NULL) {
      errormsg("There is nothing by that name in your nation");
      break;
    }

    /* now adjust the information */
    adjust_region(CITY_XLOC, CITY_YLOC);
    break;
  case INFO_ITEM:
    /* allow minor modifications to commodities */
    break;
  case INFO_NTN:
    /* allow some adjustments using score board? */
    break;
  case INFO_DIP:
    /* allow alteration of diplomacy status */
    mvaddstr(line++, 0, "(B)ribe a nation or (A)djust status with a nation? ");
    refresh();
    switch (next_char()) {
    case '\n':
    case '\r':
    case ' ':
      /* allow quick exit */
      break;
    case 'b':
    case 'B':
      /* check bribery of nations */
      mvaddstr(line++, 0, "A diplomatic mission will be sent to carry the bribe");
      mvaddstr(line++, 0, "Bribe which nation? ");
      value = get_country(FALSE);
      if (no_input == TRUE) break;
      if (value == UNOWNED) {
	errormsg("What use does a god have for mortal money?");
	break;
      }
      if (value == country) {
	errormsg("Using diplomats to embezzle funds isn't too bright");
	break;
      }
      if ((value < 0) || (value >= MAXNTN)) {
	errormsg("There is no such nation");
	break;
      }
      ntn_tptr = world.np[value];
      if (ntn_tptr == NULL) {
	errormsg("There is no such nation");
	break;
      }

      /* now check statuses */
      i = ntn_ptr->dstatus[value];
      number = ntn_tptr->dstatus[country] % dstatus_number;
      if ((i == DIP_UNMET) && (number == DIP_UNMET)) {
	errormsg("Your two nations have not yet met");
	break;
      }
      /* UNIMPLEMENTED */
      break;
    case 'a':
    case 'A':
      /* adjust nation status */
      clear_bottom(0);
      line = LINES - 4;
      mvaddstr(line++, 0, "Adjust your status with what nation? ");
      value = get_country(FALSE);
      if (no_input == TRUE) break;
      if (value == UNOWNED) {
	errormsg("You may not change your status with god");
	break;
      }
      if (value == country) {
	errormsg("Okay, you decide to take a new view towards yourself");
	break;
      }
      if ((value >= MAXNTN) || (value < 0)) {
	errormsg("There is no such nation");
	break;
      }
      ntn_tptr = world.np[value];
      if (ntn_tptr == NULL) {
	errormsg("There is no such nation");
	break;
      }

      /* check for monsters */
      if ((is_god == FALSE) &&
	  n_ismonster(ntn_tptr->active)) {
	errormsg("You may not adjust your status with a monster nation");
	break;
      }

      /* now check statuses */
      i = ntn_ptr->dstatus[value];
      if (i >= dstatus_number) {
	i = i / dstatus_number - 1;
      }
      number = ntn_tptr->dstatus[country] % dstatus_number;
      if (is_god == FALSE) {
	if ((i == DIP_UNMET) && (number == DIP_UNMET)) {
	  errormsg("Your two nations have not yet met");
	  break;
	}
	if ((i == DIP_ALLIED) &&
	    ((number == DIP_TREATY) || (number == DIP_ALLIED))) {
	  errormsg("You may not break an alliance");
	  break;
	}
	if ((i == DIP_JIHAD)) {
	  errormsg("You may not end a Jihad until all the infidels are dead");
	  break;
	}
      }

      /* adjust it */
      move(line, 0);
      number = get_diplomacy(i, number);
      if ((no_input == TRUE) || (i == number)) break;
      if (number >= dstatus_number) {
	errormsg("Huh?  That wasn't one of your choices!");
	break;
      }
      if (is_god == FALSE) {
	if (number == DIP_UNMET) {
	  errormsg("Huh?  That wasn't one of your choices!");
	  break;
	}
	if (((number > ntn_tptr->dstatus[country]) ||
	     (number < DIP_NEUTRAL)) &&
	    (abs(number - i) > MAXDIPADJ)) {
	  errormsg("You may not adjust your status that far in one turn");
	  break;
	}
      }

      /* assign it */
      global_int = value;
      ntn_ptr->dstatus[global_int] = number;
      if (is_god == FALSE) {
	ntn_ptr->dstatus[global_int] += (dstatus_number * (1 + i));
      }
      XADJDIPLO;
      break;
    default:
      /* el stupido! */
      errormsg("That isn't among your possible selections");
      break;
    }
    break;
  default:
    /* must be some kinda mistake */
    errormsg("Boy, I am clueless as to how this happened!");
    break;
  }
}
