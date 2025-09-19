
/*
 * A program to convert conquer-maps to postscript Feel free to hack'n slash
 * Comments should be sent to  d8forma@dtek.chalmers.se
 * 
 */

#include <stdio.h>
#include "psmap.h"

char buffer[BUFSIZ];
int c, xsize, ysize;
int xmin, ymin, xmax, ymax, centx, centy;
FILE *infile, *outfile, *fh, *fopen();
int grid = TRUE, sqsize = 9, maptype = SIMPLE, onepage = FALSE;
int verbose = FALSE, coords = TRUE, center = TRUE, note = FALSE;
int pagewidth, pageheight, xoffset, yoffset;
char title[81] = "", foot[81], fontname[81];
char progname[80];

int
parsepagesize(buf)
  char *buf;
{
  if (!strcmp(buf, "A4"))
    return (1);
  if (!strcmp(buf, "a4"))
    return (1);
  if (!strcmp(buf, "LETTER"))
    return (2);
  if (!strcmp(buf, "letter"))
    return (2);
  return (0);
}

void
setpagesize(defpag)
  int defpag;
{
  switch (defpag) {
  case 1:
    pagewidth = PAGEWIDTH_A4;
    pageheight = PAGEHEIGHT_A4;
    xoffset = XOFFSET_A4;
    yoffset = YOFFSET_A4;
    break;
  case 2:
    pagewidth = PAGEWIDTH_LETTER;
    pageheight = PAGEHEIGHT_LETTER;
    xoffset = XOFFSET_LETTER;
    yoffset = YOFFSET_LETTER;
    break;
  default:
    pagewidth = PAGEWIDTH_OTHER;
    pageheight = PAGEHEIGHT_OTHER;
    xoffset = XOFFSET_OTHER;
    yoffset = YOFFSET_OTHER;
    break;
  }
}

void
get_pagesize()
{
  char *buf;
  int defpag = DEFAULTPAGE;
  
  buf = (char *) getenv("CONQ_PSMAPDEFAULTPAGE");
  if (buf != NULL)
    defpag = parsepagesize(buf);
  setpagesize(defpag);
}

void
psstring(fh, str)
  FILE *fh;
  char *str;
{
  fprintf(fh, "(");
  while (*str != NULL) {
    switch (*str) {
    case ('('):
      fprintf(fh, "\\(");
      break;
    case (')'):
      fprintf(fh, "\\)");
      break;
    case ('\\'):
      fprintf(fh, "\\\\");
      break;
    case ('\n'):
    case ('\f'):
      break;
    default:
      fputc(*str, fh);
      break;
    }
    str++;
  }
  fprintf(fh, ")");
}

int
isinstr(string, word)
  char *string, *word;
{
  int i,l1=strlen(string),l2=strlen(word);
  
  if (l1 < l2) return(FALSE);
  for(i = 0; i < l1; i++ ) {
    if (l1-i<l2) break;
    if (string[i] == word[0]) {
      if (strncmp(string+i,word,l2)==0) return(TRUE);
    }
  }
  return(FALSE);
}

int
getmaptype(string)
  char *string;
{
  if (isinstr(string, "Altitude"))
    return (ALTITUDES);
  if (isinstr(string, "Designation"))
    return (DESIGNATIONS);
  if (isinstr(string, "Nation"))
    return (NATIONS);
  if (isinstr(string, "Vegetation"))
    return (VEGETATIONS);
  return (SIMPLE);
}

void
readmap()
{
  int x, none;
  
    /*
     * Now we read the map. Anybody got an idea how big it is ? I don't, but
     * who cares ?
     */
  
  x = 0;
  xsize = 0;
  ysize = 0;
  xmin = 0xffffff;
  xmax = (-1);
  ymin = (-1);
  ymax = 0;
  none = TRUE;
  
  fprintf(outfile, "/Map [\n");
  
  while ((c = fgetc(infile)) != EOF) {
    switch (c) {
    case ('\n'):
      if (xsize == 0)
	xsize = x;
      if (xsize != x) {
	fprintf(stderr, "Error in map-file. Lines different length\n");
	exit(1);
      }
      x = 0;
      if ((xmax > -1) && (ymin == -1))
	ymin = ysize;
      ysize++;
      fprintf(outfile, ")\n");
      none = TRUE;
      break;
    case (' '):
      x++;
      if (none) {
	fprintf(outfile, "(");
	none = FALSE;
      }
      fprintf(outfile, " ");
      break;
    default:
      if (x < xmin)
	xmin = x;
      if (x > xmax)
	xmax = x;
      if (ysize > ymax)
	ymax = ysize;
      x++;
      if (none) {
	fprintf(outfile, "(");
	none = FALSE;
      }
      fprintf(outfile, "%c", c);
      break;
    }
  }
  
  fprintf(outfile, "] def\n\n");
  
  /* Did I get anything ? */
  if (xmax == -1) {
    fprintf(stderr, "Empty input file\n");
    exit(1);
  }
  if (verbose) {
    fprintf(stderr, "Map is %d * %d  squares\n", xsize, ysize);
    fprintf(stderr, "You can see %d * %d  squares\n",
	    xmax - xmin + 1, ymax - ymin + 1);
  }
}

void
buildps()
{
  int xbeg, ybeg, xnumb, ynumb, x, y, xpages, ypages, xcorr, ycorr;
  
  /*
   * The map should be dumped to the outfile now. Maybe we should tell the
   * printer what to do with it?
   */
  
  xpages = 1 + (xmax - xmin) * sqsize / (pagewidth - XMARGINS);
  ypages = 1 + (ymax - ymin) * sqsize / (pageheight - YMARGINS);
  
  if (verbose)
    fprintf(stderr, "The map will be %d * %d pages\n", xpages, ypages);
  
  fprintf(outfile, "/xmin %d def\n", xmin);
  fprintf(outfile, "/ymin %d def\n", ymin);
  fprintf(outfile, "/xmax %d def\n", xmax);
  fprintf(outfile, "/ymax %d def\n", ymax);
  fprintf(outfile, "/xpages %d def\n", xpages);
  fprintf(outfile, "/ypages %d def\n", ypages);
  fprintf(outfile, "/sqsize %d def\n", sqsize);
  fprintf(outfile, "/title ");
  psstring(outfile, title);
  fprintf(outfile, " def\n");
  fprintf(outfile, "/foot ");
  psstring(outfile, foot);
  fprintf(outfile, " def\n");
  fprintf(outfile, "/foot2 (ConqPS Version %s) def\n", VERSION);
  fprintf(outfile, "/PAGEWIDTH %d def\n", pagewidth);
  fprintf(outfile, "/PAGEHEIGHT %d def\n", pageheight);
  fprintf(outfile, "/XMARGINS %d def\n", XMARGINS);
  fprintf(outfile, "/YMARGINS %d def\n", YMARGINS);
  fprintf(outfile, "/XOFFSET %d def\n", xoffset);
  fprintf(outfile, "/YOFFSET %d def\n", yoffset);
  fprintf(outfile, "/maptype %d def\n", maptype);
  fprintf(outfile, "/font /%s def\n", fontname);
  fprintf(outfile, "/grid ");
  if (grid) {
    fprintf(outfile, "true def\n");
  } else fprintf(outfile, "false def\n");
  fprintf(outfile, "/coords ");
  if (coords) {
    fprintf(outfile, "true def\n");
  } else fprintf(outfile, "false def\n");
  fprintf(outfile, "/noteq ");
  if (note) {
    fprintf(outfile, "true def\n");
  } else fprintf(outfile, "false def\n");
  
  fprintf(outfile, "\n\n");

  fh = fopen(PSFILE, "r");
  
  if (fh == NULL) {
    perror(progname);
    exit(-10);
  }
  while ((c = fgetc(fh)) != EOF)
    fputc(c, outfile);

  fclose(fh);

  fprintf(outfile, "\n %% Here we start the magic\n");

  xnumb = (int) ((pagewidth - XMARGINS) / sqsize);
  ynumb = (int) ((pageheight - YMARGINS) / sqsize);

  if (center) {
    xcorr = (int) ((xpages * xnumb - (xmax - xmin)) / 2);
    ycorr = (int) ((ypages * ynumb - (ymax - ymin)) / 2);
  } else {
    xcorr = 0;
    ycorr = 0;
  }

  if (onepage) {
    fprintf(outfile, "%d %d %d %d %d %d DoPage\n",
		1, 1, centx - xnumb / 2, xnumb, centy - ynumb / 2, ynumb);
  } else for (x = 0; x < xpages; x++)
    for (y = 0; y < ypages; y++) {
      xbeg = x * xnumb + x + xmin - xcorr;
      ybeg = y * ynumb + y + ymin - ycorr;
      fprintf(outfile, "%d %d %d %d %d %d DoPage\n",
	      x + 1, y + 1, xbeg, xnumb, ybeg, ynumb);
    }
}

int
main(argc, argv)
  int argc;
  char **argv;
{
  extern char *optarg;
  extern int optind;
  char *buf, firstline[81];
  
  strcpy(progname, argv[0]);
  infile = stdin;
  outfile = stdout;
  buf = (char *) getenv("CONQ_PSFONT");
  if (buf != NULL) {
    strncpy(fontname, buf, 80);
    fontname[79] = '\0';
  } else strcpy(fontname, "Times-Roman");
  get_pagesize();
  
  while ((c = getopt(argc, argv, "nuf:gs:t:vcho:p:lW:L:X:Y:")) != -1)
    switch (c) {
    case 'h':
      fprintf(stderr, "%s version %s\n", progname, VERSION);
      fprintf(stderr, "Default pagesize is ");
      switch (DEFAULTPAGE) {
      case 1:
	fprintf(stderr, "A4\n");
	break;
      case 2:
	fprintf(stderr, "LETTER\n");
	break;
      default:
	fprintf(stderr, "OTHER\n");
	break;
      }
      fprintf(stderr, USAGE, progname);
      fprintf(stderr, "\t-c  Turn off coordinates\n");
      fprintf(stderr, "\t-f  Set the font\n");
      fprintf(stderr, "\t-g  Turn off grid\n");
      fprintf(stderr, "\t-h  Show this text\n");
      fprintf(stderr, "\t-l  Print large maps\n");
      fprintf(stderr, "\t-L  Set the pagelength\n");
      fprintf(stderr, "\t-n  Turn off map centering\n");
      fprintf(stderr, "\t-o  Show one page centered around x,y\n");
      fprintf(stderr, "\t-p  Set pagesize (A4,LETTER or OTHER)\n");
      fprintf(stderr, "\t-s  Set size of square (default: %d)\n", sqsize);
      fprintf(stderr, "\t-t  Set the title of the map\n");
      fprintf(stderr, "\t-u  Force simple map output (just letters)\n");
      fprintf(stderr, "\t-v  Verbose mode\n");
      fprintf(stderr, "\t-W  Set the pagewidth\n");
      fprintf(stderr, "\t-X  Set the X-offset\n");
      fprintf(stderr, "\t-Y  Set the Y-offset\n");
      exit(1);
    case 'u':
      maptype = FORCED;
      break;
    case 'o':
      onepage = TRUE;	/* Mode one of onepage */
      if (2 != sscanf(optarg, "%d,%d", &centx, &centy)) {
	fprintf(stderr, "Error in coordinates to o-option\n");
	exit(1);
      }
      break;
    case 'l':
      note = TRUE;
      break;
    case 'p':
      setpagesize(parsepagesize(optarg));
      break;
    case 'n':
      center = FALSE;
      break;
    case 'g':
      grid = FALSE;
      break;
    case 's':
      sscanf(optarg, "%d", &sqsize);
      break;
    case 'f':
      strncpy(fontname, optarg, 80);
      fontname[79] = '\0';
      break;
    case 't':
      strncpy(title, optarg, 80);
      title[79] = '\0';
      break;
    case 'v':
      verbose = TRUE;
      break;
    case 'c':
      coords = FALSE;
      break;
    case 'W':
      sscanf(optarg, "%d", &pagewidth);
      break;
    case 'L':
      sscanf(optarg, "%d", &pageheight);
      break;
    case 'X':
      sscanf(optarg, "%d", &xoffset);
      break;
    case 'Y':
      sscanf(optarg, "%d", &yoffset);
      break;
    default:
      fprintf(stderr, USAGE, argv[0]);
      exit(1);
    }
  if (optind < argc)
    infile = fopen(argv[optind], "r");
  
  if (infile == NULL) {
    perror(progname);
    exit(-10);
  }
  if (++optind < argc)
    outfile = fopen(argv[optind], "w");
  
  if (outfile == NULL) {
    perror(progname);
    exit(-10);
  }
  setbuf(outfile, buffer);	/* They recomended this on the net today */
  
  if (++optind < argc) {
    fprintf(stderr, USAGE, argv[0]);
    exit(1);
  }
  /* If verbose identify program */
  
  if (verbose)
    fprintf(stderr, "Psmap version %s\n", VERSION);
  

  /* First we check if the infile is a valid conquer map file */
  
  if (NULL == fgets(firstline, 80, infile)) {
    fprintf(stderr, "Empty input file\n");
    exit(1);
  }
  if (0 != strncmp(firstline, MATCHSTRING, strlen(MATCHSTRING))) {
    fprintf(stderr, "Not a Conquer Map file\n");
    exit(1);
  }
  for (c = 0; c < strlen(firstline); c++) {
    if (firstline[c] == ':')
      break;
  }

  strncpy(foot, firstline, c);
  foot[c] = '\0';
  if (title[0] == '\0') {
    strncpy(title, &firstline[c + 1], 80);
    title[79] = '\0';
  }
  /* Find out which type of map it is */
  
  if (maptype != FORCED) {
    maptype = getmaptype(&firstline[c + 1]);
  } else maptype = SIMPLE;
  
  if (verbose) {
    fprintf(stderr, "Maptype is ");
    switch (maptype) {
    case (SIMPLE):
      fprintf(stderr, "simple\n");
      break;
    case (ALTITUDES):
      fprintf(stderr, "altitudes\n");
      break;
    case (DESIGNATIONS):
      fprintf(stderr, "designations\n");
      break;
    case (NATIONS):
      fprintf(stderr, "nations\n");
      break;
    case (VEGETATIONS):
      fprintf(stderr, "vegetations\n");
      break;
    default:
      break;
    }
  }
  /* Send the first part of the postscript file to outfile */
  
  fprintf(outfile, "%%!\n");
  fprintf(outfile, "%% Created by conqps version %s\n\n", VERSION);
  if (note)
    fprintf(outfile, "\nnote\n\n");
  fprintf(outfile, "%% Here comes the map data:\n");
  
  /* Parse the map */
  
  readmap();

  /* Build the last part of postscript file */

  buildps();

  /* Phu that was it.  */

  if (infile != NULL)
    fclose(infile);
  if (outfile != NULL)
    fclose(outfile);

  return (0);
}
