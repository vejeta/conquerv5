// SPDX-License-Identifier: GPL-3.0-or-later
/* This file handles all of the time accessing checks for conquer */
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
 *  Copyright (c) 1991 Tero T Mononen
 *  All rights reserved
 */

#include "dataG.h"

int time_check = TRUE;

#ifdef DO_TIME_CHECK
#include "keyvalsX.h"
#ifdef SYS_SIGNAL
#include <sys/signal.h>
#else
#include <signal.h>
#endif /* SYS_SIGNAL */
#include <sys/time.h>
#include <sys/resource.h>

#include <time.h>

#define TIME_CHECK_INTERVAL	(2 * 60)
#define TIME_UPDATE_WARN	(2 * 60)
#define TIME_CLOSING_WARN	(2 * 60)
#define LINE_LEN		100

#define TIME_HOURS_FILE 	"hours"
#define TIME_CLOSED		(-1)
#define TIME_DEFAULT_PRIORITY 	(4)

typedef enum { UPDATE=0, TIMELIMIT=1, CHECK=2 } alert_t;

typedef enum { SUN=0, MON=1, TUE=2, WED=3, THU=4, FRI=5, SAT=6, DIS=7
	       } daynum_t;

struct jtable_s {
  char 	*dayname;
  daynum_t	daynum;
} jtable[] = { 
  {"SUN", SUN},
  {"MON", MON},
  {"TUE", TUE},
  {"WED", WED},
  {"THU", THU},
  {"FRI", FRI},
  {"SAT", SAT},
  {"DIS", DIS},
  {NULL,  0}
};

/* DAYNUM -- return day number */
static int
daynum PARM_1(char *, day) 
{
  int i = 0;
  for(i = 0; jtable[i].dayname != NULL; i++) {
    if(!strncmp(day, jtable[i].dayname, 3)) {
      return(jtable[i].daynum);
    } 
  }
  return((daynum_t)TIME_CLOSED);
}

static alert_t alertn = CHECK;

/* TC_QUIT -- exit conquer due some error etc */
static void
tc_quit PARM_0 (void)
{
  if (country != -1) {
    close_ntn(country, nationname);
  }
  bye();
  /*NOTREACHED*/
}

/* ALERT -- order SIGALRM after n seconds */
static void
alert PARM_1(long, sec)
{
  struct itimerval value;
  value.it_value.tv_sec = sec;
  value.it_value.tv_usec = 0;
  value.it_interval.tv_sec = sec;
  value.it_interval.tv_usec = 0;
  if (setitimer(ITIMER_REAL, &value, (struct itimerval *)NULL) != 0) {
    perror("Setitimer failed");
    tc_quit();
  }
}

/* UPCASE -- return string converted to uppercase */
static char *
upcase PARM_1(char *, str)
{
  char *p;
  for(p = str; *p; p++)
    if (islower(*p))
      *p = toupper(*p);
  return str;
}

#define SKIP_SPACE(p) do { for(; *p == ' ' || *p == '\t'; p++) \
			     ; \
			     } while(0)
#define SKIP_NONSPACE(p) do { for(; *p != ' ' && *p != '\t' && *p; p++) \
			     ; \
			     } while(0)
static char host[64];


/* PRIORITY -- return new priority or information if game is closed */
static int
priority PARM_0(void)
{
  FILE *in;
  char line[LINE_LEN];
  long clock_val;
  struct tm *t;
  char *p, *day, *values;
  char c1, c2;
  int d;

  clock_val = time(NULL);
  in = fopen(TIME_HOURS_FILE, "r");
  if (in == NULL) {
    return('O');
  }
  t = localtime(&clock_val);
  while (fgets(line, LINE_LEN, in) != NULL) {
    p = line;
    SKIP_SPACE(p);
    day = p;
    SKIP_NONSPACE(p);
    *p++ = '\0';
    d = daynum(upcase(day));

    if (d == DIS) {
      SKIP_SPACE(p);
      values = p;
      if (strncmp(host, values, strlen(host)) == 0) {
	return 'A';
      }
    }
    if (t->tm_wday == d) {
      fclose(in);
      SKIP_SPACE(p);
      values = p;
      if (strlen(values) >= 47)	{
	c1 = values[2 * t->tm_hour];
	c2 = values[2 * t->tm_hour + 1];
	if (c2 != ' ') {
	  if (c2 < '0' || c2 > '5') {
	    bottommsg("Illegal character in update field");
	    return('I');
	  }
	  if (t->tm_min >= 10 * (c2 - '0') &&
	      t->tm_min < 10 * (c2 - '0' + 1)) {
	    return('U');
	  }
	}
	if (index(".NX0123456789", c1) != NULL) {
	  return (c1);
	} else {
	  bottommsg("Illegal character in first field");
	  return('i');
	}
      } else {
	bottommsg("Syntax error in time file");
	return('S');
      }
    }
  }
  return 'X';
}

/* CHECK_PLAY -- Check and set time limits and nice values etc... */
static void
check_play PARM_0(void)
{
  static int openerrcnt = 0;
  int pr;

  switch(pr = priority()) {
  case 'X':
    openerrcnt = 0;
    alertn = TIMELIMIT;
    bottommsg("Game is closing, please quit");
    alert(TIME_CLOSING_WARN);
    break;
  case 'N':
    openerrcnt = 0;
#ifdef SETPRIORITY
    setpriority(PRIO_PROCESS, 0, TIME_DEFAULT_PRIORITY);
#endif /* SETPRIORITY */
    alert(TIME_CHECK_INTERVAL);
    break;
  case 'A':
    if (time_check == TRUE) {
      bottommsg("Game has been disabled in this host");
      tc_quit();
    }
    alert(TIME_CHECK_INTERVAL);
    break;
  case 'i':
  case 'I':
  case 'S':
    break;
  case 'O':
    if (openerrcnt == 0) {
      bottommsg("Error opening time file");
    }
    if (openerrcnt < 3) {
      alert(TIME_CHECK_INTERVAL);
      openerrcnt++;
    }
    break;
  case '.':
    openerrcnt = 0;
    alert(TIME_CHECK_INTERVAL);
    break;
  case 'U':
    openerrcnt = 0;
    alertn = UPDATE;
    bottommsg("Game is starting update soon.");
    alert(TIME_UPDATE_WARN);
    break;
  default:
    openerrcnt = 0;
#ifdef SETPRIORITY
    setpriority(PRIO_PROCESS, 0, (int)(2 * (pr - '0')));
#endif /* SETPRIORITY */
    alert(TIME_CHECK_INTERVAL);
    break;
  }
}

/* DOUPEXIT -- Conquer is starting update soon, exit. */
static void
doupexit PARM_0(void)
{
  if (time_check == FALSE) {
    bottommsg("Update is starting.");
    return;
  }
  bottommsg("Exiting for a update");
  hangup();
  /*NOTREACHED*/
}

/* DOEXIT -- Conquer is closing down exit. */
static void
doexit PARM_0(void)
{
  if (time_check == FALSE) {
    bottommsg("Game is closed down");
    return;
  }
  bottommsg("Game is closed down");
  hangup();
  /*NOTREACHED*/
}

/* ALRM_HANDLER -- signal handler for alarms */
static void
alrm_handler PARM_0(void)
{
  /* reset the alarm, can't worry about failure here */
  signal(SIGALRM, alrm_handler);
  switch(alertn) {
  case UPDATE:
    doupexit();
    break;
  case TIMELIMIT:
    doexit();
    break;
  case CHECK:
    check_play();
    break;
  }
}
#endif /* DO_TIME_CHECK */

/* INIT_TIME_CHECK -- Initialize time check stuff */
void
init_time_check PARM_0(void)
{
#ifdef DO_TIME_CHECK
  if (exists(TIME_HOURS_FILE) == 0) {
    if ((int) signal(SIGALRM, alrm_handler) == -1) {
      bottommsg("Signal failed");
      tc_quit();
    }
    alert(TIME_CHECK_INTERVAL);
  }
#endif /* DO_TIME_CHECK */
}

/* INITIAL_CHECK -- Initial time/host check. */
int
initial_check PARM_0(void)
{
#ifdef DO_TIME_CHECK
  int pr;

  /* is there an hours file in place? */
  if (exists(TIME_HOURS_FILE) != 0) return(0);

  /* perform the check */
  gethostname(host, 64);
  switch(pr = priority()) {
  case 'X':
    bottommsg("Game is closed");
    if (time_check == TRUE) {
      return(1);
    }
    break;
  case 'N':
#ifdef SETPRIORITY
    setpriority(PRIO_PROCESS, 0, TIME_DEFAULT_PRIORITY);
#endif /* SETPRIORITY */
    break;
  case 'A':
    bottommsg("Game is not allowed in this host");
    if (time_check == TRUE) {
      return(-1);
    }
    break;
  case 'i':
  case 'I':
  case 'S':
    break;
  case 'O':
    break;
  case '.':
    break;
  case 'U':
    bottommsg("Update Starting really soon...");
    if (time_check == TRUE) {
      return(1);
    }
    break;
  default:
#ifdef SETPRIORITY
    setpriority(PRIO_PROCESS, 0, (int)(2 * (pr - '0')));
#endif /* SETPRIORITY */
    break;
  }
#endif /* DO_TIME_CHECK */
  return 0;
}
