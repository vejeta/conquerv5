// SPDX-License-Identifier: GPL-3.0-or-later
/* mechanism to transfer items between storage sites */
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
#include "xferG.h"
#include "desigX.h"
#include "mtrlsX.h"
#include "statusX.h"
#include "keyvalsX.h"
#define USE_CODES
#define PRINT_CODES
#include "armyX.h"
#include "cityX.h"
#include "navyX.h"
#include "worldX.h"
#include "caravanX.h"
#include "dstatusX.h"

/* Declare the structures used for storage in these routines */
static XFER_STRUCT xfer_sites[3];

/* SCT2XFER -- Copy information about a sector into the current slot */
void
sct2xfer PARM_3(int, slot, int, xloc, int, yloc)
{
  XFER_PTR xfer_ptr;
  SCT_PTR s1_ptr;
  int i, dval;

  /* reality check */
  if ((slot < 0) || (slot > 1) ||
      !XY_ONMAP(xloc, yloc)) {
    return;
  }
  xfer_ptr = &(xfer_sites[slot]);

  /* set the description */
  xfer_ptr->site_class = XFER_SECTOR;
  s1_ptr = &(sct[xloc][yloc]);
  dval = major_desg(s1_ptr->designation);
  sprintf(xfer_ptr->description, "%s [%d,%d]",
	  (dval == MAJ_NONE) ? "Sector" : maj_dinfo[dval].name,
	  xloc_relative(xloc), yloc_relative(yloc));
  xfer_ptr->xloc = xloc;
  xfer_ptr->yloc = yloc;

  /* check the ownership */
  if ((s1_ptr->owner == country) &&
      (tofood(s1_ptr, country) > 0)) {
    xfer_ptr->civ_max = XF_NOLIMIT;
    xfer_ptr->crew_max = XF_NOLIMIT;
    xfer_ptr->onb_cmax = XF_NOLIMIT;
    xfer_ptr->civies = s1_ptr->people;
    xfer_ptr->crews = 0;
    xfer_ptr->civ_div = 1;
    xfer_ptr->crew_div = 1;
  } else {
    xfer_ptr->civ_max = 0;
    xfer_ptr->crew_max = 0;
    xfer_ptr->civ_div = 0;
    xfer_ptr->crew_div = 0;
    xfer_ptr->civies = 0;
    xfer_ptr->crews = 0;
    xfer_ptr->onb_cmax = 0;
  }
  xfer_ptr->onb_id = EMPTY_HOLD;
  xfer_ptr->onb_cid = EMPTY_HOLD;
  xfer_ptr->onb_max = XF_NOLIMIT;
  for (i = 0; i < MTRLS_NUMBER; i++) {
    xfer_ptr->mtrls[i] = 0;
  }
  xfer_ptr->mtrl_max = 0;
}

/* XFER2SCT -- Return the transferred materials to the sector */
void
xfer2sct PARM_3(int, slot, int, xloc, int, yloc)
{
  XFER_PTR xfer_ptr;
  SCT_PTR s1_ptr;
  long num_people;

  /* reality check */
  if ((slot < 0) || (slot > 1) ||
      !XY_ONMAP(xloc, yloc)) {
    return;
  }
  xfer_ptr = &(xfer_sites[slot]);

  /* check the ownership */
  if (xfer_ptr->site_class != XFER_SECTOR) {
    errormsg("ERROR:  This stuff doesn't belong in a sector");
    return;
  }
  s1_ptr = &(sct[xloc][yloc]);
  if (s1_ptr->owner == country) {
    num_people = (xfer_ptr->crews + xfer_ptr->civies) - s1_ptr->people;
    if (num_people != 0) {

      /* transfer the sector location */
      int oxloc = XREAL;
      int oyloc = YREAL;
      xcurs = xloc - xoffset;
      ycurs = yloc - yoffset;

      /* place the difference into the sector */
      sct[XREAL][YREAL].people += num_people;
      SADJPEOP;

      /* replace the sector location */
      xcurs = oxloc - xoffset;
      ycurs = oyloc - yoffset;

    }

  }
}

/* CITY2XFER -- Copy a city structure into the indicated xfer slot */
void
city2xfer PARM_2(int, slot, CITY_PTR, c1_ptr)
{
  XFER_PTR xfer_ptr;
  SCT_PTR s1_ptr;
  int i, dval;

  /* reality check */
  if ((slot < 0) || (slot > 1) || (c1_ptr == NULL)) {
    return;
  }
  xfer_ptr = &(xfer_sites[slot]);

  /* set the description */
  s1_ptr = &(sct[c1_ptr->xloc][c1_ptr->yloc]);
  dval = major_desg(s1_ptr->designation);
  sprintf(xfer_ptr->description, "%s %s",
	  maj_dinfo[dval].name, c1_ptr->name);
  xfer_ptr->xloc = c1_ptr->xloc;
  xfer_ptr->yloc = c1_ptr->yloc;

  /* build it up */
  xfer_ptr->site_class = XFER_CITY;
  if (s1_ptr->people > 0) {
    xfer_ptr->civies = s1_ptr->people;
    if (c1_ptr->i_people > 0) {
      xfer_ptr->crews = c1_ptr->i_people;
    } else {
      xfer_ptr->crews = 0;
    }
  } else {
    xfer_ptr->civies = 0;
    xfer_ptr->crews = 0;
  }
  for (i = 0; i < MTRLS_NUMBER; i++) {
    xfer_ptr->mtrls[i] = c1_ptr->i_mtrls[i];
  }
  xfer_ptr->onb_id = EMPTY_HOLD;
  xfer_ptr->onb_cid = EMPTY_HOLD;
  xfer_ptr->mtrl_max = XF_NOLIMIT;
  xfer_ptr->onb_max = XF_NOLIMIT;
  xfer_ptr->onb_cmax = XF_NOLIMIT;

  /* check the limits for civilians and crew */
  if (attract_val(c1_ptr->xloc, c1_ptr->yloc) > 0) {
    xfer_ptr->civ_max = XF_NOLIMIT;
    xfer_ptr->civ_div = -1;
    xfer_ptr->crew_max = XF_JOINED;
    xfer_ptr->crew_div = -1;
  } else {
    xfer_ptr->civ_max = 0;
    xfer_ptr->civ_div = 0;
    xfer_ptr->crew_max = 0;
    xfer_ptr->crew_div = 0;
  }
}

/* XFER2CITY -- Shift the results back into the city structure */
void
xfer2city PARM_2(int, slot, CITY_PTR, c1_ptr)
{
  XFER_PTR xfer_ptr;
  SCT_PTR s1_ptr;
  CITY_PTR chold_ptr = city_ptr;
  long num_people, crew_shift = 0;
  itemtype xch_mtrl;
  int i;

  /* reality check */
  if ((slot < 0) || (slot > 1) || (c1_ptr == NULL)) {
    return;
  }
  xfer_ptr = &(xfer_sites[slot]);

  /* configuration check */
  if (xfer_ptr->site_class != XFER_CITY) {
    errormsg("ERROR:  This stuff doesn't belong in a city");
    return;
  }
  city_ptr = c1_ptr;
  s1_ptr = &(sct[CITY_XLOC][CITY_YLOC]);

  /* transfer civilians */
  if (CITY_PEOPLE > 0) {
    if (CITY_PEOPLE != xfer_ptr->crews) {
      crew_shift = xfer_ptr->crews - CITY_PEOPLE;
      CITY_PEOPLE += crew_shift;
      CADJPEOP;
      if (CITY_PEOPLE < 0) {
	CITY_PEOPLE = 0;
      }
      CADJPEOP;
    } else {
      crew_shift = 0;
    }
  } else {
    crew_shift = 0;
  }
  num_people = xfer_ptr->civies - s1_ptr->people;
  if (num_people != 0) {

    /* transfer the sector location */
    int oxloc = XREAL;
    int oyloc = YREAL;
    xcurs = CITY_XLOC - xoffset;
    ycurs = CITY_YLOC - yoffset;

    /* place the difference into the sector */
    sct[XREAL][YREAL].people += num_people;
    SADJPEOP;

    /* replace the sector location */
    xcurs = oxloc - xoffset;
    ycurs = oyloc - yoffset;

    /* check the adjustment */
    if ((CITY_PEOPLE >= 0) &&
	(num_people - crew_shift < 0)) {
      CITY_PEOPLE += (num_people - crew_shift);
      if (CITY_PEOPLE < 0) {
	CITY_PEOPLE = 0;
      }
      CADJPEOP;
    }

  }

  /* transfer the materials */
  for (i = 0; i < MTRLS_NUMBER; i++) {
    xch_mtrl = xfer_ptr->mtrls[i] - CITY_IMTRLS[i];
    if (xch_mtrl < 0) {
      CITY_IMTRLS[i] = xfer_ptr->mtrls[i];
      if (CITY_IMTRLS[i] < (itemtype) 0) {
	CITY_IMTRLS[i] = (itemtype) 0;
      }
      CADJIMTRLS(i);
      CITY_MTRLS[i] += xch_mtrl;
      CADJMTRLS(i);
    } else {
      CITY_MTRLS[i] += xch_mtrl;
      CADJMTRLS(i);
    }
  }

  /* fix before leaving */
  city_ptr = chold_ptr;
}

/* NAVY2XFER -- Copy a navy structure into the indicated xfer slot */
void
navy2xfer PARM_2(int, slot, NAVY_PTR, n1_ptr)
{
  XFER_PTR xfer_ptr;
  int i;

  /* reality check */
  if ((slot < 0) || (slot > 1) || (n1_ptr == NULL)) {
    return;
  }
  xfer_ptr = &(xfer_sites[slot]);

  /* set the description */
  sprintf(xfer_ptr->description, "Naval Fleet %d", n1_ptr->navyid);
  xfer_ptr->xloc = n1_ptr->xloc;
  xfer_ptr->yloc = n1_ptr->yloc;

  /* now file the information */
  xfer_ptr->site_class = XFER_NAVY;

  /* set the current storage */
  xfer_ptr->civies = (long) n1_ptr->people *
    navy_holds(n1_ptr, NSHP_GALLEYS);
  xfer_ptr->onb_id = n1_ptr->armynum;
  xfer_ptr->onb_cid = n1_ptr->cvnnum;
  for (i = 0; i < MTRLS_NUMBER; i++) {
    xfer_ptr->mtrls[i] = n1_ptr->mtrls[i];
  }

  /* set it up */
  if (n1_ptr->crew > 0) {
    xfer_ptr->crews = (long) 0;
    for (i = 0; i < NSHP_NUMBER; i++) {
      xfer_ptr->crews += (long) (n1_ptr->crew - 1) *
	navy_holds(n1_ptr, i);
    }
  } else {
    xfer_ptr->crews = 0;
  }

  /* set the max values */
  xfer_ptr->mtrl_max = ((long) ninfo_list[NSHP_MERCHANTS].capacity *
			navy_holds(n1_ptr, NSHP_MERCHANTS));
  xfer_ptr->civ_div = (int) navy_holds(n1_ptr, NSHP_GALLEYS);
  xfer_ptr->civ_max = ((long) ninfo_list[NSHP_GALLEYS].capacity *
		       xfer_ptr->civ_div) / 1000;
  xfer_ptr->onb_max = ((long) ninfo_list[NSHP_WARSHIPS].capacity *
		       navy_holds(n1_ptr, NSHP_WARSHIPS)) / 1000;
  xfer_ptr->onb_cmax = ((long) ninfo_list[NSHP_BARGES].capacity *
			navy_holds(n1_ptr, NSHP_BARGES)) / 10000;
  xfer_ptr->crew_div = (int) 0;
  for (i = 0; i < NSHP_NUMBER; i++) {
    xfer_ptr->crew_div += (int) navy_holds(n1_ptr, i);
  }
  xfer_ptr->crew_max = ((long) (MAXNAVYCREW - (n1_ptr->crew > 0)) *
			xfer_ptr->crew_div);
}

/* XFER2NAVY -- Finalize the changes made on the commodity */
void
xfer2navy PARM_2(int, slot, NAVY_PTR, n1_ptr)
{
  NAVY_PTR nhold_ptr = navy_ptr;
  XFER_PTR xfer_ptr;
  int i, j;

  /* reality check */
  if ((slot < 0) || (slot > 1) || (n1_ptr == NULL)) {
    return;
  }
  xfer_ptr = &(xfer_sites[slot]);

  /* configuration check */
  if (xfer_ptr->site_class != XFER_NAVY) {
    errormsg("ERROR:  This stuff doesn't belong on a fleet");
    return;
  }
  navy_ptr = n1_ptr;

  /* check the army storage */
  if (NAVY_ARMY != xfer_ptr->onb_id) {
    NAVY_ARMY = xfer_ptr->onb_id;
    NADJARMY;
  }
  if (NAVY_CVN != xfer_ptr->onb_cid) {
    NAVY_CVN = xfer_ptr->onb_cid;
    NADJCVN;
  }

  /* check the materials onboard */
  for (i = 0; i < MTRLS_NUMBER; i++) {
    if (xfer_ptr->mtrls[i] != NAVY_MTRLS[i]) {
      NAVY_MTRLS[i] = xfer_ptr->mtrls[i];
      NMTRLS(i);
    }
  }

  /* check the civilians */
  if ((i = navy_holds(navy_ptr, NSHP_GALLEYS)) > 0) {
    i = (int) (xfer_ptr->civies / i);
    if (i != (int) NAVY_PEOP) {
      NAVY_PEOP = (uns_char) i;
      NADJPEOP;
    }
  }

  /* check the crew */
  i = 0;
  for (j = 0; j < NSHP_NUMBER; j++) {
    i += navy_holds(navy_ptr, j);
  }
  if (i > 0) {
    i = (int) (xfer_ptr->crews / i);
    if (i != ((int) NAVY_CREW + (NAVY_CREW > 0))) {
      NAVY_CREW = (uns_char) (i + (NAVY_CREW > 0));
      NADJCREW;
    }
  }

  /* reset before bugging out */
  navy_ptr = nhold_ptr;
}

/* CVN2XFER -- Copy a carvan structure into the indicated xfer slot */
void
cvn2xfer PARM_2(int, slot, CVN_PTR, v1_ptr)
{
  XFER_PTR xfer_ptr;
  int i;

  /* reality check */
  if ((slot < 0) || (slot > 1) || (v1_ptr == NULL)) {
    return;
  }
  xfer_ptr = &(xfer_sites[slot]);

  /* set the description */
  sprintf(xfer_ptr->description, "Caravan %d", v1_ptr->cvnid);
  xfer_ptr->xloc = v1_ptr->xloc;
  xfer_ptr->yloc = v1_ptr->yloc;

  /* now file the information */
  xfer_ptr->site_class = XFER_CARAVAN;

  /* set the current storage */
  xfer_ptr->civies = ((long) v1_ptr->people) * v1_ptr->size;
  if (v1_ptr->crew > 0) {
    xfer_ptr->crews = ((long) (v1_ptr->crew - 1)) * v1_ptr->size;
  } else {
    xfer_ptr->crews = 0;
  }
  xfer_ptr->onb_id = EMPTY_HOLD;
  xfer_ptr->onb_cid = EMPTY_HOLD;
  for (i = 0; i < MTRLS_NUMBER; i++) {
    xfer_ptr->mtrls[i] = v1_ptr->mtrls[i];
  }

  /* set the max values */
  xfer_ptr->mtrl_max = ((long) CVN_HOLD) * v1_ptr->size;
  xfer_ptr->civ_max = XF_JOINED;
  xfer_ptr->civ_div = (int) v1_ptr->size;
  xfer_ptr->crew_max = ((long) (MAXCVNCREW - (v1_ptr->crew > 0)) *
			v1_ptr->size);
  xfer_ptr->crew_div = (int) v1_ptr->size;
  xfer_ptr->onb_max = 0;
  xfer_ptr->onb_cmax = 0;
}

/* XFER2CVN -- Finalize the changes made on the caravan baggage */
void
xfer2cvn PARM_2(int, slot, CVN_PTR, v1_ptr)
{
  CVN_PTR vhold_ptr = cvn_ptr;
  XFER_PTR xfer_ptr;
  int i;

  /* reality check */
  if ((slot < 0) || (slot > 1) || (v1_ptr == NULL)) {
    return;
  }
  xfer_ptr = &(xfer_sites[slot]);

  /* configuration check */
  if (xfer_ptr->site_class != XFER_CARAVAN) {
    errormsg("ERROR:  This stuff doesn't belong on a caravan");
    return;
  }
  cvn_ptr = v1_ptr;

  /* check the materials onboard */
  for (i = 0; i < MTRLS_NUMBER; i++) {
    if (xfer_ptr->mtrls[i] != CVN_MTRLS[i]) {
      CVN_MTRLS[i] = xfer_ptr->mtrls[i];
      VMTRLS(i);
    }
  }

  /* check the civilians */
  if (CVN_SIZE > 0) {
    i = (int) (xfer_ptr->civies / CVN_SIZE);
    if (i != (int) CVN_PEOP) {
      CVN_PEOP = (uns_char) i;
      VADJPEOP;
    }
  }

  /* check the crew */
  if (CVN_SIZE > 0) {
    i = (int) (xfer_ptr->crews / CVN_SIZE);
    if (i != ((int) CVN_CREW + (CVN_CREW > 0))) {
      CVN_CREW = (uns_char) (i + (CVN_CREW > 0));
      VADJCREW;
    }
  }

  /* reset before buzzing out */
  cvn_ptr = vhold_ptr;
}

/* === From here down, the code is for the interactive transfer mode === */

/* declaration of keybindings at bottom */
extern KBIND_STRUCT xfer_klist[];
extern PARSE_STRUCT xfer_funcs[];
extern KEYSYS_STRUCT xfer_keysys;
KLIST_PTR xfer_bindings = NULL;

/* various storage variables */
static char xfer_indicator[3] = "<>", xfer_error[LINELTH];
static long xfer_xchanges[MTRLS_NUMBER + 2];
static ARMY_PTR xfer_armylist;
static long xfer_civ_units, xfer_crew_units;
static long xfer_took_off;
static int xfer_do_army, xfer_do_mtrls, xfer_do_cvn;
static int xfer_selection, xfer_direction;
static int xf_max_items, xf_min_army;
static int xf_sel_crew, xf_sel_civ, xf_sel_cvn;
static int xfer_aslot[2], xfer_vslot[2];
static int xfer_done;

/* XFER_INIT -- Initialize the keybindings, if needed, and other things */
int
xfer_init PARM_0(void)
{
  static int hd_xchanges = FALSE;
  int i;

  /* check the bindings */
  align_xfer_keys();

  /* calculate the edges */
  if (xfer_do_mtrls == TRUE) {
    xf_max_items = MTRLS_NUMBER;
  } else {
    xf_max_items = 0;
  }
  if (xfer_civ_units > 0) {
    xf_sel_civ = xf_max_items;
    xf_max_items++;
  } else {
    xf_sel_civ = -1;
  }
  if (xfer_crew_units > 0) {
    xf_sel_crew = xf_max_items;
    xf_max_items++;
  } else {
    xf_sel_crew = -1;
  }
  if (xfer_do_cvn == TRUE) {
    xf_sel_cvn = xf_max_items;
    xf_max_items++;
  } else {
    xf_sel_cvn = -1;
  }
  xf_min_army = xf_max_items;
  if (xfer_do_army == TRUE) {
    for (xfer_armylist = ntn_ptr->army_list;
	 xfer_armylist != NULL;
	 xfer_armylist = xfer_armylist->next) {
      if ((xfer_armylist->xloc == xfer_sites[0].xloc) &&
	  (xfer_armylist->yloc == xfer_sites[0].yloc)) {
	break;
      }
    }
    xf_max_items = LINES - XF_USEDLINES;
  } else {
    xfer_armylist = NULL;
  }

  /* set the minimum transfer values */
  if (hd_xchanges == FALSE) {
    for (i = 0; i < MTRLS_NUMBER; i++) {
      xfer_xchanges[i] = 1000;
    }
    xfer_xchanges[i++] = xfer_civ_units;
    xfer_xchanges[i++] = xfer_crew_units;
    hd_xchanges = TRUE;
  } else {
    if ((xfer_civ_units > 0) &&
	((xfer_xchanges[MTRLS_NUMBER] <= 0) ||
	 (xfer_xchanges[MTRLS_NUMBER] % xfer_civ_units != 0))) {
      xfer_xchanges[MTRLS_NUMBER] = xfer_civ_units;
    }
    if ((xfer_crew_units > 0) &&
	((xfer_xchanges[MTRLS_NUMBER + 1] <= 0) ||
	 (xfer_xchanges[MTRLS_NUMBER + 1] % xfer_crew_units != 0))) {
      xfer_xchanges[MTRLS_NUMBER + 1] = xfer_crew_units;
    }
  }
  return(0);
}

/* XFER_UNLIMITED -- Return TRUE if selection has unlimited storage */
static int
xfer_unlimited PARM_2(int, slot, int, select_num)
{
  if ((slot < 0) ||
      (slot > 1) ||
      (select_num < 0) ||
      (select_num >= xf_max_items)) return(FALSE);

  /* check the army selection */
  if (select_num >= xf_min_army) {
    if (xfer_sites[slot].onb_max == XF_NOLIMIT) {
      return(TRUE);
    } else {
      return(FALSE);
    }
  }

  /* check the selection for caravans */
  if (select_num == xf_sel_cvn) {
    if (xfer_sites[slot].onb_cmax == XF_NOLIMIT) {
      return(TRUE);
    } else {
      return(FALSE);
    }
  }

  /* check the materials selection */
  if (select_num == xf_sel_civ) {
    if ((xfer_sites[slot].civ_max == XF_NOLIMIT) ||
	((xfer_sites[slot].civ_max == XF_JOINED) &&
	 (xfer_sites[slot].mtrl_max == XF_NOLIMIT))) {
      return(TRUE);
    }
  } else if (select_num == xf_sel_crew) {
    if ((xfer_sites[slot].crew_max == XF_NOLIMIT) ||
	(xfer_sites[slot].crew_max == XF_JOINED)) {
      /* joined is always unlimited */
      return(TRUE);
    }
  } else {
    if (xfer_sites[slot].mtrl_max == XF_NOLIMIT) {
      return(TRUE);
    }
  }

  /* must be false */
  return(FALSE);
}

/* XFER_SPACE -- The amount of storage available for the selection */
static long
xfer_space PARM_2(int, slot, int, select_num)
{
  long hold = 0;

  /* check the selection */
  if ((slot < 0) ||
      (slot > 1) ||
      (select_num < 0) ||
      (select_num >= xf_max_items)) return(0);

  /* check for a major return */
  if (xfer_unlimited(slot, select_num)) {
    return(BIGINT);
  }

  /* check for army space available */
  if (select_num >= xf_min_army) {
    /* check the army selection */
    hold = xfer_sites[slot].onb_max;
    if (xfer_sites[slot].onb_id != EMPTY_HOLD) {
      hold -= army_load(armybynum(xfer_sites[slot].onb_id));
    }
  } else if (select_num == xf_sel_cvn) {
    /* check the army selection */
    hold = xfer_sites[slot].onb_cmax;
    if (xfer_sites[slot].onb_cid != EMPTY_HOLD) {
      hold -= cvn_load(cvnbynum(xfer_sites[slot].onb_cid));
    }
  } else if (select_num == xf_sel_crew) {
    /* check the crew space */
    hold = xfer_sites[slot].crew_max;
    hold -= xfer_sites[slot].crews;
  } else if (xfer_sites[slot].civ_max == XF_JOINED) {
    /* combined civilians and raw materials */
    hold = xfer_sites[slot].mtrl_max;
    hold -= mtrls_load(&(xfer_sites[slot].mtrls[0]));
    hold -= (xfer_sites[slot].civies * 1000);
    if (select_num == xf_sel_civ) {
      hold /= 1000;
    } else {
      hold /= mtrls_info[select_num].weight;
    }
  } else if (select_num == xf_sel_civ) {
    /* only civilians */
    hold = xfer_sites[slot].civ_max;
    hold -= xfer_sites[slot].civies;
  } else {
    /* only raw materials */
    hold = xfer_sites[slot].mtrl_max;
    hold -= mtrls_load(&(xfer_sites[slot].mtrls[0]));
    hold /= mtrls_info[select_num].weight;
  }

  /* done */
  return(hold);
}

/* XFER_ARMYPTR -- Return a pointer to the indicated army */
static ARMY_PTR
xfer_armyptr PARM_2(int, slot, int, which)
{
  int cnt = 0, target;
  ARMY_PTR a1_ptr;

  /* check input */
  if ((slot < 0) ||
      (slot > 1)) {
    return((ARMY_PTR) NULL);
  }

  /* check the selection */
  which -= xf_min_army;

  /* determine the target */
  target = xfer_aslot[slot] + which;

  /* check the slot */
  if (xfer_sites[slot].onb_id == EMPTY_HOLD) {
    if (xfer_sites[slot].onb_max >= 0) {
      return((ARMY_PTR) NULL);
    }
  } else {
    if (which == 0) {
      /* return the shipboard leader */
      return(armybynum(xfer_sites[slot].onb_id));
    }
    target--;
  }

  /* try to locate the given army */
  for (a1_ptr = xfer_armylist;
       a1_ptr != NULL;
       a1_ptr = a1_ptr->nrby) {

    /* is this the leaderbum? */
    if (xfer_sites[slot].onb_id == a1_ptr->armyid) {
      continue;
    }

    /* check if the bloke is onboard this bugger */
    if (xfer_sites[slot].onb_id == army_shipleader(a1_ptr)) {
      if (++cnt > target) break;
    }

  }

  /* done */
  return(a1_ptr);
}

/* XFER_CVNPTR -- Return a pointer to the desired caravan */
static CVN_PTR
xfer_cvnptr PARM_1(int, slot)
{
  int cnt = 0, target, noton;
  CVN_PTR v1_ptr;

  /* check input */
  if ((slot < 0) ||
      (slot > 1)) {
    return((CVN_PTR) NULL);
  }

  /* check if the item carries caravans */
  if (xfer_sites[slot].onb_cid != EMPTY_HOLD) {
    /* got it */
    return(cvnbynum(xfer_sites[slot].onb_cid));
  } else if (xfer_sites[slot].onb_cmax >= 0) {
    return((CVN_PTR) NULL);
  }

  /* try to locate the given caravan */
  target = xfer_vslot[slot];
  noton = xfer_sites[(slot + 1) % 1].onb_cid;
  for (v1_ptr = ntn_ptr->cvn_list;
       v1_ptr != NULL;
       v1_ptr = v1_ptr->next) {

    /* check if the bloke is in this sector */
    if ((v1_ptr->xloc == xfer_sites[slot].xloc) &&
	(v1_ptr->yloc == xfer_sites[slot].yloc)) {

      /* skip over any already on the other side */
      if (noton == v1_ptr->cvnid) continue;
      if (++cnt > target) break;
    }

  }

  /* done */
  return(v1_ptr);
}

/* XFER_MAXARMY -- Determine the number of army units in the given site */
static int
xfer_maxarmy PARM_1(int, slot)
{
  ARMY_PTR a1_ptr;
  int cnt = 0;

  /* check input */
  if ((slot < 0) || (slot > 1) ||
      ((xfer_sites[slot].onb_max >= 0) &&
       (xfer_sites[slot].onb_id == EMPTY_HOLD))) {
    return(0);
  }

  /* count up the units in the given site */
  for (a1_ptr = xfer_armylist;
       a1_ptr != NULL;
       a1_ptr = a1_ptr->nrby) {
    /* at this site? */
    if (xfer_sites[slot].onb_id == army_shipleader(a1_ptr)) {
      cnt++;
    }
  }
  return(cnt);
}

/* XFER_MAXCVN -- Determine the number of caravans in the given site */
static int
xfer_maxcvn PARM_1(int, slot)
{
  int cnt = 0, noton;
  CVN_PTR v1_ptr;

  /* check input */
  if ((slot < 0) ||
      (slot > 1)) {
    return(cnt);
  }

  /* check if the item carries caravans */
  if (xfer_sites[slot].onb_cmax >= 0) {
    return(cnt);
  }
  noton = xfer_sites[(slot + 1) % 1].onb_cid;
  for (v1_ptr = ntn_ptr->cvn_list;
       v1_ptr != NULL;
       v1_ptr = v1_ptr->next) {

    /* check if the bloke is in this sector */
    if ((v1_ptr->xloc == xfer_sites[slot].xloc) &&
	(v1_ptr->yloc == xfer_sites[slot].yloc)) {

      /* skip over any already on the other side */
      if (noton == v1_ptr->cvnid) continue;
      cnt++;
    }

  }
  return(cnt);
}

/* XFER_QUIT -- Let them go */
static int
xfer_quit PARM_0(void)
{
  mvaddstr(LINES - 1, 0, "Are you done transfering items? ");
  clrtoeol();
  if (y_or_n()) {
    xfer_done = TRUE;
  }
  return(0);
}

/* XFER_DOWN -- Move the selection pointer down */
static int
xfer_down PARM_0(void)
{
  /* move down if possible */
  if (xfer_selection < xf_max_items - 1) {
    xfer_selection++;
  } else {
    beep();
  }
  return(0);
}

/* XFER_UP -- Move the selection pointer upward */
static int
xfer_up PARM_0(void)
{
  /* move up if possible */
  if (xfer_selection > 0) {
    xfer_selection--;
  } else {
    beep();
  }
  return(0);
}

/* XFER_XFER -- Actually perform the shifting of goods */
static void
xfer_xfer PARM_1(int, all_over)
{
  ARMY_PTR a1_ptr, a2_ptr;
  CVN_PTR v1_ptr, v2_ptr;
  long value;
  int sown;

  /* switch based on the type of the material */
  if (xfer_selection >= xf_min_army) {

    /* is there an army here? */
    if ((a1_ptr = xfer_armyptr(xfer_direction ^ 1, xfer_selection))
	== NULL) {
      beep();
      return;
    }

    /* now find out if it fits */
    if (!xfer_unlimited(xfer_direction, xfer_selection) &&
	(army_load(a1_ptr) >
	 xfer_space(xfer_direction, xfer_selection))) {
      strcpy(xfer_error, "That army does not fit onboard");
      return;
    }

    /* does it need to be unloaded? */
    army_ptr = a1_ptr;
    if (xfer_unlimited(xfer_direction, xfer_selection)) {

      /* needs to be unloaded from the ship */
      if (real_stat(army_ptr) == ST_ONBOARD) {

	/* can it be unloaded? */
	sown = sct[ARMY_XLOC][ARMY_YLOC].owner;
	if ((sown != country) &&
	    (is_god == FALSE)) {
	  if (sown == UNOWNED) {
	    /* can it get there from here? */
	    if (!a_isleader(ARMY_TYPE) &&
		!a_mayunload(ARMY_TYPE)) {
	      errormsg("That army may not disembark in unowned land");
	      return;
	    }
	  } else if ((world.np[sown])->dstatus[country] %
		     dstatus_number != DIP_ALLIED) {
	    /* how about from here? */
	    if (!a_assault(ARMY_TYPE)) {
	      strcpy(xfer_error,
		     "That unit may not disembark in someone else's land");
	      return;
	    } else if (ntn_ptr->dstatus[sown] % dstatus_number < DIP_WAR) {
	      strcpy(xfer_error,
		     "You must declare war before invading another nation");
	      return;
	    }
	  }
	}
	
	/* unload it */
	if (!a_isscout(ARMY_TYPE) &&
	    minor_desg(sct[ARMY_XLOC][ARMY_YLOC].designation, MIN_SIEGED)) {
	  set_status(ARMY_STAT, ST_SIEGED);
	} else {
	  set_status(ARMY_STAT, ST_DEFEND);
	}
	if (unit_speed(ARMY_STAT) == SPD_STUCK) {
	  set_speed(ARMY_STAT, SPD_NORMAL);
	}
	AADJSTAT;

	/* align the speed properly */
	if (ARMY_MOVE > NAVY_MOVE) {
	  ARMY_MOVE = NAVY_MOVE;
	  if (unit_leading(ARMY_STAT)) {
	    set_grpmove(ARMY_ID, unit_speed(ARMY_STAT), ARMY_MOVE);
	    GADJMOVE;
	  } else {
	    AADJMOVE;
	  }
	} else if (NAVY_MOVE > ARMY_MOVE) {
	  NAVY_MOVE = ARMY_MOVE;
	  NADJMOVE;
	}

	/* check for any scouts on board */
	if (unit_leading(ARMY_STAT)) {
	  ungroup_scouts(ARMY_ID);
	}
      }

      /* now undo earlier ship */
      if (xfer_sites[xfer_direction ^ 1].onb_id == ARMY_ID) {
	xfer_sites[xfer_direction ^ 1].onb_id = EMPTY_HOLD;
      }

    } else {

      /* needs to be loaded onto the ship */
      if ((xfer_sites[xfer_direction].onb_id == EMPTY_HOLD) ||
	  ((a2_ptr = armybynum(xfer_sites[xfer_direction].onb_id))
	   == NULL)) {

	/* easy load up */
	xfer_sites[xfer_direction].onb_id = ARMY_ID;
	if (unit_status(ARMY_STAT) != ST_ONBOARD) {
	  set_status(ARMY_STAT, ST_ONBOARD);
	  AADJSTAT;
	  if (ARMY_MOVE > NAVY_MOVE) {
	    ARMY_MOVE = NAVY_MOVE;
	    if (unit_leading(ARMY_STAT)) {
	      set_grpmove(ARMY_ID, unit_speed(ARMY_STAT), ARMY_MOVE);
	      GADJMOVE;
	    } else {
	      AADJMOVE;
	    }
	  } else if (NAVY_MOVE > ARMY_MOVE) {
	    NAVY_MOVE = ARMY_MOVE;
	    NADJMOVE;
	  }
	}

      } else if (a_isleader(a2_ptr->unittype)) {

	/* group under the leader */
	if (unit_status(ARMY_STAT) != ST_GROUPED) {
	  set_status(ARMY_STAT, ST_GROUPED);
	  AADJSTAT;
	}
	if (ARMY_LEAD != a2_ptr->armyid) {
	  ARMY_LEAD = a2_ptr->armyid;
	  AADJLEAD;
	}
	if (ARMY_MOVE > NAVY_MOVE) {
	  ARMY_MOVE = NAVY_MOVE;
	  if (unit_leading(ARMY_STAT)) {
	    set_grpmove(ARMY_ID, unit_speed(ARMY_STAT), ARMY_MOVE);
	    GADJMOVE;
	  } else {
	    AADJMOVE;
	  }
	} else if (NAVY_MOVE > ARMY_MOVE) {
	  NAVY_MOVE = ARMY_MOVE;
	  NADJMOVE;
	}

	/* now check the leadership setting */
	if (!unit_leading(a2_ptr->status)) {
	  army_ptr = a2_ptr;
	  set_leading(ARMY_STAT);
	  AADJSTAT;
	}

      } else if (a_isleader(ARMY_TYPE)) {

	/* have the leader board the ship */
	xfer_sites[xfer_direction].onb_id = ARMY_ID;
	set_status(ARMY_STAT, ST_ONBOARD);
	if (!unit_leading(ARMY_STAT)) {
	  set_leading(ARMY_STAT);
	}
	AADJSTAT;
	if (ARMY_MOVE > NAVY_MOVE) {
	  ARMY_MOVE = NAVY_MOVE;
	  if (unit_leading(ARMY_STAT)) {
	    set_grpmove(ARMY_ID, unit_speed(ARMY_STAT), ARMY_MOVE);
	    GADJMOVE;
	  } else {
	    AADJMOVE;
	  }
	} else if (NAVY_MOVE > ARMY_MOVE) {
	  NAVY_MOVE = ARMY_MOVE;
	  NADJMOVE;
	}

	/* now group the first bugger under this guy */
	if (ARMY_ID != a2_ptr->leader) {
	  a2_ptr->leader = ARMY_ID;
	  army_ptr = a2_ptr;
	  AADJLEAD;
	}
	army_ptr = a2_ptr;
	set_status(ARMY_STAT, ST_GROUPED);
	AADJSTAT;

      } else {

	/* can't board this ship */
	strcpy(xfer_error, "This unit must be lead onto the ship");
	return;

      }

      /* now undo earlier ship */
      if (xfer_sites[xfer_direction ^ 1].onb_id == a1_ptr->armyid) {
	xfer_sites[xfer_direction ^ 1].onb_id = EMPTY_HOLD;
      }

    }

  } else if (xfer_selection == xf_sel_cvn) {

    /* is there a caravan here? */
    if ((v1_ptr = xfer_cvnptr(xfer_direction ^ 1)) == NULL) {
      beep();
      return;
    }

    /* now find out if it fits */
    if (!xfer_unlimited(xfer_direction, xfer_selection) &&
	(cvn_load(v1_ptr) >
	 xfer_space(xfer_direction, xfer_selection))) {
      strcpy(xfer_error, "That caravan does not fit onboard");
      return;
    }

    /* does it need to be unloaded? */
    cvn_ptr = v1_ptr;
    if (xfer_unlimited(xfer_direction, xfer_selection)) {

      /* needs to be unloaded from the ship */
      if (onship_stat(CVN_STAT)) {
	sown = sct[CVN_XLOC][CVN_YLOC].owner;
	if ((is_god == FALSE) &&
	    (sown != country) &&
	    ((world.np[sown])->dstatus[country] % dstatus_number >=
	     DIP_NEUTRAL)) {
	  errormsg("You cannot unload a caravan in this sector");
	  return;
	}

	/* now fix the status */
	if (unit_status(CVN_STAT) == ST_ONBSPLY) {
	  set_status(CVN_STAT, ST_SUPPLY);
	} else if (unit_status(CVN_STAT) == ST_ONBSSPLY) {
	  set_status(CVN_STAT, ST_SSUPPLY);
	} else {
	  set_status(CVN_STAT, ST_CARRY);
	}
	VADJSTAT;

	/* align the movement properly */
	if (CVN_MOVE > NAVY_MOVE) {
	  CVN_MOVE = NAVY_MOVE;
	  VADJMOVE;
	} else if (NAVY_MOVE > CVN_MOVE) {
	  NAVY_MOVE = CVN_MOVE;
	  NADJMOVE;
	}

      }

      /* now undo earlier ship */
      if (xfer_sites[xfer_direction ^ 1].onb_cid == CVN_ID) {
	xfer_sites[xfer_direction ^ 1].onb_cid = EMPTY_HOLD;
      }

    } else {

      /* align the movement properly */
      if (CVN_MOVE > NAVY_MOVE) {
	CVN_MOVE = NAVY_MOVE;
	VADJMOVE;
      } else if (NAVY_MOVE > CVN_MOVE) {
	NAVY_MOVE = CVN_MOVE;
	NADJMOVE;
      }

      /* place it on this ship */
      if ((xfer_sites[xfer_direction].onb_cid == EMPTY_HOLD) ||
	  ((v2_ptr = cvnbynum(xfer_sites[xfer_direction].onb_cid)) == NULL)) {
	xfer_sites[xfer_direction].onb_cid = CVN_ID;
	if (!onship_stat(CVN_STAT)) {
	  if (unit_status(CVN_STAT) == ST_SUPPLY) {
	    set_status(CVN_STAT, ST_ONBSPLY);
	  } else if (unit_status(CVN_STAT) == ST_SSUPPLY) {
	    set_status(CVN_STAT, ST_ONBSSPLY);
	  } else {
	    set_status(CVN_STAT, ST_ONBOARD);
	  }
	  VADJSTAT;
	}
      } else {
	/* combine the two caravan units into one */
	if (v2_ptr->size + (int) CVN_SIZE >= 256) {
	  errormsg("You cannot combine those two caravans");
	  return;
	} else {
	  if (v2_ptr->efficiency != CVN_EFF) {
	    value = (long) v2_ptr->efficiency * v2_ptr->size +
	      (long) CVN_EFF * CVN_SIZE;
	    v2_ptr->efficiency = value / (v2_ptr->size + CVN_SIZE);
	  }
	  if (v2_ptr->crew != CVN_CREW) {
	    value = (long) v2_ptr->crew * v2_ptr->size +
	      (long) CVN_CREW * CVN_SIZE;
	    v2_ptr->crew = value / (v2_ptr->size + CVN_SIZE);
	  }
	  if (v2_ptr->people != CVN_PEOP) {
	    value = (long) v2_ptr->people * v2_ptr->size +
	      (long) CVN_PEOP * CVN_SIZE;
	    v2_ptr->people = value / (v2_ptr->size + CVN_SIZE);
	  }
	  v2_ptr->size += CVN_SIZE;
	  cvn_ptr = v2_ptr;
	  for (global_int = 0; global_int < MTRLS_NUMBER; global_int++) {
	    if (v1_ptr->mtrls[global_int] != 0) {
	      CVN_MTRLS[global_int] += v1_ptr->mtrls[global_int];
	      VMTRLS(global_int);
	    }
	  }
	  VADJEFF;
	  VADJPEOP;
	  VADJCREW;
	  VADJSIZE;
	  cvn_ptr = v1_ptr;
	  VDESTROY;
	  dest_cvn(CVN_ID);
	}
      }

      /* now undo earlier ship */
      if (xfer_sites[xfer_direction ^ 1].onb_cid == CVN_ID) {
	xfer_sites[xfer_direction ^ 1].onb_cid = EMPTY_HOLD;
      }

    }

  } else if (xfer_selection == xf_sel_crew) {

    /* crew adjustment */
    if (all_over) {
      value = min(xfer_space(xfer_direction, xfer_selection),
		  xfer_sites[xfer_direction ^ 1].crews);
    } else {
      value = min(xfer_space(xfer_direction, xfer_selection),
		  xfer_xchanges[MTRLS_NUMBER + 1]);
      value = min(value, xfer_sites[xfer_direction ^ 1].crews);
    }

    /* check */
    if (value <= 0) {
      strcpy(xfer_error, "You may not transfer any more crew");
      return;
    }

    /* do the addition */
    if (xfer_sites[xfer_direction].crew_max == XF_JOINED) {
      xfer_sites[xfer_direction].crews += value;
      xfer_sites[xfer_direction].civies += value;
    } else {
      xfer_sites[xfer_direction].crews += value;
    }

    /* do the subtraction */
    if (xfer_sites[xfer_direction ^ 1].crew_max == XF_JOINED) {
      xfer_sites[xfer_direction ^ 1].civies -= value;
      xfer_sites[xfer_direction ^ 1].crews -= value;
    } else {
      xfer_sites[xfer_direction ^ 1].crews -= value;
    }

  } else if (xfer_selection == xf_sel_civ) {

    /* civilian transportation */
    if (all_over) {
      value = min(xfer_space(xfer_direction, xfer_selection),
		  xfer_sites[xfer_direction ^ 1].civies);
    } else {
      value = min(xfer_space(xfer_direction, xfer_selection),
		  xfer_xchanges[MTRLS_NUMBER]);
      value = min(value, xfer_sites[xfer_direction ^ 1].civies);
    }

    /* check */
    if (value <= 0) {
      strcpy(xfer_error, "You may not transfer any more civilians");
      return;
    }

    /* make adjustments */
    xfer_sites[xfer_direction].civies += value;
    xfer_sites[xfer_direction ^ 1].civies -= value;

    /* do the addition */
    if (xfer_sites[xfer_direction].crew_max == XF_JOINED) {
      if (xfer_took_off > 0) {
	if (value > xfer_took_off) {
	  value = xfer_took_off;
	}
	xfer_sites[xfer_direction].crews += value;
	xfer_took_off -= value;
      }
    }

    /* do the subtraction */
    if (xfer_sites[xfer_direction ^ 1].crew_max == XF_JOINED) {
      if (xfer_sites[xfer_direction ^ 1].civies <
	  xfer_sites[xfer_direction ^ 1].crews) {
	xfer_took_off += min(value, xfer_sites[xfer_direction ^ 1].crews);
	xfer_sites[xfer_direction ^ 1].crews -=
	  min(value, xfer_sites[xfer_direction ^ 1].crews);
      }
    }

  } else {

    /* materials transfer */
    if (all_over) {
      value = min(xfer_space(xfer_direction, xfer_selection),
		  xfer_sites[xfer_direction ^ 1].mtrls[xfer_selection]);
    } else {
      value = min(xfer_space(xfer_direction, xfer_selection),
		  xfer_xchanges[xfer_selection]);
      value = min(value,
		  xfer_sites[xfer_direction ^ 1].mtrls[xfer_selection]);
    }

    /* check */
    if (value <= 0) {
      strcpy(xfer_error, "You may not transfer any more of that raw material");
      return;
    }

    /* do the exchange */
    xfer_sites[xfer_direction].mtrls[xfer_selection] += value;
    xfer_sites[xfer_direction ^ 1].mtrls[xfer_selection] -= value;

  }
}

/* XFER_LEFT -- Shift things to the left site */
static int
xfer_left PARM_0(void)
{
  /* are we going this way already? */
  if (xfer_direction == 0) {
    /* shift everything over */
    xfer_xfer(TRUE);
  } else {
    /* then go this way */
    xfer_direction = 0;
  }
  return(0);
}

/* XFER_RIGHT -- Shift things to the right site */
static int
xfer_right PARM_0(void)
{
  /* are we going this way already? */
  if (xfer_direction == 1) {
    /* shift everything over */
    xfer_xfer(TRUE);
  } else {
    /* then go this way */
    xfer_direction = 1;
  }
  return(0);
}

/* XFER_MOVE -- Shift the items from one side to the other */
static int
xfer_move PARM_0(void)
{
  /* just do it */
  xfer_xfer(FALSE);
  return(0);
}

/* XFER_ADD -- Subtract one unit from the transfer amount */
static int
xfer_add PARM_0(void)
{
  /* check the selection */
  if ((xfer_selection >= xf_min_army) ||
      (xfer_selection == xf_sel_cvn)) {
    beep();
  } else if (xfer_selection == xf_sel_civ) {
    /* simple addition */
    if (xfer_xchanges[MTRLS_NUMBER] >= 1000000) {
      beep();
    } else {
      xfer_xchanges[MTRLS_NUMBER] += xfer_civ_units;
    }
  } else if (xfer_selection == xf_sel_crew) {
    /* simple addition */
    if (xfer_xchanges[MTRLS_NUMBER + 1] >= 1000000) {
      beep();
    } else {
      xfer_xchanges[MTRLS_NUMBER + 1] += xfer_crew_units;
    }
  } else {
    /* simple addition */
    if (xfer_xchanges[xfer_selection] >= 1000000) {
      beep();
    } else {
      xfer_xchanges[xfer_selection]++;
    }
  }
  return(0);
}

/* XFER_SUB -- Subtract a unit from the target amount */
static int
xfer_subtract PARM_0(void)
{
  /* check the selection */
  if ((xfer_selection >= xf_min_army) ||
      (xfer_selection == xf_sel_cvn)) {
    beep();
  } else if (xfer_selection == xf_sel_civ) {
    /* just subtract */
    if (xfer_xchanges[MTRLS_NUMBER] > xfer_civ_units) {
      xfer_xchanges[MTRLS_NUMBER] -= xfer_civ_units;
    } else {
      beep();
    }
  } else if (xfer_selection == xf_sel_crew) {
    /* just subtract */
    if (xfer_xchanges[MTRLS_NUMBER + 1] > xfer_crew_units) {
      xfer_xchanges[MTRLS_NUMBER + 1] -= xfer_crew_units;
    } else {
      beep();
    }
  } else {
    /* simple subtraction */
    if (xfer_xchanges[xfer_selection] > 1) {
      xfer_xchanges[xfer_selection]--;
    } else {
      beep();
    }
  }
  return(0);
}

/* XFER_MULT -- Multiply the amounts (by 10) */
static int
xfer_mult PARM_0(void)
{
  /* check the selection */
  if ((xfer_selection >= xf_min_army) &&
      (xfer_selection == xf_sel_cvn)) {
    beep();
  } else if (xfer_selection == xf_sel_civ) {
    /* simple multiplication by 10 */
    if (xfer_xchanges[MTRLS_NUMBER] >= 1000000) {
      beep();
    } else {
      xfer_xchanges[MTRLS_NUMBER] *= 10;
    }
  } else if (xfer_selection == xf_sel_crew) {
    /* simple multiplication by 10 */
    if (xfer_xchanges[MTRLS_NUMBER + 1] >= 1000000) {
      beep();
    } else {
      xfer_xchanges[MTRLS_NUMBER + 1] *= 10;
    }
  } else {
    /* simple multiplication by 10 */
    if (xfer_xchanges[xfer_selection] >= 1000000) {
      beep();
    } else {
      xfer_xchanges[xfer_selection] *= 10;
    }
  }
  return(0);
}

/* XFER_DIV -- Divide the amounts (by 10) */
static int
xfer_div PARM_0(void)
{
  int hold;

  /* check the selection */
  if ((xfer_selection >= xf_min_army) &&
      (xfer_selection == xf_sel_cvn)) {
    beep();
  } else if (xfer_selection == xf_sel_civ) {

    /* division by 10 and check for proper settings */
    hold = xfer_xchanges[MTRLS_NUMBER] / 10;
    if (hold < xfer_civ_units) {
      hold = xfer_civ_units;
    } else if ((hold % xfer_civ_units) != 0) {
      hold = (hold / xfer_civ_units) * xfer_civ_units;
    }
    xfer_xchanges[MTRLS_NUMBER] = hold;

  } else if (xfer_selection == xf_sel_crew) {

    /* division by 10 and check for proper settings */
    hold = xfer_xchanges[MTRLS_NUMBER + 1] / 10;
    if (hold < xfer_crew_units) {
      hold = xfer_crew_units;
    } else if ((hold % xfer_crew_units) != 0) {
      hold = (hold / xfer_crew_units) * xfer_crew_units;
    }
    xfer_xchanges[MTRLS_NUMBER + 1] = hold;

  } else {

    /* division by 10 and check for proper settings */
    hold = xfer_xchanges[xfer_selection] / 10;
    if (hold < 1) {
      hold = 1;
    }
    xfer_xchanges[xfer_selection] = hold;

  }
  return(0);
}

/* XFER_ASSIGN -- Assign the value to the amounts */
static int
xfer_assign PARM_0(void)
{
  long value = 0;

  /* check selection */
  if ((xfer_selection >= xf_min_army) ||
      (xfer_selection == xf_sel_cvn)) {
    beep();
  } else if (xfer_selection == xf_sel_civ) {
    /* civilians setting */
    mvprintw(LINES - 1, 0,
	     "Set transfer amount to how many civilians [Mult of %ld] ? ",
	     xfer_civ_units);
    clrtoeol();
    value = get_number(FALSE);
    if (no_input == TRUE) return(0);
    if (value == 0) {
      errormsg("You may only set some amount greater than zero");
    } else if ((value % xfer_civ_units) != 0) {
      sprintf(string, "That was not divisible by %ld", xfer_civ_units);
      errormsg(string);
    } else {
      xfer_xchanges[MTRLS_NUMBER] = value;
    }
  } else if (xfer_selection == xf_sel_crew) {
    /* civilians setting */
    mvprintw(LINES - 1, 0,
	     "Set transfer amount to how much crew [Mult of %ld] ? ",
	     xfer_crew_units);
    clrtoeol();
    value = get_number(FALSE);
    if (no_input == TRUE) return(0);
    if (value == 0) {
      errormsg("You may only set some amount greater than zero");
    } else if ((value % xfer_crew_units) != 0) {
      sprintf(string, "That was not divisible by %ld", xfer_crew_units);
      errormsg(string);
    } else {
      xfer_xchanges[MTRLS_NUMBER + 1] = value;
    }
  } else {
    /* some other materials */
    mvprintw(LINES - 1, 0,
	     "Set transfer amount to how many %s? ",
	     mtrls_info[xfer_selection].lname);
    clrtoeol();
    value = get_number(FALSE);
    if (no_input == TRUE) return(0);
    if (value == 0) {
      errormsg("You may only set some amount greater than zero");
    } else {
      xfer_xchanges[xfer_selection] = value;
    }
  }
  return(0);
}

/* XFER_SHIFTUP -- Shift the army or caravan units upward */
static int
xfer_shiftup PARM_0(void)
{
  int which_slot = xfer_direction ^ 1;

  /* simple check */
  if (xfer_selection >= xf_min_army) {
    if (xfer_aslot[which_slot] >= xfer_maxarmy(which_slot) - 1) {
      beep();
    } else {
      xfer_aslot[which_slot]++;
    }
  } else if (xfer_selection == xf_sel_cvn) {
    if (xfer_vslot[which_slot] >= xfer_maxcvn(which_slot) - 1) {
      beep();
    } else {
      xfer_vslot[which_slot]++;
    }
  } else {
    beep();
  }
  return(0);
}

/* XFER_SHIFTDOWN -- Shift the army or caravan units downward */
static int
xfer_shiftdown PARM_0(void)
{
  int which_slot = xfer_direction ^ 1;

  /* army shift */
  if (xfer_selection >= xf_min_army) {
    /* simple check */
    if (xfer_aslot[which_slot] <= 0) {
      beep();
    } else {
      xfer_aslot[which_slot]--;
    }
  } else if (xfer_selection == xf_sel_cvn) {
    /* simple check */
    if (xfer_vslot[which_slot] <= 0) {
      beep();
    } else {
      xfer_vslot[which_slot]--;
    }
  } else {
    beep();
  }
  return(0);
}

/* XFER_OPTIONS -- Perform the various conquer options */
static int
xfer_options PARM_0(void)
{
  option_cmd(xfer_keysys, &xfer_bindings);
  return(0);
}

/* XFER_HELP -- Show some documentation */
static int
xfer_help PARM_0(void)
{
  /* create the help system */
  create_help("Conquer Transfer Mode Command List",
	      xfer_bindings, xfer_funcs, xfer_keysys.num_parse);
  return(0);
}

/* XFER_CAPSTR -- Build the capacity information into the string */
static void
xfer_capstr PARM_3(int, slot, int, select_num, char *, out_str)
{
  long lval;

  /* check the input */
  if (out_str == NULL) return;
  out_str[0] = '\0';
  if ((slot < 0) || (slot > 1)) return;
  if ((select_num < 0) ||
      (select_num >= xf_max_items) ||
      (select_num > xf_min_army)) return;

  /* check for unlimited entries */
  if (xfer_unlimited(slot, select_num)) {
    strcpy(out_str, "unlimited");
    return;
  }

  /* now figure out what item is in use */
  if (select_num == xf_min_army) {
    lval = xfer_space(slot, select_num);
    sprintf(out_str, "%ld %s", lval, (lval != 1) ? "men" : "man");
  } else if (select_num == xf_sel_cvn) {
    lval = xfer_space(slot, select_num);
    sprintf(out_str, "%ld %s", lval, (lval != 1) ? "wagons" : "wagon");
  } else {
    sprintf(out_str, "%ld", xfer_space(slot, select_num));
  }
}

/* XFER_STOSTR -- Build the storage information into the string */
static void
xfer_stostr PARM_3(int, slot, int, select_num, char *, out_str)
{
  ARMY_PTR a1_ptr;
  CVN_PTR v1_ptr;

  /* check the input */
  if (out_str == NULL) return;
  out_str[0] = '\0';
  if ((slot < 0) || (slot > 1)) return;
  if ((select_num < 0) ||
      (select_num >= xf_max_items)) return;

  /* determine the item type */
  if (select_num >= xf_min_army) {
    if ((a1_ptr = xfer_armyptr(slot, select_num)) != NULL) {
      if (a_isnormal(a1_ptr->unittype)) {
	sprintf(out_str, "%s %d",
		ainfo_list[a1_ptr->unittype].sname,
		a1_ptr->armyid);
      } else {
	sprintf(out_str, "%s %d", ainfo_list[a1_ptr->unittype].name,
		a1_ptr->armyid);
      }
    }
  } else if (select_num == xf_sel_cvn) {
    if ((v1_ptr = xfer_cvnptr(slot)) != NULL) {
      sprintf(out_str, "caravan %d", v1_ptr->cvnid);
    }
  } else if (select_num == xf_sel_civ) {
    sprintf(out_str, "%ld", xfer_sites[slot].civies);
  } else if (select_num == xf_sel_crew) {
    sprintf(out_str, "%ld", xfer_sites[slot].crews);
  } else {
    sprintf(out_str, "%.0f",
	    (double) xfer_sites[slot].mtrls[select_num]);
  }
}

/* XFER_MIDSTR -- Build up the middle string */
static void
xfer_midstr PARM_2(int, select_num, char *, out_str)
{
  ARMY_PTR a1_ptr;
  CVN_PTR v1_ptr;
  long lval;

  /* check the input */
  if (out_str == NULL) return;
  out_str[0] = '\0';
  if ((select_num < 0) ||
      (select_num >= xf_max_items)) return;

  /* determine the type */
  if (select_num >= xf_min_army) {
    if ((a1_ptr = xfer_armyptr(xfer_direction ^ 1,
			       select_num)) != NULL) {
      lval = army_load(a1_ptr);
      sprintf(out_str, "%ld %s", lval, (lval != 1) ? "men" : "man");
    }
  } else if (select_num == xf_sel_cvn) {
    if ((v1_ptr = xfer_cvnptr(xfer_direction ^ 1)) != NULL) {
      lval = cvn_load(v1_ptr);
      sprintf(out_str, "%ld %s", lval, (lval != 1) ? "wagons" : "wagon");
    }
  } else if (select_num == xf_sel_civ) {
    /* just show the transfer amount */
    sprintf(out_str, "%ld", xfer_xchanges[MTRLS_NUMBER]);
  } else if (select_num == xf_sel_crew) {
    /* just show the transfer amount */
    sprintf(out_str, "%ld", xfer_xchanges[MTRLS_NUMBER + 1]);
  } else {
    /* just show the transfer amounts */
    sprintf(out_str, "%ld", xfer_xchanges[select_num]);
  }
}

/* XFER_SHOW -- Display the gathered information */
static void
xfer_show PARM_0(void)
{
  int i, xloc, yloc, width, apt, bpt;
  char astr[LINELTH], bstr[LINELTH];

  /* calculate the width of the information squares */
  width = (COLS - (XF_INDENT + XF_SPACING)) / 5;

  /* start with the bottom title */
  errorbar("Transfer Mode", "Hit '?' for Bindings");

  /* show the site titles */
  xloc = XF_INDENT + width + 2;
  for (i = 0; i < 2; i++) {
    /* center the item in the appropriate position */
    xloc -= (strlen(xfer_sites[i].description) / 2);
    standout();
    mvaddstr(0, xloc, xfer_sites[i].description);
    standend();
    xloc = XF_INDENT + 4 * (width + 1) + 3;
  }

  /* now display the item titles */
  yloc = 4;
  if (xfer_do_mtrls == TRUE) {
    for (i = 0; i < MTRLS_NUMBER; i++) {
      mvaddstr(yloc++, 1, mtrls_info[i].name);
    }
  }
  if (xfer_civ_units > 0) {
    mvaddstr(yloc++, 1, "Civilians");
  }
  if (xfer_crew_units > 0) {
    mvaddstr(yloc++, 1, "Crew");
  }
  if (xfer_do_cvn == TRUE) {
    mvaddstr(yloc++, 1, "Caravans");
  }
  if (xfer_do_army == TRUE) {
    mvaddstr(yloc++, 1, "Armies");
  }

  /* now display the column headings */
  yloc = 2;
  xloc = XF_INDENT;
  for (i = 0; i < 2; i++) {

    /* find the central points */
    apt = xloc + (width + 1) / 2 + 1;
    bpt = xloc + (3 * width + 1) / 2 + 2;

    /* build the items */
    strcpy((i == 0) ? astr : bstr, "Capacity");
    apt -= strlen(astr) / 2;
    strcpy((i == 0) ? bstr : astr, "Storage");
    bpt -= strlen(bstr) / 2;

    /* now position */
    mvaddstr(yloc, apt, astr);
    mvaddstr(yloc, bpt, bstr);

    /* build the separator line */
    mvaddch(yloc + 1, xloc, '|');
    for (apt = 0; apt < 2; apt++) {
      for (bpt = 0; bpt < width; bpt++) {
	addch('-');
      }
      addch('|');
    }

    /* next positioning */
    xloc += (5 + 3 * width);
  }

  /* provide the title of the amount column */
  xloc = XF_INDENT + (5 * width + 1) / 2 + 4;
  strcpy(astr, "Amounts");
  xloc -= strlen(astr) / 2;
  mvaddstr(yloc + 1, xloc, astr);

  /* now display all of the possible selections */
  yloc = 4;
  for (i = 0; i < xf_max_items; i++) {

    /* Build up the strings for the first site */
    xloc = XF_INDENT;
    xfer_capstr(0, i, astr);
    xfer_stostr(0, i, bstr);
    mvprintw(yloc, xloc, "|%*s|%*s|",
	     width, astr, width, bstr);

    /* now build up the middle string */
    xloc += (2 * width + 3);
    xfer_midstr(i, astr);
    apt = (width - strlen(astr) + 1) / 2;
    if (apt < 0) apt = 0;
    if (i == xfer_selection) {
      sprintf(bstr, "%c%*s%-*s%c",
	      xfer_indicator[xfer_direction],
	      apt, "",
	      width - apt, astr,
	      xfer_indicator[xfer_direction]);
      standout();
      mvaddstr(yloc, xloc, bstr);
      standend();
    } else {
      mvaddstr(yloc, xloc + apt + 1, astr);
    }

    /* now conclude with the second site */
    xloc += (width + 2);
    xfer_capstr(1, i, bstr);
    xfer_stostr(1, i, astr);
    mvprintw(yloc++, xloc, "|%*s|%*s|",
	     width, astr, width, bstr);

  }

  /* now complete the box */
  xloc = XF_INDENT;
  for (i = 0; i < 2; i++) {

    /* draw the bottom line */
    mvaddch(yloc, xloc, '+');
    for (apt = 0; apt < 2; apt++) {
      for (bpt = 0; bpt < width; bpt++) {
	addch('-');
      }
      addch('+');
    }

    /* next positioning */
    xloc += (5 + 3 * width);
  }

  /* finish with the quickie instructions */
  if (xfer_error[0] != '\0') {
    bottommsg(xfer_error);
    xfer_error[0] = '\0';
  } else {
    mvaddstr(LINES - 1, 3,
	     "' ' - xfer  '<'- xfer left '>' - xfer right  '=' - set amount  'q' - quit");
  }
  refresh();
}

/* GET_DIVUNITS -- Get the lowest common multiple of the items */
static long
get_divunits PARM_2(long, a, long, b)
{
  long tmp, hold = a * b;

  /* check for negatives */
  if ((a == 0) ||
      (b == 0)) {
    return(0);
  }
  if (a < 0) {
    if (b < 0) {
      return(0);
    }
    return(b);
  }
  if (b < 0) {
    return(a);
  }

  /* compute the greatest common divisor */
  while ((b > 1) && (a % b != 0)) {
    tmp = b;
    b = a % b;
    a = tmp;
  }

  /* should be it now */
  return(hold / b);
}

/* XFER_MODE -- Interactively transfer items between two storage elements */
void
xfer_mode PARM_0(void)
{
  FNCI fnc_ptr;
  int i;

  /* check mtrls input */
  if ((xfer_sites[0].mtrl_max == 0) ||
      (xfer_sites[1].mtrl_max == 0)) {
    xfer_do_mtrls = FALSE;
  } else {
    xfer_do_mtrls = TRUE;
  }

  /* check the input */
  xfer_civ_units = get_divunits(xfer_sites[0].civ_div,
				xfer_sites[1].civ_div);
  xfer_crew_units = get_divunits(xfer_sites[0].crew_div,
				 xfer_sites[1].crew_div);

  /* now check the army and caravan transference */
  if ((xfer_sites[0].site_class != XFER_NAVY) &&
      (xfer_sites[1].site_class != XFER_NAVY)) {
    xfer_do_army = FALSE;
    xfer_do_cvn = FALSE;
  } else {
    xfer_do_cvn = TRUE;
    xfer_do_army = TRUE;
    for (i = 0; i < 2; i++) {
      if (xfer_sites[i].onb_max == 0) {
	xfer_do_army = FALSE;
      }
      if (xfer_sites[i].onb_cmax == 0) {
	xfer_do_cvn = FALSE;
      }
    }
  }

  /* initialize some stuff */
  xfer_init();
  xfer_aslot[0] = xfer_aslot[1] = 0;
  xfer_vslot[0] = xfer_vslot[1] = 0;
  xfer_selection = 0;
  xfer_direction = 0;
  xfer_took_off = 0;
  xfer_done = FALSE;
  xfer_error[0] = '\0';

  /* check if there is anything to transfer */
  if (xf_max_items == 0) {
    errormsg("There is nothing which can be transfered between these sites");
    return;
  }

  /* go into the xfer loop */
  while (xfer_done == FALSE) {

    /* display the transfer screen */
    move(0, 0);
    clrtobot();
    xfer_show();

    /* do the keyboard thing */
    if ((fnc_ptr = parse_keys(xfer_bindings, FALSE)) == NULL) {

      /* bad input */
      clear_bottom(1);
      mvaddstr(LINES - 1, 0, "Unknown key binding: ");
      show_str(string, FALSE);
      presskey();

    } else {

      /* perform the function */
      (*fnc_ptr)();

    }

  }
}

/* -=- do the functions and default keybindings -=- */

PARSE_STRUCT xfer_funcs[] = {
  { xfer_options, "conquer-options",
      "Adjust the conquer environment by changing various options" },
  { do_ignore, "ignore-key",
      "I'll just pretend as if I never saw that key stroke" },
  { do_redraw, "redraw-screen",
      "redraw the display of the screen" },
  { xfer_add, "xfer-add",
      "Xfer-mode, add one unit to the transfer amount" },
  { xfer_assign, "xfer-assign",
      "Xfer-mode, assign a value to the transfer amount" },
  { xfer_div, "xfer-divide",
      "Xfer-mode, divide the transfer amount by ten" },
  { xfer_down, "xfer-down",
      "Xfer-mode, move the selection pointer downward" },
  { xfer_help, "xfer-help",
      "Xfer-mode, provide a listing of the commands in transfer mode" },
  { xfer_left, "xfer-left",
      "Xfer-mode, move goods from the right to the left" },
  { xfer_move, "xfer-move",
      "Xfer-mode, exchange goods between the two sites" },
  { xfer_mult, "xfer-multiply",
      "Xfer-mode, mulitply the transfer amount by ten" },
  { xfer_quit, "xfer-quit",
      "Xfer-mode, exit the transfer mode, finalizing the transfers" },
  { xfer_right, "xfer-right",
      "Xfer-mode, move goods from the left to the right" },
  { xfer_shiftdown, "xfer-shiftdown",
      "Xfer-mode, move the list of army or caravan units downward" },
  { xfer_shiftup, "xfer-shiftup",
      "Xfer-mode, move the list of army or caravan units upward" },
  { xfer_subtract, "xfer-subtract",
      "Xfer-mode, subtract one unit from the transfer amount" },
  { xfer_up, "xfer-up",
      "Xfer-mode, move the selection pointer upward" }
};

KBIND_STRUCT xfer_klist[] = {
  { ESTR_IGN, do_ignore },
  { CSTR_B, xfer_left },
  { CSTR_F, xfer_right },
  { CSTR_L, do_redraw },
  { CSTR_N, xfer_down },
  { CSTR_P, xfer_up },
  { CSTR_R, do_redraw },
  { AKEY_UP1, xfer_up },
  { AKEY_UP2, xfer_up },
  { AKEY_DOWN1, xfer_down },
  { AKEY_DOWN2, xfer_down },
  { AKEY_LEFT1, xfer_left },
  { AKEY_LEFT2, xfer_left },
  { AKEY_RIGHT1, xfer_right },
  { AKEY_RIGHT2, xfer_right },
  { "\t", do_ignore },
  { "?", xfer_help },
  { "<", xfer_left },
  { ">", xfer_right },
  { "=", xfer_assign },
  { "*", xfer_mult },
  { "/", xfer_div },
  { "+", xfer_add },
  { "-", xfer_subtract },
  { " ", xfer_move },
  { "H", xfer_left },
  { "h", xfer_left },
  { "J", xfer_down },
  { "j", xfer_down },
  { "K", xfer_up },
  { "k", xfer_up },
  { "L", xfer_right },
  { "l", xfer_right },
  { "O", xfer_options },
  { "o", xfer_shiftup },
  { "p", xfer_shiftdown },
  { "Q", xfer_quit },
  { "q", xfer_quit }
};

/* information structure concerning movement keybindings */
KEYSYS_STRUCT xfer_keysys = {
  "xfer", xfer_funcs, xfer_klist, 0, 0
};

/* ALIGN_XFER_KEYS -- Align all of the transfer mode keys */
void
align_xfer_keys PARM_0(void)
{
  /* initialize the system */
  if (xfer_bindings == NULL) {
    xfer_keysys.num_binds = (sizeof(xfer_klist)/sizeof(KBIND_STRUCT));
    xfer_keysys.num_parse = (sizeof(xfer_funcs)/sizeof(PARSE_STRUCT));
    init_keys(&xfer_bindings, xfer_klist, xfer_keysys.num_binds);
  }
}

