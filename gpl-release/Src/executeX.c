// SPDX-License-Identifier: GPL-3.0-or-later
/* This file reads in player commands from the execute file */
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
#define USE_CODES
#include "dataX.h"
#include "armyX.h"
#include "cityX.h"
#include "itemX.h"
#include "navyX.h"
#include "butesX.h"
#include "magicX.h"
#include "mtrlsX.h"
#include "worldX.h"
#include "activeX.h"
#include "elevegX.h"
#include "nclassX.h"
#include "statusX.h"
#include "caravanX.h"
#include "dstatusX.h"
#include "executeX.h"

/* local file parameter list */
char str1[LINELTH], str2[LINELTH], exe_ntnname[NAMELTH + 1];
double dval1, dval2;
int idnum, ival1, ival2;
Exectype cmdnum;
long lval1, lval2;

/* EXEC_ARMY -- Process individual army commands */
static void
exec_army PARM_0(void)
{
  /* set the army pointer properly */
  if ((cmdnum != EX_ARMYCREATE) &&
      (cmdnum != EX_ARMYMERCS)) {
    if (army_ptr == NULL || ARMY_ID != idnum) {
      army_ptr = armybynum(idnum);
    }

    /* verify army */
    if (army_ptr == NULL) {
      sprintf(string, "Error(%s): Cmd #%d, Army #%d does not exist",
	      exe_ntnname, cmdnum, idnum);
      errormsg(string);
      return;
    }
  }

  /* now process the possible commands */
  switch (cmdnum) {
  case EX_ARMYLOC:
    /* set the army location */
    if (!XY_ONMAP(ival1, ival2)) {
      sprintf(string, "Error(%s): A_LOC, location %d, %d off map",
	      exe_ntnname, ival1, ival2);
      errormsg(string);
    } else {
      ARMY_XLOC = ival1;
      ARMY_YLOC = ival2;
      army_sort(FALSE);
    }
    break;
  case EX_ARMYOLOC:
    /* set the army location */
    if (!XY_ONMAP(ival1, ival2)) {
      sprintf(string, "Error(%s): A_OLOC, location %d, %d off map",
	      exe_ntnname, ival1, ival2);
      errormsg(string);
    } else {
      ARMY_LASTX = ival1;
      ARMY_LASTY = ival2;
    }
    break;
  case EX_ARMYTYPE:
    /* change the army type */
    if (ival1 < 0 || ival1 >= num_armytypes) {
      sprintf(string, "Error(%s): A_TYPE, type %d invalid",
	      exe_ntnname, ival1);
      errormsg(string);
    } else {
      ARMY_TYPE = ival1;
    }
    break;
  case EX_ARMYSTAT:
    /* change the army status */
    ARMY_STAT = lval1;
    break;
  case EX_ARMYMOVE:
    /* change a units movement */
    if (ival1 < 0 || ival1 > 200) {
      sprintf(string, "Error(%s): A_MOVE, movement of %d invalid",
	      exe_ntnname, ival1);
      errormsg(string);
    } else {
      ARMY_MOVE = ival1;
    }
    break;
  case EX_ARMYLEAD:
    /* change the leader */
    if (ival1 == EMPTY_HOLD) {
      ARMY_LEAD = EMPTY_HOLD;
    } else if (ARMY_ID == ival1) {
      sprintf(string, "Error(%s): A_LEAD, lead value %d equals id of unit",
	      exe_ntnname, ival1);
      errormsg(string);
    } else if ((army_tptr = armybynum(ival1)) == NULL) {
      sprintf(string, "Error(%s): A_LEAD, unit %d non-existant",
	      exe_ntnname, ival1);
      errormsg(string);
    } else if (!a_isleader(ARMYT_TYPE)) {
      sprintf(string, "Error(%s): A_LEAD, unit %d not a leader",
	      exe_ntnname, ival1);
      errormsg(string);
    } else {
      ARMY_LEAD = ival1;
    }
    break;
  case EX_ARMYSIZE:
    /* set the unit size */
    if (lval1 < 0) {
      sprintf(string, "Error(%s): A_SIZE, size %ld invalid",
	      exe_ntnname, lval1);
      errormsg(string);
    } else {
      ARMY_SIZE = lval1;
    }
    break;
  case EX_ARMYID:
    /* change a unit id number */
    if (ival1 < 0 || ival1 > MAX_IDTYPE || ival1 == EMPTY_HOLD) {
      sprintf(string, "Error(%s): A_ID, id number %d invalid",
	      exe_ntnname, ival1);
      errormsg(string);
    } else if (armybynum(ival1) != NULL) {
      sprintf(string, "Error(%s): A_ID, id number %d already in use",
	      exe_ntnname, ival1);
      errormsg(string);
    } else {
      army_renum(ival1);
      army_sort(FALSE);
    }
    break;
  case EX_ARMYSPLY:
    /* set the army supply */
    if (ival1 < 0 || ival1 > MAXSUPPLIES) {
      sprintf(string, "Error(%s): A_SPLY, supply value %d invalid",
	      exe_ntnname, ival1);
      errormsg(string);
    } else {
      ARMY_SPLY = ival1;
    }
    break;
  case EX_ARMYSPTS:
    /* set the army spell points */
    if (ival1 < 0 || ival1 > 200) {
      sprintf(string, "Error(%s): A_SPTS, spell points %d really big",
	      exe_ntnname, ival1);
      errormsg(string);
    } else {
      ARMY_SPTS = ival1;
    }
    break;
  case EX_ARMYEFF:
    /* set the army efficiency */
    if (ival1 < 0 || ival1 > ARMY_MAXEFF + 5) {
      sprintf(string, "Error(%s): A_EFF, efficiency %d > %d",
	      exe_ntnname, ival1, ARMY_MAXEFF + 5);
      errormsg(string);
    } else {
      ARMY_EFF = ival1;
    }
    break;
  case EX_ARMYMAXEFF:
    /* set the army maximum efficiency */
    if (ival1 < 0 || ival1 > 200) {
      sprintf(string, "Error(%s): A_EFF, max efficiency %d quite amazing",
	      exe_ntnname, ival1);
      errormsg(string);
    } else {
      ARMY_MAXEFF = ival1;
    }
    break;
  case EX_ARMYCREATE:
    /* build a new army unit */
    if (ival1 < 0 || ival1 >= num_armytypes) {
      sprintf(string, "Error(%s): A_CREATE, unit type %d invalid",
	      exe_ntnname, ival1);
      errormsg(string);
    } else {
      army_ptr = crt_army(ival1);
      if (idnum != ARMY_ID) {
	if (armybynum(idnum) != NULL) {
	  sprintf(string, "Error(%s): A_CREATE, id %d != %d",
		  exe_ntnname, idnum, ARMY_ID);
	  errormsg(string);
	} else {
#ifdef NOT_DONE
	  sprintf(string, "Warning(%s): A_CREATE, id %d != %d (fixed)",
		  exe_ntnname, idnum, ARMY_ID);
	  errormsg(string);
#endif /* NOT_DONE */
	  ARMY_ID = idnum;
	  army_sort(FALSE);
	}
      }
    }
    break;
  case EX_ARMYDESTROY:
    /* remove a unit */
    dest_army(idnum);
    army_ptr = NULL;
    break;
  case EX_ARMYMERCS:
    /* adjust the mercenary settings */
    if (is_update) {
      /* record adjustments to mercenary numbers */
      MERCMEN += ival1;
      if (MERCMEN < 0) MERCMEN = 0;
    } else if (ival1 < 0) {
      /* keep proper count of drafted mercs */
      conq_mercsused -= ival1;
    }
    break;
  default:
    /* uh oh! */
    sprintf(string, "Error(%s): unknown army command #%d",
	    exe_ntnname, cmdnum);
    errormsg(string);
    break;
  }
}

/* EXEC_NAVY -- Process navy commands */
static void
exec_navy PARM_0(void)
{
  /* set the navy pointer properly */
  if (cmdnum != EX_NAVYCREATE) {
    if (navy_ptr == NULL || NAVY_ID != idnum) {
      navy_ptr = navybynum(idnum);
    }

    /* verify navy */
    if (navy_ptr == NULL) {
      sprintf(string, "Error(%s): Cmd #%d, Navy #%d does not exist",
	      exe_ntnname, cmdnum, idnum);
      errormsg(string);
      return;
    }
  }

  /* now process the possible commands */
  switch (cmdnum) {
  case EX_NAVYLOC:
    /* set the navy location */
    if (!XY_ONMAP(ival1, ival2)) {
      sprintf(string, "Error(%s): N_LOC, location %d, %d off map",
	      exe_ntnname, ival1, ival2);
      errormsg(string);
    } else {
      NAVY_XLOC = ival1;
      NAVY_YLOC = ival2;
    }
    break;
  case EX_NAVYOLOC:
    /* set the navy location */
    if (!XY_ONMAP(ival1, ival2)) {
      sprintf(string, "Error(%s): N_OLOC, location %d, %d off map",
	      exe_ntnname, ival1, ival2);
      errormsg(string);
    } else {
      NAVY_LASTX = ival1;
      NAVY_LASTY = ival2;
    }
    break;
  case EX_NAVYSHIP:
    /* adjust the warship configuration */
    if (ival1 < 0 || ival1 >= NSHP_NUMBER) {
      sprintf(string, "Error(%s): N_SHIP, ship class %d invalid",
	      exe_ntnname, ival1);
      errormsg(string);
    } else if (ival2 < 0 || ival2 >= 1<<16) {
      sprintf(string, "Error(%s): N_SHIP, ship value %d invalid",
	      exe_ntnname, ival2);
      errormsg(string);
    } else {
      NAVY_SHIPS[ival1] = ival2;
    }
    break;
  case EX_NAVYSTAT:
    /* change the navy status */
    NAVY_STAT = lval1;
    break;
  case EX_NAVYMOVE:
    /* change the movement */
    if (ival1 < 0 || ival1 > 200) {
      sprintf(string, "Error(%s): N_MOVE, movement of %d invalid",
	      exe_ntnname, ival1);
      errormsg(string);
    } else {
      NAVY_MOVE = ival1;
    }
    break;
  case EX_NAVYID:
    /* change a unit id number */
    if (ival1 < 0 || ival1 > MAX_IDTYPE) {
      sprintf(string, "Error(%s): N_ID, id number %d invalid",
	      exe_ntnname, ival1);
      errormsg(string);
    } else if (navybynum(ival1) != NULL) {
      sprintf(string, "Error(%s): N_ID, id number %d already in use",
	      exe_ntnname, ival1);
      errormsg(string);
    } else {
      navy_renum(ival1);
      navy_sort();
    }
    break;
  case EX_NAVYCREW:
    /* fix the crew for the navy */
    if (ival1 < 0 || ival1 > 100) {
      sprintf(string, "Error(%s): N_CREW percentage %d out of range",
	      exe_ntnname, ival1);
      errormsg(string);
    } else {
      NAVY_CREW = ival1;
    }
    break;
  case EX_NAVYPEOP:
    /* adjust number of people carried per hold */
    if (ival1 < 0 || ival1 > 255) {
      sprintf(string, "Error(%s): N_PEOP value %d out of range",
	      exe_ntnname, ival1);
      errormsg(string);
    } else {
      NAVY_PEOP = ival1;
    }
    break;
  case EX_NAVYMTRLS:
    /* adjust raw materials */
    ival1 = (int) dval1;
    if ((ival1 < 0) || (ival1 >= MTRLS_NUMBER)) {
      sprintf(string, "Error(%s): N_MTRLS, invalid material type %d",
	      exe_ntnname, ival1);
      errormsg(string);
    } else if (dval2 < (itemtype) 0) {
      sprintf(string, "Error(%s): N_MTRLS, negative %s value",
	      exe_ntnname, mtrls_info[ival1].name);
      errormsg(string);
    } else {
      NAVY_MTRLS[ival1] = (itemtype) dval2;
    }
    break;
  case EX_NAVYSPLY:
    /* set the navy supply */
    if (ival1 < 0 || ival1 > MAXSUPPLIES * 4) {
      sprintf(string, "Error(%s): N_SPLY, supply value %d invalid",
	      exe_ntnname, ival1);
      errormsg(string);
    } else {
      NAVY_SPLY = ival1;
    }
    break;
  case EX_NAVYARMY:
    /* change the army unit being carried */
    if ((ival1 != EMPTY_HOLD) &&
	(armybynum(ival1) == NULL)) {
      sprintf(string, "Error(%s): N_ARMY, army unit %d non-existent",
	      exe_ntnname, ival1);
      errormsg(string);
    } else {
      NAVY_ARMY = ival1;
    }
    break;
  case EX_NAVYCVN:
    /* change the army unit being carried */
    if ((ival1 != EMPTY_HOLD) &&
	(cvnbynum(ival1) == NULL)) {
      sprintf(string, "Error(%s): N_CVN, navy unit %d non-existent",
	      exe_ntnname, ival1);
      errormsg(string);
    } else {
      NAVY_CVN = ival1;
    }
    break;
  case EX_NAVYEFF:
    /* change the efficiency */
    if ((ival1 < 0) || (ival1 > 100)) {
      sprintf(string, "Error(%s): N_EFF, efficiency %d overly impressive",
	      exe_ntnname, ival1);
      errormsg(string);
    } else if ((ival2 < 0) || (ival2 >= NSHP_NUMBER)) {
      sprintf(string, "Error(%s): N_EFF, no ship class %d",
	      exe_ntnname, ival2);
      errormsg(string);
    } else {
      NAVY_EFF[ival2] = ival1;
    }
    break;
  case EX_NAVYCREATE:
    /* build a new navy unit */
    navy_ptr = crt_navy();
    if (idnum != NAVY_ID) {
      if (navybynum(idnum) != NULL) {
	sprintf(string, "Error(%s): N_CREATE, id %d != %d",
		exe_ntnname, idnum, NAVY_ID);
	errormsg(string);
      } else {
	sprintf(string, "Warning(%s): N_CREATE, id %d != %d (fixed)",
		exe_ntnname, idnum, NAVY_ID);
	errormsg(string);
	NAVY_ID = idnum;
	navy_sort();
      }
    }
    break;
  case EX_NAVYDESTROY:
    /* remove a unit */
    if (navybynum(idnum) == NULL) {
      sprintf(string, "Error(%s): N_DESTROY, unit #%d non-existent",
	      exe_ntnname, idnum);
      errormsg(string);
    } else {
      dest_navy(idnum);
      navy_ptr = NULL;
    }
    break;
  default:
    /* uh oh! */
    sprintf(string, "Error(%s): unknown navy command #%d",
	    exe_ntnname, cmdnum);
    errormsg(string);
    break;
  }
}

/* EXEC_CVN -- Process caravan commands */
static void
exec_cvn PARM_0(void)
{
  /* set the caravan pointer properly */
  if (cmdnum != EX_CVNCREATE) {
    if (cvn_ptr == NULL || CVN_ID != idnum) {
      cvn_ptr = cvnbynum(idnum);
    }

    /* verify caravan */
    if (cvn_ptr == NULL) {
      sprintf(string, "Error(%s): Cmd #%d, Caravan #%d does not exist",
	      exe_ntnname, cmdnum, idnum);
      errormsg(string);
      return;
    }
  }

  /* now process the possible commands */
  switch (cmdnum) {
  case EX_CVNLOC:
    /* set the caravan location */
    if (!XY_ONMAP(ival1, ival2)) {
      sprintf(string, "Error(%s): V_LOC, location %d, %d off map",
	      exe_ntnname, ival1, ival2);
      errormsg(string);
    } else {
      CVN_XLOC = ival1;
      CVN_YLOC = ival2;
    }
    break;
  case EX_CVNOLOC:
    /* set the caravan location */
    if (!XY_ONMAP(ival1, ival2)) {
      sprintf(string, "Error(%s): V_OLOC, location %d, %d off map",
	      exe_ntnname, ival1, ival2);
      errormsg(string);
    } else {
      CVN_LASTX = ival1;
      CVN_LASTY = ival2;
    }
    break;
  case EX_CVNSIZE:
    /* adjust the size of the caravan */
    if (ival1 < 0 || ival1 > 255) {
      sprintf(string, "Error(%s): V_SIZE, size %d out of range",
	      exe_ntnname, ival1);
      errormsg(string);
    } else {
      CVN_SIZE = ival1;
    }
    break;
  case EX_CVNSTAT:
    /* change the caravan status */
    CVN_STAT = lval1;
    break;
  case EX_CVNMOVE:
    /* change the movement */
    if (ival1 < 0 || ival1 > 200) {
      sprintf(string, "Error(%s): V_MOVE, movement of %d invalid",
	      exe_ntnname, ival1);
      errormsg(string);
    } else {
      CVN_MOVE = ival1;
    }
    break;
  case EX_CVNID:
    /* change a unit id number */
    if (ival1 < 0 || ival1 > MAX_IDTYPE) {
      sprintf(string, "Error(%s): V_ID, id number %d invalid",
	      exe_ntnname, ival1);
      errormsg(string);
    } else if (cvnbynum(ival1) != NULL) {
      sprintf(string, "Error(%s): V_ID, id number %d already in use",
	      exe_ntnname, ival1);
      errormsg(string);
    } else {
      cvn_renum(ival1);
      cvn_sort();
    }
    break;
  case EX_CVNCREW:
    /* fix the crew for the caravan */
    if (ival1 < 0 || ival1 > 100) {
      sprintf(string, "Error(%s): V_CREW percentage %d out of range",
	      exe_ntnname, ival1);
      errormsg(string);
    } else {
      CVN_CREW = ival1;
    }
    break;
  case EX_CVNPEOP:
    /* adjust number of people carried per hold */
    if (ival1 < 0 || ival1 > 255) {
      sprintf(string, "Error(%s): V_PEOP value %d out of range",
	      exe_ntnname, ival1);
      errormsg(string);
    } else {
      CVN_PEOP = ival1;
    }
    break;
  case EX_CVNMTRLS:
    /* adjust raw materials */
    ival1 = (int) dval1;
    if ((ival1 < 0) || (ival1 >= MTRLS_NUMBER)) {
      sprintf(string, "Error(%s): V_MTRLS, invalid material type %d",
	      exe_ntnname, ival1);
      errormsg(string);
    } else if (dval2 < (itemtype) 0) {
      sprintf(string, "Error(%s): V_MTRLS, negative %s value",
	      exe_ntnname, mtrls_info[ival1].name);
      errormsg(string);
    } else {
      CVN_MTRLS[ival1] = (itemtype) dval2;
    }
    break;
  case EX_CVNSPLY:
    /* set the caravan supply */
    if (ival1 < 0 || ival1 > MAXSUPPLIES * 2) {
      sprintf(string, "Error(%s): V_SPLY, supply value %d invalid",
	      exe_ntnname, ival1);
      errormsg(string);
    } else {
      CVN_SPLY = ival1;
    }
    break;
  case EX_CVNEFF:
    /* set the caravan efficiency */
    if (ival1 < 0 || ival1 > 100) {
      sprintf(string, "Error(%s): V_EFF, efficiency value %d invalid",
	      exe_ntnname, ival1);
      errormsg(string);
    } else {
      CVN_EFF = ival1;
    }
    break;
  case EX_CVNCREATE:
    /* build a new caravan unit */
    cvn_ptr = crt_cvn();
    if (idnum != CVN_ID) {
      if (cvnbynum(idnum) != NULL) {
	sprintf(string, "Error(%s): V_CREATE, id %d != %d",
		exe_ntnname, idnum, CVN_ID);
	errormsg(string);
      } else {
	sprintf(string, "Warning: V_CREATE, id %d != %d (fixed)",
		idnum, CVN_ID);
	errormsg(string);
	CVN_ID = idnum;
	cvn_sort();
      }
    }
    break;
  case EX_CVNDESTROY:
    /* remove a unit */
    if (cvnbynum(idnum) == NULL) {
      sprintf(string, "Error(%s): V_DESTROY, unit #%d non-existent",
	      exe_ntnname, idnum);
      errormsg(string);
    } else {
      dest_cvn(idnum);
      cvn_ptr = NULL;
    }
    break;
  default:
    /* uh oh! */
    sprintf(string, "Error(%s): unknown caravan command #%d",
	    exe_ntnname, cmdnum);
    errormsg(string);
    break;
  }
}

/* EXEC_CITY -- Process city commands */
static void
exec_city PARM_0(void)
{
  /* set the city pointer properly */
  if (cmdnum != EX_CITYCREATE) {
    if (city_ptr == NULL ||
	str_test(CITY_NAME, str1)) {
      city_ptr = citybyname(str1);
    }

    /* verify city */
    if (city_ptr == NULL) {
      sprintf(string, "Error(%s): Cmd #%d, City %s does not exist",
	      exe_ntnname, cmdnum, str1);
      errormsg(string);
      return;
    }
  }

  /* now process the possible commands */
  switch (cmdnum) {
  case EX_CITYLOC:
    /* set the city location */
    if (!XY_ONMAP(ival1, ival2)) {
      sprintf(string, "Error(%s): C_LOC [%s], location %d, %d off map",
	      exe_ntnname, CITY_NAME, ival1, ival2);
      errormsg(string);
    } else {
      CITY_XLOC = ival1;
      CITY_YLOC = ival2;
    }
    break;
  case EX_CITYNAME:
    /* change the name */
    if (strlen(str2) == 0) {
      sprintf(string, "Error(%s): C_NAME, no name specified",
	      exe_ntnname);
      errormsg(string);
    } else if ((citybyname(str2) != NULL) &&
	       (citybyname(str2) != city_ptr)) {
      sprintf(string, "Error(%s): C_NAME, name %s already in use",
	      exe_ntnname, str2);
      errormsg(string);
    } else {
      strcpy(CITY_NAME, str2);
      city_sort();
    }
    break;
  case EX_CITYPEOP:
    /* adjust number of people in the city */
    if (ival1 < -50) {
      sprintf(string, "Error(%s): C_PEOP [%s] value %d out of range",
	      exe_ntnname, CITY_NAME, ival1);
      errormsg(string);
    } else {
      CITY_PEOPLE = ival1;
    }
    break;
  case EX_CITYWEIGHT:
    /* adjust distribution weighting */
    if ((ival1 < 0) || (ival1 > 255)) {
      sprintf(string, "Error(%s): C_WEIGHT [%s] value %d out of range",
	      exe_ntnname, CITY_NAME, ival1);
      errormsg(string);
    } else {
      CITY_WEIGHT = ival1;
      set_weights(FALSE);
    }
    break;
  case EX_CITYMTRLS:
    /* adjust raw materials */
    if ((idnum < 0) || (idnum >= MTRLS_NUMBER)) {
      sprintf(string, "Error(%s): C_MTRLS [%s], invalid material type %d",
	      exe_ntnname, CITY_NAME, ival1);
      errormsg(string);
    } else if ((idnum != MTRLS_TALONS) &&
	       (dval1 < (itemtype) 0)) {
      sprintf(string, "Error(%s): C_MTRLS [%s], negative %s value",
	      exe_ntnname, CITY_NAME, mtrls_info[idnum].name);
      errormsg(string);
    } else {
      CITY_MTRLS[idnum] = (itemtype) dval1;
    }
    break;
  case EX_CITYSTALONS:
    /* adjust starting treasury */
    CITY_STALONS = (itemtype) dval1;
    break;
  case EX_CITYIMTRLS:
    /* adjust incremental raw materials */
    if ((idnum < 0) || (idnum >= MTRLS_NUMBER)) {
      sprintf(string, "Error(%s): C_MTRLS [%s], invalid material type %d",
	      exe_ntnname, CITY_NAME, ival1);
      errormsg(string);
    } else if ((idnum != MTRLS_TALONS) &&
	       (dval1 < (itemtype) 0)) {
      sprintf(string, "Error(%s): C_MTRLS [%s], negative %s value",
	      exe_ntnname, CITY_NAME, mtrls_info[idnum].name);
      errormsg(string);
    } else {
      CITY_IMTRLS[idnum] = (itemtype) dval1;
    }
    break;
  case EX_CITYFORT:
    /* adjust the city fortification */
    if (ival1 < 0 || ival1 > MAXFORTVAL) {
      sprintf(string, "Error(%s), C_FORT [%s], fort value %d out of range",
	      exe_ntnname, CITY_NAME, ival1);
      errormsg(string);
    } else {
      CITY_FORT = ival1;
    }
    break;
  case EX_CITYCREATE:
    /* build a new city unit */
    if (citybyname(str1) != NULL) {
      sprintf(string, "Error(%s): C_CREATE, name of %s in use",
	      exe_ntnname, str1);
      errormsg(string);
    } else {
      city_ptr = crt_city(str1);
    }
    break;
  case EX_CITYDESTROY:
    /* remove a unit */
    dest_city(str1);
    city_ptr = NULL;
    set_weights(FALSE);
    break;
  default:
    /* uh oh! */
    sprintf(string, "Error(%s): unknown city command #%d",
	    exe_ntnname, cmdnum);
    errormsg(string);
    break;
  }
}

/* EXEC_ITEM -- Process item commands */
static void
exec_item PARM_0(void)
{
  /* set the item pointer properly */
  if (cmdnum != EX_ITEMCREATE) {
    if (item_ptr == NULL || ITEM_ID != idnum) {
      item_ptr = itembynum(idnum);
    }

    /* verify item */
    if (item_ptr == NULL) {
      sprintf(string, "Error(%s): Cmd #%d, Item #%d does not exist",
	      exe_ntnname, cmdnum, idnum);
      errormsg(string);
      return;
    }
  }

  /* now process the possible commands */
  switch (cmdnum) {
  case EX_ITEMLOC:
    /* set the item location */
    if (!XY_ONMAP(ival1, ival2)) {
      sprintf(string, "Error(%s): I_LOC, location %d, %d off map",
	      exe_ntnname, ival1, ival2);
      errormsg(string);
    } else {
      ITEM_XLOC = ival1;
      ITEM_YLOC = ival2;
    }
    break;
  case EX_ITEMID:
    /* change the name */
    if (ival1 < 0 || ival1 > MAX_IDTYPE) {
      sprintf(string, "Error(%s): I_ID, number %d out of range",
	      exe_ntnname, ival1);
      errormsg(string);
    } else if (itembynum(ival1) != NULL) {
      sprintf(string, "Error(%s): I_ID, number %d already in use",
	      exe_ntnname, ival1);
      errormsg(string);
    } else {
      ITEM_ID = ival1;
      item_sort();
    }
    break;
  case EX_ITEMMTRLS:
    /* adjust raw materials */
    ival1 = (int) dval1;
    if ((ival1 < 0) || (ival1 >= MTRLS_NUMBER)) {
      sprintf(string, "Error(%s): I_MTRLS, invalid material type %d",
	      exe_ntnname, ival1);
      errormsg(string);
    } else if ((ival1 != MTRLS_TALONS) &&
	       (dval2 < (itemtype) 0)) {
      sprintf(string, "Error(%s): I_MTRLS, negative %s value",
	      exe_ntnname, mtrls_info[ival1].name);
      errormsg(string);
    } else {
      ITEM_MTRLS[ival1] = (itemtype) dval2;
    }
    break;
  case EX_ITEMCREATE:
    /* build a new item unit */
    item_ptr = crt_item();
    if (idnum != ITEM_ID) {
      if (itembynum(idnum) != NULL) {
	sprintf(string, "Error(%s): I_CREATE, id %d != %d",
		exe_ntnname, idnum, ITEM_ID);
	errormsg(string);
      } else {
	sprintf(string, "Warning: I_CREATE, id %d != %d (fixed)",
		idnum, ITEM_ID);
	errormsg(string);
	ITEM_ID = idnum;
	item_sort();
      }
    }
    break;
  case EX_ITEMMEN:
    /* number of men needed */
    if (lval1 < 0) {
      sprintf(string, "Error(%s): I_MEN(%d), Negative value %d",
	      exe_ntnname, idnum, lval1);
      errormsg(string);
    } else {
      ITEM_MEN = lval1;
    }
    break;
  case EX_ITEMINFO:
    /* information */
    ITEM_INFO = lval1;
    break;
  case EX_ITEMTYPE:
    /* what type of item is it */
    ITEM_TYPE = ival1;
    break;
  case EX_ITEMARMY:
    /* army unit */
    if (armybynum(ival1) == NULL) {
      sprintf(string, "Error(%s): I_ARMY(%d), No such army %d",
	      exe_ntnname, idnum, ival1);
      errormsg(string);
    } else {
      ITEM_ARMY = ival1;
    }
    break;
  case EX_ITEMNAVY:
    /* navy unit */
    if (navybynum(ival1) == NULL) {
      sprintf(string, "Error(%s): I_NAVY(%d), No such navy %d",
	      exe_ntnname, idnum, ival1);
      errormsg(string);
    } else {
      ITEM_CVN = ival1;
    }
    break;
  case EX_ITEMCVN:
    /* caravan unit */
    if (cvnbynum(ival1) == NULL) {
      sprintf(string, "Error(%s): I_CVN(%d), No such caravan %d",
	      exe_ntnname, idnum, ival1);
      errormsg(string);
    } else {
      ITEM_CVN = ival1;
    }
    break;
  case EX_ITEMDESTROY:
    /* remove a unit */
    dest_item(idnum);
    item_ptr = NULL;
    break;
  default:
    /* uh oh! */
    sprintf(string, "Error(%s): unknown commodity command #%d",
	    exe_ntnname, cmdnum);
    errormsg(string);
    break;
  }
}

/* EXEC_TAKE -- Handle the resource selection commands */
static void
exec_take PARM_0(void)
{
  static ITEM_PTR cost_ptr = NULL;
  int i;

  /* check input */
  if (cmdnum != EX_TAKESTART) {
    if (cost_ptr == NULL) {
      sprintf(string, "Error(%s): cost_ptr not set for command #%d",
	      exe_ntnname, cmdnum);
      errormsg(string);
      return;
    }
  } else {
    if (cost_ptr != NULL) {
      sprintf(string, "Warning(%s): cost_ptr already set for take start",
	      exe_ntnname);
      errormsg(string);
    }
  }

  /* process the commands */
  switch (cmdnum) {
  case EX_TAKESTART:
    cost_ptr = new_item();
    cost_ptr->xloc = ival1;
    cost_ptr->yloc = ival2;
    for (i = 0; i < MTRLS_NUMBER; i++) {
      cost_ptr->mtrls[i] = (itemtype) 0;
    }
    break;
  case EX_TAKEFINISH:
    if ((ival1 != cost_ptr->xloc) || (ival2 != cost_ptr->yloc)) {
      sprintf(string, "Error(%s): T_FINISH, non-matching sectors",
	      exe_ntnname);
      errormsg(string);
      break;
    }
    if (take_resources(ival1, ival2, cost_ptr, idnum)) {
      sprintf(string, "Error(%s): T_FINISH, resource take failed",
	      exe_ntnname);
      errormsg(string);
    }
    free(cost_ptr);
    cost_ptr = NULL;
    break;
  case EX_TAKEMTRLS:
  default:
    if ((idnum != cost_ptr->xloc) || (lval1 != cost_ptr->yloc)) {
      sprintf(string, "Error(%s): T_MTRLS, non-matching sectors",
	      exe_ntnname);
      errormsg(string);
      break;
    } else if ((cmdnum < EX_TAKEMTRLS) ||
	       (cmdnum >= EX_TAKEMTRLS + MTRLS_NUMBER)) {
      /* uh oh! */
      sprintf(string, "Error(%s): unknown take command #%d",
	      exe_ntnname, cmdnum);
      errormsg(string);
    } else {
      cost_ptr->mtrls[cmdnum - EX_TAKEMTRLS] = (itemtype) lval2;
    }
    break;
  }
}

/* EXEC_GIVE -- Handle the resource distribution commands */
static void
exec_give PARM_0(void)
{
  int i;
  static ITEM_PTR give_ptr = NULL;

  /* check input */
  if (cmdnum != EX_GIVESTART) {
    if (give_ptr == NULL) {
      sprintf(string, "Error(%s): give_ptr not set for command #%d",
	      exe_ntnname, cmdnum);
      errormsg(string);
      return;
    }
  } else {
    if (give_ptr != NULL) {
      sprintf(string, "Warning(%s): give_ptr already set for take start",
	      exe_ntnname);
      errormsg(string);
    }
  }

  /* process the commands */
  switch (cmdnum) {
  case EX_GIVESTART:
    give_ptr = new_item();
    give_ptr->xloc = ival1;
    give_ptr->yloc = ival2;
    for (i = 0; i < MTRLS_NUMBER; i++) {
      give_ptr->mtrls[i] = (itemtype) 0;
    }
    break;
  case EX_GIVEFINISH:
    if ((ival1 != give_ptr->xloc) || (ival2 != give_ptr->yloc)) {
      sprintf(string, "Error(%s): G_FINISH, non-matching sectors",
	      exe_ntnname);
      errormsg(string);
      break;
    }
    if (send_resources(ival1, ival2, give_ptr, idnum)) {
      sprintf(string, "Error(%s): G_FINISH, resource redistribution failed",
	      exe_ntnname);
      errormsg(string);
    }
    free(give_ptr);
    give_ptr = NULL;
    break;
  case EX_GIVEMTRLS:
  default:
    if ((idnum != give_ptr->xloc) || (lval1 != give_ptr->yloc)) {
      sprintf(string, "Error(%s): G_MTRLS, non-matching sectors",
	      exe_ntnname);
      errormsg(string);
      break;
    } else if ((cmdnum < EX_GIVEMTRLS) ||
	       (cmdnum >= EX_GIVEMTRLS + MTRLS_NUMBER)) {
      /* uh oh! */
      sprintf(string, "Error(%s): unknown give command #%d",
	      exe_ntnname, cmdnum);
      errormsg(string);
    } else {
      give_ptr->mtrls[cmdnum - EX_GIVEMTRLS] = (itemtype) lval2;
    }
    break;
  }
}

/* EXEC_MISC -- This routine takes care of the remaining commands */
static void
exec_misc PARM_0(void)
{
  char buf[LINELTH];
  long combpow;

  switch (cmdnum) {
  case EX_GRPLOC:
    /* set the group location */
    if (((army_ptr = armybynum(idnum)) == NULL) ||
	!unit_leading(ARMY_STAT)) {
      sprintf(string, "Error(%s): G_LOC, unknown group leader number %d",
	      exe_ntnname, idnum);
      errormsg(string);
    } else if (!XY_ONMAP(ival1, ival2)) {
      sprintf(string, "Error(%s): G_LOC, location %d,%d off map",
	      exe_ntnname, ival1, ival2);
      errormsg(string);
    } else {
      set_grploc(idnum, ival1, ival2);
    }
    break;
  case EX_GRPMOVE:
    /* set the group movement */
    if (((army_ptr = armybynum(idnum)) == NULL) ||
	!unit_leading(ARMY_STAT)) {
      sprintf(string, "Error(%s): G_MOVE, unknown group leader number %d",
	      exe_ntnname, idnum);
      errormsg(string);
    } else if (ival1 < SPD_MINIMUM || ival1 > SPD_MINIMUM + SPD_STUCK) {
      sprintf(string, "Error(%s): G_MOVE, speed %d out of range",
	      exe_ntnname, ival1);
      errormsg(string);
    } else if (ival2 < 0 || ival2 > 255) {
      sprintf(string, "Error(%s): G_MOVE, movement %d out of range",
	      exe_ntnname, ival2);
      errormsg(string);
    } else {
      set_grpmove(idnum, ival1, ival2);
    }
    break;
  case EX_NTNNAME:
    /* set the nation name */
    if ((ntnbyname(str1) != NULL) &&
	(ntnbyname(str1) != ntn_ptr)) {
      sprintf(string, "Error(%s): X_NAME, name %s in use",
	      exe_ntnname, str1);
      errormsg(string);
    } else if ((str1[0] == '\0') || (strlen(str1) > NAMELTH)) {
      sprintf(string, "Error(%s): X_NAME, invalid namelength of %d",
	      exe_ntnname, strlen(str1));
      errormsg(string);
    } else {
      if (is_update == TRUE) {
	fprintf(fnews, "1.\tNation %s changes their name to %s\n",
		ntn_ptr->name, str1);
	fprintf(fupdate, "==> Renaming Nation %s to %s\n",
		ntn_ptr->name, str1);
	sprintf(string, "%s.%s", ntn_ptr->name, msgtag);
	sprintf(buf, "%s.%s", str1, msgtag);
	if (move_file(string, buf) != 0) {
	  fprintf(fupdate, "   (could not find a mail file to move)\n");
	}
      }
      strcpy(ntn_ptr->name, str1);
    }
    break;
  case EX_NTNLOGIN:
    /* set the user name */
    if (user_exists(str1) == FALSE) {
      sprintf(string, "Error(%s): X_LOGIN, no user %s on system",
	      exe_ntnname, str1);
      errormsg(string);
    } else {
      if (is_update == TRUE) {
	if (!world.hide_login) {
	  fprintf(fnews, "1.\tUser %s now controls nation %s\n",
		  str1, exe_ntnname);
	}
	fprintf(fupdate, "==> Changing user of %s from %s to %s\n",
		exe_ntnname, ntn_ptr->login, str1);
      }
      strcpy(ntn_ptr->login, str1);
    }
    break;
  case EX_NTNPASSWD:
    /* set the password */
    if ((str1[0] == '\0') || (strlen(str1) > PASSLTH)) {
      sprintf(string, "Error(%s): X_PASSWD, invalid password length of %d",
	      exe_ntnname, strlen(str1));
      errormsg(string);
    } else {
      strcpy(ntn_ptr->passwd, str1);
    }
    break;
  case EX_NTNLEADER:
    /* change the leader name */
    if ((str1[0] == '\0') || (strlen(str1) > LEADERLTH)) {
      sprintf(string, "Error(%s): X_LEADER, invalid leader length of %d",
	      exe_ntnname, strlen(str1));
      errormsg(string);
    } else {
      strcpy(ntn_ptr->leader, str1);
    }
    break;
  case EX_NTNLOC:
    /* change the capital location */
    if (!XY_ONMAP(ival1, ival2)) {
      sprintf(string, "Error(%s): X_LOC, location %d, %d off map",
	     exe_ntnname, ival1, ival2);
      errormsg(string);
    } else {
      ntn_ptr->capx = ival1;
      ntn_ptr->capy = ival2;
    }
    break;
  case EX_NTNAPLUS:
    /* change the attack bonus */
    ntn_ptr->aplus = ival1;
    break;
  case EX_NTNDPLUS:
    /* change the defense bonus */
    ntn_ptr->dplus = ival1;
    break;
  case EX_NTNACTIVE:
    /* change the nation activity */
    if (idnum < 0 || idnum >= MAXNTN) {
      sprintf(string, "Error(%s): X_ACTIVE, nation number %d out of range",
	      exe_ntnname, idnum);
      errormsg(string);
    } else if (ival1 < 0 || ival1 > ACT_MAXIMUM) {
      sprintf(string, "Error(%s): X_ACTIVE, value %d out of range",
	      exe_ntnname, ival1);
      errormsg(string);
    } else if ((ntn_tptr = world.np[idnum]) == NULL) {
      sprintf(string, "Error(%s): X_ACTIVE, NULL nation number %d",
	      exe_ntnname, idnum);
      errormsg(string);
    } else {
      if ((ntn_tptr->active == INACTIVE) &&
	  (strcmp("god", exe_ntnname) != 0)) break;
      if (is_update &&
	  (strcmp("god", exe_ntnname) == 0)) {
	if (ival1 == INACTIVE) {
	  fprintf(fupdate, "    Nation %s being deleted\n", ntn_tptr->name);
	} else if (ntn_tptr->active == INACTIVE) {
	  fprintf(fupdate, "    Nation %s being reinstated\n",
		  ntn_tptr->name);
	}
      }
      ntn_tptr->active = ival1;
    }
    break;
  case EX_NTNDIPLO:
    /* change the diplomacy value */
    if ((ival1 < 0) || (ival1 >= ABSMAXNTN) ||
	(world.np[ival1] == NULL)) {
      sprintf(string, "Error(%s): X_DIPLO, invalid nation #%d",
	      exe_ntnname, ival1);
      errormsg(string);
    } else if (ival2 < 0 || ival2 >= dstatus_number * dstatus_number) {
      sprintf(string, "Error(%s): X_DIPLO, status %d out of range",
	      exe_ntnname, ival2);
      errormsg(string);
    } else {
      if (is_update) {
	ival2 %= dstatus_number;
	if (ival2 == DIP_ALLIED) {
	  if ((world.np[ival1])->dstatus[country] == DIP_ALLIED) {
	    fprintf(fnews, "1.\tNations %s and %s form an alliance.\n",
		    ntn_ptr->name, (world.np[ival1])->name);
	  } else {
	    fprintf(fnews, "1.\tNation %s offers an alliance to %s.\n",
		    ntn_ptr->name, (world.np[ival1])->name);
	  }
	} else if (ntn_ptr->dstatus[ival1] == DIP_ALLIED) {
	  fprintf(fnews, "1.\tNation %s withdraws the alliance offer to %s.\n",
		    ntn_ptr->name, (world.np[ival1])->name);
	} else if ((ntn_ptr->dstatus[ival1] > DIP_BELLICOSE) &&
		   (ival2 <= DIP_BELLICOSE)) {
	  fprintf(fnews, "1.\tNation %s ends the war on %s.\n",
		    ntn_ptr->name, (world.np[ival1])->name);
	}
	if (ival2 > DIP_BELLICOSE) {
	  fprintf(fnews, "1.\tNation %s declares %s on %s.\n",
		  ntn_ptr->name, dipname[ival2],
		  (world.np[ival1])->name);
	}
      }
      ntn_ptr->dstatus[ival1] = ival2;
    }
    break;
  case EX_NTNBUTE:
    /* change the charity level */
    if (idnum < 0 || idnum >= BUTE_NUMBER) {
      sprintf(string, "Error(%s): X_BUTE, unknown attribute type %d",
	      exe_ntnname, idnum);
      errormsg(string);
    } else if ((ival1 < bute_info[idnum].min_base) ||
	       (ival1 > bute_info[idnum].max_base)) {
      sprintf(string, "Error(%s): X_BUTE, value %d out of range for %s",
	      exe_ntnname, ival1, bute_info[idnum].name);
      errormsg(string);
    } else {
      ntn_ptr->attribute[idnum] = ival1;
    }
    break;
  case EX_NTNRLOC:
    /* change the capital location */
    if (!XY_ONMAP(ival1, ival2)) {
      sprintf(string, "Error(%s): X_RLOC, location [%d,%d] off map",
	     exe_ntnname, ival1, ival2);
      errormsg(string);
    } else {
      ntn_ptr->centerx = ival1;
      ntn_ptr->centery = ival2;
    }
    break;
  case EX_NTNREPRO:
    /* change the reproduction value of the nation */
    if ((ival1 < 0) || (ival1 > 15)) {
      sprintf(string, "Error(%s): X_REPRO, repro of %d invalid",
	      exe_ntnname, ival1);
      errormsg(string);
    } else {
      ntn_ptr->repro = ival1;
    }
    break;
  case EX_NTNRACE:
    if ((ival1 < 0) || (ival1 >= RACE_NUMBER)) {
      sprintf(string, "Error(%s): X_RACE, race of %d invalid",
	      exe_ntnname, ival1);
      errormsg(string);
    } else {
      ntn_ptr->race = ival1;
    }
    break;
  case EX_NTNMARK:
    if (!markok(ival1, (ntn_ptr != NULL)?ntn_ptr->race:UNOWNED, FALSE)) {
      sprintf(string, "Error(%s): X_MARK, mark of %d[%c] invalid",
	      exe_ntnname, ival1, isprint(ival1)?ival1:' ');
      errormsg(string);
    } else {
      ntn_ptr->mark = ival1;
    }
    break;
  case EX_NTNLEDGE:
    if ((ival1 > MAPX) || (ival1 < - MAPX)) {
      sprintf(string, "Error(%s): X_LEDGE, X location %d off map",
	      exe_ntnname, ival1);
      errormsg(string);
    } else {
      ntn_ptr->leftedge = ival1;
    }
    break;
  case EX_NTNREDGE:
    if ((ival1 > MAPX) || (ival1 < - MAPX)) {
      sprintf(string, "Error(%s): X_REDGE, X location %d off map",
	      exe_ntnname, ival1);
      errormsg(string);
    } else {
      ntn_ptr->rightedge = ival1;
    }
    break;
  case EX_NTNBEDGE:
    if ((ival1 > MAPY) || (ival1 < 0)) {
      sprintf(string, "Error(%s): X_BEDGE, Y location %d off map",
	      exe_ntnname, ival1);
      errormsg(string);
    } else {
      ntn_ptr->bottomedge = ival1;
    }
    break;
  case EX_NTNTEDGE:
    if ((ival1 > MAPY) || (ival1 < 0)) {
      sprintf(string, "Error(%s): X_TEDGE, Y location %d off map",
	      exe_ntnname, ival1);
      errormsg(string);
    } else {
      ntn_ptr->topedge = ival1;
    }
    break;
  case EX_NTNCLASS:
    /* adjust national class */
    if ((ival1 < 0) || (ival1 >= nclass_number)) {
      sprintf(string, "Error(%s): X_CLASS, class type %d invalid",
	      exe_ntnname, ival1);
      errormsg(string);
    } else {
      ntn_ptr->class = ival1;
    }
    break;
  case EX_NTNSCORE:
    /* adjust national score */
    ntn_ptr->score = lval1;
    break;
  case EX_NTNMOVE:
    /* adjustment of national movement */
    if ((ival1 < 4) || (ival1 > 50)) {
      sprintf(string, "Error(%s): X_NMOVE, movement value of %d invalid",
	      exe_ntnname, ival1);
      errormsg(string);
    } else {
      ntn_ptr->maxmove = ival1;
    }
    break;
  case EX_BRIBE:
    /* bribery */
    if ((idnum < 0) || (idnum >= ABSMAXNTN)) {
      sprintf(string, "Error(%s): X_BRIBE, invalid country %d",
	      exe_ntnname, idnum);
      errormsg(string);
    } else if (world.np[idnum] == NULL) {
      sprintf(string, "Error(%s): X_BRIBE, no country %d",
	      exe_ntnname, idnum);
      errormsg(string);
    } else {
      /* perform the bribe */
      /* bribe_ntn(country, idnum, dval1) UNIMPLEMENTED */;
    }
    break;
  case EX_NEWSSIZE:
    if ((is_god == TRUE) &&
	(ntn_ptr == NULL)) {
      world.news_size = lval1;
    } else if (ntn_ptr != NULL) {
      ntn_ptr->news_size = lval1;
    }
    break;
  case EX_MAILSIZE:
    if ((is_god == TRUE) &&
	(ntn_ptr == NULL)) {
      world.mbox_size = lval1;
    } else if (ntn_ptr != NULL) {
      ntn_ptr->mbox_size = lval1;
    }
    break;
  case EX_SCTDESG:
    /* change the sector designation */
    if (!XY_ONMAP(ival1, ival2)) {
      sprintf(string, "Error(%s): S_DESG, location %d, %d off map",
	      exe_ntnname, ival1, ival2);
      errormsg(string);
    } else if (idnum < 0 || idnum > 0xFFFF) {
      sprintf(string, "Error(%s): S_DESG, invalid designation %d",
	      exe_ntnname, idnum);
      errormsg(string);
    } else {
      sct[ival1][ival2].designation = idnum;
    }
    break;
  case EX_SCTPEOPLE:
    /* change the population */
    if (!XY_ONMAP(idnum, lval1)) {
      sprintf(string, "Error(%s): S_DESG, location %d, %ld off map",
	      exe_ntnname, idnum, lval1);
      errormsg(string);
    } else if (lval2 < 0L) {
      sprintf(string, "Error(%s): S_DESG, population %ld invalid",
	      exe_ntnname, lval2);
      errormsg(string);
    } else {
      sct[idnum][lval1].people = lval2;
    }
    break;
  case EX_SCTOWN:
    /* change the owner */
    if (!XY_ONMAP(ival1, ival2)) {
      sprintf(string, "Error(%s): S_OWN, location %d, %d off map",
	      exe_ntnname, ival1, ival2);
      errormsg(string);
    } else if ((idnum < 0) || (idnum >= ABSMAXNTN) ||
	       ((idnum != UNOWNED) && (world.np[idnum] == NULL))) {
      sprintf(string, "Error(%s): S_OWN, owner #%d invalid",
	      exe_ntnname, idnum);
      errormsg(string);
    } else {
      sct[ival1][ival2].owner = idnum;
    }
    break;
  case EX_SCTALT:
    /* change the altitude */
    if (!XY_ONMAP(ival1, ival2)) {
      sprintf(string, "Error(%s): S_ALT, location %d, %d off map",
	      exe_ntnname, ival1, ival2);
      errormsg(string);
    } else if ((idnum < 0) || (idnum > ELE_PEAK)) {
      sprintf(string, "Error(%s): S_ALT, altitude #%d invalid",
	      exe_ntnname, idnum);
      errormsg(string);
    } else {
      sct[ival1][ival2].altitude = idnum;
    }
    break;
  case EX_SCTVEG:
    /* change the vegetation */
    if (!XY_ONMAP(ival1, ival2)) {
      sprintf(string, "Error(%s): S_VEG, location %d, %d off map",
	      exe_ntnname, ival1, ival2);
      errormsg(string);
    } else if ((idnum < 0) || (idnum > VEG_NONE)) {
      sprintf(string, "Error(%s): S_VEG, vegetation #%d invalid",
	      exe_ntnname, idnum);
      errormsg(string);
    } else {
      sct[ival1][ival2].vegetation = idnum;
    }
    break;
  case EX_SCTTGOOD:
    /* change the tradegood */
    if (!XY_ONMAP(ival1, ival2)) {
      sprintf(string, "Error(%s): S_TGOOD, location %d, %d off map",
	      exe_ntnname, ival1, ival2);
      errormsg(string);
    } else if ((idnum < 0) || (idnum >= tgoods_number)) {
      sprintf(string, "Error(%s): S_TGOOD, tradegood #%d invalid",
	      exe_ntnname, idnum);
      errormsg(string);
    } else {
      sct[ival1][ival2].tradegood = idnum;
    }
    break;
  case EX_SCTMNRLS:
    /* change the minerals value */
    if (!XY_ONMAP(ival1, ival2)) {
      sprintf(string, "Error(%s): S_MNRLS, location %d, %d off map",
	      exe_ntnname, ival1, ival2);
      errormsg(string);
    } else if ((idnum < 0) || (idnum > 255)) {
      sprintf(string, "Error(%s): S_MNRLS, minerals value %d invalid",
	      exe_ntnname, idnum);
      errormsg(string);
    } else {
      sct[ival1][ival2].minerals = idnum;
    }
    break;
  case EX_MGK_ADJ:
    /* set the magic powers */
    if ((idnum < 0) ||
	(idnum >= MAG_NUMBER)) {
      sprintf(string, "Warning(%s): M_ADJ, power class %d out of range",
	      exe_ntnname, idnum);
      errormsg(string);
      break;
    } 
    if (lval2 != ntn_ptr->powers[idnum]) {
      sprintf(string, "Warning(%s): M_ADJ, %s power checksum doesn't match",
	      exe_ntnname, mclass_list[idnum].name);
      errormsg(string);
    }
    combpow = lval1 | lval2;
    add_powers(idnum, combpow ^ lval2);
    kill_powers(idnum, combpow ^ lval1);
    break;
  case EX_MGKSPELL:
    /* examine or record the spells */
    check_spells(idnum, ival1, ival2);
    break;
  case EX_MGKSENDING:
    /* check the input */
    if ((idnum < 0) ||
	(idnum >= num_armytypes) ||
	!a_ismonster(idnum)) {
      sprintf(string, "Error(%s): M_SEND: %d not a monster unit",
	      exe_ntnname, idnum);
      errormsg(string);
      break;
    }
    if (world.np[ival1] == NULL) {
      sprintf(string, "Error(%s): M_SEND: %d is invalid nation target",
	      exe_ntnname, ival1);
      errormsg(string);
      break;
    }

    /* don't do it until the update */
    if (is_update == FALSE) break;

    /* create the unit */
    if (ival1 != country) {
      rand_sector((world.np[ival1])->capx, (world.np[ival1])->capy,
		  5, TRUE, TRUE);
    } else {
      global_int = ntn_ptr->capx;
      global_long = ntn_ptr->capy;
    }
    if (make_madunit(((ival1 != country) && (country != UNOWNED))
		     ? country : -1, idnum, 1, global_int, global_long)) {
      sprintf(string, "Error(%s): M_SEND: failed without savage nation",
	      exe_ntnname);
      errormsg(string);
    } else {
      /* SEND MAIL TO VICTIM: UNIMPLEMENTED */
    }
    break;
  case EX_NTNRENUM:
    /* simply run the reorganization function */
    army_reorganize();
    break;
  case EX_UNUMCOPY:
    /* copy in the default settings */
    unum_copydefault();
    army_ptr = NULL;
    break;
  case EX_UNUMRESET:
    /* reset the national slot */
    resetnumbers();
    army_ptr = NULL;
    break;
  case EX_UNUMDEFAULT:
    /* use the built in defaults */
    unum_defaults();
    army_ptr = NULL;
    break;
  case EX_UNUMSLOT:
    /* check the data, then assign it */
    newslotnumber(idnum, ival1, ival2);
    army_ptr = NULL;
    break;
  default:
    /* uh oh! */
    sprintf(string, "Error(%s): unknown miscellaneous command #%d",
	    exe_ntnname, cmdnum);
    errormsg(string);
    break;
  }
}

/* EXECUTE -- Read in the commands entered using the conquer interface
              Returns 0 for no file, 1 for success, > or -1 for errors */
int
execute PARM_1(int, cntry)
{
  ARMY_PTR ahold_ptr = army_ptr;
  CITY_PTR chold_ptr = city_ptr;
  NAVY_PTR nhold_ptr = navy_ptr;
  CVN_PTR vhold_ptr = cvn_ptr;
  ITEM_PTR ihold_ptr = item_ptr;
  NTN_PTR ntnhld_ptr = ntn_ptr;
  int cmdhold;

  /* useful string declarations */
  char line[BIGLTH];
  int err, numlines = 0, hold = 0;

  /* check validity */
  if (cntry < 0 || cntry >= ABSMAXNTN) return(-1);

  /* now check if the nation exists */
  if (cntry != UNOWNED) {
    if ((ntn_ptr = world.np[cntry]) == NULL)
      return(-1);
  } else {
    ntn_ptr = NULL;
  }

  /* clear out the generic pointers */
  city_ptr = NULL;
  army_ptr = NULL;
  navy_ptr = NULL;
  cvn_ptr = NULL;
  item_ptr = NULL;

  /* open the file */
  if (cntry == UNOWNED) {
    sprintf(string, "god.%s", exetag);
    strcpy(exe_ntnname, "god");
  } else {
    sprintf(string, "%s.%s", ntn_ptr->name, exetag);
    strcpy(exe_ntnname, ntn_ptr->name);
  }
  if ((fexe = fopen(string, "r")) != NULL) {

    /* set the weights for the nation */
    set_weights(TRUE);

    /* read all of the information from the file */
    while (!feof(fexe)) {

      /* get the string from the file */
      if (fgets(line, BIGLTH, fexe) != NULL) {

	/* parse the input */
	numlines++;
	if (line[1] != '_') {
	  sprintf(string, "Misformatted line #%d in file %s.%s",
		  numlines, exe_ntnname, exetag);
	  errormsg(string);
	  sprintf(string, "Line: %s", line);
	  errormsg(string);
	  hold++;
	  continue;
	}

	/* determine input type */
	err = FALSE;
	cmdhold = 0;
	if (line[0] == 'D') {
	  /* double values -- %lf needed no matter what anyone says */
	  if (sscanf(line, "%s\t%d\t%d\t%lf\t%lf\t%12s\t%12s", string,
		     &cmdhold, &idnum, &dval1, &dval2, str1, str2) != 7) {
	    err = TRUE;
	  }
	} else if (line[0] == 'L') {
	  /* long values */
	  if (sscanf(line, "%s\t%d\t%d\t%ld\t%ld\t%12s\t%12s", string,
		     &cmdhold, &idnum, &lval1, &lval2, str1, str2) != 7) {
	    err = TRUE;
	  }
	} else {
	  /* integer values */
	  if (sscanf(line, "%s\t%d\t%d\t%d\t%d\t%12s\t%12s", string,
		     &cmdhold, &idnum, &ival1, &ival2, str1, str2) != 7) {
	    err = TRUE;
	  }
	}
	cmdnum = cmdhold;

	/* report any problems */
	if (err == TRUE) {
	  sprintf(string, "Misformatted line #%d in file %s.%s",
		  numlines, exe_ntnname, exetag);
	  errormsg(string);
	  sprintf(string, "Line: %s", line);
	  errormsg(string);
	  hold++;
	  continue;
	}

	/* now check all entries */
	if (cmdnum >= EX_ARMYBEGIN && cmdnum <= EX_ARMYEND) {
	  /* process the army commands */
	  exec_army();
	} else if (cmdnum >= EX_NAVYBEGIN && cmdnum <= EX_NAVYEND) {
	  /* process navy commands */
	  exec_navy();
	} else if (cmdnum >= EX_CVNBEGIN && cmdnum <= EX_CVNEND) {
	  /* process caravan commands */
	  exec_cvn();
	} else if (cmdnum >= EX_CITYBEGIN && cmdnum <= EX_CITYEND) {
	  /* process city commands */
	  trim_str(str1, FALSE);
	  trim_str(str2, FALSE);
	  exec_city();
	} else if (cmdnum >= EX_ITEMBEGIN && cmdnum <= EX_ITEMEND) {
	  /* process commodity commands */
	  exec_item();
	} else if (cmdnum >= EX_TAKEBEGIN && cmdnum <= EX_TAKEEND) {
	  /* process resource extraction */
	  exec_take();
	} else if (cmdnum >= EX_GIVEBEGIN && cmdnum <= EX_GIVEEND) {
	  /* process resource redistribution */
	  exec_give();
	} else {
	  /* process group, nation, sector, and magic commands */
	  trim_str(str1, FALSE);
	  trim_str(str2, FALSE);
	  exec_misc();
	}

      } /* if ... */
    }  /* while (!foef) ... */

    /* close the file */
    hold++;
    fclose(fexe);
  }

  /* calculate national totals */
  ntn_totals(cntry);

  /* return things to their original state */
  army_ptr = ahold_ptr;
  city_ptr = chold_ptr;
  navy_ptr = nhold_ptr;
  cvn_ptr = vhold_ptr;
  item_ptr = ihold_ptr;
  ntn_ptr = ntnhld_ptr;

  /* send back value */
  return(hold);
}
