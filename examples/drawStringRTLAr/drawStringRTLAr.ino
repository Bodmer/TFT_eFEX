// Demo of Right To Left (RTL) font rendering functions
// Arabic language is a connected letters language 
// Same letter Renders at the begining, middle, end and Isolated in diffrent shape
// Arabic right-to-left (RTLAr):
// https://en.wikipedia.org/wiki/Arabic_alphabet

// Coded be MShokry 16/1/21
// Example for TFT_eSPI library

#define AA_FONT_SMALL "CairoLight22"

// Font files are stored in SPIFFS, so load the library
#include <FS.h>

#include <SPI.h>

// https://github.com/Bodmer/TFT_eSPI
#include <TFT_eSPI.h>              // Hardware-specific library
TFT_eSPI tft = TFT_eSPI();         // Invoke custom library

// https://github.com/Bodmer/TFT_eFEX
#include <TFT_eFEX.h>              // Include the extension graphics functions library
TFT_eFEX  fex = TFT_eFEX(&tft);    // Create TFT_eFX object "efx" with pointer to "tft" object

void setup(void) {

  Serial.begin(115200);

  tft.begin();

  tft.setRotation(1);

  if (!SPIFFS.begin()) {
    Serial.println("SPIFFS initialisation failed!");
    while (1) yield(); // Stay here twiddling thumbs waiting
  }
  Serial.println("\r\nSPIFFS available!");

  // ESP32 will crash if any of the fonts are missing
  bool font_missing = false;
  if (SPIFFS.exists("/CairoLight22.vlw") == false)
    font_missing = true;

  if (font_missing)
  {
    Serial.println("\r\nFont missing in SPIFFS, did you upload it?");
    tft.fillScreen(TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.drawString("Font file missing",tft.width()/2, tft.height()/2, 1); 
    while (1) yield();
  }
  else Serial.println("\r\nFonts found OK.");
}

void loop() {

  tft.fillScreen(TFT_BLACK);

  tft.setTextColor(TFT_WHITE, TFT_BLACK); // Set the font colour and the background colour

  int xpos = tft.width() / 2; // Half the screen width
  int ypos = 10;

  tft.loadFont(AA_FONT_SMALL); // Must load the font first

  fex.setCursorRTL(xpos, ypos);

  fex.drawStringRTLAr("بسم الله الرحمن الرحيم");

  ypos += tft.fontHeight();   // Get the font height and move ypos down
  xpos  = tft.width() - 1;    // Righthand side of screen

  fex.setCursorRTL(xpos, ypos);

  tft.setTextColor(TFT_GREEN, TFT_BLACK);
 
  // If the string does not fit the screen width, then the next character will wrap to a new line
  fex.drawStringRTLAr("اللغة العربية لغة جميلة");

  ypos += tft.fontHeight(); // Get the font height and move ypos down
  xpos = tft.width() - 1;   // Righthand side of screen

  fex.setCursorRTL(xpos, ypos);

  tft.setTextColor(TFT_GREEN, TFT_BLACK); 
  fex.drawStringRTLAr("تجربة كتابة متعددة للحروف العربية");

  tft.unloadFont(); // Remove the font to recover memory used

  delay(4000);

}
