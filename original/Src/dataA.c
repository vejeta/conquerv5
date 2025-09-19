/* This file contains data definitions used only during conqrun */
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
#define DATA_DECLARE
#include "dataA.h"
#undef DATA_DECLARE
#include "magicX.h"
#include "adduserA.h"

/* partial names -- change dataA.h definitions */
char *mild_begin[] = {
  "A'", "Ael", "Aer", "Bai", "Bae", "B`", "Be", "Ben", "Cae", "Cai",
  "D`", "Dae", "De", "Dol", "E`", "Ee'", "Ea", "Ear", "Eir", "Eal",
  "Fa", "Fie", "Feo", "Gan", "Gin", "Hei", "Heo", "Ia", "Ier", "Ii",
  "Iia", "J`", "J'", "Jae", "Je", "Joa", "Kae", "Lae", "Lei", "Lei",
  "M'", "Mae", "Mea", "Mia", "Nae", "Nia", "Oa", "Oae", "Oai", "Oer",
  "Ole", "On", "Pae", "Pai", "Pao", "Q'", "Q`", "Qai", "Qe'", "Qei",
  "Que", "Qui", "Ra", "Rae", "Rai", "Sae", "Sai", "Soi", "Tea", "Tei",
  "Ua", "Ua'", "Uai", "Vae", "Vi", "W'", "Wai", "Wae", "Wa'", "Xai",
  "Xae", "Xea", "Yae", "Yai", "Yea", "Zae", "Zea"
};

/* useful definitions */
int remake;
char scenario[NAMELTH+1];
SPLINFO_PTR upd_spl_list;
