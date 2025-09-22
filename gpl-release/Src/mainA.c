// SPDX-License-Identifier: GPL-3.0-or-later
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
#ifdef SYS_SIGNAL
#include <sys/signal.h>
#else
#include <signal.h>
#endif /* SYS_SIGNAL */
#ifndef VAXC
#include <fcntl.h>
#else
#include <file.h>
#endif /* VAXC */
#include "worldX.h"

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

/* check the locks */
int addlocknum = -1, uplocknum = -1;
char lock_string[FILELTH];

/* MAIN -- parse command line options and control program flow */
int
main PARM_2 (int, argc, char **, argv)
{
  register int i;
  char upd_outfile[FILELTH];
  char tmppass[PASSLTH + 1];
  FILE *timefp;

  /* mflag = make world, aflag = add player, xflag = execute */
  /* rflag = make world from read in files */
  int mflag, aflag, xflag, rflag, Aflag;
  int Zflag, Iflag, Eflag, Tflag;
  int Qflag = 0;
  extern char *optarg;

  /* find the program name */
  fupdate = stderr;
  mflag = strlen(argv[0]);
  for (rflag = mflag - 1; rflag > 0; rflag--) {
    if (fname_char(argv[0][rflag]) == FALSE) {
      rflag++;
      break;
    }
  }
  strcpy(prog_name, &(argv[0][rflag]));

  /* initialization */
  mflag = aflag = xflag = rflag = Aflag = 0;
  Zflag = Iflag = Eflag = Tflag = 0;

  rand_seed();
  nationname[0] = '\0';
  upd_outfile[0] = '\0';
  is_update = FALSE;
  is_god = FALSE;

  /* check conquer options */
  if ((argc < 2) ||
      (strcmp(argv[1], "-nc") != 0)) {
    sprintf(string, "~/%s", CONQRC_FILE);
    read_custom(string);
  }
  read_environ();

  /* process the command line arguments */
  while ((i = getopt (argc, argv, "ACEIQTZncmaxo:r:d:")) != EOF)
    switch (i) {
      /* process the command line arguments */
    case 'm':		/* make a new world */
      mflag++;
      break;
    case 'I':		/* provide statistics */
      Iflag++;
      break;
    case 'Z':		/* zero out the file */
      Zflag++;
      break;
    case 'E':		/* edit the world */
      Eflag++;
      break;
    case 'A':		/* create an npc nation */
      Aflag++;
      break;
    case 'a':		/* anyone with password can add player */
      aflag++;
      break;
    case 'x':		/* execute program */
      xflag++;
      break;
    case 'r':		/* read map file */
      rflag++;
      if (strlen (optarg) > NAMELTH) {
        fprintf (stderr, "ERROR: Mapfile stem longer than %d\n", NAMELTH);
        exit (FAIL);
      }
      strcpy (scenario, optarg);
      break;
    case 'd':
      strcpy (datadirname, optarg);
      break;
    case 'Q':
      /* simple test routine of items */
      Qflag++;
      break;
    case 'T':
      /* toggle the nologin feature */
      Tflag++;
      break;
    case 'n':
    case 'c':
      /* skip 'em */
      break;
    case 'o':
      /* set the output file */
      strcpy (upd_outfile, optarg);
      break;
    case '?':              /*  print out command line arguments */
      goto usage_list;
    };

  /* assign the output file pointer */
  if ((upd_outfile[0] != '\0') &&
      (xflag || Iflag)) {
    if ((fupdate = fopen(upd_outfile, "w")) == NULL) {
      fprintf(stderr, "Error: Unable to open output file <%s>\n",
	      upd_outfile);
      exit(1);
    }

    /* realign the standard input and standard output */
    freopen(NULL_INPUT, "r", stdin);
    freopen(NULL_OUTPUT, "w", stdout);
  }

  /* may now replace user identity */
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

  /* set proper defaultdir */
  init_datadir();

  /*
   * Now that we have parsed the args, we can goto the
   * directory where the files are kept and do some work.
   */
  if (chdir (datadir)) {
#ifdef MKDIR
    if ((mflag || rflag) && (strcmp(loginname, LOGIN) == 0)) {
      fprintf(fupdate, "Create directory %s? (y or n)", datadir);
      while ((global_int = getchar()) != 'y') {
	if (global_int == 'n') break;
      }
      if (global_int == 'y' && mkdir(datadir, 0700)) {
	fprintf(fupdate, "Could not create directory %s\n", datadir);
	perror("conqrun");
	exit(FAIL);
      } else if (global_int == 'n') {
	fprintf(fupdate, "Unable to change directory to %s\n", datadir);
	exit(FAIL);
      }
      if (chdir (datadir)) {
	fprintf(fupdate, "Still could not access the directory %s\n",
		datadir);
	perror("conqrun");
	exit(FAIL);
      }
      /* get rid of extra characters */
      do {
	remake = getchar();
      } while (remake != '\n');
      remake = FALSE;
    } else {
#endif /* MKDIR */
      fprintf (fupdate, "Unable to change directory to %s\n", defaultdir);
      exit (FAIL);
#ifdef MKDIR
    }
#endif /* MKDIR */
  }

  /* check the the toggling of logins */
  if (Tflag) {

    /* check the god / demigod identity */
    if (read_data() != FALSE) {
      verify_data(__FILE__, __LINE__);
    }
    if ((strcmp(loginname, LOGIN) != 0) &&
	((world.demigod[0] == '0') ||
	 (strcmp(loginname, world.demigod) != 0))) {
      fprintf (fupdate, "Sorry -- you can not toggle logins.\n");
      fprintf (fupdate, "You need to be logged in as %s", LOGIN);
      if ((world.demigod[0] != '\0') &&
	  (strcmp (LOGIN, world.demigod) != 0)) {
	fprintf (fupdate, " or %s", world.demigod);
      }
      fprintf (fupdate, ".\n");
      exit (FAIL);
    }

    /* if the file exists, kill it */
    if (exists(blockfile) == 0) {

      /* check for the enabling of logins */
      fprintf(fupdate, "Logins currently disabled.  Enable them? ");
      scanf("%s", string);
      if ((strcmp (string, "yes") != 0) &&
	  (strcmp (string, "y") != 0)) {
	exit (SUCCESS);
      }
      do {
	string[0] = getchar();
      } while (string[0] != '\n');

      /* kill it */
      unlink(blockfile);

    } else {

      /* ask if it needs to be created */
      fprintf(fupdate, "Logins currently enabled.  Disable them? ");
      scanf("%s", string);
      if ((strcmp (string, "yes") != 0) &&
	  (strcmp (string, "y") != 0)) {
	exit (SUCCESS);
      }
      do {
	string[0] = getchar();
      } while (string[0] != '\n');

      /* now open it and get the reasoning */
      if ((fexe = fopen(blockfile, "w")) == NULL) {
	syserr_msg("unable to open the nologin file");
	exit(FAIL);
      }

      /* now get the input */
      fprintf(fupdate, "\nEnter a reason for disabling logins; end by\n");
      fprintf(fupdate, "entering just a single period:\n");
      do {
	fprintf(fupdate, "msg> ");
	if (fgets(string, BIGLTH, stdin) == NULL) break;
  /* Remove trailing newline if present */
  string[strcspn(string, "\n")] = '\0';
	if (strcmp(string, ".") != 0) {
	  fprintf(fexe, "  %s\n", string);
	}
      } while (strcmp(string, ".") != 0);
      fclose(fexe);

      /* let them know the delay */
      fprintf(fupdate, "Logins now disabled; anyone still in conquer will\n");
      fprintf(fupdate, "forced out after %d seconds.\n", TIMETOBOOT);

    }

    /* done */
    exit(SUCCESS);
  }

  /* check for the 'Z' flag */
  if (Zflag) {
    if (strcmp(loginname, LOGIN) != 0) {
      fprintf(fupdate, "Sorry, only %s may remove the npc configuration file\n",
	      LOGIN);
      exit(FAIL);
    }
    unlink(npcfile);
    if (!Aflag) exit(SUCCESS);
  }

  /* check for login blocks and show a warning */
  if (exists(blockfile) == 0) {
    fprintf(fupdate, "\n   **** Warning == Warning ****\n");
    fprintf(fupdate, "User Logins are currently disabled.\n");
    fprintf(fupdate, "   **** Warning == Warning ****\n");
  }

  /* create npc logins */
  if (Aflag) {
    newlogin(TRUE);
    exit(SUCCESS);
  }

  /* check for statistics */
  if (Iflag) {

    /* get the data */
    if (read_data() == FALSE) {
      exit(FAIL);
    }
    verify_data(__FILE__, __LINE__);

    /* check permissions */
    if ((strcmp(loginname, LOGIN) != 0) &&
	(strcmp(loginname, world.demigod) != 0)) {
      fprintf (fupdate, "Sorry -- you can not display world statistics\n");
      fprintf (fupdate, "you need to be logged in as %s", LOGIN);
      if (strcmp (LOGIN, world.demigod) != 0) {
	fprintf (fupdate, " or %s", world.demigod);
      }
      fprintf (fupdate, ".\n");
      exit (FAIL);
    }
    
    /* now get the statistics */
    world_stats();
    exit(SUCCESS);
  }


  /* adjust the nation information */
  if (Eflag) {

    /* check for updates */
    sprintf (string, "godup.%s", isontag);
    if (check_lock (string, FALSE) == -1) {
      fprintf (fupdate, "Conquer is updating\n");
      fprintf (fupdate, "Please try again later.\n");
      exit (FAIL);
    }

    /* lock the additions file */
    sprintf (lock_string, "godadd.%s", isontag);
    if ((addlocknum = check_lock(lock_string, TRUE)) == -1) {
      fprintf (fupdate, "Someone is adding a nation.\n");
      fprintf (fupdate, "Please try again later.\n");
      exit (FAIL);
    }

    /* get the data */
    if (read_data() == FALSE) {
      kill_lock(addlocknum, lock_string);
      exit(FAIL);
    }
    verify_data(__FILE__, __LINE__);

    /* check permissions */
    if ((strcmp(loginname, LOGIN) != 0) &&
	(strcmp(loginname, world.demigod) != 0)) {
      fprintf (fupdate, "Sorry -- you can not adjust world options\n");
      fprintf (fupdate, "you need to be logged in as %s", LOGIN);
      if (strcmp (LOGIN, world.demigod) != 0) {
	fprintf (fupdate, " or %s", world.demigod);
      }
      fprintf (fupdate, ".\n");
      kill_lock(addlocknum, lock_string);
      exit (FAIL);
    }

    /* now adjust things */
    if (Eflag) {
      cq_init("cqadjust");
      if (bld_config(FALSE) == TRUE) {
	cq_reset();
	verify_data(__FILE__, __LINE__);
	write_data();
      } else {
	cq_reset();
      }
    } else {
      write_data();
    }
    kill_lock(addlocknum, lock_string);
    exit(SUCCESS);
  }

  if (Qflag) {
    int old_numdice;

    /* check for player additions */
    sprintf (string, "godadd.%s", isontag);
    if (check_lock (string, FALSE) == -1) {
      fprintf (fupdate, "Someone is adding a new nation\n");
      fprintf (fupdate, "Please try again later.\n");
      exit (FAIL);
    }

    /* check if an update is going on */
    sprintf (lock_string, "godup.%s", isontag);
    if ((uplocknum = check_lock(lock_string, TRUE)) == -1) {
      fprintf (fupdate, "An update is still executing.\n");
      fprintf (fupdate, "Dice test aborted.\n");
      exit (FAIL);
    }

    /* get the data */
    if (read_data() == FALSE) {
      kill_lock(uplocknum, lock_string);
      exit(FAIL);
    }
    verify_data(__FILE__, __LINE__);

    /* check permissions */
    if ((strcmp(loginname, LOGIN) != 0) &&
	(strcmp(loginname, world.demigod) != 0)) {
      fprintf (fupdate, "Sorry -- you can not run combat roll tests\n");
      fprintf (fupdate, "you need to be logged in as %s", LOGIN);
      if (strcmp (LOGIN, world.demigod) != 0) {
	fprintf (fupdate, " or %s", world.demigod);
      }
      fprintf (fupdate, ".\n");
      kill_lock(uplocknum, lock_string);
      exit (FAIL);
    }

    /* initialize display */
    cq_init("cqdice");
    old_numdice = NUMDICE;

    /* call the routine to test the dice, then exit */
    dice_tester();

    /* store the change, if it took place */
    if (old_numdice != NUMDICE) {
      sprintf(string, "Do you wish to set the combat dice to %d? ",
	      NUMDICE);
      bottommsg(string);
      if (y_or_n()) {
	cq_reset();
	write_data();
      } else {
	cq_reset();
      }
    } else {
      cq_reset();
    }

    /* clean up before leaving */
    kill_lock(uplocknum, lock_string);
    exit(SUCCESS);
  }

  /* check for data file */
  if (mflag || rflag) {
#ifdef REMAKE
    /* check if datafile already exists */
    strcpy(string, datafile);
#ifdef COMPRESS
    strcat(string, COMP_SFX);
#endif /*COMPRESS*/
    /* check if datafile currently exists */
    if (exists(string) == 0) {

      /* read in the data */
      if (read_data() == FALSE) {
	remake = FALSE;
	strcpy(world.demigod, LOGIN);
      } else {
	remake = TRUE;
	verify_data(__FILE__, __LINE__);
      }

      /* verify ability to remake the world */
      if (!world.demibuild ||
	  world.demilimit) {

	if (strcmp(loginname, LOGIN) != 0) {
	  fprintf (fupdate, "Sorry -- you can not create a world\n");
	  fprintf (fupdate, "you need to be logged in as %s.\n", LOGIN);
	  exit(FAIL);
	}

      } else {

	if ((strcmp(loginname, LOGIN) != 0) &&
	    (strcmp(loginname, world.demigod) != 0)) {
	  fprintf (fupdate, "Sorry -- you can not create a world\n");
	  fprintf (fupdate, "you need to be logged in as %s", LOGIN);
	  if (strcmp (LOGIN, world.demigod) != 0) {
	    fprintf (fupdate, " or %s", world.demigod);
	  }
	  fprintf (fupdate, ".\n");
	  exit (FAIL);
	}
      }

      if (remake == TRUE) {
	fprintf(fupdate, "***************** WARNING!!!! *****************\n\n");
	fprintf(fupdate, "     There is already a game in progress.\n\n");
	fprintf(fupdate, "***********************************************\n\n");
	fprintf(fupdate, "Do you wish to destroy the current game? ");
	scanf ("%s", string);
	if (strcmp (string, "yes") != 0 && strcmp (string, "y") != 0) {
	  fprintf (fupdate, "Okay... the world is left intact\n");
	  exit (FAIL);
	}
	fprintf (fupdate, "Are you absolutely certain? ");
	scanf ("%s", string);
	if (strcmp (string, "yes") != 0 && strcmp (string, "y") != 0) {
	  fprintf (fupdate, "Okay... the world is left intact\n");
	  exit (FAIL);
	}
	fprintf (fupdate, "The re-destruction of the world has begun...\n");
	/* get rid of extra characters */
	do {
	  remake = getchar();
	} while (remake != '\n');
	remake = TRUE;
	sleep (2);
      }
    }
#else /*REMAKE*/
    /* check for god permissions */
    if (strcmp(loginname, LOGIN) != 0) {
      fprintf (fupdate, "Sorry -- you can not create a world\n");
      fprintf (fupdate, "you need to be logged in as %s.\n", LOGIN);
      exit (FAIL);
    }

    /* check if datafile already exists */
    strcpy(string, datafile);
#ifdef COMPRESS
    strcat(string, COMP_SFX);
#endif /*COMPRESS*/
    if (exists(string) == 0) {
      fprintf (fupdate, "ABORTING: File %s exists\n", datafile);
      fprintf (fupdate, "\tthis means that a game is in progress. To proceed, you must remove \n");
      fprintf (fupdate, "\tthe existing data file. This will, of course, destroy that game.\n\n");
      exit (FAIL);
    }
#endif /* REMAKE */

    makeworld (rflag);
    sprintf (string, "godup.%s", isontag);
    unlink (string);
    exit (SUCCESS);
  }

  /* add a new player to the game */
  if (aflag) {

    /* prevent more than one addition */
    sprintf (lock_string, "godadd.%s", isontag);
    if ((addlocknum = check_lock(lock_string, TRUE)) == -1) {
      fprintf (fupdate, "Someone else is adding\n");
      fprintf (fupdate, "Please try again later.\n");
      exit (FAIL);
    }

    /* read and verify data */
    if (read_data() == FALSE) {
      kill_lock(addlocknum, lock_string);
      exit(FAIL);
    }
    verify_data(__FILE__, __LINE__);

    /* check if a nation exists for this player */
    if (world.verify_login &&
	(strcmp(LOGIN, loginname) != 0) &&
	(strcmp(world.demigod, loginname) != 0)) {

      /* check the nations for the same login name */
      for (i = 0; i < MAXNTN; i++) if ((ntn_ptr = world.np[i]) != NULL) {
	if (strcmp(ntn_ptr->login, loginname) == 0) {
	  fprintf (fupdate, "Nation %s is already owned by you.\n",
		   ntn_ptr->name);
	  fprintf (fupdate, "You may not build another nation\n");
	  kill_lock(addlocknum, lock_string);
	  exit (FAIL);
	}
      }
    }

    fprintf (fupdate, "\n********************************************");
    fprintf (fupdate, "\n*      PREPARING TO ADD NEW PLAYERS        *");
    fprintf (fupdate, "\n********************************************\n");
    if ((world.latestart) &&
	(TURN - START_TURN > world.latestart)) {
      fprintf (fupdate, "more than %d turns have passed since game start!\n",
	       world.latestart);
      fprintf (fupdate, "permission of game administrator required\n");

      /* get password and perform any encryption */
      fprintf(fupdate, "\nWhat is the Conquer Add Nation Password:");
      strncpy(tmppass, getpass(""), PASSLTH + 1);
#ifdef CRYPT
      strncpy(string, crypt(tmppass, SALT), PASSLTH);
#else
      strncpy(string, tmppass, PASSLTH);
#endif
      string[PASSLTH] = '\0';
      if (strncmp(string, world.addpwd, PASSLTH) != 0) {
        fprintf (fupdate, "sorry...\n");
	kill_lock(addlocknum, lock_string);
        exit (FAIL);
      }
    }

    /* intercept interrupts */
    signal (SIGINT, SIG_IGN);
    signal (SIGQUIT, SIG_IGN);
    signal (SIGHUP, hangup);
    signal (SIGTERM, hangup);
    signal(SIGFPE, hangup);
    signal(SIGSEGV, hangup);
    need_hangup = TRUE;

    /* create the nation and store the information */
    if (newlogin(FALSE)) {
      write_data();
    }

    /* clean up before leaving */
    kill_lock(addlocknum, lock_string);
    exit (SUCCESS);
  }

  /* perform the game update */
  if (xflag) {

    /* catch hangups */
    signal (SIGHUP, hangup);
    signal (SIGTERM, hangup);
    need_hangup = TRUE;

    /* check for player additions */
    sprintf (string, "godadd.%s", isontag);
    if (check_lock (string, FALSE) == -1) {
      fprintf (fupdate, "Someone is adding a new nation\n");
      fprintf (fupdate, "Please try again later.\n");
      exit (FAIL);
    }

    /* check if an update is going on */
    sprintf (lock_string, "godup.%s", isontag);
    if ((uplocknum = check_lock(lock_string, TRUE)) == -1) {
      fprintf (fupdate, "Another update is still executing.\n");
      fprintf (fupdate, "Update aborted.\n");
      exit (FAIL);
    }

    /* read and check the data */
    if (read_data() == FALSE) {
      kill_lock(uplocknum, lock_string);
      exit(FAIL);
    }
    verify_data(__FILE__, __LINE__);

    if ((strcmp(loginname, LOGIN) != 0) &&
        (strcmp(loginname, world.demigod) != 0)) {
      fprintf (fupdate, "Sorry -- your login name is invalid for updating\n");
      fprintf (fupdate, "you need to be logged in as %s", LOGIN);
      if (strcmp (LOGIN, world.demigod) != 0) {
        fprintf (fupdate, " or %s", world.demigod);
      }
      fprintf (fupdate, "\n");
      kill_lock(uplocknum, lock_string);
      exit (FAIL);
    }

#ifdef RUNSTOP
#if UPDATESLEEP
  retry_sleep_check:
    /* check if any players are on */
    for (i = 0; i < MAXNTN; i++) if ((ntn_ptr = world.np[i]) != NULL) {
      sprintf (string, "%s.%s", ntn_ptr->name, isontag);
      if (check_lock(string, FALSE) == -1) {
        fprintf (fupdate, "Nation %s is still in the game.\n", ntn_ptr->name);
        fprintf (fupdate, "Sleeping %d seconds.\n", UPDATESLEEP);
	sleep(UPDATESLEEP);
	goto retry_sleep_check;
      }
    }
#else /* UPDATESLEEP */
    /* check if any players are on */
    for (i = 0; i < MAXNTN; i++) if ((ntn_ptr = world.np[i]) != NULL) {
      sprintf (string, "%s.%s", ntn_ptr->name, isontag);
      if (check_lock(string, FALSE) == -1) {
        fprintf (fupdate, "Nation %s is still in the game.\n", ntn_ptr->name);
        fprintf (fupdate, "Update aborted.\n");
	kill_lock(uplocknum, lock_string);
        exit (FAIL);
      }
    }
#endif /* UPDATESLEEP */
#endif /* RUNSTOP */

    /* go to it */
    is_update = TRUE;
    update();
    write_data();

    /* clean up shop */
    if ((timefp = fopen(timefile, "w")) != NULL) {
      fprintf(timefp, "%s\n", mach_time());
      fclose(timefp);
    } else {
      fprintf(fupdate, "Warning: could not store update time\n");
    }
    kill_lock(uplocknum, lock_string);
#ifndef VMS
    sprintf (string, "%s *%s *%s *%s.%03d 2> /dev/null", REMOVE_NAME,
	     isontag, exetag, newsfile, TURN - START_TURN - MAXNEWS);
#else
    sprintf (string, "%s *.%s;*, *.%s;*, %s.%03d;* /nolog",
	     REMOVE_NAME, isontag, exetag, newsfile,
	     TURN - START_TURN - MAXNEWS);
#endif
#ifdef DEBUG
    fprintf(fupdate, "%s\n", string);
#endif /* DEBUG */
    system(string);
    exit (SUCCESS);
  }

  fprintf (fupdate, "Error: a functional option must be specified.\n");
 usage_list:
  /*  print out command line arguments */
  fprintf (fupdate, "Command line format: %s [-nc] [-ACEQITZamx -dDIR]\n",
	   argv[0]);
  fprintf (fupdate, "\t-nc      do not read in %s file [first option only]\n",
	   CONQRC_FILE);
  fprintf (fupdate, "\t-E       edit the world campaign settings\n");
  fprintf (fupdate, "\t-I       display some statistics\n");
  fprintf (fupdate, "\t-A       add some npc nations to the nations file\n");
  fprintf (fupdate, "\t-Q       examine the combat dice probabilities\n");
  fprintf (fupdate, "\t-T       (dis/en)able logins; %d sec time to finish for those in\n", TIMETOBOOT);
  fprintf (fupdate, "\t-Z       delete all npc nation specifications\n");
  fprintf (fupdate, "\t-a       add new player\n");
  fprintf (fupdate, "\t-d DIR   to specify a different game directory\n");
  fprintf (fupdate, "\t-o ofile send update output to the given file\n");
  fprintf (fupdate, "\t-m       make a world\n");
  fprintf (fupdate, "\t-x       execute program\n");
  exit (SUCCESS);
  return(0);
}
