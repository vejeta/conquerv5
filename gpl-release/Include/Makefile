# Include/Makefile.inc - Template for Include directory Makefile

SHELL = /opt/local/bin/bash
CD = cd
CC = cc
RM = rm -f
CP = cp
ECHO = echo
NULL = 2>/dev/null

# Administrator settings
LOGIN = jmy99
CFLAGS = -Wall -Wextra -Wshadow -Wpointer-arith -Wcast-qual -g -O0 -DDEBUG -DBSD -DPLATFORM_MACOS -DPLATFORM_UNIX -DLOGIN="jmy99" -std=c99 -fno-omit-frame-pointer -fstack-protector-strong

# Build target - generate header.h from header.h.dist
all: build

build: header.h

# Generate header.h from template using awk (NOT compiler flags)
header.h: header.h.dist
	@$(ECHO) "Generating header.h for administrator: $(LOGIN)"
	@awk -v login="$(LOGIN)" '
		/^#define[ \t]+LOGIN/ {
			print "#define LOGIN \"" login "\""
			next
		}
		{ print }
	' header.h.dist > header.h
	@$(ECHO) "Header file generated successfully"

clean:
	@$(ECHO) "Include directory: cleaning..."

clobber: clean
	-$(RM) header.h $(NULL)
	@$(ECHO) "Include directory: cleaned"

install:
	@$(ECHO) "Include directory: nothing to install"

info:
	@$(ECHO) "Include directory configuration:"
	@$(ECHO) "  Administrator: $(LOGIN)"

.PHONY: all build clean clobber install info
