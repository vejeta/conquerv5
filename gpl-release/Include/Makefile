#	conquer: Copyright (c) 1991 by Edward M Barlow and Adam Bryant
#		 All Rights Reserved
#
#	This Makefile should not need changing.
#
#	Please report any problems that you encounter with any of the
#	Makefiles.  If any problems develop while compiling, or if any
#	files but the Makefiles and header.h files need editing,
#	please send me mail.
#			adb@cs.bu.edu
#			adb@bucsf.bu.edu
#       		adb@bu.edu
#

#	All of the default settings
MAKE	= /usr/bin/make
MKDPND	= makedepend
MKDIR	= mkdir
SHELL	= /bin/sh
CD	= cd
CC	= gcc
RM      = /bin/rm -f
CP	= cp
MV	= mv
ECHO	= echo
STRIP	= strip
NULL	= 2>/dev/null
TOUCH	= touch
CHOWN	= echo ... ignore
CHMOD	= chmod
LIBS	= -lcurses -ltermcap -lcrypt
SYSFLG	= -DBSD
CFLAGS	= -O -g -Wall -Wshadow -Wpointer-arith -Wcast-qual -Wid-clash-12 -ansi -DDEBUG
TOPDIR	= /home/brand1/kevin/foo/conquer
DATADIR	= /usr/local/games/lib/conquer
BINDIR	= /usr/local/games/bin
INCDIR	= /home/brand1/kevin/foo/conquer/Include
SRCDIR	= /home/brand1/kevin/foo/conquer/Src
AUXDIR	= /home/brand1/kevin/foo/conquer/Auxil
DOCDIR	= /home/brand1/kevin/foo/conquer/Docs
CQUER	= conquer
CQRUN	= conqrun
CQSORT	= conqsort
CXTRACT	= cextract
GFILS	= mainG.c armyG.c caravanG.c customG.c dataG.c displayG.c emailG.c enlistG.c hexmapG.c ieditG.c infoG.c ioG.c iodataG.c keybindG.c magicG.c mailG.c miscG.c moveG.c navyG.c ntninfoG.c pagerG.c regionG.c sectorG.c selectG.c xferG.c time_ckG.c
AFILS	= mainA.c adduserA.c combatA.c configA.c createA.c dataA.c economyA.c magicA.c mailA.c miscA.c monsterA.c moveA.c npcA.c sectorA.c updateA.c
XFILS	= dataX.c datamagX.c datamilX.c checkX.c computeX.c convertX.c customX.c executeX.c hexmapX.c ioX.c iodataX.c magicX.c mailX.c memoryX.c miscX.c moveX.c sectorX.c selectX.c unitsX.c  
GOJBS	= mainG.o armyG.o caravanG.o customG.o dataG.o displayG.o emailG.o enlistG.o hexmapG.o ieditG.o infoG.o ioG.o iodataG.o keybindG.o magicG.o mailG.o miscG.o moveG.o navyG.o ntninfoG.o pagerG.o regionG.o sectorG.o selectG.o xferG.o time_ckG.o
AOBJS	= mainA.o adduserA.o combatA.o configA.o createA.o dataA.o economyA.o magicA.o mailA.o miscA.o monsterA.o moveA.o npcA.o sectorA.o updateA.o
XOBJS	= dataX.o datamagX.o datamilX.o checkX.o computeX.o convertX.o customX.o executeX.o hexmapX.o ioX.o iodataX.o magicX.o mailX.o memoryX.o miscX.o moveX.o sectorX.o selectX.o unitsX.o    

#
#	Building and Installation Procedures
#
all:	
	@${ECHO} "Use one of:"
	@${ECHO} "   make build       -- build everything"
	@${ECHO} "   make install     -- install everything"
	@${ECHO} "   make clean       -- clean up some files"
	@${ECHO} "   make clobber     -- clean up all files"

#
build:	proto
	@${ECHO} Build complete in Include directory

#
install:
	@${ECHO} Install complete in Include directory

#	Cleaning things up
clean:
	${RM} *.o *~ *.bak \#* ${NULL}

#	Really clean things up
clobber:	clean

#
#	Variables unique to this Makefile
#

#	Options for cextract program
CXTFLG	= -S +CcPFaZ

#	Actual command to get cextract...
CXTCMD	= ${CXTRACT}

#	preprocessor definitions
CPPFLGS	= ${SYSFLG} -I${INCDIR}

#
#	Rules and dependencies unique to this Makefile
#

#	build all the prototype files
proto:	prota protg protx

#	build the joint prototypes
protx:	
	(${CD} ${SRCDIR}; ${CXTCMD} ${CPPFLGS} ${CXTFLG} -o ${INCDIR}/fileX.h ${XFILS} jointA.c)

#	build the conqrun prototypes
prota:	
	(${CD} ${SRCDIR}; ${CXTCMD} ${CPPFLGS} ${CXTFLG} -o ${INCDIR}/fileA.h ${AFILS})

#	build the conquer prototypes
protg:	
	(${CD} ${SRCDIR}; ${CXTCMD} ${CPPFLGS} ${CXTFLG} -o ${INCDIR}/fileG.h ${GFILS})
#
