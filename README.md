[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![Relicensed: 2025](https://img.shields.io/badge/Relicensed-2025-green.svg)](./gpl-release/RELICENSING-PERMISSIONS.md)

# Conquer - Classic Multi-Player Strategy Game

This repository contains both the original and GPL-licensed versions of Conquer v5, the classic Unix strategy game originally created by Edward M. Barlow and Adam Bryant.

## Repository Structure

```
conquer-v5/
├── README.md                  # This file
├── LICENSE-NOTICE.md          # Dual licensing explanation
├── original/                  # Original distribution (restrictive license)
│   ├── README                 # Original README file
│   ├── Makefile               # Original build configuration
│   ├── CONVERTING             # Data conversion instructions
│   └── [complete original source tree]
├── gpl-release/               # GPLv3 licensed version
│   ├── README.md              # Modern documentation
│   ├── LICENSE                # GPL v3 license text
│   ├── AUTHORS.md             # Contributors and attribution
|   ├── HISTORY.md             # Project timeline
│   ├── RELICENSING-PERMISSIONS.md  # Legal documentation
│   └── [clean source distribution]
|
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

## GPL Relicensing History

The transformation from restrictive to GPL licensing represents one of the most comprehensive retroactive relicensing efforts in gaming history, spanning nearly two decades (2006-2025).

### Original Distribution Context

Conquer was originally distributed via USENET newsgroups starting **October 26, 1987**, under restrictive terms that prohibited redistribution and modification. This was typical for software distributed in the pre-GPL era when "copyleft" licensing didn't exist.

### Relicensing Timeline

**2006-2011**: Initial contact and negotiations
- Juan Manuel Méndez Rey (Vejeta) began tracking down original authors
- Extensive internet searches to locate Ed Barlow and Adam Bryant
- Initial discussions about open-source licensing

**2011-2016**: Author permissions secured
- **Adam Bryant (2011)**: *"Just wanted to confirm that I had no issues with publication of version 4 of Conquer under the GPL."*
- **Ed Barlow (2016)**: *"Yes i delegated it all to adam aeons ago. Im easy on it all.... copyleft didnt exist when i wrote it and it was all for fun so..."*

**2025**: Final contributor permissions
- **Martin Forssen (2025)**: *"And I have no problem with relicensing it to GPL."* (PostScript utilities contributor)

### Legal Foundation

**Complete Documentation**: The full, unedited email correspondence containing explicit relicensing permissions from all copyright holders is preserved in [`gpl-release/RELICENSING-PERMISSIONS.md`](gpl-release/RELICENSING-PERMISSIONS.md) for legal compliance and transparency.

This approach follows **Debian Legal guidelines** for proper open-source licensing:
- Preserve complete written permissions from copyright holders
- Maintain transparent legal framework for ongoing development
- Document the entire relicensing process with verifiable evidence

### Community Involvement

The relicensing effort involved extensive community discussion:
- **Debian Legal mailing lists**: Legal framework validation
- **GNU Savannah**: Project hosting and GPL compliance
- **USENET gaming community**: Historical context and source verification
- **Academic institutions**: Historical preservation efforts

### Technical Restoration

Beyond legal licensing, the project involved significant technical work:
- **Source code extraction**: Recovery from original USENET posts
- **Build system modernization**: Cross-platform compilation support
- **Code archaeology**: Understanding 1987-era Unix programming techniques
- **Community contribution**: **quixadhal (Dread Quixadhal)** provided crucial assistance with source compilation

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
- **Legal Framework**: See `gpl-release/RELICENSING-PERMISSIONS.md` for licensing details

## Community and Support

- **Repository**: https://github.com/vejeta/conquerv5
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
- **Legal scholars** studying retroactive open-source licensing

## The Relicensing Legacy

This project demonstrates that even decades-old proprietary software can be successfully transitioned to open-source licensing through:

1. **Persistent effort**: Nearly 20 years of patient work
2. **Legal diligence**: Proper documentation and community consultation
3. **Technical preservation**: Maintaining both historical and modern versions
4. **Community support**: Involving legal experts and gaming historians
5. **Transparent process**: Complete documentation of permissions and procedures

The result provides a **legally sound GPL release** while maintaining **complete historical preservation** - a model for other retroactive relicensing efforts.

## Acknowledgments

- **Original Authors**: Ed Barlow and Adam Bryant (1987-1988)
- **PostScript Contributor**: Martin Forssen (1989)
- **GPL Relicensing Coordinator**: Juan Manuel Méndez Rey (Vejeta) (2006-present)
- **Technical Assistance**: quixadhal (Dread Quixadhal)
- **Legal Guidance**: Debian Legal community
- **Historical Preservation**: USENET gaming community, GNU Savannah
- **Additional Contributors**: See `gpl-release/AUTHORS.md` for complete list

---

**Choose your path**: `gpl-release/` for modern development, `original/` for history.

For detailed instructions, see the README file in your chosen folder.
