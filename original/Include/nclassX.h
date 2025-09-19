/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/* This include file defines all of the nation classes */

/* the default national class number */
#define NC_DEFAULT	1

/* The data structure to handle national class information */
typedef struct s_nclass {
  char *name;		/* the name of the class */
  char *rulertype;	/* the name of the national ruler type */
  char *minleadtype;	/* the name of the minor leader type */
  char *racetype;	/* the list of possible races */
  char *powname;	/* textual list of powers given to class */
  int cost;		/* additional point cost to gain the class */
  int leadnum;		/* the number of beginning leaders */
  long pow_given[MAG_NUMBER];	/* list of powers given to the class */
} NCLASS_STRUCT, *NCLASS_PTR;

/* the information structure of the class types */
extern NCLASS_STRUCT nclass_list[];
