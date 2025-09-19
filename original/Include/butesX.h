/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/* This file contains definitions for the national attributes */
#ifndef __butesX_h__
#define __butesX_h__

/* The list of national attributes -- BUTE_NUMBER is in dataX.h */
typedef enum butetype {
  BUTE_CHARITY, BUTE_COMMRANGE, BUTE_CURRENCY, BUTE_EATRATE,
  BUTE_HEALTH, BUTE_INFLATION, BUTE_JEWELWORK, BUTE_KNOWLEDGE,
  BUTE_MERCREP, BUTE_METALWORK, BUTE_MINING, BUTE_MORALE,
  BUTE_POPULARITY, BUTE_REPUTATION, BUTE_SPELLPTS, BUTE_SPOILRATE,
  BUTE_TAXRATE, BUTE_TERROR, BUTE_WIZSKILL
} Butetype;

/* The informational structure for the attributes */
typedef struct s_bute {
  char *name;		/* The name of the national attribute */
  char *description;	/* A description of the national attribute */
  short start_val;	/* Starting value for the attribute */
  short min_base;	/* Low value for the attribute */
  short max_base;	/* High value for the attribute */
  short divisor;	/* To obtain floating point integration */
  short modifier;	/* Adjustment when positively influenced */
  short clear_it;	/* Indicates a need to clear it each update */
} BUTE_STRUCT, *BUTE_PTR;

/* The declaration of the attribute information structure */
extern BUTE_STRUCT bute_info[BUTE_NUMBER];

#endif /* __butesX_h__ */
