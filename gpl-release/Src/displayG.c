// SPDX-License-Identifier: GPL-3.0-or-later
/* This file creates the map and sidebar displays for conquer */
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
#include "armyX.h"
#include "cityX.h"
#include "navyX.h"
#include "moveX.h"
#include "calenX.h"
#include "desigX.h"
#include "magicX.h"
#include "mtrlsX.h"
#include "racesX.h"
#include "worldX.h"
#include "activeX.h"
#include "elevegX.h"
#include "hlightX.h"
#include "statusX.h"
#include "stringX.h"
#include "tgoodsX.h"
#include "caravanX.h"
#include "displayG.h"
#include "keyvalsX.h"
#include "patchlevel.h"

/* this variable is only used between set_select() and make_side() */
extern int scnd_selector, scnd_pager;

/* MAKEMAP -- Display the visible map to the screen */
void
makemap PARM_0(void)
{
  register int x, y;
  int xmax, ymax;

  /* go along the entire screen, skipping along the proper points */
  xmax = max_xcurs();
  for (x = 0; x < xmax; x++) {

    /* now go along the columns */
    ymax = max_ycurs(x + xoffset);

    /* go along the column */
    for (y = 0; y < ymax; y++) {

      /* show the sector */
      show_sect(x + xoffset, y + yoffset, -1, -1, 2);

    }
  }

  /* now go to the proper cursor position */
  show_cursor();
}

/* GET_TARGET -- Routine to select the setting of the target */
static int
get_target PARM_1(int, style)
{
  int hold;

  /* check it out */
  switch (style) {
  case HI_MINDESG:
    do {
      mvaddstr(LINES - 2, 0, "Highlight which minor designation? ");
      clrtoeol();
      hold = get_mindesg();
      if (no_input == TRUE) return(-2);
      if (hold == MIN_NUMBER) {
	errormsg("That is not a minor designation");
      }
    } while (hold == MIN_NUMBER);
    break;
  case HI_MAJDESG:
    do {
      mvaddstr(LINES - 2, 0, "Highlight which major designation? ");
      clrtoeol();
      hold = get_designation();
      if (no_input == TRUE) return(-2);
      if (hold == MAJ_NUMBER) {
	errormsg("That is not a major designation");
      }
    } while (hold == MAJ_NUMBER);
    break;
  case HI_OWN:
    do {
      mvaddstr(LINES - 2, 0, "Highlight land owned by whom? [*=all] ");
      clrtoeol();
      hold = get_country(FALSE);
      if (no_input == TRUE) return(-2);
      if (hold == MAXNTN) {
	if (strcmp(string, "*") == 0) {
	  hold = UNOWNED;
	} else {
	  errormsg("That is not a nation that I know about?");
	}
      } else if (hold < 0) {
	hold = MAXNTN;
	errormsg("That is not a valid nation");
      }
    } while (hold == MAXNTN);
    break;
  case HI_TGOODS:
    do {
      mvaddstr(LINES - 2, 0, "Highlight which tradegood class? [*=all] ");
      clrtoeol();
      hold = get_tgclass();
      if (no_input == TRUE) return(-2);
      if (hold == tgclass_number) {
	if (strcmp(string, "*") == 0) {
	  hold = TG_NONE;
	} else {
	  errormsg("That is not a tradegood class that I know about?");
	}
      } else if (hold < 0) {
	hold = MAXNTN;
	errormsg("That is not a valid tradegood class");
      }
    } while (hold == tgclass_number);
    break;
  default:
    /* no value here */
    hold = 0;
    break;
  }

  /* get it out */
  return(hold);
}

/* CHANGE_HMODE -- Change the current highlighting method */
static void
change_hmode PARM_1(int, maskval)
{
  int value, count, tvalue;

  /* get a new highlight setting */
  value = get_hd_info(TRUE);

  /* check the input */
  if (no_input == TRUE) return;
  if (value == HI_MAXIMUM + 1) {
    errormsg("That is not a possible highlight option");
    return;
  }

  /* does it need a new target setting? */
  if (hl_targets(value)) {
    if ((tvalue = get_target(value)) == -2) {
      return;
    }
  } else {
    tvalue = 0;
  }

  /* now secure it */
  for (count = 0; count < HXPOS_NUMBER; count++) {
    if (maskval & (1 << count)) {
      display_mode.highlight[count] = value;
      display_mode.target[count] = tvalue;
    }
  }
}

/* == The various user interfaces to the highlight changes == */

/* HL_ALL -- Change all of the highlights */
int
hl_all PARM_0(void)
{
  change_hmode((int) 0x0F);
  return(0);
}

/* HL_ULLR -- Change upper left and lower right highlights */
int
hl_ullr PARM_0(void)
{
  change_hmode((int) 0x06);
  return(0);
}

/* HL_URLL -- Change the upper right and lower left highlights */
int
hl_urll PARM_0(void)
{
  change_hmode((int) 0x09);
  return(0);
}

/* HL_VERT -- Change the verticle highlighting */
int
hl_vert PARM_0(void)
{
  change_hmode((display_mode.focus % 2)?
	       ((int) 0x0A):((int) 0x05));
  return(0);
}

/* HL_HORZ -- Change the horizontal highlighting */
int
hl_horz PARM_0(void)
{
  change_hmode((display_mode.focus / 2)?
	       ((int) 0x0C):((int) 0x03));
  return(0);
}

/* HL_CURRENT -- Change the current focus highlight */
int
hl_current PARM_0(void)
{
  change_hmode((int)(1L << display_mode.focus));
  return(0);
}

/* SHIFT_FOCUS -- Shift the focus slot to the next position */
int
shift_focus PARM_0(void)
{
  display_mode.focus = (display_mode.focus + 1) % HXPOS_NUMBER;
  return(0);
}

/* SHIFT_RFOCUS -- Shift the focus slot to the next position */
int
shift_rfocus PARM_0(void)
{
  display_mode.focus = ((display_mode.focus + HXPOS_NUMBER - 1)) %
    HXPOS_NUMBER;
  return(0);
}

/* CHANGE_DMODE -- Change the curent display mode to the provided one */
static void
change_dmode PARM_2(DISPLAY_PTR, d1_ptr, int, keepkeep)
{
  int count;

  /* check the input */
  if (d1_ptr == NULL) return;

  /* keep the name as well */
  strcpy(display_mode.name, d1_ptr->name);

  /* now adjust things needing adjustment */
  display_mode.focus = d1_ptr->focus;

  /* now finally fill the slots */
  for (count = 0; count < HXPOS_NUMBER; count++) {
    if ((keepkeep == TRUE) ||
	(d1_ptr->style[count] != DI_KEEP)) {
      display_mode.style[count] = d1_ptr->style[count];
    }
    if ((keepkeep == TRUE) ||
	(d1_ptr->highlight[count] != HI_KEEP)) {
      display_mode.highlight[count] = d1_ptr->highlight[count];
      display_mode.target[count] = d1_ptr->target[count];
    }
  }
}

/* DMODE_BYNAME -- Return pointer to the given display mode if it exists */
static DMODE_PTR
dmode_byname PARM_1(char *, dname)
{
  /* simple fetch it */
  for (dmode_tptr = dmode_list;
       dmode_tptr != NULL;
       dmode_tptr = dmode_tptr->next) {
    if (strcmp(dmode_tptr->d.name, dname) == 0) break;
  }
  return(dmode_tptr);
}

/* CUSTOM_DISPLAY -- Allow interactive customization of the display */
int
custom_display PARM_0(void)
{
  DISPLAY_STRUCT hold_display;
  DISPLAY_PTR dtmp_ptr;
  DMODE_PTR work_mode;
  char buffer[LINELTH];
  int mainone, xloc, yloc, count;
  int abandon = FALSE, done = FALSE;

  /* selection of the focus sector */
  if ((ntn_ptr == NULL) ||
      (XY_ONMAP(XREAL, YREAL) &&
       (VIS_CHECK(XREAL, YREAL) != HS_NOSEE))) {
    xloc = XREAL;
    yloc = YREAL;
  } else {
    xloc = ntn_ptr->capx;
    yloc = ntn_ptr->capy;
  }

  /* find out what mode needs editing */
  mainone = FALSE;
  work_mode = get_dmode("Customize which display mode? [* for main mode] ");
  if (no_input == TRUE) {
    return(0);
  } else if (work_mode == NULL) {
    if (strcmp(string, "*") != 0) {
      sprintf(buffer, "Create new display mode called \"%s\"? ", string);
      bottommsg(buffer);
      if (y_or_n() == FALSE) {
	return(0);
      }
      if ((work_mode = crt_dmode(string)) == NULL) {
	return(0);
      }
      dtmp_ptr = &(work_mode->d);
    } else {
      mainone = TRUE;
      dtmp_ptr = &display_mode;
    }
  } else {
    dtmp_ptr = &(work_mode->d);
  }

  /* store all of the current mode */
  strcpy(hold_display.name, display_mode.name);
  hold_display.focus = display_mode.focus;
  for (count = 0; count < HXPOS_NUMBER; count++) {
    hold_display.style[count] = display_mode.style[count];
    hold_display.highlight[count] = display_mode.highlight[count];
    hold_display.target[count] = display_mode.target[count];
  }

  /* use the current mode */
  change_dmode(dtmp_ptr, TRUE);

  /* loop until complete */
  done = FALSE;
  while (done == FALSE) {

    /* clean up the bottom */
    clear_bottom(0);

    /* show the customization commands */
    mvprintw(LINES - 4, 1, "Name: \"%s\"", display_mode.name);
    mvprintw(LINES - 3, 1, "Focus: %s", hex_list[display_mode.focus]);
    mvaddstr(LINES - 2, 1, " '?'=command list");
    mvprintw(LINES - 1, 1, " 'q'=exit  ESC=abort");

    /* show the display techniques */
    mvaddstr(LINES - 4, COLS / 2 - 17,
	     "Display and Highlighting Selections");
    mvaddstr(LINES - 2, COLS / 2 - 15,
	     "---------------------+---------------------");
    for (count = 0; count < HXPOS_NUMBER; count++) {

      /* relocate to a nice neighborhood */
      move(LINES - ((count / 2) * 2 + 1),
	   COLS / 2 - 15 + (21 * (count % 2)));

      /* add the pipe character */
      if (count % 2) {
	addch('|');
	addch(' ');
      }

      /* perform any needed frills */
      if (display_mode.focus == count) {
	standout();
      }

      /* show the display and highlight selections */
      printw("%.9s / %.9s",
	     display_list[display_mode.style[count]],
	     highl_list[display_mode.highlight[count]]);

      /* unperform any needed frills */
      if (display_mode.focus == count) {
	standend();
      }

    }

    /* give visual display of the mode */
    show_sect(xloc, yloc, COLS - 8, LINES - 2, 3);
    move(LINES - 2 - display_mode.focus / 2,
	 COLS - 8 + 1 + display_mode.focus % 2);
    refresh();

    /* parse it */
    switch (next_char()) {
    case EXT_ESC:
      bottommsg("Abort the display mode edit? ");
      clrtoeol();
      if (y_or_n()) {
	done = TRUE;
	abandon = TRUE;
	if (mainone) {
	  change_dmode(&hold_display, TRUE);
	} else {
	  change_dmode(dtmp_ptr, TRUE);
	}
      }
      break;
    case '(':
      /* reverse shift the focus */
      shift_rfocus();
      break;
    case ' ':
    case '\n':
    case '\r':
    case '\t':
    case '+':
    case ')':
      /* shift the focus */
      shift_focus();
      break;
    case '?':
      /* provide the command summaries */
      clear_bottom(0);

      /* first the basic commands */
      mvaddstr(LINES - 5, 1, "'D' - set display at the focus");
      mvaddstr(LINES - 4, 1, "'N' - change the mode name");
      mvaddstr(LINES - 3, 1, "'R' - reset the settings");
      mvaddstr(LINES - 2, 1, "' ' - shift the focus");
      mvaddstr(LINES - 1, 1, "'Q' - quit display modifier");

      /* now the highlighting commands */
      mvaddstr(LINES - 5, COLS / 2, "'H','.' - set highlight at focus");
      mvaddstr(LINES - 4, COLS / 2, "'='     - set all highlights");
      mvaddstr(LINES - 3, COLS / 2, "'/','\\' - set diagonal highlights");
      mvaddstr(LINES - 2, COLS / 2, "'-'     - set horizontal highlights");
      mvaddstr(LINES - 1, COLS / 2, "'|'     - set vertical highlights");

      /* hold it right there */
      refresh();
      next_char();
      break;
    case 'Q':
    case 'q':
      /* quit and be done with it */
      done = TRUE;
      break;
    case 'D':
    case 'd':
      /* get a new display setting */
      count = get_hd_info(FALSE);

      /* check the input */
      if (no_input == TRUE) break;
      if (count == DI_MAXIMUM + 1) {
	errormsg("That is not a possible display option");
	break;
      }
      display_mode.style[display_mode.focus] = count;
      break;
    case '=':
      hl_all();
      break;
    case '/':
      hl_urll();
      break;
    case '\\':
      hl_ullr();
      break;
    case '-':
      hl_horz();
      break;
    case '|':
      hl_vert();
      break;
    case 'H':
    case 'h':
    case ',':
      hl_current();
      break;
    case 'R':
    case 'r':
      /* reset to the original */
      if (mainone) {
	change_dmode(&hold_display, TRUE);
      } else {
	change_dmode(dtmp_ptr, TRUE);
      }
      break;
    case 'N':
    case 'n':
      /* name change */
      mvaddstr(LINES - 1, 0, "Change name of mode to what? ");
      get_string(buffer, STR_SPACE, DISPLAYLTH);

      /* check it */
      if (no_input == TRUE) break;
      if (dmode_byname(buffer) != NULL) {
	errormsg("A mode by that name already exists");
	break;
      }
      strcpy(display_mode.name, buffer);
      break;
    default:
      /* ack */
      errormsg("Don't know what you mean by that.");
      break;
    }
  }

  /* only perform storage if needed */
  if (!abandon) {

    /* reassign original display */
    if (mainone) {
      bottommsg("Do you wish to store this setting under a new mode? ");
      clrtoeol();
      if (y_or_n()) {
	done = FALSE;
	strcpy(buffer, display_mode.name);
	while (done == FALSE) {
	  if (dmode_byname(buffer) != NULL) {
	    bottommsg("Name already exists; Give it what new name? ");
	    clrtoeol();

	    /* check it */
	    get_string(buffer, STR_SPACE, DISPLAYLTH);
	    if (no_input == TRUE) break;
	  } else {
	    done = TRUE;
	  }
	}
	if (done == TRUE) {
	  if ((work_mode = crt_dmode(buffer)) != NULL) {

	    /* copy it */
	    work_mode->d.focus = display_mode.focus;
	    for (count = 0; count < HXPOS_NUMBER; count++) {
	      work_mode->d.style[count] = display_mode.style[count];
	      work_mode->d.highlight[count] = display_mode.highlight[count];
	      work_mode->d.target[count] = display_mode.target[count];
	    }

	  }
	}
      }

    } else {

      /* copy everything into the mode memory */
      strcpy(work_mode->d.name, display_mode.name);
      work_mode->d.focus = display_mode.focus;
      for (count = 0; count < HXPOS_NUMBER; count++) {
	work_mode->d.highlight[count] = display_mode.highlight[count];
	work_mode->d.style[count] = display_mode.style[count];
	work_mode->d.target[count] = display_mode.target[count];
      }

      /* go back to the original */
      change_dmode(&hold_display, TRUE);

    }

  } else {
    /* change back to the original */
    change_dmode(&hold_display, TRUE);
  }

  /* skip town */
  return(0);
}

/* ADJ_DISPLAY -- Select a different highlight or display mode */
int
adj_display PARM_0(void)
{
  /* display mode switch */
  dmode_tptr = get_dmode("Use which display mode? ");

  /* check the input */
  if (no_input == TRUE) return(0);
  if (dmode_tptr == NULL) {
    errormsg("That is not a currently defined display mode");
    return(0);
  }

  /* now switch it */
  change_dmode(&(dmode_tptr->d), FALSE);
  return(0);
}

/* GET_HD_INFO -- Get a highlight or display selection */
int
get_hd_info PARM_1(int, hilitep)
{
  int xloc, yloc = LINES - 5, in_ch;
  char **hd_list;
  int temp, max_val;

  /* initialize */
  no_input = FALSE;
  clear_bottom(0);

  /* select highlight or display mode */
  if (hilitep) {
    /* highlight settings */
    mvaddstr( yloc, 0, "Highlight Options: " );
    max_val = HI_MAXIMUM;
    xloc = 19;
    hd_list = highl_list;
  } else {
    /* display settings */
    mvaddstr( yloc, 0, "Display Options: " );
    max_val = DI_MAXIMUM;
    xloc = 17;
    hd_list = display_list;
  }

  /* display all of the possible selections */
  for (temp = 0; temp <= max_val; temp++ ) {
    if (temp > 0) {
      addstr(", ");
      xloc += 2;
    }
    if (xloc + strlen( hd_list[temp] ) > COLS - 7) {
      xloc = 2;
      yloc++;
      move( yloc, xloc );
    }
    hip_string(hd_list[temp], 1, TRUE);
    xloc += strlen(hd_list[temp]) + 2;
  }

  /* display the query */
  mvprintw(++yloc, 0, "Set the %s to what? ",
	   hilitep ? "highlight" : "display selection");
  refresh();

  /* parse the input */
  in_ch = next_char();
  if (in_ch == ' ' || in_ch == '\n' || in_ch == '\r') {
    no_input = TRUE;
    return(max_val + 1);
  }

  /* search for match */
  for (temp = 0; temp <= max_val; temp++) {
    if (in_ch == hd_list[temp][0]) {
      break;
    }
  }
  return(temp);
}

/* RECENTER_SCREEN -- Recenter the current location into middle of screen */
int
recenter_screen PARM_0(void)
{
  /* just a quick call to centermap */
  centermap();
  return(0);
}

/* MAKEBOTTOM -- Place standard display at bottom of screen */
void
makebottom PARM_0(void)
{
  int y;

  /* clean up the bottom area */
  clear_bottom(0);

  /* show the quiting commands and turn status */
  roman_number( string, TURN - START_TURN + 1);
  mvprintw(LINES - 4, 0, "Conquer: %s.%d Turn %s", VERSION,
	   PATCHLEVEL, string);
  mvaddstr(LINES - 2, 0, "  Type 'Q' to end turn");
  mvaddstr(LINES - 3, 0, "  Type 'q' to save & exit");
  mvaddstr(LINES - 1, 0, "  Type '?' for help");

  /* show the id and treasury */
  y = LINES - 4;
  if(country == 0) {
    mvaddstr(y++, COLS - 20, "nation...GOD");
  } else {
    mvprintw(y++, COLS - 20, "nation...%s", ntn_ptr->name);
    mvprintw(y++, COLS - 20, "treasury.%.0f",
	     (double)ntn_ptr->mtrls[MTRLS_TALONS]);
  }
  mvprintw(y++, COLS - 20, "%s (%s)", PMONTH(TURN), PSEASON(TURN) );
  roman_number( string, YEAR(TURN) );
  mvprintw( y,  COLS - 20, "Year %s", string );
  
  /* mail statuses */
  y = LINES - 3;
  if (conq_mail_status == STMAIL_NEW) {
    mvaddstr(y, COLS / 2 - 7, "You have Conquer Mail");
  }
  y++;
  if (conq_news_status == STMAIL_NEW) {
    mvaddstr(y, COLS / 2 - 7, "There is Conquer News");
  }
#ifdef SYSMAIL
  y++;
  /* display mail information */
  if (sys_mail_status == STMAIL_NEW) {
    mvaddstr(y, COLS / 2 - 7, "You have System Mail");
  }
#endif /*SYSMAIL*/

}

/* MSIDE_CHECK -- Set the value if within vision range */
static void
mside_check PARM_2(int, x, int, y)
{
  /* check it out */
  if (VIS_CHECK(x, y) != HS_NOSEE) {
    global_int = TRUE;
  }
}

/* A_SIDESHOW -- Display full information about a given unit */
static void
a_sideshow PARM_2(ARMY_PTR, a1_ptr, int, fval)
{
  char sidestr[LINELTH];
  ARMY_PTR ahold_ptr = army_ptr;

  /* use the quicky pointer */
  army_ptr = a1_ptr;

  /* highlight the selected unit */
  if  (selector == (fval % SCREEN_SIDE) * 2) {
    if (is_god == TRUE) {
      mvaddch((fval % SCREEN_SIDE) * 2,
	      COLS - (SCREEN_RIGHT + 1), ntn_ptr->mark);
    } else if (unit_leading(ARMY_STAT)) {
      /* leaders are loaded! */
      mvaddch((fval % SCREEN_SIDE) * 2,
	      COLS - (SCREEN_RIGHT + 1), '+');
    } else {
      mvaddch((fval % SCREEN_SIDE) * 2,
	      COLS - (SCREEN_RIGHT + 1), '*');
    }
    standout();
  } else if ((pager == scnd_pager) &&
	     (scnd_selector == (fval % SCREEN_SIDE) * 2)) {
    mvaddch((fval % SCREEN_SIDE) * 2,
	    COLS - (SCREEN_RIGHT + 1), '#');
  } else {
    mvaddch((fval % SCREEN_SIDE) * 2,
	    COLS - (SCREEN_RIGHT + 1), '>');
  }

  /* display unit name, size and id */
  if (a_isleader(ARMY_TYPE)) {
    sprintf(string, "%s %d x=%ld p=%d",
	    ainfo_list[ARMY_TYPE].sname,
	    (int) ARMY_ID, ARMY_SIZE, (int) ARMY_SPTS );
  } else if (a_castspells(ARMY_TYPE)) {
    sprintf(string, "%d: %ld %s p=%d",
	    (int) ARMY_ID, ARMY_SIZE, ainfo_list[ARMY_TYPE].sname,
	    (int) ARMY_SPTS );
  } else {
    sprintf(string, "army %d: %ld %s",
	    (int) ARMY_ID, ARMY_SIZE, ainfo_list[ARMY_TYPE].sname );
  }
  sprintf(sidestr, "%.*s", SCREEN_RIGHT, string);
  addstr(sidestr);

  /* display second line of information */
  if (unit_status(ARMY_STAT) == ST_GROUPED) {

    if ((MAXSUPPLIES > 0) &&
	!a_freesupply(ARMY_TYPE)) {
      if (ARMY_SPLY < 10) {
	sprintf(sidestr, " %d", ARMY_SPLY);
      } else {
	strcpy(sidestr, " +");
      }
    } else {
      sidestr[0] = '\0';
    }
    sprintf(string, "%s member group %d", sidestr, ARMY_LEAD );
    if (unit_spelled(ARMY_STAT)) strcat(string, "!");

  } else if ((MAXSUPPLIES > 0) &&
	     !a_freesupply(ARMY_TYPE)) {

    if (nomove_stat(ARMY_STAT)) {
      if (wall_patrol(country, army_ptr)) {
	strcpy(string, *(shortspeed + SPD_PATROL));
      } else {
	strcpy(string, *(shortspeed + SPD_STUCK));
      }
    } else {
      strcpy(string, *(shortspeed + unit_speed(ARMY_STAT)) );
    }
    if (nomove_stat(ARMY_STAT)) {
      sprintf(sidestr, " m:0(%s) st:%s",
	      string, stat_info[unit_status(ARMY_STAT)].nickname );
    } else if (unit_flight(ARMY_STAT)) {
      sprintf(sidestr, " m:%d[F%s] st:%s", ARMY_MOVE,
	      string, stat_info[unit_status(ARMY_STAT)].nickname );
    } else {
      sprintf(sidestr, " m:%d(%s) st:%s", ARMY_MOVE,
	      string, stat_info[unit_status(ARMY_STAT)].nickname );
    }
    if (unit_spelled(ARMY_STAT)) strcat(sidestr, "!");
    if (ARMY_SPLY < 10) {
      sprintf(string, " %d", ARMY_SPLY);
    } else {
      strcpy(string, " +");
    }
    strcat(string, sidestr);

  } else {

    if (nomove_stat(ARMY_STAT)) {
      if (wall_patrol(country, army_ptr)) {
	strcpy(sidestr, *(shortspeed + SPD_PATROL));
      } else {
	strcpy(sidestr, *(shortspeed + SPD_STUCK));
      }
    } else if (unit_flight(ARMY_STAT)) {
      sprintf(sidestr, "F%s",
	      *(shortspeed + unit_speed(ARMY_STAT)) );
    } else {
      strcpy(sidestr, *(shortspeed + unit_speed(ARMY_STAT)) );
    }
    if (nomove_stat(ARMY_STAT)) {
      sprintf(string, " m:0 sp:%s st:%s", sidestr,
	      stat_info[unit_status(ARMY_STAT)].nickname );
    } else {
      sprintf(string, " m:%d sp:%s st:%s", ARMY_MOVE, sidestr,
	      stat_info[unit_status(ARMY_STAT)].nickname );
    }
    if (unit_spelled(ARMY_STAT)) strcat(string, "!");

  }
  sprintf(sidestr, "%.*s", SCREEN_RIGHT, string);
  mvaddstr((fval % SCREEN_SIDE) * 2 + 1,
	   COLS - SCREEN_RIGHT, sidestr);

  /* clean up highlighting */
  if (selector == (fval % SCREEN_SIDE) * 2) standend();

  /* restore setting */
  army_ptr = ahold_ptr;
}

/* N_SIDESHOW -- show the full naval unit information along the side */
static void
n_sideshow PARM_2(NAVY_PTR, n1_ptr, int, fval)
{
  char sidestr[LINELTH];
  NAVY_PTR nhold_ptr = navy_ptr;
  int i, j;

  /* set up information */
  navy_ptr = n1_ptr;

  /* highlight the selected unit */
  if (selector == (fval % SCREEN_SIDE) * 2) {

    /* check if it is loaded */
    if (is_god == TRUE) {
      mvaddch((fval % SCREEN_SIDE) * 2,
	      COLS - (SCREEN_RIGHT + 1), ntn_ptr->mark);
    } else if (navy_carrying(navy_ptr)) {
      mvaddch( (fval % SCREEN_SIDE) * 2,
	      COLS - (SCREEN_RIGHT + 1), '+');
    } else {
      mvaddch( (fval % SCREEN_SIDE) * 2,
	      COLS - (SCREEN_RIGHT + 1), '*');
    }
    standout();

  } else if ((pager == scnd_pager) &&
	     (scnd_selector == (fval % SCREEN_SIDE) * 2)) {
    mvaddch((fval % SCREEN_SIDE) * 2,
	    COLS - (SCREEN_RIGHT + 1), '#');
  } else {
    mvaddch((fval % SCREEN_SIDE) * 2,
	    COLS - (SCREEN_RIGHT + 1), '>');
  }

  /* display fleet information */
  sprintf(string, "navy %d",
	  NAVY_ID);
  for (i = 0; i < NSHP_NUMBER; i++) {
    j = (int) N_CNTSHIPS(NAVY_SHIPS[i], N_LIGHT) +
        (int) N_CNTSHIPS(NAVY_SHIPS[i], N_MEDIUM) +
	(int) N_CNTSHIPS(NAVY_SHIPS[i], N_HEAVY);
    sprintf(sidestr, " %s%c",
	    ninfo_list[i].sname,
	    (j > 9)?'+':('0' + j));
    strcat(string, sidestr);
  }
  sprintf(sidestr, "%.*s", SCREEN_RIGHT, string);
  addstr(sidestr);
  strcpy(string, *(shortspeed + unit_speed(NAVY_STAT)) );
  if (MAXSUPPLIES > 0) {
    if (nomove_stat(NAVY_STAT)) {
      sprintf(sidestr, " mv:0(-) st:%s",
	      stat_info[unit_status(NAVY_STAT)].nickname );
    } else {
      sprintf(sidestr, " mv:%d(%s) st:%s", NAVY_MOVE,
	      string, stat_info[unit_status(NAVY_STAT)].nickname );
    }
    if (unit_spelled(NAVY_STAT)) strcat(sidestr, "!");
    if (NAVY_SPLY < 10) {
      sprintf(string, " %d", NAVY_SPLY);
    } else {
      strcpy(string, " +");
    }
    strcat(string, sidestr);
  } else {
    if (nomove_stat(NAVY_STAT)) {
      sprintf(sidestr, " mv:0 sp:- st:%s",
	      stat_info[unit_status(NAVY_STAT)].nickname );
    } else {
      sprintf(sidestr, " mv:%d sp:%s st:%s", NAVY_MOVE,
	      string, stat_info[unit_status(NAVY_STAT)].nickname );
    }
    if (unit_spelled(NAVY_STAT)) strcat(sidestr, "!");
    strcpy(string, sidestr);
  }
  sprintf(sidestr, "%.*s", SCREEN_RIGHT, string);
  mvaddstr((fval % SCREEN_SIDE) * 2 + 1,
	   COLS - SCREEN_RIGHT, sidestr);

  /* cleanup */
  if (selector == (fval % SCREEN_SIDE) * 2) standend();

  /* restore information */
  navy_ptr = nhold_ptr;
}

/* V_SIDESHOW -- show the full caravan information at the side */
static void
v_sideshow PARM_2 (CVN_PTR, v1_ptr, int, fval)
{
  char sidestr[LINELTH];
  CVN_PTR vhold_ptr = cvn_ptr;

  /* set up the information */
  cvn_ptr = v1_ptr;

  /* highlight the selected unit */
  if (selector == (fval % SCREEN_SIDE) * 2) {

    /* check if it is loaded */
    if (is_god == TRUE) {
      mvaddch((fval % SCREEN_SIDE) * 2,
	      COLS - (SCREEN_RIGHT + 1), ntn_ptr->mark);
    } else if (cvn_carrying(cvn_ptr)) {
      mvaddch( (fval % SCREEN_SIDE) * 2,
	      COLS - (SCREEN_RIGHT + 1), '+');
    } else {
      mvaddch( (fval % SCREEN_SIDE) * 2,
	      COLS - (SCREEN_RIGHT + 1), '*');
    }
    standout();

  } else if ((pager == scnd_pager) &&
	     (scnd_selector == (fval % SCREEN_SIDE) * 2)) {
    mvaddch((fval % SCREEN_SIDE) * 2,
	    COLS - (SCREEN_RIGHT + 1), '#');
  } else {
    mvaddch((fval % SCREEN_SIDE) * 2,
	    COLS - (SCREEN_RIGHT + 1), '>');
  }

  /* display caravan information */
  sprintf(string, "cvn %d cw:%d wg:%d", CVN_ID,
	  (int) CVN_CREW, (int) CVN_SIZE * WAGONS_IN_CVN);
  sprintf(sidestr, "%.*s", SCREEN_RIGHT, string);
  addstr(sidestr);
  strcpy(string, *(shortspeed + unit_speed(CVN_STAT)) );
  if (MAXSUPPLIES > 0) {
    if (nomove_stat(CVN_STAT)) {
      sprintf(sidestr, " m:0(-) st:%s",
	      stat_info[unit_status(CVN_STAT)].nickname );
    } else {
      sprintf(sidestr, " m:%d(%s) st:%s", CVN_MOVE,
	      string, stat_info[unit_status(CVN_STAT)].nickname );
    }
  } else {
    if (nomove_stat(CVN_STAT)) {
      sprintf(sidestr, " m:0 sp:- st:%s",
	      stat_info[unit_status(CVN_STAT)].nickname );
    } else {
      sprintf(sidestr, " m:%d sp:%s st:%s", CVN_MOVE,
	      string, stat_info[unit_status(CVN_STAT)].nickname );
    }
  }
  if (unit_spelled(CVN_STAT)) strcat(sidestr, "!");
  if (MAXSUPPLIES > 0) {
    if (CVN_SPLY < 10) {
      sprintf(string, " %d", CVN_SPLY);
    } else {
      strcpy(string, " +");
    }
    strcat(string, sidestr);
  } else {
    strcpy(string, sidestr);
  }
  sprintf(sidestr, "%.*s", SCREEN_RIGHT, string);
  mvaddstr((fval % SCREEN_SIDE) * 2 + 1,
	   COLS - SCREEN_RIGHT, sidestr);

  /* cleanup */
  if (selector == (fval % SCREEN_SIDE) * 2) standend();

  /* reset the information */
  cvn_ptr = vhold_ptr;
}

/* S_SIDESHOW -- show sector description */
static void
s_sideshow PARM_1(int, move_ind)
{
  int sown, value;

  /* set up information */
  sct_ptr = &sct[XREAL][YREAL];
  sown = sct_ptr->owner;
  ntn_tptr = world.np[sown];
  if (ntn_tptr == NULL) {
    sown = UNOWNED;
  }

  /* show the designation; if appropriate */
  if ((is_god == TRUE) ||
      (country == sown) ||
      (sown == UNOWNED) ||
      WIZ_MAGIC(MW_SEEALL) ||
      !MAGIC(ntn_tptr->powers[MAG_WIZARDRY], MW_THEVOID)) {

    if (((value = major_desg(sct_ptr->designation)) != MAJ_NONE) &&
	((value != MAJ_CACHE) ||
	 (country == sown) ||
	 (country == UNOWNED))) {
      mvaddstr(LINES - (conq_infomode ? 9 : 11),
	       (conq_infomode ? (19 + (2 * (COLS - 80)) / 3) :
		(COLS - SCREEN_RIGHT)),
	       maj_dinfo[value].name);
    }

  }

  /* calculate the value for the move cost */
  switch (move_ind) {
  case MOVEIND_NORM:
    /* use land values */
    value = move_cost(XREAL, YREAL, MOVE_ARMY);
    break;
  case MOVEIND_FLY:
    /* use air values */
    value = move_cost(XREAL, YREAL, MOVE_FLYARMY);
    break;
  case MOVEIND_WATER:
    /* use navy values */
    value = move_cost(XREAL, YREAL, MOVE_NAVY);
    break;
  default:
    /* uh oh */
    value = MV_OFFMAP;
  }

  /* build movment cost */
  move_stval(string, value, FALSE);

  /* check for a water sector */
  if ((sct_ptr->altitude == ELE_WATER) &&
      (is_god == FALSE)) {

    /* show only a bit of information */
    if (conq_infomode) {
      mvaddstr(LINES - 9, 10 + (COLS - 80) / 3,
	       ele_info[sct_ptr->altitude].name );
      mvaddstr(LINES - 8,  10 + (COLS - 80) / 3, string);
    } else {
      mvaddstr(LINES - 10, COLS - SCREEN_RIGHT,
	       ele_info[sct_ptr->altitude].name );
      mvaddstr(LINES - 9, COLS - SCREEN_RIGHT, string);
    }

  } else {

    /* show movement cost */
    if (conq_infomode) {
      mvaddstr(LINES - 7, 19 + (2 * (COLS - 80)) / 3, string);
    } else {
      mvaddstr(LINES - 9, COLS - 9, string);
    }

    /* display owner and race; if any */
    if (sown != UNOWNED) {
      mvprintw(LINES - (conq_infomode ? 10 : 12),
	       (conq_infomode ? 0 : (COLS - SCREEN_RIGHT)),
	       "owner: %s", ntn_tptr->name );
      mvaddstr(LINES - (conq_infomode ? 8 : 11),
	       (conq_infomode ? (19 + (2 * (COLS - 80)) / 3) :
		(COLS - 9)),
	       race_info[ntn_tptr->race].name );
    }

    /* show vegetation and altitude */
    if (sct_ptr->vegetation != VEG_NONE) {
      mvaddstr(LINES - (conq_infomode ? 9 : 10),
	       (conq_infomode ? 0 : (COLS - SCREEN_RIGHT)),
	       veg_info[sct_ptr->vegetation].name );
    }
    mvaddstr(LINES - (conq_infomode ? 9 : 10),
	     (conq_infomode ? (10 + (COLS - 80) / 3) :
	      (COLS - 9)),
	     ele_info[sct_ptr->altitude].name );

    /* now check how much is visible */
    if ((is_god == TRUE) || (sown == UNOWNED) ||
	(country == sown) || WIZ_MAGIC(MW_SEEALL) ||
	!MAGIC(ntn_tptr->powers[MAG_WIZARDRY], MW_THEVOID)) {
      if (conq_infomode) {
	mvprintw(LINES - 8, 0, "people: %ld",
		 distort_vision(XREAL, YREAL,
				sct_ptr->people,
				sct_ptr->people));
      } else {
	mvprintw(LINES - 9, COLS - SCREEN_RIGHT, "pop:%6ld",
		 distort_vision(XREAL, YREAL,
				sct_ptr->people,
				sct_ptr->people));
      }
    } else {
      if (conq_infomode) {
	mvprintw(LINES - 8, 0, "people: ?");
      } else {
	mvprintw(LINES - 9, COLS - SCREEN_RIGHT, "pop:     ?");
      }
    }
    if (conq_infomode) {
      mvprintw(LINES - 7, 0, "food: %d",
	       distort_vision(XREAL, YREAL,
			      tofood(sct_ptr, country),
			      sct_ptr->people));
      mvprintw(LINES - 7, 10 + (COLS - 80) / 3, "wood: %d",
	       distort_vision(XREAL, YREAL, towood(sct_ptr, country),
			      sct_ptr->people));
    } else {
      mvprintw(LINES - 8, COLS - SCREEN_RIGHT, "food:%5d",
	       distort_vision(XREAL, YREAL,
			      tofood(sct_ptr, country),
			      sct_ptr->people));
      mvprintw(LINES - 8, COLS - 9, "wood:%3d",
	       distort_vision(XREAL, YREAL, towood(sct_ptr, country),
			      sct_ptr->people));
    }

    /* tradegood dependent items */
    if ((is_god == TRUE) || (sown == UNOWNED) ||
	(country == sown) || WIZ_MAGIC(MW_SEEALL)) {

      if (sct_ptr->tradegood != TG_NONE
	  && tg_ok(sct_ptr, country)) {

	/* show the jewels, metals, or magic */
	if (jewel_value(sct_ptr)) {
	  mvprintw(LINES - 6,
		   (conq_infomode ? (19 + (2 * (COLS - 80)) / 3) :
		    (COLS - SCREEN_RIGHT)),
		   "jewel: %*d", (conq_infomode ? 0 : 3),
		   distort_vision(XREAL, YREAL,
				  jewel_value(sct_ptr),
				  sct_ptr->people));
	} else if (metal_value(sct_ptr)) {
	  mvprintw(LINES - 6,
		   (conq_infomode ? (19 + (2 * (COLS - 80)) / 3) :
		    (COLS - SCREEN_RIGHT)),
		   "metal: %*d", (conq_infomode ? 0 : 3),
		   distort_vision(XREAL, YREAL,
				  metal_value(sct_ptr),
				  sct_ptr->people));
	} else if (magic_value(sct_ptr)) {
	  mvprintw(LINES - 6,
		   (conq_infomode ? (19 + (2 * (COLS - 80)) / 3) :
		    (COLS - SCREEN_RIGHT)),
		   "magic: %*d", (conq_infomode ? 0 : 3),
		   distort_vision(XREAL, YREAL,
				  magic_value(sct_ptr),
				  sct_ptr->people));
	}

	/* show the tradegood */
	standout();
	mvprintw(LINES - (conq_infomode ? 6 : 7),
		 (conq_infomode ? 0 : (COLS - SCREEN_RIGHT)),
		 "item: %.*s", (conq_infomode ? 8 : 9),
		 tg_info[sct_ptr->tradegood].name );
	if (tg_info[sct_ptr->tradegood].need_desg == MAJ_NONE)
	  addstr(" ANY");
	else printw(" (%c)",
		    maj_dinfo[tg_info[sct_ptr->tradegood].need_desg].symbol);
	standend();

      } else {
	mvaddstr(LINES - (conq_infomode ? 6 : 7),
		 (conq_infomode ? 0 : (COLS - SCREEN_RIGHT)),
		 "item: none");
      }

    }
  }
}

/* MAKESIDE -- Show information about sector along side of screen;
               alwayssee indicates that all info should be shown */
void
makeside PARM_2 (int, alwayssee, int, moveind)
{
  int count, acount, ncount, vcount, start;
  int found, found2, y, c_start, c_end;
  int old_cntry = country;
  NTN_PTR old_nptr = ntn_ptr;

  /* clear right hand side */
  for (y = 0; y < LINES - 5; y++) {
    move(y, COLS - (SCREEN_RIGHT + 1));
    clrtoeol();
  }

  /* determine if the sector is visible */
  global_int = FALSE;
  if (!world.relative_map || alwayssee ||
      (Y_ONMAP(YREAL) && VIS_CHECK(XREAL, YREAL))) {
    global_int = TRUE;
  } else if (Y_ONMAP(YREAL)) {
    map_loop(XREAL, YREAL, VISRANGE, mside_check);
  }
  if (conq_infomode) {
    /* display underneath the small map */
    y = LINES - 11;
    count = 10 + (COLS - 80) / 3;
  } else {
    /* display it at start of short summary */
    y = LINES - 13;
    count = COLS - SCREEN_RIGHT;
  }

  /* show it */
  if (global_int == TRUE) {
    mvprintw(y, count, "sector [%d,%d]",
	     xloc_relative(XREAL), yloc_relative(YREAL));
  } else {
    mvaddstr(y, count, "sector [??,??]");
  }

  /* exit if sector is not visible */
  if (!Y_ONMAP(YREAL) ||
      (!alwayssee &&
       (VIS_CHECK(XREAL, YREAL) == HS_NOSEE)) ) {
    return;
  }

  /* are we in god mode */
  if (country == UNOWNED) {
    c_start = 1;
    c_end = MAXNTN;
  } else {
    c_start = country;
    c_end = country + 1;
  }

  /* check for all units */
  if (VIS_CHECK(XREAL, YREAL) > HS_SEEPART) {
    acount = armies_in_sector(country, XREAL, YREAL);
    ncount = navies_in_sector(country, XREAL, YREAL);
    vcount = cvns_in_sector(country, XREAL, YREAL);
    count = acount + ncount + vcount;
    if (pager * SCREEN_SIDE + selector / 2 > count) {
      pager = 0;
      selector = 0;
    } else if (selector / 2 >= SCREEN_SIDE) {
      selector = 0;
    }

    /* initialize values */
    start = pager * SCREEN_SIDE;
    y = 0;

    /* indicate that more troops are viewable */
    if (count > start + SCREEN_SIDE) {
      mvaddstr( SCREEN_SLIM - (SCREEN_SLIM % 2), COLS - 19,
	       "--- more ---");
    }

    /* determine if armies are going to be shown */
    if (acount > start) {

      /* begin search for troops */
      found = 0;

      /* do the loop */
      for (country = c_start; country < c_end; country++) {

	/* check it */
	if ((ntn_ptr = world.np[country]) == NULL) continue;

	/* go and find the beginning army unit */
	for (army_tptr = ntn_ptr->army_list;
	     army_tptr != NULL;
	     army_tptr = army_tptr->next) {
	  if ((ARMYT_XLOC == XREAL) &&
	      (ARMYT_YLOC == YREAL)) {
	    break;
	  }
	}

	/* go through all armies in the sector */
	for (; army_tptr != NULL; army_tptr = army_tptr->nrby) {

	  /* check for units to display */
	  if (found >= start && found < start + SCREEN_SIDE) {
	    /* display the unit */
	    a_sideshow(army_tptr, found);
	  }
	  if (++found > start + SCREEN_SIDE) {
	    goto side_units_done;
	  }

	}

      }

    } else {
      /* keep track of all of the armies found */
      found = acount;
    }

    /* check if navies need displaying */
    if (ncount + found > start && found < start + SCREEN_SIDE) {

      /* do the search loop */
      for (country = c_start; country < c_end; country++) {

	/* check it */
	if ((ntn_ptr = world.np[country]) == NULL) continue;

	/* go through all of the navy units */
	for (navy_tptr = ntn_ptr->navy_list;
	     navy_tptr != NULL;
	     navy_tptr = navy_tptr->next) {

	  /* check for units in the sector */
	  if (NAVYT_XLOC == XREAL && NAVYT_YLOC == YREAL) {

	    /* display it if it is in view */
	    if (found >= start && found < start + SCREEN_SIDE) {
	      n_sideshow(navy_tptr, found);
	    }
	    if (++found > start + SCREEN_SIDE) {
	      goto side_units_done;
	    }

	  }

	}
      }

    } else {
      /* account for the naval units in the sector */
      found += ncount;
    }

    /* check if caravans need displaying */
    if (vcount + found > start && found < start + SCREEN_SIDE) {

      /* do the loop */
      for (country = c_start; country < c_end; country++) {

	/* check it */
	if ((ntn_ptr = world.np[country]) == NULL) continue;

	/* travel through all of the caravans */
	for (cvn_tptr = ntn_ptr->cvn_list;
	     cvn_tptr != NULL;
	     cvn_tptr = cvn_tptr->next) {

	  /* count it in the location */
	  if (CVNT_XLOC == XREAL && CVNT_YLOC == YREAL) {

	    /* display it if need be */
	    if (found >= start && found < start + SCREEN_SIDE) {
	      v_sideshow(cvn_tptr, found);
	    }
	    if (++found > start + SCREEN_SIDE) {
	      goto side_units_done;
	    }

	  }

	}

      }

    } else {
      /* now add in the caravans */
      found += vcount;
    }

    /* assure that everything is back on track */
    country = old_cntry;
    ntn_ptr = old_nptr;

    /* display other nation's troops */
    if ((is_god == FALSE) &&
	(VIS_CHECK(XREAL, YREAL) >= HS_SEEMOST) &&
	(found <= start + SCREEN_SIDE)) {

      /* show beginning of enemy list */
      found2 = side_troops((found - start) * 2, SCREEN_SIDE * 2, FALSE);

      /* indicate that more things were found */
      if (found2 > SCREEN_SIDE * 2) {
	mvaddstr(SCREEN_SIDE * 2, COLS - 19, "--- and others ---");
      }

    } else {
      found2 = (found - start) * 2;
    }

    /* now check for appending of "more" indicator */
    if ((start != 0) &&
	(found <= start + SCREEN_SIDE) &&
	(found2 <= SCREEN_SIDE * 2)) {
      mvaddstr(found2, COLS - 19, "--- more ---");
    }

  }

 side_units_done:
  /* assure that everything is back on track */
  country = old_cntry;
  ntn_ptr = old_nptr;

  /* now show the sector information */
  s_sideshow(moveind);
  if (conq_infomode) {
    sect_info();
  }
}

/* CHANGE_VIEW -- Adjust the value of a display character interactively */
int
change_view PARM_0(void)
{
  int choice, i, x, y, in_ch;
  char tmpstr[20];

  /* ask which type to adjust */
  clear_bottom(0);
  strcpy (tmpstr, "");
  mvaddstr( LINES - 3, 0, "Character Settings: ");
  x = 20;
  y = LINES - 3;
  for (i = 0; i < CV_NUMBER; i++) {
    if (i > 0) {
      addstr(", ");
      x += 2;
    }
    if (x + strlen( cv_name[i] ) > COLS - 10) {
      x = 5;
      y++;
      move(y, x);
    }
    hip_string(cv_name[i], 1, TRUE);
    x += strlen( cv_name[i] ) + 2;
  }

  /* read in and parse */
  mvaddstr(++y, 0, "Alter which character set? ");
  refresh();
  if ((in_ch = next_char()) == ' ' || in_ch == '\n' || in_ch == '\r') {
    /* provide quick exit */
    return(0);
  }

  /* compare with upper case only */
  if (islower(in_ch)) in_ch = toupper(in_ch);
  for (choice = 0; choice < CV_NUMBER; choice++) {
    if (in_ch == cv_name[choice][0]) break;
  }

  /* verify input */
  if (choice == CV_NUMBER) {
    errormsg("Hey, That isn't among your choices!");
    return(0);
  }

  /* initialize display list */
  in_ch = '~';
  clear_bottom(0);
  sprintf( string, "%s Character List: ", cv_name[choice] );
  mvaddstr( LINES - 4, 0, string );
  x = strlen( string );
  y = LINES - 4;

  /* now provide with the list of characters to change */ 
  for (i = 0; i < cv_max[choice]; i++) {
    /* assign the next value */
    switch (choice) {
    case CV_CONTOUR:
      in_ch = ele_info[i].symbol;
      strcpy( tmpstr, ele_info[i].name );
      break;
    case CV_DESIGNATION:
      in_ch = maj_dinfo[i].symbol;
      strcpy( tmpstr, maj_dinfo[i].name );
      break;
    case CV_VEGETATION:
      in_ch = veg_info[i].symbol;
      strcpy( tmpstr, veg_info[i].name );
      break;
    default:
      errormsg("Uh oh... something is screwy here!");
      return(0);
    }

    /* check for wrap */
    if (i > 0) {
      addch(',');
      x++;
    }
    sprintf( string, " %s [%c]", tmpstr, in_ch );
    if (x + strlen(string) > COLS - 5) {
      x = 2;
      y++;
    }
    mvaddstr( y, x, string );
    x += strlen( string );
  }

  /* ask for character to adjust */
  mvaddstr( LINES - 5, 0, "Adjust display character for which? ");
  refresh();
  switch (choice) {
  case CV_CONTOUR:
    /* elevation input */
    x = get_altitude();
    break;
  case CV_DESIGNATION:
    /* desiganation input */
    x = get_designation();
    break;
  case CV_VEGETATION:
    /* vegetation input */
    x = get_vegetation();
    break;
  }
  if (no_input == TRUE) {
    return(0);
  }
  if (x >= cv_max[choice]) {
    errormsg("Huh?  That isn't one of the possible selections");
    return(0);
  }

  /* now find out what it should become */
  switch(choice) {
  case CV_CONTOUR:
    strcpy( tmpstr, ele_info[x].name );
    break;
  case CV_DESIGNATION:
    strcpy( tmpstr, maj_dinfo[x].name );
    break;
  case CV_VEGETATION:
    strcpy( tmpstr, veg_info[x].name );
    break;
  }
  mvprintw(LINES - 5, 0, "Use what character for %s? ", tmpstr );
  clrtoeol();
  refresh();

  /* provide quick exit */
  if ((in_ch = next_char()) == '\n' || in_ch == '\r') {
    return(0);
  }

  /* validate character */
  if ((isdigit(in_ch) ||
       !isprint(in_ch) ||
       (in_ch == ' ')) &&
      ((in_ch != ' ') ||
       !(((choice == CV_CONTOUR) &&
	  (x == ELE_WATER)) ||
	 ((choice == CV_VEGETATION) &&
	  (x == VEG_NONE))))) {
    errormsg("That character is not valid for display purposes");
    return(0);
  }

  /* now assign it */
  switch (choice) {
  case CV_CONTOUR:
    ele_info[x].symbol = in_ch;
    break;
  case CV_DESIGNATION:
    maj_dinfo[x].symbol = in_ch;
    break;
  case CV_VEGETATION:
    veg_info[x].symbol = in_ch;
    break;
  default:
    break;
  }

  /* the whole screen should be adjusted now */
  return(0);
}

/* TROOP_LISTING -- List all of the troops within the sector */
int
troop_listing PARM_0(void)
{
  int i;

  /* first check if it is allowed */
  if (!XY_ONMAP(XREAL, YREAL) ||
      ((sct[XREAL][YREAL].owner != country) &&
       (VIS_CHECK(XREAL, YREAL) < HS_SEEMOST))) {
    errormsg("You cannot get any detail on troops within this sector");
    return(0);
  }

  /* traverse the list of troops */
  for (i = 0; i <= LINES - 1; i++) {
    move(i, COLS - (SCREEN_RIGHT + 1));
    clrtoeol();
  }
  if (side_troops(0, LINES - 2, TRUE) == 0) {
    errormsg("There are no troops not owned by you in the sector");
  }

  /* all done */
  return(0);
}

/* SIDE_TROOPS -- Display additional troops along edge */
int
side_troops PARM_3(int, start_pos, int, max_pos, int, show_all)
{
  char sidestr[LINELTH];
  long enemy;
  int found2, i, j, k, asked = FALSE;

  /* go through all nations */
  found2 = start_pos;
  for (i = 1; i < MAXNTN && found2 <= max_pos; i++)
    if (i != country && (ntn_tptr = world.np[i]) != NULL) {

      /* if troops are not visible, then skip the nation */
      if((country != UNOWNED) &&
	 MAGIC(ntn_tptr->powers[MAG_WIZARDRY], MW_THEVOID) &&
	 !WIZ_MAGIC(MW_SEEALL) ) continue;

      /* count army troops in the sector */
      enemy = 0L;
      for (army_tptr = ntn_tptr->army_list;
	   army_tptr != NULL;
	   army_tptr = army_tptr->next) {

	/* go until troops are found in the sector */
	if (ARMYT_XLOC == XREAL && ARMYT_YLOC == YREAL) break;

      }
      /* now go through those in the sector! */
      for (; army_tptr != NULL; army_tptr = army_tptr->nrby) {

	/* monsters are shown; others are counted   */
	/* monsters are unaffected by nation powers */
	if (a_ismonster(ARMYT_TYPE)) {
	  found2++;
	show_monster_troop:
	  if (found2 - 1 < max_pos) {
	    sprintf(string, "%d %s",
		    ARMYT_SIZE, ainfo_list[ARMYT_TYPE].name);
	    if (ARMYT_SIZE > 1) strcat(string, "s");
	    sprintf(sidestr, "%.*s", SCREEN_RIGHT, string);
	    mvaddstr(found2 - 1, COLS - 20, sidestr);
	  } else {
	    if (show_all == FALSE) break;
	    mvaddstr(max_pos, COLS - 19, "-- 'Q' to quit --");
	    clrtoeol();
	    refresh();
	    if (((j = next_char()) == 'q') || (j == 'Q')) {
	      asked = TRUE;
	      break;
	    }
	    for (j = start_pos; j <= max_pos + 1; j++) {
	      move(j, COLS - 20);
	      clrtoeol();
	    }
	    found2 = start_pos;
	    goto show_monster_troop;
	  }
	} else if (a_isleader(ARMYT_TYPE)) {
	  /* leaders are only a single person */
	  enemy++;
	} else if ((is_god == TRUE) ||
		   (!a_isagent(ARMYT_TYPE) &&
		    !(a_isscout(ARMYT_TYPE) &&
		      a_needtrain(ARMYT_TYPE)))) {
	  enemy += ARMYT_SIZE;
	}

      }

      /* display troop count */
      if ((enemy > 0L) && ((found2 < max_pos) || show_all)) {

	/* check for exit */
	if (found2 == max_pos) {
	  mvaddstr(max_pos, COLS - 19, "-- 'Q' to quit --");
	  clrtoeol();
	  refresh();
	  if (((j = next_char()) == 'q') || (j == 'Q')) {
	    asked = TRUE;
	    break;
	  }
	  for (j = start_pos; j <= max_pos + 1; j++) {
	    move(j, COLS - 20);
	    clrtoeol();
	  }
	  found2 = start_pos;
	}

	/* can they see everything? */
	if (country == UNOWNED || WIZ_MAGIC(MW_SEEALL)) {
	  sprintf(string, "%s: %ld men",
		  ntn_tptr->name, distort_vision(XREAL, YREAL,
						 enemy, enemy * 3));
	} else if (MAGIC(ntn_tptr->powers[MAG_WIZARDRY], MW_HIDDEN)) {
	  /* hidden army count */
	  sprintf(string, "%s: ?? men", ntn_tptr->name);
	} else {
	  /* provide the estimate */
	  sprintf(string, "%s: %d men", ntn_tptr->name,
		  distort_vision(XREAL, YREAL, enemy, enemy * 3));
	}
	sprintf(sidestr, "%.*s", SCREEN_RIGHT, string);
	mvaddstr(found2, COLS - 20, sidestr);
      }
      if (enemy > 0L) found2++;

      /* now count all naval units */
      enemy = 0L;
      for (navy_tptr = ntn_tptr->navy_list;
	   navy_tptr != NULL;
	   navy_tptr = navy_tptr->next) {

	/* check if in sector */
	if (NAVYT_XLOC == XREAL && NAVYT_YLOC == YREAL) {

	  /* account for ships in the area */
	  for (j = N_LIGHT; j <= N_HEAVY; j++) {
	    for (k = 0; k < NSHP_NUMBER; k++) {
	      enemy += N_CNTSHIPS(NAVYT_SHIPS[k], j);
	    }
	  }

	  if ((found2 >= max_pos) &&
	      !show_all) break;

	}
      }

      /* display any count */
      if ((enemy > 0L) && ((found2 < max_pos) || show_all)) {

	/* check for exit */
	if (found2 == max_pos) {
	  mvaddstr(max_pos, COLS - 19, "-- 'Q' to quit --");
	  clrtoeol();
	  refresh();
	  if (((j = next_char()) == 'q') || (j == 'Q')) {
	    asked = TRUE;
	    break;
	  }
	  for (j = start_pos; j <= max_pos + 1; j++) {
	    move(j, COLS - 20);
	    clrtoeol();
	  }
	  found2 = start_pos;
	}

	/* can they see clearly? */
	if (country == UNOWNED || WIZ_MAGIC(MW_SEEALL)) {
	  sprintf(string, "%s: %d ships", ntn_tptr->name,
		  distort_vision(XREAL, YREAL, enemy, enemy * 3));
	} else if (MAGIC(ntn_tptr->powers[MAG_WIZARDRY], MW_HIDDEN)) {
	  sprintf(string, "%s: ?? ships", ntn_tptr->name);
	} else {
	  /* estimate */
	  sprintf(string, "%s: %d ships", ntn_tptr->name,
		  distort_vision(XREAL, YREAL, enemy, enemy * 3));
	}
	sprintf(sidestr, "%.*s", SCREEN_RIGHT, string);
	mvaddstr(found2, COLS - 20, sidestr);
      }
      if (enemy > 0L) found2++;

      /* now count all caravan units */
      enemy = 0L;
      for (cvn_tptr = ntn_tptr->cvn_list;
	   cvn_tptr != NULL;
	   cvn_tptr = cvn_tptr->next) {

	/* check if in sector */
	if (CVNT_XLOC == XREAL && CVNT_YLOC == YREAL) {

	  /* account for fleets in the area */
	  enemy += CVNT_SIZE * 10;

	  /* no need to go on */
	  if ((found2 >= max_pos) &&
	      !show_all) break;

	}
      }

      /* display any count */
      if ((enemy > 0L) && ((found2 < max_pos) || show_all)) {

	/* check for exit */
	if (found2 == max_pos) {
	  mvaddstr(max_pos, COLS - 19, "-- 'Q' to quit --");
	  clrtoeol();
	  refresh();
	  if (((j = next_char()) == 'q') || (j == 'Q')) {
	    asked = TRUE;
	    break;
	  }
	  for (j = start_pos; j <= max_pos + 1; j++) {
	    move(j, COLS - 20);
	    clrtoeol();
	  }
	  found2 = start_pos;
	}

	/* can they see clearly? */
	if (country == UNOWNED || WIZ_MAGIC(MW_SEEALL)) {
	  sprintf(string, "%s: %d wagons", ntn_tptr->name,
		  distort_vision(XREAL, YREAL, enemy, enemy * 3));
	} else if (MAGIC(ntn_tptr->powers[MAG_WIZARDRY], MW_HIDDEN)) {
	  sprintf(string, "%s: ?? wagons", ntn_tptr->name);
	} else {
	  /* estimate */
	  sprintf(string, "%s: %d wagons", ntn_tptr->name,
		  distort_vision(XREAL, YREAL, enemy, enemy * 3));
	}
	sprintf(sidestr, "%.*s", SCREEN_RIGHT, string);
	mvaddstr(found2, COLS - 20, sidestr);
      }
      if (enemy > 0L) found2++;

    }

  /* now finish up */
  if ((asked == FALSE) && (show_all == TRUE) && (found2 > start_pos)) {
    mvaddstr(max_pos, COLS - 19, "-- Hit Any Key --");
    clrtoeol();
    refresh();
    (void) next_char();
  }
  return(found2);
}

/* ZOOM_OUT -- Make the range of sectors covered larger */
int
zoom_out PARM_0(void)
{
  if (zoom_level < ZOOM_NUMBER - 1) {
    zoom_level++;
    centermap();
  }
  return(0);
}

/* ZOOM_IN -- Increase the detail of the map */
int
zoom_in PARM_0(void)
{
  if (zoom_level > 0) {
    zoom_level--;
    centermap();
  }
  return(0);
}

/* TOGGLE_INFOMODE -- toggle the information mode */
int
toggle_infomode PARM_0(void)
{
  /* quickie function to toggle it */
  if (conq_infomode == TRUE) {
    conq_infomode = FALSE;
  } else {
    conq_infomode = TRUE; 
  }
  centermap();
  return(0);
}
