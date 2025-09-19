/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/* This include file handles the army data definitions */

/* Very Careful with this one; must be set to infantry value */
#define DEFAULT_ARMYTYPE	38

/* Careful; what is the type number of the default leader types */
#define DEFAULT_RULERTYPE	0
#define DEFAULT_LEADERTYPE	1

/* The list of different army classes -- strings in datamilX.c */
typedef enum aclasstype {
  AC_LEADER,		/* a leader unit */
  AC_SPELLCASTER,	/* one of the spell caster units */
  AC_MONSTER,		/* a monster unit */
  AC_NORMAL,		/* == normal army units from here down == */
  AC_SCOUT,		/* normal army scouts */
  AC_AGENT,		/* hired agents from enemy ranks */
  AC_MERCS,		/* mercenary troops */
  AC_CAVALRY,		/* cavalry class troops */
  AC_SAILORS,		/* seaworthy troops */
  AC_ORCISH,		/* orcish units */
  AC_ARCHERS,		/* units trained to use bows and arrows */
  AC_UNIQUE		/* unique army unit type */
} Aclasstype;

/* Structure for army unit type information */
typedef struct s_ainfo {
  char *name;		/* the full unit type name */
  char *sname;		/* shorter name of the unit type */
  char *select;		/* the selection string for drafting */
  char *descript;	/* a description of the unit */
  long properties;	/* the properties of the army type */
  Aclasstype class;	/* the class of the army unit */
  int minsth;		/* minimum unit strength */
  int att_bonus;	/* unit offensive combat adjustment */
  int def_bonus;	/* unit defensive combat adjustment */
  uns_char speed;	/* the relative speed of the unit type times 10 */
  long ore_enlist;	/* metal/jewel enlistment cost of the unit */
  long tal_enlist;	/* the talons/spell pts. for enlistment */
  long maint;		/* cost to maintain a unit; leader birth rate */
  int capt_val;		/* the land capture value of the unit */
  int work_val;		/* the "worker" (constructions) value */
  long pow_need[MAG_NUMBER];	/* list of magical powers needed */
} AINFO_STRUCT, *AINFO_PTR;

/* Army Definitions */
#define ARMY_ID		army_ptr->armyid
#define ARMY_TYPE	army_ptr->unittype
#define ARMY_XLOC	army_ptr->xloc
#define ARMY_YLOC	army_ptr->yloc
#define ARMY_LASTX	army_ptr->lastx
#define ARMY_LASTY	army_ptr->lasty
#define ARMY_SIZE	army_ptr->strength
#define ARMY_MOVE	army_ptr->umove
#define ARMY_STAT	army_ptr->status
#define ARMY_LEAD	army_ptr->leader
#define ARMY_SPLY	army_ptr->supply
#define ARMY_SPTS	army_ptr->spellpts
#define ARMY_EFF	army_ptr->efficiency
#define ARMY_MAXEFF	army_ptr->max_eff
#define ARMYT_ID	army_tptr->armyid
#define ARMYT_TYPE	army_tptr->unittype
#define ARMYT_XLOC	army_tptr->xloc
#define ARMYT_YLOC	army_tptr->yloc
#define ARMYT_LASTX	army_tptr->lastx
#define ARMYT_LASTY	army_tptr->lasty
#define ARMYT_SIZE	army_tptr->strength
#define ARMYT_MOVE	army_tptr->umove
#define ARMYT_STAT	army_tptr->status
#define ARMYT_LEAD	army_tptr->leader
#define ARMYT_SPLY	army_tptr->supply
#define ARMYT_SPTS	army_tptr->spellpts
#define ARMYT_EFF	army_tptr->efficiency
#define ARMYT_MAXEFF	army_tptr->max_eff

/* The list of unit properties -- strings in datamilX.c */
#define UP_RULER	0x00000001L	/* is a ruler and a leader */
#define UP_SLIPPERY	0x00000002L	/* can slip through enemy lines */
#define UP_FLIGHT	0x00000004L	/* has inate flight ability */
#define UP_UNDEAD	0x00000008L	/* is undead; absorbs dead men */
#define UP_ANTIAIR	0x00000010L	/* can force flying units to land */
#define UP_BALLISTICS	0x00000020L	/* projectile weapons */
#define UP_UNLOAD	0x00000040L	/* may unload in unowned territory */
#define UP_NAVALTAKE	0x00000080L	/* may unload in enemy territory */
#define UP_SIGHT	0x00000100L	/* able to see better in land */
#define UP_NEEDMIN	0x00000200L	/* must meet minimum str or no bonus */
#define UP_FORTDAMAGE	0x00000400L	/* can cause damage to forts */
#define UP_ARROWWEAK	0x00000800L	/* fight worse against projectiles */
#define UP_PAYOFF	0x00001000L	/* must be given severence */
#define UP_COVERBONUS	0x00002000L	/* unit gives bonus to all units */
#define UP_ANYDISB	0x00004000L	/* may disband in any sector */
#define UP_HALFRECRUIT	0x00008000L	/* only use half the recruits */
#define UP_SPCLTRAIN	0x00010000L	/* troops receive special training */
#define UP_FIRETYPE	0x00020000L	/* troops with origin in flame */
#define UP_WATERTYPE	0x00040000L	/* troops with origin in water */
#define UP_EARTHTYPE	0x00080000L	/* troops with origin in earth */
#define UP_FREESUPPLY	0x00100000L	/* unit does not need supplies */
#define UP_DECAY	0x00200000L	/* unit will slowly lose men */
#define UP_SPELLCAST	0x00400000L	/* unit may cast spells */
#define UP_FULLCASTER	0x00800000L	/* a full fledged spell caster */
#define UP_ENLISTAWAY	0x01000000L	/* can be drafted from others ranks */
#define UP_SAPPERUNIT	0x02000000L	/* a unit with sapper potential */
#define UP_MAPPING	0x04000000L	/* can survey land and take it */
#define UP_NODRAFT	0x08000000L	/* unit cannot be drafted directly */

/* now the macros for unit class determinations */
#define a_isnormal(x)	(ainfo_list[x].class >= AC_NORMAL)
#define a_isleader(x)	(ainfo_list[x].class <= AC_SPELLCASTER)
#define a_ismagician(x)	(ainfo_list[x].class == AC_SPELLCASTER)
#define a_ismonster(x)	(ainfo_list[x].class == AC_MONSTER)
#define a_castspells(x)	(ainfo_list[x].properties & UP_SPELLCAST)
#define a_fullcaster(x)	(ainfo_list[x].properties & UP_FULLCASTER)
#define a_isruler(x)	(ainfo_list[x].properties & UP_RULER)
#define a_isscout(x)	((ainfo_list[x].class == AC_SCOUT)||a_isagent(x))
#define a_isagent(x)	(ainfo_list[x].class == AC_AGENT)
#define a_ismerc(x)	(ainfo_list[x].class == AC_MERCS)
#define a_isarcher(x)	(ainfo_list[x].class == AC_ARCHERS)
#define a_anydisband(x)	(ainfo_list[x].properties & UP_ANYDISB)
#define a_isavian(x)	(ainfo_list[x].properties & UP_FLIGHT)
#define a_isundead(x)	(ainfo_list[x].properties & UP_UNDEAD)
#define a_cansee(x)	(ainfo_list[x].properties & UP_SIGHT)
#define a_needpay(x)	(ainfo_list[x].properties & UP_PAYOFF)
#define a_needmin(x)	(ainfo_list[x].properties & UP_NEEDMIN)
#define a_needtrain(x)	(ainfo_list[x].properties & UP_SPCLTRAIN)
#define a_arrowweak(x)	(ainfo_list[x].properties & UP_ARROWWEAK)
#define a_arrowmen(x)	(ainfo_list[x].properties & UP_BALLISTICS)
#define a_antiair(x)	(ainfo_list[x].properties & UP_ANTIAIR)
#define a_mayunload(x)	(ainfo_list[x].properties & UP_UNLOAD)
#define a_assault(x)	(ainfo_list[x].properties & UP_NAVALTAKE)
#define a_slippery(x)	(ainfo_list[x].properties & UP_SLIPPERY)
#define a_damfort(x)	(ainfo_list[x].properties & UP_FORTDAMAGE)
#define a_coverbonus(x)	(ainfo_list[x].properties & UP_COVERBONUS)
#define a_halfmen(x)	(ainfo_list[x].properties & UP_HALFRECRUIT)
#define a_freesupply(x)	(ainfo_list[x].properties & UP_FREESUPPLY)
#define a_decay(x)	(ainfo_list[x].properties & UP_DECAY)
#define a_enlistaway(x)	(ainfo_list[x].properties & UP_ENLISTAWAY)
#define a_sapper(x)	(ainfo_list[x].properties & UP_SAPPERUNIT)
#define a_mapper(x)	(ainfo_list[x].properties & UP_MAPPING)
#define a_nodraft(x)	(ainfo_list[x].properties & UP_NODRAFT)

/* The list of codes for army adjustments */
#ifdef USE_CODES

#include "executeX.h"

/* now the bounds of the army code numbers */
#define EX_ARMYBEGIN	EX_ARMYLOC
#define EX_ARMYEND	EX_ARMYMERCS

/* bounds for army groups */
#define EX_GRPBEGIN	EX_GRPLOC
#define EX_GRPEND	EX_GRPMOVE

/* first, those for individual army units */
#ifdef PRINT_CODES
#define AADJLOC	fprintf(fexe, "A_LOC\t%d\t%d\t%d\t%d\tnull\tnull\n", EX_ARMYLOC, (int)ARMY_ID, (int)ARMY_XLOC, (int)ARMY_YLOC)
#define AADJOLOC	fprintf(fexe, "A_LOC\t%d\t%d\t%d\t%d\tnull\tnull\n", EX_ARMYOLOC, (int)ARMY_ID, (int)ARMY_LASTX, (int)ARMY_LASTY)
#define AADJTYPE	fprintf(fexe, "A_TYPE\t%d\t%d\t%d\t0\tnull\tnull\n", EX_ARMYTYPE, (int)ARMY_ID, (int)ARMY_TYPE)
#define AADJSTAT	fprintf(fexe, "L_ASTAT\t%d\t%d\t%ld\t0\tnull\tnull\n", EX_ARMYSTAT, (int)ARMY_ID, ARMY_STAT)
#define AADJMOVE	fprintf(fexe, "A_MOVE\t%d\t%d\t%d\t0\tnull\tnull\n", EX_ARMYMOVE, (int)ARMY_ID, (int)ARMY_MOVE)
#define AADJLEAD	fprintf(fexe, "A_LEAD\t%d\t%d\t%d\t0\tnull\tnull\n", EX_ARMYLEAD, (int)ARMY_ID, (int)ARMY_LEAD)
#define AADJSIZE	fprintf(fexe, "L_ASIZE\t%d\t%d\t%ld\t0\tnull\tnull\n", EX_ARMYSIZE, (int)ARMY_ID, ARMY_SIZE)
#define AADJID	fprintf(fexe, "A_ID\t%d\t%d\t%d\t0\tnull\tnull\n", EX_ARMYID, global_int, (int)ARMY_ID)
#define AADJSPLY	fprintf(fexe, "A_SPLY\t%d\t%d\t%d\t0\tnull\tnull\n", EX_ARMYSPLY, (int)ARMY_ID, (int)ARMY_SPLY)
#define AADJSPTS	fprintf(fexe, "A_SPTS\t%d\t%d\t%d\t0\tnull\tnull\n", EX_ARMYSPTS, (int)ARMY_ID, (int)ARMY_SPTS)
#define AADJEFF	fprintf(fexe, "A_EFF\t%d\t%d\t%d\t0\tnull\tnull\n", EX_ARMYEFF, (int)ARMY_ID, (int)ARMY_EFF)
#define AADJMAXEFF	fprintf(fexe, "A_MAXEFF\t%d\t%d\t%d\t0\tnull\tnull\n", EX_ARMYMAXEFF, (int)ARMY_ID, (int)ARMY_MAXEFF)
#define ACREATE	fprintf(fexe, "A_CREATE\t%d\t%d\t%d\t0\tnull\tnull\n", EX_ARMYCREATE, (int)ARMY_ID, (int)ARMY_TYPE)
#define ADESTROY	fprintf(fexe, "A_DESTROY\t%d\t%d\t0\t0\tnull\tnull\n", EX_ARMYDESTROY, (int)ARMY_ID)
#define AADJMERC	fprintf(fexe, "A_MERCS\t%d\t0\t%d\t0\tnull\tnull\n", EX_ARMYMERCS, global_int)

/* now, those for army groups */
#define GADJLOC	fprintf(fexe, "G_LOC\t%d\t%d\t%d\t%d\tnull\tnull\n", EX_GRPLOC, (int)ARMY_ID, (int)ARMY_XLOC, (int)ARMY_YLOC)
#define GADJMOVE	fprintf(fexe, "G_MOVE\t%d\t%d\t%d\t%d\tnull\tnull\n", EX_GRPMOVE, (int)ARMY_ID, (int)unit_speed(ARMY_STAT), (int)ARMY_MOVE)
#endif /* PRINT_CODES */

#endif /* USE_CODES */

/* The definitions */
extern ARMY_PTR army_ptr, army_tptr;
extern AINFO_STRUCT ainfo_list[];
extern char *traits_list[];
extern char *ainfo_clist[];
extern int num_armytypes, num_atraits, num_aclasses;
