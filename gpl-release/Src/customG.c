// SPDX-License-Identifier: GPL-3.0-or-later
/* File to allow the writing of configuration files */
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
#include "dataG.h"
#include "armyX.h"
#include "desigX.h"
#include "worldX.h"
#include "elevegX.h"
#include "tgoodsX.h"
#include "displayG.h"
#include "optionsX.h"
#include "patchlevel.h"

/* SEND_KEYS -- Send to FILE* keybindings that differ, with identifier */
static void
send_keys PARM_3(FILE *, fp, KEYSYS_STRUCT, key_info, KLIST_PTR, klist_p)
{
  int count, max_kb;
  PARSE_PTR parse_ptr;
  char fnd_kb[BIGLTH];

  /* no bindings changed, if no bindings around */
  if (klist_p == NULL) return;

  /* first, mark all bindings as not found */
  max_kb = key_info.num_binds;
  for (count = 0; count < max_kb; count++) {
    fnd_kb[count] = FALSE;
  }

  /* go through the entire list */
  for (; klist_p != NULL; klist_p = klist_p->next) {

    /* go through and match the key bindings */
    for (count = 0; count < max_kb; count++) {
      /* check it out */
      if (strcmp(key_info.kbind_p[count].chlist,
		 klist_p->key_data.chlist) == 0) break;
    }

    /* record that it is found */
    fnd_kb[count] = TRUE;

    /* check out the results */
    if (count == max_kb) {

      /* new key binding */
      form_str(string, klist_p->key_data.chlist, FALSE);
      parse_ptr = find_func(klist_p->key_data.func,
			    key_info.parse_p, key_info.num_parse);
      if (parse_ptr != NULL) {
	fprintf(fp, "%s %s %s %s\n", opt_list[OPT_BINDKEY],
		key_info.name, string, parse_ptr->realname);
      }

    } else if (key_info.kbind_p[count].func != klist_p->key_data.func) {

      /* rebound function */
      form_str(string, klist_p->key_data.chlist, FALSE);
      parse_ptr = find_func(klist_p->key_data.func,
			    key_info.parse_p, key_info.num_parse);
      if (parse_ptr != NULL) {
	fprintf(fp, "%s %s %s %s\n", opt_list[OPT_REBIND],
		key_info.name, string, parse_ptr->realname);
      }

    }

  }

  /* report all deleted bindings */
  for (count = 0; count < max_kb; count++) {

    /* is it gone? */
    if (fnd_kb[count] == FALSE) {
      /* deleted keybinding */
      form_str(string, key_info.kbind_p[count].chlist, FALSE);
      fprintf(fp, "%s %s %s\n", opt_list[OPT_UNBIND],
	      key_info.name, string);
    }

  }
}

/* TARGET_STR -- Conversion routine to send string from target */
static char *
target_str PARM_2(int, hstyle, int, value)
{
  static char output_str[LINELTH];

  switch (hstyle) {
  case HI_MINDESG:
    /* give back proper range */
    if ((value < 0) ||
	(value >= MIN_NUMBER)) {
      value = 0;
    }
    strcpy(output_str, min_dinfo[value].name);
    break;
  case HI_MAJDESG:
    /* give back proper range */
    if ((value < 0) ||
	(value >= MAJ_NUMBER)) {
      value = 0;
    }
    strcpy(output_str, maj_dinfo[value].name);
    break;
  case HI_OWN:
    /* find proper nation */
    if ((value < 0) ||
	(value >= MAXNTN)) {
      value = UNOWNED;
    }
    if (value == UNOWNED) {
      strcpy(output_str, "*");
    } else {
      strcpy(output_str, (world.np[value])->name);
    }
    break;
  case HI_TGOODS:
    /* find proper tradegood */
    if ((value < 0) ||
	(value >= tgclass_number)) {
      value = TG_NONE;
    }
    if (value == TG_NONE) {
      strcpy(output_str, "*");
    } else {
      strcpy(output_str, tgclass_info[value].name);
    }
    break;
  default:
    /* no target possible */
    output_str[0] = '\0';
    break;
  }

  /* no match */
  return(&(output_str[0]));
}

/* SEND_DISPLAY -- Create the output of all of the display settings */
static void
send_display PARM_1(FILE *, fp)
{
  DMODE_PTR travel_dmode;
  char fnd_mode[DMODE_NUMBER];
  int count, count2;

  /* indicate that displays are being worked on */
  fprintf(fp, "#\n# display mode adjustments... if any\n");

  /* mark all modes as not found */
  for (count = 0; count < DMODE_NUMBER; count++) {
    fnd_mode[count] = FALSE;
  }

  /* compare all of the current display settings with the default */
  for (travel_dmode = dmode_list;
       travel_dmode != NULL;
       travel_dmode = travel_dmode->next) {

    /* now search among the original modes for it */
    for (count = 0; count < DMODE_NUMBER; count++) {
      if (strcmp(base_modes[count].name, travel_dmode->d.name) == 0) break;
    }

    /* new mode altogether */
    if (count == DMODE_NUMBER) {

      /* write out everything */
      fprintf(fp, "display-mode \"%s\" focus %s\n",
	      travel_dmode->d.name,
	      hex_list[travel_dmode->d.focus]);
      for (count2 = 0; count2 < HXPOS_NUMBER; count2++) {
	if (travel_dmode->d.style[count2] != DI_KEEP) {
	  fprintf(fp, "display-mode \"%s\" %s %s\n",
		  travel_dmode->d.name, hex_list[count2],
		  display_list[travel_dmode->d.style[count2]]);
	}
	if (travel_dmode->d.highlight[count2] != HI_KEEP) {
	  fprintf(fp, "display-mode \"%s\" highlight %s %s\n",
		  travel_dmode->d.name,
		  hex_list[count2],
		  highl_list[travel_dmode->d.highlight[count2]]);
	  if (hl_targets(travel_dmode->d.highlight[count2])) {
	    fprintf(fp, "display-mode \"%s\" target %s %s\n",
		    travel_dmode->d.name,
		    hex_list[count2],
		    target_str(travel_dmode->d.highlight[count2],
			       travel_dmode->d.target[count2]));
	  }
	}
      }
      
    } else {
      /* got a match; check each component */
      fnd_mode[count] = TRUE;

      /* check focus */
      if (travel_dmode->d.focus != base_modes[count].focus) {
	fprintf(fp, "display-mode \"%s\" focus %s\n",
		travel_dmode->d.name,
		hex_list[travel_dmode->d.focus]);
      }

      /* check all of the styles */
      for (count2 = 0; count2 < HXPOS_NUMBER; count2++) {
	if (travel_dmode->d.style[count2] !=
	    base_modes[count].style[count2]) {
	  fprintf(fp, "display-mode \"%s\" %s %s\n",
		  travel_dmode->d.name, hex_list[count2],
		  display_list[travel_dmode->d.style[count2]]);
	}

	/* check highlighting */
	if (travel_dmode->d.highlight[count2] !=
	    base_modes[count].highlight[count2]) {
	  fprintf(fp, "display-mode \"%s\" highlight %s %s\n",
		  travel_dmode->d.name,
		  hex_list[count2],
		  highl_list[travel_dmode->d.highlight[count2]]);
	  if (hl_targets(travel_dmode->d.highlight[count2])) {
	    fprintf(fp, "display-mode \"%s\" target %s %s\n",
		    travel_dmode->d.name,
		    hex_list[count2],
		    target_str(travel_dmode->d.highlight[count2],
			       travel_dmode->d.target[count2]));
	  }
	}
      }
    }

  }

  /* indicate any display modes that have been eliminated */
  for (count = 0; count < DMODE_NUMBER; count++) {
    if (fnd_mode[count] == FALSE) {
      /* the mode has been removed/renamed */
      fprintf(fp, "display-mode \"%s\" delete\n",
	      base_modes[count].name);
    }
  }

  /* now send out the default settings */
  fprintf(fp, "#\n# default display mode\n");
  fprintf(fp, "default-display focus %s\n",
	  hex_list[display_mode.focus]);
  for (count2 = 0; count2 < HXPOS_NUMBER; count2++) {
    fprintf(fp, "default-display %s %s\n", hex_list[count2],
	    display_list[display_mode.style[count2]]);
  }
  for (count2 = 0; count2 < HXPOS_NUMBER; count2++) {
    fprintf(fp, "default-display highlight %s %s\n",
	    hex_list[count2],
	    highl_list[display_mode.highlight[count2]]);
    if (hl_targets(display_mode.highlight[count2])) {
      fprintf(fp, "default-display target %s %s\n",
	      hex_list[count2],
	      target_str(display_mode.highlight[count2],
			 display_mode.target[count2]));
    }
  }
}

/* ALLKEYS_INIT -- Initialize all of the key bindings at once */
void
allkeys_init PARM_0(void)
{
  align_global_keys();
  align_move_keys();
  align_xfer_keys();
  align_email_keys();
  align_reader_keys();
  align_magic_keys();
  align_ninfo_keys();
}

/* ALLKEYS_CHECK -- Check all of the key bindings at once */
int
allkeys_check PARM_0(void)
{
  int hold;

  hold = check_keys(&cq_bindings, global_keysys);
  hold += check_keys(&mparse_bindings, mparse_keysys);
  hold += check_keys(&email_bindings, email_keysys);
  hold += check_keys(&reader_bindings, reader_keysys);
  hold += check_keys(&xfer_bindings, xfer_keysys);
  hold += check_keys(&mg_bindings, magic_keysys);
  hold += check_keys(&ni_bindings, ninfo_keysys);

  /* give back results */
  return(hold);
}

/* WRITE_KEYBINDS -- Write out the keybindings to the customization file */
static void
write_keybinds PARM_1(FILE *, fp)
{
  /* call each of the key binding functions */
  fprintf(fp, "#\n# key binding information (if any)\n");
  send_keys(fp, global_keysys, cq_bindings);
  send_keys(fp, email_keysys, email_bindings);
  send_keys(fp, reader_keysys, reader_bindings);
  send_keys(fp, xfer_keysys, xfer_bindings);
  send_keys(fp, magic_keysys, mg_bindings);
  send_keys(fp, mparse_keysys, mparse_bindings);
  send_keys(fp, ninfo_keysys, ni_bindings);
}

/* WRITE_CUSTOM -- This function writes out the list of currently options */
int
write_custom PARM_1(char *, fname)
{
  FILE *cust_fp;
  int count;
#ifndef VMS
#ifdef SWITCHID
#ifdef SYSV4
  uid_t uid, euid;
#else
  int uid, euid;
#endif /* SYSV4 */

  uid = getuid();
  euid = geteuid();

  /* does the effective uid == real uid */
  if (euid != uid) {
#ifdef SVSV4
    setuid(uid);
#else
    setreuid(euid, uid);
#endif /* SYSV4 */
  }
#endif /* SWITCHID */

  /* set the umask */
  (void ) umask(CUSTOM_UMASK);

#endif /* VMS */  

  /* open the file */
  if ((cust_fp = fopen(fname, "w")) == NULL) {
#ifndef VMS
    /* reset the umask */
    (void ) umask(DEFAULT_UMASK);
#ifdef SWITCHID
    if (euid != uid) {
#ifdef SYSV4
      setuid(euid);
#else
      setreuid(uid, euid);
#endif /* SYSV4 */
    }
#endif /* SWITCHID */
#endif /* VMS */  
    return(-1);
  }

  /* give the initial header */
  fprintf(cust_fp, "#\n");
  fprintf(cust_fp, "# Conquer Configuration File\n");
  fprintf(cust_fp, "#\n");
  fprintf(cust_fp, "# Automatically Generated by Conquer %s.%d\n",
	  VERSION, PATCHLEVEL);
  fprintf(cust_fp, "# Created: %s\n", mach_time());
  fprintf(cust_fp, "#\n");

  /* is this option set? */
  if (conq_allblanks) {
    fprintf(cust_fp, "%s\n", opt_list[OPT_ALLBLANKS]);
  } else {
    fprintf(cust_fp, "!%s\n", opt_list[OPT_ALLBLANKS]);
  }

  /* well, is this option set? */
  if (conq_bottomlines) {
    fprintf(cust_fp, "%s\n", opt_list[OPT_BOTTOMLINES]);
  } else {
    fprintf(cust_fp, "!%s\n", opt_list[OPT_BOTTOMLINES]);
  }

  /* how about this one? */
  if (conq_expert) {
    fprintf(cust_fp, "%s\n", opt_list[OPT_EXPERT]);
  } else {
    fprintf(cust_fp, "!%s\n", opt_list[OPT_EXPERT]);
  }

  /* or this one? */
  if (conq_gaudy) {
    fprintf(cust_fp, "%s\n", opt_list[OPT_GAUDY]);
  } else {
    fprintf(cust_fp, "!%s\n", opt_list[OPT_GAUDY]);
  }

  /* mail headers mode? */
  if (conq_mheaders) {
    fprintf(cust_fp, "%s\n", opt_list[OPT_HEADERS]);
  } else {
    fprintf(cust_fp, "!%s\n", opt_list[OPT_HEADERS]);
  }

  /* how about he information mode */
  if (conq_infomode) {
    fprintf(cust_fp, "%s\n", opt_list[OPT_INFOMODE]);
  } else {
    fprintf(cust_fp, "!%s\n", opt_list[OPT_INFOMODE]);
  }

  /* system mail check? */
  if (dosysm_check) {
    fprintf(cust_fp, "%s\n", opt_list[OPT_MAILCHECK]);
  } else {
    fprintf(cust_fp, "!%s\n", opt_list[OPT_MAILCHECK]);
  }

  /* how about the terminal bell? */
  if (conq_beeper) {
    fprintf(cust_fp, "%s\n", opt_list[OPT_TBELL]);
  } else {
    fprintf(cust_fp, "!%s\n", opt_list[OPT_TBELL]);
  }

  /* and then let's check this one? */
  if (conq_waterbottoms) {
    fprintf(cust_fp, "%s\n", opt_list[OPT_WATER_BTM]);
  } else {
    fprintf(cust_fp, "!%s\n", opt_list[OPT_WATER_BTM]);
  }

  /* store the pager information */
  fprintf(cust_fp, "%s: %d\n", opt_list[OPT_SUPPLY],
	  (conq_supply_level == MAXSUPPLIES)? 0 : conq_supply_level);
  fprintf(cust_fp, "%s: %d\n", opt_list[OPT_PAGETAB], pager_tab);
  fprintf(cust_fp, "%s: %d\n", opt_list[OPT_PAGESCROLL], pager_scroll);
  fprintf(cust_fp, "%s: %d\n", opt_list[OPT_PAGEOFFSET],
	  (pager_offset == LINES - 1)? 0 : pager_scroll);

  /* show the zoom level */
  fprintf(cust_fp, "zoom-level: %s\n", zooms[zoom_level]);

  /* store all of the contour symbols */
  fprintf(cust_fp, "#\n# countour symbols\n");
  for (count = 0; count < ELE_NUMBER; count++) {
    fprintf(cust_fp, "contour %s=%c\n",
	    ele_info[count].name,
	    ele_info[count].symbol);
  }

  /* store all of the vegetation symbols */
  fprintf(cust_fp, "#\n# vegetation symbols\n");
  for (count = 0; count < VEG_NUMBER; count++) {
    fprintf(cust_fp, "vegetation %s=%c\n",
	    veg_info[count].name,
	    veg_info[count].symbol);
  }

  /* store all of the designation symbols */
  fprintf(cust_fp, "#\n# designation symbols\n");
  for (count = 0; count < MAJ_NUMBER; count++) {
    fprintf(cust_fp, "designation %s=%c\n",
	    maj_dinfo[count].name,
	    maj_dinfo[count].symbol);
  }

  /* now handle the display modes and key mappings */
  write_keybinds(cust_fp);
  send_display(cust_fp);

  /* end of file */
  fprintf(cust_fp, "#\n# end of prefab file\n#\n");
  fclose(cust_fp);
#ifndef VMS
  /* reset the umask */
  (void ) umask(DEFAULT_UMASK);
#ifdef SWITCHID
  if (euid != uid) {
#ifdef SYSV4
    setuid(euid);
#else
    setreuid(uid, euid);
#endif /* SYSV4 */
  }
#endif /* SWITCHID */
#endif /* VMS */  
  return(0);
}
