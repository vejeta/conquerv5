// SPDX-License-Identifier: GPL-3.0-or-later
/* This file contains data definitions used only during conqrun */
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
