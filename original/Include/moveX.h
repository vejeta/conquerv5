/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/* This file describes various movement options */

/* Types of Movement */
typedef enum movetype {
  MOVE_CANCEL = (-2), MOVE_NOMOVE, MOVE_ARMY, MOVE_NAVY, MOVE_CVN,
  MOVE_PEOPLE, MOVE_TELEPORT, MOVE_FLYARMY, MOVE_FLYCVN, MOVE_PATROL,
  MOVE_ONEWAY
} Movetype;

/* Codes for Conditional Movement */
typedef enum mverrtype {
  MV_ACTOFWAR = (-18), MV_HOSTILE, MV_ONEWAY, MV_BADBRIDGE, MV_UNSEEN,
  MV_DRAG, MV_OTHNATION, MV_NOACCESS, MV_HOSTILESCOUT, MV_UNMET,
  MV_NOTWALL, MV_TOOFAR, MV_LANDING, MV_INHABITABLE, MV_WATER,
  MV_GROUND, MV_IMPASSABLE, MV_OFFMAP
} Mverrtype;

/* Movement selection indicators */
#define MOVEIND_NORM	0
#define MOVEIND_FLY	1
#define MOVEIND_WATER	2
