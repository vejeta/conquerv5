/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/* This file contains definitions for elevation and vegetation information */

/* Simple Contour Map Definitions */
#define ELE_WATER	0
#define ELE_VALLEY	1
#define ELE_CLEAR	2
#define ELE_HILL	3
#define ELE_MOUNTAIN	4
#define ELE_PEAK	5
/* ELE_NUMBER defined in dataX.h */

/* Vegetation Types */
#define VEG_VOLCANO	0
#define VEG_DESERT	1
#define VEG_TUNDRA	2
#define VEG_BARREN	3
#define VEG_LT_VEG	4
#define VEG_GOOD	5
#define VEG_WOOD	6
#define VEG_FOREST	7
#define VEG_JUNGLE	8
#define VEG_SWAMP	9
#define VEG_ICE		10
#define VEG_NONE	11
/* VEG_NUMBER defined in dataX.h */

/* Structure information concerning vegetation and elevation */
typedef struct s_eleveg {
  char *name;			/* The name of the eleveg */
  char symbol;			/* The map representation of the eleveg */
  int food_val;			/* The food value of the eleveg */
  int wood_val;			/* The wood value of the eleveg */
  int flight_cost;		/* The cost to fly into the sector */
  int move_cost[RACE_NUMBER];	/* how easy/hard to enter sector */
  int exposure[SEASON_NUMBER];	/* Seasonal exposure values */
  int attract[RACE_NUMBER];	/* how attractive to various races */
} ELEVEG_STRUCT, *ELEVEG_PTR;

/* The information structures */
extern ELEVEG_STRUCT veg_info[VEG_NUMBER];
extern ELEVEG_STRUCT ele_info[ELE_NUMBER];
