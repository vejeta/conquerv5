// SPDX-License-Identifier: GPL-3.0-or-later
/* This file handles mallocing, sorting and other memory dohickys */
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
#include "cityX.h"
#include "itemX.h"
#include "navyX.h"
#include "butesX.h"
#include "desigX.h"
#include "racesX.h"
#include "activeX.h"
#include "statusX.h"
#include "caravanX.h"
#include "displayX.h"
#include "dstatusX.h"

/* CLR_MEMORY -- Clear out a patch of memory */
void
clr_memory PARM_2(char *, mem_ptr, int, len)
{
  /* clean out len bytes of data */
#ifdef BZERO
  bzero( mem_ptr, len);
#else
  memset( mem_ptr, 0, len );
#endif /* BZERO */
}

/* ALIGN_DATA -- Destroy nations and count elements */
void
align_data PARM_0(void)
{
  UNUM_PTR unum_ptr;
  MAP_PTR map_ptr;

  /* go through all nations keeping track of usages */
  MAXNTN = 1;
  world.active_ntns = 0;
  fprintf(fupdate, "aligning data...");
  for (country = 0; country < ABSMAXNTN; country++)
    if ((ntn_ptr = world.np[country]) != NULL) {

      /* check if the nation needs removal */
      if (ntn_ptr->active == INACTIVE) {
	/* nuke those useless nations */
	dest_ntn(ntn_ptr->name);
	continue;
      }

      /* count the nation */
      MAXNTN++;
      if (n_isactive(ntn_ptr->active)) {
	world.active_ntns++;
      }

      /* count army elements */
      ntn_ptr->num_army = 0;
      for (army_ptr = ntn_ptr->army_list;
	   army_ptr != NULL;
	   army_ptr = army_ptr->next) {
	ntn_ptr->num_army++;
      }

      /* count navy elements */
      ntn_ptr->num_navy = 0;
      for (navy_ptr = ntn_ptr->navy_list;
	   navy_ptr != NULL;
	   navy_ptr = navy_ptr->next) {
	ntn_ptr->num_navy++;
      }

      /* count caravan elements */
      ntn_ptr->num_cvn = 0;
      for (cvn_ptr = ntn_ptr->cvn_list;
	   cvn_ptr != NULL;
	   cvn_ptr = cvn_ptr->next) {
	ntn_ptr->num_cvn++;
      }

      /* count city elements */
      ntn_ptr->num_city = 0;
      for (city_ptr = ntn_ptr->city_list;
	   city_ptr != NULL;
	   city_ptr = city_ptr->next) {
	ntn_ptr->num_city++;
      }

      /* count item elements */
      ntn_ptr->num_item = 0;
      for (item_ptr = ntn_ptr->item_list;
	   item_ptr != NULL;
	   item_ptr = item_ptr->next) {
	ntn_ptr->num_item++;
      }

      /* count unum elements */
      ntn_ptr->num_unum = 0;
      for (unum_ptr = ntn_ptr->unum_list;
	   unum_ptr != NULL;
	   unum_ptr = unum_ptr->next) {
	ntn_ptr->num_unum++;
      }

      /* count map elements */
      ntn_ptr->num_maps = 0;
      for (map_ptr = ntn_ptr->map_list;
	   map_ptr != NULL;
	   map_ptr = map_ptr->next) {
	ntn_ptr->num_maps++;
      }

      /* now redefine the range for the nation */
      if (n_isntn(ntn_ptr->active)) {
	find_area(country);
      }

    }

  /* count the default unum elements */
  world.num_unum = 0;
  for (unum_ptr = world.dflt_unum;
       unum_ptr != NULL;
       unum_ptr = unum_ptr->next) {
    world.num_unum++;
  }

  /* finish the process; by resorting at end of update */
  fprintf(fupdate, "done\n");
  if (is_update == TRUE) ntn_sort();
}

/* M2ALLOC -- Allocate a two dimensional array of memory
              nrows: number of rows, ncols: number of columns
              entrysize: number of bytes in a unit            */
char **
m2alloc PARM_3 (int, nrows, int, ncols, int, entrysize)
{
  char **baseaddr;
  int j;

  entrysize *= ncols;
  baseaddr = (char **) malloc((unsigned)
			      (nrows * (sizeof(char *) + entrysize)));

  if (baseaddr == (char **) NULL) {
    sprintf(string, "ERROR -- can't allocate %d by %d blocks of %d bytes",
	    nrows, ncols, entrysize);
    errormsg(string);
    abrt();
  }
  if(nrows > 0){
    *baseaddr = (char *) (baseaddr + nrows);
    for(j = 1; j < nrows; j++)
      baseaddr[j] = baseaddr[j - 1] + entrysize;
  }
  return(baseaddr);
}

/* ARMY_SORT() -- Resort army list into ascending order and connect
                  armies in same sector by the near structure       */
void
army_sort PARM_1(int, skipnears)
{
  ARMY_PTR a1_ptr, a2_ptr;
  int done = FALSE;

  /* check for no need of ordering */
  if ((ntn_ptr->army_list == NULL) ||
      ((ntn_ptr->army_list)->next == NULL)) {
    done = TRUE;
  }

  /* perform sort */
  while (!done) {

    /* test for initial element swap */
    a1_ptr = ntn_ptr->army_list;
    if (a1_ptr->armyid > (a1_ptr->next)->armyid) {
      ntn_ptr->army_list = a1_ptr->next;
      a1_ptr->next = (ntn_ptr->army_list)->next;
      (ntn_ptr->army_list)->next = a1_ptr;
      continue;
    }

    /* initialization */
    done = TRUE;

    /* sort remaining linked list */
    for (; (a1_ptr->next)->next != NULL; a1_ptr = a1_ptr->next) {
      a2_ptr = (a1_ptr->next)->next;
      if ((a1_ptr->next)->armyid > a2_ptr->armyid) {
	/* swap locations in list */
	(a1_ptr->next)->next = a2_ptr->next;
	a2_ptr->next = a1_ptr->next;
	a1_ptr->next = a2_ptr;
	/* continue sort */
	done = FALSE;
      }
    }
  }

  /* now connect all near pointers properly */
  if (!skipnears) {
    align_armynear();
  }
}

/* NAVY_SORT() -- Resort navy list into ascending order */
void
navy_sort PARM_0(void)
{
  NAVY_PTR n1_ptr, n2_ptr;
  int done = FALSE;

  /* check for no need of ordering */
  if (ntn_ptr->navy_list == NULL || (ntn_ptr->navy_list)->next == NULL) {
    done = TRUE;
  }

  /* perform sort */
  while (!done) {
    /* initialization */
    n1_ptr = ntn_ptr->navy_list;
    done = TRUE;

    /* test for initial element swap */
    if (n1_ptr->navyid > (n1_ptr->next)->navyid) {
      ntn_ptr->navy_list = n1_ptr->next;
      n1_ptr->next = (ntn_ptr->navy_list)->next;
      (ntn_ptr->navy_list)->next = n1_ptr;
      n1_ptr = ntn_ptr->navy_list;
    }

    /* sort remaining linked list */
    for (; (n1_ptr->next)->next != NULL; n1_ptr = n1_ptr->next) {
      n2_ptr = (n1_ptr->next)->next;
      if ((n1_ptr->next)->navyid > n2_ptr->navyid) {
	/* swap locations in list */
	(n1_ptr->next)->next = n2_ptr->next;
	n2_ptr->next = n1_ptr->next;
	n1_ptr->next = n2_ptr;
	/* continue sort */
	done = FALSE;
      }
    }
  }
}

/* CITY_SORT() -- Resort city list into ascending order */
void
city_sort PARM_0(void)
{
  CITY_PTR c1_ptr, c2_ptr;
  int done = FALSE;

  /* check for no need of ordering */
  if (ntn_ptr->city_list == NULL || (ntn_ptr->city_list)->next == NULL) {
    done = TRUE;
  }

  /* perform sort */
  while (!done) {
    /* initialization */
    c1_ptr = ntn_ptr->city_list;
    done = TRUE;

    /* test for initial element swap */
    if ( str_test( c1_ptr->name, (c1_ptr->next)->name ) > 0) {
      ntn_ptr->city_list = c1_ptr->next;
      c1_ptr->next = (ntn_ptr->city_list)->next;
      (ntn_ptr->city_list)->next = c1_ptr;
      c1_ptr = ntn_ptr->city_list;
    }

    /* sort remaining linked list */
    for (; (c1_ptr->next)->next != NULL; c1_ptr = c1_ptr->next) {
      c2_ptr = (c1_ptr->next)->next;
      if ( str_test( (c1_ptr->next)->name, c2_ptr->name ) > 0) {
	/* swap locations in list */
	(c1_ptr->next)->next = c2_ptr->next;
	c2_ptr->next = c1_ptr->next;
	c1_ptr->next = c2_ptr;
	/* continue sort */
	done = FALSE;
      }
    }
  }
}

/* CVN_SORT() -- Resort caravan list into ascending order */
void
cvn_sort PARM_0(void)
{
  CVN_PTR c1_ptr, c2_ptr;
  int done = FALSE;

  /* check if ordering is not needed */
  if (ntn_ptr->cvn_list == NULL || (ntn_ptr->cvn_list)->next == NULL) {
    done = TRUE;
  }

  /* perform sort */
  while (!done) {
    /* initialization */
    c1_ptr = ntn_ptr->cvn_list;
    done = TRUE;

    /* test for initial element swap */
    if (c1_ptr->cvnid > (c1_ptr->next)->cvnid) {
      ntn_ptr->cvn_list = c1_ptr->next;
      c1_ptr->next = (ntn_ptr->cvn_list)->next;
      (ntn_ptr->cvn_list)->next = c1_ptr;
      c1_ptr = ntn_ptr->cvn_list;
    }

    /* sort remaining linked list */
    for (; (c1_ptr->next)->next != NULL; c1_ptr = c1_ptr->next) {
      c2_ptr = (c1_ptr->next)->next;
      if ((c1_ptr->next)->cvnid > c2_ptr->cvnid) {
	/* swap locations in list */
	(c1_ptr->next)->next = c2_ptr->next;
	c2_ptr->next = c1_ptr->next;
	c1_ptr->next = c2_ptr;
	/* continue sort */
	done = FALSE;
      }
    }
  }
}

/* ITEM_SORT() -- Resort item list into ascending order */
void
item_sort PARM_0(void)
{
  ITEM_PTR i1_ptr, i2_ptr;
  int done = FALSE;

  /* check for no need of ordering */
  if (ntn_ptr->item_list == NULL || (ntn_ptr->item_list)->next == NULL) {
    done=TRUE;
  }

  /* perform sort */
  while (!done) {
    /* initialization */
    i1_ptr = ntn_ptr->item_list;
    done = TRUE;

    /* test for initial element swap */
    if (i1_ptr->itemid > (i1_ptr->next)->itemid) {
      ntn_ptr->item_list = i1_ptr->next;
      i1_ptr->next = (ntn_ptr->item_list)->next;
      (ntn_ptr->item_list)->next = i1_ptr;
      i1_ptr = ntn_ptr->item_list;
    }

    /* sort remaining linked list */
    for (; (i1_ptr->next)->next != NULL; i1_ptr = i1_ptr->next) {
      i2_ptr = (i1_ptr->next)->next;
      if ((i1_ptr->next)->itemid > i2_ptr->itemid) {
	/* swap locations in list */
	(i1_ptr->next)->next = i2_ptr->next;
	i2_ptr->next = i1_ptr->next;
	i1_ptr->next = i2_ptr;
	/* continue sort */
	done = FALSE;
      }
    }
  }
}

/* ALIGN_ARMYNEAR -- Assign location pointers properly */
void
align_armynear PARM_0(void)
{
  ARMY_PTR a1_ptr, a2_ptr;

  /* go through the list one by one */
  for (a1_ptr = ntn_ptr->army_list;
       a1_ptr != NULL; a1_ptr = a1_ptr->next) {

    /* find nearest unit */
    for (a2_ptr = a1_ptr->next;
	 a2_ptr != NULL;
	 a2_ptr = a2_ptr->next) {
      if ((a2_ptr->xloc == a1_ptr->xloc) &&
	  (a2_ptr->yloc == a1_ptr->yloc)) break;
    }
    a1_ptr->nrby = a2_ptr;

  }

}

/* NEW_MAPCHAR -- Allocate character memory the size of the world */
char *
new_mapchar PARM_1(char *, mem_ptr)
{
  /* check it */
  if (mem_ptr == NULL) {
    if ((mem_ptr = (char *) malloc(sizeof(char) * (MAPX * MAPY))) == NULL) {
      errormsg("MAJOR ERROR: Memory allocation error");
      abrt();
    }
  }

  /* zeroify it */
  clr_memory( mem_ptr, MAPX * MAPY * sizeof(char));
  return(mem_ptr);
}

/* NEW_MAPSHORT -- Allocate short integer memory the size of the world */
short *
new_mapshort PARM_1(short *, mem_ptr)
{
  /* check it */
  if (mem_ptr == NULL) {
    if ((mem_ptr = (short *) malloc(sizeof(short) *
				    (MAPX * MAPY))) == NULL) {
      errormsg("MAJOR ERROR: Memory allocation error");
      abrt();
    }
  }

  /* zeroify it */
  clr_memory( (char *) mem_ptr, MAPX * MAPY * sizeof(short) );
  return(mem_ptr);
}

/* NEW_MAPLONG -- Allocate long integer memory the size of the world */
long *
new_maplong PARM_1(long *, mem_ptr)
{
  /* check it */
  if (mem_ptr == NULL) {
    if ((mem_ptr = (long *) malloc(sizeof(long) * (MAPX * MAPY))) == NULL) {
      errormsg("MAJOR ERROR: Memory allocation error");
      abrt();
    }
  }

  /* zeroify it */
  clr_memory( (char *) mem_ptr, MAPX * MAPY * sizeof(long) );
  return(mem_ptr);
}

/* NEW_DMODE -- Allocate space for a new display entry */
DMODE_PTR
new_dmode PARM_0(void)
{
  DMODE_PTR d1_ptr;

  /* create the space */
  d1_ptr = (DMODE_PTR) malloc(sizeof(DMODE_STRUCT));

  /* verify memory */
  if (d1_ptr == NULL) {
    errormsg("Major error: could not allocate memory for display");
    abrt();
  }
  return(d1_ptr);
}

/* NEW_UNUM -- Allocate space for a new unit numbering structure */
UNUM_PTR
new_unum PARM_0(void)
{
  UNUM_PTR u1_ptr;

  /* create the space */
  u1_ptr = (UNUM_PTR) malloc(sizeof(UNITNUM));

  /* verify memory */
  if (u1_ptr == NULL) {
    errormsg("Major error: could not allocate memory for a unum");
    abrt();
  }
  return(u1_ptr);
}

/* NEW_MAP -- Allocate space for a new unit mapping structure */
MAP_PTR
new_map PARM_0(void)
{
  MAP_PTR m1_ptr;

  /* create the space */
  m1_ptr = (MAP_PTR) malloc(sizeof(MAP_STRUCT));

  /* verify memory */
  if (m1_ptr == NULL) {
    errormsg("Major error: could not allocate memory for a map struct");
    abrt();
  }
  return(m1_ptr);
}

/* NEW_ARMY -- Allocate space for a new army unit */
ARMY_PTR
new_army PARM_0(void)
{
  ARMY_PTR a1_ptr;

  /* create the space */
  a1_ptr = (ARMY_PTR) malloc(sizeof(ARMY_STRUCT));

  /* verify memory */
  if (a1_ptr == NULL) {
    errormsg("Major error: could not allocate memory for army");
    abrt();
  }
  return(a1_ptr);
}

/* NEW_NAVY -- Allocate space for a new naval unit */
NAVY_PTR
new_navy PARM_0(void)
{
  NAVY_PTR n1_ptr;

  /* create the space */
  n1_ptr = (NAVY_PTR) malloc(sizeof(NAVY_STRUCT));

  /* verify memory */
  if (n1_ptr == NULL) {
    errormsg("Major error: could not allocate memory for navy");
    abrt();
  }
  return(n1_ptr);
}

/* NEW_CVN -- Allocate space for a new cvn unit */
CVN_PTR
new_cvn PARM_0(void)
{
  CVN_PTR c1_ptr;

  /* create the space */
  c1_ptr = (CVN_PTR) malloc(sizeof(CVN_STRUCT));

  /* verify memory */
  if (c1_ptr == NULL) {
    errormsg("Major error: could not allocate memory for cvn");
    abrt();
  }
  return(c1_ptr);
}

/* NEW_CITY -- Allocate space for a new city */
CITY_PTR
new_city PARM_0(void)
{
  CITY_PTR c1_ptr;

  /* create the space */
  c1_ptr = (CITY_PTR) malloc(sizeof(CITY_STRUCT));

  /* verify memory */
  if (c1_ptr == NULL) {
    errormsg("Major error: could not allocate memory for city");
    abrt();
  }
  return(c1_ptr);
}

/* NEW_ITEM -- Allocate space for a new item */
ITEM_PTR
new_item PARM_0(void)
{
  ITEM_PTR i1_ptr;

  /* create the space */
  i1_ptr = (ITEM_PTR) malloc(sizeof(ITEM_STRUCT));

  /* verify memory */
  if (i1_ptr == NULL) {
    errormsg("Major error: could not allocate memory for item");
    abrt();
  }
  return(i1_ptr);
}

/* NEW_NTN -- Allocate space for a new nation */
NTN_PTR
new_ntn PARM_0(void)
{
  NTN_PTR n1_ptr;

  /* create the space */
  n1_ptr = (NTN_PTR) malloc(sizeof(NTN_STRUCT));

  /* verify memory */
  if (n1_ptr == NULL) {
    errormsg("Major error: could not allocate memory for nation");
    abrt();
  }
  return(n1_ptr);
}

/* DEST_ARMY -- Remove an army unit from the army list */
void
dest_army PARM_1(int, idnum)
{
  ARMY_PTR a1_ptr, a2_ptr;

  /* stupidity check */
  if (ntn_ptr == NULL) return;

  /* start the predecessor pointer */
  a2_ptr = (ARMY_PTR) NULL;

  /* go through and find the unit */
  for (a1_ptr = ntn_ptr->army_list;
       a1_ptr != NULL;
       a1_ptr = a1_ptr->next) {
    /* check for it */
    if (a1_ptr->armyid == idnum) break;
    a2_ptr = a1_ptr;
  }

  /* get rid of it */
  if (a1_ptr != NULL) {

    /* check for the head of the list */
    if (a2_ptr == NULL) {
      ntn_ptr->army_list = a1_ptr->next;
    } else {
      a2_ptr->next = a1_ptr->next;
    }

    /* check if the unit was a leader and fix any followers */
    if (unit_leading(a1_ptr->status) ||
	a_isleader(a1_ptr->unittype)) {
      for (a2_ptr = ntn_ptr->army_list;
	   a2_ptr != NULL;
	   a2_ptr = a2_ptr->next) {
	/* clean up them doggies */
	if (a2_ptr->leader == a1_ptr->armyid) {
	  a2_ptr->leader = a1_ptr->leader;
	  if (unit_status(a2_ptr->status) == ST_GROUPED) {
	    set_status(a2_ptr->status, unit_status(a1_ptr->status));
	  }
	}
      }
    }

    /* now make the space available */
    a1_ptr->next = (ARMY_PTR) NULL;
    a1_ptr->nrby = (ARMY_PTR) NULL;
    free(a1_ptr);

    /* fix the list */
    army_sort(FALSE);
  }
}

/* DEST_NAVY -- Remove a navy unit from the navy list */
void
dest_navy PARM_1(int, idnum)
{
  NAVY_PTR n1_ptr, n2_ptr;

  /* stupidity check */
  if (ntn_ptr == NULL) return;

  /* start the predecessor pointer */
  n2_ptr = (NAVY_PTR) NULL;

  /* go through and find the unit */
  for (n1_ptr = ntn_ptr->navy_list;
       n1_ptr != NULL;
       n1_ptr = n1_ptr->next) {
    /* check for it */
    if (n1_ptr->navyid == idnum) break;
    n2_ptr = n1_ptr;
  }

  /* get rid of it */
  if (n1_ptr != NULL) {

    /* check for the head of the list */
    if (n2_ptr == NULL) {
      ntn_ptr->navy_list = n1_ptr->next;
    } else {
      n2_ptr->next = n1_ptr->next;
    }

    /* now make the space available */
    n1_ptr->next = (NAVY_PTR) NULL;
    free(n1_ptr);

    /* the list should be in order */
  }
}

/* DEST_CVN -- Remove a caravan from the caravan list */
void
dest_cvn PARM_1(int, idnum)
{
  CVN_PTR c1_ptr, c2_ptr;

  /* stupidity check */
  if (ntn_ptr == NULL) return;

  /* start the predecessor pointer */
  c2_ptr = (CVN_PTR) NULL;

  /* go through and find the unit */
  for (c1_ptr = ntn_ptr->cvn_list;
       c1_ptr != NULL;
       c1_ptr = c1_ptr->next) {
    /* check for it */
    if (c1_ptr->cvnid == idnum) break;
    c2_ptr = c1_ptr;
  }

  /* get rid of it */
  if (c1_ptr != NULL) {

    /* check for the head of the list */
    if (c2_ptr == NULL) {
      ntn_ptr->cvn_list = c1_ptr->next;
    } else {
      c2_ptr->next = c1_ptr->next;
    }

    /* now make the space available */
    c1_ptr->next = (CVN_PTR) NULL;
    free(c1_ptr);

    /* the list should be in order */
  }
}

/* DEST_CITY -- Remove a city from the national city list */
void
dest_city PARM_1(char *, cname)
{
  CITY_PTR c1_ptr, c2_ptr;

  /* stupidity check */
  if (ntn_ptr == NULL) return;

  /* start the predecessor pointer */
  c2_ptr = (CITY_PTR) NULL;

  /* go through and find the unit */
  for (c1_ptr = ntn_ptr->city_list;
       c1_ptr != NULL;
       c1_ptr = c1_ptr->next) {
    /* check for it */
    if (str_test(c1_ptr->name, cname) == 0) break;
    c2_ptr = c1_ptr;
  }

  /* get rid of it */
  if (c1_ptr != NULL) {

    /* check for the head of the list */
    if (c2_ptr == NULL) {
      ntn_ptr->city_list = c1_ptr->next;
    } else {
      c2_ptr->next = c1_ptr->next;
    }

    /* now make the space available */
    c1_ptr->next = (CITY_PTR) NULL;
    free(c1_ptr);

    /* the list should be in order */
  }
}

/* DEST_ITEM -- Remove an item from the national commodity list */
void
dest_item PARM_1(int, idnum)
{
  ITEM_PTR i1_ptr, i2_ptr;

  /* stupidity check */
  if (ntn_ptr == NULL) return;

  /* start the predecessor pointer */
  i2_ptr = (ITEM_PTR) NULL;

  /* go through and find the unit */
  for (i1_ptr = ntn_ptr->item_list;
       i1_ptr != NULL;
       i1_ptr = i1_ptr->next) {
    /* check for it */
    if (i1_ptr->itemid == idnum) break;
    i2_ptr = i1_ptr;
  }

  /* get rid of it */
  if (i1_ptr != NULL) {

    /* check for the head of the list */
    if (i2_ptr == NULL) {
      ntn_ptr->item_list = i1_ptr->next;
    } else {
      i2_ptr->next = i1_ptr->next;
    }

    /* now make the space available */
    i1_ptr->next = (ITEM_PTR) NULL;
    free(i1_ptr);

    /* the list should be in order */
  }
}

/* CRT_DMODE -- Initialize a new display mode into display mode list */
DMODE_PTR
crt_dmode PARM_1(char *, dmodename)
{
  DMODE_PTR d1_ptr;
  int count;

  /* go through the list */
  for (dmode_tptr = dmode_list;
       dmode_tptr != NULL;
       dmode_tptr = dmode_tptr->next) {

    /* if there is a match, just return it */
    if (strcmp(dmode_tptr->d.name, dmodename) == 0) {
      return(dmode_tptr);
    }

    /* exit when pointing to the last item */
    if (dmode_tptr->next == NULL) break;
  }

  /* now create the new display mode */
  d1_ptr = new_dmode();
  if (dmode_list == NULL) {
    dmode_list = d1_ptr;
  } else {
    dmode_tptr->next = d1_ptr;
  }

  /* now initialize things */
  strcpy(d1_ptr->d.name, dmodename);
  d1_ptr->next = NULL;
  d1_ptr->d.focus = HXPOS_LOWLEFT;
  for (count = 0; count < HXPOS_NUMBER; count++) {
    d1_ptr->d.highlight[count] = HI_KEEP;
    d1_ptr->d.style[count] = DI_KEEP;
    d1_ptr->d.target[count] = 0;
  }
  return(d1_ptr);
}

/* CRT_NTN -- Initialize a new nation into the world */
NTN_PTR
crt_ntn PARM_2(char *, ntnname, int, actval)
{
  int count;
  NTN_PTR n1_ptr, n2_ptr;

  /* make sure it is okay to build a new nation */
  for (count = 1; count < ABSMAXNTN; count++) {
    if ((n1_ptr = world.np[count]) == NULL) break;
  }
  if (count == ABSMAXNTN) return((NTN_PTR) NULL);
  global_int = count;

  /* now create the new nation */
  n1_ptr = new_ntn();
  world.np[global_int] = n1_ptr;

  /* assign some default values */
  clr_memory((char *) n1_ptr, sizeof(NTN_STRUCT));
  strcpy(n1_ptr->name, ntnname);
  strcpy(n1_ptr->login, loginname);
  strcpy(n1_ptr->passwd, world.passwd);
  strcpy(n1_ptr->leader, "");
  n1_ptr->race = TUNKNOWN;
  n1_ptr->mark = '-';
  n1_ptr->capx = MAPX / 2;
  n1_ptr->capy = MAPY / 2;
  n1_ptr->rightedge = MAPX - 1;
  n1_ptr->bottomedge = MAPY - 1;
  n1_ptr->active = actval;
  n1_ptr->army_list = (ARMY_PTR) NULL;
  n1_ptr->navy_list = (NAVY_PTR) NULL;
  n1_ptr->city_list = (CITY_PTR) NULL;
  n1_ptr->item_list = (ITEM_PTR) NULL;
  n1_ptr->cvn_list = (CVN_PTR) NULL;
  for (count = 0; count < BUTE_NUMBER; count++) {
    n1_ptr->attribute[count] = bute_info[count].start_val;
  }

  /* now reset the diplomacy vectors */
  n2_ptr = NULL;
  n1_ptr->dstatus[UNOWNED] = DIP_NEUTRAL;
  for (count = 1; count < ABSMAXNTN; count++) {
    if (((n2_ptr = world.np[count]) != NULL) &&
	(count != global_int) &&
	(n_ismonster(n1_ptr->active) ||
	 n_ismonster(n2_ptr->active))) {
      n1_ptr->dstatus[count] = DIP_WAR;
      n2_ptr->dstatus[global_int] = DIP_WAR;
    } else {
      n1_ptr->dstatus[count] = DIP_UNMET;
      if (n2_ptr != NULL) 
	n2_ptr->dstatus[global_int] = DIP_UNMET;
    }
  }

  /* done... nation will be resorted during the writing */
  return(n1_ptr);
}

/* CRT_ARMY -- Build an army unit assigning an id number to it
                  based on the type of the new unit.              */
ARMY_PTR
crt_army PARM_1(int, utype)
{
  ARMY_PTR a1_ptr;
  int newunum;

  /* make sure it is okay to find a new unit */
  if (ntn_ptr == NULL) return((ARMY_PTR) NULL);

  /* find location to begin count from */
  newunum = find_newarmynum(utype);

  /* verify that a new unit is possible */
  if (newunum >= MAX_IDTYPE) {
#ifdef DEBUG
    sprintf(string, "Ran out of army numbers %d >= %ld",
	    newunum, (long) MAX_IDTYPE);
    bottommsg(string);
    sleep(1);
#endif /*DEBUG*/
    return( (ARMY_PTR) NULL );
  }

  /* now create the new unit */
  a1_ptr = new_army();

  /* assign some default values */
  clr_memory((char *) a1_ptr, sizeof(ARMY_STRUCT));
  a1_ptr->armyid = newunum;
  a1_ptr->unittype = utype;
  a1_ptr->status = ST_DEFEND;
  set_speed(a1_ptr->status, SPD_NORMAL);
  a1_ptr->efficiency = 100;
  a1_ptr->max_eff = 100;
  a1_ptr->leader = EMPTY_HOLD;
  a1_ptr->nrby = NULL;

  /* now reorganize the army list */
  a1_ptr->next = ntn_ptr->army_list;
  ntn_ptr->army_list = a1_ptr;
  army_sort(FALSE);

  /* return the location of the new unit */
  return(a1_ptr);
}

/* CRT_NAVY -- Add a new navy to the nation list */
NAVY_PTR
crt_navy PARM_0(void)
{
  NAVY_PTR n1_ptr;
  int i, startnum = EMPTY_HOLD + 1;

  /* check nation */
  if (ntn_ptr == NULL) return( (NAVY_PTR) NULL );

  /* find the lowest available id */
  for (n1_ptr = ntn_ptr->navy_list;
       n1_ptr != NULL;
       n1_ptr = n1_ptr->next) {
    /* increment if a unit already has that number */
    if (startnum == n1_ptr->navyid) startnum++;
    else if (startnum < n1_ptr->navyid) break;
  }

  /* check limit */
  if (startnum >= MAX_IDTYPE) return( (NAVY_PTR) NULL);

  /* create the unit */
  n1_ptr = new_navy();
  clr_memory((char *) n1_ptr, sizeof(NAVY_STRUCT));
  n1_ptr->navyid = startnum;
  n1_ptr->status = ST_CARRY;
  n1_ptr->armynum = EMPTY_HOLD;
  n1_ptr->cvnnum = EMPTY_HOLD;
  for (i = 0; i < NSHP_NUMBER; i++) {
    n1_ptr->ships[i] = 0;
    n1_ptr->efficiency[i] = 100;
  }

  /* reorganize nation fleet */
  n1_ptr->next = ntn_ptr->navy_list;
  ntn_ptr->navy_list = n1_ptr;
  navy_sort();

  /* return the value */
  return (n1_ptr);
}

/* CRT_CVN -- Add a new caravan to the nation list */
CVN_PTR
crt_cvn PARM_0(void)
{
  int startnum = 1;

  /* check nation */
  if (ntn_ptr == NULL) return( (CVN_PTR) NULL );

  /* find the lowest available id */
  for (cvn_tptr = ntn_ptr->cvn_list;
       cvn_tptr != NULL;
       cvn_tptr = cvn_tptr->next) {
    /* increment if a unit already has that number */
    if (startnum == CVNT_ID) startnum++;
    else if (startnum < CVNT_ID) break;
  }

  /* check limit */
  if (startnum >= MAX_IDTYPE) return( (CVN_PTR) NULL);

  /* create the unit */
  cvn_tptr = new_cvn();
  clr_memory((char *) cvn_tptr, sizeof(CVN_STRUCT));
  CVNT_ID = startnum;
  CVNT_STAT = ST_CARRY;
  CVNT_EFF = 100;

  /* reorganize nation fleet */
  cvn_tptr->next = ntn_ptr->cvn_list;
  ntn_ptr->cvn_list = cvn_tptr;
  cvn_sort();

  /* return the value */
  return (cvn_tptr);
}

/* CRT_ITEM -- Add a new item to the nation list */
ITEM_PTR
crt_item PARM_0(void)
{
  ITEM_PTR i1_ptr;
  int startnum = 1;

  /* check nation */
  if (ntn_ptr == NULL) return( (ITEM_PTR) NULL );

  /* find the lowest available id */
  for (i1_ptr = ntn_ptr->item_list;
       i1_ptr != NULL;
       i1_ptr = i1_ptr->next) {
    /* increment if a unit already has that number */
    if (startnum == i1_ptr->itemid) startnum++;
    else if (startnum < i1_ptr->itemid) break;
  }

  /* check limit */
  if (startnum >= MAX_IDTYPE) return( (ITEM_PTR) NULL);

  /* create the unit */
  i1_ptr = new_item();
  clr_memory((char *) i1_ptr, sizeof(ITEM_STRUCT));
  i1_ptr->itemid = startnum;

  /* reorganize nation fleet */
  i1_ptr->next = ntn_ptr->item_list;
  ntn_ptr->item_list = i1_ptr;
  item_sort();

  /* return the value */
  return (i1_ptr);
}

/* CRT_CITY -- Add a new city to the nation city list */
CITY_PTR
crt_city PARM_1(char *, cname)
{
  CITY_PTR c1_ptr;
  int count, num_val = 0, new_num = FALSE;

  /* check nation */
  if (ntn_ptr == NULL) return( (CITY_PTR) NULL );

  /* check for uniqueness */
  if (citybyname(cname) != NULL) {
    return( (CITY_PTR) NULL);
  }

  /* find a unique number for starting */
  do {
    CITY_PTR ctmp_ptr;
    new_num = TRUE;
    num_val++;
    for (ctmp_ptr = ntn_ptr->city_list;
	 ctmp_ptr != NULL;
	 ctmp_ptr = ctmp_ptr->next) {
      if (num_val == ctmp_ptr->cityid) {
	new_num = FALSE;
	break;
      }
    }
  } while (new_num == FALSE);

  /* assign default values */
  c1_ptr = new_city();
  strcpy(c1_ptr->name, cname);
  c1_ptr->xloc = 0;
  c1_ptr->yloc = 0;
  c1_ptr->cityid = num_val;
  c1_ptr->i_people = 0;
  c1_ptr->weight = 0;
  c1_ptr->fortress = 0;
  c1_ptr->s_talons = (itemtype) 0;
  c1_ptr->cmd_flag = 0L;
  for (count = 0; count < MTRLS_NUMBER; count++) {
    c1_ptr->c_mtrls[count] = (itemtype) 0;
    c1_ptr->i_mtrls[count] = (itemtype) 0;
    c1_ptr->m_mtrls[count] = (itemtype) 0;
    c1_ptr->auto_flags[count] = 0L;
  }

  /* reorganize national townships */
  c1_ptr->next = ntn_ptr->city_list;
  ntn_ptr->city_list = c1_ptr;
  city_sort();

  /* return the value */
  return (c1_ptr);
}

/* DEST_NTN -- This routines frees up the memory associated with a
                  nation structure.                                   */
void
dest_ntn PARM_1 (char *, nname)
{
  ARMY_PTR a1_ptr, a2_ptr;
  NAVY_PTR n1_ptr, n2_ptr;
  CITY_PTR c1_ptr, c2_ptr;
  CVN_PTR v1_ptr, v2_ptr;
  ITEM_PTR i1_ptr, i2_ptr;
  int cntry;

  /* this routine need only free the memory, */
  /* not reallign diplomacy vectors.         */
  for (cntry = 1; cntry < ABSMAXNTN; cntry++) {
    if ((ntn_tptr = world.np[cntry]) != NULL) {
      if (strcmp(nname, ntn_tptr->name) == 0) break;
    }
  }

  /* return if nation is not found */
  if (cntry == ABSMAXNTN) return;

  /* first... unlink the nation */
  world.np[cntry] = (NTN_PTR) NULL;

  /* now go and remove all of the armies */
  a2_ptr = (ARMY_PTR) NULL;
  for (a1_ptr = ntn_tptr->army_list;
       a1_ptr != NULL; ) {

    /* go the the next element */
    a2_ptr = a1_ptr;
    a1_ptr = a1_ptr->next;

    /* destroy the old element */
    if (a2_ptr != NULL) {
      a2_ptr->next = (ARMY_PTR) NULL;
      a2_ptr->nrby = (ARMY_PTR) NULL;
      free(a2_ptr);
    }

  }

  /* >poof< goes all of the navies */
  n2_ptr = (NAVY_PTR) NULL;
  for (n1_ptr = ntn_tptr->navy_list;
       n1_ptr != NULL; ) {

    /* go the the next element */
    n2_ptr = n1_ptr;
    n1_ptr = n1_ptr->next;

    /* destroy the old element */
    if (n2_ptr != NULL) {
      n2_ptr->next = (NAVY_PTR) NULL;
      free(n2_ptr);
    }

  }

  /* convoy munching */
  v2_ptr = (CVN_PTR) NULL;
  for (v1_ptr = ntn_tptr->cvn_list;
       v1_ptr != NULL; ) {

    /* go the the next element */
    v2_ptr = v1_ptr;
    v1_ptr = v1_ptr->next;

    /* destroy the old element */
    if (v2_ptr != NULL) {
      v2_ptr->next = (CVN_PTR) NULL;
      free(v2_ptr);
    }

  }

  /* play godzilla and destroy cities! */
  c2_ptr = (CITY_PTR) NULL;
  for (c1_ptr = ntn_tptr->city_list;
       c1_ptr != NULL; ) {

    /* go the the next element */
    c2_ptr = c1_ptr;
    c1_ptr = c1_ptr->next;

    /* destroy the old element */
    if (c2_ptr != NULL) {
      c2_ptr->next = (CITY_PTR) NULL;
      free(c2_ptr);
    }

  }

  /* now gobble up the commodities */
  i2_ptr = (ITEM_PTR) NULL;
  for (i1_ptr = ntn_tptr->item_list;
       i1_ptr != NULL; ) {

    /* go the the next element */
    i2_ptr = i1_ptr;
    i1_ptr = i1_ptr->next;

    /* destroy the old element */
    if (i2_ptr != NULL) {
      i2_ptr->next = (ITEM_PTR) NULL;
      free(i2_ptr);
    }

  }
  
  /* now take care of the nation itself */
#ifndef VMS
  sprintf (string, "%s %s* 2> /dev/null", REMOVE_NAME, ntn_tptr->name);
#else
  sprintf (string, "%s %s.*;* 2> /dev/null", REMOVE_NAME, ntn_tptr->name);
#endif
  system(string);
  free(ntn_tptr);
}

/* element to keep track of switched nations */
static ntntype slot_val[ABSMAXNTN];

/* NTN_SWAP -- Swap two nations and keep track of the swap */
static void
ntn_swap PARM_2(int, left, int, right)
{
  NTN_PTR n_ptr;
  int value;

  /* swap the nation */
  n_ptr = world.np[left];
  world.np[left] = world.np[right];
  world.np[right] = n_ptr;

  /* keep track of the swap */
  value = slot_val[left];
  slot_val[left] = slot_val[right];
  slot_val[right] = value;
}

/* NTN_QSORT -- This routine is from K&R Second Edition; Page 110 */
static void
ntn_qsort PARM_2(int, left, int, right)
{
  int i, last;

  /* check for single elements */
  if (left >= right) return;

  ntn_swap(left, (left + right) / 2);
  last = left;
  for (i = left + 1; i <= right; i++) {
    if (str_test((world.np[i])->name, (world.np[left])->name) < 0) {
      ntn_swap(++last, i);
    }
  }
  ntn_swap(left, last);
  ntn_qsort(left, last - 1);
  ntn_qsort(last + 1, right);
}

/* NTN_SORT -- This routine places the nations in alphabetical order
               and gets rid of any empty slots.                      */
void
ntn_sort PARM_0(void)
{
  register int i, j;
  int count;
  ntntype newloc[ABSMAXNTN];
  uns_char stats[ABSMAXNTN];

  /* let them know what is up */
  if (!in_curses) {
    fprintf(fupdate, "sorting nations...");
  }

  /* assign default locations */
  for (i = 0; i < ABSMAXNTN; i++) {
    slot_val[i] = UNOWNED;
    newloc[i] = UNOWNED;
  }

  /* go through and shift out any empty slots */
  count = 1;
  for (i = 1; i < ABSMAXNTN; i++) {
    if (world.np[i] != NULL) {
      slot_val[count] = i;
      world.np[count++] = world.np[i];
    }
  }

  /* there should NEVER be a nation zero */
  if (world.np[0] != NULL) {
    slot_val[0] = count;
    world.np[count++] = world.np[0];
  }

  /* now make sure all remaining slots are nulled out */
  for (i = count; i < ABSMAXNTN; i++) {
    world.np[i] = (NTN_PTR) NULL;
  }

  /* the value of MAXNTN may now be set */
  MAXNTN = count;

  /* now perform a sort on the remaining nations */
  ntn_qsort(1, MAXNTN - 1);

  /* figure out where things came from */
  for (i = 0; i < ABSMAXNTN; i++) {
    if (slot_val[i] != UNOWNED) {
      newloc[slot_val[i]] = i;
    }
  }

  /* go through all nations; re-assigning storage */
  for (i = 0; i < MAXNTN; i++) {
    
    /* locate nation informatin */
    if ((ntn_tptr = world.np[i]) == NULL) continue;

    /* store diplomacy vectors */
    for (j = 0; j < ABSMAXNTN; j++) {
      stats[j] = ntn_tptr->dstatus[j];
    }

    /* go through and switch those vectors which need it */
    for (j = 0; j < ABSMAXNTN; j++) {
      if (newloc[j] == UNOWNED) {
	ntn_tptr->dstatus[j] = DIP_UNMET;
      } else {
	ntn_tptr->dstatus[newloc[j]] = stats[j];
      }
    }
    ntn_tptr->dstatus[UNOWNED] = DIP_NEUTRAL;
  }

  /* now assign sector ownership properly */
  for (i = 0; i < MAPX; i++)
    for (j = 0; j < MAPY; j++) {
      if ((newloc[sct[i][j].owner] == UNOWNED) &&
	  (sct[i][j].owner != UNOWNED)) {
	sct[i][j].designation = MAJ_NONE;
	sct[i][j].people = 0;
      }
      sct[i][j].owner = newloc[sct[i][j].owner];
    }
  if (!in_curses) {
    fprintf(fupdate, "done\n");
  }
#ifdef DEBUG
  verify_data(__FILE__, __LINE__);
#endif /*DEBUG*/
}

/* UNUM_DEFAULTS -- Set the default values */
void
unum_defaults PARM_0(void)
{
  /* now initialize the default unit numbering */
  resetnumbers();
#ifdef SAVE_SPACE
  newslotnumber(100, 0, 0);
  startnumber("all_leader 1", " in main_init");
  startnumber("all_spellcaster 25", " in main_init");
  startnumber("all_monster 50", " in main_init");
  startnumber("all_scout 200", " in main_init");
  startnumber("all_agent 200", " in main_init");
#else
  newslotnumber(200, 0, 0);
  startnumber("all_leader 1", " in main_init");
  startnumber("all_spellcaster 50", " in main_init");
  startnumber("all_monster 100", " in main_init");
  startnumber("all_scout 1000", " in main_init");
  startnumber("all_agent 1000", " in main_init");
#endif /* SAVE_SPACE */
}
