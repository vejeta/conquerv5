// SPDX-License-Identifier: GPL-3.0-or-later
/* This file contains miscellaneous functions used in both programs */ 
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
#include "armyX.h"
#include "desigX.h"
#include "racesX.h"
#include "elevegX.h"
#include "nclassX.h"
#include "tgoodsX.h"
#include "displayX.h"

/* STR_TEST -- This function is basically a casefolding strcmp */
int
str_test PARM_2(char *, s1, char *, s2)
{
  int ch1, ch2;

  for (; *s1 != '\0'; s1++, s2++) {
    ch1 = (islower(*s1) ? toupper(*s1) : *s1);
    ch2 = (islower(*s2) ? toupper(*s2) : *s2);
    if (ch1 != ch2) return(ch1 - ch2);
  }
  return(*s1 - *s2);
}

/* STR_NTEST -- This function is basically a casefolding strncmp */
int
str_ntest PARM_3(char *, s1, char *, s2, int, num)
{
  int ch1, ch2, count = 0;

  for (; *s1 != '\0' && count < num; s1++, s2++, count++) {
    ch1 = (islower(*s1) ? toupper(*s1) : *s1);
    ch2 = (islower(*s2) ? toupper(*s2) : *s2);
    if (ch1 != ch2) return(ch1 - ch2);
  }
  if (count == num) return(0);
  return(*s1 - *s2);
}

/* the variable used for a "nonrandom" random value */
long nrand_number;

/* NRAND_SEED -- Set the storage value for the nrand_val() function */
void
nrand_seed PARM_0(void)
{
  int i, digit = 1;

  /* must have already seeded the real randomizor */
  nrand_number = 0;

  /* compute a random value of NRAND_DIGITS */
  for (i = 0; i < NRAND_DIGITS; i++) {
    nrand_number += (rand_val(10) * digit);
    digit *= 10;
  }
}

/* NRAND_VAL -- Adjusted random value to gain the same results each time */
long
nrand_val PARM_2(long, limit, long, adj)
{
  /* overflow here might be desired */
  return((nrand_number + adj) % limit);
}

/* RAND_SEED -- Seed the random number generator */
void
rand_seed PARM_0(void)
{
#ifdef LRAND48
  srand48((long) time ((long *) 0));
#else
#ifdef RANDOM
  srandom((unsigned) time ((long *) 0));
#else
  srand((unsigned) time ((long *) 0));
#endif /* RANDOM */
#endif /* LRAND48 */
}

/* RAND_VAL -- This routine is used for generating a random number
               from 0 to N - 1.                                    */
long
rand_val PARM_1(int, limit)
{
#ifdef LRAND48
  return(lrand48() % limit);
#else
#ifdef RANDOM
  return(random() % limit);
#else
  return((rand() / 102)  % limit);
#endif /* RANDOM */
#endif /* LRAND48 */
}

/* MACH_TIME -- Return a pointer to a string containing the date */
char *
mach_time PARM_0(void)
{
  long timeval;

  /* first get the time */
  timeval = time(0);

  /* now find the string using ctime */
  return(ctime(&timeval));
}

/* MARKOK -- Is the nation mark valid?  If so, return TRUE */
int
markok PARM_3( int, mark, int, racetype, int, showwhy )
{
  NTN_PTR n1_ptr;
  register int i;
  char tmpstr[LINELTH];

  /* only alphabet characters must be used */
  if (!isalpha(mark)) {
    if (showwhy) {
      sprintf(tmpstr, "%c is not an alpha character", mark);
      errormsg(tmpstr);
    }
    return(FALSE);
  }

  /* now make sure it is not used for other displays */
  for (i = 0; i < ELE_NUMBER; i++) {
    if (mark == ele_info[i].symbol) {
      if (showwhy) {
	sprintf(tmpstr, "%c is an elevation character", mark);
	errormsg(tmpstr);
      }
      return(FALSE);
    }
  }
  for (i = 0; i < MAJ_NUMBER; i++) {
    if (mark == maj_dinfo[i].symbol) {
      if (showwhy) {
	sprintf(tmpstr, "%c is a designation character", mark);
	errormsg(tmpstr);
      }
      return(FALSE);
    }
  }
  for (i = 0; i < VEG_NUMBER; i++) {
    if (mark == veg_info[i].symbol) {
      if (showwhy) {
	sprintf(tmpstr, "%c is a vegetation character", mark);
	errormsg(tmpstr);
      }
      return(FALSE);
    }
  }

  /* now check for those already in use */
  for (i = 0; i < MAXNTN; i++)
    if (i != country && ((n1_ptr = world.np[i]) != NULL)) {
      if ((n1_ptr->mark == mark) && (n1_ptr->race == racetype)) {
	if (showwhy) {
	  sprintf(tmpstr, "%c is already in use", mark);
	  errormsg(tmpstr);
	}
	return(FALSE);
      }
    }

  /* now it is okay */
  return(TRUE);
}

/* RAND_TGOOD -- Select a random tradegood of given class of > min value */
int
rand_tgood PARM_2(int, tg_class, int, minval)
{
  static int sum_rates = 0, num_elems = 0;
  static int old_class = -1, old_minval = 0;
  static int *tg_list = NULL;
  int count, randnum, result = TG_NONE;

  /* check if it is initialized */
  if (tg_list == NULL) {
    if ((tg_list = (int *)malloc(sizeof(int) * tgoods_number)) == NULL) {
      errormsg("Serious Error: Memory allocation failure");
      abrt();
    }
  }

  /* keep the information around to speed up function */
  if ((old_class != tg_class) ||
      (old_minval != minval)) {
    /* fix settings */
    sum_rates = 0;
    num_elems = 0;
    old_class = tg_class;
    old_minval = minval;

    /* count 'em up and lay 'em down */
    for (count = 0; count < tgoods_number; count++) {
      /* check it */
      if ((tg_info[count].class == tg_class) &&
	  (tg_info[count].value >= minval)) {
	/* grab it */
	tg_list[num_elems++] = count;
	sum_rates += tg_info[count].rate;
      }
    }

  }

  /* was there anything that matched? */
  if ((num_elems == 0) ||
      (sum_rates == 0)) {
    return(result);
  }

  /* now get it */
  randnum = rand_val(sum_rates);
  for (count = 0; count < num_elems; count++) {
    /* search */
    randnum -= tg_info[tg_list[count]].rate;
    if (randnum <= 0) {
      result = tg_list[count];
      break;
    }
  }

  /* now leave */
  return(result);
}

/* NUM_BITS_ON -- Return the number of active bits in the given long */
int
num_bits_on PARM_1(long, lng_list)
{
  int i, count = 0;

  /* go through it */
  for (i = 0; i < 32; i++) {
    if ((1 << i) & lng_list) {
      count++;
    }
  }
  return(count);
}

/* HL_TARGETS -- Does the highlight method need a target value */
int
hl_targets PARM_1(int, style)
{
  switch (style) {
  case HI_MINDESG:
  case HI_MAJDESG:
  case HI_OWN:
  case HI_TGOODS:
    /* yep */
    return(TRUE);
    /*NOTREACHED*/
    break;
  default:
    /* nope */
    break;
  }
  return(FALSE);
}

/* GIVE_SPELLCASTERS -- Generate spell casters for each nation */
void
give_spellcasters PARM_0(void)
{
  int count, chance = 50;

  /* determined chance for new leaders */
  if (r_magicskill(ntn_ptr->race)) {
    chance *= 110;
    chance /= 100;
  }
  if (r_wizardly(ntn_ptr->race)) {
    chance *= 115;
    chance /= 100;
  }
  if (r_antimagic(ntn_ptr->race)) {
    /* don't do well with magic */
    chance *= 75;
    chance /= 100;
  }
  if (a_castspells(unitbyname(nclass_list[ntn_ptr->class].rulertype))) {
    /* already have enough spell casters */
    chance /= 2;
  }

  /* add a few of them */
  for (count = 0; count < ntn_ptr->tleaders; count++) {
    if (rand_val(100) < chance) {
      if ((army_ptr = crt_army(unitbyname("Magician"))) != NULL) {
	ARMY_XLOC = ntn_ptr->capx;
	ARMY_YLOC = ntn_ptr->capy;
	ARMY_SIZE = ainfo_list[ARMY_TYPE].minsth;
	ARMY_MOVE = 75;
      }
    }
  }
}
