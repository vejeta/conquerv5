#!/bin/bash
set -e

echo "🏰 Sir Chapi reporting for duty… preparing the ConquerV5 build!"

SCRIPT_DIR="$(dirname "$0")"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
cd "$REPO_ROOT"

VERSION=5.0
BUILD_DIR="$REPO_ROOT/build"
TARBALL="$BUILD_DIR/conquerv5-release.tar.gz"
MELANGE_YAML="$BUILD_DIR/melange.yaml"
OUT_DIR="$PWD/packages/alpine/conquerv5-${VERSION}"

rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"

# Pack the source (content-only)
tar -C "$REPO_ROOT/gpl-release" -czf "$TARBALL" .

# Compute SHA256
SHA256=$(sha256sum "$TARBALL" | awk '{print $1}')
echo "🔒 Calculated SHA256: $SHA256"

# Generate final YAML from template
sed -e "s/PLACEHOLDER_SHA256/$SHA256/" \
    -e "s/PLACEHOLDER_VERSION/$VERSION/" \
    "$REPO_ROOT/packaging/melange/melange.yaml.template" > "$MELANGE_YAML"

# Run Melange build
docker run --privileged --rm \
    -v "$BUILD_DIR":/staged:ro \
    -v "$OUT_DIR":/out \
    cgr.dev/chainguard/melange build \
    --arch=x86_64 \
    --out-dir=/out \
    /staged/$(basename "$MELANGE_YAML")

# ===============================
# 🛡️ Generate Checksums
# ===============================
echo "🛡️ Generating checksums for artifacts…"

# Detect if running in GitHub Actions
if [ -n "$GITHUB_WORKSPACE" ]; then
  # Safer location inside CI
  CHECKSUM_DIR="$GITHUB_WORKSPACE/checksums"
else
  # Local builds: put checksums alongside packages
  CHECKSUM_DIR="$OUT_DIR"
fi

mkdir -p "$CHECKSUM_DIR"

# Generate checksums for all built APKs
find "$OUT_DIR" -type f -name "*.apk" -exec sha256sum {} \; > "$CHECKSUM_DIR/checksums.txt"

echo "✅ Checksums written to: $CHECKSUM_DIR/checksums.txt"


# Cleanup
rm -f "$MELANGE_YAML" "$TARBALL"

echo "🏁 ConquerV5 package built successfully in $OUT_DIR"
ls -la "$OUT_DIR"
echo "🎉 All glory to Sir Chapi!"
