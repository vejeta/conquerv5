// SPDX-License-Identifier: GPL-3.0-or-later
/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/* This file defines all of the nation activity status information */

/* Activity Values */
#define ACT_NOMOVE	0
#define ACT_STATIC	1
#define ACT_ENFORCE	2
#define ACT_OVERT	3
#define ACT_MOBILE	4
#define ACT_KILLER	5
#define ACT_NUMBER	6
#define ACT_PCMODULUS	24
#define ACT_MAXIMUM	47

/* Alignment values */
#define ALIGN_GOOD	1
#define ALIGN_NEUTRAL	2
#define ALIGN_EVIL	3

/* Nation Activity Values */
#define INACTIVE	0
#define DEAD_LIZARD	4
#define DEAD_PEASANT	8
#define DEAD_SAVAGE	12
#define DEAD_NOMAD	16
#define DEAD_PIRATE	20
#define NPC_INACTIVE	24
#define NPC_LIZARD	28
#define NPC_PEASANT	32
#define NPC_SAVAGE	36
#define NPC_NOMAD	40
#define NPC_PIRATE	44

/* Nation Determination Macros */
#define n_alignment(x)	((x) % 4)
#define n_aggression(x)	(((x) % ACT_PCMODULUS) / 4)
#define n_isactive(x)	((x) != INACTIVE)
#define n_notactive(x)	((x) == INACTIVE)
#define n_ismonster(x)	(n_alignment(x) == INACTIVE)
#define n_islizard(x)	(((x) == NPC_LIZARD) || ((x) == DEAD_LIZARD))
#define n_ispeasant(x)	(((x) == NPC_PEASANT) || ((x) == DEAD_PEASANT))
#define n_issavage(x)	(((x) == NPC_SAVAGE) || ((x) == DEAD_SAVAGE))
#define n_isnomad(x)	(((x) == NPC_NOMAD) || ((x) == DEAD_NOMAD))
#define n_ispirate(x)	(((x) == NPC_PIRATE) || ((x) == DEAD_PIRATE))
#define n_isntn(x)	(n_alignment(x) != INACTIVE)
#define n_ispc(x)	(((x) < ACT_PCMODULUS) && ((x) != INACTIVE))
#define n_isnpc(x)	(n_isntn(x) && ((x) > ACT_PCMODULUS))
#define n_isgood(x)	(n_alignment(x) == ALIGN_GOOD)
#define n_isneutral(x)	(n_alignment(x) == ALIGN_NEUTRAL)
#define n_isevil(x)	(n_alignment(x) == ALIGN_EVIL)
#define n_automove(x)	((x) >= DEAD_LIZARD)
