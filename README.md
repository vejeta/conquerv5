[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![Relicensed: 2025](https://img.shields.io/badge/Relicensed-2025-green.svg)](./gpl-release/RELICENSING-PERMISSIONS.md)

# Conquer - Classic Multi-Player Strategy Game

This repository contains both the original and GPL-licensed versions of Conquer v5, the classic Unix strategy game originally created by Edward M. Barlow and Adam Bryant.

## Repository Structure

```
conquer-v5/
├── README.md                    # This file
├── LICENSE-NOTICE.md           # Dual licensing explanation
├── original/                   # Original distribution (restrictive license)
│   ├── README                  # Original README file
│   ├── Makefile               # Original build configuration
│   ├── CONVERTING             # Data conversion instructions
│   └── [complete original source tree]
├── gpl-release/               # GPLv3 licensed version
│   ├── README.md              # Modern documentation
│   ├── LICENSE                # GPL v3 license text
│   ├── AUTHORS.md             # Contributors and attribution
│   ├── RELICENSING-PERMISSIONS.md  # Legal documentation
│   └── [clean source distribution]
└── docs/
    ├── HISTORY.md             # Project timeline
    └── relicensing-history.md # Detailed legal process
```

## What is Conquer?

Conquer is a classic multi-player strategy game set in Middle Earth, originally distributed via USENET in the late 1980s. Players control nations, managing resources, armies, diplomacy, and territorial expansion in a rich fantasy world.

**Key Features:**
- Multi-player strategy gameplay supporting multiple nations
- Resource management (food, gold, metal, jewels)
- Military units, naval fleets, and siege warfare
- Magic system with spells and artifacts
- Diplomatic relations and alliances
- NPC nations with AI behavior
- Random events and dynamic world systems
- Customizable world generation

## Version Information

This repository contains **Conquer v5**, which includes:
- Enhanced data file conversion system
- Improved stability and bug fixes
- Better administrative tools
- Expanded configuration options
- All features from previous versions

## Licensing Information

**Important:** This repository contains software under two different licenses:

### Original Distribution (`original/` folder)
- **License**: Original restrictive license (see `original/README`)
- **Purpose**: Historical preservation
- **Use**: Reference and historical study only

### GPL Release (`gpl-release/` folder)  
- **License**: GNU General Public License v3.0+
- **Purpose**: Modern open-source development
- **Use**: Free to use, modify, and distribute under GPL terms

**Legal Foundation**: Both original authors (Ed Barlow and Adam Bryant) have provided explicit written permission for GPL relicensing. See `RELICENSING-PERMISSIONS.md` for complete documentation.

## Quick Start

### For Players and Modern Development
Use the **GPL version** in the `gpl-release/` folder:

```bash
cd gpl-release/
make Makefiles
make build
make install
```

### For Historical Study
The **original version** in the `original/` folder preserves the exact distribution as received, including build artifacts.

## Getting Started

1. **Choose your version**: GPL for development/play, original for historical reference
2. **Read the documentation**: Each folder contains appropriate README files
3. **Configure and build**: Follow the build instructions in your chosen version
4. **Set up a game**: Use `conqrun -m` to create a new world

## Documentation

- **Modern Documentation**: See `gpl-release/README.md` for current instructions
- **Original Documentation**: See `original/README` for historical reference  
- **Project History**: See `docs/HISTORY.md` for complete timeline
- **Legal Framework**: See `RELICENSING-PERMISSIONS.md` for licensing details

## Community and Support

- **Repository**: https://github.com/vejeta/conquer
- **Issue Tracker**: GitHub Issues (for GPL version)
- **Historical Reference**: http://vejeta.com/historia-del-conquer/
- **Original USENET**: comp.sources.games archives (historical)

## Contributing

Contributions are welcome for the **GPL version** only:

1. Use code from the `gpl-release/` folder
2. Follow GPL v3.0+ licensing terms
3. Submit pull requests with clear descriptions
4. Maintain compatibility with the original game mechanics
5. Update documentation as needed

**Note**: Do not modify files in the `original/` folder - they are preserved for historical accuracy.

## Historical Significance

Conquer represents one of the early multi-player computer strategy games, distributed through USENET when source code sharing was done manually across newsgroups. This preservation effort ensures the game remains available for:

- **Gaming historians** studying early computer game development
- **Developers** learning from classic game design
- **Players** experiencing retro gaming
- **Students** understanding Unix game programming techniques

## Legal Notes

This relicensing effort took place over 15+ years (2006-2025) and involved:
- Tracking down original authors via internet searches
- Obtaining explicit written permissions from all copyright holders
- Following Debian Legal guidelines for proper documentation
- Creating transparent legal framework for ongoing development

The result is a legally sound GPL release while maintaining complete historical preservation.

## Acknowledgments

- **Original Authors**: Ed Barlow and Adam Bryant
- **GPL Coordinator**: Juan Manuel Méndez Rey (Vejeta)
- **Community**: Debian Legal, GNU Savannah, USENET gaming community
- **Additional Contributors**: See `AUTHORS.md` for complete list

---

**Choose your path**: `gpl-release/` for modern development, `original/` for history.

For detailed instructions, see the README file in your chosen folder.
