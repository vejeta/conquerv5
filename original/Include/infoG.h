/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/* Include file for the different information types */

/* Information Screen Definitions */
#define INFO_MAX	7
typedef enum infotype {
  INFO_ARMY, INFO_GRPARM, INFO_NAVY, INFO_CVN, INFO_CITY, INFO_ITEM,
  INFO_NTN, INFO_DIP
} Infotype;

/* City Information Screen Modes */
typedef enum cinfotype {
  CINFO_RESOURCE, CINFO_ECONOMY, CINFO_SUMMARY
} Cinfotype;
