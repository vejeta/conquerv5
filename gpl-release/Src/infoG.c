// SPDX-License-Identifier: GPL-3.0-or-later
/* This file creates formatted displays of various information */
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
#include "infoG.h"
#include "itemX.h"
#include "navyX.h"
#include "desigX.h"
#include "mtrlsX.h"
#include "racesX.h"
#include "worldX.h"
#include "activeX.h"
#include "nclassX.h"
#include "statusX.h"
#include "stringX.h"
#include "caravanX.h"
#include "dstatusX.h"
#include "keyvalsX.h"

/* quick constant for the bottom limit */
#define INFO_BOTTOM	(LINES - 3)

/* column to begin information */
int info_dwidth[INFO_MAX+1] = { 15, 15, 12, 13, 0, 15, 14, 0 };

/* line to begin information */
int info_tlength[INFO_MAX+1] = { 2, 2, 2, 2, 2, 2, 2, 4 };

/* how many columns between information columns */
int info_width[INFO_MAX+1] = { 14, 14, 17, 14, 80, 20, 12, 40 };

/* how many lines between information rows */
int info_length[INFO_MAX+1] = { 11, 11, 18, 13, 16, 15, 13, 1 };

/* local variables to hold locations */
ARMY_PTR army_Pptr, army_Iptr;
NAVY_PTR navy_Pptr, navy_Iptr;
CITY_PTR city_Pptr, city_Iptr;
CVN_PTR cvn_Pptr, cvn_Iptr;
ITEM_PTR item_Pptr, item_Iptr;
NTN_PTR ntn_Pptr, ntn_Iptr;
int cntryI, cntryP;

/* what mode is the city report in */
static char *city_rep_type[] = {
  "Resource Report", "Economic Report", "Totals" };
static int city_finish, city_start;

/* column widths of top of city information */
#define CINFO_WIDTH1	28
#define CINFO_WIDTH2	60

/* DISP_CITY_INFO -- Provide information on city/region */
static void
disp_city_info PARM_2 (int, xpos, int, line)
{
  SHEET_STRUCT nation_consume, nation_produce;
  SHEET_PTR cityconsume_ptr, cityproduce_ptr;
  itemtype start_talons = 0, itemval1, itemval2;
  itemtype t_mtrls[MTRLS_NUMBER];
  long num_people;
  int i, j, k;

  /* offset the xloc appropriately */
  i = info_width[INFO_CITY];
  if (COLS < i) {
    mvprintw(line, xpos, "Need at least %d columns", i);
    return;
  }
  xpos += ((COLS % i) / (COLS / i)) / 2;

  /* show title information based on situation */
  if (city_Iptr == NULL) {
    /* total info */
    ntn_totals(country);

    /* show all the information */
    standout();
    mvprintw(line, xpos + 1, "Nation %s Summary", ntn_ptr->name);
    standend();
    mvprintw(line + 1, xpos + 1, "Capital: [%d,%d]",
	     xloc_relative(ntn_ptr->capx),
	     yloc_relative(ntn_ptr->capy));

    /* show the military information */
    mvprintw(line, xpos + CINFO_WIDTH1,     "Total Soldiers:%8ld",
	     ntn_ptr->tmil);
    mvprintw(line + 1, xpos + CINFO_WIDTH1, "Total Civilians:%7ld",
	     ntn_ptr->tciv);
    mvprintw(line + 2, xpos + CINFO_WIDTH1, "Naval Ships:%11d",
	     ntn_ptr->tships);
    mvprintw(line + 3, xpos + CINFO_WIDTH1, "Caravan Wagons:%8d",
	     ntn_ptr->twagons);

    /* next column shows stored raw materials */
    for (i = 0; i < MTRLS_NUMBER; i++) {
      mvprintw(line + i, xpos + CINFO_WIDTH2, "%s:%*.0lf",
	       mtrls_info[i].name,
	       18 - strlen(mtrls_info[i].name),
	       (double) ntn_ptr->mtrls[i]);
    }

  } else {

    /* just city info */
    sct_ptr = &(sct[city_Iptr->xloc][city_Iptr->yloc]);
    mvprintw(line, xpos + 1, "%s: ",
	     maj_dinfo[major_desg(sct_ptr->designation)].name);
    standout();
    addstr(city_Iptr->name);
    standend();
    mvprintw(line + 1, xpos + 1, "Location: [%d,%d]",
	     xloc_relative(city_Iptr->xloc),
	     yloc_relative(city_Iptr->yloc));
    mvprintw(line + 2, xpos + 1, "Weight: %d",
	     city_Iptr->weight);

    /* military information in middle column */
    if (city_Iptr->i_people > 0) {
      sprintf(string, "%d", city_Iptr->i_people);
    } else {
      strcpy(string, "[none]");
    }

    mvprintw(line + 1, xpos + CINFO_WIDTH1, "Volunteers:%12s", string);
    if (IS_HARBOR(sct_ptr->designation)) {
      mvprintw(line + 2, xpos + CINFO_WIDTH1, "Ships in Harbor:%7d",
	       ships_in_sector(city_Iptr->xloc, city_Iptr->yloc));
      move(line + 3, xpos + CINFO_WIDTH1);
    } else {
      move(line + 2, xpos + CINFO_WIDTH1);
    }
    printw( "Wagons in Sector:%6d",
	   wagons_in_sector(city_Iptr->xloc, city_Iptr->yloc));

    /* show current raw materials in third column */
    for (i = 0; i < MTRLS_NUMBER; i++) {
      mvprintw(line + i, xpos + CINFO_WIDTH2, "%s:%*.0lf",
	       mtrls_info[i].name,
	       18 - strlen(mtrls_info[i].name),
	       (double) city_Iptr->c_mtrls[i]);
    }

  }

  /* gather the information */
  if (city_Iptr == NULL) {

    /* initialize nation value */
    clr_memory( (char *) &nation_produce, sizeof(SHEET_STRUCT) );
    clr_memory( (char *) &nation_consume, sizeof(SHEET_STRUCT) );

    /* get info from all cities */
    start_talons = 0;

    /* just go though the entire nation and count sectors */
    for (i = ntn_ptr->leftedge; i <= ntn_ptr->rightedge; i++) {
      for (j = ntn_ptr->bottomedge; j <= ntn_ptr->topedge; j++) {
	/* just count sectors */
	if (Y_ONMAP(j) &&
	    ((sct_ptr = &(sct[(i + MAPX) % MAPX][j]))->owner ==
	     country)) {

	  /* count the sectors and population */
	  nation_consume.people += sct_ptr->people;
	  nation_consume.sectors++;

	  /* now get some type specifics */
	  k = major_desg(sct_ptr->designation);
	  nation_consume.type_people[k] += sct_ptr->people;
	  nation_consume.type_sectors[k]++;

	}
      }
    }

    if (city_mode != CINFO_SUMMARY) {

      /* traverse the entire national spectrum */
      if (ntn_ptr->num_city > 10) {
	bottommsg("Thinking...");
	refresh();
      }
      for (city_tptr = ntn_ptr->city_list;
	   city_tptr != NULL;
	   city_tptr = city_tptr->next) {

	/* accumulate the production information */
	if ((cityproduce_ptr = region_produce(CITYT_NAME, TRUE)) != NULL) {

	  /* add everytin up */
	  nation_produce.people += cityproduce_ptr->people;
	  for (i = 0; i < MTRLS_NUMBER; i++) {
	    nation_produce.mtrls[i] += cityproduce_ptr->mtrls[i];
	  }
	  nation_produce.sectors += cityproduce_ptr->sectors;

	  /* include all specifics */
	  for (i = 0; i < MAJ_NUMBER; i++) {
	    nation_produce.type_people[i] += cityproduce_ptr->type_people[i];
	    nation_produce.type_talons[i] += cityproduce_ptr->type_talons[i];
	    nation_produce.type_sectors[i] += cityproduce_ptr->type_sectors[i];
	  }

	  /* don't drain memory */
	  free(cityproduce_ptr);
	}

	/* accumulate the consumption information */
	if ((cityconsume_ptr = region_consume(CITYT_NAME, TRUE)) != NULL) {

	  /* add everytin up */
	  for (i = 0; i < MTRLS_NUMBER; i++) {
	    nation_consume.mtrls[i] += cityconsume_ptr->mtrls[i];
	  }
	  nation_consume.army_men += cityconsume_ptr->army_men;
	  nation_consume.ship_holds += cityconsume_ptr->ship_holds;
	  nation_consume.caravan_wagons += cityconsume_ptr->caravan_wagons;
	  nation_consume.army_cost += cityconsume_ptr->army_cost;
	  nation_consume.monst_jewels += cityconsume_ptr->monst_jewels;
	  nation_consume.navy_cost += cityconsume_ptr->navy_cost;
	  nation_consume.cvn_cost += cityconsume_ptr->cvn_cost;

	  /* include all specifics */
	  for (i = 0; i < MAJ_NUMBER; i++) {
	    nation_consume.type_talons[i] += cityconsume_ptr->type_talons[i];
	  }

	  /* don't drain memory */
	  free(cityconsume_ptr);
	}

	/* count initial talons */
	if (start_talons + start_talons > BIGITEM) {
	  start_talons = BIGITEM;
	} else {
	  start_talons += CITYT_STALONS;
	}
	nation_consume.monst_troops = ntn_ptr->tmonst;
      }

      /* done */
      if (ntn_ptr->num_city > 10) {
	bottommsg("Thinking...done");
	refresh();
	bottommsg("");
	refresh();
      }
    }

    /* now point to what we are looking at */
    cityproduce_ptr = &nation_produce;
    cityconsume_ptr = &nation_consume;

  } else {
    /* get them for just the indicated city */
    if ((cityproduce_ptr = region_produce(city_Iptr->name, TRUE)) == NULL) {
      return;
    }
    if ((cityconsume_ptr = region_consume(city_Iptr->name, TRUE)) == NULL) {
      return;
    }
  }

  /* adjust production via taxes and magic */
  adjust_production(cityproduce_ptr, TRUE);

  /* show the sector counting information */
  if (city_Iptr == NULL) {
    mvprintw(line + 2, xpos + 1, "Unsupported Sectors: %d",
	     ntn_ptr->tunsctrs);
    mvprintw(line + 3, xpos + 1, "Total Sectors: %d", ntn_ptr->tsctrs);
    for (i = 0; i < MTRLS_NUMBER; i++) {
      t_mtrls[i] = ntn_ptr->mtrls[i];
    }

    /* find any unsupported troops: UNIMPLEMENTED */

  } else {
    mvprintw(line + 3, xpos + 1, "Sectors: %d",
	     cityproduce_ptr->sectors);
    start_talons = city_Iptr->s_talons;
    for (i = 0; i < MTRLS_NUMBER; i++) {
      t_mtrls[i] = city_Iptr->c_mtrls[i];
    }
    mvprintw(line + 3, xpos + CINFO_WIDTH1, "Supported Troops:%6ld",
	     cityconsume_ptr->army_men);
  }

  /* now show the generated information */
  line += 6;
  switch (city_mode) {
  case CINFO_RESOURCE:
    /* resource production information */

    /* provide the column titles */
    if (city_Iptr != NULL) {
      sprintf(string, "|%-10s|%-10s|%-10s|%-10s|%-10s|%-10s|%-10s|",
	      "   raw", "  sector", " regional", "  region",
	      "  region", "", " estimated");
    } else {
      sprintf(string, "|%-10s|%-10s|%-10s|%-10s|%-10s|%-10s|%-10s|",
	      "   raw", "  sector", " national", "  nation",
	      "  nation", "", " estimated");
    }
    mvaddstr(line++, xpos + 1, string);
    sprintf(string, "|%-10s|%-10s|%-10s|%-10s|%-10s|%-10s|%-10s|",
	    " material", "   type", "population", "production",
	    "   usage", "    net", "  totals");
    mvaddstr(line++, xpos + 1, string);
    move(line++, xpos + 1);
    for (i = 0; i <= 77; i++)
      if ((i % 11) != 0) addch('-');
      else addch('+');

    /* first the gold talons */
    itemval1 = cityproduce_ptr->mtrls[MTRLS_TALONS];
    itemval2 = cityconsume_ptr->mtrls[MTRLS_TALONS];
    mvprintw(line++, xpos + 1,
	     "|%-10s|%-10.10s|%10ld|%10.0f|%10.0f|%10.0f|%10.0f|",
	     "Talons", "All",
	     cityconsume_ptr->people,
	     (double) itemval1, (double) itemval2,
	     (double) (itemval1 - itemval2),
	     (double) (t_mtrls[MTRLS_TALONS] + itemval1 - itemval2));

    /* then the jewels */
    itemval1 = cityproduce_ptr->mtrls[MTRLS_JEWELS];
    itemval2 = cityconsume_ptr->mtrls[MTRLS_JEWELS];
    mvprintw(line++, xpos + 1,
	     "|%-10s|%-10.10s|%10ld|%10.0f|%10.0f|%10.0f|%10.0f|",
	     "Jewels", maj_dinfo[MAJ_JEWELMINE].name,
	     cityconsume_ptr->type_people[MAJ_JEWELMINE],
	     (double) itemval1, (double) itemval2,
	     (double) (itemval1 - itemval2),
	     (double) (t_mtrls[MTRLS_JEWELS] + itemval1 - itemval2));

    /* now the metals */
    itemval1 = cityproduce_ptr->mtrls[MTRLS_METALS];
    itemval2 = cityconsume_ptr->mtrls[MTRLS_METALS];
    mvprintw(line++, xpos + 1,
	     "|%-10s|%-10.10s|%10ld|%10.0f|%10.0f|%10.0f|%10.0f|",
	     "Metals", maj_dinfo[MAJ_METALMINE].name,
	     cityconsume_ptr->type_people[MAJ_METALMINE],
	     (double) itemval1, (double) itemval2,
	     (double) (itemval1 - itemval2),
	     (double) (t_mtrls[MTRLS_METALS] + itemval1 - itemval2));

    /* handle the food information */
    mvprintw(line++, xpos + 1, "|%-10s|%-10s|", "Food", "Farmland");
    num_people = 0L;
    itemval1 = cityproduce_ptr->mtrls[MTRLS_FOOD];
    itemval2 = cityconsume_ptr->mtrls[MTRLS_FOOD];
    for (i = MAJ_FARM; i <= MAJ_FRUITFUL; i++) {
      /* count population */
      num_people += cityconsume_ptr->type_people[i];
    }
    printw("%10ld|%10.0f|%10.0f|%10.0f|%10.0f|", num_people,
	   (double) itemval1, (double) itemval2,
	   (double) (itemval1 - itemval2),
	   (double) (t_mtrls[MTRLS_FOOD] + itemval1 - itemval2));

    /* now handle the wood */
    itemval1 = cityproduce_ptr->mtrls[MTRLS_WOOD];
    itemval2 = cityconsume_ptr->mtrls[MTRLS_WOOD];
    mvprintw(line++, xpos + 1,
	     "|%-10s|%-10.10s|%10ld|%10.0f|%10.0f|%10.0f|%10.0f|",
	     "Wood", maj_dinfo[MAJ_LUMBERYARD].name,
	     cityconsume_ptr->type_people[MAJ_LUMBERYARD],
	     (double) itemval1, (double) itemval2,
	     (double) (itemval1 - itemval2),
	     (double) (t_mtrls[MTRLS_WOOD] + itemval1 - itemval2));

    /* finish off the bottom */
    mvaddch(line++, xpos + 1, '-');
    for (i = 1; i < 77; i++)
      if ((i % 11) != 0) addch('-');
      else addch('+');
    addch('-');
    break;
  case CINFO_ECONOMY:
    /* display the economic information */
    j = line - 1; /* use the extra line above */
    mvprintw(j++, xpos + 1, "|%-10s|%-10s|",
	     "  source", "  income");
    mvaddstr(j++, xpos + 1, "|----------+----------|");

    /* show the city information first */
    itemval1 = cityproduce_ptr->type_talons[MAJ_CITY] +
      cityproduce_ptr->type_talons[MAJ_CAPITAL];
    mvprintw(j++, xpos + 1, "|%-10s|%10.0f|",
	     "Cities", (double) itemval1);

    /* show town/stockade information next */
    itemval1 = cityproduce_ptr->type_talons[MAJ_TOWN] +
      cityproduce_ptr->type_talons[MAJ_STOCKADE];
    mvprintw(j++, xpos + 1, "|%-10.10s|%10.0f|",
	     "Town/Stockades", (double) itemval1);

    /* show farming information */
    itemval1 = 0;
    for (i = MAJ_FARM; i <= MAJ_FRUITFUL; i++) {
      itemval1 += cityproduce_ptr->type_talons[i];
    }
    mvprintw(j++, xpos + 1, "|%-10s|%10.0f|",
	     "Farms", (double) itemval1);

    /* handle the talons from mines */
    itemval1 = cityproduce_ptr->type_talons[MAJ_METALMINE] +
      cityproduce_ptr->type_talons[MAJ_JEWELMINE];
    mvprintw(j++, xpos + 1, "|%-10s|%10.0f|",
	     "Mines", (double) itemval1);
    
    /* now the rest */
    itemval1 = 0;
    for (i = MAJ_NONE; i < MAJ_STOCKADE; i++) {
      /* only the remaining types are needed */
      if (IS_FARM(i) || (i == MAJ_METALMINE) || (i == MAJ_JEWELMINE))
	continue;

      /* add it up */
      itemval1 += cityproduce_ptr->type_talons[i];
    }
    mvprintw(j++, xpos + 1, "|%-10.10s|%10.0f|",
	     "Others", (double) itemval1);

    /* now provide the totals */
    mvaddstr(j++, xpos + 1, "|----------+----------|");
    mvprintw(j++, xpos + 1, "|%-10s|",
	     "Income");
    standout();
    printw("%10.0f",
	     (double) cityproduce_ptr->mtrls[MTRLS_TALONS]);
    standend();
    addch('|');
    mvaddstr(j, xpos + 1, "-----------+-----------");

    /* now take care of the second column */
    i = xpos + 27;
    j = line;
    mvprintw(j++, i, "|%-10s|%-10s|",
	     "   item", " expense");
    mvaddstr(j++, i, "|----------+----------|");

    /* first the army expenditures */
    itemval1 = cityconsume_ptr->army_cost;
    mvprintw(j++, i, "|%-10s|%10.0f|",
	     "Army", (double) cityconsume_ptr->army_cost);

    /* now the navy */
    itemval1 += cityconsume_ptr->navy_cost;
    mvprintw(j++, i, "|%-10s|%10.0f|",
	     "Fleets", (double) cityconsume_ptr->navy_cost);

    /* caravans */
    itemval1 += cityconsume_ptr->cvn_cost;
    mvprintw(j++, i, "|%-10s|%10.0f|",
	     "Caravans", (double) cityconsume_ptr->cvn_cost);

    /* include sector spendings here */
    itemval2 = cityconsume_ptr->mtrls[MTRLS_TALONS] - itemval1;
    mvprintw(j++, i, "|%-10.10s|%10.0f|",
	     "Sectors", (double) itemval2);

    /* give the tally */
    mvaddstr(j++, i, "|----------+----------|");
    mvprintw(j++, i, "|%-10s|",
	     "Subtotal");
    standout();
    printw("%10.0f", (double) cityconsume_ptr->mtrls[MTRLS_TALONS]);
    standend();
    addch('|');
    mvaddstr(j++, i, "-----------+-----------");

    /* and last, but not least, the totals */
    xpos += 52;
    j = line;

    /* first the beginning information */
    strcpy(string, "Initial Treasury");
    i = strlen(string);
    mvaddstr(j++, xpos, string);
    sprintf(string, "$%.0f", (double) start_talons);
    for (i += strlen(string); i < 27; i++) addch('.');
    standout();
    addstr(string);
    standend();

    /* now show the income */
    strcpy(string, "Probable Income");
    i = strlen(string);
    mvaddstr(j++, xpos, string);
    sprintf(string, "%.0f", (double) cityproduce_ptr->mtrls[MTRLS_TALONS]);
    for (i += strlen(string); i < 27; i++) addch('.');
    addstr(string);

    /* now show the outgo */
    strcpy(string, "Probable Expenses");
    i = strlen(string);
    mvaddstr(j++, xpos, string);
    sprintf(string, "%.0f",
	    (double) (- cityconsume_ptr->mtrls[MTRLS_TALONS]));
    for (i += strlen(string); i < 27; i++) addch('.');
    addstr(string);

    /* now show the other expenses */
    strcpy(string, "Misc. Items");
    i = strlen(string);
    mvaddstr(j++, xpos, string);
    sprintf(string, "%.0f",
	    (double) (t_mtrls[MTRLS_TALONS] - start_talons));
    for (i += strlen(string); i < 27; i++) addch('.');
    addstr(string);

    /* loss from inflation */
    itemval1 = t_mtrls[MTRLS_TALONS] + cityproduce_ptr->mtrls[MTRLS_TALONS] -
      cityconsume_ptr->mtrls[MTRLS_TALONS];
    itemval2 = infl_of(itemval1);
    strcpy(string, "Inflation Loss");
    i = strlen(string);
    mvaddstr(j++, xpos, string);
    sprintf(string, "%.0f", (double) (- itemval2));
    for (i += strlen(string); i < 27; i++) addch('.');
    addstr(string);

    /* now the final totals */
    mvaddstr(j++, xpos + 17, "----------");
    strcpy(string, "Probable Treasury");
    i = strlen(string);
    mvaddstr(j++, xpos, string);
    sprintf(string, "$%.0f", (double) (itemval1 - itemval2));
    for (i += strlen(string); i < 27; i++) addch('.');
    standout();
    addstr(string);
    standend();

    /* let them know about monster */
    if (cityconsume_ptr->monst_troops > 0) {
      mvprintw(++j, xpos, "%d Monsters = %.0f Jewels",
	       cityconsume_ptr->monst_troops,
	       (double) cityconsume_ptr->monst_jewels);
    }
    break;
  case CINFO_SUMMARY:
    /* summarize number and population of the sectors */
    for (j = 0; j < MAJ_NUMBER; j++) {

      /* find the column position */
      if (j < 6) i = xpos + 1;
      else i = xpos + j / 6 * 27;

      /* check if the title should be shown */
      if ((j % 6) == 0) {
	mvprintw(line, i, "|%-3s|%-10s|%-8s|", "num",
		 "   type", " people");
	mvaddstr(line + 1, i, "|===+==========+========|");
	mvaddstr(line + 8, i, "----+----------+---------");
      }

      /* show the information */
      mvprintw(line + (j % 6) + 2, i, "|%3d|%-10s|%8ld|",
	       cityconsume_ptr->type_sectors[j],
	       maj_dinfo[j].name, cityconsume_ptr->type_people[j]);
    }

    /* add in the final totals */
    mvaddstr(line + 6, i, "|---+----------+--------|");
    mvprintw(line + 7, i, "|%3d|%-10s|%8ld|",
	     cityconsume_ptr->sectors, "Totals",
	     cityconsume_ptr->people);
    break;
  }

  /* get rid of any malloced memory */
  if (city_Iptr != NULL) {
    if (cityproduce_ptr != NULL) free(cityproduce_ptr);
    if (cityconsume_ptr != NULL) free(cityconsume_ptr);
  }
}

/* DISP_INFO -- Show information about the selected item */
static void
disp_info PARM_3 (int, x, int, y, Infotype, choice)
{
  int i, j, k;

  switch (choice) {
  case INFO_ARMY:
  case INFO_GRPARM:
    standout();
    mvprintw( y++, x, "%d", army_Iptr->armyid );
    standend();
    if (a_isleader(army_Iptr->unittype)) {
      mvprintw( y++, x, "%ld", army_Iptr->strength );
    } else if (a_ismonster(army_Iptr->unittype)) {
      mvprintw( y++, x, "%ld x%d", army_Iptr->strength,
	       ainfo_list[army_Iptr->unittype].minsth );
    } else {
      mvprintw( y++, x, "%ld men", army_Iptr->strength );
    }
    if (is_god == TRUE) {
      if (army_Iptr->efficiency < 100) standout();
      if (a_isleader(army_Iptr->unittype)) {
	mvprintw( y++, x, "%d%%",
		 army_Iptr->efficiency);
      } else {
	mvprintw( y++, x, "%d%% (%d%%)",
		 army_Iptr->efficiency,
		 army_Iptr->max_eff );
      }
      if (army_Iptr->efficiency < 100) standend();
      mvprintw( y++, x, "%s",
	       stat_info[unit_status(army_Iptr->status)].name );
      if (unit_spelled(army_Iptr->status)) {
	addch('!');
      }
    } else {
      if (army_Iptr->efficiency < 100) standout();
      mvprintw( y++, x, "%d%% %s", army_Iptr->efficiency,
	       stat_info[unit_status(army_Iptr->status)].name );
      if (unit_spelled(army_Iptr->status)) {
	addch('!');
      }
      if (army_Iptr->efficiency < 100) standend();
    }
    mvprintw( y++, x, "[%d,%d]",
	     xloc_relative(army_Iptr->xloc), yloc_relative(army_Iptr->yloc) );
    if (is_god == TRUE) {
      mvprintw( y++, x, "[%d,%d]", army_Iptr->lastx, army_Iptr->lasty );
    }
    if (nomove_stat(army_Iptr->status)) {
      if (wall_patrol(country, army_Iptr)) {
	mvaddstr( y++, x, "1 sector");
      } else {
	mvaddstr( y++, x, "unmovable");
      }
    } else if (unit_flight(army_Iptr->status)) {
      mvprintw( y++, x, "Fly%s: %d%%",
	       speedname[unit_speed(army_Iptr->status)],
	       (int) army_Iptr->umove );
    } else {
      mvprintw( y++, x, "%s: %d%%",
	       speedname[unit_speed(army_Iptr->status)],
	       (int) army_Iptr->umove );
    }
    mvprintw( y++, x, "%s", ainfo_list[army_Iptr->unittype].name );
    if ((army_tptr = armybynum(army_Iptr->leader)) != NULL) {
      mvprintw( y++, x, "%s %d",
	       ainfo_list[ARMYT_TYPE].name,
	       (int) army_Iptr->leader );
    } else {
      mvaddstr( y++, x, "None");
    }
    if (a_isleader(army_Iptr->unittype)) {
      mvprintw( y++, x, "%d pts", (int) army_Iptr->spellpts );
    } else if ((MAXSUPPLIES > 0) &&
	       !a_freesupply(army_Iptr->unittype)) {
      if (army_Iptr->supply < 2) standout();
      if (a_castspells(army_Iptr->unittype)) {
	mvprintw( y++, x, "%d m/%d p", (int) army_Iptr->supply,
		 (int) army_Iptr->spellpts );
      } else {
	mvprintw( y++, x, "%d mths", (int) army_Iptr->supply );
      }
      if (army_Iptr->supply < 2) standend();
    } else if (a_castspells(army_Iptr->unittype)) {
      mvprintw( y++, x, "%d pts", (int) army_Iptr->spellpts );
    } else {
      mvaddstr( y++, x, "supplied");
    }
    if (a_isleader(army_Iptr->unittype)) {
      mvaddstr( y++, x, "None" );
      mvaddstr( y++, x, "None" );
    } else {
      itemtype supt_cost[MTRLS_NUMBER];
      army_support(army_Iptr, &(supt_cost[0]), 1);
      if (a_ismonster(army_Iptr->unittype)) {
	mvprintw( y++, x, "%.0f",
		 (double) supt_cost[MTRLS_JEWELS] );
	mvprintw( y++, x, "%ld pts",
		 ainfo_list[army_Iptr->unittype].tal_enlist *
		 army_Iptr->strength );
      } else {
	if (a_freesupply(army_Iptr->unittype)) {
	  mvaddstr( y++, x, "None" );
	} else {
	  mvprintw( y++, x, "%.0f",
		   (double) supt_cost[MTRLS_TALONS] );
	}
	set_armycosts(ntn_ptr, &(supt_cost[0]),
		      army_Iptr->unittype,
		      army_Iptr->strength);
	mvprintw( y++, x, "%ld",
		 supt_cost[MTRLS_TALONS]);
      }
    }
    break;
  case INFO_NAVY:
    standout();
    mvprintw( y++, x, "%d", navy_Iptr->navyid );
    standend();
    for (j = 0; j <= N_HEAVY; j++) {
      int fnd_one = FALSE;

      move(y++, x);
      for (i = 0; i < NSHP_NUMBER; i++) {
	k = (int) N_CNTSHIPS(navy_Iptr->ships[i], j);
	if (k > 0) {
	  if (fnd_one == TRUE) addch(',');
	  printw("%d%s", k, ninfo_list[i].sname);
	  fnd_one = TRUE;
	}
      }
    }
    for (i = 0; i < NSHP_NUMBER; i++) {
      if ((navy_Iptr->ships[i] != 0) &&
	  (navy_Iptr->efficiency[i] < 100)) {
	standout();
	break;
      }
    }
    move(y++, x);
    j = FALSE;
    for (i = 0; i < NSHP_NUMBER; i++) {
      if (navy_Iptr->ships[i] != 0) {
	if (j == TRUE) {
	  addch(',');
	}
	j = TRUE;
	printw("%d%s", (int) navy_Iptr->efficiency[i],
	       ninfo_list[i].sname);
      }
    }
    for (i = 0; i < NSHP_NUMBER; i++) {
      if ((navy_Iptr->ships[i] != 0) &&
	  (navy_Iptr->efficiency[i] < 100)) {
	standend();
	break;
      }
    }
    mvprintw( y++, x, "%s",
	     stat_info[unit_status(navy_Iptr->status)].name );
    if (is_god == TRUE) {
      mvprintw( y++, x, "[%d,%d:%d,%d]",
	       xloc_relative(navy_Iptr->xloc),
	       yloc_relative(navy_Iptr->yloc),
	       navy_Iptr->lastx, navy_Iptr->lasty );
    } else {
      mvprintw( y++, x, "[%d,%d]",
	       xloc_relative(navy_Iptr->xloc),
	       yloc_relative(navy_Iptr->yloc) );
    }
    mvprintw( y++, x, "%s: %d%%",
	     speedname[unit_speed(navy_Iptr->status)],
	     navy_Iptr->umove );
    if (MAXSUPPLIES > 0) {
      if ((navy_Iptr->supply < 2) ||
	  (navy_Iptr->crew * 2 < MAXNAVYCREW)) {
	standout();
      }
      mvprintw( y++, x, "%d%%: %d mths",
	       ((int) navy_Iptr->crew * 100) / MAXNAVYCREW,
	       navy_Iptr->supply );
      if ((navy_Iptr->supply < 2) ||
	  (navy_Iptr->crew * 2 < MAXNAVYCREW)) {
	standend();
      }
    } else {
      if (navy_Iptr->crew * 2 < MAXNAVYCREW) {
	standout();
      }
      mvprintw( y++, x, "%d%%",
	       ((int) navy_Iptr->crew * 100) / MAXNAVYCREW);
      if (navy_Iptr->crew * 2 < MAXNAVYCREW) {
	standend();
      }
    }
    if ((cvn_tptr = cvnbynum(navy_Iptr->cvnnum)) != NULL) {
      mvprintw( y++, x, "Caravan %d", CVNT_ID);
    } else {
      mvaddstr( y++, x, "None" );
    }
    if ((army_tptr = armybynum(navy_Iptr->armynum)) != NULL) {
      mvprintw( y++, x, "%s %d", ainfo_list[ARMYT_TYPE].name, ARMYT_ID );
    } else {
      mvaddstr( y++, x, "None" );
    }
    mvprintw( y++, x, "%ld", (long) navy_Iptr->people *
	     navy_holds(navy_Iptr, NSHP_GALLEYS) );
    for (j = 0; j < MTRLS_NUMBER; j++) {
      mvprintw( y++, x, "%.0f", (double) navy_Iptr->mtrls[j] );
    }
    break;
  case INFO_CVN:
    standout();
    mvprintw( y++, x, "%d", cvn_Iptr->cvnid );
    standend();
    mvprintw( y++, x, "%d", cvn_Iptr->size * WAGONS_IN_CVN );
    if (cvn_Iptr->efficiency < 100) standout();
    mvprintw( y++, x, "%d%% %s",
	     cvn_Iptr->efficiency,
	     stat_info[unit_status(cvn_Iptr->status)].name );
    if (cvn_Iptr->efficiency < 100) standend();
    mvprintw( y++, x, "[%d,%d]",
	     xloc_relative(cvn_Iptr->xloc), yloc_relative(cvn_Iptr->yloc) );
    if (is_god == TRUE)
      mvprintw( y++, x, "[%d,%d]", cvn_Iptr->lastx, cvn_Iptr->lasty );
    mvprintw( y++, x, "%s: %d%%",
	     speedname[unit_speed(cvn_Iptr->status)],
	     (int) cvn_Iptr->umove);
    if (MAXSUPPLIES > 0) {
      if ((cvn_Iptr->supply < 2) ||
	  (cvn_Iptr->crew * 2 < MAXCVNCREW)) {
	standout();
      }
      mvprintw( y++, x, "%d%%: %d mths",
	       ((int) cvn_Iptr->crew * 100) / MAXCVNCREW,
	       cvn_Iptr->supply );
      if ((cvn_Iptr->supply < 2) ||
	  (cvn_Iptr->crew * 2 < MAXCVNCREW)) {
	standend();
      }
    } else {
      if (cvn_Iptr->crew * 2 < MAXCVNCREW) {
	standout();
      }
      mvprintw( y++, x, "%d%%",
	       ((int) cvn_Iptr->crew * 100) / MAXCVNCREW);
      if (cvn_Iptr->crew * 2 < MAXCVNCREW) {
	standend();
      }
    }
    mvprintw( y++, x, "%ld", (long)cvn_Iptr->people * cvn_Iptr->size );
    for (j = 0; j < MTRLS_NUMBER; j++) {
      mvprintw( y++, x, "%.0f", (double) cvn_Iptr->mtrls[j] );
    }
    break;
  case INFO_CITY:
    disp_city_info(x, y);
    break;
  case INFO_ITEM:
    standout();
    mvprintw( y++, x, "Item %d", item_Iptr->itemid );
    standend();
    mvprintw( y++, x, "[%d,%d]",
	     xloc_relative(item_Iptr->xloc),
	     yloc_relative(item_Iptr->yloc) );
    y++; /* find out what it is in */
    for (j = 0; j < MTRLS_NUMBER; j++) {
      mvprintw( y++, x, "%.0f", (double) item_Iptr->mtrls[j] );
    }
    break;
  case INFO_NTN:
    standout();
    mvprintw( y++, x, "%s", ntn_Iptr->name );
    standend();
    mvprintw( y++, x, "%s", ntn_Iptr->leader);
    mvprintw( y++, x, "%s", race_info[ntn_Iptr->race].name );
    mvprintw( y++, x, "%s", nclass_list[ntn_Iptr->class].name );
    mvprintw( y++, x, "%s", alignment[n_alignment(ntn_Iptr->active)] );
    if (((world.hide_scores) &&
	 (country != UNOWNED)) ||
	n_ismonster(ntn_Iptr->active)) {
      mvaddstr( y++, x, ". . .");
      mvaddstr( y++, x, ". . .");
      mvaddstr( y++, x, ". . .");
      mvaddstr( y++, x, ". . .");
      mvaddstr( y++, x, ". . .");
    } else {
      mvprintw( y++, x, "%d", ntn_Iptr->score );
      mvprintw( y++, x, "%.0f", (double) ntn_Iptr->mtrls[MTRLS_TALONS] );
      mvprintw( y++, x, "%.0f", (double) ntn_Iptr->tmil );
      mvprintw( y++, x, "%.0f", (double) ntn_Iptr->tciv );
      mvprintw( y++, x, "%d", ntn_Iptr->tsctrs );
    }
    mvprintw( y++, x, "%s", aggressname[n_aggression(ntn_Iptr->active)]);
    mvprintw( y++, x, "%s", (n_isnpc(ntn_Iptr->active)) ? "Yes" : "No");
    break;
  case INFO_DIP:
    /* adjust offset properly */
    k = info_width[INFO_DIP];
    i = ntn_ptr->dstatus[cntryI];
    j = ntn_Iptr->dstatus[country];
    if (COLS < k) {
      mvprintw(y, x, "Need at least %d columns", k);
      return;
    }
    x += (COLS % k) / ((COLS * 2) / k);

    /* show diplomacy statuses */
    sprintf(string, "%-12.12s(%c)", ntn_Iptr->name,
	    race_info[ntn_Iptr->race].name[0]);
    if (i >= dstatus_number) {
      mvprintw(y, x, "*%s", string);
    } else {
      mvprintw(y, x, " %s", string);
    }
    if (i % dstatus_number > DIP_HOSTILE) standout();
    strcpy(string, dipname[ i % dstatus_number ]);
    mvaddstr(y, x + 21 - (strlen(string) / 2), string);
    if (i % dstatus_number > DIP_HOSTILE) standend();
    mvaddstr(y, x + 26, " vs ");
    if (j % dstatus_number > DIP_HOSTILE) standout();
    strcpy(string, dipname[ j % dstatus_number ]);
    mvaddstr(y, x + 35 - ((strlen(string) + 1) / 2), string);
    if (j % dstatus_number > DIP_HOSTILE) standend();
    break;
  default:
    mvprintw(y, x, "=<ERROR>=");
    break;
  }
}

/* TITLE_INFO -- Display the desired titles for the information */
static void
title_info PARM_1 (Infotype, choice)
{
  char str[BIGLTH];
  int i, j, k;

  switch (choice) {
  case INFO_ARMY:
    sprintf(str, "Army Report for Nation %s", ntn_ptr->name);
    break;
  case INFO_GRPARM:
    sprintf(str, "Sector [%d,%d] Army Summary for Nation %s",
	    xloc_relative(XREAL), yloc_relative(YREAL), ntn_ptr->name);
    break;
  case INFO_NAVY:
    sprintf(str, "Naval Report for Nation %s", ntn_ptr->name);
    break;
  case INFO_CVN:
    sprintf(str, "Caravan Report for Nation %s", ntn_ptr->name);
    break;
  case INFO_CITY:
    sprintf(str, "Region %s for Nation %s", city_rep_type[city_mode],
	    ntn_ptr->name);
    break;
  case INFO_ITEM:
    sprintf(str, "Commodity Report for Nation %s", ntn_ptr->name);
    break;
  case INFO_NTN:
    sprintf(str, "World Status Report");
    break;
  case INFO_DIP:
    sprintf(str, "Diplomatic Status Report for Nation %s", ntn_ptr->name);
    standout();
    k = info_width[INFO_DIP];
    if (COLS < k) {
      j = k;
    } else {
      j = COLS / k;
      j = k + (COLS - k * j) / j;
    }

    /* adjust offset properly */
    if (COLS < k) {
      return;
    }
    k = (COLS % k) / ((COLS * 2) / k) + 2;
    for (i = 0; i <= COLS - j; i += j) {
      mvprintw(2, i + k, "%-16s %s", "Nation (Race)", "By You  --  To You" );
    }
    standend();
    break;
  default:
    break;
  }

  /* show it */
  standout();
  mvaddstr( 0, (COLS - strlen(str)) / 2, str);
  standend();
}

/* DESC_INFO -- Display decription of each row */
static void
desc_info PARM_2 (int, line, Infotype, choice)
{
  int j;

  switch (choice) {
  case INFO_ARMY:
  case INFO_GRPARM:
    line++;
    mvaddstr( line++, 0, "strength    :" );
    if (is_god == TRUE) {
      mvaddstr( line++, 0, "effncy (max):" );
    }
    mvaddstr( line++, 0, "status      :" );
    mvaddstr( line++, 0, "location    :" );
    if (is_god == TRUE) {
      mvaddstr( line++, 0, "last locale :" );
    }
    mvaddstr( line++, 0, "movement    :" );
    mvaddstr( line++, 0, "unit type   :" );
    mvaddstr( line++, 0, "commanded by:" );
    if (MAXSUPPLIES > 0) {
      mvaddstr( line++, 0, "sply-spellpt:" );
    } else {
      mvaddstr( line++, 0, "spell points:" );
    }
    mvaddstr( line++, 0, "cost/turn   :" );
    mvaddstr( line++, 0, "enlist cost :" );
    break;
  case INFO_NAVY:
    line++;
    mvaddstr( line++, 0, "-light-   :" );
    mvaddstr( line++, 0, "-medium-  :" );
    mvaddstr( line++, 0, "-heavy-   :" );
    mvaddstr( line++, 0, "efficiency:" );
    mvaddstr( line++, 0, "status    :" );
    if (is_god == TRUE) {
      mvaddstr( line++, 0, "locale,old:");
    } else {
      mvaddstr( line++, 0, "location  :" );
    }
    mvaddstr( line++, 0, "movement  :" );
    if (MAXSUPPLIES > 0) {
      mvaddstr( line++, 0, "crw & sply:" );
    } else {
      mvaddstr( line++, 0, "crew level:" );
    }
    mvaddstr( line++, 0, "wagons    :" );
    mvaddstr( line++, 0, "troops    :" );
    mvaddstr( line++, 0, "civilians :" );
    for (j = 0; j < MTRLS_NUMBER; j++) {
      mvprintw( line++, 0, "%-10s:", mtrls_info[j].lname );
    }
    break;
  case INFO_CVN:
    line++;
    mvaddstr( line++, 0, "wagons    :" );
    mvaddstr( line++, 0, "status    :" );
    mvaddstr( line++, 0, "location  :" );
    if (is_god == TRUE) {
      mvaddstr( line++, 0, "last local:" );
    }
    mvaddstr( line++, 0, "movement  :" );
    if (MAXSUPPLIES > 0) {
      mvaddstr( line++, 0, "crw & sply:" );
    } else {
      mvaddstr( line++, 0, "crew level:" );
    }
    mvaddstr( line++, 0, "civilians :" );
    for (j = 0; j < MTRLS_NUMBER; j++) {
      mvprintw( line++, 0, "%-10s:", mtrls_info[j].lname );
    }
    break;
  case INFO_CITY:
    break;
  case INFO_ITEM:
    line++;
    mvaddstr( line++, 0, "location  :" );
    mvaddstr( line++, 0, "stored in :" );
    for (j = 0; j < MTRLS_NUMBER; j++) {
      mvprintw( line++, 0, "%-10s:", mtrls_info[j].lname );
    }
    break;
  case INFO_NTN:
    line++;
    mvaddstr( line++, 0, "leader     :" );
    mvaddstr( line++, 0, "race       :" );
    mvaddstr( line++, 0, "class      :" );
    mvaddstr( line++, 0, "alignment  :" );
    mvaddstr( line++, 0, "score      :" );
    mvaddstr( line++, 0, "treasury   :" );
    mvaddstr( line++, 0, "military   :" );
    mvaddstr( line++, 0, "civilians  :" );
    mvaddstr( line++, 0, "sectors    :" );
    mvaddstr( line++, 0, "status     :" );
    mvaddstr( line++, 0, "npc nation :" );
    break;
  case INFO_DIP:
    /* no decription is made in the diplomacy report */
    break;
  default:
    break;
  }
}

/* INC_INFO -- Select the next item for display */
static void
inc_info PARM_1 (Infotype, choice)
{
  switch (choice) {
  case INFO_ARMY:
    if (army_Iptr != NULL)
      army_Iptr = army_Iptr->next;
    break;
  case INFO_GRPARM:
    if (army_Iptr != NULL)
      army_Iptr = army_Iptr->nrby;
    break;
  case INFO_NAVY:
    if (navy_Iptr != NULL)
      navy_Iptr = navy_Iptr->next;
    break;
  case INFO_CVN:
    if (cvn_Iptr != NULL)
      cvn_Iptr = cvn_Iptr->next;
    break;
  case INFO_CITY:
    if (city_Iptr == NULL) {
      city_finish = TRUE;
    } else {
      city_Iptr = city_Iptr->next;
    }
    break;
  case INFO_ITEM:
    if (item_Iptr != NULL)
      item_Iptr = item_Iptr->next;
    break;
  case INFO_NTN:
    /* next active nation */
    do {
      cntryI++;
    } while ((cntryI < MAXNTN) && ((ntn_Iptr = world.np[cntryI]) == NULL));
    break;
  case INFO_DIP:
    /* next active nation; excepting current nation */
    do {
      cntryI++;
    } while ((cntryI < MAXNTN) &&
	     ((cntryI == country) || (ntn_Iptr = world.np[cntryI]) == NULL)) ;
    break;
  default:
    /* caught elsewhere */
    break;
  }
}

/* DONE_INFO -- Return test result if search is done */
static int
done_info PARM_1 (Infotype, choice)
{
  int hold;

  switch (choice) {
  case INFO_ARMY:
  case INFO_GRPARM:
    hold = (army_Iptr == NULL);
    break;
  case INFO_NAVY:
    hold = (navy_Iptr == NULL);
    break;
  case INFO_CVN:
    hold = (cvn_Iptr == NULL);
    break;
  case INFO_CITY:
    hold = ((city_Iptr == NULL) && (city_finish == TRUE));
    break;
  case INFO_ITEM:
    hold = (item_Iptr == NULL);
    break;
  case INFO_NTN:
  case INFO_DIP:
    hold = (cntryI >= MAXNTN);
    break;
  default:
    /* caught elsewhere */
    hold = TRUE;
    break;
  }
  return (hold);
}

/* INIT_INFO -- Begin from the beginning of the list of items */
static void
init_info PARM_1 (Infotype, choice)
{
  switch (choice) {
  case INFO_ARMY:
    army_Iptr = ntn_ptr->army_list;
    break;
  case INFO_GRPARM:
    for (army_Iptr = ntn_ptr->army_list;
	 army_Iptr != NULL;
	 army_Iptr = army_Iptr->next) {
      if ((army_Iptr->xloc == XREAL) &&
	  (army_Iptr->yloc == YREAL)) break;
    }
    break;
  case INFO_NAVY:
    navy_Iptr = ntn_ptr->navy_list;
    break;
  case INFO_CVN:
    cvn_Iptr = ntn_ptr->cvn_list;
    break;
  case INFO_CITY:
    if ((city_start == TRUE) &&
	XY_ONMAP(XREAL, YREAL) &&
	(sct[XREAL][YREAL].owner == country) &&
	IS_CITY(sct[XREAL][YREAL].designation)) {
      /* try to get the current city */
      city_Iptr = citybyloc(ntn_ptr, XREAL, YREAL);
      if (city_Iptr == NULL) {
	city_Iptr = ntn_ptr->city_list;
      }
    } else {
      city_Iptr = ntn_ptr->city_list;
    }
    city_start = FALSE;
    city_finish = FALSE;
    break;
  case INFO_ITEM :
    item_Iptr = ntn_ptr->item_list;
    break;
  case INFO_NTN:
  case INFO_DIP:
    if (country == 1) {
      cntryI = 2;
    } else {
      cntryI = 1;
    }
    ntn_Iptr = (world.np[cntryI]);
    break;
  default:
    /* caught elsewhere */
    break;
  }
}

/* SAVEPAGE_INFO -- Note which item is at the beginning of the page */
static void
savepage_info PARM_1 (Infotype, choice)
{
  switch (choice) {
  case INFO_ARMY:
  case INFO_GRPARM:
    army_Pptr = army_Iptr;
    break;
  case INFO_NAVY:
    navy_Pptr = navy_Iptr;
    break;
  case INFO_CVN:
    cvn_Pptr = cvn_Iptr;
    break;
  case INFO_CITY:
    city_Pptr = city_Iptr;
    break;
  case INFO_ITEM:
    item_Pptr = item_Iptr;
    break;
  case INFO_NTN:
  case INFO_DIP:
    ntn_Pptr = ntn_Iptr;
    cntryP = cntryI;
    break;
  }
}

/* SETPAGE_INFO -- Reset to the beginning of the page */
static void
setpage_info PARM_1 (Infotype, choice)
{
  switch (choice) {
  case INFO_ARMY:
  case INFO_GRPARM:
    army_Iptr = army_Pptr;
    break;
  case INFO_NAVY:
    navy_Iptr = navy_Pptr;
    break;
  case INFO_CVN:
    cvn_Iptr = cvn_Pptr;
    break;
  case INFO_CITY:
    city_Iptr = city_Pptr;
    break;
  case INFO_ITEM:
    item_Iptr = item_Pptr;
    break;
  case INFO_NTN:
  case INFO_DIP:
    ntn_Iptr = ntn_Pptr;
    cntryI = cntryP;
    break;
  }
}

/* SEARCH_INFO -- Go to a selected item */
static void
search_info PARM_1 (Infotype, choice)
{
  int num;

  /* clean up */
  clear_bottom(4);
  switch (choice) {
  case INFO_ARMY:
  case INFO_GRPARM:
    mvaddstr(LINES - 2, 1, "Go to which army unit? ");
    num = get_number(FALSE);
    if (no_input == TRUE) break;
    if (((army_Iptr = armybynum(num)) == NULL) ||
	((choice == INFO_GRPARM) &&
	 ((army_Iptr->xloc != XREAL) ||
	  (army_Iptr->yloc != YREAL)))) {
      if (choice == INFO_GRPARM) {
	errormsg("I can't find that army unit among those listed");
      } else {
	errormsg("There is no such army unit");
      }
    } else {
      savepage_info(choice);
    }
    break;
  case INFO_NAVY:
    mvaddstr(LINES - 2, 1, "Go to which fleet? ");
    num = get_number(FALSE);
    if (no_input == TRUE) break;
    if ((navy_Iptr = navybynum(num)) == NULL) {
      errormsg("There is no such fleet");
    } else {
      savepage_info(choice);
    }
    break;
  case INFO_CVN:
    mvaddstr(LINES - 2, 1, "Go to which caravan? ");
    num = get_number(FALSE);
    if (no_input == TRUE) break;
    if ((cvn_Iptr = cvnbynum(num)) == NULL) {
      errormsg("There is no such caravan");
    } else {
      savepage_info(choice);
    }
    break;
  case INFO_CITY:
    mvaddstr(LINES - 2, 1,
	     "Examine which city region? [\"*\" for Nation Summary] ");
    get_string(string, STR_CITY, NAMELTH);
    if (no_input == TRUE) break;
    if (strcmp(string, "*") == 0) {
      city_Iptr = NULL;
      savepage_info(choice);
    } else if ((city_Iptr = citybyname(string)) == NULL) {
      errormsg("There is no such city");
    } else {
      savepage_info(choice);
    }
    break;
  case INFO_ITEM:
    mvaddstr(LINES - 2, 1, "Go to which item? ");
    num = get_number(FALSE);
    if (no_input == TRUE) break;
    if ((item_Iptr = itembynum(num)) == NULL) {
      errormsg("There is no such item");
    } else {
      savepage_info(choice);
    }
    break;
  case INFO_NTN:
  case INFO_DIP:
    mvaddstr(LINES - 2, 1, "Go to which nation? ");
    cntryI = get_country(FALSE);
    if (no_input == TRUE) break;
    if (ntn_tptr == NULL) {
      errormsg("There is no such nation");
    } else {
      ntn_Iptr = ntn_tptr;
      savepage_info(choice);
    }
    break;
  }
}

/* SHOW_INFO -- Page through information on a selected item */
int
show_info PARM_1 (int, inchoice)
{
  Infotype choice = inchoice;
  static int god_inits = FALSE;
  int xpos = 0, ypos, deltax, deltay;
  int done = FALSE, newrow;

  /* perform error checking and perform query of deity */
  if (choice > INFO_MAX) return(FALSE);
  if ((choice != INFO_NTN) &&
      (country == UNOWNED)) {
    if (get_god(-1, FALSE))
      return(FALSE);
    if (god_inits == FALSE) {
      god_inits = TRUE;
      info_length[INFO_ARMY] += 2;
      info_length[INFO_GRPARM] += 2;
      info_length[INFO_CVN]++;
    }
  }

  /* initialize things */
  init_info(choice);
  if ((choice == INFO_CITY) ||
      (choice == INFO_DIP)) {
    /* use deltay temporarily to store minimum width */
    deltay = info_width[choice];
    if (COLS < deltay) {
      deltax = deltay;
    } else {
      deltax = COLS / deltay;
      deltax = deltay + (COLS - deltay * deltax) / deltax;
    }
  } else {
    deltax = info_width[choice];
  }
  deltay = info_length[choice];

  /* check for empty display */
  if (is_god == FALSE && done_info( choice )) {
    return(TRUE);
  }

  /* clear the whole screen; it will need to be redrawn */
  clear();
  redraw = DRAW_FULL;

  /* display until user is done */
  while (done == FALSE) {

    /* clean the screen */
    move(0, 0);
    clrtobot();

    /* start the page */
    newrow = TRUE;
    title_info(choice);
    savepage_info(choice);
    ypos = info_tlength[choice];

    /* display a page worth of info */
    while (!done_info(choice) && (ypos + deltay < INFO_BOTTOM)) {

      /* add a header if neccesary */
      if (newrow == TRUE) {
	desc_info(ypos, choice);
	xpos = info_dwidth[choice];
	newrow = FALSE;
      }

      /* check positioning */
      if (((choice != INFO_DIP) &&
	   (choice != INFO_CITY) &&
	   (xpos + deltax < COLS))
	  || (xpos + deltax < COLS + 1)) {

	/* show the current item */
	disp_info(xpos, ypos, choice);
	xpos += deltax;

	/* go on to the next element */
	inc_info(choice);

      } else {

	/* see if the next row is usable */
	ypos += deltay;
	newrow = TRUE;

      }
    }

    /* position the bottom prompt */
    if (deltay == 1) {
      ypos = INFO_BOTTOM;
    } else {
      while (ypos + deltay < INFO_BOTTOM) {
	ypos += deltay;
      }
    }

    /* display the bottom prompt */
    standout();
    if (choice == INFO_NTN) {
      mvprintw(ypos++, COLS / 2 - 18,
	       "HIT 'Q' TO EXIT; HIT '/' TO SEARCH");
      mvprintw(ypos, COLS / 2 - 13,
	       "ANY OTHER KEY TO SEE MORE");
    } else if (choice == INFO_CITY) {
      if (city_mode == CINFO_RESOURCE) {
	mvprintw(ypos, COLS / 2 - 26,
		 "HIT 'E' FOR REGION ECONOMY OR 'T' FOR REGION TOTALS");
      } else if (city_mode == CINFO_ECONOMY) {
	mvprintw(ypos, COLS / 2 - 27,
		 "HIT 'R' FOR REGION RESOURCE OR 'T' FOR REGION TOTALS");
      } else {
	mvprintw(ypos, COLS / 2 - 27,
		 "HIT 'E' FOR REGION ECONOMY OR 'R' FOR REGION RESOURCE");
      }
      if (god_browsing == TRUE) {
	mvprintw(ypos + 2, COLS / 2 - 17,
		 "HIT '/' TO JUMP TO A SUPPLY CENTER");
      } else {
	mvprintw(ypos + 2, COLS / 2 - 19,
		 "HIT RETURN TO ADJUST OR '/' TO SEARCH");
      }
      mvprintw(ypos + 1, COLS / 2 - 24,
	       "HIT 'Q' TO EXIT; ANY OTHER KEY FOR NEXT REGION");
    } else {
      if (god_browsing == TRUE) {
	mvprintw(ypos, COLS / 2 - 18,
		 "HIT '/' TO SEARCH FOR A SPECIFIC ITEM");
      } else {
	mvprintw(ypos, COLS / 2 - 18,
		 "HIT RETURN TO ADJUST OR '/' TO SEARCH");
      }
      mvprintw(ypos + 1, COLS / 2 - 21,
	       "HIT 'Q' TO EXIT; ANY OTHER KEY TO SEE MORE");
    }
    standend();
    refresh();

    /* reallign if at end of data */
    if (done_info(choice)) {
      init_info(choice);
    }

    switch (next_char()) {
    case 'q':
    case 'Q':
      /* done with the routine */
      done = TRUE;
      break;
    case CNTRL_R:
    case CNTRL_L:
      /* fix the screen */
      wrefresh(curscr);
      setpage_info(choice);
      break;
    case '/':
      /* find a target quickly */
      search_info(choice);
      setpage_info(choice);
      break;
    case EXT_ESC:
    case '\n':
    case '\r':
      /* adjust the information */
      if (god_browsing == FALSE) {
	edit_info(ypos, choice);
	setpage_info(choice);
      }
      break;
    case 'e':
    case 'E':
      /* use the city economic report */
      if ((choice == INFO_CITY) && (city_mode != CINFO_ECONOMY)) {
	city_mode = CINFO_ECONOMY;
	setpage_info(choice);
      }
      break;
    case 'r':
    case 'R':
      /* use the city resource report */
      if ((choice == INFO_CITY) && (city_mode != CINFO_RESOURCE)) {
	city_mode = CINFO_RESOURCE;
	setpage_info(choice);
      }
      break;
    case 't':
    case 'T':
      /* use the city summary report */
      if ((choice == INFO_CITY) && (city_mode != CINFO_SUMMARY)) {
	city_mode = CINFO_SUMMARY;
	setpage_info(choice);
      }
      break;
    default:
      break;
    }
  }

  /* close up shop */
  if (is_god == TRUE) {
    reset_god();
  }
  return(FALSE);
}

/* SHOW_CITY_INFO -- Filter to call the show_info routine */
int
show_city_info PARM_0(void)
{
  /* check the input */
  if ((city_mode < CINFO_RESOURCE) || (city_mode > CINFO_SUMMARY)) {
    city_mode = CINFO_ECONOMY;
  }

  /* call the routine */
  city_start = TRUE;
  if (show_info(INFO_CITY) == TRUE) {
    errormsg("Your nation has no cities to display");
  }
  return(MOVECOST);
}
