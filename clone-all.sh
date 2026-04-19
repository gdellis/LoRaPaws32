#!/bin/bash
set -euo pipefail

# LoRaPaws32 Clone Script
# Clones all component repositories into sibling directories

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PARENT_DIR="$(dirname "$SCRIPT_DIR")"

REPOS=(
  "git@github.com:gdellis/lorapaws32-firmware.git"
  "git@github.com:gdellis/lorapaws32-hardware.git"
  "git@github.com:gdellis/lorapaws32-base-station.git"
)

echo "Cloning LoRaPaws32 component repositories..."

for repo in "${REPOS[@]}"; do
  name=$(basename "$repo" .git)
  target_dir="$PARENT_DIR/$name"

  if [ -d "$target_dir" ]; then
    echo "  Skipping $name (already exists)"
  else
    echo "  Cloning $name..."
    git clone "$repo" "$target_dir"
  fi
done

echo ""
echo "Done! Repos cloned to:"
for repo in "${REPOS[@]}"; do
  name=$(basename "$repo" .git)
  echo "  - $name/"
done
