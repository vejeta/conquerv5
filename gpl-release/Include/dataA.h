// SPDX-License-Identifier: GPL-3.0-or-later
/* This file declares global data for the administrative program */
/*
 * Conquer - Classic Multi-Player Strategy Game
 *
 * Originally Copyright (C) 1987-1992 Ed Barlow <barlowedward@hotmail.com>
 * Originally Copyright (C) 1987-1992 Adam Bryant <adb@usa.com>
 * Copyright (C) 2025 Juan Manuel Méndez Rey <vejeta@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Relicensed to GPL v3+ with explicit permission from original authors.
 * For relicensing documentation, see RELICENSING-PERMISSIONS.md
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
