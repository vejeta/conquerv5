// SPDX-License-Identifier: GPL-3.0-or-later
/* Provides a mvprintw function for VAX C.  Written for CONQUER */
/* John R Carr  <JRCARR@IUP.BITNET>                             */
#include "header.h"
#include "paramX.h"
#define PASSLTH 8
#ifdef VAXC
#include stdio
#include varargs
#include <curses.h>
int
mvprintw(y, x, fspec, va_alist)
int y,x;
char *fspec;
va_dcl
{
  va_list varap;
  char string[256];

  string[0] = 0;

  move(y, x);
  va_start(varap);

  vsprintf(string, fspec, varap);
  printw("%s", string);
}

void
getlogon PARM_1 (int, user)
{
  char temp[13];
  char *temp2;
  int i;

  cuserid(temp);
  for(i=0;i<13;i++){
    if(temp[i]=='\0') break;
  }
  strncpy(temp2, temp, i+1);
}

char *
getpass PARM_1(char *, prompt)
{
  static unsigned char buffer[PASSLTH + 1];

  system("set term/noecho");
  printf(prompt);
  gets(buffer);
  system("set term/echo");
  return(buffer);
}
#endif /*VAXC*/

#ifdef VMS
#include descrip
#include iodef
#include tt2def

static $DESCRIPTOR (term_name, "SYS$INPUT:");
struct char_buffer_type { unsigned short int dummy;
			  unsigned short int size;
			  unsigned long int tchars;
			  unsigned long int tchars2; } oldbuf, newbuf;
short term_chan;

void
setterm_pas()
{
  sys$assign(&term_name,&term_chan,0,0);

  sys$qiow(0,term_chan,IO$_SENSEMODE,0,0,0,&oldbuf,12,0,0,0,0);
  newbuf = oldbuf;

#ifdef TSERVER
  newbuf.tchars2 = newbuf.tchars2 | TT2$M_PASTHRU;
#endif /* TSERVER */

  sys$qiow(0,term_chan,IO$_SETMODE,0,0,0,&newbuf,12,0,0,0,0);
}

void
resetterm()
{
  sys$qiow(0,term_chan,IO$_SETMODE,0,0,0,&oldbuf,12,0,0,0,0);
  sys$dassgn(term_chan);
}
#endif /* VMS */
