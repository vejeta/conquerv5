// SPDX-License-Identifier: GPL-3.0-or-later
/* Conqrun implementation of routines defined for both sections */
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
#include "magicX.h"
/* variable needed but not really used yet */
int shrine_helped = 0;

/* DFLT_DISP_SETUP -- Build up one of the settings of the default display */
void
dflt_disp_setup PARM_3(char *, str, char *, fstr, int, lnum)
{
  /*ARGSUSED*/
}

/* DISPLAY_SETUP -- Configure the display modes */
void
display_setup PARM_3(char *, str, char *, fstr, int, lnum)
{
  /*ARGSUSED*/
}

/* KEYSYS_SETUP -- Configure the keybindings */
void
keysys_setup PARM_4(int, type, char *, str, char *, fstr, int, lnum)
{
  /*ARGSUSED*/
}

/* CHECK_SPELLS -- Implement the spell list */
void
check_spells PARM_3(int, spellnum, int, xloc, int, yloc)
{
  /* keep last known position */
  SPLINFO_PTR new_spell;
  static SPLINFO_PTR last_spell = NULL;

  /* read in the new spell */
  if ((new_spell = (SPLINFO_PTR) malloc(sizeof(SPLINFO_STRUCT))) == NULL) {
    fprintf(fupdate, "%s %d: Malloc failure\n", __FILE__, __LINE__);
    abrt();
  }

  /* this should be set properly during the update read in */
  new_spell->caster = country;

  /* set remaining information */
  new_spell->type = spellnum;
  new_spell->xloc = xloc;
  new_spell->yloc = yloc;
  new_spell->next = NULL;

  /* add to the list */
  if (upd_spl_list == NULL) {
    upd_spl_list = new_spell;
  } else {
    last_spell->next = new_spell;
  }
  last_spell = new_spell;
}

/* BIND_FUNC -- return a string name for the given function */
char *
bind_func PARM_1(int, which)
{
  /*ARGSUSED*/
  return((char *) NULL);
}

/* HANGUP -- signal catching routine */
void
hangup PARM_0(void)
{
  extern addlocknum, uplocknum;
  extern char lock_string[FILELTH];

  /* close any locks that are set */
  if (addlocknum != -1) {
    kill_lock(addlocknum, lock_string);
  }
  if (uplocknum != -1) {
    kill_lock(uplocknum, lock_string);
  }
}
