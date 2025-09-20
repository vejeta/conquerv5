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
#       Please report any problems with compilation or build issues
#       through the project's issue tracker.
#
#       Current Maintainer: Juan Manuel Méndez Rey <vejeta@gmail.com>
#       Project Repository: https://github.com/vejeta/conquerv5
#       Bug Reports: https://github.com/vejeta/conquerv5/issues
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
GOBJS	= mainG.o armyG.o caravanG.o customG.o dataG.o displayG.o emailG.o enlistG.o hexmapG.o ieditG.o infoG.o ioG.o iodataG.o keybindG.o magicG.o mailG.o miscG.o moveG.o navyG.o ntninfoG.o pagerG.o regionG.o sectorG.o selectG.o xferG.o time_ckG.o
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
build:	${CQSORT}
	@${ECHO} Build in Auxil done

#
install:	in${CQSORT}
	@${ECHO} Installation in Auxil done

#	Cleaning things up
clean:
	${RM} *.o *~ *.bak \#* ${NULL}

#	Really clean things up
clobber:	clean
	${RM} in${CQSORT} ${CQSORT} ${NULL}

#
#	Makefile specfic variables
#

#	use the value of C flags
CFLGS	= ${CFLAGS}
CPPFLGS	= ${SYSFLG}

#	list of object files
SORTOBJS	= sort.o

#
#	Rules and dependencies for this Makefile
#
.c.o:	$<
	${CC} ${CFLGS} ${CPPFLGS} -c $*.c

${CQSORT}:	${SORTOBJS}
	${CC} ${CFLGS} -o ${CQSORT} ${SORTOBJS}

in${CQSORT}:	${CQSORT}
	@${ECHO} Installing ${CQSORT}...
	${STRIP} ${CQSORT}
	-${RM} ${BINDIR}/${CQSORT} ${NULL}
	-${MKDIR} ${BINDIR}
	${MV} ${CQSORT} ${BINDIR}
	${CHOWN} ${BINDIR}/${CQSORT}
	${CHMOD} 755 ${BINDIR}/${CQSORT}
	${TOUCH} ${CQSORT}
	${TOUCH} in${CQSORT}

#
