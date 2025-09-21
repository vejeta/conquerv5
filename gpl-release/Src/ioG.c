// SPDX-License-Identifier: GPL-3.0-or-later
/* input and output routines for user interface */
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
#include "cityX.h"
#include "itemX.h"
#include "navyX.h"
#include "butesX.h"
#include "calenX.h"
#include "desigX.h"
#include "mtrlsX.h"
#include "racesX.h"
#include "worldX.h"
#include "activeX.h"
#include "elevegX.h"
#include "nclassX.h"
#include "spellsX.h"
#include "statusX.h"
#include "stringX.h"
#include "tgoodsX.h"
#include "caravanX.h"
#include "dstatusX.h"
#include "patchlevel.h"

/* CREATE_HELP -- Given the list of functions, and the list of key
                  bindings, build a help list of the functions. */
void
create_help PARM_4(char *, title, KLIST_PTR, kl_ptr,
		   PARSE_PTR, fnc_l, int, lim)
{
  FILE *tmp_fp;
  char keystr[LINELTH], quick_file[FILELTH];
  int cur_fnc, first, out_lines;
  KLIST_PTR klist_ptr;

  /* open the temporary file */
  sprintf(quick_file, "%s.%s", nationname, tmptag);
  if ((tmp_fp = fopen(quick_file, "w")) == NULL) {
    errormsg("Unable to create the temporary help file");
    return;
  }

  /* first, provide the title of the help file */
  fprintf(tmp_fp, "\14\n%s\n", title);
  out_lines = 1;

  /* traverse the entire list of functions */
  for (cur_fnc = 0; cur_fnc < lim; cur_fnc++) {

    /* check the paging */
    if (out_lines + 3 >= LINES - 1) {
      for (; out_lines < LINES; out_lines++) {
	putc('\n', tmp_fp); 
      }
      out_lines = 3;
    } else {
      putc('\n', tmp_fp);
      out_lines += 3;
    }

    /* check the list */
    fprintf(tmp_fp, "  Func: %-20s Keys: ",
	    (fnc_l[cur_fnc]).realname);

    /* now look for all of the bindings */
    first = TRUE;
    for (klist_ptr = kl_ptr;
	 klist_ptr != NULL;
	 klist_ptr = klist_ptr->next) {

      /* get and display any matching keybinding */
      if (klist_ptr->key_data.func == (fnc_l[cur_fnc]).func) {

	/* get and show it */
	if (first == FALSE) {
	  fprintf(tmp_fp, ", ");
	} else {
	  first = FALSE;
	}
	form_str(keystr, klist_ptr->key_data.chlist, FALSE);
	fprintf(tmp_fp, "%s", keystr);

      }

    }

    /* end it */
    if (first == TRUE) {
      fprintf(tmp_fp, "[none]\n");
    } else {
      fprintf(tmp_fp, "\n");
    }

    /* now show the description */
    fprintf(tmp_fp, "  Info: %s\n", (fnc_l[cur_fnc]).descript);

  }

  /* successful build; now close it */
  fclose(tmp_fp);

  /* now just use the pager to read it */
  page_file(quick_file, FALSE);

  /* can get rid of it */
  unlink(quick_file);
}

/* MOTD_DISPLAY -- Show the motd to the screen */
void
motd_display PARM_0(void)
{
  FILE *fp;
  char m_line[FILELTH];
  int count = 0;
  int xloc, yloc = (LINES - MOTDLINES) / 2;

  /* is there an MOTD file? */
  clear();
  if ((fp = fopen(motdfile, "r")) != NULL) {

    /* use the local motd */
    while (count < MOTDLINES) {

      /* read the next line */
      if (fgets(m_line, LINELTH, fp) == NULL) break;

      /* show it */
      xloc = strlen(m_line);
      if (xloc > 0) {
	/* remove the newline character */
	m_line[xloc - 1] = '\0';
      }
      if (xloc >= COLS - 4) {
	xloc = 0;
	m_line[COLS] = '\0';
      } else {
	xloc = (COLS - xloc) / 2 - 2;
      }
      mvaddstr(yloc++, xloc, m_line);

      /* keep on truckin' */
      count++;
    }

    /* finish up */
    fclose(fp);

  } else {

    /* provide a default message */
    for (count = 0; count < MOTDLINES; count++) {
      xloc = (COLS - strlen(dflt_motd[count])) / 2 - 2;
      mvaddstr(yloc++, xloc, dflt_motd[count]);
    }

  }
}

/* GET_COUNTRY -- Return an integer representation of a nation;
                  As a side effect 'ntn_tptr' is set properly.  */
int
get_country PARM_1(int, allinp)
{
  int i, l;

  /* initialize for news/god and invalid input */
  ntn_tptr = NULL;

  /* check for input */
  if (allinp == TRUE) {
    l = get_string(string, STR_XNAME, NAMELTH);
  } else {
    l = get_string(string, STR_NAME, NAMELTH);
  }
  if (l <= 0) {
    no_input = TRUE;
    return(MAXNTN);
  }
  no_input = FALSE;

  /* search through all nation names */
  if ((str_test(string, "god") == 0) ||
      (str_test(string, "unowned") == 0))
    return(UNOWNED);
  if (str_test(string, "news") == 0)
    return(NEWSPAPER);
  for (i = 1; i < MAXNTN; i++)
  if ((ntn_tptr = world.np[i]) != NULL) {
    if (str_test(string, ntn_tptr->name) == 0) {
      return(i);
    }
  }

  /* send out MAXNTN */
  ntn_tptr = NULL;
  return(MAXNTN);
}

/* GET_FUNC -- Obtain a bindable function */
FNCI
get_func PARM_1(KEYSYS_STRUCT, key_info)
{
  extern PARSE_PTR tmp_parsep;
  int count;

  /* check for input */
  global_int = key_info.num_parse;
  tmp_parsep = key_info.parse_p;
  if (get_string(string, STR_FUNCS, LINELTH) <= 0) {
    no_input = TRUE;
    return((FNCI) NULL);
  }
  no_input = FALSE;

  /* now check for the rest */
  for (count = 0; count < key_info.num_parse; count++) {
    if (str_test(key_info.parse_p[count].realname, string) == 0) {
      return(key_info.parse_p[count].func);
    }
  }
  errormsg("That is not among the functions that I know of");
  return((FNCI) NULL);
}

/* ENTER_UNITTYPE -- Select a new army unit */
int
enter_unittype PARM_3(int, style, char *, prompt, char *, emsg)
{
  int xloc, yloc, count, found_one = FALSE;
  int merc_number = 0, upg_class = 0, upg_men = 0;
  int start_num = 0, result = num_armytypes;

  /* initialize some stuff */
  switch (style) {
  case 0:
    merc_number = unitbyname("Mercenaries");
    break;
  case 2:
    global_int = 0;
    if (army_ptr != NULL) {
      global_int = ARMY_TYPE;
      upg_class = ainfo_list[ARMY_TYPE].class;
      upg_men = ARMY_SIZE;
      start_num = ARMY_TYPE + 1;
    }
    break;
  case 4:
  case 5:
    /* find the total spell points available */
    if (army_ptr != NULL) {
      upg_men = ARMY_SPTS;
    }

    /* if case 5 ... now switch to case 4 */
    if (style == 5) {
      upg_men = -upg_men;
      style = 4;
    }
    break;
  default:
    break;
  }

  /* provide the list of choices */
  xloc = 10;
  yloc = LINES - 5;
  for (count = start_num; count < num_armytypes; count++) {

    /* check the input */
    if (a_nodraft(count) &&
	(is_god == FALSE)) continue;

    /* check the type */
    if ((style < 4) &&
	!a_isnormal(count)) continue;

    /* specific blockouts */
    switch (style) {
    case 0:
      if ((a_ismerc(count) &&
	   (count != merc_number)) ||
	  ((is_god == FALSE) &&
	   a_isagent(count)))
	continue;
      break;
    case 1:
      if (!a_ismerc(count)) continue;
      break;
    case 2:
      if (ainfo_list[count].class != upg_class) continue;
      break;
    case 3:
      if (!a_enlistaway(count)) continue;
      break;
    case 4:
      /* must be a monster */
      if (!a_ismonster(count)) continue;
      break;
    default:
      break;
    }

    /* is this possible with the powers */
    if (utype_mayuse(ntn_ptr, count) > 0) {

      /* give the comma */
      if (found_one == TRUE) {
	addstr(", ");
	xloc += 2;
      } else {
	/* can now do some enlisting */
	clear_bottom(0);
	mvaddstr(yloc, 0, "Choices: ");
	found_one = TRUE;
      }

      /* show as a choice */
      if (xloc + strlen(ainfo_list[count].select) > COLS - 3) {
	xloc = 1;
	yloc++;
	move(yloc, xloc);
      }
      hip_string(ainfo_list[count].select, 1,
		 utype_ok(ntn_ptr, city_ptr, count, upg_men, FALSE));
      xloc += strlen(ainfo_list[count].select) + 2;

    }
  }

  /* now check it out */
  if (found_one == FALSE) {
    errormsg(emsg);
    return(result);
  }

  /* now get the selection */
  if (yloc < LINES - 1) {
    mvprintw(++yloc, 0, "%s which type? ", prompt);
  } else if (is_god == TRUE) {
    mvprintw(yloc, xloc, "; %s? ", prompt);
  } else {
    mvprintw(yloc, xloc + 2, "==%s which? ", prompt);
  }
  refresh();
  xloc = next_char();

  /* check for quick exit */
  if ((xloc == ' ') || (xloc == '\n') || (xloc == '\r'))
    return(result);

  /* check validity */
  for (count = start_num; count < num_armytypes; count++) {

    /* allow only valid input */
    if (a_nodraft(count) &&
	(is_god == FALSE)) continue;

    /* check the type */
    if ((style < 3) &&
	!a_isnormal(count)) continue;

    /* specific blockouts */
    switch (style) {
    case 0:
      if ((a_ismerc(count) &&
	   (count != merc_number)) ||
	  ((is_god == FALSE) &&
	   a_isagent(count)))
	continue;
      break;
    case 1:
      if (!a_ismerc(count)) continue;
      break;
    case 2:
      if (ainfo_list[count].class != upg_class) continue;
      break;
    case 3:
      if (!a_enlistaway(count)) continue;
      break;
    case 4:
      /* must be a monster */
      if (!a_ismonster(count)) continue;
      break;
    default:
      break;
    }

    /* does it match? */
    if (xloc == (ainfo_list[count].select)[0]) break;
  }
  if ((result = count) == num_armytypes) {
    errormsg("That was not among the list of choices");
    return(result);
  }
  if (utype_ok(ntn_ptr, city_ptr, result, upg_men, TRUE) == FALSE) {
    return(num_armytypes);
  }
  return(result);
}

/* GET_DMODE -- Enter in a major designation */
DMODE_PTR
get_dmode PARM_1(char *, prompt)
{
  char buffer[BIGLTH];
  int xloc, yloc;

  /* fix the display */
  clear_bottom(0);
  mvaddstr(LINES - 5, 0, "Display Modes:");
  yloc = LINES - 5;
  xloc = 15;

  /* show as many as possible */
  for (dmode_tptr = dmode_list;
       dmode_tptr != NULL;
       dmode_tptr = dmode_tptr->next) {

    /* keep it comin' */
    if ((yloc != LINES - 5) ||
	(xloc != 15)) {
      mvaddstr(yloc, xloc, ", ");
      xloc += 2;
    }

    /* now check it */
    if (xloc + strlen(dmode_tptr->d.name) > COLS - 2) {
      if (yloc == LINES - 3) {
	mvaddstr(yloc, xloc, "...");
	break;
      }
      yloc++;
      xloc = 2;
    }

    /* now place it */
    mvaddstr(yloc, xloc, dmode_tptr->d.name);
    xloc += strlen(dmode_tptr->d.name);

  }

  /* now ask for the input */
  yloc++;
  mvaddstr(yloc, 0, prompt);

  /* get the information */
  dmode_tptr = NULL;
  get_string(buffer, STR_DMODES, DISPLAYLTH);
  strcpy(string, buffer);
  if (no_input == TRUE) return(dmode_tptr);

  /* find the match */
  for (dmode_tptr = dmode_list;
       dmode_tptr != NULL;
       dmode_tptr = dmode_tptr->next) {
    if (str_test(buffer, dmode_tptr->d.name) == 0) break;
  }

  /* give back the results */
  return(dmode_tptr);
}

/* GET_DESIGNATION -- Enter in a major designation */
int
get_designation PARM_0(void)
{
  char buffer[BIGLTH];
  int i;

  /* get the information */
  get_string(buffer, STR_DESG, 20);
  if (no_input == TRUE) return(MAJ_NUMBER);

  /* find the match */
  for (i = 0; i < MAJ_NUMBER; i++) {
    if (str_test(buffer, maj_dinfo[i].name) == 0) break;
  }

  /* give back the results */
  return(i);
}

/* GET_MINDESG -- Enter in a minor designation */
int
get_mindesg PARM_0(void)
{
  char buffer[BIGLTH];
  int i;

  /* get the information */
  get_string(buffer, STR_MINDESG, 20);
  if (no_input == TRUE) return(MIN_NUMBER);

  /* find the match */
  for (i = 0; i < MIN_NUMBER; i++) {
    if (str_test(buffer, min_dinfo[i].name) == 0) break;
  }

  /* give back the results */
  return(i);
}

/* GET_TGCLASS -- Enter in a tradegood class */
int
get_tgclass PARM_0(void)
{
  int i;

  /* get the information */
  get_string(string, STR_TGCLASS, 20);
  if (no_input == TRUE) return(tgclass_number);

  /* find the match */
  for (i = 0; i < tgclass_number; i++) {
    if (str_test(string, tgclass_info[i].name) == 0) break;
  }

  /* give back the results */
  return(i);
}

/* GET_TRADEGOOD -- Enter in a tradegood */
int
get_tradegood PARM_0(void)
{
  char buffer[BIGLTH];
  int i;

  /* get the information */
  get_string(buffer, STR_TGOOD, 20);
  if (no_input == TRUE) return(tgoods_number);

  /* find the match */
  for (i = 0; i < tgoods_number; i++) {
    if (str_test(buffer, tg_info[i].name) == 0) break;
  }

  /* give back the results */
  return(i);
}

/* GET_ALTITUDE -- Enter in an altitude type */
int
get_altitude PARM_0(void)
{
  char buffer[BIGLTH];
  int i;

  /* get the information */
  get_string(buffer, STR_ALT, 20);
  if (no_input == TRUE) return(ELE_NUMBER);

  /* find the match */
  for (i = 0; i < ELE_NUMBER; i++) {
    if (str_test(buffer, ele_info[i].name) == 0) break;
  }

  /* give back the results */
  return(i);
}

/* GET_DIPLOMACY -- Read in the diplomacy status of the nation */
int
get_diplomacy PARM_2(int, oldstatus, int, othstatus)
{
  char buffer[LINELTH];
  int i, start_num = 1, xloc, yloc, choice;

  /* get the information */
  if (is_god == TRUE) {
    start_num = 0;
  }
  yloc = LINES - 3;
  xloc = 0;
  move(yloc, xloc);
  addstr("Select which status: ");
  xloc += 21;
  for (i = start_num; i < dstatus_number; i++) {
    if (i < dstatus_number - 1) {
      sprintf(buffer, "%s, ", dipname[i]);
    } else {
      strcpy(buffer, dipname[i]);
    }
    if (xloc + strlen(buffer) > COLS - 5) {
      yloc++;
      xloc = 2;
    }
    move(yloc, xloc);
    xloc += strlen(buffer) + 2;
    hip_string(buffer, 1, (is_god == TRUE) ||
	       (abs(i - oldstatus) <= MAXDIPADJ) ||
	       ((i >= DIP_NEUTRAL) && (i <= othstatus)));
  }
  addstr("? ");
  refresh();
  choice = next_char();
  if ((choice == ' ') || (choice == '\n') || (choice == '\r')) {
    no_input = TRUE;
    return(dstatus_number);
  }
  no_input = FALSE;
  if (islower(choice)) {
    choice = toupper(choice);
  }

  /* find the match */
  for (i = start_num; i < dstatus_number; i++) {
    if (choice == dipname[i][0]) break;
  }

  /* give back the results */
  return(i);
}

/* GET_VEGETATION -- Enter in a vegetation type */
int
get_vegetation PARM_0(void)
{
  char buffer[BIGLTH];
  int i;

  /* get the information */
  get_string(buffer, STR_VEG, 20);
  if (no_input == TRUE) return(VEG_NUMBER);

  /* find the match */
  for (i = 0; i < VEG_NUMBER; i++) {
    if (str_test(buffer, veg_info[i].name) == 0) break;
  }

  /* give back the results */
  return(i);
}

/* GAUDY_LINEOUT -- Gaudily highlight the nation name
                    if found in output line*/
void
gaudy_lineout PARM_2(int, loc, char *, str)
{
  register int i;
  int j, outlen = 0, out_ch;
  int len = strlen(str), nlen = strlen(nationname);

  /* go to set position */
  move(loc, 0);

  /* go through the entire string */
  for (i = 0; i < len; i++) {

    /* is there a match? */
    out_ch = str[i];
    if (str_ntest(&(str[i]), nationname, nlen) == 0) {
      standout();
      for (j = 0; j < nlen; j++) {
	addch(str[i++]);
	if (++outlen > COLS - 2) break;
      }
      standend();

      /* be sure to add the ending character */
      if ((i < len) && (outlen <= COLS - 2)) {
	addch(str[i]);
	outlen++;
      }
    } else if (out_ch == '\t') {
      do {
	if (outlen > COLS - 2) break;
	addch(' ');
      } while (((++outlen) % pager_tab) != 0);
    } else if (!isprint(out_ch)) {
      addch('^');
      if (++outlen <= COLS - 2) {
	addch(non_cntrl(out_ch));
	outlen++;
      }
    } else {
      addch(out_ch);
      outlen++;
    }
    if (outlen > COLS - 2) break;

  }
}

/* NORMAL_LINEOUT -- Just send the string out to the screen */
void
normal_lineout PARM_2(int, loc, char *, str)
{
  register int i;
  int out_ch, outlen = 0, len = strlen(str);

  /* set the starting position */
  move(loc, 0);

  /* traverse the entire string */
  for (i = 0; i < len; i++) {

    /* check for tabs */
    out_ch = str[i];
    if (out_ch == '\t') {
      do {
	if (outlen > COLS - 2) break;
	addch(' ');
      } while (((++outlen) % pager_tab) != 0);
    } else if (!isprint(out_ch)) {
      addch('^');
      if (++outlen <= COLS - 2) {
	addch(non_cntrl(out_ch));
	outlen++;
      }
    } else {
      addch(out_ch);
      outlen++;
    }
    if (outlen > COLS - 2) break;

  }
}

/* SPAWN_OUT -- This routines allows the player to get a shell will still
                in the conquer interface */
int
spawn_out PARM_0(void)
{
#ifdef SPAWN_OUT
#ifndef VMS
  /* just pretend nothing happened */
  return(0);
#else /* VMS */
  int status;

  /* go the full route */
  clear();
  refresh();
  status = lib$spawn();
  redraw=DRAW_FULL;
  return(0);
#endif /* VMS */
#else
  return(0);
#endif /* SPAWN_OUT */
}

/* SHOW_SCORES -- List all of the nation scores to standard output */
void
show_scores PARM_0(void)
{
  FILE *readfp;
  char tmpstr[FILELTH];
  int i, statval;

  /* inform what campaign is being checked */
  printf(" Campaign: %s\n", datadirname);
  printf(" Demigod: %s\n", (strcmp(world.demigod, LOGIN) == 0) ?
	 "[none]" : world.demigod);
  roman_number( string, YEAR(TURN) );
  printf(" Turn %d: %s of Year %s\n",
	 TURN - START_TURN + 1, PMONTH(TURN),
	 string);

  /* show the time if it is there */
  if ((readfp = fopen(timefile, "r")) != NULL) {
    fgets(string, 50, readfp);
    string[strlen(string) - 1] = '\0';
    printf(" Last Update: %s\n", string);
    fclose(readfp);
  }
  printf("\n");

  /* now display any MOTD */
  if ((readfp = fopen(motdfile, "r")) != NULL) {
    char line[LINELTH], l;
    printf(" Message of the day:\n");
    while (!feof(readfp)) {
      if (fgets(line, LINELTH, readfp) != NULL) {
	l = strlen(line);
	if (l < 79) {
	  l = (79 - l) / 2;
	  for (i = 0; i < l; i++) {
	    putchar(' ');
	  }
	}
	printf(" %s", line);
      }
    }
    printf("\n");
  }

  /* show the header */
  printf(" %-*.*s", NAMELTH + 4, NAMELTH + 4, " Nation (Race)");
  printf("  Owner    Align      Class      Score Treasury Solds  Civs  Sct\n");
  for (i = 0; i < NAMELTH + 4; i++) {
    string[i] = '-';
  }
  string[i] = '\0';
  printf(" %-*s", NAMELTH + 4, string);
  printf(" -------- ------- -------------- ----- -------- ----- ------ ---\n");

  /* go through all of the nations */
  for (country = 1; country < MAXNTN; country++) {

    /* is there a nation to give a score about? */
    if ((ntn_ptr = world.np[country]) != NULL) {

      /* test for the existence of various files */
      sprintf(string, "%s.%s", ntn_ptr->name, exetag);
      sprintf(tmpstr, "%s.%s", ntn_ptr->name, isontag);
      sprintf(conqmail, "%s.%s", ntn_ptr->name, msgtag);
      if (n_ispc(ntn_ptr->active) &&
	  (exists(string) != FALSE)) {
	/* hasn't logged in since update */
	printf("*");
      } else if (check_lock(tmpstr, FALSE) == -1) {
	/* indicate anyone currently logged in */
	printf("!");
      } else {

	/* initialize some information */
	statval = 0;
	(void) execute(country);
	conq_news_status = STMAIL_NONE;
	conq_mail_status = STMAIL_NONE;

	/* determine what needs looking at */
	news_check();
	mbox_check();
	if (n_ispc(ntn_ptr->active) &&
	    (conq_news_status == STMAIL_NEW)) {
	  statval |= 1;
	}
	if (conq_mail_status == STMAIL_NEW) {
	  statval |= 2;
	}

	/* now output results */
	switch (statval) {
	case 3:
	  printf("+");
	  break;
	case 2:
	  printf("-");
	  break;
	case 1:
	  printf("|");
	  break;
	default:
	  printf(" ");
	  break;
	}
      }

      /* show the name, race, nation owner and alignment */
      printf("%-*.*s (%c) ", NAMELTH, NAMELTH, ntn_ptr->name,
	     race_info[ntn_ptr->race].name[0]);
      /* hide the login name if appropriate */
      if (n_ismonster(ntn_ptr->active) ||
	  ((world.hide_login) &&
	   (strcmp(loginname, LOGIN) != 0) &&
	   (!world.demilimit &&
	    (strcmp(loginname, world.demigod) != 0)) &&
	   (strcmp(loginname, ntn_ptr->login) != 0))) {
	printf("-------- ");
      } else {
	printf("%-8.8s ", ntn_ptr->login);
      }
      printf("%-7.7s ", alignment[n_alignment(ntn_ptr->active)]);
      if (n_ismonster(ntn_ptr->active)) {
	strcpy(string, "Monster");
      } else if (n_isnpc(ntn_ptr->active)) {
	strcpy(string, "NPC ");
	strcat(string, nclass_list[ntn_ptr->class].name);
      } else {
	strcpy(string, nclass_list[ntn_ptr->class].name);
      }
      printf("%-14.14s ", string);

      /* now show the rest */
      if (n_ismonster(ntn_ptr->active) ||
	  ((world.hide_scores) &&
	   (strcmp(loginname, LOGIN) != 0) &&
	   (strcmp(loginname, world.demigod) != 0) &&
	   (strcmp(loginname, ntn_ptr->login) != 0))){

	/* blank out the scores */
	printf("  --- ");
	printf("  ------ ");
	printf(" ---- ");
	printf(" ----- ");
	printf(" --");

      } else {

	/* show the scores */
	printf("%5d ", ntn_ptr->score);
	printf("%8.0f ", (double) ntn_ptr->mtrls[MTRLS_TALONS]);
	printf("%5ld ", ntn_ptr->tmil);
	printf("%6ld ", ntn_ptr->tciv);
	printf("%3ld", ntn_ptr->tsctrs);

      }

      /* end the line */
      printf("\n");
      
    }

  }

  /* provide a single blank line at the end */
  printf("\n");
}

/* GET_MTRLS -- Select a single entry from the materials */
int
get_mtrls PARM_1(itemtype *, m1_ptr)
{
  char bufstr[LINELTH];
  int count, inchar;

  for (count = 0; count < MTRLS_NUMBER; count++) {
    if (count < MTRLS_NUMBER - 1) {
      sprintf(bufstr, "%s, ", mtrls_info[count].name);
    } else {
      strcpy(bufstr, mtrls_info[count].name);
    }
    hip_string(bufstr, 1, (m1_ptr == NULL) ||
	       (m1_ptr[count] > 0));
  }
  addstr("? ");

  /* get the input */
  refresh();
  inchar = next_char();
  if ((inchar == ' ') || (inchar == '\r') || (inchar == '\n')) {
    no_input = TRUE;
    return(MTRLS_NUMBER);
  }
  no_input = FALSE;
  if (islower(inchar)) {
    inchar = toupper(inchar);
  }
  for (count = 0; count < MTRLS_NUMBER; count++) {
    if (inchar == mtrls_info[count].name[0]) break;
  }
  return(count);
}

/* COPYSCREEN -- display the copyright notice on the screen
 * THIS SUBROUTINE MAY NOT BE ALTERED, AND THE MESSAGE CONTAINED HEREIN
 * MUST BE SHOWN TO EACH AND EVERY PLAYER, EVERY TIME THEY LOG IN	*/
void
copyscreen PARM_0(void)
{
  FILE *timefp, *fopen();

  standout();
  sprintf(string, "Conquer %s.%d", VERSION, PATCHLEVEL);
  mvprintw(3, COLS / 2 - (strlen(string) + 2) / 2, string);
  standend();
  mvaddstr(5, COLS / 2 - 27,
	   "Copyright (c) 1992 by Edward M Barlow and Adam Bryant");
  mvaddstr(6, COLS / 2 - 21, "Written by Edward M Barlow and Adam Bryant");
  mvaddstr(7, COLS / 2 - 10, "All Rights Reserved");

  /* this section will be altered for distribution release */
  mvaddstr(LINES - 8, COLS / 2 - 19,
	   "This version is for personal use only");
  mvaddstr(LINES - 6, COLS / 2 - 31,
	   "It is expressly forbidden to port this software to any form of");
  mvaddstr(LINES - 5, COLS / 2 - 30,
	   "personal computer or to redistribute this software without");
  mvaddstr(LINES - 4, COLS / 2 - 24,
	   "the permission of Edward Barlow or Adam Bryant");

  /* show the time if it is there */
  if ((timefp = fopen(timefile, "r")) != NULL) {
    fgets(string, 50, timefp);
    mvprintw(LINES - 1, 0, "Last Update: %s", string);
    fclose(timefp);
  }
  mvaddstr(LINES - 1, COLS - 13, "Please Wait");
  refresh();
}

/* DUMP_NTN_INFO -- Dump out nation information in machine readable format */
void
dump_ntn_info PARM_0(void)
{
  int i;

  printf("Campaign: %s\n", datadirname);
  printf("Demigod: %s\n", (strcmp(world.demigod, LOGIN) == 0) ?
	 "[none]" : world.demigod);
  roman_number( string, YEAR(TURN) );
  printf("Turn %d: %s of Year %s\n",
	 TURN - START_TURN + 1, PMONTH(TURN),
	 string);

#define DUMPS(x,y) printf("%s: %s\n", x, ntn_ptr->y)
#define DUMPD(x,y) printf("%s: %d\n", x, (int) (ntn_ptr->y))

  DUMPS("Nation_Name", name);
  DUMPS("Login_Name", login);
  DUMPS("Nation_Leader", leader);
  DUMPD("Reproduction", repro);
  printf("Race: %s\n", race_info[ntn_ptr->race].name);
  printf("Nation_Mark: %c\n", ntn_ptr->mark);
  if (is_god == TRUE) {
    printf("Capital: %d %d\n", ntn_ptr->capx, ntn_ptr->capy);
    printf("Center: %d %d\n", ntn_ptr->centerx, ntn_ptr->centery);
    printf("Edges: %d %d %d %d\n", ntn_ptr->leftedge, ntn_ptr->rightedge,
	   ntn_ptr->topedge, ntn_ptr->bottomedge);
  }
  printf("Class: %s\n", nclass_list[ntn_ptr->class].name);
  DUMPD("Attack_bonus", aplus);
  DUMPD("Defence_bonus", dplus);
  DUMPD("Score", score);
  DUMPD("Maxmovement", maxmove);
  DUMPD("Num_army", num_army);
  DUMPD("Num_navy", num_navy);
  DUMPD("Num_city", num_city);
  DUMPD("Num_item", num_item);
  DUMPD("Num_cvn", num_cvn);
  for (i = 0; i < MTRLS_NUMBER; i++) {
    printf("Materials[%s]: %.0f\n", mtrls_info[i].name,
	   (double)ntn_ptr->mtrls[i]);
  }
  for (i = 0; i < MTRLS_NUMBER; i++) {
    printf("Produced_materials[%s]: %.0f\n", mtrls_info[i].name,
	   (double)ntn_ptr->m_new[i]);
  }
  DUMPD("Total_no_sectors", tsctrs);
  DUMPD("Total_no_unsupported_sectors", tunsctrs);
  DUMPD("Total_no_ships", tships);
  DUMPD("Total_no_wagons", twagons);
  DUMPD("Total_no_monst", tmonst);
  DUMPD("Total_no_leaders", tleaders);
  DUMPD("Total_no_military", tmil);
  DUMPD("Total_no_civilians", tciv);
  for (i = 0; i < BUTE_NUMBER; i++) {
    printf("Attribute[%s]: %d\n", bute_info[i].name,
	   ntn_ptr->attribute[i]);
  }
  
  for (army_ptr = ntn_ptr->army_list; army_ptr != NULL;
       army_ptr = army_ptr->next) {
    if (is_god == TRUE) {
      printf("Army_location[%d]: %d %d\n", ARMY_ID, ARMY_XLOC, ARMY_YLOC);
      printf("Army_last[%d]: %d %d\n", ARMY_ID, ARMY_LASTX, ARMY_LASTY);
      printf("Army_max_eff[%d]: %d\n", ARMY_ID, ARMY_MAXEFF);
    }
    printf("Army_type[%d]: %s (%d)\n", ARMY_ID, ainfo_list[ARMY_TYPE].name,
	   ARMY_TYPE);
    printf("Army_efficiency[%d]: %d\n", ARMY_ID, ARMY_EFF);
    printf("Army_strength[%d]: %d\n", ARMY_ID, ARMY_SIZE);
    printf("Army_umove[%d]: %d\n", ARMY_ID, ARMY_MOVE);
    printf("Army_spellpts[%d]: %d\n", ARMY_ID, ARMY_SPTS);
    printf("Army_supply[%d]: %d\n", ARMY_ID, ARMY_SPLY);
    printf("Army_leader[%d]: %d\n", ARMY_ID, ARMY_LEAD);
    printf("Army_status[%d]: %s\n", ARMY_ID,
	   stat_info[unit_status(ARMY_STAT)].name);
  }
  for (navy_ptr = ntn_ptr->navy_list; navy_ptr != NULL;
       navy_ptr = navy_ptr->next) {
    for(i = 0; i < NSHP_NUMBER; i++)
      {
	printf("Navy_ships[%d][%s]: %d\n", NAVY_ID,
	       ninfo_list[i].name, NAVY_SHIPS[i]);
	printf("Navy_efficiency[%d][%s]: %d\n", NAVY_ID,
	       ninfo_list[i].name, NAVY_EFF[i]);
      }
    if (is_god == TRUE) {
      printf("Navy_location[%d]: %d %d\n", NAVY_ID, NAVY_XLOC, NAVY_YLOC);
      printf("Navy_last[%d]: %d %d\n", NAVY_ID, NAVY_LASTX, NAVY_LASTY);
    }
    printf("Navy_umove[%d]: %d\n", NAVY_ID, NAVY_MOVE);
    printf("Navy_status[%d]: %s\n", NAVY_ID,
	   stat_info[unit_status(NAVY_STAT)].name);
    printf("Navy_crew[%d]: %d\n", NAVY_ID, NAVY_CREW);
    printf("Navy_people[%d]: %d\n", NAVY_ID, NAVY_PEOP);
    printf("Navy_supply[%d]: %d\n", NAVY_ID, NAVY_SPLY);
    printf("Navy_armynum[%d]: %d\n", NAVY_ID, NAVY_ARMY);
    printf("Navy_cvnnum[%d]: %d\n", NAVY_ID, navy_ptr->cvnnum);
    
    for (i = 0; i < MTRLS_NUMBER; i++) {
      printf("Navy_materials[%d][%s]: %.0f\n", NAVY_ID,
	     mtrls_info[i].name, (double)NAVY_MTRLS[i]);
    }
  }
  for (city_ptr = ntn_ptr->city_list; city_ptr != NULL;
       city_ptr = city_ptr->next) {
    if (is_god == TRUE) {
      printf("City_location[%s]: %d %d\n", CITY_NAME, CITY_XLOC, CITY_YLOC);
    }
    printf("City_i_people[%s]: %d\n", CITY_NAME, CITY_PEOPLE);
    printf("City_weight[%s]: %d\n", CITY_NAME, CITY_WEIGHT);
    printf("City_s_talons[%s]: %d\n", CITY_NAME, CITY_STALONS);
    printf("City_fortress[%s]: %d\n", CITY_NAME, CITY_FORT);
    
    for (i = 0; i < MTRLS_NUMBER; i++) {
      printf("City_c_materials[%s][%s]: %.0f\n", CITY_NAME,
	     mtrls_info[i].name, (double)CITY_MTRLS[i]);
    }
    for (i = 0; i < MTRLS_NUMBER; i++) {
      printf("City_I_materials[%s][%s]: %.0f\n", CITY_NAME,
	     mtrls_info[i].name, (double)CITY_IMTRLS[i]);
    }
  }
  
  for(item_ptr = ntn_ptr->item_list; item_ptr != NULL;
      item_ptr = item_ptr->next) {
    if (is_god == TRUE) {
      printf("Item_xloc[%d]: %d\n", ITEM_ID, ITEM_XLOC);
      printf("Item_yloc[%d]: %d\n", ITEM_ID, ITEM_YLOC);
    }
    printf("Item_type[%d]: %d\n", ITEM_ID, ITEM_TYPE);
    printf("Item_army[%d]: %d\n", ITEM_ID, ITEM_ARMY);
    printf("Item_navy[%d]: %d\n", ITEM_ID, ITEM_NAVY);
    printf("Item_cvn[%d]: %d\n", ITEM_ID, ITEM_CVN);
    printf("Item_info[%d]: %ld\n", ITEM_ID, ITEM_INFO);
    printf("Item_men[%d]: %ld\n", ITEM_ID, ITEM_MEN);
    for (i = 0; i < MTRLS_NUMBER; i++) {
      printf("Item_materials[%d][%s]: %.0f\n", ITEM_ID,
	     mtrls_info[i].name, (double)ITEM_MTRLS[i]);
    }
  }
  
  for (cvn_ptr = ntn_ptr->cvn_list; cvn_ptr != NULL;
       cvn_ptr = cvn_ptr->next) {
    if (is_god == TRUE) {
      printf("Cvn_location[%d]: %d %d\n", CVN_ID, CVN_XLOC, CVN_YLOC);
      printf("Cvn_last[%d]: %d %d\n", CVN_ID, CVN_LASTX, CVN_LASTY);
    }
    printf("Cvn_efficiency[%d]: %d\n", CVN_ID, CVN_EFF);
    printf("Cvn_size[%d]: %d\n", CVN_ID, CVN_SIZE);
    printf("Cvn_umove[%d]: %d\n", CVN_ID, CVN_MOVE);
    printf("Cvn_crew[%d]: %d\n", CVN_ID, CVN_CREW);
    printf("Cvn_people[%d]: %d\n", CVN_ID, CVN_PEOP);
    printf("Cvn_supply[%d]: %d\n", CVN_ID, CVN_SPLY);
    
    for (i = 0; i < MTRLS_NUMBER; i++) {
      printf("Cvn_materials[%d][%s]: %.0f\n", CVN_ID,
	     mtrls_info[i].name, (double)CVN_MTRLS[i]);
    }
  }
  
#undef DUMPS
#undef DUMPD
  
  return;
}
