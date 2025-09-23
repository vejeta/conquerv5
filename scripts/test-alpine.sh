#!/bin/bash

# Find the APK file automatically
APK_PATH=$(find . .. ../.. -name "conquerv5-*.apk" -type f 2>/dev/null | head -1)

if [ -z "$APK_PATH" ]; then
    echo "ERROR: Could not find conquerv5 APK file"
    echo "Searched in current directory and parent directories"
    exit 1
fi

echo "Found APK: $APK_PATH"

# Get the directory containing the APK to mount
MOUNT_DIR=$(dirname "$APK_PATH")
APK_BASENAME=$(basename "$APK_PATH")

# Convert relative path to absolute for Docker mount
ABSOLUTE_MOUNT_DIR=$(cd "$MOUNT_DIR" && pwd)

echo "Mounting: $ABSOLUTE_MOUNT_DIR"
echo "Installing: /work/$APK_BASENAME"

# Start Alpine container with automatic APK detection
docker run --rm -it --platform=linux/amd64 \
    -v "$ABSOLUTE_MOUNT_DIR":/work \
    alpine:latest sh -c "
  # Install dependencies and the game
  apk add --no-cache ncurses-terminfo-base ncurses ncompress
  apk add --allow-untrusted /work/$APK_BASENAME

  # Debug what got installed
  echo '=== Checking installation ==='
  find /usr -name 'conq*' -type f 2>/dev/null || echo 'No conquer binaries found'
  ls -la /usr/bin/conq* 2>/dev/null || echo 'No binaries in /usr/bin'
  apk info conquerv5 || echo 'Package info not available'

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

  # Drop into interactive shell
  /bin/sh
"
