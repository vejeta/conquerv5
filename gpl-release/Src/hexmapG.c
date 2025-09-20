// SPDX-License-Identifier: GPL-3.0-or-later
/* This file contains the conquer map display routines */
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
#include "dataG.h"
#include "armyX.h"
#include "cityX.h"
#include "moveX.h"
#include "navyX.h"
#include "desigX.h"
#include "magicX.h"
#include "racesX.h"
#include "activeX.h"
#include "elevegX.h"
#include "hlightX.h"
#include "statusX.h"
#include "tgoodsX.h"
#include "dstatusX.h"
#include "caravanX.h"
#include "displayG.h"
#define USE_CODES
#define PRINT_CODES
#include "worldX.h"

/* SHOW_CURSOR -- Move the cursor to the proper position */
void
show_cursor PARM_0(void)
{
  int xhold, yhold;

  /* determine original horizontal offset */
  if (curmap_screen[zoom_level].has_border) {
    xhold = 1;
  } else {
    xhold = 0;
  }

  /* determine horizontal position */
  xhold += xcurs * curmap_screen[zoom_level].xshift;

  /* determine original vertical offset */
  if ((curmap_screen[zoom_level].oddlift) &&
      (xcurs % 2 == 1)) {
    yhold = 1;
  } else {
    yhold = 0;
  }

  /* now find the vertical position */
  yhold += ycurs * curmap_screen[zoom_level].yshift;

  /* position the cursor in the focus position */
  if (curmap_screen[zoom_level].has_border) {
    xhold += (display_mode.focus % 2);
    yhold += (display_mode.focus / 2);
  }

  /* finally, position it */
  move(SCREEN_Y_SIZE - yhold - 1, xhold);
}

/* MAX_XCURS -- Determine the maximum xcurs value */
int
max_xcurs PARM_0(void)
{
  int available = SCREEN_X_SIZE;

  /* this is determined as a function of the display zoom and mode */
  available -= (curmap_screen[zoom_level].xsize - 
		curmap_screen[zoom_level].xshift);

  /* now can calculate it easily */
  return(available / curmap_screen[zoom_level].xshift);
}

/* MAX_YCURS -- Determine the maximum ycurs value */
int
max_ycurs PARM_1(int, xval)
{
  int available = SCREEN_Y_SIZE;

  /* check for adjustments */
  if (curmap_screen[zoom_level].oddlift &&
      ((xval - xoffset) % 2 == 1)) {
    /* hopefully this will work in all cases */
    available--;
  }

  /* now can calculate it easily */
  return(available / curmap_screen[zoom_level].yshift);
}

/* ON_SCREEN -- Is the sector on the display screen? */
int
on_screen PARM_2(int, x, int, y)
{
  int hold = TRUE;

  /* realign horizontal wrap */
  if (x < xoffset) {
    x += MAPX;
  }

  /* check it */
  if ((x - xoffset > max_xcurs()) ||
      (y < yoffset) ||
      (y - yoffset > max_ycurs(x))) {
    hold = FALSE;
  }
  return(hold);
}

/* CENTERMAP -- Relocated the current sector to the center of the screen */
void
centermap PARM_0(void)
{
  int temp, maxy;

  /* first align the X location */
  temp = XREAL;
  xcurs = max_xcurs() / 2;
  xoffset = temp - xcurs;

  /* assure that edge is set properly */
  if ((curmap_screen[zoom_level].oddlift) &&
      (((xoffset + MAPX) % 2) == 1)) {
    xoffset++;
    xcurs--;
  }
  if (xoffset < 0) {
    xoffset += MAPX;
  }

  /* now align the Y axis; adjust for boundaries */
  maxy = max_ycurs(xcurs);
  if (!world.relative_map || (is_god == TRUE) ||
      (ntn_ptr == NULL) || WIZ_MAGIC(MW_KNOWALL)) {
    temp = YREAL;
    if (MAPY <= maxy) {
      /* make sure that the bottom of screen is always set */
      yoffset = 0;
      ycurs = temp;
    } else {
      /* recenter and adjust */
      ycurs = (maxy - 1) / 2;
      yoffset = temp - ycurs;
      if (yoffset < 0) {
	ycurs = YREAL;
	yoffset = 0;
      } else if (yoffset > MAPY - maxy - 1) {
	yoffset = MAPY - maxy;
	ycurs = temp - yoffset;
      }
    }
  } else {
    /* just recenter */
    temp = YREAL;
    ycurs = (maxy - 1) / 2;
    yoffset = temp - ycurs;
  }
}

/* STYLE_VOIDABLE -- This mode cannot view into voided sectors */
static int
style_voidable PARM_1(int, style)
{
  int outval = FALSE;

  /* simple switch */
  switch (style) {
  case DI_DEFENSE:
  case DI_JEWEL:
  case DI_METAL:
  case DI_PEOP:
  case DI_FOOD:
  case DI_MAGIC:
  case DI_DESG:
  case DI_TGDESGS:
    /* can't see it ... oh well */
    outval = TRUE;
    break;
  default:
    /* yep, you can see it */
    break;
  }

  /* ske'daddle */
  return(outval);
}

/* NEED_WEDGE -- Is there land below this water sector? */
static int
need_wedge PARM_2(int, x, int, y)
{
  int hold = FALSE;

  /* check the sector below this one */
  if (XY_ONMAP(x, y - 1) &&
      on_screen(x, y - 1) &&
      (sct[x][y - 1].altitude != ELE_WATER)) {
    /* do it */
    hold = TRUE;
  }

  /* beat it */
  return(hold);
}

/* MAP_CHAR -- Get a character for the single sector of the screen */
static char
map_char PARM_4(int, x, int, y, int, position, int, style)
{
  int dflt_char = FALSE, ch = ' ';
  int value, sown;

  /* get the sector information */
  if (!XY_ONMAP(x, y)) {
    return '#';
  }

  /* allow quick referencing */
  sct_ptr = &(sct[x][y]);
  sown = sct_ptr->owner;
  ntn_tptr = world.np[sown];
  if (ntn_tptr == NULL)
    sown = UNOWNED;

  /* check it out */
  if ((country != UNOWNED) &&
      (country != sown) &&
      (sown != UNOWNED) &&
      (MAGIC(ntn_tptr->powers[MAG_WIZARDRY], MW_THEVOID) &&
       !WIZ_MAGIC(MW_SEEALL)) &&
      style_voidable(style)) {
    /* can't see nuthin' */
    ch = '?';
  } else {

    /* varies with display style */
    switch (style) {
    case DI_BLANK:
    case DI_KEEP:
      /* just empty */
      dflt_char = TRUE;
      break;
    case DI_AMOVE:
    case DI_FMOVE:
    case DI_NMOVE:
      /* different movement display modes */
      if (style == DI_AMOVE) {
	value = move_cost(x, y, MOVE_ARMY);
      } else if (style == DI_FMOVE) {
	value = move_cost(x, y, MOVE_FLYARMY);
      } else {
	value = move_cost(x, y, MOVE_NAVY);
      }
      
      /* compute results */
      switch (value) {
      case MV_UNMET:
      case MV_HOSTILE:
	/* cannot enter the sector */
	ch = 'X';
	break;
      case MV_LANDING:
	/* costs all movement to enter */
	ch = '+';
	break;
      case MV_WATER:
      case MV_GROUND:
      case MV_IMPASSABLE:
	/* can't be entered */
	dflt_char = TRUE;
	break;
      default:
	/* number please */
	if (value < 0) {
	  dflt_char = TRUE;
	} else {
	  if (value > 9) {
	    ch = '+';
	  } else {
	    ch = value + '0';
	  }
	}
	break;
      }
      break;
    case DI_FOOD:
      /* display the food value of a sector */
      value = distort_vision(x, y, tofood(sct_ptr, country),
			     sct_ptr->people);

      /* display based on values */
      if (value == 0) {
	dflt_char = TRUE;
      } else if (value < 10) {
	ch = value + '0';
      } else {
	ch = '+';
      }
      break;
    case DI_VEGE:
      /* display the vegetation value of the sector */
      if (sct_ptr->altitude == ELE_WATER) {
	dflt_char = TRUE;
      } else {
	ch = veg_info[sct_ptr->vegetation].symbol;
      }
      break;
    case DI_DESG:
      /* display the designation of the sector */
      value = major_desg(sct_ptr->designation);
      if ((value == MAJ_NONE) ||
	  ((value == MAJ_CACHE) &&
	   (country != sct_ptr->owner) &&
	   (country != UNOWNED))) {
	dflt_char = TRUE;
      } else {
	ch = maj_dinfo[value].symbol;
      }
      break;
    case DI_YDESG:
      /* display the designation of the sector if you own it */
      value = major_desg(sct_ptr->designation);
      if (sown != UNOWNED && sown != country) {
	ch = ntn_tptr->mark;
      } else {
	if (value == MAJ_NONE) {
	  dflt_char = TRUE;
	} else {
	  ch = maj_dinfo[value].symbol;
	}
      }
      break;
    case DI_CONT:
      /* show the elevation of the sector */
      ch = ele_info[sct_ptr->altitude].symbol;
      break;
    case DI_NATN:
      /* display the owner of the sector */
      if (sown == UNOWNED) {
	dflt_char = TRUE;
      } else {
	ch = ntn_tptr->mark;
      }
      break;
    case DI_RACE:
      /* display the race of the sector owner */
      if (sown == UNOWNED) {
	dflt_char = TRUE;
      } else {
	ch = race_info[ntn_tptr->race].name[0];
      }
      break;
    case DI_DEFENSE:
      /* compute defensive power of a sector based on terrain */
      if (sct_ptr->altitude == ELE_WATER ||
	  sct_ptr->altitude == ELE_PEAK) {
	dflt_char = TRUE;
      } else {
	value = defense_val(x, y);
	if (value >= 95) {
	  value = (value - 50) / 100;
	  ch = rnumerals[roman_base(value)];
	} else {
	  ch = (value + 5) / 10 + '0';
	}
      }
      break;
    case DI_PEOP:
      /* display the number of people in the sector */
      if (sct_ptr->owner == UNOWNED) {
	dflt_char = TRUE;
      } else {

	/* determine visibility of sector */
	value = distort_vision(x, y, sct_ptr->people,
			       sct_ptr->people);
	
	if (value < 950) {
	  ch = (50 + value) / 100 + '0';
	} else {
	  value = (value - 500) / 1000;
	  ch = rnumerals[roman_base(value)];
	}
      }
      break;
    case DI_JEWEL:
    case DI_METAL:
    case DI_MAGIC:
    case DI_VALUES:
      /* display the magical, jewel or metal value of a sector */
      value = tofood( sct_ptr, country );
      if (value == 0) {
	dflt_char = TRUE;
      } else if ((country == UNOWNED) || (sown == UNOWNED)
		 || (country == sown) || WIZ_MAGIC(MW_SEEALL)) {

	/* can we dig it? */
	if (tg_ok(sct_ptr, country)) {

	  /* get resulting value */
	  switch (style) {
	  case DI_JEWEL:
	    value = jewel_value(sct_ptr);
	    break;
	  case DI_METAL:
	    value = metal_value(sct_ptr);
	    break;
	  case DI_MAGIC:
	    value = magic_value(sct_ptr);
	    break;
	  case DI_VALUES:
	    /* get 'em all */
	    if (((value = jewel_value(sct_ptr)) == 0) &&
		((value = metal_value(sct_ptr)) == 0) &&
		((value = magic_value(sct_ptr)) == 0)) {
	      value = tg_info[sct_ptr->tradegood].value;
	    }
	    break;
	  default:
	    value = 0;
	    break;
	  }

	  /* distort */
	  value = distort_vision(x, y, value, sct_ptr->people);

	  /* show resulting value */
	  if (value == 0) {
	    dflt_char = TRUE;
	  } else if (value > 9) {
	    ch = '+';
	  } else ch = value + '0';

	} else {
	  dflt_char = TRUE;
	}

      } else {
	ch = '?';
      }
      break;
    case DI_TGDESGS:
      /* display designations needed for tradegoods */
      value = tofood(sct_ptr, country);
      if ((value == 0) ||
	  ((tg_info[sct_ptr->tradegood].need_desg == MAJ_FARM) &&
	   (value < DESFOOD))) {
	dflt_char = TRUE;
      } else if ((country == UNOWNED) || (sown == UNOWNED)
		 || (country == sown) || WIZ_MAGIC(MW_SEEALL)) {
	if ((sct_ptr->tradegood != TG_NONE) &&
	    (tg_info[sct_ptr->tradegood].need_desg != MAJ_NONE) &&
	    tg_ok(sct_ptr, country) ) {
	  ch = maj_dinfo[tg_info[sct_ptr->tradegood].need_desg].symbol;
	} else {
	  dflt_char = TRUE;
	}
      } else {
	ch = '?';
      }
      break;
    case DI_WOOD:
      /* display the wood value for a sector */
      value = tofood(sct_ptr, country);
      if (value == 0) {
	dflt_char = TRUE;
      } else {

	/* determine the value that is thought to be there */
	value = distort_vision(x, y, towood(sct_ptr, country),
			       sct_ptr->people);
	if (value == 0) {
	  dflt_char = TRUE;
	} else if (value > 9) {
	  ch = '+';
	} else ch = value + '0';
      }
      break;
    case DI_WEIGHTS:
      /* show distribution weighting */
      if (((country == UNOWNED) && (sct_ptr->owner != UNOWNED)) ||
	  ((country != UNOWNED) && (sct_ptr->owner == country))) {

	/* find the value */
	if ((city_ptr =
	     citybyloc(world.np[sct_ptr->owner], x, y)) == NULL) {
	  value = 0;
	} else {
	  value = CITY_WEIGHT;
	}

	/* display it */
	if (value == 0) {
	  ch = '0';
	} else {
	  if (value >= 90) {
	    ch = '+';
	  } else {
	    ch = value / 10 + '1';
	  }
	}
      } else {
	dflt_char = TRUE;
      }
      break;
    default:
      /* hmmm... something funny here */
      break;
    }

  }

  /* check it out */
  if (dflt_char == TRUE) {

    /* water is just, oh so special */
    if (sct_ptr->altitude == ELE_WATER) {

      /* show either fleets in the water or the water itself */
      if (position == HXPOS_LOWLEFT) {
	/* nation mark if unique nation in place */
	if (TROOPLOCS(x, y) != 0) {
	  if (TROOPLOCS(x, y) == ABSMAXNTN) {
	    ch = '+';
	  } else {
	    ch = world.np[(int) TROOPLOCS(x, y)]->mark;
	  }
	} else if (!conq_allblanks &&
		   !conq_waterbottoms &&
		   (conq_bottomlines ||
		    need_wedge(x, y))) {
	  ch = '_';
	} else {
	  ch = ele_info[ELE_WATER].symbol;
	}
      } else {
	if ((position == HXPOS_LOWRIGHT) &&
	    !conq_allblanks &&
	    !conq_waterbottoms &&
	    (conq_bottomlines ||
	     need_wedge(x, y))) {
	  ch = '_';
	} else {
	  ch = ele_info[ELE_WATER].symbol;
	}
      }

    } else {

      /* depends on what position it is in */
      switch (position) {
      case HXPOS_LOWLEFT:
      case HXPOS_LOWRIGHT:
	/* underline sector, to look nice! */
	if (conq_allblanks) {
	  ch = ' ';
	} else {
	  ch = '_';
	}
	break;
      case HXPOS_UPLEFT:
      case HXPOS_UPRIGHT:
	/* just the blanks m'am */
	ch = ' ';
	break;
      default:
	/* show the elevation or designation of the sector */
	if (tofood( sct_ptr, country ) < DESFOOD) {
	  ch = veg_info[sct_ptr->vegetation].symbol;
	} else {
	  ch = ele_info[sct_ptr->altitude].symbol;
	}
	break;
      }

    }
  }

  /* all done... bye bye */
  return (ch);
}

/* MAP_HIGHLIGHT -- Is the sector in need of highlight? */
static int
map_highlight PARM_4(int, x, int, y, int, style, int, opval)
{
  CITY_PTR c1_ptr = NULL;
  SCT_PTR s1_ptr = NULL;
  static int hold = FALSE, ox = -1, oy = -1, ostyle = -1;
  int range, value, sown;

  /* quick return if out of bounds */
  if (x < 0 || y < 0 || x >= MAPX || y >= MAPY) {
    return (hold = FALSE);
  }
  if ((style == HI_KEEP) ||
      (style == HI_NONE)) {
    return (hold = FALSE);
  }

  /* now check for repetition */
  if ((x == ox) &&
      (y == oy) &&
      (style == ostyle)) {
    /* same as the last request */
    return(hold);
  }
  hold = FALSE;
  ox = x;
  oy = y;
  ostyle = style;

  /* allow quick referencing */
  sct_ptr = &(sct[x][y]);
  sown = sct_ptr->owner;
  ntn_tptr = world.np[sown];
  if (ntn_tptr == NULL)
    sown = UNOWNED;

  /* return result based on the desired mode */
  switch (style) {
  case HI_MOVEABLE:
    /* highlight units with original movement */
    if (has_unmoved(x, y)) {
      hold = TRUE;
    }
    break;
#ifdef UNIMP
  case HI_STARTMOVE:
    /* highlight units with movement remaining */
    if (has_movable(x, y)) {
      hold = TRUE;
    }
    break;
#endif /* UNIMP */
  case HI_SCOUT:
    /* highlight any scouting units */
    if (has_scouts(x, y)) {
      hold = TRUE;
    }
    break;
  case HI_YUNITS:
    /* highlight any armies, etc. owned by the current nation */
    if (has_troops(x, y)) {
      hold = TRUE;
    }
    break;
  case HI_UNITS:
    /* highlight any armies, etc. on the map */
    if (TROOPLOCS(x, y)) {
      hold = TRUE;
    }
    break;
  case HI_TGOODS:
    /* highlight sectors with trade goods */
    if ((sown != UNOWNED) &&
	(country != UNOWNED) &&
	(sown != country) &&
	MAGIC(ntn_tptr->powers[MAG_WIZARDRY], MW_ILLUSION) &&
	!WIZ_MAGIC(MW_VISION)) {
      /* give no report for illusionary nations */
      break;
    }
    if ( (tg_ok( sct_ptr, country )
	  && sct_ptr->tradegood != TG_NONE)
	&& (((sown == UNOWNED) ||
	     !MAGIC(ntn_tptr->powers[MAG_WIZARDRY], MW_THEVOID)) ||
	    (country == UNOWNED) ||
	    (country == sown) ||
	    WIZ_MAGIC(MW_SEEALL)) ) {
      if ((opval == TG_NONE) ||
	  (opval == tg_info[sct_ptr->tradegood].class)) {
	hold = TRUE;
      }
    }
    break;
  case HI_OWN:
    /* highlight owned sectors */
    if (opval == UNOWNED) {
      if (sown != UNOWNED) hold = TRUE;
    } else {
      if (sown == opval) hold = TRUE;
    }
    break;
  case HI_ALLIED:
    if (country == UNOWNED) {

      /* highlight good nations */
      if (sown == UNOWNED) break;
      if (n_isgood(ntn_tptr->active)) {
	hold = TRUE;
      }

    } else {

      /* highlight sectors owned by friendly nations */
      if ((sown == UNOWNED) ||
	  (sown == country)) break;
      value = ntn_tptr->dstatus[country] % dstatus_number;
      if (value != DIP_UNMET && value < DIP_NEUTRAL) {
	hold = TRUE;
      }

    }
    break;
  case HI_ENEMY:
    if (country == UNOWNED) {

      /* highlight evil nations */
      if (sown == UNOWNED) break;
      if (n_isevil(ntn_tptr->active) || n_ismonster(ntn_tptr->active)) {
	hold = TRUE;
      }

    } else {

      /* highlight sectors owned by unfriendly nations */
      if ((sown == UNOWNED) ||
	  (sown == country)) break;
      value = ntn_tptr->dstatus[country] % dstatus_number;
      if (value > DIP_NEUTRAL) {
	hold = TRUE;
      }

    }
    break;
  case HI_NEUTRAL:
    if (country == UNOWNED) {

      /* highlight neutral nations */
      if (sown == UNOWNED) break;
      if (n_isneutral(ntn_tptr->active)) {
	hold = TRUE;
      }

    } else {

      /* highlight sectors owned by neutral nations */
      if ((sown == UNOWNED) ||
	  (sown == country)) break;
      value = ntn_tptr->dstatus[country];
      if (value == DIP_UNMET || value == DIP_NEUTRAL) {
	hold = TRUE;
      }

    }
    break;
  case HI_MINDESG:
    /* check input */
    if ((opval < 0) ||
	(opval >= MIN_NUMBER)) break;

    /* highlight a sector containing a given minor designation */
    if (((MIN_START << opval) != MIN_DEVASTATED) &&
	((MIN_START << opval) != MIN_TRADINGPOST)) {
      if ((sown != UNOWNED) &&
	  (country != UNOWNED) &&
	  (sown != country) &&
	  (ntn_tptr->dstatus[country] % dstatus_number > DIP_FRIENDLY) &&
	  MAGIC(ntn_tptr->powers[MAG_WIZARDRY], MW_ILLUSION) &&
	  !WIZ_MAGIC(MW_VISION)) {
	/* give no report for illusionary nations */
	break;
      }
    }

    /* check the value */
    if (minor_desg(sct_ptr->designation, MIN_START << opval)) {
      hold = TRUE;
    }
    break;
  case HI_MAJDESG:
    /* check input */
    if ((opval < 0) ||
	(opval >= MAJ_NUMBER)) break;

    /* highlight a sector containing a given minor designation */
    if (opval == MAJ_CACHE) {
      if ((sown != UNOWNED) &&
	  (country != UNOWNED) &&
	  (sown != country)) {
	break;
      }
    }

    /* check the value */
    if ((opval == MAJ_FARM) &&
	IS_FARM(sct_ptr->designation)) {
      hold = TRUE;
    } else if (major_desg(sct_ptr->designation) == opval) {
      hold = TRUE;
    }
    break;
  case HI_REGION:
    /* check if they do not belong */
    if (sown != sct[XREAL][YREAL].owner) break;
  case HI_RANGE:
    /* more checking */
    if (country == UNOWNED) {
      /* first check if the cursor is on a city */
      s1_ptr = &(sct[XREAL][YREAL]);
      ntn_tptr = world.np[s1_ptr->owner];
      if ((c1_ptr = citybyloc(ntn_tptr, XREAL, YREAL)) == NULL) {
	/* then check if the current sector is within range of any city */
	s1_ptr = &(sct[x][y]);
	ntn_tptr = world.np[s1_ptr->owner];
	c1_ptr = citybyloc(ntn_tptr, x, y);
      }
    } else {
      /* first check if the cursor is on a city */
      s1_ptr = &(sct[XREAL][YREAL]);
      ntn_tptr = ntn_ptr;
      if ((c1_ptr = citybyloc(ntn_tptr, XREAL, YREAL)) == NULL) {
	/* then check if the cursor is within range of any city */
	s1_ptr = &(sct[x][y]);
	c1_ptr = citybyloc(ntn_tptr, x, y);
      }
    }
    if (c1_ptr != NULL) {
      range = r10_region(ntn_tptr, c1_ptr, s1_ptr) / 10;
      hold = map_within(XREAL, YREAL, x, y, range);
    }
    break;
  case HI_UNSUPPORTED:
    if ((((country == UNOWNED) && (sct_ptr->owner != UNOWNED)) ||
	 ((country != UNOWNED) && (sct_ptr->owner == country))) &&
	(SUM_WEIGHTS(x, y) == 0)) hold = TRUE;
    break;
  case HI_SUPPORTED:
    if ((((country == UNOWNED) && (sct_ptr->owner != UNOWNED)) ||
	 ((country != UNOWNED) && (sct_ptr->owner == country))) &&
	(SUM_WEIGHTS(x, y) != 0)) hold = TRUE;
    break;
  default:
    bottommsg("Unknown highlighting selection!!!!!");
    break;
  }

  /* give back results */
  return(hold);
}

/* HEX_INIT -- Initialize information for the hex-map display */
void
hex_init PARM_0(void)
{
  int count, count2;

  /* prep the map mode */
  if (world.hexmap) {
    curmap_screen = hexmap_screen;
  } else {
    curmap_screen = rectmap_screen;
  }
  centermap();

  /* now build up the list of displays */
  for (count = 0; count < DMODE_NUMBER; count++) {
    /* add in each one */
    if ((dmode_tptr = crt_dmode(base_modes[count].name)) != NULL) {
      dmode_tptr->d.focus = base_modes[count].focus;
      for (count2 = 0; count2 < HXPOS_NUMBER; count2++) {
	dmode_tptr->d.style[count2] = base_modes[count].style[count2];
	dmode_tptr->d.highlight[count2] = base_modes[count].highlight[count2];
	dmode_tptr->d.target[count2] = base_modes[count].target[count2];
      }
    }
  }

  /* do memory dependent calculations */
  hex_recalc();
  set_weights(FALSE);
}

/* HEX_RECALC -- Recalculate the information for the hex-map */
void
hex_recalc PARM_0(void)
{
  ARMY_PTR ahold_ptr = army_ptr;
  NAVY_PTR nhold_ptr = navy_ptr;
  CVN_PTR chold_ptr = cvn_ptr;
  int count;

  /* clean up the memory */
  sectstat = new_mapchar(sectstat);
  trooplocs = new_mapchar(trooplocs);

  /* go through appropriate nations */
  for (count = 1; count < MAXNTN; count++) {

    /* check if country should be skipped */
    if ((ntn_tptr = world.np[count]) == NULL) continue;
    if ((country != UNOWNED) &&
	(country != count) &&
	MAGIC(ntn_tptr->powers[MAG_WIZARDRY], MW_THEVOID) &&
	!WIZ_MAGIC(MW_SEEALL)) {
      continue;
    }

    /* check armies in sector */
    for (army_tptr = ntn_tptr->army_list;
	 army_tptr != NULL;
	 army_tptr = army_tptr->next) {

      /* check all armies */
      if (ARMYT_SIZE > 0) {
	if ((country == UNOWNED) ||
	    (country == count) ||
	    !MAGIC(ntn_tptr->powers[MAG_WIZARDRY], MW_ILLUSION) ||
	    WIZ_MAGIC(MW_VISION) ||
	    rand_val(2)) {

	  /* is owned by this nation? or is god watching? */
	  if ((country == count) ||
	      (country == UNOWNED)) {

	    /* possessive of troops? */
	    if ((count == country) ||
		n_ismonster(ntn_tptr->active)) {
	      set_has_troops(ARMYT_XLOC, ARMYT_YLOC);
	    }

	    /* scout? */
	    if (a_isscout(ARMYT_TYPE)) {
	      set_has_scouts(ARMYT_XLOC, ARMYT_YLOC);
	    }

	    /* check for movement */
	    if (!nomove_stat(ARMYT_MOVE) &&
		(ARMYT_MOVE != 0)) {
	      set_has_movable(ARMYT_XLOC, ARMYT_YLOC);
	      if (ARMYT_MOVE == 100) {
		set_has_unmoved(ARMYT_XLOC, ARMYT_YLOC);
	      }
	    } else {
	      /* wall patrol? */
	      if (wall_patrol(count, army_tptr) == FALSE) {
		set_has_movable(ARMYT_XLOC, ARMYT_YLOC);
		set_has_unmoved(ARMYT_XLOC, ARMYT_YLOC);
	      }
	    }

	  }

	  /* now add it to the list of troops in the sector */
	  if ((TROOPLOCS(ARMYT_XLOC, ARMYT_YLOC) != count) &&
	      (TROOPLOCS(ARMYT_XLOC, ARMYT_YLOC) != UNOWNED)) {
	    TROOPLOCS(ARMYT_XLOC, ARMYT_YLOC) = ABSMAXNTN;
	  } else {
	    TROOPLOCS(ARMYT_XLOC, ARMYT_YLOC) = count;
	  }

	}
      }

    }

    /* go through all navies in the nation */
    for (navy_tptr = ntn_tptr->navy_list;
	 navy_tptr != NULL;
	 navy_tptr = navy_tptr->next) {

      /* check if a navy is in the given sector */
      if ((country == UNOWNED) ||
	  (country == count) ||
	  rand_val(2) ||
	  !MAGIC(ntn_tptr->powers[MAG_WIZARDRY], MW_ILLUSION) ||
	    WIZ_MAGIC(MW_VISION)) {

	if ((country == count) ||
	    (country == UNOWNED)) {

	  /* these be owned by them? */
	  if ((count == country) ||
	      n_ismonster(ntn_tptr->active)) {
	    set_has_troops(NAVYT_XLOC, NAVYT_YLOC);
	  }

	  /* check movement */
	  if (NAVYT_MOVE != 0) {
	    set_has_movable(NAVYT_XLOC, NAVYT_YLOC);
	    if (NAVYT_MOVE == navy_mvpts(ntn_tptr, navy_tptr)) {
	      set_has_unmoved(NAVYT_XLOC, NAVYT_YLOC);
	    }
	  }
	}

	/* now add it to the list of troops in the sector */
	if ((TROOPLOCS(NAVYT_XLOC, NAVYT_YLOC) != count) &&
	    (TROOPLOCS(NAVYT_XLOC, NAVYT_YLOC) != UNOWNED)) {
	  TROOPLOCS(NAVYT_XLOC, NAVYT_YLOC) = ABSMAXNTN;
	} else {
	  TROOPLOCS(NAVYT_XLOC, NAVYT_YLOC) = count;
	}
      }
    }

    /* go through all caravans in the nation */
    for (cvn_tptr = ntn_tptr->cvn_list;
	 cvn_tptr != NULL;
	 cvn_tptr = cvn_tptr->next) {

      /* check if a caravan is in the given sector */
      if (CVNT_SIZE != 0) {
	if ((country == UNOWNED) ||
	    (country == count) ||
	    rand_val(2) ||
	    !MAGIC(ntn_tptr->powers[MAG_WIZARDRY], MW_ILLUSION) ||
	    WIZ_MAGIC(MW_VISION)) {

	  if ((country == count) ||
	      (country == UNOWNED)) {

	    /* these be owned by them? */
	    if ((count == country) ||
		n_ismonster(ntn_tptr->active)) {
	      set_has_troops(CVNT_XLOC, CVNT_YLOC);
	    }

	    /* check movement */
	    if (CVNT_MOVE != 0) {
	      set_has_movable(CVNT_XLOC, CVNT_YLOC);
	      if (CVNT_MOVE == cvn_mvpts(ntn_tptr, cvn_tptr)) {
		set_has_unmoved(CVNT_XLOC, CVNT_YLOC);
	      }
	    }
	  }

	  /* now add it to the list of troops in the sector */
	  if ((TROOPLOCS(CVNT_XLOC, CVNT_YLOC) != count) &&
	      (TROOPLOCS(CVNT_XLOC, CVNT_YLOC) != UNOWNED)) {
	    TROOPLOCS(CVNT_XLOC, CVNT_YLOC) = ABSMAXNTN;
	  } else {
	    TROOPLOCS(CVNT_XLOC, CVNT_YLOC) = count;
	  }

	}
      }

    }
  }

  /* now take care of the other things */
  whatcansee();
  army_ptr = ahold_ptr;
  navy_ptr = nhold_ptr;
  cvn_ptr = chold_ptr;
}

/* GO_NORTH -- Move north */
int
go_north PARM_0(void)
{
  pager = 0;
  selector = 0;
  ycurs++;
  return(0);
}

/* GO_NE -- Move northeast */
int
go_ne PARM_0(void)
{
  pager = 0;
  selector = 0;
  if (world.hexmap) {
    ycurs += (XREAL % 2);
    xcurs++;
  } else {
    xcurs++;
    ycurs++;
  }
  return(0);
}

/* GO_NW -- Move northwest */
int
go_nw PARM_0(void)
{
  pager = 0;
  selector = 0;
  if (world.hexmap) {
    ycurs += (XREAL % 2);
    xcurs--;
  } else {
    xcurs--;
    ycurs++;
  }
  return(0);
}

/* GO_WEST -- Move west */
int
go_west PARM_0(void)
{
  pager = 0;
  selector = 0;
  if (world.hexmap) {
    xcurs -= 2;
  } else {
    xcurs--;
  }
  return(0);
}

/* GO_EAST -- Move east */
int
go_east PARM_0(void)
{
  pager = 0;
  selector = 0;
  if (world.hexmap) {
    xcurs += 2;
  } else {
    xcurs++;
  }
  return(0);
}

/* GO_SOUTH -- Move south */
int
go_south PARM_0(void)
{
  pager = 0;
  selector = 0;
  ycurs--;
  return(0);
}

/* GO_SE -- Move southeast */
int
go_se PARM_0(void)
{
  pager = 0;
  selector = 0;
  if (world.hexmap) {
    ycurs -= (XREAL + 1) % 2;
    xcurs++;
  } else {
    ycurs--;
    xcurs++;
  }
  return(0);
}

/* GO_SW -- Move southwest */
int
go_sw PARM_0(void)
{
  pager = 0;
  selector = 0;
  if (world.hexmap) {
    ycurs -= (XREAL + 1) % 2;
    xcurs--;
  } else {
    ycurs--;
    xcurs--;
  }
  return(0);
}

/* SCR_NORTH -- Move north */
int
scr_north PARM_0(void)
{
  pager = 0;
  selector = 0;
  ycurs += max_ycurs(xcurs) / 2;
  return(0);
}

/* SCR_NE -- Move northeast */
int
scr_ne PARM_0(void)
{
  pager = 0;
  selector = 0;
  xcurs += max_xcurs() / 2;
  ycurs += max_ycurs(xcurs) / 2;
  return(0);
}

/* SCR_NW -- Move northwest */
int
scr_nw PARM_0(void)
{
  pager = 0;
  selector = 0;
  xcurs -= max_xcurs() / 2;
  ycurs += max_ycurs(xcurs) / 2;
  return(0);
}

/* SCR_WEST -- Move west */
int
scr_west PARM_0(void)
{
  pager = 0;
  selector = 0;
  xcurs -= max_xcurs() / 2;
  return(0);
}

/* SCR_EAST -- Move east */
int
scr_east PARM_0(void)
{
  pager = 0;
  selector = 0;
  xcurs += max_xcurs() / 2;
  return(0);
}

/* SCR_SOUTH -- Move south */
int
scr_south PARM_0(void)
{
  pager = 0;
  selector = 0;
  ycurs -= max_ycurs(xcurs) / 2;
  return(0);
}

/* SCR_SE -- Move southeast */
int
scr_se PARM_0(void)
{
  pager = 0;
  selector = 0;
  ycurs -= max_ycurs(xcurs) / 2;
  xcurs += max_xcurs() / 2;
  return(0);
}

/* SCR_SW -- Move southwest */
int
scr_sw PARM_0(void)
{
  pager = 0;
  selector = 0;
  ycurs -= max_ycurs(xcurs) / 2;
  xcurs -= max_xcurs() / 2;
  return(0);
}

/* SHOW_SECT -- Show the sector at the indicated position,
                a position of -1, -1 indicates to go to normal place */
void
show_sect PARM_5(int, x, int, y, int, x_loc, int, y_loc, int, method)
{
  int high_on = FALSE, focus, ch1, ch2, tmp;
  int i, j, k;

  /* check the positioning */
  if ((x_loc == -1) ||
      (y_loc == -1)) {

    /* check positioning */
    if (!on_screen(x, y) ||
	!Y_ONMAP(y)) return;

    /* check if the sector is visible; if necessary */
    if (((method & 1) == 0) &&
	(VIS_CHECK((x + MAPX) % MAPX, y) == HS_NOSEE)) return;

    /* now find the real location */
    if (x < xoffset) {
      tmp = x + MAPX;
    } else {
      tmp = x;
    }
    x_loc = (tmp - xoffset) * curmap_screen[zoom_level].xshift;
    y_loc = (y - yoffset) * curmap_screen[zoom_level].yshift;
    if (curmap_screen[zoom_level].oddlift &&
	((x % 2) == 1)) {
      y_loc++;
    }
    y_loc = SCREEN_Y_SIZE - y_loc - 1;
    
  }

  /* check highlighting of the sector */
  focus = display_mode.focus;

  /* have the position, now place the data */
  if (zoom_level == ZOOM_DETAIL) {

    /* set the borders */
    if (world.hexmap) {
      ch1 = '/';
      ch2 = '\\';
    } else {
      ch1 = '|';
      ch2 = '|';
    }

    /* provide the topping, if it fits */
    if (conq_allblanks == FALSE) {
      if ((method & 2) && (y_loc > 1)) {
	mvaddstr(y_loc - 2, x_loc + 1, "__");
      }
    }

    /* first build the sector brace */
    mvprintw(y_loc, x_loc, "%c  %c", ch2, ch1);
    mvprintw(y_loc - 1, x_loc, "%c  %c", ch1, ch2);

    /* now show the top row */
    for (i = 0; i < 2; i++) {
      for (j = 0; j < 2; j++) {

	/* find the positioning */
	k = i + 2 * j;
	move(y_loc - j, x_loc + 1 + i);

	/* check the highlighting */
	if (map_highlight((x + MAPX) % MAPX, y,
			  display_mode.highlight[k],
			  display_mode.target[k])) {
	  high_on = TRUE;
	  standout();
	}

	/* show it and stow it */
	addch(map_char((x + MAPX) % MAPX, y, k,
		       display_mode.style[k]));
	if (high_on == TRUE) {
	  high_on = FALSE;
	  standend();
	}

      }
    }

  } else {

    /* just show the focus sector */
    if (map_highlight((x + MAPX) % MAPX, y,
		      display_mode.highlight[focus],
		      display_mode.target[focus])) {
      high_on = TRUE;
      standout();
    }
    ch1 = map_char((x + MAPX) % MAPX, y, HXPOS_MINIMAL,
		  display_mode.style[focus]);
    mvaddch(y_loc, x_loc, ch1);

    /* turn off highlighting if on */
    if (high_on) standend();

  }
}

/* PRINT_MAP -- Send a map to the standard output */
void
print_map PARM_1(int, widemap)
{
  /*ARGSUSED*/
#ifdef UNIMPLEMENTED
  char tempstr[20];
  register int x, y;
  char *x_seen;
  int ch, choice, hichoice = 0, coord_on, last_on;
  int map_top, map_bottom, map_left, map_right;
  int center_left, center_right;

  fprintf(stderr, "\n=== Conquer Map Display ===\n\n");
  if (widemap) {
    fprintf(stderr, "To capture the map use: conquer -P > foo\n\n");
  } else {
    fprintf(stderr, "To capture the map use: conquer -p > foo\n\n");
  }
  if (widemap) {

    /* show the available highlighting methods */
    fprintf(stderr, "Map Highlighting Options:\n");
    fprintf(stderr, "   ");
    x = 3;
    for (hichoice = 1; hichoice <= HI_MAXIMUM; hichoice++) {

      /* keep track of screen wrapping */
      x += strlen(highl_list[hichoice]) + 4;
      if (x > 70) {
	fprintf(stderr, ",\n   ");
	x = strlen(highl_list[hichoice]) + 3;
      } else if (hichoice != 1) {
	fprintf(stderr, ", ");
      }
      fprintf(stderr, "(%c)%s", highl_list[hichoice][0],
	      &(highl_list[hichoice][1]));
    }
    fprintf(stderr, "\nUse which type of highlighting? ");

    /* now find the choice */
    scanf("%s", string);
    if (strlen(string) == 0) return;
    for (hichoice = 1; hichoice <= HI_MAXIMUM; hichoice++) {
      if (completion(string, highl_list[hichoice])) break;
    }
    if (hichoice > HI_MAXIMUM) {
      fprintf(stderr, "Unknown highlight type; no map generated\n");
      return;
    }
    fprintf(stderr, "\n");
  }

  /* now provide display selection */
  fprintf(stderr, "Map Display Options:\n");
  fprintf(stderr, "   ");
  x = 3;
  for (choice = 1; choice <= DI_MAXIMUM; choice++) {

    /* keep track of screen wrapping */
    x += strlen(display_list[choice]) + 4;
    if (x > 70) {
      fprintf(stderr, ",\n   ");
      x = strlen(display_list[choice]) + 3;
    } else if (choice != 1) {
      fprintf(stderr, ", ");
    }
    fprintf(stderr, "(%c)%s", display_list[choice][0],
	    &(display_list[choice][1]));
  }
  fprintf(stderr, "\nDisplay which type of map? ");

  /* now find the choice */
  scanf("%s", string);
  if (strlen(string) == 0) return;
  for (choice = 1; choice <= DI_MAXIMUM; choice++) {
    if (completion(string, display_list[choice])) break;
  }
  if (choice > DI_MAXIMUM) {
    fprintf(stderr, "Unknown map display type; no map generated\n");
    return;
  }

  /* show the title */
  ch = display_list[choice][0];
  if (islower(ch)) ch = toupper(ch);
  fprintf(stdout, "Conquer %s.%d:  %c%s ",
	  VERSION, PATCHLEVEL, ch, &(display_list[choice][1]));
  if (widemap && hichoice != HI_NONE) {
    ch = highl_list[hichoice][0];
    if (islower(ch)) ch = toupper(ch);
    fprintf(stdout, "/ %c%s ", ch, &(highl_list[hichoice][1]));
  }
  fprintf(stdout, "Map for ");
  if ((country == UNOWNED) || (ntn_ptr == NULL)) {
    fprintf(stdout, "the World");
  } else {
    fprintf(stdout, "Nation %s", ntn_ptr->name);
  }
  roman_number(string, TURN - START_TURN + 1);
  fprintf(stdout, " on Turn %s\n", string);

  /* calculate the range of the map */
  if ((country == UNOWNED) ||
      ( (ntn_ptr != NULL) && WIZ_MAGIC(MW_KNOWALL) )) {

    /* provide the whole map */
    map_top = MAPY - 1;
    map_bottom = 0;
    map_right = MAPX;
    map_left = 0;

  } else {

    /* provide space for horizontal edge detection */
    if ((x_seen = (char *) malloc(sizeof(char) * MAPX)) == NULL) {
      fprintf(stderr, "Memory allocation error in print_map()\n");
      abrt();
    }

    /* clear the array */
    for (x = 0; x < MAPX; x++) x_seen[x] = FALSE;

    /* find the rectangle of what they can see; start with top */
    map_bottom = (-1);
    for (y = 0; (map_bottom == (-1)) && (y < MAPY); y++) {
      for (x = 0; (map_bottom == (-1)) && (x < MAPX); x++) {
	if (VIS_CHECK(x, y)) {
	  map_bottom = y;
	  break;
	}
      }
    }
    map_top = (-1);
    for (y = MAPY - 1; (map_top == (-1)) && (y >= 0); y--) {
      for (x = 0; (map_top == (-1)) && (x < MAPX); x++) {
	if (VIS_CHECK(x, y)) {
	  map_top = y;
	  break;
	}
      }
    }

    /* now keep track of the pattern of visible land */
    map_left = MAPX;
    map_right = (-1);
    for (y = map_bottom; y <= map_top; y++) {
      for (x = 0; x < MAPX; x++) {
	if (VIS_CHECK(x, y)) {
	  x_seen[x] = TRUE;
	  if (x > map_right) map_right = x;
	  if (x < map_left) map_left = x;
	}
      }
    }

    /* now analyze the pattern */
    last_on = map_left;
    center_left = center_right = (-1);

    /* first set the left edge */
    for (x = map_left; x <= map_right; x++) {

      /* check off and on status */
      if (last_on != x - 1) {
	if (x_seen[x] == TRUE) {
	  if (center_right - center_left < x - last_on) {
	    center_right = x;
	    center_left = last_on;
	  }
	}
      }

      /* mark that it is now within owned region */
      if (x_seen[x] == TRUE) {
	last_on = x;
      }

    }

    /* now compare the two large gaps */
    if (center_right - center_left > MAPX + map_left - map_right) {
      map_left = center_right - MAPX;
      map_right = center_left;
    }

    free(x_seen);
  }

  /* find out about coordinates */
  fprintf(stderr, "\n");
  fprintf(stderr, "Do you wish to see the coordinates? ");
  scanf("%s", string);
  putc('\n', stderr);
  if ((string[0] == '\0') ||
      (strcmp(string, "yes") == 0) ||
      (strcmp(string, "y") == 0)) {
    coord_on = 1;
  } else {
    coord_on = FALSE;
  }

  /* display horizontal coordinates */
  if (coord_on == 1) {

    /* determine range of placement */
    if (map_right - map_left >= 100) {
      coord_on = 10;
    } else {
      coord_on = 5;
    }

    /* calculate the coordinate axis */
    for (x = 0; x <= map_right - map_left + 6; x++) {
      string[x] = ' ';
    }
    string[x] = '\0';
    for (x = 0; x <= map_right - map_left; x++) {
      if (((global_int = xloc_relative(x + map_left)) % coord_on) == 0) {
	sprintf(tempstr, "%d", global_int);
	strncpy(string + x + 7 - strlen(tempstr), tempstr, strlen(tempstr));
      }
    }
    fprintf(stdout, "\n%s\n\n", string);
  } else {
    fprintf(stdout, "\n\n\n");
  }

  /* now display the map */
  for (y = map_top; y >= map_bottom; y--) {
    if ((coord_on != 0) && ((global_int = yloc_relative(y)) % 2 == 0)) {
      fprintf(stdout, "%5d ", global_int);
    } else {
      fprintf(stdout, "%5s ", " ");
    }
    for (x = map_left; x <= map_right; x++) {
      if (VIS_CHECK((x + MAPX) % MAPX, y)) {
	ch = get_display_for((x + MAPX) % MAPX, y, choice);
	if (widemap && highl_on((x + MAPX) % MAPX, y, hichoice)) {
	  putc(ch, stdout);
	  putc('\b', stdout);
	}
	putc(ch, stdout);
      } else {
	putc(' ', stdout);
      }
    }
    fprintf(stdout, "\n");
  }
#endif /* UNIMPLEMENTED */
}

/* SEE_ONE -- See just this one sector if posible */
static void
see_one PARM_2(int, x, int, y)
{
  show_sect(x, y, -1, -1, 1);
}

/* SEE_AROUND -- Simply show the surrounding sectors */
void
see_around PARM_2 ( int, x, int, y )
{
  /* visit each sector */
  map_loop(x, y, 1, see_one);
}

/* COFFMAP -- Check if the cursor is out of bounds */
void
coffmap PARM_0(void)
{
  int x, maxx, maxy;

  /* set boundaries */
  if (!world.relative_map ||
      ((is_god == TRUE) || (ntn_ptr == NULL) || WIZ_MAGIC(MW_KNOWALL))) {
    if (YREAL > MAPY - 1) {
      ycurs = MAPY - yoffset - 1;
    }
    if (YREAL < 0) {
      ycurs = - yoffset;
    }
  }

  /* find the boundaries */
  maxx = max_xcurs();
  maxy = max_ycurs(xcurs + xoffset);

  /* now check them */
  if (!world.relative_map || (is_god == TRUE) ||
      (ntn_ptr == NULL) || WIZ_MAGIC(MW_KNOWALL)) {

    /* check the wrapping */
    if ((xcurs <= 0) ||
	((ycurs == 0) && (YREAL != 0)) ||
	(ycurs < 0) ||
	(xcurs >= maxx - 1) ||
	((ycurs == maxy - 1) && (YREAL < MAPY - 1)) ||
	(ycurs > maxy - 1)) {
      centermap();
    }

  } else {

    /* check wrapping at all times */
    if ((xcurs <= 0) ||
	(ycurs <= 0) ||
	(xcurs >= maxx - 1) ||
	(ycurs >= maxy - 1)) {
      centermap();
    }

  }

  /* clear screen appropriately */
  if (redraw == DRAW_FULL) {

    /* redraw all of the screen */
    clear();

#ifdef VAXC
    /* extra refreshment for VAXC curses */
    refresh();
#endif /* VAXC */

  } else {

    /* just draw what has been changed */
    move(0,0);
    clrtobot();

  }

  /* build the map */
  makemap();

  /* handle bottom and side of screen */
  if (redraw != DRAW_NOFRILLS) {

    /* do the bottom */
    makebottom();

    /* redraw the side of the screen */
    if (display_mode.style[display_mode.focus] == DI_FMOVE) {
      x = MOVEIND_FLY;
    } else if (display_mode.style[display_mode.focus] == DI_NMOVE) {
      x = MOVEIND_WATER;
    } else {
      x = MOVEIND_NORM;
    }
    makeside(FALSE, x);

    /* show the cursor */
    show_cursor();
    refresh();
    redraw = DRAW_DONE;

  }
}

/* HS_SEEPART -- Assign the HS_SEEPART to the visibility setting */
static void
hs_seepart PARM_2(int, x, int, y)
{
  /* check and assign */
  if (VIS_CHECK(x, y) < HS_SEEPART)
    VIS_STORE(x, y, HS_SEEPART);
}

/* HS_ARMYSEE -- Closeup visibility based on army information */
static void
hs_armysee PARM_2(int, x, int, y)
{
  /* set the spot as visible */
  if (a_cansee(ARMY_TYPE) &&
      (unit_status(ARMY_STAT) != ST_SIEGED)) {
    if (VIS_CHECK(x, y) < HS_SEEFULL) {
      VIS_STORE(x, y, HS_SEEFULL);
    }
  } else if (VIS_CHECK(x, y) < HS_SEEMOST) {
    VIS_STORE(x, y, HS_SEEMOST);
  }
}

/* WHATCANSEE -- track visibility of map or screen*/
void
whatcansee PARM_0(void)
{
  register int x = 0, y = 0;
  int i, j;

  /* the shadow knows... */
  if ((is_god == TRUE) ||
      (ntn_ptr == NULL) ||
      WIZ_MAGIC(MW_KNOWALL)) {
    for (x = 0; x < MAPX; x++)
    for (y = 0; y < MAPY; y++) {
      VIS_STORE(x ,y, HS_SEEALL);
    }
    return;
  }

  /* blank out the what is known */
  for (x = 0; x < MAPX; x++)
  for (y = 0; y < MAPY; y++) {
    VIS_STORE(x, y, HS_NOSEE);
  }

  /* search for all visible sectors */
  for (x = ntn_ptr->leftedge; x <= ntn_ptr->rightedge; x++)
  for (y = ntn_ptr->bottomedge; y <= ntn_ptr->topedge; y++) {

    /* if a sector is owned by the country */
    if (Y_ONMAP(y)
	&& (sct[(x + MAPX) % MAPX][y].owner == country)) {

      /* loop around the sector */
      map_loop((x + MAPX) % MAPX, y, LANDSEE, hs_seepart);

    }

  }

  /* go through all of the navies and mark visibility */
  for (navy_ptr = ntn_ptr->navy_list;
       navy_ptr != NULL;
       navy_ptr = navy_ptr->next) {

    /* if the navy is active it can see */
    j = 0;
    for (i = 0; i < NSHP_NUMBER; i++) {
      j |= NAVY_SHIPS[i];
    }
    if (j == 0) continue;
    x = NAVY_XLOC;
    y = NAVY_YLOC;
    if (XY_ONMAP(x, y)) {
      /* loop around location of the navy */
      map_loop(x, y, NAVYSEE, hs_seepart);
      if (VIS_CHECK(x, y) < HS_SEEMOST) {
	VIS_STORE(x, y, HS_SEEMOST);
      }
    }
  }

  /* go though all of the soldiers */
  for (army_ptr = ntn_ptr->army_list;
       army_ptr != NULL;
       army_ptr = army_ptr->next) {

    /* check if the army is active */
    if (ARMY_SIZE > 0) {

      /* See around it */
      map_loop(ARMY_XLOC, ARMY_YLOC, ARMYSEE, hs_seepart);

      /* See near it */
      map_loop(ARMY_XLOC, ARMY_YLOC, 1, hs_armysee);

      /* See on it */
      if (a_isagent(ARMY_TYPE) &&
	  a_cansee(ARMY_TYPE)) {
	VIS_STORE(ARMY_XLOC, ARMY_YLOC, HS_SEEALL);
      } else if ((a_isagent(ARMY_TYPE) || a_cansee(ARMY_TYPE)) &&
		 VIS_CHECK(ARMY_XLOC, ARMY_YLOC) < HS_SEEFULL) {
	VIS_STORE(ARMY_XLOC, ARMY_YLOC, HS_SEEFULL);
      }

    }
  }

  /* go though all of the supply lines */
  for (cvn_ptr = ntn_ptr->cvn_list;
       cvn_ptr != NULL;
       cvn_ptr = cvn_ptr->next) {

    /* check if the caravan is active */
    if (CVN_SIZE == 0) continue;

    /* set visibility */
    x = CVN_XLOC;
    y = CVN_YLOC;
    if (XY_ONMAP(x, y)) {
      map_loop(x, y, CVNSEE, hs_seepart);
      if (VIS_CHECK(x, y) < HS_SEEMOST) {
	VIS_STORE(x, y, HS_SEEMOST);
      }
    }

  }

}

/* MARK_SECTOR -- Store the current sector position */
int
mark_sector PARM_0(void)
{
  clear_bottom(0);
  bottommsg("That sector may be revisited using the \"jump-to-mark\" command");
  move(LINES - 1, 0);
  sleep(2);
  xloc_mark = XREAL;
  yloc_mark = YREAL;
  return(0);
}

/* RECENTER_NTN -- Realign relative center at the current position */
int
recenter_ntn PARM_0(void)
{
  /* clean bottom */
  clear_bottom(0);

  /* do we need to realign system? */
  if (!world.relative_map ||
      (is_god == TRUE) ||
      (ntn_ptr == NULL)) {
    errormsg("There is no need to adjust the coordinate system");
    return(0);
  }

  /* is this a proper sector? */
  if (!Y_ONMAP(YREAL) ||
      (VIS_CHECK(XREAL, YREAL) == FALSE)) {
    /* can't realign it */
    errormsg("Your coordinate system cannot be centered on uncharted lands");
    return(0);
  }

  /* align it */
  ntn_ptr->centerx = XREAL;
  ntn_ptr->centery = YREAL;
  mvaddstr(LINES - 3, 0, "Aligning coordinates to new central position");

  /* query permanence */
  mvaddstr(LINES - 2, 0, "Make the adjustment permanent? ");
  if (y_or_n() == TRUE) {
    XADJRLOC;
  }
  return(0);
}
