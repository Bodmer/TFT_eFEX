// Created by Bodmer 15/11/18 as an example for the TFT_eFEX graphics extension library to TFT_eSPI

// https://github.com/Bodmer/TFT_eSPI
#include <TFT_eSPI.h>            // Include the graphics library (this includes the sprite functions)

// https://github.com/Bodmer/TFT_eFEX
#include <TFT_eFEX.h>             // Include the extension graphics functions library

TFT_eSPI tft = TFT_eSPI();       // Create object "tft"

TFT_eFEX  fex = TFT_eFEX(&tft);    // Create TFT_eFEX object "fex" with pointer to "tft" object

// =========================================================================
void setup(void) {

  Serial.begin(250000);

  tft.init();

  tft.setRotation(1);

}

// =========================================================================
void loop() {
  tft.fillScreen(TFT_BLACK);

  uint16_t xc = 60;
  uint16_t yb = 120;
  uint16_t yh = 100 - random(100);
  uint16_t color = random(0x10000);

  // This function exists only in the TFT_eFEX library
  fex.drawBezier(xc - 55, yb, xc, yb - yh, xc + 55, yb, color);

  // Draw in two segments with ends at p0 and p2, peak at p1
  fex.drawBezierSegment(xc - 35, yb, xc - 20, yb - yh, xc, yb - yh, color);
  fex.drawBezierSegment(xc + 35, yb, xc + 20, yb - yh, xc, yb - yh, color);

  // The TFT_eFEX library "fex" object has inherited the graphics functions in the TFT_eSPI library
  fex.fillRect( 0, 0, 20, 20, TFT_RED); // fillRect does not exist in the TFT_eFEX library

  // The "tft" object function calls still work and make it clearer where the function resides
  tft.fillRect(20, 0, 20, 20, TFT_GREEN);
  tft.fillRect(40, 0, 20, 20, TFT_BLUE);

  delay(2000);
}
