/*
 * sort.c
 *
 * Written by Adam Bryant as a replacement for 'sort'
 * for use with the conquer program.  Note that it
 * is initially written with this program in mind and
 * will not have many features useful elsewhere.
 *
 * VARIOUS METHODS OF USE:
 *
 *   standard in to standard out:
 *          'cat foo | conqsort > foonew'
 *   file input to standard output:
 *          'conqsort foo > foonew'
 *   file input to file output:
 *          'conqsort foo foonew'
 *
 * OR MAY EVEN OVERWRITE INITIAL FILE VIA:
 *
 *          'conqsort foo foo'
 *
 * Design Notes:
 *   - all of the file will have to be placed into memory;
 *   - to allow for any line length, space will be malloced()
 *      as it comes in.
 *
 * Initial Revision:		(adb@bucsf.bu.edu)
 *     Tuesday March 21th, 1989 - Began the program at 23:26 EST
 *     Wednesday March 22nd, 1989 - Finished the initial version 11:29 EST
 *     Wednesday July 5th, 1989 - Stopped use of sysexits.h 7:22 EST
 *     Friday April 5th, 1991 - Added in some system ULTRIX tests 2:56 PM EST
 *     February, 1992 (adb@csa.bu.edu === bucsf dead) ;-)
 *     Friday April 24th, 1992 - Fixed "repeats" code which was given
 *		another user (who?). 4:25 PM EST
 *     Thursday May 7th, 1992 - Make "1 more times" -> "1 more time".
 *     Saturday October 10th, 1992 - Make sure blank lines aren't messed up.
 *
 */

#include <stdio.h>
#include <ctype.h>

/* function checker */
#ifndef toupper
extern int toupper();
#endif /* toupper */
#ifndef islower
extern int islower();
#endif /* islower */

/* system definitions just in case */
#ifndef FALSE
#define FALSE (0)
#define TRUE  (1)
#endif /* FALSE */

/* system exit definitions from sysexits.h on BSD machines */
#ifndef EX_OK
#define EX_OK             0   /* successful termination */
#define EX_USAGE          64  /* invalid command line format */
#define EX_NOINPUT        66  /* could not open input file */
#define EX_SOFTWARE       70  /* software error; couldn't malloc */
#define EX_CANTCREAT      73  /* could not create output file */
#endif /* EX_OK */

/* number of characters to compare by default */
#define DEFAULT_COMP 2
#define MAX_STR      200

/* structure for holding a line of information */
typedef struct holder {
  char *line;
  struct holder *next;
} L_DATA, *L_PTR;

/* pointer to head of list of lines */
L_PTR head;

/* usage string */
static char usage[]="Usage: %s [-num -h] [infile] [outfile]\n\
	-h		display this screen\n\
	-i		treat upper and lower case characters as equivalent\n\
	-num		Sort on first num characters\n";

/* number of characters for comparison */
int compnum = DEFAULT_COMP;

/* flag for folding upper and lowercase characters */
int iflag = FALSE;

/* file pointers to infile and outfile */
FILE *infile, *outfile;

/* macros */
#define UPPER(x) ((islower(x))?(toupper(x)):(x))

/* external system funtions */
#ifdef BSD
#include <strings.h>
#else
#include <string.h>
#endif /* BSD */
#ifndef ULTRIX
#ifndef AIX
extern int fprintf();
#endif /* AIX */
#endif /* ULTRIX */
#ifdef HPUX
extern int __filbuf();
#else
extern int _filbuf();
#endif /* HPUX */
extern int fclose();
extern void exit();
extern char *malloc();

/* the whole enchaladas */
int
main(argc, argv)
  int argc;
  char *argv[];
{
  /* declare temporary variables and functions */
  int i, j, num_args = 0, l, innum = 0,outnum = 0;
  FILE *fopen();
  int get_line();
  void place(), send_out();

  /* input string */
  char data[MAX_STR];

#ifndef VAXC
  infile = stdin;
  outfile = stdout;
#endif

  /* parse command line arguments */
  for (i = 1; i < argc; i++) {

    /* process non-switch statements */
    if (argv[i][0] != '-') {
      num_args++;
      switch(num_args) {
      case 1:
	/* do not open unless arguments are valid */
	innum = i;
	break;
      case 2:
	/* do not open unless arguments are valid */
	outnum = i;
	break;
      default:
	fprintf(stderr, "%s: Too many parameters\n", argv[0]);
	fprintf(stderr, usage, argv[0]);
	exit(EX_USAGE);
	break;
      }
      continue;
    }
    
    /* process switch statements */
    l = strlen(argv[i]);
    for (j = 1; j < l; j++) {
      switch(argv[i][j]) {
      case 'h':
	fprintf(stderr, usage, argv[0]);
	exit(EX_OK);
	break;
      case 'i':
	iflag = TRUE;
	break;
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
	/* assign new comparison number */
	compnum = 0;
	for(; j < l && argv[i][j] >= '0' && argv[i][j] <= '9'; j++) {
	  compnum *= 10;
	  compnum += argv[i][j] - '0';
	}
	j--;
	break;
      default:
	fprintf(stderr, "%s: invalid option '%c' in <%s>\n",
		argv[0], argv[i][j], argv[i]);
	fprintf(stderr, usage, argv[0]);
	exit(EX_USAGE);
	break;
      }
    }
  }
  
  /* open input file now */
  if (innum != 0) {
    if ((infile = fopen(argv[innum], "r")) == (FILE *)NULL) {
      fprintf(stderr, "%s: can't open file <%s> for input\n",
	      argv[0], argv[innum]);
      exit(EX_NOINPUT);
    }
  }

  /* ======= main processing loop ======== */

  while(!feof(infile)) {

    /* remove any lines less than sort characters */
    if (get_line(data) >= compnum) {
      place(data);
    }

  }

  /* ==== end of main processing loop ==== */

  /* close input file if not stdin */
  if (infile != stdin) {
    (void) fclose(infile);
  }
  
  /* open output file now */
  if(outnum != 0) {
    if ((outfile = fopen(argv[outnum], "w")) == (FILE *)NULL) {
      fprintf(stderr, "%s: cannot open file <%s> for output\n",
	      argv[0], argv[outnum]);
      exit(EX_CANTCREAT);
    }
  }
  send_out();

  /* close output file if not stdout */
  if (outfile != stdout) {
    (void) fclose(outfile);
  }

  exit(EX_OK);
  return(0);
}

/* routine to read all characters in until carriage returns */
int
get_line(data)
  char data[];
{
  int in,ch;

  /* discard exceptionally long lines */
  for (in = 0;
      !feof(infile) && (in < MAX_STR - 1) && ((ch = getc(infile)) != '\n');
      ) {
    /* copy valid input into data */
    if ((ch == '\t') || (ch == ' ') || ((ch >= ' ') && (ch <= '~'))) {
      data[in] = ch;
      in++;
    }
  }

  /* end string */
  data[in] = '\0';

  return(in);
}

/* routine to output entire sorted file to outfile */
void
send_out()
{
  L_PTR temp = head, prev;
  int cnt = 0;

  fprintf(outfile, "%s\n", temp->line);
  prev = temp;
  temp = temp->next;
  
  while (temp != (L_PTR)NULL) {
    if ((strcmp(temp->line, prev->line) == 0) &&
	(prev->line[0] != '5')) {
      cnt++;
    } else if (cnt != 0) {
      fprintf(outfile, "%-*.*s\t\tLast message repeated %d more %s.\n",
 	      compnum, compnum, prev->line, cnt,
	      (cnt == 1) ? "time" : "times");
      cnt = 0;
      fprintf(outfile, "%s\n", temp->line);
    } else {
      fprintf(outfile, "%s\n", temp->line);
    }
    prev = temp;
    temp = temp->next;
  }
}

/* routine to sort list as it comes in */
void
place(data)

  char data[];
{
  L_PTR temp, build_node();
  int comp_line();

  /* find location for placing input */
  if (head == (L_PTR)NULL) {
    /* begin list properly */
    head = build_node(data,(L_PTR)NULL);
  } else if (comp_line(head->line, data) == 1) {
    /* add to beginning of list */
    head = build_node(data,head);
  } else {
    /* otherwise add in proper position */
    temp = head;
    while ((temp->next != (L_PTR)NULL) &&
	   (comp_line(temp->next->line, data) != 1)) {
      temp = temp->next;
    }
    temp->next = build_node(data, temp->next);
  }
}

/* routine to compare two lines through N characters */
/* where N is given by the variable compnum          */
/*    returns:  0 on equal                           */
/*             -1 on a preceding b                   */
/*              1 on a following b                   */
int
comp_line(a, b)
  char *a, *b;
{
  int i;

  for(i = 0; i < compnum; i++) {
    /* do checks */
    if (iflag) {
      if (UPPER(a[i]) < UPPER(b[i])) return(-1);
      if (UPPER(b[i]) < UPPER(a[i])) return(1);
    } else {
      if (a[i] < b[i]) return(-1);
      if (b[i] < a[i]) return(1);
    }
    if (a[i] == '\0') return(0);
  }
  return(0);
}

/* create L_DATA structure containing a line of data */
/* and the next value set to the given location      */
L_PTR
build_node(data, nptr)
  char data[];
  L_PTR nptr;
{
  L_PTR temp;
  char *strcpy();
  
  /* build the memory space */
  if ((temp = (L_PTR)malloc(sizeof(L_DATA))) == (L_PTR)NULL) {
    fprintf(stderr, "Error in creating structure memory!\n");
    exit(EX_SOFTWARE);
  }
  if ((temp->line = (char *)malloc((strlen(data) + 1) *
				   sizeof(char))) == NULL) {
    fprintf(stderr, "Error in creating data memory!\n");
    exit(EX_SOFTWARE);
  }

  /* assign the values */
  (void) strcpy(temp->line, data);
  temp->next = nptr;
  return(temp);
}
