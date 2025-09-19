/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/* The status definitions for the military units */

/* Base Unit Statuses */
#define ST_SORTIE	0
#define ST_AMBUSH	1
#define ST_ATTACK	2
#define ST_ENGAGE	3
#define ST_DEFEND	4
#define ST_GARRISON	5
#define ST_ONBOARD	6
#define ST_SIEGE	7
#define ST_GROUPED	8
#define ST_SIEGED	9
#define ST_RESERVE	10
#define ST_SWEEP	11
#define ST_WORKCREW	12
#define ST_TRADED	13
#define ST_CARRY	14
#define ST_DECOY	15
#define ST_SUPPLY	16
#define ST_REPAIR	17
#define ST_ONBSPLY	18
#define ST_SSUPPLY	19
#define ST_ONBSSPLY	20
#define ST_ROVER	21

/* Extra Status Indicators */
#define SX_MAGIC	0x00100L
#define SX_LEADER	0x00200L
#define SX_FLIGHT	0x00400L
#define SX_HEALED	0x00800L
#define SX_SPEED	0x000C0L
#define SX_SHOWSTAT	0x0003FL
#define SX_SPDSLOT	6

/* Unit Speed */
#define SPD_SLOW	0
#define SPD_NORMAL	1
#define SPD_MARCH	2
#define SPD_STUCK	3
#define SPD_PATROL	4
#define SPD_MINIMUM	0
#define SPD_NUMBER	5

/* Status Macros */
#define set_speed(x,y)	x = ((x & ~SX_SPEED) | ((y) << SX_SPDSLOT))
#define set_status(x,y)	x = ((x & ~SX_SHOWSTAT) | (y))
#define set_leading(x)	x |= SX_LEADER
#define set_unlead(x)	x &= ~SX_LEADER
#define set_spelled(x)	x |= SX_MAGIC
#define set_unspell(x)	x &= ~SX_MAGIC
#define set_flight(x)	x |= SX_FLIGHT
#define set_unflight(x)	x &= ~SX_FLIGHT
#define set_healed(x)	x |= SX_HEALED
#define set_unhealed(x)	x &= ~SX_HEALED
#define unit_speed(x)	(((x) & SX_SPEED) >> SX_SPDSLOT)
#define unit_status(x)	((x) & SX_SHOWSTAT)
#define unit_leading(x)	((x) & SX_LEADER)
#define unit_spelled(x)	((x) & SX_MAGIC)
#define unit_flight(x)	((x) & SX_FLIGHT)
#define unit_healed(x)	((x) & SX_HEALED)
#define unit_built(x)	((x) & SX_BUILT)
#define unit_orig(x)	((x) & SX_ORIGSTAT)

/* The information structure of statuses */
typedef struct s_status {
  char *name;		/* the full name of the status type */
  char *nickname;	/* the short sidebar name of the status */
  char *input;		/* the prompt for entering the status */
  int key_char;		/* key character in the status name */
  long special;		/* holder for special treatment of status */
  int attackval;	/* the combat bonus gained during an attack */
  int defendval;	/* the combat bonus gained during defense */
} STATUS_STRUCT, *STATUS_PTR;

/* the list of special status handlers */
#define SPST_ATT	0x00000001L	/* will initiate combat if possible */
#define SPST_CAPT	0x00000002L	/* may capture land with this */
#define SPST_DIST	0x00000004L	/* distant combat possible */
#define SPST_INVERSE	0x00000008L	/* inverse ratio of distance */
#define SPST_HIDDEN	0x00000010L	/* status hides unit from view */
#define SPST_FORT	0x00000020L	/* uses defensive foritifications */
#define SPST_UNCOMB	0x00000040L	/* can't combine with unlike status */
#define SPST_NOLIKE	0x00000080L	/* nor with like statuses */
#define SPST_UNSWITCH	0x00000100L	/* cannot leave status */
#define SPST_UNSPLIT	0x00000200L	/* cannot split the unit */
#define SPST_UNMOVE	0x00000400L	/* cannot move the unit */
#define SPST_SIEGED	0x00000800L	/* status can be changed to seiged */
#define SPST_SORTIE	0x00001000L	/* status can be changed to sortie */
#define SPST_NOHOLD	0x00002000L	/* cannot protect territory */
#define SPST_HOLDIT	0x00004000L	/* doubly good at holding land */
#define SPST_GROUNDED	0x00008000L	/* gain terrain bonus on attack	*/
#define SPST_SUPPLY	0x00010000L	/* unit acts as a supply center	*/
#define SPST_DECOY	0x00020000L	/* unit will act as decoy */
#define SPST_SPLYINSECT	0x00040000L	/* unit must be in sector to supply */
#define SPST_DIESLAST	0x00080000L	/* unit is protected by others */
#define SPST_RISKY	0x00100000L	/* risky to attack */
#define SPST_USABLE	0x00200000L	/* this status can be set directly */
#define SPST_SWEEP	0x00400000L	/* only a partial attack in sector */
#define SPST_ONSHIP	0x00800000L	/* this unit is on a ship */
#define SPST_ITEMINFO	0x01000000L	/* unit is used in an item */
#define SPST_NOCAST	0x02000000L	/* no spells with this status */
#define SPST_ASTATUS	0x04000000L	/* an army unit status */
#define SPST_NSTATUS	0x08000000L	/* a navy unit status */
#define SPST_VSTATUS	0x10000000L	/* caravan unit status */
#define SPST_SSUPPLY	0x20000000L	/* also include supplying for self */
#define SPST_ROVER	0x40000000L	/* unit wanders about taking land */

/* macros to test the status types */
#define usable_stat(x)	(stat_info[unit_status(x)].special & SPST_USABLE)
#define risky_stat(x)	(stat_info[unit_status(x)].special & SPST_RISKY)
#define sweep_stat(x)	(stat_info[unit_status(x)].special & SPST_SWEEP)
#define attack_stat(x)	(stat_info[unit_status(x)].special & SPST_ATT)
#define capture_stat(x)	(stat_info[unit_status(x)].special & SPST_CAPT)
#define distant_stat(x)	(stat_info[unit_status(x)].special & SPST_DIST)
#define inverse_stat(x)	(stat_info[unit_status(x)].special & SPST_INVERSE)
#define hidden_stat(x)	(stat_info[unit_status(x)].special & SPST_HIDDEN)
#define fort_stat(x)	(stat_info[unit_status(x)].special & SPST_FORT)
#define nocomb_stat(x)	(stat_info[unit_status(x)].special & SPST_UNCOMB)
#define nolike_stat(x)	(stat_info[unit_status(x)].special & SPST_NOLIKE)
#define nochange_stat(x)	(stat_info[unit_status(x)].special & SPST_UNSWITCH)
#define nosplit_stat(x)	(stat_info[unit_status(x)].special & SPST_UNSPLIT)
#define nomove_stat(x)	(stat_info[unit_status(x)].special & SPST_UNMOVE)
#define siege_stat(x)	(stat_info[unit_status(x)].special & SPST_SIEGED)
#define sortie_stat(x)	(stat_info[unit_status(x)].special & SPST_SORTIE)
#define nohold_stat(x)	(stat_info[unit_status(x)].special & SPST_NOHOLD)
#define holdit_stat(x)	(stat_info[unit_status(x)].special & SPST_HOLDIT)
#define grounded_stat(x)	(stat_info[unit_status(x)].special & SPST_GROUNDED)
#define supply_stat(x)	(stat_info[unit_status(x)].special & (SPST_SUPPLY | SPST_SSUPPLY))
#define selfsply_stat(x)	(stat_info[unit_status(x)].special & SPST_SSUPPLY)
#define decoy_stat(x)	(stat_info[unit_status(x)].special & SPST_DECOY)
#define splyinsect_stat(x)	(stat_info[unit_status(x)].special & SPST_SPLYINSECT)
#define dieslast_stat(x)	(stat_info[unit_status(x)].special & SPST_DIESLAST)
#define onship_stat(x)	(stat_info[unit_status(x)].special & SPST_ONSHIP)
#define iteminfo_stat(x)	(stat_info[unit_status(x)].special & SPST_ITEMINFO)
#define nocast_stat(x)	(stat_info[unit_status(x)].special & SPST_NOCAST)
#define aunit_stat(x)	(stat_info[unit_status(x)].special & SPST_ASTATUS)
#define nunit_stat(x)	(stat_info[unit_status(x)].special & SPST_NSTATUS)
#define vunit_stat(x)	(stat_info[unit_status(x)].special & SPST_VSTATUS)
#define rover_stat(x)	(stat_info[unit_status(x)].special & SPST_ROVER)

/* the variables to hold the status information */
extern STATUS_STRUCT stat_info[];
extern int num_statuses;
