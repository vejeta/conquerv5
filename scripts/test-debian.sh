#!/bin/bash
# Equivalent to your test-alpine.sh but for Debian

set -e

# Change to repo root directory
SCRIPT_DIR="$(dirname "$0")"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
cd "$REPO_ROOT"

echo "Debug: Script location: $SCRIPT_DIR"
echo "Debug: Repo root: $REPO_ROOT"
echo "Debug: Current PWD: $PWD"

# Check if packages exist
PACKAGE_DIR="$REPO_ROOT/packages/debian"
echo "Debug: Looking for packages in: $PACKAGE_DIR"

if [ ! -d "$PACKAGE_DIR" ]; then
    echo "Error: Package directory $PACKAGE_DIR not found"
    echo "Please run scripts/build-debian.sh first"
    echo ""
    echo "Debug: Contents of packages directory:"
    ls -la "$REPO_ROOT/packages/" 2>/dev/null || echo "packages/ directory doesn't exist"
    exit 1
fi

# Find the .deb package (architecture-agnostic)
DEB_FILE=$(find "$PACKAGE_DIR" -name "conquerv5_*.deb" | head -1)
if [ -z "$DEB_FILE" ]; then
    echo "Error: No .deb package found in $PACKAGE_DIR"
    echo "Available files:"
    ls -la "$PACKAGE_DIR" 2>/dev/null || echo "Directory is empty"
    exit 1
fi

echo "Found package: $DEB_FILE"

# Get the container path (relative to /work mount point)
REL_PATH="${DEB_FILE#$REPO_ROOT/}"
CONTAINER_DEB_PATH="/work/$REL_PATH"

echo "Debug: Relative path: $REL_PATH"
echo "Debug: Container path: $CONTAINER_DEB_PATH"

# Start Debian container with the DEB available
docker run --rm -it -v "$REPO_ROOT":/work debian:trixie bash -c "
  # Update package list and install dependencies
  apt-get update
  apt-get install -y libncurses6 ncurses-base

  # Install the game package (using the container path)
  echo 'Installing package: $CONTAINER_DEB_PATH'
  dpkg -i '$CONTAINER_DEB_PATH' || true
  apt-get install -f -y  # Fix any dependency issues

  # Set up environment
  export TERM=xterm
  export HOME=/tmp/conquer-test
  mkdir -p \$HOME

  # Show what's available
  echo '=== Game installed! Available commands: ==='
  echo 'conquer -h    # Game help'
  echo 'conqrun -h    # Admin help'
  echo 'conquer       # Start the game'
  echo 'conqrun -m    # Create a world (admin)'
  echo ''
  echo 'Package info:'
  dpkg -l | grep conquerv5
  echo ''
  echo 'Installed files:'
  dpkg -L conquerv5 | head -10
  echo ''

  # Drop into interactive shell
  /bin/bash
"
