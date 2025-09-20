// SPDX-License-Identifier: GPL-3.0-or-later
/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/* definitions for the data file input / output routines */

/* structure definition */
typedef struct s_dioinfo {
  char *name;		/* the name of the element */
  int size;		/* the size (in bytes?) of the element */
} DIO_STRUCT, *DIO_PTR;

/* type size definitions  -- NEVER CHANGE THESE */
#define DIO_BITS	0
#define DIO_CHAR	1
#define DIO_UNSCHAR	2
#define DIO_SHORT	3
#define DIO_UNSSHORT	4
#define DIO_INT		5
#define DIO_LONG	6
#define DIO_DOUBLE	7
#define DIO_IDTYPE	8
#define DIO_MAPTYPE	9
#define DIO_ITEMTYPE	10
#define DIO_NTNTYPE	11
#define DIO_NUMBER	20	/* allow some extra space */

/* struct size definitions  -- NEVER CHANGE THESE */
#define DIOS_WORLD	0
#define DIOS_SECTOR	1
#define DIOS_NATION	2
#define DIOS_CITY	3
#define DIOS_ARMY	4
#define DIOS_NAVY	5
#define DIOS_CARAVAN	6
#define DIOS_NUMBER	15	/* allow some extra space */
