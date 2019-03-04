// Demo of Right To Left (RTL) font rendering functions

// Some writing systems of the world, including the Arabic and Hebrew scripts or derived
// systems such as the Persian, Urdu, and Yiddish scripts, are written in a form known as
// right-to-left (RTL):
// https://en.wikipedia.org/wiki/Bi-directional_text

// Coded be Bodmer 1/3/19
// Example for TFT_eSPI library

#define AA_FONT_SMALL "NotoSansBold15"

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
  if (SPIFFS.exists("/NotoSansBold15.vlw") == false) font_missing = true;

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

  fex.drawStringRTL("This is RTL!");

  ypos += tft.fontHeight();   // Get the font height and move ypos down
  xpos  = tft.width() - 1;    // Righthand side of screen

  fex.setCursorRTL(xpos, ypos);

  tft.setTextColor(TFT_GREEN, TFT_BLACK);
 
  // If the string does not fit the screen width, then the next character will wrap to a new line
  fex.drawStringRTL("Ode To A Small Lump Of Green Putty I Found In My Armpit One Midsummer Morning");

  // We can render LTR text as well whilst still moving the cursor left ready for the next RTL string
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  fex.drawStringLTR(" <01234567890> ");

  tft.setTextColor(TFT_GREEN, TFT_BLACK); 
  fex.drawStringRTL("Ode To A Small Lump Of Green Putty I Found In My Armpit One Midsummer Morning");

  tft.unloadFont(); // Remove the font to recover memory used

  delay(4000);

}
