// SPDX-License-Identifier: GPL-3.0-or-later
/* formating routines for mail messages */
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
#include "dataX.h"
#include "calenX.h"
#include "rmailX.h"

/* file pointer for use with mail messaging */
FILE *mailfp = NULL;

/* pointer to the current mail message */
RMAIL_PTR cur_message = NULL;

/* MAIL_HEADER -- Send a mail header to the current target */
void
mail_header PARM_5(char *, nnstr, char *, nkstr, char *, subj, char *, dstr, int, statval)
{
  char buf[LINELTH];

  /* check the file pointer */
  if (mailfp == NULL) return;

  /* check the name.... */
  fprintf(mailfp, ":BEGIN:\n");
  if (nnstr != NULL) {
    fprintf(mailfp, "%s:\n", nnstr);
  } else {
    fprintf(mailfp, "%s:\n", CQ_MAIL_NAME);
  }

  /* now send the name description */
  if (nkstr != NULL) {
    fprintf(mailfp, "%s:\n", nkstr);
  } else {
    fprintf(mailfp, "%s:\n", CQ_MAIL_NICK);
  }

  /* now send the message status */
  fprintf(mailfp, "%d:\n", statval);

  /* put the postmark on it */
  if (dstr != NULL) {
    fprintf(mailfp, "%s:\n", dstr);
  } else {
    roman_number(buf, YEAR(TURN));
    fprintf(mailfp, "%s of Year %s:\n", PMONTH(TURN), buf);
  }

  /* now send the subject */
  if ((subj != NULL) &&
      (strlen(subj) > 0)) {
    fprintf(mailfp, "%s:\n", subj);
  } else {
    fprintf(mailfp, "[none]:\n");
  }
}

/* INIT_MAIL -- Initialize the storage structure for the current message */
void
init_mail PARM_0(void)
{
  int i;

  /* can we clean it? */
  if (cur_message == NULL) return;

  /* set all of the targets to NULL */
  for (i = 0; i < MAX_ADR; i++) {
    cur_message->to_whom[i] = ABSMAXNTN;
  }

  /* no status */
  cur_message->status = MSTAT_NONE;

  /* simply align else everything to NULL */
  cur_message->sender = NULL;
  cur_message->nickname = NULL;
  cur_message->subj = NULL;
  cur_message->date = NULL;
  cur_message->text = NULL;
  cur_message->next = NULL;
  cur_message->prev = NULL;
}

/* FREE_MAIL -- Deallocate all of the current mail memory in use */
void
free_mail PARM_0(void)
{
  MAILD_PTR maild_ptr, mdnext_ptr;

  /* check it */
  if (cur_message == NULL) {
    return;
  }

  /* remove all of the settings */
  if (cur_message->sender != NULL)
    free(cur_message->sender);
  if (cur_message->nickname != NULL)
    free(cur_message->nickname);
  if (cur_message->date != NULL)
    free(cur_message->date);
  if (cur_message->subj != NULL)
    free(cur_message->subj);

  /* get rid of all of the text */
  for (maild_ptr = cur_message->text;
       maild_ptr != NULL;
       maild_ptr = mdnext_ptr) {
    mdnext_ptr = maild_ptr->next;
    maild_ptr->next = NULL;
    free(maild_ptr);
  }

  /* now finish it */
  free(cur_message);
  cur_message = NULL;
}

/* KILL_MAIL -- Remove the currently pointed to mail message from the list */
void
kill_mail PARM_0(void)
{
  /* ain't nothing to delete */
  if (cur_message == NULL) return;

  /* squeeze in */
  if (cur_message->next != NULL) {
    (cur_message->next)->prev = cur_message->prev;
  }
  if (cur_message->prev != NULL) {
    (cur_message->prev)->next = cur_message->next;
  }
  cur_message->next = NULL;
  cur_message->prev = NULL;

  /* now get rid of it */
  free_mail();
}

/* DELIVER_MAIL -- Send the current mail message to the indicated countries */
void
deliver_mail PARM_0(void)
{
  MAILD_PTR travel_ptr;
  int i, target = UNOWNED, nl_last;
  char fname[FILELTH], whoname[NAMELTH + 1];

  /* is there a mail message? */
  if ((cur_message == NULL) ||
      (cur_message->text == NULL)) return;

  /* now go through and send it */
  for (i = 0; i < MAX_ADR; i++) {

    /* can we send it? */
    nl_last = -1;
    if ((target = cur_message->to_whom[i]) != ABSMAXNTN) {

      /* find the name of the nation to send it to */
      if (ntn_realname(whoname, target)) continue;
      if (target == NEWSPAPER) {
	sprintf(fname, "%s.%03d", whoname, TURN - START_TURN);
      } else {
	sprintf(fname, "%s.%s", whoname, msgtag);
      }

      /* open the mail file */
      if ((mailfp = fopen(fname, "a")) == NULL) {
	fprintf(fupdate, "Could not open mail file %s\n", fname);
	return;
      }

      /* send the header; if needed */
      if (target != NEWSPAPER) {
	mail_header(cur_message->sender, cur_message->nickname,
		    cur_message->subj, cur_message->date,
		    cur_message->status);
      } else {
	/* give just a subject line */
	if ((cur_message->subj != NULL) &&
	    (strlen(cur_message->subj) > 0)) {
	  fprintf(mailfp, "5.\n5.\"%s\"\n5.\n", cur_message->subj);
	} else {
	  fprintf(mailfp, "5.\n");
	}
      }

      /* now display the enire mail message */
      for (travel_ptr = cur_message->text;
	   travel_ptr != NULL;
	   travel_ptr = travel_ptr->next) {

	/* check for blank lines */
	if (strlen(travel_ptr->line_data) == 0) {
	  if (nl_last == FALSE) {
	    nl_last = TRUE;
	  }
	  continue;
	} else {
	  if (nl_last == TRUE) {
	    /* add a newline */
	    if (target == NEWSPAPER) {
	      fprintf(mailfp, "5.%s--\n", cur_message->sender);
	    } else {
	      fprintf(mailfp, "\n");
	    }
	  }
	  nl_last = FALSE;
	}

	/* display prefix for news */
	if ((target == NEWSPAPER) &&
	    (cur_message->sender != NULL)) {
	  fprintf(mailfp, "5.%s-- ", cur_message->sender);
	}

	/* send the message */
	fprintf(mailfp, "%s\n", travel_ptr->line_data);

      }

      /* finish up */
      fclose(mailfp);
      mailfp = NULL;

    }

  }
}
