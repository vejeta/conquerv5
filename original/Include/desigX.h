/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/* This include file handles the sector designation definitions */

/* Minor Designations */
#define MIN_START	0x0010
#define MIN_DEVASTATED	0x0010
#define MIN_FORSALE	0x0020
#define MIN_SIEGED	0x0040
#define MIN_TRADINGPOST	0x0080
#define MIN_ROADS	0x0100
#define MIN_BLACKSMITH	0x0200
#define MIN_UNIVERSITY	0x0400
#define MIN_CHURCH	0x0800
#define MIN_MILL	0x1000
#define MIN_GRANARY	0x2000
#define MIN_FORTIFIED	0x4000
#define MIN_HARBOR	0x8000
/* MIN_NUMBER defined in dataX.h */

/* Major Designations */
#define MAJ_NONE	0
#define MAJ_FARM	1
#define MAJ_FERTILE	2
#define MAJ_FRUITFUL	3
#define MAJ_METALMINE	4
#define MAJ_JEWELMINE	5
#define MAJ_LUMBERYARD	6
#define MAJ_SHRINE	7
#define MAJ_BRIDGE	8
#define MAJ_CANAL	9
#define MAJ_WALL	10
#define MAJ_CACHE	11
#define MAJ_STOCKADE	12
#define MAJ_TOWN	13
#define MAJ_CITY	14
#define MAJ_CAPITAL	15
/* MAJ_NUMBER defined in dataX.h */

/* The various information about the designations constructions */
#define DINFO_BUILDA	1	/* small scale building */
#define DINFO_BUILDB	2	/* medium level building */
#define DINFO_BUILDC	3	/* good deal of work to build */
#define DINFO_BUILDD	4	/* most difficult to build */

/* Information structure for designations */
typedef struct s_desg {
  char *name;		/* The name of the designation */
  char symbol;		/* The display character of the designation */
  uns_char build_time;	/* Turns to construct this item, if applicable */
  uns_char draft_pct;	/* % of people able to join up, if applicable */
  uns_char tax_value;	/* The value of the given designation */
  uns_char multiplier;	/* Adjustment due to the designation */
  uns_char attract[RACE_NUMBER];	/* the attractiveness of the sector */
  uns_char production[SEASON_NUMBER];	/* seasonal production values */
  uns_short info_val;	/* Added information; maj=what minors aren't good */
  itemtype mtrls_cost[MTRLS_NUMBER];	/* Cost in materials to build */
  itemtype mtrls_spt[MTRLS_NUMBER];	/* Cost in materials to support */
} DESG_STRUCT, *DESG_PTR;

/* Designation Determination Macros */
#define major_desg(x)	((x) & 0x0F)
#define minor_desg(x,y)	((x) & (y) & 0xFFF0)
#define built_desg(x)	((x) & 0x0F)
#define majd_build(x)	build_desg(maj_dinfo[x].info)
#define mind_build(x)	build_desg(min_dinfo[x].info)
#define IS_FARM(x)	(major_desg(x) >= MAJ_FARM && major_desg(x) <= MAJ_FERTILE)
#define IS_CITY(x)	(major_desg(x) > MAJ_WALL)
#define IS_HARBOR(x)	minor_desg(x, MIN_HARBOR)

/* Designation Assignment Macros */
#define set_majordesg(x,y)	x = ((x & 0xFFF0) | (y))
#define set_minordesg(x,y)	x |= y
#define clr_minordesg(x,y)	x &= ~y

/* the two designation information structures */
extern DESG_STRUCT maj_dinfo[MAJ_NUMBER];
extern DESG_STRUCT min_dinfo[MIN_NUMBER];
