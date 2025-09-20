#	conquer: Copyright (c) 1991 by Edward M Barlow and Adam Bryant
#		 All Rights Reserved
#
#	The latest release and patches for conquer are available
#	via anonymous ftp at cs.bu.edu (128.197.2.1) in the
#	directory "conquer".  Beta code is contained in the
#	sub-directory "beta".
#
#	BY CHANGING THIS FILE, YOU AGREE TO ABIDE BY THE
#	LIMITATIONS STATED IN THE LIMITED USE CONTRACT
#	CONTAINED IN THE FILE "header.h".
#
#	Make sure to set your desired configuration by editing
#	the file "header.h", within the Include directory.
#
#	To configure all of the Makefiles, first copy this file to
#	Makefile and then type "make Makefiles".
#
#	Make sure to also check the Makefiles within each of the
#	subdirectories to be sure that everything is properly
#	configured.
#
#	The following variables are all passed on from this Makefile
#	by the above command:
#
#		MAKE	- the make command
#		MKDPND	- the make depend command
#		MKDIR	- the command to create a new directory
#		SHELL	- the shell used when running the make
#		CD	- command to change to a different directory
#		CC	- the compiler
#		RM	- the program to remove files
#		CP	- command to copy files
#		MV	- command to relocate files
#		ECHO	- simple command to print arguments literally
#		STRIP	- "strip" command used to remove symbol table
#		TOUCH	- command to create empty file or update file time
#		NULL	- place to send useless error messages
#		CHOWN	- command to adjust file ownership
#		CHMOD	- command to adjust file permissions
#		LIBS	- various libraries used when compiling conquer
#		SYSFLG	- *important* flags used for system definition
#		CFLAGS	- the list of compiler options
#		CKFLAGS	- special flags for the checkX.o compilation
#		TOPDIR	- full path to this directory
#		DATADIR	- full path of the default data directory
#		BINDIR	- full path where executables will be placed
#		INCDIR	- the name of the include directory
#		SRCDIR	- the name of the src directory
#		AUXDIR	- the name of the auxilary directory
#		DOCDIR	- the name of the documentation directory
#		CQUER	- name for the user interface program
#		CQRUN	- name for the administrative program
#		CQSORT	- name for the sorting program
#		CXTRACT	- name for the prototype extractor
#		NROFF	- command to create online documentation
#		TROFF	- command to create hardcopy documentation
#
#	These are also passed on, but should not be changed:
#
#		GFILS	- source code for "conquer" files
#		AFILS	- source code for "conqrun" files
#		XFILS	- source code for both programs
#		GOBJS	- object files for "conquer" compilation
#		AOBJS	- object files for "conqrun" compilation
#		XOBJS	- object files for both programs
#
#	Please report any problems that you encounter with any of the
#	Makefiles.  If any problems develop while compiling, or if any
#	files but the Makefiles and header.h files need editing,
#	please send me mail.
#			adb@cs.bu.edu
#			adb@bucsf.bu.edu
#       		adb@bu.edu
#
#       Conquer news mailing list: conquer-news@cs.bu.edu.
#	Deletion/Additon requests to: conquer-news-request@cs.bu.edu
#
#	Version 5.0 Beta Bug Reports to: conquer-bugs@cs.bu.edu
#	To Join the Beta Mailing List: conquer-news-request@cs.bu.edu
#				   or: conquer-beta-request@cs.bu.edu
#				   or: adb@cs.bu.edu
#
#	This should be installed by whomever you want to own the game.
#	I recommend "games" or "root".

#	The various making things
MAKE	= /usr/bin/make
MKDPND	= makedepend
MKDIR	= mkdir

#	This bourne shell should work fine but the csh would
#	probably work if you set the NULL to >&/dev/null
SHELL	= /bin/sh

#	Other standard things, such as the compiler
CD	= cd
CC	= gcc
RM      = /bin/rm -f
CP	= cp
MV	= mv
ECHO	= echo
STRIP	= strip
NULL	= 2>/dev/null
TOUCH	= touch

#	*Very important*  This indicates the kind of machine
#	you are working on.  The current list of options is:
#
#	  SYSV3   - for system V R3 derivatives {?}
#	  SYSV4   - for system V R4 derivatives
#	  BSD     - for normal BSD derivatives
#	  SUN41   - for Sun OS 4.1 >
#	  MACHOS  - for NeXT machines running Mach {?}
#	  AIX     - for IBM Unix systems
#	  AIX370  - for IBM 370 systems running AIX
#	  ULTRIX  - for DEC's Unix systems
#	  HPUX    - for Hewlett Packards Unix systems
#	  VAXC    - for the DEC VMS VAXC compiler
#	  VMS     - for DEC's VMS operating systems
#
#	for normal System V derivatives, use one of these (AT+T, etc)
#SYSFLG	= -DSYSV4
#SYSFLG	= -DSYSV3
#
#	for normal BSD derivatives (UMax 4.[23], SunOS 3.x, 4.0(?))
SYSFLG	= -DBSD
#
#	for Sun OS 4.1 this is the right entry
#SYSFLG	= -DSUN41
#
#	for the NeXT Mach systems {?}
#SYSFLG	= -DMACHOS
#
#	for AIX systems
#SYSFLG	= -DAIX
#
#	for AIX 370 systems
#SYSFLG	= -DAIX370
#
#	for DEC Ultrix systems
#SYSFLG	= -DULTRIX
#
#	for HPUNIX systems, just use the HPUX definition
#SYSFLG	= -DHPUX
#
#	for Xenix machines, choose whichever base derivitive fits
#SYSFLG	= -DSYSV4 -DXENIX
#SYSFLG	= -DBSD -DXENIX
#
#	for DEC's Vax VMS systems, the following definitions should work
#SYSFLG	= -DVMS -DVAXC

#	Ownership changing command
#		: use this when installing as root
#CHOWN	= chown scores
#		: use this for normal user installation
CHOWN	= echo ... ignore

#	Permission changing command
#		: use this for setuid installation (recommended)
CHMOD	= chmod
#		: use this for non-setuid installation (non-UNIX?)
#CHMOD	= echo ... ignore also

#	uncomment the next line if you dont have getopt in your library
#	(eg you are on a pc, or a non unix system).  getopt.c is a
#	public domain software, and has not been tested by the authors
#	of conquer.
#GETOPT	= getopt.c
#GETOBJ	= getopt.o

#	for VMS systems without the mvprintw command in the curses
#	library, use the vms.c file.
#VMSFILE	= vms.c
#VMSOBJ	= vms.o

#
#	libraries for BSD systems:
LIBS	= -lcurses -ltermcap -lcrypt
#
#	libraries for SYSV systems:
#LIBS	= -lcurses
#
#	libraries for Xenix systems:
#LIBS	= -ltermlib -ltcap -lcrypt
#
#	(To gain regular expression use, add your Reg Exp
#	 library here, and use a -DREGEX in CFLAGS)
#	libraries for HPUX systems (varies with systems):
#LIBS	= -lcurses
#
#	libraries for SCO SysV systems:
#LIBS	= -lcurses -lcrypt_i
#
#	(?) libraries for SCO Xenix systems:
#LIBS	= -ltermlib -ltcap -lcrypt_i

#	Various Compiler Options
#
#	WARNING: AIX370 System, do not use the '-O' with
#		 your CFLAGS definitions below.
#
#	Note:	with gcc name conflicts in header files can be avoided by
#		increasing the -Wid-clash-# value.
#
#	Options flag used for non-debugging purposes (normal C compiler)
#CFLAGS	= -O
#	Options flag used when not debugging (GNU C compiler)
#CFLAGS	= -O -Wall -Wshadow -Wpointer-arith -Wcast-qual -Wid-clash-12 -ansi
#	Options flag used when debugging with normal C compiler.
#CFLAGS	= -g -DDEBUG
#	Options flag used when debugging with the GNU C Compiler
CFLAGS	= -O -g -Wall -Wshadow -Wpointer-arith -Wcast-qual -Wid-clash-12 -ansi -DDEBUG
#

#	With GCC on SUN 4 (SPARC) Machines, the optimizer cannot
#	be used when compiling checkX.c.  Switch comments as needed.
#CKFLAGS	= -g -Wall -Wshadow -Wpointer-arith -Wcast-qual -Wid-clash-12 -ansi -DDEBUG
CKFLAGS	= ${CFLAGS}

#	TOPDIR is this directory.  It is important to provide a full
#	(from root) path to this directory.  NOTE: some make programs
#	do not support the ${PWD} construct.
TOPDIR	= ${PWD}

#	DATADIR is the directory where the main (or default) campaign
#	will be located.  It is also the location for all of the
#	global data files, such as the help files.
DATADIR	= /usr/local/games/lib/conquer

#	BINDIR is the directory where the binaries will be placed
#	upon the installation.
BINDIR	= /usr/local/games/bin

#	The various subdirectories for the source code
#	and support files.
INCDIR	= Include
SRCDIR	= Src
AUXDIR	= Auxil
DOCDIR	= Docs

#	CQUER is the name for the user interface portion
CQUER	= conquer

#	CQRUN is the name for the administrative portion
CQRUN	= conqrun

#	CQSORT is the name for the program to perform the sorting
CQSORT	= conqsort

#	CXTRACT is the name of the prototype extractor
CXTRACT	= cextract

#	NROFF is the roff processing command which is used
#	to create online versions of documenation.
NROFF	= nroff

#	TROFF is the roff processing command which is used
#	to create the hardcopy (postscript) version of the
#	documentation.  Be sure to specify the flag to send the
#	output to the standard output and not to the printer.
TROFF	= psroff -t

#
#	=== Adjust below here at your own risk ===
#

# GFILS are files only needed for the user interface.
GFILS	= mainG.c armyG.c caravanG.c customG.c dataG.c displayG.c \
emailG.c enlistG.c hexmapG.c ieditG.c infoG.c ioG.c iodataG.c \
keybindG.c magicG.c mailG.c miscG.c moveG.c navyG.c ntninfoG.c \
pagerG.c regionG.c sectorG.c selectG.c xferG.c time_ckG.c

# AFILS are files only needed for the adminstration program.
AFILS	= mainA.c adduserA.c combatA.c configA.c createA.c dataA.c \
economyA.c magicA.c mailA.c miscA.c monsterA.c moveA.c npcA.c sectorA.c \
updateA.c

# XFILS are files used in both programs.
XFILS	= dataX.c datamagX.c datamilX.c checkX.c computeX.c convertX.c \
customX.c executeX.c hexmapX.c ioX.c iodataX.c magicX.c mailX.c \
memoryX.c miscX.c moveX.c sectorX.c selectX.c unitsX.c ${GETOPT} ${VMSFILE}

# List of object files
GOBJS	= ${GFILS:.c=.o}
AOBJS	= ${AFILS:.c=.o}
XOBJS	= ${XFILS:.c=.o} ${GETOBJ} ${VMSOBJ}

# === BROKEN MAKE ===
# Comment the above and uncomment these for bad make versions.
# [I highly recommend you upgrade your make if this is needed]
#GOBJS	= mainG.o armyG.o caravanG.o customG.o dataG.o displayG.o \
#emailG.o enlistG.o hexmapG.o ieditG.o infoG.o ioG.o iodataG.o \
#keybindG.o mailG.o moveG.o miscG.o navyG.o ntninfoG.o pagerG.o \
#regionG.o sectorG.o selectG.o magicG.o xferG.o time_ckG.o
#AOBJS	= mainA.o adduserA.o combatA.o configA.o createA.o dataA.o \
#economyA.o magicA.o mailA.o miscA.o monsterA.o moveA.o npcA.o sectorA.o \
#updateA.o
#XOBJS	= dataX.o datamagX.o datamilX.o checkX.o computeX.o convertX.o \
#customX.o executeX.o hexmapX.o ioX.o iodataX.o magicX.o mailX.o \
#memoryX.o miscX.o moveX.o sectorX.o selectX.o unitsX.o ${GETOBJ} ${VMSOBJ}

all:	
	@echo "Use one of:"
	@echo "   make build       -- build everything"
	@echo "   make install     -- install everything"
	@echo "   make clean       -- clean up some files"
	@echo "   make clobber     -- clean up all files"
	@echo "   make Makefiles   -- configure all Makefiles properly"
	@echo "   make mkfiles     -- same as above"

#
#	Build within each directory
build:
#	(${CD} ${INCDIR}; ${MAKE} build)  ## this need only be done by adb
	(${CD} ${SRCDIR}; ${MAKE} build)
	(${CD} ${AUXDIR}; ${MAKE} build)
	(${CD} ${DOCDIR}; ${MAKE} build)

#
#	Install within each directory
install:
	(${CD} ${SRCDIR}; ${MAKE} install)
	(${CD} ${AUXDIR}; ${MAKE} install)
	(${CD} ${DOCDIR}; ${MAKE} install)
	@echo Installation complete...
	@echo You may now build the world with \"conqrun -m\".

#
#	Clean up within each directory
clean:
	-(${CD} ${INCDIR}; ${MAKE} clean)
	-(${CD} ${SRCDIR}; ${MAKE} clean)
	-(${CD} ${AUXDIR}; ${MAKE} clean)
	-(${CD} ${DOCDIR}; ${MAKE} clean)
	${RM} *.o *~ \#* *.bak ${NULL}

#
#	Really clean up within each directory
clobber:
	-(${CD} ${INCDIR}; ${MAKE} clobber)
	-(${CD} ${SRCDIR}; ${MAKE} clobber)
	-(${CD} ${AUXDIR}; ${MAKE} clobber)
	-(${CD} ${DOCDIR}; ${MAKE} clobber)
	-${RM} ${INCDIR}/Makefile ${SRCDIR}/Makefile ${NULL}
	-${RM} ${AUXDIR}/Makefile ${DOCDIR}/Makefile ${NULL}
	-${RM} ${INCDIR}/header.h ${NULL}

#
#	Build all of the Makefiles
mkfiles:
	echo 's:%%MAKE%%:${MAKE}:g' > sed.out
	echo 's:%%MKDPND%%:${MKDPND}:g' >> sed.out
	echo 's:%%MKDIR%%:${MKDIR}:g' >> sed.out
	echo 's:%%CD%%:${CD}:g' >> sed.out
	echo 's:%%CC%%:${CC}:g' >> sed.out
	echo 's:%%RM%%:${RM}:g' >> sed.out
	echo 's:%%CP%%:${CP}:g' >> sed.out
	echo 's:%%MV%%:${MV}:g' >> sed.out
	echo 's:%%ECHO%%:${ECHO}:g' >> sed.out
	echo 's:%%SHELL%%:${SHELL}:g' >> sed.out
	echo 's:%%STRIP%%:${STRIP}:g' >> sed.out
	echo 's:%%NULL%%:${NULL}:g' >> sed.out
	echo 's:%%TOUCH%%:${TOUCH}:g' >> sed.out
	echo 's:%%CHOWN%%:${CHOWN}:g' >> sed.out
	echo 's:%%CHMOD%%:${CHMOD}:g' >> sed.out
	echo 's:%%LIBS%%:${LIBS}:g' >> sed.out
	echo 's:%%SYSFLG%%:${SYSFLG}:g' >> sed.out
	echo 's:%%CFLAGS%%:${CFLAGS}:g' >> sed.out
	echo 's:%%CKFLAGS%%:${CKFLAGS}:g' >> sed.out
	echo 's:%%TOPDIR%%:${TOPDIR}:g' >> sed.out
	echo 's:%%DATADIR%%:${DATADIR}:g' >> sed.out
	echo 's:%%BINDIR%%:${BINDIR}:g' >> sed.out
	echo 's:%%INCDIR%%:${INCDIR}:g' >> sed.out
	echo 's:%%SRCDIR%%:${SRCDIR}:g' >> sed.out
	echo 's:%%AUXDIR%%:${AUXDIR}:g' >> sed.out
	echo 's:%%DOCDIR%%:${DOCDIR}:g' >> sed.out
	echo 's:%%CQUER%%:${CQUER}:g' >> sed.out
	echo 's:%%CQRUN%%:${CQRUN}:g' >> sed.out
	echo 's:%%CQSORT%%:${CQSORT}:g' >> sed.out
	echo 's:%%CXTRACT%%:${CXTRACT}:g' >> sed.out
	echo 's:%%NROFF%%:${NROFF}:g' >> sed.out
	echo 's:%%TROFF%%:${TROFF}:g' >> sed.out
	echo 's:%%GFILS%%:${GFILS}:g' >> sed.out
	echo 's:%%AFILS%%:${AFILS}:g' >> sed.out
	echo 's:%%XFILS%%:${XFILS}:g' >> sed.out
	echo 's:%%GOBJS%%:${GOBJS}:g' >> sed.out
	echo 's:%%AOBJS%%:${AOBJS}:g' >> sed.out
	echo 's:%%XOBJS%%:${XOBJS}:g' >> sed.out
	-sed -f sed.out ${INCDIR}/Makefile.inc > ${INCDIR}/Makefile
	-sed -f sed.out ${SRCDIR}/Makefile.src > ${SRCDIR}/Makefile
	-sed -f sed.out ${DOCDIR}/Makefile.dcm > ${DOCDIR}/Makefile
	-sed -f sed.out ${AUXDIR}/Makefile.aux > ${AUXDIR}/Makefile
	${RM} sed.out ${NULL}

#	just in case
makefiles:	mkfiles

#
Makefiles:	mkfiles

#
