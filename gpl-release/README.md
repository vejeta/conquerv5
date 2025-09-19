*********************************************************
*	README FILE FOR THE INSTALLER OF CONQUER v5	*
*********************************************************

CONQUER - Classic Multi-Player Strategy Game

What you have here is a GPL v3 licensed version of the classic CONQUER game v5,
originally created by Edward M. Barlow and Adam Bryant. This version has been
relicensed to GPL v3 in 2025 by Juan Manuel Méndez Rey (Vejeta) with explicit
permission from the original authors.

LICENSING INFORMATION:
======================
- Originally Copyright (C) 1988-1989 by Edward M. Barlow and Adam Bryant
- Copyright (C) 2025 Juan Manuel Méndez Rey (Vejeta)
- Licensed under GPL v3 with permission from original authors

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version. See the COPYING file for the complete GPL v3 license text.

HISTORICAL NOTE:
================
This is based on the original version 5 beta release of CONQUER. Version 5 
introduced significant improvements including automatic data file conversion,
enhanced stability, and improved administrative tools. The original authors 
made no guarantees to the sanity or style of this code, but believed that it 
should work as documented.

The original development team included Edward M. Barlow and Adam Bryant, who
set up mailing lists and provided community support throughout the late 1980s
and early 1990s. Their innovative work created one of the early multi-player
strategy games that influenced many later games in the genre.

VERSION 5 ENHANCEMENTS:
======================
- **Automatic Data Conversion**: Built-in routines for converting old data files
- **Enhanced Stability**: Numerous bug fixes and stability improvements  
- **Better Administration**: Improved administrative tools and options
- **Expanded Configuration**: More flexible configuration system
- **Backward Compatibility**: Seamless upgrade path from earlier versions

CURRENT STATUS:
===============
This version maintains the original gameplay and functionality while being
made available under modern open-source licensing terms. Bug reports, feature
requests, and contributions are welcome through the project's repository.

INCLUDED IN THIS DISTRIBUTION:
===============================
	1) A Brief Description of Conquer v5
	2) Installation (unpacking) Instructions
	3) Configuration Instructions
	4) Compilation Instructions
	5) Administration instructions
	6) Data Conversion Guidelines
	7) GPL v3 License Information

-----------------------------------------------------------
I   A Brief Description of Conquer v5
-----------------------------------------------------------
A complete description of Conquer v5 is contained in the documentation files
and can be generated using "make docs". The game is a multi-player strategy 
simulation where players control nations, managing resources, armies, 
diplomacy, and territorial expansion.

Key features include:
- Multi-player strategy gameplay supporting multiple concurrent players
- Resource management (food, gold, metal, jewels)
- Military units, naval fleets, and advanced combat systems
- Magic system with spells, artifacts, and mystical powers
- Diplomatic relations, alliances, and treaties between nations
- NPC nations with sophisticated AI behavior
- Random events and dynamic world systems
- Customizable world generation and scenarios
- **NEW in v5**: Automatic data file conversion from earlier versions
- **Enhanced**: Improved administrative interface and tools

The documentation files txt[0-5] contain help information that is customized
from data in the header files and converted to help[0-5] files during
compilation. Use "make docs" to create documentation from current data.

-----------------------------------------------------------
II  Installation Instructions
-----------------------------------------------------------
SYSTEM REQUIREMENTS:
- Unix-like operating system (Linux, BSD, macOS, Solaris)
- C compiler (gcc recommended)
- make utility (GNU make preferred)
- curses library (ncurses recommended)
- Standard Unix utilities (sed, etc.)

INSTALLATION PROCESS:
1. Extract the source code to a directory
2. Review and modify configuration files (see Configuration section)
3. Configure build system: `make Makefiles`
4. Compile: `make build`
5. Install: `make install`
6. Set up new game: `conqrun -m`

If curses linking fails, you may need to modify the LIBS setting in the
Makefile to include appropriate terminal libraries for your system.

-----------------------------------------------------------
III Configuration
-----------------------------------------------------------
THE FOLLOWING FILES SHOULD BE MODIFIED TO REFLECT YOUR ENVIRONMENT
AND THE TYPE OF GAME YOU WISH TO PLAY:

REQUIRED MODIFICATIONS:
- **header.h**: Game configuration constants and system settings
- **Makefile**: Build configuration, paths, and compiler options

OPTIONAL MODIFICATIONS:
- **rules**: Grammar rules for NPC message generation
- **nations**: NPC nation configurations for world creation

The options specified in these files will be reflected in the documentation
and help files when the program is compiled.

CRITICAL CONFIGURATION STEPS:

1. **Copy header.h template**: `cp Include/header.h.dist Include/header.h`
2. **Edit Include/header.h** and configure:
   - `OWNER`: Administrator name and contact information
   - `LOGIN`: Administrator login ID  
   - Directory paths for game data and executables
   - System-specific settings (BSD vs SYSV flags)
   - Game balance and timing parameters

3. **Edit Makefile** and configure:
   - `SYSFLG`: Set appropriate system type (BSD, SYSV3, SYSV4, etc.)
   - `LIBS`: Adjust library settings for your system
   - `TOPDIR`: Full path to your conquer directory
   - `DATADIR`: Where game data will be stored
   - `BINDIR`: Where executables will be installed

SYSTEM FLAGS (choose one):
- `SYSFLG = -DBSD` for BSD derivatives (Linux, macOS, FreeBSD)
- `SYSFLG = -DSYSV4` for System V R4 derivatives  
- `SYSFLG = -DSYSV3` for System V R3 derivatives
- `SYSFLG = -DSUN41` for SunOS 4.1+
- `SYSFLG = -DULTRIX` for DEC Ultrix systems
- `SYSFLG = -DHPUX` for HP-UX systems
- `SYSFLG = -DAIX` for IBM AIX systems

The "rules" file contains grammar rules for random messages generated by
NPCs. You may customize this with local names or creative content following
the format: %CLASS declares a class, with %MAIN being the top level.

-----------------------------------------------------------
IV  Compilation Instructions
-----------------------------------------------------------
After configuring header.h and Makefile:

**Basic Build Process:**
```bash
# Configure all Makefiles
make Makefiles

# Build everything
make build

# Install executables and data files
make install

# Generate documentation
make docs
```

**Development Commands:**
```bash
make clean      # Remove object files and temporary files
make clobber    # Remove all generated files (complete cleanup)
```

**Important Notes:**
- Always run `make Makefiles` first to configure the build system
- The build process creates executables for both player and admin interfaces
- Object files (.o) are automatically cleaned from the source distribution

TROUBLESHOOTING:
- **Curses linking errors**: Add `-ltermcap` or `-lncurses` to LIBS in Makefile
- **Make command errors**: Try `setenv SHELL /bin/sh` or use bash
- **Permission errors**: Ensure all directory paths in header.h exist and are writable
- **Compiler warnings**: Modern compilers may show warnings; these are usually safe to ignore

-----------------------------------------------------------
V   Administration Instructions
-----------------------------------------------------------
COMMAND LINE ADMINISTRATION:

**conqrun options:**
```bash
conqrun -m          # Create/make a new world
conqrun -a          # Add new player to existing world  
conqrun -x          # Execute update (process turn)
conqrun -E          # Read and write datafile (for conversion)
conqrun -T          # Prevent logins (maintenance mode)
conqrun -d DIR      # Use specified game directory
conqrun -r SCENARIO # Read map from scenario files during world creation
```

**Version 5 Data Conversion:**
Conquer v5 includes automatic conversion routines for older data files:

1. **Before upgrading**: Run `conqrun -x` with the OLD version to update all *.exc files
2. **Prevent logins**: Use `conqrun -T` to block new connections during conversion
3. **Convert data**: With new v5 binaries in place, run `conqrun -E` to convert data files
4. **Alternative**: If you forgot step 1, remove all *.exc files before conversion

GAME ADMINISTRATION LEVELS:
1. **God**: Primary administrator (defined by LOGIN in header.h)
2. **Demi-God**: World-specific administrator (assigned during world creation)

SETUP PROCEDURE:

**1. Create World:**
```bash
conqrun -m
```
- Sets up the game world and NPC nations per the nations file
- NPCs initially use the same password as god
- Creates terrain, resources, and starting positions

**2. Add Players:**
```bash
conqrun -a
```
- Interactive player addition system
- Won't work if world is full from scenario loading
- Assigns starting nations and positions

**3. Alternative - Scenario-based Setup:**
```bash
conqrun -r SCENARIO
```
- Uses SCENARIO.ele, SCENARIO.veg, and SCENARIO.ntn files
- Pre-configured world with custom terrain and nations

GOD FUNCTIONS:
Access god functions by logging in with: `conquer -n god`
- Modify terrain and sectors directly
- Create, destroy, or modify nations
- Adjust game balance and parameters
- Monitor all player activities
- Emergency administrative controls

GAME BALANCE AND FAIRNESS:
The automatic world generation may not be perfectly balanced. As god, you can:
- Redesignate terrain to improve starting positions
- Destroy and recreate nations in better locations  
- Modify surrounding terrain for fairness
- Use change nation command to adjust player situations
- Balance resources and strategic opportunities

REGULAR OPERATION:
- **Players connect**: `conquer` (or `conquer -n playername`)
- **Process turns**: `conqrun -x` (typically automated via cron)
- **Monitor activity**: Use god login for administrative oversight
- **Handle issues**: Emergency admin commands available through god mode

AUTOMATED UPDATES:
Create a script for automated turn processing. Example cron entry:
```bash
# Process turns every 6 hours
0 */6 * * * /usr/local/games/bin/conqrun -x
```

Customize the timing based on your game's pace and player preferences.

-----------------------------------------------------------
VI  Data Conversion and Upgrading
-----------------------------------------------------------
**NEW IN VERSION 5**: Automatic data file conversion system

If upgrading from an earlier version of Conquer:

**Recommended Conversion Process:**
1. **Backup everything**: Copy your entire game directory
2. **Final update with old version**: Run `conqrun -x` with the previous version
3. **Enable maintenance mode**: Run `conqrun -T` to prevent new logins
4. **Install new binaries**: Replace executables with v5 versions
5. **Convert data**: Run `conqrun -E` to convert all data files
6. **Test**: Verify the conversion worked correctly
7. **Resume normal operation**: Remove maintenance mode

**Emergency Conversion:**
If you forgot to run the final update with the old version:
```bash
# Remove all .exc files and run conversion
rm *.exc
conqrun -E
```

**Troubleshooting Conversion:**
- Keep backups of original data files
- Test conversion on a copy first
- Contact community if problems arise
- Check file permissions after conversion

-----------------------------------------------------------
VII Contributing and Support
-----------------------------------------------------------
This open-source version welcomes contributions:

**Development Areas:**
- Bug reports and fixes
- Feature enhancements  
- Documentation improvements
- Platform compatibility updates
- Translation efforts
- Modern UI improvements

**Development Guidelines:**
- Maintain compatibility with original game mechanics
- Follow GPL v3 licensing terms
- Document all changes thoroughly
- Test on multiple platforms when possible
- Preserve the classic feel while modernizing code

**Getting Help:**
- **Repository**: https://github.com/vejeta/conquer
- **Issues**: Use GitHub issue tracker for bugs and features
- **Historical reference**: See HISTORY.md and documentation
- **Community**: Gaming and open source communities

**Legal Framework:**
This GPL v3 version is based on explicit written permissions from both
original authors (Ed Barlow and Adam Bryant). The complete legal documentation
is preserved in RELICENSING-PERMISSIONS.md.

The original creators laid the foundation for this classic strategy game.
This GPL v3 version ensures it remains available for future generations
of strategy game enthusiasts while respecting the original vision.

-----------------------------------------------------------
VIII Game Play Quick Reference
-----------------------------------------------------------
**Basic Commands:**
- `?` - Help system
- `m` - Move units
- `v` - View sectors
- `i` - Information displays
- `e` - Enlist new units
- `t` - Transfer resources
- `mail` - Communication system

**For detailed gameplay help:**
- Use the in-game `?` help system
- Generate documentation with `make docs`
- Read the man page and documentation files
- Join the community for strategy discussions

**Administrative Quick Start:**
```bash
# Create new world
conqrun -m

# Add a player  
conqrun -a

# Process a turn
conqrun -x

# Login as god
conquer -n god
```

-----------------------------------------------------------
For comprehensive information, see the complete documentation generated
by `make docs` and the in-game help system accessed with `?`.

This version represents the successful preservation and modernization of
a classic computer game, made possible through community effort and
proper legal channels.