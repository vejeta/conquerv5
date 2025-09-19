/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/* Mechanisms to transfer items from one storage site to another */

/* the possible transfer sites */
#define XFER_CITY	0
#define XFER_NAVY	1
#define XFER_CARAVAN	2
#define XFER_SECTOR	3

/* unlimited transfer of this selection */
#define XF_NOLIMIT	-1
#define XF_JOINED	-2

/* positioning constants */
#define XF_INDENT	11
#define XF_SPACING	8
#define XF_USEDLINES	10

/* structure to hold goods during transfer */
typedef struct s_xfer {
  char description[LINELTH];	/* the description of the site */
  uns_char site_class;		/* is this a city, navy, or caravan? */
  maptype xloc, yloc;		/* the position of the site */
  itemtype mtrls[MTRLS_NUMBER];	/* the materials carried */
  long civies;			/* civilian being transfered */
  long crews;			/* crew being transfered */
  idtype onb_id;		/* id number of the armies on board */
  idtype onb_cid;		/* id number of the caravans on board */
  long mtrl_max;		/* max weight of the materials */
  long civ_max;			/* max weight of the civilians */
  long onb_max;			/* max weight of the troops */
  long onb_cmax;		/* max weight of the carvans */
  long crew_max;		/* max weight of the crew */
  int civ_div;			/* atomic transfer size for civilians */
  int crew_div;			/* atomic transfer size for crew */
} XFER_STRUCT, *XFER_PTR;
