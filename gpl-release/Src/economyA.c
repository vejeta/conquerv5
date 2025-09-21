// SPDX-License-Identifier: GPL-3.0-or-later
/* This file contains updating routines for world economics */
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
#include "dataA.h"
#include "armyX.h"
#include "cityX.h"
#include "itemX.h"
#include "navyX.h"
#include "butesX.h"
#include "calenX.h"
#include "desigX.h"
#include "racesX.h"
#include "magicX.h"
#include "mtrlsX.h"
#include "worldX.h"
#include "activeX.h"
#include "elevegX.h"
#include "statusX.h"
#include "caravanX.h"
#include "dstatusX.h"

/* production information */
extern itemtype j_produced, m_produced;

/* char value from the update.c file */
extern char *sct_status;

/* quick macro for status references */
#define SCT_STATUS(x, y)	sct_status[(x) + (y) * MAPX]

/* UPD_SECTORS -- Set sieges and other world sector statuses */
void
upd_sectors PARM_0(void)
{
  int i, j;

  /* intro */
  fprintf(fupdate, "=== Assigning sieges and devastation\n");

  /* check each and every sector and clear out old sieges */
  for (i = 0; i < MAPX; i++) {
    for (j = 0; j < MAPY; j++) {

      /* check it out */
      sct_ptr = &(sct[i][j]);

      /* check statuses */
      if (SCT_STATUS(i, j) & SET_SIEGE) {
	set_minordesg(sct_ptr->designation, MIN_SIEGED);
      } else {
	clr_minordesg(sct_ptr->designation, MIN_SIEGED);
      }

      /* check if devastation is neccesary */
      if (SCT_STATUS(i, j) & SET_DEVASTATE) {
	set_minordesg(sct_ptr->designation, MIN_DEVASTATED);
      }

    }
  }

  /* now set the newly determined nation boundaries */
  for (country = 1; country < MAXNTN; country++) {
    find_area(country);
  }

}

/* UPD_PRODUCE -- Transfer produced goods to each central point */
void
upd_produce PARM_0(void)
{
  SHEET_STRUCT use_sht;
  SHEET_PTR prod_ptr;
  int i, adjust_told;

  /* initialize stuff */
  fprintf(fupdate, "=== Gathering Produced Resources\n");
  msg_ginit("Resource Production Report");
  j_produced = m_produced = 1;

  /* around the world */
  for (country = 1; country < MAXNTN; country++) {

    /* assign the proper weights and begin record keeping */
    if ((ntn_ptr = world.np[country]) == NULL) continue;
    if (n_ismonster(ntn_ptr->active)) continue;
    clr_memory( (char *) &use_sht, sizeof(SHEET_STRUCT) );
    set_weights(FALSE);
    adjust_made = FALSE;
    adjust_told = FALSE;
    adjust_xloc = -1;
    adjust_yloc = -1;

    /* traverse the regions */
    for (city_ptr = ntn_ptr->city_list;
	 city_ptr != NULL;
	 city_ptr = city_ptr->next) {

      /* get the production */
      if ((prod_ptr = region_produce(CITY_NAME, FALSE)) == NULL)
	continue;

      /* fix the production */
      adjust_production(prod_ptr, FALSE);

      /* start regional report */
      msg_grouped(country, -1, -1, "");
      i = major_desg(sct[CITY_XLOC][CITY_YLOC].designation);
      sprintf(string, "%s %s Regional Production",
	      maj_dinfo[i].name, CITY_NAME);
      msg_grouped(country, CITY_XLOC, CITY_YLOC, string);
      sprintf(string, "%d sectors contribute raw materials",
	      prod_ptr->sectors);
      msg_grouped(country, CITY_XLOC, CITY_YLOC, string);

      /* check for mine dilutions */
      if ((adjust_made == TRUE) &&
	  (adjust_told == FALSE) &&
	  XY_ONMAP(adjust_xloc, adjust_yloc)) {
	SCT_PTR s1_ptr = &sct[adjust_xloc][adjust_yloc];
	sprintf(string, "%s reports mine is being overworked",
		maj_dinfo[major_desg(s1_ptr->designation)].name);
	msg_grouped(country, adjust_xloc, adjust_yloc,
		    string);
	adjust_told = TRUE;
      }

      /* accumulate raw materials */
      for (i = 0; i < MTRLS_NUMBER; i++) {

	/* report it */
	if (prod_ptr->mtrls[i] > 0) {
	  sprintf(string, "  %ld %s",
		  prod_ptr->mtrls[i], mtrls_info[i].name);
	  msg_grouped(country, -1, -1, string);
	}

	/* record it */
	use_sht.mtrls[i] += prod_ptr->mtrls[i];
	if (CITY_MTRLS[i] + prod_ptr->mtrls[i] > BIGITEM) {
	  CITY_MTRLS[i] = BIGITEM;
	} else {
	  CITY_MTRLS[i] += prod_ptr->mtrls[i];
	}
      }

      /* special tracking of jewels */
      if (j_produced + prod_ptr->mtrls[MTRLS_JEWELS] > BIGITEM) {
	j_produced = BIGITEM;
      } else {
	j_produced += prod_ptr->mtrls[MTRLS_JEWELS];
      }

      /* special tracking of metals */
      if (m_produced + prod_ptr->mtrls[MTRLS_METALS] > BIGITEM) {
	m_produced = BIGITEM;
      } else {
	m_produced += prod_ptr->mtrls[MTRLS_METALS];
      }

      /* get rid of the memory */
      free(prod_ptr);
    }

    /* now send out the report */
    msg_grouped(country, -1, -1, "");
    msg_grouped(country, -1, -1, "National Production Summary");
    for (i = 0; i < MTRLS_NUMBER; i++) {
      /* report it */
      if (use_sht.mtrls[i] > 0) {
	sprintf(string, "  %ld %s",
		use_sht.mtrls[i], mtrls_info[i].name);
	msg_grouped(country, -1, -1, string);
      }
    }
  }

  /* finish off the messages */
  msg_gfinish();
}

/* UPD_CONSUME -- Distribute goods as needed */
void
upd_consume PARM_0(void)
{
  ARMY_PTR atmp_ptr, anext_ptr = NULL, alast_ptr = NULL;
  NAVY_PTR nnext_ptr = NULL, nlast_ptr = NULL;
  CVN_PTR vnext_ptr = NULL, vlast_ptr = NULL;
  SHEET_STRUCT use_sht, spend_val;
  ITEM_STRUCT spend_item;
  register int i, j;
  int xhold, pval = 0, k, tval, splyok, splytest;
  int is_supplying, db_armynews, db_popnews;
#ifdef NOT_DONE
  int sown;
#endif /* NOT_DONE */
  long lval;

  /* initialize */
  fprintf(fupdate, "=== Distributing Consumed Resources\n");
  msg_ginit("Resource Consumption Report");
  for (country = 1; country < MAXNTN; country++) {

    /* monsters go home! */
    if ((ntn_ptr = world.np[country]) == NULL) continue;
    if (n_notactive(ntn_ptr->active)) continue;

    /* initialize news listings */
    db_armynews = db_popnews = FALSE;
    if (n_ismonster(ntn_ptr->active)) {
      goto military_reset;
    }

    /* now set the weights */
    set_weights(FALSE);

    /* begin the resource counting */
    clr_memory( (char *) &use_sht, sizeof(SHEET_STRUCT) );

    /* calculate consumption over entire nation */
    for (i = ntn_ptr->leftedge; i <= ntn_ptr->rightedge; i++) {
      for (j = ntn_ptr->bottomedge; j <= ntn_ptr->topedge; j++) {

	/* are the sector resources available? */
	xhold = (i + MAPX) % MAPX;
	if (((sct_ptr = &(sct[xhold][j])) == NULL) ||
	    (sct_ptr->owner != country)) continue;

	/* determine support costs */
	splytest = minor_desg(sct_ptr->designation, MIN_SIEGED);
	if ((item_tptr = find_resources(xhold, j, splytest)) == NULL) {
	  for (k = 0; k < MTRLS_NUMBER; k++) {
	    spend_item.mtrls[k] = (itemtype) 0;
	  }
	} else {
	  for (k = 0; k < MTRLS_NUMBER; k++) {
	    spend_item.mtrls[k] = ITEMT_MTRLS[k];
	  }
	  free(item_tptr);
	}

	/* check the costs */
	sector_consume(xhold, j, &spend_val);

	/* determine if there is enough */
	if ((spend_item.mtrls[MTRLS_FOOD] < spend_val.mtrls[MTRLS_FOOD]) &&
	    (spend_val.mtrls[MTRLS_FOOD] > 0)) {

	  /* famine? */
	  if (sct_ptr->people > 0) {

	    /* N% starve maxiumum */
	    pval = (PSTARVE *
		    (spend_val.mtrls[MTRLS_FOOD] -
		     spend_item.mtrls[MTRLS_FOOD])) /
		       spend_val.mtrls[MTRLS_FOOD];
	    lval = (pval * sct_ptr->people) / 100;
	    if (lval > 0) {
	      adjust_attribute(ntn_ptr->attribute, BUTE_POPULARITY, -4);
	      sct_ptr->people -= lval;
	      sprintf(string, "%ld people starve from lack of food",
		    lval);
	      msg_grouped(country, xhold, j, string);
	      if (db_popnews == FALSE) {
		adjust_attribute(ntn_ptr->attribute, BUTE_EATRATE, -5);
		db_popnews = TRUE;
		fprintf(fnews, "2.\tfamine occurs in nation %s\n",
			ntn_ptr->name);
	      }
	    }
	  }

	} else {
	  spend_item.mtrls[MTRLS_FOOD] = spend_val.mtrls[MTRLS_FOOD];
	}

	/* now spend the money */
	spend_item.mtrls[MTRLS_TALONS] = spend_val.mtrls[MTRLS_TALONS];

	/* pass out the jewels */
	if (spend_item.mtrls[MTRLS_JEWELS] < spend_val.mtrls[MTRLS_JEWELS]) {
	  /* UNIMPLEMENTED: PCOLLAPSE */
	} else {
	  spend_item.mtrls[MTRLS_JEWELS] = spend_val.mtrls[MTRLS_JEWELS];
	}

	/* now divy out the metals */
	if (spend_item.mtrls[MTRLS_METALS] < spend_val.mtrls[MTRLS_METALS]) {
	  /* UNIMPLEMENTED: PCOLLAPSE */
	} else {
	  spend_item.mtrls[MTRLS_METALS] = spend_val.mtrls[MTRLS_METALS];
	}

	/* finally, handle any wood */
	if (spend_item.mtrls[MTRLS_WOOD] < spend_val.mtrls[MTRLS_WOOD]) {
	  /* UNIMPLEMENTED: PCOLLAPSE */
	} else {
	  spend_item.mtrls[MTRLS_WOOD] = spend_val.mtrls[MTRLS_WOOD];
	}

	/* now make sure to account for the spending */
	for (k = 0; k < MTRLS_NUMBER; k++) {
	  use_sht.mtrls[k] += spend_item.mtrls[k];
	}
	use_sht.sectors++;
	use_sht.people += sct_ptr->people;
	take_resources(xhold, j, &spend_item, splytest);

      }
    }

    /* army spending */
  military_reset:
    for (army_ptr = ntn_ptr->army_list;
	 army_ptr != NULL;
	 alast_ptr = army_ptr, army_ptr = anext_ptr) {

      /* which is the next unit */
      anext_ptr = army_ptr->next;

      /* get rid of empty units */
      if (ARMY_SIZE <= 0) goto army_goodbye;

      /* check for monster nations */
      if (n_ismonster(ntn_ptr->active)) continue;

      /* skip out of this world units */
      if (!XY_INMAP(ARMY_XLOC, ARMY_YLOC)) continue;

      /* is supplying needed? */
      if ((MAXSUPPLIES == 0) ||
	  a_freesupply(ARMY_TYPE)) continue;

      /* get needs */
      army_support(army_ptr, &(spend_val.mtrls[0]), 1);

      /* where can it be supplied from? */
      splyok = TRUE;
#ifdef NOT_DONE
      if (((sown = sct[ARMY_XLOC][ARMY_YLOC].owner) != country) &&
	  (sown != UNOWNED) &&
	  (world.np[sown]->dstatus[country] != DIP_ALLIED) &&
	  (world.np[sown]->dstatus[country] != DIP_TREATY)) {
	/* must gain supplies from within sector itself */
	splytest = TRUE;
      } else {
#endif /* NOT_DONE */
	/* depends on the status */
	splytest = splyinsect_stat(ARMY_STAT);
#ifdef NOT_DONE
      }
#endif /* NOT_DONE */

      /* get support from region */
      if ((item_tptr = find_resources(ARMY_XLOC, ARMY_YLOC, splytest))
	  != NULL) {

	/* now check if there is enough */
	for (k = 0; k < MTRLS_NUMBER; k++) {
	  if ((spend_val.mtrls[k] > 0) &&
	      (ITEMT_MTRLS[k] < spend_val.mtrls[k])) {
	    splyok = FALSE;
	  }
	}
	free(item_tptr);

      } else {
	/* no supplies for this guy */
	splyok = FALSE;
      }

      /* if so, buy it */
      if (splyok == TRUE) {
	/* set the limits */
	for (k = 0; k < MTRLS_NUMBER; k++) {
	  spend_item.mtrls[k] = spend_val.mtrls[k];
	}
	take_resources(ARMY_XLOC, ARMY_YLOC, &spend_item, splytest);

	/* record any support */
	for (k = 0; k < MTRLS_NUMBER; k++) {
	  use_sht.mtrls[k] += spend_val.mtrls[k];
	}
	if (a_ismonster(ARMY_TYPE)) {
	  use_sht.monst_troops += ARMY_SIZE;
	  use_sht.monst_jewels += spend_val.mtrls[MTRLS_JEWELS];
	} else {
	  use_sht.army_men += ARMY_SIZE;
	  use_sht.army_cost += spend_val.mtrls[MTRLS_TALONS];
	}
	ARMY_SPLY++;
      }

      /* now check for exposure in sectors */
      if (a_isnormal(ARMY_TYPE) &&
	  !fort_stat(ARMY_STAT) &&
	  (citybyloc(ntn_ptr, ARMY_XLOC, ARMY_YLOC) == NULL)) {

	/* get the exposure value of the sector */
	pval = exposure_value(ntn_ptr, &(sct[ARMY_XLOC][ARMY_YLOC]));

	/* zombies have reduced exposure values */
	if (a_isundead(ARMY_TYPE)) {
	  pval /= 2;
	}

	/* check for the losses */
	if (pval > 0) {

	  /* scouting type units die on a percentage basis */
	  if (a_isscout(ARMY_TYPE)) {

	    /* does the scout just die, lost and forgotten? */
	    if (rand_val(100) < pval) {
	      ARMY_SIZE = 0;
	    }

	  } else {

	    /* some of the soldiers die off */
	    if (ARMY_SIZE > 9) {
	      lval = (ARMY_SIZE * rand_val(pval + 1)) / 100;
	    } else {
	      lval = rand_val(ARMY_SIZE) + 1;
	    }
	    if (lval > 0) {
	      ARMY_SIZE -= lval;
	      if (ARMY_SIZE > 0) {
		sprintf(string, "Your %s %d loses %ld men due to %s",
			ainfo_list[ARMY_TYPE].name, ARMY_ID, lval,
			(SEASON(TURN) == WINTER_SEASON)?
			"exposure":"the environment");
		msg_grouped(country, ARMY_XLOC, ARMY_YLOC, string);
		if (a_ismerc(ARMY_TYPE)) {
		  adjust_attribute(ntn_ptr->attribute, BUTE_MERCREP, -1);
		}
	      } else if (a_ismerc(ARMY_TYPE)) {
		adjust_attribute(ntn_ptr->attribute, BUTE_MERCREP, -2);
	      }
	    }

	  }

	}

      }

      /* decrease supplies */
      if (ARMY_SPLY > 0) {
	ARMY_SPLY--;
	if (a_ismerc(ARMY_TYPE) &&
	    ((int)ARMY_SPLY >= MAXSUPPLIES) &&
	    (ARMY_SIZE >= 500) &&
	    (ntn_ptr->attribute[BUTE_MERCREP] < 75)) {
	  /* well supplied mercs are happy mercs */
	  adjust_attribute(ntn_ptr->attribute, BUTE_MERCREP, 1);
	}
      } else {

	/* check for units getting discombobulated */
	if (a_ismonster(ARMY_TYPE)) {

	  /* they leave due to lack of jewels */
	  if (PDISBAND * 2 > rand_val(100)) {
	    make_madunit(-1, ARMY_TYPE, ARMY_SIZE, ARMY_XLOC, ARMY_YLOC);
	    sprintf(string, "%ld %s in unit %d leave due to lack of jewels",
		    ARMY_SIZE, ainfo_list[ARMY_TYPE].name, ARMY_ID);
	    msg_grouped(country, ARMY_XLOC, ARMY_YLOC, string);
	  } else {
	    make_madunit(-1, ARMY_TYPE, ARMY_SIZE, ntn_ptr->capx,
			 ntn_ptr->capy);
	    sprintf(string,
		    "%ld %s in unit %d turn on your own troops in search of jewels",
		    ARMY_SIZE, ainfo_list[ARMY_TYPE].name, ARMY_ID);
	    msg_grouped(country, ARMY_XLOC, ARMY_YLOC, string);
	  }
	  ARMY_SIZE = 0;

	} else if (a_isnormal(ARMY_TYPE)) {

	  /* check for desertions */
	  if (unit_status(ARMY_TYPE) != ST_GROUPED) {

	    if (a_ismerc(ARMY_TYPE) ||
		(ARMY_SIZE < 10)) {
	      pval = 100;
	    } else {
	      pval = PDISBAND;
	    }
	    lval = (ARMY_SIZE * pval) / 100;
	    if (lval <= 0) lval = 1;
	    ARMY_SIZE -= lval;
	    MERCMEN += lval;
	    if (ARMY_SIZE > 0) {
	      sprintf(string, "%s Army %d loses %ld deserters",
		      ainfo_list[ARMY_TYPE].name, ARMY_ID, lval);
	      msg_grouped(country, ARMY_XLOC, ARMY_YLOC, string);
	    }
	    if (db_armynews == FALSE) {
	      db_armynews = TRUE;
	      fprintf(fnews, "2.\tsoldiers in nation %s desert\n",
		      ntn_ptr->name);
	    }
	    if (a_ismerc(ARMY_TYPE)) {
	      /* good way to teach them a lesson */
	      adjust_attribute(ntn_ptr->attribute, BUTE_MERCREP, -20);
	      make_madunit(-1, ARMY_TYPE, lval, ARMY_XLOC, ARMY_YLOC);
	    }
	  }

	  /* now check for starvation */
	  if (!a_isscout(ARMY_TYPE)) {

	    /* men die */
	    lval = (ARMY_SIZE * rand_val(PSTARVE + 1)) / 100;
	    if (lval <= 1) lval = 1;
	    ARMY_SIZE -= lval;
	    sprintf(string, "%ld men in army unit %s %d starve without food",
		    lval, ainfo_list[ARMY_TYPE].name, ARMY_ID);
	    msg_grouped(country, ARMY_XLOC, ARMY_YLOC, string);

	    /* rest are weakened */
	    if (ARMY_EFF > 35) {
	      ARMY_EFF -= 10;
	    } else {
	      ARMY_EFF = 25;
	    }

	    /* some permanent weakness */
	    if (ARMY_MAXEFF > 95) {
	      ARMY_MAXEFF--;
	    }

	  }

	}

      }

      /* remove any empty units */
      if (ARMY_SIZE <= 0) {
      army_goodbye:
	/* be sure to clean up kiddies */
	if (unit_leading(ARMY_STAT) ||
	    a_isleader(ARMY_TYPE)) {
	  for (atmp_ptr = ntn_ptr->army_list;
	       atmp_ptr != NULL;
	       atmp_ptr = atmp_ptr->next) {
	    /* clean up them doggies */
	    if (atmp_ptr->leader == ARMY_ID) {
	      atmp_ptr->leader = ARMY_LEAD;
	      if (unit_status(atmp_ptr->status) == ST_GROUPED) {
		set_status(atmp_ptr->status,
			   unit_status(ARMY_STAT));
	      }
	    }
	  }
	}

	army_ptr->next = NULL;
	if (army_ptr == ntn_ptr->army_list) {
	  free(army_ptr);
	  ntn_ptr->army_list = anext_ptr;
	  army_ptr = NULL;
	} else {
	  free(army_ptr);
	  alast_ptr->next = anext_ptr;
	  army_ptr = alast_ptr;
	}
      }
    }

    /* now check the ship supplying */
    for (navy_ptr = ntn_ptr->navy_list;
	 navy_ptr != NULL;
	 nlast_ptr = navy_ptr, navy_ptr = nnext_ptr) {

      /* what comes after this? */
      nnext_ptr = navy_ptr->next;

      /* get rid of any empty units */
      tval = 0;
      for (i = 0; i < NSHP_NUMBER; i++) {
	tval |= NAVY_SHIPS[i];
      }
      if ((tval == 0) ||
	  (NAVY_CREW == 0)) goto navy_goodbye;

      /* check for monster nations */
      if (n_ismonster(ntn_ptr->active)) continue;

      /* ship invalid locations */
      if (!XY_INMAP(NAVY_XLOC, NAVY_YLOC)) continue;

      /* store the ship size */
      xhold = 0;
      for (i = 0; i < NSHP_NUMBER; i++) {
	xhold += navy_holds(navy_ptr, i);
      }

      /* check support costs */
      if (MAXSUPPLIES > 0) {

	/* try to purchase more supplies */
	navy_support(navy_ptr, &(spend_val.mtrls[0]), 1);

	/* check for turning of the self supplying */
	if (supply_stat(NAVY_STAT) &&
	    !selfsply_stat(NAVY_STAT) &&
	    (NAVY_SPLY > 1)) {
	  if (onship_stat(NAVY_STAT)) {
	    set_status(NAVY_STAT, ST_ONBOARD);
	  } else {
	    set_status(NAVY_STAT, ST_CARRY);
	  }
	  is_supplying = TRUE;
	} else {
	  is_supplying = FALSE;
	}

	/* get support from region */
	splyok = TRUE;
	if ((item_tptr =
	     find_resources(NAVY_XLOC, NAVY_YLOC,
			    (sct[NAVY_XLOC][NAVY_YLOC].altitude
			     == ELE_WATER))) != NULL) {

	  /* now check if there is enough */
	  for (k = 0; k < MTRLS_NUMBER; k++) {
	    if ((spend_val.mtrls[k] > 0) &&
		(ITEMT_MTRLS[k] < spend_val.mtrls[k])) {
	      splyok = FALSE;
	    }
	  }
	  free(item_tptr);

	} else {
	  /* no supplies for this guy */
	  splyok = FALSE;
	}

	/* buy the supplies */
	if (splyok == TRUE) {
	  NAVY_SPLY++;
	  use_sht.ship_holds += xhold;
	  for (k = 0; k < MTRLS_NUMBER; k++) {
	    spend_item.mtrls[k] = spend_val.mtrls[k];
	    use_sht.mtrls[k] += spend_val.mtrls[k];
	  }
	  use_sht.navy_cost += spend_val.mtrls[MTRLS_TALONS];
	  take_resources(NAVY_XLOC, NAVY_YLOC, &spend_item, TRUE);
	}

	/* check for the supplying */
	if (is_supplying == TRUE) {
	  if (onship_stat(NAVY_STAT)) {
	    set_status(NAVY_STAT, ST_ONBSPLY);
	  } else {
	    set_status(NAVY_STAT, ST_SUPPLY);
	  }
	}

	/* now adjust the supplying */
	if (NAVY_SPLY == 0) {

	  /* check for deterioration */
	  lval = rand_val(PBARNICLES + 1);
	  if (lval > 0) {

	    /* simple wear? */
	    if (NAVY_EFF[0] > 25) {

	      /* deterioration */
	      if (NAVY_EFF[0] - lval < 25) {
		NAVY_EFF[0] = 25;
	      } else {
		NAVY_EFF[0] -= lval;
	      }

	    } else {

	      /* possible sinking */
	      if (lval / 2 > rand_val((PBARNICLES + 1) * 2)) {
		/* SINK ONE SHIP: UNIMPLEMENTED */
	      }

	    }
	  }

	  /* now check for crew starvation */
	  lval = ((int) NAVY_CREW * rand_val(PSTARVE + 1)) / 100;
	  if (lval > 0) {
	    NAVY_CREW -= lval;
	    sprintf(string,
		    "%ld men in the crew of fleet %d starve",
		    lval * xhold, NAVY_ID);
	    msg_grouped(country, NAVY_XLOC, NAVY_YLOC, string);
	  }

	} else {
	  NAVY_SPLY--;
	}

      }

      /* remove any empty units */
      tval = 0;
      for (i = 0; i < NSHP_NUMBER; i++) {
	tval |= NAVY_SHIPS[i];
      }
      if (tval == 0) {
      navy_goodbye:
	navy_ptr->next = NULL;
	free(navy_ptr);
	if (navy_ptr == ntn_ptr->navy_list) {
	  ntn_ptr->navy_list = nnext_ptr;
	  navy_ptr = NULL;
	} else {
	  nlast_ptr->next = nnext_ptr;
	  navy_ptr = nlast_ptr;
	}
      }
    }

    /* now check the caravan supplying */
    for (cvn_ptr = ntn_ptr->cvn_list;
	 cvn_ptr != NULL;
	 vlast_ptr = cvn_ptr, cvn_ptr = vnext_ptr) {

      /* what comes after this baby? */
      vnext_ptr = cvn_ptr->next;

      /* get rid of any empty units */
      if ((CVN_SIZE == 0) ||
	  (CVN_CREW == 0)) goto cvn_goodbye;

      /* check for monster nations */
      if (n_ismonster(ntn_ptr->active)) continue;

      /* ship invalid locations */
      if (!XY_INMAP(CVN_XLOC, CVN_YLOC)) continue;

      /* store the caravan size */
      xhold = CVN_SIZE;

      /* check for unit support costs */
      if (MAXSUPPLIES != 0) {

	/* find the amount of supplies needed */
	cvn_support(cvn_ptr, &(spend_val.mtrls[0]), 1);
	splyok = TRUE;
	splytest = splyinsect_stat(CVN_STAT);

	/* check for turning of the self supplying */
	if (supply_stat(CVN_STAT) &&
	    !selfsply_stat(CVN_STAT) &&
	    (CVN_SPLY > 1)) {
	  if (onship_stat(CVN_STAT)) {
	    set_status(CVN_STAT, ST_ONBOARD);
	  } else {
	    set_status(CVN_STAT, ST_CARRY);
	  }
	  is_supplying = TRUE;
	} else {
	  is_supplying = FALSE;
	}

	/* get support from region */
	if ((item_tptr = find_resources(CVN_XLOC, CVN_YLOC, splytest))
	    != NULL) {

	  /* now check if there is enough */
	  for (k = 0; k < MTRLS_NUMBER; k++) {
	    if ((spend_val.mtrls[k] > 0) &&
		(ITEMT_MTRLS[k] < spend_val.mtrls[k])) {
	      splyok = FALSE;
	    }
	  }
	  free(item_tptr);

	} else {
	  /* no supplies for this guy */
	  splyok = FALSE;
	}

	/* if so, buy it */
	if (splyok == TRUE) {
	  /* set the limits and record spending */
	  for (k = 0; k < MTRLS_NUMBER; k++) {
	    spend_item.mtrls[k] = spend_val.mtrls[k];
	    use_sht.mtrls[k] += spend_val.mtrls[k];
	  }
	  take_resources(CVN_XLOC, CVN_YLOC, &spend_item, splytest);
	  use_sht.cvn_cost += spend_val.mtrls[MTRLS_TALONS];
	  use_sht.caravan_wagons += CVN_SIZE * WAGONS_IN_CVN;
	  CVN_SPLY++;
	}

	/* check for the supplying */
	if (is_supplying == TRUE) {
	  if (onship_stat(CVN_STAT)) {
	    set_status(CVN_STAT, ST_ONBSPLY);
	  } else {
	    set_status(CVN_STAT, ST_SUPPLY);
	  }
	}

	/* now try to do the supplying */
	if (CVN_SPLY == 0) {

	  /* check for deterioration */
	  lval = rand_val(PBARNICLES + 1);
	  if (lval > 0) {

	    /* simple wear? */
	    if (CVN_EFF > 25) {

	      /* deterioration */
	      if (CVN_EFF - lval < 25) {
		CVN_EFF = 25;
	      } else {
		CVN_EFF -= lval;
	      }

	    } else {

	      /* possible destruction of wagons */
	      if (lval / 2 > rand_val((PBARNICLES + 1) * 2)) {
		/* DESTROY TEN WAGONS: UNIMPLEMENTED */
	      }

	    }
	  }

	  /* now check for crew starvation */
	  lval = ((int) CVN_CREW * rand_val(PSTARVE + 1)) / 100;
	  if (lval > 0) {
	    CVN_CREW -= lval;
	    sprintf(string,
		    "%ld men in the crew of caravan %d starve",
		    lval * xhold, CVN_ID);
	    msg_grouped(country, CVN_XLOC, CVN_YLOC, string);
	  }

	} else {
	  /* consume the supplies */
	  CVN_SPLY--;
	}

      }

      /* remove any empty units */
      if (CVN_SIZE == 0) {
      cvn_goodbye:
	cvn_ptr->next = NULL;
	free(cvn_ptr);
	if (cvn_ptr == ntn_ptr->cvn_list) {
	  ntn_ptr->cvn_list = vnext_ptr;
	  cvn_ptr = NULL;
	} else {
	  vlast_ptr->next = vnext_ptr;
	  cvn_ptr = vlast_ptr;
	}
      }

      /* keep track of the last one */
      cvn_tptr = cvn_ptr;
    }

    /* now reset the city information */
    for (city_ptr = ntn_ptr->city_list;
	 city_ptr != NULL;
	 city_ptr = city_ptr->next) {

      /* perform contruction */
      if (CITY_PEOPLE < 0) {
	CITY_PEOPLE++;
      }

      /* account for inflation */
      use_sht.type_talons[0] += infl_of(CITY_MTRLS[MTRLS_TALONS]);
      CITY_MTRLS[MTRLS_TALONS] -= infl_of(CITY_MTRLS[MTRLS_TALONS]);

      /* jewel supply is taken */
      if (CITY_MTRLS[MTRLS_TALONS] < 0) {
	if (CITY_MTRLS[MTRLS_JEWELS] < (-CITY_MTRLS[MTRLS_TALONS] / 10)) {
	  sprintf(string, "%ld jewels from %s used to pay off debts",
		  CITY_MTRLS[MTRLS_JEWELS], CITY_NAME);
	  CITY_MTRLS[MTRLS_JEWELS] = 0;
	} else {
	  CITY_MTRLS[MTRLS_JEWELS] += CITY_MTRLS[MTRLS_TALONS] / 10;
	  sprintf(string, "%ld jewels from %s used to pay of debts",
		  - CITY_MTRLS[MTRLS_TALONS] / 10, CITY_NAME);
	}
	use_sht.mtrls[MTRLS_JEWELS] -= CITY_MTRLS[MTRLS_TALONS] / 10;
	msg_grouped(country, CITY_XLOC, CITY_YLOC, string);
	CITY_MTRLS[MTRLS_TALONS] = 0;
      }

      /* reestablish initial settings */
      if (CITY_PEOPLE >= 0) {
	if (XY_INMAP(CITY_XLOC, CITY_YLOC)) {
	  CITY_PEOPLE = (PVOLUNTEERS *
			 sct[CITY_XLOC][CITY_YLOC].people) / 100;
	} else {
	  CITY_PEOPLE = 0;
	}
      }

      /* now the raw materials */
      for (k = 0; k < MTRLS_NUMBER; k++) {
	if (CITY_MTRLS[k] > 0) {
	  CITY_IMTRLS[k] = CITY_MTRLS[k];
	} else {
	  CITY_IMTRLS[k] = (itemtype) 0;
	}
      }
      CITY_STALONS = CITY_MTRLS[MTRLS_TALONS];

    }
    use_sht.mtrls[MTRLS_TALONS] += use_sht.type_talons[0];

    /* produce some consumption summary messages */
    msg_grouped(country, -1, -1, "");
    if (use_sht.sectors > 0) {
      sprintf(string, "%d Sectors with %ld Civilians supported",
	      use_sht.sectors, use_sht.people);
      msg_grouped(country, -1, -1, string);
    }
    if (use_sht.army_men > 0) {
      sprintf(string, "%ld Soldiers supplied %ld talons",
	      use_sht.army_men, use_sht.army_cost);
      msg_grouped(country, -1, -1, string);
    }
    if (use_sht.monst_troops > 0) {
      sprintf(string, "%d Monsters supplied %ld jewels",
	      use_sht.monst_troops, use_sht.monst_jewels);
      msg_grouped(country, -1, -1, string);
    }
    if (use_sht.ship_holds > 0) {
      sprintf(string, "%d Naval holds supplied %ld talons",
	      use_sht.ship_holds, use_sht.navy_cost);
      msg_grouped(country, -1, -1, string);
    }
    if (use_sht.caravan_wagons > 0) {
      sprintf(string, "%d Caravan wagons supplied %ld talons",
	      use_sht.caravan_wagons, use_sht.cvn_cost);
      msg_grouped(country, -1, -1, string);
    }
    if (use_sht.type_talons[0] > 0) {
      sprintf(string, "%ld talons lost due to inflation",
	      use_sht.type_talons[0]);
      msg_grouped(country, -1, -1, string);
    } else if (use_sht.type_talons[0] < 0) {
      sprintf(string, "%ld talons gained due to deflation",
	      use_sht.type_talons[0]);
      msg_grouped(country, -1, -1, string);
    }
      
    /* now report total concumption amounts */
    msg_grouped(country, -1, -1, "");
    msg_grouped(country, -1, -1, "Total Resources Consumed:");
    for (k = 0; k < MTRLS_NUMBER; k++) {
      if (use_sht.mtrls[k] > 0) {
	sprintf(string, "  %ld %s", use_sht.mtrls[k], mtrls_info[k].name);
	msg_grouped(country, -1, -1, string);
      }
    }
    
  }

  /* finish off the messages */
  msg_gfinish();
}
