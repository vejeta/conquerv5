// SPDX-License-Identifier: GPL-3.0-or-later
/* This file is used to allow quick retrieval of data */
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
#include "dataG.h"
#include "moveX.h"
#include "displayG.h"
#include "keyvalsX.h"

/* PICK_PREVIOUS -- Select the unit one above the current unit */
int
pick_previous PARM_0(void)
{
  /* back one */
  selector -= 2;
  if (selector < 0) {
    selector = (SCREEN_SIDE - 1) * 2;
    pager--;
  }

  /* move to last unit in the current sector */
  if (pager < 0) {
    selector = units_in_sector(country, XREAL, YREAL);
    if (selector > 0) {
      pager = (selector - 1) / SCREEN_SIDE;
      selector = (selector - (pager * SCREEN_SIDE) - 1) * 2;
    } else {
      selector = 0;
      pager = 0;
    }
  }
  return(0);
}

/* PICK_NEXT -- Select the unit one below the current unit */
int
pick_next PARM_0(void)
{
  /* one down... one to go... */
  selector += 2;
  if (selector >= SCREEN_SIDE * 2) {
    selector = 0;
    pager += 1;
  }

  /* current selected unit is selector / 2 + SCREEN_SIDE * pager */
  if (selector / 2 + (pager * SCREEN_SIDE) + 1 >
      units_in_sector(country, XREAL, YREAL)) {
    pager = 0;
    selector = 0;
  }
  return(0);
}

/* variables to hold the old selector information */
int scnd_selector = -1;
int scnd_pager = -1;

/* SET_SELECT -- Allow adjustment to the selector */
int
set_select PARM_2(int, type, char *, desc)
{
  /* type is as yet unused, but must be TRUE... to be limitation indicator */
  int keep_going = TRUE, mtype = 0;

  /* calculate the movement type */
  if (display_mode.style[display_mode.focus] == DI_FMOVE) {
    mtype = MOVEIND_FLY;
  } else if (display_mode.style[display_mode.focus] == DI_NMOVE) {
    mtype = MOVEIND_WATER;
  } else {
    mtype = MOVEIND_NORM;
  }

  /* show the usage information */
  scnd_pager = pager;
  scnd_selector = selector;

  /* now keep adjusting the selection */
  if (type) {

    /* continuous loop */
    while (keep_going) {

      /* showing side */
      makeside(FALSE, mtype);
      clear_bottom(0);

      /* now display the prompt */
      mvaddstr(LINES - 3, 0, "  'q' or SPACE to end selection");
      mvaddstr(LINES - 3, COLS / 2,
	       "'p' or RETURN to move downward");
      mvaddstr(LINES - 2, 0, "  'Q' or ESC to abort selection");
      mvaddstr(LINES - 2, COLS / 2,
	       "'o' or DELETE to move backward");
      mvprintw(LINES - 4, 0, "Unit Selection Mode -- %s", desc);

      /* now go back into the proper sector for display */
      show_cursor();
      refresh();

      /* now adjust the selection -- TEMPORARY NON-KEYBINDING METHOD */
      switch (next_char()) {
      case EXT_DEL:
      case '\b':
      case 'o':
	/* move backward */
	pick_previous();
	break;
      case '\n':
      case '\r':
      case 'p':
	/* move forward */
	pick_next();
	break;
      case 'q':
      case ' ':
	/* exit; selection done */
	scnd_selector = (-1);
	scnd_pager = (-1);
	return(TRUE);
      case 'Q':
      case EXT_ESC:
	/* exit and reset to original position */
	keep_going = FALSE;
	break;
      default:
	/* oops */
	errormsg("That key is unavailable in selection mode");
	break;
      }

    }

  }

  /* restore and exit */
  selector = scnd_selector;
  pager = scnd_pager;
  scnd_selector = (-1);
  scnd_pager = (-1);
  return(FALSE);
}

/* EXT_CMD -- Perform an extend command on a selected unit */
int
ext_cmd PARM_0(void)
{
  /* check the current selection */
  if (get_armyselect() != NULL) {
    ext_armycmd(-1);
  } else if (get_navyselect() != NULL) {
    ext_navycmd(-1);
  } else if (get_cvnselect() != NULL) {
    ext_cvncmd(-1);
  } else {
    errormsg("There is no unit selected.");
    return(0);
  }
  if (is_god == TRUE) {
    reset_god();
  }
  return(MOVECOST);
}

/* TRANS_CMD -- Load or unload the currently selected caravan or navy */
int
trans_cmd PARM_0(void)
{
  /* check the type */
  if (get_navyselect() != NULL) {
    navy_transport(NULL);
  } else if (get_cvnselect() != NULL) {
    cvn_transport(NULL);
  } else {
    errormsg("You must select a fleet or caravan to transport items");
  }

  /* all done */
  return(0);
}

/* GET_ARMYSELECT -- If current selection is an army return it */
ARMY_PTR 
get_armyselect PARM_0(void)
{
  ARMY_PTR a1_ptr;
  NTN_PTR n1_ptr;
  int c_start, c_end, c_cnt, army_count, hold;

  /* initialize */
  army_count = 0;
  hold = selector / 2 + pager * SCREEN_SIDE;
  if (country == UNOWNED) {
    c_start = 1;
    c_end = MAXNTN;
  } else {
    c_start = country;
    c_end = country + 1;
  }

  /* loop over the nations */
  for (c_cnt = c_start; c_cnt < c_end; c_cnt++) {

    /* check in input */
    if ((n1_ptr = world.np[c_cnt]) == NULL) continue;

    /* find armies in sector */
    for (a1_ptr = n1_ptr->army_list;
	 a1_ptr != NULL;
	 a1_ptr = a1_ptr->next) {
      if ((a1_ptr->xloc == XREAL) &&
	  (a1_ptr->yloc == YREAL)) break;
    }

    /*count to proper army in sector*/
    for ( ; a1_ptr != NULL;
	 a1_ptr = a1_ptr->nrby) {

      /* is there a match? */
      if (army_count++ >= hold) {
	/* store the nation where it is found */
	global_int = c_cnt;
	return(a1_ptr);
      }

    }
  }

  /* store the number of armies counted for later use */
  global_int = army_count;
  return((ARMY_PTR) NULL);
}

/* GET_NAVYSELECT -- If current selection is a navy return it */
NAVY_PTR
get_navyselect PARM_0(void)
{
  NAVY_PTR n1_ptr;
  NTN_PTR nt_ptr;
  int c_start, c_end, c_cnt;
  int navy_count = 0, hold;

  /* count preceeding army units */
  if (get_armyselect() != NULL)
    return ((NAVY_PTR)NULL);
  navy_count += global_int;

  /* find all navy units in sector */
  hold = selector / 2 + pager * SCREEN_SIDE;
  if (country == UNOWNED) {
    c_start = 1;
    c_end = MAXNTN;
  } else {
    c_start = country;
    c_end = country + 1;
  }

  /* loop over the nations */
  for (c_cnt = c_start; c_cnt < c_end; c_cnt++) {

    /* check in input */
    if ((nt_ptr = world.np[c_cnt]) == NULL) continue;

    /* loop over the fleets */
    for (n1_ptr = nt_ptr->navy_list;
	 (n1_ptr != NULL);
	 n1_ptr = n1_ptr->next) {

      /* find one? */
      if ((n1_ptr->xloc == XREAL) &&
	  (n1_ptr->yloc == YREAL)) {
	if (navy_count++ >= hold) {
	  /* this is it */
	  global_int = c_cnt;
	  return(n1_ptr);
	}
      }

    }
  }

  /* return results */
  global_int = navy_count;
  return ((NAVY_PTR)NULL);
}

/* GET_CVNSELECT -- If current selection is a caravan return it */
CVN_PTR
get_cvnselect PARM_0(void) 
{
  CVN_PTR c1_ptr;
  NTN_PTR nt_ptr;
  int c_start, c_end, c_cnt;
  int cvn_count = 0, hold;

  /* check if selected unit is not a caravan */
  if ((get_armyselect() != NULL) ||
      (get_navyselect() != NULL))
    return ((CVN_PTR)NULL);
  cvn_count += global_int;

  /* now search for the proper caravan in the sector */
  hold = selector / 2 + pager * SCREEN_SIDE;
  if (country == UNOWNED) {
    c_start = 1;
    c_end = MAXNTN;
  } else {
    c_start = country;
    c_end = country + 1;
  }

  /* loop over the nations */
  for (c_cnt = c_start; c_cnt < c_end; c_cnt++) {

    /* check in input */
    if ((nt_ptr = world.np[c_cnt]) == NULL) continue;

    /* loop over the caravans */
    for (c1_ptr = nt_ptr->cvn_list;
	 (c1_ptr != NULL);
	 c1_ptr = c1_ptr->next) {

      /* find one? */
      if ((c1_ptr->xloc == XREAL) &&
	  (c1_ptr->yloc == YREAL)) {
	/* this it? */
	if (cvn_count++ >= hold) {
	  global_int = c_cnt;
	  return(c1_ptr);
	}
      }

    }
  }

  /* return failure */
  global_int = cvn_count;
  return ((CVN_PTR)NULL);
}

/* SET_ARMYSELECT -- Set the selection indicator to the desired army */
void
set_armyselect PARM_1 (int, idnum)
{
  ARMY_PTR a1_ptr;
  NTN_PTR nt_ptr;
  int c_start, c_end, c_cnt;
  int hold = 0;

  /* set beginning loop */
  if (country == UNOWNED) {
    c_start = 1;
    c_end = MAXNTN;
  } else {
    c_start = country;
    c_end = country + 1;
  }

  /* peruse the nation lists */
  for (c_cnt = c_start; c_cnt < c_end; c_cnt++) {

    /* check in input */
    if ((nt_ptr = world.np[c_cnt]) == NULL) continue;

    /* find the first unit in the current sector */
    for (a1_ptr = nt_ptr->army_list;
	 a1_ptr != NULL;
	 a1_ptr = a1_ptr->next) {
      if ((a1_ptr->xloc == XREAL) &&
	  (a1_ptr->yloc == YREAL)) {
	break;
      }
    }

    /* now find the proper army unit */
    for ( ; (a1_ptr != NULL);
	 a1_ptr = a1_ptr->nrby) {
      /* set it if found */
      if (a1_ptr->armyid == idnum) {
	pager = hold / SCREEN_SIDE;
	selector = (hold % SCREEN_SIDE) * 2;
      }
      hold++;
    }

  }
}

/* SET_NAVYSELECT -- Set the selection indicator to the desired navy */
void
set_navyselect PARM_1 (int, idnum)
{
  NAVY_PTR n1_ptr;
  NTN_PTR nt_ptr;
  int c_start, c_end, c_cnt;
  int hold = 0;

  /* set beginning loop */
  if (country == UNOWNED) {
    c_start = 1;
    c_end = MAXNTN;
  } else {
    c_start = country;
    c_end = country + 1;
  }

  /* peruse the nation lists */
  for (c_cnt = c_start; c_cnt < c_end; c_cnt++) {

    /* check in input */
    if ((nt_ptr = world.np[c_cnt]) == NULL) continue;

    /* scan all of the naval units */
    for (n1_ptr = nt_ptr->navy_list;
	 n1_ptr != NULL;
	 n1_ptr = n1_ptr->next) {

      /* look for units in the sector */
      if ((n1_ptr->xloc == XREAL) &&
	  (n1_ptr->yloc == YREAL)) {
	if (n1_ptr->navyid == idnum) {
	  hold += armies_in_sector(country, XREAL, YREAL);
	  pager = hold / SCREEN_SIDE;
	  selector = (hold % SCREEN_SIDE) * 2;
	}
	hold++;
      }
    }

  }
}

/* SET_CVNSELECT -- Set the selection indicator to the desired caravan */
void
set_cvnselect PARM_1 (int, idnum)
{
  CVN_PTR c1_ptr;
  NTN_PTR nt_ptr;
  int c_start, c_end, c_cnt;
  int hold = 0;

  /* set beginning loop */
  if (country == UNOWNED) {
    c_start = 1;
    c_end = MAXNTN;
  } else {
    c_start = country;
    c_end = country + 1;
  }

  /* peruse the nation lists */
  for (c_cnt = c_start; c_cnt < c_end; c_cnt++) {

    /* check in input */
    if ((nt_ptr = world.np[c_cnt]) == NULL) continue;

    /* scan all of the units */
    for (c1_ptr = nt_ptr->cvn_list;
	 (c1_ptr != NULL);
	 c1_ptr = c1_ptr->next) {

      /* sector sitter */
      if ((c1_ptr->xloc == XREAL) &&
	  (c1_ptr->yloc == YREAL)) {
	/* found it */
	if (c1_ptr->cvnid == idnum) {
	  hold += armies_in_sector(country, XREAL, YREAL);
	  hold += navies_in_sector(country, XREAL, YREAL);
	  pager = hold / SCREEN_SIDE;
	  selector = (hold % SCREEN_SIDE) * 2;
	}
	hold++;
      }

    }
  }
}

/* GOTO_CITY -- set the current sector to the desired city */
void
goto_city PARM_1 (CITY_PTR, c1_ptr)
{
  /* if god, just bop out */
  if (c1_ptr == NULL) return;

  /* set XREAL, YREAL and selector */
  xcurs = ((int) c1_ptr->xloc) - xoffset;
  ycurs = ((int) c1_ptr->yloc) - yoffset;
  selector = 0;
  pager = 0;
}

/* GOTO_ARMY -- set the current sector and selector to the desired army */
int
goto_army PARM_1 (int, idnum)
{
  ARMY_PTR a1_ptr;

  /* if only god, just bop out */
  if (country == UNOWNED) return(TRUE);

  /* select the army */
  if (idnum == (-1)) {
    a1_ptr = get_armyselect();
    if (a1_ptr == NULL || (a1_ptr = a1_ptr->next) == NULL)
      a1_ptr = ntn_ptr->army_list;
    if (a1_ptr == NULL) {
      errormsg("You have no army units to go to... oh well.");
      return(TRUE);
    }
    idnum = a1_ptr->armyid;
  } else {
    a1_ptr = armybynum(idnum);
  }

  if (a1_ptr != NULL) {
    /* set XREAL, YREAL and selector */
    if (XREAL != a1_ptr->xloc) {
      xcurs = ((int) a1_ptr->xloc) - xoffset;
    }
    if (YREAL != a1_ptr->yloc) {
      ycurs = ((int) a1_ptr->yloc) - yoffset;
    }
    set_armyselect(idnum);
  } else {
    /* unknown army */
    errormsg("Hmm... that unit only exists in your imagination");
    return(TRUE);
  }
  return(FALSE);
}

/* GOTO_NAVY -- set the current sector and selector to the desired navy */
int
goto_navy PARM_1 (int, idnum)
{
  NAVY_PTR n1_ptr;

  /* if god, just bop out */
  if (country == UNOWNED) return(TRUE);

  /* select the navy */
  if (idnum == (-1)) {
    n1_ptr = get_navyselect();
    if ((n1_ptr == NULL) || ((n1_ptr = n1_ptr->next) == NULL))
      n1_ptr = ntn_ptr->navy_list;
    if (n1_ptr == NULL) {
      errormsg("You have no navies to go to");
      return(TRUE);
    }
    idnum = n1_ptr->navyid;
  } else {
    n1_ptr = navybynum(idnum);
  }

  if (n1_ptr != NULL) {
    /* set XREAL, YREAL and selector */
    if (XREAL != n1_ptr->xloc) {
      xcurs = ((int) n1_ptr->xloc) - xoffset;
    }
    if (YREAL != n1_ptr->yloc) {
      ycurs = ((int) n1_ptr->yloc) - yoffset;
    }
    set_navyselect(idnum);
  } else {
    /* unknown navy */
    errormsg("Hmm... that fleet only exists in your imagination");
    return(TRUE);
  }
  return(FALSE);
}

/* GOTO_CVN -- set the current sector and selector to the desired caravan */
int
goto_cvn PARM_1 (int, idnum)
{
  CVN_PTR c1_ptr;

  /* if god, just bop out */
  if (country == UNOWNED) return(TRUE);

  /* select the caravan */
  if (idnum == (-1)) {
    c1_ptr = get_cvnselect();
    if (c1_ptr == NULL || (c1_ptr = c1_ptr->next) == NULL)
      c1_ptr = ntn_ptr->cvn_list;
    if (c1_ptr == NULL) {
      errormsg("You have no caravans to go to");
      return(TRUE);
    }
    idnum = c1_ptr->cvnid;
  } else {
    c1_ptr = cvnbynum(idnum);
  }

  if (c1_ptr != NULL) {
    /* set XREAL, YREAL and selector */
    if (YREAL != c1_ptr->xloc) {
      xcurs = ((int) c1_ptr->xloc) - xoffset;
    }
    if (YREAL != c1_ptr->yloc) {
      ycurs = ((int) c1_ptr->yloc) - yoffset;
    }
    set_cvnselect(idnum);
  } else {
    /* unknown caravan */
    errormsg("Hmm... that caravan only exists in your imagination");
    return(TRUE);
  }
  return(FALSE);
}
