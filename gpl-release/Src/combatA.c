// SPDX-License-Identifier: GPL-3.0-or-later
/* Resolve conflicts and provide for general dismemberment */
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
#include "navyX.h"
#include "butesX.h"
#include "desigX.h"
#include "magicX.h"
#include "worldX.h"
#include "activeX.h"
#include "combatA.h"
#include "elevegX.h"
#include "statusX.h"
#include "caravanX.h"

/* combat groupings */
static char *side_desc[CGRP_NUMBER] = {
  "Sweeping", "Attacking", "Defending", "Fortified", "Protected"
};

/* dice information */
static int cb_dicelimit, cb_boundary;

/* troop type numbers and tracking information */
static int zombie_type, lich_type, wraith_type, nazgul_type;
static int zombie_casters, zombie_leaders, zombie_rulers;

/* battle location */
static int cb_xloc, cb_yloc;

/* who gets the mail */
static int cb_mail[MAX_COMBAT];

/* sector combat status */
static char *sct_combval = NULL;

/* list of all combats */
static COMBAT_PTR combat_list = NULL;

/* list of all 'distancing' troops */
static CDIST_PTR distance_list = NULL;

/* strength increment */
static int strength_inc;

/* ADD_CBMAIL -- Add to the list of nations getting mail */
static void
add_cbmail PARM_1(int, cntry)
{
  int i;

  /* put in an empty slot */
  for (i = 0; i < MAX_COMBAT; i++) {
    if (cb_mail[i] == -1) {
      cb_mail[i] = cntry;
      return;
    } else if (cb_mail[i] == cntry) {
      return;
    }
  }
  fprintf(fupdate, "    WARNING: out of slots for mail, increase MAX_COMBAT\n");
}

/* INIT_COMBAT_ROLL -- intialize the combat roll settings */
void
init_combat_roll PARM_0(void)
{
  /* check the dice setting */
  if ((NUMDICE < 1) || (NUMDICE > 100))  {
    errormsg("WARNING: number of dice invalid, reseting to 10");
    NUMDICE = 10;
  }

  /* determine the maximum boundary (multiple of 100) */
  cb_boundary = 100;
  while ((cb_boundary % NUMDICE) != 0) {
    /* want a base 100 roll */
    cb_boundary += 100;
  }

  /* now figure out the size of the dice */
  cb_dicelimit = (cb_boundary / NUMDICE) + 1;
}

/* COMBAT_ROLL -- generate a combat roll, between 0 and 100 */
int
combat_roll PARM_0(void)
{
  int cnt, hold = 0;

  /* simply run it N times and give back result */
  for (cnt = 0; cnt < NUMDICE; cnt++) {
    hold += rand_val(cb_dicelimit);
  }
  return((100 * hold) / cb_boundary);
}

/* NEW_CUNIT -- Allocate storage for a combat unit */
static CUNIT_PTR
new_cunit PARM_1(Ucombattype, type)
{
  CUNIT_PTR cu_ptr;

  /* get the memory */
  if ((cu_ptr = (CUNIT_PTR) malloc(sizeof(CUNIT_STRUCT))) == NULL) {
    errormsg("MAJOR ERROR: memory allocation error in new_cunit()");
    abrt();
  }

  /* initialize it */
  cu_ptr->owner = country;
  cu_ptr->type = type;
  cu_ptr->usage = 0;
  cu_ptr->damage = 0;
  cu_ptr->rel_size = 0;
  cu_ptr->adjustment = 0;
  cu_ptr->destruct_value = 0;
  cu_ptr->ui.army_p = NULL;
  cu_ptr->next = NULL;
  return(cu_ptr);
}

/* DISTANCE_ADD -- Add to the distance list */
static void
distance_add PARM_3(CUNIT_PTR, cu_ptr, int, x, int, y)
{
  static CDIST_PTR last_dist = NULL;
  CDIST_PTR cd_ptr;

  /* check input */
  if (cu_ptr == NULL) return;

  /* allocate the new element */
  if ((cd_ptr = (CDIST_PTR) malloc(sizeof(CDIST_STRUCT))) == NULL) {
    errormsg("MAJOR ERROR: memory allocation failure in distance_add()");
    abrt();
  }

  /* assign the values */
  cd_ptr->xloc = x;
  cd_ptr->yloc = y;
  cd_ptr->unit = cu_ptr;
  cd_ptr->next = NULL;

  /* connect into the list */
  if (distance_list == NULL) {
    last_dist = cd_ptr;
    distance_list = cd_ptr;
  } else {
    last_dist->next = cd_ptr;
    last_dist = cd_ptr;
  }
}

/* COMBAT_INIT -- Initialize combat settings */
static void
combat_init PARM_0(void)
{
  static int did_roll = FALSE;

  /* tracking */
  if (is_update) {
    fprintf(fupdate, "  initializing the combat system\n");
  }

  /* find the zombie, wraith and lich units */
  zombie_type = unitbyname("Zombies");
  lich_type = unitbyname("Lich");
  wraith_type = unitbyname("Wraith");
  nazgul_type = unitbyname("Nazgul");

  /* first, check if the roll needs fixing */
  if (did_roll == FALSE) {
    did_roll = TRUE;
    init_combat_roll();
  }

  /* check if other items need fixing */
  if ((DAMAGE_LIMIT < 25) ||
      (DAMAGE_LIMIT > 95)) {
    fprintf(fupdate, "    Error: reseting DAMAGE_LIMIT to 50%%\n");
    DAMAGE_LIMIT = 50;
  }
  if ((AVG_DAMAGE < 25) ||
      (AVG_DAMAGE > 75)) {
    fprintf(fupdate, "    Error: reseting Average Damage to 50%%\n");
    AVG_DAMAGE = 50;
  }
  if ((OVERMATCH_ADJ > 50) ||
      (OVERMATCH_ADJ < 5)) {
    fprintf(fupdate, "    Error: reseting Overmatch Adjustment to 10%%\n");
    OVERMATCH_ADJ = 10;
  }
  if ((PMINDAMAGE > 10) ||
      (PMINDAMAGE < 0)) {
    fprintf(fupdate, "    Error: reseting Minimum Damage to 5%%\n");
    PMINDAMAGE = 5;
  }

  /* allocate the memory space for combat tracking */
  if (sct_combval == NULL) {

    /* get and initialize memory */
    sct_combval = new_mapchar(sct_combval);

  }
}

/* DAMAGE_UNIT -- Inflict a given damage amount to a given unit */
static int
damage_unit PARM_3(CUNIT_PTR, cu_ptr, int, dval, Cgrptype, sval)
{
  NTN_PTR nhold_ptr = ntn_ptr;
  char buf_str[BIGLTH], ntn_hdr[LINELTH];
  char str_tmp[LINELTH];
  itemtype holditm;
  long hold = 0, sunk = 0, deaths = 0;
  int chold_val = country, cnt, cnt2, cnt3;
  int rval, holdcnt, itmcnt, capcnt, damcnt;

  /* check input */
  if ((cu_ptr == NULL) ||
      (cu_ptr->rel_size <= 0)) return(deaths);

  /* get the nation information */
  if ((ntn_ptr = world.np[cu_ptr->owner]) == NULL) {
    ntn_ptr = nhold_ptr;
    return(deaths);
  }
  country = cu_ptr->owner;

  /* Start out the report properly */
  buf_str[0] = '\0';
  sprintf(ntn_hdr, "  %s %s ",
	  side_desc[sval], ntn_ptr->name);

  /* differing results for navy, caravan, and army units */
  switch (cu_ptr->type) {
  case UNIT_ARMY:
    /* army unit */
    army_ptr = cu_ptr->ui.army_p;
    if ((army_ptr == NULL) ||
	(ARMY_SIZE == 0)) break;

    /* now inflict the damages */
    if (a_isleader(ARMY_TYPE)) {

      /* start the report */
      sprintf(buf_str, "%s %d; x.p.=%ld (%d%%)",
	      ainfo_list[ARMY_TYPE].name,
	      ARMY_ID,
	      ARMY_SIZE, cu_ptr->adjustment);

      /* death on a % chance */
      if (rand_val(100) < dval) {
	/* time for taps */
	hold = 100;
	ARMY_SIZE = 0;
	deaths++;
	fprintf(fupdate, "      A %s in nation %s becomes a statistic\n",
		ainfo_list[ARMY_TYPE].name,
		ntn_ptr->name);
	strcat(buf_str, " [killed]");
	if (!a_isundead(ARMY_TYPE)) {
	  if (a_isruler(ARMY_TYPE)) {
	    zombie_rulers++;
	  } else if (a_castspells(ARMY_TYPE)) {
	    zombie_casters++;
	  } else {
	    zombie_leaders++;
	  }
	}
      } else {
	/* slight size increase */
	ARMY_SIZE += rand_val(3);
	hold = 0;
      }

    } else if (a_ismonster(ARMY_TYPE)) {

      /* give the information */
      sprintf(buf_str, "%s %d; str=%ldx%d (%d%%)",
	      ainfo_list[ARMY_TYPE].name,
	      ARMY_ID,
	      ARMY_SIZE,
	      ainfo_list[ARMY_TYPE].minsth,
	      cu_ptr->adjustment);

      /* check for the number of monsters damaged */
      if ((dval == 100) || (dval == 0)) {
	/* results already known */
	deaths = (ARMY_SIZE * dval) / 100;
	if ((dval == 100) &&
	    (MIN_GAIN_STR < ainfo_list[ARMY_TYPE].minsth)) {
	  for(cnt = 0; cnt < ARMY_SIZE; cnt++) {
	    if (rand_val(PROB_GAIN_STR) < ainfo_list[ARMY_TYPE].minsth) {
	      strength_inc++;
	    }
	  }
	}
      } else {
	/* check each individual monster */
	for (cnt = 0; cnt < ARMY_SIZE; cnt++) {
	  if (rand_val(100) < dval) {
	    deaths++;
	    if ((MIN_GAIN_STR < ainfo_list[ARMY_TYPE].minsth) &&
		(rand_val(PROB_GAIN_STR) < ainfo_list[ARMY_TYPE].minsth)) {
	      strength_inc++;
	    }
	  }
	}
      }

      /* list deaths */
      if (deaths > 0) {

	/* provide output to "screen" */
	if (deaths == 1) {
	  fprintf(fupdate, "      A %s serving nation %s is destroyed\n",
		  ainfo_list[ARMY_TYPE].name,
		  ntn_ptr->name);
	} else if (deaths > 1) {
	  fprintf(fupdate, "      %ld %ss serving nation %s are destroyed\n",
		  deaths, ainfo_list[ARMY_TYPE].name,
		  ntn_ptr->name);
	}

	/* damage done */
	hold = (100 * deaths) / ARMY_SIZE;
	ARMY_SIZE -= deaths;

	/* combat report output */
	if (a_isundead(ARMY_TYPE)) {
	  /* switch undead figures */
	  sprintf(str_tmp, " [%ld destroyed]", deaths);
	  deaths = -deaths;
	} else {
	  sprintf(str_tmp, " [%ld killed]", deaths);
	}
	strcat(buf_str, str_tmp);

      } else {
	deaths = 0;
      }

    } else {

      /* report on the unit */
      sprintf(buf_str, "Army %s %d; men=%ld (%d%%)",
	      ainfo_list[ARMY_TYPE].name,
	      ARMY_ID,
	      ARMY_SIZE,
	      cu_ptr->adjustment);

      /* check for adjustments of mercenary appreciation */
      if (a_ismerc(ARMY_TYPE)) {
	int adjval = 0;

	if (dval == 0) {
	  if (ARMY_SIZE > 250) {
	    adjval = 2;
	  }
	} else if (dval <= 20) {
	  if (ARMY_SIZE > 125) {
	    adjval = 1;
	  }
	} else if (dval == 100) {
	  adjval = -3;
	} else if (dval > 80) {
	  adjval = -2;
	} else if (dval > 40) {
	  adjval = -1;
	}

	/* adjustment due to mercs */
	if (adjval != 0) {
	  adjust_attribute(ntn_ptr->attribute, BUTE_MERCREP, adjval);
	}
      }

      /* perform a simple stomping */
      deaths = (ARMY_SIZE * dval) / 100 + (rand_val(100) < dval % 100);
      ARMY_SIZE -= deaths;

      if (ARMY_EFF > (dval / 10)) {
	ARMY_EFF -= dval / 10;
      }
      if (rand_val(100) < (100 - dval) / 2) {
	if (ARMY_MAXEFF < 105) {
	  ARMY_MAXEFF++;
	}
      }

      /* report the damage */
      if (deaths > 0) {
	if (a_isundead(ARMY_TYPE)) {
	  sprintf(str_tmp, " [%ld destroyed]", deaths);
	  deaths = -deaths;
	} else {
	  sprintf(str_tmp, " [%ld killed]", deaths);
	}
	strcat(buf_str, str_tmp);
      } else {
	deaths = 0;
      }
      hold = dval;

    }
    break;
  case UNIT_NAVY:
    /* naval unit */
    navy_ptr = cu_ptr->ui.navy_p;
    if (navy_ptr == NULL) break;
    for (cnt = 0; cnt < NSHP_NUMBER; cnt++) {
      if (NAVY_SHIPS[cnt] != 0) break;
    }
    if (cnt == NSHP_NUMBER) break;

    /* report separately for each class of unit */
    for (cnt = 0; cnt < NSHP_NUMBER; cnt++) {

      /* are there any of the current class? */
      sunk = 0;
      if (NAVY_SHIPS[cnt] != 0) {

	/* list the specs */
	sprintf(buf_str, "Navy %d", NAVY_ID);

	/* how many ships of which type */
	holdcnt = navy_holds(navy_ptr, cnt);
	sprintf(str_tmp, "; %d %s; men=%d (%d%%)" ,
		(int) N_CNTSHIPS(NAVY_SHIPS[cnt], N_LIGHT) +
		(int) N_CNTSHIPS(NAVY_SHIPS[cnt], N_MEDIUM) +
		(int) N_CNTSHIPS(NAVY_SHIPS[cnt], N_HEAVY),
		ninfo_list[cnt].name,
		((int) NAVY_CREW) * holdcnt,
		cu_ptr->adjustment);
	strcat(buf_str, str_tmp);

	/* now report what it is made of */
	if (dval > 0) {

	  /* determine how much damage there is */
	  for (cnt2 = 0; cnt2 <= N_HEAVY; cnt2++) {

	    /* determine damage by each individual ship */
	    capcnt = damcnt = 0;
	    itmcnt = (int) N_CNTSHIPS(NAVY_SHIPS[cnt], cnt2);
	    for (cnt3 = 0; cnt3 < itmcnt; cnt3++) {
	      if ((rval = rand_val(100)) <
		  (dval * ninfo_list[cnt].dam_chance) / 100) {
		damcnt++;
	      } else if (rval < dval) {
		capcnt++;
	      }

	      /* capturing UNIMPLEMENTED for now */
	      damcnt += capcnt;
	    }
	    if (damcnt > itmcnt) {
	      /* just in case */
	      damcnt = itmcnt;
	    }

	    /* damage things on the ships */
	    if (damcnt > 0) {
	      cnt3 = (100 * damcnt * (cnt2 + 1)) / holdcnt;
	      if (cnt3 > 100) {
		cnt3 = 100;
	      } else if (cnt3 < 0) {
		cnt3 = 0;
	      }
	      switch (cnt) {
	      case NSHP_WARSHIPS:
		/* damage troops -- Unload if on land? */
		if ((army_ptr = armybynum(NAVY_ARMY)) != NULL) {
		  ARMY_SIZE *= (100 - cnt3);
		  ARMY_SIZE /= 100;
		  if (sct[ARMY_XLOC][ARMY_YLOC].altitude != ELE_WATER) {
		    if (cnt3 >= 75) {
		      /* this allows for the possibility that some */
		      /* of them sneak out alive */
		      set_status(ARMY_STAT, ST_DEFEND);
		      NAVY_ARMY = EMPTY_HOLD;
		    }
		  }
		}
		break;
	      case NSHP_MERCHANTS:
		/* destroy materials */
		for (itmcnt = 0; itmcnt < MTRLS_NUMBER; itmcnt++) {
		  holditm = (((damcnt * (cnt2 + 1)) *
			   NAVY_MTRLS[itmcnt]) / holdcnt);
		  NAVY_MTRLS[itmcnt] -= holditm;
		}
		break;
	      case NSHP_GALLEYS:
		/* kill people (run if on land?) */
		deaths += (damcnt * (cnt2 + 1)) * (int) NAVY_PEOP;
		break;
	      case NSHP_BARGES:
		/* damage caravans -- Unload if on land? */
		if ((cvn_ptr = cvnbynum(NAVY_CVN)) != NULL) {
		  CVN_SIZE *= (100 - cnt3);
		  CVN_SIZE /= 100;
		}
		break;
	      default:
		/* don't know */
		break;
	      }

	      /* record the crew damage */
	      deaths += (damcnt * (cnt2 + 1)) * (int) NAVY_CREW;

	      /* remove the ships */
	      sunk += damcnt;
	      NAVY_SHIPS[cnt] = navy_subships(NAVY_SHIPS[cnt],
					      cnt2, damcnt);

	    }

	    /* decrease total number of holds */
	    holdcnt -= (damcnt * (cnt2 + 1));

	  }

	  /* record the kills */
	  if (sunk > 0) {
	    sprintf(str_tmp, " [%ld sunk %d%% damage]",
		    sunk, dval);
	  } else if (dval > 0) {
	    sprintf(str_tmp, " [%d%% damage]",
		    dval);
	  }
	  strcat(buf_str, str_tmp);

	}

	/* now store it */
	strcpy(string, ntn_hdr);
	strcat(string, buf_str);
	strcat(string, "\n");
	country = chold_val;
	msg_conquer(string);
	country = cu_ptr->owner;
	buf_str[0] = '\0';
      }
      
      /* decrease crew and efficiency */
      if ((hold = dval) > 0) {
	hold = NAVY_EFF[cnt] * (100 - (hold + 1) / 2);
	NAVY_EFF[cnt] = (uns_char) (hold / 100);
      }

    }
    break;
  case UNIT_CVN:
    /* caravan unit */
    cvn_ptr = cu_ptr->ui.cvn_p;
    if ((cvn_ptr == NULL) ||
	(CVN_SIZE == 0)) break;

    /* describe the wagons */
    sprintf(buf_str, "Caravan %d; %d wagons, %ld men (%d%%)",
	    CVN_ID, (int) CVN_SIZE * WAGONS_IN_CVN,
	    cu_ptr->rel_size,
	    cu_ptr->adjustment);

    /* check for destroyed wagons */
    for (cnt = 0; cnt < CVN_SIZE; cnt++) {
      if (rand_val(100) < dval / 4) {

	/* now just eliminate a portion of the items */
	for (itmcnt = 0; itmcnt < MTRLS_NUMBER; itmcnt++) {
	  holditm = CVN_MTRLS[itmcnt] / CVN_SIZE;
	  CVN_MTRLS[itmcnt] -= holditm;
	}
	deaths += CVN_PEOP;
	deaths += CVN_CREW;

	/* now destroy a bunch of wagons */
	CVN_SIZE--;

      }
    }

    /* check the size now */
    if (CVN_SIZE == 0) {

      /* all done */
      CVN_PEOP = 0;
      CVN_CREW = 0;
      CVN_EFF = 0;
      hold = 100;

    } else {

      /* a portion of the passengers bite it */
      hold = ((int) CVN_PEOP) * (100 - dval / 2);
      hold /= 100;
      deaths += hold * CVN_SIZE;
      CVN_PEOP -= hold;

      /* perform simple crew adjustment */
      hold = ((int) CVN_CREW) * (100 - dval);
      hold /= 100;
      hold = CVN_CREW - hold;
      deaths += hold * CVN_SIZE;
      CVN_CREW -= hold;
      hold = dval;
      if (CVN_EFF > 5)
	CVN_EFF -= 5;
      else
	CVN_EFF = 5;

    }

    /* report the damage percentage */
    if (hold > 0) {
      sprintf(str_tmp, " [%ld%% damage]", hold);
      strcat(buf_str, str_tmp);
    }
    break;
  default:
    /* nada */
#ifdef DEBUG
    errormsg("DEBUG: unknown unit type in damage_unit()");
#endif /* DEBUG */
    break;
  }

  /* set the damage */
  cu_ptr->damage = hold;
  ntn_ptr = nhold_ptr;
  country = chold_val;

  /* now send the report */
  if (buf_str[0] != '\0') {
    strcpy(string, ntn_hdr);
    strcat(string, buf_str);
    strcat(string, "\n");
    msg_conquer(string);
  }
  return(deaths);
}

/* CBVAL_ARMY -- Combat bonus of a given unit */
static int
cbval_army PARM_4(int, owner_id, ARMY_PTR, a1_ptr, Cgrptype, sideval,
		  Cgrptype, othside)
{
  NTN_PTR nhold_ptr = ntn_ptr;
  ARMY_PTR a2_ptr = NULL;
  int hold = 0, type, astat, spd;

  /* check input */
  if ((world.np[owner_id] == NULL) ||
      (a1_ptr == NULL)) return(hold);

  /* set the values */
  ntn_ptr = world.np[owner_id];
  type = a1_ptr->unittype;
  spd = unit_speed(a1_ptr->status);

  /* check the status */
  if ((astat = unit_status(a1_ptr->status)) == ST_GROUPED) {
    /* moral bonus? */
    if (!a_isleader(type)) {
      hold += 10;
    }
    astat = group_stat(a1_ptr->leader);
  } else if (!a_isleader(type) &&
	     ((a2_ptr = armybynum(a1_ptr->leader)) != NULL)) {
    /* minor moral bonus */
    if ((a2_ptr->xloc == a1_ptr->xloc) &&
	(a2_ptr->yloc == a1_ptr->yloc)) {
      hold += 5;
    }
  }

  /* get proper base bonuses */
  if (a_ismerc(type)) {

    /* mercenary bonuses */
    if (sideval <= CGRP_ATTACKER) {
      hold += MERCATT;
    } else {
      hold += MERCDEF;
    }

  } else {

    /* national combat bonus */
    if (sideval <= CGRP_ATTACKER) {
      hold += ntn_ptr->aplus;
    } else {
      hold += ntn_ptr->dplus;
    }

  }

  /* unit power and other factors */
  if (sideval <= CGRP_ATTACKER) {

    /* attacking combat bonus */
    if (!a_isnormal(type) ||
	!a_needmin(type) ||
	(ainfo_list[type].att_bonus < 0) ||
	(ainfo_list[type].minsth <= a1_ptr->strength)) {
      if (!risky_stat(astat) ||
	  (othside >= CGRP_ATTACKER)) {
	hold += ainfo_list[type].att_bonus;
      }
    }

    /* check if the unit gains terrain bonus */
    if (grounded_stat(astat)) {
      hold += defense_val(cb_xloc, cb_yloc);
    }

  } else {

    /* defensive bonus */
    if (!a_isnormal(type) ||
	!a_needmin(type) ||
	(ainfo_list[type].minsth <= a1_ptr->strength)) {
      hold += ainfo_list[type].def_bonus;
    }

    /* terrain bonus */
    hold += defense_val(cb_xloc, cb_yloc);

    /* gain fortification value */
    if (fort_stat(astat)) {
      hold += fort_val(cb_xloc, cb_yloc);
    }

    /* check for ship protection */
    if (onship_stat(astat)) {
      /* protection of ship */
      hold += 30;
    }

  }

  /* now give the speed bonuses */
  if (!nomove_stat(astat)) {
    switch (spd) {
    case SPD_MARCH:
      /* oops... caught unprepared */
      hold -= 40;
      break;
    case SPD_SLOW:
      /* well rested */
      hold += 40;
      break;
    }
  }

  /* additional bonus for flying units */
  if (unit_flight(a1_ptr->status)) {
    hold += 20;
  }

  /* check for magical enhancement */
  if (unit_spelled(a1_ptr->status)) {
    /* 30% for magically enhanced troops */
    hold += 30;
  }

  /* degredation based on efficiency */
  hold -= (hold * (100 - (int) a1_ptr->efficiency)) / 200;
  ntn_ptr = nhold_ptr;
  return(hold);
}

/* CBVAL_NAVY -- Combat bonus of a given unit */
static int
cbval_navy PARM_3(int, owner_id, NAVY_PTR, y1_ptr, Cgrptype, sideval)
{
  NTN_PTR nhold_ptr = ntn_ptr;
  /* default combat bonus for fleets */
  int hold = FLEET_CBVAL;
  int spd;

  /* check the input */
  if ((world.np[owner_id] == NULL) ||
      (y1_ptr == NULL)) return(hold);
  spd = unit_speed(y1_ptr->status);
  ntn_ptr = world.np[owner_id];

  /* on water or on land? */
  if (sct[y1_ptr->xloc][y1_ptr->yloc].altitude == ELE_WATER) {

    /* speed is an asset on water */
    hold = max(0, hold);
    switch (spd) {
    case SPD_MARCH:
      hold += 40;
      break;
    case SPD_SLOW:
      hold -= 40;
      break;
    default:
      /* no adjustment */
      break;
    }

    /* bonuses should be related to ship type: UNIMPLEMENTED */

  } else {

    /* terrain bonus */
    hold += defense_val(cb_xloc, cb_yloc);
    if (((sideval == CGRP_PROTECTED) ||
	 (sideval == CGRP_FORTIFIED)) &&
	((sct[cb_xloc][cb_yloc].owner == owner_id) ||
	 ((sct[cb_xloc][cb_yloc].owner != UNOWNED) &&
	  ((world.np[sct[cb_xloc][cb_yloc].owner])->dstatus[owner_id]
	   == DIP_ALLIED)))) {
      /* if within a fortification */
      hold += fort_val(cb_xloc, cb_yloc);
    }

  }

  /* increased fighting ability for certain magical powers */
  if (CIV_MAGIC(MC_MARINE)) {
    hold += 20;
  }
  if (CIV_MAGIC(MC_SAILOR)) {
    hold += 20;
  }

  /* check if used as a foritification */
  if (sideval == CGRP_PROTECTED) {
    hold += 30;
  }

  /* check for magical enhancement */
  if (unit_spelled(y1_ptr->status)) {
    /* 30% for magically enhanced troops */
    hold += 30;
  }

  /* degredation based on efficiency */
  hold -= (hold * (100 - (int) y1_ptr->efficiency[0])) / 200;
  ntn_ptr = nhold_ptr;
  return(hold);
}

/* CBVAL_CVN -- Combat bonus of a given unit */
static int
cbval_cvn PARM_3(int, owner_id, CVN_PTR, v1_ptr, Cgrptype, sideval)
{
  NTN_PTR nhold_ptr = ntn_ptr;
  /* default combat bonus for wagons */
  int hold = WAGON_CBVAL;

  /* check input */
  if ((world.np[owner_id] == NULL) ||
      (v1_ptr == NULL)) return(hold);
  ntn_ptr = world.np[owner_id];

  /* speed is detrimental on land */
  switch (unit_speed(v1_ptr->status)) {
  case SPD_MARCH:
    hold -= 20;
    break;
  case SPD_SLOW:
    hold += 20;
    break;
  default:
    /* no adjustment */
    break;
  }

  /* check for fortified wagons */
  if (((sideval == CGRP_PROTECTED) ||
       (sideval == CGRP_FORTIFIED)) &&
      ((sct[cb_xloc][cb_yloc].owner == owner_id) ||
       ((sct[cb_xloc][cb_yloc].owner != UNOWNED) &&
	((world.np[sct[cb_xloc][cb_yloc].owner])->dstatus[owner_id]
	 == DIP_ALLIED)))) {
    hold += fort_val(cb_xloc, cb_yloc);
  }

  /* always on the defensive */
  hold += defense_val(cb_xloc, cb_yloc);

  /* check for magical enhancement */
  if (unit_spelled(v1_ptr->status)) {
    /* 30% for magically enhanced troops */
    hold += 30;
  }

  /* do efficiency at last */
  hold -= (hold * (100 - (int) v1_ptr->efficiency)) / 200;
  ntn_ptr = nhold_ptr;
  return(hold);
}

/* CB_DESTVAL -- Destructive potential of the unit */
static int
cb_destval PARM_3(CUNIT_PTR, cu_ptr, Cgrptype, sd_val, Cgrptype, oth_val)
{
  ARMY_PTR a1_ptr;
  int hold = 0;

  /* default outs */
  if ((cu_ptr->type != UNIT_ARMY) ||
      (sd_val != CGRP_ATTACKER) ||
      (oth_val != CGRP_FORTIFIED)) {
    return(hold);
  }

  /* examine the unit */
  if (((a1_ptr = cu_ptr->ui.army_p) == NULL) ||
      !a_damfort(a1_ptr->unittype)) {
    return(hold);
  }
  if (a_isleader(a1_ptr->unittype)) {
    hold = 1;
  } else {
    hold = (a1_ptr->strength *
	    (int) ainfo_list[a1_ptr->unittype].capt_val) / 10;
  }
  return(hold);
}

/* CB_VALUE -- Return the combat value of a given unit */
static int
cb_value PARM_4(CSIDE_PTR, cs_ptr, CUNIT_PTR, cu_ptr,
		Cgrptype, sd_val, Cgrptype, oth_val)
{
  NTN_PTR n1_ptr;
  int hold = 0;

  /* check input */
  if (cu_ptr == NULL) return(hold);

  /* get the nation type */
  if (cu_ptr->owner >= ABSMAXNTN) {
    return(hold);
  }
  if ((n1_ptr = world.np[cu_ptr->owner]) == NULL) {
    return(hold);
  }

  /* now work on a base value of 100 */
  hold = 100;

  /* switch based on unit type */
  switch (cu_ptr->type) {
  case UNIT_ARMY:
    /* army unit */
    hold += cbval_army(cu_ptr->owner, cu_ptr->ui.army_p, sd_val, oth_val);
    break;
  case UNIT_NAVY:
    /* naval unit */
    hold += cbval_navy(cu_ptr->owner, cu_ptr->ui.navy_p, sd_val);
    break;
  case UNIT_CVN:
    /* caravan unit */
    hold += cbval_cvn(cu_ptr->owner, cu_ptr->ui.cvn_p, sd_val);
    break;
  default:
    /* just wasn't worth looking at */
    return(hold);
    /*NOTREACHED*/
    break;
  }

  /* check for cover bonuses */
  if (sd_val == CGRP_ATTACKER) {
    if (oth_val >= CGRP_FORTIFIED) {
      hold += cs_ptr->attack_cover;
    }
  } else if (sd_val == CGRP_FORTIFIED) {
    hold += cs_ptr->fort_cover;
  }

  /* magical sector bonuses */
  hold += mgk_sctval(n1_ptr, cb_xloc, cb_yloc);

  /* give back results */
  return(hold);
}

/* GET_CSIDE -- retrieve the side structure from within a combat */
static CSIDE_PTR
get_cside PARM_4(COMBAT_PTR, cb_ptr, int, ntnnum, int, x, int, y)
{
  CSIDE_PTR cs_ptr;
  int cpos, i;

  /* check input */
  if (cb_ptr == NULL) {
    errormsg("ERROR: invalid input to get_cside()");
    return((CSIDE_PTR) NULL);
  }

  /* check for the owner */
  if (sct[x][y].owner == country) {
    /* must be the first element */
    cs_ptr = cb_ptr->cside[cpos = 0];
  } else {
    /* look for a match */
    for (cpos = 1; cpos < MAX_COMBAT; cpos++) {
      /* empty slot?  or a match? */
      if (((cs_ptr = cb_ptr->cside[cpos]) == NULL) ||
	  (cs_ptr->owner == ntnnum)) break;
    }
  }

  /* check limit */
  if (cpos == MAX_COMBAT) {
    /* reached limit */
    errormsg("Warning:  Too many combatants... raise MAX_COMBAT in combatX.h");
    return((CSIDE_PTR) NULL);
  }

  /* get it? */
  if (cs_ptr == NULL) {

    /* allocate the memory space */
    if ((cs_ptr = (CSIDE_PTR) malloc(sizeof(CSIDE_STRUCT))) == NULL) {
      errormsg("MAJOR ERROR: cannot allocate memory for combat");
      abrt();
    }

    /* initialize */
    cs_ptr->owner = ntnnum;
    cs_ptr->attack_cover = 0;
    cs_ptr->fort_cover = 0;
    cs_ptr->has_spys = FALSE;
    for (i = 0; i < CGRP_NUMBER; i++) {
      cs_ptr->sum_size[i] = 0;
      cs_ptr->avg_bonus[i] = (double) 0;
      cs_ptr->units[i] = (CUNIT_PTR) NULL;
    }
    for (i = 0; i < MAX_COMBAT; i++) {
      cs_ptr->rstat[CGRP_SWEEPER][i] = 0;
      cs_ptr->rstat[CGRP_ATTACKER][i] = 0;
    }

    /* put it in place */
    cb_ptr->cside[cpos] = cs_ptr;
    
  }

  /* got it... good! */
  return(cs_ptr);
}

/* GET_COMBAT -- retrieve/build a combat location */
static COMBAT_PTR
get_combat PARM_2(int, x, int, y)
{
  COMBAT_PTR cb_ptr;
  int i;

  /* perform traversal search */
  for (cb_ptr = combat_list;
       cb_ptr != NULL;
       cb_ptr = cb_ptr->next) {
    /* got it? */
    if ((cb_ptr->xloc == x) &&
	(cb_ptr->yloc == y)) break;
  }

  /* get it? */
  if (cb_ptr == NULL) {

    /* get the memory */
    if ((cb_ptr = (COMBAT_PTR) malloc(sizeof(COMBAT_STRUCT))) == NULL) {
      errormsg("MAJOR ERROR: could not allocate memory for combat code");
      abrt();
    }

    /* now initialize it */
    cb_ptr->xloc = x;
    cb_ptr->yloc = y;
    for (i = 0; i < MAX_COMBAT; i++) {
      cb_ptr->cside[i] = NULL;
    }

    /* now place it in the list */
    cb_ptr->next = combat_list;
    combat_list = cb_ptr;
  }

  /* got it!... good */
  return(cb_ptr);
}

/* CALC_RELSIZE -- Determine the relative size of the unit */
static long
calc_relsize PARM_1(CUNIT_PTR, cu_ptr)
{
  ARMY_PTR a1_ptr;
  NAVY_PTR n1_ptr;
  CVN_PTR v1_ptr;
  long hold = 0;
  int i;

  /* check input */
  if (cu_ptr == NULL) return(hold);

  /* based on type */
  switch (cu_ptr->type) {
  case UNIT_ARMY:
    /* an army unit */
    if ((a1_ptr = cu_ptr->ui.army_p) != NULL) {
      if (a_ismonster(a1_ptr->unittype)) {
	hold = a1_ptr->strength *
	  ainfo_list[a1_ptr->unittype].minsth;
      } else {
	hold = a1_ptr->strength;
      }
    }
    break;
  case UNIT_NAVY:
    /* navy unit */
    if ((n1_ptr = cu_ptr->ui.navy_p) != NULL) {
      for (i = 0; i < NSHP_NUMBER; i++) {
	hold += ((long) n1_ptr->crew) * navy_holds(n1_ptr, i);
      }
    }
    break;
  case UNIT_CVN:
    /* caravan unit */
    if ((v1_ptr = cu_ptr->ui.cvn_p) != NULL) {
      hold = v1_ptr->crew * v1_ptr->size;
    }
    break;
  default:
    /* nothing doin' */
    break;
  }

  /* all done */
  return(hold);
}

/* BATTLE_GROUPING -- Gather all of the units into their respective sectors */
static void
battle_grouping PARM_0(void)
{
  COMBAT_PTR cb_ptr;
  CSIDE_PTR cs_ptr;
  CUNIT_PTR cu_ptr;
  int side_val, stat_val;

  /* tracking */
  fprintf(fupdate, "  gathering all troops into battle groups\n");

  /* round 'em up */
  for (country = 1; country < MAXNTN; country++) {

    /* get them together */
    if ((ntn_ptr = world.np[country]) == NULL) continue;
    if (n_notactive(ntn_ptr->active)) continue;

    /* go through all of the military units -- army first */
    for (army_ptr = ntn_ptr->army_list;
	 army_ptr != NULL;
	 army_ptr = army_ptr->next) {

      /* abandon bad locations */
      if (!XY_INMAP(ARMY_XLOC, ARMY_YLOC)) continue;

      /* check for exclusion of scouts, etc. */
      if (a_isagent(ARMY_TYPE)) continue;
      if (a_isscout(ARMY_TYPE) &&
	  (rand_val(100) < PSCOUT)) continue;
      if (a_isscout(ARMY_TYPE) &&
	  a_cansee(ARMY_TYPE) &&
	  (rand_val(100) < PSCOUT)) continue;

      /* get the appropriate sector and side structure */
      if ((cb_ptr = get_combat(ARMY_XLOC, ARMY_YLOC)) == NULL) continue;
      if ((cs_ptr = get_cside(cb_ptr, country, ARMY_XLOC, ARMY_YLOC))
	  == NULL) continue;

      /* build the unit entry */
      if ((cu_ptr = new_cunit(UNIT_ARMY)) == NULL) continue;
      cu_ptr->ui.army_p = army_ptr;
      cu_ptr->rel_size = calc_relsize(cu_ptr);

      /* determine side and such */
      stat_val = real_stat(army_ptr);
      if (sweep_stat(stat_val)) {
	side_val = CGRP_SWEEPER;
      } else if (attack_stat(stat_val)) {
	side_val = CGRP_ATTACKER;
	if (a_coverbonus(ARMY_TYPE)) {
	  cs_ptr->attack_cover += (15 * ARMY_SIZE) /
	    ainfo_list[ARMY_TYPE].minsth;
	}
      } else if (dieslast_stat(stat_val)) {
	side_val = CGRP_PROTECTED;
      } else if (fort_stat(stat_val)) {
	side_val = CGRP_FORTIFIED;
	if (a_coverbonus(ARMY_TYPE)) {
	  cs_ptr->fort_cover += (20 * ARMY_SIZE) /
	    ainfo_list[ARMY_TYPE].minsth;
	}
      } else {
	side_val = CGRP_DEFENDER;
      }

      /* track any undead */
      if (a_isundead(ARMY_TYPE)) {
	if (a_isleader(ARMY_TYPE)) {
	  cs_ptr->zombie_mkr[side_val]++;
	} else {
	  cs_ptr->zombie_mkr[side_val] += ARMY_SIZE;
	}
      }

      /* now distribute the troop */
      cs_ptr->sum_size[side_val] += cu_ptr->rel_size;
      cu_ptr->next = cs_ptr->units[side_val];
      cs_ptr->units[side_val] = cu_ptr;
      if (distant_stat(stat_val)) {
	distance_add(cu_ptr, ARMY_XLOC, ARMY_YLOC);
      }

    }

    /* now for the naval units */
    for (navy_ptr = ntn_ptr->navy_list;
	 navy_ptr != NULL;
	 navy_ptr = navy_ptr->next) {

      /* abandon bad locations */
      if (!XY_INMAP(NAVY_XLOC, NAVY_YLOC)) continue;

      /* get the appropriate sector and side structure */
      if ((cb_ptr = get_combat(NAVY_XLOC, NAVY_YLOC)) == NULL) continue;
      if ((cs_ptr = get_cside(cb_ptr, country, NAVY_XLOC, NAVY_YLOC))
	  == NULL) continue;

      /* build the unit entry */
      if ((cu_ptr = new_cunit(UNIT_NAVY)) == NULL) continue;
      cu_ptr->ui.navy_p = navy_ptr;
      cu_ptr->rel_size = calc_relsize(cu_ptr);

      /* determine side and such */
      if (attack_stat(NAVY_STAT) &&
	  (sct[NAVY_XLOC][NAVY_YLOC].altitude == ELE_WATER)) {
	side_val = CGRP_ATTACKER;
      } else {
	side_val = CGRP_PROTECTED;
      }

      /* now distribute the troop */
      cs_ptr->sum_size[side_val] += cu_ptr->rel_size;
      cu_ptr->next = cs_ptr->units[side_val];
      cs_ptr->units[side_val] = cu_ptr;
      if (distant_stat(NAVY_STAT) &&
	  (sct[NAVY_XLOC][NAVY_YLOC].altitude == ELE_WATER)) {
	distance_add(cu_ptr, NAVY_XLOC, NAVY_YLOC);
      }

    }

    /* and lastly, the caravans */
    for (cvn_ptr = ntn_ptr->cvn_list;
	 cvn_ptr != NULL;
	 cvn_ptr = cvn_ptr->next) {

      /* abandon bad locations */
      if (!XY_INMAP(CVN_XLOC, CVN_YLOC)) continue;

      /* get the appropriate sector and side structure */
      if ((cb_ptr = get_combat(CVN_XLOC, CVN_YLOC)) == NULL) continue;
      if ((cs_ptr = get_cside(cb_ptr, country, CVN_XLOC, CVN_YLOC))
	  == NULL) continue;

      /* build the unit entry */
      if ((cu_ptr = new_cunit(UNIT_CVN)) == NULL) continue;
      cu_ptr->ui.cvn_p = cvn_ptr;
      cu_ptr->rel_size = calc_relsize(cu_ptr);

      /* always "protected" */
      side_val = CGRP_PROTECTED;

      /* now distribute the troop */
      cs_ptr->sum_size[side_val] += cu_ptr->rel_size;
      cu_ptr->next = cs_ptr->units[side_val];
      cs_ptr->units[side_val] = cu_ptr;

    }
  }
}

/* JOIN_SIDES -- join two sides into one */
static void
join_sides PARM_2(CSIDE_PTR, cs1_ptr, CSIDE_PTR, cs2_ptr)
{
  CUNIT_PTR cu_ptr;
  int i;

  /* take side B and place it in side A */
  if ((cs1_ptr == NULL) ||
      (cs2_ptr == NULL)) return;

  /* go through each division */
  for (i = 0; i < CGRP_NUMBER; i++) {

    /* find the beginning of the troops */
    for (cu_ptr = cs1_ptr->units[i];
	 cu_ptr != NULL;
	 cu_ptr = cu_ptr->next) {
      if (cu_ptr->next == NULL) break;
    }

    /* now shift in the other troops */
    if (cu_ptr == NULL) {
      cs1_ptr->units[i] = cs2_ptr->units[i];
    } else {
      cu_ptr->next = cs2_ptr->units[i];
    }

    /* add together the sizes */
    cs1_ptr->sum_size[i] += cs2_ptr->sum_size[i];

  }
}

/* MERGE_ALLIES -- join allies together into the same groups */
static int
merge_allies PARM_1(COMBAT_PTR, cb_ptr)
{
  CSIDE_PTR cs_ptr, cs_tmp, cbase_ptr;
  int n_cntry, curmbr, cnt, i, j, may_merge, num_total;
  Diplotype dstat[MAX_COMBAT];
  NTN_PTR n1_ptr;
  long side_size;

  /* check the input */
  if (cb_ptr == NULL) return(0);

  /* first find the last used element */
  for (num_total = MAX_COMBAT - 1; num_total > 0; num_total--) {
    if (cb_ptr->cside[num_total] != NULL) {
      num_total++;
      break;
    }
  }

  /* search over all the combatents */
  for (curmbr = 0; curmbr < num_total; curmbr++) {

    /* check the selection */
    if ((cbase_ptr = cb_ptr->cside[curmbr]) == NULL) {

      /* shift and reduce */
      if (curmbr != 0) {
	for (i = curmbr; i < num_total - 1; i++) {
	  cb_ptr->cside[i] = cb_ptr->cside[i + 1];
	}
	cb_ptr->cside[i] = NULL;
	curmbr--;
      }
      continue;

    }

    /* get the base nation's identity */
    country = cbase_ptr->owner;
    if ((ntn_ptr = world.np[country]) == NULL) continue;

    /* first record all statuses by this nation towards combatents */
    for (cnt = 0; cnt < num_total; cnt++) {
      if ((cs_ptr = cb_ptr->cside[cnt]) == NULL) {
	dstat[cnt] = DIP_UNMET;
      } else {
	n_cntry = cs_ptr->owner;
	if ((dstat[cnt] = ntn_ptr->dstatus[n_cntry]) == DIP_UNMET) {
	  dstat[cnt] = DIP_NEUTRAL;
	}
      }
    }

    /* now traverse the list; searching for allies */
    for (cnt = curmbr; cnt < num_total; cnt++) {

      /* check the information on the possible allies */
      if (dstat[cnt] != DIP_ALLIED) continue;
      if ((cs_ptr = cb_ptr->cside[cnt]) == NULL) continue;
      n_cntry = cs_ptr->owner;
      if ((n1_ptr = world.np[n_cntry]) == NULL) continue;

      /* are they allied? */
      if (n1_ptr->dstatus[country] == DIP_ALLIED) {

	/* verify that the merge is possible */
	may_merge = TRUE;
	for (i = 0; i < num_total; i++) {
	  if (((cs_tmp = cb_ptr->cside[cnt]) != NULL) &&
	      ((j = n1_ptr->dstatus[cs_tmp->owner]) != DIP_UNMET)) {
	    /* cannot merge if hostile to allies' friend */
	    if (((j < DIP_NEUTRAL) && (dstat[i] > DIP_HOSTILE)) ||
		((j > DIP_HOSTILE) && (dstat[i] < DIP_NEUTRAL))) {
	      may_merge = FALSE;
	      break;
	    }
	  }
	}

	/* merge the two sides */
	if (may_merge == TRUE) {
	  side_size = 0;
	  if (curmbr != 0) {
	    for (i = 0; i < CGRP_NUMBER; i++) {
	      side_size += cbase_ptr->sum_size[i];
	      side_size -= cbase_ptr->sum_size[i];
	    }
	  }
	  join_sides(cbase_ptr, cs_ptr);
	  if (side_size < 0) {
	    /* owner switches, but diplomacy references first nation */
	    cbase_ptr->owner = n_cntry;
	  }

	  /* free up the memory of the second side */
	  free(cs_ptr);

	  /* shift the others */
	  for (i = cnt; i < num_total - 1; i++) {
	    cb_ptr->cside[i] = cb_ptr->cside[i + 1];
	  }
	  cb_ptr->cside[i] = NULL;
	  num_total--;
	  cnt--;

	}

      }

    }

  }

  /* run with it */
  return(num_total);
}

/* SET_HOSTILITIES -- Set all of the hostility ratings */
static int
set_hostilities PARM_2(COMBAT_PTR, cb_ptr, int, num_there)
{
  NTN_PTR n1_ptr;
  CSIDE_PTR cs1_ptr, cs2_ptr;
  ntntype c1_num, c2_num;
  int meets = 0, own_site, i, j;

  /* check input */
  if (cb_ptr == NULL) return(0);

  /* traverse them all */
  for (i = 0;  i < num_there; i++) {

    /* check the site */
    if ((cs1_ptr = cb_ptr->cside[i]) == NULL) continue;

    /* get the nation information */
    c1_num = cs1_ptr->owner;
    if ((n1_ptr = world.np[c1_num]) == NULL) continue;
    if (c1_num == sct[cb_xloc][cb_yloc].owner) {
      own_site = 1;
    } else {
      own_site = 0;
    }

    /* now go through all of the other elements */
    for (j = 0; j < num_there; j++) {

      /* find the other blokes */
      if (i == j) continue;
      if ((cs2_ptr = cb_ptr->cside[j]) == NULL) continue;
      c2_num = cs2_ptr->owner;
      if (world.np[c2_num] == NULL) continue;

      /* now set the combat interest */
      if ((cs1_ptr->rstat[CGRP_ATTACKER][j] =
	   n1_ptr->dstatus[c2_num] + own_site) >
	  DIP_BELLICOSE) {
	/* found a possible battle! */
	meets++;
      }
      cs1_ptr->rstat[CGRP_SWEEPER][j] =
	cs1_ptr->rstat[CGRP_ATTACKER][j];

    }
  }

  /* done; give results */
  return(meets);
}

/* ORDER_ATTACKS -- Quickie sort to determine attack preference */
static void
order_attacks PARM_4(CSIDE_PTR, cs_ptr, int, whichatk,
		     int *, att_wgt, int, num)
{
  int i, swapped;
  
  /* check input */
  if ((num == 0) ||
      (att_wgt == NULL) ||
      (cs_ptr == NULL)) return;

  /* assign default positions */
  for (i = 0; i < num; i++) {
    att_wgt[i] = i;
  }

  /* rearrange things properly */
  /* modified bubble sort; IMPROVEMENT: use other sort */
  do {
    swapped = FALSE;
    for (i = 0; i < num - 1; i++) {
      if (cs_ptr->rstat[whichatk][att_wgt[i + 1]] >
	  cs_ptr->rstat[whichatk][att_wgt[i]]) {
	swapped = att_wgt[i + 1];
	att_wgt[i + 1] = att_wgt[i];
	att_wgt[i] = swapped;
	swapped = TRUE;
      }
    }
  } while (swapped == TRUE);
}

/* ADD_FIGHTLIGHT -- List the fighters in the battle */
static void
add_fightlist PARM_2(int *, list, int, who)
{
  int i;

  /* check if that person is listed */
  for (i = 0; i < MAX_COMBAT; i++) {
    if (list[i] == who) break;
    if (list[i] == UNOWNED) {
      list[i] = who;
      break;
    }
  }
}

/* CALC_STRENGTHS -- Compute the strength and bonuses of the side */
static void
calc_strengths PARM_3(int, type, int, othtype, CSIDE_PTR, targ_ptr)
{
  CUNIT_PTR cu_ptr;

  /* check the input */
  if (targ_ptr == NULL) return;

  /* initialize the settings */
  targ_ptr->sum_size[type] = 0;
  targ_ptr->avg_bonus[type] = 0.0;

  /* go through all of the fighters */
  for (cu_ptr = targ_ptr->units[type];
       cu_ptr != NULL;
       cu_ptr = cu_ptr->next) {

    /* determine the unit size */
    cu_ptr->rel_size = calc_relsize(cu_ptr);
    if (cu_ptr->rel_size <= 0) continue;

    /* compute the combat bonus of the unit */
    cu_ptr->adjustment = cb_value(targ_ptr, cu_ptr, type, othtype);
    cu_ptr->destruct_value = cb_destval(cu_ptr, type, othtype);

    /* calculate it as the average */
    targ_ptr->avg_bonus[type] =
      ((double) ((targ_ptr->avg_bonus[type] * targ_ptr->sum_size[type]) +
		 (cu_ptr->rel_size * cu_ptr->adjustment))) /
		   (cu_ptr->rel_size + targ_ptr->sum_size[type]);

    /* add in total to side trooops */
    targ_ptr->sum_size[type] += cu_ptr->rel_size;
  }
}

/* GIVE_ZOMBIES -- Slap some undead onto a side */
static long
give_zombies PARM_3(CUNIT_PTR, cu_list, int, dead_pool, int, dam_swing)
{
  CUNIT_PTR cu_ptr;
  ARMY_PTR alast_ptr = NULL, ahold_ptr = army_ptr;
  NTN_PTR nhold_ptr = ntn_ptr;
  int i, nhold_num = country;
  long num_scum, extra_scum, num_makers;

  /* figure out how many zombies there should be */
  num_scum = dead_pool * (dam_swing + 100) / 4;
  num_scum /= 100;

  /* which type is a zombie */
  if (zombie_type == -1) {
    errormsg("WARNING:  Could not find the \"Zombies\" army type");
    msg_cunmark();
    msg_cunmark();
    msg_cunmark();
    msg_cunmark();
    return(0);
  }

  /* total up potential zombie makers */
  ntn_ptr = NULL;
  num_makers = 0;
  for (cu_ptr = cu_list;
       cu_ptr != NULL;
       cu_ptr = cu_ptr->next) {

    /* locate army units */
    if (cu_ptr->type == UNIT_ARMY) {
      if (((army_ptr = cu_ptr->ui.army_p) != NULL) &&
	  a_isundead(ARMY_TYPE)) {
	alast_ptr = army_ptr;
	country = cu_ptr->owner;
	ntn_ptr = world.np[country];
	if (a_ismonster(ARMY_TYPE)) {
	  num_makers += (ainfo_list[ARMY_TYPE].minsth * ARMY_SIZE) / 7;
	} else if (a_isleader(ARMY_TYPE)) {
	  num_makers += ARMY_SIZE / 5;
	} else {
	  num_makers += ARMY_SIZE;
	}
      }
    }

  }

  /* check out the list */
  if (ntn_ptr == NULL) {
    msg_cunmark();
    msg_cunmark();
    msg_cunmark();
    msg_cunmark();
    num_scum = 0;
    goto reset_my_stuff;
  }

  /* now find out how many they get */
  if (num_makers < num_scum) {
    num_scum = num_makers;
  }

  /* now do it */
  if (num_scum > 0) {

    if ((army_ptr = crt_army(zombie_type)) == NULL) {
      msg_cunmark();
      msg_cunmark();
      msg_cunmark();
      msg_cunmark();
      goto reset_my_stuff;
    }

    /* report it */
    sprintf(string, " %ld Zombies rise from the dead and join Nation %s\n",
	    num_scum, ntn_ptr->name);

    /* assign the traits of the unit */
    ARMY_SIZE = num_scum;
    ARMY_STAT = unit_status(alast_ptr->status);
    set_speed(ARMY_STAT, unit_speed(alast_ptr->status));
    ARMY_LEAD = alast_ptr->leader;
    ARMY_XLOC = alast_ptr->xloc;
    ARMY_YLOC = alast_ptr->yloc;
    ARMY_SPLY = start_unitsply();	/* zombies shouldn't need supplies */
    ARMY_LASTX = alast_ptr->lastx;
    ARMY_LASTY = alast_ptr->lasty;

    /* build the unit entry */
    if ((cu_ptr = new_cunit(UNIT_ARMY)) == NULL) {
      dest_army(ARMY_ID);
      army_ptr = NULL;
      num_scum = 0;
      msg_cunmark();
      msg_cunmark();
      msg_cunmark();
      msg_cunmark();
      goto reset_my_stuff;
    }
    cu_ptr->ui.army_p = army_ptr;
    cu_ptr->rel_size = calc_relsize(cu_ptr);
    cu_ptr->next = cu_list->next;
    cu_list->next = cu_ptr;
    msg_creturn(string);

  } else {
    msg_cunmark();
    num_scum = 0;
  }

  /* now handle any lich generation */
  if (lich_type == -1) {
    errormsg("WARNING:  Could not find the \"Lich\" army type");
    msg_cunmark();
    msg_cunmark();
    msg_cunmark();
    goto reset_my_stuff;
  }

  /* now count lich recruits! */
  extra_scum = 0;
  for (i = 0; i < zombie_casters; i++) {
    if (rand_val(100) < (dam_swing + 100) / 5) extra_scum++;
  }

  /* check it out */
  if (extra_scum == 0) {
    msg_cunmark();
  } else {

    /* try to build a new lich unit */
    if ((army_ptr = crt_army(lich_type)) == NULL) {
      msg_cunmark();
      msg_cunmark();
      msg_cunmark();
      goto reset_my_stuff;
    }

    /* report it */
    sprintf(string, " %ld Liches rise from the dead and join Nation %s\n",
	    extra_scum, ntn_ptr->name);

    /* assign the traits of the unit */
    ARMY_SIZE = extra_scum;
    ARMY_STAT = unit_status(alast_ptr->status);
    set_speed(ARMY_STAT, unit_speed(alast_ptr->status));
    ARMY_LEAD = alast_ptr->leader;
    ARMY_XLOC = alast_ptr->xloc;
    ARMY_YLOC = alast_ptr->yloc;
    ARMY_SPLY = start_unitsply();	/* content for a few turns */
    ARMY_LASTX = alast_ptr->lastx;
    ARMY_LASTY = alast_ptr->lasty;

    /* build the unit entry */
    if ((cu_ptr = new_cunit(UNIT_ARMY)) == NULL) {
      dest_army(ARMY_ID);
      army_ptr = NULL;
      msg_cunmark();
      msg_cunmark();
      msg_cunmark();
      goto reset_my_stuff;
    }
    cu_ptr->ui.army_p = army_ptr;
    cu_ptr->rel_size = calc_relsize(cu_ptr);
    cu_ptr->next = cu_list->next;
    cu_list->next = cu_ptr;
    zombie_casters -= extra_scum;
    num_scum += ARMY_SIZE;
    msg_creturn(string);

  }

  /* finally handle any wraith generation */
  if (wraith_type == -1) {
    errormsg("WARNING:  Could not find the \"Wraith\" army type");
    msg_cunmark();
    msg_cunmark();
    goto reset_my_stuff;
  }

  /* now count wraith recruits! */
  extra_scum = 0;
  for (i = 0; i < zombie_leaders; i++) {
    if (rand_val(100) < (dam_swing + 100) / 4) extra_scum++;
  }

  /* check it out */
  if (extra_scum == 0) {
    msg_cunmark();
  } else {

    /* try to build a new lich unit */
    if ((army_ptr = crt_army(wraith_type)) == NULL) {
      msg_cunmark();
      msg_cunmark();
      goto reset_my_stuff;
    }

    /* report it */
    sprintf(string, " %ld Wraiths rise from the dead and join Nation %s\n",
	    extra_scum, ntn_ptr->name);

    /* assign the traits of the unit */
    ARMY_SIZE = extra_scum;
    ARMY_STAT = unit_status(alast_ptr->status);
    set_speed(ARMY_STAT, unit_speed(alast_ptr->status));
    ARMY_LEAD = alast_ptr->leader;
    ARMY_XLOC = alast_ptr->xloc;
    ARMY_YLOC = alast_ptr->yloc;
    ARMY_SPLY = start_unitsply();	/* content for a few turns */
    ARMY_LASTX = alast_ptr->lastx;
    ARMY_LASTY = alast_ptr->lasty;

    /* build the unit entry */
    if ((cu_ptr = new_cunit(UNIT_ARMY)) == NULL) {
      dest_army(ARMY_ID);
      army_ptr = NULL;
      msg_cunmark();
      msg_cunmark();
      goto reset_my_stuff;
    }
    cu_ptr->ui.army_p = army_ptr;
    cu_ptr->rel_size = calc_relsize(cu_ptr);
    cu_ptr->next = cu_list->next;
    cu_list->next = cu_ptr;
    zombie_leaders -= extra_scum;
    num_scum += ARMY_SIZE;
    msg_creturn(string);

  }

  /* lastly handle any nazgul generation */
  if (nazgul_type == -1) {
    errormsg("WARNING:  Could not find the \"Nazgul\" army type");
    msg_cunmark();
    goto reset_my_stuff;
  }

  /* now count wraith recruits! */
  extra_scum = 0;
  for (i = 0; i < zombie_rulers; i++) {
    if (rand_val(100) < (dam_swing + 100) / 4) extra_scum = 1;
  }

  /* check it out */
  if (extra_scum == 0) {
    msg_cunmark();
  } else {

    /* try to build a new nazgul unit */
    if ((army_ptr = crt_army(nazgul_type)) == NULL) {
      msg_cunmark();
      goto reset_my_stuff;
    }

    /* report it */
    sprintf(string, " A Nagul rises from the dead and joins Nation %s\n",
	    ntn_ptr->name);

    /* assign the traits of the unit */
    ARMY_SIZE = ainfo_list[nazgul_type].minsth;
    ARMY_STAT = unit_status(alast_ptr->status);
    set_speed(ARMY_STAT, unit_speed(alast_ptr->status));
    ARMY_LEAD = alast_ptr->leader;
    ARMY_XLOC = alast_ptr->xloc;
    ARMY_YLOC = alast_ptr->yloc;
    ARMY_SPLY = start_unitsply();	/* naguls should always be safe */
    ARMY_LASTX = alast_ptr->lastx;
    ARMY_LASTY = alast_ptr->lasty;

    /* build the unit entry */
    if ((cu_ptr = new_cunit(UNIT_ARMY)) == NULL) {
      dest_army(ARMY_ID);
      army_ptr = NULL;
      msg_cunmark();
      goto reset_my_stuff;
    }
    cu_ptr->ui.army_p = army_ptr;
    cu_ptr->rel_size = calc_relsize(cu_ptr);
    cu_ptr->next = cu_list->next;
    cu_list->next = cu_ptr;
    zombie_rulers -= extra_scum;
    num_scum++;
    msg_creturn(string);

  }

  /* reset things */
 reset_my_stuff:
  army_ptr = ahold_ptr;
  ntn_ptr = nhold_ptr;
  country = nhold_num;
  return(num_scum);
}

/* LUCK_STRING -- Return a string based on the luck roll of combat */
static char *
luck_string PARM_1(int, roll)
{
  static char lstr[10];

  if (roll > 80) {
    strcpy(lstr, "horrid");
  } else if (roll > 70) {
    strcpy(lstr, "worse");
  } else if (roll > 60) {
    strcpy(lstr, "bad");
  } else if (roll > 55) {
    strcpy(lstr, "poor");
  } else if (roll > 44) {
    strcpy(lstr, "average");
  } else if (roll > 39) {
    strcpy(lstr, "fair");
  } else if (roll > 29) {
    strcpy(lstr, "good");
  } else if (roll > 19) {
    strcpy(lstr, "great");
  } else {
    strcpy(lstr, "superb");
  }
  return(lstr);
}

/* FIGHT_IT_OUT -- implement the battle between the two sides */
static int
fight_it_out PARM_4(int, atype, int, type,
		    CSIDE_PTR, atk_ptr, CSIDE_PTR, other_ptr)
{
  CUNIT_PTR cu_ptr;
  CSIDE_PTR cs_ptr;
  int dice_val[2];
  int dam_val[2];
  int dead_men[2];
  long size_val[2];
  int str_inc[2], str_owner[2];
  long zombie_fodder = 0;
  int fdam = 0, odds, dths, cnt, dval, side;

  /* check the input */
  if ((atk_ptr == NULL) || (other_ptr == NULL)) return(0);
  zombie_casters = 0;
  zombie_leaders = 0;
  zombie_rulers = 0;
  str_inc[0] = str_inc[1] = 0;
  str_owner[0] = str_owner[1] = 0;

  /* now test the input */
  if (type < CGRP_NUMBER) {
    /* calculate the unit bonuses and strengths */
    calc_strengths(type, atype, other_ptr);
    calc_strengths(atype, type, atk_ptr);
  } else {
    /* align properly */
    type %= CGRP_NUMBER;
  }

  /* compute total sizes */
  if (atk_ptr->avg_bonus[atype] <= 0)
    atk_ptr->avg_bonus[atype] = 1;
  size_val[0] = (long) (atk_ptr->avg_bonus[atype]) *
    atk_ptr->sum_size[atype];
  if (other_ptr->avg_bonus[type] <= 0)
    other_ptr->avg_bonus[type] = 1;
  size_val[1] = (long) (other_ptr->avg_bonus[type]) *
    other_ptr->sum_size[type];
  if (size_val[0] <= 0) return(0);
  if (size_val[1] <= 0) return(0);

  /* determine the odds (times 10) */
  odds = (10 * (size_val[0] - size_val[1])) / min(size_val[0], size_val[1]);

  /* now make the combat rolls */
  dice_val[0] = combat_roll();
  dice_val[1] = combat_roll();

  /* determine the damages */
  if (odds > 1000) {
    dam_val[0] = 0;
    dam_val[1] = 100;
  } else if (odds < -1000) {
    dam_val[0] = 100;
    dam_val[1] = 0;
  } else {
    for (cnt = 0; cnt < 2; cnt++) {
      dam_val[cnt] = ((int) AVG_DAMAGE * dice_val[cnt]) / 50;
      dam_val[cnt] +=  ((cnt == 0)?-1:1) * (odds * (int) OVERMATCH_ADJ) / 10;
      if (dam_val[cnt] < (dice_val[cnt] * (int) PMINDAMAGE) / 100) {
	dam_val[cnt] = (dice_val[cnt] * (int) PMINDAMAGE) / 100;
      } else if (dam_val[cnt] > 100) {
	dam_val[cnt] = 100;
      }
    }
  }

  /* start the report of this clash */
  sprintf(string, "\n%s %s force meets %s %s force\n",
	  side_desc[atype], (world.np[atk_ptr->owner])->name,
	  (world.np[other_ptr->owner])->name, side_desc[type]);
  msg_conquer(string);
  if (odds > 0) {
    sprintf(string, "Battle Odds = %d.%d to 1\n\n",
	    (odds + 10) / 10, odds % 10);
  } else {
    sprintf(string, "Battle Odds = 1 to %d.%d\n\n",
	    (10 - odds) / 10, (-odds) % 10);
  }
  msg_conquer(string);

  /* give damage to the appropriate side */
  cs_ptr = atk_ptr;
  side = atype;
  cnt = 0;
 do_the_damage:
  /* add to the mail list properly */
  add_cbmail(cs_ptr->owner);

  /* spread it over the individual units */
  dead_men[cnt] = 0;
  for (cu_ptr = cs_ptr->units[side];
       cu_ptr != NULL;
       cu_ptr = cu_ptr->next) {
    if ((dam_val[cnt] == 0) ||
	(dam_val[cnt] == 100) ||
	(cu_ptr->rel_size == cs_ptr->sum_size[side])) {
      dval = dam_val[cnt];
    } else if (cu_ptr->adjustment > 0) {
      dval = (dam_val[cnt] * cs_ptr->avg_bonus[side]) /
	cu_ptr->adjustment;
      if (dval > 100) {
	dval = 100;
      } else if (dval < 0) {
	dval = 0;
      }
    } else {
      dval = 100;
    }
    strength_inc = 0;
    dths = damage_unit(cu_ptr, dval, side);
    str_inc[cnt] = strength_inc;
    str_owner[cnt] = cs_ptr->owner;
    if (dths < 0) {
      dead_men[cnt] -= dths;
      cs_ptr->zombie_mkr[side] += dths;
    } else {
      zombie_fodder += dths;
      dead_men[cnt] += dths;
    }
    if (cu_ptr->owner != cs_ptr->owner) {
      add_cbmail(cu_ptr->owner);
    }
    if (cu_ptr->destruct_value > 0) {
      long lholdval = ((dam_val[1] - dam_val[0] / 2) *
		       cu_ptr->destruct_value) / 2000;
      if (lholdval > 0) {
	fdam += lholdval / 100 + (rand_val(100) < lholdval % 100);
      }
    }
  }

  /* describe it to the mail summary */
  sprintf(string, " %s: Strength Level = %ld;  Avg Bonus = %.1f%%\n",
	  (world.np[cs_ptr->owner])->name,
	  cs_ptr->sum_size[side],
	  cs_ptr->avg_bonus[side]);
  msg_conquer(string);
  sprintf(string, " %s: Luck = %s; Avg Damage = %d%%; Deaths = %d\n",
	  (world.np[cs_ptr->owner])->name,
	  luck_string(dice_val[cnt]), dam_val[cnt], dead_men[cnt]);
  msg_conquer(string);
  if ((side == CGRP_FORTIFIED) && (fdam > 0)) {
    sct_ptr = &(sct[cb_xloc][cb_yloc]);
    if ((city_ptr = citybyloc(world.np[cs_ptr->owner], cb_xloc,
			      cb_yloc)) != NULL) {
      if (fdam > CITY_FORT) {
	msg_conquer(" Fortifications in the sector were destroyed\n");
	CITY_FORT = 0;
	clr_minordesg(sct_ptr->designation, MIN_DEVASTATED);
      } else {
	msg_conquer(" Fortifications in the sector were damaged\n");
	CITY_FORT -= fdam;
      }
    } else {
      msg_conquer(" Fortifications in the sector were destroyed\n");
      clr_minordesg(sct_ptr->designation, MIN_DEVASTATED);
    }
  }
  if (cs_ptr->zombie_mkr[side] > 0) {
    msg_cmark();
    msg_cmark();
    msg_cmark();
    msg_cmark();
  }
  msg_conquer("\n");

  /* go back and damage the other side */
  if (cs_ptr != other_ptr) {
    cs_ptr = other_ptr;
    cnt = 1;
    side = type;
    goto do_the_damage;
  }

  for (cnt = 0; cnt < 2; cnt++) {
    if (str_inc[cnt] != 0) {
      sprintf(string,
	      "1.\tFierce monsters meet their doom against Nation %s.\n",
	      (world.np[str_owner[cnt ^ 1]])->name);
      sorted_news(str_owner[cnt ^ 1], string);
      if (((cnt == 1) && (atype <= CGRP_ATTACKER)) ||
	  ((cnt == 0) && (type <= CGRP_ATTACKER))) {
	sprintf(string, " Nation %s gains +%d attack bonus from the combat\n",
		(world.np[str_owner[cnt ^ 1]])->name,
		str_inc[cnt]);
	(world.np[str_owner[cnt ^ 1]])->aplus += str_inc[cnt];
      } else {
	sprintf(string, " Nation %s gains +%d defense bonus from the combat\n",
		(world.np[str_owner[cnt ^ 1]])->name,
		str_inc[cnt]);
	(world.np[str_owner[cnt ^ 1]])->dplus += str_inc[cnt];
      }
      msg_conquer(string);
    }
  }

  /* now check for zombifications */
  if (atk_ptr->zombie_mkr[atype] > 0) {
    atk_ptr->zombie_mkr[atype] += give_zombies(atk_ptr->units[atype],
					       zombie_fodder,
					       dam_val[1] - dam_val[0]);
  }
  if (other_ptr->zombie_mkr[type] > 0) {
    other_ptr->zombie_mkr[type] += give_zombies(other_ptr->units[type],
						zombie_fodder,
						dam_val[0] - dam_val[1]);
  }

  /* is the attacker stopped? */
  cnt = 0;
  if ((dam_val[0] >= DAMAGE_LIMIT) ||
      (dam_val[0] >= dam_val[1])) {
    cnt |= 1;
  }
  if ((type == CGRP_FORTIFIED) &&
      (dam_val[1] < 80)) {
    cnt |= 1;
  }
  if ((type <= CGRP_ATTACKER) &&
      ((dam_val[1] >= DAMAGE_LIMIT) ||
       (dam_val[1] >= dam_val[0]))) {
    cnt |= 2;
  }
  return(cnt);
}

/* BEGIN_BATTLES -- Traverse all of the combat structures */
static void
begin_battles PARM_0(void)
{
  COMBAT_PTR cb_ptr;
  CSIDE_PTR cs1_ptr, cs2_ptr;
  int i, j, k;
  int their_fault[MAX_COMBAT], att_weights[MAX_COMBAT];
  int hit_grp, targ_grp, targ_side, btl_done, combatants;

  /* tracking */
  fprintf(fupdate, "  checking for any battles\n");

  /* do each battle */
  for (cb_ptr = combat_list;
       cb_ptr != NULL;
       cb_ptr = cb_ptr->next) {

    /* join up them allies */
    combatants = merge_allies(cb_ptr);

    /* set some stuff */
    cb_xloc = cb_ptr->xloc;
    cb_yloc = cb_ptr->yloc;

    /* check if there are any possible battles */
    if (set_hostilities(cb_ptr, combatants)) {

      /* initialize the some stuff */
      for (i = 0; i < MAX_COMBAT; i++) {
	their_fault[i] = UNOWNED;
	cb_mail[i] = -1;
      }

      /* start up the mail message */
      country = UNOWNED;
      ntn_ptr = NULL;
      sprintf(string, "Battle Summary for Sector [%d,%d]",
	      cb_xloc, cb_yloc);
      msg_cinit(string);

      /* continuation point */
      hit_grp = CGRP_SWEEPER;
    do_cgrp_attackers:

      /* loop over each target group */
      btl_done = FALSE;
      for (targ_grp = 0;
	   (targ_grp < CGRP_NUMBER) &&
	   (btl_done == FALSE);
	   targ_grp++) {

	/* discontinued attack */
	if ((hit_grp == CGRP_SWEEPER) &&
	    (targ_grp > CGRP_DEFENDER)) break;

	/* implement the battle(s) */
	for (i = 0; i < combatants; i++) {

	  /* check the side information */
	  if ((cs1_ptr = cb_ptr->cside[i]) == NULL) continue;

	  /* check for attackers within this group */
	  add_cbmail(cs1_ptr->owner);
	  if (cs1_ptr->units[hit_grp] == NULL) continue;

	  /* determine preferential attack ordering */
	  order_attacks(cs1_ptr, hit_grp, &(att_weights[0]), combatants);

	  /* now traverse and find victims */
	  for (j = 0; j < combatants; j++) {

	    /* check for the victim */
	    targ_side = att_weights[j];
	    if (cs1_ptr->rstat[hit_grp][targ_side] > DIP_BELLICOSE) {

	      /* verify victimization */
	      if ((cs2_ptr = cb_ptr->cside[targ_side]) == NULL) continue;
	      if (cs2_ptr->units[targ_grp] == NULL) continue;

	      /* WE HAVE A BATTLE! */
#ifdef DEBUG
	      fprintf(fupdate, "    %s %s force meets %s %s force\n",
		      (world.np[cs1_ptr->owner])->name, side_desc[hit_grp],
		      (world.np[cs2_ptr->owner])->name, side_desc[targ_grp]);
#endif /* DEBUG */
	      add_fightlist(&(their_fault[0]), cs1_ptr->owner);
	      add_fightlist(&(their_fault[0]), cs2_ptr->owner);
	      if ((k = fight_it_out(hit_grp, targ_grp,
				    cs1_ptr, cs2_ptr)) > 0) {
		if (k & 1) {
		  for (j = 0; j < combatants; j++) {
		    /* no more fighting for this turkey */
		    cs1_ptr->rstat[hit_grp][j] = 0;
		  }
		}
		if (k & 2) {
		  for (j = 0; j < combatants; j++) {
		    /* no more fighting for this one */
		    cs2_ptr->rstat[targ_grp][j] = 0;
		  }
		}
	      }

	    } else {
	      /* nobody else left worth fighting */
	      break;
	    }
	  }

	}

      }

      /* check it out */
      if (hit_grp < CGRP_ATTACKER) {
	hit_grp++;
	goto do_cgrp_attackers;
      }

      /* report of any completed battle */
      if (their_fault[0] != UNOWNED) {

	/* give the news report */
	strcpy(string, "4.\tBattle occurs between ");
	for (i = 0; i < MAX_COMBAT; i++) {
	  if (their_fault[i] == UNOWNED) break;
	  if (i > 0) strcat(string, ", ");
	  strcat(string, world.np[their_fault[i]]->name);
	}
	strcat(string, ".\n");
	sorted_news(their_fault[0], string);

#ifdef DEBUG
	/* while debugging; god gets all reports */
	msg_csend();
#endif /* DEBUG */

	/* now give the full mail reports to combatants */
	for (i = 0; i < MAX_COMBAT; i++) {
	  /* check for data */
	  if (cb_mail[i] == -1) break;

	  /* now build the information */
	  country = cb_mail[i];
	  ntn_ptr = world.np[country];

	  /* set the header */
	  sprintf(string, "Battle Summary for Sector [%d,%d]",
		  xloc_relative(cb_xloc),
		  yloc_relative(cb_yloc));
	  msg_cadjust(country, string);
	  msg_csend();
	}
	msg_cfinish(FALSE);
      }

    }

  }
}

/* GRAB_BATTLES -- Perform distance battles */
static void
grab_battles PARM_0(void)
{
  /* tracking */
  fprintf(fupdate, "  searching among distance attacks for battles (unimp)\n");
}

/* WIPE_MAP -- free up utilized memory */
static void
wipe_map PARM_0(void)
{
  COMBAT_PTR cb_ptr, cb_tmp;
  CSIDE_PTR cs_ptr;
  CUNIT_PTR cu_ptr, cu_tmp;
  CDIST_PTR cd_ptr, cd_tmp;
  int i, j;

  /* tracking */
  fprintf(fupdate, "  recovering memory used during combat\n");

  /* traverse the battle lists */  
  for (cb_ptr = combat_list;
       cb_ptr != NULL; ) {

    /* search out all of the sides */
    for (i = 0; i < MAX_COMBAT; i++) {

      /* is there anything there */
      if ((cs_ptr = cb_ptr->cside[i]) == NULL) continue;

      /* now fix the units */
      for (j = 0; j < CGRP_NUMBER; j++) {
	/* find all of the units */
	for (cu_ptr = cs_ptr->units[j];
	     cu_ptr != NULL; ) {
	  /* free them up */
	  cu_tmp = cu_ptr;
	  cu_ptr = cu_ptr->next;
	  cu_tmp->next = NULL;
	  free(cu_tmp);
	}
      }

      /* can free up the data */
      free(cs_ptr);
    }

    /* release combat memory */
    cb_tmp = cb_ptr;
    cb_ptr = cb_ptr->next;
    cb_tmp->next = NULL;
    free(cb_tmp);
  }
  combat_list = NULL;

  /* get rid of distance attackers */
  for (cd_ptr = distance_list;
       cd_ptr != NULL; ) {
    /* nuke it */
    cd_tmp = cd_ptr;
    cd_ptr = cd_ptr->next;
    cd_tmp->next = NULL;
    free(cd_tmp);
  }
  distance_list = NULL;

  /* now release the memory for the sector tracking */
  free(sct_combval);
  sct_combval = NULL;

  /* finally send out all of the stored up news */
  send_sortednews();
}

/* COMBAT -- Controlling routine to handle the entire combat scheme */
void
combat PARM_0(void)
{
  NTN_PTR nhold_ptr = ntn_ptr;
  int chold_val = country;

  /* give the introduction */
  fprintf(fupdate, "=== Searching for Battles\n");

  /* initialize everything */
  combat_init();

  /* gather the troops */
  battle_grouping();

  /* perform the single sector battles */
  begin_battles();

  /* check the distance encounters */
  grab_battles();

  /* clean up things */
  wipe_map();

  /* restore settings */
  ntn_ptr = nhold_ptr;
  country = chold_val;
}
