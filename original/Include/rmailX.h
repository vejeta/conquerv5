/* header for data structures concerning read in mail messages */
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

/* THIS FILE IS ONLY NEEDED IF DIRECT ACCESS TO
   READING OR WRITING MAIL MESSAGES IS NECESSARY */

/* limit on number of targets for a mail message */
#define MAX_ADR	20	/* max number of nations for simultaneous mail */

/* the various status values of mail */
#define MSTAT_NONE	0	/* clean status */
#define MSTAT_READ	0x0001	/* the mail message has been read before */
#define MSTAT_DELETED	0x0002	/* marked as deleted */
#define MSTAT_REPLIED	0x0004	/* a reply has been sent */
#define MSTAT_FORWARDED	0x0008	/* the message has been forwarded */

typedef struct s_maildata {
  char *line_data;		/* a line of text of a message	*/
  struct s_maildata *next;	/* next line in the message	*/
  struct s_maildata *prev;	/* previous line in the message	*/
} MAILD_STRUCT, *MAILD_PTR;

typedef struct s_rmail {
  char *sender;			/* who send it */
  char *nickname;		/* how else the sender is known */
  int to_whom[MAX_ADR];	/* nations to be included in the message */
  char *date;			/* when it was sent */
  char *subj;			/* what the message is about */
  int status;			/* status indicators */
  MAILD_PTR text;		/* the text of the message */
  struct s_rmail *next;		/* the next message */
  struct s_rmail *prev;		/* the previous message */
} RMAIL_STRUCT, *RMAIL_PTR;

/* The data storage */
extern RMAIL_PTR cur_message;
