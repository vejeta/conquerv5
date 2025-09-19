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

/* ::: THIS FILE SHOULD NOT NEED TO BE ALTERED ::: */
#include "header.h"
#include "sysconf.h"
#include "paramX.h"

/* check for inclusion */
#ifndef BIGINT

/* ------------------------------------------------------------------ */
/*								      */
/*     This file is to be included by all files composing either      */
/*   the Administrative Interface (conqrun) or the User Interface     */
/*   (conquer).							      */
/*     Only those definitions used in both programs should be set     */
/*   here.							      */
/*								      */
/* ------------------------------------------------------------------ */

/* Shell Exit Statuses */
#define FAIL	1
#define SUCCESS	0

/* Standard Logical Definitions */
#ifndef TRUE
#define TRUE	1
#define FALSE	0
#endif /*TRUE*/

/* Generic Constants */
#define SALT		"az"	/* Simple salt for crypt */
#define BIGINT	500000000L	/* For overflow detection */
#define BIGDOUBLE	3.0E99	/* For overflow detection */
#define BREAKJIHAD	200000L /* $ cost to break confederacy or jihad */
#define PASSLTH		8	/* Num characters in the password */
#define NAMELTH		10	/* Num characters in nation name */
#define LEADERLTH	10	/* Num characters in leader name */
#define DISPLAYLTH	40	/* Num characters for display name */
#define LINELTH		100	/* Num characters in generic string */
#define FILELTH		255	/* Num characters in maximum file name */
#define BIGLTH		500	/* Num characters in large string */
#define OPT_LTH		40	/* maximum characters for option names */
#define MOTDLINES	4	/* Maximum number of lines in MOTD */
#define FULLPCT		100	/* Maximum setting of national stats */

/* Environment Variable and Customization Information */
#define ENVIRON_OPTS	"CONQ_OPTS"
#define ENVIRON_DFLT	"CONQ_DEFAULTDIR"
#define ENVIRON_HELP	"CONQ_HELPDIR"
#define ENVIRON_EXEC	"CONQ_EXEDIR"
#define CONQRC_FILE	".conqrc"
#define CQRC_TAG	".conqrc"

/* Constant for Land Ownership and God Indication */
#define UNOWNED		0

/* Constant for News Mailing */
#define NEWSPAPER	-1

/* Various max constants */
#define MTRLS_NUMBER	5
#define MIN_NUMBER	12
#define MAJ_NUMBER	16
#define ELE_NUMBER	6
#define VEG_NUMBER	12
#define RACE_NUMBER	10
#define MAG_NUMBER	3
#define BUTE_NUMBER	19
#define AU_NUMBER	11
#define NSHP_NUMBER	4
#define ZOOM_NUMBER	3
#define HXPOS_NUMBER	4
#define SEASON_NUMBER	4	/* how many seasons are there, anyway? */

/* integer function pointer */
typedef int (*FNCI)();

/* void function pointer */
#ifdef __STDC__
typedef void (*FNCV2)(int x, int y);
#else /* FNCV */
typedef void (*FNCV2)();
#endif /* FNCV */

/* Spread Sheet Data Structure */
typedef struct s_sheet {
  long people;			/* total population		*/
  itemtype mtrls[MTRLS_NUMBER];	/* the raw materials in region	*/
  short sectors;		/* sectors covered by structure	*/
  long type_people[MAJ_NUMBER];		/* people by major_desg	*/
  itemtype type_talons[MAJ_NUMBER];	/* money by major_desg	*/
  int type_sectors[MAJ_NUMBER];	/* sectors by major designation	*/
  long army_men;		/* number of army men in region */
  int monst_troops;		/* number of monster troops	*/
  int ship_holds;		/* ship holds in region		*/
  int caravan_wagons;		/* caravan wagons in region	*/
  itemtype army_cost;		/* support cost for troops	*/
  itemtype monst_jewels;	/* tribute to monster units	*/
  itemtype navy_cost;		/* support cost for fleets	*/
  itemtype cvn_cost;		/* support cost for wagons	*/
} SHEET_STRUCT, *SHEET_PTR;

/* Sector Data Structure */
typedef struct s_sector {
  uns_short designation;	/* design of a sector	*/
  uns_char altitude;	/* sector altitude		*/
  uns_char vegetation;	/* sector vegetation		*/
  ntntype owner;	/* nation id of owner		*/
  uns_char efficiency;	/* the efficiency of the sector	*/
  long people;		/* civilians in sector		*/
  uns_char minerals;	/* jewel / metal value		*/
  uns_char tradegood;	/* exotic trade goods in sector	*/
  /* uns_char region;	 index of region		*/
} SCT_STRUCT, *SCT_PTR;

/* Army Data Structure */
typedef struct s_army {
  idtype armyid;	/* unique identifier for unit	*/
  uns_char unittype;	/* type of the army unit	*/
  uns_char max_eff;	/* the efficiency of the unit	*/
  uns_char efficiency;	/* the efficiency of the unit	*/
  maptype xloc, yloc;	/* location of the army unit	*/
  maptype targx, targy;	/* target sector for the unit	*/
  maptype lastx, lasty;	/* prior location of the unit	*/
  long strength;	/* number of soldiers		*/
  uns_char umove;	/* movement ability of the unit	*/
  long status;		/* the status of the unit	*/
  uns_char spellpts;	/* magic strength of leaders	*/
  uns_char supply;	/* food supply per man		*/
  idtype leader;	/* unit the army is lead by	*/
  struct s_army *next;	/* to next army			*/
  struct s_army *nrby;	/* to armies in same sector	*/
} ARMY_STRUCT, *ARMY_PTR;

/* Navy Data Structure */
typedef struct s_navy {
  idtype navyid;	/* unique identifier		*/
  uns_short ships[NSHP_NUMBER];	/* warships, merchants galleys */
  uns_char efficiency[NSHP_NUMBER];	/* the efficiency of the fleet	*/
  maptype xloc, yloc;	/* location of the ships	*/
  maptype targx, targy;	/* target sector for the fleet	*/
  maptype lastx, lasty;	/* prior location of the ships	*/
  uns_char umove;	/* movement ability of ships	*/
  long status;		/* the status of the unit	*/
  uns_char crew;	/* percent of crew on the ships	*/
  uns_char people;	/* people carried per ghold	*/
  uns_char supply;	/* supplies per crew member	*/
  idtype armynum;	/* army unit carried		*/
  idtype cvnnum;	/* caravan unit carried		*/
  itemtype mtrls[MTRLS_NUMBER];	/* materials carried	*/
  struct s_navy *next;	/* next navy unit		*/
} NAVY_STRUCT, *NAVY_PTR;

/* Caravan Data Structure */
typedef struct s_caravan {
  idtype cvnid;		/* unique identifier		*/
  uns_char efficiency;	/* the efficiency of the cvan	*/
  maptype xloc, yloc;	/* location of the caravan	*/
  maptype targx, targy;	/* target sector for the cvan	*/
  maptype lastx, lasty;	/* last location of the caravan	*/
  uns_char size;	/* how many wagons in caravan	*/
  uns_char umove;	/* movement ability of caravan	*/
  long status;		/* the status of the unit	*/
  uns_char crew;	/* percent of caravan crew	*/
  uns_char people;	/* people carried per wagon	*/
  uns_char supply;	/* supplies per crew member	*/
  itemtype mtrls[MTRLS_NUMBER];	/* materials carried	*/
  struct s_caravan *next;	/* next caravan unit	*/
} CVN_STRUCT, *CVN_PTR;

/* City Structure */
typedef struct s_city {
  maptype xloc, yloc;	/* location of the city		*/
  char name[NAMELTH+1];	/* name	of the city		*/
  uns_short cityid;	/* internal identification	*/
  short i_people;	/* initial civilians in sector	*/
  uns_char efficiency;	/* the efficiency of the region	*/
  uns_char weight;	/* distribution value of a city	*/
  itemtype c_mtrls[MTRLS_NUMBER];	/* materials	*/
  itemtype i_mtrls[MTRLS_NUMBER];	/* dist mtrls	*/
  itemtype s_talons;	/* starting talons in city	*/
  itemtype m_mtrls[MTRLS_NUMBER];	/* min keep	*/
  long auto_flags[MTRLS_NUMBER];	/* dist info	*/
  long cmd_flag;	/* navy/cvn command information	*/
  uns_char fortress;	/* fortification value of city	*/
  struct s_city *next;	/* next city storage structure	*/
} CITY_STRUCT, *CITY_PTR;

/* Item Structure */
typedef struct s_item {
  maptype xloc, yloc;	/* location of the item		*/
  idtype itemid;	/* unique identifier of item	*/
  unsigned char type;	/* the task of this item	*/
  long iteminfo;	/* information about item use	*/
  long menforjob;	/* the number of men needed	*/
  idtype armyid;	/* army id (sale or construct)	*/
  idtype navyid;	/* navy id (sale or construct)	*/
  idtype cvnid;		/* caravan id (for sale)	*/
  itemtype mtrls[MTRLS_NUMBER];	/* the raw materials	*/
  struct s_item *next;	/* pointer to next item struct	*/
} ITEM_STRUCT, *ITEM_PTR;

/* Automatic Unit Numbering */
typedef struct s_unitnum {
  int number;			/* number to start from */
  int ident;			/* identifier of the class/unit/etc. */
  int type;			/* type of the numeric */
  struct s_unitnum *next;	/* to next element in chain */
} UNITNUM, *UNUM_PTR;

/* Mapping Structure */
typedef struct s_map {
  maptype xloc, yloc;		/* location of the mapping element	*/
  uns_char range;		/* distance visible from this locale	*/
  uns_char strength;		/* the strength of the visibility	*/
  uns_char lifetime;		/* duration of visibility -- 0 = perm	*/
  struct s_map *next;
} MAP_STRUCT, *MAP_PTR;

/* Nation Data Structure */
typedef struct s_nation {
  char name[NAMELTH+1];		/* name				*/
  char login[NAMELTH+1];	/* login name of user		*/
  char passwd[PASSLTH+1];	/* password			*/
  char leader[LEADERLTH+1];	/* leader title			*/
  short repro;			/* reproduction rate of nation	*/
  short race;			/* national race		*/
  char mark;			/* unique mark for nation	*/
  char location;		/* how well placed on creation	*/
  maptype capx;			/* Capital x coordinate		*/
  maptype capy;			/* Capital y coordinate		*/
  maptype centerx;		/* Relative central X location	*/
  maptype centery;		/* Relative central Y location	*/
  short leftedge;		/* Left Edge of Nation		*/
  short rightedge;		/* Right Edge of Nation		*/
  short topedge;		/* Top Edge of Nation		*/
  short bottomedge;		/* Bottom Edge of Nation	*/
  short class;			/* national class		*/
  short aplus;			/* attack plus of all soldiers	*/
  short dplus;			/* defense plus of all soldiers	*/
  long score;			/* score			*/
  short active;			/* nation type and strategy	*/
  short maxmove;		/* maximum movement of soldiers */
  idtype num_army;		/* for saves; number of armies	*/
  idtype num_navy;		/* for saves; number of navies	*/
  idtype num_city;		/* for saves; # of city structs	*/
  idtype num_item;		/* for saves; # of item structs	*/
  idtype num_cvn;		/* for saves; # of caravans	*/
  idtype num_unum;		/* for saves; # of autonum itms	*/
  idtype num_maps;		/* for saves; # of map structs	*/
  ARMY_PTR army_list;		/* the army units of the nation	*/
  NAVY_PTR navy_list;		/* the navy units of the nation	*/
  CITY_PTR city_list;		/* the city information		*/
  ITEM_PTR item_list;		/* national commodities		*/
  CVN_PTR cvn_list;		/* trading caravans		*/
  UNUM_PTR unum_list;		/* autonumbering schemes	*/
  MAP_PTR map_list;		/* mapping of sectors		*/
  char dstatus[ABSMAXNTN];	/* diplomatic status		*/
  itemtype mtrls[MTRLS_NUMBER];	/* sum of materials in nation	*/
  itemtype m_new[MTRLS_NUMBER];	/* sum of produced materials	*/
  short tsctrs;			/* total number sectors		*/
  short tunsctrs;		/* total number unsupported	*/
  short tships;			/* number of ships		*/
  short twagons;		/* number of wagons		*/
  long tmonst;			/* total number of monsters	*/
  long tleaders;		/* total number of leaders	*/
  long tmil;			/* total military		*/
  long tciv;			/* total civilians		*/
  long powers[MAG_NUMBER];	/* lists of national powers	*/
  short attribute[BUTE_NUMBER];	/* national attributes		*/
  off_t mbox_size;		/* last known size of conq mail	*/
  off_t news_size;		/* last known size of conq news	*/
} NTN_STRUCT, *NTN_PTR;

/* World Data Structure */
struct s_world
{
  maptype mapx, mapy;	/* size of world		*/
  ntntype nations;	/* total number of nations	*/
  ntntype npcs;		/* number of beginning npcs	*/
  ntntype active_ntns;	/* number of active nations	*/
  NTN_PTR np[ABSMAXNTN];	/* nations of the world */
  short turn;		/* count of game turn		*/
  short start_turn;	/* turn on which campaign began	*/
  char demigod[NAMELTH+1];	/* name of the demi-god	*/
  char passwd[PASSLTH+1];	/* god's password	*/
  char addpwd[PASSLTH+1];	/* add nation password	*/
  char demibuild;	/* flag allows demigod rebuild	*/
  char demilimit;	/* flag limits demigod powers	*/
  char relative_map;	/* flag for messing up players	*/
  char hexmap;		/* flag to indicate hexmap mode	*/
  char verify_login;	/* flag to increase security	*/
  char hide_scores;	/* flag to hide nation scores	*/
  char hide_login;	/* flag to hide nation owners	*/
  char builddist;	/* min dist between nations	*/
  short fleet_cbval;	/* base combat bonus of navies	*/
  short wagon_cbval;	/* base combat bonus of cvns	*/
  char num_dice;	/* number of combat roll dice	*/
  char avg_damage;	/* average damage during combat	*/
  char damage_limit;	/* damage needed to stop attack	*/
  char overmatch_adj;	/* adjust to dam for rel_size	*/
  char pmindamage;	/* min damage level (% of roll)	*/
  char smoothings;	/* smoothing algorithm repeats	*/
  char land_range;	/* displacement check for water	*/
  char preject_land;	/* % chance rejection for land	*/
  char pwater;		/* percentage of water		*/
  char pmount;		/* % of land that is mountains	*/
  char pmercval;	/* % of disbanded to mercs	*/
  char prevolt;		/* % yearly revolts level	*/
  char pscout;		/* % chance to capture scouts	*/
  char pmonster;	/* % per year for monster repro	*/
  char pmercmonst;	/* % of monsters who goto mercs	*/
  char ptrade;		/* % of sectors with tradegoods	*/
  char ptgmetal;	/* metal tradegood percentage	*/
  char ptgjewel;	/* jewel tradegood percentage	*/
  char ptgspell;	/* magic tradegood percentage	*/
  char supplylimit;	/* limit on supply carrying	*/
  char nvsplydist;	/* distance of supply for n & v	*/
  char cityxfdist;	/* max dist for city transfers	*/
  uns_char exposelvl;	/* the exposure level: 10=norm	*/
  uns_char maxpts;	/* maximum number of build pts	*/
  uns_char lizards;	/* how many lizards are created	*/
  uns_char savages;	/* how many savages are created	*/
  uns_char nomads;	/* how many nomads are created	*/
  uns_char pirates;	/* how many pirates are created	*/
  char numrandom;	/* number of random events/turn	*/
  char numweather;	/* number of weather/turn	*/
  long m_mil;		/* number of mercs available	*/
  short m_aplus;	/* mercenary attack bonus	*/
  short m_dplus;	/* mercenary defense bonus	*/
  itemtype bribelevel;	/* value level of bribes	*/
  itemtype w_mtrls[MTRLS_NUMBER];	/* all resource	*/
  long w_civ;		/* world population		*/
  long w_mil;		/* world military		*/
  long w_sctrs;		/* owned sectors in world	*/
  long w_score;		/* world score total		*/
  off_t mbox_size;	/* size of god's mbox when read	*/
  off_t news_size;	/* gods knowledge of news size	*/
  int num_unum;		/* how many default unit nums	*/
  UNUM_PTR dflt_unum;	/* default world unit numbering	*/
  uns_char maxdipadj;	/* maximum diplomacy adjustment	*/
  uns_char latestart;	/* turns before password check	*/
  uns_char growthrate;	/* speed of world reproduction	*/
};

/* display information structures */
typedef struct s_display {
  char name[DISPLAYLTH + 1];	/* the name of the display option */
  int focus;			/* which quadrant is the focus quadrant */
  int style[HXPOS_NUMBER];	/* display selection in each quadrant */
  int highlight[HXPOS_NUMBER];	/* highlight option in each quadrant */
  int target[HXPOS_NUMBER];	/* numeric target of highlights */
} DISPLAY_STRUCT, *DISPLAY_PTR;

/* the mode structure */
typedef struct s_dmode {
  DISPLAY_STRUCT d;		/* the actual display information */
  struct s_dmode *next;		/* the next display for linked lists */
} DMODE_STRUCT, *DMODE_PTR;

/* Everything below this point is not used by dataX.c */
#ifndef DATA_DECLARE

/* Name and nickname of update mail messages */
#define CQ_MAIL_NAME	"Conquer"
#define CQ_MAIL_NICK	"The Grand Overseer"

/* Various Generic Macros */
#ifndef min
#define min(a,b)	((b) < (a) ? (b) : (a))
#endif /* min */
#ifndef max
#define max(a,b)	((b) > (a) ? (b) : (a))
#endif /* max */
#ifndef abs
#define abs(a)		((a) < 0 ? -(a) : (a))
#endif /* abs */

#define	abrt() { \
if (in_curses) cq_reset(); \
fprintf(stderr,"\nSerious Error (File %s, Line %d) - Aborting\n",__FILE__,__LINE__); \
if ((fupdate != stderr) && (fupdate != NULL)) fclose(fupdate); \
if (need_hangup) hangup(); \
abort(); \
}

#define beep()	if (conq_beeper) putc('\007', stderr)

/* macros to determine proper sector */
#define XY_ONMAP(x,y)	(((x) >= 0) && ((y) >= 0) && ((x) < MAPX) && ((y) < MAPY))
#define XY_INMAP(x,y)	(((x) < MAPX) && ((y) < MAPY))
#define X_ONMAP(x)	(((x) >= 0) && ((x) < MAPX))
#define Y_ONMAP(y)	(((y) >= 0) && ((y) < MAPY))

/* World Definitions */
#define MAPX		((int) world.mapx + 1)	/* map size */
#define MAPY		((int) world.mapy + 1)
#define TURN		world.turn		/* game turn storage loc */
#define START_TURN	world.start_turn	/* relative starting point */
#define MAXNTN		world.nations

/* edge shortcuts */
#define LEFTEDGE	ntn_ptr->leftedge
#define RIGHTEDGE	ntn_ptr->rightedge
#define TOPEDGE		ntn_ptr->topedge
#define BOTTOMEDGE	ntn_ptr->bottomedge

/* other nation information definitions */
#define NTN_D_EATRATE	(((double) ntn_ptr->attribute[BUTE_EATRATE]) / 10.0)
#define NTN_I_EATRATE	(ntn_ptr->attribute[BUTE_EATRATE] / 10)
#define COMM_D_RANGE	(((double) ntn_ptr->attribute[BUTE_COMMRANGE]) / 10.0)
#define COMM_I_RANGE	(ntn_ptr->attribute[BUTE_COMMRANGE] / 10)

/* Army Unit Numbering Schemes */
#define EMPTY_HOLD	0

/* Nation weighting distributions */
#define SUM_WEIGHTS(x,y)	sum_weights[(x) + MAPX * (y)]

/* Sector location definitions */
#define	XREAL		(((xcurs + xoffset) + MAPX) % MAPX)
#define	YREAL		(ycurs + yoffset)

/* File: dataX.c -- global data for both programs */
extern FILE *fnews, *fexe, *fm, *fupdate;
extern int country, global_int, no_input, owneruid, adjust_made;
extern int is_god, is_update, in_curses, need_hangup, dosysm_check;
extern int xcurs, ycurs, xoffset, yoffset, movemode;
extern int adjust_xloc, adjust_yloc;
extern long global_long;
extern SCT_PTR sct_ptr, sct_tptr;
extern NTN_PTR ntn_ptr, ntn_tptr;
extern SCT_STRUCT **sct;
extern struct s_world world;
extern char datadir[], loginname[], defaultdir[], datadirname[];
extern char nationname[], string[], prog_name[], helpdir[], progdir[];

/* declaration of some global strings */
extern char rnumerals[];
extern char *aggressname[], *speedname[];
extern char *alignment[], *opt_list[];

/* miscellaneous file name constants */
extern char *help_files[], *zooms[ZOOM_NUMBER];
extern char *hex_list[HXPOS_NUMBER];
extern char *exetag, *datafile, *motdfile, *msgtag, *timefile;
extern char *newsfile, *tmptag, *isontag, *rulesfile, *npcfile, *blockfile;

/* sizings */
extern int help_number, options_number, tgoods_number, tgclass_number;
extern int nclass_number;

/* the mail formats */
extern char rmail_quote_prefix[LINELTH], rmail_from_format[LINELTH];

/* global options */
extern int conq_allblanks, conq_bottomlines, conq_gaudy, conq_beeper;
extern int conq_expert, zoom_level, conq_waterbottoms, conq_supply_level;
extern int conq_infomode, conq_mercsused, conq_mheaders;

/* pager settings */
int pager_tab, pager_scroll, pager_offset;

/* other miscellaneous declarations */
extern char *hasseen;
extern short *sum_weights;
extern DMODE_PTR dmode_list, dmode_tptr;

/* all global functions */
#ifndef __CEXTRACT__
#include "fileX.h"
#endif /* __CEXTRACT__ */

#endif /*DATA_DECLARE*/
#endif /*BIGINT*/
