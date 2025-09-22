#!/bin/bash
# ConquerV5 Debian package build script (works for both local development and CI)

set -e

# Change to repo root directory
cd "$(dirname "$0")/.."

# Build the Docker image if it doesn't exist
if ! docker images | grep -q conquerv5-debian-builder; then
    echo "Building ConquerV5 Debian packaging Docker image..."
    docker build -t conquerv5-debian-builder packaging/debian/docker/
fi

# Build the Debian package using local repository
echo "Building ConquerV5 Debian package..."
docker run --privileged --rm \
    -v "$PWD":/work \
    -w /work \
    conquerv5-debian-builder \
    /work/packaging/debian/docker/build.sh

echo "ConquerV5 Debian package built successfully!"
echo "Package location: packages/debian/"
