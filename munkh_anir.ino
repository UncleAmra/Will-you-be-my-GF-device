#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

#include "frame_1.h"
#include "frame_2.h"
#include "frame33.h"
#include "frame_4.h"
#include "frame_5.h"
#include "frame_6.h"
#include "frame_7.h"
#include "frame_8.h"
#include "frame_9.h"
#include "frame_10.h"

#define SDA_PIN 21
#define SCL_PIN 22
LiquidCrystal_PCF8574 lcd(0x27);

#define TFT_CS   5
#define TFT_RST  4
#define TFT_DC   2
#define TFT_BL   15

#define BUTTON1_PIN 0    // G0: Proposal trigger
#define BUTTON2_PIN 17   // G17: Acceptance trigger

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

const int TFT_WIDTH = 128;
const int TFT_HEIGHT = 160;

bool triggered_1 = false;
bool triggered_2 = false;
bool celebrationDone = false;
int celebrationLoopCount = 0;

void drawFrame(const uint16_t* frame, int len) {
  for (int x = 0; x < TFT_WIDTH; x++) {
    for (int y = 0; y < TFT_HEIGHT; y++) {
      int index = x * TFT_HEIGHT + y;
      if (index < len) {
        uint16_t color = pgm_read_word(&frame[index]);
        tft.drawPixel(x, y, color);
      }
    }
  }
}

void setup() {
  Wire.begin(SDA_PIN, SCL_PIN);
  lcd.begin(16, 2);
  lcd.setBacklight(true);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("    Tursun udriin mend hurgii!!");
  lcd.setCursor(0, 1);
  lcd.print("    Munkh Aniraa!!");

  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);

  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(ST77XX_BLACK);
  tft.setRotation(2);  // Flip display orientation

  pinMode(BUTTON1_PIN, INPUT_PULLUP);
  pinMode(BUTTON2_PIN, INPUT_PULLUP);
}

void loop() {
  // First button press: show proposal
  if (!triggered_1 && digitalRead(BUTTON1_PIN) == LOW) {
    triggered_1 = true;

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Will you be my");
    lcd.setCursor(0, 1);
    lcd.print("girlfriend?");

    tft.fillScreen(ST77XX_BLACK);
    drawFrame(frame_4_bitmap, frame_4_bitmap_len);
    delay(300);  // debounce
  }

  // Second button press: start celebration
  if (triggered_1 && !triggered_2 && digitalRead(BUTTON2_PIN) == LOW) {
    triggered_2 = true;

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("   YAY!!");
    lcd.setCursor(0, 1);
    lcd.print("I LOVE YOUUU");

    tft.fillScreen(ST77XX_BLACK);
    delay(500);
  }

  // Initial animation loop before any button
  if (!triggered_1) {
    delay(500);
    lcd.scrollDisplayLeft();

    static unsigned long lastFrameChange = 0;
    static int frameIndex = 0;

    if (millis() - lastFrameChange > 400) {
      lastFrameChange = millis();
      switch (frameIndex) {
        case 0: drawFrame(frame_1_bitmap, frame_1_bitmap_len); break;
        case 1: drawFrame(frame_2_bitmap, frame_2_bitmap_len); break;
        case 2: drawFrame(frame33_bitmap, frame33_bitmap_len); break;
      }
      frameIndex = (frameIndex + 1) % 3;
    }
  }

  // Celebration animation: loop frames 5-8 four times
  if (triggered_1 && triggered_2 && !celebrationDone) {
    static unsigned long lastFrameChange = 0;
    static int frameIndex = 0;

    if (millis() - lastFrameChange > 400) {
      lastFrameChange = millis();

      switch (frameIndex) {
        case 0: drawFrame(frame_5_bitmap, frame_5_bitmap_len); break;
        case 1: drawFrame(frame_6_bitmap, frame_6_bitmap_len); break;
        case 2: drawFrame(frame_7_bitmap, frame_7_bitmap_len); break;
        case 3: drawFrame(frame_8_bitmap, frame_8_bitmap_len); break;
      }

      frameIndex++;
      if (frameIndex >= 4) {
        frameIndex = 0;
        celebrationLoopCount++;
      }

      if (celebrationLoopCount >= 4) {
        celebrationDone = true;
        tft.fillScreen(ST77XX_BLACK);
      }
    }
  }

  // After celebration: loop frame_9 and frame_10 forever
  if (celebrationDone) {
    static unsigned long lastFrameChange = 0;
    static int foreverFrameIndex = 0;

    if (millis() - lastFrameChange > 400) {
      lastFrameChange = millis();

      if (foreverFrameIndex == 0) {
        drawFrame(frame_9_bitmap, frame_9_bitmap_len);
        foreverFrameIndex = 1;
      } else {
        drawFrame(frame_10_bitmap, frame_10_bitmap_len);
        foreverFrameIndex = 0;
      }
    }
  }
}
