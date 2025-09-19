/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/* Quickie references to item information */
#define ITEM_ID		item_ptr->itemid
#define ITEM_TYPE	item_ptr->type
#define ITEM_INFO	item_ptr->iteminfo
#define ITEM_MEN	item_ptr->menforjob
#define ITEM_ARMY	item_ptr->armyid
#define ITEM_NAVY	item_ptr->navyid
#define ITEM_CVN	item_ptr->cvnid
#define ITEM_XLOC	item_ptr->xloc
#define ITEM_YLOC	item_ptr->yloc
#define ITEM_MTRLS	item_ptr->mtrls
#define ITEMT_ID	item_tptr->itemid
#define ITEMT_TYPE	item_tptr->type
#define ITEMT_INFO	item_ptr->iteminfo
#define ITEMT_MEN	item_ptr->menforjob
#define ITEMT_ARMY	item_ptr->armyid
#define ITEMT_NAVY	item_ptr->navyid
#define ITEMT_CVN	item_ptr->cvnid
#define ITEMT_XLOC	item_tptr->xloc
#define ITEMT_YLOC	item_tptr->yloc
#define ITEMT_MTRLS	item_tptr->mtrls

/* adjustment codes */
#ifdef USE_CODES

#include "executeX.h"

/* item information */
#define EX_ITEMBEGIN	EX_ITEMLOC
#define EX_ITEMEND	EX_ITEMCVN

/* take care of the commodity adjustment codes */
#ifdef PRINT_CODES
#define IADJLOC	fprintf(fexe, "I_LOC\t%d\t%d\t%d\t%d\tnull\tnull\n", EX_ITEMLOC, (int)ITEM_ID, (int)ITEM_XLOC, (int)ITEM_YLOC)
#define IADJID	fprintf(fexe, "I_ID\t%d\t%d\t%d\t0\tnull\tnull\n", EX_ITEMID, global_int, (int)ITEM_ID)
#define IARMY	fprintf(fexe, "I_ID\t%d\t%d\t%d\t0\tnull\tnull\n", EX_ITEMARMY, (int)ITEM_ID, (int)ITEM_ARMY)
#define INAVY	fprintf(fexe, "I_ID\t%d\t%d\t%d\t0\tnull\tnull\n", EX_ITEMNAVY, (int)ITEM_ID, (int)ITEM_NAVY)
#define ICVN	fprintf(fexe, "I_ID\t%d\t%d\t%d\t0\tnull\tnull\n", EX_ITEMCVN, (int)ITEM_ID, (int)ITEM_CVN)
#define ITYPE	fprintf(fexe, "I_TYPE\t%d\t%d\t%d\t0\tnull\tnull\n", EX_ITEMTYPE, (int)ITEM_ID, (int)ITEM_TYPE)
#define IINFO	fprintf(fexe, "L_IINFO\t%d\t%d\t%ld\t0\tnull\tnull\n", EX_ITEMINFO, (int)ITEM_ID, ITEM_INFO)
#define IMEN	fprintf(fexe, "L_IMEN\t%d\t%d\t%ld\t0\tnull\tnull\n", EX_ITEMMEN, (int)ITEM_ID, ITEM_MEN)
#define ICREATE	fprintf(fexe, "I_CREATE\t%d\t%d\t0\t0\tnull\tnull\n", EX_ITEMCREATE, (int)ITEM_ID)
#define IMTRLS(x)	fprintf(fexe, "D_IMTRLS\t%d\t%d\t%d\t%f\tnull\tnull\n", EX_ITEMMTRLS, (int)ITEM_ID, (x), (double)ITEM_MTRLS[x])
#define IDESTROY	fprintf(fexe, "I_DESTROY\t%d\t%d\t0\t0\tnull\tnull\n", EX_ITEMDESTROY, (int)ITEM_ID)
#endif /* PRINT_CODES */

#endif /* USE_CODES */

/* The variables */
extern ITEM_PTR item_ptr, item_tptr;
