// SPDX-License-Identifier: GPL-3.0-or-later
/* Routines concerning sector adjustment and reading are in this file */
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
#include "cityX.h"
#include "itemX.h"
#include "butesX.h"
#include "calenX.h"
#include "desigX.h"
#include "magicX.h"
#include "mtrlsX.h"
#include "racesX.h"
#include "worldX.h"
#include "elevegX.h"
#include "hlightX.h"
#include "statusX.h"
#include "tgoodsX.h"

/* MAJDESG_COSTS -- Calculate the resource price to build a selection */
ITEM_PTR
majdesg_costs PARM_4(int, which, int, x, int, y, int, verbal)
{
  /*ARGSUSED*/
  ITEM_PTR cost_ptr = NULL;
  SCT_PTR sptr;
  int olddesg, mult, i, j;

  /* input check */
  if (!XY_ONMAP(x, y) ||
      ((sptr = &(sct[x][y])) == NULL) ||
      (which < 0) ||
      (which >= MAJ_NUMBER)) {
    return(cost_ptr);
  }

  /* check the pricing data */
  olddesg = major_desg(sptr->designation);
  mult = maj_dinfo[which].multiplier - maj_dinfo[olddesg].multiplier;
  cost_ptr = new_item();
  cost_ptr->xloc = x;
  cost_ptr->yloc = y;
  cost_ptr->itemid = 0;
  for (i = 0; i < MTRLS_NUMBER; i++) {
    cost_ptr->mtrls[i] = (itemtype) 0;
  }

  /* now compute the costs */
  for (i = 0; i < MTRLS_NUMBER; i++) {
    cost_ptr->mtrls[i] = maj_dinfo[which].mtrls_cost[i];
    if (mult > 0) {
      for (j = 0; j < MIN_NUMBER; j++) {
	if ((MIN_START << j) == MIN_DEVASTATED) continue;
	if (minor_desg(sptr->designation, MIN_START << j) &&
	    !minor_desg(MIN_DEVASTATED | maj_dinfo[which].info_val,
			MIN_START << j)) {
	  cost_ptr->mtrls[i] += mult * min_dinfo[j].mtrls_cost[i];
	}
      }
    }
  }

  /* check the magical cost adjustment */
  mgk_cost_adjust(4, &(cost_ptr->mtrls[0]));
  return(cost_ptr);
}

/* DGOK_OWNCHECK -- Looks for unowned land */
static
void dgok_owncheck PARM_2(int, x, int, y)
{
  if (sct[x][y].altitude != ELE_WATER) {
    global_long++;
    if (sct[x][y].owner != country) {
      global_int = TRUE;
    }
  }
}

/* DESG_OK -- Determine if the major designation is valid for the sector */
int
desg_ok PARM_4(int, x, int, y, int, dtype, int, verbal)
{
  CITY_PTR c1_ptr = NULL;
  SCT_PTR sptr;
  ITEM_PTR cost_ptr;
  int count, okval, olddesg, is_sieged = FALSE;

  /* check the input */
  if (!XY_ONMAP(x, y)) {
    if (verbal) errormsg("ERROR: Invalid parameters to desg_ok");
    return(FALSE);
  }
  sptr = &(sct[x][y]);
  olddesg = major_desg(sptr->designation);
  if ((sptr == NULL) || (dtype < 0) || (dtype > MAJ_CAPITAL)) {
    if (verbal) errormsg("ERROR: Invalid parameters to desg_ok");
    return(FALSE);
  }
  if (minor_desg(sptr->designation, MIN_SIEGED)) {
    is_sieged = TRUE;
  }

  /* check if there is water */
  if (sptr->altitude == ELE_WATER) {
    if (dtype != MAJ_BRIDGE) {
      if (verbal) {
	sprintf(string, "You may not build a %s underwater",
		maj_dinfo[dtype].name);
	errormsg(string);
      }
      return(FALSE);
    }
  } else {
    if (dtype == MAJ_BRIDGE) {
      if (verbal) errormsg("You may only build bridges in water sectors");
      return(FALSE);
    }
  }

  /* check if the designation is the same */
  if (dtype == olddesg) {
    if (verbal) errormsg("That designation is already there!");
    return(FALSE);
  }

  /* if not god then check limitations */
  if (is_god != TRUE) {

    /* check the vegetation */
    if ((sptr->altitude != ELE_WATER) &&
	(dtype != MAJ_NONE) &&
	((dtype < MAJ_CANAL) ||
	 (dtype > MAJ_STOCKADE)) &&
	(tofood(sptr, country) == 0)) {
      if (verbal) errormsg("That sector cannot support your people");
      return(FALSE);
    }

    /* check for fertile and fruitful designations */
    if ((dtype == MAJ_FERTILE) || (dtype == MAJ_FRUITFUL)) {
      if (verbal) errormsg("You must begin by building a Farm");
      return(FALSE);
    }

    /* check for construction going on */
    if (IS_CITY(olddesg) &&
	((c1_ptr = citybyloc(ntn_ptr, x, y)) != NULL) &&
	(c1_ptr->i_people < 0)) {
      if (verbal) errormsg("That sector is currently under construction");
      return(FALSE);
    }

    /* check for not removing materials */
    if (IS_CITY(olddesg) &&
	(c1_ptr != NULL) &&
	!IS_CITY(dtype)) {
      if (c1_ptr->c_mtrls[MTRLS_TALONS] < 0) {
	if (verbal) {
	  errormsg("A debt may not be absolved by destroying the treasury");
	}
	return(FALSE);
      }
      for (count = 0; count < MTRLS_NUMBER; count++) {
	if (c1_ptr->c_mtrls[count] > (itemtype) 0) {
	  if (verbal) {
	    errormsg("Remove all raw materials before destruction");
	  }
	  return(FALSE);
	}
      }
    }

    /* now check other qualifications */
    switch (dtype) {
    case MAJ_BRIDGE:
      /* check neighboring lands */
      if (is_god == FALSE) {
	global_int = FALSE;
	global_long = 0;
	map_loop(x, y, 1, dgok_owncheck);
	if (global_int == TRUE) {
	  if (verbal) {
	    errormsg("You must own all neighboring lands to build a bridge");
	  }
	  return(FALSE);
	}
	if (global_long) {
	  if (verbal) {
	    errormsg("A bridge must have at least one edge on land");
	  }
	  return(FALSE);
	}
      }
      break;
    case MAJ_METALMINE:
      if ((olddesg == MAJ_CITY) &&
	  !minor_desg(sptr->designation, MIN_DEVASTATED)) {
	if (verbal)
	  errormsg("Cities must first be devastated or turned into towns");
	return(FALSE);
      }
      if (!tg_ok(sptr, country) || (metal_value(sptr) == 0)) {
	if (verbal) errormsg("Do you want your people to be unemployed?");
	return(FALSE);
      }
      break;
    case MAJ_JEWELMINE:
      if ((olddesg == MAJ_CITY) &&
	  !minor_desg(sptr->designation, MIN_DEVASTATED)) {
	if (verbal)
	  errormsg("Cities must first be devastated or turned into towns");
	return(FALSE);
      }
      if (!tg_ok(sptr, country) || (jewel_value(sptr) == 0)) {
	if (verbal) errormsg("Do you want your people to be unemployed?");
	return(FALSE);
      }
      break;
    case MAJ_CITY:
      if ((olddesg != MAJ_TOWN) ||
	  !minor_desg(sptr->designation, MIN_FORTIFIED)) {
	if (verbal)
	  errormsg("Only fortified towns may become cities");
	return(FALSE);
      }
      if (sptr->people < 1000) {
	if (verbal)
	  errormsg("This town is not large enough to become a city");
	return(FALSE);
      }
      break;
    case MAJ_TOWN:
      /* must have enough people in the town to build it */
      if ((sptr->people < 500) &&
	  (olddesg != MAJ_CITY)) {
	if (verbal)
	  errormsg("You need 500 people to build a town");
	return(FALSE);
      }
      break;
    case MAJ_CAPITAL:
      /* must have a city */
      if (olddesg != MAJ_CITY) {
	if (verbal)
	  errormsg("Only cities may be used as the national Capital");
	return(FALSE);
      }
      break;
    case MAJ_LUMBERYARD:
      /* can't kill a city */
      if ((olddesg == MAJ_CITY) &&
	  !minor_desg(sptr->designation, MIN_DEVASTATED)) {
	if (verbal)
	  errormsg("Cities must first be devastated or turned into towns");
	return(FALSE);
      }

      /* check the wood value */
      if (towood(sptr, country) == 0) {
	if (verbal) errormsg("Do you want your people to be unemployed?");
	return(FALSE);
      }
      break;
    case MAJ_FARM:
      /* cannot kill a city */
      if ((olddesg == MAJ_CITY) &&
	  !minor_desg(sptr->designation, MIN_DEVASTATED)) {
	if (verbal)
	  errormsg("Cities must first be devastated or turned into towns");
	return(FALSE);
      }

      /* need to be able to farm it */
      if (tofood(sptr, country) < DESFOOD) {
	if (verbal) errormsg("Your people cannot cultivate a farm there");
	return(FALSE);
      }
      break;
    case MAJ_CANAL:
      if ((sptr->altitude > ELE_HILL) ||
	  (sptr->altitude < ELE_CLEAR)) {
	if (verbal) errormsg("The elevation prevents canal construction");
	return(FALSE);
      }
    default:
      if ((olddesg == MAJ_CITY) &&
	  !minor_desg(sptr->designation, MIN_DEVASTATED)) {
	if (verbal)
	  errormsg("Cities must first be devastated or turned into towns");
	return(FALSE);
      }
      break;
    }

    /* check the costs */
    if ((item_tptr = find_resources(x, y, is_sieged)) == NULL) {
      if (verbal) {
	errormsg("There are no available resources within range");
      }
      return(FALSE);
    }

    /* get the costs */
    if ((cost_ptr = majdesg_costs(dtype, x, y, verbal)) == NULL) {
      errormsg("Help me, I am so confused");
      free(item_tptr);
      return(0);
    }

    /* check if it is okay */
    okval = TRUE;
    for (count = 0; count < MTRLS_NUMBER; count++) {
      if (ITEMT_MTRLS[count] < cost_ptr->mtrls[count]) {
	if (verbal) {
	  sprintf(string, "You do not have %.0f %s available\n",
		  (double) cost_ptr->mtrls[count],
		  mtrls_info[count].lname);
	  errormsg(string);
	}
	okval = FALSE;
      }
    }

    /* don't need them anymore */
    free(item_tptr);
    free(cost_ptr);
    if (okval == FALSE) {
      return(FALSE);
    }

  }
  return(TRUE);
}

/* WATER_NEARBY -- Determine if water is in a sector */
static void
water_nearby PARM_2(int, x, int, y)
{
  if (sct[x][y].altitude == ELE_WATER) {
    global_int = TRUE;
  }
}

/* CANAL_NEARBY -- Determine if a canal is in a nearby sector */
static void
canal_nearby PARM_2(int, x, int, y)
{
  if ((sct[x][y].owner == country) &&
      (major_desg(sct[x][y].designation) == MAJ_CANAL)) {
    global_int = TRUE;
  }
}

/* MINDESG_COSTS -- Compute the cost for the minor designation */
ITEM_PTR
mindesg_costs PARM_4(int, which, int, x, int, y, int, verbal)
{
  ITEM_PTR cost_ptr = NULL;
  SCT_PTR sptr;
  int value, majval, mult, i, j;

  /* input check */
  if (!XY_ONMAP(x, y) ||
      ((sptr = &(sct[x][y])) == NULL) ||
      (which < 0) ||
      (which >= MIN_NUMBER)) {
    return(cost_ptr);
  }

  /* check the pricing */
  value = MIN_START << which;
  majval = major_desg(sptr->designation);
  mult = maj_dinfo[majval].multiplier;
  cost_ptr = new_item();
  cost_ptr->xloc = x;
  cost_ptr->yloc = y;
  cost_ptr->itemid = 0;
  for (i = 0; i < MTRLS_NUMBER; i++) {
    cost_ptr->mtrls[i] = (itemtype) 0;
  }

  /* increase the multiplier for harbors */
  if (value == MIN_HARBOR) {
    if (is_god == FALSE) {
      global_int = FALSE;
      map_loop(x, y, 1, water_nearby);
      if (global_int == FALSE) {
	/* dramatically increased cost for land-bound harbors */
	mult *= 4;
      }
    }
  }

  /* check which costs */
  if (((value != MIN_FORTIFIED) &&
       (value != MIN_DEVASTATED)) ||
      (!minor_desg(sptr->designation, value))) {

    /* assign cost */
    for (i = 0; i < MTRLS_NUMBER; i++) {
      cost_ptr->mtrls[i] = mult * min_dinfo[which].mtrls_cost[i];
    }

  } else if (value == MIN_DEVASTATED) {

    /* compute the costs for rebuilding the designation */
    for (i = 0; i < MTRLS_NUMBER; i++) {
      cost_ptr->mtrls[i] = maj_dinfo[majval].mtrls_cost[i];
      for (j = 0; j < MIN_NUMBER; j++) {
	if ((MIN_START << j) == MIN_DEVASTATED) continue;
	if (minor_desg(sptr->designation, MIN_START << j)) {
	  cost_ptr->mtrls[i] += mult * min_dinfo[j].mtrls_cost[i];
	}
      }
      cost_ptr->mtrls[i] /= 2;
    }

  } else if ((city_ptr = citybyloc(ntn_ptr, x, y)) != NULL) {

    /* check current level and cost */
    if (majval == MAJ_CACHE) {
      if (verbal)
	errormsg("Caches cannot contain extensive fortifications");
      free(cost_ptr);
      return(cost_ptr = NULL);
    } else if (CITY_FORT >= MAXFORTVAL) {
      if (verbal)
	errormsg("Your fortifications are as strong as possible");
      free(cost_ptr);
      return(cost_ptr = NULL);
    }
    cost_ptr->mtrls[MTRLS_TALONS] = (CITY_FORT + 1) * 3 * FORTCOST;
    cost_ptr->mtrls[MTRLS_METALS] = (CITY_FORT + 1) * 2 * FORTCOST;
    cost_ptr->mtrls[MTRLS_WOOD] = (CITY_FORT + 1) * 1 * FORTCOST;
    if (CIV_MAGIC(MC_ARCHITECT)) {
      for (i = 0; i < MTRLS_NUMBER; i++) {
	cost_ptr->mtrls[i] *= 80;
	cost_ptr->mtrls[i] /= 100;
      }
    }

  } else {

    /* oops */
    if (verbal) errormsg("Your city seems to have been relocated");
    free(cost_ptr);
    return(cost_ptr = NULL);

  }

  /* find the adjustment costs */
  sct_cost_adjust(0, sptr, &(cost_ptr->mtrls[0]));
  mgk_cost_adjust(0, &(cost_ptr->mtrls[0]));
  return(cost_ptr);
}

/* MINDESG_OK -- Check if a minor designation is valid for a sector */
int
mindesg_ok PARM_4(int, x, int, y, int, mdtype, int, verbal)
{
  CITY_PTR c1_ptr;
  SCT_PTR sptr;
  int i, closed_harbor = FALSE, majdesg, value, is_sieged = FALSE;

  /* check the input */
  if (!XY_ONMAP(x, y)) {
    if (verbal) errormsg("ERROR: Invalid parameters to mindesg_ok()");
    return(FALSE);
  }
  sptr = &(sct[x][y]);
  if ((sptr == NULL) || (mdtype < 0) || (mdtype >= MIN_NUMBER)) {
    if (verbal) errormsg("ERROR: Invalid parameters to mindesg_ok()");
    return(FALSE);
  }
  if (minor_desg(sptr->designation, MIN_SIEGED)) {
    is_sieged = TRUE;
  }
  majdesg = major_desg(sptr->designation);

  /* set the value */
  value = MIN_START << mdtype;
  if ((is_god == FALSE) &&
      (value != MIN_DEVASTATED) &&
      (value < MIN_TRADINGPOST)) {
    if (verbal) errormsg("You may not build that construction here");
    return(FALSE);
  }

  /* check for construction */
  if (is_god == FALSE) {
    if (IS_CITY(majdesg) &&
	((c1_ptr = citybyloc(ntn_ptr, x, y)) != NULL) &&
	(c1_ptr->i_people < 0)) {
      if (verbal) errormsg("That sector is currently under construction");
      return(FALSE);
    }
  }

  /* special checks for harbors */
  if (value == MIN_HARBOR) {

    /* only in stockades or better */
    if (!IS_CITY(majdesg) ||
	(majdesg == MAJ_CACHE)) {
      if (verbal) errormsg("Harbors may only be built in population centers");
      return(FALSE);
    }

    /* check for neighboring water */
    if (is_god == FALSE) {
      global_int = FALSE;
      map_loop(x, y, 1, water_nearby);
      if (global_int == FALSE) {
	closed_harbor = TRUE;
	map_loop(x, y, 1, canal_nearby);
	if (global_int == FALSE) {
	  if (verbal) errormsg("A harbor needs access to water");
	  return(FALSE);
	}
      }
    }

  }

  /* is it possible here? */
  if (minor_desg(maj_dinfo[majdesg].info_val, value)) {
    if (verbal) errormsg("That construction is not possible in this sector");
    return(FALSE);
  }

  /* check for devastation */
  if (value == MIN_DEVASTATED) {
    if (IS_CITY(majdesg) &&
	!minor_desg(sptr->designation, value)) {
      if (verbal) errormsg("City sectors may not be devastated");
      return(FALSE);
    }
  } else {

    /* check seasons for granaries */
    if ((value == MIN_GRANARY) &&
	(MONTH(TURN) > 8)) {
      if (verbal) errormsg("The granary needs to be build before the harvest");
      return(FALSE);
    }

    /* is the construction already built? */
    if (minor_desg(sptr->designation, value) &&
	((value != MIN_FORTIFIED) ||
	 (majdesg == MAJ_CACHE) ||
	 !IS_CITY(sptr->designation))) {
      if (verbal) errormsg("Huh?  We've already got that here.");
      return(FALSE);
    }

  }

  /* now for the price checking */
  if (is_god == FALSE) {

    /* figure out the costs */
    ITEM_PTR cost_ptr;
    int hold;

    /* get the information */
    if ((item_tptr = find_resources(x, y, is_sieged)) == NULL) {
      if (verbal) errormsg("There are no resources available to this sector");
      return(FALSE);
    }

    /* get the costs and check on them */
    if ((cost_ptr = mindesg_costs(mdtype, x, y, verbal)) == NULL) {
      free(item_tptr);
      return(FALSE);
    }

    /* now compare */
    hold = TRUE;
    for (i = 0; i < MTRLS_NUMBER; i++) {
      if (cost_ptr->mtrls[i] > item_tptr->mtrls[i]) {
	if (verbal) {
	  sprintf(string, "You do not have %.0f %s available",
		  (double) cost_ptr->mtrls[i],
		  mtrls_info[i].lname);
	  errormsg(string);
	}
	hold = FALSE;
      }
    }

    /* done... free them */
    free(cost_ptr);
    free(item_tptr);
    return(hold);

  }

  /* everything a_ok */
  return(TRUE);
}

/* FR_ACCUME -- accumulation routine */
static ITEM_PTR
fr_accume PARM_4(ITEM_PTR, i_ptr, itemtype *, stash_ptr, int, xloc, int, yloc)
{
  int i;

  /* check it out */
  if (stash_ptr == NULL) return(i_ptr);

  /* need a new one? */
  if (i_ptr == NULL) {
    i_ptr = new_item();
    i_ptr->xloc = xloc;
    i_ptr->yloc = yloc;
    i_ptr->itemid = 0;
    for (i = 0; i < MTRLS_NUMBER; i++) {
      i_ptr->mtrls[i] = (itemtype) 0;
    }
  }

  /* count it up */
  i_ptr->itemid++;
  for (i = 0; i < MTRLS_NUMBER; i++) {
    if (i == MTRLS_TALONS) {
      i_ptr->mtrls[i] += (stash_ptr[i] +
			  10 * stash_ptr[MTRLS_JEWELS]);
    } else {
      i_ptr->mtrls[i] += stash_ptr[i];
    }
  }

  /* done */
  return(i_ptr);
}

/* FIND_RESOURCES -- Build an item_struct holding all nearby resources */
ITEM_PTR
find_resources PARM_3(int, xloc, int, yloc, int, insect_only)
{
  int range, check_val;
  CITY_PTR c1_ptr;
  NAVY_PTR n1_ptr;
  CVN_PTR v1_ptr;

  /* go through all cities and add in any resources */
  item_tptr = NULL;
  for (c1_ptr = ntn_ptr->city_list;
       c1_ptr != NULL;
       c1_ptr = c1_ptr->next) {

    /* set the range based on the type of "city" */
    if (!XY_INMAP(c1_ptr->xloc, c1_ptr->yloc)) continue;
    if (insect_only) {
      range = 0;
    } else {
      range = r10_region(ntn_ptr, c1_ptr,
			 &(sct[c1_ptr->xloc][c1_ptr->yloc])) / 10;
    }
    if (range == 0) {
      check_val = ((xloc == c1_ptr->xloc) && (yloc == c1_ptr->yloc));
    } else {
      check_val = map_within(c1_ptr->xloc, c1_ptr->yloc, xloc, yloc, range);
    }

    /* now check if it was within the area */
    if (check_val) {

      /* create it if it ain't there */
      item_tptr = fr_accume(item_tptr, &(c1_ptr->c_mtrls[0]), xloc, yloc);

    }
  }

  /* search for neighboring navies */
  for (n1_ptr = ntn_ptr->navy_list;
       n1_ptr != NULL;
       n1_ptr = n1_ptr->next) {

    /* check supplying status */
    if (!XY_INMAP(n1_ptr->xloc, n1_ptr->yloc)) continue;
    if (!supply_stat(n1_ptr->status)) continue;

    /* check the range */
    if (insect_only) {
      range = 0;
    } else {
      if (NVSPLYDIST == NTN_DISTIND) {
	range = COMM_I_RANGE;
      } else {
	range = NVSPLYDIST;
      }
    }
    if (range == 0) {
      check_val = ((xloc == n1_ptr->xloc) && (yloc == n1_ptr->yloc));
    } else {
      check_val = map_within(n1_ptr->xloc, n1_ptr->yloc, xloc, yloc, range);
    }

    /* now check if it was within the area */
    if ((check_val) &&
	((sct[xloc][yloc].altitude == ELE_WATER) ==
	 (sct[n1_ptr->xloc][n1_ptr->yloc].altitude == ELE_WATER))) {

      /* create it if it ain't there */
      item_tptr = fr_accume(item_tptr, &(n1_ptr->mtrls[0]), xloc, yloc);

    }
  }

  /* search for neighboring caravans */
  for (v1_ptr = ntn_ptr->cvn_list;
       v1_ptr != NULL;
       v1_ptr = v1_ptr->next) {

    /* check supplying status */
    if (!XY_INMAP(v1_ptr->xloc, v1_ptr->yloc)) continue;
    if (!supply_stat(v1_ptr->status)) continue;

    /* check the range */
    if (insect_only) {
      range = 0;
    } else {
      if (NVSPLYDIST == NTN_DISTIND) {
	range = COMM_I_RANGE;
      } else {
	range = NVSPLYDIST;
      }
    }
    if (range == 0) {
      check_val = ((xloc == v1_ptr->xloc) && (yloc == v1_ptr->yloc));
    } else {
      check_val = map_within(v1_ptr->xloc, v1_ptr->yloc, xloc, yloc, range);
    }

    /* now check if it was within the area */
    if (check_val) {

      /* create it if it ain't there */
      item_tptr = fr_accume(item_tptr, &(v1_ptr->mtrls[0]), xloc, yloc);

    }
  }

  /* done */
  return(item_tptr);
}

/* TR_CONSUME -- Perform the syphoning off of materials */
static void
tr_consume PARM_3(itemtype *, need_ptr, itemtype *, stash_ptr,
		  itemtype *, extra_ptr)
{
  double tvalue;
  int i;

  /* check input */
  if ((need_ptr == NULL) ||
      (stash_ptr == NULL)) {
    return;
  }

  /* now eat it */
  if (need_ptr[MTRLS_TALONS] > 0) {
    tvalue = ( need_ptr[MTRLS_TALONS] *
	      (((double) (stash_ptr[MTRLS_TALONS] +
			  10 * stash_ptr[MTRLS_JEWELS])) /
	       ITEMT_MTRLS[MTRLS_TALONS]) );
    if (extra_ptr != NULL) {
      stash_ptr[MTRLS_TALONS] -= (itemtype) tvalue;
      extra_ptr[MTRLS_TALONS] -= (itemtype) tvalue;
      if (extra_ptr[MTRLS_TALONS] < (itemtype) 0) {
	extra_ptr[MTRLS_TALONS] = (itemtype) 0;
      }
    } else {
      if ((itemtype) tvalue <= stash_ptr[MTRLS_TALONS]) {
	stash_ptr[MTRLS_TALONS] -= (itemtype) tvalue;
      } else {
	tvalue -= (double) stash_ptr[MTRLS_TALONS];
	stash_ptr[MTRLS_TALONS] = 0;
	stash_ptr[MTRLS_JEWELS] -= (itemtype) (tvalue / 10);
      }
    }

  }
  for (i = 0; i < MTRLS_NUMBER; i++) {
    if (i == MTRLS_TALONS) continue;
    if ((need_ptr[i] > 0) &&
	(stash_ptr[i] > 0)) {
      tvalue = ( need_ptr[i] *
		((double) stash_ptr[i] / ITEMT_MTRLS[i]));
      stash_ptr[i] -= (itemtype) tvalue;
      if (extra_ptr != NULL) {
	extra_ptr[i] -= (itemtype) tvalue;
	if (extra_ptr[i] < (itemtype) 0) {
	  extra_ptr[i] = (itemtype) 0;
	}
      }
    }
  }
}

/* TAKE_RESOURCES -- Remove resources from neighoring areas */
int
take_resources PARM_4(int, xloc, int, yloc, ITEM_PTR, take_ptr, int, insect)
{
  CITY_PTR c1_ptr;
  NAVY_PTR n1_ptr;
  CVN_PTR v1_ptr;
  int i, range, check_val;

  /* must first collect all available resources in the area */
  item_tptr = find_resources(xloc, yloc, insect);
  if (item_tptr == NULL) return(TRUE);
  for (i = 0; i < MTRLS_NUMBER; i++) {
    if ((take_ptr->mtrls[i] > 0) &&
	(ITEMT_MTRLS[i] < take_ptr->mtrls[i])) {
              free(item_tptr);
      return(TRUE);
    }
  }

  /* go through all cities and add in any resources */
  for (c1_ptr = ntn_ptr->city_list;
       c1_ptr != NULL;
       c1_ptr = c1_ptr->next) {

    /* set the range based on the type of "city" */
    if (!XY_INMAP(c1_ptr->xloc, c1_ptr->yloc)) continue;
    if (insect) {
      range = 0;
    } else {
      range = r10_region(ntn_ptr, c1_ptr,
			 &(sct[c1_ptr->xloc][c1_ptr->yloc])) / 10;
    }
    if (range == 0) {
      check_val = ((xloc == c1_ptr->xloc) && (yloc == c1_ptr->yloc));
    } else {
      check_val = map_within(c1_ptr->xloc, c1_ptr->yloc, xloc, yloc, range);
    }

    /* now check those within range */
    if (check_val) {

      /* remove proportional amount */
      tr_consume(&(take_ptr->mtrls[0]),
		 &(c1_ptr->c_mtrls[0]),
		 &(c1_ptr->i_mtrls[0]));

    }
  }

  /* search for neighboring navies */
  for (n1_ptr = ntn_ptr->navy_list;
       n1_ptr != NULL;
       n1_ptr = n1_ptr->next) {

    /* check supplying status */
    if (!XY_INMAP(n1_ptr->xloc, n1_ptr->yloc)) continue;
    if (!supply_stat(n1_ptr->status)) continue;

    /* check the range */
    if (insect) {
      range = 0;
    } else {
      if (NVSPLYDIST == NTN_DISTIND) {
	range = COMM_I_RANGE;
      } else {
	range = NVSPLYDIST;
      }
    }
    if (range == 0) {
      check_val = ((xloc == n1_ptr->xloc) && (yloc == n1_ptr->yloc));
    } else {
      check_val = map_within(n1_ptr->xloc, n1_ptr->yloc, xloc, yloc, range);
    }

    /* now check if it was within the area */
    if (check_val) {

      /* remove proportional amount */
      tr_consume(&(take_ptr->mtrls[0]),
		 &(n1_ptr->mtrls[0]),
		 (itemtype *) NULL);

    }
  }

  /* search for neighboring caravans */
  for (v1_ptr = ntn_ptr->cvn_list;
       v1_ptr != NULL;
       v1_ptr = v1_ptr->next) {

    /* check supplying status */
    if (!XY_INMAP(v1_ptr->xloc, v1_ptr->yloc)) continue;
    if (!supply_stat(v1_ptr->status)) continue;

    /* check the range */
    if (insect) {
      range = 0;
    } else {
      if (NVSPLYDIST == NTN_DISTIND) {
	range = COMM_I_RANGE;
      } else {
	range = NVSPLYDIST;
      }
    }
    if (range == 0) {
      check_val = ((xloc == v1_ptr->xloc) && (yloc == v1_ptr->yloc));
    } else {
      check_val = map_within(v1_ptr->xloc, v1_ptr->yloc, xloc, yloc, range);
    }

    /* now check if it was within the area */
    if (check_val) {

      /* remove proportional amount */
      tr_consume(&(take_ptr->mtrls[0]),
		 &(v1_ptr->mtrls[0]),
		 (itemtype *) NULL);

    }
  }

  /* clean up and leave */
  free(item_tptr);
  return(FALSE);
}

/* SEND_RESOURCES -- Redistribute materials to nearby supply centers */
int
send_resources PARM_4(int, xloc, int, yloc, ITEM_PTR, give_ptr, int, insect)
{
  CITY_PTR c1_ptr;
  int check_val, range;
  int i, total_weight;

  /* input check */
  if (!Y_ONMAP(yloc) ||
      (give_ptr == NULL)) {
    return(TRUE);
  }
  xloc = (MAPX + xloc) % MAPX;

  /* is this within range of a center? */
  if ((total_weight = SUM_WEIGHTS(xloc, yloc)) <= 0) {
    return(TRUE);
  }

  /* so distribute it */
  for (c1_ptr = ntn_ptr->city_list;
       c1_ptr != NULL;
       c1_ptr = c1_ptr->next) {

    /* set the range based on the type of "city" */
    if (!XY_INMAP(c1_ptr->xloc, c1_ptr->yloc)) continue;
    if (insect) {
      range = 0;
    } else {
      range = r10_region(ntn_ptr, c1_ptr,
			 &(sct[c1_ptr->xloc][c1_ptr->yloc])) / 10;
    }
    if (range == 0) {
      check_val = ((xloc == c1_ptr->xloc) && (yloc == c1_ptr->yloc));
    } else {
      check_val = map_within(c1_ptr->xloc, c1_ptr->yloc, xloc, yloc, range);
    }

    /* now check those within range */
    if (check_val) {

      /* add in appropriate amount */
      for (i = 0; i < MTRLS_NUMBER; i++) {
	if (give_ptr->mtrls[i] > 0) {
	  c1_ptr->c_mtrls[i] += (c1_ptr->weight * give_ptr->mtrls[i]) /
	    total_weight;
	}
      }

    }
  }

  /* all done */
  return(FALSE);
}

/* FORT_VAL -- Compute the fortification value of a sector */
int
fort_val PARM_2(int, x, int, y)
{
  int sown, value = 0;
  NTN_PTR targ_nptr;
  CITY_PTR c1_ptr;

  /* provide quick reference to the sector */
  sct_tptr = &(sct[x][y]);
  sown = sct_tptr->owner;
  if ((sown == UNOWNED) ||
      (world.np[sown] == NULL)) {
    return(value);
  }

  /* check the designation */
  if (IS_CITY(sct_tptr->designation)) {

    /* find the owner */
    targ_nptr = world.np[sown];

    /* locate the city and compute the value */
    if ((c1_ptr = citybyloc(targ_nptr, x, y)) != NULL) {

      /* compute the value */
      value += c1_ptr->fortress * 10;
      if (minor_desg(sct_tptr->designation, MIN_FORTIFIED)) {
	value *= 2;
	value += 10;
      }

    }

  } else if (major_desg(sct_tptr->designation) == MAJ_WALL) {
    value += 20;
    if (minor_desg(sct_tptr->designation, MIN_FORTIFIED)) {
      value *= 2;
      value += 10;
    }
  }

  /* check if the sector has been fortified */
  if (minor_desg(sct_tptr->designation, MIN_FORTIFIED)) {
    value += 10;
  }

  /* finito */
  return(value);
}

/* DEFENSE_VAL -- Compute the defensive value for a sector */
int
defense_val PARM_2(int, x, int, y)
{
  int value = 0;

  /* check for existing sector */
  if (!XY_ONMAP(x, y)) {
    return(value);
  }

  /* provide quick reference to sector */
  sct_tptr = &(sct[x][y]);

  /* compute defensive adjustment from terrain */
  switch (sct_tptr->altitude) {
  case ELE_MOUNTAIN:
    value += 40;
    break;
  case ELE_VALLEY:
    value += 30;
    break;
  case ELE_HILL:
    value += 20;
    break;
  default:
    /* no adjustment */
    break;
  }

  /* compute defense from vegetation */
  switch (sct_tptr->vegetation) {
  case VEG_JUNGLE:
  case VEG_SWAMP:
    value += 30;
    break;
  case VEG_FOREST:
    value += 20;
    break;
  case VEG_WOOD:
    value += 10;
    break;
  }

  /* now send back the results */
  return(value);
}

/* GETMETAL -- Assign a metal value to a given sector */
void
getmetal PARM_1(SCT_PTR, sptr)
{
  int tgoodnum, rand1, rand2;

  /* assign the metal amount and type in the sector */
  if (sptr->tradegood != TG_NONE) {
    return;
  }

  /* get the trade good value */
  if ((tgoodnum = rand_tgood(TG_METALS, 0)) == TG_NONE) {
    return;
  }

  /* now build the sector based on it */
  sptr->tradegood = tgoodnum;
  rand1 = tg_info[tgoodnum].value;
  rand2 = (rand1 + 2) / 3;
  if (rand2 >= rand1) {
    sptr->minerals = rand2;
  } else {
    sptr->minerals = rand_val(rand1 - rand2) + rand2;
  }
}

/* GETJEWEL -- Assign value and type to a sector's jewels */
void
getjewel PARM_1(SCT_PTR, sptr)
{
  int tgoodnum, rand1, rand2;

  /* assign the metal amount and type in the sector */
  if (sptr->tradegood != TG_NONE) {
    return;
  }

  /* get the trade good value for jewels */
  if ((tgoodnum = rand_tgood(TG_JEWELS, 0)) == TG_NONE) {
    return;
  }

  /* now build the sector based on it */
  sptr->tradegood = tgoodnum;
  rand1 = tg_info[tgoodnum].value;
  rand2 = (rand1 + 2) / 3;
  if (rand2 >= rand1) {
    sptr->minerals = rand2;
  } else {
    sptr->minerals = rand_val(rand1 - rand2) + rand2;
  }
}

/* GETSPELL -- Assign magic spell enchantment sectors */
void
getspell PARM_1( SCT_PTR, sptr )
{
  int tgoodnum;
  int rand1, rand2;

  /* assign the metal amount and type in the sector */
  if (sptr->tradegood != TG_NONE) {
    return;
  }

  /* get the trade good value for magical items */
  if ((tgoodnum = rand_tgood(TG_SPELLS, 0)) == TG_NONE) {
    return;
  }

  /* now build the sector based on it */
  sptr->tradegood = tgoodnum;
  rand1 = tg_info[tgoodnum].value;
  rand2 = (rand1 + 2) / 3;
  if (rand2 >= rand1) {
    sptr->minerals = rand2;
  } else {
    sptr->minerals = rand_val(rand1 - rand2) + rand2;
  }
}

/* TG_OK -- TRUE if a trade good can be seen by the given nation */
int
tg_ok PARM_2( SCT_PTR, sptr, int, nation )
{
  NTN_PTR n1_ptr;
  int mdesg, tdesg;

  /* check for default truth */
  if ((nation == UNOWNED)
      ||((n1_ptr = world.np[nation]) == NULL)){
    return(TRUE);
  }
  mdesg = major_desg(sptr->designation);
  tdesg = tg_info[sptr->tradegood].need_desg;
  if ((mdesg == tdesg) ||
      (IS_CITY(tdesg) &&
       mdesg > tdesg) ||
      (IS_FARM(tdesg) &&
       IS_FARM(mdesg))) {
    return(TRUE);
  }

  /* otherwise test using the trade good */
  if (tg_ismetal(sptr->tradegood)) {
    /* must be within a certain amount */
    if ((n1_ptr->attribute[BUTE_MINING] +
	 n1_ptr->attribute[BUTE_METALWORK]) / 2 + 2 <
	tg_info[sptr->tradegood].value * 3 / 2) {
      return(FALSE);
    }
  } else if (tg_isjewel(sptr->tradegood)) {
    if ((n1_ptr->attribute[BUTE_MINING] +
	 n1_ptr->attribute[BUTE_JEWELWORK]) / 2 + 2 <
	tg_info[sptr->tradegood].value * 3 / 2) {
      return(FALSE);
    }
#ifdef NOTDONE
  } else if (tg_isspell(sptr->tradegood)) {
#endif /* NOTDONE */
  }

  /* check the food value of the sector */
  if (tofood(sptr, nation) == 0) return(FALSE);
  return(TRUE);
}

/* TOFOOD -- Compute the food value of a sector */
int
tofood PARM_2(SCT_PTR, sptr, int, cntry)
{
  NTN_PTR n1_ptr;
  int foodvalue;

  /* find the normal value */
  foodvalue = veg_info[sptr->vegetation].food_val;
  if (foodvalue > 0) {
    foodvalue += ele_info[sptr->altitude].food_val;
  }

  /* adjust based on nation type */
  if ((n1_ptr = world.np[cntry]) != NULL ) {
    if (foodvalue == 0) {
      if ((MAGIC(n1_ptr->powers[MAG_CIVILIAN], MC_DERVISH) ||
	   MAGIC(n1_ptr->powers[MAG_WIZARDRY], MW_DESTROYER)) &&
	  (sptr->vegetation == VEG_DESERT || sptr->vegetation == VEG_ICE))
	foodvalue = 6;
      else if (MAGIC(n1_ptr->powers[MAG_CIVILIAN], MC_AMPHIBIAN)) {
	if (sptr->vegetation == VEG_JUNGLE)
	  foodvalue = 6;
	else if (sptr->vegetation == VEG_SWAMP)
	  foodvalue = 5;
      }

    }

    /* botany helps */
    if ((foodvalue > 0) &&
	MAGIC(n1_ptr->powers[MAG_CIVILIAN], MC_BOTANY)) {
      foodvalue++;
    }

    /* elves are better with trees around */
    if (r_woodwinter(n1_ptr->race)) {
      if (sptr->vegetation == VEG_FOREST) foodvalue += 3;
      else if (sptr->vegetation == VEG_BARREN) foodvalue--;
    }
  }

  /* provide bonuses for special tradegoods */
  if (tg_fishing(sptr->tradegood) ||
      tg_farming(sptr->tradegood)) {
    foodvalue += tg_info[sptr->tradegood].value;
  }

  /* done with computation, so give back results */
  return( foodvalue );
}

/* TOWOOD -- Compute the lumber value of a sector */
int
towood PARM_2(SCT_PTR, sptr, int, cntry)
{
  NTN_PTR n1_ptr;
  int woodvalue;

  /* find the normal value */
  woodvalue = veg_info[sptr->vegetation].wood_val;
  if (woodvalue > 0) {
    woodvalue += ele_info[sptr->altitude].wood_val;
  } else if (woodvalue < 0) {
    /* precautionary */
    woodvalue = 0;
  }

  /* adjust based on nation type */
  if ((n1_ptr = world.np[cntry]) != NULL ) {

    /* increase production for some magical powers */
    if ((woodvalue > 0) &&
	MAGIC(n1_ptr->powers[MAG_CIVILIAN], MC_BOTANY)) {
      woodvalue++;
    }
  }

  /* provide bonuses for special tradegoods */
  if (tg_islumber(sptr->tradegood)) {
    woodvalue += tg_info[sptr->tradegood].value;
  }

  /* done with computation, so give back results */
  return( woodvalue );
}

/* IS_HABITABLE -- Determine if a sector can be lived in */
int
is_habitable PARM_2( int, x, int, y )
{
  int tval;

  /* check if the elevation makes it unlivable */
  if ((tval = sct[x][y].altitude) == ELE_WATER ||
      (tval == ELE_PEAK)) return(FALSE);

  /* now check the vegetation */
  switch (sct[x][y].vegetation) {
  case VEG_BARREN:
  case VEG_LT_VEG:
  case VEG_GOOD:
  case VEG_WOOD:
  case VEG_FOREST:
    tval = TRUE;
    break;
  default:
    /* not a habitable vegetation */
    tval = FALSE;
    break;
  }

  /* done */
  return(tval);
}

/* JEWEL_VALUE -- Return the value of the jewels in the sector */
int
jewel_value PARM_1(SCT_PTR, sptr)
{
  int hold;

  /* check if the tradegood is a jewel type */
  if (tg_isjewel(sptr->tradegood)) {
    hold = sptr->minerals;
  } else {
    hold = 0;
  }
  return(hold);
}

/* MAGIC_VALUE -- Return the value of the spells in the sector */
int
magic_value PARM_1(SCT_PTR, sptr)
{
  int hold;

  /* check if the tradegood is a jewel type */
  if (tg_isspell(sptr->tradegood)) {
    hold = sptr->minerals;
  } else {
    hold = 0;
  }
  return(hold);
}

/* METAL_VALUE -- Return the value of the metals in the sector */
int
metal_value PARM_1(SCT_PTR, sptr)
{
  int hold;

  /* check if the tradegood is a jewel type */
  if (tg_ismetal(sptr->tradegood)) {
    hold = sptr->minerals;
  } else {
    hold = 0;
  }
  return(hold);
}

/* SECTOR_PRODUCE -- Calculate all of the production ability */
void
sector_produce PARM_3(int, xloc, int, yloc, SHEET_PTR, out_ptr)
{
  long prod_amount, workpop;
  int dtype, pval, count, dcval = 0;

  /* initialize structure */
  if (out_ptr == NULL) return;
  clr_memory( (char *) out_ptr, sizeof(SHEET_STRUCT) );

  /* check location */
  if (!XY_ONMAP(xloc, yloc)) return;

  /* check for quick out */
  sct_ptr = &(sct[xloc][yloc]);
  if ((sct_ptr->people == 0) ||
      ((sct_ptr->owner != country) && (is_god == FALSE))) return;

  /* calculate information */
  out_ptr->people = sct_ptr->people;

  /* now check the production */
  dtype = major_desg(sct_ptr->designation);
  pval = maj_dinfo[dtype].production[SEASON(TURN)];

  /* determine a relative figure for number of people working */
  workpop = poptoworkers(sct_ptr->people);
  if ((is_update == TRUE) &&
      (adjust_made == FALSE)) {
    dcval = (int) (100 * (sct_ptr->people - workpop)) / sct_ptr->people;
  }

  /* compute any adjustments due to minor designations */
  for (count = 0; count < MIN_NUMBER; count++) {
    if (minor_desg(sct_ptr->designation, MIN_START << count)) {

      /* found one, check for the appropriate type */
      if (((MIN_START << count) == MIN_MILL) &&
	  (dtype != MAJ_FARM) &&
	  (dtype != MAJ_LUMBERYARD))
	continue;
      if (((MIN_START << count) == MIN_BLACKSMITH) &&
	  (dtype != MAJ_METALMINE))
	continue;
      pval = (pval * (int) min_dinfo[count].multiplier) / 100;

    }
  }

  /* go find the output */
  switch (dtype) {
  case MAJ_FARM:
  case MAJ_FERTILE:
  case MAJ_FRUITFUL:
    /* calculate production of food */
    out_ptr->mtrls[MTRLS_FOOD] =
      (tofood(sct_ptr, country) * workpop * pval) / 100;
    prod_amount = out_ptr->mtrls[MTRLS_FOOD] / 5;
    break;
  case MAJ_METALMINE:
    /* calculate production of metals */
    if (tg_ok(sct_ptr, country)) {
      out_ptr->mtrls[MTRLS_METALS] =
	(metal_value(sct_ptr) * workpop * pval) / 100;
    }
    prod_amount = out_ptr->mtrls[MTRLS_METALS];
    if ((sct_ptr->minerals > 0) &&
	(dcval > rand_val(100))) {
      if (adjust_made == FALSE) {
	adjust_made = TRUE;
	adjust_xloc = xloc;
	adjust_yloc = yloc;
      }
      sct_ptr->minerals--;
    }
    break;
  case MAJ_JEWELMINE:
    /* calculate production of jewels */
    if (tg_ok(sct_ptr, country)) {
      out_ptr->mtrls[MTRLS_JEWELS] =
	(jewel_value(sct_ptr) * workpop * pval) / 100;
    }
    prod_amount = out_ptr->mtrls[MTRLS_JEWELS];
    if ((sct_ptr->minerals > 0) &&
	(dcval > rand_val(100))) {
      if (adjust_made == FALSE) {
	adjust_made = TRUE;
	adjust_xloc = xloc;
	adjust_yloc = yloc;
      }
      sct_ptr->minerals--;
    }
    break;
  case MAJ_LUMBERYARD:
    /* calculate production of wood */
    out_ptr->mtrls[MTRLS_WOOD] =
      10 * (towood(sct_ptr, country) * workpop * pval) / 100;
    prod_amount = out_ptr->mtrls[MTRLS_WOOD] / 2;
    break;
  default:
    /* people are the production amounts for the remaining types */
    prod_amount = (sct_ptr->people * pval) / 100;
    break;
  }

  /* sector benefits */
  if (sct_ptr->people > 100) {
    out_ptr->mtrls[MTRLS_WOOD] += towood(sct_ptr, country);
    out_ptr->mtrls[MTRLS_FOOD] += tofood(sct_ptr, country);
  }

  /* now calculate the taxable output (before tax percentage) */
  if (pval > 0) {
    out_ptr->mtrls[MTRLS_TALONS] =
      prod_amount * maj_dinfo[dtype].tax_value;
  }
}

/* SECTOR_CONSUME -- Calculate all of the sector needs */
void
sector_consume PARM_3(int, xloc, int, yloc, SHEET_PTR, out_ptr)
{
  int count, i;
  int dtype, pval;

  /* initialize structure */
  if (out_ptr == NULL) return;
  clr_memory( (char *) out_ptr, sizeof(SHEET_STRUCT) );

  /* check location */
  if (!XY_ONMAP(xloc, yloc)) return;

  /* check for quick out */
  sct_ptr = &(sct[xloc][yloc]);
  if ((sct_ptr->owner != country) && (is_god == FALSE)) return;

  /* record population */
  out_ptr->people = sct_ptr->people;

  /* now check the consumption */
  dtype = major_desg(sct_ptr->designation);

  /* first check food consumption */
  if (!IS_FARM(dtype) ||
      ((SEASON(TURN) == WINTER_SEASON) &&
       !minor_desg(sct_ptr->designation, MIN_GRANARY))) {
    /* farms with granaries are self feeding during winter */
    out_ptr->mtrls[MTRLS_FOOD] +=
      (itemtype) (sct_ptr->people * NTN_D_EATRATE);
  }

  /* set costs for the major designation */
  for (i = 0; i < MTRLS_NUMBER; i++) {
    out_ptr->mtrls[i] += maj_dinfo[dtype].mtrls_spt[i];
  }
  pval = maj_dinfo[dtype].multiplier;

  /* now go through support of minor designations */
  for (count = 0; count < MIN_NUMBER; count++) {

    /* that designation exists */
    if (minor_desg(sct_ptr->designation, MIN_START << count)) {

      /* charge it! */
      for (i = 0; i < MTRLS_NUMBER; i++) {
	out_ptr->mtrls[i] += min_dinfo[count].mtrls_spt[i] * pval;
      }

    }
    
  }

  /* devastated sectors only have 1/4 metal, wood and talons cost */
  if (minor_desg(sct_ptr->designation, MIN_DEVASTATED)) {
    for (i = 0; i < MTRLS_NUMBER; i++) {
      if ((i != MTRLS_FOOD) &&
	  (i != MTRLS_JEWELS)) {
	out_ptr->mtrls[i] /= 4;
      }
    }
  }
  sct_cost_adjust(0, sct_ptr, &(out_ptr->mtrls[0]));
  mgk_cost_adjust(0, &(out_ptr->mtrls[0]));
}

/* SCT_COST_ADJUST -- Adjustment due to what is in the sector */
void
sct_cost_adjust PARM_3(int, type, SCT_PTR, s1_ptr, itemtype *, expenses)
{
  /* check owner */
  if ((s1_ptr == NULL) ||
      (s1_ptr->owner != country)) return;

  /* check input */
  if (expenses == NULL) return;

  /* check the minor designations */
  if ((type & 1) == 0) {
    if (minor_desg(s1_ptr->designation, MIN_TRADINGPOST)) {
      /* trade tarriff supplementing some costs */
      expenses[MTRLS_TALONS] *= 98;
      expenses[MTRLS_TALONS] /= 100;
    }
  }
  if (minor_desg(s1_ptr->designation, MIN_MILL)) {
    /* reduced wood costs in mill areas */
    expenses[MTRLS_WOOD] *= 95;
    expenses[MTRLS_WOOD] /= 100;
  }
  if (minor_desg(s1_ptr->designation, MIN_SIEGED)) {
    /* higher prices when under siege */
    expenses[MTRLS_TALONS] *= 110;
    expenses[MTRLS_TALONS] /= 100;
  }
  if (minor_desg(s1_ptr->designation, MIN_BLACKSMITH)) {
    /* good metal workmanship */
    expenses[MTRLS_METALS] *= 90;
    expenses[MTRLS_METALS] /= 100;
  }
}

/* S_USELEVEL -- Usage level of the sector within it's discipline */
int
s_uselevel PARM_1(SCT_PTR, s1_ptr)
{
  /* check for null value */
  if (tofood(s1_ptr, country) == 0) return(0);

  /* different values for different classes */
  switch (major_desg(s1_ptr->designation)) {
  case MAJ_JEWELMINE:
    /* jewel value */
    if ((is_god == TRUE) ||
	(UNOWNED == s1_ptr->owner) ||
	(country == s1_ptr->owner) ||
	WIZ_MAGIC(MW_SEEALL)) {
      if (tg_ok(s1_ptr, country)) {
	return(jewel_value(sct_ptr));
      }
    }
    break;
  case MAJ_METALMINE:
    /* metal value */
    if ((is_god == TRUE) ||
	(UNOWNED == s1_ptr->owner) ||
	(country == s1_ptr->owner) ||
	WIZ_MAGIC(MW_SEEALL)) {
      if (tg_ok(s1_ptr, country)) {
	return(metal_value(sct_ptr));
      }
    }
    break;
  case MAJ_SHRINE:
    /* spell casting value */
    if ((is_god == TRUE) ||
	(UNOWNED == s1_ptr->owner) ||
	(country == s1_ptr->owner) ||
	WIZ_MAGIC(MW_SEEALL)) {
      if (tg_ok(s1_ptr, country)) {
	return(magic_value(sct_ptr));
      }
    }
    break;
  case MAJ_LUMBERYARD:
    /* just the wood value */
    return(towood(s1_ptr, country));
    break;
  default:
    /* food value for farms, silch for others */
    if (IS_FARM(s1_ptr->designation)) {
      return(tofood(s1_ptr, country));
    }
    break;
  }
  return(0);
}

/* EXPOSURE_VALUE -- The amount of exposure troops face in a sector */
int
exposure_value PARM_2(NTN_PTR, n1_ptr, SCT_PTR, s1_ptr)
{
  int hold = 0, vegval, eleval;

  /* check the input */
  if ((s1_ptr == NULL) ||
      (EXPOSURE == 0)) return(hold);
  vegval = s1_ptr->vegetation;
  eleval = s1_ptr->altitude;

  /* start with the base values */
  hold += veg_info[vegval].exposure[SEASON(TURN)];
  if ((n1_ptr == NULL) ||
      !r_mountaineer(n1_ptr->race)) {
    hold += ele_info[eleval].exposure[SEASON(TURN)];
  }

  /* adjust if needed */
  if (n1_ptr != NULL) {

    /* check for vegetation specific powers */
    switch (vegval) {
    case VEG_NONE:
      /* sailors and marines can handle it */
      if (MAGIC(n1_ptr->powers[MAG_CIVILIAN], MC_SAILOR)) {
	hold *= 4;
	hold /= 5;
      }
      if (MAGIC(n1_ptr->powers[MAG_CIVILIAN], MC_MARINE)) {
	hold *= 4;
	hold /= 5;
      }
      break;
    case VEG_WOOD:
    case VEG_FOREST:
      /* some races are protected by the woods */
      if (r_woodwinter(n1_ptr->race)) {
	hold = 0;
      }
      break;
    case VEG_DESERT:
    case VEG_ICE:
    case VEG_TUNDRA:
      /* less exposure in the 'dervish' realms */
      if (MAGIC(n1_ptr->powers[MAG_CIVILIAN], MC_DERVISH)) {
	hold /= 2;
      }
      if (MAGIC(n1_ptr->powers[MAG_WIZARDRY], MW_DESTROYER)) {
	hold *= 3;
	hold /= 4;
      }
      break;
    case VEG_SWAMP:
    case VEG_JUNGLE:
      /* the amphibians can handle it */
      if (MAGIC(n1_ptr->powers[MAG_CIVILIAN], MC_AMPHIBIAN)) {
	hold /= 2;
      }
      break;
    default:
      /* don't do nothin' */
      break;
    }

    /* check for dervish powers */
    if (MAGIC(n1_ptr->powers[MAG_CIVILIAN], MC_DERVISH)) {
      hold /= 2;
    }
    if (MAGIC(n1_ptr->powers[MAG_WIZARDRY], MW_DESTROYER)) {
      hold *= 3;
      hold /= 4;
    }
  }

  /* adjust and then output */
  if (hold > 0) {
    hold *= EXPOSURE;
    hold += 5;
    hold /= 10;
  }
  return(hold);
}

/* DISTORT_VISION -- Distort numerical values based on vision of a sector */
long
distort_vision PARM_4(int, xval, int, yval, long, trueval, long, distval)
{
  int sown, distortion = 25;
  long lval = trueval;

  /* check and set sector ownership */
  xval = (xval + MAPX) % MAPX;
  sown = sct[xval][yval].owner;
  if ((sown == UNOWNED) ||
      (sown < 0) ||
      (sown > MAXNTN) ||
      (ntn_tptr == NULL)) {
    sown = UNOWNED;
  }
  
  /* display the food value of a sector */
  if ((country != UNOWNED) &&
      (sown != UNOWNED) &&
      (country != sown)) {

    if (MAGIC(ntn_tptr->powers[MAG_WIZARDRY], MW_ILLUSION)) {
      distortion += 25;
    }
    if (MAGIC(ntn_tptr->powers[MAG_WIZARDRY], MW_THEVOID)) {
      distortion *= 2;
    }
    if (WIZ_MAGIC(MW_VISION)) {
      distortion /= 2;
    }
    if (WIZ_MAGIC(MW_SEEALL)) {
      distortion /= 2;
    }

    /* check increased viewing */
    if (is_update == FALSE) {
      distortion -= (VIS_CHECK(xval, yval) * 5);
    }
    if (distortion <= 5) distortion = 5;
    else if (distortion > 100) distortion = 100;

    /* now do the distortion */
    lval = (trueval * ((100 - distortion) +
		       nrand_val(distortion * 2, distval))) / 100;

  }
  return lval;
}

/* RAND_SECTOR -- Set the global int and global longs to a random locale */
void
rand_sector PARM_5(int, x, int, y, int, range, int, nowater, int, visionrand)
{
  int count = 0, newx, newy;

  /* keep searching until a sector is found */
  do {
    if ((visionrand) &&
	(count++ < 20)) {
      newx = x - range +
	rand_val(distort_vision(x , y, range, MAPX * MAPY) + 1);
      newy = y - range +
	rand_val(distort_vision(x , y, range, MAPX * MAPY) + 1);
    } else {
      newx = x + (range - rand_val(range + 1));
      newy = y + (range - rand_val(range + 1));
    }
    newx = (newx + MAPX) % MAPX;
  } while (!Y_ONMAP(newy) ||
	   (nowater &&
	    (sct[newx][newy].altitude == ELE_WATER)));
  global_int = newx;
  global_long = newy;
}
