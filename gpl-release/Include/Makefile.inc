#	conquer: Copyright (c) 1991 by Edward M Barlow and Adam Bryant
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
MAKE	= %%MAKE%%
MKDPND	= %%MKDPND%%
MKDIR	= %%MKDIR%%
SHELL	= %%SHELL%%
CD	= %%CD%%
CC	= %%CC%%
RM      = %%RM%%
CP	= %%CP%%
MV	= %%MV%%
ECHO	= %%ECHO%%
STRIP	= %%STRIP%%
NULL	= %%NULL%%
TOUCH	= %%TOUCH%%
CHOWN	= %%CHOWN%%
CHMOD	= %%CHMOD%%
LIBS	= %%LIBS%%
SYSFLG	= %%SYSFLG%%
CFLAGS	= %%CFLAGS%%
TOPDIR	= %%TOPDIR%%
DATADIR	= %%DATADIR%%
BINDIR	= %%BINDIR%%
INCDIR	= %%TOPDIR%%/%%INCDIR%%
SRCDIR	= %%TOPDIR%%/%%SRCDIR%%
AUXDIR	= %%TOPDIR%%/%%AUXDIR%%
DOCDIR	= %%TOPDIR%%/%%DOCDIR%%
CQUER	= %%CQUER%%
CQRUN	= %%CQRUN%%
CQSORT	= %%CQSORT%%
CXTRACT	= %%CXTRACT%%
GFILS	= %%GFILS%%
AFILS	= %%AFILS%%
XFILS	= %%XFILS%%
GOJBS	= %%GOBJS%%
AOBJS	= %%AOBJS%%
XOBJS	= %%XOBJS%%

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
