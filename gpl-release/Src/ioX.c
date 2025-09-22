// SPDX-License-Identifier: GPL-3.0-or-later
/* This file handles most generic input/output commands */
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
#include "dataX.h"
#ifdef SYS_SIGNAL
#include <sys/signal.h>
#else
#include <signal.h>
#endif /* SYS_SIGNAL */
#include "armyX.h"
#include "cityX.h"
#include "navyX.h"
#include "itemX.h"
#include "calenX.h"
#include "desigX.h"
#include "magicX.h"
#include "statusX.h"
#include "stringX.h"
#include "elevegX.h"
#include "spellsX.h"
#include "tgoodsX.h"
#include "optionsX.h"
#include "keyvalsX.h"
#include "caravanX.h"
#include "patchlevel.h"
#ifdef WINCH_HANDLER
#include <sgtty.h>
#endif /* WINCH_HANDLER */
#ifdef ALLOW_EDIT_FORK
#include <sys/wait.h>
#endif /* ALLOW_EDIT_FORK */

/* indicate that a false character was sent */
static int fake_char = FALSE;

/* SEND_DUMMY_CHAR -- sends dummy char so system pretends it had been typed */
#ifdef WINCH_HANDLER
static void
send_dummy_char()
{
  char temp;
  temp = 255;
  if (ioctl(2, TIOCSTI, &temp) == -1) {
    errormsg("TIOCSTI ioctl failed");
  }
  fake_char = TRUE;
  return;
}
#endif /* WINCH_HANDLER */

/* WIN_SIZE_CHANGE -- signal handler for window size change */
void
win_size_change PARM_0(int sig)
{
#ifdef WINCH_HANDLER
  struct winsize w;
  int set_size = FALSE;

  if (ioctl(2, TIOCGWINSZ, &w) == -1) {
    errormsg("TIOCGWINSZ ioctl failed");
    return;
  }
  if (w.ws_col < 80) {
    w.ws_col = 80;
    set_size = TRUE;
  }
  if (w.ws_row < 24) {
    w.ws_row = 24;
    set_size = TRUE;
  }
  if (set_size == TRUE) {
    w.ws_xpixel = 0;
    w.ws_ypixel = 0;
    if (ioctl(2, TIOCSWINSZ, &w) == -1) {
      errormsg("TIOCSWINSZ ioctl failed");
      return;
    }
    errormsg("Screen too small.. please resize again.");
  }
  LINES = w.ws_row;
  COLS = w.ws_col;
  send_dummy_char();
  initscr();
  do_redraw();
  return;
#endif /* WINCH_HANDLER */
}

#ifdef ALLOW_EDIT_FORK
/* COPY_FILE -- function to copy from the first into the second */
static void
copy_file PARM_2(char *, from_file, char *, to_file)
{
  FILE *fp1, *fp2;
  int ch;

  /* open the source file */
  if ((fp1 = fopen(from_file, "r")) == NULL) {
    sprintf(string, "Unable to open file <%s> for reading.",
	    from_file);
    errormsg(string);
    return;
  }

  /* open the target file */
  if ((fp2 = fopen(to_file, "w")) == NULL) {
    sprintf(string, "Unable to open file <%s> for writing.",
	    to_file);
    errormsg(string);
    return;
  }

  /* perform the copy */
  while (!feof(fp1) &&
	 ((ch = getc(fp1)) != EOF)) {
    putc(ch, fp2);
  }

  /* close up */
  fclose(fp1);
  fclose(fp2);
}
#endif /* ALLOW_EDIT_FORK */

/* FORK_EDIT_ON_FILE -- execute an edit on a file copy, then copy in */
void
fork_edit_on_file PARM_2(char *, fname, char *, pstr)
{
  /*ARGSUSED*/
#ifdef ALLOW_EDIT_FORK
  int proc_id, wproc_id;
  char edit_prog[FILELTH], tmp_file_targ[FILELTH];
#ifdef SWITCHID
#ifdef SYSV4
  uid_t uid, euid;
#else
  int uid, euid;
#endif /* SYSV4 */

  uid = getuid();
  euid = geteuid();
#endif /* SWITCHID */

  /* build the name of the temporary file */
  sprintf(tmp_file_targ, TMP_FILE_FMT, fname, getpid());

  /* spawn the editor */
  switch (proc_id = fork()) {
  case -1:
    errormsg("Unable to create an additional process for editing");
    break;
  case 0:
    /* attempt to get rid of any old file */
    (void) unlink(tmp_file_targ);

#ifndef VMS
    /* make sure file is editable by player */
    (void) umask(FULLWRITE_UMASK);
#endif /* VMS */

    /* make a copy of the original */
    if (exists(fname) == 0) {
      /* begin by copying the file over */
      copy_file(fname, tmp_file_targ);
    }

    /* now switch uids */
#ifdef SWITCHID
    if (euid != uid) {
#ifdef SETREUID
      setreuid(euid, uid);
#else /* SETREUID */
      setuid(euid);
#endif /* SETREUID */
    }
#endif /* SWITCHID */

    /* generate the name of the editor process */
    if (getenv(ENV_EDITOR)) {
      strcpy(edit_prog, getenv(ENV_EDITOR));
    } else {
      strcpy(edit_prog, DEFAULT_EDITOR);
    }

    /* now totally become the original person */
#ifdef SWITCHID
    if (euid != uid) {
#ifdef SETREUID
      setreuid(uid, uid);
#endif /* SETREUID */
    }
#endif SWITCHID

    /* change the default directory */
    if (chdir(TMP_DIR)) {
      errormsg("Unable to access temporary directory");
      exit(FAIL);
    }

    /* final message */
    if (pstr != NULL) errormsg(pstr);

    /* for security purposes close up possible open files */
    if (fexe != NULL) {
      fclose(fexe);
    }

    /* spawn the new editor process */
#ifdef SEARCH_PATHENV
    if (execlp(edit_prog, edit_prog, tmp_file_targ, (char *)0) == -1) {
      errormsg("Unable to start the editor process");
      exit(FAIL);
    }
#else
    if (execl(edit_prog, edit_prog, tmp_file_targ, (char *)0) == -1) {
      errormsg("Unable to start the editor process");
      exit(FAIL);
    }
#endif /* SEARCH_PATHENV */
    exit(SUCCESS);
    break;
  default:
    /* this is the parent of a bouncing baby process */
    do {
      sleep(1);
      wproc_id = wait(0);
    } while ((wproc_id != proc_id) &&
	     (wproc_id != -1));

    /* gather up the file, if it exists */
    if (exists(tmp_file_targ) == 0) {

      /* relocate it back into the current directory */
      copy_file(tmp_file_targ, fname);

      /* attempt to get rid of the old file */
      (void) unlink(tmp_file_targ);
    }

    /* be sure that raw mode is properly enabled, if needed */
    if (in_curses) {
      crmode();
      noecho();
    }
    break;
  }
#else
  errormsg("This function is unavailable within this version of conquer");
#endif /* ALLOW_EDIT_FORK */  
}

/* DO_REDRAW -- Alternative method of refreshing the screen */
int
do_redraw PARM_0(void)
{
  /* clean the screen */
  clear();

#ifdef VAXC
  /* stupid VAXC implementation */
  refresh();
#endif /* VAXC */
  return(0);
}

/* local storage for the input queue */
static int inch_count = 0;
static int inch_list[LINELTH];

/* NEXT_CHAR -- Obtain the next character of input */
int
next_char PARM_0(void)
{
  int c;

  /* is there any more input pending? */
  if (inch_count == 0) {
    /* straight input */
    c = getch();
    if (fake_char == TRUE) {
      fake_char = FALSE;
      c = EXT_IGN;
    }
    return(c);
  } else {
    /* input from the stack */
    return(inch_list[--inch_count]);
  }
}

/* PUSH_CHAR -- Store input on the stack */
void
push_char PARM_1(int, ch_in)
{
  if (inch_count == LINELTH) {
    errormsg("SERIOUS ERROR: Input Queue Overflow");
  } else {
    inch_list[inch_count++] = ch_in;
  }
}

/* ERRORBAR -- function to display a highlighted region at screen bottom */
void
errorbar PARM_2( char *, str1, char *, str2)
{
  int i;

  move(LINES - 4, 0);
  standout();
  for (i = 0; i < COLS - 1; i++)
    addch(' ');
  standend();
  mvprintw(LINES - 3, 0, " Conquer %s.%d: %s", VERSION, PATCHLEVEL, str1);
  mvaddstr(LINES - 3, COLS - strlen(str2) - 2, str2);
  move(LINES - 2, 0);
  for (i = 0; i < COLS - 1; i++)
    addch('-');
}

/* PRESSKEY -- Display message, refresh, get key, clear last line */
int
presskey PARM_0(void)
{
  int hold;

  mvaddstr(LINES - 1, COLS - 16, " Press Any Key");
  refresh();
  hold = next_char();
  clear_bottom(1);
  return(hold);
}

/* ERRORMSG -- Display a message and wait for a keystroke */
int
errormsg PARM_1 (char *, str)
{
  /* error message to stderr or screen bottom */
  if (in_curses) {
    mvaddstr(LINES - 1, 0, str);
    clrtoeol();
    beep();
    return(presskey());
  } else {
    fprintf(fupdate, "%s\n", str);
  }
  return(0);
}

/* BOTTOMMSG -- Display a message, without waiting for any keystroke */
void
bottommsg PARM_1 (char *, str)
{
  if (in_curses) {
    mvaddstr(LINES - 1, 0, str);
    clrtoeol();
    refresh();
  } else {
    fprintf(fupdate, "%s\n", str);
  }
}

/* Y_OR_N -- Return TRUE for 'y' or 'Y' character press */
int
y_or_n PARM_0(void)
{
  int ch;

  /* just get it */
  refresh();
  if (((ch = next_char()) == 'y') ||
      (ch == 'Y')) {
    return(TRUE);
  }
  return(FALSE);
}

/* CR_OR_Y -- Return TRUE for 'y' or 'Y' character press */
int
cr_or_y PARM_0(void)
{
  /* just get it */
  refresh();
  switch (next_char()) {
  case 'Y':
  case 'y':
  case ' ':
  case '\n':
  case '\r':
    return(TRUE);
  }
  return(FALSE);
}

/* CQ_INIT -- Initialize the curses display */
void
cq_init PARM_1 (char *, progname)
{
#ifdef TSERVER
  extern void setterm_pas();
  setterm_pas();
#endif /* TSERVER */
#ifdef WINCH_HANDLER
  signal(SIGWINCH, win_size_change);
#endif /* WINCH_HANDLER */
  initscr();
#ifdef VMS
  system("set term/numeric");
#endif /* VMS */

  /* may now check terminal size */
  in_curses = TRUE;
  if (COLS < 80 || LINES < 24) {
    mvprintw(LINES - 2, 0, "%s: terminal should be at least 80x24", progname);
    errormsg("Please try again with a different setup");
    cq_bye(SUCCESS);
    in_curses = FALSE;
  }

  /* set raw mode and turn off echoing */
  crmode();
  noecho();
}

/* CQ_RESET -- Terminate the curses display gracefully */
void
cq_reset PARM_0(void)
{
#ifdef VMS
  extern void resetterm();
#endif /* VMS */
#ifdef WINCH_HANDLER
  signal(SIGWINCH, SIG_IGN);
#endif /* WINCH_HANDLER */

  /* really clear it */
  move(0,0);
  clrtobot();
  refresh();
  clear();
  move(0,0);
  refresh();

  /* clean up the display */
  nocrmode();
  echo();
  endwin();
#ifdef VMS
  resetterm();
#endif /* VMS */
  in_curses = FALSE;
}

/* CQ_BYE -- Call cq_reset() and exit program with supplied status */
void
cq_bye PARM_1 (int, status)
{
  cq_reset();
  exit(status);
}

/* CLEAR_BOTTOM -- Empty out the message area on bottom of screen */
void
clear_bottom PARM_1 (int, l)
{
  if (in_curses == FALSE) return;
  if (l == 0) l = 5;
  for (; l > 0 ; l--) {
    move(LINES - l, 0);
    clrtoeol();
  }
}

/* SHOW_CHAR -- Show a character to the current position */
void
show_char PARM_2(char, ch, int, full)
{
  /* show the character */
  if (!isprint(ch)) {
    addch('^');
    addch(non_cntrl(ch));
  } else {
    if (full) addch('\'');
    addch(ch);
    if (full) addch('\'');
  }

  /* finish */
  if (full) addch('-');
}

/* UNSHOW_CHAR -- Remove a character from the current position */
void
unshow_char PARM_2(char, ch, int, full)
{
  int xpos, ypos;

  /* find the current position */
  getyx(stdscr, ypos, xpos);

  /* remove separating dash */
  if (full == TRUE) {
    move(ypos, --xpos);
    addch(' ');
  }

  /* show the key itself */
  if (!isprint(ch)) {
    move(ypos, --xpos);
    addch(' ');
    move(ypos, --xpos);
    addch(' ');
  } else {
    if (full) {
      move(ypos, --xpos);
      addch(' ');
    }
    move(ypos, --xpos);
    addch(' ');
    if (full) {
      move(ypos, --xpos);
      addch(' ');
    }
  }

  /* get back to where we once belonged */
  move(ypos, xpos);
}

/* SHOW_STR -- Display the entire string at the current location */
void
show_str PARM_2(char *, str, int, full)
{
  char str_data[LINELTH];

  /* show all of the characters in the string */
  form_str(str_data, str, full);
  addstr(str_data);
}

/* GET_NUMBER -- Read an integer; Set no_input for just carriage return */
long
get_number PARM_1(int, allowneg)
{
  long sum=0;
  int ch, negval=FALSE;
  int done=FALSE, count=0, xpos, ypos;

  /* enter input until a return is hit */
  refresh();
  while (!done) {
    ch = next_char();
    if (isdigit(ch) &&
	(sum < BIGINT / 10) &&
	(count < 12)) {
      /* only print numbers to the screen */
      addch(ch);
      refresh();
      sum *= 10L;
      if (count == 0) {
	negval = FALSE;
      }
      count++;
      sum += (long)(ch - '0');
    } else if (allowneg && ch == '-' && count == 0) {
      addch(ch);
      refresh();
      negval = TRUE;
      count++;
      sum = 0L;
    } else if (ch == CNTRL_L || ch == CNTRL_R) {
      /* make sure that redrawing works */
      wrefresh(curscr);
    } else if (ch == EXT_ESC) {
      /* quick out */
      no_input = TRUE;
      return(-1L);
    } else if (ch == CNTRL_U) {
      /* make sure that ^U works */
      getyx(stdscr, ypos, xpos);
      while (count > 0) {
	move(ypos, --xpos);
	addch(' ');
	count--;
      }
      move(ypos, xpos);
      refresh();
      sum = 0L;
    } else if ((ch == '\b' || ch == EXT_DEL) && count) {
      /* only delete what was printed */
      getyx(stdscr, ypos, xpos);
      move(ypos, --xpos);
      addch(' ');
      move(ypos, xpos);
      refresh();
      sum /= 10L;
      count--;
    } else if((ch == '\n') || (ch == '\r')) {
      done = TRUE;
    }
  }

  /* set variable for no input */
  if (count == 0) {
    no_input = TRUE;
    return(0L);
  }
  no_input = FALSE;
  if (negval == TRUE) {
    sum = -sum;
  } else if (sum < 0) {
    sum = 0;
  }
  return( sum );
}

/* GET_DOUBLE -- Read a double; Set no_input for just carriage return */
double
get_double PARM_1(int, allowneg)
{
  double sum=0.0, decimal=1.0;
  int ch, negval=FALSE, lastdigit=0;
  int done=FALSE, count=0, xpos, ypos;

  /* enter input until a return is hit */
  refresh();
  while (!done) {
    ch = next_char();
    if (isdigit(ch) && count < 12) {
      /* only print numbers to the screen */
      addch(ch);
      refresh();
      if (decimal > 2.0) {
	lastdigit = (int)(ch - '0');
	sum += (float) lastdigit / decimal;
	decimal *= 10.0;
      } else {
	sum *= 10.0;
	sum += (double)(ch - '0');
      }
      if (count == 0) {
	negval = FALSE;
      }
      count++;
    } else if (allowneg == TRUE && ch == '-' && count == 0) {
      addch(ch);
      refresh();
      negval = TRUE;
      count++;
      sum = 0L;
    } else if (ch == '.' && decimal < 2.0) {
      addch(ch);
      refresh();
      count++;
      decimal = 10.0;
    } else if (ch == CNTRL_L || ch == CNTRL_R) {
      /* make sure redraw works */
      wrefresh(curscr);
    } else if (ch == CNTRL_U) {
      /* make sure that ^U works */
      getyx(stdscr, ypos, xpos);
      while (count > 0) {
	move(ypos, --xpos);
	addch(' ');
	count--;
      }
      move(ypos, xpos);
      refresh();
      decimal = 1.0;
      sum = 0.0;
    } else if ((ch == '\b' || ch == EXT_DEL) && count) {
      /* only delete what was printed */
      getyx(stdscr, ypos, xpos);
      move(ypos, --xpos);
      addch(' ');
      move(ypos, xpos);
      refresh();
      if (decimal > 20.0) {
	decimal /= 10.0;
	sum -= (float) lastdigit / decimal;
      } else if (decimal > 2.0) {
	decimal = 1.0;
      } else {
	sum /= 10.0;
      }
      count--;
    } else if((ch == '\n') || (ch == '\r')) {
      done = TRUE;
    }
  }
  /* set variable for no input */
  if (count == 0) {
    no_input = TRUE;
    return(0.0);
  }
  no_input = FALSE;
  if (negval == TRUE) {
    sum = -sum;
  } else if (sum < 0.0) {
    sum = 0.0;
  }
  return( sum );
}

/* GET_OPTION -- Enter a possible option string */
int
get_option PARM_0(void)
{
  int i;

  /* first get the string */
  get_string(string, STR_OPTION, OPT_LTH);
  if (no_input == TRUE) return(options_number);

  for (i = 0; i < options_number; i++) {
    if (str_test(opt_list[i], string) == 0) break;
  }
  return(i);
}

/* TEST_COMPLETE -- Set buffer and completion string if completion */
static int
test_complete PARM_4 (char *, s1, char *, s2, char *, bstr, char *, cstr)
{
  int hold=FALSE;

  /* is it a completion? */
  if (completion(s1, s2)) {
    hold = TRUE;

    /* is the first completion? */
    if (bstr[0] == '\0') {
      (void) strcpy(bstr, s2);
      (void) strcpy(cstr, bstr);
    } else {
      /* find the smallest unique portion */
      while ( !completion(cstr, s2) ) {
	cstr[strlen(cstr) - 1] = '\0';
      }

      /* append any matches */
      if (strlen(bstr) + strlen(s2) < BIGLTH - 2) {
	(void) strcat(bstr, " ");
	(void) strcat(bstr, s2);
      }
    }
  }
  return(hold);
}

/* EXTEND_STR -- For certain string types complete unique words */
static int
extend_str PARM_2 (char *, str, int, stype)
{
  int matches = 0, count, start, end;
  int xspot, yspot;
  char bufstr[BIGLTH], cmpstr[BIGLTH];

  /* initialization */
  bufstr[0] = cmpstr[0] = '\0';

  /* perform completion based on type of input requested */
  switch(stype) {
  case STR_XNAME:
    /* add in the "news" name */
    if (str[0] == 'n') {
      if (test_complete(str, "news", bufstr, cmpstr)) {
	matches++;
      }
    }
  case STR_NAME:
    /* compare with all nation names */
    if (str[0] == 'g') {
      if (test_complete(str, "god", bufstr, cmpstr)) {
	matches++;
      }
    }
    for (count = 0; count < MAXNTN; count++) {
      if ((ntn_tptr = world.np[count]) != NULL) {
	if (test_complete(str, ntn_tptr->name, bufstr, cmpstr)) {
	  matches++;
	}
      }
    }
    break;
  case STR_CITY:
    if (country == 0) {
      /* go through all cities in the world */
      start = 0;
      end = MAXNTN;
    } else {
      /* compare with all city names in the current nation */
      start = country;
      end = country + 1;
    }

    for (count = start; count < end; count++) {
      if ((ntn_tptr = world.np[count]) != NULL) {
	/* compare with all city names in this nation */
	for (city_tptr = ntn_tptr->city_list;
	     city_tptr != NULL;
	     city_tptr = city_tptr->next) {
	  if (test_complete(str, city_tptr->name, bufstr, cmpstr)) {
	    matches++;
	  }
	}
      }
    }
    break;
  case STR_ARMY:
    /* compare with army type names */
    for (count = 0; count < num_armytypes; count++) {
      if (test_complete(str, ainfo_list[count].name, bufstr, cmpstr)) {
	matches++;
      }
    }
    break;
  case STR_ACLASS:
    /* compare with army type names */
    for (count = 0; count < num_aclasses; count++) {
      if (test_complete(str, ainfo_clist[count], bufstr, cmpstr)) {
	matches++;
      }
    }
    break;
  case STR_STATUS:
    /* compare with all of the army statuses */
    for (count = 0; count < num_statuses; count++) {
      if (test_complete(str, stat_info[count].name, bufstr, cmpstr)) {
	matches++;
      }
    }
    break;
  case STR_MONTH:
    /* compare with all of the months */
    for (count = 0; count < 12; count++) {
      if (test_complete(str, monthstr[count], bufstr, cmpstr)) {
	matches++;
      }
    }
    break;
  case STR_SPEED:
    /* compare with all of the speeds */
    for (count = SPD_MINIMUM; count < SPD_NUMBER; count++) {
      if (test_complete(str, speedname[count], bufstr, cmpstr)) {
	matches++;
      }
    }
    break;
  case STR_DESG:
    /* check all of the major designations */
    for (count = 0; count < MAJ_NUMBER; count++) {
      if (test_complete(str, maj_dinfo[count].name, bufstr, cmpstr)) {
	matches++;
      }
    }
    break;
  case STR_MINDESG:
    /* check all of the minor designations */
    for (count = 0; count < MIN_NUMBER; count++) {
      if (test_complete(str, min_dinfo[count].name, bufstr, cmpstr)) {
	matches++;
      }
    }
    break;
  case STR_DMODES:
    /* go through the list of display modes */
    for (dmode_tptr = dmode_list;
	 dmode_tptr != NULL;
	 dmode_tptr = dmode_tptr->next) {
      if (test_complete(str, dmode_tptr->d.name, bufstr, cmpstr)) {
	matches++;
      }
    }
    break;
  case STR_ALT:
    /* check all of the elevations */
    for (count = 0; count < ELE_NUMBER; count++) {
      if (test_complete(str, ele_info[count].name, bufstr, cmpstr)) {
	matches++;
      }
    }
    break;
  case STR_VEG:
    /* check all of the vegetations */
    for (count = 0; count < VEG_NUMBER; count++) {
      if (test_complete(str, veg_info[count].name, bufstr, cmpstr)) {
	matches++;
      }
    }
    break;
  case STR_TGOOD:
    /* check all of the tradegoods */
    for (count = 0; count < tgoods_number; count++) {
      if (test_complete(str, tg_info[count].name, bufstr, cmpstr)) {
	matches++;
      }
    }
    break;
  case STR_TGCLASS:
    /* check all of the tradegoods */
    for (count = 0; count < tgclass_number; count++) {
      if (test_complete(str, tgclass_info[count].name, bufstr, cmpstr)) {
	matches++;
      }
    }
    break;
  case STR_OPTION:
    /* check among the list of options */
    for (count = 0; count < options_number; count++) {
      if (test_complete(str, opt_list[count], bufstr, cmpstr)) {
	matches++;
      }
    }
    break;
  case STR_FUNCS:
    /* check among bindable functions */
    for (count = 0; count < global_int; count++) {
      if (test_complete(str, bind_func(count), bufstr, cmpstr)) {
	matches++;
      }
    }
    break;
  default:
    /* something funny here -- oh well */
    break;
  }

  /* check for no completions */
  if (matches == 0) return(FALSE);

  /* extend string as far as possible */
  end = strlen(cmpstr);
  for (count = strlen(str); count <= end; count++) {
    if (cmpstr[count] != '\0') addch(cmpstr[count]);
    str[count] = cmpstr[count];
  }

  /* check for non-unique completion */
  if (matches > 1) {

    /* end at last full completion if the list is too long */
    if (strlen(bufstr) > COLS - 35) {
      for (count = COLS - 35; count > 0;
	   count--) {
	if (bufstr[count] != ' ') break;
      }
      bufstr[count] = '\0';
      strcat(bufstr, " ...");
    }

    /* save the current screen location */
    getyx(stdscr, yspot, xspot);

    /* display list of completions */
    sprintf(cmpstr, "Matches: %s", bufstr);
    if (!isspace(count = errormsg(cmpstr))) {
      push_char(count);
    }

    /* return to the proper screen location */
    move(yspot, xspot);
  }

  refresh();
  return(TRUE);
}

/* BADFILECHAR -- Return TRUE for bad file name characters */
static
int badfilechar PARM_1(int, ch)
{
  if (isalnum(ch) ||
      (ch == '_')) {
    return(FALSE);
  }
  return(TRUE);
}

/* GET_STRING -- Read in a string of characters; Return length < NAMELTH
                 The parameter stype indicates type of input requested.
		 Return -1 and exit for escape key */
int
get_string PARM_3 (char *, str, int, stype, int, max_len)
{
  int i, ch;
  int done=0, count=0, xpos, ypos;

  /* obtain all user input */
  refresh();
  while (!done) {
    ch = next_char();
    if (ch == EXT_ESC) {
      /* check for the quick out */
      str[0] = '\0';
      no_input = TRUE;
      return(-1);
    } else if ((stype != STR_SPACE) &&
	       (ch == '\\')) {
      /* quoted input */
      addch('\\');
      refresh();
      getyx(stdscr, ypos, xpos);
      move(ypos, --xpos);
      ch = next_char();
      if (isprint(ch) &&
	  ((stype != STR_NAME) ||
	   !badfilechar(ch)) &&
	  (ch != '\t')) {
	if (count < max_len) {
	  if ((count == 0) &&
	      ((stype == STR_ARMY) ||
	       (stype == STR_ACLASS) ||
	       ((stype < STR_TGOOD) &&
		(stype >= STR_STATUS)))) {
	    /* first letter always upper case */
	    if (islower(ch)) {
	      ch = toupper(ch);
	    }
	  }
	  addch(ch);
	  str[count++] = ch;
	} else {
	  addch(' ');
	  move(ypos, xpos);
	  beep();
	}
      } else {
	addch(' ');
	move(ypos, xpos);
	beep();
      }
      refresh();
    } else if ((stype != STR_SPACE) &&
	       (ch == ' ' || ch == '?' || ch == '\t')) {
      /* check tabs, question marks and spaces */
      str[count] = '\0';
      if (stype != STR_NORM) {
	if (extend_str(str, stype)) {
	  count = strlen(str);
	} else {
	  getyx(stdscr, ypos, xpos);
	  if (!isspace(ch = errormsg("There are no matches"))) {
	    push_char(ch);
	  }
	  move(ypos, xpos);
	  refresh();
	}
      }
    } else if ((stype == STR_XNAME) &&
	       ((ch == ',') || (ch == '\n') || (ch == '\r'))) {
      /* the end... don't lose the character */
      push_char(ch);
      global_int = TRUE;
      done = TRUE;
    } else if (isprint(ch)) {
      /* only input displayable characters */
      if (count < max_len) {
	if ((count == 0) &&
	    ((stype == STR_ARMY) ||
	     (stype == STR_ACLASS) ||
	     ((stype < STR_TGOOD) &&
	      (stype >= STR_STATUS)))) {
	  /* first letter always upper case */
	  if (islower(ch)) {
	    ch = toupper(ch);
	  }
	}
	addch(ch);
	refresh();
	str[count++] = ch;
      } else {
	beep();
      }
    } else if (ch == CNTRL_U) {
      /* make sure that ^U works */
      getyx(stdscr, ypos, xpos);
      while (count > 0) {
	move(ypos,--xpos);
	addch(' ');
	count--;
      }
      move(ypos, xpos);
      refresh();
    } else if (ch == CNTRL_L || ch == CNTRL_R) {
      wrefresh(curscr);
    } else if ((ch == '\b' || ch == EXT_DEL) && count) {
      /* only delete what was printed */
      getyx(stdscr, ypos, xpos);
      move(ypos, --xpos);
      addch(' ');
      move(ypos, xpos);
      refresh();
      count--;
    } else if ((ch == '\n') || (ch == '\r')) {
      global_int = FALSE;
      for (i = count - 1; i > 0; i--) {
	if (str[i] != ' ') break;
	count--;
      }
      done = TRUE;
    }
  }
  str[count] = '\0';
  if (count == 0) {
    no_input = TRUE;
  } else {
    no_input = FALSE;
  }
  return(count);
}

/* GET_PASS -- Enter a string without echoing; Return length */
int
get_pass PARM_1 (char *, str)
{
  int ch;
  int done = FALSE, count = 0;

  refresh();
  no_input = FALSE;
  while(done == FALSE) {
    ch = next_char();
    if (ch == '\b' || ch == EXT_DEL) {
      /* delete any entered characters */
      if (count > 0) {
	count--;
      }
    } else if ((ch == CNTRL_L) || (ch == CNTRL_R)) {
      wrefresh(curscr);
    } else if (ch == EXT_ESC) {
      str[0] = '\0';
      no_input = TRUE;
      return(-1);
    } else if (ch == CNTRL_U) {
      /* make sure that ^U works */
      count = 0;
    } else if ((ch == '\n') || (ch == '\r')) {
      done = TRUE;
    } else if (ch != '\0') {
      /* add any other character to the string */
      if (count < PASSLTH) {
	/* don't try adding too many */
	str[count] = ch;
      }
      count++;
    }
  }

  /* truncate too long a password and end others properly */
  if (count < PASSLTH) {
    str[count] = '\0';
  } else {
    str[PASSLTH] = '\0';
  }
  if (count == 0) {
    no_input = TRUE;
  }
  return(count);
}

/* GET_ATYPE -- Return an army type interactively;
                   num_armytypes indicates invalid input  */
int
get_atype PARM_0(void)
{
  char buffer[BIGLTH];
  int i;

  /* get the information */
  get_string(buffer, STR_ARMY, 20);
  if (no_input == TRUE) return(num_armytypes);

  /* find the match */
  for (i = 0; i < num_armytypes; i++) {
    if (str_test(buffer, ainfo_list[i].name) == 0) break;
  }

  /* give back the results */
  strcpy(string, buffer);
  return(i);
}

/* GET_ACLASS -- Return an army class; AC_NUMBER on invalid input */
int
get_aclass PARM_0(void)
{
  char buffer[BIGLTH];
  int i;

  /* get the information */
  get_string(buffer, STR_ACLASS, 20);
  if (no_input == TRUE) return(num_aclasses);

  /* find the match */
  for (i = 0; i < num_aclasses; i++) {
    if (str_test(buffer, ainfo_clist[i]) == 0) break;
  }

  /* give back the results */
  strcpy(string, buffer);
  return(i);
}

/* GET_MONTH -- Return a month interactively */
int
get_month PARM_0(void)
{
  char buffer[BIGLTH];
  int i;

  /* get the information */
  get_string(buffer, STR_MONTH, 20);
  if (no_input == TRUE) return(12);

  /* find the match */
  for (i = 0; i < 12; i++) {
    if (str_test(buffer, monthstr[i]) == 0) break;
  }

  /* give back the results */
  return(i);
}

/* GET_SPEED -- Return a speed value interactively */
int
get_speed PARM_0(void)
{
  char buffer[BIGLTH];
  int i;

  /* get the information */
  get_string(buffer, STR_SPEED, 20);
  if (no_input == TRUE) return(SPD_NUMBER);

  /* find the match */
  for (i = SPD_MINIMUM; i < SPD_NUMBER; i++) {
    if (str_test(buffer, speedname[i]) == 0) break;
  }

  /* give back the results */
  return(i);
}

/* GET_STATUS -- Return a status value interactively */
int
get_status PARM_0(void)
{
  char buffer[BIGLTH];
  int i;

  /* get the information */
  get_string(buffer, STR_STATUS, 20);
  if (no_input == TRUE) return(num_statuses);

  /* find the match */
  for (i = 0; i < num_statuses; i++) {
    if (str_test(buffer, stat_info[i].name) == 0) break;
  }

  /* give back the results */
  return(i);
}

/* ONE_CHAR -- get one character which is contained in the given set */
int
one_char PARM_2(char *, set, int, fold)
{
  char emsg[LINELTH];
  int ch, xpos, ypos;

  /* clean up and display selection */
  refresh();
  no_input = FALSE;
  if (set) sprintf(emsg, "Choose one of \"%s\"", set);

  /* must get one of the indicated set */
  for (;;) {

    /* get and check the input */
    ch = next_char();
    if (fold) {
      if (islower(ch)) {
	ch = toupper(ch);
      }
    }
    if ((ch == '\n') ||
	(ch == ' ') ||
	(ch == '\r')) {
      no_input = TRUE;
      break;
    }

    /* is it within the selection list */
    if (!set ||
	(set[0] == '\0') ||
	char_in_str(ch, set)) {
      /* got char in set or no set to limit it with. */
      break;
    }

    /* not in set; complain */
    getyx(stdscr, ypos, xpos);
    if (!isspace(ch = errormsg(emsg))) {
      push_char(ch);
    }
    move(ypos, xpos);
    refresh();
  }

  /* got it, now gone */
  return(ch);
}

/* HIP_STRING -- Highlight a character in parenthesis on true */
void
hip_string PARM_3(char *, str_in, int, keychar, int, bolden)
{
  int i;

  /* place the initial characters */
  for (i = 0; i < keychar - 1; i++) {
    if (str_in[i] == '\0') return;
    if (str_in[i] == '-') {
      addch(' ');
    } else {
      addch(str_in[i]);
    }
  }
  addch('(');
  if (bolden) standout();
  addch(str_in[i++]);
  if (bolden) standend();
  addch(')');
  for (; str_in[i] != '\0'; i++) {
    if (str_in[i] == '-') {
      addch(' ');
    } else {
      addch(str_in[i]);
    }
  }
}
