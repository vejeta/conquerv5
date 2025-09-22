#!/bin/bash
# ConquerV5 Debian package build script

set -e

WORK_DIR="/work"
BUILD_DIR="/home/builder/build"
OUTPUT_DIR="/work/packages/debian"

echo "=== Starting ConquerV5 Debian package build ==="

# Create build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Create build directory and copy source
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Copy the local repository instead of cloning
echo "Copying ConquerV5 source..."
cp -r "$WORK_DIR" conquerv5-5.0
cd conquerv5-5.0/gpl-release

# Copy Debian packaging files
echo "Setting up Debian packaging..."
mkdir -p debian
cp -r "$WORK_DIR/packaging/debian/control/"* debian/
chmod +x debian/rules

# Skip build testing - let debuild handle it
echo "=== Skipping build test - debuild will handle the building ==="

# Build the package
echo "=== Building Debian package ==="
export LEGACY_CFLAGS="-std=gnu99 -D_GNU_SOURCE -Wno-implicit-function-declaration -Wno-incompatible-pointer-types -Wno-implicit-int -Wno-return-type -Wno-old-style-definition -Wno-unused-variable -Wno-unused-function -Wno-format"
export DEB_CFLAGS_APPEND="$LEGACY_CFLAGS"

debuild -us -uc -b

# Create output directory and copy packages
mkdir -p "$OUTPUT_DIR"
cp ../*.deb "$OUTPUT_DIR/"
cp ../*.changes "$OUTPUT_DIR/" || true

echo "=== Package build complete ==="
ls -la "$OUTPUT_DIR/"

# Verify package contents
DEB_FILE=$(find "$OUTPUT_DIR" -name "*.deb" | head -1)
if [ -n "$DEB_FILE" ]; then
    echo "=== Package verification ==="
    echo "Package: $DEB_FILE"
    dpkg-deb --info "$DEB_FILE"
    echo ""
    echo "Contents (first 20 files):"
    dpkg-deb --contents "$DEB_FILE" | head -20
fi
