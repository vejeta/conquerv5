// SPDX-License-Identifier: GPL-3.0-or-later
/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/* This include file defines all of the spells */
#ifndef SPL_STR_LIM

/* Structure to hold all of the spell information */
typedef struct s_spells {
  char *name;		/* the name of the spell */
  char *prompt;		/* the prompt to select the spell */
  char *descript;	/* description of the the spell */
  char *good_str;	/* description given upon success of spell */
  char *bad_str;	/* description given upon failure of spell */
  int keypos;		/* the letter in the prompt to cast the spell */
  char class;		/* the class of the spell */
  long info;		/* additional information about the spell */
  int cost;		/* cost to cast the spell */
  int drain;		/* strength drain of the spell */
  int rate;		/* percent change of a successful spell casting */
  long pow_need[MAG_NUMBER];	/* necessary powers */
} SPELL_STRUCT, *SPELL_PTR;

/* Spell list -- data in datamagX.c */
typedef enum spelltype {
  SPL_COMBAT, SPL_FLIGHT, SPL_HEAL, SPL_QUAKE, SPL_SENDING, SPL_SCARE,
  SPL_SUMMON, SPL_TPORT, SPL_TRANSMUTE, SPL_TRANSFER
} Spelltype;

/* spell information bits */
#define SI_ANYCAST	0x00000001L	/* any unit with spell pts may cast */
#define SI_SPELLCASTER	0x00000002L	/* must be a spell caster to cast */
#define SI_FULLCASTER	0x00000004L	/* must be a full fledged magician */
#define SI_NONMONSTER	0x00000008L	/* monsters may not use this spell */

/* the checks */
#define si_anycaster(x)	(spell_list[x].info & SI_ANYCAST)
#define si_needcaster(x)	(spell_list[x].info & SI_SPELLCASTER)
#define si_needfull(x)	(spell_list[x].info & SI_FULLCASTER)
#define si_nonmonster(x)	(spell_list[x].info & SI_NONMONSTER)

/* The strength limitations */
#define SPL_STR_LIM	50	/* the minimum strength for spell casting */
#define SPL_STR_MIN	20	/* the minimum strength after casting */

/* the spell information structure */
extern SPELL_STRUCT spell_list[];
extern int spell_number;

#endif /* SPL_STR_LIM */
