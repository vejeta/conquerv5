/* Code for the mail editor for sending interactive messages */
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

/* indicators current status */
#define EM_S_TOLINE	1
#define EM_S_SUBJECT	2
#define EM_S_BODY	3
#define EM_S_INSERTON	0x0004
#define EM_S_VICOMMAND	0x0008		/* vi mode not implemented */
#define EM_S_VIINSERT	0x0010		/* vi mode not implemented */

/* maximum number of mail targets; <= MAX_ADR */
#define EM_MAXLOCKS	MAX_ADR

/* declaration for lock file management */
static int em_locks[EM_MAXLOCKS];

/* declaration of keybindings at bottom */
extern KBIND_STRUCT email_klist[];
extern PARSE_STRUCT email_funcs[];
KLIST_PTR email_bindings = NULL;

/* information structure concerning movement keybindings */
KEYSYS_STRUCT email_keysys = {
  "mail", email_funcs, email_klist, 0, 0
};

/* declaration of location indicators */
static MAILD_PTR cur_line = NULL;
static int char_position = 0;

/* various indicator variables */
static int email_done;
static int email_status;
int email_collim;
int email_subjlim = 70;		/* 80 - sizeof(Subject:) */

/* EMAIL_LOCK -- Set a lock if possible */
static int
email_lock PARM_2(int, lnum, int, who)
{
  char buf[NAMELTH + 1], tfname[LINELTH];

  /* is it used already */
  if (em_locks[lnum] > -1) {
    /* cannot supply a lock */
    errormsg("You cannot send mail to that many people");
    return(FALSE);
  } else {
    /* supply it */
    if (ntn_realname(&(buf[0]), who)) return(FALSE);
    sprintf(tfname, "%s%s.%s", buf, msgtag, isontag);
    if ((em_locks[lnum] = check_lock(tfname, TRUE)) == -1) {
      sprintf(string,
	      "Nation %s's mail file is in use; Try again later...",
	      buf);
      if (global_int == TRUE) next_char();
      errormsg(string);
      return(FALSE);
    }
    cur_message->to_whom[lnum] = who;
  }
  return(TRUE);
}

/* EMAIL_UNLOCK -- Remove the specified lock file */
static void
email_unlock PARM_1(int, lnum)
{
  char buf[NAMELTH + 1], tfname[LINELTH];

  if (em_locks[lnum] > -1) {

    /* kill the lock */
    if (!ntn_realname(&(buf[0]), cur_message->to_whom[lnum])) {
      sprintf(tfname, "%s%s.%s", buf, msgtag, isontag);
      kill_lock(em_locks[lnum], tfname);
    }

    /* clear it */
    em_locks[lnum] = -1;
    cur_message->to_whom[lnum] = ABSMAXNTN;

  }
}

/* EMAIL_CLOSE -- Free allocated space and remove all set locks */
void
email_close PARM_0(void)
{
  int i;

  /* handle all of the locks */
  for (i = 0; i < EM_MAXLOCKS; i++) {
    email_unlock(i);
  }

  /* now free up the used space */
  mail_ind &= ~MAIL_SENDING;
  free_mail();
  redraw = DRAW_FULL;
}

/* EMAIL_INIT -- Initialize the bindings */
int
email_init PARM_0(void)
{
  init_keys(&email_bindings, email_klist, email_keysys.num_binds);
  return(0);
}

/* EM_SEND -- Deliver the mail message */
static int
em_send PARM_0(void)
{
  /* first check if there is anyone to send to */
  if (cur_message->to_whom[0] == ABSMAXNTN) {
    errormsg("You have not indicated anyone to send the message to");
    return(FALSE);
  }

  /* query it */
  standout();
  mvaddstr(LINES - 1, 0, "Deliver the mail message? ");
  standend();
  if (y_or_n()) {
    deliver_mail();
    bottommsg("Sent...");
    refresh();
    sleep(1);
    return(TRUE);
  }
  return(FALSE);
}

/* EM_EXIT -- Deliver the mail and then exit */
static int
em_exit PARM_0(void)
{
  if (em_send() == TRUE) {
    email_done = TRUE;
  }
  return(0);
}

/* EM_QUIT -- Leave, after a query */
static int
em_quit PARM_0(void)
{
  /* query an exit */
  bottommsg("Do you wish to exit without sending the mail? ");
  if (y_or_n()) {
    email_done = TRUE;
  }
  return(0);
}

/* EM_TOGGLE -- Toggle between overwrite and insert mode */
static int
em_toggle PARM_0(void)
{
  email_status ^= EM_S_INSERTON;
  return(0);
}

/* EM_TOEOL -- Go to the end of the line */
static int
em_toeol PARM_0(void)
{
  switch (email_status % 4) {
  case EM_S_TOLINE:
    /* last element in the To: line */
    for (char_position = 0;
	 char_position < EM_MAXLOCKS;
	 char_position++) {
      if (cur_message->to_whom[char_position] == ABSMAXNTN) break;
    }
    break;
  case EM_S_SUBJECT:
    /* length of the subject */
    char_position = strlen(cur_message->subj);
    break;
  case EM_S_BODY:
    /* simply set the value to the length of the string */
    char_position = strlen(cur_line->line_data);
    break;
  }
  return(0);
}

/* EM_TOBOL -- Go to the beginning of the line */
static int
em_tobol PARM_0(void)
{
  return(char_position = 0);
}

/* EM_INSERTCHAR -- Insert the space at the char_pos, shiftin all others */
static void
em_insertchar PARM_3(char *, line, int, pos, int, limit)
{
  int i;
  
  for (i = limit; i > pos; i--) {
    line[i] = line[i - 1];
  }
  line[i] = ' ';
  line[limit] = '\0';
}

/* EM_DELCHAR -- Delete the given character... shifting all others */
static int
em_delchar PARM_1(int, which)
{
  int byechar = '\0', i;
  char *ch_ptr;

  /* check the mode */
  switch (email_status % 4) {
  case EM_S_TOLINE:
    /* delete within the toline */
    if ((which < EM_MAXLOCKS) &&
	(cur_message->to_whom[which] != ABSMAXNTN)) {

      /* remove the current lock */
      email_unlock(which);

      /* traverse and shift copy the target nations */
      byechar = 'a';
      cur_message->to_whom[which] = 0; /* email_unlock sets
					  this to ABSMAXNTN */
      for (i = which;
	   (i < EM_MAXLOCKS) &&
	   (cur_message->to_whom[i] != ABSMAXNTN);
	   i++) {
	cur_message->to_whom[i] = cur_message->to_whom[i + 1];
	em_locks[i] = em_locks[i + 1];
      }
      if (i < EM_MAXLOCKS) {
	cur_message->to_whom[i] = ABSMAXNTN;
	em_locks[i] = -1;
      }

    } else {
      /* yell at 'em */
      beep();
    }
    break;
  case EM_S_SUBJECT:
    /* delete within the subject line */
    if (which < strlen(cur_message->subj)) {

      /* traverse and shift copy */
      ch_ptr = &(cur_message->subj[which]);
      byechar = *ch_ptr;
      while (*ch_ptr != '\0') {
	*ch_ptr = *(ch_ptr + 1);
	ch_ptr++;
      }

    } else {
      /* yell at 'em */
      beep();
    }
    break;
  case EM_S_BODY:
    /* delete within the body */
    if ((cur_line != NULL) &&
	(which < strlen(cur_line->line_data))) {

      /* traverse and shift copy */
      ch_ptr = &(cur_line->line_data[which]);
      byechar = *ch_ptr;
      while (*ch_ptr != '\0') {
	*ch_ptr = *(ch_ptr + 1);
	ch_ptr++;
      }

    } else {
      /* yell at 'em */
      beep();
    }
    break;
  }
  return(byechar);
}

/* EM_DELRIGHT -- Delete the character to the right */
static int
em_delright PARM_0(void)
{
  if (((email_status % 4) == EM_S_BODY) &&
      (char_position == 0) &&
      (cur_line->line_data[0] == '\0') &&
      (cur_line->next == NULL)) {
    return(em_exit());
  }
  return(em_delchar(char_position));
}

/* EM_DELLEFT -- Delete the character to the left */
static int
em_delleft PARM_0(void)
{
  /* check if a deletion is possible */
  if (char_position > 0) {

    /* check for being beyond end */
    if (email_status % 4 == EM_S_BODY) {
      if (char_position > strlen(cur_line->line_data)) {
	char_position = strlen(cur_line->line_data);
      }
    } else if ((email_status % 4 == EM_S_SUBJECT) &&
	       char_position > strlen(cur_message->subj)) {
      char_position = strlen(cur_message->subj);
    }
    return(em_delchar(--char_position));
  }
  beep();
  return('\0');
}

/* EM_PREKILL -- Remove all of the line to the left */
static int
em_prekill PARM_0(void)
{
  /* just delete 'em */
  while (char_position > 0) {
    em_delleft();
  }
  return(0);
}

/* EM_KILL -- Remove all of the line to the right */
static int
em_kill PARM_0(void)
{
  int i;

  switch (email_status % 4) {
  case EM_S_TOLINE:
    /* delete all to the right on the To: line */
    for (i = char_position;
	 i < EM_MAXLOCKS;
	 i++) {
      email_unlock(i);
    }
    break;
  case EM_S_SUBJECT:
    /* delete all to the right on the subject line */
    cur_message->subj[char_position] = '\0';
    break;
  case EM_S_BODY:
    /* do we kill the line? */
    if ((char_position == 0) &&
	(cur_line->line_data[0] == '\0') &&
	(cur_line->next != NULL)) {
      MAILD_PTR temp_ptr;

      /* remove the current line */
      temp_ptr = cur_line;
      if (cur_line->prev == NULL) {
	cur_message->text = cur_line->next;
      } else {
	cur_line->prev->next = cur_line->next;
      }
      cur_line = cur_line->next;
      cur_line->prev = temp_ptr->prev;

      /* now free it up */
      temp_ptr->prev = temp_ptr->next = NULL;
      free(temp_ptr->line_data);
      free(temp_ptr);

    } else {
      /* delete all on the current line to the right */
      cur_line->line_data[char_position] = '\0';
    }
    break;
  }
  return(0);
}

/* EM_UP -- Go up one line */
static int
em_up PARM_0(void)
{
  /* inside the body? */
  if ((email_status % 4) == EM_S_BODY) {

    /* bounce around */
    cur_line = cur_line->prev;
    if (cur_line == NULL) {
      email_status--;
      char_position = 0;
    }

  } else {

    if ((email_status % 4) > EM_S_TOLINE) {

      /* up to the next stage */
      email_status--;
      char_position = 0;

    } else {

      /* can't go anywhere */
      beep();

    }
  }
  return(0);
}

/* EM_DOWN -- Go down one line */
static int
em_down PARM_0(void)
{
  /* are we within the body */
  if ((email_status % 4) == EM_S_BODY) {

    /* just go forward */
    if (cur_line->next != NULL) {
      cur_line = cur_line->next;
    } else {
      beep();
    }

  } else {

    /* on to the next stage */
    email_status++;
    if ((email_status % 4) == EM_S_BODY) {
      cur_line = cur_message->text;
    } else {
      cur_line = NULL;
    }
    char_position = 0;

  }
  return(0);
}

/* EM_FORWARD -- Go forward one character */
static int
em_forward PARM_0(void)
{
  /* dependent on where we are */
  switch (email_status % 4) {
  case EM_S_TOLINE:
    /* traverse or go to the subject line */
    if ((char_position < EM_MAXLOCKS) &&
	(cur_message->to_whom[char_position] != ABSMAXNTN)) {
      char_position++;
    } else {
      em_down();
      em_tobol();
    }
    break;
  case EM_S_SUBJECT:
    /* traverse or go to the first body line */
    if (char_position < strlen(cur_message->subj)) {
      char_position++;
    } else {
      em_down();
      em_tobol();
    }
    break;
  case EM_S_BODY:
    /* traverse or go the next following line */
    if (char_position < strlen(cur_line->line_data)) {
      char_position++;
    } else if (cur_line->next != NULL) {
      em_down();
      em_tobol();
    } else {
      beep();
    }
    break;
  }
  return(0);
}

/* EM_BACKWARD -- Go backward one character */
static int
em_backward PARM_0(void)
{
  switch(email_status % 4) {
  case EM_S_TOLINE:
    /* go back along it */
    if (char_position > 0) {
      if ((char_position >= EM_MAXLOCKS) ||
	  (cur_message->to_whom[char_position] == ABSMAXNTN)) {
	em_toeol();
      }
      char_position--;
    } else {
      /* woops */
      beep();
    }
    break;
  case EM_S_SUBJECT:
    /* go back along the subject */
    if (char_position > 0) {
      if (char_position > strlen(cur_message->subj)) {
	char_position = strlen(cur_message->subj);
      }
      char_position--;
    } else {
      /* go to the To: line */
      em_up();
      em_toeol();
    }
    break;
  case EM_S_BODY:
    /* simply travel backwards */
    if (char_position > 0) {
      if (char_position > strlen(cur_line->line_data)) {
	char_position = strlen(cur_line->line_data);
      }
      char_position--;
    } else {
      /* go to the subject line */
      em_up();
      em_toeol();
    }
    break;
  }
  return(0);
}

/* EM_NEWLINE -- Break the line at the current point, creating a new line */
static int
em_newline PARM_0(void)
{
  MAILD_PTR temp_ptr;

  /* dependent on where we are */
  switch (email_status % 4) {
  case EM_S_TOLINE:
  case EM_S_SUBJECT:
    /* just go the the next line */
    em_down();
    break;
  case EM_S_BODY:
    /* check for past end of line */
    if (char_position > strlen(cur_line->line_data)) {
      char_position = strlen(cur_line->line_data);
    }

    /* now allocate space for a new line of data */
    if ((temp_ptr =
	 (MAILD_PTR) malloc(sizeof(MAILD_STRUCT))) == NULL) {
      errormsg("Major Error:  Memory Allocation Error");
      abrt();
    }
    if ((temp_ptr->line_data =
	 (char *) malloc(sizeof(char) * (email_collim + 1))) == NULL) {
      errormsg("Major Error:  Memory allocation error");
      abrt();
    }

    /* place it within the list */
    temp_ptr->next = cur_line->next;
    temp_ptr->prev = cur_line;
    cur_line->next = temp_ptr;
    if (temp_ptr->next != NULL) {
      temp_ptr->next->prev = temp_ptr;
    }

    /* copy it */
    strcpy(cur_line->next->line_data,
	   &(cur_line->line_data[char_position]));

    /* now adjust location */
    cur_line->line_data[char_position] = '\0';
    cur_line = cur_line->next;
    char_position = 0;
    break;
  }
  return(0);
}

/* EMAIL_ADDCHAR -- Add the given character to the output */
static void
email_addchar PARM_1(int, inp_ch)
{
  int i, j, was_eol = FALSE;

  switch (email_status % 4) {
  case EM_S_TOLINE:
    /* check the limit */
    if (char_position >= EM_MAXLOCKS) {
      errormsg("You may not send to any more nations");
      break;
    } else if (em_locks[char_position] != -1) {
      errormsg("You may only add nations to the end of the list");
      break;
    }

    /* check the type of character */
    switch (inp_ch) {
    case ' ':
    case ',':
      /* simply go and get the new name */
      break;
    default:
      /* just add it in */
      push_char(inp_ch);
      break;
    }

    /* find the current location and give a quick prompt */
    if (char_position > 0) addstr(", ");
    getyx(stdscr, j, i);

    /* display the bottom prompt line */
    errorbar("Mail Editor", "  Hit [RETURN] when done");

    /* now get the input */
    move(j, i);
    refresh();
    if ((i = get_country(TRUE)) == MAXNTN) {
      if (no_input == FALSE) {
	if (global_int == TRUE) next_char();
	errormsg("That nation does not seem to exist");
      }
      break;
    }

    /* now add it */
    if (email_lock(char_position, i) == TRUE) {
      char_position++;
    }
    break;
  case EM_S_SUBJECT:
    if (char_position > strlen(cur_message->subj)) {
      char_position = strlen(cur_message->subj);
    }
    /* check the limit */
    if (char_position >= email_subjlim) {
      beep();
      break;
    }

    /* check the boundary */
    if (cur_message->subj[char_position] == '\0') {
      was_eol = TRUE;
    }

    /* add to the line in the body */
    if (inp_ch == '\t') {
      do {
	if (cur_message->subj[char_position] == '\0') {
	  was_eol = TRUE;
	}
	if (email_status & EM_S_INSERTON && !was_eol) {
	  em_insertchar(cur_message->subj, char_position, email_subjlim);
	}
	cur_message->subj[char_position++] = ' ';
      } while ((char_position < email_subjlim) &&
	       ((char_position % 8) != 0));
    } else if (inp_ch == ' ') {
      if (email_status & EM_S_INSERTON && !was_eol) {
	em_insertchar(cur_message->subj, char_position, email_subjlim);
      }
      cur_message->subj[char_position++] = ' ';
    } else if (isprint(inp_ch) &&
	       !isspace(inp_ch)) {
      if (email_status & EM_S_INSERTON && !was_eol) {
	em_insertchar(cur_message->subj, char_position, email_subjlim);
      }
      cur_message->subj[char_position++] = inp_ch;
    } else {
      /* ain't a good character */
      beep();
      break;
    }

    /* check if that was the end of the line */
    if (was_eol == TRUE) {
      cur_message->subj[char_position] = '\0';
    }
    break;
  case EM_S_BODY:
    if (char_position > strlen(cur_line->line_data)) {
      char_position = strlen(cur_line->line_data);
    }
    /* check the limit */
    if (char_position >= email_collim) {
      beep();
      break;
    }

    /* check the boundary */
    if (cur_line->line_data[char_position] == '\0') {
      was_eol = TRUE;
    }

    /* add to the line in the body */
    if (inp_ch == '\t') {
      do {
	if (cur_line->line_data[char_position] == '\0') {
	  was_eol = TRUE;
	}
	if (email_status & EM_S_INSERTON && !was_eol) {
	  em_insertchar(cur_line->line_data, char_position, email_collim);
	}
	cur_line->line_data[char_position++] = ' ';
      } while ((char_position < email_collim) &&
	       ((char_position % 8) != 0));
    } else if (inp_ch == ' ') {
      if (email_status & EM_S_INSERTON && !was_eol) {
	em_insertchar(cur_line->line_data, char_position, email_collim);
      }
      cur_line->line_data[char_position++] = ' ';
    } else if (isprint(inp_ch) &&
	       !isspace(inp_ch)) {
      if (email_status & EM_S_INSERTON && !was_eol) {
	em_insertchar(cur_line->line_data, char_position, email_collim);
      }
      cur_line->line_data[char_position++] = inp_ch;
    } else {
      /* ain't a good character */
      beep();
      break;
    }

    /* check if that was the end of the line */
    if (was_eol == TRUE) {
      cur_line->line_data[char_position] = '\0';
    }
    break;
  }
}

/* EM_HELP -- Show some documentation */
static int
em_help PARM_0(void)
{
  /* create the help system */
  create_help("Conquer Mail Editor Command List",
	      email_bindings, email_funcs, email_keysys.num_parse);
  return(0);
}

/* EMS_SUBJ -- Show the subject */
static void
ems_subj PARM_1(int, line)
{
  mvprintw(line, 0, "Subject: %.*s", COLS - 9, cur_message->subj);
}

/* EMAIL_SHOW -- Display the current message */
static int
email_show PARM_0(void)
{
  char tostr[(NAMELTH + 3) * EM_MAXLOCKS + 10], nstr[NAMELTH + 1];
  int tolines = 1, base;
  int xpos = 0, ypos = 0, count1, count2;
  MAILD_PTR travel_line;

  /* build the To: line? or simply display it? */
  base = email_status % 4;

  /* build/display the To: line */
  count1 = 4;
  strcpy(tostr, "To: ");
  for (count2 = 0; count2 < EM_MAXLOCKS; count2++) {

    /* check for the end of it */
    if (cur_message->to_whom[count2] == ABSMAXNTN) {
      if ((base == EM_S_TOLINE) &&
	  (count2 == char_position)) {
	xpos = count1;
	ypos = tolines - 1;
      }
      break;
    }

    /* add it on */
    if (count2 != 0) {
      strcat(tostr, ", ");
      count1 += 2;
    }
    (void) ntn_realname(&(nstr[0]), cur_message->to_whom[count2]);
    if (count1 + strlen(nstr) + 2 > COLS - 8) {
      mvaddstr(tolines - 1, 0, tostr);
      strcpy(tostr, "    ");
      count1 = 4;
      tolines++;
    }

    /* check positioning */
    if ((base == EM_S_TOLINE) &&
	(count2 == char_position)) {
      xpos = count1;
      ypos = tolines - 1;
    }

    /* now tag it on */
    strcat(tostr, nstr);
    count1 += strlen(nstr);
  }

  /* show the bottom To: line */
  mvaddstr(tolines - 1, 0, tostr);
  if (base == EM_S_TOLINE) {
    mvaddstr(LINES - 1, 0,
  "Enter a nation, \"god\" (to mail deity), or \"news\" (to send a personal)");
  }

  /* now handle the subject line */
  if (base == EM_S_SUBJECT) {
    xpos = char_position + 9;
    ypos = tolines;
  }
  ems_subj(tolines);

  /* now prepare the body */
  if (base != EM_S_BODY) {

    /* setup the body */
    travel_line = cur_message->text;

  } else {

    /* find the length of the body upto and including current line */
    count1 = 1;
    for (travel_line = cur_line;
	 travel_line->prev != NULL;
	 travel_line = travel_line->prev) {
      count1++;
    }

    /* may only fit so many lines */
    if (count1 > LINES - 7 - tolines) {

      /* indicate that there is more to it */
      mvaddstr(tolines + 1, 0, "...cont...");

      /* only allow so many lines */
      count1 %= (LINES - 7 - tolines);
      count1++;
      for (travel_line = cur_line;
	   travel_line->prev != NULL;
	   travel_line = travel_line->prev) {
	if (--count1 == 0) break;
      }

    }

  }

  /* now show the body */
  count2 = tolines + 2;
  for (; travel_line != NULL; travel_line = travel_line->next) {
    if ((base == EM_S_BODY) &&
	(travel_line == cur_line)) {
      xpos = min(char_position, strlen(travel_line->line_data));
      ypos = count2;
    }
    mvaddstr(count2++, 0, travel_line->line_data);
    if (count2 == LINES - 5) break;
  }
  if ((travel_line != NULL) &&
      (travel_line->next != NULL)) {
    mvaddstr(count2, 0, "...more...");
  }

  /* display the bottom prompt line */
  errorbar("Mail Editor", "Hit \"ESC-?\" for Bindings");

  /* now relocate the cursor */
  move(ypos, xpos);
  refresh();
  return(0);
}

/* EMAIL_PREP -- Perform initialization of the mail message */
static void
email_prep PARM_0(void)
{
  int i;

  /* who is it to? */
  global_int = FALSE;
  for (i = 0; i < EM_MAXLOCKS; i++) {
    em_locks[i] = -1;
    if (cur_message->to_whom[i] != ABSMAXNTN) {
      if (email_lock(i, cur_message->to_whom[i]) != TRUE) {
	cur_message->to_whom[i] = ABSMAXNTN;
      }
    }
  }

  /* who is it???? */
  if (cur_message->sender == NULL) {
    /* allocate the space */
    if ((cur_message->sender =
	 (char *) malloc((NAMELTH + 1) * sizeof(char))) == NULL) {
      errormsg("MAJOR ERROR: Memory Allocation Error");
      abrt();
    }
    strcpy(cur_message->sender, nationname);
  }
  if (cur_message->nickname == NULL) {
    /* allocate the space */
    if ((cur_message->nickname =
	 (char *) malloc(LINELTH * sizeof(char))) == NULL) {
      errormsg("MAJOR ERROR: Memory Allocation Error");
      abrt();
    }
    if (country == UNOWNED) {
      switch (rand_val(3)) {
      case 0:
	strcpy(cur_message->nickname, "The All Knowing");
      case 1:
	strcpy(cur_message->nickname, "The All Mighty");
      case 2:
	strcpy(cur_message->nickname, "The All Powerful");
	break;
      }
    } else {
      strcpy(cur_message->nickname, ntn_ptr->leader);
    }
  }

  /* check what it is about */
  if (cur_message->subj == NULL) {

    /* allocate the space */
    if ((cur_message->subj =
	 (char *) malloc((email_subjlim + 1) * sizeof(char))) == NULL) {
      errormsg("MAJOR ERROR: Memory Allocation Error");
      abrt();
    }
    cur_message->subj[0] = '\0';

  }

  /* check if there is any text yet */
  if (cur_message->text == NULL) {

    /* allocate it */
    if ((cur_message->text =
	 (MAILD_PTR) malloc( sizeof(MAILD_STRUCT) )) == NULL) {
      errormsg("MAJOR ERROR: Memory Allocation Error");
      abrt();
    }
    if ((cur_message->text->line_data =
	 (char *) malloc( (email_collim + 1) * sizeof(char) )) == NULL) {
      errormsg("MAJOR ERROR: Memory Allocation Error");
      abrt();
    }
    cur_message->text->line_data[0] = '\0';
    cur_message->text->prev = NULL;
    cur_message->text->next = NULL;

  }

  /* assign bindings */
  if (email_bindings == NULL) {
    email_init();
  }
}

#ifdef ALLOW_EDIT_FORK
/* EMAIL_TOEDIT -- Write the current mail message to the given file */
static int
email_toedit PARM_1(char *, fname)
{
  FILE *fpout;
  char temp_str[BIGLTH];
  MAILD_PTR travel_line;
  int i;

  /* open the output file */
  if ((fpout = fopen(fname, "w")) == NULL) {
    /* oops */
    errormsg("Error: Unable to open the temporary file for editing");
    return(TRUE);
  }

  /* write out the header */
  fprintf(fpout, "To: ");
  for (i = 0; i < EM_MAXLOCKS; i++) {
    if (cur_message->to_whom[i] == ABSMAXNTN) break;
    if (i > 0) fprintf(fpout, ", ");
    (void) ntn_realname(&(temp_str[0]), cur_message->to_whom[i]);
    fprintf(fpout, temp_str);
  }
  fprintf(fpout, "\nSubject: %s\n\n", cur_message->subj);

  /* now show the body */
  for (travel_line = cur_message->text;
       travel_line != NULL;
       travel_line = travel_line->next) {
    fprintf(fpout, "%s\n", travel_line->line_data);
  }

  /* finish it */
  fclose(fpout);
  return(FALSE);
}

/* EMAIL_FROMEDIT -- Read the current mail message from the given file */
static void
email_fromedit PARM_1(char *, fname)
{
  FILE *fpin;
  MAILD_PTR travel_ptr, placement_ptr;
  int i, j, numtos = 0, lval, first_blank = FALSE;
  int found_to = FALSE, found_subj = FALSE;
  char line[BIGLTH], nname[LINELTH];

  /* open the file */
  if ((fpin = fopen(fname, "r")) == NULL) return;

  /* clear up the old space */
  cur_message->subj[0] = '0';
  for (i = 0; i < EM_MAXLOCKS; i++) {
    if (cur_message->to_whom[i] != ABSMAXNTN) {
      email_unlock(i);
    }
  }
  placement_ptr = cur_message->text;
  for (travel_ptr = placement_ptr;
       travel_ptr != NULL;
       travel_ptr = placement_ptr) {
    placement_ptr = placement_ptr->next;
    travel_ptr->next = NULL;
    if (travel_ptr->line_data != NULL) {
      free(travel_ptr->line_data);
    }
    free(travel_ptr);
  }

  /* read all of the data, line by line */
  while (!feof(fpin)) {

    /* read in a line of data */
    if (fgets(line, BIGLTH - 1, fpin) == NULL) break;
  more_to_line:
    lval = strlen(line);

    /* look for the To: field */
    if ((found_to == FALSE) &&
	(strncmp(line, "To:", 3) == 0)) {

      /* parse through all of the line */
      if (line[lval - 1] == '\n') {
	line[lval - 1] = '\0';
      }
      found_to = TRUE;
      j = 0;
      for (i = 3; i < lval; i++) {
	/* look for a name portion */
	if (isalnum(line[i])) {
	  nname[j++] = line[i];
	} else {
	  /* is there a name up to here? */
	  if (j == 0) continue;
	  nname[j] = '\0';
	  if ((ntnbyname(nname) == NULL) &&
	      (global_int == MAXNTN)) {
	    sprintf(string, "Unknown nation name <%s> encountered", nname);
	    errormsg(string);
	    continue;
	  }
	  if (numtos < EM_MAXLOCKS) {
	    cur_message->to_whom[numtos] = global_int;
	    email_lock(numtos, global_int);
	    numtos++;
	  }
	  nname[j = 0] = '\0';

	}
      }

    } else if ((found_subj == FALSE) &&
	       (strncmp(line, "Subject:", 8) == 0)) {

      /* get the subject line */
      if (line[lval - 1] == '\n') {
	line[lval - 1] = '\0';
      }
      trim_str(&(line[8]), FALSE);
      strncpy(cur_message->subj, &(line[8]), email_subjlim);
      cur_message->subj[email_subjlim] = '\0';
      found_subj = TRUE;

    } else {

      /* was the first line blank? */
      if (line[lval - 1] == '\n') {
	line[lval - 1] = '\0';
      }
      lval--;
      if (first_blank == FALSE) {
	first_blank = TRUE;
	if (line[0] == '\0') continue;
      }

      /* make the storage and copy the text */
      if ((travel_ptr = (MAILD_PTR) malloc( sizeof(MAILD_STRUCT) )) == NULL) {
	errormsg("MAJOR ERROR: Memory Allocation Error");
	abrt();
      }
      if ((travel_ptr->line_data =
	   (char *) malloc( (email_collim + 1) * sizeof(char) )) == NULL) {
	errormsg("MAJOR ERROR: Memory Allocation Error");
	abrt();
      }
      strncpy(travel_ptr->line_data, line, email_collim);
      travel_ptr->line_data[email_collim] = '\0';
      travel_ptr->next = NULL;

      /* now place it */
      if (placement_ptr == NULL) {
	cur_message->text = travel_ptr;
      } else {
	placement_ptr->next = travel_ptr;
      }
      travel_ptr->prev = placement_ptr;
      placement_ptr = travel_ptr;

      /* check if there is more to this line */
      if (lval > email_collim) {
	for (i = 0; i < email_collim; i++) {
	  line[i] = ' ';
	}
	trim_str(line, FALSE);
	goto more_to_line;
      }

    }
    
  }

  /* finish up */
  if ((cur_line = cur_message->text) == NULL) {
    if ((cur_line = (MAILD_PTR) malloc( sizeof(MAILD_STRUCT) )) == NULL) {
      errormsg("MAJOR ERROR: Memory Allocation Error");
      abrt();
    }
    if ((cur_line->line_data =
	 (char *) malloc( (email_collim + 1) * sizeof(char) )) == NULL) {
      errormsg("MAJOR ERROR: Memory Allocation Error");
      abrt();
    }
    cur_line->line_data[0] = '\0';
    cur_line->next = NULL;
    cur_line->prev = NULL;
    cur_message->text = cur_line;
  }
  char_position = 0;
  fclose(fpin);
  unlink(fname);
  clear();
}
#endif /* ALLOW_EDIT_FORK */

/* EM_SPAWNEDIT -- Spawn an editor to edit the file, if available */
static int
em_spawnedit PARM_0(void)
{
#ifdef ALLOW_EDIT_FORK
  char tmp_edit_filename[FILELTH];

  /* check the input */
  if (cur_message == NULL) {
    errormsg("Error: you have no current message to edit");
    return(0);
  }

  /* build the file name */
  sprintf(tmp_edit_filename, "%s.etmp", (ntn_ptr != NULL) ?
	  ntn_ptr->name : "god");
  
  /* first translate the mail message to normal text */
  if (email_toedit(tmp_edit_filename)) return(0);

  /* run the editor on it */
  fork_edit_on_file(tmp_edit_filename,
    "SPAWNING EDIT: Be sure that the To: line is properly formed.");

  /* now translate it back into the mail format */
  email_fromedit(tmp_edit_filename);
#endif /* ALLOW_EDIT_FORK */
  return(0);
}

/* EMAIL_PARSE -- Main interface for sending mail; message in cur_message */
void
email_parse PARM_0(void)
{
  FNCI fnc_ptr;

  /* initialize things */
  email_done = FALSE;
  email_collim = 80 - (strlen(nationname) + 4);
  mail_ind |= MAIL_SENDING;
  email_status = EM_S_TOLINE;
  email_status |= email_mode;
  email_prep();
  clear();

  /* have we finished? */
  while (email_done == FALSE) {

    /* display what there is of the message */
    move(0, 0);
    clrtobot();
    email_show();

    /* now get the input */
    if ((fnc_ptr = parse_keys(email_bindings, FALSE)) == NULL) {

      /* now is it an input key or is it memorex? */
      if (strlen(string) > 1) {
	/* memorex */
	clear_bottom(1);
	mvaddstr(LINES - 1, 0, "Unknown key binding: ");
	show_str(string, FALSE);
	presskey();
      } else {
	/* new input */
	email_addchar(string[0]);
      }

    } else {

      /* perform the function */
      (*fnc_ptr)();

    }
    
  }
  
  /* remove locks and free up the space */
  email_close();
}

/* EM_OPTIONS -- Quickie command to allow the setting of options */
static int
em_options PARM_0(void)
{
  option_cmd(email_keysys, &email_bindings);
  return(0);
}

/* function list */
PARSE_STRUCT email_funcs[] = {
  {em_options, "conquer-options",
     "Adjust the conquer environment by changing various options"},
  {do_ignore, "ignore-key",
     "Just pretend this key press didn't really happen"},
  {em_backward, "mail-backward",
     "Mail-mode, move cursor one to the left"},
  {em_tobol, "mail-beginning-of-line",
     "Mail-mode, move cursor to the beginning of the current line"},
/*{em_cmdline, "mail-command",
   "Mail-mode, execute a specified mail command by name"}, */
  {em_delleft, "mail-delete-backward",
     "Mail-mode, delete the character to the left of the cursor"},
  {em_delright, "mail-delete-forward",
     "Mail-mode, delete the character under the cursor"},
  {em_down, "mail-downline",
     "Mail-mode, move cursor down one line"},
  {em_toeol, "mail-end-of-line",
     "Mail-mode, move cursor to the end of the current line"},
  {em_exit, "mail-exit",
     "Mail-mode, deliver the message and then leave mail"},
  {em_forward, "mail-forward",
     "Mail-mode, move cursor one to the right"},
  {em_help, "mail-help",
     "Mail-mode, provide documentation on the mail mode"},
  {em_prekill, "mail-kill-to-beginning",
     "Mail-mode, delete from the cursor to the start of the current line"},
  {em_kill, "mail-kill-line",
     "Mail-mode, delete from the cursor to the end of the current line"},
  {em_newline, "mail-newline",
     "Mail-mode, break the current line at the cursor, creating a new line"},
  {em_quit, "mail-quit",
     "Mail-mode, leave mail without sending the message"},
  {em_send, "mail-send",
     "Mail-mode, send the message to all of the recipients"},
#ifdef ALLOW_EDIT_FORK
  {em_spawnedit, "mail-spawn-editor",
     "Mail-mode, use your personal EDITOR to edit your mail message"},
#else
  {em_spawnedit, "mail-spawn-editor",
     "Mail-mode, this command is not supported in this version of conquer"},
#endif /* ALLOW_EDIT_FORK */
  {em_toggle, "mail-toggle",
     "Mail-mode, toggle between overwrite and insert mode in the editor"},
  {em_up, "mail-upline",
     "Mail-mode, move cursor up one line"},
  {do_redraw, "redraw-screen",
     "redraw the screen display of the mail message"}
};

/* Default editor key bindings */
KBIND_STRUCT email_klist[] = {
  { ESTR_DEL, em_delleft },
  { "\n", em_newline },
  { "\r", em_newline },
  { CSTR_B, em_backward },
  { CSTR_C, do_ignore },
  { CSTR_D, em_delright },
  { CSTR_E, em_toeol },
  { CSTR_F, em_forward },
  { CSTR_G, em_quit },
  { CSTR_K, em_kill },
  { CSTR_L, do_redraw },
  { CSTR_O, em_options },
  { CSTR_N, em_down },
  { CSTR_P, em_up },
  { CSTR_R, do_redraw },
  { CSTR_U, em_prekill },
  { CSTR_W, em_send },
  { CSTR_X, em_exit },
#ifdef VMS
  { CSTR_A, em_toggle },
  { CSTR_H, em_tobol },
  { CSTR_Z, em_exit },
#else
  { CSTR_A, em_tobol },
  { CSTR_H, em_delleft },
  { CSTR_T, em_toggle },
#endif /* VMS */
  { "\033?", em_help },
/*{ "\033x", em_cmdline }, */
  { "\033$", em_spawnedit },
  { AKEY_UP1, em_up },
  { AKEY_UP2, em_up },
  { AKEY_DOWN1, em_down },
  { AKEY_DOWN2, em_down },
  { AKEY_RIGHT1, em_forward },
  { AKEY_RIGHT2, em_forward },
  { AKEY_LEFT1, em_backward },
  { AKEY_LEFT2, em_backward }
};

/* ALIGN_EMAIL_KEYS -- Align all of the mail editor keys */
void
align_email_keys PARM_0(void)
{
  /* initialize the system */
  if (email_bindings == NULL) {
    email_keysys.num_binds = (sizeof(email_klist)/sizeof(KBIND_STRUCT));
    email_keysys.num_parse = (sizeof(email_funcs)/sizeof(PARSE_STRUCT));
    init_keys(&email_bindings, email_klist, email_keysys.num_binds);
  }
}
