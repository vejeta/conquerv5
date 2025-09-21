// SPDX-License-Identifier: GPL-3.0-or-later
/* This file declares global data for the user interface */
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

/* Include the global definitions file */
#include "dataX.h"
#include "keybindG.h"

/* ------------------------------------------------------------------ */
/*								      */
/*     This file is to be included by all files composing only        */
/*   the Conquer User Interface (conquer).			      */
/*     Only those definitions used within all of the conquer files    */
/*   and only in the conquer files should be set here.		      */
/*								      */
/* ------------------------------------------------------------------ */

/* number of command keys parsed */
#define MAXCOMMANDS	255

/* number of lines and width for pager help */
#define MAX_PAGERHELP	21
#define WIDTH_PAGERHELP	42

/* Structure for file perusal */
typedef struct s_line {
  char *line;
  uns_char highlight;
} LINE_STRUCT;

/* Mail system statuses */
#define MAIL_DONE	0
#define MAIL_READING	1
#define MAIL_SENDING	2

/* indicator of mail editing mode */
#define EM_M_NORMAL	0x00
#define EM_M_INSERTON	0x04
#define EM_M_VISTYLE	0x08

/* Everything past this point is not needed in dataG.c */
#ifndef DATA_DECLARE

/* Constants for Screen Redisplay */
#define DRAW_DONE	0
#define DRAW_FULL	1
#define DRAW_NOFRILLS	2

/* Jump routine notation */
#define JUMP_CAP	0
#define JUMP_PICK	1
#define JUMP_SAVE	2

/* Onto the generic macros and definitions */

/* Mail status indicators */
#define STMAIL_NEW	0
#define STMAIL_NONE	1

/* Generic Constants */
#define SCREEN_SLIM	(conq_infomode?(LINES - 5):(LINES - 14))
#define SCREEN_SIDE	(SCREEN_SLIM / 2)

/* file: dataG.c -- global data for user interface */
extern int xloc_mark, yloc_mark;
extern int lockfilenum, mail_ind, redraw;
extern char fison[], conqmail[], *dflt_motd[MOTDLINES];
extern itemtype command_cost;
extern int email_mode, pager, selector, city_mode;
extern int conq_news_status, conq_mail_status, god_browsing;
#ifdef SYSMAIL
extern int sys_mail_status;
extern char sysmail[];
#endif /*SYSMAIL*/
extern int cv_max[];
extern char *display_list[], *highl_list[], *cv_name[], *desg_selects[];
extern char *shortspeed[], *shortdir[];
extern char *shipsize[], *min_desg_selects[];
extern char *sectstat, *trooplocs;
extern DISPLAY_STRUCT display_mode;

#ifdef REGEXP
/* system function declarations */
extern int re_exec();
extern char *re_comp();
#endif /* REGEXP */

/* call in the function prototypes file */
#ifndef __CEXTRACT__
#include "fileG.h"
#endif /* __CEXTRACT__ */

#endif /*DATA_DECLARE*/
