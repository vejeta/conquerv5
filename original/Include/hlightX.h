/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/* This file contains the mechanisms to implement the highlights and */
/* sector visibility [old HAS_SEEN code] functionality.              */

/* quickie to reference it */
#define _VD_(x,y)	visibility_data[(x) + MAPX * (y)]

/* the constants for checking */
#define SCTR_SKIP	4
#define SCTR_MASK	0xFFFFFFF0
#define SCTR_NMASK	0x0000000F

/* the new HAS_SEEN stuff */
#define VIS_CHECK(x,y)	(_VD_(x,y) & SCTR_NMASK)
#define VIS_STORE(x,y,N)	_VD_(x,y) = ((_VD_(x,y) & SCTR_MASK) | N)

/* the new highlighting checks */
#define HIGH_CHECK(x,y,T)	(_VD_(x,y) & (1 << (T + SCTR_SKIP)))
#define HIGH_SETON(x,y,T)	_VD_(x,y) |= (1 << (T + SCTR_SKIP))
#define HIGH_SETOFF(x,y,T)	_VD_(x,y) &= ~(1 << (T + SCTR_SKIP))

/* Constants for Sector Visibility */
#define HS_NOSEE	0
#define HS_SEEPART	1
#define HS_SEEMOST	2
#define HS_SEEFULL	3
#define HS_SEEALL	4

#ifdef NOTDONE

/* the list of the new highlighting schemes */
/* ABS maximum of 28 schemes -- move to displayX.h later */
#define HI_POPLEVEL	1
#define HI_DIPLOMACY	2
#define HI_MAJDESG	3
#define HI_MINDESG	4
#define HI_VEGETATION	5
#define HI_ELEVATION	6
#define HI_TRADEGOOD	7
#define HI_OWNEDBY	8
#define HI_INRANGE	9
#define HI_SUPPORTED	10
#define HI_MOVEMENT	11
#define HI_NTNARMY	12
#define HI_NTNNAVY	13
#define HI_NTNCVN	14
#define HI_UTYPE	15
#define HI_UCLASS	16
#define HI_USTATUS	17
#define HI_USIZE	18
#define HI_NUMBER	19
#endif /* NOTDONE */

/* define the variables to be used */
extern long *visibility_data;
