// SPDX-License-Identifier: GPL-3.0-or-later
/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/* Quickie references to city information */
#define CITY_NAME	city_ptr->name
#define CITY_XLOC	city_ptr->xloc
#define CITY_YLOC	city_ptr->yloc
#define CITY_MTRLS	city_ptr->c_mtrls
#define CITY_IMTRLS	city_ptr->i_mtrls
#define CITY_STALONS	city_ptr->s_talons
#define CITY_FORT	city_ptr->fortress
#define CITY_PEOPLE	city_ptr->i_people
#define CITY_WEIGHT	city_ptr->weight
#define CITYT_NAME	city_tptr->name
#define CITYT_XLOC	city_tptr->xloc
#define CITYT_YLOC	city_tptr->yloc
#define CITYT_MTRLS	city_tptr->c_mtrls
#define CITYT_IMTRLS	city_tptr->i_mtrls
#define CITYT_STALONS	city_tptr->s_talons
#define CITYT_FORT	city_tptr->fortress
#define CITYT_PEOPLE	city_tptr->i_people
#define CITYT_WEIGHT	city_tptr->weight

/* the codes */
#ifdef USE_CODES

#include "executeX.h"

/* city information */
#define EX_CITYBEGIN	EX_CITYLOC
#define EX_CITYEND	EX_CITYWEIGHT

/* now for the city information adjustment commands */
#ifdef PRINT_CODES
#define CADJLOC	fprintf(fexe, "C_LOC\t%d\t0\t%d\t%d\t%12s\tnull\n", EX_CITYLOC, (int)CITY_XLOC, (int)CITY_YLOC, CITY_NAME)
#define CADJNAME	fprintf(fexe, "C_NAME\t%d\t0\t0\t0\t%12s\t%12s\n", EX_CITYNAME, CITY_NAME, newnamestr)
#define CADJPEOP	fprintf(fexe, "C_PEOP\t%d\t0\t%d\t0\t%12s\tnull\n", EX_CITYPEOP, (int)CITY_PEOPLE, CITY_NAME)
#define CADJWEIGHT	fprintf(fexe, "C_WEIGHT\t%d\t0\t%d\t0\t%12s\tnull\n", EX_CITYWEIGHT, (int)CITY_WEIGHT, CITY_NAME)
#define CADJMTRLS(x)	fprintf(fexe, "D_CMTRLS\t%d\t%d\t%f\t0\t%12s\tnull\n", EX_CITYMTRLS, (x), (double)CITY_MTRLS[x], CITY_NAME)
#define CADJSTALONS	fprintf(fexe, "D_CTLNS\t%d\t0\t%f\t0\t%12s\tnull\n", EX_CITYSTALONS, (double)CITY_STALONS, CITY_NAME)
#define CADJIMTRLS(x)	fprintf(fexe, "D_CMTRLS\t%d\t%d\t%f\t0\t%12s\tnull\n", EX_CITYIMTRLS, (x), (double)CITY_IMTRLS[x], CITY_NAME)
#define CADJFORT	fprintf(fexe, "C_FORT\t%d\t0\t%d\t0\t%12s\tnull\n", EX_CITYFORT, (int)CITY_FORT, CITY_NAME)
#define CCREATE	fprintf(fexe, "C_CREATE\t%d\t0\t0\t0\t%12s\tnull\n", EX_CITYCREATE, CITY_NAME)
#define CDESTROY	fprintf(fexe, "C_DESTROY\t%d\t0\t0\t0\t%12s\tnull\n", EX_CITYDESTROY, CITY_NAME)
#endif /* PRINT_CODES */

#endif /* USE_CODES */

/* The variables */
extern CITY_PTR city_ptr, city_tptr;
