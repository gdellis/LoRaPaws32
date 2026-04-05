# IDE Setup Guide

This guide covers IDE configuration for ESP32 firmware development.

## VS Code Extensions

### Required

- **ESP-IDF** (Espressif) - Official ESP-IDF extension
- **clangd** (llvm) - C++ language server (recommended over C/C++ extension)

### Recommended

- **Markdownlint** (David Anson) - Markdown linting
- **pre-commit** (pre-commit) - Pre-commit hook integration

## clangd Configuration

### Setup

1. **Build project first** to generate `compile_commands.json`:

   ```bash
   cd firmware
   ./build.sh
   ```

2. **clangd config** at `.clangd/config.yaml`:

   ```yaml
   CompileFlags:
     CompilationDatabase: firmware/build
     Remove:
       - "-mlongcalls"
       - "-fno-shrink-wrap"
       - "-fstrict-volatile-bitfields"
   ```

3. **Reload VS Code** to apply changes

### Expected LSP Warnings

You will see warnings like:

```
Unknown argument '-mlongcalls'; did you mean '-mlong-calls'?
Unknown argument: '-fno-shrink-wrap'
'driver/uart.h' file not found
```

**These are safe to ignore** because:

- ESP-IDF uses `xtensa-esp-elf-gcc` (not host GCC)
- Xtensa-specific flags are valid for ESP32 compilation
- Host LSP uses standard GCC which doesn't recognize these flags
- Actual builds work correctly

### Troubleshooting

**"Unknown argument" warnings**
- Expected for ESP-IDF code
- Safe to ignore - builds work correctly

**Missing ESP-IDF headers**
- Rebuild project: `./build.sh`
- Ensure `compile_commands.json` exists in `firmware/build/`

**Target mismatch errors**
- Clean and rebuild: `rm -rf firmware/build firmware/sdkconfig`
- Rebuild: `./build.sh`

## Code Formatting

### C++ Code

**Automatic** (via pre-commit):
- Staged files auto-formatted on commit
- GNU style (4-space tabs, 100 char line length)

**Manual**:

```bash
# Format specific files
clang-format -i firmware/main/main.cpp

# Format all firmware files
find firmware/main -type f \( -name '*.cpp' -o -name '*.hpp' \) | xargs clang-format -i
```

### Markdown

**Automatic** (via pre-commit):
- Staged files auto-linted on commit
- 120 character line length

**Manual**:

```bash
# Lint all markdown
markdownlint-cli2 "**/*.md" \
  "!firmware/tests/build/**" \
  "!firmware/build/**" \
  "!third_party/**" \
  "!docs/plans/**" \
  "!.opencode/**"
```

## Pre-commit Hooks

### Installation

```bash
# Install pre-commit
pip install pre-commit

# Install git hooks
pre-commit install
```

### What it checks

| Hook | Files | Action |
|------|-------|--------|
| `clang-format` | `.cpp`, `.hpp`, `.c`, `.h` | Auto-format |
| `markdownlint-cli2` | `.md` | Lint (blocks commit on error) |
| `shellcheck` | `.sh` | Lint (blocks commit on error) |
| `trailing-whitespace` | All | Auto-fix |
| `end-of-file-fixer` | All | Auto-fix |

### Manual run

```bash
# Run on all files
pre-commit run --all-files

# Run on staged files only
pre-commit run
```

### Skip hooks (emergency only)

```bash
git commit --no-verify -m "message"
```

**Note**: Only skip if absolutely necessary. Fix issues properly when possible.

## ESP-IDF Extension

### Configuration

1. **Select ESP-IDF version**: v6.0 (per `sdkconfig.defaults`)
2. **Set target**: ESP32-S3 or ESP32-C6
3. **Python virtualenv**: Let extension manage automatically

### Build & Flash

```bash
# Build
idf.py build

# Flash and monitor
idf.py -p /dev/ttyACM0 flash monitor

# Using Docker (recommended)
cd firmware
./build.sh --flash --monitor
```

## Recommended Settings

### VS Code `settings.json`

```json
{
  "editor.formatOnSave": true,
  "editor.defaultFormatter": "llvm-vs-code-extensions.vscode-clangd",
  "clangd.path": "clangd",
  "markdownlint.config": {
    "default": true,
    "MD013": { "line_length": 120 }
  }
}
```

## Resources

- [ESP-IDF Documentation](https://docs.espressif.com/projects/esp-idf/en/stable/)
- [clangd Configuration](https://clangd.llvm.org/config.html)
- [pre-commit Hooks](https://pre-commit.com/)
