/* file containing code specific to sector calculations for conqrun */
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
#include "magicX.h"
#include "racesX.h"
#include "desigX.h"
#include "activeX.h"
#include "statusX.h"
#include "dstatusX.h"

/* Military Sector Summary Information */
typedef struct s_milsum {
  ntntype owner;		/* who owns the units */
  long size;			/* combined strength of units */
  struct s_milsum *next;
} MILSUM_STRUCT, *MILSUM_PTR;

typedef struct s_sctsum {
  maptype xloc, yloc;		/* the sector being summarized */
  ntntype owner;		/* the owner of the sector */
  MILSUM_STRUCT hold_troops;	/* troops preventing sector capture/siege */
  MILSUM_STRUCT siege_troops;	/* troops putting the sector under siege */
  MILSUM_PTR take_troops;	/* troops trying to capture the sector */
  MILSUM_PTR other_troops;	/* any other troops */
  struct s_sctsum *next;
} SCTSUM_STRUCT, *SCTSUM_PTR;

/* CRT_MILSUM -- Allocate the memory for a military summary */
static MILSUM_PTR
crt_milsum PARM_1(int, who)
{
  MILSUM_PTR MS_ptr;

  /* get the memory */
  if ((MS_ptr = (MILSUM_PTR) malloc(sizeof(MILSUM_STRUCT))) == NULL) {
    fprintf(fupdate, "Memory allocation error\n");
    abrt();
  }

  /* now initialize values */
  MS_ptr->owner = who;
  MS_ptr->size = 0;
  MS_ptr->next = NULL;
  return(MS_ptr);
}

/* CRT_SCTSUM -- Build a sector summary structure, properly initialized */
static SCTSUM_PTR
crt_sctsum PARM_2(int, x, int, y)
{
  SCTSUM_PTR SS_ptr;

  /* get the memory */
  if ((SS_ptr = (SCTSUM_PTR) malloc(sizeof(SCTSUM_STRUCT))) == NULL) {
    fprintf(fupdate, "Memory allocation error\n");
    abrt();
  }

  /* now initialize values */
  SS_ptr->xloc = x;
  SS_ptr->yloc = y;
  SS_ptr->owner = UNOWNED;
  SS_ptr->siege_troops.owner = UNOWNED;
  SS_ptr->siege_troops.size = 0L;
  SS_ptr->siege_troops.next = NULL;
  SS_ptr->hold_troops.owner = UNOWNED;
  SS_ptr->hold_troops.size = 0L;
  SS_ptr->hold_troops.next = NULL;
  SS_ptr->take_troops = NULL;
  SS_ptr->other_troops = NULL;
  SS_ptr->next = NULL;
  return(SS_ptr);
}

/* SCTSUM_BYLOC -- Retrieve a sector summary by its location */
static SCTSUM_PTR
sctsum_byloc PARM_3(SCTSUM_PTR, SS_ptr, int, x, int, y)
{
  /* take advantage of C parameter passing */
  for (; SS_ptr != NULL; SS_ptr = SS_ptr->next) {
    if ((SS_ptr->xloc == x) && (SS_ptr->yloc == y)) break;
  }
  return(SS_ptr);
}

/* MILSUM_BYOWNER -- Retrieve the military sumary by owner */
static MILSUM_PTR
milsum_byowner PARM_2(MILSUM_PTR, MS_ptr, int, ntnnum)
{
  /* take advantage of C parameter passing */
  for (; MS_ptr != NULL; MS_ptr = MS_ptr->next) {
    if (MS_ptr->owner == ntnnum) break;
  }
  return(MS_ptr);
}

/* SORT_MILSUM -- Sort a military summary by size */
static MILSUM_PTR
sort_milsum PARM_1(MILSUM_PTR, MS_list)
{
  MILSUM_PTR first_ptr, travel_ptr, hold_ptr;

  /* check input */
  if ((MS_list == NULL) || (MS_list->next == NULL)) {
    return(MS_list);
  }

  /* now go through the entire list */
  hold_ptr = first_ptr = MS_list;
  MS_list = MS_list->next;
  first_ptr->next = NULL;
  for (travel_ptr = MS_list; MS_list != NULL; travel_ptr = MS_list) {

    /* compare with the beginning of the list */
    if (travel_ptr->size > first_ptr->size) {

      /* add it to the beginning of the list */
      MS_list = MS_list->next;
      travel_ptr->next = first_ptr;
      first_ptr = travel_ptr;

    } else {

      /* find where to put it */
      for (hold_ptr = first_ptr;
	   hold_ptr->next != NULL;
	   hold_ptr = hold_ptr->next) {
	if (travel_ptr->size > hold_ptr->next->size) break;
      }

      /* add it here */
      MS_list = MS_list->next;
      travel_ptr->next = hold_ptr->next;
      hold_ptr->next = travel_ptr;
    }
  }

  /* now give back the new head of the list */
  return(first_ptr);
}

/* SCTSUM_FREE -- Free up a sector summary storage class */
static void
sctsum_free PARM_1(SCTSUM_PTR, SS_ptr)
{
  MILSUM_PTR MS1_ptr, MS2_ptr;

  /* first free up all of the capturing troops */
  for (MS1_ptr = SS_ptr->take_troops;
       MS1_ptr != NULL; MS1_ptr = MS2_ptr) {

    /* first keep track of next storage */
    MS2_ptr = MS1_ptr->next;
    MS1_ptr->next = NULL;

    /* now nuke it */
    free(MS1_ptr);
  }

  /* free up the list of siegers */
  for (MS1_ptr = SS_ptr->siege_troops.next;
       MS1_ptr != NULL; MS1_ptr = MS2_ptr) {

    /* first keep track of next storage */
    MS2_ptr = MS1_ptr->next;
    MS1_ptr->next = NULL;

    /* now nuke it */
    free(MS1_ptr);
  }

  /* now free up all of the remaining troops */
  for (MS1_ptr = SS_ptr->other_troops;
       MS1_ptr != NULL; MS1_ptr = MS2_ptr) {

    /* first keep track of next storage */
    MS2_ptr = MS1_ptr->next;
    MS1_ptr->next = NULL;

    /* now nuke it */
    free(MS1_ptr);
  }

  /* finally clear out the main storage */
  SS_ptr->next = NULL;
  free(SS_ptr);
}

/* MEN_TO_CAPTURE -- Returns number of men needed to take a sector */
long
men_to_capture PARM_3(int, xloc, int, yloc, ntntype, by)
{
  long hold = TAKESECTOR;
  int pval = BASE_TAKEPCT;
  NTN_PTR n1_ptr, n2_ptr;

  /* check the input */
  if (!XY_ONMAP(xloc, yloc) || ((n2_ptr = world.np[by]) == NULL))
    return(hold);

  /* now find any neccesary adjustments */
  sct_ptr = &(sct[xloc][yloc]);
  if ((sct_ptr->owner == UNOWNED) ||
      (sct_ptr->people == 0) ||
      ((n1_ptr = world.np[sct_ptr->owner]) == NULL)) {
    return(hold);
  }

  /* determine base value for designation */
  switch (major_desg(sct_ptr->designation)) {
  case MAJ_CAPITAL:
    /* the people live for the govt.; politicians */
    pval *= 3;
    break;
  case MAJ_CITY:
    /* tougher people live in the city */
    pval *= 2;
    break;
  case MAJ_TOWN:
  case MAJ_STOCKADE:
    /* 1 & 1/2 times normal capture cost */
    pval *= 3;
    pval /= 2;
    break;
  case MAJ_CANAL:
    /* doubled value since the canal crews are mostly govt. employees */
    pval *= 2;
  default:
    /* check for slight increase from fortifications */
    if (minor_desg(sct_ptr->designation, MIN_FORTIFIED)) {
      /* 20% increase */
      pval *= 5;
      pval /= 4;
    }
    break;
  }

  /* now find racial and diplomatic adjustments */
  pval *= race_info[n2_ptr->race].take_value[n1_ptr->race];
  pval /= 100;
  if (n1_ptr->dstatus[by] == DIP_JIHAD) {
    pval *= 110;
    pval /= 100;
  }

  /* now add in the population adjustment */
  hold += (sct_ptr->people * pval) / 100;
  return(hold);
}

/* CAPTURE_LAND -- Transfer ownership and cause fleeing */
void
capture_land PARM_3(ntntype, whom, int, xloc, int, yloc)
{
  NTN_PTR n1_ptr, n2_ptr, nhold_ptr = ntn_ptr;
  long num_kill, num_people;
  int i, pval, align_adj, nhold_cntry = country;

  /* check input */
  if (!XY_ONMAP(xloc, yloc)) return;
  if (whom >= ABSMAXNTN) return;

  /* get quickie information */
  sct_ptr = &(sct[xloc][yloc]);
  ntn_ptr = n2_ptr = world.np[sct_ptr->owner];
  country = sct_ptr->owner;

  /* check unowned */
  if ((whom == UNOWNED) ||
      ((n1_ptr = world.np[whom]) == NULL)) {

    /* nullify everything */
    sct_ptr->owner = UNOWNED;
    sct_ptr->people = 0;
    sct_ptr->designation = MAJ_NONE;

    /* eliminate any cities encountered */
    if (IS_CITY(sct_ptr->designation) &&
	((city_ptr = citybyloc(ntn_ptr, xloc, yloc)) != NULL)) {
      dest_city(CITY_NAME);
    }

  } else {

    /* give to somebody */
    if ((n2_ptr == NULL) ||
	((sct_ptr->people == 0) &&
	 !IS_CITY(sct_ptr->designation))) {

      /* just adjust the ownership */
      sct_ptr->owner = whom;

    } else {

      /* hold how many are in the sector */
      num_people = sct_ptr->people;

      /* find adjustment due to alignment */
      align_adj = 5 * (abs(n_alignment(n1_ptr->active) -
			   n_alignment(n2_ptr->active)) - 1);

      /* factor in the deaths */
      pval = race_info[n1_ptr->race].kill_pop[n2_ptr->race];
      pval -= align_adj;
      if (MAGIC(n1_ptr->powers[MAG_CIVILIAN], MC_SLAVER)) {
	pval *= 4;
	pval /= 5;
      }
      num_people *= pval;
      num_people /= 100;

      /* now factor in the amount kept in the land */
      num_kill = num_people;
      num_people = sct_ptr->people;
      pval = race_info[n1_ptr->race].keep_pop[n2_ptr->race];
      pval += align_adj;
      if (MAGIC(n1_ptr->powers[MAG_CIVILIAN], MC_SLAVER)) {
	pval *= 3;
	pval /= 2;
	if (pval > 100) {
	  pval = 100;
	}
      }
      num_people *= pval;
      num_people /= 100;

      /* cause the fleeing of the remainder */
      country = whom;
      flee_people(sct_ptr->people - num_people - num_kill, xloc, yloc);

      /* figure the final totals */
      sct_ptr->people -= num_kill;
      num_people = sct_ptr->people;

      /* check for any damage */
      if (num_kill > num_people / 2) {
	switch (major_desg(sct_ptr->designation)) {
	case MAJ_FARM:
	case MAJ_FERTILE:
	case MAJ_FRUITFUL:
	  /* knocks out the farm sector and causes devastation */
	  set_majordesg(sct_ptr->designation, MAJ_NONE);
	default:
	  /* devastation */
	  set_minordesg(sct_ptr->designation, MIN_DEVASTATED);
	  break;
	}
      }

      /* now change the owner */
      sct_ptr->owner = whom;
      if (IS_CITY(sct_ptr->designation)) {

	/* transfer the city */
	if ((city_ptr = citybyloc(ntn_ptr, xloc, yloc)) == NULL) {
	  set_majordesg(sct_ptr->designation, MAJ_NONE);
	} else {

	  /* copy it */
	  city_tptr = new_city();
	  strcpy(CITYT_NAME, CITY_NAME);
	  CITYT_XLOC = CITY_XLOC;
	  CITYT_YLOC = CITY_YLOC;
	  CITYT_PEOPLE = CITY_PEOPLE;
	  CITYT_FORT = CITY_FORT;
	  CITYT_WEIGHT = CITY_WEIGHT;
	  CITYT_STALONS = CITY_STALONS;
	  for (i = 0; i < MTRLS_NUMBER; i++) {
	    if (CITY_MTRLS[i] > (itemtype) 0) {
	      CITYT_MTRLS[i] = CITY_MTRLS[i];
	    } else {
	      CITYT_MTRLS[i] = (itemtype) 0;
	    }
	    if (CITY_IMTRLS[i] > (itemtype) 0) {
	      CITYT_IMTRLS[i] = CITY_IMTRLS[i];
	    } else {
	      CITYT_IMTRLS[i] = (itemtype) 0;
	    }
	  }

	  /* get rid of original */
	  sprintf(string, "3.\tNation %s sacks %s %s in Nation %s\n",
		  n1_ptr->name,
		  maj_dinfo[major_desg(sct_ptr->designation)].name,
		  CITY_NAME,
		  n2_ptr->name);
	  sorted_news(whom, string);
	  dest_city(CITY_NAME);

	  /* check for a capital */
	  if (major_desg(sct_ptr->designation) == MAJ_CAPITAL) {
	    n1_ptr->aplus += 2;
	    set_majordesg(sct_ptr->designation, MAJ_CITY);
	  }
	  if (major_desg(sct_ptr->designation) > MAJ_STOCKADE) {
	    n1_ptr->aplus ++;
	    n1_ptr->dplus ++;
	  }
	  if (major_desg(sct_ptr->designation) != MAJ_CACHE) {
	    n1_ptr->aplus ++;
	  }

	  /* now add it to the other nation */
	  ntn_ptr = n1_ptr;
	  while (citybyname(CITYT_NAME) != NULL) {
	    /* adjust names */
	    do {
	      city_tptr->name[0] = 'A' + rand_val('Z' - 'A');
	    } while (!isprint(city_tptr->name[0]));
	  }

	  /* transfer it */
	  city_tptr->next = ntn_ptr->city_list;
	  ntn_ptr->city_list = city_tptr;
	  city_sort();

	}
      }
    }
  }

  /* reset values */
  ntn_ptr = nhold_ptr;
  country = nhold_cntry;
}

/* data on sector accessibility */
static char reach_init = FALSE;
static int reach_xloc, reach_yloc, reach_limit, reach_target;
static int reach_val[11][11];
static char reach_visit[11][11];

/* ALIGN_XDIFF -- align the x differential if along edges */
static int
align_xdiff PARM_1(int, xd_val)
{
  /* check for edge wrapping */
  if (abs(xd_val) > MAPX - abs(xd_val)) {
    if (xd_val <= 0) {
      xd_val = MAPX - abs(xd_val);
    } else {
      xd_val = abs(xd_val) - MAPX;
    }
  }
  return(xd_val);
}

/* NEXT_CHECKP -- Call the next check element */
static void
next_checkp PARM_2(int, xloc, int, yloc)
{
  int xspot, yspot, xdiff, sown;

  /* check the range */
  if (!map_within(reach_xloc, reach_yloc, xloc, yloc, reach_limit)) {
    return;
  }

  /* set indicators */
  xdiff = align_xdiff(xloc - reach_xloc);
  xspot = xdiff + reach_limit;
  yspot = yloc - reach_yloc + reach_limit;

  /* have we been here before? */
  if (reach_visit[xspot][yspot]) return;

  /* must we stop with this visit? */
  reach_visit[xspot][yspot] = 1;
  if (tofood(&(sct[xloc][yloc]), country) == 0) return;

  /* set the information */
  sown = sct[xloc][yloc].owner;
  if ((reach_target == -1) &&
      (ntn_ptr != NULL)) {
    /* check for "MV_OTHNATION" restriction */
    int dstat = ntn_ptr->dstatus[sown];
    if ((sown != country) &&
	(sown != UNOWNED) &&
	(dstat != DIP_ALLIED)) return;
    reach_val[xspot][yspot] = sown;
  } else {
    reach_val[xspot][yspot] = sown;
    if (reach_val[xspot][yspot] == reach_target)
      reach_val[xspot][yspot] = MAXNTN;
  }

  /* now play with recursion */
  map_loop(xloc, yloc, 1, next_checkp);
}

/* SET_REACH -- Set the reach values around the given location */
void
set_reach PARM_3(int, xloc, int, yloc, int, value)
{
  register int i, j;

  /* it has a proper setting */
  reach_init = TRUE;
  reach_xloc = xloc;
  reach_yloc = yloc;
  reach_limit = 2;
  reach_target = value;

  /* initialize entire settings */
  for (i = 0; i < 11; i++) {
    for (j = 0; j < 11; j++) {
      reach_val[i][j] = -1;
      reach_visit[i][j] = 0;
    }
  }

  /* start it */
  next_checkp(xloc, yloc);
}

/* GET_REACH -- Is is within reach? */
int
get_reach PARM_2(int, xloc, int, yloc)
{
  int xdiff;

  /* invalid input */
  if ((reach_init == FALSE) ||
      !Y_ONMAP(yloc) ||
      !map_within(reach_xloc, reach_yloc, xloc, yloc, reach_limit)) {
    return(-1);
  }

  /* give the result */
  xdiff = align_xdiff(xloc - reach_xloc);
  return(reach_val[xdiff + reach_limit]
	 [yloc - reach_yloc + reach_limit]);
}

/* CHANGE_REACH -- Adjust the given value */
void
change_reach PARM_3(int, xloc, int, yloc, int, value)
{
  int xind, yind, xdiff;

  /* invalid input */
  if (reach_init == FALSE) {
    fprintf(fupdate, "Warning: change_reach()... changing unitialized value\n");
    return;
  }
  if (!map_within(reach_xloc, reach_yloc, xloc, yloc, reach_limit)) {
    fprintf(fupdate, "Warning: change_reach()... sector out of range\n");
    return;
  }

  /* now find the position */
  xdiff = align_xdiff(xloc - reach_xloc);
  xind = xdiff + reach_limit;
  yind = yloc - reach_yloc + reach_limit;
  if (reach_val[xind][yind] == -1) {
    fprintf(fupdate, "Warning: change_reach()... adjusting unset value\n");
  }
  reach_val[xind][yind] = value;
}

/* ADJ_RELOC -- Adjust the reach settings for relocation */
static void
adj_reloc PARM_2(int, x, int, y)
{
  int rval;

  if ((rval = get_reach(x, y)) != -1) {
    if (rval == country) {
      change_reach(x, y, attract_val(x, y));
    } else {
      change_reach(x, y, -1);
    }
  }
}

/* SET_RELOCATION -- Build reach settings for relocation */
void
set_relocation PARM_2(int, xloc, int, yloc)
{
  /* initialize what is reachable */
  set_reach(xloc, yloc, -1);

  /* reassign values */
  map_loop(xloc, yloc, 2, adj_reloc);
}

/* FLEE_FIND -- Find allies / friends to run to */
static void
flee_find PARM_2(int, x, int, y)
{
  NTN_PTR n1_ptr;
  int who;

  /* check input */
  who = get_reach(x, y);
  if ((who < 0) ||
      (who > ABSMAXNTN)) return;

  /* find allies and ourselves */
  if ((who == country) ||
      (((n1_ptr = world.np[who]) != NULL) &&
       (n1_ptr->dstatus[country] == DIP_ALLIED))) {
    global_int++;
  } else if ((n1_ptr != NULL) &&
	     (n1_ptr->dstatus[country] <= DIP_NEUTRAL) &&
	     (n1_ptr->dstatus[country] != DIP_UNMET)) {
    global_long++;
  }
}

/* FLEE_RUN -- Actually run away and hide */
static void
flee_run PARM_2(int, x, int, y)
{
  NTN_PTR n1_ptr;
  int who;

  /* get the value */
  who = get_reach(x, y);
  if ((who < 0) || (who >= ABSMAXNTN)) {
    n1_ptr = NULL;
  } else {
    n1_ptr = world.np[who];
  }

  /* people... get your people, here! */
  if ((global_int == 0) &&
      ((who == country) ||
       ((n1_ptr != NULL) &&
	(n1_ptr->dstatus[country] == DIP_ALLIED)))) {
    /* distribute to self and allies */
    sct[x][y].people += global_long;
  } else if ((global_int == 1) && 
	     (n1_ptr != NULL) &&
	     (n1_ptr->dstatus[country] <= DIP_NEUTRAL) &&
	     (n1_ptr->dstatus[country] != DIP_UNMET)) {
    /* distribute to "just" friends */
    sct[x][y].people += global_long;
  }

}

/* FLEE_PEOPLE -- Cause N people to run away and hide */
void
flee_people PARM_3(long, amount, int, xloc, int, yloc)
{
  int found, friends, ontnnum, mode = 0;

  /* check input */
  if (!XY_ONMAP(xloc, yloc))
    return;
  if (amount > sct[xloc][yloc].people) {
    amount = sct[xloc][yloc].people;
  }
  if (amount <= 0) return;

  /* check ownership and sizing */
  if (((global_int = sct[xloc][yloc].owner) == UNOWNED) ||
      (world.np[global_int] == NULL)) {
    /* no need to go on */
    return;
  }
  ontnnum = country;
  country = global_int;

  /* find possible sectors within range */
  set_reach(xloc, yloc, -2);
  if (get_reach(xloc, yloc) != -1) {
    change_reach(xloc, yloc, -1);
  }

  /* distribute the people */
  global_int = 0;
  global_long = 0;
  map_loop(xloc, yloc, 2, flee_find);

  /* check if only friendly land is within range */
  found = global_int;
  friends = global_long;
  if (found == 0) {
    if (friends > 0) {
      found = friends;
      mode = 1;
    } else {
      mode = 2;
    }
  }

  /* set the distribution */
  if (mode != 2) {

    /* how many per sector? */
    global_long = amount / found;
    global_int = mode;

    /* go to it */
    if (global_long > 0) {
      map_loop(xloc, yloc, 2, flee_run);
    }

    /* they have flown the coop */
    sct[xloc][yloc].people -= amount;

  } else {

    /* kill a quarter of those trying to flee */
    sct[xloc][yloc].people -= amount / 4;

  }
  country = ontnnum;
}

/* UPD_CAPTURE -- Compute change is ownership as well as sieges */
void
upd_capture PARM_0(void)
{
  /* This routine is much more rugged and time consuming than
     prior methods.  It involves two passes:
        1. traverse all army units in all nations, gathering
           the data into a summary on a per sector basis.
        2. go through all summarized sectors, determining
           whether a siege or ownership change takes place.
   */
  SCTSUM_PTR SS_list = NULL, SS_ptr = NULL, SS_nptr;
  MILSUM_PTR MS_ptr = NULL;
  ntntype ntnnum;
  int xspot, yspot, dstat, ustat, owner_change;
  long total_troops, thold_troops;

  /* initialize the sector adjustment message */
  msg_ginit("Sector Ownership Report");
  fprintf(fupdate, "=== Checking Troops for Captures and Sieges\n");

  /* go through all nations, accumulating the data */
  for (country = 1; country < MAXNTN; country++) {

    /* check it out */
    if ((ntn_ptr = world.np[country]) == NULL) continue;
    if (n_notactive(ntn_ptr->active)) continue;

    /* go through all armies in the nation */
    for (army_ptr = ntn_ptr->army_list;
	 army_ptr != NULL;
	 army_ptr = army_ptr->next) {

      /* check the location */
      if (!XY_INMAP(ARMY_XLOC, ARMY_YLOC)) continue;
      if (ARMY_SIZE <= 0) continue;

      /* find the size */
      if ((total_troops = army_captsize(army_ptr, TRUE)) > 0) {

	/* obtain the sector summary */
	if ((SS_ptr == NULL) ||
	    (SS_ptr->xloc != ARMY_XLOC) ||
	    (SS_ptr->yloc != ARMY_YLOC)) {

	  if ((SS_ptr =
	       sctsum_byloc(SS_list, ARMY_XLOC, ARMY_YLOC)) == NULL) {
	    /* need a new one */
	    SS_ptr = crt_sctsum(ARMY_XLOC, ARMY_YLOC);
	    SS_ptr->owner = sct[ARMY_XLOC][ARMY_YLOC].owner;
	    if (world.np[SS_ptr->owner] == NULL) {
	      /* check for errors */
	      SS_ptr->owner = UNOWNED;
	    }

	    /* now add to the list */
	    if (SS_list != NULL) {
	      SS_ptr->next = SS_list;
	    }
	    SS_list = SS_ptr;
	  }

	}

	/* now obtain the proper military summary; if needed */
	dstat = ntn_ptr->dstatus[SS_ptr->owner];
	ustat = real_stat(army_ptr);
	if (fort_stat(ustat)) {

	  /* just add into the protectors */
	  SS_ptr->hold_troops.size += total_troops;

	} else if (capture_stat(ustat) &&
		   ((SS_ptr->owner == UNOWNED) ||
		    (dstat >= DIP_WAR))) {

	  /* find the military summary */
	  if ((MS_ptr =
	       milsum_byowner(SS_ptr->take_troops, country)) == NULL) {
	    /* need a new one */
	    MS_ptr = crt_milsum(country);
	    if (SS_ptr->take_troops != NULL) {
	      MS_ptr->next = SS_ptr->take_troops;
	    }
	    SS_ptr->take_troops = MS_ptr;
	  }

	  /* now add it in */
	  MS_ptr->size += total_troops;

	  /* check for sieges */
	  if (unit_status(ARMY_STAT) == ST_SIEGE) {
	    SS_ptr->siege_troops.size += total_troops;
	    if (SS_ptr->siege_troops.owner == UNOWNED) {
	      SS_ptr->siege_troops.owner = country;
	    } else if ((SS_ptr->siege_troops.owner != country) &&
		       (milsum_byowner(SS_ptr->siege_troops.next,
				       country)) == NULL) {
	      MS_ptr = crt_milsum(country);
	      if (SS_ptr->siege_troops.next != NULL) {
		MS_ptr->next = SS_ptr->siege_troops.next;
	      }
	      SS_ptr->siege_troops.next = MS_ptr;
	    }
	  }

	} else {

	  /* find the military summary for others */
	  if ((MS_ptr =
	       milsum_byowner(SS_ptr->other_troops, country)) == NULL) {
	    /* need a new one */
	    MS_ptr = crt_milsum(country);
	    if (SS_ptr->other_troops != NULL) {
	      MS_ptr->next = SS_ptr->other_troops;
	    }
	    SS_ptr->other_troops = MS_ptr;
	  }

	  /* now add it in */
	  MS_ptr->size += total_troops;

	}

      }

    }

  }

  /* now go through all of the sector summaries */
  for (SS_ptr = SS_list;
       SS_ptr != NULL;
       SS_ptr = SS_nptr) {

    /* get sector information */
    if (!XY_INMAP(SS_ptr->xloc, SS_ptr->yloc))
      goto free_it_up;

    /* keep quick reference for sector */
    sct_ptr = &(sct[xspot = SS_ptr->xloc][yspot = SS_ptr->yloc]);
    country = SS_ptr->owner;
    ntn_ptr = world.np[country];

    /* must sort the list of takers */
    SS_ptr->take_troops = sort_milsum(SS_ptr->take_troops);

    /* first determine ownership adjustment */
    owner_change = FALSE;
    if ((SS_ptr->take_troops != NULL) &&
	(SS_ptr->hold_troops.size == 0)) {

      /* there is a possible sector takeover */
      MS_ptr = SS_ptr->take_troops;
      total_troops = MS_ptr->size;
      ntnnum = MS_ptr->owner;
      thold_troops = SS_ptr->hold_troops.size;

      /* check for additional support on both sides */
      for (MS_ptr = MS_ptr->next;
	   MS_ptr != NULL;
	   MS_ptr = MS_ptr->next) {
	if ((ntn_tptr = world.np[MS_ptr->owner])
	    ->dstatus[ntnnum] >= DIP_BELLICOSE) {
	  /* prevents sector capturing */
	  thold_troops += MS_ptr->size;
	} else if ((ntn_tptr->dstatus[ntnnum] <= DIP_NEUTRAL) &&
		   (ntn_tptr->dstatus[ntnnum] != DIP_UNMET)) {
	  /* aids in the capture */
	  total_troops += MS_ptr->size;
	}
      }

      /* now examine remaining troops for help or hurt */
      for (MS_ptr = SS_ptr->other_troops;
	   MS_ptr != NULL;
	   MS_ptr = MS_ptr->next) {
	if ((ntn_tptr = world.np[MS_ptr->owner])
	    ->dstatus[ntnnum] >= DIP_BELLICOSE) {
	  /* prevents sector capturing */
	  thold_troops += MS_ptr->size;
	} else if (((ntn_tptr->dstatus[ntnnum] == DIP_ALLIED) ||
		    (ntn_tptr->dstatus[ntnnum] == DIP_TREATY)) &&
		   (ntn_tptr->dstatus[country] >= DIP_NEUTRAL)) {
	  /* aids in the capture */
	  total_troops += MS_ptr->size;
	}
      }

      /* compare sizing */
      if (total_troops > TAKE_RATIO * thold_troops) {

	/* overpowered enemy troops */
	int desg_val = major_desg(sct[xspot][yspot].designation);

	MS_ptr = SS_ptr->take_troops;
	ntn_tptr = world.np[MS_ptr->owner];
	if (total_troops - thold_troops >=
	    men_to_capture(xspot, yspot, MS_ptr->owner)) {

	  /* Sector Is Taken */
	  if ((country == UNOWNED) || (ntn_ptr == NULL)) {
	    sprintf(string, "3.\tNation %s claims unowned land\n",
		    ntn_tptr->name);
	    sorted_news(MS_ptr->owner, string);
	    sprintf(string, "unowned land claimed as own");
	  } else {
	    sprintf(string, "3.\tNation %s captures land from %s\n",
		    ntn_tptr->name, ntn_ptr->name);
	    sorted_news(MS_ptr->owner, string);
	    sprintf(string, "your %s is taken by nation %s",
		    (desg_val == MAJ_NONE) ? "land" :
		    maj_dinfo[desg_val].name,
		    ntn_tptr->name);
	    msg_grouped(SS_ptr->owner, xspot, yspot, string);
	    sprintf(string, "%s captured from nation %s",
		    (desg_val == MAJ_NONE) ? "land" :
		    maj_dinfo[desg_val].name,
		    ntn_ptr->name);
	  }

	  /* inform the victor */
	  msg_grouped(MS_ptr->owner, xspot, yspot, string);
	  capture_land(MS_ptr->owner, xspot, yspot);

	} else if (MS_ptr->size >= TAKESECTOR) {

	  /* Populace too much */
	  sprintf(string, "your %s resists capture by nation %s",
		  (desg_val == MAJ_NONE) ? "sector" :
		  maj_dinfo[desg_val].name,
		  ntn_tptr->name);
	  msg_grouped(SS_ptr->owner, xspot, yspot, string);
	  sprintf(string, "%s population resists capture",
		  ((desg_val == MAJ_NONE) ||
		   (desg_val == MAJ_CACHE)) ? "sector" :
		  maj_dinfo[desg_val].name);
	  msg_grouped(MS_ptr->owner, xspot, yspot, string);

	}

      }

    }

    /* only a city sector may be placed under siege */
    if (!IS_CITY(sct_ptr->designation) ||
	(ntn_ptr == NULL)) goto no_siege_here;

    /* now check for sieges */
    if ((owner_change == FALSE) &&
	(SS_ptr->owner != UNOWNED) &&
	(SS_ptr->siege_troops.size >
	 (2 * SS_ptr->hold_troops.size))) {

      /* there is a possible siege */
      total_troops = 2 * SS_ptr->hold_troops.size;
      ntnnum = SS_ptr->siege_troops.owner;

      /* check if any of the remaining troops oppose the siege */
      for (MS_ptr = SS_ptr->other_troops;
	   MS_ptr != NULL;
	   MS_ptr = MS_ptr->next) {
	if (((ntn_tptr = world.np[MS_ptr->owner]) != NULL) &&
	    (ntn_tptr->dstatus[ntnnum] >= DIP_WAR)) {
	  total_troops += MS_ptr->size;
	}
      }

      /* now check if the siege takes place */
      if (SS_ptr->siege_troops.size >
	  total_troops) {

	/* A siege takes place */
	SCT_STATUS(SS_ptr->xloc, SS_ptr->yloc) |= SET_SIEGE;

	/* notify sector owner */
	sprintf(string, "your %s is under siege.",
		maj_dinfo[major_desg(sct_ptr->designation)].name);
	msg_grouped(SS_ptr->owner, xspot ,yspot, string);

	/* notify the news */
	fprintf(fnews, "2.\t%s in nation %s is under siege.\n",
		maj_dinfo[major_desg(sct_ptr->designation)].name,
		world.np[SS_ptr->owner]->name);

	/* notify siegers */
	sprintf(string, "siege against nation %s successful",
		world.np[SS_ptr->owner]->name);
	msg_grouped(SS_ptr->siege_troops.owner, xspot, yspot, string);
	for (MS_ptr = SS_ptr->siege_troops.next;
	     MS_ptr != NULL;
	     MS_ptr = MS_ptr->next) {
	  msg_grouped(MS_ptr->owner, xspot, yspot, string);
	}
	goto free_it_up;

      }

    }

    /* inform siegers that no siege takes place */
  no_siege_here:
    if (SS_ptr->siege_troops.size > 0) {
      sprintf(string, "your siege was broken");
      msg_grouped(SS_ptr->siege_troops.owner, xspot, yspot, string);
      for (MS_ptr = SS_ptr->siege_troops.next;
	   MS_ptr != NULL;
	   MS_ptr = MS_ptr->next) {
	msg_grouped(MS_ptr->owner, xspot, yspot, string);
      }
    }

    /* now free up the memory, keeping track of location */
  free_it_up:
    SS_nptr = SS_ptr->next;
    sctsum_free(SS_ptr);
  }

  /* close up the group messages and news */
  send_sortednews();
  msg_gfinish();
}

