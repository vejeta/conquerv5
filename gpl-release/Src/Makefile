#	conquer: Copyright 1991 by Edward M Barlow and Adam Bryant
#		 All Rights Reserved 
#
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
CC	= gcc
CD	= cd
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
CKFLAGS	= -O -g -Wall -Wshadow -Wpointer-arith -Wcast-qual -Wid-clash-12 -ansi -DDEBUG
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

#	lists of object files
AOBJS	= mainA.o adduserA.o combatA.o configA.o createA.o dataA.o economyA.o magicA.o mailA.o miscA.o monsterA.o moveA.o npcA.o sectorA.o updateA.o jointA.o
GOBJS	= mainG.o armyG.o caravanG.o customG.o dataG.o displayG.o emailG.o enlistG.o hexmapG.o ieditG.o infoG.o ioG.o iodataG.o keybindG.o magicG.o mailG.o miscG.o moveG.o navyG.o ntninfoG.o pagerG.o regionG.o sectorG.o selectG.o xferG.o time_ckG.o jointG.o
XOBJS	= dataX.o datamagX.o datamilX.o checkX.o computeX.o convertX.o customX.o executeX.o hexmapX.o ioX.o iodataX.o magicX.o mailX.o memoryX.o miscX.o moveX.o sectorX.o selectX.o unitsX.o    

#
#	Makefile specific variables
#

#	preprocessor definitions
CPPFLGS	= -I${INCDIR} ${SYSFLG}

#	list of support files to be installed
SUPPORT	= nations

#
#	Building and Installation Procedures
#
all:	
	@${ECHO} "Use one of:"
	@${ECHO} "   make build       -- build everything"
	@${ECHO} "   make install     -- install everything"
	@${ECHO} "   make clean       -- clean up everything"
	@${ECHO} "   make clobber     -- really do some cleanup"

#
build:	${CQUER} ${CQRUN}
	@${ECHO} Done with build in Src

#
install:	in${CQUER} in${CQRUN} insupport
	@${ECHO} Done with install in Src

#
#	installation of support files
insupport:	${SUPPORT}
	-${MKDIR} ${DATADIR}
	${CHOWN} ${DATADIR}
	${CHMOD} 700 ${DATADIR}
	${CP} ${SUPPORT} ${DATADIR}
	${CHOWN} ${DATADIR}/*
	${TOUCH} insupport

#
#	Makefile specific dependencies and rules

#	User interface
${CQUER}:	${GOBJS} ${XOBJS}
	@${ECHO} ${CQUER} object files done...
	@${ECHO} === Compiling User Interface
	${CC} ${CFLAGS} -o ${CQUER} ${GOBJS} ${XOBJS} ${LIBS}

#	Administrative program
${CQRUN}:	${AOBJS} ${XOBJS}
	@${ECHO} ${CQRUN} object files done...
	@${ECHO} === Compiling Administrative Program
	${CC} ${CFLAGS} -o ${CQRUN} ${AOBJS} ${XOBJS} ${LIBS}

#	Installation of user interface
in${CQUER}:	${CQUER}
	@${ECHO} Installing ${CQUER}...
	${STRIP} ${CQUER}
	-${RM} ${BINDIR}/${CQUER} ${NULL}
	-${MKDIR} ${BINDIR}
	${MV} ${CQUER} ${BINDIR}
	${CHOWN} ${BINDIR}/${CQUER}
	${CHMOD} 4751 ${BINDIR}/${CQUER}
	${TOUCH} ${CQUER}
	${TOUCH} in${CQUER}

#	Installation of administrative program
in${CQRUN}:	${CQRUN}
	@${ECHO} Installing ${CQRUN}...
	${STRIP} ${CQRUN}
	-${RM} ${BINDIR}/${CQRUN} ${NULL}
	-${MKDIR} ${BINDIR}
	${MV} ${CQRUN} ${BINDIR}
	${CHOWN} ${BINDIR}/${CQRUN}
	${CHMOD} 4751 ${BINDIR}/${CQRUN}
	${TOUCH} ${CQRUN}
	${TOUCH} in${CQRUN}
#
checkX.o:	checkX.c
	${CC} ${CKFLAGS} ${CPPFLGS} -c $*.c

#
miscA.o:	miscA.c Makefile
	${CC} ${CFLAGS} -DCONQ_SORT=\"${CQSORT}\" ${CPPFLGS} -c $*.c

#
customX.o:	customX.c Makefile
	${CC} ${CFLAGS} -DDEFAULTDIR=\"${DATADIR}\" -DEXEDIR=\"${BINDIR}\" ${CPPFLGS} -c $*.c

#	Creating object files
.c.o:	$<
	${CC} ${CFLAGS} ${CPPFLGS} -c $*.c

#	Generate the dependencies
depend:
	@${ECHO} Building the dependencies
	-${MKDPND} -- ${CPPFLGS} -- ${AFILS} ${GFILS} ${XFILS} jointA.c jointG.c
	@${ECHO} Dependency building done

#	Cleaning things up
clean:
	${RM} *.o *~ *.bak *.orig \#* ${NULL}

#	Really clean things up
clobber:	clean
	${RM} ${CQUER} ${CQRUN} in${CQUER} in${CQRUN} insupport ${NULL}

#	Stuff for Saber C v3.0
saber_${CQRUN}:	saber_xobj saber_aobj
	#load ${LIBS}

saber_un${CQRUN}:	saber_unx saber_una
	#unload ${LIBS}

saber_${CQUER}:	saber_xobj saber_gobj
	#load ${LIBS}

saber_un${CQUER}:	saber_unx saber_ung
	#unload ${LIBS}

saber_xobj:	${XOBJS}
	#load ${CFLAGS} \'${CPPFLGS}\' ${XOBJS}

saber_aobj:	${AOBJS}
	#load ${CFLAGS} \'${CPPFLGS}\' ${AOBJS}

saber_gobj:	${GOBJS}
	#load ${CFLAGS} \'${CPPFLGS}\' ${GOBJS}

saber_unx:
	#unload ${XOBJS}

saber_una:
	#unload ${AOBJS}

saber_ung:
	#unload ${GOBJS}

#	Add to the suffixes list for Saber C loading
.SUFFIXES:	.src .obj

#	Lines to load just one source or object file into Saber
.c.src:	
	#load ${CFLAGS} ${CPPFLGS} $<

.o.obj:	
	#load ${CFLAGS} \'${CPPFLGS}\' $<

#
# DO NOT DELETE THIS LINE -- make depend depends on it.

# all of these dependencies are the default values, for those systems
# that do not have the makedepend command.

#
# The global dependencies
#
${AOBJS}: ${INCDIR}/dataA.h
${AOBJS}: ${INCDIR}/dataX.h 
${AOBJS}: ${INCDIR}/header.h
${AOBJS}: ${INCDIR}/paramX.h
${AOBJS}: ${INCDIR}/sysconf.h 
${AOBJS}: ${INCDIR}/fileX.h
${AOBJS}: ${INCDIR}/fileA.h 

${GOBJS}: ${INCDIR}/dataG.h
${GOBJS}: ${INCDIR}/dataX.h
${GOBJS}: ${INCDIR}/header.h
${GOBJS}: ${INCDIR}/paramX.h
${GOBJS}: ${INCDIR}/sysconf.h
${GOBJS}: ${INCDIR}/fileX.h
${GOBJS}: ${INCDIR}/fileG.h
${GOBJS}: ${INCDIR}/keybindG.h

${XOBJS}: ${INCDIR}/dataX.h 
${XOBJS}: ${INCDIR}/header.h
${XOBJS}: ${INCDIR}/paramX.h
${XOBJS}: ${INCDIR}/sysconf.h 
${XOBJS}: ${INCDIR}/fileX.h

#
# Individual object file dependencies
#
mainA.o: ${INCDIR}/worldX.h

adduserA.o: ${INCDIR}/armyX.h
adduserA.o: ${INCDIR}/cityX.h
adduserA.o: ${INCDIR}/calenX.h
adduserA.o: ${INCDIR}/magicX.h
adduserA.o: ${INCDIR}/buildA.h
adduserA.o: ${INCDIR}/desigX.h
adduserA.o: ${INCDIR}/mtrlsX.h
adduserA.o: ${INCDIR}/racesX.h
adduserA.o: ${INCDIR}/worldX.h
adduserA.o: ${INCDIR}/activeX.h
adduserA.o: ${INCDIR}/elevegX.h
adduserA.o: ${INCDIR}/nclassX.h
adduserA.o: ${INCDIR}/stringX.h
adduserA.o: ${INCDIR}/tgoodsX.h
adduserA.o: ${INCDIR}/statusX.h
adduserA.o: ${INCDIR}/adduserA.h
adduserA.o: ${INCDIR}/keyvalsX.h

combatA.o: ${INCDIR}/armyX.h
combatA.o: ${INCDIR}/cityX.h
combatA.o: ${INCDIR}/navyX.h
combatA.o: ${INCDIR}/butesX.h
combatA.o: ${INCDIR}/desigX.h
combatA.o: ${INCDIR}/magicX.h
combatA.o: ${INCDIR}/worldX.h
combatA.o: ${INCDIR}/activeX.h
combatA.o: ${INCDIR}/combatA.h
combatA.o: ${INCDIR}/elevegX.h
combatA.o: ${INCDIR}/statusX.h
combatA.o: ${INCDIR}/caravanX.h
combatA.o: ${INCDIR}/dstatusX.h

configA.o: ${INCDIR}/buildA.h
configA.o: ${INCDIR}/calenX.h
configA.o: ${INCDIR}/worldX.h
configA.o: ${INCDIR}/activeX.h
configA.o: ${INCDIR}/stringX.h
configA.o: ${INCDIR}/keyvalsX.h

createA.o: ${INCDIR}/armyX.h
createA.o: ${INCDIR}/cityX.h
createA.o: ${INCDIR}/navyX.h
createA.o: ${INCDIR}/buildA.h
createA.o: ${INCDIR}/desigX.h
createA.o: ${INCDIR}/magicX.h
createA.o: ${INCDIR}/racesX.h
createA.o: ${INCDIR}/mtrlsX.h
createA.o: ${INCDIR}/worldX.h
createA.o: ${INCDIR}/activeX.h
createA.o: ${INCDIR}/elevegX.h
createA.o: ${INCDIR}/statusX.h
createA.o: ${INCDIR}/tgoodsX.h

dataA.o: ${INCDIR}/magicX.h
dataA.o: ${INCDIR}/adduserA.h

economyA.o: ${INCDIR}/armyX.h
economyA.o: ${INCDIR}/cityX.h
economyA.o: ${INCDIR}/itemX.h
economyA.o: ${INCDIR}/navyX.h
economyA.o: ${INCDIR}/butesX.h
economyA.o: ${INCDIR}/calenX.h
economyA.o: ${INCDIR}/desigX.h
economyA.o: ${INCDIR}/racesX.h
economyA.o: ${INCDIR}/magicX.h
economyA.o: ${INCDIR}/mtrlsX.h
economyA.o: ${INCDIR}/worldX.h
economyA.o: ${INCDIR}/activeX.h
economyA.o: ${INCDIR}/elevegX.h
economyA.o: ${INCDIR}/statusX.h
economyA.o: ${INCDIR}/caravanX.h
economyA.o: ${INCDIR}/dstatusX.h

mailA.o: ${INCDIR}/activeX.h

miscA.o: ${INCDIR}/armyX.h
miscA.o: ${INCDIR}/cityX.h
miscA.o: ${INCDIR}/itemX.h
miscA.o: ${INCDIR}/navyX.h
miscA.o: ${INCDIR}/butesX.h
miscA.o: ${INCDIR}/worldX.h
miscA.o: ${INCDIR}/hlightX.h
miscA.o: ${INCDIR}/caravanX.h

monsterA.o: ${INCDIR}/armyX.h
monsterA.o: ${INCDIR}/cityX.h
monsterA.o: ${INCDIR}/navyX.h
monsterA.o: ${INCDIR}/desigX.h
monsterA.o: ${INCDIR}/mtrlsX.h
monsterA.o: ${INCDIR}/worldX.h
monsterA.o: ${INCDIR}/elevegX.h
monsterA.o: ${INCDIR}/statusX.h

moveA.o: ${INCDIR}/armyX.h
moveA.o: ${INCDIR}/moveX.h

npcA.o: ${INCDIR}/activeX.h

sectorA.o: ${INCDIR}/armyX.h
sectorA.o: ${INCDIR}/cityX.h
sectorA.o: ${INCDIR}/magicX.h
sectorA.o: ${INCDIR}/racesX.h
sectorA.o: ${INCDIR}/desigX.h
sectorA.o: ${INCDIR}/activeX.h
sectorA.o: ${INCDIR}/statusX.h
sectorA.o: ${INCDIR}/dstatusX.h

updateA.o: ${INCDIR}/armyX.h
updateA.o: ${INCDIR}/navyX.h
updateA.o: ${INCDIR}/butesX.h
updateA.o: ${INCDIR}/calenX.h
updateA.o: ${INCDIR}/desigX.h
updateA.o: ${INCDIR}/magicX.h
updateA.o: ${INCDIR}/mtrlsX.h
updateA.o: ${INCDIR}/racesX.h
updateA.o: ${INCDIR}/worldX.h
updateA.o: ${INCDIR}/activeX.h
updateA.o: ${INCDIR}/hlightX.h
updateA.o: ${INCDIR}/statusX.h
updateA.o: ${INCDIR}/tgoodsX.h
updateA.o: ${INCDIR}/caravanX.h
updateA.o: ${INCDIR}/dstatusX.h

mainG.o: ${INCDIR}/moveX.h
mainG.o: ${INCDIR}/worldX.h
mainG.o: ${INCDIR}/activeX.h
mainG.o: ${INCDIR}/hlightX.h
mainG.o: ${INCDIR}/displayG.h
mainG.o: ${INCDIR}/displayX.h
mainG.o: ${INCDIR}/patchlevel.h

armyG.o: ${INCDIR}/armyX.h
armyG.o: ${INCDIR}/cityX.h
armyG.o: ${INCDIR}/navyX.h
armyG.o: ${INCDIR}/desigX.h
armyG.o: ${INCDIR}/mtrlsX.h
armyG.o: ${INCDIR}/worldX.h
armyG.o: ${INCDIR}/statusX.h
armyG.o: ${INCDIR}/dstatusX.h
armyG.o: ${INCDIR}/keyvalsX.h

caravanG.o: ${INCDIR}/cityX.h
caravanG.o: ${INCDIR}/navyX.h
caravanG.o: ${INCDIR}/mtrlsX.h
caravanG.o: ${INCDIR}/worldX.h
caravanG.o: ${INCDIR}/elevegX.h
caravanG.o: ${INCDIR}/statusX.h
caravanG.o: ${INCDIR}/caravanX.h

customG.o: ${INCDIR}/armyX.h
customG.o: ${INCDIR}/desigX.h
customG.o: ${INCDIR}/worldX.h
customG.o: ${INCDIR}/elevegX.h
customG.o: ${INCDIR}/tgoodsX.h
customG.o: ${INCDIR}/displayG.h
customG.o: ${INCDIR}/displayX.h
customG.o: ${INCDIR}/optionsX.h
customG.o: ${INCDIR}/patchlevel.h

dataG.o: ${INCDIR}/infoG.h
dataG.o: ${INCDIR}/displayG.h
dataG.o: ${INCDIR}/displayX.h

displayG.o: ${INCDIR}/armyX.h
displayG.o: ${INCDIR}/cityX.h
displayG.o: ${INCDIR}/navyX.h
displayG.o: ${INCDIR}/moveX.h
displayG.o: ${INCDIR}/calenX.h
displayG.o: ${INCDIR}/desigX.h
displayG.o: ${INCDIR}/magicX.h
displayG.o: ${INCDIR}/mtrlsX.h
displayG.o: ${INCDIR}/racesX.h
displayG.o: ${INCDIR}/worldX.h
displayG.o: ${INCDIR}/activeX.h
displayG.o: ${INCDIR}/elevegX.h
displayG.o: ${INCDIR}/hlightX.h
displayG.o: ${INCDIR}/statusX.h
displayG.o: ${INCDIR}/stringX.h
displayG.o: ${INCDIR}/tgoodsX.h
displayG.o: ${INCDIR}/displayG.h
displayG.o: ${INCDIR}/displayX.h
displayG.o: ${INCDIR}/caravanX.h
displayG.o: ${INCDIR}/patchlevel.h

emailG.o: ${INCDIR}/rmailX.h
emailG.o: ${INCDIR}/keyvalsX.h

enlistG.o: ${INCDIR}/armyX.h
enlistG.o: ${INCDIR}/cityX.h
enlistG.o: ${INCDIR}/itemX.h
enlistG.o: ${INCDIR}/navyX.h
enlistG.o: ${INCDIR}/desigX.h
enlistG.o: ${INCDIR}/magicX.h
enlistG.o: ${INCDIR}/mtrlsX.h
enlistG.o: ${INCDIR}/worldX.h
enlistG.o: ${INCDIR}/elevegX.h
enlistG.o: ${INCDIR}/activeX.h
enlistG.o: ${INCDIR}/hlightX.h
enlistG.o: ${INCDIR}/statusX.h
enlistG.o: ${INCDIR}/caravanX.h

hexmapG.o: ${INCDIR}/armyX.h
hexmapG.o: ${INCDIR}/cityX.h
hexmapG.o: ${INCDIR}/moveX.h
hexmapG.o: ${INCDIR}/navyX.h
hexmapG.o: ${INCDIR}/desigX.h
hexmapG.o: ${INCDIR}/magicX.h
hexmapG.o: ${INCDIR}/racesX.h
hexmapG.o: ${INCDIR}/activeX.h
hexmapG.o: ${INCDIR}/elevegX.h
hexmapG.o: ${INCDIR}/hlightX.h
hexmapG.o: ${INCDIR}/statusX.h
hexmapG.o: ${INCDIR}/tgoodsX.h
hexmapG.o: ${INCDIR}/dstatusX.h
hexmapG.o: ${INCDIR}/caravanX.h
hexmapG.o: ${INCDIR}/displayG.h
hexmapG.o: ${INCDIR}/displayX.h
hexmapG.o: ${INCDIR}/worldX.h

ieditG.o: ${INCDIR}/armyX.h
ieditG.o: ${INCDIR}/cityX.h
ieditG.o: ${INCDIR}/infoG.h
ieditG.o: ${INCDIR}/navyX.h
ieditG.o: ${INCDIR}/mtrlsX.h
ieditG.o: ${INCDIR}/worldX.h
ieditG.o: ${INCDIR}/activeX.h
ieditG.o: ${INCDIR}/statusX.h
ieditG.o: ${INCDIR}/stringX.h
ieditG.o: ${INCDIR}/caravanX.h
ieditG.o: ${INCDIR}/dstatusX.h

infoG.o: ${INCDIR}/armyX.h
infoG.o: ${INCDIR}/cityX.h
infoG.o: ${INCDIR}/infoG.h
infoG.o: ${INCDIR}/itemX.h
infoG.o: ${INCDIR}/navyX.h
infoG.o: ${INCDIR}/desigX.h
infoG.o: ${INCDIR}/mtrlsX.h
infoG.o: ${INCDIR}/racesX.h
infoG.o: ${INCDIR}/worldX.h
infoG.o: ${INCDIR}/activeX.h
infoG.o: ${INCDIR}/nclassX.h
infoG.o: ${INCDIR}/statusX.h
infoG.o: ${INCDIR}/stringX.h
infoG.o: ${INCDIR}/caravanX.h
infoG.o: ${INCDIR}/dstatusX.h
infoG.o: ${INCDIR}/keyvalsX.h

ioG.o: ${INCDIR}/armyX.h
ioG.o: ${INCDIR}/cityX.h
ioG.o: ${INCDIR}/itemX.h
ioG.o: ${INCDIR}/navyX.h
ioG.o: ${INCDIR}/butesX.h
ioG.o: ${INCDIR}/calenX.h
ioG.o: ${INCDIR}/desigX.h
ioG.o: ${INCDIR}/mtrlsX.h
ioG.o: ${INCDIR}/racesX.h
ioG.o: ${INCDIR}/worldX.h
ioG.o: ${INCDIR}/activeX.h
ioG.o: ${INCDIR}/elevegX.h
ioG.o: ${INCDIR}/nclassX.h
ioG.o: ${INCDIR}/spellsX.h
ioG.o: ${INCDIR}/statusX.h
ioG.o: ${INCDIR}/stringX.h
ioG.o: ${INCDIR}/tgoodsX.h
ioG.o: ${INCDIR}/caravanX.h
ioG.o: ${INCDIR}/dstatusX.h
ioG.o: ${INCDIR}/patchlevel.h

iodataG.o: ${INCDIR}/mtrlsX.h
iodataG.o: ${INCDIR}/activeX.h
iodataG.o: ${INCDIR}/cityX.h
iodataG.o: ${INCDIR}/worldX.h

keybindG.o: ${INCDIR}/keyvalsX.h

magicG.o: ${INCDIR}/armyX.h
magicG.o: ${INCDIR}/cityX.h
magicG.o: ${INCDIR}/moveX.h
magicG.o: ${INCDIR}/navyX.h
magicG.o: ${INCDIR}/butesX.h
magicG.o: ${INCDIR}/desigX.h
magicG.o: ${INCDIR}/mtrlsX.h
magicG.o: ${INCDIR}/magicX.h
magicG.o: ${INCDIR}/racesX.h
magicG.o: ${INCDIR}/worldX.h
magicG.o: ${INCDIR}/activeX.h
magicG.o: ${INCDIR}/spellsX.h
magicG.o: ${INCDIR}/statusX.h
magicG.o: ${INCDIR}/caravanX.h
magicG.o: ${INCDIR}/dstatusX.h
magicG.o: ${INCDIR}/keyvalsX.h

mailG.o: ${INCDIR}/rmailX.h
mailG.o: ${INCDIR}/keyvalsX.h

miscG.o: ${INCDIR}/armyX.h
miscG.o: ${INCDIR}/infoG.h
miscG.o: ${INCDIR}/itemX.h
miscG.o: ${INCDIR}/moveX.h
miscG.o: ${INCDIR}/butesX.h
miscG.o: ${INCDIR}/calenX.h
miscG.o: ${INCDIR}/desigX.h
miscG.o: ${INCDIR}/magicX.h
miscG.o: ${INCDIR}/mtrlsX.h
miscG.o: ${INCDIR}/activeX.h
miscG.o: ${INCDIR}/hlightX.h
miscG.o: ${INCDIR}/stringX.h
miscG.o: ${INCDIR}/tgoodsX.h
miscG.o: ${INCDIR}/optionsX.h
miscG.o: ${INCDIR}/cityX.h
miscG.o: ${INCDIR}/worldX.h

moveG.o: ${INCDIR}/armyX.h
moveG.o: ${INCDIR}/itemX.h
moveG.o: ${INCDIR}/moveX.h
moveG.o: ${INCDIR}/navyX.h
moveG.o: ${INCDIR}/calenX.h
moveG.o: ${INCDIR}/desigX.h
moveG.o: ${INCDIR}/statusX.h
moveG.o: ${INCDIR}/caravanX.h
moveG.o: ${INCDIR}/dstatusX.h
moveG.o: ${INCDIR}/keyvalsX.h

navyG.o: ${INCDIR}/armyX.h
navyG.o: ${INCDIR}/cityX.h
navyG.o: ${INCDIR}/navyX.h
navyG.o: ${INCDIR}/desigX.h
navyG.o: ${INCDIR}/mtrlsX.h
navyG.o: ${INCDIR}/worldX.h
navyG.o: ${INCDIR}/elevegX.h
navyG.o: ${INCDIR}/statusX.h
navyG.o: ${INCDIR}/caravanX.h

ntninfoG.o: ${INCDIR}/butesX.h
ntninfoG.o: ${INCDIR}/mtrlsX.h
ntninfoG.o: ${INCDIR}/racesX.h
ntninfoG.o: ${INCDIR}/worldX.h
ntninfoG.o: ${INCDIR}/activeX.h
ntninfoG.o: ${INCDIR}/nclassX.h
ntninfoG.o: ${INCDIR}/stringX.h
ntninfoG.o: ${INCDIR}/ntninfoG.h
ntninfoG.o: ${INCDIR}/keyvalsX.h

pagerG.o: ${INCDIR}/stringX.h
pagerG.o: ${INCDIR}/keyvalsX.h

regionG.o: ${INCDIR}/armyX.h
regionG.o: ${INCDIR}/cityX.h
regionG.o: ${INCDIR}/navyX.h
regionG.o: ${INCDIR}/butesX.h
regionG.o: ${INCDIR}/desigX.h
regionG.o: ${INCDIR}/mtrlsX.h
regionG.o: ${INCDIR}/worldX.h
regionG.o: ${INCDIR}/stringX.h
regionG.o: ${INCDIR}/caravanX.h
sectorG.o: ${INCDIR}/cityX.h
sectorG.o: ${INCDIR}/worldX.h
sectorG.o: ${INCDIR}/magicX.h
sectorG.o: ${INCDIR}/mtrlsX.h
sectorG.o: ${INCDIR}/desigX.h
sectorG.o: ${INCDIR}/elevegX.h
sectorG.o: ${INCDIR}/hlightX.h
sectorG.o: ${INCDIR}/stringX.h
sectorG.o: ${INCDIR}/tgoodsX.h
sectorG.o: ${INCDIR}/displayG.h
sectorG.o: ${INCDIR}/displayX.h
selectG.o: ${INCDIR}/moveX.h
selectG.o: ${INCDIR}/displayG.h
selectG.o: ${INCDIR}/displayX.h
selectG.o: ${INCDIR}/keyvalsX.h

xferG.o: ${INCDIR}/xferG.h
xferG.o: ${INCDIR}/desigX.h
xferG.o: ${INCDIR}/mtrlsX.h
xferG.o: ${INCDIR}/statusX.h
xferG.o: ${INCDIR}/keyvalsX.h
xferG.o: ${INCDIR}/armyX.h
xferG.o: ${INCDIR}/cityX.h
xferG.o: ${INCDIR}/navyX.h
xferG.o: ${INCDIR}/worldX.h
xferG.o: ${INCDIR}/caravanX.h
xferG.o: ${INCDIR}/dstatusX.h

time_ckG.o: ${INCDIR}/keyvalsX.h

dataX.o: ${INCDIR}/butesX.h
dataX.o: ${INCDIR}/desigX.h
dataX.o: ${INCDIR}/magicX.h
dataX.o: ${INCDIR}/mtrlsX.h
dataX.o: ${INCDIR}/racesX.h
dataX.o: ${INCDIR}/elevegX.h
dataX.o: ${INCDIR}/nclassX.h
dataX.o: ${INCDIR}/tgoodsX.h
dataX.o: ${INCDIR}/displayX.h
dataX.o: ${INCDIR}/optionsX.h

datamagX.o: ${INCDIR}/butesX.h
datamagX.o: ${INCDIR}/magicX.h
datamagX.o: ${INCDIR}/spellsX.h

datamilX.o: ${INCDIR}/armyX.h
datamilX.o: ${INCDIR}/navyX.h
datamilX.o: ${INCDIR}/magicX.h
datamilX.o: ${INCDIR}/statusX.h

checkX.o: ${INCDIR}/armyX.h
checkX.o: ${INCDIR}/cityX.h
checkX.o: ${INCDIR}/itemX.h
checkX.o: ${INCDIR}/navyX.h
checkX.o: ${INCDIR}/calenX.h
checkX.o: ${INCDIR}/desigX.h
checkX.o: ${INCDIR}/mtrlsX.h
checkX.o: ${INCDIR}/racesX.h
checkX.o: ${INCDIR}/activeX.h
checkX.o: ${INCDIR}/elevegX.h
checkX.o: ${INCDIR}/nclassX.h
checkX.o: ${INCDIR}/statusX.h
checkX.o: ${INCDIR}/tgoodsX.h
checkX.o: ${INCDIR}/caravanX.h
checkX.o: ${INCDIR}/dstatusX.h 

computeX.o: ${INCDIR}/armyX.h
computeX.o: ${INCDIR}/cityX.h
computeX.o: ${INCDIR}/itemX.h
computeX.o: ${INCDIR}/navyX.h
computeX.o: ${INCDIR}/butesX.h
computeX.o: ${INCDIR}/calenX.h
computeX.o: ${INCDIR}/desigX.h
computeX.o: ${INCDIR}/magicX.h
computeX.o: ${INCDIR}/mtrlsX.h
computeX.o: ${INCDIR}/worldX.h
computeX.o: ${INCDIR}/activeX.h
computeX.o: ${INCDIR}/elevegX.h
computeX.o: ${INCDIR}/statusX.h
computeX.o: ${INCDIR}/tgoodsX.h
computeX.o: ${INCDIR}/weightX.h
computeX.o: ${INCDIR}/caravanX.h

convertX.o: ${INCDIR}/magicX.h
convertX.o: ${INCDIR}/keyvalsX.h

customX.o: ${INCDIR}/desigX.h
customX.o: ${INCDIR}/elevegX.h
customX.o: ${INCDIR}/optionsX.h

executeX.o: ${INCDIR}/armyX.h
executeX.o: ${INCDIR}/cityX.h
executeX.o: ${INCDIR}/itemX.h
executeX.o: ${INCDIR}/navyX.h
executeX.o: ${INCDIR}/butesX.h
executeX.o: ${INCDIR}/magicX.h
executeX.o: ${INCDIR}/mtrlsX.h
executeX.o: ${INCDIR}/worldX.h
executeX.o: ${INCDIR}/activeX.h
executeX.o: ${INCDIR}/elevegX.h
executeX.o: ${INCDIR}/nclassX.h
executeX.o: ${INCDIR}/statusX.h
executeX.o: ${INCDIR}/caravanX.h
executeX.o: ${INCDIR}/dstatusX.h

ioX.o: ${INCDIR}/armyX.h
ioX.o: ${INCDIR}/cityX.h
ioX.o: ${INCDIR}/navyX.h
ioX.o: ${INCDIR}/itemX.h
ioX.o: ${INCDIR}/calenX.h
ioX.o: ${INCDIR}/desigX.h
ioX.o: ${INCDIR}/magicX.h
ioX.o: ${INCDIR}/statusX.h
ioX.o: ${INCDIR}/stringX.h
ioX.o: ${INCDIR}/elevegX.h
ioX.o: ${INCDIR}/spellsX.h
ioX.o: ${INCDIR}/tgoodsX.h
ioX.o: ${INCDIR}/optionsX.h
ioX.o: ${INCDIR}/keyvalsX.h
ioX.o: ${INCDIR}/caravanX.h
ioX.o: ${INCDIR}/patchlevel.h

iodataX.o: ${INCDIR}/armyX.h
iodataX.o: ${INCDIR}/cityX.h
iodataX.o: ${INCDIR}/navyX.h
iodataX.o: ${INCDIR}/itemX.h
iodataX.o: ${INCDIR}/butesX.h
iodataX.o: ${INCDIR}/activeX.h
iodataX.o: ${INCDIR}/statusX.h
iodataX.o: ${INCDIR}/dataioX.h
iodataX.o: ${INCDIR}/caravanX.h
iodataX.o: ${INCDIR}/olddataX.h
iodataX.o: ${INCDIR}/patchlevel.h

magicX.o: ${INCDIR}/magicX.h
magicX.o: ${INCDIR}/racesX.h
magicX.o: ${INCDIR}/elevegX.h

mailX.o: ${INCDIR}/calenX.h
mailX.o: ${INCDIR}/rmailX.h

memoryX.o: ${INCDIR}/armyX.h
memoryX.o: ${INCDIR}/cityX.h
memoryX.o: ${INCDIR}/itemX.h
memoryX.o: ${INCDIR}/navyX.h
memoryX.o: ${INCDIR}/butesX.h
memoryX.o: ${INCDIR}/desigX.h
memoryX.o: ${INCDIR}/racesX.h
memoryX.o: ${INCDIR}/activeX.h
memoryX.o: ${INCDIR}/statusX.h
memoryX.o: ${INCDIR}/caravanX.h
memoryX.o: ${INCDIR}/displayX.h
memoryX.o: ${INCDIR}/dstatusX.h

miscX.o: ${INCDIR}/armyX.h
miscX.o: ${INCDIR}/desigX.h
miscX.o: ${INCDIR}/racesX.h
miscX.o: ${INCDIR}/elevegX.h
miscX.o: ${INCDIR}/nclassX.h
miscX.o: ${INCDIR}/tgoodsX.h
miscX.o: ${INCDIR}/displayX.h

moveX.o: ${INCDIR}/armyX.h
moveX.o: ${INCDIR}/moveX.h
moveX.o: ${INCDIR}/navyX.h
moveX.o: ${INCDIR}/desigX.h
moveX.o: ${INCDIR}/magicX.h
moveX.o: ${INCDIR}/elevegX.h
moveX.o: ${INCDIR}/hlightX.h
moveX.o: ${INCDIR}/statusX.h
moveX.o: ${INCDIR}/caravanX.h
moveX.o: ${INCDIR}/dstatusX.h

sectorX.o: ${INCDIR}/cityX.h
sectorX.o: ${INCDIR}/itemX.h
sectorX.o: ${INCDIR}/butesX.h
sectorX.o: ${INCDIR}/calenX.h
sectorX.o: ${INCDIR}/desigX.h
sectorX.o: ${INCDIR}/magicX.h
sectorX.o: ${INCDIR}/mtrlsX.h
sectorX.o: ${INCDIR}/racesX.h
sectorX.o: ${INCDIR}/worldX.h
sectorX.o: ${INCDIR}/elevegX.h
sectorX.o: ${INCDIR}/hlightX.h
sectorX.o: ${INCDIR}/statusX.h
sectorX.o: ${INCDIR}/tgoodsX.h

selectX.o: ${INCDIR}/armyX.h

unitsX.o: ${INCDIR}/armyX.h
unitsX.o: ${INCDIR}/cityX.h
unitsX.o: ${INCDIR}/itemX.h
unitsX.o: ${INCDIR}/navyX.h
unitsX.o: ${INCDIR}/butesX.h
unitsX.o: ${INCDIR}/calenX.h
unitsX.o: ${INCDIR}/desigX.h
unitsX.o: ${INCDIR}/magicX.h
unitsX.o: ${INCDIR}/mtrlsX.h
unitsX.o: ${INCDIR}/racesX.h
unitsX.o: ${INCDIR}/worldX.h
unitsX.o: ${INCDIR}/activeX.h
unitsX.o: ${INCDIR}/elevegX.h
unitsX.o: ${INCDIR}/statusX.h
unitsX.o: ${INCDIR}/nclassX.h
unitsX.o: ${INCDIR}/caravanX.h
unitsX.o: ${INCDIR}/dstatusX.h

jointA.o: ${INCDIR}/magicX.h

jointG.o: ${INCDIR}/armyX.h
jointG.o: ${INCDIR}/moveX.h
jointG.o: ${INCDIR}/desigX.h
jointG.o: ${INCDIR}/magicX.h
jointG.o: ${INCDIR}/spellsX.h
jointG.o: ${INCDIR}/tgoodsX.h
jointG.o: ${INCDIR}/displayX.h
jointG.o: ${INCDIR}/optionsX.h
#
