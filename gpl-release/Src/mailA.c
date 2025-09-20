// SPDX-License-Identifier: GPL-3.0-or-later
/* Functions for sending mail to nations during update */
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
#include "dataA.h"
#include "activeX.h"

/* mail file pointer */
extern FILE *mailfp;

/* local variables for grouped mail messaging */
static int gmsg_during = FALSE;
static int gmsg_target[ABSMAXNTN];
static char gmsg_header[LINELTH];

/* MSG_GINIT -- Begin sending a group message */
void
msg_ginit PARM_1(char *, str)
{
  int i;

  /* give 'em hell */
  if (gmsg_during == TRUE) {
    fprintf(fupdate, "Mailing Error: attempting to start a new group\n");
    fprintf(fupdate, "old = %s\n", gmsg_header);
    fprintf(fupdate, "new = %s\n", str);
    return;
  }

  /* initial info */
  strcpy(gmsg_header, str);
  gmsg_during = TRUE;
  for (i = 0; i < ABSMAXNTN; i++) {
    gmsg_target[i] = FALSE;
  }
}

/* MSG_GROUPED -- Append to the currently grouped message */
void
msg_grouped PARM_4(ntntype, to, int, xloc, int, yloc, char *, msgstr)
{
  NTN_PTR n1_ptr = NULL, ntemp_ptr = ntn_ptr;
  char tname[NAMELTH + 1];
  char fname[FILELTH + 1];

  /* check if a message is set */
  if (gmsg_during == FALSE) {
    errormsg("Warning: msg_grouped() ... no group message started");
    return;
  }

  /* check if it is to the deity */
  if (to == UNOWNED) {
    strcpy(tname, "god");
  } else {
    if (((n1_ptr = world.np[to]) == NULL) ||
	!n_ispc(n1_ptr->active)) {
      return;
    }
    strcpy(tname, n1_ptr->name);
  }

  /* open the output file */
  sprintf(fname, "%s.%s", tname, msgtag);
  if ((mailfp = fopen(fname, "a")) == NULL) {
    sprintf(string, "Could not open mail file %s\n", fname);
    errormsg(string);
    return;
  }

  /* check if this is the first message for this nation */
  if (gmsg_target[to] == FALSE) {
    gmsg_target[to] = TRUE;
    mail_header(NULL, NULL, gmsg_header, NULL, 0);
  }

  /* display the coordinates of the message */
  if ((xloc != -1) && (yloc != -1)) {
    ntn_ptr = n1_ptr;
    fprintf(mailfp, " [%d,%d]  \t", xloc_relative(xloc), yloc_relative(yloc));
  } else {
    fprintf(mailfp, "    ");
  }
  fprintf(mailfp, msgstr);
  putc('\n', mailfp);
  fclose(mailfp);
  mailfp = NULL;
  ntn_ptr = ntemp_ptr;
}

/* MSG_GFINISH -- Close up shop and end the mail messages */
void
msg_gfinish PARM_0(void)
{
  /* check settings */
  if (gmsg_during == FALSE) return;

  /* done with messaging */
  gmsg_during = FALSE;
}

/* storage for multi-line messages to the current country */
typedef struct s_cmsg {
  int line_info;	/* the status of this line */
  char *data;		/* the text of the message */
  struct s_cmsg *next;
} CMSG_STRUCT, *CMSG_PTR;
static CMSG_PTR cmsg_mail = NULL, last_cmsg = NULL;
static char cmsg_header[100];
static int cmsg_target = -1;

/* some definitions for conquer messages */
#define CMG_MARK	-1
#define CMG_NORMAL	0

/* MSG_CINIT -- Start a multi-line to the current nation */
void
msg_cinit PARM_1(char *, headstr)
{
  /* check for completion */
  if (cmsg_mail != NULL) {
    errormsg("Warning: msg_cinit()...  sending undelivered mail");
    msg_cfinish(TRUE);
  }

  /* otherwise, initialize settings properly */
  strcpy(cmsg_header, headstr);
  cmsg_target = country;
}

/* MSG_CADJUST -- Adjust the header and target of the current message */
void
msg_cadjust PARM_2(int, ntarget, char *, nheader)
{
  if (cmsg_target == -1) {
    errormsg("Warning: msg_cadjust() ... starting up new msg");
    msg_cinit("This and that...");
  }
  cmsg_target = ntarget;
  strcpy(cmsg_header, nheader);
}

/* MSG_CONQUER -- Add a line to the pending message */
void
msg_conquer PARM_1(char *, str)
{
  CMSG_PTR tmp_cmsg;

  /* check the target */
  if (cmsg_target == -1) {
    errormsg("Warning: msg_conquer() ... starting up new msg");
    msg_cinit("This and that...");
  } else if (country != cmsg_target) {
    errormsg("Warning: msg_conquer() ... country not target");
  }

  /* position the new message in sequence */
  if ((tmp_cmsg = (CMSG_PTR) malloc(sizeof(CMSG_STRUCT))) == NULL) {
    errormsg("Memory allocation error");
    abrt();
  }
  tmp_cmsg->line_info = CMG_NORMAL;
  tmp_cmsg->next = NULL;

  /* store the new line of text */
  if ((tmp_cmsg->data = (char *) malloc(sizeof(char) * (strlen(str) + 1)))
      == NULL) {
    errormsg("Memory allocation error");
    abrt();
  }
  strcpy(tmp_cmsg->data, str);

  /* find the position of the new item */
  if (cmsg_mail == NULL) {
    cmsg_mail = tmp_cmsg;
  } else {
    last_cmsg->next = tmp_cmsg;
  }
  last_cmsg = tmp_cmsg;
}

/* MSG_CMARK -- Provide space at the current position for later use */
void
msg_cmark PARM_0(void)
{
  CMSG_PTR tmp_cmsg;

  /* check the target */
  if (cmsg_target == -1) {
    errormsg("Warning: msg_cmark() ... starting up new msg");
    msg_cinit("This and that...");
  } else if (country != cmsg_target) {
    errormsg("Warning: msg_cmark() ... country not target");
  }

  /* position the new message in sequence */
  if ((tmp_cmsg = (CMSG_PTR) malloc(sizeof(CMSG_STRUCT))) == NULL) {
    errormsg("Memory allocation error");
    abrt();
  }
  tmp_cmsg->line_info = CMG_MARK;
  tmp_cmsg->next = NULL;
  tmp_cmsg->data = NULL;

  /* find the position of the new item */
  if (cmsg_mail == NULL) {
    cmsg_mail = tmp_cmsg;
  } else {
    last_cmsg->next = tmp_cmsg;
  }
  last_cmsg = tmp_cmsg;
}

/* MSG_CRETURN -- Now put the given string in the previously marked spot */
void
msg_creturn PARM_1(char *, str)
{
  CMSG_PTR tmp_cmsg;

  /* find the first marked line */
  for (tmp_cmsg = cmsg_mail;
       tmp_cmsg != NULL;
       tmp_cmsg = tmp_cmsg->next) {
    if (tmp_cmsg->line_info == CMG_MARK) break;
  }

  /* was there a success? */
  if (tmp_cmsg == NULL) {
    errormsg("Warning: msg_creturn() ... could not find marked line");
  } else {

    /* store the new line of text */
    if ((tmp_cmsg->data = (char *) malloc(sizeof(char) * (strlen(str) + 1)))
	== NULL) {
      errormsg("Memory allocation error");
      abrt();
    }
    strcpy(tmp_cmsg->data, str);

    /* clear out the mark */
    tmp_cmsg->line_info = CMG_NORMAL;

  }
}

/* MSG_CUNMARK -- Now remove a mark from the list */
void
msg_cunmark PARM_0(void)
{
  CMSG_PTR tmp_cmsg, tmp2_cmsg;

  /* find the first marked line */
  tmp_cmsg = cmsg_mail;
  if ((cmsg_mail != NULL) &&
      (cmsg_mail->line_info == CMG_MARK)) {

    /* eliminate the first line of the mail message */
    cmsg_mail = cmsg_mail->next;
    free(tmp_cmsg);

  } else {

    /* find the mark */
    for (; tmp_cmsg->next != NULL;
	 tmp_cmsg = tmp_cmsg->next) {
      if ((tmp_cmsg->next)->line_info == CMG_MARK) break;
    }

    /* was there a success? */
    if ((tmp_cmsg == NULL) ||
	(tmp_cmsg->next == NULL)) {
      /* no go */
      errormsg("Warning: msg_cunmark() ... could not find marked line");
    } else {
      /* elminate the line from the message */
      tmp2_cmsg = tmp_cmsg->next;
      tmp_cmsg->next = tmp2_cmsg->next;
      tmp2_cmsg->next = NULL;
      free(tmp2_cmsg);
    }

  }
}

/* MSG_CSEND -- Send the pending message to the target nation */
void
msg_csend PARM_0(void)
{
  NTN_PTR n1_ptr;
  CMSG_PTR travel_cmsg;
  char tname[NAMELTH + 1];
  char fname[FILELTH + 1];

  /* check the length */
  if (cmsg_mail == NULL) {
    /* no message... quiet return */
    return;
  }

  /* check the recipient */
  if (cmsg_target != country) {
    errormsg("Warning: msg_csend() ... target ntn not current");
  }

  /* check if a grouped message is being sent */
  if (gmsg_during == TRUE) {
    errormsg("Warning: msg_csend() ... grouped msg still active");
  }

  /* check if it is an npc */
  if (cmsg_target == UNOWNED) {
    strcpy(tname, "god");
  } else {
    if (((n1_ptr = world.np[cmsg_target]) == NULL) ||
	!n_ispc(ntn_ptr->active)) {
      return;
    } else {
      strcpy(tname, n1_ptr->name);
    }
  }

  /* open the output file */
  sprintf(fname, "%s.%s", tname, msgtag);
  if ((mailfp = fopen(fname, "a")) == NULL) {
    sprintf(string, "Could not open mail file %s\n", fname);
    errormsg(string);
    return;
  }

  /* send the header */
  mail_header(NULL, NULL, cmsg_header, NULL, 0);

  /* now send all of the mail */
  for (travel_cmsg = cmsg_mail;
       travel_cmsg != NULL;
       travel_cmsg = travel_cmsg->next) {
    if (travel_cmsg->line_info == CMG_MARK) {
      errormsg("Warning: msg_csend() ... marked line encountered");
    } else if (travel_cmsg->data != NULL) {
      fprintf(mailfp, "%s", travel_cmsg->data);
    }
  }
  putc('\n', mailfp);

  /* all done */
  fclose(mailfp);
  mailfp = NULL;
}

/* MSG_CFINISH -- Deliver and then remove storage */
void
msg_cfinish PARM_1(int, sendit)
{
  CMSG_PTR cmsg_next;

  /* first deliver it */
  if (sendit) msg_csend();

  /* now free everything up */
  for ( ; cmsg_mail != NULL; cmsg_mail = cmsg_next) {
    cmsg_next = cmsg_mail->next;
    free(cmsg_mail->data);
    free(cmsg_mail);
  }
  cmsg_target = -1;
}

/* structure and data for holding on to the news item */
typedef struct s_snews {
  int country;			/* the country who did the capturing */
  char newsitem[LINELTH+1];	/* the actual blurb about the capture */
  struct s_snews *next;
} SNEWS_STRUCT, *SNEWS_PTR;

static SNEWS_PTR snews_list = NULL;

/* SORTED_NEWS -- Store a news item that will be sorted later */
void
sorted_news PARM_2(int, cntry, char *, line)
{
  SNEWS_PTR snews_tmp, snews_travel;

  /* check the input */
  if (line == NULL) return;

  /* allocate the storage and copy in the data */
  if ((snews_tmp = (SNEWS_PTR) malloc(sizeof(SNEWS_STRUCT))) == NULL) {
    errormsg("SERIOUS ERROR: memory allocation failure in sorted_news()");
    abrt();
  }
  snews_tmp->country = cntry;
  strcpy(snews_tmp->newsitem, line);

  /* find the position */
  if (snews_list == NULL) {

    /* begin the list */
    snews_list = snews_tmp;
    snews_tmp->next = NULL;

  } else if (snews_tmp->country < snews_list->country) {

    /* position it at the beginning of the list */
    snews_tmp->next = snews_list;
    snews_list = snews_tmp;

  } else {

    /* insert it in the middle of the list */
    for (snews_travel = snews_list;
	 snews_travel->next != NULL;
	 snews_travel = snews_travel->next) {
      if (snews_tmp->country < (snews_travel->next)->country) {
	break;
      }
    }
    snews_tmp->next = snews_travel->next;
    snews_travel->next = snews_tmp;

  }
}

/* SEND_SORTEDNEWS -- Send out all of the accumulated sorted news */
void
send_sortednews PARM_0(void)
{
  SNEWS_PTR snews_next;
  
  /* just keep going until done */
  for (; snews_list != NULL; snews_list = snews_next) {

    /* keep on track */
    snews_next = snews_list->next;

    /* send out the item */
    fprintf(fnews, snews_list->newsitem);

    /* clean up */
    snews_list->next = NULL;
    free(snews_list);

  }
}
