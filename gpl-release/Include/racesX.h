// SPDX-License-Identifier: GPL-3.0-or-later
/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/* This file lists definitions of the different races */

/* the list of racial types */
#define LIZARD		5
#define PIRATE		6
#define SAVAGE		7
#define NOMAD		8
#define TUNKNOWN	9
/* RACE_NUMBER defined in dataX.h */

/* the list of racial traits */
#define RT_WOODWINTER	0x00000001	/* race can survive winter in woods */
#define RT_MOUNTAINEER	0x00000002	/* race can better handle mountains */
#define RT_MAGICSKILL	0x00000004	/* race is proficient in magic */
#define RT_ANTIMAGIC	0x00000008	/* race shuns use of magic */
#define RT_MINING	0x00000010	/* race is good at mining materials */
#define RT_MONSTERSKILL	0x00000020	/* race is skilled with monsters */
#define RT_TRADERS	0x00000040	/* race is skilled in trading */
#define RT_ANTIMILITARY	0x00000080	/* race shuns use of military */
#define RT_WIZARDLY	0x00000100	/* race is very skilled in magic */
#define RT_FIGHTERS	0x00000200	/* race has superlative warriors */
#define RT_NOTRACE	0x00000400	/* not a player racial selection */

/* the macros to tell these traits */
#define r_woodwinter(x)	(race_info[x].traits & RT_WOODWINTER)
#define r_mountaineer(x)	(race_info[x].traits & RT_MOUNTAINEER)
#define r_magicskill(x)	(race_info[x].traits & RT_MAGICSKILL)
#define r_antimagic(x)	(race_info[x].traits & RT_ANTIMAGIC)
#define r_mining(x)	(race_info[x].traits & RT_MINING)
#define r_monsterly(x)	(race_info[x].traits & RT_MONSTERSKILL)
#define r_traders(x)	(race_info[x].traits & RT_TRADERS)
#define r_antiwar(x)	(race_info[x].traits & RT_ANTIMILITARY)
#define r_wizardly(x)	(race_info[x].traits & RT_WIZARDLY)
#define r_fighters(x)	(race_info[x].traits & RT_FIGHTERS)
#define r_notrace(x)	(race_info[x].traits & RT_NOTRACE)

/* data structure for the racial information */
typedef struct s_race {
  char *name;			/* name of the race */
  char *description;		/* the description of the race */
  long pow_start[MAG_NUMBER];	/* the starting powers of the race */
  long pow_limit[MAG_NUMBER];	/* powers the race cannot have */
  long take_value[RACE_NUMBER];	/* racial adjustments on sector capturing */
  long kill_pop[RACE_NUMBER];	/* what % pop is killed of other races */
  long keep_pop[RACE_NUMBER];	/* what % pop is grabbed of other races */
  int repro_limit;		/* the maximum reproduction potential */
  long traits;			/* the traits for each race */
  long cost_base[MAG_NUMBER];	/* the base expense for magical powers */
  int au_maxval[AU_NUMBER];	/* the maximum settings for each race */
  int au_start[AU_NUMBER];	/* the starting values for each race */
  int au_costs[AU_NUMBER];	/* the actual costs for each item */
  int au_units[AU_NUMBER];	/* the number of units for each point spent */
} RACE_STRUCT, *RACE_PTR;

/* the data structure */
extern RACE_STRUCT race_info[RACE_NUMBER];
