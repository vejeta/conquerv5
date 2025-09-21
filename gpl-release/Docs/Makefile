# SPDX-License-Identifier: GPL-3.0-or-later
# SPDX-FileCopyrightText: 1987-1992 Ed Barlow <barlowedward@hotmail.com>
# SPDX-FileCopyrightText: 1987-1992 Adam Bryant <adb@usa.com>
# SPDX-FileCopyrightText: 2025 Juan Manuel Méndez Rey <vejeta@gmail.com>
#
# Conquer - Classic Multi-Player Strategy Game - Build Configuration
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
#
# Relicensed to GPL v3+ with explicit permission from original authors.
# For relicensing documentation, see RELICENSING-PERMISSIONS.md


#	This Makefile should not need changing.
#
#	Please report any problems that you encounter with any of the
#	Makefiles.  If any problems develop while compiling, or if any
#	files but Makefile.top and header.h files need editing.
#
#   Please report any problems that you encounter with compilation,
#   build issues, or bugs through the project's issue tracker.
#
#   Current Maintainer: Juan Manuel Méndez Rey <vejeta@gmail.com>
#   Project Repository: https://github.com/vejeta/conquerv5
#   Bug Reports: https://github.com/vejeta/conquerv5/issues
#   Documentation: See README.md and RELICENSING-PERMISSIONS.md
#
#   This is a community-maintained GPL v3+ version of the classic
#   Conquer game. Contributions and improvements are welcome.
#

#	All of the default settings
MAKE	= make
MKDPND	= makedepend
MKDIR	= mkdir -p
SHELL	= /opt/local/bin/bash
CD	= cd
CC	= cc
RM      = rm -f
CP	= cp
MV	= mv
ECHO	= echo
STRIP	= echo "Not stripping"
NULL	= 2>/dev/null
TOUCH	= touch
CHOWN	= echo "... ignore (not root)"
CHMOD	= chmod
LIBS	= -lcurses -ltermcap 
SYSFLG	= -DBSD -DPLATFORM_MACOS -DPLATFORM_UNIX
CFLAGS	= -Wall -Wextra -Wshadow -Wpointer-arith -Wcast-qual -g -O0 -DDEBUG -DBSD -DPLATFORM_MACOS -DPLATFORM_UNIX -DLOGIN="jmy99" -std=c99 -fno-omit-frame-pointer -fstack-protector-strong
TOPDIR	= /Users/jmy99/development/misc/conquerv5/gpl-release
DATADIR	= /Users/jmy99/.local/share/conquer
BINDIR	= /Users/jmy99/.local/bin
INCDIR	= /Users/jmy99/development/misc/conquerv5/gpl-release/Include
SRCDIR	= /Users/jmy99/development/misc/conquerv5/gpl-release/Src
AUXDIR	= /Users/jmy99/development/misc/conquerv5/gpl-release/Auxil
DOCDIR	= /Users/jmy99/development/misc/conquerv5/gpl-release/Docs
CQUER	= conquer
CQRUN	= conqrun
CQSORT	= conqsort
CXTRACT	= cextract
GFILS	= mainG.c armyG.c caravanG.c customG.c dataG.c displayG.c emailG.c enlistG.c hexmapG.c ieditG.c infoG.c ioG.c iodataG.c keybindG.c magicG.c mailG.c miscG.c moveG.c navyG.c ntninfoG.c pagerG.c regionG.c sectorG.c selectG.c xferG.c time_ckG.c
AFILS	= mainA.c adduserA.c combatA.c configA.c createA.c dataA.c economyA.c magicA.c mailA.c miscA.c monsterA.c moveA.c npcA.c sectorA.c updateA.c
XFILS	= dataX.c datamagX.c datamilX.c checkX.c computeX.c convertX.c customX.c executeX.c hexmapX.c ioX.c iodataX.c magicX.c mailX.c memoryX.c miscX.c moveX.c sectorX.c selectX.c unitsX.c  
GOBJS	= mainG.o armyG.o caravanG.o customG.o dataG.o displayG.o emailG.o enlistG.o hexmapG.o ieditG.o infoG.o ioG.o iodataG.o keybindG.o magicG.o mailG.o miscG.o moveG.o navyG.o ntninfoG.o pagerG.o regionG.o sectorG.o selectG.o xferG.o time_ckG.o
AOBJS	= mainA.o adduserA.o combatA.o configA.o createA.o dataA.o economyA.o magicA.o mailA.o miscA.o monsterA.o moveA.o npcA.o sectorA.o updateA.o
XOBJS	= dataX.o datamagX.o datamilX.o checkX.o computeX.o convertX.o customX.o executeX.o hexmapX.o ioX.o iodataX.o magicX.o mailX.o memoryX.o miscX.o moveX.o sectorX.o selectX.o unitsX.o  
NROFF	= nroff
TROFF	= psroff -t

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
build:
	@${ECHO} "Build in Doc complete"

#
install:	indocs
	@${ECHO} "Install in Doc complete"

#	Cleaning things up
clean:
	${RM} *.o *~ *.bak \#* ${NULL}

#	Really clean things up
clobber:	clean clobber2
	${RM} ${NCONV} indocs ${NULL}

#
#	Makefile specific variables
#

#	Name of the program to properly convert the nroff output
NCONV	= ./ezconv

#	List of source files for documentation
DSRC	= Helpfiles.nr Overview.nr Functions.nr Display.nr \
Sectors.nr Nations.nr Economics.nr Movement.nr Warfare.nr \
Powers.nr Xchanges.nr Beginnings.nr God-Powers.nr Customize.nr \
Quickies.nr Version.nr

#	List of output files for hardcopy build
DHRD	= Conquer-Doc.ps

#	List of output files for online build
DONL	= ${DSRC:.nr=.doc}
# === BROKEN MAKE ===
#	(note: bad makes need this)
#DONL	= Helpfiles.doc Overview.doc Functions.doc Display.doc \
#Sectors.doc Nations.doc Economics.doc Movement.doc Warfare.doc \
#Powers.doc Xchanges.doc Beginnings.doc God-Powers.doc Customize.doc \
#Quickies.doc Version.doc

#	List of dependencies for installation
#	[if you do not have nroff, comment this out]
INDOCS	= ${DONL}

#	Add to the suffixes list
.SUFFIXES:	.nr .doc .ps

#
#	Makefile specific rules and building information
#
online:	${DONL}
	@${ECHO} "== Online Documentation Updated =="

hardcopy:	${DHRD}
	@${ECHO} "== Hardcopy Documentation Updated =="

docs:	${NCONV} online hardcopy

indocs:	${DONL}
	-${MKDIR} ${DATADIR}
	${CHOWN} ${DATADIR}
	${CHMOD} 700 ${DATADIR}
	${CP} ${INDOCS} ${DATADIR}
	${CHOWN} ${DATADIR}/*
	${TOUCH} indocs

clobber2:
	${RM} ${DONL} ${DHRD} ${NULL}

#
#	how to create each of the online documents
.nr.doc:	$<
	${NROFF} roff-mac.nr $*.nr | ${NCONV} > $*.doc

#
#	how to create the postscript documents
.nr.ps:	$<
	${TROFF} $*.nr > $*.ps

#
#	quick conversion program
${NCONV}:	${NCONV}.c
	${CC} -O ${SYSFLG} -o ${NCONV} ${NCONV}.c
	${STRIP} ${NCONV}

#
#	additional dependencies
${DONL}:	roff-mac.nr ${NCONV}
#
${DHRD}:	roff-mac.nr ${DSRC}
#
build:	${DONL} ${DHRD}
#
