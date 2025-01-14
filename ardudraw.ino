#include <Arduboy2.h>

Arduboy2 arduboy;

constexpr uint8_t framesPerSecond = 60;
const uint16_t canvasSizeBytes = EEPROM.length() - EEPROM_STORAGE_SPACE_START;

// The width is 126 pixels because there isn't enough space in the EEPROM
constexpr uint8_t width   = arduboy.width() - 2;
constexpr uint8_t height  = arduboy.height();

uint8_t cursorX           = 0;
uint8_t cursorY           = 0;
uint8_t pixelUnderCursor  = BLACK;

uint16_t leftRepeat   = 0;
uint16_t rightRepeat  = 0;
uint16_t upRepeat     = 0;
uint16_t downRepeat   = 0;
uint16_t aRepeat      = 0;
uint16_t bRepeat      = 0;


void setup() {
  Serial.begin(38400);
  arduboy.begin();
  arduboy.setFrameRate(framesPerSecond);


  // Load image from EEPROM
  arduboy.clear();
  for (uint16_t i = 0; i < canvasSizeBytes; i++)
    arduboy.sBuffer[i] = EEPROM[EEPROM_STORAGE_SPACE_START + i];
  arduboy.display();

  pixelUnderCursor = arduboy.getPixel(cursorX, cursorY);
}

void loop() {
  if (!arduboy.nextFrame()) return;
  arduboy.pollButtons();


  // Button repeat
  if (arduboy.pressed(LEFT_BUTTON))
    leftRepeat++;
  else
    leftRepeat = 0;

  if (arduboy.pressed(RIGHT_BUTTON))
    rightRepeat++;
  else
    rightRepeat = 0;

  if (arduboy.pressed(UP_BUTTON))
    upRepeat++;
  else
    upRepeat = 0;

  if (arduboy.pressed(DOWN_BUTTON))
    downRepeat++;
  else
    downRepeat = 0;

  if (arduboy.pressed(A_BUTTON))
    aRepeat++;
  else
    aRepeat = 0;

  if (arduboy.pressed(B_BUTTON))
    bRepeat++;
  else
    bRepeat = 0;


  // If B is held for one second
  if (bRepeat > framesPerSecond && bRepeat < framesPerSecond + 2) {

    // Restore the pixel under the cursor
    arduboy.drawPixel(cursorX, cursorY, pixelUnderCursor);

    // Upload to the computer if there is a serial connection
    if (Serial) {
      // Canvas dimensions
      Serial.write(width);
      Serial.write(height);

      // Canvas data
      for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
          Serial.write(arduboy.getPixel(x, y));

      Serial.write('\n'); // terminator

    } else {
      // Save to EEPROM and flash the LED green while saving
      arduboy.setRGBled(0x00, 0xff, 0x00);

      for (uint16_t i = 0; i < canvasSizeBytes; i++)
        EEPROM.update(EEPROM_STORAGE_SPACE_START + i, arduboy.sBuffer[i]);

      arduboy.setRGBled(0x00, 0x00, 0x00);
    }
    return;
  }


  // Cursor movement
  uint8_t newCursorX = cursorX;
  uint8_t newCursorY = cursorY;

  // Just-press or hold for 250ms to move
  if (arduboy.justPressed(LEFT_BUTTON) || leftRepeat > framesPerSecond / 4)
    newCursorX = (cursorX > 0) ? cursorX - 1 : width - 1;

  if (arduboy.justPressed(RIGHT_BUTTON) || rightRepeat > framesPerSecond / 4)
    newCursorX = (cursorX < width - 1) ? cursorX + 1 : 0;

  if (arduboy.justPressed(UP_BUTTON) || upRepeat  > framesPerSecond / 4)
    newCursorY = (cursorY > 0) ? cursorY - 1 : height - 1;

  if (arduboy.justPressed(DOWN_BUTTON) || downRepeat > framesPerSecond / 4)
    newCursorY = (cursorY < height - 1) ? cursorY + 1 : 0;


  // Toggle pixel
  if (arduboy.justPressed(A_BUTTON))
    pixelUnderCursor = (pixelUnderCursor == BLACK) ? WHITE : BLACK;


  // Clear the screen if A is held for 3 seconds
  if (aRepeat > framesPerSecond * 3) {
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
    arduboy.drawPixel(
      cursorX, cursorY,
      (arduboy.getPixel(cursorX, cursorY) == BLACK) ? WHITE : BLACK
    );
    arduboy.display();
  }
}
