# Ardudraw

Ardudraw is a simple Arduboy pixel art program

> [!CAUTION]
> 
> If you save your artwork, any EEPROM data stored by other programs (such as high scores or game saves) will be overwritten.

## Features

Draw in a `126`x`64` canvas.  
Save your artwork to non-volatile storage to continue your work after powering off.  
Download your artwork to a computer as a bitmap `.bmp` image file.

## Instructions

> [!NOTE]
>
> You may see garbage on the screen when you first run the program, this is EEPROM data that previous programs you've run have written. Hold `A` for 3 seconds to clear the screen.

- Use the *D-pad* to **move** the cursor around the screen
- *Use `A`* to **toggle** the colour of the pixel under the cursor
- *Hold `A` for 3 seconds* to **clear** the screen and start a new piece
- *Hold `B` for 1 second* to **save** your artwork to non-volatile storage

1. Connect your Arduboy to a computer
2. Run the included python script (`pyserial` and `microbmp` libraries are required)
3. *Hold `B` for 1 second* to **download** your artwork

## Technical Details

### EEPROM and Canvas Size

The canvas isn't the full screen size since there isn't enough space in the EEPROM. When packed into bytes, the screen takes up `1024` bytes, and the `ATmega32u4` has exactly `1024` bytes of EEPROM. However, the `Arduboy2` library uses 16 bytes to store user settings, the device name, etc. So I removed 2 columns of the screen from the canvas to make it fit in the EEPROM.

### Cursor

The cursor blinks at ~`6 Hz` and can wrap around the screen. It also has key repeat so that you can do single-pixel movements by tapping the button, and continuous movement by holding (the repeat threshold is ~`250 ms`).

### Computer Upload

The upload is done over serial at `38400` baud. The first two bytes sent are the canvas width and height. Then the pixels are sent left-to-right then top-down with `0x00` being black and `0x01` being white. Finally, `\n` is sent to indicate that the image has been sent and the connection can be terminated.
