// SPDX-License-Identifier: GPL-3.0-or-later
/* This file declares global data for the administrative program */
/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * A good deal of time and effort has gone into the writing of this
 * code and it is our hope that you respect this.  We give permission
 * to alter the code, but not to redistribute modified versions of the
 * code without our explicit permission.  If you do modify the code,
 * please document the changes made, and send us a copy, so that all
 * people may have it.  The code, to the best of our knowledge, works
 * well, but there will probably always be a need for bug fixes and
 * improvements.  We disclaim any responsibility for the codes'
 * actions.  [Use at your own risk].  This notice is just our way of
 * saying, "Happy gaming!", while making an effort to not get sued in
 * the process.
 *                           Ed Barlow, Adam Bryant
 */

/* ::: THIS FILE SHOULD NOT NEED TO BE ALTERED ::: */

#include "dataX.h"

/* Spell casting information */
typedef struct s_spell {
  ntntype caster;	/* nation who cast the spell	*/
  int type;		/* type of spell that was cast	*/
  int xloc;		/* x position of spell casting	*/
  int yloc;		/* y position of spell casting	*/
  struct s_spell *next;
} SPLINFO_STRUCT, *SPLINFO_PTR;

/* Definitions for Sector Statuses */
#define SET_SIEGE	0x01
#define SET_DAMAGE	0x02
#define SET_DEVASTATE	0x04

#ifndef DATA_DECLARE
/* don't declare things from dataA.c */

/* nation placement definitions */
#define OOPS	(-1)

/* quick macro for status references */
#define SCT_STATUS(x, y)	sct_status[(x) + (y) * MAPX]

/* global variables for conqrun */
extern char *mild_begin[], *sct_status;
extern int spent[], remake;
extern char scenario[NAMELTH+1];

/* the list of spells cast in the world */
extern SPLINFO_PTR upd_spl_list;

/* now call in all of the header definitions */
#ifndef __CEXTRACT__
#include "fileA.h"
#endif /* __CEXTRACT__ */

#endif /*DATA_DECLARE*/
