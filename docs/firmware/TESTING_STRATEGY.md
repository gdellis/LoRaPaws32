# Testing Strategy for LoRaPaws32 Firmware

## Overview

The firmware uses a multi-tier testing approach: host-based unit tests, target tests, and manual integration testing.

## Host-Based Unit Tests

### Purpose

Run on Linux during CI without hardware. Test individual components with mocked ESP-IDF headers.

### Location

`firmware/tests/`

### Running

```bash
cd firmware/tests
mkdir -p build && cd build
cmake .. && make -j$(nproc)

# Run individual tests
./test_nmea_parser
./test_button_handler
./test_led_driver
./test_lora_driver
./test_geofence
./test_ble
./test_gps_geofence
```

### Mock Strategy

Mocks are minimal - only what's needed to compile and run tests:

| ESP-IDF Function | Mock Location |
|-----------------|---------------|
| `esp_log.h` macros | `tests/include/esp_log.h` |
| `esp_err.h` codes | `tests/include/esp_err.h` |
| `esp_timer_get_time()` | `tests/src/esp_timer_mock.cpp` |
| `gpio_get_level()` | `tests/src/gpio_mock.cpp` |
| `xEventGroupWaitBits()` | `tests/src/event_groups_mock.cpp` |

### Coverage

- `test_nmea_parser` - NMEA sentence parsing, checksum validation, GGA/RMC field extraction
- `test_button_handler` - Button debounce logic
- `test_led_driver` - LED on/off/toggle state
- `test_lora_driver` - LoRa driver initialization and basic operations
- `test_geofence` - Geofence zone loading, containment checks
- `test_ble` - BLE struct serialization, size validation
- `test_gps_geofence` - GPS + geofence integration

## Target Tests (Hardware)

### Purpose

Run on actual ESP32-S3 hardware via `idf.py test`.

### Running

```bash
cd firmware
idf.py test
```

### Notes

- Requires physical ESP32-S3 device connected
- Tests actual hardware peripherals (GPS, LoRa, etc.)
- Uses Unity test framework (included in ESP-IDF)

## Manual Integration Testing

### Procedure

1. Flash firmware to device:
   ```bash
   cd firmware
   idf.py -p /dev/ttyACM0 flash monitor
   ```

2. Observe startup logs for component initialization
3. Test button press → LED toggle → LoRa transmission cycle
4. Verify GPS fix acquisition after cold start
5. Check base station receives and displays location data

### Base Station Testing

1. Start base station: `cd base_station && python app.py`
2. Power on tracker
3. Observe MQTT messages in base station console
4. Check web UI at `http://localhost:5000`

## CI Pipeline

| Check | Trigger | What it does |
|-------|---------|--------------|
| Lint (pre-commit) | Every push | clang-format, markdownlint, shellcheck |
| Lint C++ | Every push | clang-tidy on firmware |
| Build Firmware | Every push | ESP-IDF Docker build for ESP32-S3 |
| Host Tests | Every push | CMake + Catch2 tests |
| CodeQL | Every push | Security analysis |

## Future Improvements

- [ ] Hardware-in-the-loop (HIL) tests with real GPS/LoRa
- [ ] Fuzz testing for NMEA parser input
- [ ] Property-based testing for geofence calculations
- [ ] Integration tests with base station (mock MQTT)
- [ ] Performance benchmarks for LoRa TX timing