// SPDX-License-Identifier: GPL-3.0-or-later
/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/* This file contains quickie references to caravan information */
#define CVN_ID		cvn_ptr->cvnid
#define CVN_XLOC	cvn_ptr->xloc
#define CVN_YLOC	cvn_ptr->yloc
#define CVN_LASTX	cvn_ptr->lastx
#define CVN_LASTY	cvn_ptr->lasty
#define CVN_SIZE	cvn_ptr->size
#define CVN_CREW	cvn_ptr->crew
#define CVN_PEOP	cvn_ptr->people
#define CVN_STAT	cvn_ptr->status
#define CVN_MOVE	cvn_ptr->umove
#define CVN_MTRLS	cvn_ptr->mtrls
#define CVN_SPLY	cvn_ptr->supply
#define CVN_EFF		cvn_ptr->efficiency
#define CVNT_ID		cvn_tptr->cvnid
#define CVNT_XLOC	cvn_tptr->xloc
#define CVNT_YLOC	cvn_tptr->yloc
#define CVNT_SIZE	cvn_tptr->size
#define CVNT_CREW	cvn_tptr->crew
#define CVNT_PEOP	cvn_tptr->people
#define CVNT_STAT	cvn_tptr->status
#define CVNT_MOVE	cvn_tptr->umove
#define CVNT_MTRLS	cvn_tptr->mtrls
#define CVNT_SPLY	cvn_tptr->supply
#define CVNT_EFF	cvn_tptr->efficiency

/* the adjustment codes for the caravans */
#ifdef USE_CODES

#include "executeX.h"

/* caravan settings */
#define EX_CVNBEGIN	EX_CVNLOC
#define EX_CVNEND	EX_CVNOLOC

/* caravan adjustment print statements */
#ifdef PRINT_CODES
#define VADJLOC	fprintf(fexe, "V_LOC\t%d\t%d\t%d\t%d\tnull\tnull\n", EX_CVNLOC, (int)CVN_ID, (int)CVN_XLOC, (int)CVN_YLOC)
#define VADJOLOC	fprintf(fexe, "V_LOC\t%d\t%d\t%d\t%d\tnull\tnull\n", EX_CVNOLOC, (int)CVN_ID, (int)CVN_LASTX, (int)CVN_LASTY)
#define VADJSIZE	fprintf(fexe, "V_SIZE\t%d\t%d\t%d\t0\tnull\tnull\n", EX_CVNSIZE, (int)CVN_ID, (int)CVN_SIZE)
#define VADJSTAT	fprintf(fexe, "L_VSTAT\t%d\t%d\t%ld\t0\tnull\tnull\n", EX_CVNSTAT, (int)CVN_ID, CVN_STAT)
#define VADJMOVE	fprintf(fexe, "V_MOVE\t%d\t%d\t%d\t0\tnull\tnull\n", EX_CVNMOVE, (int)CVN_ID, (int)CVN_MOVE)
#define VADJID	fprintf(fexe, "V_ID\t%d\t%d\t%d\t0\tnull\tnull\n", EX_CVNID, global_int, (int)CVN_ID)
#define VADJCREW	fprintf(fexe, "V_CREW\t%d\t%d\t%d\t0\tnull\tnull\n", EX_CVNCREW, (int)CVN_ID, (int)CVN_CREW)
#define VADJSPLY	fprintf(fexe, "V_SPLY\t%d\t%d\t%d\t0\tnull\tnull\n", EX_CVNSPLY, (int)CVN_ID, (int)CVN_SPLY)
#define VADJPEOP	fprintf(fexe, "V_PEOP\t%d\t%d\t%d\t0\tnull\tnull\n", EX_CVNPEOP, (int)CVN_ID, (int)CVN_PEOP)
#define VMTRLS(x)	fprintf(fexe, "D_VMTRLS\t%d\t%d\t%d\t%f\tnull\tnull\n", EX_CVNMTRLS, (int)CVN_ID, (x), (double)CVN_MTRLS[x])
#define VADJEFF	fprintf(fexe, "V_EFF\t%d\t%d\t%d\t0\tnull\tnull\n", EX_CVNEFF, (int)CVN_ID, (int)CVN_EFF)
#define VCREATE	fprintf(fexe, "V_CREATE\t%d\t%d\t0\t0\tnull\tnull\n", EX_CVNCREATE, (int)CVN_ID)
#define VDESTROY	fprintf(fexe, "V_DESTROY\t%d\t%d\t0\t0\tnull\tnull\n", EX_CVNDESTROY, (int)CVN_ID)
#endif /* PRINT_CODES */

#endif /* USE_CODES */

/* The variables */
extern CVN_PTR cvn_ptr, cvn_tptr;
