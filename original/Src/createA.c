/* This routines in this file actually create the world */
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
#include "navyX.h"
#include "buildA.h"
#include "desigX.h"
#include "magicX.h"
#include "racesX.h"
#include "mtrlsX.h"
#include "worldX.h"
#include "activeX.h"
#include "elevegX.h"
#include "statusX.h"
#include "tgoodsX.h"

/* macros to adjust contour */
#define TOVLY(x,y) if ( nmountains != 0 && sct[x][y].altitude == ELE_CLEAR) { \
sct[(x)][(y)].altitude = ELE_VALLEY; \
nmountains--; \
}
#define TOHILL(x,y) if( nmountains != 0 && sct[x][y].altitude == ELE_CLEAR) { \
sct[(x)][(y)].altitude=ELE_HILL; \
nmountains--; \
}
#define TOMT(x,y) if( nmountains != 0 && sct[x][y].altitude == ELE_CLEAR) { \
sct[(x)][(y)].altitude=ELE_MOUNTAIN; \
nmountains--; \
}
#define TOPEAK(x,y) if( nmountains != 0 && sct[x][y].altitude == ELE_CLEAR) { \
sct[(x)][(y)].altitude=ELE_PEAK; \
nmountains--; \
}

/* Value Map of Areas */
static char **area_map;
static char **type;
static char **tplace;

/* FILL_EDGE -- Subroutine to fill in a square edges with land or sea */
static void
fill_edge PARM_2 (int, AX, int, AY)
{
  /*      1)   water
   *      2)   water with major islands (25% land)
   *      3)   50/50 water/land
   *      4)   land with major water (75% Land)
   *      5)   land
   */
  register int i;
  int edgearea, X0, Y0, X1, Y1, X2, Y2, X3, Y3, X4, Y4;
  int area;

  X0 = AX;
  Y0 = AY;
  X1 = X0 - 1;
  Y1 = Y0;
  X2 = X0 + 1;
  Y2 = Y0;
  X3 = X0;
  Y3 = Y0 - 1;
  X4 = X0;
  Y4 = Y0 + 1;

  /* normalize for the edge of world */
  if( X1 < 0 ) X1 = MAXX - 1;
  if( X2 >= MAXX ) X2 = 0;
  if( Y3 < 0 ) Y3 = MAXY - 1;
  if( Y4 >= MAXY ) Y4 = 0;
  area = area_map[X0][Y0];

  /* fill in south */
  edgearea = area_map[X4][Y4];
  if (area + edgearea >= 6) {

    for (i = 0; i < 7; i++)
      type[X0 * 8 + i][Y0 * 8 + 7] = WORLD_LAND;

  } else if ((area + edgearea) >= 3) {

    for (i = 0; i < 7; i++)
      if (rand_val(2) == 0)
	type[(X0 * 8) + i][Y0 * 8 + 7] = WORLD_LAND;
      else type[(X0 * 8) + i][Y0 * 8 + 7] = WORLD_WATER;

  } else {

    for (i = 0; i < 7; i++)
      type[(X0 * 8) + i][Y0 * 8+  7] = WORLD_WATER;

  }

  /* fill in east */
  edgearea = area_map[X2][Y2];
  if (area + edgearea >= 6) {

    for (i = 1; i < 8; i++)
      type[X0 * 8 + 7][Y0 * 8 + i] = WORLD_LAND;

  } else if (area + edgearea >= 3) {

    for (i = 1; i < 8; i++)
      if (rand_val(2) == 0)
	type[X0 * 8 + 7][Y0 * 8 + i] = WORLD_LAND;
      else type[X0 * 8 + 7][Y0 * 8 + i] = WORLD_WATER;

  } else {

    for (i = 1; i < 8; i++)
      type[X0 * 8 + 7][Y0 * 8 + i] = WORLD_WATER;

  }

  /* fill in west */
  edgearea = area_map[X1][Y1];
  if (area + edgearea >= 6) {

    for (i = 0; i < 7; i++)
      type[X0 * 8][Y0 * 8 + i] = WORLD_LAND;

  } else if ( (area + edgearea) >= 3) {

    for (i = 0; i < 7 ; i++)
      if (rand_val(2) == 0)
	type[X0 * 8][Y0 * 8 + i] = WORLD_LAND;
      else type[X0 * 8][Y0 * 8 + i] = WORLD_WATER;

  } else {

    for (i = 0; i < 7; i++)
      type[X0 * 8][Y0 * 8 + i] = WORLD_WATER;

  }

  /* fill in north */
  edgearea = area_map[X3][Y3];
  if (area + edgearea >= 6) {

    for (i = 1; i < 8; i++)
      type[X0 * 8 + i][Y0 * 8] = WORLD_LAND;

  } else if (area + edgearea >= 3) {

    for (i = 1; i < 8; i++)
      if (rand_val(2) == 0)
	type[(X0 * 8) + i][Y0 * 8] = WORLD_LAND;
      else type[(X0 * 8) + i][Y0 * 8] = WORLD_WATER;

  } else {

    for (i = 1; i < 8; i++)
      type[(X0 * 8) + i][Y0 * 8] = WORLD_WATER;

  }

}

/* CR_ALTCOUNT -- Count the altitude value of the sector */
static void
cr_altcount PARM_2(int, x, int, y)
{
  /* count all altitudes but WATER */
  if (sct[x][y].altitude != ELE_WATER) {
    global_int += sct[x][y].altitude;
    global_long++;
  }
}

/* CR_TYPEWATER -- Eliminate initial peaks or valleys near water */
static void
cr_typewater PARM_2(int, x, int, y)
{
  if (type[x][y] == WORLD_WATER) {
    global_int++;
  }
}

/* CR_WATERCOUNT -- Eliminate initial peaks or valleys near water */
static void
cr_watercount PARM_2(int, x, int, y)
{
  if (sct[x][y].altitude == ELE_WATER) {
    global_int++;
  }
}

/* CR_SWAMPGROW -- Change a sector into a swamp */
static void
cr_swampgrow PARM_2(int, x, int, y)
{
  /* change into a swampland on random chance */
  if ((type[x][y] == WORLD_LAND) &&
      (rand_val(4) == 0) &&
      (sct[x][y].vegetation != VEG_DESERT) &&
      (sct[x][y].altitude < ELE_MOUNTAIN)) {
    if (rand_val(2)) {
      sct[x][y].vegetation = VEG_SWAMP;
    } else {
      sct[x][y].vegetation = VEG_JUNGLE;
    }
  }
}

/* CR_DESERTGROW -- Change a sector into a swamp */
static void
cr_desertgrow PARM_2(int, x, int, y)
{
  /* change into a swampland on random chance */
  if ((type[x][y] == WORLD_LAND) &&
      (rand_val(5) == 0) &&
      (sct[x][y].altitude < ELE_MOUNTAIN)) {
    if (rand_val(3)) {
      sct[x][y].vegetation = VEG_BARREN;
    } else {
      sct[x][y].vegetation = VEG_DESERT;
    }
  }
}

/* CREATEWORLD -- This routine generates a new world */
void
createworld PARM_0(void)
{
  int x, y, count, n, rnd;
  int number[5];		/* Number of sectors with type=[0-4] */
  int alloc = NUMAREAS * 2;	/* Value Allocated */
  int X, Y, chance;
  long nmountains, nwater;
  double avvalue;		/* Average water tvalue of sectors */
  register int i, j = 0;

  /* allocate the necessary memory */
  tplace = (char **) m2alloc(MAXX, MAXY, sizeof(char));
  area_map = (char **) m2alloc(MAXX, MAXY, sizeof(char));
  type = (char **) m2alloc(MAPX, MAPY, sizeof(char));
  sct = (SCT_PTR *) m2alloc(MAPX, MAPY, sizeof(SCT_STRUCT));

  /* make initializations */
  bottommsg("Day 1:  Let there be light!");
#ifndef DEBUG
  sleep(1);
#endif
  avvalue = (((double) (100 - PWATER)) / 25.0);
  for (i = 0; i < MAXX; i++)
    for (j = 0; j < MAXY; j++)
      tplace[i][j] = area_map[i][j] = 0;

  for (i = 0; i < 5; i++)
    number[i] = NUMAREAS / 5;  /* areas with type=[i] */
  number[2] = NUMAREAS - 4 * number[0]; /* correct for roundoff */

#ifdef DEBUG
  bottommsg("Allocating Areas");
#endif /* DEBUG */
  /* determine # of area types to areas */
  for (i = 0; i < 500; i++) {
    if ( (avvalue * NUMAREAS) > alloc) {
      /* still need to allocate more */
      x = rand_val(4);
      if (number[x] > 0) {
	number[x] = number[x] - 1;
	number[x+1] = number[x+1] + 1;
	alloc++;
      }
    } else {
      /* must allocate less */
      x = rand_val(4) + 1;
      if(number[x] > 0) {
	number[x] = number[x] - 1;
	number[x-1] = number[x-1] + 1;
	alloc--;
      }
    }
  }

#ifdef DEBUG
  bottommsg("Molding Map");
#endif /* DEBUG */
  /* go through and mold sections */
  i = 0;
  while ((number[4] > 0) && (i < 1000)) {
    i++;

    /* place a full land sector anywhere but on top/bottom edge */
    X = rand_val(MAXX);
    Y = rand_val(MAXY - 2) + 1;

    /* check if it is already placed */
    if (tplace[X][Y] != 0) continue;

    /* check around for neighboring blocks */
    if ((world.land_range > 0) && (world.preject_land > 0)) {
      chance = TRUE;
      for (x = X - world.land_range;
	   x <= X + world.land_range; x++)
	for (y = Y - world.land_range;
	     y <= Y + world.land_range; y++) {
	  if ((y >= 0) && (y < MAXY) &&
	      (tplace[(x + MAXX) % MAXX][y] != 0) &&
	      (rand_val(100) < world.preject_land)) {
	    chance = FALSE;
	    goto done_neighbor;
	  }
	}
    done_neighbor:
      if (chance == FALSE) continue;
    }

    /* place one solid land */
    tplace[X][Y] = 1;
    area_map[X][Y] = 4;
    number[4]--;

    /* place surrounding sectors */
    if (tplace[(X + 1) % MAXX][Y] == 0) {
      rnd = rand_val(100) + 1;
      if ((rnd < 25) && (number[4] > 0)) {
	area_map[(X + 1) % MAXX][Y] = 4;
	number[4]--;
	tplace[(X + 1) % MAXX][Y] = 1;
      }
      if ((rnd > 25) && (number[3] > 0)) {
	area_map[(X + 1) % MAXX][Y] = 3;
	number[3]--;
	tplace[(X + 1) % MAXX][Y] = 1;
      }
    }

    if (tplace[(X + MAXX - 1) % MAXX][Y] == 0) {
      rnd = rand_val(100) + 1;
      if ((rnd < 25) && (number[4] > 0)) {
	area_map[(X + MAXX - 1) % MAXX][Y] = 4;
	number[4]--;
	tplace[(X + MAXX - 1) % MAXX][Y] = 1;
      }
      if ((rnd > 25) && (number[3] > 0)) {
	area_map[(X + MAXX - 1) % MAXX][Y] = 3;
	number[3]--;
	tplace[(X + MAXX - 1) % MAXX][Y] = 1;
      }
    }

    if (tplace[X][Y+1] == 0) {
      rnd = rand_val(100) + 1;
      if ((rnd < 25) && (number[4] > 0)) {
	area_map[X][Y+1] = 4;
	number[4]--;
	tplace[X][Y+1] = 1;
      }
      if ((rnd > 25) && (number[3] > 0)) {
	area_map[X][Y+1] = 3;
	number[3]--;
	tplace[X][Y+1] = 1;
      }
    }

    if (tplace[X][Y-1] == 0) {
      rnd = rand_val(100) + 1;
      if ((rnd < 25) && (number[4] > 0)) {
	area_map[X][Y-1] = 4;
	number[4]--;
	tplace[X][Y-1] = 1;
      }
      if ((rnd > 25) && (number[3] > 0)) {
	area_map[X][Y-1] = 3;
	number[3]--;
	tplace[X][Y-1] = 1;
      }
    }

  }

  /* place all other areas */
  for (i = 0; i < MAXX; i++)
  for (j = 0; j < MAXY; j++) {

    while (tplace[i][j] == 0) {
      rnd = rand_val(5);
      if (number[rnd] > 0) {
	area_map[i][j] = rnd;
	number[rnd]--;
	tplace[i][j] = 1;
      }
    }

  }

#ifdef DEBUG
  bottommsg("Determining Sector Types");
#endif /* DEBUG */
  /* ALL AREAS PLACED, NOW DETERMINE SECTOR TYPE
   * fill in each area with sectors
   *      0)   water
   *      1)   water with major islands (25% land)
   *      2)   50/50 water/land
   *      3)   land with major water (75% Land)
   *      4)   land
   */
  for (Y = 0; Y < MAXY; Y++)
  for (X = 0; X < MAXX; X++) {

    /* fill in edges */
    fill_edge(X, Y);

    /* fill in center */
    for (i = 1; i < 7; i++) {
      for (j = 1; j < 7; j++)
	switch (area_map[X][Y]) {

	case 0:
	  if ( rand_val(100) < 95)
	    type[X * 8 + i][Y * 8 + j] = WORLD_WATER;
	  else
	    type[X * 8 + i][Y * 8 + j] = WORLD_HALF;
	  break;

	case 1:
	  if ( rand_val(2) == 0)
	    type[X * 8 + i][Y * 8 + j] = WORLD_HALF;
	  else
	    type[X * 8 + i][Y * 8 + j] = WORLD_WATER;
	  break;

	case 2:
	  if ( rand_val(2) == 0)
	    type[X * 8 + i][Y * 8 + j] = WORLD_LAND;
	  else
	    type[X * 8 + i][Y * 8 + j] = WORLD_WATER;
	  break;

	case 3:
	  if ( rand_val(2) == 0)
	    type[X * 8 + i][Y * 8 + j] = WORLD_HALF;
	  else
	    type[X * 8 + i][Y * 8 + j] = WORLD_LAND;
	  break;

	case 4:
	  if ( rand_val(100) < 95)
	    type[X * 8 + i][Y * 8 + j] = WORLD_LAND;
	  else
	    type[X * 8 + i][Y * 8 + j] = WORLD_HALF;
	  break;

	default:
	  errormsg("Uh oh!!!  The world has gone wacky.");
	  cq_reset();
	  abrt();
	}
    }
  }
#ifdef DEBUG
  bottommsg("Calculating 50% areas");
#endif /* DEBUG */

  /* calculate all 50% areas */
  for (i = 0; i < MAPX; i++)
  for (j = 0; j < MAPY; j++) {
    if (type[i][j] == WORLD_HALF)
      if (rand_val(2)) {
	type[i][j] = WORLD_LAND;
      } else {
	type[i][j] = WORLD_WATER;
      }
  }

  /* count the water */
  nwater = 0;
  for (i = 0; i < MAPX; i++)
    for (j = 0; j < MAPY; j++)
      if(type[i][j] == WORLD_WATER) {
	nwater++;
      }


  /* tell them where we are */
  fprintf(fnews, "1.\tThe world is created out of the void\n");
  bottommsg("Day 2:  God added water to the world");
#ifndef DEBUG
  sleep(2);
#endif
  sprintf(string, "...Water makes up %ld out of %d sectors", nwater, NUMSECTS);
  bottommsg(string);
  sleep(2);

  /* code to smooth out the world; balance to keep percentage */
  bottommsg("But, God was not pleased, and smoothed out the oceans");
  count = 0;
  if (world.hexmap) {
    rnd = 10;
  } else {
    rnd = 13;
  }
  while (count < world.smoothings) {

    /* around the world... hopefully quicker than 80 days */
    for (i = 0; i < MAPX; i++)
    for (j = 0; j < MAPY; j++) {

      /* 33% chance to smooth a sector */
      if (rand_val(3) == 0) {

	/* recalc the current percent */
	chance = (nwater * 100) / NUMSECTS - PWATER;

	/* count # of land and sea sides */
	global_int = 0;
	map_loop(i, j, 2, cr_typewater);

	/* now make sure of percentages */
	if (global_int < rnd + ((chance + 3) / 5)) {
	  if ((type[i][j] == WORLD_WATER) &&
	      (rand_val(2) == 0)) {
	    type[i][j] = WORLD_LAND;
	    nwater--;
	  }
	} else {
	  if ((type[i][j] == WORLD_LAND) &&
	      (rand_val(2) == 0)) {
	    type[i][j] = WORLD_WATER;
	    nwater++;
	  }
	}

      }

    }

    count++;
  }

  sprintf(string, "...Water now makes up %ld out of %d sectors (%ld%%)",
	  nwater, NUMSECTS, (nwater * 100) / NUMSECTS);
  bottommsg(string);
  fprintf(fnews, "1.\t ...water makes up %ld%% of the world\n",
	 (nwater * 100) / NUMSECTS);
  sleep(3);

  /* Adjust world given sectors as land or sea, place vegetation,
     designations, and altitude */

  /* now set the land and water */
  for (i = 0; i < MAPX; i++)
    for (j = 0; j < MAPY; j++) {
      if(type[i][j] == WORLD_LAND) {
	sct[i][j].altitude = ELE_CLEAR;
      } else {
	sct[i][j].altitude = ELE_WATER;
      }
      sct[i][j].vegetation = VEG_NONE;
      sct[i][j].tradegood = TG_NONE;
      sct[i][j].designation = MAJ_NONE;
      sct[i][j].owner = UNOWNED;
      sct[i][j].people = 0L;
      sct[i][j].minerals = 0;
    }

  /* find out how many mountains should be placed */
  count = NUMSECTS - nwater;
  nmountains = PMOUNT * count / 100;
  avvalue = ((double) nmountains) / NUMSECTS;
  bottommsg("Day 3:  God saw that the world was too flat and added contour");
#ifndef DEBUG
  sleep(3);
#endif
  sprintf(string, "...Hills and Mountains make up %ld out of %d sectors",
	  nmountains, NUMSECTS);
  bottommsg(string);

  /* heuristic says that 5 is cutoff number to stop placing ranges */
  /* and 1 third of mountains are placed as random hills */

#ifdef DEBUG
  bottommsg("Contouring the world");
#endif /* DEBUG */
  while (nmountains > 0) {

    /* go through the entire world, changing sectors */
    for (x = 0; x < MAPX; x++)
      for (y = 0; y < MAPY; y++) {
	if (sct[x][y].altitude == ELE_CLEAR) {
	  if (rand_val(100) < PMOUNT) {

	    /* place a mountain in this sector */
	    rnd = rand_val(100) + 1;
	    switch (area_map[x / 8][y / 8]) {

	    case 0:
	      /* heavy duty water */
	      if (rnd > 95) {
		TOMT(x, y);
	      } else {
		TOHILL(x, y);
	      }
	      break;

	    case 1:
	      /* mostly water */
	      if (rnd > 98) {
		TOPEAK(x, y);
	      } else if (rnd > 82) {
		TOMT(x, y);
	      } else if (rnd > 5) {
		TOHILL(x, y);
	      } else {
		TOVLY(x, y);
	      }
	      break;

	    case 2:
	      /* 50% / 50% land and water */
	      if (rnd > 94) {
		TOPEAK(x, y);
	      } else if (rnd > 75) {
		TOMT(x, y);
	      } else if (rnd > 8) {
		TOHILL(x, y);
	      } else {
		TOVLY(x, y);
	      }
	      break;

	    case 3:
	      /* mostly land */
	      if (rnd > 90) {
		TOPEAK(x, y);
	      } else if (rnd > 68) {
		TOMT(x, y);
	      } else if (rnd > 12) {
		TOHILL(x, y);
	      } else {
		TOVLY(x, y);
	      }
	      break;

	    case 4:
	      /* heavy duty land */
	      if (rnd > 88) {
		TOPEAK(x, y);
	      } else if (rnd > 60) {
		TOMT(x, y);
	      } else if (rnd > 15) {
		TOHILL(x, y);
	      } else {
		TOVLY(x, y);
	      }
	      break;

	    }

	  }
	}
      }

  }

  bottommsg("Smoothing the contours");
  count = 0;
  while (count < (world.smoothings + 1) / 2) {

    for (x = 0; x < MAPX; x++)
      for (y = 0; y < MAPY; y++) {

	/* ignore water sectors */
	if (sct[x][y].altitude == ELE_WATER) continue;

	/* count amount of contour */
	global_long = 0;
	global_int = 0;
	map_loop(x, y, 2, cr_altcount);

	/* adjust based on contour */
	if (rand_val(3) == 0) {

	  if (global_int / global_long < sct[x][y].altitude) {
	    if (rand_val(100) > PMOUNT)
	      sct[x][y].altitude--;
	  } else if (global_int / global_long > sct[x][y].altitude) {
	    if (rand_val(100) < PMOUNT)
	      sct[x][y].altitude++;
	  }

	}

      }

    count++;
  }

#ifdef DEBUG
  bottommsg("...check peak and valley locations");
#endif /* DEBUG */
  /* make sure no peak or valley is next to water */
  for (y = 1; y < MAPY - 1; y++)
    for (x = 1; x < MAPX - 1; x++)
      if ((sct[x][y].altitude == ELE_PEAK) ||
	  (sct[x][y].altitude == ELE_VALLEY)) {

	/* clean up out of place elevations */
	global_int = 0;
	map_loop(x, y, 1, cr_watercount);
	if (global_int != 0) {
	  if (rand_val(100) < PMOUNT) {
	    if (rand_val(3) == 0)
	      sct[x][y].altitude = ELE_MOUNTAIN;
	    else sct[x][y].altitude = ELE_HILL;
	  } else {
	    sct[x][y].altitude = ELE_CLEAR;
	  }
	}

      }

  /* count results */
  nmountains = 0;
  for (i = 0; i < MAPX; i++)
    for (j = 0; j < MAPY; j++)
      if ((sct[i][j].altitude != ELE_CLEAR) &&
	  (sct[i][j].altitude != ELE_WATER))
	nmountains++;
  sprintf(string,
	  "...Hills and Mountains are on %ld%% of land (%ld of %ld sectors)",
	  (nmountains * 100) / (NUMSECTS - nwater),
	  nmountains, NUMSECTS - nwater);
  bottommsg(string);
  fprintf(fnews, "1.\t ...%ld%% of the land is contoured\n",
	 (nmountains * 100) / (NUMSECTS - nwater));
  sleep(2);

  /* FIGURE OUT SECTOR VEGETATION TYPE
   * use sector.altitude, and sector to determine vegetation
   * from water is distance from nearest water
   */

#ifdef DEBUG
  bottommsg("...setting vegetation types");
#else
  sleep(2);
#endif /* DEBUG */
  for (i = 0; i < MAPX; i++)
    for (j = 0; j < MAPY; j++)
      if (type[i][j] == WORLD_LAND) {

	sct[i][j].vegetation = 3 + rand_val(5);
	/* if hill then decrement vegetation */
	if (sct[i][j].altitude == ELE_HILL) {

	  if (sct[i][j].vegetation != 0) sct[i][j].vegetation--;
	  if (area_map[i / 8][j / 8] < 1) {
	    if (rand_val(5) == 0)
	      sct[i][j].vegetation = VEG_VOLCANO;
	    else sct[i][j].vegetation = VEG_BARREN;
	  }

	} else if (sct[i][j].altitude == ELE_MOUNTAIN) {

	  if ( ( rand_val(6) == 4) && ((j > MAPY / 2 + 8) ||
				       (j < MAPY / 2 - 8)) )
	    sct[i][j].vegetation = VEG_ICE;
	  else sct[i][j].vegetation = 2 + rand_val(3);

	} else if (sct[i][j].altitude == ELE_PEAK) {

	  if ( (rand_val(2) == 0) && ((j > MAPY / 2 + 8) ||
				      (j < MAPY / 2 - 8)) )
	    sct[i][j].vegetation = VEG_ICE;
	  else sct[i][j].vegetation = VEG_VOLCANO;

	} else if (sct[i][j].altitude == ELE_VALLEY) {

	  if ( (rand_val(3) == 0) && ((j > MAPY / 2 + 8) ||
				      (j < MAPY / 2 - 8)) )
	    sct[i][j].vegetation = 3 + rand_val(5);
	  else sct[i][j].vegetation = 6 + rand_val(3) + rand_val(2);

	}
      }

#ifdef DEBUG
  bottommsg("...fixing up the polar areas");
#endif /* DEBUG */
  /* REWORK POLEAR/EQUATORIAL sector.vegetation */
  for (i = 0; i < MAPX; i++) {

    /* fix the northern pole */
    for (j = 0; j < 6; j++)
      if (type[i][j] == WORLD_LAND) {

	if (rand_val(4) == 0) {
	  sct[i][j].vegetation = VEG_ICE;
	} else if (sct[i][j].altitude == ELE_PEAK) {
	  if (rand_val(10) == 0) {
	    sct[i][j].vegetation = VEG_VOLCANO;
	  } else {
	    sct[i][j].vegetation = VEG_ICE;
	  }
	} else if ((n = sct[i][j].vegetation) > 2 && n != VEG_ICE) {
	  sct[i][j].vegetation--;
	} else if (n != VEG_ICE) {
	  sct[i][j].vegetation = VEG_TUNDRA;
	}

      }

    /* fix the southern pole */
    for (j = MAPY - 7; j < MAPY; j++)
      if (type[i][j] == WORLD_LAND) {

	if (rand_val(4) == 0) {
	  sct[i][j].vegetation = VEG_ICE;
	} else if (sct[i][j].altitude == ELE_PEAK) {
	  if (rand_val(10) == 0) {
	    sct[i][j].vegetation = VEG_VOLCANO;
	  } else {
	    sct[i][j].vegetation = VEG_ICE;
	  }
	} else if ((n = sct[i][j].vegetation) > 2 && n != VEG_ICE) {
	  sct[i][j].vegetation--;
	} else if (n != VEG_ICE) {
	  sct[i][j].vegetation = VEG_TUNDRA;
	}

      }

    /* Now insert the equator */
    for (j = (MAPY / 2) - 8; j <= (MAPY / 2) + 8; j++) {

      if (type[i][j] == WORLD_LAND)
	if (rand_val(10) == 0 && sct[i][j].altitude != ELE_PEAK) {
	  sct[i][j].vegetation = VEG_DESERT;
	} else {
	  /* increase vegetation of clear sectors */
	  if (sct[i][j].altitude == ELE_CLEAR &&
	      sct[i][j].vegetation >= VEG_TUNDRA &&
	      sct[i][j].vegetation <= VEG_JUNGLE && rand_val(4) == 0) {
	    sct[i][j].vegetation++;
	  }
	}

    }

    /* now go over the very central section */
    for (j = (MAPY / 2) - 2; j <= (MAPY / 2) + 2; j++) {

      /* only work on flat sectors */
      if (type[i][j] == WORLD_LAND && sct[i][j].altitude == ELE_CLEAR) {

	if (rand_val(10) == 0) sct[i][j].vegetation = VEG_DESERT;
	else if (rand_val(10) == 0) sct[i][j].vegetation = VEG_JUNGLE;
	else if (rand_val(10) == 0) sct[i][j].vegetation = VEG_SWAMP;
	else if (sct[i][j].vegetation >= VEG_TUNDRA &&
		 sct[i][j].vegetation <= VEG_LT_VEG) {
	  /* increment vegetation again, but only Waste to Light */
	  sct[i][j].vegetation++;
	}

      }
    }

  }

#ifdef DEBUG
  bottommsg("...expanding swamps and deserts");
#endif /* DEBUG */
  /* trip through the world */
  count = 0;
  while (count < (world.smoothings + 2) / 3) {

    for (i = 0; i < MAPX; i++)
      for (j = 0; j < MAPY; j++) {

	/* expand swampland */
	if ((sct[i][j].vegetation == VEG_SWAMP) ||
	    (sct[i][j].vegetation == VEG_JUNGLE)) {

	  /* expand outward */
	  map_loop(i, j, 1, cr_swampgrow);

	} else if (sct[i][j].vegetation == VEG_DESERT) {

	  /* expand deserts */
	  map_loop(i, j, 1, cr_desertgrow);

	}

      }

    count++;
  }

#ifdef DEBUG
  bottommsg("...performing final checks");
#endif /* DEBUG */
  /* final checks */
  for (i = 0; i < MAPX; i++)
    for (j = 0; j < MAPY; j++) {

      /* make all volcanos be on peaks */
      if (sct[i][j].vegetation == VEG_VOLCANO)
	sct[i][j].altitude = ELE_PEAK;

      /* make sure that no desert is next to water */
      if (sct[i][j].vegetation == VEG_DESERT) {

	if (sct[i][j].altitude == ELE_PEAK) {
	  if (rand_val(3) == 0) sct[i][j].vegetation = VEG_ICE;
	  else sct[i][j].vegetation = VEG_VOLCANO;
	} else if ((j < MAPY / 6) || (j > (MAPY * 5) / 6)) {
	  if (rand_val(3) == 0) sct[i][j].vegetation = VEG_TUNDRA;
	  else sct[i][j].vegetation = VEG_BARREN;
	} else {
	  global_int = 0;
	  map_loop(i, j, 1, cr_watercount);
	  if (global_int != 0) {
	    sct[i][j].vegetation = VEG_BARREN;
	  }
	}

      }

    }
  fprintf(fnews, "1.\t ...vegetation is placed upon the surface\n");

  /* now free up the space */
  free(tplace);
  free(type);
  free(area_map);
}

/* LIZ_TAKEIT -- Claim land in the name of the lizards! */
static void
liz_takeit PARM_2(int, x, int, y)
{
  /* snag any land; make some into swamp */
  if ((sct[x][y].altitude != ELE_WATER) &&
      (sct[x][y].altitude < ELE_MOUNTAIN)) {

    sct[x][y].owner = country;
    if (sct[x][y].altitude != ELE_HILL)
      sct[x][y].vegetation = VEG_SWAMP;

  }
}

/* BLD_LIZARDS -- Add any lizards cities to the world */
static void
bld_lizards PARM_0(void)
{
  int x, y, count = 0;
  int nlizards = 0, nwater;
  int archer_type, infantry_type;
  char rand_cname[NAMELTH+1];

  /* check if they need creating */
  if (world.lizards == 0) return;
  sprintf(string, "Building %d Lizards fortresses", world.lizards);
  bottommsg(string);

  /* make the backbone of the nation */
  if ((ntn_ptr = crt_ntn("lizard", NPC_LIZARD)) == NULL) {
    errormsg("Could not create the lizard nation");
    return;
  }
  country = global_int;

  /* now add lizard specifics */
  strcpy(ntn_ptr->leader, "S'lilth");
  ntn_ptr->mark = 'L';
  ntn_ptr->race = LIZARD;
  ntn_ptr->aplus = MERCATT;
  ntn_ptr->dplus = MERCDEF + 20;
  ntn_ptr->maxmove = 10;
  if ((infantry_type = unitbyname("Infantry")) == -1) {
    infantry_type = DEFAULT_ARMYTYPE;
  }
  if ((archer_type = unitbyname("Archers")) == -1) {
    archer_type = infantry_type;
  }

  /* provide protective and visual magic */
  ADDMAGIC(ntn_ptr->powers[MAG_WIZARDRY], MW_THEVOID);
  ADDMAGIC(ntn_ptr->powers[MAG_WIZARDRY], MW_ILLUSION);
  ADDMAGIC(ntn_ptr->powers[MAG_WIZARDRY], MW_HIDDEN);
  ADDMAGIC(ntn_ptr->powers[MAG_WIZARDRY], MW_KNOWALL);
  ADDMAGIC(ntn_ptr->powers[MAG_WIZARDRY], MW_SEEALL);
  ADDMAGIC(ntn_ptr->powers[MAG_WIZARDRY], MW_VISION);

  /* give them extra strength in swamps and jungles */
  ADDMAGIC(ntn_ptr->powers[MAG_CIVILIAN], MC_AMPHIBIAN);

  /* let them use archers... they are an intelligent race */
  ADDMAGIC(ntn_ptr->powers[MAG_MILITARY], MM_ARCHERY);

  /* now specify all of the interesting stuff */
  while (count++ < 1500 && nlizards < world.lizards) {

    /* find proper sector */
    if (rand_val(4) == 0) {
      /* 25% chance to be near equator */
      x = rand_val(MAPX);
      y = MAPY / 2 - 10 + rand_val(20);
    } else {
      /* elsewhere */
      x = rand_val(MAPX);
      y = rand_val(MAPY);
    }
    sct_ptr = &(sct[x][y]);

    /* verify that it is usable */
    if (sct_ptr->owner != UNOWNED ||
	sct_ptr->altitude == ELE_WATER ||
	sct_ptr->altitude == ELE_MOUNTAIN ||
	sct_ptr->altitude == ELE_PEAK) continue;

    /* check around to be sure of some land */
    global_int = 0;
    map_loop(x, y, 1, cr_watercount);

    /* should have some water but not alot */
    if (global_int > 7) continue;
    if (global_int == 0 && rand_val(15)) continue;

    /* configure the sector */
    set_majordesg(sct_ptr->designation, MAJ_STOCKADE);
    set_minordesg(sct_ptr->designation, MIN_FORTIFIED);
    sct_ptr->tradegood = TG_NONE;
    nwater = 55;
    while (sct_ptr->tradegood == TG_NONE) {
      nwater -= 5;
      sct_ptr->tradegood = rand_tgood(TG_JEWELS, nwater);
    }
    nwater = rand_val(nwater) + 2;
    sct_ptr->minerals = 10 + nwater;

    /* claim surrounding territory */
    map_loop(x, y, 1, liz_takeit);

    /* people must be able to use the mine */
    sct_ptr->vegetation = VEG_LT_VEG;

    /* make stockade storage and fortificaton consistant with jewels */
    do {
      random_name(rand_cname, LIZARD);
    } while (citybyname(rand_cname) != NULL);
    city_ptr = crt_city(rand_cname);
    if (city_ptr == NULL) abrt();
    CITY_XLOC = x;
    CITY_YLOC = y;
    CITY_STALONS = CITY_IMTRLS[MTRLS_TALONS] = CITY_MTRLS[MTRLS_TALONS] =
      100000L + nwater * 10000L + rand_val(100000);
    CITY_IMTRLS[MTRLS_JEWELS] = CITY_MTRLS[MTRLS_JEWELS] =
      10000L + nwater * 1000L + rand_val(10000);
    CITY_IMTRLS[MTRLS_FOOD] = CITY_MTRLS[MTRLS_FOOD] =
      20000L + nwater * 200L + rand_val(2000);
    CITY_FORT = 6 + nwater / 5;

    /* create the patrolling army */
    army_ptr = crt_army(infantry_type);
    ARMY_STAT = ST_DEFEND;
    set_speed(ARMY_STAT, SPD_NORMAL);
    ARMY_MOVE = 100;
    ARMY_LASTX = ARMY_XLOC = x;
    ARMY_LASTY = ARMY_YLOC = y;
    ARMY_SIZE = 750 + 100 * rand_val(10);

    /* create the garrison unit */
    army_ptr = crt_army(archer_type);
    set_speed(ARMY_STAT, SPD_STUCK);
    ARMY_MOVE = 75;
    ARMY_LASTX = ARMY_XLOC = x;
    ARMY_LASTY = ARMY_YLOC = y;
    ARMY_STAT = ST_GARRISON;
    ARMY_SIZE = 800 + 125 * rand_val(8) + 20 * nwater;

    /* built one city, now build the next */
    nlizards++;
  }

  /* built them lizards */
  fprintf(fnews, "1.\tGod created %d lizard fortresses\n",
	  nlizards);
}

/* BLD_SAVAGES -- Add any savage armies to the world */
static void
bld_savages PARM_0(void)
{
  int nsavages = 0, x, y, count, infantry_type;

  /* check if they need creating */
  if (world.savages == 0) return;
  sprintf(string, "Building %d savages and monsters", world.savages);
  bottommsg(string);

  /* make the backbone of the nation */
  if ((ntn_ptr = crt_ntn("savage", NPC_SAVAGE)) == NULL) {
    errormsg("Could not create the savage nation");
    return;
  }
  country = global_int;

  /* now add savage specifics */
  strcpy(ntn_ptr->leader, "Erkel");
  ntn_ptr->mark = '*';
  ntn_ptr->race = SAVAGE;
  ntn_ptr->aplus = MERCATT;
  ntn_ptr->dplus = MERCDEF;
  ntn_ptr->maxmove = 12;
  if ((infantry_type = unitbyname("Infantry")) == -1) {
    infantry_type = DEFAULT_ARMYTYPE;
  }

  /* provide visual magic */
  ADDMAGIC(ntn_ptr->powers[MAG_WIZARDRY], MW_KNOWALL);
  ADDMAGIC(ntn_ptr->powers[MAG_WIZARDRY], MW_SEEALL);
  ADDMAGIC(ntn_ptr->powers[MAG_WIZARDRY], MW_VISION);

  /* now specify all of the interesting stuff */
  count = 0;
  while (count++ < 1000 && nsavages < world.savages) {

    /* find proper sector */
    if (rand_val(3) == 0) {

      /* 33% chance to be on an edge */
      if (rand_val(3) == 0) {
	/* only 33% chance that it is in the ice caps */
	y = (MAPY - 16 + rand_val(32)) % MAPY;
	x = rand_val(MAPX);
      } else {
	/* otherwise they hover around the east-west junction */
	x = (MAPX - 20 + rand_val(40)) % MAPX;
	y = rand_val(MAPY);
      }

    } else {

      /* can be anywhere */
      x = rand_val(MAPX);
      y = rand_val(MAPY);

    }
    sct_ptr = &(sct[x][y]);

    /* verify that it is usable */
    if (sct_ptr->owner != UNOWNED ||
	sct_ptr->altitude == ELE_WATER ||
	sct_ptr->altitude == ELE_PEAK) continue;

    /* claim the sector */
    sct_ptr->owner = country;

    /* build the unit */
    if (rand_val(2) == 0) {

      /* 50% chance it is a savage infantry */
      army_ptr = crt_army(infantry_type);
      ARMY_STAT = ST_ATTACK;
      set_speed(ARMY_STAT, rand_val(SPD_STUCK));
      ARMY_MOVE = 100;
      ARMY_LASTX = ARMY_XLOC = x;
      ARMY_LASTY = ARMY_YLOC = y;
      ARMY_SIZE = 200 + 50 * rand_val(15);

    } else {

      /* create a random monster unit */
      army_ptr = crt_army(rand_monstunit(5000));
      ARMY_STAT = ST_ATTACK;
      set_speed(ARMY_STAT, rand_val(SPD_STUCK));
      ARMY_MOVE = 100;
      ARMY_LASTX = ARMY_XLOC = x;
      ARMY_LASTY = ARMY_YLOC = y;

      /* possible number based on the size */
      if (ainfo_list[ARMY_TYPE].minsth < 500) {
	ARMY_SIZE = rand_val(500 / ainfo_list[ARMY_TYPE].minsth) + 1;
      } else {
	ARMY_SIZE = 1;
      }

    }

    /* built one unit, now build the next */
    nsavages++;
  }

  /* built them savages */
  fprintf(fnews, "1.\t%d savage armies were scattered about the world\n",
	  nsavages);
}

/* BLD_NOMADS -- Add any nomad armies to the world */
static void
bld_nomads PARM_0(void)
{
  int nnomads = 0, x, y, count, dragoon_type, cav_type, hvycav_type;

  /* check if they need creating */
  if (world.nomads == 0) return;
  sprintf(string, "Building %d nomad armies", world.nomads);
  bottommsg(string);

  /* make the backbone of the nation */
  if ((ntn_ptr = crt_ntn("nomad", NPC_NOMAD)) == NULL) {
    errormsg("Could not create the nomadic nation");
    return;
  }
  country = global_int;

  /* now add savage specifics */
  strcpy(ntn_ptr->leader, "Ghenghis");
  ntn_ptr->mark = '*';
  ntn_ptr->race = NOMAD;
  ntn_ptr->aplus = MERCATT + 10;
  ntn_ptr->dplus = MERCDEF;
  ntn_ptr->maxmove = 16;
  if ((dragoon_type = unitbyname("Dragoons")) == -1) {
    dragoon_type = DEFAULT_ARMYTYPE;
  }
  if ((cav_type = unitbyname("Lt_Cavalry")) == -1) {
    cav_type = dragoon_type;
  }
  if ((hvycav_type = unitbyname("Hv_Cavalry")) == -1) {
    hvycav_type = cav_type;
  }

  /* provide visual magic */
  ADDMAGIC(ntn_ptr->powers[MAG_WIZARDRY], MW_KNOWALL);
  ADDMAGIC(ntn_ptr->powers[MAG_WIZARDRY], MW_SEEALL);
  ADDMAGIC(ntn_ptr->powers[MAG_WIZARDRY], MW_VISION);

  /* give dervish and equine powers */
  ADDMAGIC(ntn_ptr->powers[MAG_MILITARY], MM_EQUINE);
  ADDMAGIC(ntn_ptr->powers[MAG_CIVILIAN], MC_DERVISH);

  /* now specify all of the interesting stuff */
  count = 0;
  while (count++ < 1000 && nnomads < world.nomads) {

    /* find proper sector */
    if (rand_val(3) == 0) {

      /* 33% chance to be on an edge */
      if (rand_val(3) == 0) {
	/* only 33% chance that it is in the ice caps */
	y = (MAPY - 16 + rand_val(32)) % MAPY;
	x = rand_val(MAPX);
      } else {
	/* otherwise they hover around the east-west junction */
	x = (MAPX - 20 + rand_val(40)) % MAPX;
	y = rand_val(MAPY);
      }

    } else {

      /* can be anywhere */
      x = rand_val(MAPX);
      y = rand_val(MAPY);

    }
    sct_ptr = &(sct[x][y]);

    /* verify that it is usable */
    if (sct_ptr->owner != UNOWNED ||
	sct_ptr->altitude == ELE_WATER ||
	sct_ptr->altitude == ELE_PEAK) continue;

    /* claim the sector */
    sct_ptr->owner = country;

    /* build the unit */
    if (rand_val(2) == 0) {

      /* 50% chance it is a dragoon unit */
      army_ptr = crt_army(dragoon_type);
      ARMY_STAT = ST_ATTACK;
      set_speed(ARMY_STAT, SPD_SLOW);
      ARMY_MOVE = 100;
      ARMY_LASTX = ARMY_XLOC = x;
      ARMY_LASTY = ARMY_YLOC = y;
      ARMY_SIZE = 200 + 60 * rand_val(15);

    } else if (rand_val(5) == 0) {

      /* 10% chance it is a heavy cavalry unit */
      army_ptr = crt_army(hvycav_type);
      ARMY_STAT = ST_ATTACK;
      set_speed(ARMY_STAT, SPD_SLOW);
      ARMY_MOVE = 100;
      ARMY_LASTX = ARMY_XLOC = x;
      ARMY_LASTY = ARMY_YLOC = y;
      ARMY_SIZE = 300 + 80 * rand_val(10);

    } else {

      /* Otherwise, it is a light cavalry unit */
      army_ptr = crt_army(cav_type);
      ARMY_STAT = ST_ATTACK;
      set_speed(ARMY_STAT, rand_val(3));
      ARMY_MOVE = 100;
      ARMY_LASTX = ARMY_XLOC = x;
      ARMY_LASTY = ARMY_YLOC = y;
      ARMY_SIZE = 225 + 50 * rand_val(15);

    }

    /* built one unit, now build the next */
    nnomads++;
  }

  /* now them nomad varmints are here */
  fprintf(fnews, "1.\t%d nomad tribes were placed upon the earth\n",
	  nnomads);
}

/* PIR_OKSECT -- Check if the sector is liked by pirates */
static void
pir_oksect PARM_2(int, x, int, y)
{
  if (sct[x][y].owner != UNOWNED) {
    global_int = FALSE;
  } else if (sct[x][y].altitude != ELE_WATER) {
    if (rand_val(2) == 0) global_int = FALSE;
  }
}

/* PIR_TAKEIT -- Clean up surrounding land */
static void
pir_takeit PARM_2(int, x, int, y)
{
  sct[x][y].vegetation = VEG_NONE;
  sct[x][y].altitude = ELE_WATER;
  sct[x][y].tradegood = TG_NONE;
  sct[x][y].minerals = 0;
}

/* BLD_PIRATES -- Add any pirate bases to the world */
static void
bld_pirates PARM_0(void)
{
  int npirates = 0, x, y, i, valid, count, infantry_type;
  char rand_cname[NAMELTH+1];

  /* check if they need creating */
  if (world.pirates == 0) return;
  sprintf(string, "Building %d pirate bases", world.pirates);
  bottommsg(string);

  /* make the backbone of the nation */
  if ((ntn_ptr = crt_ntn("pirate", NPC_PIRATE)) == NULL) {
    errormsg("Could not create the pirate nation");
    return;
  }
  country = global_int;

  /* now add savage specifics */
  strcpy(ntn_ptr->leader, "Redbeard");
  ntn_ptr->mark = 'P';
  ntn_ptr->race = PIRATE;
  ntn_ptr->aplus = MERCATT + 20;
  ntn_ptr->dplus = MERCDEF - 10;
  ntn_ptr->maxmove = 15;
  if ((infantry_type = unitbyname("Infantry")) == -1) {
    infantry_type = DEFAULT_ARMYTYPE;
  }

  /* provide visual magic as well as some protective */
  ADDMAGIC(ntn_ptr->powers[MAG_WIZARDRY], MW_KNOWALL);
  ADDMAGIC(ntn_ptr->powers[MAG_WIZARDRY], MW_SEEALL);
  ADDMAGIC(ntn_ptr->powers[MAG_WIZARDRY], MW_VISION);
  ADDMAGIC(ntn_ptr->powers[MAG_WIZARDRY], MW_ILLUSION);
  ADDMAGIC(ntn_ptr->powers[MAG_WIZARDRY], MW_HIDDEN);

  /* provide the nation with sailor and marine power */
  ADDMAGIC(ntn_ptr->powers[MAG_CIVILIAN], MC_SAILOR);
  ADDMAGIC(ntn_ptr->powers[MAG_CIVILIAN], MC_MARINE);

  /* now specify all of the interesting stuff */
  count = 0;
  while (count++ < 2000 && npirates < world.pirates) {

    /* find proper sector */
    if (rand_val(3) == 0) {

      /* 33% chance to be near the equator */
      y = MAPY / 2 - 10 + rand_val(20);
      x = rand_val(MAPX);

    } else {

      /* can be anywhere */
      x = rand_val(MAPX);
      y = rand_val(MAPY);

    }
    sct_ptr = &(sct[x][y]);

    /* verify that it is usable */
    if (sct_ptr->owner != UNOWNED ||
	sct_ptr->altitude == ELE_PEAK) continue;

    /* 10% chance to build an non-polar island in water */
    if (sct_ptr->altitude == ELE_WATER &&
	((y < MAPY / 5) || (y > (MAPY * 4) / 5) ||
	 (rand_val(10) == 0))) continue;

    /* verify the chance of making it an island */
    global_int = TRUE;
    map_loop(x, y, 1, pir_oksect);

    /* now try again if it cannot be made */
    if (global_int == FALSE) continue;

    /* make it an island with metal on it */
    map_loop(x, y, 1, pir_takeit);

    /* island mountain fortress */
    sct_ptr->owner = country;
    sct_ptr->altitude = ELE_MOUNTAIN;
    sct_ptr->vegetation = VEG_LT_VEG;

    /* now provide some incentive */
    sct_ptr->tradegood = TG_NONE;
    valid = 55;
    while (sct_ptr->tradegood == TG_NONE) {
      valid -= 5;
      sct_ptr->tradegood = rand_tgood(TG_METALS, valid);
    }
    valid = rand_val(valid) + 2;
    sct_ptr->minerals = 10 + valid;

    /* beef it up */
    set_majordesg(sct_ptr->designation, MAJ_STOCKADE);
    set_minordesg(sct_ptr->designation, MIN_FORTIFIED);
    set_minordesg(sct_ptr->designation, MIN_HARBOR);

    /* make stockade storage and fortificaton consistant with metals */
    do {
      random_name(rand_cname, PIRATE);
    } while (citybyname(rand_cname) != NULL);
    city_ptr = crt_city(rand_cname);
    if (city_ptr == NULL) abrt();
    CITY_XLOC = x;
    CITY_YLOC = y;
    CITY_STALONS = CITY_IMTRLS[MTRLS_TALONS] = CITY_MTRLS[MTRLS_TALONS] =
      100000L + valid * 10000L + rand_val(100000);
    CITY_IMTRLS[MTRLS_METALS] = CITY_MTRLS[MTRLS_METALS] =
      10000L + valid * 1000L + rand_val(10000);
    CITY_IMTRLS[MTRLS_FOOD] = CITY_MTRLS[MTRLS_FOOD] =
      20000L + valid * 200L + rand_val(2000);
    CITY_FORT = 6 + valid / 5;

    /* build the infantry garrison */
    army_ptr = crt_army(infantry_type);
    ARMY_STAT = ST_GARRISON;
    set_speed(ARMY_STAT, SPD_STUCK);
    ARMY_MOVE = 75;
    ARMY_LASTX = ARMY_XLOC = x;
    ARMY_LASTY = ARMY_YLOC = y;
    ARMY_SIZE = 400 + 20 * valid;

    /* build the pirate fleet */
    navy_ptr = crt_navy();

    /* add ships (assume no errors) */
    for (i = 0; i <= N_HEAVY; i++) {
      NAVY_SHIPS[NSHP_WARSHIPS] =
	navy_addships(NAVY_SHIPS[NSHP_WARSHIPS], i, rand_val(4 - i) + 2 - i);
    }
    NAVY_LASTX = NAVY_XLOC = x;
    NAVY_LASTY = NAVY_YLOC = y;
    NAVY_CREW = 100;
    NAVY_STAT = ST_ENGAGE;
    NAVY_MOVE = 100;

    /* built pirate base, now build the next */
    npirates++;
  }

  /* pesky pirates are here */
  fprintf(fnews, "1.\tGod placed %d pirate coves upon the oceans\n",
	  npirates);
}

/* POPULATE -- allocate populations of the world */
static void
populate PARM_0(void)
{
  FILE *fp;
  int i;
  char fname[FILELTH + 1];

  /* create any lizard cities */
  bld_lizards();

  /* create any savage tribes */
  bld_savages();

  /* create any nomad tribes */
  bld_nomads();

  /* create any pirate bases */
  bld_pirates();

  /* create the npc nations */
  read_npc(world.npcs);

  /* now confirm the helpfiles */
  for (i = 0; i < help_number; i++) {

#ifndef VMS
    sprintf(fname,"%s/%s.doc", helpdir, help_files[i]);
#else
    sprintf(fname,"%s%s.doc", helpdir, help_files[i]);
#endif /* VMS */
    if ((fp = fopen(fname, "r")) == NULL) {
      sprintf(string, "Warning: cannot find helpfile <%s>.", fname);
      errormsg(string);
    }
    if (fp != NULL) fclose(fp);

  }

#ifdef UNIMP
  /* check for the rules file */
#ifndef VMS
  sprintf(fname, "%s/%s", defaultdir, rulesfile);
#else
  sprintf(fname, "%s%s", defaultdir, rulesfile);
#endif /* VMS */
  if ((fp = fopen(fname, "r")) == NULL) {
    sprintf(string, "warning: cannot find rulesfile <%s>.", fname);
    errormsg(string);
  }
  if (fp != NULL) fclose(fp);
#endif /* UNIMP */
}

/* RAWMATERIALS -- Place the jewels, metals and tradegoods */
void
rawmaterials PARM_0(void)
{
  register int x, y;
  int i, rnd;
  int valid;
  struct s_sector *sptr;

  bottommsg("Day 4:  God placed vegetation and raw materials upon the world");
#ifndef DEBUG
  sleep(2);
#endif

  /* go through all sectors */
  for (y = 0; y < MAPY; y++)
    for (x = 0; x < MAPX; x++) {

      /* get quick reference */
      sptr = &sct[x][y];

      /* exotic trade goods !!! */
      if (rand_val(100) < PTRADE) {

	valid = FALSE;
	rnd = rand_val(100);

	/* now find out which tradegood */
	if ((rnd < PTGMETAL) ||
	    (sptr->altitude == ELE_MOUNTAIN)) {
	  /* give metals */
	  getmetal (sptr);
	} else if (rnd < PTGMETAL + PTGJEWEL) {
	  /* give jewels */
	  getjewel (sptr);
	} else if (rnd < PTGMETAL + PTGJEWEL + PTGSPELL) {
	  /* give magics */
	  getspell (sptr);
	} else while (valid == FALSE ) {
	  /* some other random good */
	  i = rand_tgood(rand_val(TG_TERROR) + 1, 0);

	  /* fish	- next to water */
	  if (tg_fishing(i)) {
	    if (tofood(sptr, 0) < 4)
	      continue;
	    global_int = 0;
	    map_loop(x, y, 1, cr_watercount);
	    if (global_int == 0)
	      continue;
	  }

	  /* corn,fruit - should be arable land */
	  if ((tg_eatrate(i) ||
	       tg_farming(i)) &&
	      (tofood(sptr, 0) < 5))
	    continue;

	  /* timber,pine,oak - wood/forest */
	  if (tg_islumber(i) &&
	      (sptr->vegetation != VEG_FOREST) &&
	      (sptr->vegetation != VEG_WOOD))
	    continue;

	  valid = TRUE;
	  sptr->tradegood = i;
	}

	/* check for wierd error */
	if (sptr->tradegood == TG_NONE)
	  errormsg("??? DEBUG -> tradegood == NONE ???");
      }
    }

  /* now provide the population */
  bottommsg("Day 5:  God decreed that world would be populated");
#ifndef DEBUG
  sleep(2);
#endif
  bottommsg("...All manner of creatures were created: big ones, little ones,");
#ifndef DEBUG
  sleep(2);
#endif
  bottommsg("fat ones, skinny ones, orange ones, turquoise ones, bright blue ones.");
#ifndef DEBUG
  sleep(2);
#endif
  bottommsg("WAIT!!!  God suddenly realized that smurfs were taking things too far");
#ifndef DEBUG
  sleep(2);
#endif
  bottommsg("and stopped creating new ones to place everybody on the map...");
#ifndef DEBUG
  sleep(2);
#endif

  /* now put the population in */
  populate();

  bottommsg("Day 6:  God, believing in long weekends, went out and got smashed");
#ifndef DEBUG
  sleep(2);
#endif
  bottommsg("Day 7:  God rested (to get rid of that stupid hangover)");
#ifndef DEBUG
  sleep(2);
#endif
  strcpy(string, "... Log in via 'conquer -n god");
  if (strcmp(datadirname, "[default]") != 0) {
    strcat(string, " -d ");
    strcat(string, datadirname);
  }
  strcat(string, "'");
  errormsg(string);
  strcpy(string, "... Players may be added via 'conqrun -a");
  if (strcmp(datadirname, "[default]") != 0) {
    strcat(string, " -d ");
    strcat(string, datadirname);
  }
  strcat(string, "'");
  errormsg(string);
}
