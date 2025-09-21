// SPDX-License-Identifier: GPL-3.0-or-later
/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/* This include file defines the magical powers */
#ifndef MAG_MILITARY
#include "butesX.h"

/* Magical Power Definitions */
/* military powers -- data in datamagX.c */
#define MM_ARCHERY	0x00000001L
#define MM_ARMOR	0x00000002L
#define MM_AVIAN	0x00000004L
#define MM_CAPTAIN	0x00000008L
#define MM_DRAGON	0x00000010L
#define MM_EQUINE	0x00000020L
#define MM_NINJA	0x00000040L
#define MM_OGRE		0x00000080L
#define MM_ORC		0x00000100L
#define MM_SAPPER	0x00000200L
#define MM_WARLORD	0x00000400L
#define MM_WARRIOR	0x00000800L

/* civilian powers -- data in datamagX.c */
#define MC_ACCOUNTANT	0x00000001L
#define MC_AMPHIBIAN	0x00000002L
#define MC_ARCHITECT	0x00000004L
#define MC_BOTANY	0x00000008L
#define MC_BREEDER	0x00000010L
#define MC_DEMOCRACY	0x00000020L
#define MC_DERVISH	0x00000040L
#define MC_FARMING	0x00000080L
#define MC_JEWELER	0x00000100L
#define MC_MARINE	0x00000200L
#define MC_METALCRAFT	0x00000400L
#define MC_MINER	0x00000800L
#define MC_RELIGION	0x00001000L
#define MC_ROADS	0x00002000L
#define MC_SAILOR	0x00004000L
#define MC_SLAVER	0x00008000L
#define MC_SOCIALISM	0x00010000L
#define MC_URBAN	0x00020000L
#define MC_WOODCRAFT	0x00040000L

/* wizardry powers -- data in datamagX.c */
#define MW_ALCHEMY	0x00000001L
#define MW_AIR		0x00000002L
#define MW_DESTROYER	0x00000004L
#define MW_DRUIDISM	0x00000008L
#define MW_EARTH	0x00000010L
#define MW_FIRE		0x00000020L
#define MW_HIDDEN	0x00000040L
#define MW_ILLUSION	0x00000080L
#define MW_KNOWALL	0x00000100L
#define MW_SEEALL	0x00000200L
#define MW_SENDING	0x00000400L
#define MW_SORCERER	0x00000800L
#define MW_SUMMON	0x00001000L
#define MW_THEVOID	0x00002000L
#define MW_VAMPIRE	0x00004000L
#define MW_VISION	0x00008000L
#define MW_WATER	0x00010000L
#define MW_WEATHER	0x00020000L
#define MW_WYZARD	0x00040000L

/* Magic Class Definitions; MAG_NUMBER defined in dataX.h */
#define MAG_MILITARY	0
#define MAG_CIVILIAN	1
#define MAG_WIZARDRY	2

/* Magic Determination Macros */
#define MAGIC(x,y)	(((x)&(y)) == (y))
#define ADDMAGIC(x,y)	x |= (y)
#define KILLMAGIC(x,y)	x &= ~(y)
#define MIL_MAGIC(x)	MAGIC(ntn_ptr->powers[MAG_MILITARY], x)
#define CIV_MAGIC(x)	MAGIC(ntn_ptr->powers[MAG_CIVILIAN], x)
#define WIZ_MAGIC(x)	MAGIC(ntn_ptr->powers[MAG_WIZARDRY], x)

/* A Magical Power Structure */
typedef struct s_magic {
  char *name;		/* the string identifier for the magic power */
  char *descript;	/* the description of what the power is */
  char *info;		/* a description of what the power does */
  Butetype bute_num;	/* national attribute affected by the power */
  int bute_val;		/* amount of adjustment given to attribute */
  long stats;		/* extra statistics of the power [used?] */
  long pow_need[MAG_NUMBER];	/* the list of prerequisite powers */
} MAGIC_STRUCT, *MAGIC_PTR;

/* The magical class structure */
typedef struct s_mclass {
  char *name;		/* the name of the magical class */
  int maxval;		/* the maximum number of elements in the class */
  MAGIC_PTR pow_list;	/* the list of powers in that class */
} MCLASS_STRUCT, *MCLASS_PTR;

/* The codes for the magical power adjustments */
#ifdef USE_CODES

#include "executeX.h"

/* now adjust the magic information powers */
#define EX_MGKBEGIN	EX_MGK_ADJ
#define EX_MGKEND	EX_MGKSENDING

#ifdef PRINT_CODES
/* finally, store the magic commands */
#define MGKADJ	fprintf(fexe, "L_MGKADJ\t%d\t%d\t%ld\t%ld\tnull\tnull\n", EX_MGK_ADJ, global_int, ntn_ptr->powers[global_int], global_long)
#define MGKSPELL	fprintf(fexe, "X_MGKSPELL\t%d\t%d\t%d\t%d\tnull\tnull\n", EX_MGKSPELL, spellnum, XREAL, YREAL)
#define MGKSEND	fprintf(fexe, "X_MGKSEND\t%d\t%d\t%d\t0\tnull\tnull\n", EX_MGKSENDING, monst_val, ntn_num)
#endif /* PRINT_CODES */

#endif /* USE_CODES */

/* Magical data definitions */
extern MAGIC_STRUCT milpow_list[], civpow_list[], wizpow_list[];
extern MCLASS_STRUCT mclass_list[MAG_NUMBER];

#endif /* MAG_MILITARY */
