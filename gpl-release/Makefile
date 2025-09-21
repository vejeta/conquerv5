# SPDX-License-Identifier: GPL-3.0-or-later
# SPDX-FileCopyrightText: 1987-1992 Ed Barlow <barlowedward@hotmail.com>
# SPDX-FileCopyrightText: 1987-1992 Adam Bryant <adb@usa.com>
# SPDX-FileCopyrightText: 2025 Juan Manuel Méndez Rey <vejeta@gmail.com>
#
# Conquer - Classic Multi-Player Strategy Game - Modernized Build Configuration
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
#
# MODERNIZED CROSS-PLATFORM MAKEFILE
#
# This modernized Makefile provides:
# - Automatic platform detection
# - Cross-platform compiler support (GCC, Clang, MSVC)
# - Modern compiler flags and optimizations
# - Improved dependency handling
# - Better error handling and diagnostics
# - Preserves all original functionality
#
# To configure all of the Makefiles, first copy this file to
# Makefile and then type "make Makefiles".
#
# Current Maintainer: Juan Manuel Méndez Rey <vejeta@gmail.com>
# Project Repository: https://github.com/vejeta/conquerv5
# Bug Reports: https://github.com/vejeta/conquerv5/issues

# ================================================================
# PLATFORM DETECTION
# ================================================================

# Detect operating system
UNAME_S := $(shell uname -s 2>/dev/null || echo Windows)
UNAME_M := $(shell uname -m 2>/dev/null || echo unknown)

# Platform-specific settings
ifeq ($(UNAME_S),Linux)
    PLATFORM = linux
    PLATFORM_FLAGS = -DPLATFORM_LINUX -DPLATFORM_UNIX
    PLATFORM_LIBS = -lcrypt
endif

ifeq ($(UNAME_S),Darwin)
    PLATFORM = macos
    PLATFORM_FLAGS = -DPLATFORM_MACOS -DPLATFORM_UNIX
    PLATFORM_LIBS =
endif

ifeq ($(UNAME_S),FreeBSD)
    PLATFORM = freebsd
    PLATFORM_FLAGS = -DPLATFORM_FREEBSD -DPLATFORM_UNIX
    PLATFORM_LIBS = -lcrypt
endif

ifeq ($(UNAME_S),OpenBSD)
    PLATFORM = openbsd
    PLATFORM_FLAGS = -DPLATFORM_OPENBSD -DPLATFORM_UNIX
    PLATFORM_LIBS =
endif

ifeq ($(UNAME_S),NetBSD)
    PLATFORM = netbsd
    PLATFORM_FLAGS = -DPLATFORM_NETBSD -DPLATFORM_UNIX
    PLATFORM_LIBS = -lcrypt
endif

ifneq ($(findstring CYGWIN,$(UNAME_S)),)
    PLATFORM = cygwin
    PLATFORM_FLAGS = -DPLATFORM_CYGWIN -DPLATFORM_UNIX
    PLATFORM_LIBS = -lcrypt
endif

ifneq ($(findstring MINGW,$(UNAME_S)),)
    PLATFORM = mingw
    PLATFORM_FLAGS = -DPLATFORM_WINDOWS
    PLATFORM_LIBS =
endif

# Default to Windows if no uname
ifeq ($(UNAME_S),Windows)
    PLATFORM = windows
    PLATFORM_FLAGS = -DPLATFORM_WINDOWS
    PLATFORM_LIBS =
endif

# ================================================================
# COMPILER DETECTION AND CONFIGURATION
# ================================================================

# Try to detect compiler
ifeq ($(CC),)
    CC := $(shell which gcc 2>/dev/null || which clang 2>/dev/null || which cc 2>/dev/null || echo gcc)
endif

# Compiler-specific flags
CC_VERSION := $(shell $(CC) --version 2>/dev/null | head -n1)

ifneq ($(findstring gcc,$(CC_VERSION)),)
    COMPILER = gcc
    COMPILER_FLAGS = -Wall -Wextra -Wshadow -Wpointer-arith -Wcast-qual
endif

ifneq ($(findstring clang,$(CC_VERSION)),)
    COMPILER = clang
    COMPILER_FLAGS = -Wall -Wextra -Wshadow -Wpointer-arith -Wcast-qual
endif

# Default compiler flags if detection fails
ifeq ($(COMPILER_FLAGS),)
    COMPILER_FLAGS = -Wall
endif

# ================================================================
# ADMINISTRATOR CONFIGURATION
# ================================================================

# Auto-detect current user (like v4)

# Auto-detect current user
CURRENT_USER := $(shell whoami 2>/dev/null || echo $(USER) 2>/dev/null || echo "admin")
LOGIN ?= $(CURRENT_USER)

# For sed substitution - just pass the plain username
LOGIN_USERNAME = $(LOGIN)

# Build type (can be overridden: make BUILD=release)
BUILD ?= debug

ifeq ($(BUILD),release)
    OPTIMIZATION = -O2 -DNDEBUG
    STRIP_BINARIES = yes
else ifeq ($(BUILD),debug)
    OPTIMIZATION = -g -O0 -DDEBUG
    STRIP_BINARIES = no
else
    OPTIMIZATION = -O1 -g
    STRIP_BINARIES = no
endif

# ================================================================
# DIRECTORY CONFIGURATION
# ================================================================

# Build tools
MAKE = make
MKDPND = makedepend
MKDIR = mkdir -p

# Shell (prefer bash if available)
SHELL := $(shell which bash 2>/dev/null || which sh 2>/dev/null || echo /bin/sh)

# Standard utilities
CD = cd
RM = rm -f
CP = cp
MV = mv
ECHO = echo
TOUCH = touch
NULL = 2>/dev/null

# Platform-specific utilities
ifeq ($(PLATFORM),windows)
    MKDIR = mkdir
    RM = del /F /Q
    CP = copy
    MV = move
    ECHO = echo
    NULL = >nul 2>&1
endif

# Strip command
ifeq ($(STRIP_BINARIES),yes)
    STRIP = strip
else
    STRIP = echo "Not stripping"
endif

# ================================================================
# SYSTEM FLAGS (Modernized)
# ================================================================

# Automatically determine system type
ifeq ($(PLATFORM),linux)
    SYSFLG = -DBSD $(PLATFORM_FLAGS)
else ifeq ($(PLATFORM),macos)
    SYSFLG = -DBSD $(PLATFORM_FLAGS)
else ifeq ($(PLATFORM),freebsd)
    SYSFLG = -DBSD $(PLATFORM_FLAGS)
else ifeq ($(PLATFORM),openbsd)
    SYSFLG = -DBSD $(PLATFORM_FLAGS)
else ifeq ($(PLATFORM),netbsd)
    SYSFLG = -DBSD $(PLATFORM_FLAGS)
else ifeq ($(PLATFORM),cygwin)
    SYSFLG = -DBSD $(PLATFORM_FLAGS)
else ifeq ($(PLATFORM),mingw)
    SYSFLG = $(PLATFORM_FLAGS)
else ifeq ($(PLATFORM),windows)
    SYSFLG = $(PLATFORM_FLAGS)
else
    # Default to BSD-style for unknown Unix systems
    SYSFLG = -DBSD -DPLATFORM_UNIX
endif

# ================================================================
# LIBRARY CONFIGURATION
# ================================================================

# Base libraries
BASE_LIBS = -lcurses

# Platform-specific libraries
ifeq ($(PLATFORM),linux)
    LIBS = $(BASE_LIBS) -ltermcap $(PLATFORM_LIBS)
else ifeq ($(PLATFORM),macos)
    LIBS = $(BASE_LIBS) -ltermcap $(PLATFORM_LIBS)
else ifeq ($(PLATFORM),freebsd)
    LIBS = $(BASE_LIBS) -ltermcap $(PLATFORM_LIBS)
else ifeq ($(PLATFORM),openbsd)
    LIBS = $(BASE_LIBS) -ltermcap $(PLATFORM_LIBS)
else ifeq ($(PLATFORM),netbsd)
    LIBS = $(BASE_LIBS) -ltermcap $(PLATFORM_LIBS)
else ifeq ($(PLATFORM),cygwin)
    LIBS = $(BASE_LIBS) -ltermcap $(PLATFORM_LIBS)
else ifeq ($(PLATFORM),mingw)
    LIBS = -lpdcurses $(PLATFORM_LIBS)
else ifeq ($(PLATFORM),windows)
    LIBS = -lpdcurses $(PLATFORM_LIBS)
else
    # Default Unix libraries
    LIBS = $(BASE_LIBS) -ltermcap -lcrypt
endif

# ================================================================
# COMPILER FLAGS
# ================================================================

# Base flags
BASE_CFLAGS = $(COMPILER_FLAGS) $(OPTIMIZATION) $(SYSFLG)

# Standard compliance
STD_FLAGS = -std=c99

# Debug/Release specific flags
ifeq ($(BUILD),release)
    PERF_FLAGS = -fomit-frame-pointer -ffast-math
    CFLAGS = $(BASE_CFLAGS) $(STD_FLAGS) $(PERF_FLAGS)
else
    DEBUG_FLAGS = -fno-omit-frame-pointer -fstack-protector-strong
    ifeq ($(COMPILER),gcc)
        DEBUG_FLAGS += -fsanitize=address -fsanitize=undefined
    endif
    CFLAGS = $(BASE_CFLAGS) $(STD_FLAGS) $(DEBUG_FLAGS)
endif

# Special flags for checkX.c (some compilers have issues with optimization)
CKFLAGS = $(CFLAGS)
ifeq ($(COMPILER),gcc)
    ifneq ($(findstring sparc,$(UNAME_M)),)
        CKFLAGS = $(filter-out -O%,$(CFLAGS)) -O0
    endif
endif

# ================================================================
# INSTALLATION PATHS
# ================================================================

# Installation ownership and permissions
ifneq ($(shell id -u 2>/dev/null),0)
    # Non-root installation
    CHOWN = echo "... ignore (not root)"
    CHMOD = chmod
    PREFIX ?= $(HOME)/.local
else
    # Root installation
    CHOWN = chown games:games
    CHMOD = chmod
    PREFIX ?= /usr/local
endif

# Directory paths
TOPDIR = $(PWD)
DATADIR = $(PREFIX)/share/conquer
BINDIR = $(PREFIX)/bin

# Source directories
INCDIR = Include
SRCDIR = Src
AUXDIR = Auxil
DOCDIR = Docs

# Binary names
CQUER = conquer
CQRUN = conqrun
CQSORT = conqsort
CXTRACT = cextract

# Documentation tools
NROFF = nroff
TROFF = psroff -t

# ================================================================
# Header files
# ================================================================
HEADER_DIST := $(INCDIR)/header.h.dist
HEADER := $(INCDIR)/header.h

# ================================================================
# OPTIONAL FEATURES
# ================================================================

# Uncomment if you don't have getopt in your library
#GETOPT = getopt.c
#GETOBJ = getopt.o

# For VMS systems without mvprintw command
#VMSFILE = vms.c
#VMSOBJ = vms.o

# ================================================================
# SOURCE FILES (preserved from original)
# ================================================================

# GFILS are files only needed for the user interface
GFILS = mainG.c armyG.c caravanG.c customG.c dataG.c displayG.c \
emailG.c enlistG.c hexmapG.c ieditG.c infoG.c ioG.c iodataG.c \
keybindG.c magicG.c mailG.c miscG.c moveG.c navyG.c ntninfoG.c \
pagerG.c regionG.c sectorG.c selectG.c xferG.c time_ckG.c

# AFILS are files only needed for the administration program
AFILS = mainA.c adduserA.c combatA.c configA.c createA.c dataA.c \
economyA.c magicA.c mailA.c miscA.c monsterA.c moveA.c npcA.c sectorA.c \
updateA.c

# XFILS are files used in both programs
XFILS = dataX.c datamagX.c datamilX.c checkX.c computeX.c convertX.c \
customX.c executeX.c hexmapX.c ioX.c iodataX.c magicX.c mailX.c \
memoryX.c miscX.c moveX.c sectorX.c selectX.c unitsX.c $(GETOPT) $(VMSFILE)

# Object files
GOBJS = $(GFILS:.c=.o)
AOBJS = $(AFILS:.c=.o)
XOBJS = $(XFILS:.c=.o) $(GETOBJ) $(VMSOBJ)

# ================================================================
# TARGETS
# ================================================================

.PHONY: all build install clean clobber Makefiles mkfiles makefiles info help

all:
	@echo "Conquer v5 Build System (Modernized)"
	@echo "Platform: $(PLATFORM) ($(UNAME_S)/$(UNAME_M))"
	@echo "Compiler: $(CC) ($(COMPILER))"
	@echo "Build:    $(BUILD)"
	@echo ""
	@echo "Available targets:"
	@echo "   make build       -- build everything"
	@echo "   make install     -- install everything"
	@echo "   make clean       -- clean up build files"
	@echo "   make clobber     -- clean up all generated files"
	@echo "   make Makefiles   -- configure all Makefiles properly"
	@echo "   make info        -- show build configuration"
	@echo "   make help        -- show detailed help"
	@echo ""
	@echo "Build options:"
	@echo "   make BUILD=release  -- optimized release build"
	@echo "   make BUILD=debug    -- debug build (default)"
	@echo "   make PREFIX=/path   -- set installation prefix"

info:
	@echo "Build Configuration:"
	@echo "  Platform:     $(PLATFORM)"
	@echo "  System:       $(UNAME_S) $(UNAME_M)"
	@echo "  Compiler:     $(CC)"
	@echo "  Build Type:   $(BUILD)"
	@echo "  Optimization: $(OPTIMIZATION)"
	@echo "  Flags:        $(CFLAGS)"
	@echo "  Libraries:    $(LIBS)"
	@echo "  Prefix:       $(PREFIX)"
	@echo "  Data Dir:     $(DATADIR)"
	@echo "  Bin Dir:      $(BINDIR)"

help:
	@echo "Conquer v5 Modernized Build System"
	@echo ""
	@echo "This build system automatically detects your platform and compiler,"
	@echo "then configures appropriate flags and libraries."
	@echo ""
	@echo "Supported Platforms:"
	@echo "  - Linux (with GCC/Clang)"
	@echo "  - macOS (with GCC/Clang)"
	@echo "  - FreeBSD/OpenBSD/NetBSD"
	@echo "  - Windows (with MinGW/MSYS2)"
	@echo "  - Cygwin"
	@echo ""
	@echo "Build Types:"
	@echo "  debug   - Debug build with symbols and sanitizers"
	@echo "  release - Optimized build for distribution"
	@echo ""
	@echo "Examples:"
	@echo "  make                    # Show this help"
	@echo "  make info               # Show build configuration"
	@echo "  make Makefiles          # Configure build system"
	@echo "  make build              # Build with debug settings"
	@echo "  make BUILD=release build # Build optimized version"
	@echo "  make PREFIX=/opt/conquer install # Install to /opt/conquer"

# Build within each directory
build:
	@echo "Building Conquer v5 ($(BUILD) mode on $(PLATFORM))..."
	($(CD) $(SRCDIR) && $(MAKE) build)
	($(CD) $(AUXDIR) && $(MAKE) build)
	($(CD) $(DOCDIR) && $(MAKE) build)
	@echo "Build complete!"


# Generate header.h from header.h.dist replacing the LOGIN line.
# This awk script handles:
#  - a line like: #define LOGIN "foo"
# and otherwise preserves the file.
$(HEADER): $(HEADER_DIST)
	@echo "Generating $@ from $< (LOGIN=$(LOGIN))"
	@awk -v login="$(LOGIN)" '
		/^#define[ \t]+LOGIN/ {
			print "#define LOGIN \"" login "\""
			next
		}
		{ print }
	' $< > $@

# Install within each directory
install:
	@echo "Installing Conquer v5 to $(PREFIX)..."
	-$(MKDIR) $(DATADIR) $(BINDIR)
	($(CD) $(SRCDIR) && $(MAKE) install)
	($(CD) $(AUXDIR) && $(MAKE) install)
	($(CD) $(DOCDIR) && $(MAKE) install)
	@echo "Installation complete!"
	@echo "Data directory: $(DATADIR)"
	@echo "Binaries: $(BINDIR)"
	@echo ""
	@echo "You may now build the world with: $(BINDIR)/$(CQRUN) -m"

# Clean up within each directory
clean:
	@echo "Cleaning build files..."
	-($(CD) $(INCDIR) && $(MAKE) clean $(NULL))
	-($(CD) $(SRCDIR) && $(MAKE) clean $(NULL))
	-($(CD) $(AUXDIR) && $(MAKE) clean $(NULL))
	-($(CD) $(DOCDIR) && $(MAKE) clean $(NULL))
	-$(RM) *.o *~ \#* *.bak $(NULL)

# Really clean up within each directory
clobber:
	@echo "Cleaning all generated files..."
	-($(CD) $(INCDIR) && $(MAKE) clobber $(NULL))
	-($(CD) $(SRCDIR) && $(MAKE) clobber $(NULL))
	-($(CD) $(AUXDIR) && $(MAKE) clobber $(NULL))
	-($(CD) $(DOCDIR) && $(MAKE) clobber $(NULL))
	-$(RM) $(INCDIR)/Makefile $(SRCDIR)/Makefile $(NULL)
	-$(RM) $(AUXDIR)/Makefile $(DOCDIR)/Makefile $(NULL)
	-$(RM) $(INCDIR)/header.h sed.out $(NULL)

# Build all of the Makefiles
mkfiles:
	@echo "Configuring Makefiles for $(PLATFORM)..."
	@echo 's:%%MAKE%%:$(MAKE):g' > sed.out
	@echo 's:%%MKDPND%%:$(MKDPND):g' >> sed.out
	@echo 's:%%MKDIR%%:$(MKDIR):g' >> sed.out
	@echo 's:%%CD%%:$(CD):g' >> sed.out
	@echo 's:%%CC%%:$(CC):g' >> sed.out
	@echo 's:%%RM%%:$(RM):g' >> sed.out
	@echo 's:%%CP%%:$(CP):g' >> sed.out
	@echo 's:%%MV%%:$(MV):g' >> sed.out
	@echo 's:%%ECHO%%:$(ECHO):g' >> sed.out
	@echo 's:%%SHELL%%:$(SHELL):g' >> sed.out
	@echo 's:%%STRIP%%:$(STRIP):g' >> sed.out
	@echo 's:%%NULL%%:$(NULL):g' >> sed.out
	@echo 's:%%TOUCH%%:$(TOUCH):g' >> sed.out
	@echo 's:%%CHOWN%%:$(CHOWN):g' >> sed.out
	@echo 's:%%CHMOD%%:$(CHMOD):g' >> sed.out
	@echo 's:%%LOGIN%%:$(LOGIN):g' >> sed.out
    @echo 's:%%LOGIN%%:$(LOGIN_USERNAME):g' >> sed.out
	@echo 's:%%LIBS%%:$(LIBS):g' >> sed.out
	@echo 's:%%SYSFLG%%:$(SYSFLG):g' >> sed.out
	@echo 's:%%CFLAGS%%:$(CFLAGS):g' >> sed.out
	@echo 's:%%CKFLAGS%%:$(CKFLAGS):g' >> sed.out
	@echo 's:%%TOPDIR%%:$(TOPDIR):g' >> sed.out
	@echo 's:%%DATADIR%%:$(DATADIR):g' >> sed.out
	@echo 's:%%BINDIR%%:$(BINDIR):g' >> sed.out
	@echo 's:%%INCDIR%%:$(INCDIR):g' >> sed.out
	@echo 's:%%SRCDIR%%:$(SRCDIR):g' >> sed.out
	@echo 's:%%AUXDIR%%:$(AUXDIR):g' >> sed.out
	@echo 's:%%DOCDIR%%:$(DOCDIR):g' >> sed.out
	@echo 's:%%CQUER%%:$(CQUER):g' >> sed.out
	@echo 's:%%CQRUN%%:$(CQRUN):g' >> sed.out
	@echo 's:%%CQSORT%%:$(CQSORT):g' >> sed.out
	@echo 's:%%CXTRACT%%:$(CXTRACT):g' >> sed.out
	@echo 's:%%NROFF%%:$(NROFF):g' >> sed.out
	@echo 's:%%TROFF%%:$(TROFF):g' >> sed.out
	@echo 's:%%GFILS%%:$(GFILS):g' >> sed.out
	@echo 's:%%AFILS%%:$(AFILS):g' >> sed.out
	@echo 's:%%XFILS%%:$(XFILS):g' >> sed.out
	@echo 's:%%GOBJS%%:$(GOBJS):g' >> sed.out
	@echo 's:%%AOBJS%%:$(AOBJS):g' >> sed.out
	@echo 's:%%XOBJS%%:$(XOBJS):g' >> sed.out
	-sed -f sed.out $(INCDIR)/Makefile.inc > $(INCDIR)/Makefile
	-sed -f sed.out $(SRCDIR)/Makefile.src > $(SRCDIR)/Makefile
	-sed -f sed.out $(DOCDIR)/Makefile.dcm > $(DOCDIR)/Makefile
	-sed -f sed.out $(AUXDIR)/Makefile.aux > $(AUXDIR)/Makefile
	$(RM) sed.out $(NULL)
	@echo "Makefiles configured successfully!"

# Aliases for makefile generation
makefiles: mkfiles
Makefiles: mkfiles

# Ensure header exists before configuring or building
Makefiles: $(HEADER)
build: $(HEADER)

# ================================================================
# DEVELOPMENT TARGETS
# ================================================================

.PHONY: test dist package

# Run basic build test
test: build
	@echo "Running basic build test..."
	@if [ -f $(SRCDIR)/$(CQUER) ]; then \
		echo "✓ User interface built successfully"; \
	else \
		echo "✗ User interface build failed"; \
		exit 1; \
	fi
	@if [ -f $(SRCDIR)/$(CQRUN) ]; then \
		echo "✓ Admin interface built successfully"; \
	else \
		echo "✗ Admin interface build failed"; \
		exit 1; \
	fi
	@echo "All tests passed!"

# Show compiler and system information
sysinfo:
	@echo "System Information:"
	@echo "  OS: $(shell uname -a 2>/dev/null || echo 'Unknown')"
	@echo "  Compiler: $(shell $(CC) --version 2>/dev/null | head -n1 || echo 'Unknown')"
	@echo "  Make: $(shell $(MAKE) --version 2>/dev/null | head -n1 || echo 'Unknown')"
	@echo "  Shell: $(SHELL)"
	@echo ""
	@echo "Feature Detection:"
	@echo "  Curses: $(shell pkg-config --exists ncurses && echo 'ncurses' || echo 'unknown')"
	@echo "  Termcap: $(shell test -f /usr/lib/libtermcap.* && echo 'yes' || echo 'no')"
	@echo "  Crypt: $(shell test -f /usr/lib/libcrypt.* && echo 'yes' || echo 'no')"

# ================================================================
# MODERNIZATION NOTES
# ================================================================

# This modernized Makefile provides:
#
# 1. Automatic platform detection for Linux, macOS, *BSD, Windows
# 2. Compiler detection (GCC, Clang, others)
# 3. Modern compiler flags with security enhancements
# 4. Debug/Release build configurations
# 5. Improved error handling and user feedback
# 6. Sanitizer support for development builds
# 7. Cross-platform library detection
# 8. Modern installation paths with user/system support
# 9. Better documentation and help system
# 10. Backward compatibility with original structure
#
# All original functionality is preserved while adding modern
# cross-platform build capabilities.
