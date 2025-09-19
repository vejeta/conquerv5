/* This file declares global data for the user interface */
/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * A good deal of time and effort has gone into the writing of this
 * code and it is our hope that you respect this.  We give permission
 * to alter the code, but not to redistribute modified versions of the
 * code without our explicit permission.  If you do modify the code,
 * please document the changes made, and send us a copy, so that all
 * people may have it.  The code, to the best of our knowledge, works
 * well, but there will probably always be a need for bug fixes and
 * improvements.  We disclaim any responsibility for the codes'
 * actions.  [Use at your own risk].  This notice is just our way of
 * saying, "Happy gaming!", while making an effort to not get sued in
 * the process.
 *                           Ed Barlow, Adam Bryant
 */

/* Include the global definitions file */
#include "dataX.h"
#include "keybindG.h"

/* ------------------------------------------------------------------ */
/*								      */
/*     This file is to be included by all files composing only        */
/*   the Conquer User Interface (conquer).			      */
/*     Only those definitions used within all of the conquer files    */
/*   and only in the conquer files should be set here.		      */
/*								      */
/* ------------------------------------------------------------------ */

/* number of command keys parsed */
#define MAXCOMMANDS	255

/* number of lines and width for pager help */
#define MAX_PAGERHELP	21
#define WIDTH_PAGERHELP	42

/* Structure for file perusal */
typedef struct s_line {
  char *line;
  uns_char highlight;
} LINE_STRUCT;

/* Mail system statuses */
#define MAIL_DONE	0
#define MAIL_READING	1
#define MAIL_SENDING	2

/* indicator of mail editing mode */
#define EM_M_NORMAL	0x00
#define EM_M_INSERTON	0x04
#define EM_M_VISTYLE	0x08

/* Everything past this point is not needed in dataG.c */
#ifndef DATA_DECLARE

/* Constants for Screen Redisplay */
#define DRAW_DONE	0
#define DRAW_FULL	1
#define DRAW_NOFRILLS	2

/* Jump routine notation */
#define JUMP_CAP	0
#define JUMP_PICK	1
#define JUMP_SAVE	2

/* Onto the generic macros and definitions */

/* Mail status indicators */
#define STMAIL_NEW	0
#define STMAIL_NONE	1

/* Generic Constants */
#define SCREEN_SLIM	(conq_infomode?(LINES - 5):(LINES - 14))
#define SCREEN_SIDE	(SCREEN_SLIM / 2)

/* file: dataG.c -- global data for user interface */
extern int xloc_mark, yloc_mark;
extern int lockfilenum, mail_ind, redraw;
extern char fison[], conqmail[], *dflt_motd[MOTDLINES];
extern itemtype command_cost;
extern int email_mode, pager, selector, city_mode;
extern int conq_news_status, conq_mail_status, god_browsing;
#ifdef SYSMAIL
extern int sys_mail_status;
extern char sysmail[];
#endif /*SYSMAIL*/
extern int cv_max[];
extern char *display_list[], *highl_list[], *cv_name[], *desg_selects[];
extern char *shortspeed[], *shortdir[];
extern char *shipsize[], *min_desg_selects[];
extern char *sectstat, *trooplocs;
extern DISPLAY_STRUCT display_mode;

#ifdef REGEXP
/* system function declarations */
extern int re_exec();
extern char *re_comp();
#endif /* REGEXP */

/* call in the function prototypes file */
#ifndef __CEXTRACT__
#include "fileG.h"
#endif /* __CEXTRACT__ */

#endif /*DATA_DECLARE*/
