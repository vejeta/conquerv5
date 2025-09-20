// SPDX-License-Identifier: GPL-3.0-or-later
/* routines concerning the relocation of units between sectors */
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
#include "dataX.h"
#include "armyX.h"
#include "moveX.h"
#include "navyX.h"
#include "desigX.h"
#include "magicX.h"
#include "elevegX.h"
#include "hlightX.h"
#include "statusX.h"
#include "caravanX.h"
#include "dstatusX.h"

/* movement local variables */
static int mp_potential = 1;
int xhome, yhome, range_limit;
int mult_10 = FALSE;

/* MV_CNTWATER -- Count the water sectors */
static void
mv_cntwater PARM_2(int, x, int, y)
{
  if (sct[x][y].altitude == ELE_WATER) {
    global_int++;
  }
}

/* MOVE_COST -- Routine to calculate the move cost for a specified unit type */
int
move_cost PARM_3( int, x, int, y, int, type )
{
  SCT_PTR shold_ptr = sct_ptr;
  static int hold = 0;
  static int ox = -1, oy = -1, otype = -1;
  int rtype, vegcost, elecost, sown, dstat, dstat2;

  /* check sector location */
  if (!Y_ONMAP(y)) {
    return(MV_OFFMAP);
  } else if ((is_god == TRUE) &&
	     (country != UNOWNED)) {
    return(0);
  }
  if ((ox == x) &&
      (oy == y) &&
      (otype == type)) {
    return(hold);
  }

  /* provide quick information reference */
  sct_ptr = &(sct[(x + MAPX) % MAPX][y]);
  sown = sct_ptr->owner;
  if (sown == UNOWNED) dstat = DIP_NEUTRAL;
  else dstat = (world.np[sown])->dstatus[country] % dstatus_number;
  if ((country == UNOWNED) || (ntn_ptr == NULL)) {
    dstat2 = DIP_NEUTRAL;
    rtype = 0;
  } else {
    dstat2 = ntn_ptr->dstatus[sown] % dstatus_number;
    rtype = ntn_ptr->race;
  }

  /* check movement according to method used */
  switch (type) {
  case MOVE_ONEWAY:
  case MOVE_ARMY:
  case MOVE_CVN:
    /* is movement to this sector allowed? */
    if (type == MOVE_ONEWAY) {
      if (((x + MAPX) % MAPX != xhome) ||
	  (y != yhome)) {
	hold = MV_ONEWAY;
	break;
      }
    }

    /* check for water sectors */
    if (sct_ptr->altitude == ELE_WATER) {

      /* bridges are accessible to any nation */
      if (major_desg(sct_ptr->designation) == MAJ_BRIDGE) {
	if (minor_desg(sct_ptr->designation, MIN_DEVASTATED)) {
	  hold = MV_BADBRIDGE;
	} else {
	  hold = 2;
	}
      } else {
	/* otherwise, water can't be crossed */
	hold = MV_WATER;
      }

    } else {
      /* otherwise use standard land calculations */

      /* first find vegetation cost */
      if ((country != UNOWNED) &&
	  (CIV_MAGIC(MC_DERVISH) || WIZ_MAGIC(MW_DESTROYER)) &&
	  ((sct_ptr->vegetation == VEG_ICE) ||
	   (sct_ptr->vegetation == VEG_DESERT))) {
	vegcost = 1;
      } else if ((country != UNOWNED) &&
		 CIV_MAGIC(MC_AMPHIBIAN) &&
		 ((sct_ptr->vegetation == VEG_SWAMP) ||
		  (sct_ptr->vegetation == VEG_JUNGLE))) {
	vegcost = 1;
      } else if ((country != UNOWNED) &&
		 WIZ_MAGIC(MW_FIRE) &&
		 (sct_ptr->vegetation == VEG_VOLCANO)) {
	vegcost = 4;
      } else {
	vegcost = veg_info[sct_ptr->vegetation].move_cost[rtype];
      }

      /* now find the elevation movement cost */
      elecost = ele_info[sct_ptr->altitude].move_cost[rtype];
      if ((vegcost == (-1)) || (elecost == (-1))) {
	hold = MV_IMPASSABLE;
	break;
      }

      /* now do the final calculations */
      hold = vegcost + elecost;
      if ((major_desg(sct_ptr->designation) == MAJ_CANAL) ||
	  (major_desg(sct_ptr->designation) == MAJ_WALL)) {
	/* increased move due to canal or wall placement */
	hold *= 2;
      }
      if (minor_desg(sct_ptr->designation, MIN_ROADS)) {
	/* decreased move due to roads */
	hold = (hold + 1) / 2;
      }
    }

    /* check for diplomatic intrusions */
    if ((sown == UNOWNED) ||
	(is_god == TRUE) ||
	(sown == country)) break;
    if (dstat == DIP_UNMET) {
      hold = MV_UNMET;
      break;
    }
    if ((type != MOVE_CVN) &&
	(dstat > DIP_TREATY) &&
	(dstat2 < DIP_WAR)) {
      if ((army_ptr == NULL) ||
	  (!a_isscout(ARMY_TYPE) &&
	   (!a_isleader(ARMY_TYPE) ||
	    unit_leading(ARMY_STAT)))) {
	hold = MV_ACTOFWAR;
	break;
      }
      if ((dstat >= DIP_NEUTRAL) &&
	  (dstat2 < DIP_HOSTILE)) {
	hold = MV_HOSTILESCOUT;
      }
    }

    /* caravans cannot enter hostile territory for any reason */
    if ((type == MOVE_CVN) &&
	(dstat >= DIP_HOSTILE)) {
      hold = MV_HOSTILE;
    }
    break;
  case MOVE_NAVY:
    /* count the number of neighboring water sectors */
    if (sct_ptr->altitude == ELE_WATER) {

      /* first check previous sector */
      if (movemode == MOVE_NAVY) {
	if ((sct[XREAL][YREAL].altitude != ELE_WATER) &&
	    (major_desg(sct[XREAL][YREAL].designation) != MAJ_CANAL) &&
	    !IS_HARBOR(sct[XREAL][YREAL].designation)) {
	  /* cannot enter a water sector from different direction */
	  if (map_within(XREAL, YREAL, x, y, 1) &&
	      ((XREAL != NAVY_LASTX) || (YREAL != NAVY_LASTY)) &&
	      (((x + MAPX) % MAPX != NAVY_LASTX) || (y != NAVY_LASTY))) {
	    hold = MV_NOACCESS;
	    break;
	  }
	}
      }

      /* count amount of neighboring water */
      global_int = -1;
      map_loop(x, y, 2, mv_cntwater);

      /* movement is based on number of neighboring water sectors */
      /* switch statement is faster than a formula */
      if (world.hexmap) {
	switch (abs(global_int - 9)) {
	case 9:
	  /* totally surrounded by either water or land */
	  hold = 5;
	  break;
	case 8:
	case 7:
	  /* mostly land or mostly water */
	  hold = 4;
	  break;
	case 6:
	case 5:
	case 4:
	  /* more land or water than others */
	  hold = 3;
	  break;
	case 3:
	case 2:
	  /* a bit more of one than the other */
	  hold = 2;
	  break;
	case 1:
	case 0:
	default:
	  /* even distribution of land or water */
	  hold = 1;
	  break;
	}
      } else {
	switch (abs(global_int - 12)) {
	case 12:
	  /* totally surrounded by either water or land */
	  hold = 5;
	  break;
	case 11:
	case 10:
	case 9:
	  /* alot of either land or water */
	  hold = 4;
	  break;
	case 8:
	case 7:
	case 6:
	case 5:
	  /* more of land or water */
	  hold = 3;
	  break;
	case 4:
	case 3:
	case 2:
	  /* slightly more of one or the other */
	  hold = 2;
	  break;
	case 1:
	case 0:
	default:
	  /* even distribution */
	  hold = 1;
	  break;
	}
      }

    } else {

      /* first check previous sector */
      if (movemode == MOVE_NAVY) {
	if ((sct[XREAL][YREAL].altitude != ELE_WATER) &&
	    (major_desg(sct[XREAL][YREAL].designation) != MAJ_CANAL) &&
	    !IS_HARBOR(sct[XREAL][YREAL].designation)) {
	  /* cannot pull ship down the beach */
	  hold = MV_DRAG;
	  break;
	}
      }

      /* check for unmet land */
      if ((is_god == FALSE) &&
	  (sown != country) &&
	  (sown != UNOWNED)) {
	if (dstat == DIP_UNMET) {
	  hold = MV_UNMET;
	  break;
	}
	if (dstat >= DIP_HOSTILE && dstat2 < DIP_HOSTILE) {
	  hold = MV_HOSTILE;
	  break;
	}
      }

      /* check for move cost in land sectors */
      if ((sown == UNOWNED) ||
	  (sown != country && dstat > DIP_NEUTRAL) ||
	  !IS_HARBOR(sct_ptr->designation)) {

	/* check for accessible canals */
	if (major_desg(sct_ptr->designation) == MAJ_CANAL) {
	  if ((sown == country) ||
	      (dstat < DIP_HOSTILE)) {
	    if ((sct[XREAL][YREAL].altitude == ELE_WATER) ||
		(movemode != MOVE_NAVY) ||
		(major_desg(sct[XREAL][YREAL].designation) == MAJ_CANAL) ||
		IS_HARBOR(sct[XREAL][YREAL].designation)) {
	      hold = 2;
	      break;
	    }
	  }
	}

	/* first check for mountainous / peak sectors */
	if (sct_ptr->altitude == ELE_PEAK) {

	  /* can't land on a mountain peak! */
	  hold = MV_GROUND;

	} else {

	  /* check neighboring water sectors  */
	  global_int = 0;
	  map_loop(x, y, 1, mv_cntwater);

	  /* inaccessible sector */
	  if (global_int == 0) {
	    hold = MV_GROUND;
	    break;
	  }

	  /* query for landing */
	  hold = MV_LANDING;

	}

      } else {

	/* cost to move into a harbor */
	hold = 4;

      }
    }
    break;
  case MOVE_PEOPLE:
  case MOVE_TELEPORT:
    /* may move anywhere within a specified range */
    if ((is_god == FALSE) &&
	!map_within(xhome, yhome, x, y, range_limit)) {
      hold = MV_TOOFAR;
      break;
    }
    if ((is_god == FALSE) &&
	(VIS_CHECK(x, y) == HS_NOSEE)) {
      hold = MV_UNSEEN;
      break;
    }

    /* check for water sectors teleportation and bridges */
    if (type == MOVE_TELEPORT) {
      hold = 1;
      break;
    } else if (sct_ptr->altitude == ELE_WATER) {
      if (major_desg(sct_ptr->designation) == MAJ_BRIDGE) {
	if (minor_desg(sct_ptr->designation, MIN_DEVASTATED)) {
	  hold = MV_BADBRIDGE;
	} else {
	  hold = 1;
	}
      } else {
	hold = MV_WATER;
	break;
      }
    } else if ((is_god == FALSE) &&
	       (tofood(sct_ptr, country) == 0)) {
      hold = MV_INHABITABLE;
      break;
    }
    if ((is_god == FALSE) &&
	(sown != country) &&
	(sown != UNOWNED) &&
	(dstat != DIP_ALLIED)) {
      hold = MV_OTHNATION;
    } else {
      hold = 1;
    }
    break;
  case MOVE_FLYARMY:
  case MOVE_FLYCVN:
    /* find movement cost for flying units */

    /* vegetation cost */
    if ((country != UNOWNED) &&
	(CIV_MAGIC(MC_DERVISH) || WIZ_MAGIC(MW_DESTROYER)) &&
	((sct_ptr->vegetation == VEG_ICE) ||
	 (sct_ptr->vegetation == VEG_DESERT))) {
      vegcost = 0;
    } else {
      vegcost = veg_info[sct_ptr->vegetation].flight_cost;
      if (vegcost == (-1)) {
	hold = MV_IMPASSABLE;
	break;
      }
    }

    /* elevation cost */
    elecost = ele_info[sct_ptr->altitude].flight_cost;
    if (elecost == (-1)) {
      hold = MV_IMPASSABLE;
      break;
    }

    /* collect the results */
    hold = vegcost + elecost;

    /* check for diplomatic airspace violations */
    if (sown == UNOWNED || sown == country) break;
    if (dstat == DIP_UNMET) {
      hold = MV_UNMET;
      break;
    }
    if ((dstat > DIP_TREATY) &&
	(dstat2 < DIP_WAR)) {
      if ((army_ptr == NULL) ||
	  (!a_isscout(ARMY_TYPE) &&
	   (!a_isleader(ARMY_TYPE) ||
	    unit_leading(ARMY_STAT)))) {
	hold = MV_ACTOFWAR;
	break;
      }
      if (a_isscout(ARMY_TYPE) &&
	  (dstat <= DIP_HOSTILE) &&
	  (dstat2 < DIP_HOSTILE)) {
	hold = MV_HOSTILESCOUT;
	break;
      }
    }
    break;
  case MOVE_PATROL:
    /* movement along owned walls only */
    if ((sown == country || dstat == DIP_ALLIED) &&
	major_desg(sct_ptr->designation) == MAJ_WALL) {
      hold = 1;
    } else {
      hold = MV_NOTWALL;
    }
    break;
  default:
    errormsg("Okay, who goofed?");
    hold = MV_IMPASSABLE;
    break;
  }

  /* figure out a percentage */
  if ((movemode != MOVE_NOMOVE) &&
      (hold > 0) &&
      (mp_potential > 0)) {
    if (mult_10) hold *= 10;
    hold *= 100;
    hold += (mp_potential - 4);
    hold /= mp_potential;
  }
  sct_ptr = shold_ptr;
  return(hold);
}

/* SET_MOVEPOTENTIAL -- Set the movement potential for a unit */
void
set_movepotential PARM_1(int, type)
{
  /* assumes settings are aligned before calling */
  switch (type) {
  case MOVE_ARMY:
  case MOVE_FLYARMY:
  case MOVE_ONEWAY:
    /* cost is based on unit's potential */
    mp_potential = army_mvpts(ntn_ptr, army_ptr);
    break;
  case MOVE_NAVY:
    /* cost is based on unit's potential */
    mp_potential = navy_mvpts(ntn_ptr, navy_ptr);
    break;
  case MOVE_CVN:
  case MOVE_FLYCVN:
    /* cost is based on unit's potential */
    mp_potential = cvn_mvpts(ntn_ptr, cvn_ptr);
    break;
  case MOVE_PATROL:
    /* 100% cost to go one sector */
    mp_potential = 1;
    break;
  default:
    /* 1% per relocation */
    mp_potential = 100;
    break;
  }
}
