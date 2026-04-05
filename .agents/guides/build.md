# Build Guide

## Docker Build (Recommended)

```bash
cd firmware
./build.sh                    # Build for ESP32S3 (default)
./build.sh --board esp32c6    # Build for ESP32C6
./build.sh --flash --monitor  # Build, flash, and monitor
```

## Flash to Device

```bash
docker run --rm -v $(pwd):/workspace -w /workspace espressif/idf:v6.0 \
  sh -c ". /opt/esp/idf/export.sh && idf.py -D IDF_TARGET=esp32s3 -p /dev/ttyACM0 flash monitor"
```

## Manual Build

```bash
cd firmware
idf.py set-target esp32s3    # Set target once
idf.py build                  # Build
idf.py -p /dev/ttyACM0 flash monitor  # Flash and monitor
```

## CI Build Notes

The CI builds for **both ESP32S3 and ESP32C6 targets**:
- Always test locally for your target first
- If CI fails with target mismatch, run `rm -rf build sdkconfig` locally and rebuild
- When adding ESP-IDF APIs, verify the API exists in ESP-IDF v6.0 for both architectures

## Code Formatting

### C++ Code

- **Config**: `.clang-format` (GNU style, 4-space tabs, 100 char line length)
- **Format files**: `clang-format -i <files>`
- **Pre-commit**: Auto-formats staged C++ files
- **CI check**: Non-blocking (ESP-IDF conventions differ from LLVM style)

### Markdown

- **Line length**: 120 characters
- **Lint**: `markdownlint-cli2 "**/*.md"`
- **Pre-commit**: Auto-lints staged Markdown files

### Pre-commit Hooks

Install once:

```bash
pip install pre-commit
pre-commit install
```

Pre-commit runs automatically on `git commit`. To run manually:

```bash
pre-commit run --all-files
```
