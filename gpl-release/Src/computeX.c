// SPDX-License-Identifier: GPL-3.0-or-later
/* Routines for calculating national production, budget and stats */
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
#include "cityX.h"
#include "itemX.h"
#include "navyX.h"
#include "butesX.h"
#include "calenX.h"
#include "desigX.h"
#include "magicX.h"
#include "mtrlsX.h"
#include "worldX.h"
#include "activeX.h"
#include "elevegX.h"
#include "statusX.h"
#include "tgoodsX.h"
#include "weightX.h"
#include "caravanX.h"

/* DFLT_CITYWEIGHT -- Determine the default weighting value */
int
dflt_cityweight PARM_2(int, ntnnum, CITY_PTR, c1_ptr)
{
  SCT_PTR shold_ptr = sct_ptr;
  int i, j, hold = 0;

  /* check input */
  if (c1_ptr == NULL) return(hold);

  /* find the city sector */
  i = c1_ptr->xloc;
  j = c1_ptr->yloc;
  if (!XY_ONMAP(i, j) || (sct[i][j].owner != ntnnum)) return(hold);

  /* distribution */
  sct_ptr = &(sct[i][j]);

  /* find weighting based on designation */
  switch (major_desg(sct_ptr->designation)) {
  case MAJ_CAPITAL:
    hold = WEIGHT_CAPITAL;
    break;
  case MAJ_CITY:
    hold = WEIGHT_CITY + sct_ptr->people / 1000L;
    break;
  case MAJ_TOWN:
    hold = WEIGHT_TOWN + sct_ptr->people / 500L;
    break;
  case MAJ_STOCKADE:
    hold = WEIGHT_STOCKADE;
    break;
  default:
    hold = 0;
    break;
  }
  sct_ptr = shold_ptr;
  return(hold);
}

/* DIST_WEIGHTS -- Distribute the actual weight into other sectors */
static void
dist_weights PARM_2(int, x, int, y)
{
  if (sct[x][y].owner == global_int) {
    SUM_WEIGHTS(x, y) += CITY_WEIGHT;
  }
}

/* SET_WEIGHTS -- Set distribution weights for nation */
void
set_weights PARM_1(int, recalc)
{
  CITY_PTR chold_ptr = city_ptr;
  SCT_PTR shold_ptr = sct_ptr;
  int cntry, start, end, range, hold;

  /* allocate if needed */
  sum_weights = new_mapshort(sum_weights);

  /* check the range */
  ntn_tptr = ntn_ptr;
  if (country == UNOWNED) {
    start = 1;
    end = MAXNTN;
  } else {
    start = country;
    end = country + 1;
  }

  /* now set the values appropriately */
  for (cntry = start; cntry < end; cntry++) {

    /* check initialization */
    ntn_ptr = world.np[cntry];
    if (ntn_ptr == NULL) continue;

    /* go through all of the cities in the nation */
    for (city_ptr = ntn_ptr->city_list;
	 city_ptr != NULL;
	 city_ptr = city_ptr->next) {

      /* check the city location */
      if (!XY_INMAP(CITY_XLOC, CITY_YLOC)) continue;

      /* redo the calculation if needed */
      if (recalc == TRUE) {

	/* find the default weighting */
	hold = dflt_cityweight(cntry, city_ptr);

	/* store resulting weight */
	if (CITY_WEIGHT <= hold) {
	  CITY_WEIGHT = (uns_char) hold;
	}
      }

      /* distribute surrounding weights */
      sct_ptr = &(sct[CITY_XLOC][CITY_YLOC]);
      range = r10_region(ntn_ptr, city_ptr, sct_ptr) / 10;
      if ((CITY_PEOPLE < 0) ||
	  (minor_desg(sct_ptr->designation, MIN_DEVASTATED | MIN_SIEGED))) {
	/* keep sieges, devastation and new towns tight */
	if (CITY_WEIGHT == 0) {
	  CITY_WEIGHT = 1;
	}
      }

      /* now go about the area */
      global_int = cntry;
      map_loop(CITY_XLOC, CITY_YLOC, range, dist_weights);

    }

  }

  /* now reset the nation pointer */
  ntn_ptr = ntn_tptr;
  city_ptr = chold_ptr;
  sct_ptr = shold_ptr;
}

/* The variable to be used among the local routines */
static SHEET_PTR region_spread;
static int need_all;

/* RG_PRODVAL -- Perform the increase of resources due to the sector */
static void
rg_prodval PARM_2(int, x, int, y)
{
  SHEET_STRUCT sector_spread;
  int total_weight, dtype, count;
  itemtype share;
  long share_peop;

  /* check if it is a target sector */
  if ((sct[x][y].owner == country) &&
      (sct[x][y].people > 0L)) {

    /* find balance of sector */
    total_weight = SUM_WEIGHTS(x, y);

    /* doesn't have any place to go */
    if (total_weight == 0) return;

    /* now distribute sector production */
    sector_produce(x, y, &sector_spread);

    /* find the designation type */
    dtype = major_desg(sct[x][y].designation);

    /* count information relevant to all sectors */
    region_spread->people += 
      (share_peop = ((sector_spread.people * CITY_WEIGHT)
		     / total_weight));
    region_spread->sectors++;

    /* take care of some detailed sector specifics */
    share = (sector_spread.mtrls[MTRLS_TALONS] * CITY_WEIGHT) / total_weight;
    if (need_all == TRUE) {
      if (region_spread->type_talons[dtype] + share > BIGITEM) {
	region_spread->type_talons[dtype] = BIGITEM;
      } else {
	region_spread->type_talons[dtype] += share;
      }
      region_spread->type_people[dtype] += share_peop;
      region_spread->type_sectors[dtype]++;
    }

    /* raw materials produced */
    for (count = 0; count < MTRLS_NUMBER; count++) {
      share = (sector_spread.mtrls[count] * CITY_WEIGHT) / total_weight;
      if (region_spread->mtrls[count] + share > BIGITEM) {
	region_spread->mtrls[count] = BIGITEM;
      } else {
	region_spread->mtrls[count] += share;
      }
    }

  }
}

/* REGION_PRODUCE -- Calculate amount produced within communication range */
SHEET_PTR
region_produce PARM_2(char *, cname, int, fullinfo)
{
  int xloc, yloc, range;

  /* first check the national validity */
  need_all = fullinfo;
  if ((ntn_ptr == NULL) || ((city_ptr = citybyname(cname)) == NULL)) {
    return((SHEET_PTR) NULL);
  }

  /* set the sector */
  xloc = CITY_XLOC;
  yloc = CITY_YLOC;

  /* check the sector */
  if (!XY_ONMAP(xloc, yloc)) {
    return((SHEET_PTR) NULL);
  }
  sct_ptr = &(sct[xloc][yloc]);
  if ((sct_ptr->owner == UNOWNED) ||
      (sct_ptr->owner != country) ||
      !IS_CITY(sct_ptr->designation)) {
    return((SHEET_PTR) NULL);
  }

  /* now set bounds and other information */
  range = r10_region(ntn_ptr, city_ptr, sct_ptr) / 10;

  /* set region storage */
  if ((region_spread = (SHEET_PTR) malloc(sizeof(SHEET_STRUCT))) == NULL) {
    fprintf(fupdate, "Unable to allocate SHEET_STRUCT in region_produce()\n");
    abrt();
  }

  /* initialize storage */
  clr_memory((char *) region_spread, sizeof(SHEET_STRUCT));

  /* just to be safe */
  region_spread->sectors = 0;

  /* calculate for region */
  map_loop(xloc, yloc, range, rg_prodval);

  /* HEREIAM -- adjust the regional calculations based on minor desgs */  

  /* give back the results */
  return(region_spread);
}

/* RG_CONSVAL -- Add in consumption of sector if appropriate */
static void
rg_consval PARM_2(int, x, int, y)
{
  SHEET_STRUCT sector_spread;
  double share;
  long share_peop;
  int dtype, count, total_weight;

  /* calculate for region */
  if (sct[x][y].owner == country) {

    /* find resource balance of sector */
    item_tptr = find_resources(x, y,
			       minor_desg(sct[x][y].designation, MIN_SIEGED));
    total_weight = SUM_WEIGHTS(x, y);

    /* no go */
    if (item_tptr == NULL) return;

    /* now find the sector consumption */
    sector_consume(x, y, &sector_spread);

    /* find the designation type */
    dtype = major_desg(sct[x][y].designation);

    /* count information relevant to all sectors */
    if (total_weight > 0) {
      region_spread->people += 
	(share_peop = (sector_spread.people
		       * CITY_WEIGHT) / total_weight);
    }  else {
      share_peop = 0;
    }
    share = (double) 0;
    if ((sector_spread.mtrls[MTRLS_TALONS] > 0) &&
	(ITEMT_MTRLS[MTRLS_TALONS] >= sector_spread.mtrls[MTRLS_TALONS])) {
      share = ((double)sector_spread.mtrls[MTRLS_TALONS] *
	       (CITY_MTRLS[MTRLS_TALONS] + 10 * CITY_MTRLS[MTRLS_JEWELS])) /
		 ITEMT_MTRLS[MTRLS_TALONS];
      if (region_spread->mtrls[MTRLS_TALONS] + (itemtype) share > BIGITEM) {
	region_spread->mtrls[MTRLS_TALONS] = BIGITEM;
      } else {
	region_spread->mtrls[MTRLS_TALONS] += (itemtype) share;
      }
    }
    region_spread->sectors++;

    /* take care of detailed sector specifics */
    if (need_all) {
      if (region_spread->type_talons[dtype] + (itemtype) share > BIGITEM) {
	region_spread->type_talons[dtype] = BIGITEM;
      } else {
	region_spread->type_talons[dtype] += (itemtype) share;
      }
      region_spread->type_people[dtype] += share_peop;
      region_spread->type_sectors[dtype]++;
    }

    /* now calculate the other commodities; skipping talons */
    for (count = 0; count < MTRLS_NUMBER; count++) {

      /* skip talons... already done */
      if (count == MTRLS_TALONS) continue;

      /* now distribute the materials */
      if (ITEMT_MTRLS[count] > 0) {
	share = ((double) sector_spread.mtrls[count] *
		 CITY_MTRLS[count]) / ITEMT_MTRLS[count];
	if (region_spread->mtrls[count] + (itemtype) share > BIGITEM) {
	  region_spread->mtrls[count] = BIGITEM;
	} else {
	  region_spread->mtrls[count] += (itemtype) share;
	}
      } else if ((is_update == FALSE) &&
		 (ITEMT_ID > 0)) {
	share = ((double) sector_spread.mtrls[count]) / ITEMT_ID;
	if (region_spread->mtrls[count] + (itemtype) share > BIGITEM) {
	  region_spread->mtrls[count] = BIGITEM;
	} else {
	  region_spread->mtrls[count] += (itemtype) share;
	}
      }

    }

    /* make sure not to eat up the memory */
    free(item_tptr);
  }
}

/* REGION_CONSUME -- Calculate amount consumed from city resources */
SHEET_PTR
region_consume PARM_2(char *, cname, int, fullinfo)
{
  int i, xloc, yloc, range;
  double holddbl;
  itemtype acct_vals[MTRLS_NUMBER];

  /* first check the national validity */
  need_all = fullinfo;
  if ((ntn_ptr == NULL) || ((city_ptr = citybyname(cname)) == NULL)) {
    return((SHEET_PTR) NULL);
  }

  /* set the sector */
  xloc = CITY_XLOC;
  yloc = CITY_YLOC;

  /* check the sector */
  if (!XY_ONMAP(xloc, yloc)) {
    return((SHEET_PTR) NULL);
  }
  sct_ptr = &(sct[xloc][yloc]);
  if ((sct_ptr->owner == UNOWNED) ||
      (sct_ptr->owner != country) ||
      !IS_CITY(sct_ptr->designation)) {
    return((SHEET_PTR) NULL);
  }

  /* now set bounds and other information */
  range = r10_region(ntn_ptr, city_ptr, sct_ptr) / 10;

  /* set region storage */
  if ((region_spread = (SHEET_PTR) malloc(sizeof(SHEET_STRUCT))) == NULL) {
    fprintf(fupdate, "Unable to allocate SHEET_STRUCT in region_consume()\n");
    abrt();
  }

  /* initialize storage */
  clr_memory( (char *) region_spread, sizeof(SHEET_STRUCT) );

  /* no consumption for monster nations! */
  if (n_ismonster(ntn_ptr->active)) {
    return(region_spread);
  }

  /* do the sector consumption */
  map_loop(xloc, yloc, range, rg_consval);

  /* no need to continue if there is no supplying */
  if (MAXSUPPLIES == 0) return(region_spread);

  /* check for supplying of troops, navies and carvans */
  army_tptr = NULL;
  item_tptr = NULL;
  for (army_ptr = ntn_ptr->army_list;
       army_ptr != NULL;
       army_ptr = army_ptr->next) {

    /* anywhere in region */
    if (map_within(CITY_XLOC, CITY_YLOC, ARMY_XLOC, ARMY_YLOC, range) &&
	a_issupplyable(army_ptr, CITY_XLOC, CITY_YLOC, FALSE)) {

      /* get support costs */
      army_support(army_ptr, &(acct_vals[0]), 1);

      /* find resource balance of sector */
      if ((item_tptr = find_resources(ARMY_XLOC, ARMY_YLOC,
				      splyinsect_stat(ARMY_STAT)))
	  == NULL)
	continue;
      if (ITEMT_MTRLS[MTRLS_TALONS] <
	  acct_vals[MTRLS_TALONS] + region_spread->army_cost) {
	/* empty supply sector */
	free(item_tptr);
	continue;
      }

      /* check the type */
      for (i = 0; i < MTRLS_NUMBER; i++) {
	if (acct_vals[i] != 0) {
	  switch (i) {
	  case MTRLS_TALONS:
	    /* find amount that must be contributed */
	    if (ITEMT_MTRLS[MTRLS_TALONS] > 0) {
	      holddbl = (100 * (double) (CITY_MTRLS[MTRLS_TALONS] +
					 10 * CITY_MTRLS[MTRLS_JEWELS]))
		/ ((double) ITEMT_MTRLS[MTRLS_TALONS]);
	      region_spread->army_cost +=
		(itemtype) (holddbl * acct_vals[i]) / 100;
	    }
	    break;
	  case MTRLS_JEWELS:
	    /* compute any jewel costs */
	    if (ITEMT_MTRLS[i] > 0) {
	      holddbl = (100 * (double) CITY_MTRLS[i]) /
		ITEMT_MTRLS[i];
	      region_spread->monst_jewels +=
		(itemtype) ((double) holddbl * acct_vals[i]) / 100;
	    }
	    break;
	  default:
	    /* everything else is just a bit of give and take */
	    if (ITEMT_MTRLS[i] > 0) {
	      holddbl = (100 * (double) CITY_MTRLS[i]) /
		ITEMT_MTRLS[i];
	      region_spread->mtrls[i] +=
		(itemtype) (holddbl * acct_vals[i]) / 100;
	    }
	    break;
	  }
	}
      }

      /* count 'em */
      if (a_ismonster(ARMY_TYPE)) {
	region_spread->monst_troops += ARMY_SIZE;
      } else if (a_isleader(ARMY_TYPE)) {
	region_spread->army_men++;
      } else {
	region_spread->army_men += ARMY_SIZE;
      }
      free(item_tptr);
    }
  }
  region_spread->mtrls[MTRLS_TALONS] += region_spread->army_cost;
  region_spread->mtrls[MTRLS_JEWELS] += region_spread->monst_jewels;

  /* now go through all of the navy units */
  for (navy_ptr = ntn_ptr->navy_list;
       navy_ptr != NULL;
       navy_ptr = navy_ptr->next) {

    /* check if it is in range and supplyable */
    if (n_issupplyable(navy_ptr, CITY_XLOC, CITY_YLOC, FALSE) &&
	map_within(CITY_XLOC, CITY_YLOC, NAVY_XLOC, NAVY_YLOC, range)) {
      /* get the costs */
      navy_support(navy_ptr, &(acct_vals[0]), 1);
      mgk_cost_adjust(2, &(acct_vals[0]));

      /* check how much of expenses must be met */
      if ((item_tptr =
	   find_resources(NAVY_XLOC, NAVY_YLOC,
			  (sct[NAVY_XLOC][NAVY_YLOC].altitude
			   == ELE_WATER))) == NULL)
	continue;
      if (ITEMT_MTRLS[MTRLS_TALONS] <
	  acct_vals[MTRLS_TALONS] + region_spread->army_cost +
	  region_spread->navy_cost) {
	/* empty supply sector */
	free(item_tptr);
	continue;
      }
      
      for (i = 0; i < MTRLS_NUMBER; i++) {
	if (acct_vals[i] > 0) {
	  switch (i) {
	  case MTRLS_TALONS:
	    /* find amount that must be contributed */
	    if (ITEMT_MTRLS[MTRLS_TALONS] > 0) {
	      holddbl = (100 * (double) (CITY_MTRLS[MTRLS_TALONS] +
					 10 * CITY_MTRLS[MTRLS_JEWELS]))
		/ (double) (ITEMT_MTRLS[MTRLS_TALONS]);
	      region_spread->navy_cost +=
		(itemtype) (holddbl * acct_vals[i]) / 100;
	    }
	    break;
	  default:
	    /* everything else is just a bit of give and take */
	    if (ITEMT_MTRLS[i] > 0) {
	      holddbl = (100 * (double) CITY_MTRLS[i]) /
		ITEMT_MTRLS[i];
	      region_spread->mtrls[i] +=
		(itemtype) (holddbl * acct_vals[i]) / 100;
	    }
	    break;
	  }
	}
      }

      /* count up the holds */
      for (xloc = 0; xloc < NSHP_NUMBER; xloc++) {
	region_spread->ship_holds += navy_holds(navy_ptr, xloc);
      }
      free(item_tptr);

    }
  }
  /* cost for maintaining the ships */
  region_spread->mtrls[MTRLS_TALONS] += region_spread->navy_cost;

  /* tromp through the caravans */
  for (cvn_ptr = ntn_ptr->cvn_list;
       cvn_ptr != NULL;
       cvn_ptr = cvn_ptr->next) {

    /* check if it is within range */
    if (map_within(CVN_XLOC, CVN_YLOC, CITY_XLOC, CITY_YLOC, range) &&
	v_issupplyable(cvn_ptr, CITY_XLOC, CITY_YLOC, FALSE)) {

      /* compute the costs */
      cvn_support(cvn_ptr, &(acct_vals[0]), 1);

      /* find resource balance of sector */
      if ((item_tptr = find_resources(CVN_XLOC, CVN_YLOC,
				      splyinsect_stat(CVN_STAT))) == NULL)
	continue;
      if (ITEMT_MTRLS[MTRLS_TALONS] <
	  acct_vals[MTRLS_TALONS] + region_spread->army_cost +
	  region_spread->navy_cost + region_spread->cvn_cost) {
	/* useless supply sector */
	free(item_tptr);
	continue;
      }
      for (i = 0; i < MTRLS_NUMBER; i++) {
	if (acct_vals[i] > 0) {
	  switch (i) {
	  case MTRLS_TALONS:
	    /* keep track of the talons */
	    if (ITEMT_MTRLS[MTRLS_TALONS] > 0) {
	      holddbl = (100 * (double) (CITY_MTRLS[MTRLS_TALONS] +
					 10 * CITY_MTRLS[MTRLS_JEWELS]))
		/ (double) (ITEMT_MTRLS[MTRLS_TALONS]);
	      region_spread->cvn_cost +=
		(itemtype) (holddbl * acct_vals[i]) / 100;
	    }
	    break;
	  default:
	    /* just do it */
	    if (ITEMT_MTRLS[i] > 0) {
	      holddbl = (100 * (double) CITY_MTRLS[i]) /
		ITEMT_MTRLS[i];
	      region_spread->mtrls[i] +=
		(itemtype) (holddbl * acct_vals[i]) / 100;
	    }
	    break;
	  }
	}
      }

      /* count them up */
      region_spread->caravan_wagons += CVN_SIZE * WAGONS_IN_CVN;
    }
  }
  region_spread->mtrls[MTRLS_TALONS] += region_spread->cvn_cost;

  /* give back the results */
  return(region_spread);
}

/* NTN_TOTALS -- Gather together all of the city and item information */
void
ntn_totals PARM_1(int, nation)
{
  register int i, j;

  /* check the information */
  if ((nation < 0) || (nation >= ABSMAXNTN) ||
      ((ntn_tptr = world.np[nation]) == NULL)) return;

  /* initialize nation values */
  for (i = 0; i < MTRLS_NUMBER; i++) {
    ntn_tptr->mtrls[i] = (itemtype) 0;
  }
  ntn_tptr->tships = 0;
  ntn_tptr->twagons = 0;
  ntn_tptr->tmonst = 0;
  ntn_tptr->tleaders = 0;
  ntn_tptr->tsctrs = 0;
  ntn_tptr->tunsctrs = 0;
  ntn_tptr->tmil = 0L;
  ntn_tptr->tciv = 0L;

  /* go through all of the sectors */
  for (i = ntn_tptr->leftedge; i <= ntn_tptr->rightedge; i++) {
    for (j = ntn_tptr->bottomedge; j <= ntn_tptr->topedge; j++) {
      if (sct[(i + MAPX) % MAPX][j].owner == nation) {
	ntn_tptr->tciv += sct[(i + MAPX) % MAPX][j].people;
	ntn_tptr->tsctrs++;
	if ((sum_weights != NULL) &&
	    (SUM_WEIGHTS((i + MAPX) % MAPX, j) == 0))
	  ntn_tptr->tunsctrs++;
      }
    }
  }

  /* summarize all of the city totals */
  for (city_ptr = ntn_tptr->city_list;
       city_ptr != NULL;
       city_ptr = city_ptr->next) {
    for (i = 0; i < MTRLS_NUMBER; i++) {
      ntn_tptr->mtrls[i] += CITY_MTRLS[i];
    }
  }

  /* include all of the commodities */
  for (item_ptr = ntn_tptr->item_list;
       item_ptr != NULL;
       item_ptr = item_ptr->next) {
    for (i = 0; i < MTRLS_NUMBER; i++) {
      ntn_tptr->mtrls[i] += ITEM_MTRLS[i];
    }
  }

  /* count all of the little toy soldiers */
  for (army_ptr = ntn_tptr->army_list;
       army_ptr != NULL;
       army_ptr = army_ptr->next) {
    if (a_isleader(ARMY_TYPE)) {
      ntn_tptr->tmil++;
      ntn_tptr->tleaders++;
    } else if (a_ismonster(ARMY_TYPE)) {
      ntn_tptr->tmonst += ARMY_SIZE;
    } else {
      ntn_tptr->tmil += ARMY_SIZE;
    }
  }

  /* account for all of the people on ships */
  for (navy_ptr = ntn_tptr->navy_list;
       navy_ptr != NULL;
       navy_ptr = navy_ptr->next) {
    for (i = 0; i < NSHP_NUMBER; i++) {
      ntn_tptr->tships += (NAVY_CNTSHIPS(i, N_LIGHT) +
			   NAVY_CNTSHIPS(i, N_MEDIUM) + 
			   NAVY_CNTSHIPS(i, N_HEAVY));
    }
    for (i = 0; i < MTRLS_NUMBER; i++) {
      ntn_tptr->mtrls[i] += NAVY_MTRLS[i];
    }
    ntn_tptr->tciv += (long) NAVY_PEOP *
      navy_holds(navy_ptr, NSHP_GALLEYS);
  }

  /* account for all of the people on caravans */
  for (cvn_ptr = ntn_tptr->cvn_list;
       cvn_ptr != NULL;
       cvn_ptr = cvn_ptr->next) {
    for (i = 0; i < MTRLS_NUMBER; i++) {
      ntn_tptr->mtrls[i] += CVN_MTRLS[i];
    }
    ntn_tptr->twagons += WAGONS_IN_CVN * CVN_SIZE;
    ntn_tptr->tciv += (long) CVN_PEOP * CVN_SIZE;
  }
}

/* ADJUST_PRODUCTION -- Adjustment of produced raw materials */
void
adjust_production PARM_2( SHEET_PTR, produce_ptr, int, fullinfo )
{
  int i;

  /* check input */
  if ((produce_ptr == NULL) || (ntn_ptr == NULL)) return;

  /* check for better book keeping */
  if (CIV_MAGIC(MC_ACCOUNTANT)) {
    produce_ptr->mtrls[MTRLS_TALONS] *= 120;
    produce_ptr->mtrls[MTRLS_TALONS] /= 100;
    if (fullinfo) {
      for (i = 0; i < MAJ_NUMBER; i++) {
	produce_ptr->type_talons[i] *= 120;
	produce_ptr->type_talons[i] /= 100;
      }
    }
  }

  /* check for health plan costs */
  if (CIV_MAGIC(MC_SOCIALISM)) {
    produce_ptr->mtrls[MTRLS_TALONS] *= 110;
    produce_ptr->mtrls[MTRLS_TALONS] /= 100;
    if (fullinfo) {
      for (i = 0; i < MAJ_NUMBER; i++) {
	produce_ptr->type_talons[i] *= 110;
	produce_ptr->type_talons[i] /= 100;
      }
    }
  }

  /* handle the taxes */
  produce_ptr->mtrls[MTRLS_TALONS] *= ntn_ptr->attribute[BUTE_TAXRATE];
  produce_ptr->mtrls[MTRLS_TALONS] /= 100;
  if (fullinfo) {
    for (i = 0; i < MAJ_NUMBER; i++) {
      produce_ptr->type_talons[i] *= ntn_ptr->attribute[BUTE_TAXRATE];
      produce_ptr->type_talons[i] /= 100;
    }
  }

  /* check food production increase */
  i = 100;
  if (CIV_MAGIC(MC_FARMING)) {
    i += 20;
  }
  if (WIZ_MAGIC(MW_EARTH)) {
    i += 10;
  }
  produce_ptr->mtrls[MTRLS_FOOD] *= i;
  produce_ptr->mtrls[MTRLS_FOOD] /= 100;

  /* check wood production increase */
  if (WIZ_MAGIC(MW_DRUIDISM)) {
    produce_ptr->mtrls[MTRLS_WOOD] *= 120;
    produce_ptr->mtrls[MTRLS_WOOD] /= 100;
  }

  /* mining production increase */
  if (CIV_MAGIC(MC_MINER)) {
    produce_ptr->mtrls[MTRLS_JEWELS] *= 120;
    produce_ptr->mtrls[MTRLS_JEWELS] /= 100;
    produce_ptr->mtrls[MTRLS_METALS] *= 120;
    produce_ptr->mtrls[MTRLS_METALS] /= 100;
  }
}

/* MGK_COST_ADJUST -- Just account for the adjustment of normal expensive */
void
mgk_cost_adjust PARM_2(int, type, itemtype *, expenses)
{
  int i;

  /* check input */
  if (expenses == NULL) return;

  /* go through the warrior powers */
  if (type & 1) {

    /* check for decreased soldier costs */
    if (MIL_MAGIC(MM_WARRIOR)) {
      i = 20;
      if (MIL_MAGIC(MM_CAPTAIN)) {
	i += 10;
      }
      if (MIL_MAGIC(MM_WARLORD)) {
	i += 10;
      }
      expenses[MTRLS_TALONS] *= (100 - i);
      expenses[MTRLS_TALONS] /= 100;
    }

    /* check for reduced monster support */
    if (MIL_MAGIC(MM_ORC)) {
      i = 20;
      if (MIL_MAGIC(MM_OGRE)) {
	i += 20;
      }
      if (MIL_MAGIC(MM_DRAGON)) {
	i += 20;
      }
      expenses[MTRLS_JEWELS] *= (100 - i); 
      expenses[MTRLS_JEWELS] /= 100;
    }
  }

  /* is it a naval purchase? */
  if (type & 2) {

    /* check for reduced fleet costs */
    if (CIV_MAGIC(MC_SAILOR)) {
      i = 20;
      if (CIV_MAGIC(MC_MARINE)) {
	i += 20;
      }
      expenses[MTRLS_TALONS] *= (100 - i);
      expenses[MTRLS_TALONS] /= 100;
    }

  }

  /* construction bonuses */
  if (type & 4) {

    /* check adjustments */
    if (CIV_MAGIC(MC_ARCHITECT)) {
      expenses[MTRLS_METALS] *= 80;
      expenses[MTRLS_METALS] /= 100;
      expenses[MTRLS_TALONS] *= 80;
      expenses[MTRLS_TALONS] /= 100;
      expenses[MTRLS_WOOD] *= 80;
      expenses[MTRLS_WOOD] /= 100;
    }

  }

  /* check for reduced jewel expenditures */
  if (CIV_MAGIC(MC_JEWELER)) {
    expenses[MTRLS_JEWELS] *= 80;
    expenses[MTRLS_JEWELS] /= 100;
  }

  /* check for reduced metal costs */
  if (CIV_MAGIC(MC_METALCRAFT)) {
    expenses[MTRLS_METALS] *= 80;
    expenses[MTRLS_METALS] /= 100;
  }

  /* check for better wood use */
  if (CIV_MAGIC(MC_WOODCRAFT)) {
    expenses[MTRLS_WOOD] *= 80;
    expenses[MTRLS_WOOD] /= 100;
  }
}

/* R10_REGION -- Determine the supply range (x10) of the supply center */
int
r10_region PARM_3(NTN_PTR, n1_ptr, CITY_PTR, c1_ptr, SCT_PTR, s1_ptr)
{
  int dval = 0, hold = 0;

  /* check it */
  if ((n1_ptr == NULL) ||
      (c1_ptr == NULL) ||
      (s1_ptr == NULL) ||
      /* check for non-supply centers */
      ((dval = major_desg(s1_ptr->designation)) <= MAJ_CACHE) ||
      /* check for full sieges and devastation */
      minor_desg(s1_ptr->designation, MIN_DEVASTATED | MIN_SIEGED)) {
    return(hold);
  }

  /* sites under contruction mean downgraded ranges */
  if ((c1_ptr->i_people < 0) &&
      (dval > MAJ_STOCKADE)) {
    dval--;
  }

  /* find the national communication range */
  hold = n1_ptr->attribute[BUTE_COMMRANGE];
  switch (dval) {
  case MAJ_STOCKADE:
    /* stockade is only 1/4 range; minimum 1 */
    hold /= 4;
    if (hold < 10) hold = 10;
    break;
  case MAJ_TOWN:
    /* town is only 1/2 range; minimum 2 */
    hold /= 2;
    if (hold < 20) hold = 20;
    break;
  case MAJ_CITY:
    /* full range; minimum 3 */
    if (hold < 30) hold = 30;
    break;
  case MAJ_CAPITAL:
    /* full range + 1 sector; minimum 3 */
    hold += 10;
    if (hold < 30) hold = 30;
    break;
  default:
    /* don't know how this could happen */
    break;
  }

  /* check the tradegood type */
  if (tg_communicate(s1_ptr->tradegood) &&
      (tg_info[s1_ptr->tradegood].need_desg <= dval) &&
      (tg_info[s1_ptr->tradegood].pop_support <= s1_ptr->people)) {
    hold += tg_info[s1_ptr->tradegood].value;
  }
  return(hold);
}

/* ATTRACT_VAL -- The attractiveness of a particular sector */
int
attract_val PARM_2(int, xloc, int, yloc)
{
  NTN_PTR n1_ptr;
  int sum = 0, cnt, cntry, ival, rval;

  /* check input */
  if (!XY_ONMAP(xloc, yloc)) {
    return(0);
  }
  if (ntn_ptr == NULL) {
    n1_ptr = world.np[cntry = sct[xloc][yloc].owner];
  } else {
    n1_ptr = ntn_ptr;
    cntry = country;
  }
  if (n1_ptr == NULL) return(0);
  rval = n1_ptr->race;

  /* can't move into a sieged or real estate sector */
  sct_tptr = &(sct[xloc][yloc]);
  if (minor_desg(sct_tptr->designation, MIN_FORSALE | MIN_SIEGED)) {
    return(0);
  }

  /* first find the vegetation attraction */
  switch (sct_tptr->vegetation) {
  case VEG_ICE:
  case VEG_DESERT:
    /* is there a dervish or destroyer nation? */
    if (MAGIC(n1_ptr->powers[MAG_CIVILIAN], MC_DERVISH) ||
	MAGIC(n1_ptr->powers[MAG_WIZARDRY], MW_DESTROYER)) {
      sum = 60;
    } else {
      return(0);
    }
    break;
  case VEG_SWAMP:
  case VEG_JUNGLE:
    /* any amphibian traits? */
    if (MAGIC(n1_ptr->powers[MAG_CIVILIAN], MC_AMPHIBIAN)) {
      sum = 50;
    } else {
      return(0);
    }
    break;
  default:
    /* just normal vegetation settings */
    sum = (int) veg_info[sct_tptr->vegetation].attract[rval];
    break;
  }

  /* now adjust due to elevation */
  sum *= (int) ele_info[sct_tptr->altitude].attract[rval];
  sum /= 100;
  if (sum == 0) return(0);

  /* compute in designation modifier */
  ival = major_desg(sct_tptr->designation);
  sum *= (int) maj_dinfo[ival].attract[rval];
  sum /= 100;
  if (sum == 0) return(0);
  switch (ival) {
  case MAJ_METALMINE:
    if (!tg_ok(sct_tptr, cntry)) {
      sum /= 4;
    } else {
      sum += 3 * metal_value(sct_tptr);
    }
    break;
  case MAJ_JEWELMINE:
    if (!tg_ok(sct_tptr, cntry)) {
      sum /= 4;
    } else {
      sum += 3 * jewel_value(sct_tptr);
    }
    break;
  case MAJ_LUMBERYARD:
    sum /= 2;
    sum += (towood(sct_tptr, cntry) * 3) / 2;
    break;
  default:
    if (IS_CITY(ival)) {
      if (MAGIC(n1_ptr->powers[MAG_CIVILIAN], MC_URBAN)) {
	sum *= 102;
	sum /= 100;
      }
    } else if (IS_FARM(ival)) {
      if ((world.np[cntry] != NULL) &&
	  ((world.np[cntry])->mtrls[MTRLS_FOOD] / 2 <
	   (world.np[cntry])->tciv)) {
	sum += 5 * tofood(sct_tptr, cntry);
      } else {
	sum += 3 * tofood(sct_tptr, cntry);
      }
      switch (SEASON(TURN)) {
      case WINTER_SEASON:
	/* no like; but no like leave */
	sum /= 4;
	break;
      case SPRING_SEASON:
	/* okay, lets get back in slowly */
	sum /= 2;
	break;
      case SUMMER_SEASON:
	/* should be creapin' up now */
	if (ival != MAJ_FERTILE) {
	  sum *= 2;
	  sum /= 3;
	} else {
	  sum *= 3;
	  sum /= 4;
	}
	break;
      case FALL_SEASON:
	/* jump on in thar */
	if (ival != MAJ_FRUITFUL) {
	  sum *= 2;
	  sum /= 3;
	}
	break;
      }
    }
    break;
  }

  /* now check minor designation adjustments */
  for (cnt = 0; cnt < MIN_NUMBER; cnt++) {
    if (minor_desg(sct_tptr->designation, MIN_START << cnt)) {
      sum *= (int) min_dinfo[cnt].attract[rval];
      sum /= 100;
    }
  }

  /* is it supported? */
  if ((ntn_ptr != NULL) &&
      (SUM_WEIGHTS(xloc, yloc) <= 0)) {
    /* just ain't as pretty */
    sum /= 8;
  }

  /* all done... */
  return(sum);
}

/* FIND_AREA -- This routine sets the limit indicators for a nation */
void
find_area PARM_1(int, ntnnum)
{
  register int i, j;
#ifndef SIMPLE_FIND
  int last_on, center_left, center_right;
  char *x_owned;
#endif

  /* verify the nation */
  if ((ntn_tptr = world.np[ntnnum]) == NULL) return;

#ifndef SIMPLE_FIND
  /* provide space for horizontal edge detection */
  if ((x_owned = (char *) malloc(sizeof(char) * MAPX)) == NULL) {
    fprintf(fupdate, "Memory allocation error in find_area()\n");
    abrt();
  }

  /* clear the array */
  for (i = 0; i < MAPX; i++) x_owned[i] = FALSE;
#endif /*SIMPLE_FIND*/

  /* first find the bottom bounds */
  ntn_tptr->bottomedge = (-1);
  for (j = 0; j < MAPY; j++) {
    for (i = 0; i < MAPX; i++) {

      /* check for the target */
      if (sct[i][j].owner == ntnnum) {
	ntn_tptr->bottomedge = j;
	break;
      }

    }

    /* check for end */
    if (ntn_tptr->bottomedge == j) break;
  }

  /* now find the lowest */
  ntn_tptr->topedge = (-1);
  for (j = MAPY - 1; j >= 0; j--) {
    for (i = 0; i < MAPX; i++) {

      /* check for the target */
      if (sct[i][j].owner == ntnnum) {
	ntn_tptr->topedge = j;
	break;
      }

    }

    /* check for end */
    if (ntn_tptr->topedge == j) break;
  }

  /* configure the horizontal locations */
  ntn_tptr->leftedge = MAPX;
  ntn_tptr->rightedge = 0;

  /* given those, search for left and right sides */
  for (j = ntn_tptr->bottomedge; j <= ntn_tptr->topedge; j++) {

    /* find the leftmost position */
    for (i = 0; i < ntn_tptr->leftedge; i++) {
      if (sct[i][j].owner == ntnnum) {
	ntn_tptr->leftedge = i;
#ifndef SIMPLE_FIND
	x_owned[i] = TRUE;
#endif
	break;
      }
    }

#ifndef SIMPLE_FIND
    /* mark all owned locations */
    for (; i < MAPX; i++) {
      if (sct[i][j].owner == ntnnum) {
	if (i > ntn_tptr->rightedge) ntn_tptr->rightedge = i;
	x_owned[i] = TRUE;
      }
    }
#else
    /* find the rightmost position */
    for (i = MAPX - 1; i > ntn_tptr->rightedge; i--) {
      if (sct[i][j].owner == ntnnum) {
	ntn_tptr->rightedge = i;
	break;
      }
    }
#endif /*SIMPLE_FIND*/
  }

#ifndef SIMPLE_FIND
  /* setting configurations */
  last_on = ntn_tptr->leftedge;
  center_left = center_right = (-1);

  /* first set the left edge */
  for (i = ntn_tptr->leftedge; i <= ntn_tptr->rightedge; i++) {

    /* check off and on status */
    if (last_on != i - 1) {
      if (x_owned[i] == TRUE) {
	if (center_right - center_left < i - last_on) {
	  center_right = i;
	  center_left = last_on;
	}
      }
    }

    /* mark that it is now within owned region */
    if (x_owned[i] == TRUE) {
      last_on = i;
    }

  }

  /* now compare the two large gaps */
  if (center_right - center_left >
      MAPX + ntn_tptr->leftedge - ntn_tptr->rightedge) {
    ntn_tptr->leftedge = center_right - MAPX;
    ntn_tptr->rightedge = center_left;
  }

  free(x_owned);
#endif /*SIMPLE_FIND*/
}

/* INFL_OF -- computer the inflation expense on some talons */
itemtype
infl_of PARM_1( itemtype, talon_value )
{
  /* check input */
  if ((ntn_ptr == NULL) || (talon_value == 0)) {
    return((itemtype) 0);
  }

  return((itemtype) ntn_ptr->attribute[BUTE_INFLATION] * abs(talon_value));
}

/* FIND_NEWARMYNUM -- Determine a new army unit number by the given utype */
int
find_newarmynum PARM_1(int, utype)
{
  ARMY_PTR a1_ptr;
  int hold;

  /* find the starting slot */
  if ((hold = unit_basenum(utype)) == EMPTY_HOLD) {
    hold++;
  }

  /* find the lowest number greater than or equal to hold */
  for (a1_ptr = ntn_ptr->army_list;
       a1_ptr != NULL;
       a1_ptr = a1_ptr->next) {
    /* increment if a unit already has that number */
    if (hold == a1_ptr->armyid) {
      do {
	hold++;
      } while (hold == EMPTY_HOLD);
    }
    else if (hold < a1_ptr->armyid) break;
  }
  return(hold);
}

/* ARMY_REORGANIZE -- Reorganize all of the unit numbers */
void
army_reorganize PARM_0(void)
{
  ARMY_PTR army_next;
  int oldnumber;

  /* check the settings */
  if (ntn_ptr == NULL) return;

  /* go through the list of army units */
  army_ptr = ntn_ptr->army_list;
  ntn_ptr->army_list = NULL;
  for (/* DONE: army_ptr = ntn_ptr->army_list */;
       army_ptr != NULL;
       army_ptr = army_next) {
    /* ready for next unit */
    army_next = army_ptr->next;

    /* renumber and trace through the naval units */
    oldnumber = ARMY_ID;
    army_renum(find_newarmynum(ARMY_TYPE));

    /* make sure to renumber any additional references as well */
    if (a_isleader(ARMY_TYPE)) {
      for (army_tptr = army_next;
	   army_tptr != NULL;
	   army_tptr = army_tptr->next) {
	if (ARMYT_LEAD == oldnumber) {
	  ARMYT_LEAD = ARMY_ID;
	}
      }
    }

    /* next step */
    army_ptr->next = ntn_ptr->army_list;
    ntn_ptr->army_list = army_ptr;
    army_sort(TRUE);
  }
  army_sort(FALSE);
}

/* ADJUST_ATTRIBUTE -- Shift a given national attribute by a given amount */
void
adjust_attribute PARM_3(short *, bute_list, int, butenum, int, amount)
{
  int hold;

  /* check the input */
  if (bute_list == NULL) return;
  if ((butenum < 0) || (butenum >= BUTE_NUMBER)) return;

  /* now perform the adjustment */
  hold = bute_list[butenum] + amount;
  if (hold > bute_info[butenum].max_base) {
    bute_list[butenum] = bute_info[butenum].max_base;
  } else if (hold < bute_info[butenum].min_base) {
    bute_list[butenum] = bute_info[butenum].min_base;
  } else {
    bute_list[butenum] = hold;
  }
}

/* MERC_COSTRATE -- % of going rate that the current nation pays for mercs */
int
merc_costrate PARM_0(void)
{
  /* depends on nation pointer */
  if (ntn_ptr == NULL) return(100);
  return (150 - (int)ntn_ptr->attribute[BUTE_MERCREP]);
}
