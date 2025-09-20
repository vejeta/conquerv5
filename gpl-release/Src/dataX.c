// SPDX-License-Identifier: GPL-3.0-or-later
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
#include "dataX.h"
#undef DATA_DECLARE
#include "butesX.h"
#include "desigX.h"
#include "magicX.h"
#include "mtrlsX.h"
#include "racesX.h"
#include "elevegX.h"
#include "nclassX.h"
#include "tgoodsX.h"
#include "displayX.h"
#include "optionsX.h"

/* raw materials description */
MTRLS_STRUCT mtrls_info[MTRLS_NUMBER] = {
  { "Talons", "talons", 1, 1 },
  { "Jewels", "jewels", 4, 10 },
  { "Metals", "metals", 5, 5 },
  { "Food", "food", 10, 1 },
  { "Wood", "wood", 8, 3 }
};

/* nation attribute information */
BUTE_STRUCT bute_info[BUTE_NUMBER] = {
  { "Charity",
      "The percentage of net income given to support the population",
      0, 0, MAXCHARITY, 1, 1, FALSE },
  { "Communication",
      "The distance, in sectors, over which supply centers may work",
      20, 20, 80, 10, 1, TRUE },
  { "Currency",
      "The relative value of the currency used within your nation",
      50, 10, 90, 1, 1, FALSE },
  { "Eat Rate",
      "The number of food units consumed per civilian in the nation",
      10, 7, 40, 10, 1, FALSE },
  { "Health",
      "The relative health value of the civilian population",
      70, 10, 100, 1, 1, FALSE },
  { "Inflation",
      "The yearly inflation rate of the national economy",
      0, -150, 150, 1, 1, FALSE },
  { "Jewelcraft",
      "An indication as to how well a nation mines and processes jewels",
      10, 5, 100, 1, 1, TRUE },
  { "Knowledge",
      "The average intellectual level of the population in the nation",
      10, 5, 100, 1, 1, TRUE },
  { "Merc Rep",
      "How well respected your nation is by the mercenaries of the world",
      50, 0, 100, 1, 1, FALSE },
  { "Metalcraft",
      "An indication as to how well a nation mines and processes metals",
      10, 5, 100, 1, 1, TRUE },
  { "Mine Ability",
      "The ability and willingness of a population to mine materials",
      10, 5, 100, 1, 1, TRUE },
  { "Morale",
      "The general morale of the soldiers within your nation",
      50, 0, 100, 1, 1, TRUE },
  { "Popularity",
      "The favorability rating of the government by its people",
      25, 0, 100, 1, 1, TRUE },
  { "Reputation",
      "How well this nation is regarded by the other nations of the world",
      10, 5, 95, 1, 1, TRUE },
  { "Spell Pts",
      "The maximum number of spell points which a leader may have",
      0, 0, 150, 1, 1, TRUE },
  { "Spoilrate",
      "The yearly rate at which food spoils within a nation",
      90, 3, 95, 1, -1, FALSE },
  { "Tax Rate",
      "The rate at which taxes are collected from the population",
      10, 0, MAXTAX, 1, 0, FALSE },
  { "Terror",
      "The percentage of the population which fears the government",
      0, 0, 99, 1, 1, TRUE },
  { "Wizardcraft",
      "The skill with which magical spells are cast by the nation's units",
      1, 1, 100, 1, 1, TRUE }
};

/* attraction settings */
unsigned char veg_attract[RACE_NUMBER][VEG_NUMBER] = {
  /* God */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  /* Orc */
  0, 0, 0, 60, 70, 100, 80, 60, 0, 0, 0, 0,
  /* Elf */
  0, 0, 0, 30, 80, 100, 70, 80, 0, 0, 0, 0,
  /* Dwarf */
  0, 0, 0, 60, 75, 100, 80, 60, 0, 0, 0, 0,
  /* Human */
  0, 0, 0, 60, 75, 100, 80, 60, 0, 0, 0, 0,
  /* Lizard */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  /* Pirate */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  /* Savage */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  /* Nomad */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  /* Unknown */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/* elevation attraction (acts as percentage multiplier) */
unsigned char ele_attract[RACE_NUMBER][ELE_NUMBER] = {
  /* God */
  0, 0, 0, 0, 0, 0,
  /* Orc */
  0, 90, 130, 110, 90, 10,
  /* Elf */
  0, 80, 110, 90, 60, 0,
  /* Dwarf */
  0, 90, 120, 110, 80, 5,
  /* Human */
  0, 80, 130, 95, 70, 5,
  /* Lizard */
  0, 0, 0, 0, 0, 0,
  /* Pirate */
  0, 0, 0, 0, 0, 0,
  /* Savage */
  0, 0, 0, 0, 0, 0,
  /* Nomad */
  0, 0, 0, 0, 0, 0,
  /* Unknown */
  0, 0, 0, 0, 0, 0
};

/* elevation information */
ELEVEG_STRUCT ele_info[ELE_NUMBER] = {
  { "Water", '~', 0, 0, 1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      1, 0, 1, 10,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { "Valley", 'v', 1, 0, 3,
      3, 2, 3, 2, 3, 3, 3, 3, 3, 3,
      0, 0, 0, 0,
      80, 90, 95, 90, 80, 80, 80, 80, 80, 80 },
  { "Flat", '-', 1, 0, 1,
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
      0, 0, 0, 1,
      120, 130, 110, 120, 130, 120, 120, 120, 120, 120 },
  { "Hill", '%', 0, 0, 1,
      2, 2, 2, 1, 2, 2, 2, 2, 2, 2,
      0, 0, 0, 3,
      100, 110, 90, 110, 95, 100, 100, 100, 100, 100 },
  { "Mountain", '^', -1, -1, 2,
      4, 3, 5, 3, 4, 4, 4, 4, 4, 4,
      1, 0, 1, 8,
      70, 90, 60, 80, 70, 70, 70, 70, 70, 70 },
  { "Peak", '#', -2, -2, 6,
      6, 4, 8, 4, 6, 6, 6, 6, 6, 6,
      8, 3, 8, 20,
      5, 10, 0, 5, 5, 5, 5, 5, 5, 5 }
};

/* vegetation information */
ELEVEG_STRUCT veg_info[VEG_NUMBER] = {
  { "Volcano", '!', 0, 0, 4,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      3, 5, 3, 1,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { "Desert", '.', 0, 0, 1,
      3, 3, 6, 5, 3, 3, 3, 3, 3, 3,
      6, 30, 6, 16,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { "Tundra", ',', 0, 0, 1,
      3, 2, 4, 3, 3, 3, 3, 3, 3, 3,
      20, 5, 20, 40,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { "Barren", 'b', 4, 0, 0,
      1, 1, 1, 0, 1, 1, 1, 1, 1, 1,
      0, 2, 0, 15,
      60, 60, 30, 60, 60, 60, 60, 60, 60, 60 },
  { "Lt Veg", 'l', 6, 1, 0,
      0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 10,
      75, 70, 80, 75, 75, 75, 75, 75, 75, 75 },
  { "Good", 'g', 9, 2, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 8,
      100, 100, 90, 100, 100, 100, 100, 100, 100, 100 },
  { "Wood", 'w', 7, 4, 0,
      0, 2, 0, 1, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 6,
      80, 80, 85, 80, 80, 80, 80, 80, 80, 80 },
  { "Forest", 'f', 4, 8, 0,
      1, 2, 0, 3, 1, 1, 1, 1, 1, 1,
      0, 0, 0, 7,
      40, 40, 70, 40, 40, 40, 40, 40, 40, 40 },
  { "Jungle", 'j', 0, 10, 1,
      3, 4, 2, 5, 3, 3, 3, 3, 3, 3,
      10, 20, 10, 6,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { "Swamp", '"', 0, 0, 0,
      3, 3, 2, 4, 3, 3, 3, 3, 3, 3,
      16, 26, 12, 10,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { "Ice", 'i', 0, 0, 0,
      2, 7, 7, 7, 2, 2, 2, 2, 2, 2,
      6, 0, 6, 18,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { "None", '~', 0, 0, 0,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
};

/* major designation descriptions */
DESG_STRUCT maj_dinfo[MAJ_NUMBER] = {
  { "None", '-', 0, 0, 1, 1,
      15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
      0, 0, 0, 0,
      /* all but roads, fort, sieged, devastated, tpost and forsale */
      (MIN_ROADS | MIN_FORTIFIED | MIN_SIEGED | MIN_DEVASTATED |
       MIN_TRADINGPOST | MIN_FORSALE) ^ 0xFFFF,
      500L, 0L, 0L, 0L, 0L,
      0L, 0L, 0L, 0L, 0L },
  { "Farm", 'F', 0, 0, 5, 1,
      60, 60, 65, 60, 65, 60, 60, 60, 60, 60,
      50, 50, 50, 0,
      MIN_UNIVERSITY | MIN_HARBOR | MIN_BLACKSMITH,
      1000L, 0L, 0L, 0L, 0L,
      20L, 0L, 0L, 0L, 0L },
  { "Fertile", 'F', 0, 0, 5, 1,
      80, 80, 85, 80, 85, 80, 80, 80, 80, 80,
      50, 100, 100, 0,
      MIN_UNIVERSITY | MIN_HARBOR | MIN_BLACKSMITH,
      1000L, 0L, 0L, 0L, 0L,
      20L, 0L, 0L, 0L, 0L },
  { "Fruitful", 'F', 0, 0, 5, 1,
      90, 90, 90, 90, 90, 90, 90, 90, 90, 90,
      50, 100, 200, 0,
      MIN_UNIVERSITY | MIN_HARBOR | MIN_BLACKSMITH,
      1000L, 0L, 0L, 0L, 0L,
      20L, 0L, 0L, 0L, 0L },
  { "Metal Mine", 'm', 0, 0, 9, 1,
      55, 60, 55, 65, 55, 55, 55, 55, 55, 55,
      95, 105, 95, 75,
      MIN_UNIVERSITY | MIN_HARBOR | MIN_MILL | MIN_GRANARY,
      3000L, 0L, 200L, 0L, 100L,
      60L, 0L, 10L, 0L, 5L },
  { "Jewel Mine", '$', 0, 0, 9, 1,
      55, 60, 55, 65, 55, 55, 55, 55, 55, 55,
      95, 105, 95, 75,
      MIN_UNIVERSITY | MIN_HARBOR | MIN_BLACKSMITH | MIN_MILL | MIN_GRANARY,
      3000L, 0L, 200L, 0L, 100L,
      60L, 0L, 10L, 0L, 5L },
  { "Lumberyard", ':', 0, 0, 5, 1,
      40, 40, 50, 40, 40, 40, 40, 40, 40, 40,
      50, 75, 100, 25,
      MIN_UNIVERSITY | MIN_HARBOR | MIN_BLACKSMITH | MIN_GRANARY,
      2000L, 0L, 200L, 0L, 50L,
      30L, 0L, 8L, 0L, 5L },
  { "Shrine", '@', 0, 0, 5, 1,
      30, 20, 35, 30, 30, 30, 30, 30, 30, 30,
      100, 100, 100, 85,
      MIN_UNIVERSITY | MIN_HARBOR | MIN_BLACKSMITH | MIN_MILL | MIN_GRANARY,
      5000L, 1000L, 500L, 0L, 150L,
      50L, 10L, 40L, 0L, 10L },
  { "Bridge", '[', 0, 0, 7, 3,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      100, 100, 100, 50,	/* all but fortified and devastated */
      ((MIN_FORTIFIED | MIN_DEVASTATED) ^ 0xFFFF) | DINFO_BUILDD,
      50000L, 0L, 3000L, 0L, 15000L,
      1000L, 0L, 50L, 0L, 50L },
  { "Canal", '=', 0, 0, 7, 2,
      5, 5, 5, 5, 5, 15, 15, 15, 15, 5,
      100, 100, 100, 50,
      MIN_UNIVERSITY | MIN_HARBOR | MIN_BLACKSMITH | MIN_MILL |
	MIN_GRANARY | DINFO_BUILDC,
      40000L, 0L, 4000L, 0L, 7500L,
      1500L, 0L, 60L, 0L, 200L },
  { "Wall", '|', 0, 0, 9, 2,
      5, 5, 5, 5, 5, 30, 30, 30, 30, 5,
      100, 100, 100, 100,
      MIN_UNIVERSITY | MIN_HARBOR | MIN_BLACKSMITH | MIN_MILL |
	MIN_GRANARY | DINFO_BUILDC,
      35000L, 0L, 5000L, 0L, 10000L,
      1250L, 0L, 100L, 0L, 100L },
  { "Cache", '&', 0, 0, 5, 1,
      15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
      100, 100, 100, 100,
      MIN_UNIVERSITY | MIN_HARBOR | MIN_BLACKSMITH | MIN_MILL |
	MIN_GRANARY | DINFO_BUILDB,
      2000L, 0L, 2000L, 0L, 1000L,
      0L, 0L, 0L, 0L, 0L },
  { "Stockade", 's', 1, 50, 20, 3,
      30, 35, 30, 40, 35, 30, 30, 30, 30, 30,
      100, 100, 100, 100,
      MIN_UNIVERSITY | MIN_MILL | DINFO_BUILDB,
      10000L, 0L, 1000L, 0L, 10000L,
      3000L, 0L, 100L, 0L, 200L },
  { "Town", 't', 1, 10, 40, 5,
      110, 110, 110, 110, 110, 110, 110, 110, 110, 110,
      100, 100, 100, 100,
      MIN_FORSALE | MIN_UNIVERSITY,
      10000L, 0L, 10000L, 0L, 10000L,
      1000L, 0L, 20L, 0L, 200L },
  { "City", 'c', 3, 20, 100, 8,
      150, 150, 150, 150, 155, 150, 150, 150, 150, 150,
      100, 100, 100, 100,
      MIN_FORSALE,
      20000L, 0L, 50000L, 0L, 5000L,
      3000L, 0L, 50L, 0L, 200L },
  { "Capital", 'C', 1, 25, 110, 8,
      185, 185, 185, 185, 190, 185, 185, 185, 185, 185,
      100, 100, 100, 100,
      MIN_FORSALE,
      150000L, 0L, 5000L, 0L, 1000L,
      3500L, 0L, 60L, 0L, 220L }
};

/* minor designation descriptions */
DESG_STRUCT min_dinfo[MIN_NUMBER] = {
  { "devastated", 'x', 0, 0, 25, 25,
      25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
      0, 0, 0, 0,
      DINFO_BUILDA,
      1000L, 0L, 0L, 0L, 0L,
      0L, 0L, 0L, 0L, 0L },
  { "for sale", 'x', 0, 0, 100, 100,
      50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
      0, 0, 0, 0,
      0,
      0L, 0L, 0L, 0L, 0L,
      0L, 0L, 0L, 0L, 0L },
  { "sieged", 'x', 0, 0, 80, 20,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0,
      0,
      0L, 0L, 0L, 0L, 0L,
      200L, 0L, 0L, 0L, 0L},
  { "trading post", '_', 0, 0, 105, 105,
      110, 110, 110, 110, 110, 110, 110, 110, 110, 110,
      0, 0, 0, 0,
      0,
      5000L, 0L, 20L, 0L, 500L,
      1000L, 0L, 1L, 0L, 1L },
  { "roads", '-', 0, 0, 100, 100,
      105, 105, 105, 105, 105, 105, 105, 105, 105, 105,
      0, 0, 0, 0,
      DINFO_BUILDB,
      3000L, 0L, 20L, 0L, 1000L,
      400L,  0L, 1L, 0L, 20L },
  { "blacksmith", 'b', 0, 0, 110, 110,
      105, 105, 105, 115, 110, 100, 100, 100, 100, 100,
      0, 0, 0, 0,
      0,
      2500L, 0L, 1000L, 0L, 0L,
      400L, 0L, 200L, 0L, 0L },
  { "university", 'u', 0, 0, 103, 103,
      100, 90, 115, 105, 110, 100, 100, 100, 100, 100,
      0, 0, 0, 0,
      0,
      2500L, 0L, 0L, 0L, 300L,
      500L, 0L, 0L, 0L, 10L },
  { "church", 'o', 0, 0, 95, 110,
      105, 110, 105, 105, 105, 105, 105, 105, 105, 105,
      0, 0, 0, 0,
      0,
      3000L, 0L, 10L, 0L, 200L,
      300L, 0L, 0L, 0L, 10L },
  { "mill", '*', 0, 0, 100, 110,
      100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
      0, 0, 0, 0,
      0,
      3000L, 0L, 500L, 0L, 300L,
      200L, 0L, 25L, 0L, 50L },
  { "granary", '0', 0, 0, 103, 100,
      105, 105, 105, 105, 105, 105, 105, 105, 105, 105,
      0, 0, 0, 0,
      0,
      2000L, 0L, 50L, 0L, 100L,
      200L, 0L, 3L, 0L, 10L },
  { "fortified", '!', 0, 0, 100, 100,
      110, 115, 115, 120, 120, 80, 80, 80, 80, 100,
      0, 0, 0, 0,
      DINFO_BUILDB,
      5000L, 0L, 2000L, 0L, 1000L,
      500L, 0L, 100L, 0L, 100L },
  { "harbor", 'h', 0, 0, 110, 100,
      105, 105, 105, 105, 105, 105, 105, 105, 105, 105,
      0, 0, 0, 0,
      DINFO_BUILDB,
      1000L, 0L, 500L, 0L, 3000L,
      100L, 0L, 25L, 0L, 100L }
};

/* the list of nation class information */
NCLASS_STRUCT nclass_list[] = {
  { "Monster", "King", "Baron", "DEHO", "None", 0, 10,
      0x0L, 0x0L, 0x0L },
  { "Kingdom", "King", "Baron", "DEHO", "None", 0, 10,
      0x0L, 0x0L, 0x0L },
  { "Empire", "Emperor", "Prince", "DEH", "None", 0, 10,
      0x0L, 0x0L, 0x0L },
  { "Magocracy", "Wizard", "Mage", "EH", "Sorcerer", 2, 7,
      0x0L, 0x0L, MW_WYZARD | MW_SORCERER },
  { "Theocracy", "Pope", "Cardinal", "DH", "Religion", 0, 10,
      0x0L, MC_RELIGION, 0x0L },
  { "Piracy", "Admiral", "Captain", "DHO", "Sailor", 0, 10,
      0x0L, MC_SAILOR, 0x0L },
  { "Plutocracy", "King", "Baron", "DEH", "Urban", 0, 10,
      0x0L, MC_URBAN, 0x0L },
  { "Feudality", "Warlord", "Lord", "DEHO", "Warlord", 0, 7,
      MM_WARRIOR | MM_CAPTAIN | MM_WARLORD, 0x0L, 0x0L },
  { "Demonarchy", "Demon", "Devil", "O", "Destroyer", 2, 6,
      0x0L, 0x0L, MW_DESTROYER },
  { "Dragocracy", "Dragyn", "Wyrm", "O", "Dragyn", 2, 6,
      MM_ORC | MM_OGRE | MM_DRAGON, 0x0L, 0x0L },
  { "Shadowland", "Shadow", "Nazgul", "O", "Vampire", 4, 7,
      0x0L, 0x0L, MW_VAMPIRE }
};

/* The racial information */
RACE_STRUCT race_info[RACE_NUMBER] = {
  { "God", "The omnipotent, omniscient, omnieverything Conquer Deity",
      0x0L, 0x0L, 0x0L,
      0x0L, 0x0L, 0x0L,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0,
      RT_NOTRACE,
      0L, 0L, 0L,
      50, 15, 4, 8, 80, 80, 10, 20, 5, 20, 10,
      25, 1, 1, 3, 20, 10, 5, 10, 0, 0, 1,
      1, 1, 1, 1, 1, 1, 2, 1, 3, 1, 1,
      1, 1, 1, 2, 8, 8, 1, 2, 1, 4, 1 },
  { "Orc",
      "A infestation to other races, Orcs are proof of power in numbers",
      MM_ORC, 0x0L, 0x0L,
      MM_EQUINE,
      MC_METALCRAFT | MC_JEWELER | MC_WOODCRAFT | MC_BOTANY,
      MW_KNOWALL,
      100, 100, 115, 110, 100, 100, 100, 100, 100, 100,
      0, 10, 15, 10, 10, 0, 0, 0, 0, 0,
      100, 25, 10, 15, 20, 100, 100, 100, 100, 100,
      15,
      RT_MOUNTAINEER | RT_MONSTERSKILL,
      65000L, 45000L, 60000L,
      50, 12, 4, 8, 70, 70, 12, 16, 4, 22, 10,
      25, 1, 1, 3, 0, 0, 6, 8, 0, 0, 1,
      1, 1, 1, 1, 1, 1, 3, 1, 4, 1, 1,
      1, 1, 1, 2, 4, 4, 2, 1, 1, 5, 1 },
  { "Elf",
      "A magical race who strive to live within the world around them",
      0x0L, 0x0L, MW_ILLUSION,
      MM_ORC | MM_OGRE | MM_DRAGON, MC_BREEDER, MW_KNOWALL,
      115, 100, 110, 100, 100, 100, 100, 100, 100, 100,
      0, 15, 5, 10, 10, 0, 0, 0, 0, 0,
      100, 15, 20, 15, 20, 100, 100, 100, 100, 100,
      10,
      RT_WOODWINTER | RT_ANTIMILITARY | RT_WIZARDLY | RT_MAGICSKILL,
      60000L, 50000L, 40000L,
      50, 14, 4, 6, 60, 60, 10, 20, 6, 18, 10,
      25, 1, 2, 2, 0, 0, 6, 8, 1, 0, 1,
      1, 1, 1, 1, 1, 1, 3, 1, 3, 1, 1,
      1, 1, 1, 1, 6, 8, 1, 2, 1, 4, 1 },
  { "Dwarf",
      "Small yet strong, this proud race gains wealth from the earth itself",
      0x0L, MC_MINER, 0x0L,
      MM_ORC | MM_OGRE | MM_DRAGON | MM_NINJA, MC_BREEDER,
      MW_KNOWALL | MW_SENDING,
      110, 110, 100, 100, 100, 100, 100, 100, 100, 100,
      0, 15, 10, 5, 10, 0, 0, 0, 0, 0,
      100, 20, 10, 20, 20, 100, 100, 100, 100, 100,
      11,
      RT_ANTIMAGIC | RT_MOUNTAINEER | RT_MINING | RT_FIGHTERS,
      40000L, 45000L, 70000L,
      50, 16, 4, 10, 100, 100, 10, 18, 3, 20, 10,
      25, 2, 1, 4, 30, 30, 6, 8, 1, 0, 1,
      1, 2, 1, 1, 1, 1, 2, 1, 5, 1, 1,
      1, 3, 1, 3, 10, 10, 1, 2, 1, 4, 2 },
  { "Human",
      "A race whose greatest skill is the ability to survive",
      MM_WARRIOR, 0x0L, 0x0L,
      MM_ORC | MM_OGRE | MM_DRAGON, MC_BREEDER, MW_KNOWALL,
      100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
      0, 10, 10, 10, 5, 0, 0, 0, 0, 0,
      100, 20, 20, 20, 20, 100, 100, 100, 100, 100,
      12,
      0x0L,
      50000L, 50000L, 50000L,
      50, 16, 4, 10, 82, 80, 10, 18, 3, 20, 10,
      25, 2, 1, 4, 27, 24, 6, 8, 1, 0, 1,
      1, 1, 1, 1, 1, 1, 2, 1, 3, 1, 1,
      1, 1, 1, 2, 9, 8, 1, 2, 1, 4, 1 },
  { "Lizard",
      "An acient race of reptiles who gaurd their hoards from others",
      0x0L, 0x0L, 0x0L,
      0x0L, 0x0L, 0x0L,
      100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
      0, 30, 30, 30, 30, 30, 30, 30, 30, 30,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,
      12,
      RT_NOTRACE,
      50000L, 50000L, 50000L,
      50, 16, 4, 10, 82, 80, 10, 18, 3, 20, 10,
      25, 2, 1, 4, 27, 24, 6, 8, 1, 0, 1,
      1, 1, 1, 1, 1, 1, 2, 1, 3, 1, 1,
      1, 1, 1, 2, 9, 8, 1, 2, 1, 4, 1 },
  { "Pirate",
      "Always in search of merchant ships, their fleets are feared by many",
      0x0L, 0x0L, 0x0L,
      0x0L, 0x0L, 0x0L,
      100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
      0, 30, 30, 30, 30, 30, 30, 30, 30, 30,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      12,
      RT_NOTRACE,
      50000L, 50000L, 50000L,
      50, 16, 4, 10, 82, 80, 10, 18, 3, 20, 10,
      25, 2, 1, 4, 27, 24, 6, 8, 1, 0, 1,
      1, 1, 1, 1, 1, 1, 2, 1, 3, 1, 1,
      1, 1, 1, 2, 9, 8, 1, 2, 1, 4, 1 },
  { "Savage",
      "Belonging only to themselves, monsters and men who roam for plunder",
      0x0L, 0x0L, 0x0L,
      0x0L, 0x0L, 0x0L,
      100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
      0, 25, 25, 25, 25, 25, 25, 25, 25, 25,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      12,
      RT_NOTRACE,
      50000L, 50000L, 50000L,
      50, 16, 4, 10, 82, 80, 10, 18, 3, 20, 10,
      25, 2, 1, 4, 27, 24, 6, 8, 1, 0, 1,
      1, 1, 1, 1, 1, 1, 2, 1, 3, 1, 1,
      1, 1, 1, 2, 9, 8, 1, 2, 1, 4, 1 },
  { "Nomad",
      "Living on horseback, they savage whatever they find",
      0x0L, 0x0L, 0x0L,
      0x0L, 0x0L, 0x0L,
      100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
      0, 20, 20, 20, 20, 20, 20, 20, 20, 20,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      12,
      RT_NOTRACE,
      50000L, 50000L, 50000L,
      50, 16, 4, 10, 82, 80, 10, 18, 3, 20, 10,
      25, 2, 1, 4, 27, 24, 6, 8, 1, 0, 1,
      1, 1, 1, 1, 1, 1, 2, 1, 3, 1, 1,
      1, 1, 1, 2, 9, 8, 1, 2, 1, 4, 1 },
  { "Unknown",
      "Serving no purpose, living without cause",
      0x0L, 0x0L, 0x0L,
      0x0L, 0x0L, 0x0L,
      100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
      0, 20, 20, 20, 20, 20, 20, 20, 20, 20,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      12,
      RT_NOTRACE,
      50000L, 50000L, 50000L,
      50, 16, 4, 10, 82, 80, 10, 18, 3, 20, 10,
      25, 2, 1, 4, 27, 24, 6, 8, 1, 0, 1,
      1, 1, 1, 1, 1, 1, 2, 1, 3, 1, 1,
      1, 1, 1, 2, 9, 8, 1, 2, 1, 4, 1 }
};

/* The Tradegood classes */
TGCLASS_STRUCT tgclass_info[] = {
  { "None",
      "This sector does not have any tradegoods within it",
      "none", BUTE_NUMBER, 1 },
  { "Popularity",
      "Such items increase the approval of the people for the government",
      "increase goverment popularity by VALUE for a populated sector",
      BUTE_POPULARITY, 1 },
  { "Communication",
      "With a supply center of at least that level, increase communications",
      "Add 0.1 to communication range for each VALUE point",
      BUTE_COMMRANGE, 1 },
  { "Fishing",
      "These items indicate sectors which generate more food for sector",
      "Adds 1 to sector food potential for every VALUE point",
      BUTE_NUMBER, 1 },
  { "Farming",
      "These items increase the food output of a sector",
      "Increase food potential within the sector by VALUE",
      BUTE_NUMBER, 1 },
  { "Spoilrate",
      "Food is preserved better, decreasing spoilage",
      "Spoilrate is decreased by VALUE percent",
      BUTE_SPOILRATE, 1 },
  { "Lumber",
      "Such sectors enhance the wood valuw of a sector",
      "Incease the wood potential of a sector by VALUE",
      BUTE_NUMBER, 1 },
  { "Knowledge",
      "Such scholarly things increase the knowledge base of the nation",
      "Increase national knowledge rating by VALUE",
      BUTE_KNOWLEDGE, 1 },
  { "Eat Rate",
      "Delicacies and nutrition lead a nation to eat a bit less food",
      "Decrease national eat rate by 0.1 for every VALUE",
      BUTE_EATRATE, -1 },
  { "Health",
      "These items increase the general health of the population",
      "Increase national health rate by VALUE percent",
      BUTE_HEALTH, 1 },
  { "Terror",
      "Such items increase fear and dread of the goverment among civilians",
      "Increase population terror of government by VALUE percent",
      BUTE_TERROR, 1 },
  { "Magical",
      "Magical goods enhanced the spell casting capabilites of a nation",
      "Provide magical potential up to a maximum of VALUE within sector",
      BUTE_SPELLPTS, 1 },
  { "Metals",
      "Mining of such goods gives metal resources to the nation",
      "Provide metal potential up to a maximum of VALUE within sector",
      BUTE_METALWORK, 1 },
  { "Jewels",
      "Mining of gems and precious metals enhances wealth and power",
      "Provide jewel potential upto to a maximum of VALUE within sector",
      BUTE_JEWELWORK, 1 }
};

/* Sector Tradegood descriptions */
TGOOD_STRUCT tg_info[] = {
  /* empty -- might want to keep this at zero slot :-) */
  { "none", 0, 99, TG_NONE, MAJ_NONE, 0 },

  /* luxury items */
  { "furs", 1, 9, TG_POPULARITY, MAJ_NONE, 75 },
  { "wool", 2, 5, TG_POPULARITY, MAJ_NONE, 150 },
  { "cloth", 3, 4, TG_POPULARITY, MAJ_NONE, 200 },
  { "beer", 4, 3, TG_POPULARITY, MAJ_FARM, 200 },
  { "wine", 5, 2, TG_POPULARITY, MAJ_FARM, 210 },
  { "cannabis", 5, 1, TG_POPULARITY, MAJ_FARM, 250 },
  { "poppy", 6, 1, TG_POPULARITY, MAJ_FARM, 300 },

  /* communication techniques */
  { "mules", 1, 7, TG_COMMUNICATE, MAJ_STOCKADE, 50 },
  { "horses", 2, 5, TG_COMMUNICATE, MAJ_STOCKADE, 100 },
  { "pigeons", 4, 2, TG_COMMUNICATE, MAJ_TOWN, 500 },
  { "griffins", 6, 1, TG_COMMUNICATE, MAJ_TOWN, 750 },

  /* fishing industry */
  { "pike", 1, 9, TG_FISHING, MAJ_FARM, 50 },
  { "crayfish", 1, 9, TG_FISHING, MAJ_FARM, 50 },
  { "crab", 2, 6, TG_FISHING, MAJ_FARM, 100 },
  { "salmon", 2, 6, TG_FISHING, MAJ_FARM, 100 },
  { "trout", 3, 4, TG_FISHING, MAJ_FARM, 200 },
  { "shrimp", 3, 4, TG_FISHING, MAJ_FARM, 200 },
  { "lobster", 4, 2, TG_FISHING, MAJ_FARM, 300 },
  { "flounder", 4, 2, TG_FISHING, MAJ_FARM, 300 },
  
  /* lower eat rate of the nation */
  { "barley", 1, 9, TG_EATRATE, MAJ_FARM, 200 },
  { "peas", 1, 9, TG_EATRATE, MAJ_FARM, 200 },
  { "wheat", 2, 6, TG_EATRATE, MAJ_FARM, 225 },
  { "dairy", 2, 6, TG_EATRATE, MAJ_FARM, 225 },
  { "rice", 3, 3, TG_EATRATE, MAJ_FARM, 275 },
  { "corn", 3, 3, TG_EATRATE, MAJ_FARM, 250 },
  { "sugar", 3, 2, TG_EATRATE, MAJ_FARM, 300 },
  { "fruit", 4, 1, TG_EATRATE, MAJ_FARM, 350 },
  { "honey", 4, 1, TG_EATRATE, MAJ_FARM, 350 },

  /* preservatives */
  { "pottery", 1, 6, TG_SPOILRATE, MAJ_STOCKADE, 50 },
  { "nails", 2, 5, TG_SPOILRATE, MAJ_STOCKADE, 100 },
  { "salt", 3, 3, TG_SPOILRATE, MAJ_STOCKADE, 150 },
  { "granite", 4, 2, TG_SPOILRATE, MAJ_STOCKADE, 300 },

  /* lumber */
  { "bamboo", 1, 3, TG_LUMBER, MAJ_LUMBERYARD, 10 },
  { "pine", 2, 5, TG_LUMBER, MAJ_LUMBERYARD, 15 },
  { "oak", 3, 4, TG_LUMBER, MAJ_LUMBERYARD, 30 },
  { "redwood", 5, 2, TG_LUMBER, MAJ_LUMBERYARD, 50 },
  { "mahogany", 7, 1, TG_LUMBER, MAJ_LUMBERYARD, 100 },

  /* knowledge */
  { "papyrus", 1, 9, TG_KNOWLEDGE, MAJ_TOWN, 500 },
  { "drama", 2, 7, TG_KNOWLEDGE, MAJ_TOWN, 750 },
  { "math", 3, 5, TG_KNOWLEDGE, MAJ_CITY, 1000 },
  { "library", 4, 3, TG_KNOWLEDGE, MAJ_CITY, 1250 },
  { "literature", 5, 2, TG_KNOWLEDGE, MAJ_CITY, 1500 },
  { "law", 6, 2, TG_KNOWLEDGE, MAJ_CITY, 2000 },
  { "philosophy", 6, 2, TG_KNOWLEDGE, MAJ_CITY, 2000 },

  /* farming adjustment */
  { "oxen", 1, 9, TG_FARMING, MAJ_FARM, 50 },
  { "yeomen", 1, 8, TG_FARMING, MAJ_FARM, 100 },
  { "mulch", 2, 7, TG_FARMING, MAJ_FARM, 125 },
  { "irrigation", 2, 6, TG_FARMING, MAJ_FARM, 150 },
  { "rotation", 3, 4, TG_FARMING, MAJ_FARM, 175 },
  { "plows", 4, 2, TG_FARMING, MAJ_FARM, 200 },
  { "manure", 5, 1, TG_FARMING, MAJ_FARM, 250 },

  /* health */
  { "sassafras", 1, 9, TG_HEALTH, MAJ_SHRINE, 10 },
  { "sulfa", 1, 6, TG_HEALTH, MAJ_SHRINE, 10 },
  { "poppy", 1, 4, TG_HEALTH, MAJ_SHRINE, 50 },
  { "foxglove", 2, 6, TG_HEALTH, MAJ_SHRINE, 50 },
  { "kannabis", 2, 3, TG_HEALTH, MAJ_SHRINE, 50 },
  { "bread mold", 3, 2, TG_HEALTH, MAJ_SHRINE, 75 },
  { "parsley", 3, 5, TG_HEALTH, MAJ_SHRINE, 75 },  
  { "sage", 3, 5, TG_HEALTH, MAJ_SHRINE, 75 },  
  { "rosemary", 3, 5, TG_HEALTH, MAJ_SHRINE, 75 },  
  { "thyme", 3, 5, TG_HEALTH, MAJ_SHRINE, 75 },  
  { "wolfsbane", 4, 2, TG_HEALTH, MAJ_SHRINE, 150 },

  /* terror */
  { "prison", 1, 7, TG_TERROR, MAJ_CITY, 1000 },
  { "curfews", 2, 6, TG_TERROR, MAJ_CITY, 1100 },
  { "torture", 3, 4, TG_TERROR, MAJ_CITY, 1250 },
  { "gallows", 3, 4, TG_TERROR, MAJ_CITY, 1250 },
  { "dungeon", 4, 3, TG_TERROR, MAJ_CITY, 1500 },
  { "pogrom", 6, 1, TG_TERROR, MAJ_CITY, 1750 },

  /* magical sectors */
  { "icons", 1, 9, TG_SPELLS, MAJ_SHRINE, 10 },
  { "scrolls", 1, 8, TG_SPELLS, MAJ_SHRINE, 25 },
  { "altars", 2, 7, TG_SPELLS, MAJ_SHRINE, 30 },
  { "potions", 3, 6, TG_SPELLS, MAJ_SHRINE, 35 },
  { "mirrors", 4, 5, TG_SPELLS, MAJ_SHRINE, 40 },
  { "scepter", 6, 4, TG_SPELLS, MAJ_SHRINE, 50 },
  { "tomes", 8, 3, TG_SPELLS, MAJ_SHRINE, 75 },
  { "orbs", 12, 2, TG_SPELLS, MAJ_SHRINE, 100 },
  { "rings", 16, 2, TG_SPELLS, MAJ_SHRINE, 150 },
  { "staves", 20, 1, TG_SPELLS, MAJ_SHRINE, 200 },
  { "crystals", 24, 1, TG_SPELLS, MAJ_SHRINE, 300 },

  /* metals */
  { "nickel", 1, 20, TG_METALS, MAJ_METALMINE, 10 },
  { "copper", 3, 16, TG_METALS, MAJ_METALMINE, 20 },
  { "lead", 3, 15, TG_METALS, MAJ_METALMINE, 20 },
  { "tin", 6, 12, TG_METALS, MAJ_METALMINE, 40 },
  { "bronze", 9, 8, TG_METALS, MAJ_METALMINE, 60 },
  { "iron", 12, 7, TG_METALS, MAJ_METALMINE, 75 },
  { "bauxite", 15, 6, TG_METALS, MAJ_METALMINE, 100 },
  { "steel", 18, 4, TG_METALS, MAJ_METALMINE, 150 },
  { "titanium", 24, 3, TG_METALS, MAJ_METALMINE, 200 },
  { "mithril", 30, 2, TG_METALS, MAJ_METALMINE, 250 },
  { "iridium", 36, 1, TG_METALS, MAJ_METALMINE, 300 },
  { "adamantine", 42, 1, TG_METALS, MAJ_METALMINE, 400 },

  /* jewels / rare metals */
  { "quartz", 1, 16, TG_JEWELS, MAJ_JEWELMINE, 10 },
  { "jade", 3, 11, TG_JEWELS, MAJ_JEWELMINE, 20 },
  { "turquoise", 3, 11, TG_JEWELS, MAJ_JEWELMINE, 20 },
  { "marble", 6, 10, TG_JEWELS, MAJ_JEWELMINE, 30 },
  { "sapphires", 6, 10, TG_JEWELS, MAJ_JEWELMINE, 30 },
  { "garnet", 9, 9, TG_JEWELS, MAJ_JEWELMINE, 50 },
  { "emeralds", 9, 8, TG_JEWELS, MAJ_JEWELMINE, 50 },
  { "corundum", 12, 7, TG_JEWELS, MAJ_JEWELMINE, 75 },
  { "silver", 15, 6, TG_JEWELS, MAJ_JEWELMINE, 100 },
  { "tourmaline", 18, 5, TG_JEWELS, MAJ_JEWELMINE, 150 },
  { "gold", 21, 4, TG_JEWELS, MAJ_JEWELMINE, 200 },
  { "opals", 24, 3, TG_JEWELS, MAJ_JEWELMINE, 250 },
  { "rubies", 27, 2, TG_JEWELS, MAJ_JEWELMINE, 300 },
  { "diamonds", 36, 1, TG_JEWELS, MAJ_JEWELMINE, 350 },
  { "platinum", 42, 1, TG_JEWELS, MAJ_JEWELMINE, 400 }
};

/* names of seasons */
char *seasonstr[SEASON_NUMBER] = { "Spring", "Summer", "Fall", "Winter" };

/* names of months -- Roman calendar before Julius Caesar */
char *monthstr[] = { 
  "Martius", "Aprillis", "Maius", "Junius", "Quintilis", "Sextilis",
  "Septembre", "Octobre", "Novembre", "Decembre", "Januarius",
  "Februarius"
};

/* list of available options -- defines in optionsX.h */
char *opt_list[] = {
  "all-blanks", "bind-key", "bottom-lines", "check-keys", "expert",
  "gaudy", "header-mode", "info-mode", "mail-check", "pager-offset",
  "pager-scroll", "pager-tab", "read-opts", "rebind-key",
  "recenter-map", "reset-keys", "store-opts", "supply-level",
  "terminal-bell", "unbind-key", "water-bottoms"
};

/* list of help files --
 *   do NOT include ".doc" as it will be appended automatically
 *   first character must be a unique upper case
 */
char *help_files[] = {
  "Helpfiles", "Overview", "Functions", "Display", "Sectors",
  "Nations", "Economics", "Movement", "Warfare", "Powers",
  "Xchanges", "Beginnings", "God-Powers", "Customize", "Quickies",
  "Version"
};

/* diplomacy status names -- matching defines in dstatusX.h */
char *dipname[] = {
  "Unmet", "Allied", "Treaty", "Friendly", "Peaceful", "Neutral",
  "Hostile", "Bellicose", "War", "Jihad"
};

/* miscellaneous descriptions */
char rnumerals[] = "IVXLCDMIVXLCDM";
char *aggressname[] = {
  "Nomove", "Static", "Enforce", "Overt", "Mobile", "Killer"
};
char *speedname[] = {
  "Slow", "Norm", "Fast", "None", "Patrol"
};
char *alignment[] = {
  "Other", "Good", "Neutral", "Evil", "Other"
};
char *zooms[ZOOM_NUMBER] = {
  "detail", "medium", "full"
};
char *hex_list[HXPOS_NUMBER] = {
  "low-left", "low-right", "up-left", "up-right"
};

/* the mail string formats */
char rmail_quote_prefix[LINELTH] = "> ";
char rmail_from_format[LINELTH] = "From $s ($S) $d:";

/* global options and their initial settings */
int conq_supply_level = 0;
int conq_allblanks = FALSE;
int conq_mheaders = FALSE;
int conq_beeper = TRUE;
int conq_bottomlines = FALSE;
int conq_waterbottoms = FALSE;
int conq_gaudy = FALSE;
int conq_expert = FALSE;
int conq_infomode = FALSE;
int zoom_level = ZOOM_DETAIL;
int conq_mercsused = 0;

/* pager settings */
int pager_tab = D_PAGETAB;
int pager_scroll = 0;
int pager_offset =D_PAGEOFF;

/* miscellaneous file name constants */
char *exetag = "exc";
char *datafile = "Data";
char *msgtag = "msg";
char *motdfile = "Motd";
char *blockfile = "Nologin";
char *timefile = "Timelog";
char *rulesfile = "rules";
char *npcfile = "nations";
char *newsfile = "news";
char *tmptag = "tmp";
char *isontag = "lck";
#ifdef TRADE
char *tradefile = "commerce";
#endif /* TRADE */

/* sizing values */
int help_number = (sizeof(help_files)/sizeof(char *));
int options_number = (sizeof(opt_list)/sizeof(char *));
int tgoods_number = (sizeof(tg_info)/sizeof(TGOOD_STRUCT));
int tgclass_number = (sizeof(tgclass_info)/sizeof(TGCLASS_STRUCT));
int nclass_number = (sizeof(nclass_list)/sizeof(NCLASS_STRUCT));
int dstatus_number = (sizeof(dipname)/sizeof(char *));

/* miscellaneous declarations */
FILE *fexe, *fnews = NULL, *fm, *fupdate = NULL;
DMODE_PTR dmode_list = NULL, dmode_tptr;
char string[BIGLTH], nationname[BIGLTH], datadirname[BIGLTH];
char defaultdir[FILELTH+1], loginname[NAMELTH+1], datadir[FILELTH+1];
char prog_name[FILELTH+1], helpdir[FILELTH+1], progdir[FILELTH+1];
long *visibility_data = NULL;
short *sum_weights = NULL;
int global_int, country, no_input, owneruid, adjust_made = TRUE;
int adjust_xloc = -1, adjust_yloc = -1;
int in_curses = FALSE, need_hangup = FALSE, is_update, is_god;
int movemode, xcurs, ycurs, xoffset = 0, yoffset = 0;
#ifdef SYSMAIL
int dosysm_check = TRUE;
#else
int dosysm_check = FALSE;
#endif /* SYSMAIL */
long global_long;
SCT_PTR sct_ptr, sct_tptr;
ARMY_PTR army_ptr, army_tptr;
NTN_PTR ntn_ptr, ntn_tptr;
NAVY_PTR navy_ptr, navy_tptr;
CITY_PTR city_ptr, city_tptr;
ITEM_PTR item_ptr, item_tptr;
CVN_PTR cvn_ptr, cvn_tptr;
SCT_STRUCT **sct;
struct s_world world;
