#include <Arduboy2.h>

Arduboy2 arduboy;

// One second in frames
const uint8_t ONE_SEC = 60;

void setup() {
  Serial.begin(38400);
  arduboy.begin();
  arduboy.setFrameRate(ONE_SEC);

  // Load image from EEPROM
  arduboy.clear();
  for (uint16_t i = 0; i < (EEPROM.length() - EEPROM_STORAGE_SPACE_START); i++)
    arduboy.sBuffer[i] = EEPROM[EEPROM_STORAGE_SPACE_START + i];
  arduboy.display();
}

// The width is 126 pixels because there isn't enough space in the EEPROM
const uint8_t width   = arduboy.width() - 2;
const uint8_t height  = arduboy.height();

uint8_t cursorX           = 0;
uint8_t cursorY           = 0;
uint8_t pixelUnderCursor  = arduboy.getPixel(cursorX, cursorY);

uint16_t leftRepeat   = 0;
uint16_t rightRepeat  = 0;
uint16_t upRepeat     = 0;
uint16_t downRepeat   = 0;
uint16_t aRepeat      = 0;
uint16_t bRepeat      = 0;

void loop() {
  if (!arduboy.nextFrame()) return;
  arduboy.pollButtons();

  // Button repeat
  // Checks if the button is pressed and was presed in the previous frame
  if (arduboy.pressed(LEFT_BUTTON   ) && !arduboy.justPressed(LEFT_BUTTON   ))  leftRepeat++;
  if (arduboy.pressed(RIGHT_BUTTON  ) && !arduboy.justPressed(RIGHT_BUTTON  ))  rightRepeat++;
  if (arduboy.pressed(UP_BUTTON     ) && !arduboy.justPressed(UP_BUTTON     ))  upRepeat++;
  if (arduboy.pressed(DOWN_BUTTON   ) && !arduboy.justPressed(DOWN_BUTTON   ))  downRepeat++;
  if (arduboy.pressed(A_BUTTON      ) && !arduboy.justPressed(A_BUTTON      ))  aRepeat++;
  if (arduboy.pressed(B_BUTTON      ) && !arduboy.justPressed(B_BUTTON      ))  bRepeat++;
  
  // Button repeat release
  if (arduboy.notPressed(LEFT_BUTTON  )) leftRepeat   = 0;
  if (arduboy.notPressed(RIGHT_BUTTON )) rightRepeat  = 0;
  if (arduboy.notPressed(UP_BUTTON    )) upRepeat     = 0;
  if (arduboy.notPressed(DOWN_BUTTON  )) downRepeat   = 0;
  if (arduboy.notPressed(A_BUTTON     )) aRepeat      = 0;
  if (arduboy.notPressed(B_BUTTON     )) bRepeat      = 0;

  // If B is held for one second
  if (bRepeat > ONE_SEC && bRepeat < ONE_SEC + 2) {
    // Upload to the computer if there is a serial connection
    arduboy.drawPixel(cursorX, cursorY, pixelUnderCursor);
    if (Serial) {
      Serial.write(width);
      Serial.write(height);
      for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
          Serial.write(arduboy.getPixel(x, y));
        }
      }
      Serial.write('\n'); // terminator byte
    } else {
      // Save to EEPROM and flash the LED green while saving
      arduboy.setRGBled(0x00, 0xff, 0x00);
      for (uint16_t i = 0; i < (EEPROM.length() - EEPROM_STORAGE_SPACE_START); i++)
        EEPROM.put(EEPROM_STORAGE_SPACE_START + i, arduboy.sBuffer[i]);
      arduboy.setRGBled(0x00, 0x00, 0x00);
    }
    return;
  }

  // Cursor movement
  uint8_t newCursorX = cursorX;
  uint8_t newCursorY = cursorY;

  // Just-press or hold for 250ms to move
  if (arduboy.justPressed(LEFT_BUTTON   ) || leftRepeat   > ONE_SEC / 4) newCursorX = (cursorX - 1) % width;
  if (arduboy.justPressed(RIGHT_BUTTON  ) || rightRepeat  > ONE_SEC / 4) newCursorX = (cursorX + 1) % width;
  if (arduboy.justPressed(UP_BUTTON     ) || upRepeat     > ONE_SEC / 4) newCursorY = (cursorY - 1) % height;
  if (arduboy.justPressed(DOWN_BUTTON   ) || downRepeat   > ONE_SEC / 4) newCursorY = (cursorY + 1) % height;

  // Handle integer underflow
  // (this should've been done by the modulo but that doesn't work?)
  if (newCursorX == 255) newCursorX = width - 1;
  if (newCursorY == 255) newCursorY = height - 1;

  // Toggle pixel
  if (arduboy.justPressed(A_BUTTON)) pixelUnderCursor = !pixelUnderCursor;

  // Clear the screen if A is held for 3 seconds
  if (aRepeat > ONE_SEC * 3) {
    arduboy.clear();
    arduboy.display();
    aRepeat = 0;
    return;
  }

  bool changed = cursorX != newCursorX || cursorY != newCursorY;

  // Update the pixelUnderCursor buffer
  if (changed) {
    arduboy.drawPixel(cursorX, cursorY, pixelUnderCursor);
    pixelUnderCursor = arduboy.getPixel(newCursorX, newCursorY);
  }

  cursorX = newCursorX;
  cursorY = newCursorY;

  if (arduboy.everyXFrames(10) || changed) {
    arduboy.drawPixel(cursorX, cursorY, !arduboy.getPixel(cursorX, cursorY));
    arduboy.display();
  }
  arduboy.idle();
}
