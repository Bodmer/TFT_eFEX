/*
  This tests the rainbow colour and luminance functions.

  Example for library:
  https://github.com/Bodmer/TFT_eFEX

  The sketch has been tested on a 320x240 ILI9341 based TFT

  Created by Bodmer 4/2/19
*/

// https://github.com/Bodmer/TFT_eSPI
#include <TFT_eSPI.h>              // Hardware-specific library
TFT_eSPI tft = TFT_eSPI();         // Invoke custom library

// https://github.com/Bodmer/TFT_eFEX
#include <TFT_eFEX.h>              // Include the extension graphics functions library
TFT_eFEX  fex = TFT_eFEX(&tft);    // Create TFT_eFX object "efx" with pointer to "tft" object

// -------------------------------------------------------------------------
// Setup
// -------------------------------------------------------------------------
void setup(void) {
  Serial.begin(250000); // Used for messages

  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
}

// -------------------------------------------------------------------------
// Main loop
// -------------------------------------------------------------------------
void loop()
{
  tft.fillScreen(TFT_BLACK);

  // Show luminance for rainbow colours
  for (uint16_t lum = 0; lum <= 255; lum += 1)
  {
    for (uint8_t s = 0; s < 191; s++)       // Spectrum value "s" is in range 0 - 127 for red to blue
    {                                       // 0 - 159 for red to violet, 0 - 191 for red back to red
      uint16_t color = fex.rainbowColor(s);
      uint16_t lumColor = fex.luminance(color, lum);  // Luminance 0 to 255 (black to full brightness)

      tft.drawPixel(s, lum, lumColor); // for demonstration c and lum are used as coordinates
    }
  }

  delay(2000);

  // Show luminance for a single RGB colour
  for (uint16_t lum = 0; lum <= 255; lum += 1)
  {
    // luminance of an RGB colour      R    G   B         // RGB and 8 bit (0-255)
    uint16_t lumColor = fex.luminance(255, 180, 0, lum);  // Luminance 0 to 255 (black to full brightness)

    tft.drawFastHLine(0, lum, 240, lumColor); // for demonstration c lum is used as y coordinate
  }

  delay(2000);
}
