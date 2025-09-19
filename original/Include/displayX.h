/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/* This file declares the data structure for display operations */

/* Possible display modes */
#define DI_BLANK	0
#define DI_CONT		1
#define DI_DESG		2
#define DI_FOOD		3
#define DI_JEWEL	4
#define DI_METAL	5
#define DI_NATN		6
#define DI_PEOP		7
#define DI_RACE		8
#define DI_VEGE		9
#define DI_WOOD		10
#define DI_YDESG	11
#define DI_AMOVE	12
#define DI_DEFENSE	13
#define DI_FMOVE	14
#define DI_MAGIC	15
#define DI_NMOVE	16
#define DI_TGDESGS	17
#define DI_VALUES	18
#define DI_WEIGHTS	19
#define DI_KEEP		20
#define DI_MAXIMUM	20

/* Possible highlight modes */
#define HI_NONE		0
#define HI_ALLIED	1
#define HI_MAJDESG	2
#define HI_ENEMY	3
#define HI_MINDESG	4
#define HI_NEUTRAL	5
#define HI_OWN		6
#define HI_RANGE	7
#define HI_SCOUT	8
#define HI_UNITS	9
#define HI_YUNITS	10
#define HI_MOVEABLE	11
#define HI_REGION	12
#define HI_SUPPORTED	13
#define HI_TGOODS	14
#define HI_UNSUPPORTED	15
#define HI_KEEP		16
#define HI_MAXIMUM	16

/* hexmap positioning modes */
#define HXPOS_MINIMAL	-1
#define HXPOS_LOWLEFT	0
#define HXPOS_LOWRIGHT	1
#define HXPOS_UPLEFT	2
#define HXPOS_UPRIGHT	3

/* zoom selections */
#define ZOOM_DETAIL	0
#define ZOOM_MEDIUM	1
#define ZOOM_LARGE	2
/* ZOOM_NUMBER set in dataX.h */
