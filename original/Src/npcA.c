/* routines to make a computer calculated update for a nation */
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
#include "moveX.h"
#include "activeX.h"
#include "statusX.h"

/* structure for use by NPC nations */
typedef struct s_npcinfo {
  long total_troops;	/* total number of troops for nation	*/
  long rov_troops;	/* how many troops still need assigning	*/
  long att_troops;	/*  for each of the tasks:  attack,	*/
  long def_troops;	/*  defense, garrison, and roving.	*/
  long gar_troops;
} NPCINFO_STRUCT, *NPCINFO_PTR;

/* sectors targetted for action */
typedef struct s_target {
  maptype mapx, mapy;		/* the location of the sector */
  int value;			/* the "target" value of the sector */
  struct s_target *next;	/* linked list information */
} TARGET_STRUCT, *TARGET_PTR;

/* storage for the NPC code */
TARGET_PTR target_list = NULL;
NPCINFO_STRUCT military_stats;

/* CPU_UPDATE -- computer controlled update of the nation */
static void
cpu_update PARM_0(void)
{
  /* give the outward indicator */
  fprintf(fupdate,
	  "    %s mode computer update being made (not fully implemented)\n",
	  aggressname[n_aggression(ntn_ptr->active)]);

#ifdef NOT_DONE
  /* determine strategy */
  assign_strategy();

  /* assign sector designations */
  sector_duties();

  /* reinforce garrisons and build new troops */
  buildup();

  /* set rovers */
  check_rovers();

  /* defensive set */
  stabilize();

  /* offensive set */
  expand();

  /* redistribute goods */
  npc_distribute();

  /* handle any constructions */
  npc_construct();
#endif /* NOT_DONE */
}

/* MOVE_FOR_NTN -- perform a turn of movement for the current nation */
void
move_for_ntn PARM_0(void)
{
  /* check data being worked on */
  if ((ntn_ptr == NULL) || (country == UNOWNED)) return;
  if (ntn_ptr != world.np[country]) {
    errormsg(" Serious Error:  nation pointer doesn't match country\n");
    return;
  }

  /* determine which form the update will take */
  if (n_ismonster(ntn_ptr->active)) {
    if (n_islizard(ntn_ptr->active)) {
      upd_lizards();
    } else if (n_issavage(ntn_ptr->active)) {
      upd_savages();
    } else if (n_ispirate(ntn_ptr->active)) {
      upd_pirates();
    } else if (n_isnomad(ntn_ptr->active)) {
      upd_nomads();
    } else {
      /* rebellion -- do nothing yet */
      fprintf(fupdate,
	      "    %s mode computer update being made (unimplemented)\n",
	      aggressname[n_aggression(ntn_ptr->active)]);
    }
  } else {
    /* computerized motion */
    cpu_update();
  }
}

/* ROVER_VALUE -- Determine the value of a sector to rover units */
static int
rover_value PARM_2(int, x, int, y)
{
  /* start with a simple ownership check */
  if (!XY_ONMAP(x, y) ||
      (sct[x][y].owner != UNOWNED)) {
    return(0);
  }

  /* just use attractiveness value for now */
  return(attract_val(x, y));
}

/* some variable to store rover information in */
static int rv_xloc, rv_yloc, rv_axloc, rv_ayloc;
static int rv_maxtotal, rv_maxneighbors, rv_mntotal;
static int rvn_total;

/* BUILD_ROVE -- Add in the rover total of the current sector */
static void
build_rove PARM_2(int, x, int, y)
{
  rvn_total += rover_value(x, y);
}

/* ROVE_LOOPFUNC -- Determine the best sector for relocation */
static void
rove_loopfunc PARM_2(int, x, int, y)
{
  /* get the value for this sector */
  int mytot = rover_value(x, y);

  /* compute the neighboring potential */
  rvn_total = 0;
  map_loop(x, y, 1, build_rove);

  /* Is this the best neighboring total? */
  if (rvn_total > rv_maxneighbors) {
    rv_maxneighbors = rvn_total;
    rv_mntotal = mytot;
    rv_axloc = x;
    rv_ayloc = y;
  }

  /* Is this the best total? */
  if (mytot > rv_maxtotal) {
    rv_maxtotal = mytot;
    rv_xloc = x;
    rv_yloc = y;
  }
}

/* ROVE_ARMY -- Take the current army unit and have it rove about */
void
rove_army PARM_0(void)
{
  int count = 0;

  /* stand still if the current sector is of interest */
  if ((army_ptr == NULL) ||
      (ARMY_MOVE < 5) ||
      rover_value(ARMY_XLOC, ARMY_YLOC)) return;

  /* set the movement mode appropriately */
  if (unit_flight(ARMY_STAT)) {
    movemode = MOVE_FLYARMY;
  } else {
    movemode = MOVE_ARMY;
  }

  /* check if they needed to back out */
  /* if (npc_retreat_army(army_ptr)) return; */

  /* determine the best location */
  rv_maxtotal = 0;
  rv_maxneighbors = 0;
  rv_mntotal = 0;
  map_loop(ARMY_XLOC, ARMY_YLOC, 1, rove_loopfunc);

  /* random movement if nothing found */
  if ((rv_maxtotal == 0) &&
      (rv_maxneighbors == 0)) {
    /* relocate to a random neighboring sector */
    do {
      rand_sector(ARMY_XLOC, ARMY_YLOC, 1, TRUE, FALSE);
    } while (!npc_movearmy(global_int, global_long) &&
	     (count++ < 100));
  } else if ((rv_maxtotal == 0) ||
	     (rv_maxtotal == rv_mntotal) ||
	     (rand_val(100) < (100 * rv_mntotal) / (rv_mntotal +
						    rv_maxtotal))) {
    /* go to the one with the highest neighboring total */
    npc_movearmy(rv_axloc, rv_ayloc);
  } else {
    /* go to the one with the highest total */
    npc_movearmy(rv_xloc, rv_yloc);
  }
}
