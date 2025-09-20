// SPDX-License-Identifier: GPL-3.0-or-later
/* routines for calculating the results of a world turn */
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
#include "navyX.h"
#include "butesX.h"
#include "calenX.h"
#include "desigX.h"
#include "magicX.h"
#include "mtrlsX.h"
#include "racesX.h"
#include "worldX.h"
#include "activeX.h"
#include "hlightX.h"
#include "statusX.h"
#include "tgoodsX.h"
#include "caravanX.h"
#include "dstatusX.h"

/* bitwise sector status storage */
char *sct_status = NULL;

/* production information */
itemtype j_produced, m_produced;

/* UPD_INIT -- Initialize update data storages */
static void
upd_init PARM_0(void)
{
  /* obtain the memory */
  sct_status = new_mapchar(sct_status);

  /* now open the news program */
  sprintf(string, "%s.%03d", newsfile, TURN - START_TURN + 1);
  if ((fnews = fopen(string, "a")) == NULL) {
    fprintf(fupdate, "Major Error: unable to open news file %s\n", string);
    abrt();
  }
}

/* UPD_FINISH -- Release memory and close up shop */
static void
upd_finish PARM_0(void)
{
  /* finish up on the news */
  roman_number(string, YEAR(TURN));
  fprintf(fnews, "1\tGLOBAL HEADLINE NEWS -- %s of Year %s\n",
	  PMONTH(TURN), string);
  fprintf(fnews, "2\tTHE GLOBAL COMMON MARKET -- %s of Year %s\n",
	  PMONTH(TURN), string);
  fprintf(fnews, "3\tREAL ESTATE LISTINGS -- %s of Year %s\n",
	  PMONTH(TURN), string);
  fprintf(fnews, "4\tTHE WORLD AT WAR! -- %s of Year %s\n",
	  PMONTH(TURN), string);
  fprintf(fnews, "5\tPERSONAL LISTINGS -- %s of Year %s\n",
	  PMONTH(TURN), string);
  fclose(fnews);
  
  /* sort the news */
  TURN++;
  sort_news(TURN);

  /* thanks for the memory */
  free(sct_status);
}

/* UPD_ROVERS -- Relocate all roving units */
static void
upd_rovers PARM_0(void)
{
  /* go through all of the nations */
  fprintf(fupdate, "=== Relocating Roving Units\n");
  for (country = 1; country < MAXNTN; country++) {
    /* process valid nations */
    if (((ntn_ptr = world.np[country]) != NULL) &&
	n_isactive(ntn_ptr->active)) {

      /* calculate the weights */
      set_weights(FALSE);

      /* go through the list of armies */
      for (army_ptr = ntn_ptr->army_list;
	   army_ptr != NULL;
	   army_ptr = army_ptr->next) {
	/* make rover units relocate */
	if (rover_stat(ARMY_STAT)) {
	  rove_army();
	}
      }

    }
  }
}

/* UPD_ARMY -- Adjust all of the army statuses and movements */
static void
upd_army PARM_0(void)
{
  ARMY_PTR a2_ptr = NULL;
  int pt_limit, chance, ival;
  int have_sorcerer = 0, have_leader = 0;

  /* mark leaders */
  if (n_ismonster(ntn_ptr->active)) {
    have_leader = 1;
  } else {
    mark_leaders();
  }
  
  /* first pass; check groups and leaders */
  for (army_ptr = ntn_ptr->army_list;
       army_ptr != NULL;
       army_ptr = army_ptr->next) {

    /* first eleminate spell statuses */
    set_unspell(ARMY_STAT);

    /* handle mislocated units */
    if (!XY_INMAP(ARMY_XLOC, ARMY_YLOC)) {
      ARMY_XLOC = ntn_ptr->capx;
      ARMY_YLOC = ntn_ptr->capy;
    }

    /* now turn flight into normal movement */
    set_unflight(ARMY_STAT);

    /* no farther for monster nations */
    if (n_ismonster(ntn_ptr->active)) continue;

    /* check grouping */
    if (unit_status(ARMY_STAT) == ST_GROUPED) {

      /* get the leader */
      if ((a2_ptr == NULL) ||
	  (a2_ptr->armyid != ARMY_LEAD)) {
	a2_ptr = armybynum(ARMY_LEAD);
      }

      /* check if the leader is proper */
      if ((a2_ptr == NULL) ||
	  !unit_leading(a2_ptr->status) ||
	  (a2_ptr->xloc != ARMY_XLOC) ||
	  (a2_ptr->yloc != ARMY_YLOC)) {
	/* not being lead */
	set_status(ARMY_STAT, ST_DEFEND);
	ARMY_LEAD = EMPTY_HOLD;
      } else {
	/* indicate to the leader that a unit was found */
	if (a2_ptr->umove != 201) {
	  a2_ptr->umove = 201;
	}
      }
    }

    /* check for the top of the heap */
    if (a_isleader(ARMY_TYPE)) {
      if (a_isruler(ARMY_TYPE)) {
	have_leader = 1;
      }
      if (a_fullcaster(ARMY_TYPE)) {
	have_sorcerer = 1;
      }
      if (ARMY_MOVE != 201) {
	ARMY_MOVE = 0;
      }
    }

  }

  /* check for births, etc. */
  if ((is_update == TRUE) &&
      XY_INMAP(ntn_ptr->capx, ntn_ptr->capy) &&
      (sct[ntn_ptr->capx][ntn_ptr->capy].owner == country)) {

    /* check for leader births */
    if (n_ismonster(ntn_ptr->active) &&
	n_isnpc(ntn_ptr->active)) {
      chance = 0;
      ival = 0;
    } else {
      ival = getminleader(ntn_ptr->class);
      if ((ival > 0) && (ival < num_armytypes)) {
	chance = (int) ainfo_list[ival].maint;
	if (have_leader == 0) {
	  chance *= 3;
	  chance /= 2;
	}
      } else {
	chance = 0;
      }
    }

    /* now perform the check */
    if (chance > rand_val(1200)) {

      /* build it */
      if ((army_ptr = crt_army(ival)) != NULL) {

	/* provide the settings */
	ARMY_SIZE = ainfo_list[ival].minsth;
	ARMY_XLOC = ntn_ptr->capx;
	ARMY_YLOC = ntn_ptr->capy;
	if (citybyloc(ntn_ptr, ARMY_XLOC, ARMY_YLOC) != NULL) {
	  ARMY_STAT = ST_RESERVE;
	} else {
	  ARMY_STAT = ST_DEFEND;
	}

	/* advertise */
	fprintf(fnews, "1.\tA new minor leader gains power in Nation %s\n",
		ntn_ptr->name);
	switch (rand_val(4)) {
	case 0:
	  sprintf(string, "\tThrough political savy, a skilled new %s joins\n the ranks of your leaders.\n",
		  ainfo_list[ival].name);
	  ARMY_SIZE *= 11;
	  ARMY_SIZE /= 10;
	  break;
	default:
	  sprintf(string, "\tA bold young %s has come of age.\n",
		  ainfo_list[ival].name);
	  break;
	}
	msg_conquer(string);

      }

    }

    /* check for monster recruitment */
    if ((SEASON(TURN) == SPRING_SEASON) &&
	MIL_MAGIC(MM_ORC)) {

      /* 25% per year chance for recruitment */
      chance = 100;
      if (chance > rand_val(1200)) {

	/* now find the size */
	chance = 100;
	if (MIL_MAGIC(MM_OGRE)) {
	  chance += 200;
	}
	if (MIL_MAGIC(MM_DRAGON)) {
	  chance += 700;
	}

	/* get a unit of the right size */
	ival = rand_monstunit(chance);

	/* report it */
	if ((army_ptr = crt_army(ival)) != NULL) {

	  /* fix it up */
	  ARMY_SIZE = 1;
	  ARMY_SPLY = start_unitsply();
	  ARMY_XLOC = ntn_ptr->capx;
	  ARMY_YLOC = ntn_ptr->capy;

	  /* report it */
	  fprintf(fnews, "1.\tMonster recruited in Nation %s\n",
		  ntn_ptr->name);
	  sprintf(string, "A %s joins your cause.\n",
		  ainfo_list[ival].name);
	  msg_conquer(string);

	}

      }

    }

  }

  /* report any lack of leader */
  if ((is_update == TRUE) &&
      (have_leader == 0)) {
    fprintf(fnews, "1.\tNation %s is without a national leader\n",
	    ntn_ptr->name);
    fprintf(fupdate, "    Nation %s is without a national leader\n",
	    ntn_ptr->name);
    msg_conquer("You have no national leader\n");
  }
      

  /* second pass; set movement and adjust statuses */
  for (army_ptr = ntn_ptr->army_list;
       army_ptr != NULL;
       army_ptr = army_ptr->next) {

    /* check for sieges */
    if (SCT_STATUS(ARMY_XLOC, ARMY_YLOC) & SET_SIEGE) {
      /* siege enforced */
      if (siege_stat(ARMY_STAT)) {
	ARMY_MOVE = 0;
	set_status(ARMY_STAT, ST_SIEGED);
      }
    } else if (unit_status(ARMY_STAT) == ST_SIEGED) {
      /* troops now released */
      if (a_isscout(ARMY_TYPE)) {
	set_status(ARMY_STAT, ST_DEFEND);
      } else {
	set_status(ARMY_STAT, ST_GARRISON);
      }
    } else if (unit_status(ARMY_STAT) == ST_SIEGE) {
      /* siege broken; troops not stuck */
      set_status(ARMY_STAT, ST_DEFEND);
    }

    /* send sorties back to the garrison  */
    if (unit_status(ARMY_STAT) == ST_SORTIE) {
      set_status(ARMY_STAT, ST_GARRISON);
    }

    /* give flight to those capable of it */
    if (a_isavian(ARMY_TYPE)) {
      set_flight(ARMY_STAT);
    }

    /* check for increase in spell points */
    pt_limit = army_spellpts(ntn_ptr, army_ptr);
    if (pt_limit > 0) {
      if ((int) ARMY_SPTS < pt_limit) {
	if ((major_desg(sct[ARMY_XLOC][ARMY_YLOC].designation) ==
	     MAJ_SHRINE) &&
	    (sct[ARMY_XLOC][ARMY_YLOC].owner == country)) {
	  ARMY_SPTS += magic_value(&(sct[ARMY_XLOC][ARMY_YLOC])) + 2;
	} else {
	  ARMY_SPTS++;
	}
      }
      if ((int)ARMY_SPTS > pt_limit) {
	ARMY_SPTS -= ((int)ARMY_SPTS - pt_limit + 1) / 2;
      }
    } else {
      ARMY_SPTS = 0;
    }

    /* now set the movement */
    if (a_isleader(ARMY_TYPE)) {

      /* check for random experience */
      if (rand_val(1200) < PLEADER_EXP) {
	ARMY_SIZE++;
      }

      /* increase the health of the leader */
      if ((ARMY_EFF < 100) && !unit_healed(ARMY_STAT)) {
	if (nomove_stat(ARMY_STAT)) {
	  ARMY_EFF += 20;
	} else {
	  ARMY_EFF += 10;
	}
      }
      if (ARMY_EFF > 100) {
	ARMY_EFF = 100;
      }

      /* assuming command? */
      if (is_update == TRUE) {
	if ((have_leader == 0) &&
	    !a_ismagician(ARMY_TYPE)) {
	  if (rand_val(3) == 0) {

	    have_leader = -1;
	    ARMY_SIZE = ainfo_list[--ARMY_TYPE].minsth;

	    /* advertise */
	    fprintf(fnews,
		    "1.\tA new national leader assumes command in Nation %s\n",
		    ntn_ptr->name);
	    sprintf(string, "\t%s %d takes the reigns of your nation\n",
		    ainfo_list[ARMY_TYPE + 1].name, ARMY_ID);
	    msg_conquer(string);
	    fprintf(fupdate, "  A minor leader in Nation %s takes over\n",
		    ntn_ptr->name);

	  }
	}
	if ((have_sorcerer == 0) &&
	    a_ismagician(ARMY_TYPE)) {
	  if (rand_val(2) == 0) {

	    have_sorcerer = 1;
	    ARMY_SIZE = ainfo_list[--ARMY_TYPE].minsth;

	    /* let them know */
	    sprintf(string, "\t%s %d attains full magical strength\n",
		    ainfo_list[ARMY_TYPE + 1].name, ARMY_ID);
	    msg_conquer(string);
	    fprintf(fupdate, "  A sorcerer in Nation %s gains power\n",
		    ntn_ptr->name);

	  }
	}
      }

      /* check movement */
      if (!unit_leading(ARMY_STAT) ||
	  (ARMY_MOVE != 201)) {
	/* movement settings */
	set_unlead(ARMY_STAT);
      }

      /* fix the movement ability */
      if (nomove_stat(ARMY_STAT)) {
	ARMY_MOVE = 75;
	set_speed(ARMY_STAT, SPD_STUCK);
      } else {
	ARMY_MOVE = 100;
      }

    } else {

      /* check the movment */
      if (unit_status(ARMY_STAT) == ST_GROUPED) {

	/* get the leader */
	if ((a2_ptr == NULL) ||
	    (a2_ptr->armyid != ARMY_LEAD)) {
	  a2_ptr = armybynum(ARMY_LEAD);
	}

	/* check the leader movment */
	if (a2_ptr == NULL) {
	  set_status(ARMY_STAT, ST_DEFEND);
	  ARMY_MOVE = 100;
	} else {
	  if (nomove_stat(a2_ptr->status)) {
	    ARMY_MOVE = 75;
	    set_speed(ARMY_STAT, SPD_STUCK);
	  } else {
	    ARMY_MOVE = 100;
	  }
	}

      } else if (have_leader != 1) {

	/* disruption among leaderless */
	ARMY_MOVE = 0;

      } else {

	/* get the proper movement */
	if (nomove_stat(ARMY_STAT)) {
	  if (!n_ismonster(ntn_ptr->active) &&
	      !a_isscout(ARMY_TYPE) &&
	      (VIS_CHECK(ARMY_XLOC, ARMY_YLOC) == FALSE)) {
	    ARMY_MOVE = 40;
	  } else {
	    ARMY_MOVE = 75;
	  }
	  set_speed(ARMY_STAT, SPD_STUCK);
	} else if (!n_ismonster(ntn_ptr->active) &&
		   !a_isscout(ARMY_TYPE) &&
		   (VIS_CHECK(ARMY_XLOC, ARMY_YLOC) == FALSE)) {
	  ARMY_MOVE = 50;
	} else {
	  ARMY_MOVE = 100;
	}

      }

      if (a_decay(ARMY_TYPE)) {
	if (ARMY_EFF > 50) {
	  ARMY_EFF -= rand_val(5) + 5;
	} else {
	  /* turn them into another unit */
	  ARMY_EFF = 100;
	  ARMY_TYPE--;
	}
      }
		
    }

    /* limit the efficiency */
    if (a_isleader(ARMY_TYPE)) {
      if (ARMY_EFF > 100) ARMY_EFF = 100;
    } else {
      if (!unit_healed(ARMY_STAT)) {
	if (ARMY_EFF < ARMY_MAXEFF) {
	  ARMY_EFF += heal_rate(ntn_ptr, army_ptr);
	}
	if ((ARMY_MAXEFF < 100) &&
	    (fort_stat(ARMY_STAT))) {
	  ARMY_MAXEFF++;
	}
      }
      if (ARMY_EFF > ARMY_MAXEFF) {
	ARMY_EFF = ARMY_MAXEFF;
      }
    }

    /* now clear up any healing */
    set_unhealed(ARMY_STAT);

  }
}

/* UPD_NAVY -- Adjust all of the navy statuses and movements */
static void
upd_navy PARM_0(void)
{
  int nstat;

  /* just keep on movin' */
  for (navy_ptr = ntn_ptr->navy_list;
       navy_ptr != NULL;
       navy_ptr = navy_ptr->next) {

    /* first eleminate spell statuses */
    set_unspell(NAVY_STAT);

    /* handle mislocated units */
    if (!XY_INMAP(NAVY_XLOC, NAVY_YLOC)) {
      NAVY_XLOC = ntn_ptr->capx;
      NAVY_YLOC = ntn_ptr->capy;
    }

    /* now check the movement based on the statuses */
    nstat = unit_status(NAVY_STAT);
    if (nomove_stat(nstat)) {
      NAVY_MOVE = 75;
    } else {
      NAVY_MOVE = 100;
    }

    /* all of the movment was lost, but ship is now back */
    if (nstat == ST_REPAIR) {
      set_status(NAVY_STAT, ST_CARRY);
    }
  }
}

/* UPD_CVN -- Adjust all of the caravan statuses and movements */
static void
upd_cvn PARM_0(void)
{
  int vstat;

  /* just keep on movin' */
  for (cvn_ptr = ntn_ptr->cvn_list;
       cvn_ptr != NULL;
       cvn_ptr = cvn_ptr->next) {

    /* first eleminate spell statuses */
    set_unspell(CVN_STAT);

    /* handle mislocated units */
    if (!XY_INMAP(CVN_XLOC, CVN_YLOC)) {
      CVN_XLOC = ntn_ptr->capx;
      CVN_YLOC = ntn_ptr->capy;
    }

    /* find the status */
    vstat = unit_status(CVN_STAT);
    if (vstat == ST_SIEGED) {
      set_status(CVN_STAT, ST_CARRY);
    }

    /* check for any sieges */
    if (SCT_STATUS(CVN_XLOC, CVN_YLOC) & SET_SIEGE) {
      /* ain't goin' no where */
      CVN_MOVE = 0;
      set_status(CVN_STAT, ST_SIEGED);
    } else {
      /* adjust the movement based on the status */
      if (nomove_stat(vstat)) {
	CVN_MOVE = 75;
      } else {
	CVN_MOVE = 100;
      }
    }

    /* all of the movment was lost, but caravan is now back */
    if (vstat == ST_REPAIR) {
      set_status(CVN_STAT, ST_CARRY);
    }
  }
}

/* UPD_MILITARY -- Call all of the appropriate army, navy, caravan routines */
void
upd_military PARM_1(int, cntry)
{
  int start, end;

  /* check the range */
  if (cntry == UNOWNED) {
    fprintf(fupdate, "=== Calculating World Military\n");
    start = 1;
    end = MAXNTN;
  } else {
    start = cntry;
    end = cntry + 1;
  }

  /* go through appropriate nations */
  for (country = start; country < end; country++) {

    /* process it */
    if ((ntn_ptr = world.np[country]) != NULL) {

      /* check this out */
      if (n_notactive(ntn_ptr->active)) continue;

#ifdef DEBUG
      /* indicate position */
      if (is_update == TRUE) {
	fprintf(fupdate, "  Updating military for nation %s\n",
		ntn_ptr->name);
      }
#endif /* DEBUG */

      /* initialize things */
      msg_cinit("Military Summary");
      sct_status = new_mapchar(sct_status);

      /* set army information */
      upd_army();

      /* set navy information */
      upd_navy();

      /* update caravan information */
      upd_cvn();

      /* close up the multi-line message */
      msg_cfinish(TRUE);
    }

  }
    
}

/* UPD_INPUT -- Read in the exec files and automate nation moves */
static void
upd_input PARM_0(void)
{
  char update_done[ABSMAXNTN];
  int i, number, hold, monst_done = FALSE;

  /* intro */
  fprintf(fupdate, "=== Reading in User Commands\n");

  /* initial nation settings */
  for (i = 0; i < ABSMAXNTN; i++) {
    update_done[i] = FALSE;
  }

  /* start with god */
  country = UNOWNED;
  execute(country);

  /* process all nations, monsters first, then a random order */
  for (i = MAXNTN; i > 0; i--) {

    /* find which selection is to be made */
    if (monst_done == FALSE) {
      for (hold = MAXNTN - i; hold < MAXNTN; hold++) {
	if ((update_done[hold] == 0) &&
	    ((ntn_ptr = world.np[hold]) != NULL) &&
	    n_ismonster(ntn_ptr->active)) {
	  break;
	}
      }
      if (hold == MAXNTN) {
	monst_done = TRUE;
	i++;
	continue;
      }
    } else {
      if (i == 1) hold = 1;
      else hold = rand_val(i) + 1;
    }

    /* determine which one */
    if (monst_done == TRUE) {
      number = 0;
      for (country = 1; country < MAXNTN; country++) {
	if ((update_done[country] == 0) &&
	    (++number == hold)) break;
      }
    } else {
      country = hold;
    }

    /* now update the correct country */
    update_done[country] = 1;
    if ((ntn_ptr = world.np[country]) == NULL) continue;

    /* give information about update */
    fprintf(fupdate, "  Updating nation %s [%d]...\n",
	    ntn_ptr->name, country);

    /* make sure to update the nation */
    set_weights(TRUE);
    if (((execute(country) == 0) &&
	 n_automove(ntn_ptr->active)) ||
	!n_ispc(ntn_ptr->active)) {
      /* automatically perform the move for the nation */
      move_for_ntn();
    }
  }

  /* now check the data */
  verify_data(__FILE__, __LINE__);
}

/* UPD_SPELLS -- Perform the spells in various sectors */
static void
upd_spells PARM_0(void)
{
  SPLINFO_PTR spell_ptr;
  char caster_name[NAMELTH + 1];
  int hold;

  /* intro */
  fprintf(fupdate, "=== Checking For Spell Effects\n");

  /* go through each spell, assigning the affects */
  for (spell_ptr = upd_spl_list;
       spell_ptr != NULL;
       spell_ptr = spell_ptr->next) {

    /* figure out the name */
    if ((hold = spell_ptr->caster) == UNOWNED) {
      ntn_tptr = NULL;
      strcpy(caster_name, "god");
    } else if ((ntn_tptr = world.np[hold]) != NULL) {
      strcpy(caster_name, ntn_tptr->name);
    } else {
      strcpy(caster_name, "unknown");
    }

    /* now find the results */
    switch (spell_ptr->type) {
    default:
      /* print an error */
      fprintf(fupdate, "Unknown spell number %d: cast by %s in [%d,%d]\n",
	      spell_ptr->type, caster_name,
	      spell_ptr->xloc, spell_ptr->yloc);
      break;
    }

  }

}

/* SCORE_NTN -- Compute the score for a single nation */
static void
score_ntn PARM_1(NTN_PTR, n1_ptr)
{
  int i, hold = n1_ptr->score;
  long lvalue = 0;

  /* first, allow for the old score to play a part */
  hold *= 50;
  hold /= 100;

  /* add in magic powers in the nation... 2 pts each */
  for (i = 0; i < 32; i++) {
    lvalue = ( ((long) 1L) << i);
    if (MAGIC(n1_ptr->powers[MAG_MILITARY], lvalue)) hold += 2;
    if (MAGIC(n1_ptr->powers[MAG_CIVILIAN], lvalue)) hold += 2;
    if (MAGIC(n1_ptr->powers[MAG_WIZARDRY], lvalue)) hold += 2;
  }

  /* 1 pt for every leader */
  hold += n1_ptr->tleaders;

  /* 2 pts for every percent of military */
  hold += (200 * n1_ptr->tmil) / WORLDMIL;

  /* 1 pt for every percent of the world population */
  hold += (100 * n1_ptr->tciv) / WORLDCIV;

  /* 1 pt for every percent of the various raw materials */
  for (i = 0; i < MTRLS_NUMBER; i++) {
    if (n1_ptr->mtrls[i] > (itemtype) 0) {
      hold += (100 * n1_ptr->mtrls[i]) / WORLDMTRLS[i];
    }
  }

  /* 1 pt for every percent of the world land area */
  hold += (100 * n1_ptr->tsctrs) / WORLDSCT;

  /* now record the score */
  n1_ptr->score = hold;
}
  
/* this looks familiar */
#define OLD_POP(x, y)	(old_pop[(x) + (y) * MAPX])

/* UPD_SEENEM -- Check if two nations have met yet */
static void
upd_seenem PARM_2(int, x, int, y)
{
  int cntry = sct[x][y].owner;

  /* is they unmet? */
  if ((cntry != UNOWNED) &&
      (cntry != country)) {
    if ((ntn_ptr->dstatus[cntry] == DIP_UNMET) &&
	(world.np[cntry] != NULL)) {
      /* now they have met */
      fprintf(fupdate, "    Nation %s and nation %s meet.\n",
	      ntn_ptr->name, (world.np[cntry])->name);
      fprintf(fnews, "1.\tNation %s and nation %s meet.\n",
	      ntn_ptr->name, (world.np[cntry])->name);
      ntn_ptr->dstatus[cntry] = DIP_NEUTRAL;
      (world.np[cntry])->dstatus[country] = DIP_NEUTRAL;
    }
  }
}

/* UPD_CNTREACH -- Count the relocation value of neighboring sectors */
static void
upd_cntreach PARM_2(int, x, int, y)
{
  int d;

  /* count up anything of value */
  if ((d = get_reach(x, y)) > 0) {
    global_int += d;
  }
}

/* holder for the old population location */
long *old_pop = NULL;
int x_focus, y_focus;

/* UPD_MOVEPOP -- Relocate civilians based on sector values */
static void
upd_movepop PARM_2(int, x, int, y)
{
  long templng;
  int d;

  if ((d = get_reach(x, y)) > 0) {

    /* the people migrate */
    templng = (d * OLD_POP(x_focus, y_focus)) / global_int;
    sct_ptr->people -= templng;
    sct[x][y].people += templng;

  }
}

/* WRLD_TOTALS -- Routine to compute all of the world totals */
static void
wrld_totals PARM_0(void)
{
  NTN_PTR n1_ptr;
  int i, cntry;

  /* initialize the totals */
  for (i = 0; i < MTRLS_NUMBER; i++) {
    WORLDMTRLS[i] = 1;
  }
  WORLDSCORE = 1;
  WORLDCIV = 1;
  WORLDSCT = 1;
  WORLDMIL = 1;
  WORLDNTN = 0;

  /* now compute world commodities */
  for (cntry = 1; cntry < MAXNTN; cntry++) {

    /* do we do it */
    if ((n1_ptr =  world.np[cntry]) == NULL) continue;
    if (n_notactive(n1_ptr->active)) continue;

    /* stop here for non-centralized nations */
    if (n_ismonster(n1_ptr->active)) continue;

    /* now summarize everything */
    ntn_totals(cntry);

    /* another nation */
    WORLDNTN++;

    /* all the materials in the world */
    for (i = 0; i < MTRLS_NUMBER; i++) {
      if (n1_ptr->mtrls[i] > (itemtype) 0) {
	if (WORLDMTRLS[i] + n1_ptr->mtrls[i] > BIGITEM) {
	  WORLDMTRLS[i] = BIGITEM;
	} else {
	  WORLDMTRLS[i] += n1_ptr->mtrls[i];
	}
      }
    }

    /* power to the people */
    if (WORLDCIV + n1_ptr->tciv > BIGINT) {
      WORLDCIV = BIGINT;
    } else {
      WORLDCIV += n1_ptr->tciv;
    }

    /* might makes right */
    if (WORLDMIL + n1_ptr->tmil > BIGINT) {
      WORLDMIL = BIGINT;
    } else {
      WORLDMIL += n1_ptr->tmil;
    }

    /* he shoots, he scores!... */
    if (WORLDSCORE + n1_ptr->score > BIGINT) {
      WORLDSCORE = BIGINT;
    } else {
      WORLDSCORE += n1_ptr->score;
    }

    /* I've got some nice beachfront property in Arizona... */
    if (WORLDSCT + n1_ptr->tsctrs > BIGINT) {
      WORLDSCT = BIGINT;
    } else {
      WORLDSCT += n1_ptr->tsctrs;
    }
  }
}

/* UCHECK_TGSETTING -- Is the designation right for the tradegood */
static int
ucheck_tgsetting PARM_2(int, value, int, setting)
{
  /* check it out */
  switch (value) {
  case MAJ_NONE:
    /* must be used */
    if (setting != MAJ_NONE) return(TRUE);
    break;
  case MAJ_FARM:
    /* must be a farming sector */
    if (IS_FARM(setting)) return(TRUE);
    break;
  default:
    /* must be a match or a better supply center */
    if (value == setting) return(TRUE);
    if (IS_CITY(value) &&
	(value < setting)) return(TRUE);
    break;
  }
  return(FALSE);
}

/* UPD_NATIONS -- Now update population and nation statistics */
void
upd_nations PARM_1(int, which)
{
  NTN_PTR nhold_ptr = ntn_ptr;
  int hold_cntry = country, start, end, good, buteval, buteweight;
  register int x, y;
  long cityfolk, townfolk, blksmths, mercs;
  itemtype foodval, mineval, jewelval, itemtemp;
  long temp1, temp2;
  int x_there, schools, roads, churchs;
  int ngrain, ntowns, ncities, d = 0;

  /* set the world statistics */
  if (is_update == TRUE) {
    fprintf(fupdate, "=== Recalculating National Attributes\n");
    world.news_size = (off_t) 0;
  }
  wrld_totals();

  /* set the range of adjustment */
  if (which == UNOWNED) {
    start = 1;
    end = MAXNTN;
  } else {
    start = which;
    end = which + 1;
  }

  /* search over the entire range */
  for (country = start; country < end; country++) {

    /* is there anybody out there */
    if ((ntn_ptr =  world.np[country]) == NULL) continue;
    if (n_notactive(ntn_ptr->active)) {
      fprintf(fnews, "1.\tNation %s has been dissolved.\n",
	      ntn_ptr->name);
      continue;
    }

    /* news watch setting may be cleared during updates */
    if (is_update == TRUE) {
      ntn_ptr->news_size = (off_t) 0;
    }

    /* just don't need recalcs */
    if (n_ismonster(ntn_ptr->active)) continue;

    /* initialize */
#ifdef DEBUG
    fprintf(fupdate, "  Updating nation %s population and statistics\n",
	    ntn_ptr->name);
#endif /* DEBUG */
    old_pop = new_maplong(old_pop);
    cityfolk = townfolk = 0;
    foodval = jewelval = mineval = 0;
    roads = churchs = schools = ngrain = ntowns = ncities = blksmths = 0;
    ntn_ptr->tsctrs = 0;

    /* check for any meeting via leaders */
    for (army_ptr = ntn_ptr->army_list;
	 army_ptr != NULL;
	 army_ptr = army_ptr->next) {
      if (a_isleader(ARMY_TYPE)) {
	map_loop(ARMY_XLOC, ARMY_YLOC, MEETNTN, upd_seenem);
      }
    }

    /* go over all of the nation sectors */
    for (x = ntn_ptr->leftedge; x <= ntn_ptr->rightedge; x++) {
      for (y = ntn_ptr->bottomedge; y <= ntn_ptr->topedge; y++) {

	/* does this nation own it? */
	if ((sct_ptr = &(sct[(x + MAPX) % MAPX][y]))->owner == country) {

	  /* check for newly met nations */
	  if (is_update) {
	    map_loop((x + MAPX) % MAPX, y, MEETNTN, upd_seenem);
	  }

	  /* check the major designations */
	  ntn_ptr->tsctrs++;
	  switch (d = major_desg(sct_ptr->designation)) {
	  case MAJ_CAPITAL:
	    /* count the capital as 3 cities */
	    ncities += 2;
	  case MAJ_CITY:
	    /* add 'em up */
	    cityfolk += sct_ptr->people;
	    ncities++;
	  case MAJ_TOWN:
	    /* townies... */
	    if (d == MAJ_TOWN) {
	      townfolk += sct_ptr->people;
	      ntowns++;
	    }
	  case MAJ_STOCKADE:
	    /* adjust for certain minor designations */
	    if (minor_desg(sct_ptr->designation, MIN_BLACKSMITH)) {
	      blksmths += maj_dinfo[d].multiplier;
	    }
	    if (minor_desg(sct_ptr->designation, MIN_GRANARY)) {
	      ngrain += maj_dinfo[d].multiplier;
	    }
	    break;
	  case MAJ_JEWELMINE:
	    /* mine, mine, all mine */
	    if (tg_ok(sct_ptr, country)) {
	      jewelval += (jewel_value(sct_ptr) * sct_ptr->people);
	    }
	    break;
	  case MAJ_METALMINE:
	    /* hi ho... hi ho... */
	    if (tg_ok(sct_ptr, country)) {
	      mineval += (metal_value(sct_ptr) * sct_ptr->people);
	    }
	    break;
	  case MAJ_FARM:
	  case MAJ_FERTILE:
	  case MAJ_FRUITFUL:
	    /* food food food -- upgrade farms */
	    foodval += d * sct_ptr->people * tofood (sct_ptr, country);
	    if ((TURN % 3) == 0) {
	      switch (SEASON(TURN)) {
	      case SPRING_SEASON:
		/* becomes fertile */
		set_majordesg(sct_ptr->designation, MAJ_FERTILE);
		break;
	      case SUMMER_SEASON:
		/* fertile land becomes fruitful */
		if (d == MAJ_FERTILE) {
		  set_majordesg(sct_ptr->designation, MAJ_FRUITFUL);
		}
		break;
	      case FALL_SEASON:
		/* all become farm */
		set_majordesg(sct_ptr->designation, MAJ_FARM);
		break;
	      case WINTER_SEASON:
		/* just leave it as a farm */
		break;
	      }
	    }
	    break;
	  default:
	    /* dont do anything */
	    break;
	  }

	  /* figure in some other minor designations */
	  if (minor_desg(sct_ptr->designation, MIN_ROADS)) {
	    roads++;
	  }
	  if (minor_desg(sct_ptr->designation, MIN_CHURCH)) {
	    churchs += maj_dinfo[d].multiplier;
	    schools++;
	  }
	  if (minor_desg(sct_ptr->designation, MIN_UNIVERSITY)) {
	    schools += maj_dinfo[d].multiplier;
	  }

	  /* check for population adjustment */
	  if (is_update == TRUE) {
	    if (sct_ptr->people > 0) {

	      /* rabbit test */
	      if (ntn_ptr->repro * (int) GROWTH > 0) {

		/* now check for yearly increase ---
		 *   this might overflow if expected limits are exceeded:
		 *
		 *      people = 60,000
		 *      repro  = 20
		 *      GROWTH = 80
		 */
		temp1 = (sct_ptr->people * ntn_ptr->repro * (int) GROWTH) / 10;

		/* determine monthly total */
		if (temp1 > 1200) {
		  /* definite population increase */
		  sct_ptr->people += temp1 / 1200;
		}
		if (rand_val(1200) < temp1 % 1200) {
		  sct_ptr->people++;
		}
	      }

	      /* record the non-migratory population */
	      OLD_POP((x + MAPX) % MAPX, y) = sct_ptr->people;
	    }
	  }

	}
      }
    }

    /* national attributes */
    if (is_update == TRUE) {

      /* finally, recalculate the new national attributes */

      /* is it still alive? */
      if ((ntn_ptr->city_list == NULL) ||
	  (ntn_ptr->tmil + ntn_ptr->tciv < TAKESECTOR)) {
	/* dead meat */
	fprintf(fnews, "1.\tNation %s has been dissolved.\n",
		ntn_ptr->name);
	ntn_ptr->active = INACTIVE;
	continue;
      }

      /* first clear any powers that need clearing */
      for (x = 0; x < BUTE_NUMBER; x++) {
	if (bute_info[x].clear_it) {
	  ntn_ptr->attribute[x] = bute_info[x].min_base +
	    (bute_info[x].start_val - bute_info[x].min_base) / 2;
	}
      }
      
      /* metal production */
      if (ntn_ptr->tciv > 0) {
	temp1 = (100 * mineval) / m_produced +
	  (20 * (cityfolk + townfolk) / ntn_ptr->tciv) + blksmths / 3;
      } else {
	temp1 = 0;
      }
      if (r_mining(ntn_ptr->race)) {
	temp1 += 5;
      }
      ntn_ptr->attribute[BUTE_MINING] += temp1;

      /* calculate knowledge */
      if (ntn_ptr->tciv > 0) {
	temp1 = (50 * (cityfolk + townfolk / 2) /
		ntn_ptr->tciv) + schools * 2;
	ntn_ptr->attribute[BUTE_KNOWLEDGE] += temp1;
      }

      /* find the eat rate */
      if (ntn_ptr->tciv > 0) {

	/* seasonal calculation */
	adjust_attribute(ntn_ptr->attribute, BUTE_EATRATE, 0);
	switch (SEASON(TURN)) {
	case SPRING_SEASON:
	  /* can eat more now */
	  itemtemp = ((13 + (TURN % 3)) * ntn_ptr->mtrls[MTRLS_FOOD]) /
	    ntn_ptr->attribute[BUTE_EATRATE];
	  break;
	case SUMMER_SEASON:
	  /* may eat well here */
	  itemtemp = ((18 + (TURN % 3)) * ntn_ptr->mtrls[MTRLS_FOOD]) /
	    ntn_ptr->attribute[BUTE_EATRATE];
	  break;
	case FALL_SEASON:
	  /* store for winter */
	  itemtemp = ((10 - (TURN % 3)) * ntn_ptr->mtrls[MTRLS_FOOD]) /
	    ntn_ptr->attribute[BUTE_EATRATE];
	  break;
	case WINTER_SEASON:
	default:
	  /* need more food */
	  itemtemp = ((9 - (TURN % 3)) * ntn_ptr->mtrls[MTRLS_FOOD]) /
	    ntn_ptr->attribute[BUTE_EATRATE];
	  break;
	}

	/* compute it */
	ntn_ptr->attribute[BUTE_EATRATE] =
	  ntn_ptr->attribute[BUTE_EATRATE] / 2 + itemtemp / ntn_ptr->tciv;
	if (CIV_MAGIC(MC_DEMOCRACY))
	  ntn_ptr->attribute[BUTE_EATRATE] += 5;

	/* adjust the score of the nation */
	score_ntn(ntn_ptr);
	if (m_produced > 0) {
	  ntn_ptr->score += (100.0 * mineval) / m_produced;
	}
	if (j_produced > 0) {
	  ntn_ptr->score += (100.0 * jewelval) / j_produced;
	}

      } else {
	ntn_ptr->attribute[BUTE_EATRATE] = bute_info[BUTE_EATRATE].start_val;
      }

      /* determine changes to the currency */
      itemtemp = WORLDMTRLS[MTRLS_JEWELS] / WORLDNTN;
      if (itemtemp * 3 < ntn_ptr->mtrls[MTRLS_JEWELS]) {
	/* nice increase if jewels supply is very good */
	ntn_ptr->attribute[BUTE_CURRENCY] += 3;
      } else if (itemtemp * 2 < ntn_ptr->mtrls[MTRLS_JEWELS]) {
	/* increase if jewels supply is good */
	ntn_ptr->attribute[BUTE_CURRENCY] += 2;
      } else if (itemtemp / 4 > ntn_ptr->mtrls[MTRLS_JEWELS]) {
	/* fair decrease if jewels supply is bad */
	ntn_ptr->attribute[BUTE_CURRENCY] -= 3;
      } else if (itemtemp / 2 > ntn_ptr->mtrls[MTRLS_JEWELS]) {
	/* medium decrease if jewels supply is off */
	ntn_ptr->attribute[BUTE_CURRENCY] -= 2;
      } else if (itemtemp > ntn_ptr->mtrls[MTRLS_JEWELS] + (itemtype)5000) {
	/* minor decrease for below average */
	ntn_ptr->attribute[BUTE_CURRENCY] -= 1;
      }

    }

    /* find communication range */
    if ((ntn_ptr->tciv > 0) &&
	(ntn_ptr->tsctrs > 0)) {
      temp1 = ((30 * (cityfolk + townfolk / 2)) / ntn_ptr->tciv) +
	((roads + ncities + ntowns / 2) * 154) / (ntn_ptr->tsctrs * 5) +
	  ncities + ntowns / 2;
    } else {
      temp1 = 0;
    }
    ntn_ptr->attribute[BUTE_COMMRANGE] = temp1;

    /* now perform increments based on the magical powers */
    for (x = 0; x < MAG_NUMBER; x++) {
      MAGIC_PTR mg_ptr = mclass_list[x].pow_list;
      int temp;
      for (y = 0; y < mclass_list[x].maxval; y++) {
	if (((temp = mg_ptr[y].bute_num) != BUTE_NUMBER) &&
	    (MAGIC(ntn_ptr->powers[x], 1L << y))) {
	  ntn_ptr->attribute[temp] += mg_ptr[y].bute_val;
	}
      }
    }

    /* get number of mercenaries and compute terror effect */
    mercs = 0;
    for (army_ptr = ntn_ptr->army_list;
	 army_ptr != NULL;
	 army_ptr = army_ptr->next) {
      if (a_ismerc(ARMY_TYPE)) mercs += ARMY_SIZE;
    }
    if (ntn_ptr->tmil > 0 && ntn_ptr->tciv > 0) {
      temp1 = (1000 * ntn_ptr->tmil) / ntn_ptr->tciv +
	(1000 * mercs) / ntn_ptr->tmil;
    } else temp1 = 0;
    ntn_ptr->attribute[BUTE_TERROR] += temp1 / 5;

    /* check for increase in charity */
    if (CIV_MAGIC(MC_DEMOCRACY)) {
      if (ntn_ptr->attribute[BUTE_CHARITY] < 15) {
	/* it creeps up until over 14 */
	ntn_ptr->attribute[BUTE_CHARITY]++;
      }
    }

    /* now adjust the national spell casting ability */
    ntn_ptr->attribute[BUTE_SPELLPTS] +=
      num_bits_on(ntn_ptr->powers[MAG_WIZARDRY]);
    if (r_antimagic(ntn_ptr->race)) {
      ntn_ptr->attribute[BUTE_SPELLPTS] /= 2;
    } else if (r_wizardly(ntn_ptr->race)) {
      ntn_ptr->attribute[BUTE_SPELLPTS] *= 3;
      ntn_ptr->attribute[BUTE_SPELLPTS] /= 2;
    }

    /* now allow for reproduction and relocating */
    if (is_update == TRUE) {
      set_weights(FALSE);
      for (x = ntn_ptr->leftedge; x <= ntn_ptr->rightedge; x++) {
	for (y = ntn_ptr->bottomedge; y <= ntn_ptr->topedge; y++) {

	  /* should it be parsed? */
	  sct_ptr = &(sct[(x + MAPX) % MAPX][y]);
	  if (sct_ptr->owner == country) {

	    /* handle the tradegood additions */
	    good = sct_ptr->tradegood;
	    d = major_desg(sct_ptr->designation);
	    if (ucheck_tgsetting(tg_info[good].need_desg, d)) {

	      /* find the proper attribute */
	      buteval = tg_info[good].class;
	      buteweight = tgclass_info[buteval].weighting;
	      buteval = tgclass_info[buteval].attribute;

	      /* is this an incrementor with enough people? */
	      if ((buteval != BUTE_NUMBER) &&
		  (sct_ptr->people >= tg_info[good].pop_support)) {
		/* then adjust the value */
		ntn_ptr->attribute[buteval] +=
		  bute_info[buteval].modifier *
		    ((tg_info[good].value + buteweight / 2) / buteweight);
	      }

	    }
	  
	    if ((!minor_desg(sct_ptr->designation, MIN_SIEGED)) &&
		(OLD_POP((x + MAPX) % MAPX, y) > 0)) {

	      /* build the distribution matrix */
	      x_there = (x + MAPX) % MAPX;
	      set_relocation(x_there, y);

	      /* check if a relocation is possible */
	      if ((temp1 = get_reach(x_there, y)) >= 0) {

		/* first perform a count */
		global_int = 0;
		map_loop(x_there, y, 2, upd_cntreach);
		temp2 = global_int;

		/* now redistribute */
		if (temp2 > temp1) {

		  /* weight towards not moving */
		  temp1 *= 2;

		  /* total weighting is in global_int */
		  global_int = temp1 + temp2;
		  x_focus = x_there;
		  y_focus = y;
		  map_loop(x_focus, y_focus, 2, upd_movepop);

		}

	      }

	    }

	  }

	}
      }
    }

    /* reallign all of the attributes */
    for (x = 0; x < BUTE_NUMBER; x++) {
      adjust_attribute(ntn_ptr->attribute, x, 0);
    }
  }

  /* now do the final power calculations, if needed */
  if (is_update == TRUE) {
    wrld_totals();
    for (country = start; country < end; country++) {

      /* is there anybody out there */
      if ((ntn_ptr =  world.np[country]) == NULL) continue;
      if (n_notactive(ntn_ptr->active)) continue;

      /* news watch setting may be cleared during updates */
      ntn_ptr->news_size = (off_t) 0;

    }
  }

  /* be sure to reset the proper country */
  ntn_ptr = nhold_ptr;
  country = hold_cntry;
}

/* UPDATE -- Go through the entire updating routine list */
void
update PARM_0(void)
{
  /* initialize everything */
  upd_init();

  /* read in all user movement and handle npc updates */
  upd_input();

  /* handle any spell events */
  upd_spells();

  /* handle the roving army units */
  upd_rovers();

  /* resolve all military encounters */
  combat();

  /* check sieges, blockades and ownership changes */
  upd_capture();

  /* now set sector statuses */
  upd_sectors();

  /* handle any weather or random events */
  /* upd_events(); : UNIMPLEMENTED */

  /* go through the nation productions */
  upd_produce();

  /* find out how much gets taken */
  upd_consume();

  /* adjust all of the army, navy and caravan statuses */
  upd_military(UNOWNED);

  /* eliminate dead nations, recalculate attributes */
  upd_nations(UNOWNED);

  /* close things up */
  upd_finish();
}
