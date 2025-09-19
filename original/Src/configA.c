/* This file contains the routines necessary for world creation */
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
#include "buildA.h"
#include "calenX.h"
#include "worldX.h"
#include "activeX.h"
#include "stringX.h"
#include "keyvalsX.h"

/* the prompt for each entry under world creation */
char *bld_name[BLD_OPTIONS] = {
  "Demi-god",
  "God Passwd",
  "Add Passwd",
  "Limit D-god",
  "D-god Builds",
  "Map Type",
  "Relative Map",
  "Verify Login",
  "Hide Scores",
  "Hide Users",
  "Late Start",
  "World X Size",
  "World Y Size",
  "Max Build Pts",
  "Min Ntn Dist",
  "Navy Bonus",
  "Cvan Bonus",
  "Combat Dice",
  "Avg Damage",
  "Deter Damage",
  "Overmatch",
  "Min Damage",
  "Year",
  "Month",
  "Pct Water",
  "Pct Contour",
  "Expose Ratio",
  "Smoothings",
  "Check Range",
  "Pct Reject",
  "Pct Tgoods",
  " % Metals",
  " % Jewels",
  " % Magical",
  "Growth Rate",
  "Npc Ntns",
  "Monst Repro",
  "Lizards",
  "Savages",
  "Nomads",
  "Pirates",
  "Revolt Pct",
  "Scout Battles",
  "Rand Events",
  "Disasters",
  "Mercenaries",
  " Atck Bonus",
  " Dfns Bonus",
  "% Disb Mercs",
  "% Mnst Mercs",
  "Max Diplo Adj",
  "Bribe Value",
  "Max Supply",
  "Unit Sply Dist",
  "City Xfer Dist"
};

/* more detail on each item for world creation */
char *bld_info[BLD_OPTIONS] = {
  "Login name of another person allowed to perform world updates",
  "What is a new password for the world?",
  "What is a new password to add a nation to the campaign?",
#ifdef SECURITY
  "If enabled, demigods will only be able to update the campaign",
#else
  "This option is unimplemented since SECURITY has not been set",
#endif /* SECURITY */
  "This flag indicates a demigod is allowed to rebuild this world",
  "Choose between a hexagonal or a rectangular coordinate system",
  "Displayed locations will be relative to some central position",
  "Prevent people from logging into nations they do not own",
  "Don't show the more revealing scoring information to players",
  "Don't display the login name of nation owners in score output",
  "How many turns before the nation addition password is needed?",
  "Enter the width of the world, in sectors [divisible by 8] ?",
  "Enter the height of the world, in sectors [divisible by 8] ?",
  "How many points should each nation have to build with?",
  "What is the minimum distance between nation created capitals?",
  "Enter the combat bonus for the crew of ships?",
  "Enter the combat bonus for the crew of caravans?",
  "Enter the number of dice to use in combat rolling?",
  "Enter the average damage received during an engagement?",
  "Enter the amount of damage an attacker receives before breaking off?",
  "Enter the % of damage caused per odds setting of troops?",
  "Enter the minimum damage, as a % of the dice roll, troops must receive?",
  "Enter the year for the world?",
  "Enter the month of the year for the world?",
  "Percentage of the surface of the world covered in water?",
  "Percentage of land in the world to be non-flat in contour?",
  "A ratio of unit exposure, higher values indicate harsher world",
  "How many times should the water smoothing routine be run?",
  "Scan for other land within how many 8 sector blocks?",
  "Chance of rejecting block if land is encountered during check",
  "What percentage of land should contain special tradegoods?",
  "What percentage of all special tradegoods should be metals?",
  "What percentage of all special tradegoods should be jewels?",
  "What percentage of all special tradegoods should be magical?",
  "The ratio of growth of world populations (1.0 == Normal)",
  "How many computer nations should be created during genesis?",
  "What is the growth rate of monster nations each year?",
  "How many lizard citadels should be created during genesis?",
  "How many savage armies should be placed during world genesis?",
  "How many nomad armies should be generated during creation?",
  "How many pirate camps should be built during world creation?",
  "Amount of total revolt chance to be taken into consideration?",
  "What chance a scout has of being captured in enemy land?",
  "How many random events should take place during each update?",
  "How many weather events should take place during each update?",
  "How many mercenaries soldiers exist within the world",
  "How good are mercenaries when they are attacking in combat?",
  "How good are mercenaries when they are defending in combat?",
  "What percentage of disbanded troops become mercenaries?",
  "What percentage of monsters become mercenaries each year?",
  "What is the maximum shift in a diplomacy status in one turn?",
  "How much must be paid to adjust an npc status one level?",
  "Maximum amount of food supplies a unit can carry? [0 = disable]",
  "Distance which navies and cvans can supply? [-1 = comm range]",
  "Maximum distance for city transfers? [-1 = comm range]"
};

/* other build routine information */
char *bld_help = " 'hjkl'-select  ' ',DEL-cycle  'c',ESC-change  'R'-reset  'q'-build  'Q'-quit";
char *cfg_help = "  'hjkl'-select item  ' ',DEL-cycle  'c',ESC-change item  'q'-save   'Q'-quit";

/* local indicators */
static int col_width, col_length;
static int cfg_world;

/* ZEROWORLD -- Routine to initialize the entire world */
void
zeroworld PARM_0(void)
{
  int i;

  /* initialize all countries */
  for (i = 0; i < ABSMAXNTN; i++) {

    /* get rid of any existing nations */
    if ((remake == TRUE) && ((ntn_ptr = world.np[i]) != NULL)) {
      dest_ntn(ntn_ptr->name);
    }

    /* now nuke the entry */
    world.np[i] = (NTN_PTR) NULL;
  }
}

/* BLD_DEFAULTS -- The routine sets the default world creation values */
static void
bld_defaults PARM_0(void)
{
  strcpy(world.demigod, LOGIN);
  strcpy(world.addpwd, world.passwd);
  START_TURN = 1;
  TURN = 1;
  world.mapx = DFLT_XSIZE - 1;
  world.mapy = DFLT_YSIZE - 1;
  MAXPTS = DFLT_MAXPTS;
  BUILDDIST = DFLT_BUILDDIST;
  PWATER = DFLT_PWATER;
  PMOUNT = DFLT_PMOUNT;
  PTRADE = DFLT_PTRADE;
  PMONSTER = DFLT_PMONSTER;
  PSCOUT = DFLT_PSCOUT;
  PREVOLT = DFLT_PREVOLT;
  PTRADE = DFLT_PTRADE;
  PTGMETAL = DFLT_PTGMETAL;
  PTGJEWEL = DFLT_PTGJEWEL;
  PTGSPELL = DFLT_PTGSPELL;
  NUMRANDOM = DFLT_NUMRANDOM;
  NUMWEATHER = DFLT_NUMWEATHER;
  world.npcs = DFLT_NPCS;
  world.demibuild = TRUE;
  if ((strcmp(loginname, LOGIN) == 0) ||
      (world.demilimit != TRUE))
    world.demilimit = FALSE;
  world.relative_map = TRUE;
  world.hide_scores = TRUE;
  world.hide_login = FALSE;
  world.hexmap = TRUE;
  world.news_size = (off_t) 0;
  world.mbox_size = (off_t) 0;
  world.verify_login = FALSE;
  world.smoothings = DFLT_SMOOTH;
  world.land_range = DFLT_LRANGE;
  world.preject_land = DFLT_PREJLAND;
  world.lizards = DFLT_LIZARD;
  world.savages = DFLT_SAVAGE;
  world.nomads = DFLT_NOMAD;
  world.pirates = DFLT_PIRATE;
  EXPOSURE = DFLT_EXPOSURE;
  GROWTH = DFLT_GROWTH;
  FLEET_CBVAL = DFLT_FLEETCBVAL;
  WAGON_CBVAL = DFLT_WAGONCBVAL;
  NUMDICE = DFLT_NUMDICE;
  AVG_DAMAGE = DFLT_AVGDAMAGE;
  DAMAGE_LIMIT = DFLT_DAMLIMIT;
  OVERMATCH_ADJ = DFLT_OVERMATCH;
  PMINDAMAGE = DFLT_PMINDAMAGE;
  BRIBELEVEL = DFLT_BRIBE;
  MAXDIPADJ = DFLT_MAXDIPADJ;
  MAXSUPPLIES = DFLT_SUPPLIES;
  NVSPLYDIST = DFLT_NVSPLYDIST;
  CITYXFDIST = DFLT_CITYXFDIST;
  MERCMEN = DFLT_MERCMEN;
  MERCATT = DFLT_MERCATT;
  MERCDEF = DFLT_MERCDEF;
  PMERCVAL = DFLT_PMERCVAL;
  PMERCMONST = DFLT_PMERCMONST;
  world.latestart = DFLT_LATESTART;
}

/* BLD_ITEM -- Show an item of information */
static void
bld_item PARM_3 (int, x, int, y, int, itemnum)
{
  char info_str[40];
  int count, len;

  /* quick check */
  if (itemnum >= BLD_OPTIONS || itemnum < 0) return;
  len = col_width - strlen(bld_name[itemnum]);

  /* tag on the information */
  switch (itemnum) {
  case BLD_DEMIGOD:
    /* show the demi-god entry */
    if (strcmp(LOGIN, world.demigod) == 0) {
      strcpy(info_str, "[none]");
    } else {
      strcpy(info_str, world.demigod);
    }
    break;
  case BLD_PASSWD:
  case BLD_ADDPWD:
    /* show a dummy string */
    info_str[0] = '\0';
    break;
  case BLD_DEMILIMIT:
    /* now show the true and false strings */
#ifdef SECURITY
    if (world.demilimit == TRUE) {
      strcpy(info_str, "True");
    } else if (world.demilimit == FALSE) {
      strcpy(info_str, "False");
    } else {
      strcpy(info_str, "ERROR");
    }
#else
    strcpy(info_str, "UNIMP");
#endif /* SECURITY */
    break;
  case BLD_DEMIBUILD:
    /* now show the true and false strings */
    if (world.demibuild == TRUE) {
      strcpy(info_str, "True");
    } else if (world.demibuild == FALSE) {
      strcpy(info_str, "False");
    } else {
      strcpy(info_str, "ERROR");
    }
    break;
  case BLD_MAPTYPE:
    /* determine the maptype */
    if (world.hexmap) {
      strcpy(info_str, "HexMap");
    } else {
      strcpy(info_str, "RectMap");
    }
    break;
  case BLD_RELATIVE:
    /* now show the true and false strings */
    if (world.relative_map == TRUE) {
      strcpy(info_str, "True");
    } else if (world.relative_map == FALSE) {
      strcpy(info_str, "False");
    } else {
      strcpy(info_str, "ERROR");
    }
    break;
  case BLD_VRFYLOGIN:
    /* now show the true and false strings */
    if (world.verify_login == TRUE) {
      strcpy(info_str, "True");
    } else if (world.verify_login == FALSE) {
      strcpy(info_str, "False");
    } else {
      strcpy(info_str, "ERROR");
    }
    break;
  case BLD_HIDESCORES:
    /* now show the true and false strings */
    if (world.hide_scores == TRUE) {
      strcpy(info_str, "True");
    } else if (world.hide_scores == FALSE) {
      strcpy(info_str, "False");
    } else {
      strcpy(info_str, "ERROR");
    }
    break;
  case BLD_HIDELOGIN:
    /* now show the true and false strings */
    if (world.hide_login == TRUE) {
      strcpy(info_str, "True");
    } else if (world.hide_login == FALSE) {
      strcpy(info_str, "False");
    } else {
      strcpy(info_str, "ERROR");
    }
    break;
  case BLD_XSIZE:
    sprintf(info_str, "%d", MAPX);
    break;
  case BLD_YSIZE:
    sprintf(info_str, "%d", MAPY);
    break;
  case BLD_POINTS:
    sprintf(info_str, "%d", MAXPTS);
    break;
  case BLD_BUILDDIST:
    sprintf(info_str, "%d", BUILDDIST);
    break;
  case BLD_EXPOSURE:
    sprintf(info_str, "%d.%d", EXPOSURE / 10, EXPOSURE % 10);
    break;
  case BLD_GROWTH:
    sprintf(info_str, "%d.%d", GROWTH / 10, GROWTH % 10);
    break;
  case BLD_LATESTART:
    if (world.latestart) {
      sprintf(info_str, "%d", world.latestart);
    } else {
      strcpy(info_str, "Off");
    }
    break;
  case BLD_FLEETCBVAL:
    sprintf(info_str, "%+d", FLEET_CBVAL);
    break;
  case BLD_WAGONCBVAL:
    sprintf(info_str, "%+d", WAGON_CBVAL);
    break;
  case BLD_DICE:
    sprintf(info_str, "%d", NUMDICE);
    break;
  case BLD_AVGDAM:
    sprintf(info_str, "%d%%", AVG_DAMAGE);
    break;
  case BLD_DAMLIM:
    sprintf(info_str, "%d%%", DAMAGE_LIMIT);
    break;
  case BLD_OVERMATCH:
    sprintf(info_str, "%d%%", OVERMATCH_ADJ);
    break;
  case BLD_PMINDAM:
    sprintf(info_str, "%d%%", PMINDAMAGE);
    break;
  case BLD_YEAR:
    sprintf(info_str, "%d", YEAR(TURN));
    break;
  case BLD_MONTH:
    sprintf(info_str, "%s", PMONTH(TURN));
    break;
  case BLD_PWATER:
    sprintf(info_str, "%d%%", PWATER);
    break;
  case BLD_PMOUNT:
    sprintf(info_str, "%d%%", PMOUNT);
    break;
  case BLD_LRANGE:
    sprintf(info_str, "%d", world.land_range);
    break;
  case BLD_PREJLAND:
    sprintf(info_str, "%d%%", world.preject_land);
    break;
  case BLD_PTRADE:
    sprintf(info_str, "%d%%", PTRADE);
    break;
  case BLD_PTGMETAL:
    sprintf(info_str, "%d%%", PTGMETAL);
    break;
  case BLD_PTGJEWEL:
    sprintf(info_str, "%d%%", PTGJEWEL);
    break;
  case BLD_PTGSPELL:
    sprintf(info_str, "%d%%", PTGSPELL);
    break;
  case BLD_NPCS:
    sprintf(info_str, "%d", world.npcs);
    break;
  case BLD_PREVOLT:
    sprintf(info_str, "%d%%", PREVOLT);
    break;
  case BLD_PMONSTER:
    sprintf(info_str, "%d%%", PMONSTER);
    break;
  case BLD_LIZARD:
    sprintf(info_str, "%d", world.lizards);
    break;
  case BLD_SAVAGE:
    sprintf(info_str, "%d", world.savages);
    break;
  case BLD_NOMAD:
    sprintf(info_str, "%d", world.nomads);
    break;
  case BLD_PIRATE:
    sprintf(info_str, "%d", world.pirates);
    break;
  case BLD_PSCOUT:
    sprintf(info_str, "%d%%", PSCOUT);
    break;
  case BLD_NUMRANDOM:
    sprintf(info_str, "%d", NUMRANDOM);
    break;
  case BLD_NUMWEATHER:
    sprintf(info_str, "%d", NUMWEATHER);
    break;
  case BLD_MERCMEN:
    sprintf(info_str, "%ld", MERCMEN);
    break;
  case BLD_MERCATT:
    sprintf(info_str, "%+d", MERCATT);
    break;
  case BLD_MERCDEF:
    sprintf(info_str, "%+d", MERCDEF);
    break;
  case BLD_PMERCVAL:
    sprintf(info_str, "%d%%", PMERCVAL);
    break;
  case BLD_PMERCMONST:
    sprintf(info_str, "%d%%", PMERCMONST);
    break;
  case BLD_MAXDIPADJ:
    sprintf(info_str, "%d", MAXDIPADJ);
    break;
  case BLD_BRIBE:
    sprintf(info_str, "%ld", BRIBELEVEL);
    break;
  case BLD_MAXSUPPLIES:
    if (MAXSUPPLIES == 0) {
      strcpy(info_str, "disabled");
    } else {
      sprintf(info_str, "%d", MAXSUPPLIES);
    }
    break;
  case BLD_NVSPLYDIST:
    if (NVSPLYDIST == NTN_DISTIND) {
      strcpy(info_str, "ntn comm");
    } else {
      sprintf(info_str, "%d", NVSPLYDIST);
    }
    break;
  case BLD_CITYDIST:
    if (CITYXFDIST == NTN_DISTIND) {
      strcpy(info_str, "ntn comm");
    } else {
      sprintf(info_str, "%d", CITYXFDIST);
    }
    break;
  case BLD_SMOOTH:
    sprintf(info_str, "%d", world.smoothings);
    break;
  default:
    strcpy(info_str, "?huh?");
    break;
  }

  /* now display it */
  len -= strlen(info_str);
  mvaddstr(y, x, bld_name[itemnum]);
  for (count = 0; count < len; count++) {
    addch('.');
  }
  addstr(info_str);
  
}

/* BLD_DISPLAY -- The routine shows all of the information for building */
static void
bld_display PARM_1(int, curnum)
{
  int count, xloc = 1, yloc = BLD_START;

  /* go through all entries */
  for (count = 0; count < BLD_OPTIONS; count++) {

    /* show each item; highlight current? */
    if (curnum == count) {
      standout();
    }
    bld_item(xloc, yloc, count);
    if (curnum == count) {
      standend();
    }

    /* now move along */
    yloc++;
    if ((count + 1) % col_length == 0) {
      yloc = BLD_START;
      xloc += col_width + 2;
    }
  }
}

/* IN_AND_CHECK -- Enter and check the data */
static long
in_and_check PARM_3(long, min_val, long, max_val, char *, hstr)
{
  long value;

  /* get the input */
  value = get_number((min_val < 0)?TRUE:FALSE);
  if (no_input == TRUE) return(min_val - 1);

  /* check the input */
  if ((value < min_val) ||
      (value > max_val)) {
    errormsg(hstr);
    return(min_val - 1);
  }
  return(value);
}

/* casting macro for safety */
#define In_and_Check(x, y, z)	in_and_check((long) (x), (long) (y), z)

/* BLD_CHANGE -- Routine to adjust values for creation customization */
static void
bld_change PARM_1 (int, itemnum)
{
  char passwd[PASSLTH + 1];
  int xloc, yloc, len;
  double tmpdbl;
  short old_turn;
  long value;

  /* check boundaries */
  if ((itemnum < 0) || (itemnum >= BLD_OPTIONS)) {
    errormsg("Hmm... what item are you looking at?");
    return;
  }

  /* find location */
  if ((itemnum == BLD_PASSWD) ||
      (itemnum == BLD_ADDPWD)) {
    len = col_width;
  } else {
    len = strlen(bld_name[itemnum]) + 2;
  }
  if (col_width - len > NAMELTH) {
    len = col_width - NAMELTH;
  }
  xloc = ((len == col_width)?(0):1) + len + (itemnum / col_length) *
    (col_width + 2);
  yloc = BLD_START + itemnum % col_length;
  len = col_width - len;

  /* clear the row */
  if ((itemnum < BLD_DEMILIMIT) ||
      (itemnum > BLD_HIDELOGIN)) {
    move(yloc, xloc);
    for (; len > 0; len--) {
      addch(' ');
    }
  }

  /* now show the detailed information */
  bottommsg(bld_info[itemnum]);
  move(yloc, xloc);
  refresh();

  /* now do the adjustments */
  switch (itemnum) {

  case BLD_DEMIGOD:
    /* find a user name */
    if (get_string(string, STR_NORM, NAMELTH) > 0) {
      if (strcmp(string, "[none]") == 0) strcpy(string, LOGIN);

      /* verify that it is an actual user */
      if (user_exists(string) == TRUE) {
	strcpy(world.demigod, string);
	if (strcmp(string, LOGIN) == 0) {
	  bottommsg("There will be no demi-god for this world");
	} else {
	  sprintf(string, "The demi-god %s may rule in this world",
		  world.demigod);
	  bottommsg(string);
	}
	refresh();
	presskey();
      } else {
	errormsg("There is no such user on the system");
      }
    }
    break;

  case BLD_DEMILIMIT:
    /* check if possible */
    if ((strcmp(loginname, LOGIN) != 0) &&
	(world.demilimit)) {
      errormsg("You are not able to turn on the demi-limit flag");
      break;
    }

    /* toggle the value */
    if (world.demilimit == TRUE) world.demilimit = FALSE;
    else world.demilimit = TRUE;
    presskey();
    break;

  case BLD_DEMIBUILD:
    /* check if possible */
    if (cfg_world == TRUE) {
      if (strcmp(loginname, LOGIN) != 0) {
	errormsg("You are not able to adjust the demi-build flag");
	break;
      }
    }

    /* toggle the value */
    if (world.demibuild == TRUE) world.demibuild = FALSE;
    else world.demibuild = TRUE;
    presskey();
    break;

  case BLD_MAPTYPE:
    /* toggle the value between settings */
    if (cfg_world == TRUE) {
      errormsg("The coordinate system may only be set during creation");
      break;
    } else {
      if (world.hexmap) world.hexmap = FALSE;
      else world.hexmap = TRUE;
    }
    presskey();
    break;

  case BLD_RELATIVE:
    /* toggle the value */
    if (world.relative_map == TRUE) world.relative_map = FALSE;
    else world.relative_map = TRUE;
    presskey();
    break;

  case BLD_HIDESCORES:
    /* toggle the value */
    if (world.hide_scores == TRUE) world.hide_scores = FALSE;
    else world.hide_scores = TRUE;
    presskey();
    break;

  case BLD_HIDELOGIN:
    /* toggle the value */
    if (world.hide_login == TRUE) world.hide_login = FALSE;
    else world.hide_login = TRUE;
    presskey();
    break;

  case BLD_PASSWD:
  case BLD_ADDPWD:
    /* check for the old password */
    if (strcmp(loginname, LOGIN) != 0) {
      bottommsg("PLEASE ENTER THE CURRENT ==>GOD<== PASSWORD: ");
      value = get_pass(passwd);
#ifdef CRYPT
      strncpy(string, crypt(passwd, SALT), PASSLTH);
#else
      strncpy(string, passwd, PASSLTH);
#endif /* CRYPT */
      string[PASSLTH] = '\0';
      if (strcmp(world.passwd, string) != 0) {
	errormsg("Sorry, the password will remain unchanged");
	break;
      }
      if (itemnum == BLD_ADDPWD) {
	bottommsg("Please Enter a New Add User Password: ");
      } else {
	bottommsg("Please Enter a New God Password: ");
      }
      move(yloc, xloc);
      refresh();
    }

    /* change the password */
    value = get_pass(string);
    if (no_input == TRUE) break;
    if (value < 4) {
      errormsg("Password Too Short");
      break;
    } else if (value > PASSLTH) {
      errormsg("Password Too Long");
      break;
    }
    bottommsg("Password Accepted; Please confirm it");
    move(yloc, xloc);
    refresh();
    value = get_pass(passwd);
    if (no_input == TRUE) break;

    if ((value < 4) || (value > PASSLTH)
	|| (strncmp(passwd, string, PASSLTH) != 0)) {
      errormsg("Invalid Password Match");
      break;
    }

    /* now store it */
#ifdef CRYPT
    if (itemnum == BLD_ADDPWD) {
      strncpy(world.addpwd, crypt(passwd, SALT), PASSLTH);
      world.addpwd[PASSLTH] = '\0';
    } else {
      strncpy(world.passwd, crypt(passwd, SALT), PASSLTH);
      world.passwd[PASSLTH] = '\0';
    }
#else
    if (itemnum == BLD_ADDPWD) {
      strncpy(world.addpwd, passwd, PASSLTH);
      world.addpwd[PASSLTH] = '\0';
    } else {
      strncpy(world.passwd, passwd, PASSLTH);
      world.passwd[PASSLTH] = '\0';
    }
#endif /* CRYPT */
    if (itemnum == BLD_ADDPWD) {
      bottommsg("Password Recorded... Remember it to add a new nation.");
    } else {
      bottommsg("Password Recorded... Remember it to access God mode.");
    }
    presskey();
    break;

  case BLD_VRFYLOGIN:
    /* toggle the value */
    if (world.verify_login == TRUE) world.verify_login = FALSE;
    else world.verify_login = TRUE;
    presskey();
    break;

  case BLD_XSIZE:
  case BLD_YSIZE:
    /* is it possible? */
    if (cfg_world == TRUE) {
      errormsg("You are not able to adjust the world size without rebuilding");
      break;
    }

    /* get the number */
    value = In_and_Check(MIN_WORLD_SIZE, MAX_MAPTYPE + 1,
			 "A world of that size is not possible");
    if (value == MIN_WORLD_SIZE - 1) break;

    /* verify the value */
    if (value % 8 != 0) {
      errormsg("The world size should be divisible by 8");
    } else {
      if (itemnum == BLD_XSIZE)
	world.mapx = (maptype) (value - 1);
      else world.mapy = (maptype) (value - 1);
    }
    break;

  case BLD_POINTS:
    /* get the number */
    value = In_and_Check(50, 200,
		 "There should be between 50 and 200 points to build with");

    /* verify the value */
    if (value == 49) break;
    MAXPTS = (uns_char) value;
    break;

  case BLD_BUILDDIST:
    value = In_and_Check(5, 50,
			 "The minimum distance should be between 5 and 50");

    /* verify */
    if (value == 4) break;
    BUILDDIST = (uns_char) value;
    break;

  case BLD_YEAR:
    /* get the number */
    value = In_and_Check(0, 5000, "The year should be between 1 and 5000");
    if (value == 0) break;

    /* change the value */
    old_turn = TURN;
    TURN = MONTH(TURN) + value * 12 - 11;
    START_TURN += (TURN - old_turn);
    break;

  case BLD_MONTH:
    /* get the month */
    value = get_month();

    /* verify input */
    if (value == 12) {
      errormsg("That isn't a month on my calander");
    } else {
      old_turn = TURN;
      TURN = TURN - MONTH(TURN) + value;
      START_TURN += (TURN - old_turn);
    }
    break;

  case BLD_EXPOSURE:
    /* get a "double" and check its value */
    tmpdbl = get_double(FALSE);

    /* check the range */
    if (tmpdbl > 5.0) {
      errormsg("The exposure ratio must be between 0.0 and 5.0");
    } else {
      EXPOSURE = (uns_char) (tmpdbl * 10.0);
    }
    break;

  case BLD_GROWTH:
    /* get a "double" and check its value */
    tmpdbl = get_double(FALSE);

    /* check the range */
    if (tmpdbl > 12.0) {
      errormsg("The growth rate must be between 0.0 and 12.0");
    } else {
      GROWTH = (uns_char) (tmpdbl * 20.0);
    }
    break;

  case BLD_PREJLAND:
  case BLD_PWATER:
  case BLD_PMOUNT:
  case BLD_PREVOLT:
  case BLD_PMONSTER:
  case BLD_PSCOUT:
  case BLD_PMERCVAL:
  case BLD_PMERCMONST:
  case BLD_PTRADE:
  case BLD_PTGMETAL:
  case BLD_PTGJEWEL:
  case BLD_PTGSPELL:
    /* check it */
    if ((cfg_world == TRUE) && (itemnum < BLD_PREVOLT) &&
	(itemnum != BLD_PMONSTER)) {
      errormsg("You may not adjust that item except during world creation");
      break;
    }

    /* get a percentage value */
    value = In_and_Check(0, 100, "A percentage should be between 0 and 100");
    if (value == -1) break;

    /* assign it */
    switch (itemnum) {
    case BLD_PREJLAND:
      world.preject_land = (char) value;
      break;
    case BLD_PWATER:
      if (value < 10) {
	errormsg("There must be at least 10% water.");
      } else if (value > 80) {
	errormsg("There may not be more than 80% water.");
      } else {
	PWATER = (char) value;
      }
      break;
    case BLD_PMOUNT:
      PMOUNT = (char) value;
      break;
    case BLD_PREVOLT:
      PREVOLT = (char) value;
      break;
    case BLD_PMONSTER:
      PMONSTER = (char) value;
      break;
    case BLD_PSCOUT:
      PSCOUT = (char) value;
      break;
    case BLD_PMERCVAL:
      PMERCVAL = (char) value;
      break;
    case BLD_PMERCMONST:
      PMERCMONST = (char) value;
      break;
    case BLD_PTRADE:
      PTRADE = (char) value;
      break;
    case BLD_PTGMETAL:
      if (value + PTGJEWEL + PTGSPELL > 85) {
	errormsg("Only up to 85% of tgoods may be Metals, Jewels and Magic");
      } else {
	PTGMETAL = (char) value;
      }
      break;
    case BLD_PTGJEWEL:
      if (value + PTGJEWEL + PTGSPELL > 85) {
	errormsg("Only up to 85% of tgoods may be Metals, Jewels and Magic");
      } else {
	PTGJEWEL = (char) value;
      }
      break;
    case BLD_PTGSPELL:
      if (value + PTGJEWEL + PTGSPELL > 85) {
	errormsg("Only up to 85% of tgoods may be Metals, Jewels and Magic");
      } else {
	PTGSPELL = (char) value;
      }
      break;
    }
    break;

  case BLD_NPCS:
    /* check it */
    if (cfg_world == TRUE) {
      errormsg("You may not adjust that item except during world creation");
      break;
    }

    /* get the input */
    value = In_and_Check(0, 50, "There should be between 0 and 50 NPCs");
    if (value == -1) break;

    /* set value */
    world.npcs = (ntntype) value;
    break;

  case BLD_LIZARD:
  case BLD_SAVAGE:
  case BLD_NOMAD:
  case BLD_PIRATE:
    /* check it */
    if (cfg_world == TRUE) {
      errormsg("You may not adjust that item except during world creation");
      break;
    }

    /* get the input */
    value = In_and_Check(0, 200, "This setting be between 0 and 200");
    if (value == -1) break;

    /* set it */
    switch (itemnum) {
    case BLD_LIZARD:
      world.lizards = (uns_char) value;
      break;
    case BLD_SAVAGE:
      world.savages = (uns_char) value;
      break;
    case BLD_NOMAD:
      world.nomads = (uns_char) value;
      break;
    case BLD_PIRATE:
      world.pirates = (uns_char) value;
      break;
    }
    break;

  case BLD_LATESTART:
    value = In_and_Check(0, 200,
			 "Late start should be between 0 [Off] and 200");
    if (value == -1) break;
    world.latestart = value;
    break;

  case BLD_FLEETCBVAL:
  case BLD_WAGONCBVAL:
    /* get the input */
    value = In_and_Check(-75, 200,
			 "That combat bonus must be between -75 and 200");
    if (value == -76) break;
    if (itemnum == BLD_FLEETCBVAL) {
      FLEET_CBVAL = (short) value;
    } else {
      WAGON_CBVAL = (short) value;
    }
    break;

  case BLD_DICE:
    /* enter in the number of dice */
    value = In_and_Check(1, 100,
			 "There should be between 1 and 100 dice. (5-10 best)");
    if (value == 0) break;

    /* check the input */
    NUMDICE = (char) value;
    break;

  case BLD_AVGDAM:
    value = In_and_Check(25, 75,
			 "Average battle damage should be between 25% and 75%");
    if (value == 24) break;
    AVG_DAMAGE = (char) value;
    break;

  case BLD_DAMLIM:
    value = In_and_Check(25, 95,
			 "The damage needed to stop attackers is between 25% and 95%");
    if (value == 24) break;
    DAMAGE_LIMIT = (char) value;
    break;

  case BLD_OVERMATCH:
    value = In_and_Check(5, 25,
			 "The overmatch adjustment is between 5% and 25%");
    if (value == 4) break;
    OVERMATCH_ADJ = (char) value;
    break;

  case BLD_PMINDAM:
    value = In_and_Check(0, 10, "Minimum battle damage is between 0% and 10%");
    if (value == -1) break;
    PMINDAMAGE = (char) value;
    break;

  case BLD_NUMRANDOM:
  case BLD_NUMWEATHER:
    value = In_and_Check(0, 50, "There should be between 0 and 50 events");
    if (value == -1) break;
    if (itemnum == BLD_NUMRANDOM) NUMRANDOM = (char) value;
    else NUMWEATHER = (char) value;
    break;

  case BLD_MERCMEN:
    /* enter the value */
    value = In_and_Check(0, BIGINT, "Boy, you must like them mercenaries!");
    if (value == -1) break;
    MERCMEN = value;
    break;

  case BLD_MERCATT:
  case BLD_MERCDEF:
    /* enter value */
    value = In_and_Check(-50, 500,
			 "Mercenary combat bonuses should be between -50 and 500");
    if (value == -51) break;
    if (itemnum == BLD_MERCATT) MERCATT = (short) value;
    else MERCDEF = (short) value;
    break;

  case BLD_SMOOTH:
    /* check it */
    if (cfg_world == TRUE) {
      errormsg("You may not adjust that item except during world creation");
      break;
    }

    /* how many times to smooth the world */
    value = In_and_Check(1, 15, "There should be between 1 and 15 smoothings");
    if (value == 0) break;
    world.smoothings = (int)value;
    break;

  case BLD_LRANGE:
    /* check it */
    if (cfg_world == TRUE) {
      errormsg("You may not adjust that item except during world creation");
      break;
    }

    /* how many blocks to search over */
    value = In_and_Check(0, 4, "The land range check is between 0 and 4");
    if (value == -1) break;
    world.land_range = (int) value;
    break;

  case BLD_MAXDIPADJ:
    /* get it */
    value = In_and_Check(1, 9,
	"The maximum diplomacy adjustment must be between 1 and 9");
    if (value == -1) break;
    MAXDIPADJ = (uns_char) value;
    break;

  case BLD_BRIBE:
    /* enter it */
    value = In_and_Check(0, BIGINT, "Isn't that a bit on the high side?");
    if (value == -1) break;

    /* verify it */
    BRIBELEVEL = value;
    break;

  case BLD_MAXSUPPLIES:
    /* enter the value */
    value = In_and_Check(0, 30, "Why even bother with supplies at that point?");
    if (value == -1) break;
    if (value == 0) {
      bottommsg("Eliminating supplying from game");
      presskey();
    }
    MAXSUPPLIES = (char) value;
    break;

  case BLD_NVSPLYDIST:
  case BLD_CITYDIST:
    /* enter the value */
    value = In_and_Check(-1, 5,
			 "Range must be between -1 (comm range) and 5");
    if (value == -2) break;
    if (value == -1) {
      bottommsg("Navies and cvans will supply over national communication range");
      presskey();
      if (itemnum == BLD_CITYDIST) {
	CITYXFDIST = NTN_DISTIND;
      } else {
	NVSPLYDIST = NTN_DISTIND;
      }
    } else if (itemnum == BLD_CITYDIST) {
      CITYXFDIST = (char) value;
    } else {
      NVSPLYDIST = (char) value;
    }
    break;

  default:
    /* huh? */
    errormsg("What's going on here?");
    break;
  }
}

/* BLD_TITLE -- Place the title of the interface on the screen */
static void
bld_title PARM_2(char *, desc_str, char *, title_str)
{
  sprintf(string, "Datadir: %s", datadirname);
  errorbar(desc_str, string);
  standout();
  mvaddstr(0, (COLS - strlen(title_str)) / 2, title_str);
  standend();
}

/* MAKEWORLD -- This routine is the backbone for creation of the world
                The rflag indicates that a scenario should be read in. */
void
makeworld PARM_1 (int, rflag)
{
  char passwd[PASSLTH+1], tnews_fname[LINELTH];
  int i, valid, done;

  /* conquer makeworld information */
  cq_init("cqbuild");
  bld_title("World Generator", "< Conquer Genesis Mode >");

  /* diplay beginning information */
  mvaddstr(2, 5, "Welcome:  This portion of the conquer interface provides for");
  mvaddstr(3, 0, "the construction of a conquer world map and the non-player nations");
  mvaddstr(4, 0, "within it");

  mvaddstr(6, 5, "Once the world has been created, you may access the extended");
  mvaddstr(7, 0, "commands available to the Super User, through the login name");
  mvaddstr(8, 0, "of \"god\".");

  mvaddstr(10, 5, "The file \"nations\" is used to configure what NPC nations are");
  mvaddstr(11, 0, "created during Genesis.  The data in that file may be changed");
  mvaddstr(12, 0, "using the administrative command: conqrun -A.");

  mvaddstr(14, 5, "The player character countries are created using the administrative");
  mvaddstr(15, 0, "command:  conqrun -a");
  if (strcmp(datadirname,"[default]")!=0)
    printw(" -d %s", datadirname);
  addch('.');
  errormsg("");

  bottommsg("....Initialize the nation structures");
  zeroworld();
  bottommsg("Initialization complete:  And god awoke....");
  sleep(2);

  /* now build the interface screen */
  clear();
  bld_title("World Generator", "< Conquer Genesis Mode >");

  /* enter super user password routine */
  valid = FALSE;
  while(valid == FALSE) {
    bottommsg("");
    mvaddstr(3, 0, "Enter a Super-User Password: ");
    clrtoeol();
    refresh();
    i = get_pass(string);
    if (i < 4) {
      errormsg("Password Too Short");
      continue;
    } else if (i > PASSLTH) {
      errormsg("Password Too Long");
      continue;
    }
    bottommsg("Password Accepted; Please confirm it");
    mvaddstr(3, 0, "Verify the Super-User Password: ");
    clrtoeol();
    refresh();
    i = get_pass(passwd);

    if ((i < 4) || (i > PASSLTH) ||
	(strncmp(passwd, string, PASSLTH) != 0)) {
      errormsg("Invalid Password Match");
    } else valid = TRUE;
  }

  /* now store it */
#ifdef CRYPT
  strncpy(world.passwd, crypt(passwd, SALT), PASSLTH);
#else
  strncpy(world.passwd, passwd, PASSLTH);
#endif /* CRYPT */
  world.passwd[PASSLTH] = '\0';
  errormsg("Password Recorded... Remember it to access God mode");

  /* set the default values for the world */
  if (remake == FALSE) bld_defaults();
  if (START_TURN > 0) {
    TURN = START_TURN;
  } else {
    TURN = START_TURN = 1;
  }
  done = bld_config(TRUE);

  /* get the flip out of here */
  if (done == (-1)) {
    cq_reset();
    return;
  }

  /* open the news file */
  sprintf(tnews_fname, "tmp%s.%03d", newsfile, 0);
  if( (fnews = fopen(tnews_fname, "w")) != NULL ) {
    fprintf(fnews, "1\tGLOBAL HEADLINE NEWS -- The Epoch\n");
    fprintf(fnews, "5\tPERSONAL LISTINGS -- The Epoch\n");
  } else {
    sprintf(string, "error opening news file <%s>\n", string);
    errormsg(string);
    return;
  }

  /* read map in from mapfiles */
  if (rflag == FALSE) createworld();
  else {
    /* UNIMPLEMENTED: readmap(); */
    fprintf(stderr, "Map reading unimplemented \n");
  }
  rawmaterials();

  cq_reset();
  verify_data(__FILE__, __LINE__);
  is_update = TRUE;
  unum_defaults();
  write_data();

  /* remove all of the prior data files */
#ifndef VMS
  sprintf(string, "%s *%s *%s *%s %s* %s 2> /dev/null", REMOVE_NAME,
	  exetag, msgtag, isontag, newsfile, timefile);
#else
  sprintf (string, "%s *.%s;*, *.%s;*, *.%s;*, %s.*;*, %s*.;*/nolog",
	   REMOVE_NAME, exetag, msgtag, isontag, newsfile, timefile);
#endif /* VMS */
#ifdef DEBUG
  fprintf(stderr, "%s\n", string);
#endif /* DEBUG */
  system(string);

  /* finish off news file */
  fclose(fnews);

  /* now relocate it */
  sprintf(string, "%s.%03d", newsfile, 0);
  if (move_file(tnews_fname, string) == -1) {
    fprintf(stderr, "warning:  could not rename the starting news file");
  } else {
    /* finally, fix it up */
    sort_news(START_TURN);
  }
}

/* BLD_CONFIG -- Set the configurations of the world */
int
bld_config PARM_1(int, buildit)
{
  char *help_str;
  int done, current;

  /* compute the number of columns and the width */
  if ((BLD_OPTIONS + 1) / 2 < LINES - 7) {
    col_length = (BLD_OPTIONS + 1) / 2;
    col_width = COLS / 2 - 2;
  } else if ((BLD_OPTIONS + 1) / 3 < LINES - 7) {
    col_length = (BLD_OPTIONS + 2) / 3;
    col_width = COLS / 3 - 2;
  } else {
    col_length = (BLD_OPTIONS + 3) / 4;
    col_width = COLS / 4 - 2;
  }

  /* start from the beginnin' */
  done = FALSE;
  current = BLD_DEMIGOD;

  /* specify help string */
  if (buildit == TRUE) {
    help_str = bld_help;
    cfg_world = FALSE;
  } else {
    help_str = cfg_help;
    cfg_world = TRUE;
  }

  /* now get the configuration of the rest */
  while (done == FALSE) {

    /* set up screen display */
    move(0,0);
    clrtobot();
    if (buildit == TRUE)
      bld_title("World Generator", "< Conquer Genesis Mode >");
    else
      bld_title("World Adjustor", "< Conquer Configuration Mode >");

    /* now show all of the information */
    bld_display(current);
    bottommsg(help_str);

    /* show the current position */
    move(BLD_START + current % col_length,
	 (current / col_length + 1) * col_width +
	 2 * (current / col_length));
    refresh();

    /* parse the input */
    switch (next_char()) {
    case '?':
      /* show the item information message */
      bottommsg(bld_info[current]);
      presskey();
      break;
    case CNTRL_L:
    case CNTRL_R:
      wrefresh(curscr);
      break;
    case '\n':
    case '\r':
    case ' ':
      /* move forward through list */
      current++;
      if (current == BLD_OPTIONS) current = 0;
      break;
    case 'H':
    case 'h':
      /* move into the left column */
      if (current >= col_length) {
	current -= col_length;
      } else {
	beep();
      }
      break;
    case 'L':
    case 'l':
      /* move into the right column */
      if (current + col_length < BLD_OPTIONS) {
	current += col_length;
      } else {
	beep();
      }
      break;
    case 'J':
    case 'j':
      /* move down */
      if (((current + 1) % col_length) &&
	  (current < BLD_OPTIONS - 1)) {
	current++;
      } else {
	beep();
      }
      break;
    case 'K':
    case 'k':
      /* move up */
      if (current % col_length) {
	current--;
      } else {
	beep();
      }
      break;
    case '\t':
      /* go through the list left to right then top to bottom */
      if (current + col_length < BLD_OPTIONS) {
	current += col_length;
      } else if (current ==
		 (BLD_OPTIONS / col_length) * col_length - 1) {
	current = 0;
      } else {
	current %= col_length;
	current++;
      }
      break;
    case '\b':
    case EXT_DEL:
      /* go through the list bottom to top then left to right */
      if (current == 0) {
	current = BLD_OPTIONS - 1;
      } else {
	current--;
      }
      break;
    case 'C':
    case 'c':
    case EXT_ESC:
      /* adjust an entry */
      bld_change(current);
      break;
    case 'Q':
      /* finish, and don't build */
      if (buildit == TRUE)
	bottommsg("Quit without building? ");
      else bottommsg("Quit without saving changes? ");
      if (y_or_n()) {
	done = (-1);
      } else {
	done = FALSE;
      }
      break;
    case 'q':
      /* finish and build */
      if (buildit == TRUE) 
	bottommsg("Quit configuration and create world? ");
      else bottommsg("Quit and save information? ");
      if (y_or_n()) {
	done = TRUE;
      } else {
	done = FALSE;
      }
      break;
    case 'R':
      /* reset all of the items */
      if (buildit == TRUE) {
	bottommsg("Do you wish to return to the default settings? ");
	if (y_or_n()) {
	  bld_defaults();
	}
	break;
      }
    default:
      /* let 'em hear it */
      beep();
      bottommsg(help_str);
      break;
    }
  }

  return(done);
}
 
