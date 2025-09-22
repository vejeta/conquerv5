// SPDX-License-Identifier: GPL-3.0-or-later
/* This was written as a quicky conversion program... */
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
#include <stdio.h>

/* At the top of ezconv.c */
#if defined(__STDC__) || defined(__STDC_VERSION__)
    #include <stdlib.h>
#else
    extern void exit();
#endif

#ifndef TRUE
#define TRUE (1)
#define FALSE (0)
#endif /* TRUE */

/* syntax */
char *usage = "Usage: %s [infile [outfile]]\n";

int main(argc,argv)
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
	  ungetc(ch, fpi);
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
