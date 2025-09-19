/* routines to read and manipulate conquer mail messages */
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
#include "dataG.h"
#include "rmailX.h"
#include "keyvalsX.h"

/* declaration of keybindings at bottom */
extern KBIND_STRUCT reader_klist[];
extern PARSE_STRUCT reader_funcs[];
KLIST_PTR reader_bindings = NULL;

/* information structure concerning movement keybindings */
KEYSYS_STRUCT reader_keysys = {
  "reader", reader_funcs, reader_klist, 0, 0
};

/* pointer to the list of all of the mail */
static RMAIL_PTR mbox_list = NULL;

/* list of other useful data settings */
static int ml_done, msg_count = 0, hdr_start = 1, cur_count;
static int line_pos, header_mode = FALSE;
static RMAIL_PTR read_msg = NULL, read_hold = NULL, last_msg = NULL;
static MAILD_PTR read_line = NULL, travel_line;
static char ml_error[LINELTH];
extern int email_subjlim;
extern int email_collim;

/* ML_FIRST -- Find the first unread mail message */
static int
ml_first PARM_0(void)
{
  /* search through the list of mail messages */
  cur_count = 1;
  for (read_msg = mbox_list;
       read_msg != NULL;
       read_msg = read_msg->next) {
    /* stop on any unread mail message */
    if (!(read_msg->status & MSTAT_READ)) break;
    cur_count++;
  }

  /* check if there were none */
  if (read_msg == NULL) {
    read_msg = mbox_list;
    cur_count = 1;
  }

  /* set the start of the message */
  if (read_msg != NULL) {
    read_line = read_msg->text;
  } else {
    read_line = NULL;
  }
  return(0);
}

/* ML_COUNTEM -- Find the current pointer position and totals */
static int
ml_countem PARM_0(void)
{
  RMAIL_PTR rmail_tmp;

  /* loop and count */
  cur_count = 1;
  msg_count = 0;
  for (rmail_tmp = mbox_list;
       rmail_tmp != NULL;
       rmail_tmp = rmail_tmp->next) {
    /* locate the current message */
    msg_count++;
    if (rmail_tmp == read_msg) {
      cur_count = msg_count;
    }
  }
  return(0);
}

/* ML_HELP -- Show some documentation */
static int
ml_help PARM_0(void)
{
  /* create the help system */
  create_help("Conquer Mail Reader Command List",
	      reader_bindings, reader_funcs, reader_keysys.num_parse);
  return(0);
}

/* RMAIL_HEADER -- Retrieve the header from the file */
static int
rmail_header PARM_1(FILE *, infp)
{
  char buf[BIGLTH];
  int lstr = 0;

  /* check the file pointer */
  if (infp == NULL) return(FALSE);

  /* obtain a new current message */
  if ((cur_message = (RMAIL_PTR) malloc(sizeof(RMAIL_STRUCT))) == NULL) {
    errormsg("Memory Allocation Failure");
    abrt();
  }

  /* clean up the storage and "send" it to us */
  init_mail();
  cur_message->to_whom[0] = country;

  /* obtain who it is from */
  if ((fgets(buf, BIGLTH - 1, infp) == NULL) ||
      ((lstr = strlen(buf)) < 2) ||
      (buf[lstr - 2] != ':')) {
  exit_on_misformat:
    free_mail();
    return(FALSE);
  }
  buf[lstr - 2] = '\0';
  if ((cur_message->sender = (char *) malloc(sizeof(char) * lstr)) == NULL) {
    errormsg("Memory allocation failure");
    abrt();
  }
  strcpy(cur_message->sender, buf);

  /* find the nickname */
  if ((fgets(buf, BIGLTH - 1, infp) == NULL) ||
      ((lstr = strlen(buf)) < 2) ||
      (buf[lstr - 2] != ':')) {
    goto exit_on_misformat;
  }
  buf[lstr - 2] = '\0';
  if ((cur_message->nickname =
       (char *) malloc(sizeof(char) * lstr)) == NULL) {
    errormsg("Memory allocation failure");
    abrt();
  }
  strcpy(cur_message->nickname, buf);

  /* get the message status */
  if ((fgets(buf, BIGLTH - 1, infp) == NULL) ||
      ((lstr = strlen(buf)) < 2) ||
      (buf[lstr - 2] != ':')) {
    goto exit_on_misformat;
  }
  buf[lstr - 2] = '\0';
  if (sscanf(buf, "%d", &cur_message->status) != 1) {
    goto exit_on_misformat;
  }

  /* when? */
  if ((fgets(buf, BIGLTH - 1, infp) == NULL) ||
      ((lstr = strlen(buf)) < 2) ||
      (buf[lstr - 2] != ':')) {
    goto exit_on_misformat;
  }
  buf[lstr - 2] = '\0';
  if ((cur_message->date = (char *) malloc(sizeof(char) * lstr)) == NULL) {
    errormsg("Memory allocation failure");
    abrt();
  }
  strcpy(cur_message->date, buf);

  /* what is it about? */
  if ((fgets(buf, BIGLTH - 1, infp) == NULL) ||
      ((lstr = strlen(buf)) < 2) ||
      (buf[lstr - 2] != ':')) {
    goto exit_on_misformat;
  }
  buf[lstr - 2] = '\0';
  if ((cur_message->subj = (char *) malloc(sizeof(char) * lstr)) == NULL) {
    errormsg("Memory allocation failure");
    abrt();
  }
  strcpy(cur_message->subj, buf);

  return(TRUE);
}

/* OPEN_MAILBOX -- Read in all of the messages inside the mail box */
static int
open_mailbox PARM_0(void)
{
  RMAIL_PTR rmail_ptr = NULL;
  MAILD_PTR maild_ptr = NULL, nextmail_ptr = NULL;
  char line[BIGLTH];
  int mcount = 0, mlen = 0;
  FILE *fp;

  /* open and read in all of the mail */
  sprintf(line, "%s.%s", nationname, msgtag);
  if ((fp = fopen(line, "r")) == NULL) {
    return(mcount);
  }

  /* now read them all in */
  while (!feof(fp)) {

    /* get the input */
    if (fgets(line, BIGLTH - 1, fp) == NULL) break;

    /* remove the nl and find length */
    mlen = strlen(line);
    line[mlen - 1] = '\0';

    /* check for a new header */
    if (strcmp(line, ":BEGIN:") == 0) {

      /* retreive the valid header */
      if (rmail_header(fp) != FALSE) {
	if (mbox_list == NULL) {
	  mbox_list = cur_message;
	  rmail_ptr = mbox_list;
	} else {
	  rmail_ptr->next = cur_message;
	  cur_message->prev = rmail_ptr;
	  rmail_ptr = cur_message;
	}
	maild_ptr = NULL;
	mcount++;
      }

    } else if (rmail_ptr != NULL) {

      /* find the space for it */
      if ((maild_ptr = (MAILD_PTR) malloc(sizeof(MAILD_STRUCT))) == NULL) {
	errormsg("SERIOUS ERROR: Memory allocation failure");
	abrt();
      }
      if ((maild_ptr->line_data =
	   (char *) malloc(sizeof(char) * mlen)) == NULL) {
	errormsg("SERIOUS ERROR: Memory allocation failure");
	abrt();
      }
      strcpy(maild_ptr->line_data, line);
      maild_ptr->next = NULL;
      maild_ptr->prev = NULL;

      /* add it to the message */
      if (rmail_ptr->text == NULL) {
	nextmail_ptr = rmail_ptr->text = maild_ptr;
      } else {
	nextmail_ptr->next = maild_ptr;
	maild_ptr->prev = nextmail_ptr;
	nextmail_ptr = maild_ptr;
      }

    }

  }

  /* finito */
  fclose(fp);
  return(mcount);
}

/* lock file storage */
static char maillock[FILELTH];
static int lockid;

/* RMAIL_CLOSE -- Store the mail messages and remove the lock */
void
rmail_close PARM_0(void)
{
  /* remove the original file */
  sprintf(string, "%s.%s", nationname, msgtag);
  unlink(string);

  /* now store all of the mail messages and free storage */
  for (cur_message = mbox_list;
       cur_message != NULL;
       cur_message = mbox_list) {

    /* keep it */
    mbox_list = cur_message->next;
    deliver_mail();
    kill_mail();

  }

  /* take care of the lock file */
  kill_lock(lockid, maillock);
}

/* FORMAT_STR -- Generate a string from the supplied format */
static void
format_str PARM_3 (RMAIL_PTR, rmail_ptr, char *, format, char *, outstr)
{
  int i, j = 0;
  char *ch_ptr;

  if (rmail_ptr == NULL) {
    outstr[j] = '\0';
  } else {
    for (i = 0; format[i] != '\0'; i++) {

      /* copy it */
      if (format[i] == '$') {
	switch (format[++i]) {
	case '\0':
	  /* must end here */
	  i--;
	  break;
	case 'd':
	  /* the date of the message */
	  for (ch_ptr = rmail_ptr->date;
	       *ch_ptr != '\0';
	       ch_ptr++) {
	    outstr[j++] = *ch_ptr;
	  }
	  break;
	case 'S':
	  /* copy in the nickname of the original sender */
	  for (ch_ptr = rmail_ptr->nickname;
	       *ch_ptr != '\0';
	       ch_ptr++) {
	    outstr[j++] = *ch_ptr;
	  }
	  break;
	case 's':
	  /* copy in the name of the original sender */
	  for (ch_ptr = rmail_ptr->sender;
	       *ch_ptr != '\0';
	       ch_ptr++) {
	    outstr[j++] = *ch_ptr;
	  }
	  break;
	default:
	  /* just copy it; no error */
	  outstr[j++] = format[i];
	  break;
	}
      } else {
	outstr[j++] = format[i];
      }

    }
    outstr[j] = '\0';
  }
}

/* RMAIL_MSTART -- Prepare a mail message for editing */
static void
rmail_mstart PARM_3(int, isreply, int, quotemsg, RMAIL_PTR, msg_ptr )
{
  MAILD_PTR in_line, out_line = NULL;
  char buf[LINELTH];

  /* now build the new mail element */
  if ((cur_message = (RMAIL_PTR) malloc(sizeof(RMAIL_STRUCT))) == NULL) {
    /* uh oh */
    errormsg("Major Error:  Memory allocation error");
    abrt();
  }

  /* now begin it */
  init_mail();
  email_collim = 80 - (strlen(nationname) + 4);

  /* how much must be done? */
  if (msg_ptr != NULL) {

    /* setup the subject line */
    if (msg_ptr->subj != NULL) {
      if (isreply == TRUE) {

	/* replying to the sender */
	if (ntnbyname(msg_ptr->sender) == NULL) {
	  if (global_int != UNOWNED) {
	    errormsg("There was no return address in that message.");
	    free_mail();
	    return;
	  }
	}
	cur_message->to_whom[0] = global_int;
	if (strncmp(msg_ptr->subj, "Re:", 3) == 0) {
	  strcpy(string, msg_ptr->subj);
	} else {
	  sprintf(string, "Re: %s", msg_ptr->subj);
	}

      } else {
	sprintf(string, "FWD: %s", msg_ptr->subj);
      }

      /* now finish it */
      if ((cur_message->subj =
	   (char *) malloc((email_subjlim + 1) * sizeof(char))) == NULL) {
	/* uh oh */
	errormsg("Major Error:  Memory allocation error");
	abrt();
      }
      strncpy(cur_message->subj, string, email_subjlim);
      cur_message->subj[email_subjlim] = '\0';
    }

    /* find the quoting prefix */
    if ((quotemsg != FALSE) &&
	(msg_ptr->text != NULL)) {

      /* build the starting line */
      if (rmail_from_format[0] != '\0') {

	/* construct it from the format */
	format_str(msg_ptr, &(rmail_from_format[0]), buf);

	/* now allocate the line space */
	if ((cur_message->text =
	     (MAILD_PTR) malloc(sizeof(MAILD_STRUCT))) == NULL) {
	  /* uh oh */
	  errormsg("Major Error:  Memory allocation error");
	  abrt();
	}
	out_line = cur_message->text;
	out_line->next = NULL;
	out_line->prev = NULL;

	/* add in the string */
	if ((out_line->line_data =
	     (char *) malloc((email_collim + 1) * sizeof(char))) == NULL) {
	  /* uh oh */
	  errormsg("Major Error:  Memory allocation error");
	  abrt();
	}
	strncpy(out_line->line_data, buf, email_collim);
	out_line->line_data[email_collim] = '\0';

	/* add in "blank line" */
	if (quotemsg == TRUE) {
	  format_str(msg_ptr, &(rmail_quote_prefix[0]), buf);
	} else {
	  /* no prefix */
	  buf[0] = '\0';
	}
	if ((out_line->next =
	     (MAILD_PTR) malloc(sizeof(MAILD_STRUCT))) == NULL) {
	  /* uh oh */
	  errormsg("Major Error:  Memory allocation error");
	  abrt();
	}
	out_line->next->prev = out_line;
	out_line = out_line->next;
	out_line->next = NULL;
	if ((out_line->line_data =
	     (char *) malloc((email_collim + 1) * sizeof(char))) == NULL) {
	  /* uh oh */
	  errormsg("Major Error:  Memory allocation error");
	  abrt();
	}
	strncpy(out_line->line_data, buf, email_collim);
	out_line->line_data[email_collim] = '\0';
      }
      

      /* parse the quote prefix format */
      if (quotemsg == TRUE) {
	format_str(msg_ptr, &(rmail_quote_prefix[0]), buf);
      } else {
	/* no prefix */
	buf[0] = '\0';
      }

      /* now add in the entire text of the message */
      for (in_line = msg_ptr->text;
	   in_line != NULL;
	   in_line = in_line->next) {

	/* is it the beginning of the message? */
	if (out_line == NULL) {

	  /* allocate the initial line space */
	  if ((cur_message->text =
	       (MAILD_PTR) malloc(sizeof(MAILD_STRUCT))) == NULL) {
	    /* uh oh */
	    errormsg("Major Error:  Memory allocation error");
	    abrt();
	  }
	  out_line = cur_message->text;
	  out_line->next = NULL;
	  out_line->prev = NULL;

	} else {

	  /* allocate the next line space */
	  if ((out_line->next =
	       (MAILD_PTR) malloc(sizeof(MAILD_STRUCT))) == NULL) {
	    /* uh oh */
	    errormsg("Major Error:  Memory allocation error");
	    abrt();
	  }
	  (out_line->next)->prev = out_line;
	  out_line = out_line->next;
	  out_line->next = NULL;

	}

	/* now add in the line */
	strcpy(string, buf);
	strcat(string, in_line->line_data);

	/* tag it in */
	if ((out_line->line_data =
	     (char *) malloc((email_collim + 1) * sizeof(char))) == NULL) {
	  /* uh oh */
	  errormsg("Major Error:  Memory allocation error");
	  abrt();
	}
	strncpy(out_line->line_data, string, email_collim);
	out_line->line_data[email_collim] = '\0';
      }

    }
  }

  /* now call the mail editor and let it handle everything else */
  email_parse();
}

/* DO_EMAIL -- Send a normal mail message */
int
do_email PARM_0(void)
{
  /* call the prefix routine */
  rmail_mstart(FALSE, FALSE, NULL);
  return(0);
}

/* ML_CENTER_HSTART -- Align the header starting position to center current */
static void
ml_center_hstart PARM_0(void)
{
  hdr_start = cur_count - (LINES - 8) / 2;
  if (hdr_start < 1) hdr_start = 1;
}

/* ML_REALIGN -- Realign the bottom portion of the mail message */
static int
ml_realign PARM_0(void)
{
  if (read_line != NULL) {
    travel_line = read_line->next;
    for (line_pos = LINES - 11;
	 line_pos > 0;
	 line_pos--) {
      if (travel_line == NULL) break;
      travel_line = travel_line->next;
    }

    /* now traverse backward the proper amount */
    for (; line_pos > 0; line_pos--) {
      if (read_line->prev == NULL) break;
      read_line = read_line->prev;
    }
  }
  return(0);
}

/* ML_DOWNONE -- Move down one line */
static int
ml_downone PARM_0(void)
{
  /* go down a single line */
  if ((read_line != NULL) &&
      (read_line->next != NULL)) {
    read_line = read_line->next;
  }

  /* now realign */
  ml_realign();
  return(0);
}

/* ML_BOTTOM -- Move down to the bottom of the message */
static int
ml_bottom PARM_0(void)
{
  /* go down all the way */
  while ((read_line != NULL) &&
	 (read_line->next != NULL)) {
    read_line = read_line->next;
  }

  /* now realign */
  ml_realign();
  return(0);
}

/* ML_TOP -- Move to the very top of the current mail message */
static int
ml_top PARM_0(void)
{
  /* set back to the beginning */
  if (read_msg != NULL) {
    read_line = read_msg->text;
  }
  return(0);
}

/* ML_PAGEUP -- Move up a page of text */
static int
ml_pageup PARM_0(void)
{
  /* page mail up */
  if (read_line != NULL) {

    /* go LINES - 11 lines back */
    for (line_pos = 0;
	 line_pos < LINES - 11;
	 line_pos++) {
      if (read_line->prev == NULL) break;
      read_line = read_line->prev;
    }

  }
  return(0);
}

/* ML_UPONE -- Move upward one line */
static int
ml_upone PARM_0(void)
{
  if ((read_line != NULL) &&
      (read_line->prev != NULL)) {
    read_line = read_line->prev;
  }
  return(0);
}

/* ML_TO_NEXT -- Move to the appropriate next mail message */
static
void ml_to_next PARM_1(int, non_deleted)
{
  /* is there a current and a next? */
  if ((read_msg != NULL) &&
      (read_msg->next != NULL)) {

    /* get the next none-deleted mail message */
    read_hold = read_msg;
    line_pos = cur_count;
    do {
      read_msg = read_msg->next;
      cur_count++;
    } while ((non_deleted) &&
	     (read_msg != NULL) &&
	     (read_msg->status & MSTAT_DELETED));

    /* reset if necessary */
    if (read_msg == NULL) {
      strcpy(ml_error, "There is no following non-deleted mail message");
      read_msg = read_hold;
      cur_count = line_pos;
    } else {
      read_line = read_msg->text;
    }

  } else {
    strcpy(ml_error, "There is no following mail message");
  }
}

/* ML_NEXT -- Move to the next mail message */
static int
ml_next PARM_0(void)
{
  ml_to_next(FALSE);
  return(0);
}

/* ML_NEXTUND -- Move to the next undeleted mail message */
static int
ml_nextund PARM_0(void)
{
  ml_to_next(TRUE);
  return(0);
}

/* ML_PAGEDOWN -- Move the currently viewed message down one screen */
static int
ml_pagedown PARM_0(void)
{
  MAILD_PTR readl_hold = read_line;

  /* switch out of header mode */
  if (header_mode == TRUE) {
    header_mode = FALSE;
    if (read_msg != NULL) {
      read_line = read_msg->text;
    } else {
      read_line = NULL;
    }
    return(0);
  }

  /* now do the paging */
  if (read_line != NULL) {

    /* go LINES - 11 lines forward */
    for (line_pos = 0;
	 line_pos < LINES - 11;
	 line_pos++) {
      if (read_line->next == NULL) break;
      read_line = read_line->next;
    }

    /* now check the alignment */
    ml_realign();
    if (read_line == readl_hold) {
      ml_nextund();
    }

  }
  return(0);
}

/* ML_TO_PREV -- Move to the appropriate previous mail message */
static
void ml_to_prev PARM_1(int, non_deleted)
{
  /* go to the previous non-deleted message */
  if ((read_msg != NULL) &&
      (read_msg->prev != NULL)) {

    /* get the next none-deleted mail message */
    read_hold = read_msg;
    line_pos = cur_count;
    do {
      read_msg = read_msg->prev;
      cur_count--;
    } while ((non_deleted) &&
	     (read_msg != NULL) &&
	     (read_msg->status & MSTAT_DELETED));

    /* reset if necessary */
    if (read_msg == NULL) {
      strcpy(ml_error, "There is no preceding non-deleted mail message");
      read_msg = read_hold;
      cur_count = line_pos;
    } else {
      read_line = read_msg->text;
    }

  } else {
    strcpy(ml_error, "There is no preceding mail message");
  }
}

/* ML_PREV -- Move to the previous mail message */
static int
ml_prev PARM_0(void)
{
  ml_to_prev(FALSE);
  return(0);
}

/* ML_PREVUND -- Move to the previous undeleted mail message */
static int
ml_prevund PARM_0(void)
{
  ml_to_prev(TRUE);
  return(0);
}

/* ML_PURGE -- Purge all of the deleted mail messages */
static int
ml_purge PARM_0(void)
{
  RMAIL_PTR rmail_tmp;

  /* first move to any previously undeleted mail message */
  if ((read_msg != NULL) &&
      (read_msg->prev != NULL)) {

    /* get the next none-deleted mail message */
    read_hold = read_msg;
    line_pos = cur_count;
    while ((read_msg != NULL) &&
	   (read_msg->status & MSTAT_DELETED)) {
      read_msg = read_msg->prev;
      cur_count--;
    }

    /* try going forward? */
    if (read_msg == NULL) {

      /* now go forward */
      read_msg = read_hold;
      cur_count = line_pos;
      while ((read_msg != NULL) &&
	     (read_msg->status & MSTAT_DELETED)) {
	read_msg = read_msg->next;
	cur_count--;
      }

    }

    /* position the text */
    if (read_msg != NULL) {
      read_line = read_msg->text;
    } else {
      read_line = NULL;
    }
  }

  /* Remove all deleted mail */
  for (cur_message = mbox_list;
       cur_message != NULL;
       cur_message = rmail_tmp) {

    /* check for deletions */
    rmail_tmp = cur_message->next;
    if (cur_message->status & MSTAT_DELETED) {

      /* now get rid of it */
      if (cur_message == mbox_list) {
	mbox_list = rmail_tmp;
      }
      kill_mail();

    }
  }
  if (ml_done != TRUE) {
    ml_countem();
  }
  return(0);
}

/* ML_QUIT -- Exit the mail reader */
static int
ml_quit PARM_0(void)
{
  ml_done = TRUE;
  return(0);
}

/* ML_PANDQ -- Purge the mail and then exit */
static int
ml_pandq PARM_0(void)
{
  ml_quit();
  ml_purge();
  return(0);
}

/* ML_GOTO -- Jump to a specified mail message */
static int
ml_goto PARM_0(void)
{
  /* query them */
  mvaddstr(LINES - 1, 0, "Move to what mail message? ");
  clrtoeol();
  line_pos = get_number(FALSE);
  if (no_input == TRUE) return(0);

  /* test the input */
  if ((line_pos < 1) || (line_pos > msg_count)) {
    errormsg("There is no such mail message");
    return(0);
  }

  /* now go there */
  if (cur_count < line_pos) {
    while (cur_count < line_pos) {
      read_msg = read_msg->next;
      cur_count++;
    }
  } else if (cur_count > line_pos) {
    while (cur_count > line_pos) {
      read_msg = read_msg->prev;
      cur_count--;
    }
  }
  read_line = read_msg->text;
  ml_center_hstart();
  return(0);
}

/* ML_DELETE -- Mark the current message for deletion */
static int
ml_delete PARM_0(void)
{
  /* do it */
  if (read_msg != NULL) {
    read_msg->status |= MSTAT_DELETED;
  }
  return(0);
}

/* ML_DMARKALL -- Mark all (or all read) messages for deletion */
static void
ml_dmarkall PARM_1(int, readonly)
{
  RMAIL_PTR rmail_tmp;

  /* loop and mark for deletion */
  for (rmail_tmp = mbox_list;
       rmail_tmp != NULL;
       rmail_tmp = rmail_tmp->next) {
    /* has this message already been read? */
    if ((readonly == FALSE) ||
	(rmail_tmp->status & MSTAT_READ)) {
      rmail_tmp->status |= MSTAT_DELETED;
    }
  }
}

/* ML_DELREAD -- Query user to Mark ALL read messages for deletion */
static int
ml_delread PARM_0(void)
{
  bottommsg("Mark all mail that has been read for deletion? ");
  if (y_or_n()) {
    ml_dmarkall(TRUE);
  }
  return(0);
}

/* ML_DELALL -- Query user to Mark ALL messages for deletion */
static int
ml_delall PARM_0(void)
{
  bottommsg("Mark ALL mail messages for deletion? ");
  if (y_or_n()) {
    ml_dmarkall(FALSE);
  }
  return(0);
}

/* ML_DELNEXT -- Delete the current message and go to the next */
static int
ml_delnext PARM_0(void)
{
  ml_delete();
  ml_nextund();
  return(0);
}

/* ML_DELPREV -- Delete the current message and go to the previous */
static int
ml_delprev PARM_0(void)
{
  ml_delete();
  ml_prevund();
  return(0);
}

/* ML_UNDELETE -- Undelete the current or previously deleted message */
static int
ml_undelete PARM_0(void)
{
  RMAIL_PTR rmail_tmp;

  if (read_msg != NULL) {

    /* find the previously undeleted message */
    line_pos = cur_count;
    for (rmail_tmp = read_msg;
	 rmail_tmp != NULL;
	 rmail_tmp = rmail_tmp->prev) {
      if (rmail_tmp->status & MSTAT_DELETED) break;
      cur_count--;
    }

    /* do the undeleting */
    if (rmail_tmp != NULL) {
      read_msg = rmail_tmp;
      read_line = read_msg->text;
      read_msg->status &= ~MSTAT_DELETED;
    } else {
      cur_count = line_pos;
      strcpy(ml_error, "There is no previously deleted message");
    }
  }
  return(0);
}

/* ML_UNREAD -- Unread the current or previously read message */
static int
ml_unread PARM_0(void)
{
  RMAIL_PTR rmail_tmp;

  if (read_msg != NULL) {

    /* find the previously unread message */
    line_pos = cur_count;
    for (rmail_tmp = read_msg;
	 rmail_tmp != NULL;
	 rmail_tmp = rmail_tmp->prev) {
      if (rmail_tmp->status & MSTAT_READ) break;
      cur_count--;
    }

    /* do the unreading */
    if (rmail_tmp != NULL) {
      read_msg = rmail_tmp;
      read_line = read_msg->text;
      read_msg->status &= ~MSTAT_READ;
    } else {
      cur_count = line_pos;
      strcpy(ml_error, "There is no previously read message");
    }
  }
  return(0);
}

/* ML_TOGGLE -- Toggle the header mode */
static int
ml_toggle PARM_0(void)
{
  header_mode ^= TRUE;
  return(0);
}

/* ML_MAIL -- Send some mail */
static int
ml_mail PARM_0(void)
{
  rmail_mstart(FALSE, FALSE, NULL);
  return(0);
}

/* ML_QREPLY -- Send a quoted reply */
static int
ml_qreply PARM_0(void)
{
  rmail_mstart(TRUE, TRUE, read_msg);
  return(0);
}

/* ML_REPLY -- Send a quoted reply */
static int
ml_reply PARM_0(void)
{
  rmail_mstart(TRUE, FALSE, read_msg);
  return(0);
}

/* ML_QFORWARD -- Send a quoted reply */
static int
ml_qforward PARM_0(void)
{
  rmail_mstart(FALSE, TRUE, read_msg);
  return(0);
}

/* ML_FORWARD -- Send a quoted reply */
static int
ml_forward PARM_0(void)
{
  rmail_mstart(FALSE, -1, read_msg);
  return(0);
}

/* ML_SHOW -- Show the mail message */
static void
ml_show PARM_0(void)
{
  /* clean up shop */
  line_pos = 0;
  move(0, 0);
  clrtobot();

  /* show the current message and information */
  if (read_msg != NULL) {

    /* now choose the mode of display */
    if (header_mode == TRUE) {

      /* variable to keep track of message number */
      int header_count = cur_count;

      /* keep header within bounds */
      if (hdr_start < 1) {
	hdr_start = 1;
      } else if (hdr_start > cur_count) {
	hdr_start = cur_count;
      } else if (cur_count - hdr_start >= LINES - 7) {
	hdr_start = cur_count - (LINES - 8);
      }

      /* found out number to preceed with */
      line_pos = cur_count - hdr_start;
      for (read_hold = read_msg;
	   line_pos > 0;
	   line_pos--) {
	if (read_hold->prev == NULL) break;
	read_hold = read_hold->prev;
	header_count--;
      }

      /* now travel forward */
      for (line_pos = 2; line_pos < LINES - 5; line_pos++) {

	/* are we highlighting? */
	if (read_hold == read_msg) standout();

	/* keep track of message number */
	move(line_pos, 0);

	/* now show the information */
	printw("%3d ", header_count++);

	/* display the status indicators */
	if (read_hold->status & MSTAT_READ) {
	  addch('-');
	} else {
	  addch('N');
	}
	if (read_hold->status & MSTAT_DELETED) {
	  addch('D');
	} else {
	  addch('-');
	}
	if (read_hold->status & MSTAT_REPLIED) {
	  addch('R');
	} else {
	  addch('-');
	}
	if (read_hold->status & MSTAT_FORWARDED) {
	  addch('F');
	} else {
	  addch('-');
	}

	/* show who sent it */
	printw(" %-*.*s", NAMELTH, NAMELTH, read_hold->sender);

	/* when was it sent? */
	printw(" %-24.24s", read_hold->date);

	/* now send the subject */
	if (strcmp(read_hold->subj, "[none]") != 0) {
	  printw(" \"%-.*s\"", COLS - NAMELTH - 38, read_hold->subj);
	} else {
	  addstr(" [none]");
	}

	/* were we highlighting? */
	if (read_hold == read_msg) standend();

	/* go to the next line */
	if ((read_hold = read_hold->next) == NULL) break;
      }

    } else {

      /* indicate the the message has been read */
      if (last_msg != read_msg) {
	read_msg->status |= MSTAT_READ;
	last_msg = read_msg;
      }

      /* show who it is from */
      line_pos++;
      mvaddstr(line_pos++, 0, "From: ");
      if (strlen(read_msg->nickname) > 0) {
	printw("%s (%s)", read_msg->sender, read_msg->nickname);
      } else {
	printw("%s", read_msg->sender);
      }

      /* show what it is about */
      if (strcmp(read_msg->subj, "[none]") != 0) {
	mvaddstr(line_pos++, 0, "Subject: ");
	standout();
	printw("%.*s", COLS - 9, read_msg->subj);
	standend();
      } else {
	mvaddstr(line_pos++, 0, "Subject: [none]");
      }

      /* show the creation time */
      mvprintw(line_pos++, 0, "Date: %s", read_msg->date);

      /* display the message */
      if ((read_line != NULL) &&
	  (read_line->prev != NULL)) {
	mvprintw(line_pos, 0, "...cont...");
      }
      line_pos++;

      /* show what is visible */
      for (travel_line = read_line;
	   (line_pos < LINES - 5) &&
	   (travel_line != NULL);
	   travel_line = travel_line->next) {

	/* show it */
	if (conq_gaudy == TRUE) {
	  gaudy_lineout(line_pos++, travel_line->line_data);
	} else {
	  normal_lineout(line_pos++, travel_line->line_data);
	}

      }

      /* check for more */
      if (travel_line != NULL) {
	mvaddstr(line_pos, 0, "...more...");
      }

    }
  }

  /* where are we in the message list? */
  mvprintw(0, 0, "Message Number %d of %d",
	   cur_count, msg_count);

  /* show any headers */
  if (read_msg != NULL) {

    /* display the status indicators */
    if (!(read_msg->status & MSTAT_READ)) {
      addstr("; unread");
    }
    if (read_msg->status & MSTAT_DELETED) {
      addstr("; deleted");
    }
    if (read_msg->status & MSTAT_REPLIED) {
      addstr("; replied");
    }
    if (read_msg->status & MSTAT_FORWARDED) {
      addstr("; forwarded");
    }
  }

  /* show the command summary */
  errorbar("Conquer Mail Reader", "Hit '?' for help");
  move(LINES - 1, 0);
  if (ml_error[0] != '\0') {
    bottommsg(ml_error);
    ml_error[0] = '\0';
  } else {
    clrtoeol();
  }
  refresh();
}

/* RMAIL_PARSE -- Peruse all of the mail for the user */
int
rmail_parse PARM_0(void)
{
  int first_time = TRUE;
  FNCI fnc_ptr;

  /* clean up the bottom for now */
  clear_bottom(0);

  /* initialize things first time in */
  if (first_time == TRUE) {
    sprintf(maillock, "%s%s.%s", nationname, msgtag, isontag);
    header_mode = conq_mheaders;
    first_time = FALSE;
  }

  /* perform the locking */
  if ((lockid = check_lock(maillock, TRUE)) == -1) {
    errormsg("Someone is sending you mail... Please Wait");
    return(0);
  }

  /* is there any mail? */
  if ((msg_count = open_mailbox()) == 0) {
    errormsg("You have no Conquer mail");
    kill_lock(lockid, maillock);
    return(0);
  }
  mail_ind = MAIL_READING;

  /* initialize mail settings */
  clear();
  ml_done = FALSE;
  cur_count = 1;
  ml_error[0] = '\0';
  align_reader_keys();

  /* find the first unread mail message */
  ml_first();

  /* give a warning for large mail files */
  if (msg_count > 100) {
    redraw = DRAW_FULL;
    ml_dmarkall(TRUE);
    ml_show();
    errormsg("WARNING:  You have over 100 messages, old mail marked for deletion");
  }

  /* peruse and manipulate the mail */
  redraw = DRAW_FULL;
  while (ml_done == FALSE) {

    /* show it */
    ml_show();

    /* now get the input */
    if ((fnc_ptr = parse_keys(reader_bindings, FALSE)) == NULL) {

      /* oops */
      clear_bottom(1);
      mvaddstr(LINES - 1, 0, "Unknown key binding: ");
      show_str(string, FALSE);
      presskey();

    } else {

      /* perform the function */
      (*fnc_ptr)();

    }

  }

  /* close up shop */
  rmail_close();
  mail_ind = MAIL_DONE;
  conq_mail_status = STMAIL_NONE;
  return(0);
}

/* ML_OPTIONS -- Quickie command to allow the setting of options */
static int
ml_options PARM_0(void)
{
  option_cmd(reader_keysys, &reader_bindings);
  return(0);
}

/* function list */
PARSE_STRUCT reader_funcs[] = {
  {ml_options, "conquer-options",
     "Adjust the conquer environment by changing various options"},
  {ml_pageup, "reader-backward",
     "Reader-mode, scroll the current message backward one page"},
  {ml_bottom, "reader-bottom",
     "Reader-mode, shift the view to the bottom of the current message"},
  {ml_delete, "reader-delete",
     "Reader-mode, mark the current mail message for deletion"},
  {ml_delall, "reader-delete-all",
     "Reader-mode, delete all mail in the mailbox"},
  {ml_delnext, "reader-delete-and-next",
     "Reader-mode, delete the current message and find the next"},
  {ml_delprev, "reader-delete-and-previous",
     "Reader-mode, delete the current message and find the previous"},
  {ml_delread, "reader-delete-read-mail",
     "Reader-mode, delete all mail that has been read before"},
  {ml_downone, "reader-down-one",
     "Reader-mode, move the current message forward one line" },
  {ml_pandq, "reader-exit",
     "Reader-mode, purge deleted mail and then exit the mail reader"},
  {ml_pagedown, "reader-forward",
     "Reader-mode, scroll the current message forward one page"},
  {ml_goto, "reader-goto",
     "Reader-mode, jump to a specified mail message"},
  {ml_help, "reader-help",
     "Show the list of available mail reader commands and bindings"},
  {ml_mail, "reader-mail",
     "Reader-mode, send a mail message to another nation"},
  {ml_forward, "reader-mail-forward",
     "Reader-mode, forward the current message to another player"},
  {ml_reply, "reader-mail-reply",
     "Reader-mode, send a reply to the current mail message"},
  {ml_next, "reader-next",
     "Reader-mode, move to the next mail message"},
  {ml_nextund, "reader-next-undeleted",
     "Reader-mode, move to the next undeleted mail message"},
  {ml_prev, "reader-previous",
     "Reader-mode, move to the previous mail message"},
  {ml_prevund, "reader-previous-undeleted",
     "Reader-mode, move to the previous undeleted mail message"},
  {ml_purge, "reader-purge",
     "Reader-mode, flush any mail messages marked as deleted"},
  {ml_quit, "reader-quit",
     "Reader-mode, exit the mail reader, leaving deleted mail intact"},
  {ml_qforward, "reader-quoted-forward",
     "Reader-mode, forward the current message, quoted, to another player"},
  {ml_qreply, "reader-quoted-reply",
     "Reader-mode, send a reply and include the original message text"},
  {ml_toggle, "reader-toggle",
     "Reader-mode, toggle between header and full screen message mode"},
  {ml_top, "reader-top",
     "Reader-mode, shift the view to the top of the current message"},
  {ml_undelete, "reader-undelete",
     "Reader-mode, remove the deletion marker from a previously delete msg"},
  {ml_unread, "reader-unread",
     "Reader-mode, remove the indication that a message has been read"},
  {ml_upone, "reader-up-one",
     "Reader-mode, move the current message backward one line" },
  {do_redraw, "redraw-screen",
     "redraw the screen display of the mail message"},
  {do_ignore, "ignore-key",
      "I'll just pretend as if I never saw that key stroke" }
};

/* Default reader key bindings */
KBIND_STRUCT reader_klist[] = {
  { CSTR_B, do_ignore },
  { CSTR_D, ml_delprev },
  { CSTR_F, do_ignore },
  { CSTR_L, do_redraw },
  { CSTR_N, ml_next },
  { CSTR_P, ml_prev },
  { CSTR_R, do_redraw },
  { AKEY_UP1, ml_prev },
  { AKEY_UP2, ml_prev },
  { AKEY_DOWN1, ml_next },
  { AKEY_DOWN2, ml_next },
  { AKEY_LEFT1, do_ignore },
  { AKEY_LEFT2, do_ignore },
  { AKEY_RIGHT1, do_ignore },
  { AKEY_RIGHT2, do_ignore },
  { ESTR_DEL, ml_upone },
  { "<", ml_top },
  { ">", ml_bottom },
  { "?", ml_help },
  { " ", ml_pagedown },
  { "\b", ml_upone },
  { "\n", ml_downone },
  { "\r", ml_downone },
  { "b", ml_pageup },
  { "C", ml_delall },
  { "c", ml_delread },
  { "D", ml_delete },
  { "d", ml_delnext },
  { "F", ml_qforward },
  { "f", ml_forward },
  { "G", ml_goto },
  { "g", ml_goto },
  { "H", ml_toggle },
  { "h", ml_toggle },
  { "J", ml_next },
  { "j", ml_nextund },
  { "K", ml_prev },
  { "k", ml_prevund },
  { "M", ml_mail },
  { "m", ml_mail },
  { "N", ml_next },
  { "n", ml_nextund },
  { "O", ml_options },
  { "P", ml_prev },
  { "p", ml_prevund },
  { "Q", ml_quit },
  { "q", ml_pandq },
  { "R", ml_qreply },
  { "r", ml_reply },
  { "U", ml_unread },
  { "u", ml_undelete },
  { "x", ml_purge }
};

/* ALIGN_READER_KEYS -- Align all of the mail reader keys */
void
align_reader_keys PARM_0(void)
{
  /* initialize the system */
  if (reader_bindings == NULL) {
    reader_keysys.num_binds = (sizeof(reader_klist)/sizeof(KBIND_STRUCT));
    reader_keysys.num_parse = (sizeof(reader_funcs)/sizeof(PARSE_STRUCT));
    init_keys(&reader_bindings, reader_klist, reader_keysys.num_binds);
  }
}
