// SPDX-License-Identifier: GPL-3.0-or-later
/* user interface routines that need to be "defined" for both */
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
#include "armyX.h"
#include "moveX.h"
#include "desigX.h"
#include "magicX.h"
#include "spellsX.h"
#include "tgoodsX.h"
#include "displayX.h"
#include "optionsX.h"

/* TARGET_VALUE -- Conversion routine to parse string from target */
static int
target_value PARM_2(int, hstyle, char *, str)
{
  int i;

  switch (hstyle) {
  case HI_MINDESG:
    for (i = 0; i < MIN_NUMBER; i++) {
      if (str_test(min_dinfo[i].name, str) == 0) {
	return(i);
      }
    }
    break;
  case HI_MAJDESG:
    for (i = 0; i < MAJ_NUMBER; i++) {
      if (str_test(maj_dinfo[i].name, str) == 0) {
	return(i);
      }
    }
    break;
  case HI_OWN:
    if (strcmp(str, "*") == 0) return(UNOWNED);
    for (i = 0; i < MAXNTN; i++) {
      if (str_test((world.np[i])->name, str) == 0) {
	return(i);
      }
    }
    break;
  case HI_TGOODS:
    if (strcmp(str, "*") == 0) return(TG_NONE);
    for (i = 0; i < tgclass_number; i++) {
      if (str_test(tgclass_info[i].name, str) == 0) {
	return(i);
      }
    }
    break;
  default:
    /* targeting value of zippo */
    return(0);
    /*NOTREACHED*/
    break;
  }

  /* no match */
  return(-2);
}

/* DFLT_DISP_SETUP -- Build up one of the settings of the default display */
void
dflt_disp_setup PARM_3(char *, str, char *, fstr, int, lnum)
{
  int fposit, len, dstyle, hstyle, targ;

  /* parse the string and break it into the appropriate parts */
  if (strncmp(str, "highlight", 9) == 0) {

    /* clear out the front */
    str += 9;
    trim_str(str, FALSE);

    /* now check for the different positions */
    for (fposit = 0; fposit < HXPOS_NUMBER; fposit++) {
      len = strlen(hex_list[fposit]);
      if (strncmp(str, hex_list[fposit], len) == 0) break;
    }

    /* was it invalid? */
    if (fposit == HXPOS_NUMBER) {

      /* invalid */
      sprintf(string, "%s [%d]: unknown position for highlight",
	      fstr, lnum);
      errormsg(string);

    } else {

      /* clean up the string for parsing */
      str += len;
      trim_str(str, FALSE);

      /* now locate the highlight type */
      for (hstyle = 0; hstyle <= HI_MAXIMUM; hstyle++) {
	if (strcmp(str, highl_list[hstyle]) == 0) break;
      }

      /* valid or not? */
      if (hstyle == HI_MAXIMUM + 1) {
	sprintf(string, "%s [%d]: unknown highlighting style option",
		fstr, lnum);
	errormsg(string);
      } else {
	/* do it */
	display_mode.highlight[fposit] = hstyle;
      }
    }

  } else if (strncmp(str, "target", 6) == 0) {

    /* clear out the front */
    str += 6;
    trim_str(str, FALSE);

    /* now check for the different positions */
    for (fposit = 0; fposit < HXPOS_NUMBER; fposit++) {
      len = strlen(hex_list[fposit]);
      if (strncmp(str, hex_list[fposit], len) == 0) break;
    }

    /* was it invalid? */
    if (fposit == HXPOS_NUMBER) {

      /* invalid */
      sprintf(string, "%s [%d]: unknown position for target",
	      fstr, lnum);
      errormsg(string);

    } else if (hl_targets(display_mode.highlight[fposit]) == FALSE) {

      /* no target highlighting */
      sprintf(string, "%s [%d]: %s highlight doesn't need target",
	      fstr, lnum, hex_list[fposit]);
      errormsg(string);

    } else {

      /* clean up the string for parsing */
      str += len;
      trim_str(str, FALSE);

      /* now find the target value */
      if ((targ = target_value(display_mode.highlight[fposit], str))
	  != -2) {
	display_mode.target[fposit] = targ;
      }
    }

  } else if (strncmp(str, "focus", 5) == 0) {

    /* clear out start of string */
    str += 5;
    trim_str(str, FALSE);

    /* need to find the match for the focus position */
    for (fposit = 0; fposit < HXPOS_NUMBER; fposit++) {
      len = strlen(hex_list[fposit]);
      if (strncmp(str, hex_list[fposit], len) == 0) break;
    }

    /* was there a match? */
    if (fposit == HXPOS_NUMBER) {

      /* bad setting */
      sprintf(string, "%s [%d]: unknown focus position",
	      fstr, lnum);
      errormsg(string);

    } else {
      /* got it */
      display_mode.focus = fposit;
    }

  } else {
    /* now check for the different positions */
    for (fposit = 0; fposit < HXPOS_NUMBER; fposit++) {
      len = strlen(hex_list[fposit]);
      if (strncmp(str, hex_list[fposit], len) == 0) break;
    }

    /* was it invalid? */
    if (fposit == HXPOS_NUMBER) {

      /* invalid */
      sprintf(string, "%s [%d]: unknown default-display option",
	      fstr, lnum);
      errormsg(string);

    } else {

      /* clean up the string for parsing */
      str += len;
      trim_str(str, FALSE);

      /* find the display mode */
      for (dstyle = 0; dstyle <= DI_MAXIMUM; dstyle++) {
	if (strcmp(str, display_list[dstyle]) == 0) break;
      }

      /* valid or not? */
      if (dstyle == DI_MAXIMUM + 1) {
	sprintf(string, "%s [%d]: unknown display style option",
		  fstr, lnum);
	errormsg(string);
      } else {
	/* do it */
	display_mode.style[fposit] = dstyle;
      }
      
    }
  }
}

/* DISPLAY_SETUP -- Configure the display modes */
void
display_setup PARM_3(char *, str, char *, fstr, int, lnum)
{
  DMODE_PTR work_ptr;
  int fposit, len = 0, dstyle, hstyle, targ;
  char dmodename[DISPLAYLTH+1];

  /* locate the name of the display */
  for (; *str != '\0'; str++) {
    if (*str == '"') break;
  }
  if (*str == '\0') return;
  for (str++; (*str != '\0') && (*str != '"'); str++) {
    dmodename[len++] = *str;
  }

  /* check it */
  if (*str == '\0') {
    /* just weren't nothing there */
    sprintf(string, "%s [%d]: could not find display mode name",
	    fstr, lnum);
    errormsg(string);
    return;
  }

  /* end name and skip over final quote */
  dmodename[len] = '\0';
  str++;
  trim_str(str, FALSE);

  /* now find/create the display mode */
  if ((work_ptr = crt_dmode(dmodename)) == NULL) {
    return;
  }

  /* parse the string and break it into the appropriate parts */
  if (strncmp(str, "highlight", 9) == 0) {

    /* clear out the front */
    str += 9;
    trim_str(str, FALSE);

    /* now check for the different positions */
    for (fposit = 0; fposit < HXPOS_NUMBER; fposit++) {
      len = strlen(hex_list[fposit]);
      if (strncmp(str, hex_list[fposit], len) == 0) break;
    }

    /* was it invalid? */
    if (fposit == HXPOS_NUMBER) {

      /* invalid */
      sprintf(string, "%s [%d]: unknown position for highlight",
	      fstr, lnum);
      errormsg(string);

    } else {

      /* clean up the string for parsing */
      str += len;
      trim_str(str, FALSE);

      /* now locate the highlight type */
      for (hstyle = 0; hstyle <= HI_MAXIMUM; hstyle++) {
	if (strcmp(str, highl_list[hstyle]) == 0) break;
      }

      /* valid or not? */
      if (hstyle == HI_MAXIMUM + 1) {
	sprintf(string, "%s [%d]: unknown highlighting style option",
		fstr, lnum);
	errormsg(string);
      } else {
	/* do it */
	work_ptr->d.highlight[fposit] = hstyle;
      }
    }

  } else if (strncmp(str, "target", 6) == 0) {

    /* clear out the front */
    str += 6;
    trim_str(str, FALSE);

    /* now check for the different positions */
    for (fposit = 0; fposit < HXPOS_NUMBER; fposit++) {
      len = strlen(hex_list[fposit]);
      if (strncmp(str, hex_list[fposit], len) == 0) break;
    }

    /* was it invalid? */
    if (fposit == HXPOS_NUMBER) {

      /* invalid */
      sprintf(string, "%s [%d]: unknown position for target",
	      fstr, lnum);
      errormsg(string);

    } else if (hl_targets(work_ptr->d.highlight[fposit]) == FALSE) {

      /* no target highlighting */
      sprintf(string, "%s [%d]: %s highlight doesn't need target",
	      fstr, lnum, hex_list[fposit]);
      errormsg(string);

    } else {

      /* clean up the string for parsing */
      str += len;
      trim_str(str, FALSE);

      /* now find the target value */
      if ((targ = target_value(work_ptr->d.highlight[fposit], str))
	  != -2) {
	work_ptr->d.target[fposit] = targ;
      }
    }

  } else if (strncmp(str, "focus", 5) == 0) {

    /* clear out start of string */
    str += 5;
    trim_str(str, FALSE);

    /* need to find the match for the focus position */
    for (fposit = 0; fposit < HXPOS_NUMBER; fposit++) {
      len = strlen(hex_list[fposit]);
      if (strncmp(str, hex_list[fposit], len) == 0) break;
    }

    /* was there a match? */
    if (fposit == HXPOS_NUMBER) {

      /* bad setting */
      sprintf(string, "%s [%d]: unknown focus position",
	      fstr, lnum);
      errormsg(string);

    } else {
      /* got it */
      work_ptr->d.focus = fposit;
    }

  } else {
    /* now check for the different positions */
    for (fposit = 0; fposit < HXPOS_NUMBER; fposit++) {
      len = strlen(hex_list[fposit]);
      if (strncmp(str, hex_list[fposit], len) == 0) break;
    }

    /* was it invalid? */
    if (fposit == HXPOS_NUMBER) {

      /* invalid */
      sprintf(string, "%s [%d]: unknown display-mode option",
	      fstr, lnum);
      errormsg(string);

    } else {

      /* clean up the string for parsing */
      str += len;
      trim_str(str, FALSE);

      /* find the display mode */
      for (dstyle = 0; dstyle <= DI_MAXIMUM; dstyle++) {
	if (strcmp(str, display_list[dstyle]) == 0) break;
      }

      /* valid or not? */
      if (dstyle == DI_MAXIMUM + 1) {
	sprintf(string, "%s [%d]: unknown display style option",
		fstr, lnum);
	errormsg(string);
      } else {
	/* do it */
	work_ptr->d.style[fposit] = dstyle;
      }
      
    }
  }
}

/* KEYSYS_SETUP -- Configure the keybindings */
void
keysys_setup PARM_4(int, action, char *, str, char *, fstr, int, lnum)
{
  KEYSYS_PTR curmap_keysys;
  KLIST_PTR *list_of_keys, klist_ptr;
  FNCI func_ptr;
  char start_ch, ksysname[LINELTH], keystr[LINELTH];
  int len = 0;

  /* first find the type of the keybindings */
  for (; !isspace(*str); str++) {
    ksysname[len++] = *str;
  }
  ksysname[len] = '\0';

  /* check if we found a system name */
  if (len == 0) {
    sprintf(string, "%s [%d]: could not find keypad type",
	    fstr, lnum);
    errormsg(string);
  }
  len = 0;

  /* now compare with the different types */
  if (strcmp(ksysname, global_keysys.name) == 0) {
    curmap_keysys = &global_keysys;
    list_of_keys = &cq_bindings;
  } else if (strcmp(ksysname, email_keysys.name) == 0) {
    curmap_keysys = &email_keysys;
    list_of_keys = &email_bindings;
  } else if (strcmp(ksysname, reader_keysys.name) == 0) {
    curmap_keysys = &reader_keysys;
    list_of_keys = &reader_bindings;
  } else if (strcmp(ksysname, magic_keysys.name) == 0) {
    curmap_keysys = &magic_keysys;
    list_of_keys = &mg_bindings;
  } else if (strcmp(ksysname, mparse_keysys.name) == 0) {
    curmap_keysys = &mparse_keysys;
    list_of_keys = &mparse_bindings;
  } else if (strcmp(ksysname, ninfo_keysys.name) == 0) {
    curmap_keysys = &ninfo_keysys;
    list_of_keys = &ni_bindings;
  } else if (strcmp(ksysname, xfer_keysys.name) == 0) {
    curmap_keysys = &xfer_keysys;
    list_of_keys = &xfer_bindings;
  } else {
    sprintf(string, "%s [%d]: unknown key map type encountered",
	    fstr, lnum);
    errormsg(string);
    return;
  }
  
  /* check the beginning of the line */
  trim_str(str, FALSE);
  if (((start_ch = *str) != '"') &&
      (start_ch != '\'')) {
    sprintf(string, "%s [%d]: could not find key binding",
	    fstr, lnum);
    errormsg(string);
    return;
  }

  /* get the keybinding */
  for (str++; (*str != '\0') && (*str != start_ch); str++) {
    if ((*str == '\\') && (*(str + 1) != '\0')) {
      keystr[len++] = *str;
      str++;
    }
    keystr[len++] = *str;
  }
  keystr[len] = '\0';

  /* was it found? */
  if (*str == '\0') {
    sprintf(string, "%s [%d]: could not find key binding",
	    fstr, lnum);
    errormsg(string);
    return;
  }

  /* clean up the key binding */
  convert_kbind(keystr);

  /* clean up the string, which should now only hold a function, if needed */
  str++;
  trim_str(str, FALSE);

  /* separate based on type */
  switch (action) {
  case OPT_BINDKEY:
  case OPT_REBIND:
    /* (re)bind the key */
    klist_ptr = find_keys(*list_of_keys, keystr);

    /* get the function */
    if ((func_ptr = func_match(str, curmap_keysys->parse_p,
			       curmap_keysys->num_parse)) == NULL) {
      sprintf(string, "%s [%d]: unknown %s function", fstr,
	      lnum, ksysname);
      errormsg(string);
      break;
    }

    /* now do the binding */
    if (klist_ptr == NULL) {
      bind_keys(list_of_keys, keystr, func_ptr);
    } else {
      klist_ptr->key_data.func = func_ptr;
    }
    break;
  case OPT_UNBIND:
    /* nuke it */
    (void) rm_keys(list_of_keys, keystr);
    break;
  default:
    /* strange */
    break;
  }
}

/* CHECK_SPELLS -- check the spell list */
void
check_spells PARM_3(int, spellnum, int, xloc, int, yloc)
{
  /* check it */
  if (spellnum > spell_number) {
    /* unknown spell? */
    sprintf(string, "Error: Unknown spell #%d in [%d,%d]",
	    spellnum, xloc, yloc);
    errormsg(string);
  }
}

/* temporary global to enable transition of function lists */
PARSE_PTR tmp_parsep;

/* BIND_FUNC -- return a string name for the given function */
char *
bind_func PARM_1(int, which)
{
  if ((which < 0) || (which > global_int))
    return((char *) NULL);
  return(tmp_parsep[which].realname);
}

/* HANGUP -- signal catching routine */
void
hangup PARM_0(void)
{
  /* finish up the nation */
  if (movemode != MOVE_NOMOVE) {
    move_relocate(movemode, FALSE);
  }    
  close_ntn(country, nationname);

  /* remove any existing mail reading/writing file */
  if (mail_ind & MAIL_SENDING) {
    /* clean up lock on target nation */
    email_close();
  }
  if (mail_ind & MAIL_READING) {
    /* save the mail messages and remove the lock file */
    rmail_close();
  }

  /* exit program */
  exit(FAIL);
}
