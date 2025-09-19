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
#ifdef SYS_SIGNAL
#include <sys/signal.h>
#else
#include <signal.h>
#endif /* SYS_SIGNAL */
#include "moveX.h"
#include "worldX.h"
#include "activeX.h"
#include "hlightX.h"
#include "displayG.h"
#include "patchlevel.h"

/* indicator that player is not done yet */
int conquer_done = FALSE;
int nologouts = FALSE;

/* FNAME_CHAR -- FALSE if character is not a normal part of a filename */
static int
fname_char PARM_1(int, ch)
{
  switch (ch) {
#ifdef MS_DOS
  case '\\':
#endif /* MS_DOS */
#ifdef VMS
  case ']':
  case ':':
#endif /* VMS */
  case '/':
    /* nope */
    return(FALSE);
  }
  return(TRUE);
}

/* TOGGLE_OUT -- Quickly output the status of a switchable option */
static void
toggle_out PARM_3(char *, outstr, int, outch, int, val)
{
  fprintf(stderr, "\t-%c       %s%s [toggle]\n",
	  (char)outch, val ? "don't ":"", outstr);
}

/* MAIN -- Interpret command line arguments and parse user input */
int
main PARM_2 (int, argc, char **, argv)
{
  register int i;
  char passwd[PASSLTH+1];
  extern char *optarg;
  extern int time_check;
  int whoflag = FALSE, sflag = FALSE, pflag = FALSE, Pflag = FALSE;
  int Dflag = FALSE, motdflag = FALSE;
#ifndef FILELOCK
  int cflag = FALSE;
#endif /* FILELOCK */
  int time_check_temp;
  char tmppass[PASSLTH + 1];

  /* align the "output" file */
  fupdate = stderr;

  /* seed the random number generators */
  rand_seed();
  nrand_seed();

  /* get the program name */
  sflag = strlen(argv[0]);
  for (i = sflag - 1; i > 0; i--) {
    if (fname_char(argv[0][i]) == FALSE) {
      i++;
      break;
    }
  }
  strcpy(prog_name, &(argv[0][i]));
  sflag = FALSE;

  /* assign givens */
  strcpy(nationname, "");
  allkeys_init();
  is_update = FALSE;
  redraw = DRAW_FULL;

  /* check if the configuration is to be read */
  if ((argc < 2) ||
      (strcmp(argv[1], "-nc") != 0)) {
    sprintf(string, "~/%s", CONQRC_FILE);
    read_custom(string);
  }

  /* check conquer options */
  read_environ();
  movemode = MOVE_NOMOVE;

  /* now verify key bindings */
  if (allkeys_check() != 0) {
    exit(FAIL);
  }

#ifndef VAXC
  /*
   *  Set the real uid to the effective.  This will avoid a
   *  number of problems involving file protection if the
   *  executable is setuid.
   */
  if (getuid() != geteuid()) {		/* we are running suid */
    (void) umask(DEFAULT_UMASK);	/* nobody else can read files */
#ifndef SETREUID
    (void) setuid (geteuid ()) ;
#endif
  }
#endif /* VAXC */

  /* process the command line arguments */
#ifndef FILELOCK
  while ((i = getopt(argc, argv, "?BbcDMwGPhlpcn:d:s")) != EOF)
#else
  while ((i = getopt(argc, argv, "?BbDMwGPhlpcn:d:s")) != EOF)
#endif /* FILELOCK */
    switch (i) {
  case 'h':
    /* first go to the help directory */
    if (chdir(helpdir) != 0) {
      perror("conquer");
      exit(1);
    }

    /* execute help program*/
    cq_init(argv[0]);		        /* setup curses display */
    sprintf(nationname, "hlp%04ld", rand_val(10000));
    signal(SIGINT, SIG_IGN);		/* disable keyboard signals */
    signal(SIGQUIT, SIG_IGN);

    /* get the help information */
    do {
      if ((i = do_help()) == FALSE) {
	clear();
      }
    } while (i == FALSE);

    /* now take off */
    cq_reset();
    exit(SUCCESS);
    break;
  case 'l':
    /* display a list of players currently logged in */
    whoflag = TRUE;
    break;
#ifndef FILELOCK
  case 'c':
    /* clear out locks, if given permission */
    cflag++;
    break;
#endif /* FILELOCK */
  case 'M':
    /* edit the motd */
    motdflag++;
    break;
  case 'P':
    /* print out a highlighted map */
    Pflag++;
    break;
  case 'p':
    /* print out a map */
    pflag++;
    break;
  case 'D':
    /* dump users data out */
    Dflag++;
    break;
  case 'B':
    /* switch on/off the all blanks option */
    conq_allblanks ^= TRUE;
    break;
  case 'b':
    /* switch on/off the bottom lines option */
    conq_bottomlines ^= TRUE;
    break;
  case 'w':
    /* implement water with no underlines display */
    conq_waterbottoms ^= TRUE;
    break;
  case 'e':
    /* implement the email check */
    dosysm_check ^= TRUE;
    break;
  case 'G':
    /* implement highlighted display */
    conq_gaudy ^= TRUE;
    break;
  case 'H':
    /* set the header mode */
    conq_mheaders ^= TRUE;
    break;
  case 'd':
    /* assign the data directory */
    strcpy(datadirname, optarg);
    break;
  case 'n':
    /* assign the nation name */
    if (strcmp(optarg, "c") != 0) {
      (void) strcpy(nationname, optarg);
    }
    break;
  case 'i':
    /* info-mode */
    conq_infomode ^= TRUE;
    break;
  case 't':
    /* terminal bell */
    conq_beeper ^= TRUE;
    break;
  case 'X':
    /* expert mode */
    conq_expert ^= TRUE;
    break;
  case 's':
    /* display the score */
    sflag++;
    break;
  case '?':
    /* display correct command line arguments */
    fprintf(stderr,
#ifndef FILELOCK
	    "Command line format: %s [-nc] [-BbceGHhilMpsw -d DIR -nNAT]\n",
#else
	    "Command line format: %s [-nc] [-BbeGHhilMpsw -d DIR -nNAT]\n",
#endif /* FILELOCK */
	    argv[0]);
    fprintf(stderr, "\t-nc      do not read in %s file [first option only]\n",
	    CONQRC_FILE);
    fprintf(stderr, "\t-n NAT   play as nation NAT\n");
    fprintf(stderr, "\t-d DIR   use to access different campaign\n");
#ifndef FILELOCK
    fprintf(stderr, "\t-c       clear lock for given nation, if allowed\n");
#endif /* FILELOCK */
    toggle_out("use blanks, not underlines", 'B', conq_allblanks);
    toggle_out("always underline water", 'b', conq_bottomlines);
    toggle_out("check for system email", 'e', dosysm_check);
    toggle_out("highlight name in news", 'G', conq_gaudy);
    toggle_out("use header-mode option", 'H', conq_mheaders);
    toggle_out("set info-mode option", 'i', conq_infomode);
    toggle_out("enable warning bells", 't', conq_beeper);
    toggle_out("draw water with no underlines", 'w', conq_waterbottoms);
    toggle_out("enable expert status", 'X', conq_expert);
    fprintf(stderr, "\t-l       dislay list of user logged in\n");
    fprintf(stderr, "\t-h       print help text\n");
    fprintf(stderr, "\t-P       print a map with highlighting\n");
    fprintf(stderr, "\t-p       print a map\n");
    fprintf(stderr, "\t-s       print scores\n");
    fprintf(stderr, "\t-M       edit the MOTD, if allowed\n");
    fprintf(stderr, "\t-D       dump nation information\n");
    exit(SUCCESS);
  };

  /* set the default data directory */
  init_datadir();

  /* now that we have parsed the args, we can go to the
   * dir where the files are kept and do some work. */
  if (chdir(datadir)) {
    fprintf(stderr, "unable to change dir to %s\n", datadir);
    exit(FAIL);
  }

#ifndef FILELOCK
  /* perform the removal of the given nation's lock file */
  if (cflag) {
    char lfilestr[FILELTH];

    /* check if the lockfile exits */
    if (nationname[0] == '\0') {
      fprintf(stderr, "Lockfile removal routines.  Use proper case for all");
      fprintf(stderr, "characters.\nFor special lock file removal, use one");
      fprintf(stderr, "the following:\n\n");
      fprintf(stderr, "\t\tgodup    - to remove update lock\n");
      fprintf(stderr, "\t\tgodadd   - to remove player addition lock\n");
      fprintf(stderr, "\t\t*        - to remove all lock files\n\n");
      fprintf(stderr, "Remove which lock file(s)? ");
      gets(nationname);
    }
    if (strcmp(nationname, "*") != 0) {
      sprintf(lfilestr, "%s.%s", nationname, isontag);
      if (exists(lfilestr) != 0) {
	fprintf(stderr, "There is no lockfile for %s.\n",
		nationname);
	exit(SUCCESS);
      }
    }

    /* read the data file to check the demigod login */
    if (strcmp(LOGIN, loginname) != 0) {
      if ((read_data() == FALSE) ||
	  (world.demigod[0] == '\0')) {
	fprintf(stderr, "You do not have permission to remove lock files.\n");
	fprintf(stderr, "Contact %s to have any lock file removed.\n", LOGIN);
	exit(FAIL);
      }
      if (strcmp(world.demigod, loginname) != 0) {
	fprintf(stderr, "You do not have permission to remove lock files.\n");
	fprintf(stderr, "Contact %s or %s to have any lock file removed.\n",
		LOGIN, world.demigod);
	exit(FAIL);
      }
    }

    /* now do it */
    if (strcmp(nationname, "*") == 0) {
#ifndef VMS
      sprintf(string, "%s *%s 2> /dev/null", REMOVE_NAME, isontag);
#else
      sprintf(string, "%s *.%s;*/nolog", REMOVE_NAME, isontag);
#endif /* VMS */
      system(string);
      fprintf(stderr, "Lock file removal command executed\n");
    } else {
      if (unlink(lfilestr) != 0) {
	fprintf(stderr, "Warning!: Unable to remove lockfile\n");
	exit(FAIL);
      }
      fprintf(stderr, "Lock file for nation %s removed successfully.\n",
	      nationname);
    }
    exit(SUCCESS);
  }
#endif /* FILELOCK */

  /* Try to edit the message of the day */
  if (motdflag) {
    if (strcmp(LOGIN, loginname) != 0) {
      if ((read_data() == FALSE) ||
	  (world.demigod[0] == '\0')) {
	fprintf(stderr, "You do not have permission to edit the MOTD.\n");
	fprintf(stderr, "Contact %s to have the MOTD changed.\n", LOGIN);
	exit(FAIL);
      }
      if (strcmp(world.demigod, loginname) != 0) {
	fprintf(stderr, "You do not have permission to edit the MOTD.\n");
	fprintf(stderr, "Contact %s or %s to have the MOTD changed.\n",
		LOGIN, world.demigod);
	exit(FAIL);
      }
    }

    /* now do it */
    fork_edit_on_file(motdfile, (char *)NULL);
    exit(SUCCESS);
  }

  /* perform time checking */
  if (strcmp(loginname, LOGIN) == 0) {
    time_check = FALSE;
  } else {
    time_check = TRUE;
  }
  time_check_temp = initial_check();
  if ((time_check_temp == -1) ||
      ((time_check_temp != 0) &&
       !(sflag || whoflag || Dflag))) {
    exit(FAIL);
  }

  /* check for update in progress */
  if (!(sflag || whoflag)) {
    sprintf(string, "godup.%s", isontag);
    if (check_lock(string, FALSE) == -1) {
      fprintf(stderr, "Conquer is updating\n");
      fprintf(stderr, "Please try again later.\n");
      exit(FAIL);
    }
  }

  /* read and verify the data */
  if (read_data() == FALSE) {
    fprintf(stderr, "The data file for the campaign was unreadable\n");
    exit(FAIL);
  }
  verify_data( __FILE__, __LINE__ );
  execute(UNOWNED);
  
  /* now start with the title */
  if (!sflag && !whoflag) {
    fprintf(stderr,
      "Conquer %s.%d: Copyright (c) 1992 by Edward M Barlow and Adam Bryant\n",
      VERSION, PATCHLEVEL);
  }

  /* now check for the nologin indicator */
  if (exists(blockfile) == 0) {
    /* prevent any logins at this time */
    fprintf(stderr, "\nNOTE, There are no logins allowed at this time");

    /* read and display the message behind it */
    if ((fexe = fopen(blockfile, "r")) != NULL) {

      /* display the contents of the file */
      fprintf(stderr, ":\n\n");
      while (!feof(fexe)) {
	/* get the input */
	if (fgets(string, BIGLTH, fexe) == NULL) break;
	fprintf(stderr, "%s", string);
      }
      fprintf(stderr, "\n");
      fclose(fexe);

    } else {
      fprintf(stderr, ".\n\n");
    }

    /* just can't get in */
    if (!sflag && !whoflag) {
      if ((strcmp(LOGIN, loginname) == 0) ||
	  (strcmp(world.demigod, loginname) == 0)) {
	fprintf(stderr, "Do you wish to continue to login? ");
	scanf ("%s", string);
	if ((strcmp (string, "yes") != 0) &&
	    (strcmp (string, "y") != 0)) {
	  fprintf(stderr, "\n Okay, you may use the \"conqrun -T\" option to\n");
	  fprintf(stderr, " enable logins again.\n");
	  exit (SUCCESS);
	}

	/* get rid of extra characters */
	do {
	  sflag = getchar();
	} while (sflag != '\n');
	nologouts = TRUE;

      } else {
	exit(SUCCESS);
      }
    }
  }

  /* now print the scores */
  if (sflag) {
    show_scores();
    exit(SUCCESS);
  }

  /* let people know who is logged in */
  if (whoflag) {
    who_is_on();
    exit(SUCCESS);
  }

  /* finish the time check */
  if ((time_check_temp != 0) &&
      (Dflag == 0)) {
    exit(FAIL);
  }

  /* align supply level */
  if ((conq_supply_level <= 0) ||
      (conq_supply_level > MAXSUPPLIES)) {
    conq_supply_level = MAXSUPPLIES;
  }

  /* get nation name from command line or by asking user.
   *     if you fail give name of administrator of game
   */
  if (nationname[0] == '\0') {
    if (pflag || Pflag)
      fprintf(stderr, "Display map for what nation: ");
    else fprintf(stderr, "What nation would you like to be: ");
    gets(nationname);
  }

  /* validate god login */
  if ((str_test(nationname, "god") == 0) ||
      (str_test(nationname, "unowned") == 0)) {
#ifdef SECURITY
    if ((strcmp(loginname, LOGIN) != 0) &&
	((world.demilimit) ||
	 (strcmp(loginname, world.demigod) != 0))) {
      fprintf(stderr, "Sorry -- you can not login as god\n");
      fprintf(stderr, "you need to be logged in as %s", LOGIN);
      if (!world.demilimit &&
	  (strcmp(LOGIN, world.demigod) != 0)) {
	fprintf(stderr, " or %s", world.demigod);
      }
      fprintf(stderr, "\n");
      exit(FAIL);
    }
#endif /*SECURITY*/
    (void) strcpy(nationname, "god");
    country = UNOWNED;
    ntn_ptr = NULL;

  } else {
    country = (-1);
    for(i = 1; i < MAXNTN; i++) if ((ntn_ptr = world.np[i]) != NULL) {
      if (str_test(nationname, ntn_ptr->name) == 0) {
	country = i;
	strcpy(nationname, ntn_ptr->name);
	break;
      }
    }
  }

  /* invalid country name */
  if (country == (-1)) {

    fprintf(stderr, "\nSorry, name <%s> not found", nationname);
    fprintf(stderr, "\nFor rules type <%s -h>", argv[0]);
    fprintf(stderr, "\nFor information on conquer please contact %s.", OWNER);
    fprintf(stderr, "\nTo enter this campaign please send mail to %s", LOGIN);
    if (strcmp(LOGIN, world.demigod) != 0) {
      fprintf(stderr, " or %s", world.demigod);
    }
    fprintf(stderr, ".\n");
    exit(FAIL);

  }

  /* get password and perform any encryption */
  fprintf(stderr, "\nWhat is your Nation's Password: ");
  strncpy(tmppass, getpass(""), PASSLTH + 1);
#ifdef CRYPT
  strncpy(passwd, crypt(tmppass, SALT), PASSLTH);
#else
  strncpy(passwd, tmppass, PASSLTH);
#endif
  passwd[PASSLTH] = '\0';

  if (((ntn_ptr == NULL) ||
       (strncmp(passwd, ntn_ptr->passwd, PASSLTH) != 0)) &&
      (strncmp(passwd, world.passwd, PASSLTH) != 0)) {

    /* check again */
    fprintf(stderr, "\nError: Reenter your Nation's Password: ");
    strncpy(tmppass, getpass(""), PASSLTH + 1);
#ifdef CRYPT
    strncpy(passwd, crypt(tmppass, SALT), PASSLTH);
#else
    strncpy(passwd, tmppass, PASSLTH);
#endif
    passwd[PASSLTH] = '\0';

    if (((ntn_ptr == NULL) ||
	 (strncmp(passwd, ntn_ptr->passwd, PASSLTH) != 0)) &&
	(strncmp(passwd, world.passwd, PASSLTH) != 0)) {
      fprintf(stderr, "\nSorry:");
      fprintf(stderr, "\nFor rules type <%s -h>", argv[0]);
      fprintf(stderr, "\nFor information on conquer please contact %s.",
	      OWNER);
      fprintf(stderr, "\nTo enter this campaign please send mail to %s",
	      LOGIN);
      if (strcmp(LOGIN, world.demigod) != 0) {
	fprintf(stderr, " or %s", world.demigod);
      }
      fprintf(stderr, ".\n");
      exit(FAIL);
    }
  }

  /* if appropriate; verify login identity */
  if ((is_god == FALSE) && (ntn_ptr != NULL) && world.verify_login) {
    if ((strcmp(LOGIN, loginname) != 0) &&
	(strcmp(world.demigod, loginname) != 0) &&
	(strcmp(ntn_ptr->login, loginname) != 0)) {
      /* UNIMPLEMENTED: SEND MAIL TO GOD */
      fprintf(stderr, "Your login name of %s is not authorized to enter\n",
	      loginname);
      fprintf(stderr, "nation %s.  If there is a problem, please send\n",
	      ntn_ptr->name);
      fprintf(stderr, "mail to %s", LOGIN);
      if (strcmp(LOGIN, world.demigod) != 0) {
	fprintf(stderr, " or %s", world.demigod);
      }
      fprintf(stderr, ".\n");
      exit(FAIL);
    }
  }

  /* now print the maps */
  if (pflag || Pflag) {

    /* get the current world state for that nation */
    execute(UNOWNED);
    if (country != UNOWNED) execute(country);
    set_weights(FALSE);

    /* set the view for the nations */
    visibility_data = new_maplong(visibility_data);
    hex_init();

    /* now query what map to display */
    print_map(Pflag);
    exit(SUCCESS);
  }

  if (Dflag) {
     /* Dump nation information */
    /* get the current world state for that nation */
    execute(UNOWNED);
    
    if (country == UNOWNED) {
      for (i = 0; i < MAXNTN; i++) {
	if ((ntn_ptr = world.np[i]) != NULL) {
	  country = i;
	  execute(country);
	  dump_ntn_info();
	}
      }
    } else {
      execute(country);
      dump_ntn_info();
    }
    exit(SUCCESS);
  }

  /* initialize curses display */
  cq_init(argv[0]);

  /* display initial title screen -- end of setup done later */
  motd_display();
  copyscreen();

  /* initialize screen memory */
  visibility_data = new_maplong(visibility_data);

  /* initialize the distribution weights */
  set_weights(TRUE);

  /* just in case of early abort */
  strcpy(fison, "START");

  /* check for nations without capitals */
  if (country == UNOWNED || ntn_ptr == NULL) {
    xcurs = (MAPX + 1) / 2;
    ycurs = (MAPY + 1) / 2;
    country = UNOWNED;

    /* create gods lock file but do not limit access */
    if (open_ntn(country) == TRUE) {

      mvaddstr(LINES - 1, 0, "God is already logged in;  override? [ny]");
      clrtoeol();
      mvaddstr(LINES - 1, COLS - 21, "Hit 'y' to Continue");
      if (y_or_n() == FALSE) {
	bye();
      }

      /* must now initialize for god */
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

      /* read everything in */
      execute(country);
    }

  } else {

    /* check if it is still valid */
    if (world.np[country]->active == INACTIVE) {
      standout();
      mvaddstr(LINES - 2, 0,
	       "Sorry, for some reason, your country no longer exists.");
      sprintf(string, "If there is a problem, please send mail to %s", LOGIN);
      if (strcmp(LOGIN, world.demigod) != 0) {
	strcat(string, " or ");
	strcat(string, world.demigod);
      }
      strcat(string, ".");
      errormsg(string);
      standend();
      bye();
    }

    /* check if they are logged in */
    if (open_ntn(country) == TRUE) {
      mvaddstr(LINES - 2, 0, "Sorry, your country is already logged in.");
      errormsg("Please try again later.");
      bye();
    }

    /* now initialize everything */
    xcurs = ntn_ptr->capx;
    ycurs = ntn_ptr->capy;
  }

  /* set the screen location */
  xoffset = 0;
  yoffset = 0;

  /* now stop interrupts and catch hangups */
  signal(SIGINT, SIG_IGN);
  signal(SIGQUIT, SIG_IGN);
  signal(SIGHUP, hangup);
  signal(SIGTERM, hangup);
  signal(SIGFPE, hangup);
  signal(SIGSEGV, hangup);
  need_hangup = TRUE;

  /* initialize some stuff */
  hex_init();
  stmail_init();

  /* get response from copyscreen */
  presskey();
  init_time_check();

  /* main routine */
  while (conquer_done == FALSE) {

    /* check for mail and other files */
    files_check();

    /* check for out of bounds movement */
    coffmap();

    /* now get the input */
    parse();

  }

  /* close up shop */
  close_ntn(country, nationname);

  /* finish up */
  bye();
  exit(0);
  return(0);
}
