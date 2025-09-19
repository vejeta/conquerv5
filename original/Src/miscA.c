/* This file contains miscellaneous subroutines for use in conqrun */
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
#include "dataA.h"
#include "armyX.h"
#include "cityX.h"
#include "itemX.h"
#include "navyX.h"
#include "butesX.h"
#include "worldX.h"
#include "hlightX.h"
#include "caravanX.h"

/* SORT_NEWS -- Resort the newspaper file */
void
sort_news PARM_1(int, newsturn)
{
  char fname[FILELTH];

  /* call the sorting program to sort the news */
  sprintf(fname, "%s.%03d", newsfile, newsturn - START_TURN);
#ifndef VMS
  sprintf(string, "%s/%s %s %s", progdir, CONQ_SORT, fname, fname);
#else
  sprintf(string, "mcr %s%s %s %s", progdir, CONQ_SORT, fname, fname);
#endif /* VMS */
#ifdef DEBUG
  if (is_update) {
    fprintf(fupdate, "%s\n", string);
  }
#endif /* DEBUG */
  system(string);
}

/* RANDOM_NAME -- Generate a random name given the type */
void
random_name PARM_2( char *, str, int, race )
{
  /*ARGSUSED*/
  register int i;
  int length = rand_val(NAMELTH - 5) + 3;

  /* give it a capital letter to begin */
  str[0] = rand_val(26) + 'A';

  /* now compute the remaining characters */
  for (i = 1; i < length; i++) {
    str[i] = rand_val(27) + '`';
  }
  str[i] = '\0';
}

/* NTN_STATS -- Provide information summary for each nation */
void
ntn_stats PARM_1(NTN_PTR, n1_ptr)
{
  int count, count2, sumint1 = 0, sumint2 = 0, sumint3 = 0;
  long sumlong = 0, numcivs = 0L;

  /* check that it exists */
  if (n1_ptr == NULL) return;

  /* display the nation name */
  fprintf(fupdate, " %-10s %-8s ", n1_ptr->name, n1_ptr->login);

  /* gather army data */
  for (army_ptr = n1_ptr->army_list;
       army_ptr != NULL;
       army_ptr = army_ptr->next) {

    /* count of all army information */
    if (a_isleader(ARMY_TYPE)) {
      sumint1++;
    } else if (a_ismonster(ARMY_TYPE)) {
      sumint2++;
    } else {
      sumint3++;
      sumlong += ARMY_SIZE;
    }

  }
  fprintf(fupdate, "%2d/%2d/%3d ",
	  sumint1, sumint2, sumint3);
  if (sumint3 != 0) {
    fprintf(fupdate, "%4ld", sumlong / sumint3);
  } else {
    fprintf(fupdate, "   0");
  }

  /* now count naval information */
  sumint1 = sumint2 = sumint3 = 0;
  sumlong = 0L;
  for (navy_ptr = n1_ptr->navy_list;
       navy_ptr != NULL;
       navy_ptr = navy_ptr->next) {
    /* count number of ships */
    for (count2 = 0; count2 < NSHP_NUMBER; count2++) {
      for (count = N_LIGHT; count <= N_HEAVY; count++) {
	sumint1 += NAVY_CNTSHIPS(count2, count);
      }
    }

    /* count carried armies */
    if (NAVY_ARMY != EMPTY_HOLD) {
      sumint2++;
    }

    /* make sure to account for people */
    numcivs += navy_holds(navy_ptr, NSHP_MERCHANTS) * NAVY_PEOP;
  }
  if (n1_ptr->num_navy > 0) {
    fprintf(fupdate, "  %2d %3d %2d/%5ld  ",
	    n1_ptr->num_navy, sumint1 / n1_ptr->num_navy,
	    sumint2, numcivs);
  } else {
    fprintf(fupdate, "   0   0  0/    0  ");
  }
  
  /* now count caravan information */
  sumint1 = sumint2 = sumint3 = 0;
  sumlong = 0L;
  for (cvn_ptr = n1_ptr->cvn_list;
       cvn_ptr != NULL;
       cvn_ptr = cvn_ptr->next) {

    /* count size */
    sumint1 += CVN_SIZE;

    /* make sure to account for people */
    sumlong += CVN_SIZE * CVN_PEOP;
  }
  numcivs += sumlong;

  /* display caravan information */
  fprintf(fupdate, "%2d ",
	  n1_ptr->num_cvn);
  if (n1_ptr->num_cvn > 0) {
    fprintf(fupdate, "%3d%5ld ",
	    sumint1 / n1_ptr->num_cvn, sumlong);
  } else {
    fprintf(fupdate, "  0    0 ");
  }

  /* accumulate remainder */
  sumint1 = sumint2 = 0;
  for (city_ptr = n1_ptr->city_list;
       city_ptr != NULL;
       city_ptr = city_ptr->next) {
    sumint1++;
  }
  for (item_ptr = n1_ptr->item_list;
       item_ptr != NULL;
       item_ptr = item_ptr->next) {
    sumint2++;
  }
  fprintf(fupdate, " %3d %3d\n",
	  sumint1, sumint2);

  /* now accumulate the remaining information */
  /* spread_sheet? UNIMPLEMENTED */
}

/* WORLD_STATS -- Display statistics on the world data */
void
world_stats PARM_0(void)
{
  /* show some world statistics */
  fprintf(fupdate, " World Size: %d x %d\n", MAPX, MAPY);
  fprintf(fupdate, " Demigod: %s\n",
	  (strcmp(LOGIN, world.demigod) == 0) ? "[none]": world.demigod);
  fprintf(fupdate, " Number of Nations: %d\n\n", MAXNTN - 1);

  /* display the heading */
  fprintf(fupdate, " %-10s %-8s   Army Units   ", "", "" );
  fprintf(fupdate, "  Navy and Cargo ");
  fprintf(fupdate, " Cvns & Cargo ");
  fprintf(fupdate, "num num\n");
  fprintf(fupdate, " %-10s %-8s ld/mo/nrm  avg", "Nation", "Owner" );
  fprintf(fupdate, "   # avg ar/peopl ");
  fprintf(fupdate, "  # avg peopl ");
  fprintf(fupdate, "Cty Itm\n");
  fprintf(fupdate, " ---------- -------- --------------  ---------------  ");
  fprintf(fupdate, "------------ --- ---\n");

  /* provide summary of all nations */
  for (country = 0; country < MAXNTN; country++) {
    ntn_stats(world.np[country]);
  }
  putc('\n', fupdate);
}

/* MK_SECT -- Set the influence within the sector */
static void
mk_sect PARM_2(int, x, int, y)
{
  VIS_STORE(x, y, TRUE);
}

/* MARK_LEADERS -- Set has_seen list to indicate leader influence */
void
mark_leaders PARM_0(void)
{
  /* initialize storage */
  visibility_data = new_maplong(visibility_data);

  /* hop to it */
  army_tptr = NULL;
  for (army_ptr = ntn_ptr->army_list;
       army_ptr != NULL;
       army_ptr = army_ptr->next) {

    /* skip it if it is the same */
    if ((army_tptr != NULL) &&
	(ARMY_XLOC == ARMYT_XLOC) &&
	(ARMY_YLOC == ARMYT_YLOC)) continue;

    /* spread realm of influence otherwise */
    if (a_isleader(ARMY_TYPE) &&
	XY_INMAP(ARMY_XLOC, ARMY_YLOC)) {

      /* spread it out */
      army_tptr = army_ptr;
      map_loop(ARMY_XLOC, ARMY_YLOC, COMM_I_RANGE, mk_sect);

    }
  }

}

/* DICE_TESTER -- Simply provide a nice test routine for the dice */
void
dice_tester PARM_0(void)
{
  short data_out[101];
  int how_many, outside, highest, lowest;
  int i, j, x, y, dice_done = FALSE;
  double avg_val, std_dev, tmp_dbl;
  long sum;

  /* initialize */
  init_combat_roll();
  how_many = 1000;

  /* routine to test it all */
  while (dice_done == FALSE) {

    /* reset stuff */
    for (i = 0; i < 101; i++) {
      data_out[i] = 0;
    }
    outside = 0;
    highest = 0;
    lowest = 100;
    sum = 0;

    /* roll the dice */
    for (i = 0; i < how_many; i++) {

      /* get the roll */
      j = combat_roll();

      /* fit it in the stats */
      sum += j;
      if ((j < 0) || (j > 100)) {
	outside++;
      } else {
	data_out[j]++;
      }
      if (j < lowest) {
	lowest = j;
      }
      if (j > highest) {
	highest = j;
      }

    }

    /* calculate and redistribute */
    avg_val = ((double) sum) / how_many;
    std_dev = 0.0;
    for (i = 0; i < 101; i++) {

      /* don't need anything if data not here */
      if (data_out[i] == 0) continue;

      /* figure deviation */
      tmp_dbl = ((double) i) - avg_val;
      if (tmp_dbl > 0.0) {
	std_dev += data_out[i] * tmp_dbl;
      } else {
	std_dev -= data_out[i] * tmp_dbl;
      }

      /* realign it */
      if ((i > 0) && (i < 100)) {
	data_out[i / 5] += data_out[i];
	data_out[i] = 0;
      }

    }

    /* now compute the standard deviation */
    std_dev /= how_many;

    /* now show the results */
    move(0, 0);
    clrtobot();
    for (i = 0; i < 20; i++) {

      /* position column */
      x = i * 3 + 4;
      y = LINES - 6;

      /* display column label */
      standout();
      mvprintw(y--, x, "%d", i * 5 + 4);
      mvprintw(y--, x, "%d-", i * 5);
      standend();

      /* now go through and show the proper amounts */
      for (j = 0; j < data_out[i] / 10; j++) {

	/* show the item */
	if (y <= 2) {
	  x++;
	  y = LINES - 8;
	}
	mvaddch(y--, x, '*');

      }

      /* check for trailers */
      if ((data_out[i] % 10) != 0) {
	mvprintw(y--, x, "%d", data_out[i] % 10);
      }

    }

    /* display the statistics */
    y = 5;
    mvprintw(y++, COLS - 16,	"Dice:%10d", NUMDICE);
    mvprintw(y++, COLS - 16,	"Tests:%9d", how_many);
    y++;
    mvprintw(y++, COLS - 16,	"Lowest:%8d", lowest);
    mvprintw(y++, COLS - 16,	"Highest:%7d", highest);
    mvprintw(y++, COLS - 16,	"Average:%7.1f", avg_val);
    mvprintw(y++, COLS - 16,	"Std Dev:%7.1f", std_dev);
    y++;
    mvprintw(y++, COLS - 16,	"100's:%9d", data_out[100]);
    if (outside > 0) {
      standout();
      y++;
      mvprintw(y++, COLS - 16,	"Out of range:%2d", outside);
      standend();
    }

    /* display the commands */
    errorbar("Combat Dice Test", "Hit 'Q' to leave");
    standout();
    mvaddstr(LINES - 4, COLS / 2 - 34,
	     "Hit 'N' to change number of tests; Hit 'D' to change number of dice");
    standend();
    move(LINES - 1, 0);
    refresh();

    /* get the next character */
    switch (next_char()) {
    case 'Q':
    case 'q':
      /* all done */
      dice_done = TRUE;
      break;
    case 'D':
    case 'd':
      /* get a new number of dice to roll */
      bottommsg("Use how many dice for each combat roll? ");
      i = get_number(FALSE);
      if (no_input == TRUE) break;
      if ((i < 0) || (i > 100)) {
	errormsg("You may only roll between 1 and 100 dice in a set");
      } else {
	NUMDICE = i;
	init_combat_roll();
      }
      break;
    case 'N':
    case 'n':
      /* get a new number of tests to run */
      bottommsg("Roll how many sets of dice? ");
      i = get_number(FALSE);
      if (no_input == TRUE) break;
      if (i <= 0) {
	errormsg("You must roll the dice at least once to get statistics!");
      } else {
	how_many = i;
      }
      break;
    default:
      /* do need to do anything, but run it */
      break;
    }
  }
}
