// SPDX-License-Identifier: GPL-3.0-or-later
/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/* This include file defines the trade goods */
#include "butesX.h"

/* The tradegood classes information -- TG_CLASSES in dataX.h */
typedef enum tgctype {
  TG_NONE, TG_POPULARITY, TG_COMMUNICATE, TG_FISHING, TG_FARMING,
  TG_SPOILRATE, TG_LUMBER, TG_KNOWLEDGE, TG_EATRATE, TG_HEALTH,
  TG_TERROR, TG_SPELLS, TG_METALS, TG_JEWELS
} Tgctype;

/* The structure for tradegood information */
typedef struct s_tgood {
  char *name;		/* the tradegood name */
  int value;		/* the value of the tradegood */
  int rate;		/* relative frequency within class of items */
  Tgctype class;	/* the class of the trade good */
  uns_short need_desg;	/* the necessary major designation */
  uns_short pop_support;/* the number of civilians needed */
} TGOOD_STRUCT, *TGOOD_PTR;

/* Quick information structure for the trade good classes */
typedef struct s_tgclass {
  char *name;		/* The name of the tradegood class */
  char *description;	/* A description of the what the class is */
  char *affect;		/* What affect it has on nation information */
  Butetype attribute;	/* which national attribute does it affect */
  int weighting;	/* which national attribute does it affect */
} TGCLASS_STRUCT, *TGCLASS_PTR;

/* some quickie checks */
#define tg_popular(x)	(tg_info[x].class == TG_EATRATE)
#define tg_communicate(x)	(tg_info[x].class == TG_COMMUNICATE)
#define tg_fishing(x)	(tg_info[x].class == TG_FISHING)
#define tg_farming(x)	(tg_info[x].class == TG_FARMING)
#define tg_unspoils(x)	(tg_info[x].class == TG_SPOILRATE)
#define tg_islumber(x)	(tg_info[x].class == TG_LUMBER)
#define tg_knowledge(x)	(tg_info[x].class == TG_KNOWLEDGE)
#define tg_eatrate(x)	(tg_info[x].class == TG_EATRATE)
#define tg_healthy(x)	(tg_info[x].class == TG_HEALTH)
#define tg_terror(x)	(tg_info[x].class == TG_TERROR)
#define tg_isspell(x)	(tg_info[x].class == TG_SPELLS)
#define tg_isjewel(x)	(tg_info[x].class == TG_JEWELS)
#define tg_ismetal(x)	(tg_info[x].class == TG_METALS)

/* the actual information location */
extern TGOOD_STRUCT tg_info[];
extern TGCLASS_STRUCT tgclass_info[];
