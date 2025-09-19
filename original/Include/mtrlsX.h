/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/* This include file defines the raw materials */

/* The hard coded list of raw materials */
#define MTRLS_TALONS	0
#define MTRLS_JEWELS	1
#define MTRLS_METALS	2
#define MTRLS_FOOD	3
#define MTRLS_WOOD	4
/* MTRLS_NUMBER in dataX.h */

/* The information structure */
typedef struct s_mtrls {
  char *name;		/* The normal name of this raw material */
  char *lname;		/* The all lowercase version of the name */
  int weight;		/* The weight of each unit of this material */
  int transval;		/* The relative transmutation value of material */
} MTRLS_STRUCT, *MTRLS_PTR;

/* The variables used to hold the information */
extern MTRLS_STRUCT mtrls_info[MTRLS_NUMBER];
