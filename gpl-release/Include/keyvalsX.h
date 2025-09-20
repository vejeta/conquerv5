// SPDX-License-Identifier: GPL-3.0-or-later
/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/* This include file handles the list of key constants */

/* ascii control key definitions */
#define EXT_IGN	'\377'
#define EXT_ESC	'\033'
#define EXT_NUL	'\000'
#define EXT_DEL	'\177'
#define CNTRL_A '\001'
#define CNTRL_B '\002'
#define CNTRL_C '\003'
#define CNTRL_D '\004'
#define CNTRL_F '\006'
#define CNTRL_G '\007'
#define CNTRL_H '\010'
#define CNTRL_J '\012'
#define CNTRL_K '\013'
#define CNTRL_L '\014'
#define CNTRL_N '\016'
#define CNTRL_O '\017'
#define CNTRL_R '\022'
#define CNTRL_U '\025'
#define CNTRL_V '\026'
#define CNTRL_X '\030'
#define CNTRL_Y '\031'
#define CNTRL_Z '\032'

/* simple strings for above keys in bindings */
#define ESTR_IGN "\377"
#define ESTR_ESC "\033"
#define ESTR_NUL ""
#define ESTR_DEL "\177"
#define CSTR_A "\001"
#define CSTR_B "\002"
#define CSTR_C "\003"
#define CSTR_D "\004"
#define CSTR_E "\005"
#define CSTR_F "\006"
#define CSTR_G "\007"
#define CSTR_H "\010"
#define CSTR_J "\012"
#define CSTR_K "\013"
#define CSTR_L "\014"
#define CSTR_N "\016"
#define CSTR_O "\017"
#define CSTR_P "\020"
#define CSTR_R "\022"
#define CSTR_T "\024"
#define CSTR_U "\025"
#define CSTR_V "\026"
#define CSTR_W "\027"
#define CSTR_X "\030"
#define CSTR_Y "\031"
#define CSTR_Z "\032"

/* bindings for the arrow keys */
#define AKEY_UP1	"\033[A"
#define AKEY_DOWN1	"\033[B"
#define AKEY_RIGHT1	"\033[C"
#define AKEY_LEFT1	"\033[D"
#define AKEY_UP2	"\033OA"
#define AKEY_DOWN2	"\033OB"
#define AKEY_RIGHT2	"\033OC"
#define AKEY_LEFT2	"\033OD"
