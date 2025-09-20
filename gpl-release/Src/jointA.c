// SPDX-License-Identifier: GPL-3.0-or-later
/* Conqrun implementation of routines defined for both sections */
/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * A good deal of time and effort has gone into the writing of this
 * code and it is our hope that you respect this.  We give permission
 * to alter the code, but not to redistribute modified versions of the
 * code without our explicit permission.  If you do modify the code,
 * please document the changes made, and send us a copy, so that all
 * people may have it.  The code, to the best of our knowledge, works
 * well, but there will probably always be a need for bug fixes and
 * improvements.  We disclaim any responsibility for the codes'
 * actions.  [Use at your own risk].  This notice is just our way of
 * saying, "Happy gaming!", while making an effort to not get sued in
 * the process.
 *                           Ed Barlow, Adam Bryant
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
