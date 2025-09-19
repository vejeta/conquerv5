/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/* Definitions for naval speeds, sizes, and cargos */

/* Naval Ship Types; NSHP_NUMBER in dataX.h */
#define NSHP_WARSHIPS	0
#define NSHP_MERCHANTS	1
#define NSHP_GALLEYS	2
#define NSHP_BARGES	3

/* Structure for naval ship type information */
typedef struct s_ninfo {
  char *name;		/* The name of the ship class */
  char *sname;		/* The display name of the class */
  uns_char speed;	/* The associated speed of the class */
  long capacity;	/* The carrying capacity of one hold */
  itemtype taloncost;	/* Cost in talons to build this class */
  itemtype woodcost;	/* Cost in wood to build this class */
  int dam_chance;	/* % change to take damage instead of capture */
} NINFO_STRUCT, *NINFO_PTR;

/* Naval Types */
#define N_LIGHT		0
#define N_MEDIUM	1
#define N_HEAVY		2
#define N_BITSIZE	5
#define N_MASK		((unsigned short) 0x001f)
#define N_NOSPD		0	/* no ships no speed */
#define N_SIZESPD	3	/* bonus speed for lighter ships */

/* Counting Macros For Navy Units */
#define	N_CNTSHIPS(x,y)	(short)( ((x)&(N_MASK<<((y)*N_BITSIZE))) >> ((y)*N_BITSIZE) )
#define NAVY_CNTSHIPS(x,y)	N_CNTSHIPS(NAVY_SHIPS[x], y)

/* Quickie references for navy information */
#define NAVY_ID		navy_ptr->navyid
#define NAVY_SHIPS	navy_ptr->ships
#define NAVY_XLOC	navy_ptr->xloc
#define NAVY_YLOC	navy_ptr->yloc
#define NAVY_LASTX	navy_ptr->lastx
#define NAVY_LASTY	navy_ptr->lasty
#define NAVY_MOVE	navy_ptr->umove
#define NAVY_CREW	navy_ptr->crew
#define NAVY_PEOP	navy_ptr->people
#define NAVY_MTRLS	navy_ptr->mtrls
#define NAVY_ARMY	navy_ptr->armynum
#define NAVY_CVN	navy_ptr->cvnnum
#define NAVY_STAT	navy_ptr->status
#define NAVY_SPLY	navy_ptr->supply
#define NAVY_EFF	navy_ptr->efficiency
#define NAVYT_ID	navy_tptr->navyid
#define NAVYT_SHIPS	navy_tptr->ships
#define NAVYT_XLOC	navy_tptr->xloc
#define NAVYT_YLOC	navy_tptr->yloc
#define NAVYT_LASTX	navy_tptr->lastx
#define NAVYT_LASTY	navy_tptr->lasty
#define NAVYT_MOVE	navy_tptr->umove
#define NAVYT_CREW	navy_tptr->crew
#define NAVYT_PEOP	navy_tptr->people
#define NAVYT_MTRLS	navy_tptr->mtrls
#define NAVYT_ARMY	navy_tptr->armynum
#define NAVYT_CVN	navy_tptr->cvnnum
#define NAVYT_STAT	navy_tptr->status
#define NAVYT_SPLY	navy_tptr->supply
#define NAVYT_EFF	navy_tptr->efficiency

/* the codes */
#ifdef USE_CODES

#include "executeX.h"

/* those for navy commands */
#define EX_NAVYBEGIN	EX_NAVYLOC
#define EX_NAVYEND	EX_NAVYCVN

/* the adjustment statements for the fleets */
#ifdef PRINT_CODES
#define NADJLOC	fprintf(fexe, "N_LOC\t%d\t%d\t%d\t%d\tnull\tnull\n", EX_NAVYLOC, (int)NAVY_ID, (int)NAVY_XLOC, (int)NAVY_YLOC)
#define NADJOLOC	fprintf(fexe, "N_LOC\t%d\t%d\t%d\t%d\tnull\tnull\n", EX_NAVYOLOC, (int)NAVY_ID, (int)NAVY_LASTX, (int)NAVY_LASTY)
#define NADJSHIP	fprintf(fexe, "N_SHIP\t%d\t%d\t%d\t%d\tnull\tnull\n", EX_NAVYSHIP, (int)NAVY_ID, global_int, (int)(NAVY_SHIPS[global_int]))
#define NADJSTAT	fprintf(fexe, "L_NSTAT\t%d\t%d\t%ld\t0\tnull\tnull\n", EX_NAVYSTAT, (int)NAVY_ID, NAVY_STAT)
#define NADJMOVE	fprintf(fexe, "N_MOVE\t%d\t%d\t%d\t0\tnull\tnull\n", EX_NAVYMOVE, (int)NAVY_ID, (int)NAVY_MOVE)
#define NADJID	fprintf(fexe, "N_ID\t%d\t%d\t%d\t0\tnull\tnull\n", EX_NAVYID, global_int, (int)NAVY_ID)
#define NADJCREW	fprintf(fexe, "N_CREW\t%d\t%d\t%d\t0\tnull\tnull\n", EX_NAVYCREW, (int)NAVY_ID, (int)NAVY_CREW)
#define NADJPEOP	fprintf(fexe, "N_PEOP\t%d\t%d\t%d\t0\tnull\tnull\n", EX_NAVYPEOP, (int)NAVY_ID, (int)NAVY_PEOP)
#define NMTRLS(x)	fprintf(fexe, "D_NMTRLS\t%d\t%d\t%d\t%f\tnull\tnull\n", EX_NAVYMTRLS, (int)NAVY_ID, (x), (double)NAVY_MTRLS[x])
#define NADJSPLY	fprintf(fexe, "N_SPLY\t%d\t%d\t%d\t0\tnull\tnull\n", EX_NAVYSPLY, (int)NAVY_ID, (int)NAVY_SPLY)
#define NADJARMY	fprintf(fexe, "N_ARMY\t%d\t%d\t%d\t0\tnull\tnull\n", EX_NAVYARMY, (int)NAVY_ID, (int)NAVY_ARMY)
#define NADJCVN	fprintf(fexe, "N_CVN\t%d\t%d\t%d\t0\tnull\tnull\n", EX_NAVYCVN, (int)NAVY_ID, (int)NAVY_CVN)
#define NADJEFF(x)	fprintf(fexe, "N_EFF\t%d\t%d\t%d\t%d\tnull\tnull\n", EX_NAVYEFF, (int)NAVY_ID, (int)NAVY_EFF[x], (x))
#define NCREATE	fprintf(fexe, "N_CREATE\t%d\t%d\t0\t0\tnull\tnull\n", EX_NAVYCREATE, (int)NAVY_ID)
#define NDESTROY	fprintf(fexe, "N_DESTROY\t%d\t%d\t0\t0\tnull\tnull\n", EX_NAVYDESTROY, (int)NAVY_ID)
#endif /* PRINT_CODES */

#endif /* USE_CODES */

/* The definitions */
extern NAVY_PTR navy_ptr, navy_tptr;
extern NINFO_STRUCT ninfo_list[NSHP_NUMBER];
