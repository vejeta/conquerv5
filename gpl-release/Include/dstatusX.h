// SPDX-License-Identifier: GPL-3.0-or-later
/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/* The diplomacy status definitions for the nations -- strings in dataX.c */
typedef enum diplotype {
  DIP_UNMET, DIP_ALLIED, DIP_TREATY, DIP_FRIENDLY, DIP_PEACEFUL,
  DIP_NEUTRAL, DIP_HOSTILE, DIP_BELLICOSE, DIP_WAR, DIP_JIHAD
} Diplotype;

/* string containing the diplomacy status */
extern char *dipname[];
extern int dstatus_number;
