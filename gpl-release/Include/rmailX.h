// SPDX-License-Identifier: GPL-3.0-or-later
/* header for data structures concerning read in mail messages */
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

/* THIS FILE IS ONLY NEEDED IF DIRECT ACCESS TO
   READING OR WRITING MAIL MESSAGES IS NECESSARY */

/* limit on number of targets for a mail message */
#define MAX_ADR	20	/* max number of nations for simultaneous mail */

/* the various status values of mail */
#define MSTAT_NONE	0	/* clean status */
#define MSTAT_READ	0x0001	/* the mail message has been read before */
#define MSTAT_DELETED	0x0002	/* marked as deleted */
#define MSTAT_REPLIED	0x0004	/* a reply has been sent */
#define MSTAT_FORWARDED	0x0008	/* the message has been forwarded */

typedef struct s_maildata {
  char *line_data;		/* a line of text of a message	*/
  struct s_maildata *next;	/* next line in the message	*/
  struct s_maildata *prev;	/* previous line in the message	*/
} MAILD_STRUCT, *MAILD_PTR;

typedef struct s_rmail {
  char *sender;			/* who send it */
  char *nickname;		/* how else the sender is known */
  int to_whom[MAX_ADR];	/* nations to be included in the message */
  char *date;			/* when it was sent */
  char *subj;			/* what the message is about */
  int status;			/* status indicators */
  MAILD_PTR text;		/* the text of the message */
  struct s_rmail *next;		/* the next message */
  struct s_rmail *prev;		/* the previous message */
} RMAIL_STRUCT, *RMAIL_PTR;

/* The data storage */
extern RMAIL_PTR cur_message;
