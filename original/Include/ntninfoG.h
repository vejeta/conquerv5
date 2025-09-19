/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/* Macros and definitions for the nation information screen */

/* some constant values */
#define NI_BOTTOM	3
#define NI_TITLE	2
#define NI_INDENT	1
#define NI_SPACING	1
#define NI_MAXHEIGHT	(LINES - NI_BOTTOM - NI_TITLE)

/* These are the possible values in the stats field */
#define NI_GOD		(1<<1)	/* Changeable by god */
#define NI_PLAYER	(1<<2)	/* Changeable by user */
#define NI_GODSEE	(1<<3)	/* Only god can see this item */
#define NI_PERCENT	(1<<4)	/* Trailing percent character */
#define NI_DOLLAR	(1<<5)	/* Dollar character preceding */
#define NI_BOLD		(1<<6)	/* makes this entry standout */
#define NI_EMPTY	(1<<7)	/* This is an empty line */
#define NI_NOSPACE	(1<<8)	/* If a string cannot support spaces */
#define NI_BONUS	(1<<9)	/* Special handling for the bonuses */
#define NI_BOUNDARY	(1<<10)	/* Special handling for the edges */

/* The list of elements */
#define NIT_BLANK	0
#define NIT_NATION	1
#define NIT_MARK	2
#define NIT_LEADER	3
#define NIT_RACE	4
#define NIT_ALIGN	5
#define NIT_CLASS	6
#define NIT_AGGRESS	7
#define NIT_NPCSTAT	8
#define NIT_LOGIN	9
/* blank */
#define NIT_TAXRATE	11
#define NIT_CHARITY	12
#define NIT_CURRENCY	13
#define NIT_INFLATION	14
/* attributes title */
#define NIT_COMMRANGE	16
#define NIT_EATRATE	17
#define NIT_HEALTH	18
#define NIT_JEWELWORK	19
#define NIT_KNOWLEDGE	20
#define NIT_MERCREP	21
#define NIT_METALWORK	22
#define NIT_MINING	23
#define NIT_MORALE	24
#define NIT_POPULARITY	25
#define NIT_REPUTATION	26
#define NIT_SPELLPTS	27
#define NIT_SPOILRATE	28
#define NIT_TERROR	29
#define NIT_WIZSKILL	30
/* nation information blank */
#define NIT_ATTACK	32
#define NIT_DEFENSE	33
#define NIT_REPRO	34
#define NIT_MOVE	35
/* commodities blank */
#define NIT_TALONS	37
#define NIT_JEWELS	38
#define NIT_METALS	39
#define NIT_FOOD	40
#define NIT_WOOD	41
/* mighty blank */
#define NIT_LEADERS	43
#define NIT_SOLDIERS	44
#define NIT_CIVILIANS	45
#define NIT_MONSTERS	46
#define NIT_SHIPS	47
#define NIT_WAGONS	48
#define NIT_SECTORS	49
#define NIT_SCORE	50
/* god title */
#define NIT_CAPCOORD	52
#define NIT_CENTERCOORD	53
#define NIT_LEFTEDGE	54
#define NIT_RIGHTEDGE	55
#define NIT_TOPEDGE	56
#define NIT_BOTTOMEDGE	57

/* the number of elements */
#define NIT_NUMBER	58

/* Macro which says if we can see this item */
#define NI_CANSEE(x) (is_god ? TRUE : !(ni_screen[x].item->stats & NI_GODSEE))

/* Another one, but this one takes an itemnumber as argument*/
#define NI_ICANSEE(x) (is_god ? TRUE : !(items[x].stats & NI_GODSEE))

/* Macro which indicates if an item is editable (also checks bounds) */
#define NI_EDITABLE(x)	(((x >= 0) && (x < numitems) && NI_CANSEE(x)) ? \
			 (is_god ? \
			  (ni_screen[x].item->stats & (NI_GOD | NI_PLAYER)):\
			  (ni_screen[x].item->stats & NI_PLAYER)) : FALSE)

/* Function pointer for the input and output functions */
#ifdef __STDC__
typedef void (*FNCV_NI)(void);
#else
typedef void (*FNCV_NI)();
#endif /* FNCV_NI */

/* union definition */
typedef union u_ndata {
  char *p_string;	/* entry is a character or string of characters */
  int *p_int;		/* entry is an integer */
  itemtype *p_itemv;	/* entry is an item value */
  short *p_short;	/* entry is a short value */
  long *p_longint;	/* entry is a long integer */
} NI_UNION, *NI_UNPTR;

/* This is the item structure */
typedef struct s_nitem {
  char *label;		/* Label to show on screen */
  int ident;		/* Identifier for this element */
  char *desc;		/* Description of item */
  char *prompt;		/* The message given when asking for input */
  char *emsg;		/* Message given on input error */
  long stats;		/* Statistics about the item */
  int minval;		/* Minimum length or value */
  int maxval;		/* Maximum length or setting */
  int modifier;		/* If appropriate, value for modifying */
  FNCV_NI inp_func;	/* The function to get the input */
  FNCV_NI out_func;	/* The function to generate the output */
  NI_UNION it;		/* The data element */
} NI_ITEM, *NI_ITEMPTR;

/* Structure to handle the actual screen lines */
typedef struct s_niscr {
  int col, line;	/* the position of the item on the screen */
  NI_ITEMPTR item;	/* the actual item this line represents */
} NI_SCREEN, *NI_SCRPTR;
