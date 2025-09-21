// SPDX-License-Identifier: GPL-3.0-or-later
/* handle all of the main input and command parsing for conquer */
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
#include "armyX.h"
#include "infoG.h"
#include "itemX.h"
#include "moveX.h"
#include "butesX.h"
#include "calenX.h"
#include "desigX.h"
#include "magicX.h"
#include "mtrlsX.h"
#include "activeX.h"
#include "hlightX.h"
#include "stringX.h"
#include "tgoodsX.h"
#include "optionsX.h"
#define USE_CODES
#define PRINT_CODES
#include "cityX.h"
#include "worldX.h"

/* DO_UNIMPLEMENTED -- Just indicate that this function is not yet done */
int
do_unimplemented PARM_0(void)
{
  errormsg("This command has not yet been written");
  return(0);
}

/* DO_QUIT -- goodbye cruel world! */
int
do_quit PARM_0(void)
{
  extern int conquer_done;

  /* confirm it */
  mvaddstr(LINES - 1, 0, "Do you really wish to exit conquer? ");
  clrtoeol();
  refresh();
  if (y_or_n()) {
    conquer_done = TRUE;
  }
  return(0);
}

/* DO_MOVEPEOP -- Relocate people from the current sector */
int
do_movepeop PARM_0(void)
{
  ITEM_PTR cost_ptr = NULL;
  long people_to_move;
  int i, oldx_people, oldy_people, newx_people, newy_people;

  /* check the sector */
  if (!XY_ONMAP(XREAL, YREAL)) {
    errormsg("You have no people in that sector");
    return(0);
  }
  sct_ptr = &(sct[XREAL][YREAL]);
  if ((sct_ptr->people == 0) ||
      ((country != UNOWNED) && (sct_ptr->owner != country))) {
    errormsg("You have no people in that sector");
    return(0);
  }
  if ((country != UNOWNED) &&
      minor_desg(sct_ptr->designation, MIN_SIEGED)) {
    errormsg("Your people are under siege and are not able to leave");
    return(0);
  }

  /* find out how many to move */
  clear_bottom(0);
  mvprintw(LINES - 4, 0, "There are %ld people in the sector",
	   sct_ptr->people);
  if (is_god == TRUE) {
    /* god has no cost */
    if (get_god(sct_ptr->owner, TRUE)) return(0);
    people_to_move = sct_ptr->people;
  } else {
    /* check how many may be moved */
    if ((cost_ptr = find_resources(XREAL, YREAL, FALSE)) == NULL) {
      errormsg("This sector does not have resources for relocating people");
      return(MOVECOST);
    }
    if (cost_ptr->mtrls[MTRLS_TALONS] < (itemtype) PEOPLE_MCOST) {
      errormsg("This sector does not have resources for relocating people");
      return(MOVECOST);
    }
    people_to_move = min(sct_ptr->people,
			 (long) ITEMT_MTRLS[MTRLS_TALONS] / PEOPLE_MCOST);
  }

  /* get how many */
  mvprintw(LINES - 3, 0, "You may move up to %ld people",
	   people_to_move);
  mvaddstr(LINES - 2, 0, "How many do you wish to move? ");
  global_long = get_number(FALSE);
  if ((no_input == TRUE) || (global_long == 0L))
    goto done_pmove;

  /* check limitations */
  if (global_long > sct_ptr->people) {
    errormsg("There are not that many people in the sector!");
    goto done_pmove;
  }
  if (global_long > people_to_move) {
    errormsg("You cannot afford to move that many people");
    goto done_pmove;
  }
  people_to_move = global_long;

  /* now move them */
  oldx_people = XREAL;
  oldy_people = YREAL;
  move_parse(MOVE_PEOPLE);
  if ((XREAL != oldx_people) || (YREAL != oldy_people)) {

    if (is_god == FALSE) {
      /* subtract the cost */
      for (i = 0; i < MTRLS_NUMBER; i++) {
	cost_ptr->mtrls[i] = (itemtype) 0L;
      }
      cost_ptr->mtrls[MTRLS_TALONS] =
	(itemtype) people_to_move * PEOPLE_MCOST;
      if (take_resources(oldx_people, oldy_people, cost_ptr, FALSE)) {
	errormsg("Major Error: Could not take resources");
	goto done_pmove;
      }
    }

    /* relocate the people */
    sct[XREAL][YREAL].people += people_to_move;
    SADJPEOP;
    newx_people = XREAL;
    newy_people = YREAL;
    xcurs = oldx_people - xoffset;
    ycurs = oldy_people - yoffset;
    sct[XREAL][YREAL].people -= people_to_move;
    SADJPEOP;

    /* now fix distribution weights if necessary */
    if (IS_CITY(sct[XREAL][YREAL].designation)) {
      set_weights(FALSE);
      if ((city_ptr = citybyloc(ntn_ptr, XREAL, YREAL)) != NULL) {
	if (CITY_PEOPLE > 0) {
	  if (CITY_PEOPLE < people_to_move) {
	    CITY_PEOPLE = 0;
	  } else {
	    CITY_PEOPLE -= people_to_move;
	  }
	  CADJPEOP;
	}
      }
    }

    /* record resource selection */
    if (is_god == FALSE) {
      TAKESTART;
      global_long = cost_ptr->mtrls[MTRLS_TALONS];
      TAKEMTRLS(MTRLS_TALONS);
      TAKEFINISH(FALSE);
    }

    /* keep sector location */
    xcurs = newx_people - xoffset;
    ycurs = newy_people - yoffset;
  }

  /* fix god if he is done */
 done_pmove:
  if (is_god == TRUE) {
    reset_god();
  } else {
    free(cost_ptr);
  }
  return(MOVECOST);
}

/* JMP_SITE -- If the sector is a possible jumping location */
static void
jmp_site PARM_2(int, x, int, y)
{
  if (VIS_CHECK(x, y)) {
    global_int = TRUE;
  }
}

/* JUMP_TO -- Go to a chosen location */
static void
jump_to PARM_1(int, jtype)
{
  NTN_PTR n1_ptr;
  int jumped = FALSE, xjump, yjump;

  switch (jtype) {
  case JUMP_CAP:
    if (country == UNOWNED || n_ismonster(ntn_ptr->active)) {
      int byonce = 0, ntnnum;

      /* first jump to the capital of the current nation */
      ntnnum = sct[XREAL][YREAL].owner;
      if ((n1_ptr = world.np[ntnnum]) == NULL ||
	  (n1_ptr->capx == XREAL && n1_ptr->capy == YREAL)) {
	ntnnum++;
      }

      /* now hop between nation capitals */
      while (((n1_ptr = world.np[ntnnum]) == NULL) ||
	     (n_ismonster(n1_ptr->active)) ||
	     !XY_INMAP(n1_ptr->capx, n1_ptr->capy) ||
	     (ntnnum != sct[n1_ptr->capx][n1_ptr->capy].owner)) {
	ntnnum++;
	if (ntnnum >= MAXNTN) {
	  ntnnum = 1;
	  if (byonce++ >= 1) return;
	}
      }

      /* jump to this capital by default */
      clear_bottom(0);
      mvprintw(LINES - 3, 0, "Jump to which Nation's capital? [%s] ",
	       n1_ptr->name);
      ntnnum = get_country(FALSE);
      if (no_input == FALSE) {
	if (ntnnum == MAXNTN) {
	  errormsg("Sorry, but there is no such nation");
	  return;
	}
	if ((ntn_tptr == NULL) ||
	    n_ismonster(ntn_tptr->active)) {
	  errormsg("Sorry, but that nation has no Capital");
	  return;
	}
      } else {
	ntn_tptr = n1_ptr;
      }
      xcurs = ntn_tptr->capx;
      ycurs = ntn_tptr->capy;
    } else {
      /* go right to the capital */
      xcurs = ntn_ptr->capx;
      ycurs = ntn_ptr->capy;
    }
    jumped = TRUE;
    xoffset = 0;
    yoffset = 0;
    break;
  case JUMP_PICK:
    clear_bottom(0);

    /* find the X sector to jump to */
    if (Y_ONMAP(YREAL) && VIS_CHECK(XREAL, YREAL)) {
      mvprintw(LINES - 3, 0, "Jump to what X location? [%d] ",
	       xloc_relative(XREAL) );
    } else {
      mvaddstr(LINES - 3, 0, "Jump to what X location? [??] ");
    }
    clrtoeol();
    xjump = get_number(TRUE);
    if (no_input != TRUE) {
      if (world.relative_map && (is_god != TRUE) && (ntn_ptr != NULL)) {
	xjump += ntn_ptr->centerx;
	xjump = (xjump + MAPX) % MAPX;
      }
      jumped = TRUE;
    } else {
      xjump = XREAL;
    }

    /* find the Y sector to jump to */
    if (Y_ONMAP(YREAL) && VIS_CHECK(XREAL, YREAL)) {
      mvprintw(LINES - 2, 0, "Jump to what Y location? [%d] ",
	       yloc_relative(YREAL) );
    } else {
      mvaddstr(LINES - 2, 0, "Jump to what Y location? [??] ");
    }
    clrtoeol();
    yjump = get_number(TRUE);
    if (no_input != TRUE) {
      if (world.relative_map && (is_god != TRUE) && (ntn_ptr != NULL)) {
	yjump += ntn_ptr->centery;
      }
      jumped = TRUE;
    } else {
      yjump = YREAL;
    }
    if (jumped == TRUE) {
      if (!world.relative_map ||
	  (Y_ONMAP(yjump) && VIS_CHECK(xjump, yjump))) {
	jumped = TRUE;
      } else if (Y_ONMAP(yjump)) {
	global_int = FALSE;
	map_loop(xjump, yjump, VISRANGE, jmp_site);
	jumped = global_int;
      }
      if (jumped == FALSE) goto bad_jump;
      xcurs = xjump;
      xoffset = 0;
      ycurs = yjump;
      yoffset = 0;
    }
    break;
  bad_jump:
    errormsg("That sector is not known to your nation");
    jumped = FALSE;
    break;
  case JUMP_SAVE:
    if (xloc_mark == (-1) || yloc_mark == (-1)) {
      jump_to(JUMP_CAP);
    } else {
      xcurs = xloc_mark;
      ycurs = yloc_mark;
      xoffset = 0;
      yoffset = 0;
      jumped = TRUE;
    }
    break;
  }

  /* set things up */
  if (jumped == TRUE) {
    centermap();
  }
}

/* DO_OPTIONS -- Select and adjust the global options */
int
do_options PARM_0(void)
{
  extern KEYSYS_STRUCT global_keysys;
  option_cmd(global_keysys, &cq_bindings);
  return(0);
}

/* OPTION_CMD -- Adjust some options */
void
option_cmd PARM_2(KEYSYS_STRUCT, key_info, KLIST_PTR *, list_of_keys)
{
  KLIST_PTR klist_ptr;
  PARSE_PTR parse_ptr;
  FNCI func_ptr;
  int optval, x, y;
  char storename[LINELTH+1], key_list[MAXKEYS + 1];

  /* list 'em */
  clear_bottom(0);
  sprintf(string, "Opts: ");
  y = LINES - 5;
  mvaddstr(y, 0, string);
  x = strlen(string);
  for (optval = 0; optval < options_number; optval++) {
    if ((y == LINES - 2) &&
	(x > 50)) break;
    strcpy(string, opt_list[optval]);
    if (optval > 0) {
      addstr(", ");
      x += 2;
    }
    if (x + strlen(string) > COLS - 1) {
      x = 1;
      y++;
    }
    mvaddstr(y, x, string);
    x += strlen(string);
  }

  /* now ask 'em */
  if (y < LINES - 2) {
    mvaddstr(LINES - 2, 0, "Perform what option? ");
  } else {
    addstr("; Your Choice? ");
  }
  optval = get_option();
  if (no_input == TRUE) return;
  if (optval == options_number) {
    errormsg("I have not heard of any such option");
    return;
  }
  clear_bottom(0);
  mvprintw(LINES - 4, 0, "Conquer Option: %s", opt_list[optval]);

  /* now adjust the various options */
  switch (optval) {
  case OPT_READ:
    /* read in info from a configuration file */
    mvaddstr(LINES - 3, 0,
	     "This reads in customization settings from a given file");
    mvaddstr(LINES - 2, 0, "Read from which file? ");

    /* get the input */
    get_string(storename, STR_NORM, LINELTH - sizeof(CQRC_TAG));
    if (no_input == TRUE) break;

    strcat(storename, CQRC_TAG);
    /* read it in */
    if (read_custom(storename) == -1) {
      errormsg("The file was not able to be read");
      break;
    }
    break;
  case OPT_STORE:
    /* save all of the configuration to a file */
    mvaddstr(LINES - 3, 0,
	     "Create a customization file with the current settings.");
    mvprintw(LINES - 2, 0,
	     "[\"%s\" appended to name] Write to which directory/file? ",
	     CQRC_TAG);

    /* get the input */
    get_string(string, STR_NORM, LINELTH - sizeof(CQRC_TAG));
    if (no_input == TRUE) break;

    /* check for quickie conversion */
    strcat(string, CQRC_TAG);
    if ((string[0] != '/') &&
	(getenv("HOME") != NULL)) {
      if ((string[0] == '~') &&
	  (string[1] == '/')) {
	optval = 2;
      } else {
	optval = 0;
      }
      sprintf(storename, "%s/%s", getenv("HOME"), string + optval);
    } else {
      strcpy(storename, string);
    }

    /* send it out */
    if (write_custom(storename) == -1) {
      errormsg("The file could not be opened for writing");
    } else {
      sprintf(string, "Write succeeded to <%s>", storename);
      bottommsg(string);
      sleep(1);
    }
    break;
  case OPT_REBIND:
    /* change a key binding to a different function */
    if (get_keys("Rebind", key_list) == 0) break;

    /* now obtain the key binding */
    if ((klist_ptr = find_keys(*list_of_keys, key_list)) == NULL) {
      errormsg("There is no such key binding set");
      break;
    }

    /* is it the last one? */
    if (num_bindings(*list_of_keys, klist_ptr->key_data.func) < 2) {
      errormsg("You may not remove the last key binding for a function");
      break;
    }

    /* which key is it currently? */
    if ((parse_ptr = find_func(klist_ptr->key_data.func,
			       key_info.parse_p,
			       key_info.num_parse)) != NULL) {
      mvprintw(LINES - 3, 0, "Currently it is bound to the function: %s",
	       parse_ptr->realname);
    } else {
      errormsg("That key is not currently bound to a function");
    }

    /* query what to become */
    clrtoeol();
    mvaddstr(LINES - 2, 0, "Make ");
    show_str(key_list, FALSE);
    addstr(" be bound to what function? ");
    clrtoeol();
    if ((func_ptr = get_func(key_info)) != NULL) {
      klist_ptr->key_data.func = func_ptr;
    }
    break;
  case OPT_CHECKKEYS:
    /* check if the key bindings are set properly */
    if (check_keys(list_of_keys, key_info) == 0) {
      errormsg("All of your keybindings are set properly");
    }
    break;
  case OPT_BINDKEY:
    /* bind a new key */
    if (get_keys("Set", key_list) == 0) break;

    /* check if there is already a key bound to that function */
    if (find_keys(*list_of_keys, key_list) != NULL) {
      errormsg("That key binding already exists");
      break;
    }

    /* now get a function to bind it to */
    move(LINES - 3, 0);
    clrtoeol();
    mvaddstr(LINES - 2, 0, "Bind ");
    show_str(key_list, FALSE);
    addstr(" to what function? ");
    clrtoeol();
    if ((func_ptr = get_func(key_info)) != NULL) {
      bind_keys(list_of_keys, key_list, func_ptr);
    }
    break;
  case OPT_UNBIND:
    /* change a key binding to a different function */
    if (get_keys("Remove", key_list) == 0) break;

    /* determine number of bindings on that function */
    if ((klist_ptr = find_keys(*list_of_keys, key_list)) == NULL) {
      errormsg("There is no such key binding set");
      break;
    }
    if (num_bindings(*list_of_keys, klist_ptr->key_data.func) < 2) {
      errormsg("You may not remove the last key binding for a function");
      break;
    }

    /* try to remove it */
    if (rm_keys(list_of_keys, key_list) == 0) {
      errormsg("There is no such key binding set");
    } else {
      errormsg("Keybinding removed");
    }
    break;
  case OPT_RESETKEYS:
    /* reset the entire key map */
    init_keys(list_of_keys, key_info.kbind_p, key_info.num_binds);
    break;
  case OPT_HEADERS:
    /* display the headers in the mail reader */
    mvprintw(LINES - 3, 0, "Starting with headers in the mail reader is %s",
	     conq_mheaders ? "on" : "off");
    mvprintw(LINES - 2, 0, "Turn it %s? [yn]", conq_mheaders ? "off" : "on");
    if (cr_or_y()) {
      conq_mheaders ^= TRUE;
    }
    break;
  case OPT_MAILCHECK:
    /* check the system mail check */
    mvprintw(LINES - 3, 0, "The checking of the system mail is currently %s.",
	     dosysm_check ? "on" : "off");
    mvprintw(LINES - 2, 0, "Turn it %s? [yn]", dosysm_check ? "off" : "on");
    if (cr_or_y()) {
      dosysm_check ^= TRUE;
    }
    break;
  case OPT_TBELL:
    /* adjust terminal bell */
    mvprintw(LINES - 3, 0, "The terminal bell is currently %s.",
	     conq_beeper ? "on" : "off");
    mvprintw(LINES - 2, 0, "Turn it %s? [yn]", conq_beeper ? "off" : "on");
    if (cr_or_y()) {
      conq_beeper ^= TRUE;
    }
    break;
  case OPT_EXPERT:
    /* adjust expert mode */
    mvprintw(LINES - 3, 0, "The expert mode option is currently %s.",
	     conq_expert ? "on" : "off");
    mvprintw(LINES - 2, 0, "Turn it %s? [yn]", conq_expert ? "off" : "on");
    if (cr_or_y()) {
      conq_expert ^= TRUE;
    }
    break;
  case OPT_INFOMODE:
    /* adjust infomode */
    mvprintw(LINES - 3, 0, "The information mode option is currently %s.",
	     conq_infomode ? "on" : "off");
    mvprintw(LINES - 2, 0, "Turn it %s? [yn]", conq_infomode ? "off" : "on");
    if (cr_or_y()) {
      conq_infomode ^= TRUE;
      centermap();
    }
    break;
  case OPT_ALLBLANKS:
    /* adjust all blanks mode */
    mvprintw(LINES - 3, 0, "The \"all blanks\" option is currently %s.",
	     conq_allblanks ? "on" : "off");
    mvprintw(LINES - 2, 0, "Turn it %s? [yn]", conq_allblanks ? "off" : "on");
    if (cr_or_y()) {
      conq_allblanks ^= TRUE;
    }
    break;
  case OPT_BOTTOMLINES:
    /* adjust bottom lines mode */
    mvprintw(LINES - 3, 0, "The \"bottom line\" option is currently %s.",
	     conq_bottomlines ? "on" : "off");
    mvprintw(LINES - 2, 0, "Turn it %s? [yn]", conq_bottomlines ?
	     "off" : "on");
    if (cr_or_y()) {
      conq_bottomlines ^= TRUE;
    }
    break;
  case OPT_WATER_BTM:
    /* adjust water bottom mode */
    mvprintw(LINES - 3, 0, "The \"water bottoms\" option is currently %s.",
	     conq_waterbottoms ? "on" : "off");
    mvprintw(LINES - 2, 0, "Turn it %s? [yn]", conq_waterbottoms ?
	     "off" : "on");
    if (cr_or_y()) {
      conq_waterbottoms ^= TRUE;
    }
    break;
  case OPT_GAUDY:
    /* adjust gaudy / non-gaudy input */
    mvprintw(LINES - 3, 0, "The Gaudy option is currently %s.",
	     conq_gaudy ? "on" : "off");
    mvprintw(LINES - 2, 0, "Turn it %s? [yn]", conq_gaudy ? "off" : "on");
    if (cr_or_y()) {
      conq_gaudy ^= TRUE;
    }
    break;
  case OPT_CENTER:
    /* realign the relative central position */
    recenter_ntn();
    break;
  case OPT_SUPPLY:
    /* check the environment */
    if (MAXSUPPLIES == 0) {
      errormsg("Supplies are not implemented in this campaign");
      break;
    }

    /* set the supply level */
    mvprintw(LINES - 3, 0, "The supply level is currently %d.",
	     conq_supply_level);
    mvprintw(LINES - 2, 0, "Set it to what value? ");
    x = get_number(FALSE);
    if (no_input == TRUE) break;

    /* check it */
    if (x > MAXSUPPLIES) {
      sprintf(string, "A supply level of %d is the maximum",
	      MAXSUPPLIES);
      errormsg(string);
      break;
    }
    conq_supply_level = x;
    break;
  case OPT_PAGEOFFSET:
    /* just set the value of this option */
    mvprintw(LINES - 3, 0, "The pager offset is currently %d.",
	     pager_offset);
    mvprintw(LINES - 2, 0, "Set it to what value? ");
    x = get_number(FALSE);
    if (no_input == TRUE) break;

    /* now set it */
    pager_offset = x;
    break;
  case OPT_PAGESCROLL:    
    /* just set the value of this option */
    mvprintw(LINES - 3, 0, "The pager scroll setting is currently %d.",
	     pager_scroll);
    mvprintw(LINES - 2, 0, "Set it to what value? ");
    x = get_number(FALSE);
    if (no_input == TRUE) break;

    /* now set it */
    pager_scroll = x;
    break;
  case OPT_PAGETAB:
    /* get it and set it */
    mvprintw(LINES - 3, 0, "The pager tab setting is currently %d.",
	     pager_tab);
    mvprintw(LINES - 2, 0, "Set it to what value? ");
    x = get_number(FALSE);
    if (no_input == TRUE) break;

    /* now set it */
    pager_tab = x;
    break;
  default:
    errormsg("unknown option encounted ... !em pleh");
    break;
  }
}

/* DO_REFRESH -- Refresh the screen */
int
do_refresh PARM_0(void)
{
  /* should redraw it fully */
  redraw = DRAW_FULL;
  return(0);
}

/* ARMY_REPORT -- Show the army information for the nation */
int
army_report PARM_0(void)
{
  /* do it */
  if (show_info(INFO_ARMY) == TRUE) {
    errormsg("You have no armies within your nation");
  }
  return(MOVECOST);
}

/* DO_NARMY -- Go to the next army in the nation */
int
do_narmy PARM_0(void)
{
  goto_army(-1);
  return(0);
}

/* DO_SARMY -- Go to a specified army */
int
do_sarmy PARM_0(void)
{
  /* begin */
  clear_bottom(0);
  if ((is_god == TRUE) && get_god(-1, -1)) return(0);

  /* get the unit id */
  mvaddstr( LINES - 3, 0, "Go to what army unit? " );
  clrtoeol();
  if (((global_int = get_number(FALSE)) >= 0) && (no_input == FALSE)) {
    goto_army(global_int);
  }
  if (is_god == TRUE) {
    reset_god();
  }
  return(0);
}

/* GROUP_REPORT -- Show the army information for a particular sector */
int
group_report PARM_0(void)
{
  /* do it */
  if (show_info(INFO_GRPARM) == TRUE) {
    errormsg("You have no armies within this sector");
  }
  return(MOVECOST);
}

/* NAVY_REPORT -- Show the navy information for the nation */
int
navy_report PARM_0(void)
{
  /* do it */
  if (show_info(INFO_NAVY) == TRUE) {
    errormsg("You have no fleets within your nation");
  }
  return(MOVECOST);
}

/* DO_NNAVY -- Go to the next fleet in the nation */
int
do_nnavy PARM_0(void)
{
  goto_navy(-1);
  return(0);
}

/* DO_SNAVY -- Go to a specified naval unit */
int
do_snavy PARM_0(void)
{
  /* begin */
  clear_bottom(0);
  if ((is_god == TRUE) && get_god(-1, -1)) return(0);

  /* get the navy id */
  mvaddstr( LINES - 3, 0, "Go to what naval unit? " );
  clrtoeol();
  if ((global_int = get_number(FALSE)) >= 0 && no_input == FALSE) {
    goto_navy(global_int);
  }
  if (is_god == TRUE) {
    reset_god();
  }
  return(0);
}

/* CVN_REPORT -- Show the caravan information for the nation */
int
cvn_report PARM_0(void)
{
  /* do it */
  if (show_info(INFO_CVN) == TRUE) {
    errormsg("You have no caravans within your nation");
  }
  return(MOVECOST);
}

/* DO_NCVN -- Go to the next caravan in the nation */
int
do_ncvn PARM_0(void)
{
  goto_cvn(-1);
  return(0);
}

/* DO_SCVN -- Go to the specified caravan */
int
do_scvn PARM_0(void)
{
  /* start it */
  clear_bottom(0);
  if ((is_god == TRUE) && get_god(-1, -1)) return(0);

  /* get the unit id */
  mvaddstr( LINES - 3, 0, "Go to what caravan unit? " );
  clrtoeol();
  if ((global_int = get_number(FALSE)) >= 0 && no_input == FALSE) {
    goto_cvn(global_int);
  }
  if (is_god == TRUE) {
    reset_god();
  }
  return(0);
}

/* DO_MOTDEDIT -- Try to edit the motd file */
int
do_motdedit PARM_0(void)
{
  /* do it, and clean up afterward */
#ifdef ALLOW_EDIT_FORK
  if (is_god == TRUE) {
    char pstr[LINELTH];
    sprintf(pstr, "NOTE: Only the first %d lines are part of the MOTD",
	    MOTDLINES);
    fork_edit_on_file(motdfile, pstr);
    redraw = DRAW_FULL;
  }
#endif /* ALLOW_FORK_EDIT */
  return(0);
}

/* DO_SCITY -- Go to the specified city */
int
do_scity PARM_0(void)
{
  CITY_PTR c1_ptr, cprev_ptr;

  /* start it */
  clear_bottom(0);
  if ((is_god == TRUE) && get_god(-1, -1)) return(0);

  /* check if there are any to select */
  if (ntn_ptr == NULL) return(0);
  if (ntn_ptr->city_list == NULL) {
    errormsg("You have no supply centers to go to");
    return(0);
  }

  /* get the city name */
  mvaddstr( LINES - 3, 0, "Go to which supply center? " );
  clrtoeol();
  get_string(string, STR_CITY, NAMELTH);
  if ((no_input == FALSE) &&
      ((c1_ptr = citybyname(string)) != NULL)) {
    goto_city(c1_ptr);
  } else if (no_input == FALSE) {
    if ((strcmp(string, "+") == 0) ||
	(strcmp(string, "-") == 0)) {
      for (c1_ptr = ntn_ptr->city_list, cprev_ptr = NULL;
	   c1_ptr != NULL;
	   cprev_ptr = c1_ptr, c1_ptr = c1_ptr->next) {
	if ((c1_ptr->xloc == XREAL) &&
	    (c1_ptr->yloc == YREAL)) break;
      }
      if (strcmp(string, "+") == 0) {
	if ((c1_ptr == NULL) ||
	    (c1_ptr->next == NULL)) {
	  c1_ptr = ntn_ptr->city_list;
	} else {
	  c1_ptr = c1_ptr->next;
	}
      } else {
	if (cprev_ptr == NULL) {
	  for (c1_ptr = ntn_ptr->city_list;
	       c1_ptr != NULL;
	       c1_ptr = c1_ptr->next) {
	    if (c1_ptr->next == NULL) break;
	  }
	} else {
	  c1_ptr = cprev_ptr;
	}
      }
      goto_city(c1_ptr);
    } else {
      errormsg("There is no city by that name in this nation");
    }
  }
  if (is_god == TRUE) {
    reset_god();
  }
  return(0);
}

/* DO_PAPER -- Select a newspaper for reading */
int
do_paper PARM_0(void)
{
  int xloc, yloc, count, value;
  int found_some = FALSE, len_val;
  char buf[LINELTH];

  /* clean up the screen */
  clear_bottom(0);
  xloc = 2;
  yloc = LINES - 4;

  /* go through all the newspapers */
  for (count = 0; count < MAXNEWS; count++) {
    if ((value = TURN - count) >= 0) {

      /* check if it exists */
      sprintf(string, "%s.%03d", newsfile, value - START_TURN);
      if (exists(string) == 0) {

	/* show the available newspaper */
	if (value - START_TURN == 0) {
	  sprintf(buf, "%c) The Epoch", 'A' + count);
	} else {
	  roman_number( string, YEAR(value - 1) );
	  sprintf(buf, "%c) %s %s", 'A' + count,
		  PMONTH(value - 1), string);
	}
	len_val = strlen(buf);
	if (len_val + xloc > COLS - 3) {
	  xloc = 2;
	  yloc++;
	}
	mvaddstr(yloc, xloc, buf);
	xloc += len_val + 2;
	found_some = TRUE;

      }
    }
  }
  if (found_some == FALSE) {
    errormsg("There are no newspapers available");
    return(0);
  }

  /* ask for the paper */
  mvaddstr(LINES - 5, 0, "Read Which Newspaper? ");
  refresh();
  value = next_char();
  if ((value == ' ') ||
      (value == '\r') ||
      (value == '\n')) {
    /* quick exit */
    return(0);
  } else if (islower(value)) {
    value = toupper(value);
  }

  /* now check the input */
  count = value - 'A';
  if ((count < 0) ||
      (count >= MAXNEWS)) {
    errormsg("Wake Up!  I didn't give you that choice");
    return(0);
  }

  /* now read the newspaper */
  sprintf(string, "%s.%03d", newsfile, TURN - START_TURN - count);
  if (page_file(string, TRUE) == TRUE) {
    errormsg("It looks as though the paper hasn't been delivered yet");;
  } else if (count == 0) {
    conq_news_status = STMAIL_NONE;
  }
  return(0);
}

/* DO_SCORE -- Show the score of the nations in the world */
int
do_score PARM_0(void)
{
  /* go to it */
  if (show_info(INFO_NTN) == TRUE) {
    errormsg("No countries?  How did you get here?");
  }
  return(MOVECOST * 2);
}

/* DO_DIPLOMACY -- Show the nation diplomacy screens */
int
do_diplomacy PARM_0(void)
{
  /* diplomacy screens */
  if (show_info(INFO_DIP) == TRUE) {
    errormsg("No countries?  How did you get here?");
  }
  return(MOVECOST);
}

/* DO_CJUMP -- Go to the capital of the nation */
int
do_cjump PARM_0(void)
{
  /* reset selection and go */
  pager = 0;
  selector = 0;
  jump_to(JUMP_CAP);
  return(0);
}

/* DO_MJUMP -- Go to the marked sector */
int
do_mjump PARM_0(void)
{
  /* reset selection and hop to it */
  pager = 0;
  selector = 0;
  jump_to(JUMP_SAVE);
  return(0);
}

/* DO_SJUMP -- Go to the specified sector */
int
do_sjump PARM_0(void)
{
  /* reset the selector and then go */
  pager = 0;
  selector = 0;
  jump_to(JUMP_PICK);
  return(0);
}

/* DO_RELOGIN -- Attempt to login as another nation */
int
do_relogin PARM_0(void)
{
  char passwd[PASSLTH+1], ontnname[NAMELTH+1];
  int ocountry;

#ifdef SECURITY
  if ((strcmp(loginname, LOGIN) != 0) &&
      ((world.demilimit) ||
       (strcmp(loginname, world.demigod) != 0))) return(0);
#endif /*SECURITY*/

  /* clean up bottom of screen */
  clear_bottom(0);
  standout();
  mvaddstr(LINES - 3, 0, "Change login to:");
  standend();
  addch(' ');
  refresh();

  /* find out who to become */
  ocountry = country;
  country = get_country(FALSE);

  /* check validity of country choice */
  if ((country < 0) || (country >= MAXNTN) ||
      ((country != UNOWNED) && !n_isactive((world.np[country])->active))) {
    country = ocountry;
    return(0);
  }
  if (country == ocountry) {
    errormsg("What?  You are already logged into that nation.");
    return(0);
  }

  /* get password */
  clear_bottom(0);
  ntn_ptr = world.np[country];
  mvaddstr(LINES - 2, 0, "What is your Nation's Password: ");
  clrtoeol();
  (void) get_pass(passwd);
#ifdef CRYPT
  strcpy(string, crypt(passwd, SALT));
#else
  strcpy(string, passwd);
#endif

  /* check the password */
  if (((ntn_ptr == NULL) ||
       (strncmp(string, ntn_ptr->passwd, PASSLTH) != 0)) &&
      (strncmp(string, world.passwd, PASSLTH) != 0)) {
    errormsg("Sorry, Invalid Password Match");
    country = ocountry;
    ntn_ptr = world.np[country];
    return(0);
  }

  /* check login */
  strcpy(ontnname, nationname);
  if (country == UNOWNED) {
    strcpy(nationname, "god");
  } else {
    strcpy(nationname, ntn_ptr->name);
  }

  /* if appropriate; verify login identity */
  if ((is_god == FALSE) && (ntn_ptr != NULL) && world.verify_login) {
    if ((strcmp(LOGIN, loginname) != 0) &&
	(strcmp(world.demigod, loginname) != 0) &&
	(strcmp(ntn_ptr->login, loginname) != 0)) {
      /* UNIMPLEMENTED: SEND MAIL TO GOD */
      errormsg("Sorry, you are not permitted to access that nation");
      country = ocountry;
      ntn_ptr = world.np[country];
      strcpy(nationname, ontnname);
      return(0);
    }

  }

  /* check lock file */
  sprintf(string, "%s.%s", nationname, isontag);
  if (check_lock(string, FALSE) == -1) {
    errormsg("Sorry, that Nation is already logged in");
    country = ocountry;
    ntn_ptr = world.np[country];
    strcpy(nationname, ontnname);
    return(0);
  }

  /* close up prior nation settings */
  close_ntn(ocountry, ontnname);

  /* now read everything in again */
  fprintf(stderr, "\n");
  move(LINES - 3, 0);
  read_data();
  execute(UNOWNED);
  visibility_data = new_maplong(visibility_data);
  set_weights(TRUE);

  /* set focus to new nation */
  open_ntn(country);
  hex_init();
  stmail_init();

  /* go to that nations capital */
  if ((country == UNOWNED) || n_ismonster(ntn_ptr->active)) {
    xcurs = MAPX / 2 - 1;
    ycurs = MAPY / 2 - 1;
  } else {
    xcurs = ntn_ptr->capx;
    ycurs = ntn_ptr->capy;
  }
  xoffset = 0;
  yoffset = 0;

  /* reset the display */
  centermap();
  redraw = DRAW_FULL;
  return(0);
}

/* AINFO_HELP -- Provide help on the army unit types */
static void
ainfo_help PARM_0(void)
{
  char quick_file[FILELTH], line[7][LINELTH];
  int i, atype, costmult, limit, allrpt = FALSE;
  int traits, lnum, out_lines = 0;
  FILE *tmp_fp = NULL;

  /* find out which type */
  clear_bottom(1);
  mvaddstr(LINES - 2, 0,
	   "Provide help on which army type? [* for all] ");
  clrtoeol();
  atype = get_atype();
  limit = atype + 1;

  /* decode input */
  if (no_input == TRUE) {
    /* quick out */
    return;
  } else if (atype == num_armytypes) {

    /* check for all of the info */
    if (strcmp("*", string) != 0) {
      errormsg("That is not any army type I have heard of");
      return;
    }

    /* initialize the output */
    allrpt = TRUE;
    limit = num_armytypes;
    atype = 0;

    /* open the temporary file and give it a title */
    sprintf(quick_file, "%s.%s", nationname, tmptag);
    if ((tmp_fp = fopen(quick_file, "w")) == NULL) {
      errormsg("Unable to create the temporary help file");
      return;
    }
    fprintf(tmp_fp, "\14\nConquer Army Types\n");
    out_lines = 1;

  }

  /* loop through all of the appropriate types */
  for (; atype < limit; atype++) {

    /* start the record off */
    lnum = 0;

    /* give the name of the unit */
    sprintf(line[lnum++], "Name: %s (%s)",
	    ainfo_list[atype].name,
	    ainfo_list[atype].sname);

    /* show some information about it? */
    if (allrpt == TRUE) {
      sprintf(line[lnum++], "Info: %s",
	      ainfo_list[atype].descript);
      if (!a_isleader(atype)) {
	int cnt = 0, hold;
	sprintf(line[lnum], "Magic Needed:  ");
	for (i = 0; i < MAG_NUMBER; i++) {
	  if ((hold = list_magics(string, i,
				  ainfo_list[atype].pow_need[i])) > 0) {
	    if (cnt > 0) {
	      strcat(line[lnum], ", ");
	    }
	    strcat(line[lnum], string);
	    cnt += hold;
	  }
	}
	if (cnt == 0) {
	  strcat(line[lnum], "[none]");
	}
	lnum++;
      }
    }

    /* display the unit class */
    sprintf(line[lnum], "Class: %s",
	    ainfo_clist[ainfo_list[atype].class]);

    /* display the unit traits */
    traits = 0;
    strcat(line[lnum], "  Traits: ");
    for (i = 0; i < num_atraits; i++) {
      if (ainfo_list[atype].properties & (1L << i)) {
	if (traits > 0) {
	  strcat(line[lnum], ", ");
	}
	traits++;
	strcat(line[lnum], traits_list[i]);
      }
    }
    if (traits == 0) {
      strcat(line[lnum], "[none]");
    }
    lnum++;

    /* describe the cost */
    if (a_isleader(atype)) {

      /* indicate unit strength */
      sprintf(string, "  Base X.P. = %d", ainfo_list[atype].minsth);
      strcat(line[0], string);

      /* is this their leader? */
      if (a_ismagician(atype)) {
	strcat(line[0], "  [magician]");
      } else if ((ntn_ptr != NULL) &&
	  (getminleader(ntn_ptr->class) != atype) &&
	  (getruler(ntn_ptr->class) != atype)) {
	strcat(line[0], "  [foreign leader]");
      }

      /* show repro rates */
      sprintf(line[lnum++], "Birth Rate  : %d%% chance per year",
	      (int) ainfo_list[atype].maint );
      strcpy(line[lnum++], "Support Cost: [none]");

    } else if (a_ismonster(atype)) {

      /* indicate unit strength */
      sprintf(string, "  Combat Str = %d men; Size = %d men",
	      ainfo_list[atype].minsth,
	      ainfo_list[atype].capt_val / 10);
      strcat(line[0], string);

      /* for now, it is not summonable */
      if ((ntn_ptr != NULL) &&
	  ((costmult = utype_mayuse(ntn_ptr, atype)) != 1)) {
	if (costmult) {
	  sprintf(string, "  [%d x costs]", costmult);
	  strcat(line[0], string);
	} else {
	  strcat(line[0], "  [not summonable]");
	}
      }

      /* check the costs */
      sprintf(line[lnum++], "Enlist Cost (per monst) : %ld pts, %ld jewels",
	      ainfo_list[atype].tal_enlist,
	      ainfo_list[atype].ore_enlist);
      sprintf(line[lnum++],
	      "Support Cost (per monst): %ld jewels",
	      ainfo_list[atype].maint);

    } else {

      /* is there a full size? */
      sprintf(string, "  Capt Value = %d.%d",
	      ainfo_list[atype].capt_val / 10,
	      ainfo_list[atype].capt_val % 10);
      strcat(line[0], string);
	      
      if (ainfo_list[atype].minsth != 1) {
	sprintf(string, "; Full Size = %d men",
		ainfo_list[atype].minsth);
	strcat(line[0], string);
      }

      /* can they draft it? */
      if ((ntn_ptr != NULL) &&
	  !utype_mayuse(ntn_ptr, atype)) {
	strcat(line[0], "  [not draftable]");
      }

      /* what is the cost */
      sprintf(line[lnum++],
	      "Enlist Cost (per man) : %ld talons, %ld metals",
	      ainfo_list[atype].tal_enlist,
	      ainfo_list[atype].ore_enlist);
      if (a_freesupply(atype)) {
	strcpy(line[lnum++], "Support Cost (per man): [none]");
      } else {
	sprintf(line[lnum++], "Support Cost (per man): %ld talons",
		ainfo_list[atype].maint);
      }

    }

    /* now show the combat bonuses */
    global_int = ainfo_list[atype].speed;
    sprintf(line[lnum++],
	    "Move Rate: %d.%dx  Attack Bonus: %+d  Defense Bonus: %+d",
	    global_int / 10, (global_int % 10),
	    ainfo_list[atype].att_bonus,
	    ainfo_list[atype].def_bonus);

    /* send out the output */
    if (allrpt == TRUE) {

      /* check the paging */
      if (out_lines + lnum >= LINES - 1) {
	for (; out_lines < LINES; out_lines++) {
	  putc('\n', tmp_fp); 
	}
	out_lines = lnum + 1;
      } else {
	putc('\n', tmp_fp);
	out_lines += lnum + 1;
      }

      /* store the lines into a the temporary file */
      for (i = 0; i < lnum; i++) {
	fprintf(tmp_fp, "%s\n", line[i]);
      }

    } else {

      /* display the lines; wait for input */
      clear_bottom(0);
      for (i = 0; i < lnum; i++) {
	mvaddstr(LINES - (5 - i), 0, line[i]);
      }
      presskey();

    }

  }

  /* show the file */
  if (allrpt == TRUE) {

    /* successful build; now close it */
    fclose(tmp_fp);

    /* now just use the pager to read it */
    page_file(quick_file, FALSE);

    /* can get rid of it */
    unlink(quick_file);

  }

}

/* DO_HELP -- Provide the player with some documentation */
int
do_help PARM_0(void)
{
  FNCI fnc_ptr;
  PARSE_PTR prs_ptr;
  int i, j, l, val, yline, xspot, ch;

  /* clean up */
  clear_bottom(0);

  /* show the static selections */
  mvaddstr(LINES - 5, 0, "Topics:");
  move(LINES - 5, 10);
  hip_string("Army Types, ", 1, TRUE);
  hip_string("Key Binds, ", 1, TRUE);
  hip_string("List Cmnds, ", 1, TRUE);
  hip_string("Tradegoods, ", 1, TRUE);
  hip_string("Version, ", 1, TRUE);

  /* list all of the help files available */
  yline = LINES - 4;
  xspot = 2;
  for (i = 0; i < help_number; i++) {

    /* check positioning */
    if (i != 0) {
      addstr(", ");
      xspot += 2;
    }
    l = strlen(help_files[i]);
    if (xspot + l > COLS - 7) {
      yline++;
      xspot = 2;
    }
    move(yline, xspot);

    /* scam over the name */
    hip_string(help_files[i], 1, TRUE);
    xspot += l + 2;

  }
  if (yline >= LINES - 1) {
    addstr("; ");
    xspot += 2;
  } else {
    yline++;
    xspot = 0;
  }
  mvaddstr(yline, xspot, "[SPACE to exit] Your Choice? ");

  /* obtain the selection */
  refresh();
  switch ((ch = next_char())) {
  case 'A':
  case 'a':
    /* describe some army types */
    ainfo_help();
    break;
  case 'k':
  case 'K':
    /* get help for the key binding */
    mvaddstr(LINES - 1, 0, "Help for which key binding? ");
    clrtoeol();
    refresh();
    if ((fnc_ptr = parse_keys(cq_bindings, TRUE)) == NULL) {
      errormsg("That key sequence is not bound to any function");
    } else {

      /* show it */
      clear_bottom(0);
      mvaddstr(LINES - 4, 0, "Key: ");
      show_str(string, FALSE);

      /* show the function */
      if ((prs_ptr = find_func(fnc_ptr, global_keysys.parse_p,
			       global_keysys.num_parse)) != NULL) {
	mvprintw(LINES - 3, 0, "Function: %s", prs_ptr->realname);
	mvprintw(LINES - 2, 0, "Help Info: %s", prs_ptr->descript);
      } else {
	mvaddstr(LINES - 3, 0, "Function: [unknown]");
      }
      errormsg("");

    }
    break;
  case 'l':
  case 'L':
    /* show all of the commands */
    create_help("The Summary of All Conquer Commands",
		cq_bindings, global_keysys.parse_p, global_keysys.num_parse);
    break;
  case 't':
  case 'T':
    /* show information concerning trade goods */
    clear_bottom(0);
    mvaddstr(LINES - 4, 0, "TG Classes: ");
    xspot = 12;
    yline = LINES - 4;
    for (i = 0; i < tgclass_number; i++) {
      if (i > 0) {
	addstr(", ");
	xspot += 2;
      }
      strcpy(string, tgclass_info[i].name);
      if (xspot + strlen(string) > COLS - 5) {
	xspot = 2;
	yline++;
      }
      mvaddstr(yline, xspot, string);
      xspot += strlen(string);
    }

    /* prompt */
    mvaddstr(LINES - 2, 0, "Help for which Tradegood Class? ");
    clrtoeol();

    /* get the selection then display the help for it */
    global_int = get_tgclass();
    if (no_input == TRUE) break;
    if (global_int == tgclass_number) {
      errormsg("That is not one of the tradegood classes");
      break;
    }

    /* got it, now describe it */
    redraw = DRAW_FULL;
    i = 0;
    clear();
    standout();
    strcpy(string, " TradeGood Class Information ");
    mvaddstr(i, COLS / 2 - strlen(string) / 2, string);
    i += 2;
    standend();

    /* show the class description */
    mvaddstr(i++, 1, "Class:   ");
    standout();
    addstr(tgclass_info[global_int].name);
    standend();
    mvprintw(i++, 1, "Decript: %s",
	     tgclass_info[global_int].description);
    mvprintw(i, 1, "Affect:  %s",
	     tgclass_info[global_int].affect);
    i += 2;

    /* find the total rate of all of the items in the class */
    val = 0;
    for (j = 0; j < tgoods_number; j++) {
      if (tg_info[j].class == global_int) {
	val += tg_info[j].rate;
      }
    }

    /* list all of the tradegoods in the class */
    mvprintw(i++, (COLS - 56) / 2, " %-20s   %5s %7s %10s  %7s",
	     "Tradegood Name", "Value", "% Class", "Need Desg", "Workers" );
    mvprintw(i++, (COLS - 56) / 2, " %-20s   %5s %7s %10s  %7s",
	     "--------------", "-----", "-------", "---------", "-------" );
    for (j = 0; j < tgoods_number; j++) {
      if (tg_info[j].class == global_int) {
	mvprintw(i++, (COLS - 56) / 2,
		 "%-20s    %4d  %3d.%1d%%  %-10.10s    %4d",
		 tg_info[j].name, tg_info[j].value,
		 (tg_info[j].rate * 100) / val,
		 ((tg_info[j].rate * 1000) / val) % 10,
		 maj_dinfo[tg_info[j].need_desg].name,
		 tg_info[j].pop_support);
      }
    }
    presskey();
    break;
  case ' ':
  case '\n':
  case '\r':
    /* quick out */
    return(TRUE);
  default:
    /* first fix the input */
    if (islower(ch)) ch = toupper(ch);

    /* check if it is among the list of help files */
    for (i = 0; i < help_number; i++) {

      /* do they want a help file? */
      if (ch == help_files[i][0]) {

	/* display the correct help file */
#ifndef VMS
	sprintf(string, "%s/%s.doc", helpdir, help_files[i]);
#else
	sprintf(string, "%s%s.doc", helpdir, help_files[i]);
#endif /* VMS */
	page_file(string, FALSE);
	i = help_number + 1;
      }

    }

    /* was it a bad choice? */
    if (i == help_number)
      errormsg("That was not among your possible choices");
    break;
  }
  return(FALSE);
}

/* DO_IGNORE -- Ignore the keystrokes */
int
do_ignore PARM_0(void)
{
  /* just don't do anything */
  return(0);
}

/* CAMP_INFO -- display information about current data file */
int
camp_info PARM_0(void)
{
  int lines = 0, startline;

  /* when returning; draw everything */
  clear();
  redraw = DRAW_FULL;

  /* provide the title screen */
  standout();
  if (LINES > 26) lines++;
  mvaddstr(lines++, COLS / 2 - 16, " CONQUER CAMPAIGN INFORMATION ");
  standend();

  /* global information */
  lines++;
  startline = lines;
  mvprintw(lines++, 1, "World Map Size............. %dx%d", MAPX, MAPY);
  mvprintw(lines++, 1, "World Exposure Ratio....... %d.%d", EXPOSURE / 10,
	   EXPOSURE % 10);
  mvprintw(lines++, 1, "Water Percentage........... %d%%", PWATER);
  mvprintw(lines++, 1, "Non-Flat Land Percentage... %d%%", PMOUNT);
  mvprintw(lines++, 1, "%% Chance of Tradegoods..... %d%%", PTRADE);
  mvprintw(lines++, 1, "   %2d%% = Metals  %2d%% = Jewels", PTGMETAL,
	   PTGJEWEL);
  mvprintw(lines++, 1, "   %2d%% = Magics  %2d%% = Others", PTGSPELL,
	   100 - PTGMETAL - PTGJEWEL - PTGSPELL);
  if (LINES > 25) lines++;
  mvprintw(lines++, 1, "Weighted Revolt Percentage. %d%%", PREVOLT);
  mvprintw(lines++, 1, "Monster Reproduction....... %d%%", PMONSTER);
  mvprintw(lines++, 1, "Mercenary Monster Gain..... %d%%", PMERCMONST);
  mvprintw(lines++, 1, "Chance of Scout Capture.... %d%%", PSCOUT);
  mvprintw(lines++, 1, "Currently Active Nations... %d", MAXNTN);
  mvprintw(lines++, 1, "Land displacement to meet.. %d", MEETNTN);
  if (LINES > 24) lines++;
  mvprintw(lines++, 1, "Combat Dice Used........... %d", NUMDICE);
  mvprintw(lines++, 1, "Navy Combat Bonus.......... %+d", FLEET_CBVAL);
  mvprintw(lines++, 1, "Caravan Combat Bonus....... %+d", WAGON_CBVAL);
  mvprintw(lines++, 1, "Average Combat Damage...... %d%%", AVG_DAMAGE);
  mvprintw(lines++, 1, "Deterrent Damage Value..... %d%%", DAMAGE_LIMIT);
  mvprintw(lines++, 1, "Overmatching Adjustment.... %d%%", OVERMATCH_ADJ);
  mvprintw(lines++, 1, "Minimum Damage Limit....... %d%%", PMINDAMAGE);

  /* optional configurations */
  lines = startline;
  mvprintw(lines++, COLS / 2 + 1, "Current Turn............... %d",
	   TURN - START_TURN + 1);
  mvprintw(lines++, COLS / 2 + 1, "Maximum Diplomacy Adjust... %d",
	   MAXDIPADJ);
  mvprintw(lines++, COLS / 2 + 1, "Relative Coordinates....... %s",
	   world.relative_map ? "True" : "False" );
  mvprintw(lines++, COLS / 2 + 1, "Login Verification......... %s",
	   world.verify_login ? "True" : "False" );
  mvprintw(lines++, COLS / 2 + 1, "Limited Demigod............ %s",
	   world.demilimit ? "True" : "False");
  mvprintw(lines++, COLS / 2 + 1, "Demigod Rebuilds........... %s",
	   (world.demilimit || !world.demibuild) ? "False" : "True");
  mvprintw(lines++, COLS / 2 + 1, "Limited Score Reports...... %s",
	   world.hide_scores ? "True" : "False");
  mvprintw(lines++, COLS / 2 + 1, "Hidden Login Names......... %s",
	   world.hide_login ? "True" : "False");
  mvprintw(lines++, COLS / 2 + 1, "National Build Points...... %d",
	   MAXPTS);
  lines++;
  mvprintw(lines++, COLS / 2 + 1, "Global Mercenary Pool...... %ld",
	   MERCMEN);
  mvprintw(lines++, COLS / 2 + 1, "Mercenary Combat Bonuses... %+d/%+d",
	   MERCATT, MERCDEF);
  mvprintw(lines++, COLS / 2 + 1, "Disbanded Mercenary Rate... %d%%",
	   PDISBAND);
  lines++;
  if (MAXSUPPLIES > 0) {
    mvprintw(lines++, COLS / 2 + 1, "Max Army/Cvn/Navy Supplies. %d/%d/%d",
	     MAXSUPPLIES, MAXSUPPLIES * 2, MAXSUPPLIES * 4);
  } else {
    mvaddstr(lines++, COLS / 2 + 1, "Max Unit Supplies.......... [None]");
  }
  if (is_god == TRUE) {
    mvaddstr(lines++, COLS / 2 + 1, "City Transfer Distance..... ");
    if (CITYXFDIST == NTN_DISTIND) {
      addstr("NTN_COMM");
    } else {
      printw("%d", CITYXFDIST);
    }
    mvaddstr(lines++, COLS / 2 + 1, "Cvn/Navy Support Distance.. ");
    if (NVSPLYDIST == NTN_DISTIND) {
      addstr("NTN_COMM");
    } else {
      printw("%d", NVSPLYDIST);
    }
  } else {
    mvprintw(lines++, COLS / 2 + 1, "City Transfer Distance..... %d",
	     MAX_TRANSFER);
    mvprintw(lines++, COLS / 2 + 1, "Cvn/Navy Support Distance.. %d",
	     ((NVSPLYDIST == NTN_DISTIND)?COMM_I_RANGE:NVSPLYDIST));
  }

  /* user information */
  lines += min(LINES - 23, 3);
  mvprintw(lines++, COLS / 2 + 1, "The Deity: %s", LOGIN);
  if (strcmp(LOGIN, world.demigod) == 0) {
    mvaddstr(lines++, COLS / 2 + 1, "The Demi-God: [none]");
  } else {
    mvprintw(lines++, COLS / 2 + 1, "The Demi-God: %s", world.demigod);
  }

  standout();
  mvaddstr(LINES - 1, COLS / 2 - 13, " Hit Any Key To Continue");
  standend();
  refresh();

  next_char();
  return(0);
}
