/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/* This file handles the prototyping information */

/* Has it been included already? */
#ifndef PARM_0

/* Prototyping Setup */
#ifdef __STDC__

#define PARM_0(a) (a)
#define PARM_1(a,b) (a b)
#define PARM_2(a,b,c,d) (a b, c d)
#define PARM_3(a,b,c,d,e,f) (a b, c d, e f)
#define PARM_4(a,b,c,d,e,f,g,h) (a b, c d, e f, g h)
#define PARM_5(a,b,c,d,e,f,g,h,i,j) (a b, c d, e f, g h, i j)

#else

#define PARM_0(a) ()
#define PARM_1(a,b) (b) a b;
#define PARM_2(a,b,c,d) (b, d) a b; c d;
#define PARM_3(a,b,c,d,e,f) (b, d, f) a b; c d; e f;
#define PARM_4(a,b,c,d,e,f,g,h) (b, d, f, h) a b; c d; e f; g h;
#define PARM_5(a,b,c,d,e,f,g,h,i,j) (b, d, f, h, j) a b; c d; e f; g h; i j;

#endif /*__STDC__*/

#endif /* PARM_0 */
