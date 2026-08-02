#!/bin/bash

# Usage:
#   ./build.sh        Build a .hex suitable for flashing to real hardware
#   ./build.sh -s     Build a .hex suitable for SimulIDE simulation
#
# The -s (simulator) flag post-processes the .hex to convert extended address
# records (Intel HEX type 02) to a plain format (type 04) that SimulIDE accepts.

SIM_MODE=false
while getopts "s" opt; do
  case $opt in
    s) SIM_MODE=true ;;
  esac
done

arduino-cli compile -e -b arduino:avr:mega \
  --libraries ../../../.. \
  --build-property build.extra_flags="-DDEBUG" .

if $SIM_MODE; then
  HEX=$(ls build/arduino.avr.mega/*.hex | grep -v bootloader | head -1)
  SIM_HEX="${HEX%.hex}.sim.hex"
  # Rewrite type 02 (Extended Segment Address) records as type 04 (Extended
  # Linear Address) records. SimulIDE accepts type 04 but not type 02.
  # Conversion: for type 02 segment value S, type 04 upper16 = S >> 12
  # (because segment base = S*16, and upper16 = (S*16) >> 16 = S >> 12).
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
fi
