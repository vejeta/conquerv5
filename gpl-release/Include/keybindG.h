// SPDX-License-Identifier: GPL-3.0-or-later
/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/* file for keybinding settings */

/* has this been used before? */
#ifndef MAXKEYS

/* maximum number of keys for a binding */
#define MAXKEYS	10

/* structure to hold keybindings */
typedef struct s_keybind {
  char chlist[MAXKEYS + 1];	/* characters for this binding		*/
  FNCI func;			/* what function the key is bound to	*/
} KBIND_STRUCT, *KBIND_PTR;

/* actual keybinding list */
typedef struct s_keylist {
  KBIND_STRUCT key_data;	/* function and key binding	*/
  struct s_keylist *next;	/* access to the next binding	*/
} KLIST_STRUCT, *KLIST_PTR;

/* function storage */
typedef struct s_parse {
  FNCI func;		/* actual function		*/
  char *realname;	/* name associated w/function	*/
  char *descript;	/* description of the function	*/
} PARSE_STRUCT, *PARSE_PTR;

/* information about keybindings */
typedef struct s_keysys {
  char *name;		/* the name of this key binding system */
  PARSE_PTR parse_p;	/* pointer to the list of system functions */
  KBIND_PTR kbind_p;	/* pointer to the list of default bindings */
  int num_parse;	/* number of entries in the function list */
  int num_binds;	/* number of entries in the key binding list */
} KEYSYS_STRUCT, *KEYSYS_PTR;

/* declarations of the keybindings */
extern KLIST_PTR email_bindings;
extern KLIST_PTR reader_bindings;
extern KLIST_PTR mg_bindings;
extern KLIST_PTR ni_bindings;
extern KLIST_PTR xfer_bindings;
extern KLIST_PTR mparse_bindings;
extern KLIST_PTR cq_bindings;

/* key system information */
extern KEYSYS_STRUCT global_keysys;
extern KEYSYS_STRUCT email_keysys;
extern KEYSYS_STRUCT reader_keysys;
extern KEYSYS_STRUCT magic_keysys;
extern KEYSYS_STRUCT mparse_keysys;
extern KEYSYS_STRUCT xfer_keysys;
extern KEYSYS_STRUCT ninfo_keysys;

#endif /* MAX_KEYS */
