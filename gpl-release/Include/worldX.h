// SPDX-License-Identifier: GPL-3.0-or-later
/* conquer : Copyright (c) 1991 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/* Quickie references to the world information */
#define PWATER		world.pwater
#define PMOUNT		world.pmount
#define EXPOSURE	world.exposelvl
#define GROWTH		world.growthrate
#define PSCOUT		world.pscout
#define PMONSTER	world.pmonster
#define PREVOLT		world.prevolt
#define PMERCVAL	world.pmercval
#define PMERCMONST	world.pmercmonst
#define PTRADE		world.ptrade
#define PTGMETAL	world.ptgmetal
#define PTGJEWEL	world.ptgjewel
#define PTGSPELL	world.ptgspell
#define BUILDDIST	world.builddist
#define MAXPTS		world.maxpts
#define MAXSUPPLIES	world.supplylimit
#define MAXDIPADJ	world.maxdipadj
#define NVSPLYDIST	world.nvsplydist
#define CITYXFDIST	world.cityxfdist
#define FLEET_CBVAL	world.fleet_cbval
#define WAGON_CBVAL	world.wagon_cbval
#define NUMDICE		world.num_dice
#define AVG_DAMAGE	world.avg_damage
#define DAMAGE_LIMIT	world.damage_limit
#define OVERMATCH_ADJ	world.overmatch_adj
#define PMINDAMAGE	world.pmindamage
#define	MERCMEN		world.m_mil
#define	MERCATT		world.m_aplus
#define	MERCDEF		world.m_dplus
#define NUMRANDOM	world.numrandom
#define NUMWEATHER	world.numweather
#define BRIBELEVEL	world.bribelevel
#define	WORLDMTRLS	world.w_mtrls
#define	WORLDSCORE	world.w_score
#define	WORLDCIV	world.w_civ
#define	WORLDSCT	world.w_sctrs
#define	WORLDMIL	world.w_mil
#define	WORLDNTN	world.active_ntns

/* constant for indicator use */
#define NTN_DISTIND	50
#define MAX_TRANSFER	((CITYXFDIST == NTN_DISTIND)?COMM_I_RANGE:CITYXFDIST)

/* All of the remaining adjustment codes */
#ifdef USE_CODES

#include "executeX.h"

/* now for the adjustment nation codes */
#define EX_NTNBEGIN	EX_NTNNAME
#define EX_NTNEND	EX_UNUMDEFAULT

/* adjustable sector information */
#define EX_SCTBEGIN	EX_SCTDESG
#define EX_SCTEND	EX_SCTMNRLS

/* resources selection commands */
#define EX_TAKEBEGIN	EX_TAKESTART
#define EX_TAKEEND	EX_TAKEFINISH
#define EX_GIVEBEGIN	EX_GIVESTART
#define EX_GIVEEND	EX_GIVEFINISH

#ifdef PRINT_CODES
/* now handle the nation adjustment information */
#define XADJNAME	fprintf(fexe, "X_NAME\t%d\t0\t0\t0\t%12s\tnull\n", EX_NTNNAME, ntn_ptr->name)
#define XADJLOGIN	fprintf(fexe, "X_LOGIN\t%d\t0\t0\t0\t%12s\tnull\n", EX_NTNLOGIN, ntn_ptr->login)
#define XADJPASSWD	fprintf(fexe, "X_PASSWD\t%d\t0\t0\t0\t%12s\tnull\n", EX_NTNPASSWD, ntn_ptr->passwd)
#define XADJLEADER	fprintf(fexe, "X_LEADER\t%d\t0\t0\t0\t%12s\tnull\n", EX_NTNLEADER, ntn_ptr->leader)
#define XADJLOC	fprintf(fexe, "X_LOC\t%d\t0\t%d\t%d\tnull\tnull\n", EX_NTNLOC, (int)ntn_ptr->capx, (int)ntn_ptr->capy)
#define XADJAPLUS	fprintf(fexe, "X_APLUS\t%d\t0\t%d\t0\tnull\tnull\n", EX_NTNAPLUS, (int)ntn_ptr->aplus)
#define XADJDPLUS	fprintf(fexe, "X_DPLUS\t%d\t0\t%d\t0\tnull\tnull\n", EX_NTNDPLUS, (int)ntn_ptr->dplus)
#define XADJACT	fprintf(fexe, "X_ACTIVE\t%d\t%d\t%d\t0\tnull\tnull\n", EX_NTNACTIVE, country, (int)ntn_ptr->active)
#define XADJDIPLO	fprintf(fexe, "X_DIPLO\t%d\t0\t%d\t%d\tnull\tnull\n", EX_NTNDIPLO, global_int, ntn_ptr->dstatus[global_int])
#define XADJBUTE	fprintf(fexe, "X_BUTE\t%d\t%d\t%d\t0\tnull\tnull\n", EX_NTNBUTE, global_int, (int)ntn_ptr->attribute[global_int])
#define XADJRLOC	fprintf(fexe, "X_RLOC\t%d\t0\t%d\t%d\tnull\tnull\n", EX_NTNRLOC, (int)ntn_ptr->centerx, (int)ntn_ptr->centery)
#define XADJREPRO	fprintf(fexe, "X_REPRO\t%d\t0\t%d\t0\tnull\tnull\n", EX_NTNLOC, (int)ntn_ptr->repro)
#define XADJRACE	fprintf(fexe, "X_RACE\t%d\t0\t%d\t0\tnull\tnull\n", EX_NTNRACE, (int)ntn_ptr->race)
#define XADJMARK	fprintf(fexe, "X_MARK\t%d\t0\t%d\t0\tnull\tnull\n", EX_NTNMARK, (int)ntn_ptr->mark)
#define XADJLEDGE	fprintf(fexe, "X_LEDGE\t%d\t0\t%d\t0\tnull\tnull\n", EX_NTNLEDGE, (int)ntn_ptr->leftedge)
#define XADJREDGE	fprintf(fexe, "X_REDGE\t%d\t0\t%d\t0\tnull\tnull\n", EX_NTNREDGE, (int)ntn_ptr->rightedge)
#define XADJTEDGE	fprintf(fexe, "X_TEDGE\t%d\t0\t%d\t0\tnull\tnull\n", EX_NTNTEDGE, (int)ntn_ptr->topedge)
#define XADJBEDGE	fprintf(fexe, "X_BEDGE\t%d\t0\t%d\t0\tnull\tnull\n", EX_NTNBEDGE, (int)ntn_ptr->bottomedge)
#define XADJCLASS	fprintf(fexe, "X_CLASS\t%d\t0\t%d\t0\tnull\tnull\n", EX_NTNCLASS, (int)ntn_ptr->class)
#define XADJSCORE	fprintf(fexe, "L_SCORE\t%d\t0\t%ld\t0\tnull\tnull\n", EX_NTNSCORE, ntn_ptr->score)
#define XADJMOVE	fprintf(fexe, "X_MOVE\t%d\t0\t%d\t0\tnull\tnull\n", EX_NTNMOVE, (int)ntn_ptr->maxmove)
#define XADJSPTS	fprintf(fexe, "X_SPELL\t%d\t0\t%d\t0\tnull\tnull\n", EX_NTNSPTS, (int)ntn_ptr->spellpts)
#define XBRIBE	fprintf(fexe, "D_BRIBE\t%d\t%d\t%f\t0\tnull\tnull\n", EX_BRIBE, global_int, (double)global_long)
#define XADJNEWS	fprintf(fexe, "L_NEWS\t%d\t0\t%ld\t0\tnull\tnull\n", EX_NEWSSIZE, global_long)
#define XADJMAIL	fprintf(fexe, "L_MAIL\t%d\t0\t%ld\t0\tnull\tnull\n", EX_MAILSIZE, global_long)
#define XNRENUM	fprintf(fexe, "X_NRENUM\t%d\t0\t0\t0\tnull\tnull\n", EX_NTNRENUM)
#define XUNUMCOPY	fprintf(fexe, "X_UNCPY\t%d\t0\t0\t0\tnull\tnull\n", EX_UNUMCOPY)
#define XUNUMRESET	fprintf(fexe, "X_UNRST\t%d\t0\t0\t0\tnull\tnull\n", EX_UNUMRESET)
#define XUNUMDEFAULT	fprintf(fexe, "X_UNDFLT\t%d\t0\t0\t0\tnull\tnull\n", EX_UNUMDEFAULT)
#define XUNUMSLOT	fprintf(fexe, "X_UNSLT\t%d\t%d\t%d\t%ld\tnull\tnull\n", EX_UNUMSLOT, x, global_int, global_long)

/* sector information */
#define SADJDESG	fprintf(fexe, "S_DESG\t%d\t%d\t%d\t%d\tnull\tnull\n", EX_SCTDESG, sct[XREAL][YREAL].designation, XREAL, YREAL)
#define SADJPEOP	fprintf(fexe, "L_PEOP\t%d\t%d\t%ld\t%ld\tnull\tnull\n", EX_SCTPEOPLE, XREAL, (long)YREAL, sct[XREAL][YREAL].people)
#define SADJOWN	fprintf(fexe, "S_OWN\t%d\t%d\t%d\t%d\tnull\tnull\n", EX_SCTOWN, sct[XREAL][YREAL].owner, XREAL, YREAL)
#define SADJALT	fprintf(fexe, "S_ALT\t%d\t%d\t%d\t%d\tnull\tnull\n", EX_SCTALT, sct[XREAL][YREAL].altitude, XREAL, YREAL)
#define SADJVEG	fprintf(fexe, "S_VEG\t%d\t%d\t%d\t%d\tnull\tnull\n", EX_SCTVEG, sct[XREAL][YREAL].vegetation, XREAL, YREAL)
#define SADJTGOOD	fprintf(fexe, "S_TGOOD\t%d\t%d\t%d\t%d\tnull\tnull\n", EX_SCTTGOOD, sct[XREAL][YREAL].tradegood, XREAL, YREAL)
#define SADJMNRLS	fprintf(fexe, "S_MNRLS\t%d\t%d\t%d\t%d\tnull\tnull\n", EX_SCTMNRLS, sct[XREAL][YREAL].minerals, XREAL, YREAL)

/* resource allocation commands */
#define TAKESTART	fprintf(fexe, "T_START\t%d\t0\t%d\t%d\tnull\tnull\n", EX_TAKESTART, XREAL, YREAL)
#define TAKEMTRLS(x)	fprintf(fexe, "L_TMTRLS\t%d\t%d\t%ld\t%ld\tnull\tnull\n", EX_TAKEMTRLS + (x), XREAL, (long)YREAL, global_long)
#define TAKEFINISH(x)	fprintf(fexe, "T_FINISH\t%d\t%d\t%d\t%d\tnull\tnull\n", EX_TAKEFINISH, (int) x, XREAL, YREAL)
#define GIVESTART	fprintf(fexe, "G_START\t%d\t0\t%d\t%d\tnull\tnull\n", EX_GIVESTART, XREAL, YREAL)
#define GIVEMTRLS(x)	fprintf(fexe, "L_GMTRLS\t%d\t%d\t%d\t%ld\tnull\tnull\n", EX_GIVEMTRLS + (x), XREAL, YREAL, global_long)
#define GIVEFINISH(x)	fprintf(fexe, "G_FINISH\t%d\t%d\t%d\t%d\tnull\tnull\n", EX_GIVEFINISH, (int) x, XREAL, YREAL)
#endif /* PRINT_CODES */

#endif /* USE_CODES */
