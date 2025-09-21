// SPDX-License-Identifier: GPL-3.0-or-later
/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/* Header file for all of the definitions of world creation */

/* world configuration entry list */
#define BLD_DEMIGOD	0
#define BLD_PASSWD	1
#define BLD_ADDPWD	2
#define BLD_DEMILIMIT	3
#define BLD_DEMIBUILD	4
#define BLD_MAPTYPE	5
#define BLD_RELATIVE	6
#define BLD_VRFYLOGIN	7
#define BLD_HIDESCORES	8
#define BLD_HIDELOGIN	9
#define BLD_LATESTART	10
#define BLD_XSIZE	11
#define BLD_YSIZE	12
#define BLD_POINTS	13
#define BLD_BUILDDIST	14
#define BLD_FLEETCBVAL	15
#define BLD_WAGONCBVAL	16
#define BLD_DICE	17
#define BLD_AVGDAM	18
#define BLD_DAMLIM	19
#define BLD_OVERMATCH	20
#define BLD_PMINDAM	21
#define BLD_YEAR	22
#define BLD_MONTH	23
#define BLD_PWATER	24
#define BLD_PMOUNT	25
#define BLD_EXPOSURE	26
#define BLD_SMOOTH	27
#define BLD_LRANGE	28
#define BLD_PREJLAND	29
#define BLD_PTRADE	30
#define BLD_PTGMETAL	31
#define BLD_PTGJEWEL	32
#define BLD_PTGSPELL	33
#define BLD_GROWTH	34
#define BLD_NPCS	35
#define BLD_PMONSTER	36
#define BLD_LIZARD	37
#define BLD_SAVAGE	38
#define BLD_NOMAD	39
#define BLD_PIRATE	40
#define BLD_PREVOLT	41
#define BLD_PSCOUT	42
#define BLD_NUMRANDOM	43
#define BLD_NUMWEATHER	44
#define BLD_MERCMEN	45
#define BLD_MERCATT	46
#define BLD_MERCDEF	47
#define BLD_PMERCVAL	48
#define BLD_PMERCMONST	49
#define BLD_MAXDIPADJ	50
#define BLD_BRIBE	51
#define BLD_MAXSUPPLIES	52
#define BLD_NVSPLYDIST	53
#define BLD_CITYDIST	54
#define BLD_OPTIONS	55

/* other build routine specifications */
#define BLD_START	2
#define MAXX		(MAPX / 8)
#define MAXY		(MAPY / 8)
#define NUMAREAS	(MAXX * MAXY)
#define NUMSECTS	(MAPX * MAPY)

/* composition of sectors */
#define WORLD_WATER	1
#define WORLD_HALF	2
#define WORLD_LAND	3

/* === Default Percentage and Value Settings for the World === */
#define DFLT_MAXPTS 80		/* default points for nation building	*/
#define DFLT_NUMDICE 9		/* the default number of combat dice	*/
#define DFLT_AVGDAMAGE 50	/* the default average combat damage	*/
#define DFLT_PMINDAMAGE 5	/* default % of roll for min damage	*/
#define DFLT_FLEETCBVAL	-25	/* default combat bonus of naval units	*/
#define DFLT_WAGONCBVAL	-50	/* default combat bonus of caravans	*/
#define DFLT_DAMLIMIT 50	/* default damage needed to stop attack	*/
#define DFLT_OVERMATCH 10	/* default damage adjust for overmatch	*/
#define DFLT_XSIZE 72		/* default horizontal map size		*/
#define DFLT_YSIZE 120		/* default vertical map size		*/
#define DFLT_BUILDDIST 10	/* default min distance between nations	*/
#define DFLT_SMOOTH 6		/* how many times to perform smoothing	*/
#define DFLT_LRANGE 2		/* range of 8 sector blocks to check	*/
#define DFLT_PREJLAND 100	/* % chance to reject checked sector.	*/
#define DFLT_PWATER 50		/* percent of the world which is water	*/
#define DFLT_PMOUNT 70		/* percent of land which is mountains	*/
#define DFLT_EXPOSURE 10	/* "1.0" ratio for unit exposure	*/
#define DFLT_GROWTH 10		/* normal rate of growth by default	*/ 
#define DFLT_PTRADE 80		/* percent of land with trade goods	*/
#define DFLT_PTGMETAL 25	/* percent of tgoods which are metals	*/
#define DFLT_PTGJEWEL 25	/* percent of tgoods which are jewels	*/
#define DFLT_PTGSPELL 15	/* percent of tgoods which are magical	*/
#define DFLT_PREVOLT 25		/* revolt percentage modifier		*/
#define DFLT_NUMRANDOM 5	/* number of random events per turn	*/
#define DFLT_NUMWEATHER 4	/* number of weather events per turn	*/
#define DFLT_PSCOUT 25		/* percent for scout capture		*/
#define DFLT_NPCS 8		/* number of non player characters	*/
#define DFLT_MAXDIPADJ 2	/* maximum slots a dstatus may change	*/
#define DFLT_LATESTART 5	/* turns after which password is needed	*/
#define DFLT_BRIBE 200000L	/* bribe base for nation adjustment	*/
#define DFLT_MERCMEN (500*ABSMAXNTN)	/* number of initial mercs	*/
#define DFLT_MERCATT 40		/* starting mercenary attack bonus	*/
#define DFLT_MERCDEF 40		/* starting mercenary defense bonus	*/
#define DFLT_PMERCVAL 15	/* % of disbanded troops going mercs	*/
#define DFLT_PMERCMONST 6	/* % of monsters who become mercs	*/
#define DFLT_PMONSTER 12	/* % reproduction for monster nations	*/
#define DFLT_LIZARD 20		/* number of lizard forts at start	*/
#define DFLT_SAVAGE 40		/* number of savage infantry and monst	*/
#define DFLT_NOMAD 30		/* number of nomad armies at start	*/
#define DFLT_PIRATE 15		/* number of pirate bases at start	*/
#define DFLT_SUPPLIES 2		/* maximum amount of supplies carryable	*/
#define DFLT_NVSPLYDIST 1	/* range which nvy and cvn supply	*/
#define DFLT_CITYXFDIST 2	/* max range for xfers between cities	*/
