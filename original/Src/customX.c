/* simple routines to allow read and write a configuration file */
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
#include "dataX.h"
#include "desigX.h"
#include "elevegX.h"
#include "optionsX.h"

/* READ_ENVIRON -- Parse the environment variable used by conquer */
void
read_environ PARM_0(void)
{
  int i, j, l, val;
  char cq_opts[BIGLTH];

  /* perform identity checking */
  if (user_exists(LOGIN) == FALSE) {
    fprintf(fupdate, "DANGER!!! DANGER!!!!  DANGER, WILL ROBINSON!!!!\n");
    fprintf(fupdate, "MY SENSORS DETECT THAT THERE IS NO GOD!!!!\n\n");
    fprintf(fupdate, "\t===  Please recompile after redefining the  ===\n");
    fprintf(fupdate, "\t=== LOGIN user name in the file \"header.h\". ===\n");
    exit(FAIL);
  }

  /* get user login name */
  if ((get_userid(loginname) == NULL) || (strlen(loginname) == 0)) {
    fprintf(fupdate, "Cannot get your login name!  Who are you?\n");
    exit(FAIL);
  }

  /* set general environment things */
  datadir[0] = '\0';
  datadirname[0] = '\0';
  defaultdir[0] = '\0';
  helpdir[0] = '\0';
  progdir[0] = '\0';

  /* set the defaults from the environment */
  if (getenv(ENVIRON_DFLT)) {
    strcpy(defaultdir, getenv(ENVIRON_DFLT));
  } else {
    strcpy(defaultdir, DEFAULTDIR);
  }
  if (getenv(ENVIRON_HELP)) {
    strcpy(helpdir, getenv(ENVIRON_HELP));
  } else {
    strcpy(helpdir, DEFAULTDIR);
  }
  if (getenv(ENVIRON_EXEC)) {
    strcpy(progdir, getenv(ENVIRON_EXEC));
  } else {
    strcpy(progdir, EXEDIR);
  }

  /* reset parameters */
  cq_opts[0] = '\0';
  val = TRUE;

  /* check environment */
  if (getenv (ENVIRON_OPTS) != NULL) {
    strncpy (cq_opts, getenv (ENVIRON_OPTS), BIGLTH - 1);
  }

  /* parse it */
  if (cq_opts[0] != '\0') {
    l = strlen (cq_opts);
    for (i = 0; i < l; i++) {
      switch (cq_opts[i]) {
      case '!':
	/* negate a setting */
	val = FALSE;
	continue;
      case 'B':
	/* set all blanks */
	conq_allblanks = val;
	break;
      case 'b':
	/* set bottom lines */
	conq_bottomlines = val;
	break;
      case 'E':
      case 'e':
	/* set the system mail check */
	dosysm_check = val;
	break;
      case 'G':
      case 'g':
        /* set gaudy display */
	conq_gaudy = val;
        break;
      case 'H':
      case 'h':
	/* set the header mode */
	conq_mheaders = val;
	break;
      case 'I':
      case 'i':
	conq_infomode = val;
	break;
      case 'T':
      case 't':
	conq_beeper = val;
	break;
      case 'W':
      case 'w':
	/* the water bottoms */
	conq_waterbottoms = val;
	break;
      case 'X':
      case 'x':
	conq_expert = val;
	break;
      case 'N':
      case 'n':
        /* read in the nation name */
        if (strncmp (cq_opts + i + 1, "ation=", 6) == 0) {
          i += 7;
	} else if (strncmp (cq_opts + i + 1, "ationname=", 10) == 0) {
	  i += 11;
	} else if (strncmp (cq_opts + i + 1, "ame=", 4) == 0) {
	  i += 5;
	} else {
	  fprintf (fupdate, "conquer: invalid environment\n");
	  fprintf (fupdate, "\t%s = %s\n", ENVIRON_OPTS, cq_opts);
	  fprintf (fupdate, "\texpected <nation=NAME>\n");
	  exit (FAIL);
	}
        if (i < l) {
          /* grab the nation name */
          for (j = 0; j < l - i && cq_opts[i + j] != ','; j++) {
            nationname[j] = cq_opts[i + j];
          }
          nationname[j] = '\0';
          i += j - 1;
        }
        break;
      case 'D':
      case 'd':
        /* check for data directory */
        if (strncmp (cq_opts + i + 1, "ata=", 4) == 0) {
          i += 5;
        } else if (strncmp (cq_opts + i + 1, "atadir=", 7) == 0) {
	  i += 8;
	} else if (strncmp (cq_opts + i + 1, "irectory=", 9) == 0) {
	  i += 10;
	} else if (strncmp (cq_opts + i + 1, "ir=", 3) == 0) {
	  i += 4;
	} else {
	  fprintf (fupdate, "conquer: invalid environment\n");
	  fprintf (fupdate, "\t%s = %s\n", ENVIRON_OPTS, cq_opts);
	  fprintf (fupdate, "\texpected <data=DIRNAME>\n");
	  exit (FAIL);
	}
        if (i < l) {
          /* grab the data directory */
          for (j = 0; j < l - i && cq_opts[i + j] != ','; j++) {
            datadirname[j] = cq_opts[i + j];
          }
          datadirname[j] = '\0';
          i += j - 1;
        }
        break;
      case ' ':
      case ',':
        /* ignore commas and spaces */
        break;
      default:
        /* complain */
        fprintf (fupdate, "conquer: invalid environment\n");
        fprintf (fupdate, "\t%s = %s\n", ENVIRON_OPTS, cq_opts);
        fprintf (fupdate, "\tunexpected option <%c>\n", cq_opts[i]);
        exit (FAIL);
        break;
      }

      /* now reset the value */
      val = TRUE;
    }
  }
}

/* INIT_DATADIR -- Assign the value of datadir based on datadirname */
void
init_datadir PARM_0(void)
{
#ifndef VMS
  if (datadirname[0] != '/') {
#endif /* VMS */
    if (strlen (datadirname) > 0) {
#ifndef VMS
      sprintf (datadir, "%s/%s", defaultdir, datadirname);
#else
      strcpy(datadir, datadirname);
#endif /* VMS */
    } else {
      strcpy (datadir, defaultdir);
      strcpy (datadirname, "[default]");
    }
#ifndef VMS
  } else {
    strcpy (datadir, datadirname);
  }
#endif /* VMS */
}

/* depth checking */
static int cust_depth = 0;

/* READ_CUSTOM -- This function reads in the list of conquer options */
int
read_custom PARM_1(char *, f_str)
{
  FILE *cust_fp;
  char line[LINELTH + 1];
  char fname[LINELTH];
  int linenum = 0, count, len, value_num;
#ifndef VMS
#ifdef SWITCHID
#ifdef SYSV4
  uid_t uid, euid;
#else
  int uid, euid;
#endif /* SYSV4 */

  uid = getuid();
  euid = geteuid();
#endif /* SWITCHID */
#endif /* VMS */

  /* now check the initial value */
  strcpy(fname, f_str);
  if ((strncmp(fname, "~/", 2) == 0) &&
      (getenv("HOME") != NULL)) {
#ifdef VMS
    sprintf(line, "%s%s", getenv("HOME"), fname + 2);
#else
    sprintf(line, "%s/%s", getenv("HOME"), fname + 2);
#endif /* VMS */
  } else {
    strcpy(line, fname);
  }

  /* open the file */
#ifndef VMS
#ifdef SWITCHID
  if (euid != uid) {
#ifdef SYSV4
    setuid(uid);
#else
    setreuid(euid, uid);
#endif /* SYSV4 */
  }
#endif /* SWITCHID */
#endif /* VMS */
  cust_fp = fopen(line, "r");
#ifndef VMS
#ifdef SWITCHID
  if (euid != uid) {
#ifdef SYSV4
    setuid(euid);
#else
    setreuid(uid, euid);
#endif /* SYSV4 */
  }
#endif /* SWITCHID */
#endif /* VMS */
  if (cust_fp == NULL) {
    return(-1);
  }

  /* keep track of depth */
  cust_depth++;
  if (cust_depth >= 10) {
    sprintf(string, "Too many include files; aborting %s read", fname);
    errormsg(string);
    return(-1);
  } else if (cust_depth > 5) {
    sprintf(string, "Warning: %d deep in include files", cust_depth);
    errormsg(string);
  }

  /* read until complete */
  line[LINELTH] = '\0';
  while (!feof(cust_fp)) {

    /* get it line by line */
    line[0] = '\0';
    if (fgets(line, LINELTH, cust_fp) == NULL) {
      break;
    }
    linenum++;

    /* clean out white space and comments */
    len = trim_str(line, TRUE);

    /* now check for empty lines */
    if (len == 0) continue;

    /* determine truth or falsehood off setting */
    if (line[0] == '!') {
      value_num = FALSE;
      line[0] = ' ';
      len = trim_str(line, TRUE);
    } else {
      value_num = TRUE;
    }

    /* parse the line */
    if (strncmp(line, "include:", 8) == 0) {
      /* recurse */
      trim_str(line + 8, FALSE);
      if (read_custom(line + 8) == -1) {
	sprintf(string, "Could not read in the include file <%s>",
		line + 8);
	errormsg(string);
      }
    } else if (strncmp(line, "contour ", 8) == 0) {

      /* get the contour type */
      for (count = 0; count < ELE_NUMBER; count++) {
	len = strlen(ele_info[count].name);
	if (strncmp(line + 8, ele_info[count].name, len) == 0) break;
      }

      /* test validity */
      if (count == ELE_NUMBER) {
	sprintf(string, "%s [%d]: unknown contour type",
		fname, linenum);
	errormsg(string);
      } else if (line[len + 8] != '=') {
	sprintf(string, "%s [%d]: format \"conquer type=c\" not found",
		fname, linenum);
	errormsg(string);
      } else {
	value_num = line[len + 9];
	if (isdigit(value_num) ||
	    !isprint(value_num) ||
	    (isspace(value_num) &&
	     ((value_num != ' ') ||
	      (count != ELE_WATER)))) {
	  sprintf(string, "%s [%d]: invalid symbol value for contour",
		  fname, linenum);
	  errormsg(string);
	} else {
	  /* store it */
	  ele_info[count].symbol = value_num;
	}
      }

    } else if (strncmp(line, "vegetation ", 11) == 0) {

      /* get the contour type */
      for (count = 0; count < VEG_NUMBER; count++) {
	len = strlen(veg_info[count].name);
	if (strncmp(line + 11, veg_info[count].name, len) == 0) break;
      }

      /* test validity */
      if (count == VEG_NUMBER) {

	sprintf(string, "%s [%d]: unknown vegetation type",
		fname, linenum);
	errormsg(string);

      } else if (line[len + 11] != '=') {

	sprintf(string, "%s [%d]: format \"vegetation type=c\" not found",
		fname, linenum);
	errormsg(string);
	  
      } else {
	value_num = line[len + 12];
	if (isdigit(value_num) ||
	    !isprint(value_num) ||
	    (isspace(value_num) &&
	     ((value_num != ' ') ||
	      (count != VEG_NONE)))) {
	  sprintf(string, "%s [%d]: invalid symbol value for vegetation",
		  fname, linenum);
	  errormsg(string);
	} else {
	  /* store it */
	  veg_info[count].symbol = value_num;
	}
      }

    } else if (strncmp(line, "designation ", 12) == 0) {

      /* get the designation type */
      for (count = 0; count < MAJ_NUMBER; count++) {
	len = strlen(maj_dinfo[count].name);
	if (strncmp(line + 12, maj_dinfo[count].name, len) == 0) break;
      }

      /* test validity */
      if (count == MAJ_NUMBER) {
	sprintf(string, "%s [%d]: unknown designation type",
		fname, linenum);
	errormsg(string);
      } else if (line[len + 12] != '=') {
	fprintf(fupdate, "%s [%d]: format \"designation type=c\" not found",
		fname, linenum);
	errormsg(string);
      } else {
	value_num = line[len + 13];
	if (isdigit(value_num) || !isprint(value_num) ||
	    isspace(value_num)) {
	  sprintf(string, "%s [%d]: invalid symbol value for designation",
		  fname, linenum);
	  errormsg(string);
	} else {
	  /* store it */
	  maj_dinfo[count].symbol = value_num;
	}
      }

    } else if (strncmp(line, "zoom-level:", 11) == 0) {

      /* set the zoom level if found */
      (void) trim_str(line + 11, FALSE);
      for (count = 0; count < ZOOM_NUMBER; count++) {
	if (strcmp(line + 11, zooms[count]) == 0) break;
      }
      if (count == ZOOM_NUMBER) {
	sprintf(string, "%s [%d]: unknown zoom specification",
		fname, linenum);
	errormsg(string);
      } else {
	zoom_level = count;
      }

    } else if ((strncmp(line, "nation", 6) == 0) ||
	       (strncmp(line, "nationname", 10) == 0) ||
	       (strncmp(line, "name", 4) == 0)) {

      /* provide the nation settings */
      for (count = 0; count < len; count++) {
	if (line[count] == ':') {
	  count++;
	  break;
	}
      }
      if (count == len) {
	sprintf(string, "%s [%d]: could not find ':' delimiter",
		  fname, linenum);
	errormsg(string);
      } else {
	/* now clean up the nation name */
	(void) trim_str(line + count, FALSE);
	strcpy(nationname, line + count);
      }

    } else if ((strncmp(line, "campaign", 7) == 0) ||
	       (strncmp(line, "data", 4) == 0) ||
	       (strncmp(line, "directory", 8) == 0)) {

      /* now determine the campaign */
      for (count = 0; count < len; count++) {
	if (line[count] == ':') {
	  count++;
	  break;
	}
      }
      if (count == len) {
	sprintf(string, "%s [%d]: could not find ':' delimiter",
		fname, linenum);
	errormsg(string);
      } else {
	/* now clean up the directory name */
	(void) trim_str(line + count, FALSE);
	strcpy(datadirname, line + count);
      }

    } else if (strncmp("display-mode", line, 12) == 0) {
      /* store the display settings */
      (void) trim_str(line + 12, FALSE);
      display_setup(line + 12, fname, linenum);
    } else if (strncmp("default-display", line, 15) == 0) {
      /* store the display settings */
      (void) trim_str(line + 15, FALSE);
      dflt_disp_setup(line + 15, fname, linenum);
    } else {

      /* must be one of the option flags */
      for (count = 0; count < options_number; count++) {
	len = strlen(opt_list[count]);
	if (strncmp(line, opt_list[count], len) == 0) break;
      }
      if (count == options_number) {
	sprintf(string, "%s [%d]: unknown configuration command",
		fname, linenum);
	errormsg(string);
      } else {

	/* switch based on the value */
	switch (count) {
	case OPT_BINDKEY:
	case OPT_REBIND:
	case OPT_UNBIND:
	  /* the key binding commands */
	  trim_str(line + len, FALSE);
	  keysys_setup(count, line + len, fname, linenum);
	  break;
	case OPT_INFOMODE:
	  /* information mode active */
	  conq_infomode = value_num;
	  break;
	case OPT_MAILCHECK:
	  /* check the mail spooler */
	  dosysm_check = value_num;
	  break;
	case OPT_ALLBLANKS:
	  /* set the all blank option */
	  conq_allblanks = value_num;
	  break;
	case OPT_BOTTOMLINES:
	  /* set the bottom lines option */
	  conq_bottomlines = value_num;
	  break;
	case OPT_WATER_BTM:
	  /* set the water bottom option */
	  conq_waterbottoms = value_num;
	  break;
	case OPT_TBELL:
	  /* to beep or not to beep! */
	  conq_beeper = value_num;
	  break;
	case OPT_EXPERT:
	  /* good or not good? */
	  conq_expert = value_num;
	  break;
	case OPT_GAUDY:
	  /* highlighting? */
	  conq_gaudy = value_num;
	  break;
	case OPT_SUPPLY:
	  /* supply level */
	  if (sscanf(line + len + 1, "%d", &value_num) == 1) {
	    conq_supply_level = value_num;
	  }
	  break;
	case OPT_PAGETAB:
	  /* check the pager tab setting */
	  if (sscanf(line + len + 1, "%d", &value_num) == 1) {
	    pager_tab = value_num;
	  }
	  break;
	case OPT_PAGESCROLL:
	  /* check the pager scroll */
	  if (sscanf(line + len + 1, "%d", &value_num) == 1) {
	    pager_scroll = value_num;
	  }
	  break;
	case OPT_PAGEOFFSET:
	  /* check the pager offset */
	  if (sscanf(line + len + 1, "%d", &value_num) == 1) {
	    pager_offset = value_num;
	  }
	  break;
	default:
	  /* not a valid option for the configuration */
	  sprintf(string, "%s [%d]: unknown or outdated configuration command",
		  fname, linenum);
	  errormsg(string);
	  break;
	}

      }

    }
  }

  /* end of file */
  fclose(cust_fp);
  return(0);
}
