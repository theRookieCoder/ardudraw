import serial
import sys
from microbmp import MicroBMP

if len(sys.argv) != 3:
    print("Usage: ardudraw PORT OUT_FILE")
    exit(1)

with serial.Serial(sys.argv[1], 38400, timeout=10) as ser:
    img = MicroBMP(ser.read()[0], ser.read()[0], 1) # 126x64 1-bit colour image

    # Reading from serial
    buf = ser.read()
    x = 0
    y = 0
    while buf != b'\n':
        print(f"({x:03}, {y:02}) = {buf}")
        img[x, y] = buf[0]
        x += 1
        if x == 126:
            x = 0
            y += 1
        buf = ser.read()
    img.save(sys.argv[2])

