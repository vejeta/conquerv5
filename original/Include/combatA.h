/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/* This include file defines the combat relative items */
#include "dstatusX.h"

/* Maxiumum number of sides in a battle */
#define MAX_COMBAT	15	/* can change w/out invalidating data */

/* Combat unit settings */
typedef enum ucombattype {
  UNIT_NONE, UNIT_ARMY, UNIT_NAVY, UNIT_CVN
} Ucombattype;

/* Combat usage values */
#ifdef NOTUSED
#define CUSE_NONE	0
#define CUSE_FULL	100
#define CUSE_SKIP	101
#define CUSE_FINISHED	102
#endif /* NOTUSED */

/* Sector Flags */
#define CBSCT_NONE	0
#define CBSCT_BATTLE	1

/* Groupings within a battle */
#define CGRP_NUMBER	5
typedef enum cgrptype {
  CGRP_SWEEPER, CGRP_ATTACKER, CGRP_DEFENDER, CGRP_FORTIFIED,
  CGRP_PROTECTED
} Cgrptype;

/* Combat unit information */
typedef struct s_cunit {
  ntntype owner;	/* owner of the unit in combat		*/
  Ucombattype type;	/* army, navy or caravan indicator	*/
  uns_char usage;	/* how much the unit has been used	*/
  uns_char damage;	/* how much the unit has been damaged	*/
  long rel_size;	/* relative size of the unit		*/
  int adjustment;	/* bonus level of the unit		*/
  int destruct_value;	/* potential for destroying fortif..	*/
  union {
    ARMY_PTR army_p;	/* pointer to any real army unit	*/
    NAVY_PTR navy_p;	/* pointer to any real navy unit	*/
    CVN_PTR cvn_p;	/* pointer to any real caravan unit	*/
  } ui;
  struct s_cunit *next;
} CUNIT_STRUCT, *CUNIT_PTR;

/* Combatant Structure */
typedef struct s_cside {
  ntntype owner;		/* which nation controls this combatant	*/
  int attack_cover;		/* cover bonuses due to special troops	*/
  int fort_cover;		/* cover bonuses due to special troops	*/
  int has_spys;			/* indicates that spys can see battle	*/
  long zombie_mkr[CGRP_NUMBER];	/* how many zomies can be created	*/
  long sum_size[CGRP_NUMBER];	/* total strength of all units		*/
  double avg_bonus[CGRP_NUMBER];	/* avg bonus level of the units	*/
  Diplotype rstat[CGRP_DEFENDER][MAX_COMBAT];	/* attitude for others	*/
  CUNIT_PTR units[CGRP_NUMBER];	/* the actual units within the combat	*/
} CSIDE_STRUCT, *CSIDE_PTR;

/* Distance Attack Listings */
typedef struct s_cdist {
  int xloc, yloc;		/* the position of the unit */
  CUNIT_PTR unit;		/* the unit under consideration */
  struct s_cdist *next;		/* the next distance element */
} CDIST_STRUCT, *CDIST_PTR;

/* Sector Combat Summaries */
typedef struct s_combat {
  maptype xloc, yloc;		/* the position of the sector in world	*/
  CSIDE_PTR cside[MAX_COMBAT];	/* the actual list of combatents	*/
  struct s_combat *next;	/* the next among those in the sector	*/
} COMBAT_STRUCT, *COMBAT_PTR;
