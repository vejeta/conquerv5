/* interaction routines between conquer and the file system */
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
#include "mtrlsX.h"
#include "activeX.h"
#define USE_CODES
#define PRINT_CODES
#include "cityX.h"
#include "worldX.h"

/* CLOSE_NTN -- unset and close up nation information */
void
close_ntn PARM_2( int, ntnnum, char *, realname )
{
  /* set nation pointer properly */
  ntn_tptr = ntn_ptr;
  ntn_ptr = world.np[ntnnum];

  /* get rid of the lock file */
  sprintf(fison, "%s.%s", realname, isontag);
  kill_lock(lockfilenum, fison);

  /* now save nation information */
  if (ntnnum == UNOWNED) {

    /* kill the lock file and fix everything */
    save_god();

    /* store the news/mail information */
    global_long = (long) world.news_size;
    if (conq_news_status != STMAIL_NONE) {
      global_long--;
    }
    XADJNEWS;
    global_long = (long) world.mbox_size;
    if (conq_mail_status != STMAIL_NONE) {
      global_long--;
    }
    XADJMAIL;

  } else if ((ntn_ptr != NULL) &&
	     (is_god == FALSE)) {

    /* finalize nation information */
    if (!n_ismonster(ntn_ptr->active)) {
      if ((city_ptr =
	   citybyloc(ntn_ptr, ntn_ptr->capx, ntn_ptr->capy)) != NULL) {
	CADJMTRLS(MTRLS_TALONS);
	CADJIMTRLS(MTRLS_TALONS);
      }
    }

    /* store the news/mail information */
    global_long = (long) ntn_ptr->news_size;
    if (conq_news_status != STMAIL_NONE) {
      global_long--;
    }
    XADJNEWS;
    global_long = (long) ntn_ptr->mbox_size;
    if (conq_mail_status != STMAIL_NONE) {
      global_long--;
    }
    XADJMAIL;
  }

  /* finish up */
  if (fexe != NULL) fclose(fexe);
  ntn_ptr = ntn_tptr;
}

/* OPEN_NTN -- initialize all nation information */
int
open_ntn PARM_1( int, ntnnum )
{
  /* initialize things */
  if (ntnnum == UNOWNED) {
    strcpy(nationname, "god");
    is_god = TRUE;
  } else {
    ntn_ptr = world.np[ntnnum];
    if (ntn_ptr != NULL) {
      sprintf(nationname, ntn_ptr->name);
    }
    is_god = FALSE;
  }
  sprintf(conqmail, "%s.%s", nationname, msgtag);

  /* now indicate if player is already on */
  if ((lockfilenum = aretheyon()) == -1) {
    return(TRUE);
  } else {
    /* reset the saved sector */
    xloc_mark = (-1);
    yloc_mark = (-1);

    /* now read in everything */
    if (ntnnum != UNOWNED) execute(ntnnum);

    /* now open the recording file */
    sprintf(string, "%s.%s", nationname, exetag);
    if ((fexe = fopen(string, "a")) == NULL) {
      errormsg("MAJOR ERROR: could not open execution file for appending");
      sprintf(string, "Please report this error to %s", LOGIN);
      if (strcmp(LOGIN, world.demigod) == 0) {
	strcat(string, " or ");
	strcat(string, world.demigod);
      }
      strcat(string, ".");
      errormsg(string);
      cq_bye(FAIL);
    }
  }

  /* leave routine */
  return(FALSE);
}

/* FILES_CHECK -- Periodically check if various files need accessing */
void
files_check PARM_0(void)
{
  static long last_time = 0, warn_time = 0;
  long curtime = time((long *)0);
  extern int conquer_done, nologouts;

  /* check if enough time has passed */
  if (curtime - last_time > MAIL_TIME) {

    /* check for the block out file */
    if (!nologouts) {
      if (exists(blockfile) == 0) {
	if ((warn_time == 0) &&
	    (TIMETOBOOT > 0)) {
	  sprintf(string,
		  "***WARNING: Logins now disabled; you have %d.%d minutes***",
		  TIMETOBOOT / 60, (TIMETOBOOT % 60) * 10 / 60);
	  errormsg(string);
	  warn_time = curtime;
	} else if (curtime - warn_time >= TIMETOBOOT) {
	  errormsg("**Forcing player logout .... goodbye***");
	  conquer_done = TRUE;
	  push_char(' ');
	}
      } else if (warn_time != 0) {
	errormsg("User Logins have been re-enabled... time limit removed.");
	warn_time = 0;
      }
    }

    /* now check the various files */
    if (dosysm_check) sysm_check();
    news_check();
    mbox_check();

    /* record final time */
    last_time = curtime;
  }
}

/* NEWS_CHECK -- Check if the news needs reading */
void
news_check PARM_0(void)
{
  struct stat fst;
  off_t holder;

  /* build the mail file name */
  sprintf(string, "%s.%03d", newsfile, TURN - START_TURN);

  /* check the size of the file with previous value */
  if (stat( string, &fst ) == 0 ) {

    /* find what needs checking */
    if (ntn_ptr == NULL) {
      holder = world.news_size;
    } else {
      holder = ntn_ptr->news_size;
    }

    /* now check it */
    if (fst.st_size > holder) {
      if (conq_news_status != STMAIL_NEW) {
	conq_news_status = STMAIL_NEW;
      }
    } else if (fst.st_size < holder) {
      if (conq_news_status != STMAIL_NONE) {
	conq_news_status = STMAIL_NONE;
      }
    }

    /* now reassign the value */
    if (ntn_ptr == NULL) {
      world.news_size = fst.st_size;
    } else {
      ntn_ptr->news_size = fst.st_size;
    }

  } else {

    /* it is empty, therefore read */
    if (conq_news_status != STMAIL_NONE) {
      conq_news_status = STMAIL_NONE;
    }
    if (ntn_ptr == NULL) {
      world.news_size = (off_t) 0;
    } else {
      ntn_ptr->news_size = (off_t) 0;
    }

  }
}

/* MBOX_CHECK -- Check if the mail needs reading */
void
mbox_check PARM_0(void)
{
  struct stat fst;
  off_t holder;

  /* check the size of the file with previous value */
  if (stat( conqmail, &fst ) == 0 ) {

    /* find what needs checking */
    if (ntn_ptr == NULL) {
      holder = world.mbox_size;
    } else {
      holder = ntn_ptr->mbox_size;
    }

    /* now check it */
    if (fst.st_size > holder) {
      if (conq_mail_status != STMAIL_NEW) {
	conq_mail_status = STMAIL_NEW;
      }
    } else if (fst.st_size < holder) {
      if (conq_mail_status != STMAIL_NONE) {
	conq_mail_status = STMAIL_NONE;
      }
    }

    /* now reassign the value */
    if (ntn_ptr == NULL) {
      world.mbox_size = fst.st_size;
    } else {
      ntn_ptr->mbox_size = fst.st_size;
    }

  } else {

    /* not there... so must have been read */
    if (conq_mail_status != STMAIL_NONE) {
      conq_mail_status = STMAIL_NONE;
    }
    if (ntn_ptr == NULL) {
      world.mbox_size = (off_t) 0;
    } else {
      ntn_ptr->mbox_size = (off_t) 0;
    }

  }
}

/* SYSM_CHECK -- Check if the system mail needs reading */
void
sysm_check PARM_0(void)
{
#ifdef SYSMAIL
  static off_t sysm_size = 0;
  static long sysm_atime = 0;
  struct stat fst;

  /* check the size of the file with previous value */
  if (stat( sysmail, &fst ) == 0 ) {
    if (fst.st_size > sysm_size) {
      if (sys_mail_status != STMAIL_NEW) {
	sys_mail_status = STMAIL_NEW;
      }
      sysm_atime = fst.st_atime;
    } else if ((fst.st_size < sysm_size) ||
	       (fst.st_atime > sysm_atime)) {
      if (sys_mail_status != STMAIL_NONE) {
	sys_mail_status = STMAIL_NONE;
      }
      sysm_atime = fst.st_atime;
    }
    sysm_size = fst.st_size;
  } else {
    sysm_size = (off_t) 0;
    sysm_atime = 0L;
    if (sys_mail_status != STMAIL_NONE) {
      sys_mail_status = STMAIL_NONE;
    }
  }
#endif /* SYSMAIL */
}

/* STMAIL_INIT -- Initialize the system mail checking setup */
void
stmail_init PARM_0(void)
{
#ifdef SYSMAIL
  /* build up the mail file path */
  if (getenv("MAIL") == NULL) {
    (void) sprintf(sysmail, "%s%s", SPOOLDIR, getenv("USER"));
  } else {
    (void) strcpy(sysmail, getenv("MAIL"));
  }

  /* now initialize and assume it is already read */
  sysm_check();
  sys_mail_status = STMAIL_NONE;
#endif /* SYSMAIL */
  conq_news_status = STMAIL_NONE;
  conq_mail_status = STMAIL_NONE;
}

/* strings to keep track of god's temporary files */
static char god_tfile[BIGLTH];
static char god_tlock[BIGLTH];
static int god_exe[ABSMAXNTN];
static int init_exe = FALSE;
static int god_enum = 0;

/* GET_GOD -- Routine to query god as to what nation to become */
int
get_god PARM_2(int, natn, int, nobrowse)
{
  static int hld_country = -1;
  int hold = -1;

  /* if this is the first time perform the initialization */
  if (init_exe == FALSE) {
    int i;
    for (i = 0; i < ABSMAXNTN; i++) {
      god_exe[i] = -1;
    }
    god_exe[UNOWNED] = -2;
    init_exe = TRUE;
  }

  /* check settings */
  if (natn == (-1)) {

    /* display query */
    clear_bottom(0);
    mvaddstr(LINES - 3, 0, "Super User:  For what nation? ");
    if (hld_country != -1) {
      printw("[%s] ", (world.np[hld_country])->name);
    }
    refresh();

    /* get a nation name or number and validate it */
    country = get_country(FALSE);
    if (no_input == TRUE) {
      if (hld_country == -1) {
	country = UNOWNED;
	return(TRUE);
      }
      country = hld_country;
      ntn_tptr = world.np[country];
    }

  } else {

    /* check if it should remain as god */
    if (natn == UNOWNED) {
      return(FALSE);
    } else if (natn >= MAXNTN) {
      return(TRUE);
    }

    /* set the country as indicated */
    country = natn;
    ntn_tptr = world.np[country];

  }

  /* check nation validity */
  if ((country <= 0) ||
      (country >= MAXNTN) ||
      (ntn_tptr == NULL)) {
    errormsg("That nation name was not among the ones in this world");
    country = UNOWNED;
    return(TRUE);
  }

  /* now verify that the nation is not currently active */
  if (god_exe[country] == -1) {
    if (nobrowse == FALSE) {
      bottommsg("Do you wish to just view things? ");
    }
    if ((nobrowse == TRUE) ||
	((nobrowse != -1) &&
	 (y_or_n() == FALSE))) {
      god_browsing = FALSE;
      if (max_descript() < god_enum + 4) {
	errormsg("You have accessed too many nations... save and relogin.");
	country = UNOWNED;
	return(TRUE);
      }
      sprintf(god_tlock, "%s.%s", ntn_tptr->name, isontag);
      if ((hold = check_lock(god_tlock, TRUE)) == -1) {
	errormsg("That nation is currently logged in");
	country = UNOWNED;
	return(TRUE);
      }
      god_enum++;
    } else {
      god_browsing = TRUE;
    }
  } else {
    god_browsing = FALSE;
  }

  /* realign pointers */
  ntn_ptr = ntn_tptr;
  hld_country = country;

  /* now read in all of their information and append to it */
  if (god_browsing == FALSE) {

    /* close up on the god file */
    if (fexe != NULL) fclose(fexe);
    if (god_exe[country] == -1) {
      bottommsg("Reading in the user's commands...");
      execute(country);
      god_exe[country] = hold;
      bottommsg("Reading in the user's commands...done");
    }
    sprintf(god_tfile, "%s.%s", ntn_ptr->name, exetag);
    if ((fexe = fopen(god_tfile, "a")) == NULL) {
      errormsg("Serious Error: Could not append to user information file\n");
      cq_reset();
      abrt();
    }
  }

  /* a valid nation has been chosen */
  return(FALSE);
}

/* SAVE_GOD -- Go and get rid of god locks from his tramping around */
void
save_god PARM_0(void)
{
  int i;

  /* check if anything was done */
  if (init_exe == FALSE) return;

  /* go and remove all the locks created */
  for (i = 0; i < ABSMAXNTN; i++) if (god_exe[i] > -1) {
    if ((ntn_tptr = world.np[i]) != NULL) {
      sprintf(god_tlock, "%s.%s", ntn_tptr->name, isontag);
      kill_lock(god_exe[i], god_tlock);
    }
  }
}

/* RESET_GOD -- Restore settings of god */
void
reset_god PARM_0(void)
{
  /* check if god was only peeking */
  if (god_browsing == FALSE) {

    /* close the execution file */
    if (fexe != NULL) fclose(fexe);

    /* now rebuild the link to gods data file */
    sprintf(string, "god.%s", exetag);
    if ((fexe = fopen(string, "a")) == NULL) {
      errormsg("Serious Error: could not open god command file");
      abrt();
    }

  }

  /* now set the proper indicators */
  god_browsing = FALSE;
  country = UNOWNED;
  ntn_ptr = NULL;
}

/* ARETHEYON -- returns TRUE if 'country' is logged on, else FALSE */
int
aretheyon PARM_0(void)
{
  /* return file descriptor for lock file */
  sprintf(fison, "%s.%s", nationname, isontag);
  return( check_lock(fison, TRUE) );
}

/* BYE -- Clean up everything prior to exiting */
void
bye PARM_0(void)
{
  cq_reset();
  fprintf(stderr, "\nquit & save\n");
  exit(SUCCESS);
}

#ifdef SYSV4
#include <sys/resource.h>
extern int getrlimit(int resource, struct rlimit *rlp);
#endif /* SYSV4 */

/* MAX_DESCRIPT -- The maximum number of descriptors available */
int
max_descript PARM_0(void)
{  
#ifdef VAXC
#include <jpidef.h>
  typedef struct item_list {
    unsigned short int item_length;
    unsigned short int item_code;
    char *item_address;
  } list_struct;

  int files, stat;
  list_struct fillm_list[2];

  fillm_list[0].item_length = 4;
  fillm_list[0].item_code = JPI$_FILLM;
  fillm_list[0].item_address = &files;

  fillm_list[1].item_length = 0;
  fillm_list[1].item_code = 0;
  fillm_list[1].item_address = 0;

  stat = sys$getjpiw(0,0,0,&fillm_list,0,0,0);
  if(stat) return(files);
  if(!stat) return(20);
#else /* VAXC */
#ifdef HPUX
  extern long sysconf();
#ifndef _SC_OPEN_MAX
#define _SC_OPEN_MAX    4
#endif /* _SC_OPEN_MAX */
  return(sysconf(_SC_OPEN_MAX));
#undef _SC_OPEN_MAX
#else /* HPUX */
#ifdef GETDTABLESIZE
  return(getdtablesize());
#else /* GETDTABLESIZE */
#ifdef SYSV4
  struct rlimit rl;
  getrlimit(RLIMIT_NOFILE, &rl);
  return rl.rlim_cur;   /* current resource limit */
#else
  /* give back a fair estimate */
  return(24);
#endif /* SYSV4 */
#endif /* GETDTABLESIZE */
#endif /* HPUX */
#endif /* VAXC */
}
