/* This file handles the customization of new conquer nations */
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
#include "calenX.h"
#define USE_CODES
#define PRINT_CODES
#include "magicX.h"
#undef USE_CODES
#undef PRINT_CODES
#include "buildA.h"
#include "desigX.h"
#include "mtrlsX.h"
#include "racesX.h"
#include "worldX.h"
#include "activeX.h"
#include "elevegX.h"
#include "nclassX.h"
#include "stringX.h"
#include "tgoodsX.h"
#include "statusX.h"
#include "adduserA.h"
#include "keyvalsX.h"

/* Variables for adding users */
int spent[AU_NUMBER], AU_leaders, AU_race, AU_class, AU_active, AU_mark;
char AU_passwd[PASSLTH+1], AU_name[NAMELTH+1], AU_lname[LEADERLTH+1];

/* adduser definitions */
char *AU_prompt[]= { "<ADD", "SUB>" };
char *AU_LType[]={ "Oops", "Random", "Fair", "Good", "Excellent" };

char *AU_labels[]= { 
  "Population", "Treasury", "Location",
  "Military", "Attack Bonus", "Defense Bonus", "Reproduction",
  "Movement", "Magic Powers", "Leaders", "Raw Materials"
  };

char *AU_items[]= {
  "people", "gold talons", "location", "soldiers",
  "percent", "percent", "percent", "move points", "powers",
  "nation leaders", "food units"
  };

char *AU_help[]= {
  "Population: Amount of citizens in your nation",
  "Treasury: Amount of monetary wealth in your nation",
  "Location: Relative value of nation placement in world",
  "Soldiers: Number of men in the national army, not counting leaders",
  "Attack Bonus: Skill level of your troops when attacking",
  "Defense Bonus: Skill level of your troops when defending",
  "Reproduction: Yearly rate of civilian population increase",
  "Movement: Number of movement points per normal army unit",
  "Magic Powers: Randomly obtain a new magical power",
  "Leaders: Number of leader units, including national ruler",
  "Raw Materials: Starting values for jewels, metal, and food"
  };

/* the minimum levels for each selection */
int AU_Minval[]= { 20, 0, 1, 1, 0, 0, 4, 6, 0, 4, 1 };

/* The list of corresponding values of each unit */
long AU_values[]= {
  1000L, 100000L, 1L, 250L, 1L, 1L, 1L, 1L, 1L, 1L, 40000L
};

/* macros to handle the others */
#define AU_cost	race_info[AU_race].au_costs
#define AU_Maxval	race_info[AU_race].au_maxval
#define AU_units	race_info[AU_race].au_units

/* TERA_CONVERT -- perform the actual teraforming */
static void
tera_convert PARM_2(int, x, int, y)
{
  int foodval;

  /* get the food value */
  foodval = tofood( &sct[x][y], country );

  /* change land sectors based on race */
  if (sct[x][y].altitude != ELE_WATER) {
    if (r_mountaineer(ntn_ptr->race)) {
      if (rand_val(3) == 0) {
	sct[x][y].altitude = ELE_MOUNTAIN;
      } else {
	sct[x][y].altitude = ELE_HILL;
      }
      if (rand_val(100) < global_int ) {
	if ((foodval >= DESFOOD - 1) &&
	    (foodval <= DESFOOD + 1)) {
	  sct[x][y].vegetation = VEG_LT_VEG;
	} else if (foodval < DESFOOD) {
	  sct[x][y].vegetation = VEG_BARREN;
	}
	getmetal( &sct[x][y] );
      }
    } else if (r_woodwinter(ntn_ptr->race)) {
      if (rand_val(3) == 0) {
	sct[x][y].vegetation = VEG_FOREST;
      } else {
	sct[x][y].vegetation = VEG_WOOD;
      }
      if (rand_val(100) < global_int) {
	if ((foodval >= DESFOOD - 1) &&
	    (foodval <= DESFOOD + 1)) {
	  sct[x][y].vegetation = VEG_LT_VEG;
	} else if (foodval < DESFOOD) {
	  sct[x][y].vegetation = VEG_BARREN;
	}
	getjewel( &sct[x][y] );
      }
    } else if (r_monsterly(ntn_ptr->race)) {
      if (rand_val(3) == 0) {
	sct[x][y].altitude = ELE_MOUNTAIN;
      } else {
	sct[x][y].altitude = ELE_HILL;
      }
      if (rand_val(100) < global_int) {
	if ((foodval >= DESFOOD - 1) &&
	    (foodval <= DESFOOD + 1)) {
	  sct[x][y].vegetation = VEG_LT_VEG;
	} else if (foodval < DESFOOD) {
	  sct[x][y].vegetation = VEG_BARREN;
	}
	if (rand_val(2) == 0)
	  getmetal( &sct[x][y] );
	else getjewel( &sct[x][y] );
      }
    } else {
      if (rand_val(2) == 0) {
	sct[x][y].altitude = ELE_CLEAR;
      }
      if (rand_val(100) < global_int) {
	if (rand_val(2) == 0) {
	  sct[x][y].vegetation = VEG_WOOD;
	} else {
	  sct[x][y].vegetation = VEG_GOOD;
	}
	if (rand_val(2) == 0)
	  getmetal( &sct[x][y] );
	else getjewel( &sct[x][y] );
      }
    }
  }
}

/* TERAFORM -- configure the area around the capital */
static void
teraform PARM_4( int, x, int, y, int, range, int, chance)
{
  /* alter surroundings based on race */
  global_int = chance;
  map_loop(x, y, range, tera_convert);

  /* fix the capital landscape */
  if (r_mountaineer(ntn_ptr->race)) {
    sct[x][y].altitude = ELE_HILL;
  } else if (r_woodwinter(ntn_ptr->race)) {
    sct[x][y].vegetation = VEG_FOREST;
    sct[x][y].altitude = ELE_CLEAR;
  } else if (r_monsterly(ntn_ptr->race)) {
    sct[x][y].altitude = ELE_HILL;
  } else {
    sct[x][y].altitude = ELE_CLEAR;
  }
}

/* DISPITEM -- display amount string at current location */
static void
dispitem PARM_2( int, item, long, amount)
{
  if (item == AU_LOCATE) {
    printw("%s %s", AU_LType[amount], AU_items[item]);
  } else {
    printw("%ld %s", amount, AU_items[item]);
  }

  /* Rawgoods show more info */
  if (item != AU_RAWGOODS) {
    addch('.');
    return;
  }

  /* now show the extras for the Raw Materials */
  printw(", %ld wood",
	 (long) (amount * ((float) AU_WOOD / AU_values[AU_RAWGOODS])));
#if AU_WOOD == AU_JEWELS && AU_WOOD == AU_METALS
  addstr(", jewels and metals.");
#else
#if AU_WOOD == AU_METALS
  printw(" and metals, and %ld jewels.",
	 (long) (amount * ((float) AU_JEWELS / AU_values[AU_RAWGOODS])));
#else
#if AU_JEWELS == AU_METALS
  printw(", %ld jewels and metals.",
	 (long) (amount * ((float) AU_JEWELS / AU_values[AU_RAWGOODS])));
#else
  printw(", %ld jewels and %ld metals.",
	 (long) (amount * ((float) AU_JEWELS / AU_values[AU_RAWGOODS])),
	 (long) (amount * ((float) AU_METALS / AU_values[AU_RAWGOODS])));
#endif
#endif
#endif
}

/* SHOWITEM -- show the current amount for country item */
static void
showitem PARM_2 (int, line, int, item)
{
  move(line, 15);
  if (item == AU_LOCATE) {
    sprintf(string,"%s %s", AU_LType[spent[item]], AU_items[item]);
    printw("%23s", string);
  } else {
    sprintf(string,"%ld %s", spent[item] * AU_values[item], AU_items[item]);
    printw("%23s", string);
  }

  /* rawgoods shows the extra portions */
  if (item != AU_RAWGOODS) return;
  line++;

  /* now show the extras for the Raw Materials */
#if AU_WOOD == AU_JEWELS && AU_JEWELS == AU_METALS
  sprintf(string, "%ld wood, jewels & metal",
	  spent[AU_RAWGOODS] * AU_WOOD);
  mvprintw(line, 0, "%38s", string);
  mvprintw(line, COLS / 2 + 13, "%ld wood, jewels & metal", AU_WOOD);
#else
#if AU_WOOD == AU_JEWELS
  sprintf(string, "%ld wood & jewels",
	  spent[AU_RAWGOODS] * AU_JEWELS);
  mvprintw(line, 0, "%38s", string);
  mvprintw(line++, COLS / 2 + 10, "%ld wood & jewels", AU_JEWELS);
  sprintf(string, "%ld metal",
	   spent[AU_RAWGOODS] * AU_METALS);
  mvprintw(line, 0, "%38s", string);
  mvprintw(line, COLS / 2 + 13, "%ld metals", AU_METALS);
#else
#if AU_WOOD == AU_METALS
  sprintf(string, "%ld wood & metals",
	  spent[AU_RAWGOODS] * AU_METALS);
  mvprintw(line, 0, "%38s", string);
  mvprintw(line++, COLS / 2 + 10, "%ld wood & metals", AU_METALS);
  sprintf(string, "%ld jewels",
	   spent[AU_RAWGOODS] * AU_JEWELS);
  mvprintw(line, 0, "%38s", string);
  mvprintw(line, COLS / 2 + 13, "%ld jewels", AU_JEWELS);
#else
#if AU_JEWELS == AU_METALS
  sprintf(string, "%ld wood",
	   spent[AU_RAWGOODS] * AU_WOOD);
  mvprintw(line, 0, "%38s", string);
  mvprintw(line++, COLS / 2 + 13, "%ld wood", AU_WOOD);
  sprintf(string, "%ld jewels & metals",
	  spent[AU_RAWGOODS] * AU_JEWELS);
  mvprintw(line, 0, "%38s", string);
  mvprintw(line, COLS / 2 + 10, "%ld jewels & metals", AU_JEWELS);
#else
  sprintf(string, "%ld wood",
	  spent[AU_RAWGOODS] * AU_WOOD);
  mvprintw(line, 0, "%38s", string);
  mvprintw(line++, COLS / 2 + 10, "%ld wood", AU_WOOD);
  sprintf(string, "%ld jewels",
	  spent[AU_RAWGOODS] * AU_JEWELS);
  mvprintw(line, 0, "%38s", string);
  mvprintw(line++, COLS / 2 + 10, "%ld jewels", AU_JEWELS);
  sprintf(string, "%ld metal",
	   spent[AU_RAWGOODS] * AU_METALS);
  mvprintw(line, 0, "%38s", string);
  mvprintw(line, COLS / 2 + 13, "%ld metals", AU_METALS);
#endif
#endif
#endif
#endif
}

/* RACE_SETUP -- Provide backbone of each race */
static void
race_setup PARM_1(int, rtype)
{
  int i;

  /* start it off right */
  AU_race = rtype;
  for (i = 0; i < AU_NUMBER; i++) {
    if (i == AU_LEADERS) {
      spent[i] += race_info[rtype].au_start[i];
    } else {
      spent[i] = race_info[rtype].au_start[i];
    }
  }
}

/* CLASS_POWERS -- Give magic powers based on the nation class */
static void
class_powers PARM_1(int, ctype)
{
  int count;

  /* take care of the power additions */
  for (count = 0; count < MAG_NUMBER; count++) {
    ADDMAGIC(ntn_ptr->powers[count],
	     nclass_list[ctype].pow_given[count]);
  }
}

/* RACE_MGPOWERS -- Initialize the magic powers based on the racial type */
static void
race_mgpowers PARM_1(int, rtype)
{
  int i;

  /* assign the list */
  for (i = 0; i < MAG_NUMBER; i++) {
    ntn_ptr->powers[i] = race_info[rtype].pow_start[i];
  }
}

/* CONVERT -- convert the stored information into the nation statistics */
static void
convert PARM_0(void)
{
  int i, j;
  long x;

  /* set the names and password */
  strncpy(ntn_ptr->leader, AU_lname, LEADERLTH);
  ntn_ptr->leader[LEADERLTH] = '\0';
  strncpy(ntn_ptr->name, AU_name, NAMELTH);
  ntn_ptr->name[NAMELTH] = '\0';
  strncpy(ntn_ptr->passwd, AU_passwd, PASSLTH);
  ntn_ptr->passwd[PASSLTH] = '\0';
  ntn_ptr->mark = AU_mark;

  /* now set the nation statistics */
  ntn_ptr->aplus = (short) AU_values[AU_ATTACK] * spent[AU_ATTACK];
  ntn_ptr->dplus = (short) AU_values[AU_DEFEND] * spent[AU_DEFEND];
  ntn_ptr->repro = (char) AU_values[AU_REPRO] * spent[AU_REPRO];
  ntn_ptr->maxmove = (unsigned char) AU_values[AU_MOVEMENT]
    * spent[AU_MOVEMENT];
  AU_leaders = (int) AU_values[AU_LEADERS] * spent[AU_LEADERS];
  ntn_ptr->race = AU_race;
  ntn_ptr->class = AU_class;
  ntn_ptr->active = AU_active;

  /* set the population and commodities */
  ntn_ptr->tciv = AU_values[AU_PEOPLE] * spent[AU_PEOPLE];
  ntn_ptr->tmil = AU_values[AU_SOLDIERS] * spent[AU_SOLDIERS];
  ntn_ptr->mtrls[MTRLS_TALONS] = AU_values[AU_TREASURY] * spent[AU_TREASURY];
  ntn_ptr->mtrls[MTRLS_FOOD] = AU_values[AU_RAWGOODS] * spent[AU_RAWGOODS];
  ntn_ptr->mtrls[MTRLS_WOOD] = AU_WOOD * spent[AU_RAWGOODS];
  ntn_ptr->mtrls[MTRLS_JEWELS] = AU_JEWELS * spent[AU_RAWGOODS];
  ntn_ptr->mtrls[MTRLS_METALS] = AU_METALS * spent[AU_RAWGOODS];

  /* set the location */
  if ((spent[AU_LOCATE] < 0) ||
      (spent[AU_LOCATE] > AU_EXCELLENT)) {
    ntn_ptr->location = AU_RANDOM;
  } else {
    ntn_ptr->location = spent[AU_LOCATE];
  }

  /* now set the magic for the nation */
  race_mgpowers(ntn_ptr->race);
  class_powers(ntn_ptr->class);
  for(i = 0; i < spent[AU_MAGIC]; i++) {

    /* verify result */
    j = rand_val(MAG_WIZARDRY - MAG_MILITARY) + MAG_MILITARY;
    x = rand_magic(j);
    if (x == 0L) {
      /* try again */
      i--;
    } else {
      ADDMAGIC(ntn_ptr->powers[j], x);
    }

  }

}

/* POINT_COST -- return the point cost of the current nation */
static int
point_cost PARM_0(void)
{
  float points = 0.0;
  int i;
  
  /* calculate cost for all so far */
  for (i = 0; i < AU_NUMBER; i++) {
    points += AU_cost[i] * (float) spent[i] / AU_units[i];
  }
  points += nclass_list[AU_class].cost;

  /* add in the cost for magical powers */
  for (i = 0; i < MAG_NUMBER; i++) {
    points += AU_cost[AU_MAGIC] *
      num_bits_on(race_info[AU_race].pow_start[i] |
		  nclass_list[AU_class].pow_given[i]);
  }
  
  /* extra points for starting late */
  points -= (float) (TURN - world.start_turn - 1) / LATESTART;

  /* return rounded upward cost */
  points += 1.0;
  return((int) points);
}

/* MIN_NEIGHBOR -- return the minimum distance between two neighbors */
static int
min_neighbor PARM_2(int, loc1, int, loc2)
{
  /* minimum building distance plus a percentage based on location type */
  return((max(loc1, loc2) * (int) BUILDDIST) / 10 + (int) BUILDDIST + 2);
}

/* CHECK_NEIGHBORS -- Eliminate any placement close to other nations */
static int
check_neighbors PARM_2(int, xp, int, yp)
{
  NTN_PTR n1_ptr;
  int cntry;

  /* quit out on any nations which are too close */
  for (cntry = 1; cntry < MAXNTN; cntry++) {

    /* skip current nation, and non-nations */
    if ((cntry == country) ||
	((n1_ptr = world.np[cntry]) == NULL) ||
	n_ismonster(n1_ptr->active)) continue;

    /* now simply test within distance */
    if (map_within(xp, yp, n1_ptr->capx, n1_ptr->capy,
		   min_neighbor(ntn_ptr->location,
				n1_ptr->location))) {
      /* not good */
      return(TRUE);
    }
  }

  /* nobody nearby */
  return(FALSE);
}

/* PL_WATERCHECK -- Check water and neighbors */
static void
pl_watercheck PARM_2(int, x, int, y)
{
  /* count water... bug out on neighbors */
  if (sct[x][y].owner != UNOWNED)
    global_int = -1;
  else if ((global_int >= 0) &&
	   (sct[x][y].altitude == ELE_WATER)) {
    global_int++;
  }
}

/* PL_VEGCHECK -- Check the vegetation of the sector */
static void
pl_vegcheck PARM_2(int, x, int, y)
{
  int foodval = tofood( &sct[x][y], country );

  /* check it */
  if (foodval > 0) {
    global_long += foodval;
    global_int++;
  }
}

/* RANGE_SECTS -- Number of sectors, around a sector, within a given range */
static int
range_sects PARM_1(int, rng)
{
  int i, nsects = 0;

  for (i = 1; i <= rng; i++) {
    nsects += i;
  }
  if (world.hexmap) {
    nsects *= 6;
  } else {
    nsects *= 8;
  }
  return(nsects);
}

/* PLACE_CHECK -- Check for the nearby water and monster sectors */
static int
place_check PARM_5(int, xp, int, yp, int, mrange, int, wrange, int, vrange)
{
  int nsects;

  /* calculate the sectors in range */
  nsects = range_sects(wrange);

  /* quick check if both equal */
  if (mrange == wrange) {

    /* check both at once */
    global_int = 0;
    map_loop(xp, yp, wrange, pl_watercheck);
    if ((global_int == -1) ||
	(global_int < wrange - 1) ||
	(global_int >= nsects - (mrange * 2))) return(FALSE);

  } else {

    /* first check the monster range */
    global_int = 0;
    map_loop(xp, yp, mrange, pl_watercheck);
    if (global_int == -1) return(FALSE);

    /* now check the water range */
    global_int = 0;
    map_loop(xp, yp, wrange, pl_watercheck);
    if ((global_int < (ntn_ptr->location - 1) / 2) ||
	(global_int > (nsects * (75 - 15 * ntn_ptr->location)) / 100))
      return(FALSE);

  }

  /* final check on habitability */
  if (vrange > 0) {

    /* determine amount of nearby vegetation (food) */
    global_int = 0;
    global_long = 0;
    map_loop(xp, yp, vrange, pl_vegcheck);

    /* is there a high enough level of food? */
    if ((global_int == 0) ||
	((global_int * 100) / nsects < 20 + 10 * ntn_ptr->location) ||
	((((double) global_long) / global_int) <
	 (2.5 + ((double) ntn_ptr->location) / 2.0))) {
      return(FALSE);
    }

  }

  /* hunky dorey */
  return(TRUE);
}

/* PL_CHOWN -- Change the ownership of neighboring sectors */
static void
pl_chown PARM_2(int, x, int, y)
{
  if ((tofood( &sct[x][y], country) >= DESFOOD) &&
      (sct[x][y].owner == UNOWNED)) {
    ntn_ptr->tsctrs++;
    sct[x][y].owner = country;

    /* need a lumberyard */
    if ((global_int < 3) &&
	(towood(&(sct[x][y]), country) > 1)) {
      global_int += towood(&(sct[x][y]), country);
      sct[x][y].designation = MAJ_LUMBERYARD;
    } else if (tg_ok(&(sct[x][y]), country) &&
	       (metal_value(&(sct[x][y])) > 3)) {
      sct[x][y].designation = MAJ_METALMINE;
    } else {
      switch (SEASON(TURN)) {
      case SUMMER_SEASON:
	/* the fertile summer */
	sct[x][y].designation = MAJ_FERTILE;
	break;
      case FALL_SEASON:
	/* fall, fruitful */
	sct[x][y].designation = MAJ_FRUITFUL;
	break;
      default:
	/* spring and winter, gains a FARM */
	sct[x][y].designation = MAJ_FARM;
	break;
      }
    }
    sct[x][y].people = global_long;
    sct_ptr->people -= global_long;
  }
}

/* PLACE -- Put nation on the map.  Fill out army structures as well. */
static int
place PARM_2(int, xloc, int, yloc)
{
  int placed = FALSE, armysize;
  long people = 0L;
  int x = 0, y = 0, trng, mrng, wrng, vrng, brdr, cpct;
  int i = 0, leadtype;
  long soldsleft;

  /* check sector location */
  if( xloc != -1 && yloc != -1 && is_habitable(xloc, yloc)) {
    placed = TRUE;
    x = xloc;
    y = yloc;
  }

  /* check the location */
  if ((ntn_ptr->location < 0) ||
      (ntn_ptr->location >AU_EXCELLENT)) {
    errormsg("Invalid location setting... set to OOPS");
    ntn_ptr->location = AU_OOPS;
  }

  /* calculate the monster, water and vegetation ranges */
  mrng = ntn_ptr->location + 1;
  vrng = ntn_ptr->location + 1;
  wrng = (ntn_ptr->location + 1) / 2;
  if (ntn_ptr->location == AU_EXCELLENT) {
    cpct = 100;
    trng = 2;
  } else {
    cpct = 25 * (ntn_ptr->location + 1);
    trng = 1;
  }

  /* find map border */
  if (n_ispc(ntn_ptr->active)) {
    brdr = (4 * (ntn_ptr->location + 1) * MAPY) / 100;
  } else {
    brdr = (2 * (ntn_ptr->location + 1) * MAPY) / 100;
  }

  /* put it anywhere away from the edge */
  while ((placed == FALSE) && (i++ < 3000)) {

    /* check for a long run */
    if ((i % 800) == 0) {
      brdr -= 2;
      if (brdr < 0) brdr = 0;
    }
    if (i == 2500) {
      vrng--;
    }

    /* pick a sector */
    x = rand_val(MAPX);
    y = rand_val(MAPY - brdr * 2) + brdr;
    if (!is_habitable(x, y)) continue;

    /* neighbor check */
    if (check_neighbors(x, y)) continue;

    /* so far the sector is good */
    placed = place_check(x, y, mrng, wrng, vrng);

    /* successfully placed */
    if (placed == TRUE) {
      teraform(x, y, trng, cpct);
    }

  }

  /* done with one try */
  if (placed == TRUE) {

    /* let the news hear about it */
    fprintf(fnews, "1.\tGod welcomes nation %s into the world\n",
	    ntn_ptr->name);

    /* create the capital sector */
    ntn_ptr->centerx = ntn_ptr->capx = x;
    ntn_ptr->centery = ntn_ptr->capy = y;
    sct_ptr = &(sct[x][y]);
    sct_ptr->designation = MAJ_CAPITAL;
    set_minordesg(sct_ptr->designation, MIN_FORTIFIED);
    if (r_monsterly(ntn_ptr->race)) {
      sct_ptr->tradegood = rand_tgood(TG_TERROR, 3);
    } else {
      sct_ptr->tradegood = rand_tgood(TG_KNOWLEDGE, 3);
    }
    sct_ptr->minerals = 0;
    sct_ptr->owner = country;
    sct_ptr->people = ntn_ptr->tciv;

    /* now create the city inventory around the capital */
    city_ptr = crt_city(ntn_ptr->name);
    CITY_XLOC = x;
    CITY_YLOC = y;
    CITY_FORT = 5;
    switch (SEASON(TURN)) {
    case WINTER_SEASON:
      /* let them survive winter */
      ntn_ptr->mtrls[MTRLS_FOOD] *= 2;
      break;
    case FALL_SEASON:
      /* gain some food */
      ntn_ptr->mtrls[MTRLS_FOOD] *= 3;
      ntn_ptr->mtrls[MTRLS_FOOD] /= 2;
      break;
    }
    for (i = 0; i < MTRLS_NUMBER; i++) {
      CITY_IMTRLS[i] = CITY_MTRLS[i] = ntn_ptr->mtrls[i];
    }
    CITY_STALONS = ntn_ptr->mtrls[MTRLS_TALONS];
    CITY_PEOPLE = (PVOLUNTEERS * sct_ptr->people) / 100;

    /* put all military into armies that may take land */
    armysize = TAKESECTOR;

    /* set variables for military creation */
    soldsleft = ntn_ptr->tmil;
    army_ptr = crt_army(defaultunit());
    ARMY_SIZE = ntn_ptr->tmil / MILINCAP;
    ARMY_SIZE += ((soldsleft - ntn_ptr->tmil / MILINCAP) % armysize);
    soldsleft -= ARMY_SIZE;
    set_status(ARMY_STAT, ST_GARRISON);
    set_speed(ARMY_STAT, SPD_NORMAL);
    ARMY_SPLY = start_unitsply();
    ARMY_LASTX = ARMY_XLOC = ntn_ptr->capx;
    ARMY_LASTY = ARMY_YLOC = ntn_ptr->capy;

    /* find the ruler type */
    ntn_ptr->tleaders = AU_leaders;
    leadtype = getruler(ntn_ptr->class);

    /* create the national leader */
    army_ptr = crt_army(leadtype);
    ARMY_SIZE = ainfo_list[leadtype].minsth;
    ARMY_LASTX = ARMY_XLOC = ntn_ptr->capx;
    ARMY_LASTY = ARMY_YLOC = ntn_ptr->capy;
    set_status(ARMY_STAT, ST_DEFEND);
    set_speed(ARMY_STAT, SPD_NORMAL);

    /* find the leader type */
    leadtype = getminleader(ntn_ptr->class);

    /* now create all of the minor leaders */
    AU_leaders--;
    while (AU_leaders > 0) {
      army_ptr = crt_army(leadtype);
      ARMY_SIZE = ainfo_list[leadtype].minsth;
      ARMY_LASTX = ARMY_XLOC = ntn_ptr->capx;
      ARMY_LASTY = ARMY_YLOC = ntn_ptr->capy;
      set_status(ARMY_STAT, ST_DEFEND);
      set_speed(ARMY_STAT, SPD_NORMAL);
      AU_leaders--;
    }

    /* give the spellcasters */
    give_spellcasters();

    /* provide the rest of the armies */
    while(soldsleft > 0) {
      army_ptr = crt_army(defaultunit());

      if (soldsleft >= armysize) {
	ARMY_SIZE = armysize;
	soldsleft -= armysize;
      } else {
	ARMY_SIZE = soldsleft ;
	soldsleft = 0;
      }
      ARMY_LASTX = ARMY_XLOC = ntn_ptr->capx;
      ARMY_LASTY = ARMY_YLOC = ntn_ptr->capy;
      ARMY_SPLY = start_unitsply();
      set_status(ARMY_STAT, ST_DEFEND);
      set_speed(ARMY_STAT, SPD_NORMAL);
    }

    /* now divide people */
    people = (sct_ptr->people * 2) /
      (range_sects(trng) * (5 - trng));

    /* now assign ownership of sectors */
    ntn_ptr->tsctrs = 1;
    global_long = people;
    global_int = 0;
    map_loop(x, y, trng, pl_chown);

    /* now update all of the military movement */
    upd_military(country);

  } else {

    if (ntn_ptr->location == AU_OOPS) {
      errormsg("Could not place the nation... I give up");
      return(FALSE);
    } else if (ntn_ptr->location == AU_RANDOM) {

      errormsg("Random place failed, trying one last time");
      ntn_ptr->location = AU_OOPS;
      return(place(-1, -1));

    } else if (ntn_ptr->location == AU_FAIR) {

      sprintf(string, "Fair place failed, trying again; adding %ld people",
	      AU_units[AU_PEOPLE] * AU_values[AU_PEOPLE] / AU_cost[AU_PEOPLE]);
      errormsg(string);

      /* give back one point -> nlpop people */
      ntn_ptr->tciv += AU_units[AU_PEOPLE] * AU_values[AU_PEOPLE]
	/ AU_cost[AU_PEOPLE];
      ntn_ptr->location = AU_RANDOM;
      return(place(-1, -1));

    } else if (ntn_ptr->location == AU_GOOD) {

      sprintf(string, "Great place failed, trying again; adding %ld people",
	      AU_units[AU_PEOPLE] * AU_values[AU_PEOPLE] / AU_cost[AU_PEOPLE]);
      errormsg(string);

      /* give back one point -> nlpop people */
      ntn_ptr->tciv += AU_units[AU_PEOPLE] * AU_values[AU_PEOPLE]
	/ AU_cost[AU_PEOPLE];

      ntn_ptr->location = AU_FAIR;
      return(place(-1, -1));

    } else if (ntn_ptr->location == AU_EXCELLENT) {

      sprintf(string, "Excellent place failed, trying again; adding %ld people",
	      AU_units[AU_PEOPLE] * AU_values[AU_PEOPLE] / AU_cost[AU_PEOPLE]);
      errormsg(string);

      /* give back one point -> nlpop people */
      ntn_ptr->tciv += AU_units[AU_PEOPLE] * AU_values[AU_PEOPLE]
	/ AU_cost[AU_PEOPLE];

      ntn_ptr->location = AU_GOOD;
      return(place(-1, -1));

    }

  }

  /* hunky dorey */
  return(TRUE);
}

/* DO_CLASS -- Initialize class information */
static void
do_class PARM_1 ( short, class_val )
{
  /* determine number of leaders you want */
  spent[AU_LEADERS] = (nclass_list[class_val].leadnum /
		       AU_values[AU_LEADERS]);
}

/* GETCLASS -- Obtain the national class */
static int
getclass PARM_1 (int, race)
{
  short chk = FALSE;
  short tmp = 0;
  short ypos = 4;
  int i, j;
  
  mvaddstr(ypos, 0, "The list of possible nation classes:");
  ypos += 2;
  mvprintw(ypos++,0,"     %-12s %4s %15s %12s %4s", "Class", "Race",
	   "", "Magic  ", "Cost");
  mvprintw(ypos++,0,"     %-12s %4s %15s %12s %4s", "--------", "----",
	   "", "---------", "----");

  /* now go through all of the possible classes */
  for (i = 1; i < nclass_number; i++) {
    if (char_in_str(race_info[race].name[0],
		    nclass_list[i].racetype) == TRUE) {
      mvprintw(ypos++,0," %2d) %-12s %4s %15s", i,
	       nclass_list[i].name,
	       nclass_list[i].racetype,
	       "...............");
      tmp = strlen(nclass_list[i].powname);
      for (j = 0; j < 12 - tmp ; j++) {
	addch('.');
      }
      printw(" %s", nclass_list[i].powname);
      tmp = 0;
      for (j = 0; j < MAG_NUMBER; j++) {
	tmp += num_bits_on(nclass_list[i].pow_given[j] |
			   race_info[AU_race].pow_start[j]);
      }
      tmp *= AU_cost[AU_MAGIC];
      printw(" %4d", tmp + nclass_list[i].cost);

    }
  }

  /* now get the selection */
  ypos++;
  while (chk == FALSE) {
    mvaddstr(ypos, 0, "Enter the number of your choice: ");
    clrtoeol();
    refresh();
    if (((tmp = get_number(FALSE)) == -1) &&
	(no_input == TRUE)) {
      mvaddstr(LINES - 1, 0,
	       "Do you wish to abort the building of this nation? ");
      if (y_or_n()) {
	return(-1);
      }
      clear_bottom(1);
      continue;
    }

    /* validate the choice */
    if (tmp < 1 || tmp > nclass_number) {
      errormsg("Invalid Choice");
    } else if (char_in_str(race_info[race].name[0],
			   nclass_list[tmp].racetype) == TRUE) {
      chk = TRUE;
    } else {
      errormsg("That class is invalid for your race");
    }
  }

  /* set the selection */
  AU_class = tmp;
  for (tmp = ypos; tmp > 4; tmp--) {
    move(tmp, 0);
    clrtoeol();
  }

  return(0);
}

/* NEWLOGIN -- Create a new nation specification */
int
newlogin PARM_1(int, makenpcs)
{
  /* use points to create empire, add if late starter */
  int points, clr, num_built = 0;
  int choice, direct;
  int valid = TRUE;
  int temp = 0, ypos, xpos;
  int no_save = FALSE, more = TRUE;
  long x;
  char tmp_passwd[PASSLTH+1];
  register i;

  /* setup curses display */
  cq_init("cqadd");

  /* increase maximums if needed */
  country = (TURN - world.start_turn - 1) / 12;
  if (country > 0) {
    for (i = 0; i < AU_NUMBER; i++) {
      switch (i) {
      case AU_LOCATE:
      case AU_REPRO:
	break;
      case AU_MAGIC:
	AU_Maxval[i] += (2 * country) / 3;
	break;
      case AU_LEADERS:
      case AU_SOLDIERS:
	AU_Maxval[i] += country / 2;
	break;
      default:
	AU_Maxval[i] += country;
	break;
      }
    }
  }

  /* find valid nation number type */
  country = 0;
  for (i = 1; i < ABSMAXNTN; i++)
  if ((ntn_ptr = world.np[i]) == NULL) {
    country = i;
    break;
  }

  /* continue loop */
  clear();
  while (more == TRUE) {

    /* clean curses memory display */
    move(0, 0);
    clrtobot();

    /* provide information */
    if (makenpcs == TRUE) {
      strcpy(string, "Npc Nation");
    } else {
      sprintf(string, "Country #%d", country);
    }

    /* give them the bottom info */
    errorbar("Nation Builder", string);
    strcpy(string, "=== Hit the ESCape Key to abort nation creation ===");
    standout();
    mvaddstr(LINES - 4, (COLS - strlen(string)) / 2, string);
    standend();

    /* let them know they are out of luck */
    if ((country == 0) && (makenpcs == FALSE)) {
      errormsg("No more nations available");
      cq_reset();
      return(0);
    }

    /* begin purchasing */
    mvprintw(0, 0, "Building Country Number %d",country);
    if (makenpcs == TRUE) {
      points = DFLT_MAXPTS;
    } else {
      points = MAXPTS;
    }
    for (i = 0; i < AU_NUMBER; i++) {
      spent[i] = 0;
    }

    valid = FALSE;
    /*get name*/
    while (valid == FALSE) {

      /* get the name for the country */
      valid = TRUE;
      mvaddstr(1,0,"Enter a Name for your Country: ");
      clrtoeol();
      refresh();
      if (get_string(string, STR_NORM, NAMELTH) == -1) {
	mvaddstr(LINES - 1, 0,
		 "Do you wish to abort the building of this nation? ");
	if (y_or_n()) {
	  goto Done_Add;
	}
	clear_bottom(1);
	valid = FALSE;
      } else if ( strlen(string) <= 1 || strlen(string) > NAMELTH) {
	/* check the length */
	errormsg("Invalid Name Length");
	valid = FALSE;
      } else if ( strcmp(string,"god") == 0 || strcmp(string,"unowned") == 0) {
	/*check if already used*/
	errormsg("There is already a nation by that name");
	valid = FALSE;
      } else if (ntnbyname(string) != NULL) {
	errormsg("There is already a nation by that name");
	valid=FALSE;
      }

    }
    strcpy(AU_name, string);
    move(0,0);
    clrtoeol();
    move(1,0);
    clrtoeol();
    standout();
    mvprintw(0, COLS / 2 - 12 - strlen(AU_name) / 2,
	     "< Building Country %s >", AU_name, country);
    standend();
    clrtoeol();

    /* now enter in the nation password */
    if (makenpcs == FALSE) {
      valid = FALSE;
      while (valid == FALSE) {

	/* display the prompt */
	mvaddstr(2, 0, "Enter National Password: ");
	clrtoeol();
	refresh();

	/* get the password */
	if ((i = get_pass(string)) == -1) {
	  mvaddstr(LINES - 1, 0,
		   "Do you wish to abort the building of this nation? ");
	  if (y_or_n()) {
	    goto Done_Add;
	  }
	  clear_bottom(1);
	  continue;
	} else if (i <= 2) {
	  errormsg("Password Too Short");
	  continue;
	} else if (i > PASSLTH) {
	  errormsg("Password Too Long");
	  continue;
	}

	/* get it again to make sure */
	bottommsg("Password accepted; Please confirm it...");
	mvaddstr(2, 0, "Reenter National Password: ");
	clrtoeol();
	refresh();

	/* verify it */
	if ((i = get_pass(tmp_passwd)) == -1) {
	  mvaddstr(LINES - 1, 0,
		   "Do you wish to abort the building of this nation? ");
	  refresh();
	  if (y_or_n()) {
	    goto Done_Add;
	  }
	  clear_bottom(1);
	} else if (i <= 2 || i > PASSLTH
	    || strncmp(tmp_passwd, string, PASSLTH) != 0) {
	  errormsg("Invalid Password Match");
	} else valid = TRUE;
      }
#ifdef CRYPT
      strncpy(AU_passwd, crypt(string, SALT), PASSLTH);
#else
      strncpy(AU_passwd, string, PASSLTH);
#endif /*CRYPT*/
      AU_passwd[PASSLTH] = '\0';
    }
    bottommsg("");
    
    /*get your name*/
    valid = FALSE;
    while (valid == FALSE) {
      valid = TRUE;
      mvaddstr(2,0, "Enter the name of your country's leader (Ex. The Ed, Gandalf, Conan)");
      clrtoeol();
      mvprintw(3,0,"    [maximum %d characters]: ",LEADERLTH);
      clrtoeol();
      refresh();
      if (get_string(string, STR_SPACE, NAMELTH) == -1) {
	mvaddstr(LINES - 1, 0,
		 "Do you wish to abort the building of this nation? ");
	if (y_or_n()) {
	  goto Done_Add;
	}
	valid = FALSE;
	clear_bottom(1);
      } else if (strlen(string) > LEADERLTH || strlen(string) < 2)  {
	errormsg("Invalid Name Length");
	valid = FALSE;
      } else {
	strcpy(AU_lname, string);
      }
    }

    mvprintw(2,0,"Leader Name: %s", AU_lname);

    /* display race choices */
    clrtoeol();

    /* now obtain the nation race */
    valid = FALSE;
    while (valid == FALSE) {
      /* display the race */
      mvaddstr(3,0," Enter your Race [ ");
      valid = TRUE;
      for (AU_race = 0; AU_race < RACE_NUMBER; AU_race++) {
	if (!r_notrace(AU_race)) {
	  if (valid == FALSE) {
	    addstr(", ");
	  }
	  valid = FALSE;
	  hip_string(race_info[AU_race].name, 1, TRUE);
	}
      }
      addstr(" ]: ");
      clrtoeol();
      refresh();
      if ((valid = next_char()) == EXT_ESC) {
	/* quick out? */
	mvaddstr(LINES - 1, 0,
		 "Do you wish to abort the building of this nation? ");
	if (y_or_n()) {
	  goto Done_Add;
	}
	clear_bottom(1);
	valid = FALSE;
      } else if ((valid == CNTRL_L) ||
		 (valid == CNTRL_R)) {
	/* redraw the screen */
	wrefresh(curscr);
	valid = FALSE;
      } else {
	if (islower(valid)) valid = toupper(valid);
	for (AU_race = 0; AU_race < RACE_NUMBER; AU_race++) {
	  if (!r_notrace(AU_race) &&
	      (valid == race_info[AU_race].name[0])) break;
	}
	if (AU_race == RACE_NUMBER) {
	  valid = FALSE;
	  continue;
	}
	valid = TRUE;
      }
    }

    /* assign specs based on the race */
    mvprintw(3, 0, "Nation Race: %s", race_info[AU_race].name);
    clrtoeol();
    if ((temp = getclass(AU_race)) == -1) {
      goto Done_Add;
    }
    do_class(AU_class);
    race_setup(AU_race);
    points -= point_cost();
    mvprintw(2, COLS / 2 - 12, "Nation Class: %s",
	     nclass_list[AU_class].name);
    clrtoeol();
    move(4, 0);
    clrtoeol();

    /* now find the alignment */
    valid = FALSE;
    if (r_monsterly(AU_race)) {
      /* orcs are always evil */
      valid = TRUE;
      AU_active = ALIGN_EVIL;
    }
    while (valid == FALSE) {
      valid = TRUE;
      mvaddstr(5, 0, "Enter Alignment of ");
      hip_string("Good, ", 1, TRUE);
      hip_string("Neutral, or ", 1, TRUE);
      hip_string("Evil: ", 1, TRUE);
      refresh();
      switch(next_char()) {
      case EXT_ESC:
	/* quick out? */
	mvaddstr(LINES - 1, 0,
		 "Do you wish to abort the building of this nation? ");
	if (y_or_n()) {
	  goto Done_Add;
	}
	clear_bottom(1);
	valid = FALSE;
	break;
      case 'G':
      case 'g':
	AU_active = ALIGN_GOOD;
	break;
      case 'N':
      case 'n':
	AU_active = ALIGN_NEUTRAL;
	break;
      case 'E':
      case 'e':
	AU_active = ALIGN_EVIL;
	break;
      default:
	valid = FALSE;
	break;
      }
    }
    mvprintw(3, COLS / 2 - 12,
	     "Alignment: %s", alignment[n_alignment(AU_active)]);
    clrtoeol();

    /* now get the nation aggressiveness */
    mvaddstr(5, 0, "Enter computer aggression level of ");
    hip_string("None, ", 1, TRUE);
    hip_string("Static, ", 1, TRUE);
    hip_string("Enforce,", 1, TRUE);
    clrtoeol();

    /* get user input */
    valid = FALSE;
    while (valid == FALSE) {
      move(6, 8);
      hip_string("Overt, ", 1, TRUE);
      hip_string("Mobile, or ", 1, TRUE);
      hip_string("Killer: ", 1, TRUE);
      clrtoeol();
      refresh();
      valid = TRUE;
      switch(next_char()) {
      case EXT_ESC:
	/* quick out? */
	mvaddstr(LINES - 1, 0,
		 "Do you wish to abort the building of this nation? ");
	if (y_or_n()) {
	  goto Done_Add;
	}
	clear_bottom(1);
	valid = FALSE;
	break;
      case 'N':
      case 'n':
	/* none */
	AU_active += 4 * ACT_NOMOVE;
	break;
      case 'S':
      case 's':
	/* static */
	AU_active += 4 * ACT_STATIC;
	break;
      case 'E':
      case 'e':
	/* enforce */
	AU_active += 4 * ACT_ENFORCE;
	break;
      case 'O':
      case 'o':
	/* overt */
	AU_active += 4 * ACT_OVERT;
	break;
      case 'M':
      case 'm':
	/* mobile */
	AU_active += 4 * ACT_MOBILE;
	break;
      case 'K':
      case 'k':
	/* killer */
	AU_active += 4 * ACT_KILLER;
	break;
      default:
	valid = FALSE;
	break;
      }
    }
    mvprintw(2, COLS - 23,
	     "Aggression: %s", aggressname[n_aggression(AU_active)]);
    clrtoeol();

    /* set the npc aggression level */
    if (makenpcs == TRUE) {
      AU_active += ACT_PCMODULUS;
    }

    /* get new nation mark */
    AU_mark = ' ';
    while (TRUE) {
      temp = 30;
      mvaddstr(6, 0, "This can be any of the following:");
      clrtoeol();
      for (string[0] = '!'; string[0] <= '~' ; string[0]++) {
	if ( markok( string[0], AU_race, FALSE ) ) {
	  temp += 2;
	  if (temp > COLS - 20) {
	    printw("\n    ");
	    temp = 8;
	  }
	  printw(" %c",string[0]);
	}
      }
      mvaddstr(5, 0, "Enter National Mark (for maps): ");
      clrtoeol();
      refresh();
      if ((string[0] = next_char()) == EXT_ESC) {
	/* quick out? */
	mvaddstr(LINES - 1, 0,
		 "Do you wish to abort the building of this nation? ");
	if (y_or_n()) {
	  goto Done_Add;
	}
	clear_bottom(1);
      } else if ( markok( string[0], AU_race, TRUE ) ) {
	AU_mark = string[0];
	break;
      }
    }

    /* display the resultant national mark */
    mvprintw(3, COLS - 23, "National Mark [%c]", AU_mark);
    clrtoeol();
    move(5,0);
    clrtoeol();
    move(6,0);
    clrtoeol();
    move(7,0);
    clrtoeol();
    refresh();

    ypos = 5;
    mvprintw(ypos, 0, "  %-13s       %s", "ITEM", "CURRENTLY HAVE" );
    mvprintw(ypos++, COLS / 2 + 5,"%4s    %s", "COST", "AMOUNT" );
    for (i = 0; i < AU_NUMBER; i++) {
      mvprintw(ypos, 0, "%-15s", AU_labels[i]);
      showitem(ypos, i);
      if (i == AU_LOCATE) {
	mvprintw(ypos, COLS / 2 + 5,"%3d     %s", AU_cost[i],
		 "Better Location");
      } else {
	mvprintw(ypos, COLS / 2 + 5,"%3d for", AU_cost[i]);
	printw(" %ld %s", AU_units[i] * AU_values[i], AU_items[i]);
      }
      ypos++;
    }

    /* show everything before menu */
    no_save = FALSE;
    direct = ADDITION;
    choice = AU_PEOPLE;
    xpos = COLS / 2;
    ypos = 6;
    valid = FALSE;
    clr = 1;
    standout();
    mvaddstr(LINES - 4, (COLS - 76) / 2,
	     "'Q'=Abort  'q'=Done  SPC=Exec  '?'=Info  'h'=Add  'l'=Sub  'k'=Up  'j'=Down");
    standend();

    while (valid == FALSE) {
      if (clr == 1) {
	standout();
	mvprintw(5, 0, "Points Left: %d", points);
	standend();
	clrtoeol();
	clr++;
      } else if (clr == 2) {
	bottommsg("");
	clr = 0;
      }
      standout();
      mvaddstr(ypos + choice, xpos, AU_prompt[direct]);
      standend();
      refresh();
      switch (next_char()) {
      case CNTRL_R:
      case CNTRL_L:
	/* redraw */
	wrefresh(curscr);
	break;
      case '?':
	/* help on topic */
	errormsg(AU_help[choice]);
	break;
      case 'Q':
	bottommsg("Do you wish to exit without building the nation?");
	if (y_or_n()) {
	  valid = TRUE;
	  no_save = TRUE;
	}
	break;
      case 'q':
      case EXT_ESC:
	/* exit option */
	if (points > 0) {
	  bottommsg("Use remaining points for population? [ny]");
	  if (y_or_n() == FALSE) {
	    errormsg("All points must be spent prior to exiting");
	    break;
	  }
	  temp = points * AU_units[AU_PEOPLE] / AU_cost[AU_PEOPLE];
	  x = temp * AU_values[AU_PEOPLE];
	  spent[AU_PEOPLE] += temp;
	  showitem(ypos + AU_PEOPLE, AU_PEOPLE);
	  points = 0;
	  sprintf(string,"Buying %ld more civilians", x);
	  errormsg(string);
	}
	bottommsg("Are you done building this nation? (y or n)");
	if (y_or_n()) {
	  valid = TRUE;
	}
	clr = 1;
	break;
      case '-':
      case '>':
      case 'L':
      case 'l':
	/* subtraction */
	direct = SUBTRACTION;
	break;
      case '+':
      case '<':
      case 'H':
      case 'h':
	/* addition */
	direct = ADDITION;
	break;
      case '\b':
      case EXT_DEL:
	/* decrease choice -- with wrap */
	mvaddstr(ypos + choice, xpos, "    ");
	if (choice == AU_PEOPLE) {
	  choice = AU_RAWGOODS;
	} else {
	  choice--;
	}
	break;
      case 'K':
      case 'k':
	/* move choice up one */
	if (choice > AU_PEOPLE) {
	  mvaddstr(ypos + choice, xpos, "    ");
	  choice--;
	} else {
	  beep();
	}
	break;
      case '\r':
      case '\n':
	/* increase choice -- with wrap */
	mvaddstr(ypos + choice, xpos, "    ");
	if (choice == AU_RAWGOODS) {
	  choice = AU_PEOPLE;
	} else {
	  choice++;
	}
	break;
      case 'J':
      case 'j':
	/* move choice down one */
	if (choice < AU_RAWGOODS) {
	  mvaddstr(ypos + choice, xpos, "    ");
	  choice++;
	} else {
	  beep();
	}
	break;
      case ' ':
      case '.':
	/* make the selection */
	temp = AU_units[choice];

	/* now check if it is added or removed */
	if (direct == ADDITION) {
	  if (AU_cost[choice] > points) {
	    sprintf(string, "You do not have %d points to spend",
		    AU_cost[choice]);
	    errormsg(string);
	  } else if (spent[choice] + temp > AU_Maxval[choice]) {
	    errormsg("You may not purchase any more of that item");
	  } else {
	    spent[choice] += temp;
	    bottommsg("You now have ");
	    dispitem(choice, spent[choice] * AU_values[choice]);
	    showitem(ypos + choice, choice);
	    points -= AU_cost[choice];
	    clr = 1;
	  }
	} else if (direct == SUBTRACTION) {
	  if (spent[choice] - temp < AU_Minval[choice]) {
	    errormsg("You may not sell back any more of that item");
	  } else {
	    spent[choice] -= temp;
	    bottommsg("You now have ");
	    dispitem(choice, spent[choice] * AU_values[choice]);
	    showitem(ypos + choice, choice);
	    points += AU_cost[choice];
	    clr = 1;
	  }
	}
	break;
      default:
	/* ignore the key */
	beep();
	break;
      }
    }

    /* check for save */
    if (no_save == FALSE) {

      /* create the new nation */
      if (makenpcs != TRUE) {

	/* create player nation storage file */
	sprintf(string, "%s.%s", AU_name, exetag);
	if ((fexe = fopen(string, "w")) == NULL) {
	  errormsg("ERROR: Could not create the nation information file!");
	  cq_reset();
	  return(0);
	}

	/* append to the news file */
	sprintf(string, "%s.%03d", newsfile, TURN - START_TURN);
	if( (fnews = fopen(string, "a")) == NULL ) {
	  errormsg("ERROR: Could not append to the news file");
	  cq_reset();
	  return(0);
	}

	/* now set the nation information and location */
	bottommsg("Creating your nation...");
	ntn_ptr = crt_ntn(AU_name, AU_active);
	country = global_int;
	convert();
	bottommsg("Placing your nation...");
	if (place(-1, -1) == FALSE) {
	  dest_ntn(AU_name);
	} else {

	  /* now update all of the starting powers for the nation */
	  for (i = 0; i < MAG_NUMBER; i++) {
	    global_long = ntn_ptr->powers[i];
	    ntn_ptr->powers[i] = 0L;
	    add_powers(i, global_long);
	  }
	  errormsg("Ok, your nation has been added to the world");

	  /* setup values for nation attributes */
	  upd_nations(country);
	  fclose(fexe);
	  
	  num_built++;

	}

	/* close and sort the news */
	fclose(fnews);
	sort_news(TURN);

      } else {

	/* store the new npc nation */
	if (write_npc() == FALSE) {
	  cq_reset();
	  return(0);
	}

      }

    }

    /* now check for more available nations */
  Done_Add:
    country = 0;
    for (i = 1; i < ABSMAXNTN; i++) {
      if ((ntn_ptr = world.np[i]) == NULL) {
	country = i;
	break;
      }
    }

    /* check to keep going */
    if (!world.verify_login ||
	(no_save == TRUE) ||
	(temp == 'n') ||
	(strcmp(LOGIN, loginname) == 0) ||
	(strcmp(world.demigod, loginname) == 0)) {
      /* check for more nations */
      if (country != 0 || makenpcs == TRUE) {
	bottommsg("Do you wish to add another nation? [ny]");
	more = y_or_n();
      } else {
	more = FALSE;
	errormsg("There are no more available nations");
      }
    } else {
      /* not able to build more than one nation */
      more = FALSE;
    }

  }
  bottommsg("Goodbye...");
  sleep(1);

  /* now save everything */
  cq_reset();
  return(num_built);
}

/* WRITE_NPC -- Store the nation for npcs; FALSE for bad read */
int
write_npc PARM_0(void)
{
  int count;

  /* check for the existance of the file */
  if (exists(npcfile) == 0) {

    /* append onto the npc file */
    if ((fexe = fopen(npcfile, "a")) == NULL) {
      errormsg("ERROR: Could not append to the NPC information file!");
      return(FALSE);
    }

  } else {

    /* create it and add a header */
    if ((fexe = fopen(npcfile, "w")) == NULL) {
      errormsg("ERROR: Could not create the NPC information file!");
      return(FALSE);
    }
    fprintf(fexe, "#\n");
    fprintf(fexe, "# Npc configuration file:\n#\n");
    fprintf(fexe, "#\tDO NOT MANUALLY EDIT THIS FILE!\n#\n");
    fprintf(fexe, "# Add more nations using the command:\n");
    fprintf(fexe, "#\tconqrun -A\n#\n");
    fprintf(fexe, "# Remove nations using the command:\n");
    fprintf(fexe, "#\tconqrun -Z\n#\n");

  }

  /* First store:
       the nation name, the leader name, the nation mark,
       the nation race, the nation class, and the activity value
   */
  fprintf(fexe, "%s %s %d %d %d %d\n",
	  AU_name, AU_lname, AU_mark, AU_race, AU_class, AU_active);

  /* Now store:
       all of the spending information
   */
  for (count = 0; count < AU_NUMBER; count++) {
    fprintf(fexe, "%d ", spent[count]);
  }
  fprintf(fexe, "\n");

  /* now close the npc file */
  fclose(fexe);
  return(TRUE);
}

/* READ_NPC -- Read in the specified number of nations */
void
read_npc PARM_1(int, amount)
{
  char line[BIGLTH], *line_ptr;
  long powers;
  int points, count, nations = 0;

  /* check for no processing */
  if (amount <= 0) {
    return;
  }

  /* check if the npcs file exists */
  if ((fexe = fopen(npcfile, "r")) == NULL) {

    /* check the default one */
#ifndef VMS
    sprintf(string, "%s/%s", defaultdir, npcfile);
#else
    sprintf(string, "%s%s", defaultdir, npcfile);
#endif /* VMS */
    if ((fexe = fopen(string, "r")) == NULL) {
      errormsg("Could not find any npc configurations; no NPCs created");
      return;
    }
    bottommsg("using default npc file...");

  }

  /* passwords set to the default */
  strcpy(AU_passwd, world.passwd);

  /* go through the entire data file */
  while ((nations < amount) && !feof(fexe)) {

    /* read in the line */
    if (fgets(line, BIGLTH - 1, fexe) == NULL) break;

    /* check commenting */
    if (line[0] == '#') continue;

    /* First read:
         the nation name, the leader name, the nation mark,
         the nation race, the nation class, and the activity value
     */
    if (sscanf(line, "%s %s %d %d %d %d",
	    AU_name, AU_lname, &AU_mark, &AU_race, &AU_class, &AU_active)
	!= 6) {
      sprintf(string, "Error: \"%s\" incomplete", line);
      errormsg(string);
      break;
    }

    /* Now read:
         all of the spending information
     */
    if (fgets(line, BIGLTH - 1, fexe) == NULL) {
      errormsg("Error: unexpected end of nations file");
      break;
    }
    line_ptr = &(line[0]);
    for (count = 0; count < AU_NUMBER; count++) {
      if (sscanf(line_ptr, "%d ", &(spent[count])) != 1) {
	sprintf(string, "Error: could not read %s value",
		AU_labels[count]);
	errormsg(string);
	break;
      }
      for (; *line_ptr != '\0'; line_ptr++) {
	if (!isdigit(*line_ptr)) break;
      }
    }
    if (count != AU_NUMBER) {
      break;
    }

    /* check the name */
    if (ntnbyname(AU_name) != NULL) {
      sprintf(string, "Nation name of %s already in use; skipping...\n",
	      AU_name);
      bottommsg(string);
      continue;
    }

    /* check the mark */
    while (!markok(AU_mark, AU_race, FALSE)) {
      AU_mark++;
      if (AU_mark > 255) AU_mark = 0;
    }

    /* initialize the class and race information */
    do_class(AU_class);
    race_setup(AU_race);

    /* get the point cost and compensate */
    points = MAXPTS;
    points -= point_cost();
    if (points > 0) {
      spent[AU_PEOPLE] += points / AU_cost[AU_PEOPLE];
    }

    /* now build it */
    sprintf(string, "Building %s nation %s\n",
	    race_info[AU_race].name, AU_name);
    bottommsg(string);
    ntn_ptr = crt_ntn(AU_name, AU_active);
    country = global_int;
    convert();
    if (place(-1, -1) == FALSE) {
      dest_ntn(AU_name);
      continue;
    }

    /* now provide the nation magical powers */
    for (count = 0; count < MAG_NUMBER; count++) {
      powers = ntn_ptr->powers[count];
      ntn_ptr->powers[count] = 0L;
      add_powers(count, powers);
    }

    /* continue with the next npc nation */
    nations++;
  }

  /* check how many were read in */
  if (nations != amount) {
    sprintf(string, "Only %d npc nations were available", nations);
    errormsg(string);
  }

  /* close up shop */
  fclose(fexe);
}
