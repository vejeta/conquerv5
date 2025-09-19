/*
 * Written by Adam Bryant (adb@bu.edu)
 *
 * This was written as a quicky conversion program... do with
 * it what you will.
 *
 */
#include <stdio.h>
#ifndef TRUE
#define TRUE (1)
#define FALSE (0)
#endif /* TRUE */

/* syntax */
char *usage = "Usage: %s [infile [outfile]]\n";

main(argc,argv)
  int argc;
  char *argv[];
{
  FILE *fpi = stdin, *fpo = stdout;
  int ch, count = 1, posit;

  /* check for the input file */
  if (argc>count && (fpi = fopen(argv[count],"r"))==(FILE *)NULL) {
    fprintf(stderr,"%s: error opening input file <%s>\n",argv[0],argv[count]);
    exit(1);
  }
  count++;

  /* check for the output file */
  if (argc>count && (fpo = fopen(argv[count],"w"))==(FILE *)NULL) {
    fprintf(stderr,"%s: error opening output file <%s>\n",argv[0],argv[count]);
    exit(1);
  }
  count++;

  /* check for proper number of arguments */
  if (argc>count) {
    fprintf(stderr,usage,argv[0]);
    exit(1);
  }

  /* processing loop */
  count = 1;
  posit = 0;
  while (!feof(fpi)) {

    /* switch appropriately */
    switch (ch = getc(fpi)) {
    case '\n':
      if (count == 0) putc('\n', fpo);
      posit = 0;
      count++;
      break;
    case '^':
      /* now check for page break combination */
      if (posit == 0) {
	/* page break indicator? */
	if ((ch = getc(fpi)) == 'L') {
	  putc('\014', fpo);
	  count = 0;
	  posit = 1;
	  break;
	} else {
	  /* put it back and continue */
	  ungetc(ch, fpo);
	  ch = '^';
	}
      }
      /* just let it go by */
    default:
    Finish_Off:
      /* check for finish */
      if (ch == EOF) break;

      /* take care of pending new lines */
      while (count > 1) {
	putc('\n', fpo);
	count--;
	posit = 0;
      }
      putc((char)ch,fpo);
      count = 0;
      posit++;
      break;
    }

  }

  /* close up shop */
  if (fpi != stdin) fclose(fpi);
  if (fpo != stdout) fclose(fpo);
  exit(0);
}
