/* This file provides the nation information screen to the user */
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
/*
 * The original version of the file "ntninfoG.c" was written by
 * Martin Forssen (maf@dtek.chalmers.se) and later rewritten by
 * me to work with new data structures.  Thanks Martin!
 *
 *         Adam Bryant
 *         adb@bu.edu 
 */
#define USE_CODES
#define PRINT_CODES
#include "dataG.h"
#include "butesX.h"
#include "mtrlsX.h"
#include "racesX.h"
#include "worldX.h"
#include "activeX.h"
#include "nclassX.h"
#include "stringX.h"
#include "ntninfoG.h"
#include "keyvalsX.h"

/* All of the input and output functions should be written here */

/* Here are the global variables  (that is: global in this routine) */
static NI_SCRPTR ni_screen;	/* Pointer to array of screen items */
static NI_SCRPTR current;	/* Pointer to current item */
static int currnum;		/* Number of current item */
static int numitems;		/* Total number of items */
static int ni_doneflag;		/* Total number of items */
static int numcolumns;		/* Number of columns */
static int colwidth;		/* The width of the columns */
static int ni_cycle_forward;	/* Directional Indicator */

/* NI_CLRTOENTER -- Clear the area needed to enter input */
static void
ni_clrtoenter PARM_1(int, len)
{
  int i;

  /* check input */
  if (current == NULL) return;

  /* position appropriately */
  move(current->line, current->col + colwidth - len);
  for (i = 0; i < len; i++) {
    addch(' ');
  }

  /* assure proper positioning */
  if (len == 0) len = 1;

  /* now relocate */
  move(current->line, current->col + colwidth - len);
  refresh();
}

/* NI_IGNORE -- Ain't gonna do nothing */
static void
ni_ignore PARM_0(void)
{
  /* don't do much of anything */
  string[0] = '\0';
}

/* NI_TITLE -- Ain't gonna do nothing but the title */
static void
ni_title PARM_0(void)
{
  /* check the setting */
  string[0] = '\0';
  if ((current == NULL) ||
      (current->item == NULL)) return;

  /* copy in the title */
  strcpy(string, current->item->label);
}

/* NI_INATT -- Read in an attribute value */
static void
ni_inatt PARM_0(void)
{
  double tempflt;
  short tempval;
  int allow_negs = FALSE;

  /* check the setting */
  if ((current == NULL) ||
      (current->item == NULL)) return;

  /* clear out the input */
  ni_clrtoenter(6);

  /* check if negatives are allowed */
  if (current->item->minval < 0) {
    allow_negs = TRUE;
  }

  /* now read in the input */
  if (current->item->modifier > 1) {

    /* must get a float and convert it */
    tempflt = get_double(allow_negs);

    /* convert it */
    tempval = (int) (tempflt * current->item->modifier);

  } else {
    /* get an integer value */
    tempval = get_number(allow_negs);
  }

  /* check the boundaries */
  if (current->item->stats & NI_BOUNDARY) {

    /* now check the proper edges */
    switch (current->item->ident) {
    case NIT_LEFTEDGE:
      if ((tempval > ntn_ptr->rightedge) ||
	  (tempval < - MAPX)) {
	errormsg("That is not within the proper bounds of a western edge");
	return;
      }
      break;
    case NIT_RIGHTEDGE:
      if ((tempval < ntn_ptr->leftedge) ||
	  (tempval > MAPX - 1)) {
	errormsg("That is not within the proper bounds of an eastern edge");
	return;
      }
      break;
    case NIT_TOPEDGE:
      if ((tempval < ntn_ptr->bottomedge) ||
	  (tempval > MAPY)) {
	errormsg("That is not within the proper bounds of a northern edge");
	return;
      }
      break;
    case NIT_BOTTOMEDGE:
      if ((tempval > ntn_ptr->topedge) ||
	  (tempval < 0)) {
	errormsg("That is not within the proper bounds of a northern edge");
	return;
      }
      break;
    }

    /* change it already */
    *(current->item->it.p_short) = tempval;

  } else {

    /* now check to see if it is allowable */
    if ((tempval < current->item->minval) ||
	(tempval > current->item->maxval)) {
      /* tell them it ain't so, joe */
      errormsg(current->item->emsg);
    } else {
      /* do the change thing */
      *(current->item->it.p_short) = tempval;
    }

  }
}

/* NI_OUTATT -- Send out an attribute value */
static void
ni_outatt PARM_0(void)
{
  int prev_ch, len;
  int value;

  /* quick check */
  string[0] = '\0';
  if ((current == NULL) ||
      (current->item == NULL)) return;
  value = (int) *(current->item->it.p_short);

  /* check for any preview character */
  if (current->item->stats & NI_DOLLAR) {
    prev_ch = '$';
  } else if ((current->item->stats & NI_BONUS) &&
	     (*current->item->it.p_short >= 0)) {
    prev_ch = '+';
  } else {
    prev_ch = ' ';
  }

  /* build it */
  if (current->item->modifier > 1) {
    sprintf(string, "%c%d.%d", prev_ch,
	    value / current->item->modifier,
	    ((value * 10) / current->item->modifier) % 10);
  } else {
    sprintf(string, "%c%d", prev_ch, value);
  }

  /* now check the trim */
  len = trim_str(string, FALSE);

  /* is there a trailing percent symbol? */
  if (current->item->stats & NI_PERCENT) {
    string[len] = '%';
    string[len+1] = '\0';
  }
}

/* string for use by the ni_outhal routine */
static char *hal_strings[] = {
  "Min", "VryLow", "Low", "LowAvg", "Avg",
  "HghAvg", "High", "VryHgh", "Max"
};

/* NI_OUTHAL -- Output a "High" "Avg" "Low" string based on the need */
static void
ni_outhal PARM_0(void)
{
  int value, range;

  /* god goes to the original method */
  if (is_god == TRUE) {
    ni_outatt();
    return;
  }

  /* quick check, then initialize */
  string[0] = '\0';
  if ((current == NULL) ||
      (current->item == NULL)) return;
  value = (int) *(current->item->it.p_short);
  if (value >= current->item->maxval) {
    value = sizeof(hal_strings)/sizeof(char *) - 1;
  } else if (value <= current->item->minval) {
    value = 0;
  } else {
    range = current->item->maxval - current->item->minval;
    value -= current->item->minval;
    value *= (sizeof(hal_strings)/sizeof(char *) - 2);
    value /= range;
    value++;
  }
  strcpy(string, hal_strings[value]);
}

/* NI_INSTR -- Call to read in a new string */
static void
ni_instr PARM_0(void)
{
  /* check the input */
  if ((current == NULL) ||
      (current->item == NULL)) return;

  /* clear out the input */
  ni_clrtoenter(current->item->maxval);

  /* check the limitations */
  if (current->item->stats & NI_NOSPACE) {
    get_string(string, STR_NORM, current->item->maxval);
  } else {
    get_string(string, STR_SPACE, current->item->maxval);
  }

  /* exit if given null input */
  if (string[0] == '\0') return;

  /* check the string lower */
  if (strlen(string) < current->item->minval) {
    errormsg("That string was too short to be used there");
    return;
  }

  /* now do special checks */
  switch (current->item->ident) {
  case NIT_NATION:
    /* check for other nation names */
    if ((ntnbyname(string) != NULL) &&
	(ntnbyname(string) != ntn_ptr)) {
      errormsg("That nation name is already in use");
      return;
    }
    break;
  case NIT_LOGIN:
    /* check the user name on the system */
    if (user_exists(string) == FALSE) {
      errormsg("I cannot find that user on the system");
      return;
    }
    break;
  }

  /* done, okay to copy it now */
  strcpy(current->item->it.p_string, string);
}

/* NI_OUTSTR -- Simple function to send out the string */
static void
ni_outstr PARM_0(void)
{
  /* check the input */
  string[0] = '\0';
  if ((current == NULL) ||
      (current->item == NULL)) return;

  /* now just copy it */
  strcpy(string, current->item->it.p_string);
}

/* NI_INCHAR -- Enter in a single character */
static void
ni_inchar PARM_0(void)
{
  int new_char;

  /* check the input */
  if ((current == NULL) ||
      (current->item == NULL)) return;

  /* now get the character */
  new_char = next_char();

  /* clear out the input */
  ni_clrtoenter(4);

  /* test the input */
  if (current->item->ident == NIT_MARK) {
    if (!markok(new_char, ntn_ptr->race, TRUE)) return;
  }

  /* 'tis okay */
  *(current->item->it.p_string) = (char) new_char;
}

/* NI_OUTCHAR -- Send out the single character */
static void
ni_outchar PARM_0(void)
{
  char buf[2];

  /* check the input */
  string[0] = '\0';
  if ((current == NULL) ||
      (current->item == NULL)) return;

  /* now simply make the character available for display */
  buf[0] = *(current->item->it.p_string);
  buf[1] = '\0';
  form_str(string, buf, FALSE);
}

/* NI_CYCLE -- Cycle through the various items */
static void
ni_cycle PARM_0(void)
{
  int value, multer, adder, modify;

  /* check the input */
  if ((current == NULL) ||
      (current->item == NULL)) return;
  modify = current->item->modifier;

  /* clear out the input */
  ni_clrtoenter(0);

  /* check the base */
  switch (current->item->ident) {
  case NIT_AGGRESS:
    /* provide proper alignment */
    value = *(current->item->it.p_short);
    adder = n_alignment(value);
    multer = 4;
    value = n_aggression(value);
    break;
  case NIT_NPCSTAT:
    /* npc adjustments */
    value = *(current->item->it.p_short);
    multer = ACT_PCMODULUS;
    adder = value % multer;
    if (value >= ACT_PCMODULUS) {
      value = 1;
    } else {
      value = 0;
    }
    break;
  case NIT_ALIGN:
    /* alignment adjustments */
    value = *(current->item->it.p_short);
    multer = 1;
    adder = value;
    value = n_alignment(value);
    adder -= value;
    break;
  default:
    /* normal mode */
    value = *(current->item->it.p_short);
    adder = 0;
    multer = 1;
    break;
  }

  /* go in the proper direction */
  if (ni_cycle_forward) {
    value += modify;
  } else {
    value -= modify;
  }
  if (value > current->item->maxval) {
    value = current->item->minval;
  } else if (value < current->item->minval) {
    value = current->item->maxval;
  }

  /* now store the adjustment */
  value *= multer;
  value += adder;
  *(current->item->it.p_short) = value;
  sleep(2);
}

/* NI_OUTIVAL -- ready the itemtype values for output */
static void
ni_outival PARM_0(void)
{
  /* check input */
  string[0] = '\0';
  if ((current == NULL) ||
      (current->item == NULL)) return;

  /* now test for the dollar symbol */
  if (current->item->stats & NI_DOLLAR) {
    sprintf(string, "$%.0f", (double) *(current->item->it.p_itemv));
  } else {
    sprintf(string, "%.0f", (double) *(current->item->it.p_itemv));
  }
}

/* NI_OUTLONG -- prepare a long value for output */
static void
ni_outlong PARM_0(void)
{
  /* check the input */
  string[0] = '\0';
  if ((current == NULL) ||
      (current->item == NULL)) return;

  /* send it out */
  if (current->item->stats & NI_DOLLAR) {
    sprintf(string, "$%ld", *(current->item->it.p_longint));
  } else {
    sprintf(string, "%ld", *(current->item->it.p_longint));
  }
}

/* NI_INCOORD -- input the coordinates */
static void
ni_incoord PARM_0(void)
{
  int xloc, yloc;

  /* check the input */
  if ((current == NULL) ||
      (current->item == NULL)) return;

  /* clear out the input area */
  ni_clrtoenter(9);

  /* get the X coordinate */
  xloc = (get_number(TRUE) + MAPX) % MAPX;
  if (no_input) return;

  /* clear out the input area */
  ni_clrtoenter(9);

  /* get the Y coordinate */
  bottommsg("Now enter a new Y coordinate");
  yloc = get_number(FALSE);
  if (no_input) return;

  /* check the Y coordinate */
  if (!Y_ONMAP(yloc)) {
    errormsg("That coordinate is off of the map");
    return;
  }

  /* there are only a couple coordinate types */
  switch (current->item->ident) {
  case NIT_CENTERCOORD:
    /* now store it */
    ntn_ptr->centerx = (maptype) xloc;
    ntn_ptr->centery = (maptype) yloc;
    break;
  }
}

/* NI_OUTCOORD -- output the coordinates */
static void
ni_outcoord PARM_0(void)
{
  /* check the input */
  string[0] = '\0';
  if ((current == NULL) ||
      (current->item == NULL)) return;

  /* there are only a couple coordinate types */
  switch (current->item->ident) {
  case NIT_CAPCOORD:
    /* just build the capital location */
    sprintf(string, "[%d,%d]", ntn_ptr->capx, ntn_ptr->capy);
    break;
  case NIT_CENTERCOORD:
    /* just build the central location */
    sprintf(string, "[%d,%d]", ntn_ptr->centerx, ntn_ptr->centery);
    break;
  }
}

/* NI_SPECIAL -- output not like any of the other routines */
static void
ni_special PARM_0(void)
{
  int value;

  /* check the input */
  string[0] = '\0';
  if ((current == NULL) ||
      (current->item == NULL)) return;

  /* build based on input */
  switch (current->item->ident) {
  case NIT_RACE:
    /* the race name */
    strcpy(string, race_info[*(current->item->it.p_short)].name);
    break;
  case NIT_ALIGN:
    /* the alignment of the nation */
    if (*(current->item->it.p_short) != INACTIVE) {
      strcpy(string, alignment[n_alignment(*(current->item->it.p_short))]);
    } else {
      strcpy(string, "*dead*");
    }
    break;
  case NIT_CLASS:
    /* show the class name */
    strcpy(string, nclass_list[*(current->item->it.p_short)].name);
    break;
  case NIT_AGGRESS:
    /* show the aggression */
    if (*(current->item->it.p_short) != INACTIVE) {
      strcpy(string, aggressname[n_aggression(*(current->item->it.p_short))]);
    } else {
      strcpy(string, "*dead*");
    }
    break;
  case NIT_NPCSTAT:
    /* to be an npc or not to be an npc */
    value = *(current->item->it.p_short);
    if (value == INACTIVE) {
      strcpy(string, "*dead*");
    } else if (n_ispc(value)) {
      strcpy(string, "Player");
    } else if (n_isnpc(value)) {
      strcpy(string, "NPC");
    } else {
      strcpy(string, "Monster");
    }
    break;
  }
}

/*
 * This itemlist controls the positioning of elements on the screen.
 *
 * To give a title:
 * 
 * { "Title Here", NIT_BLANK, "", "", "", NI_EMPTY,
 *    N, 0, 0, ni_ignore, ni_title, NULL }
 *
 * N indicates the number of items which are grouped after.
 *
 * If a set of items is to be considered a separate section, that
 * section should be proceeded by a line like:
 *
 * { "Title Here", NIT_BLANK, "", "", "", NI_EMPTY | NI_BOLD,
 *    N, 0, 0, ni_ignore, ni_title, NULL }
 *
 * Normal blank lines are possible by using:
 *
 * { "", NIT_BLANK, "", "", "", NI_EMPTY, 0, 0, 0, ni_ignore, ni_ignore, NULL }
 *
 * The actual items are label, id, description, error message for bad
 * input, flags for properties of the entry, minimum value (if
 * appropriate), maximum value (if appropriate), the modifier for
 * operations (dependent on what routines are called), the
 * function called to change the item, the function called to display
 * the item, then there is a pointer to the item itself.  [The
 * pointers should initially be set to NULL, and then set in the
 * ni_init function, otherwise they will not be set properly]
 * 
 */
static NI_ITEM items[NIT_NUMBER] = {
  /* the initial empty entry */
  { "= Identifiers =", NIT_BLANK, "", "", "", NI_EMPTY | NI_BOLD,
      19, 0, 0, ni_ignore, ni_title },

  /* the nation information */
  { "Nation", NIT_NATION,
      "The is the name by which your nation is recognized",
      "Please enter a new name for your nation",
      "That name is already in use by another nation",
      NI_PLAYER | NI_GOD | NI_NOSPACE,
      1, NAMELTH, 0, ni_instr, ni_outstr },
  { "Nation Mark", NIT_MARK,
      "This character symbol is used to mark land owned by your nation",
      "Please enter a new character to be used as your nation mark", "",
      NI_PLAYER | NI_GOD,
      1, 1, 0, ni_inchar, ni_outchar },
  { "Leader", NIT_LEADER,
      "This is the name of the noble ruler of your nation",
      "Please enter a new name for your noble leader",
      "That name does not adequately reflect the valor your leader possesses",
      NI_PLAYER | NI_GOD,
      1, LEADERLTH, 0, ni_instr, ni_outstr },
  { "Race", NIT_RACE,
      "This entry indicates the race of which your nation is composed",
      "Changing the race of this nation...",
      "You are not able to change the race of this country",
      NI_GOD,
      1, RACE_NUMBER - 1, 1, ni_cycle, ni_special },
  { "Alignment", NIT_ALIGN,
      "This indicates the tendency your nation possesses for mischief",
      "Changing the alignment of this nation...",
      "You are not able to change the alignment of this country",
      NI_GOD,
      0, ALIGN_EVIL, 1, ni_cycle, ni_special },
  { "Class", NIT_CLASS,
      "This indicates the governmental and/or economical basis of the nation",
      "Changing the national class of this nation...",
      "You are not able to change the class of this country",
      NI_GOD,
      0, 0, 1, ni_cycle, ni_special },
  { "Aggression", NIT_AGGRESS,
      "This indicates the mode the computer will use if it updates the nation",
      "Changing the computer aggressional setting of this country",
      "You are not able to change the aggression setting of this country",
      NI_GOD | NI_PLAYER,
      0, ACT_NUMBER - 1, 1, ni_cycle, ni_special },
  { "NPC status", NIT_NPCSTAT,
      "This indicates whether or not the nation is a Non-Player Country",
      "Switching the NPC status of the nation",
      "Sorry, I just can't let you change that",
      NI_GOD | NI_PLAYER,
      0, 1, 1, ni_cycle, ni_special },
  { "Login", NIT_LOGIN,
      "This indicates the login name of the player owning this country",
      "Enter a new user name to run this nation",
      "I couldn't find that user on the system",
      NI_GOD | NI_GODSEE,
      0, NAMELTH, 1, ni_instr, ni_outstr },

  /* blank line here */
  { "", NIT_BLANK, "", "", "", NI_EMPTY, 0, 0, 0, ni_ignore, ni_ignore },

  /* list the important economic settings */
  { NULL, NIT_TAXRATE,
      NULL,
      "Enter a new tax rate for the nation",
      "That tax rate was much too high to be realistic",
      NI_PLAYER | NI_GOD,
      0, 0, 0, ni_inatt, ni_outatt },
  { NULL, NIT_CHARITY,
      NULL,
      "Enter a new charity setting for the nation",
      "That charity setting was much too high to be realistic",
      NI_PLAYER | NI_GOD,
      0, 0, 0, ni_inatt, ni_outatt },
  { NULL, NIT_CURRENCY,
      NULL,
      "Enter a new currency value rate for the nation",
      "That currency was out of the range of its limits",
      NI_GOD,
      0, 0, 0, ni_inatt, ni_outhal },
  { NULL, NIT_INFLATION,
      NULL,
      "Enter a new inflation rate for the nation",
      "That inflation rate was out of the range of its limits",
      NI_GOD,
      0, 0, 0, ni_inatt, ni_outatt },

  /* attributes title */
  { "= Attributes =", NIT_BLANK, "", "", "", NI_EMPTY | NI_BOLD,
      19, 0, 0, ni_ignore, ni_title },
  { NULL, NIT_COMMRANGE,
      NULL,
      "Enter a new communication range for the nation",
      "That communication range was out of the range of its limits",
      NI_GOD,
      0, 0, 0, ni_inatt, ni_outatt },
  { NULL, NIT_EATRATE,
      NULL,
      "Enter a new eat rate for the nation",
      "That eat rate was out of the range of its limits",
      NI_GOD,
      0, 0, 0, ni_inatt, ni_outatt },
  { NULL, NIT_HEALTH,
      NULL,
      "Enter a health rate for the nation",
      "That health rate was out of the range of its limits",
      NI_GOD,
      0, 0, 0, ni_inatt, ni_outhal },
  { NULL, NIT_JEWELWORK,
      NULL,
      "Enter a jewelcraft setting for the nation",
      "That jewelcraft setting was out of the range of its limits",
      NI_GOD,
      0, 0, 0, ni_inatt, ni_outhal },
  { NULL, NIT_KNOWLEDGE,
      NULL,
      "Enter a new knowledge level for the nation",
      "That knowledge setting was out of the range of its limits",
      NI_GOD,
      0, 0, 0, ni_inatt, ni_outhal },
  { NULL, NIT_MERCREP,
      NULL,
      "Enter a new mercenary reputation value for the nation",
      "That mercencary rep was out of the range of its limits",
      NI_GOD,
      0, 0, 0, ni_inatt, ni_outhal },
  { NULL, NIT_METALWORK,
      NULL,
      "Enter a new metalcraft setting for the nation",
      "That metalcraft setting was out of the range of its limits",
      NI_GOD,
      0, 0, 0, ni_inatt, ni_outhal },
  { NULL, NIT_MINING,
      NULL,
      "Enter a new mining rating for the nation",
      "That mining rating was out of the range of its limits",
      NI_GOD,
      0, 0, 0, ni_inatt, ni_outhal },
  { NULL, NIT_MORALE,
      NULL,
      "Enter a new morale setting for the nation",
      "That morale setting was out of the range of its limits",
      NI_GOD,
      0, 0, 0, ni_inatt, ni_outhal },
  { NULL, NIT_POPULARITY,
      NULL,
      "Enter a popularity rate for the nation",
      "That popularity rate was out of the range of its limits",
      NI_GOD,
      0, 0, 0, ni_inatt, ni_outhal },
  { NULL, NIT_REPUTATION,
      NULL,
      "Enter a new reputation setting for the nation",
      "That reputation value was out of the range of its limits",
      NI_GOD,
      0, 0, 0, ni_inatt, ni_outhal },
  { NULL, NIT_SPELLPTS,
      NULL,
      "Enter a new spell point value for the nation",
      "That spell point setting was out of the range of its limits",
      NI_GOD,
      0, 0, 0, ni_inatt, ni_outatt },
  { NULL, NIT_SPOILRATE,
      NULL,
      "Enter a new spoil rate for the nation",
      "That spoil rate was out of the range of its limits",
      NI_GOD,
      0, 0, 0, ni_inatt, ni_outatt },
  { NULL, NIT_TERROR,
      NULL,
      "Enter a new terror setting for the nation",
      "That terror value was out of the range of its limits",
      NI_GOD,
      0, 100, 0, ni_inatt, ni_outhal },
  { NULL, NIT_WIZSKILL,
      NULL,
      "Enter a new wizardry skill value for the nation",
      "That wizardry skill was out of the range of its limits",
      NI_GOD,
      0, 50, 0, ni_inatt, ni_outhal },

  /* nation information title */
  { "= Abilities =", NIT_BLANK, "", "", "", NI_EMPTY | NI_BOLD,
      4, 0, 0, ni_ignore, ni_title },
  { "Attack Bonus", NIT_ATTACK,
      "This value indicates the combat bonus gained when your troops attack",
      "Enter a new offensive combat bonus for this nation",
      "Hmm... it looks as though that value is out of range",
      NI_GOD | NI_BONUS,
      -50, 500, 1, ni_inatt, ni_outatt },
  { "Defense Bonus", NIT_DEFENSE,
      "This value indicates the combus bonus given when your troops defend",
      "Enter a new defensive combat bonus for this nation",
      "Hmm... it looks as though that value is out of range",
      NI_GOD | NI_BONUS,
      -50, 500, 1, ni_inatt, ni_outatt },
  { "Reproduction", NIT_REPRO,
      "This value indicates the yearly reproduction rate of the nation",
      "Enter a new reproduction rate for the nation",
      "That reproduction rate was not within proper bounds",
      NI_GOD | NI_PERCENT,
      4, 20, 1, ni_inatt, ni_outatt },
  { "Movement", NIT_MOVE,
      "This value indicates the base movement rate of the nation",
      "Enter a new movement rate for the nation",
      "That movement rate is not within the proper bounds",
      NI_GOD,
      4, 50, 1, ni_inatt, ni_outatt },

  /* blank before raw materials */
  { "= Resources =", NIT_BLANK, "", "", "", NI_EMPTY | NI_BOLD,
      5, 0, 0, ni_ignore, ni_title },
  { "Talons", NIT_TALONS,
      "This is a total of the number of talons within the entire nation",
      "", "", NI_DOLLAR, 0, 0, 0, ni_ignore, ni_outival },
  { "Jewels", NIT_JEWELS,
      "This is the total number of jewels within the entire nation",
      "", "", 0L, 0, 0, 0, ni_ignore, ni_outival },
  { "Metals", NIT_METALS,
      "This is the total number of metals within the entire nation",
      "", "", 0L, 0, 0, 0, ni_ignore, ni_outival },
  { "Food", NIT_FOOD,
      "This is the total amount of food within the entire nation",
      "", "", 0L, 0, 0, 0, ni_ignore, ni_outival },
  { "Wood", NIT_JEWELS,
      "This is the total amount of wood within the entire nation",
      "", "", 0L, 0, 0, 0, ni_ignore, ni_outival },

  /* blank before nation military summary */
  { "= Totals =", NIT_BLANK, "", "", "", NI_EMPTY | NI_BOLD,
      8, 0, 0, ni_ignore, ni_title },
  { "Leaders", NIT_LEADERS,
      "This indicates how many national leaders are within your nation",
      "", "", 0L, 0, 0, 1, ni_ignore, ni_outlong },
  { "Soldiers", NIT_SOLDIERS,
      "This indicates how many soldiers are within your nation",
      "", "", 0L, 0, 0, 0, ni_ignore, ni_outlong },
  { "Civilians", NIT_CIVILIANS,
      "This indicates how many civilians are within your nation",
      "", "", 0L, 0, 0, 0, ni_ignore, ni_outlong },
  { "Monsters", NIT_MONSTERS,
      "This indicates how many monsters are under national control",
      "", "", 0L, 0, 0, 0, ni_ignore, ni_outlong },
  { "Ship Holds", NIT_SHIPS,
      "This indicates how many ships are within the nation",
      "", "", 0L, 0, 0, 0, ni_ignore, ni_outatt },
  { "Wagons", NIT_WAGONS,
      "This indicates how many wagons are within the nation",
      "", "", 0L, 0, 0, 0, ni_ignore, ni_outatt },
  { "Sectors", NIT_SECTORS,
      "This indicates how many sectors are owned by the nation",
      "", "", 0L, 0, 0, 0, ni_ignore, ni_outatt },
  { "Score", NIT_SCORE,
      "This is a relative index of how well conquer thinks you are doing",
      "", "", 0L, 0, 0, 0, ni_ignore, ni_outlong },

  /* Now only god sees these items */
  { "= Coordinates =", NIT_BLANK, "", "", "", NI_EMPTY | NI_BOLD | NI_GODSEE,
      6, 0, 0, ni_ignore, ni_title },
  { "Capital", NIT_CAPCOORD,
      "This position indicates the location of the national capital",
      "", "",
      NI_GODSEE, 0, 0, 0, ni_ignore, ni_outcoord },
  { "Center", NIT_CENTERCOORD,
      "This position indicates the relative central focus of the nation",
      "Enter a new X coordinate for the central focus", "",
      NI_GODSEE | NI_GOD,
      0, 0, 0, ni_incoord, ni_outcoord },
  { "West Edge", NIT_LEFTEDGE,
      "This indicates the western most boundary of the nation",
      "Enter a new western edge for the nation",
      "That value is not a possible western edge for the nation",
      NI_GODSEE | NI_GOD | NI_BOUNDARY,
      -1, 0, 1, ni_inatt, ni_outatt },
  { "East Edge", NIT_RIGHTEDGE,
      "This indicates the eastern most boundary of the nation",
      "Enter a new eastern edge for the nation",
      "That value is not a possible eastern edge for the nation",
      NI_GODSEE | NI_GOD | NI_BOUNDARY,
      0, 0, 1, ni_inatt, ni_outatt },
  { "North Edge", NIT_TOPEDGE,
      "This indicates the northern most boundary of the nation",
      "Enter a new northern edge for the nation",
      "That value is not a possible northern edge for the nation",
      NI_GODSEE | NI_GOD | NI_BOUNDARY,
      0, 0, 1, ni_inatt, ni_outatt },
  { "South Edge", NIT_LEFTEDGE,
      "This indicates the southern most boundary of the nation",
      "Enter a new southern edge for the nation",
      "That value is not a possible southern edge for the nation",
      NI_GODSEE | NI_GOD | NI_BOUNDARY,
      0, 0, 1, ni_inatt, ni_outatt }
};

/* declaration of keybindings at bottom */
extern KBIND_STRUCT ni_klist[];
extern PARSE_STRUCT ni_funcs[];
KLIST_PTR ni_bindings = NULL;

/* information structure concerning movement keybindings */
KEYSYS_STRUCT ninfo_keysys = {
  "info", ni_funcs, ni_klist, 0, 0
};
static NTN_STRUCT ni_backupvals;

/* NI_ATTRUNIQ -- Treat this attribute uniquely */
static int
ni_attruniq PARM_1(int, atval)
{
  int hold = FALSE;

  switch(atval) {
  case BUTE_CHARITY:
  case BUTE_CURRENCY:
  case BUTE_TAXRATE:
  case BUTE_INFLATION:
    hold = TRUE;
    break;
  default:
    /* don't do nothin' to 'em */
    break;
  }
  return(hold);
}

/* NI_INIT -- initialize the variables contained within the screen */
static void
ni_init PARM_0(void)
{
  int i, count;

  /* first handle all of the attributes, which are in a row */
  count = 0;
  for (i = 0; i < BUTE_NUMBER; i++) {
    if (ni_attruniq(i) == FALSE) {
      /* it is in there */
      items[count + NIT_COMMRANGE].label = bute_info[i].name;
      items[count + NIT_COMMRANGE].desc = bute_info[i].description;
      items[count + NIT_COMMRANGE].minval = bute_info[i].min_base;
      items[count + NIT_COMMRANGE].maxval = bute_info[i].max_base;
      items[count + NIT_COMMRANGE].modifier = bute_info[i].divisor;
      items[count + NIT_COMMRANGE].it.p_short = &(ntn_ptr->attribute[i]);
      count++;
    }
  }
  items[NIT_CHARITY].it.p_short = &(ntn_ptr->attribute[BUTE_CHARITY]);
  items[NIT_CHARITY].label = bute_info[BUTE_CHARITY].name;
  items[NIT_CHARITY].desc = bute_info[BUTE_CHARITY].description;
  items[NIT_CHARITY].minval = bute_info[BUTE_CHARITY].min_base;
  items[NIT_CHARITY].maxval = bute_info[BUTE_CHARITY].max_base;
  items[NIT_CHARITY].modifier = bute_info[BUTE_CHARITY].divisor;
  items[NIT_CURRENCY].it.p_short = &(ntn_ptr->attribute[BUTE_CURRENCY]);
  items[NIT_CURRENCY].label = bute_info[BUTE_CURRENCY].name;
  items[NIT_CURRENCY].desc = bute_info[BUTE_CURRENCY].description;
  items[NIT_CURRENCY].minval = bute_info[BUTE_CURRENCY].min_base;
  items[NIT_CURRENCY].maxval = bute_info[BUTE_CURRENCY].max_base;
  items[NIT_CURRENCY].modifier = bute_info[BUTE_CURRENCY].divisor;
  items[NIT_TAXRATE].it.p_short = &(ntn_ptr->attribute[BUTE_TAXRATE]);
  items[NIT_TAXRATE].label = bute_info[BUTE_TAXRATE].name;
  items[NIT_TAXRATE].desc = bute_info[BUTE_TAXRATE].description;
  items[NIT_TAXRATE].minval = bute_info[BUTE_TAXRATE].min_base;
  items[NIT_TAXRATE].maxval = bute_info[BUTE_TAXRATE].max_base;
  items[NIT_TAXRATE].modifier = bute_info[BUTE_TAXRATE].divisor;
  items[NIT_INFLATION].it.p_short = &(ntn_ptr->attribute[BUTE_INFLATION]);
  items[NIT_INFLATION].label = bute_info[BUTE_INFLATION].name;
  items[NIT_INFLATION].desc = bute_info[BUTE_INFLATION].description;
  items[NIT_INFLATION].minval = bute_info[BUTE_INFLATION].min_base;
  items[NIT_INFLATION].maxval = bute_info[BUTE_INFLATION].max_base;
  items[NIT_INFLATION].modifier = bute_info[BUTE_INFLATION].divisor;

  /* now handle the nation identities */
  items[NIT_NATION].it.p_string = &(ntn_ptr->name[0]);
  items[NIT_LOGIN].it.p_string = &(ntn_ptr->login[0]);
  items[NIT_LEADER].it.p_string = &(ntn_ptr->leader[0]);
  items[NIT_MARK].it.p_string = &(ntn_ptr->mark);
  items[NIT_RACE].it.p_short = &(ntn_ptr->race);
  items[NIT_CLASS].it.p_short = &(ntn_ptr->class);

  /* handle the activity values */
  items[NIT_ALIGN].it.p_short = &(ntn_ptr->active);
  items[NIT_AGGRESS].it.p_short = &(ntn_ptr->active);
  items[NIT_NPCSTAT].it.p_short = &(ntn_ptr->active);

  /* now for the nation bonuses */
  items[NIT_ATTACK].it.p_short = &(ntn_ptr->aplus);
  items[NIT_DEFENSE].it.p_short = &(ntn_ptr->dplus);
  items[NIT_REPRO].it.p_short = &(ntn_ptr->repro);
  items[NIT_MOVE].it.p_short = &(ntn_ptr->maxmove);

  /* now grab up the nation materials */
  items[NIT_TALONS].it.p_itemv = &(ntn_ptr->mtrls[MTRLS_TALONS]);
  items[NIT_JEWELS].it.p_itemv = &(ntn_ptr->mtrls[MTRLS_JEWELS]);
  items[NIT_METALS].it.p_itemv = &(ntn_ptr->mtrls[MTRLS_METALS]);
  items[NIT_WOOD].it.p_itemv = &(ntn_ptr->mtrls[MTRLS_WOOD]);
  items[NIT_FOOD].it.p_itemv = &(ntn_ptr->mtrls[MTRLS_FOOD]);

  /* grab up the military information */
  items[NIT_LEADERS].it.p_longint = &(ntn_ptr->tleaders);
  items[NIT_CIVILIANS].it.p_longint = &(ntn_ptr->tciv);
  items[NIT_SOLDIERS].it.p_longint = &(ntn_ptr->tmil);
  items[NIT_MONSTERS].it.p_longint = &(ntn_ptr->tmonst);
  items[NIT_SHIPS].it.p_short = &(ntn_ptr->tships);
  items[NIT_WAGONS].it.p_short = &(ntn_ptr->twagons);
  items[NIT_SECTORS].it.p_short = &(ntn_ptr->tsctrs);
  items[NIT_SCORE].it.p_longint = &(ntn_ptr->score);

  /* and finally the coordinates */
  items[NIT_LEFTEDGE].it.p_short = &(ntn_ptr->leftedge);
  items[NIT_RIGHTEDGE].it.p_short = &(ntn_ptr->rightedge);
  items[NIT_TOPEDGE].it.p_short = &(ntn_ptr->topedge);
  items[NIT_BOTTOMEDGE].it.p_short = &(ntn_ptr->bottomedge);
}

/* ni_setup_screen initializes the screen structure */
static int
ni_setup_screen PARM_0(void)
{
  int i, curline, curcol, linecount, itemcount;
  int numlines, sections, maxsect, diffval;

  /* initialize */
  numitems = numlines = 0;
  sections = maxsect = 0;
  ni_screen = NULL;
  items[NIT_CLASS].maxval = nclass_number - 1;

  /* count all of the lines and items */
  for (i = 0; i < NIT_NUMBER; i++) {
    if (NI_ICANSEE(i)) {
      if (items[i].stats & NI_EMPTY) {
	if (items[i].stats & NI_BOLD) {
	  numlines += 2;
	  numitems++;
	  sections++;
	  if (items[i].minval > maxsect) {
	    maxsect = items[i].minval + 1;
	  }
	} else {
	  numlines++;
	}
      } else {
	numlines++;
	numitems++;
      }
    }
  }

  /* find the default number of columns */
  numcolumns = max(3, COLS / 40);
  while (NI_MAXHEIGHT * numcolumns < numlines)
    numcolumns++;

  /* will now need all of the memory cells */  
  if ((ni_screen = (NI_SCRPTR) malloc(numitems * sizeof(NI_SCREEN))) == NULL) {
    errormsg("I could not allocate the right amount of memory...GOODBYE!");
    abrt();
  }

 reposition_screen:
  /* determine the width of the columns */
  colwidth = (COLS / numcolumns) - NI_SPACING;
  if (colwidth < 18) {
    errormsg("I can't possibly fit everything on a screen that small");
    return(TRUE);
  }

  /* calculate the total number of lines available */
  numlines = numcolumns * NI_MAXHEIGHT;

  /* initialize stuff */
  curline = 0;
  curcol = 0;
  linecount = 0;
  itemcount = 0;

  /* locate and position the items */
  for (i = 0; i < NIT_NUMBER; i++) {

    /* is it on the screen? */
    if (NI_ICANSEE(i)) {

      /* check if it is not a real element */
      if (items[i].stats & NI_EMPTY) {

	/* if it is a title, align everything under it */
	if (items[i].stats & NI_BOLD) {

	  /* make a blank line before it */
	  if (curline > 0) {
	    curline++;
	    linecount++;
	  }

	  /* check it out */
	  if (linecount > 0) {

	    /* check for enough space below it */
	    diffval = NI_MAXHEIGHT - curline;

	    /* check if the headers can be shifted */
	    if (numlines - linecount - diffval >
		numitems - itemcount) {
	      linecount += diffval;
	      curline = 0;
	      curcol++;
	    } else if (COLS / (numcolumns + 1) - NI_SPACING > 18) {
	      numcolumns++;
	      goto reposition_screen;
	    }

	  }

	} else {

	  /* just leave an empty line */
	  goto setup_next_line;

	}

      }

      /* now give it a place */
      ni_screen[itemcount].line = curline + NI_TITLE;
      ni_screen[itemcount].item = &(items[i]);
      ni_screen[itemcount].col = (curcol *
				  (colwidth + NI_SPACING)) + NI_INDENT;
      itemcount++;

    setup_next_line:
      /* increment the line count */
      linecount++;
      if (++curline > NI_MAXHEIGHT - 1) {
	curline = 0;
	curcol++;
      }
      if (linecount > numlines) {
	numcolumns++;
	goto reposition_screen;
      }

    }

  }

  /* all done */
  return(FALSE);
}

/* NI_SHOWITEM -- Show the item on the screen */
static void
ni_showitem PARM_1(int, indx)
{
  int high_on = FALSE, len, count;
  NI_SCRPTR old_current = current;

  /* check the input */
  if ((indx < 0) ||
      (indx >= numitems) ||
      (ni_screen[indx].item == NULL)) {
    return;
  }

  /* check if the highlight needs to be on */
  if (((indx == currnum) ? NI_BOLD : 0) ^
      (ni_screen[indx].item->stats & NI_BOLD)) {
    high_on = TRUE;    
    standout();
  }

  /* assign the "current" pointer */
  current = &(ni_screen[indx]);

  /* are we working with a column heading */
  if ((ni_screen[indx].item->stats & (NI_BOLD | NI_EMPTY)) ==
      (NI_BOLD | NI_EMPTY)) {

    /* build the heading */
    (*(ni_screen[indx].item->out_func))();
    len = strlen(string);
    if (len > 0) {
      mvaddstr(ni_screen[indx].line, ni_screen[indx].col +
	       (colwidth - len) / 2, string);
    }

  } else {

    /* first position the label */
    mvaddstr(ni_screen[indx].line, ni_screen[indx].col,
	     ni_screen[indx].item->label);
    len = colwidth - strlen(ni_screen[indx].item->label);

    /* get the data element */
    (*(ni_screen[indx].item->out_func))();
    len -= strlen(string);

    /* add appropriate number of periods */
    for (count = 0; count < len; count++) {
      addch('.');
    }

    /* now place the string */
    addstr(string);

  }

  /* all done... turn off if on */
  if (high_on == TRUE) {
    standend();
  }
  current = old_current;
}

/* write the bottom message */
static void
ni_bottom PARM_0(void)
{
  standout();
  if (god_browsing == TRUE) {
    mvaddstr(LINES - 3, COLS / 2 - 23,
	     "Hit 'Q' to leave the nation information screen");
  } else {
    mvaddstr(LINES - 3, COLS / 2 - 30,
	     "Hit 'Q' to leave the screen; Hit 'X' to change the password");
  }
  if ((is_god == TRUE) &&
      (god_browsing == FALSE)) {
    mvaddstr(LINES - 2, COLS / 2 - 30,
	     "Hit 'D' to delete nation; 'C' to change item; '?' for help");
  } else if (is_god == TRUE) {
    mvaddstr(LINES - 2, COLS / 2 - 29,
	     "Hit 'I' for information and '?' to get help for commands");
  } else {
    mvaddstr(LINES - 2, COLS / 2 - 31,
	     "Hit 'I' for information, 'C' to change item and '?' for help");
  }
  standend();
}

/* NI_SHOW -- redraw the entire display, clear if needed */
static void
ni_show PARM_0(void)
{
  char *title = "National Information Screen";
  int i;

  /* check the redraw setting */
  if (redraw == DRAW_FULL) {
    redraw = DRAW_DONE;
    clear();
  } else {
    move(0, 0);
    clrtobot();
  }

  /* show the screen title */
  standout();
  mvaddstr(0, (COLS - strlen(title)) / 2, title);
  standend();

  /* show the screen */
  for (i = 0; i < numitems; i++) {
    ni_showitem(i);
  }
  ni_bottom();
}

/* NI_SETCURSOR -- move the cursor to the end of the current item */
static void
ni_setcursor PARM_0(void)
{
  if (current == NULL) return;
  move(current->line, current->col + colwidth - 1);
  refresh();
}

/* change an item */
static void
ni_change PARM_0(void)
{
  /* check input */
  if ((current == NULL) ||
      (current->item == NULL)) return;

  /* call the function for the input, if appropriate */
  if (!NI_EDITABLE(currnum)) {
    errormsg("That item may not be changed");
    return;
  }
  if (god_browsing == TRUE) {
    errormsg("You are just browsing, cut that out");
    return;
  }

  /* display the prompt */
  bottommsg(current->item->prompt);

  /* now just invoke the routine to change it */
  (*(current->item->inp_func))();
}

/* NI_RECORD -- make sure to record all that has changed */
static void
ni_record PARM_1(NTN_PTR,org)
{
  int i;

  if (strcmp(ntn_ptr->name,org->name)) XADJNAME;
  if (strcmp(ntn_ptr->login,org->login)) XADJLOGIN;
  if (strcmp(ntn_ptr->passwd,org->passwd)) XADJPASSWD;
  if (strcmp(ntn_ptr->leader,org->leader)) XADJLEADER;
  if (ntn_ptr->repro != org->repro) XADJREPRO;
  if (ntn_ptr->race != org->race) XADJRACE;
  if (ntn_ptr->mark != org->mark) XADJMARK;
  if (ntn_ptr->location != org->location) XADJLOC;
  if ((ntn_ptr->capx != org->capx) || (ntn_ptr->capy != org->capy)) XADJLOC;
  if ((ntn_ptr->centerx != org->centerx) ||
      (ntn_ptr->centery != org->centery)) XADJRLOC;
  if (ntn_ptr->leftedge != org->leftedge) XADJLEDGE;
  if (ntn_ptr->rightedge != org->rightedge) XADJREDGE;
  if (ntn_ptr->topedge != org->topedge) XADJTEDGE;
  if (ntn_ptr->bottomedge != org->bottomedge) XADJBEDGE;
  if (ntn_ptr->class != org->class) XADJCLASS;
  if (ntn_ptr->aplus != org->aplus) XADJAPLUS;
  if (ntn_ptr->dplus != org->dplus) XADJDPLUS;
  if (ntn_ptr->score != org->score) XADJSCORE;
  if (ntn_ptr->active != org->active) {
    if (is_god == TRUE) {
      FILE *ftmp;
      sprintf(string, "god.%s", exetag);
      if ((ftmp = fopen(string, "a")) != NULL) {
	fprintf(ftmp, "X_ACTIVE\t%d\t%d\t%d\t0\tnull\tnull\n",
		EX_NTNACTIVE, country, (int)ntn_ptr->active);
	fclose(ftmp);
      }
    }
    XADJACT;
  }
  if (ntn_ptr->maxmove != org->maxmove) XADJMOVE;
  for (i = 0; i < BUTE_NUMBER; i++) {
    global_int = i;
    if (ntn_ptr->attribute[i] != org->attribute[i]) XADJBUTE;
  }
}

/* NI_EXIT -- set flag for leaving */
static int
ni_exit PARM_0(void)
{
  ni_doneflag = TRUE;
  return(0);
}

/* NI_ALIGN -- align the current point */
static void
ni_align PARM_0(void)
{
  if ((currnum >= 0) &&
      (currnum < numitems)) {
    current = &(ni_screen[currnum]);
  } else {
    current = NULL;
  }
}

/* NI_BACKWARD -- move upward among the items */
static int
ni_backward PARM_0(void)
{
  /* go up one... checking boundaries */
  do {
    if (currnum > 0) {
      currnum--;
    } else {
      currnum = numitems - 1;
    }
    ni_align();
  } while ((current->item->stats & (NI_BOLD | NI_EMPTY)) ==
	   (NI_BOLD | NI_EMPTY));

  return(0);
}

/* NI_FORWARD -- move forward among the items */
static int
ni_forward PARM_0(void)
{
  /* go down one... checking boundaries */
  do {
    if (currnum < numitems - 1) {
      currnum++;
    } else {
      currnum = 0;
    }
    ni_align();
  } while ((current->item->stats & (NI_BOLD | NI_EMPTY)) ==
	   (NI_BOLD | NI_EMPTY));
  return(0);
}

/* NI_UP -- move upward among the items */
static int
ni_up PARM_0(void)
{
  /* check input */
  if ((current == NULL) ||
      (current->item == NULL)) return(0);

  /* go up one... checking boundaries */
  if ((currnum > 0) &&
      (current->col == ni_screen[currnum - 1].col)) {

    /* go upward */
    currnum--;
    ni_align();

    if ((current->item->stats & (NI_BOLD | NI_EMPTY)) ==
	(NI_BOLD | NI_EMPTY)) {

      /* assume only 1 heading in a row */
      if (current->col == ni_screen[currnum - 1].col) {
	currnum--;
      } else {
	currnum++;
      }
      ni_align();

    }
  }
  return(0);
}

/* NI_DOWN -- move upward among the items */
static int
ni_down PARM_0(void)
{
  /* check input */
  if ((current == NULL) ||
      (current->item == NULL)) return(0);

  /* go down one... checking boundaries */
  if ((currnum < numitems) &&
      (current->col == ni_screen[currnum + 1].col)) {

    /* move down and align */
    currnum++;
    ni_align();

    if ((current->item->stats & (NI_BOLD | NI_EMPTY)) ==
	(NI_BOLD | NI_EMPTY)) {

      /* assume only 1 heading in a row */
      if (current->col == ni_screen[currnum + 1].col) {
	currnum++;
      } else {
	currnum--;
      }
      ni_align();

    }
  }
  return(0);
}

/* NI_LEFT -- move directly to the left */
static int
ni_left PARM_0(void)
{
  int target;

  /* check input */
  if ((current == NULL) ||
      (current->item == NULL)) return(0);

  /* set and find the target */
  target = current->line;
  do {
    currnum--;
    if (currnum < 0) {
      currnum = numitems - 1;
    }
    ni_align();
  } while ((target != current->line) ||
	   ((current->item->stats & (NI_BOLD | NI_EMPTY)) ==
	    (NI_BOLD | NI_EMPTY)));

  return(0);
}

/* NI_RIGHT -- move directly to the right */
static int
ni_right PARM_0(void)
{
  int target;

  /* check input */
  if ((current == NULL) ||
      (current->item == NULL)) return(0);

  /* set and find the target */
  target = current->line;
  do {
    currnum++;
    if (currnum == numitems) {
      currnum = 0;
    }
    ni_align();
  } while ((target != current->line) ||
	   ((current->item->stats & (NI_BOLD | NI_EMPTY)) ==
	    (NI_BOLD | NI_EMPTY)));

  return(0);
}

/* NI_RESET -- Reset the values to the original settings */
static int
ni_reset PARM_0(void)
{
  /* query them */
  bottommsg("Reset values back to their prior settings? ");
  if (y_or_n()) {
    /* hope this works */
    *ntn_ptr = ni_backupvals;
  }
  return(0);
}

/* NI_HELP -- Let the see the key binding description */
static int
ni_help PARM_0(void)
{
  /* create the help system */
  create_help("National Information Screen Command List",
              ni_bindings, ni_funcs, ninfo_keysys.num_parse);
  return(0);
}

/* NI_DESCRIPT -- Give a quick bit of information about the item */
static int
ni_descript PARM_0(void)
{
  /* if the current item is available, describe it */
  if ((current == NULL) ||
      (current->item == NULL)) return(0);

  /* show it at the bottom */
  bottommsg(current->item->desc);
  presskey();
  return(0);
}

/* NI_BCHANGE -- Change the item... cycle back if possible */
static int
ni_bchange PARM_0(void)
{
  /* change in a backward direction */
  if (god_browsing == FALSE) {
    ni_cycle_forward = FALSE;
    ni_change();
  }
  return(0);
}

/* NI_FCHANGE -- Change the item... cycle forward if possible */
static int
ni_fchange PARM_0(void)
{
  /* change in a forward direction */
  if (god_browsing == FALSE) {
    ni_cycle_forward = TRUE;
    ni_change();
  }
  return(0);
}

/* NI_DESTROY -- eliminate this nation from the game */
static int
ni_destroy PARM_0(void)
{
  /* god may remove the nation */
  if (god_browsing == TRUE) return(0);
  if (is_god == TRUE) {
    /* verify the deletion */
    bottommsg("Do you wish to remove this nation from the campaign? ");
    if (y_or_n() == TRUE) {
      ntn_ptr->active = INACTIVE;
    }
  }

  /* leave it */
  return(0);
}

/* NI_PASSWD -- Change the password for the nation */
static int
ni_passwd PARM_0(void)
{
  int len;
  char tmp_passwd[LINELTH];

  /* check the old password */
  if (is_god == FALSE) {

    /* get the old password */
    bottommsg("CHANGING PASSWORD:  What is the old passwd? ");
    if (get_pass(string) == -1) {
      return(0);
    }

    /* translate it */
#ifdef CRYPT
    strncpy(tmp_passwd, crypt(string, SALT), PASSLTH);
#else
    strncpy(tmp_passwd, string, PASSLTH);
#endif /*CRYPT*/
    tmp_passwd[PASSLTH] = '\0';

    /* was it the old password? */
    if (strncmp(tmp_passwd, ntn_ptr->passwd, PASSLTH) != 0) {
      errormsg("Sorry, but your password did not match");
      return(0);
    }
  }

  /* now get the new password */
  bottommsg("CHANGING PASSWORD:  Enter a new passwd? ");
  if ((len = get_pass(tmp_passwd)) <= 0) {
    return(0);
  }

  /* check the length */
  if (len < 2) {
    errormsg("Sorry, that password was too short");
  }

  /* now get verifications */
  bottommsg("CHANGING PASSWORD:  Verify the new passwd? ");
  if (get_pass(string) <= 0) {
    return(0);
  }

  /* compare, then store */
  if (strcmp(string, tmp_passwd) != 0) {
    errormsg("Sorry, that does not match your first entry");
    return(0);
  }

  /* now change it */
#ifdef CRYPT
  strncpy(ntn_ptr->passwd, crypt(tmp_passwd, SALT), PASSLTH);
#else
  strncpy(ntn_ptr->passwd, string, PASSLTH);
#endif /*CRYPT*/
  ntn_ptr->passwd[PASSLTH] = '\0';
  errormsg("Changed... the new password will be in place after the update");
  return(0);
}
    
/* NTN_INFO - this is the nation information screen routine */
int
ntn_info PARM_0(void)
{
  FNCI fnc_ptr;

  /* configure the setup */
  if (is_god == TRUE) {
    if (get_god(-1, FALSE)) return(0);
    if (ntn_ptr == NULL) {
      reset_god();
      return(0);
    }
  }
  ntn_totals(country);

  /* Create a backup */
  ni_backupvals = *ntn_ptr;

  /* can it be fit on the screen? */
  if (ni_setup_screen() == TRUE) {
    if (is_god == TRUE) {
      reset_god();
    }
    return(0);
  }
  redraw = DRAW_FULL;

  /* initialize stuff */
  ni_doneflag = FALSE;
  ni_init();

  /* find the nation name */
  for (currnum = 0; currnum < numitems; currnum++) {
    ni_align();
    if (current->item->ident == NIT_NATION) break;
  }
  if (currnum == numitems) {
    /* just start at the beginning no matter what */
    currnum = 0;
    ni_align();
  }

  /* keep going until done */
  while (ni_doneflag == FALSE) {

    /* show the screen */
    ni_show();
    ni_setcursor();

    /* now get the input */
    if ((fnc_ptr = parse_keys(ni_bindings, FALSE)) == NULL) {

      /* not valid */
      clear_bottom(1);
      mvaddstr(LINES - 1, 0, "Unknown key binding: ");
      show_str(string, FALSE);
      presskey();

    } else {

      /* perform the function */
      (*fnc_ptr)();

    }

  }
  ni_record(&ni_backupvals);
  if (is_god == TRUE) reset_god();
  redraw = DRAW_FULL;
  return(MOVECOST / 2);
}

/* NI_OPTIONS -- Quickie command to allow the setting of options */
static int
ni_options PARM_0(void)
{
  option_cmd(ninfo_keysys, &ni_bindings);
  return(0);
}

/* function list */
PARSE_STRUCT ni_funcs[] = {
  {ni_options, "conquer-options",
     "Adjust the conquer environment by changing various options"},
  {do_ignore, "ignore-key",
     "Don't do anything when this keybinding is pressed"},
  {ni_backward, "info-backward",
     "Info-mode, Move backward among the list of items on the screen"},
  {ni_fchange, "info-change",
     "Info-mode, change the currently selection item"},
  {ni_bchange, "info-change-backward",
     "Info-mode, change the current item, by backward motion, if applicable"},
  {ni_fchange, "info-change-forward",
     "Info-mode, change the current item, by forward motion, if applicable"},
  {ni_descript, "info-description",
     "Info-mode, Provide a description of the current item"},
  {ni_destroy, "info-destroy",
     "Info-mode, god may remove the nation using this command"},
  {ni_down, "info-down",
     "Info-mode, Move down along the list of items on the screen"},
  {ni_exit, "info-exit",
     "Info-mode, Leave the national information screen"},
  {ni_forward, "info-forward",
     "Info-mode, Move forward among the list of items on the screen"},
  {ni_help, "info-help",
     "Info-mode, List all of the commands and bindings in this mode"},
  {ni_left, "info-left",
     "Info-mode, move to the item to the left of the current item"},
  {ni_passwd, "info-passwd",
     "Info-mode, change the national password using this command"},
  {ni_reset, "info-reset",
     "Info-mode, reset the nation values back to their original settings"},
  {ni_right, "info-right",
     "Info-mode, move to the item to the right of the current item"},
  {ni_up, "info-up",
     "Info-mode, Move upwards along the list of items on the screen"},
  {do_refresh, "redraw-screen",
     "Redraw the screen without clearing it first"}
};

/* default key bindings */
KBIND_STRUCT ni_klist[] = {
  { ESTR_DEL, ni_backward },
  { CSTR_B, ni_left },
  { CSTR_F, ni_right },
  { CSTR_L, do_refresh },
  { CSTR_N, ni_down },
  { CSTR_P, ni_up },
  { CSTR_R, do_refresh },
  { AKEY_UP1, ni_up },
  { AKEY_UP2, ni_up },
  { AKEY_DOWN1, ni_down },
  { AKEY_DOWN2, ni_down },
  { AKEY_LEFT1, ni_left },
  { AKEY_LEFT2, ni_left },
  { AKEY_RIGHT1, ni_right },
  { AKEY_RIGHT2, ni_right },
  { "", do_ignore },
  { "\b", ni_backward },
  { "\n", ni_forward },
  { "\t", ni_forward },
  { "\r", ni_forward },
  { " ", ni_forward },
  { "?", ni_help },
  { "B", ni_bchange },
  { "b", ni_bchange },
  { "C", ni_fchange },
  { "c", ni_fchange },
  { "D", ni_destroy },
  { "d", ni_destroy },
  { "H", ni_left },
  { "h", ni_left },
  { "I", ni_descript },
  { "i", ni_descript },
  { "J", ni_down },
  { "j", ni_down },
  { "K", ni_up },
  { "k", ni_up },
  { "L", ni_right },
  { "l", ni_right },
  { "N", ni_fchange },
  { "n", ni_fchange },
  { "O", ni_options },
  { "o", ni_options },
  { "Q", ni_exit },
  { "q", ni_exit },
  { "R", ni_reset },
  { "r", ni_reset },
  { "X", ni_passwd },
  { "x", ni_passwd }
};

/* ALIGN_NINFO_KEYS -- Align all of the nation information mode keys */
void
align_ninfo_keys PARM_0(void)
{
  /* initialize the system */
  if (ni_bindings == NULL) {
    ninfo_keysys.num_binds = (sizeof(ni_klist)/sizeof(KBIND_STRUCT));
    ninfo_keysys.num_parse = (sizeof(ni_funcs)/sizeof(PARSE_STRUCT));
    init_keys(&ni_bindings, ni_klist, ninfo_keysys.num_binds);
  }
}
