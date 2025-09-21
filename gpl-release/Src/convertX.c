// SPDX-License-Identifier: GPL-3.0-or-later
/* various routines to change one thing into another */
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
#include "dataX.h"
#include "magicX.h"
#include "keyvalsX.h"

/* FORM_STR -- Create a string of characters for later use */
void
form_str PARM_3(char *, out_str, char *, in_str, int, full)
{
  int o_cnt = 1, i_cnt, ch;

  /* traverse the input string */
  for (i_cnt = 0; in_str[i_cnt] != '\0'; i_cnt++) {

    /* build it properly */
    ch = in_str[i_cnt];
    if (!isprint(ch)) {
      out_str[o_cnt++] = '^';
      out_str[o_cnt++] = non_cntrl(ch);
    } else {
      if (full) out_str[o_cnt++] = '\'';
      if ((ch == '^') || (ch == '\'') ||
	  (ch == '\"') || (ch == '\\')) {
	out_str[o_cnt++] = '\\';
      }
      out_str[o_cnt++] = ch;
      if (full) out_str[o_cnt++] = '\'';
    }

    /* add in spacing for full build */
    if (full) {
      out_str[o_cnt++] = '-';
    }
  }

  /* form the border of the string */
  if (i_cnt == 0) {
    strcpy(out_str, "'^@'");
  } else if (i_cnt == 1) {
    out_str[0] = '\'';
    out_str[o_cnt++] = '\'';
    out_str[o_cnt] = '\0';
  } else {
    out_str[0] = '"';
    out_str[o_cnt++] = '"';
    out_str[o_cnt] = '\0';
  }
}

/* LIST_MAGICS -- List any magic powers encountered into a string */
int
list_magics PARM_3(char *, out_str, int, mcls, long, powlist)
{
  int i, num = 0;

  /* check input */
  if ((out_str == NULL) ||
      (mcls < 0) ||
      (mcls >= MAG_NUMBER)) {
    return(num);
  }

  /* check for a quick out */
  out_str[0] = '\0';
  if (powlist == 0L) {
    return(num);
  }

  /* scan all of the powers of the given class */
  for (i = 0; i < mclass_list[mcls].maxval; i++) {
    if ((1L << i) & powlist) {
      num++;
      if (num > 1) {
	strcat(out_str, ", ");
      }
      strcat(out_str, mclass_list[mcls].pow_list[i].name);
    }
  }

  /* done */
  return(num);
}

/* ROMAN_VALUE -- Integer value of the given base */
static long
roman_value PARM_1 (int, value)
{
  long hold;
  int count;

  /* loop value times */
  hold = 1L;
  for (count = 0; count < value; count++) {
    if (count % 2 == 0) {
      hold *= 5L;
    } else {
      hold *= 2L;
    }
  }

  /* give back results */
  return(hold);
}

/* ROMAN_BASE -- Return the nearest base value I,V,X,... etc.
                 note that 4 => 5, 9 => 10, 40 => 50, etc.    */
int
roman_base PARM_1 (int, value)
{
  long hold = 4L, base = 5L;
  int count = 0;

  while (TRUE) {

    if (value < hold) {
      break;
    }
    count++;
    if (count % 2 == 0) {
      hold -= base;
      hold *= 10L;
      base *= 10L;
    } else {
      hold += base;
    }

  }
  return(count);
}

/* ROMAN_NUMBER -- This routine places a string containing the roman
                   numeral representation of a number.               */
void
roman_number PARM_2 ( char *, str, int, value )
{
  int neg = FALSE, count = 0, base;
  long bval;

  if (value == 0) {
    strcpy(str, "0");
    return;
  }

  if (value < 0) {
    neg = TRUE;
    value = -value;
  }

  while (value > 0) {

    /* obtain the nearest base */
    base = roman_base( value );

    /* place the next numeral */
    if ((bval = roman_value(base)) <= value) {
      str[count++] = rnumerals[base];
      value -= bval;
    } else {
      str[count++] = rnumerals[base - ((base + 1) % 2 + 1)];
      str[count++] = rnumerals[base];
      value -= (bval - roman_value(base - ((base + 1) % 2 + 1)));
    }

  }

  /* end the string */
  str[count] = '\0';
  if (neg == TRUE) {
    /* why not? */
    strcat( str, " BC");
  }

}

/* CHAR_IN_STR -- function to check if a character is in a character array */
int
char_in_str PARM_2( char, ch, char *, str)
{
  int i, l = strlen(str);
  
  for (i = 0; i < l; i++)
    if (ch == str[i]) return(TRUE);
  return(FALSE);
}

/* NON_CNTRL -- return the non-control value of a control character */
int
non_cntrl PARM_1(int, ch)
{
  int hold;

  if (!isprint(ch)) {
    if (ch != EXT_DEL) {
      hold = ch + '@';
    } else {
      hold = '?';
    }
  } else {
    hold = ch;
  }
  return(hold);
}

/* TO_CNTRL -- return the control value of a non-control character */
int
to_cntrl PARM_1(int, ch)
{
  int hold;

  if (ch == '?') {
    hold = EXT_DEL;
  } else {
    hold = ch - '@';
  }
  return(hold);
}

/* CONVERT_KBIND -- Convert the textual keybinding to an actual keybinding */
void
convert_kbind PARM_1(char *, str)
{
  int ch, out_cnt = 0, in_cnt = 0, num;

  /* traverse the string, and clean it up */
  do {
    if ((ch = str[in_cnt]) == '\\') {
      /* handle the normal C characters */
      switch (ch = str[++in_cnt]) {
      case 'n':
	/* newline */
	ch = '\n';
	break;
      case 'r':
	/* carriage return */
	ch = '\r';
	break;
      case 't':
	/* tab key */
	ch = '\t';
	break;
      case 'b':
	/* backspace */
	ch = '\b';
	break;
      case 'x':
	/* hexidecimal condition */
	ch = 0;
	num = 2;
	for (in_cnt++;
	     (num > 0) &&
	     (isalpha(str[in_cnt]) ||
	      isdigit(str[in_cnt]));
	     in_cnt++, num--) {
	  /* provide conversion */
	  if (isdigit(str[in_cnt])) {
	    ch = ch * 16 + (str[in_cnt] - '0');
	  } else {
	    ch = ch * 16 + (str[in_cnt] - 'a' + 10);
	  }
	}
	break;
      case '0':
	/* octal conversion */
	ch = 0;
	num = 3;
	do {
	  ch = ch * 8 + (str[in_cnt] - '0');
	} while (isdigit(str[in_cnt++]) && (--num > 0));

	/* don't skip the character */
	in_cnt--;
	break;
      default:
	/* direct quote */
	break;
      }
      str[out_cnt++] = ch;
    } else if (ch == '^') {
      /* control key */
      if (str[++in_cnt] != '\0') {
	str[out_cnt++] = to_cntrl(str[in_cnt]);
      }
    } else {
      str[out_cnt++] = ch;
    }
  } while (str[in_cnt++] != '\0');
}

/* COMPLETION -- Returns TRUE if second string is a possible
                 completion of the first.  Aka stncmp(a,b,strlen(a)) */
int
completion PARM_2 (char *, s1, char *, s2)
{
  int ch1, ch2, hold = TRUE;

  if (strlen(s1) > strlen(s2)) return(FALSE);
  for (; *s1 != '\0'; s1++, s2++) {
    ch1 = (islower(*s1) ? toupper(*s1) : *s1);
    ch2 = (islower(*s2) ? toupper(*s2) : *s2);
    if (ch1 != ch2) {
      hold = FALSE;
      break;
    }
  }

  return(hold);
}

/* NTN_REALNAME -- Find the proper name for a nation */
int
ntn_realname PARM_2(char *, outstr, int, who)
{
  switch (who) {
  case NEWSPAPER:
    /* sending to the newspaper */
    strcpy(outstr, newsfile);
    break;
  case UNOWNED:
    /* sending to the almighty one */
    strcpy(outstr, "god");
    break;
  default:
    /* a normal nation... */
    if (!is_update && (country == who)) {
      strcpy(outstr, nationname);
    } else if ((who < 0) || (who > ABSMAXNTN) ||
	       (ntn_tptr = world.np[who]) == NULL) {
      strcpy(outstr, "bad-name");
      errormsg("Error in ntn_realname(): Unknown nation asked for");
      return(TRUE);
    } else {
      strcpy(outstr, ntn_tptr->name);
    }
    break;
  }
  return(FALSE);
}

/* TRIM_STR -- remove blank space, and comments if so desired */
int
trim_str PARM_2(char *, str, int, killpound)
{
  register int i = 0, j = 0;
  int last_space = -1;

  /* trim leading spaces */
  while (isspace(str[i])) i++;
  if ((str[i] == '#') &&
      (killpound == TRUE)) {
    str[i] = '\0';
  }

  /* shift string */
  while (1) {
    if ((str[j] = str[i++]) == '\0') {
      break;
    } else if (isspace(str[j])) {
      last_space = j;
    } else {
      last_space = -1;
    }
    j++;
  }

  /* now remove any trailing spaces */
  if (last_space != -1) {
    str[last_space] = '\0';
    j = last_space;
  }
  return(j);
}

/* XLOC_RELATIVE -- return X value relative to centralize location */
int
xloc_relative PARM_1(int, given_xloc)
{
  int hold;

  if ((world.relative_map) &&
      (is_god == FALSE) &&
      (ntn_ptr != NULL)) {
    hold = given_xloc - ntn_ptr->centerx;
    if (hold > MAPX / 2) {
      hold -= MAPX;
    }
    if (hold <= - (MAPX / 2) ) {
      hold += MAPX;
    }
  } else {
    hold = given_xloc;
  }
  return(hold);
}

/* YLOC_RELATIVE -- return Y value relative to centralize location */
int
yloc_relative PARM_1(int, given_yloc)
{
  int hold;

  if ((world.relative_map) &&
      (is_god == FALSE) &&
      (ntn_ptr != NULL)) {
    hold = given_yloc - ntn_ptr->centery;
  } else {
    hold = given_yloc;
  }
  return(hold);
}

/* POPTOWORKERS -- Determine the number of workers based on population */
long
poptoworkers PARM_1(long, population)
{
  long workpop = 0L;
  int div_val = 1;

  /* check for maxing out */
  if (population > ABSMAXPEOPLE) {
    population = ABSMAXPEOPLE;
  }
  while (population > 0) {
    if (population > TOOMANYPEOPLE) {
      workpop += TOOMANYPEOPLE / div_val;
    } else {
      workpop += population / div_val;
    }
    population -= TOOMANYPEOPLE;
    div_val++;
  }
  return(workpop);
}
