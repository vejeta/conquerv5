// SPDX-License-Identifier: GPL-3.0-or-later
/* This file provides quick reference to data structures */
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
#include "armyX.h"

/* ARMYBYNUM -- Retrieve pointer to a given army number */
ARMY_PTR
armybynum PARM_1 (int, idnum)
{
  ARMY_PTR a1_ptr;

  for (a1_ptr = ntn_ptr->army_list; a1_ptr != NULL;
       a1_ptr = a1_ptr->next) {
    if (a1_ptr->armyid == idnum)
      break;
    if (a1_ptr->armyid > idnum)
      return ((ARMY_PTR) NULL);
  }
  return (a1_ptr);
}

/* NAVYBYNUM -- Retrieve pointer to a given navy number */
NAVY_PTR 
navybynum PARM_1 (int, idnum)
{
  NAVY_PTR n1_ptr;

  for (n1_ptr = ntn_ptr->navy_list;
       n1_ptr != NULL;
       n1_ptr = n1_ptr->next) {
    if (n1_ptr->navyid == idnum)
      break;
    if (n1_ptr->navyid > idnum)
      return((NAVY_PTR) NULL);
  }
  return (n1_ptr);
}

/* CVNBYNUM -- Retrieve pointer to a given caravan number */
CVN_PTR 
cvnbynum PARM_1 (int, idnum)
{
  CVN_PTR c1_ptr;

  for (c1_ptr = ntn_ptr->cvn_list; c1_ptr != NULL;
       c1_ptr = c1_ptr->next) {
    if (c1_ptr->cvnid == idnum)
      break;
    if (c1_ptr->cvnid > idnum)
      return((CVN_PTR) NULL);
  }
  return (c1_ptr);
}

/* ITEMBYNUM -- Retrieve a commodity by its given number */
ITEM_PTR
itembynum PARM_1 (int, idnum)
{
  ITEM_PTR i1_ptr;

  for (i1_ptr = ntn_ptr->item_list; i1_ptr != NULL;
       i1_ptr = i1_ptr->next) {

    if (i1_ptr->itemid == idnum)
      break;
    if (i1_ptr->itemid > idnum)
      return((ITEM_PTR) NULL);

  }
  return (i1_ptr);
}

/* NTNBYNAME -- Check for a nation by name or number */
NTN_PTR
ntnbyname PARM_1 (char *, str)
{
  NTN_PTR n1_ptr = NULL;

  /* is it god? */
  if (strcmp(str, "god") == 0) {
    global_int = UNOWNED;
    return(NULL);
  }

  /* is it the news */
  if (strcmp(str, "news") == 0) {
    global_int = NEWSPAPER;
    return(NULL);
  }

  /* compare with nation names */
  for (global_int = 0; global_int < MAXNTN; global_int++) {
    if ((n1_ptr = world.np[global_int]) != NULL)
      if (str_test(n1_ptr->name, str) == 0) break;
  }
  if (global_int == MAXNTN) n1_ptr = (NTN_PTR) NULL;

  return(n1_ptr);
}

/* CITYBYNAME -- Check for a city by name */
CITY_PTR
citybyname PARM_1 (char *, str)
{
  CITY_PTR c1_ptr;

  /*search through list of cities*/
  for (c1_ptr = ntn_ptr->city_list; c1_ptr != NULL;
       c1_ptr = c1_ptr->next) {
    if (str_test(c1_ptr->name, str) == 0) 
      break;
  }
  return (c1_ptr);
}

/* CITYBYLOC -- Check for a city by location */
CITY_PTR
citybyloc PARM_3 (NTN_PTR, n1_ptr, int, x, int, y)
{
  CITY_PTR c1_ptr;

  /* check input */
  if (n1_ptr == NULL)
    return((CITY_PTR) NULL);

  /* search through list of cities in the given nation */
  for (c1_ptr = n1_ptr->city_list;
       c1_ptr != NULL;
       c1_ptr = c1_ptr->next) {
    if ((c1_ptr->xloc == x) && (c1_ptr->yloc == y)) break;
  }

  return (c1_ptr);
}

/* UCLASSBYNAME -- Retrieve a unit class by the name string */
int
uclassbyname PARM_1(char *, match_str)
{
  int i;

  /* simple search routine */
  for (i = 0; i < num_aclasses; i++) {
    if (str_test(ainfo_clist[i], match_str) == 0) {
      return(i);
    }
  }
  return(-1);
}

/* UNITBYNAME -- Retrieve a unit number by the name string */
int
unitbyname PARM_1(char *, match_str)
{
  int i;

  /* simple search routine */
  for (i = 0; i < num_armytypes; i++) {
    if (str_test(ainfo_list[i].name, match_str) == 0) {
      return(i);
    }
  }
  return(-1);
}

/* ITEMBYSCT -- Find the next item pointer to a sector */
ITEM_PTR
itembysct PARM_4(ITEM_PTR, i_list, int, x, int, y, int, type)
{
  ITEM_PTR i1_ptr;

  /* search */
  for (i1_ptr = i_list;
       i1_ptr != NULL;
       i1_ptr = i1_ptr->next) {
    if ((i1_ptr->xloc == x) &&
	(i1_ptr->yloc == y)) {
      if ((type == -1) ||
	  (type == i1_ptr->type)) break;
    }
  }
  return(i1_ptr);
}

/* ITEMBYARMY -- Search for the army number in the item list */
ITEM_PTR
itembyarmy PARM_1(int, idnum)
{
  ITEM_PTR i1_ptr;

  for (i1_ptr = ntn_ptr->item_list; i1_ptr != NULL;
       i1_ptr = i1_ptr->next) {
    if (i1_ptr->armyid == idnum)
      break;
  }
  return (i1_ptr);
}

/* ITEMBYNAVY -- Search for the navy number in the item list */
ITEM_PTR
itembynavy PARM_1(int, idnum)
{
  ITEM_PTR i1_ptr;

  for (i1_ptr = ntn_ptr->item_list; i1_ptr != NULL;
       i1_ptr = i1_ptr->next) {
    if (i1_ptr->navyid == idnum)
      break;
  }
  return (i1_ptr);
}

/* ITEMBYCVN -- Search for the caravan number in the item list */
ITEM_PTR
itembycvn PARM_1(int, idnum)
{
  ITEM_PTR i1_ptr;

  for (i1_ptr = ntn_ptr->item_list; i1_ptr != NULL;
       i1_ptr = i1_ptr->next) {
    if (i1_ptr->cvnid == idnum)
      break;
  }
  return (i1_ptr);
}
