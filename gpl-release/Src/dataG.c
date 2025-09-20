// SPDX-License-Identifier: GPL-3.0-or-later
/* Declaration of global variables in the user interface */
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
#define DATA_DECLARE
#include "dataG.h"
#undef DATA_DECLARE
#include "infoG.h"
#include "displayG.h"

/* the default message of the day */
char *dflt_motd[MOTDLINES] = {
  "New movement system installed that has greater accuracy",
  "Expanded and updated documentation system",
  "Many bugs fixed",
  ""
};

/* display options list */
char *display_list[] = {
  "blank", "contour", "designation", "food", "jewel", "metal",
  "nation mark", "people", "race", "vegetation", "wood", "your desg",
  "Army mcost", "Defense", "Flight mcost", "Magic value", "Navy mcost",
  "Tgood desgs", "Values", "Weights", "Keep"
};

/* highlight options list */
char *highl_list[] = {
  "None", "allies", "designation", "enemies", "minor desgs",
  "neutrals", "owned", "range", "scouts", "units", "your units",
  "Movable", "Region", "Supported", "Tradegoods", "Unsupported",
  "Keep"
};

/* the beginning display mode */
DISPLAY_STRUCT display_mode = {
  /* Resembles the Standard Setting Below */
  "Standard", HXPOS_LOWLEFT,
  DI_DESG, DI_BLANK, DI_BLANK, DI_CONT,
  HI_OWN, HI_OWN, HI_OWN, HI_OWN,
  UNOWNED, UNOWNED, UNOWNED, UNOWNED
};

/* the original display modes */
DISPLAY_STRUCT base_modes[DMODE_NUMBER] = {
  { "Standard",
      HXPOS_LOWLEFT,
      DI_DESG, DI_BLANK, DI_BLANK, DI_CONT,
      HI_KEEP, HI_KEEP, HI_KEEP, HI_KEEP,
      0, 0, 0, 0 },
  { "Designation",
      HXPOS_LOWLEFT,
      DI_DESG, DI_KEEP, DI_KEEP, DI_KEEP,
      HI_KEEP, HI_KEEP, HI_KEEP, HI_KEEP,
      0, 0, 0, 0 },
  { "Own Designation",
      HXPOS_LOWLEFT,
      DI_YDESG, DI_KEEP, DI_KEEP, DI_KEEP,
      HI_KEEP, HI_KEEP, HI_KEEP, HI_KEEP,
      0, 0, 0, 0 },
  { "Nation Marks",
      HXPOS_UPLEFT,
      DI_KEEP, DI_KEEP, DI_NATN, DI_KEEP,
      HI_KEEP, HI_KEEP, HI_KEEP, HI_KEEP,
      0, 0, 0, 0 },
  { "Race",
      HXPOS_UPLEFT,
      DI_KEEP, DI_KEEP, DI_RACE, DI_KEEP,
      HI_KEEP, HI_KEEP, HI_KEEP, HI_KEEP,
      0, 0, 0, 0 },
  { "Population",
      HXPOS_LOWRIGHT,
      DI_KEEP, DI_PEOP, DI_KEEP, DI_KEEP,
      HI_KEEP, HI_KEEP, HI_KEEP, HI_KEEP,
      0, 0, 0, 0 },
  { "Contour",
      HXPOS_UPRIGHT,
      DI_KEEP, DI_KEEP, DI_KEEP, DI_CONT,
      HI_KEEP, HI_KEEP, HI_KEEP, HI_KEEP,
      0, 0, 0, 0 },
  { "Vegetation",
      HXPOS_UPRIGHT,
      DI_KEEP, DI_KEEP, DI_KEEP, DI_VEGE,
      HI_KEEP, HI_KEEP, HI_KEEP, HI_KEEP,
      0, 0, 0, 0 },
  { "Tradegoods",
      HXPOS_LOWRIGHT,
      DI_DESG, DI_TGDESGS, DI_KEEP, DI_KEEP,
      HI_KEEP, HI_KEEP, HI_KEEP, HI_KEEP,
      0, 0, 0, 0 },
  { "Jewels",
      HXPOS_UPLEFT,
      DI_KEEP, DI_KEEP, DI_JEWEL, DI_KEEP,
      HI_KEEP, HI_KEEP, HI_KEEP, HI_KEEP,
      0, 0, 0, 0 },
  { "Magics",
      HXPOS_UPLEFT,
      DI_KEEP, DI_KEEP, DI_MAGIC, DI_KEEP,
      HI_KEEP, HI_KEEP, HI_KEEP, HI_KEEP,
      0, 0, 0, 0 },
  { "Metals",
      HXPOS_UPLEFT,
      DI_KEEP, DI_KEEP, DI_METAL, DI_KEEP,
      HI_KEEP, HI_KEEP, HI_KEEP, HI_KEEP,
      0, 0, 0, 0 },
  { "Values",
      HXPOS_UPLEFT,
      DI_KEEP, DI_KEEP, DI_VALUES, DI_KEEP,
      HI_KEEP, HI_KEEP, HI_KEEP, HI_KEEP,
      0, 0, 0, 0 },
  { "Food",
      HXPOS_UPLEFT,
      DI_KEEP, DI_KEEP, DI_FOOD, DI_KEEP,
      HI_KEEP, HI_KEEP, HI_KEEP, HI_KEEP,
      0, 0, 0, 0 },
  { "Wood",
      HXPOS_UPLEFT,
      DI_KEEP, DI_KEEP, DI_WOOD, DI_KEEP,
      HI_KEEP, HI_KEEP, HI_KEEP, HI_KEEP,
      0, 0, 0, 0 },
  { "Defense",
      HXPOS_UPLEFT,
      DI_KEEP, DI_KEEP, DI_DEFENSE, DI_KEEP,
      HI_KEEP, HI_KEEP, HI_KEEP, HI_KEEP,
      0, 0, 0, 0 },
  { "Army Move",
      HXPOS_LOWRIGHT,
      DI_KEEP, DI_AMOVE, DI_KEEP, DI_KEEP,
      HI_KEEP, HI_KEEP, HI_KEEP, HI_KEEP,
      0, 0, 0, 0 },
  { "Navy Move",
      HXPOS_LOWRIGHT,
      DI_KEEP, DI_NMOVE, DI_KEEP, DI_KEEP,
      HI_KEEP, HI_KEEP, HI_KEEP, HI_KEEP,
      0, 0, 0, 0 },
  { "Flight Move",
      HXPOS_LOWRIGHT,
      DI_KEEP, DI_FMOVE, DI_KEEP, DI_KEEP,
      HI_KEEP, HI_KEEP, HI_KEEP, HI_KEEP,
      0, 0, 0, 0 },
  { "Weights",
      HXPOS_LOWRIGHT,
      DI_KEEP, DI_WEIGHTS, DI_KEEP, DI_KEEP,
      HI_KEEP, HI_KEEP, HI_KEEP, HI_KEEP,
      0, 0, 0, 0 },
  { "Blank",
      HXPOS_LOWLEFT,
      DI_BLANK, DI_BLANK, DI_BLANK, DI_BLANK,
      HI_KEEP, HI_KEEP, HI_KEEP, HI_KEEP,
      0, 0, 0, 0 }
};

/* the screen definitions */
SCREEN_STRUCT hexmap_screen[ZOOM_NUMBER] = {
  { 3, 2, 4, 2, 1, TRUE },
  { 2, 2, 1, 1, 1, FALSE },
  { 1, 2, 1, 1, 1, FALSE }
};
SCREEN_STRUCT rectmap_screen[ZOOM_NUMBER] = {
  { 3, 2, 4, 2, 0, TRUE },
  { 2, 1, 1, 1, 0, FALSE },
  { 1, 1, 1, 1, 0, FALSE }
};

/* view change possibilities */
char *cv_name[] = {
  "Contour", "Designation", "Vegetation"
};
int cv_max[] = { ELE_NUMBER, MAJ_NUMBER, VEG_NUMBER };

/* shortened descriptions for sidebar information */
char *shortspeed[] = { "<", "=", ">", "-", "+" };
char *shortdir[] = { "Here", "N", "NE", "E", "SE", "S", "SW", "W", "NW" };
char *shipsize[] = { "Light", "Medium", "Heavy" };

/* major designation entries */
char *desg_selects[] = {
  "XNone", "FFarm", "GFertile", "HFruitful", "mMetal mine",
  "$Jewel mine", ":Lumberyard", "@Shrine", "[Bridge", "=Canal",
  "|Wall", "&Cache", "sStockade", "tTown", "cCity", "CCapital"
};

/* minor designation entries */
char *min_desg_selects[] = {
  "Xdevastate", "Y forsale", "Z Sieged", "Trading Post", "Roads",
  "Blacksmith", "School", "Church", "Mill", "Granary",
  "Fortifications", "Harbor"
};

/* the default email edit mode... */
int email_mode = EM_M_NORMAL;

/* the default zoom setting */
SCREEN_PTR curmap_screen = NULL;

/* general declarations */
long session_random;
char fison[FILELTH], conqmail[FILELTH];
char *sectstat = NULL, *trooplocs = NULL;
int redraw, xloc_mark, yloc_mark, lockfilenum, mail_ind = MAIL_DONE;
int pager, selector;
int conq_news_status, conq_mail_status, god_browsing = FALSE;
Cinfotype city_mode = CINFO_ECONOMY;
itemtype command_cost;
#ifdef SYSMAIL
int sys_mail_status;
char sysmail[FILELTH];
#endif /*SYSMAIL*/
