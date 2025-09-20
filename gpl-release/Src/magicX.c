// SPDX-License-Identifier: GPL-3.0-or-later
/* This file handles magic power enhancement and spell results */
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
#include "dataX.h"
#include "magicX.h"
#include "racesX.h"
#include "elevegX.h"

/* MIL_UPPOW -- Go upward with military powers */
static void
mil_uppow PARM_1(long, powerval)
{
  /* increase statistics based on the power */
  switch (powerval) {
  case MM_WARRIOR:
  case MM_CAPTAIN:
  case MM_WARLORD:
    /* add 10% to combat ability */
    ntn_ptr->aplus += 10;
    ntn_ptr->dplus += 10;
    break;
  case MM_ARCHERY:
    /* add 5% to attack and 10% to defense */
    ntn_ptr->aplus += 5;
    ntn_ptr->dplus += 10;
    break;
  case MM_EQUINE:
    /* add 4 to movment */
    ntn_ptr->maxmove += 4;
    break;
  case MM_SAPPER:
    /* add +10% defense */
    ntn_ptr->dplus += 10;
    break;
  case MM_ARMOR:
    /* add +10% defense reduce movement by 2 */
    ntn_ptr->dplus += 10;
    if (ntn_ptr->maxmove > 6) {
      ntn_ptr->maxmove -= 2;
    } else {
      ntn_ptr->maxmove = 4;
    }
    break;
  case MM_AVIAN:
    /* add 4 to movment */
    ntn_ptr->maxmove += 4;
    break;
  case MM_NINJA:
    /* add 5% to attack */
    ntn_ptr->aplus += 5;
    break;
  default:
    /* doesn't enhance nation statistics */
    break;
  }
}

/* CIV_UPPOW -- Increase civilian statistics */
static void
civ_uppow PARM_1(long, powerval)
{
  /* now increase statistics based on the power */
  switch (powerval) {
  case MC_RELIGION:
    /* add 2% to reproduction; overflow goes to attack */
    if (ntn_ptr->repro < race_info[ntn_ptr->race].repro_limit - 1) {
      ntn_ptr->repro += 2;
    } else {
      ntn_ptr->aplus += (2 - (race_info[ntn_ptr->race].repro_limit -
			      ntn_ptr->repro)) * 5;
      ntn_ptr->repro = race_info[ntn_ptr->race].repro_limit;
    }
    break;
  case MC_URBAN:
    /* add 2% to repro; overflow goes to movment */
    if (ntn_ptr->repro < race_info[ntn_ptr->race].repro_limit - 1) {
      ntn_ptr->repro += 2;
    } else {
      ntn_ptr->maxmove += (2 - (race_info[ntn_ptr->race].repro_limit -
				ntn_ptr->repro)) * 2;
      ntn_ptr->repro = race_info[ntn_ptr->race].repro_limit;
    }
    break;
  case MC_BREEDER:
    /* add 2% to repro, subtract 5% from combat; overflow to combat */
    if (ntn_ptr->repro < race_info[ntn_ptr->race].repro_limit - 1) {
      ntn_ptr->repro += 2;
      ntn_ptr->aplus -= 5;
      ntn_ptr->dplus -= 5;
    } else {
      ntn_ptr->aplus -= (2 - (ntn_ptr->repro -
			      race_info[ntn_ptr->race].repro_limit)) * 5;
      ntn_ptr->dplus -= (2 - (ntn_ptr->repro - 
			      race_info[ntn_ptr->race].repro_limit)) * 5;
      ntn_ptr->repro = race_info[ntn_ptr->race].repro_limit;
    }
    break;
  case MC_DEMOCRACY:
    /* improve combat, movement, and repro */
    ntn_ptr->aplus += 10;
    ntn_ptr->dplus += 10;
    ntn_ptr->maxmove += 2;
    if (ntn_ptr->repro < race_info[ntn_ptr->race].repro_limit)
      ntn_ptr->repro++;
    else {
      ntn_ptr->aplus += 2;
      ntn_ptr->dplus += 2;
    }
    break;
  case MC_SOCIALISM:
    /* add 1% to reproduction */
    if (ntn_ptr->repro < race_info[ntn_ptr->race].repro_limit)
      ntn_ptr->repro++;
    else {
      ntn_ptr->aplus += 5;
      ntn_ptr->dplus += 5;
    }
    break;
  case MC_ROADS:
    /* increase movement by 4 */
    ntn_ptr->maxmove += 4;
    break;
  default:
    /* doesn't enhance nation statistics */
    break;
  }
}

/* WIZ_UPPOW -- Increase wizardry statistics */
static void
wiz_uppow PARM_1(long, powerval)
{
  /* now increase statistics based on the power */
  switch (powerval) {
  case MW_ILLUSION:
  case MW_HIDDEN:
  case MW_THEVOID:
    /* increase defense by 5% */
    ntn_ptr->dplus += 5;
    break;
  case MW_SEEALL:
  case MW_VISION:
    /* increase attack by 5% */
    ntn_ptr->aplus += 5;
    break;
  default:
    /* doesn't enhance nation statistics */
    break;
  }
}

/* MIL_DOWNPOW -- Decrease military statistics */
static void
mil_downpow PARM_1(long, powerval)
{
  switch (powerval) {
  case MM_WARRIOR:
  case MM_CAPTAIN:
  case MM_WARLORD:
    /* take away 10% from combat ability */
    ntn_ptr->aplus -= 10;
    ntn_ptr->dplus -= 10;
    break;
  case MM_ARCHERY:
    /* subtract 5% from attack and 10% from defense */
    ntn_ptr->aplus -= 5;
    ntn_ptr->dplus -= 10;
    break;
  case MM_EQUINE:
    /* take away 4 from movment */
    if (ntn_ptr->maxmove > 8) {
      ntn_ptr->maxmove -= 4;
    } else {
      ntn_ptr->maxmove = 4;
    }
    break;
  case MM_SAPPER:
    /* remove 10% from defense */
    ntn_ptr->dplus -= 10;
    break;
  case MM_ARMOR:
    /* remove 10% from defense and increase movement by 2 */
    ntn_ptr->dplus -= 10;
    ntn_ptr->maxmove += 2;
    break;
  case MM_AVIAN:
    /* remove 4 from movment */
    if (ntn_ptr->maxmove > 8) {
      ntn_ptr->maxmove -= 4;
    } else {
      ntn_ptr->maxmove = 4;
    }
    break;
  case MM_NINJA:
    /* remove 5% from attack */
    ntn_ptr->aplus -= 5;
    break;
  default:
    /* doesn't enhance nation statistics */
    break;
  }
}

/* CIV_DOWNPOW -- Decrease civilian statistics */
static void
civ_downpow PARM_1(long, powerval)
{
  /* now decrease statistics based on the power */
  switch (powerval) {
  case MC_RELIGION:
    /* remove 2% from reproduction; underflow comes from attack */
    if (ntn_ptr->repro > 6) {
      ntn_ptr->repro -= 2;
    } else {
      ntn_ptr->aplus -= (7 - ntn_ptr->repro) * 5;
      ntn_ptr->repro = 5;
    }
    break;
  case MC_URBAN:
    /* remove 2% from repro; underflow comes from movment */
    if (ntn_ptr->repro > 6) {
      ntn_ptr->repro -= 2;
    } else {
      ntn_ptr->maxmove -= (7 - ntn_ptr->repro) * 2;
      ntn_ptr->repro = 5;
    }
    break;
  case MC_BREEDER:
    /* remove 2% from repro, add 5% to combat; underflow from combat */
    ntn_ptr->aplus += 5;
    ntn_ptr->dplus += 5;
    if (ntn_ptr->repro > 6) {
      ntn_ptr->repro -= 2;
    } else {
      ntn_ptr->aplus -= (7 - ntn_ptr->repro) * 5;
      ntn_ptr->dplus -= (7 - ntn_ptr->repro) * 5;
      ntn_ptr->repro = 5;
    }
    break;
  case MC_DEMOCRACY:
    /* decrease combat, movement, and repro */
    ntn_ptr->aplus -= 10;
    ntn_ptr->dplus -= 10;
    ntn_ptr->maxmove -= 2;
    if (ntn_ptr->repro > 5) ntn_ptr->repro--;
    else {
      ntn_ptr->aplus -= 2;
      ntn_ptr->dplus -= 2;
    }
    break;
  case MC_SOCIALISM:
    /* subtract 1% from reproduction */
    if (ntn_ptr->repro > 5) ntn_ptr->repro--;
    else {
      ntn_ptr->aplus -= 5;
      ntn_ptr->dplus -= 5;
    }
    break;
  case MC_ROADS:
    /* decrease movement by 4 */
    ntn_ptr->maxmove -= 4;
    break;
  default:
    /* doesn't enhance nation statistics */
    break;
  }
}

/* WIZ_DOWNPOW -- Increase wizardry statistics */
static void
wiz_downpow PARM_1(long, powerval)
{
  /* now decrease statistics based on the power */
  switch (powerval) {
  case MW_ILLUSION:
  case MW_HIDDEN:
  case MW_THEVOID:
    /* decrease defense by 5% */
    ntn_ptr->dplus -= 5;
    break;
  case MW_SEEALL:
  case MW_VISION:
    /* decrease attack by 5% */
    ntn_ptr->aplus -= 5;
    break;
  default:
    /* doesn't enhance nation statistics */
    break;
  }
}

/* ADD_POWERS -- Provide enhancment from list of magic powers */
void
add_powers PARM_2(int, powtype, long, powlist)
{
  int count, maxval;
  long curpow = 1L;

  /* don't bother with invalid input */
  if (ntn_ptr == NULL || powlist == 0L) return;
  if ((powtype < 0) || (powtype >= MAG_NUMBER)) return;

  /* match any powers in the list */
  maxval = mclass_list[powtype].maxval;
  for (count = 0; count <= maxval; count++, curpow = curpow << 1) {

    /* check if the power is already there */
    if (curpow & ntn_ptr->powers[powtype]) continue;

    /* check for the power */
    if (curpow & powlist) {

      /* upgrade things */
      switch (powtype) {
      case MAG_MILITARY:
	mil_uppow(curpow);
	break;
      case MAG_CIVILIAN:
	civ_uppow(curpow);
	break;
      default:
	wiz_uppow(curpow);
	break;
      }

    }
  }

  /* now add in the powers */
  ADDMAGIC(ntn_ptr->powers[powtype], powlist);
}

/* KILL_POWERS -- Remove enhancment from list of magic powers */
void
kill_powers PARM_2(int, powtype, long, powlist)
{
  int count, maxval;
  long curpow = 1L;

  /* don't bother if there is no current nation or no powers */
  if (ntn_ptr == NULL || powlist == 0L) return;
  if ((powtype < 0) || (powtype >= MAG_NUMBER)) return;

  /* match any powers in the list */
  maxval = mclass_list[powtype].maxval;
  for (count = 0; count <= maxval; count++, curpow = curpow << 1) {

    /* check if they have it */
    if (!(ntn_ptr->powers[powtype] & curpow)) continue;

    /* check for the power */
    if (curpow & powlist) {

      /* now decrease statistics based on the power */
      switch (powtype) {
      case MAG_MILITARY:
	mil_downpow(curpow);
	break;
      case MAG_CIVILIAN:
	civ_downpow(curpow);
	break;
      default:
	wiz_downpow(curpow);
	break;
      }

    }
  }

  /* now add in the powers */
  KILLMAGIC(ntn_ptr->powers[powtype], powlist);
}

/* MAGIC_OK -- Is the new magic power possible, for the given class? */
int
magic_ok PARM_2(int, magic_type, int, new_mint)
{
  long new_magic = (1L << new_mint);
  MAGIC_PTR tmp_mgptr = NULL;

  /* check if it is already in place */
  if (MAGIC(ntn_ptr->powers[magic_type], new_magic)) {
    return(FALSE);
  }

  /* check if it is okay by race */
  if (MAGIC(race_info[ntn_ptr->race].pow_limit[magic_type],
	    new_magic)) {
    return(FALSE);
  }

  /* now check all of the necessary prereqs */
  tmp_mgptr = mclass_list[magic_type].pow_list;
  if (MIL_MAGIC(tmp_mgptr[new_mint].pow_need[MAG_MILITARY]) &&
      CIV_MAGIC(tmp_mgptr[new_mint].pow_need[MAG_CIVILIAN]) &&
      WIZ_MAGIC(tmp_mgptr[new_mint].pow_need[MAG_WIZARDRY])) {
    return(TRUE);
  }

  /* oh well, guess not */
  return(FALSE);
}

/* RAND_MAGIC -- Assign a random magical power; return power if successful */
long
rand_magic PARM_1( int, magic_type )
{
  long new_magic = 0L;
  int new_int, number, count = 0, was_set = FALSE;

  /* make note of the maximum value available */
  number = mclass_list[magic_type].maxval;

  /* randomly obtain a new magic power */
  while (was_set == FALSE && count++ < 500) {

    /* generate the number */
    new_int = rand_val(number);
    new_magic = 1L << new_int;

    /* check if it is okay */
    if (magic_ok(magic_type, new_int) == FALSE) {
      continue;
    }

    /* it was added */
    was_set = TRUE;
  }

  /* give back the power */
  if (was_set) return(new_magic);

  /* otherwise, nothing was set */
  return(0L);
}
  
/* MGK_SCTVAL -- Magical combat bonus for a given sector */
int
mgk_sctval PARM_3(NTN_PTR, n1_ptr, int, x, int, y)
{
  int hold = 0;

  /* check the sector for magical bonuses */
  if ((n1_ptr == NULL) ||
      !XY_ONMAP(x, y)) return(hold);

  /* check the magic */
  switch (sct[x][y].vegetation) {
  case VEG_DESERT:
  case VEG_ICE:
    /* dervish have advantage on such land */
    if (MAGIC(n1_ptr->powers[MAG_CIVILIAN], MC_DERVISH)) {
      hold += 20;
    }

    /* so do destroyers */
    if (MAGIC(n1_ptr->powers[MAG_WIZARDRY], MW_DESTROYER)) {
      hold += 20;
    }
    break;
  case VEG_FOREST:
    /* forests provide much help for druids */
    if (MAGIC(n1_ptr->powers[MAG_WIZARDRY], MW_DRUIDISM)) {
      hold += 20;
    }

    /* slight bonus to botanists */
    if (MAGIC(n1_ptr->powers[MAG_CIVILIAN], MC_BOTANY)) {
      hold += 5;
    }
    break;
  case VEG_WOOD:
    /* the trees help the druids */
    if (MAGIC(n1_ptr->powers[MAG_WIZARDRY], MW_DRUIDISM)) {
      hold += 10;
    }

    /* slight bonus to botanists */
    if (MAGIC(n1_ptr->powers[MAG_CIVILIAN], MC_BOTANY)) {
      hold += 2;
    }
    break;
  case VEG_SWAMP:
    /* slight advantage to water power */
    if (MAGIC(n1_ptr->powers[MAG_WIZARDRY], MW_WATER)) {
      hold += 5;
    }
  case VEG_JUNGLE:
    /* advantage in such sectors */
    if (MAGIC(n1_ptr->powers[MAG_CIVILIAN], MC_AMPHIBIAN)) {
      hold += 30;
    }
    break;
  default:
    /* not much worth doing here */
    break;
  }

  /* check the altitude now */
  switch (sct[x][y].altitude) {
  case ELE_WATER:
    /* major bonus for water magicians */
    if (MAGIC(n1_ptr->powers[MAG_WIZARDRY], MW_WATER)) {
      hold += 30;
    }
    /* earth creatures not as good in water */
    if (MAGIC(n1_ptr->powers[MAG_WIZARDRY], MW_EARTH)) {
      hold -= 10;
    }
    /* sailors get a bit and marines get more */
    if (MAGIC(n1_ptr->powers[MAG_CIVILIAN], MC_SAILOR)) {
      hold += 10;
    }
    if (MAGIC(n1_ptr->powers[MAG_CIVILIAN], MC_MARINE)) {
      hold += 20;
    }
    break;
  case ELE_MOUNTAIN:
    /* good bonus for earth people */
    if (MAGIC(n1_ptr->powers[MAG_WIZARDRY], MW_EARTH)) {
      hold += 20;
    }
    break;
  }
  return(hold);
}
