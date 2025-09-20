// SPDX-License-Identifier: GPL-3.0-or-later
/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/* This include file defines data structures from old versions of conquer */

/* Patch 28: "late login" flag; "growthrate"; expanded city structure */

/* World Data Structure */
struct s27_world
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
};

/* City Structure */
typedef struct s27_city {
  maptype xloc, yloc;	/* location of the city		*/
  char name[NAMELTH+1];	/* name	of the city		*/
  short i_people;	/* initial civilians in sector	*/
  uns_char efficiency;	/* the efficiency of the region	*/
  uns_char weight;	/* distribution value of a city	*/
  itemtype c_mtrls[MTRLS_NUMBER];	/* materials	*/
  itemtype i_mtrls[MTRLS_NUMBER];	/* dist mtrls	*/
  itemtype s_talons;	/* starting talons in city	*/
  uns_char fortress;	/* fortification value of city	*/
  struct s27_city *next;	/* next city storage	*/
} C27_STRUCT, *C27_PTR;

/* Patch 27: additonal of maxdipadj */

/* World Data Structure for Patchlevel 26 */
struct s26_world
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
};

/* === Patch 26 Upgrade; adjusted navy/caravan statuses, added army unit
                         types, added mapping information structure. */

/* old naval and caravan statuses */
#define OST_CARRY       0
#define OST_DECOY       1
#define OST_SUPPLY      2
#define OST_ENGAGE      3
#define OST_REPAIR      4
#define OST_SIEGED      5
#define OST_FORSALE     6
#define OST_ONBOARD     7
#define OST_ONBSPLY     8
#define OST_WORKCREW    9

/* old nation structure without new mapping information */
typedef struct s25_nation {
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
  ARMY_PTR army_list;		/* the army units of the nation	*/
  NAVY_PTR navy_list;		/* the navy units of the nation	*/
  CITY_PTR city_list;		/* the city information		*/
  ITEM_PTR item_list;		/* national commodities		*/
  CVN_PTR cvn_list;		/* trading caravans		*/
  UNUM_PTR unum_list;		/* autonumbering schemes	*/
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
} N25_STRUCT, *N25_PTR;

/* world structure for the information */
struct s25_world
{
  maptype mapx, mapy;	/* size of world		*/
  ntntype nations;	/* total number of nations	*/
  ntntype npcs;		/* number of beginning npcs	*/
  ntntype active_ntns;	/* number of active nations	*/
  N25_PTR np[ABSMAXNTN];	/* nations of the world */
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
};

/* === Patch 24 Upgrade; adjusted national attributes and autonumbering */

/* old attribute definitions */
#define OBUTE_CHARITY	0
#define OBUTE_COMMRANGE	1
#define OBUTE_EATRATE	2
#define OBUTE_HEALTH	4
#define OBUTE_INFLATION	5
#define OBUTE_JEWELWORK	6
#define OBUTE_KNOWLEDGE	7
#define OBUTE_METALWORK	8
#define OBUTE_MINING	9
#define OBUTE_POPULARITY	10
#define OBUTE_SPELLPTS	15
#define OBUTE_SPOILRATE	16
#define OBUTE_TAXRATE	17
#define OBUTE_TERROR	18
#define OBUTE_WEALTH	19
#define OBUTE_NUMBER	21

/* Nation Structure */
typedef struct s24_nation {
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
  ARMY_PTR army_list;		/* the army units of the nation	*/
  NAVY_PTR navy_list;		/* the navy units of the nation	*/
  CITY_PTR city_list;		/* the city information		*/
  ITEM_PTR item_list;		/* national commodities		*/
  CVN_PTR cvn_list;		/* trading caravans		*/
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
  short attribute[OBUTE_NUMBER];	/* national attributes	*/
  off_t mbox_size;		/* last known size of conq mail	*/
  off_t news_size;		/* last known size of conq news	*/
} N24_STRUCT, *N24_PTR;

/* World Data Structure */
struct s24_world
{
  maptype mapx, mapy;	/* size of world		*/
  ntntype nations;	/* total number of nations	*/
  ntntype npcs;		/* number of beginning npcs	*/
  ntntype active_ntns;	/* number of active nations	*/
  N24_PTR np[ABSMAXNTN];	/* nations of the world */
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
};
