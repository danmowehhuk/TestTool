#!/bin/bash

# Usage:
#   ./build.sh        Build a .hex suitable for flashing to real hardware
#   ./build.sh -s     Build a .hex suitable for SimulIDE simulation
#
# Unlike the sibling basic example, this one does not go through
# arduino-cli or any Arduino core at all - it compiles and links directly
# with avr-g++ using -DNO_ARDUINO -DHAL_AVR, proving the bare-metal path
# builds independently of Arduino, the same way any non-Arduino AVR
# project would use TestTool.
#
# The -s (simulator) flag post-processes the .hex to convert extended
# address records (Intel HEX type 02) to a plain format (type 04) that
# SimulIDE accepts.

set -euo pipefail

SIM_MODE=false
while getopts "s" opt; do
  case $opt in
    s) SIM_MODE=true ;;
  esac
done

# Looks for an AVR toolchain in every common place it might live - not just
# Arduino's cache. The whole point of this example is building without
# Arduino installed at all, so the search has to work for someone who never
# installed the Arduino IDE/arduino-cli in the first place: Homebrew,
# MacPorts, PlatformIO's bundled toolchain, and standard manual
# cross-toolchain install prefixes.
find_avr_tool() {
  local tool="$1"
  local found

  if command -v "$tool" >/dev/null 2>&1; then
    command -v "$tool"
    return
  fi

  local search_roots=(
    "$HOME/Library/Arduino15/packages/arduino/tools/avr-gcc"   # macOS
    "$HOME/.arduino15/packages/arduino/tools/avr-gcc"          # Linux
    "$HOME/.platformio/packages/toolchain-atmelavr"
    "/opt/homebrew/opt/avr-gcc"
    "/opt/homebrew/Cellar/avr-gcc"
    "/usr/local/opt/avr-gcc"
    "/usr/local/Cellar/avr-gcc"
    "/opt/local"
    "/usr/local/avr"
    "/opt/avr"
    "/usr/avr"
  )

  for root in "${search_roots[@]}"; do
    found=$(find "$root" -name "$tool" -type f 2>/dev/null | sort -V | tail -1)
    if [ -n "$found" ]; then echo "$found"; return; fi
  done

  echo "ERROR: $tool not found on PATH or in any of the usual install locations (Arduino15, PlatformIO, Homebrew, MacPorts, /usr/local/avr, /opt/avr, /usr/avr)" >&2
  exit 1
}

AVRGXX="$(find_avr_tool avr-g++)"
AVROBJCOPY="$(find_avr_tool avr-objcopy)"
DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$DIR/build"

# BareMetalHAL is a sibling Arduino library, not bundled with TestTool -
# override with BAREMETALHAL_SRC if it doesn't live in the usual
# sketchbook location.
BAREMETALHAL_SRC="${BAREMETALHAL_SRC:-$HOME/Arduino/libraries/BareMetalHAL/src}"
if [ ! -f "$BAREMETALHAL_SRC/BareMetalHAL.h" ]; then
  echo "ERROR: BareMetalHAL.h not found under $BAREMETALHAL_SRC - set BAREMETALHAL_SRC to its src/ directory" >&2
  exit 1
fi

mkdir -p "$BUILD_DIR"

"$AVRGXX" -std=gnu++11 -Wall -Wextra -Os -DNO_ARDUINO -DHAL_AVR -DF_CPU=16000000UL -mmcu=atmega2560 \
  -I "$DIR/../../src" \
  -I "$BAREMETALHAL_SRC" \
  "$DIR/basic-avr.cpp" "$DIR/../../src/TestInvocation.cpp" -o "$BUILD_DIR/basic-avr.elf"

"$AVROBJCOPY" -O ihex -R .eeprom "$BUILD_DIR/basic-avr.elf" "$BUILD_DIR/basic-avr.hex"

echo "Built $BUILD_DIR/basic-avr.hex"

if $SIM_MODE; then
  HEX="$BUILD_DIR/basic-avr.hex"
  SIM_HEX="${HEX%.hex}.sim.hex"
  python3 - "$HEX" "$SIM_HEX" << 'EOF'
import sys

def checksum(data_bytes):
    return (0x100 - sum(data_bytes) % 0x100) % 0x100

with open(sys.argv[1]) as f_in, open(sys.argv[2], 'w') as f_out:
    for line in f_in:
        line = line.strip()
        if line[7:9] == '02':  # Extended Segment Address record
            segment = int(line[9:13], 16)
            upper16 = segment >> 12
            b = [0x02, 0x00, 0x00, 0x04, upper16 >> 8, upper16 & 0xFF]
            f_out.write(f':{b[0]:02X}{b[1]:02X}{b[2]:02X}{b[3]:02X}{b[4]:02X}{b[5]:02X}{checksum(b):02X}\n')
        else:
            f_out.write(line + '\n')
EOF
  echo "SimulIDE-compatible hex: $SIM_HEX"
fi
