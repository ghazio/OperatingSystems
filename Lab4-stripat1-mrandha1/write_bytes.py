import struct
import sys

# Update this list to contain the desired sequence of memory addresses.
values = [
  0xF943,
  0x1501,
  0x0FF9,
  0xA8C4,
  0x7856,
  0xC350,
  0x598A,
  0x6A97,
  0x57FD,
  0x9DE2,
  0x307C,
  0x13D0,
  0x4A0D,
  0xE4E7,
  0x51B7,
  0x0781
]

# Open the file for writing and write one int after another, packed as a
# 64-bit, system-native endian integer.
f = open(sys.argv[1], 'w')
for v in values:
    f.write(struct.pack('=q', v))
f.close()
