/***************************************************************************************/
// The following class class adds additional functions to the TFT_eSPI library.
// The class inherits the generic drawing graphics functions from the TFT_eSPI class.

// Created by Bodmer 15/11/2018
// See license.txt in root folder of library
/***************************************************************************************/

#include <TFT_eSPI.h>

#include <JPEGDecoder.h>

// Call up the SPIFFS FLASH filing system this is part of the ESP Core
#if defined (ESP8266) || defined (ESP32)
  #define FS_NO_GLOBALS
  #include <FS.h>
#endif

#ifdef ESP32
  #include "SPIFFS.h" // ESP32 only
#endif

class TFT_eFEX : public TFT_eSPI {

 public:

  TFT_eFEX(TFT_eSPI *tft);

           // Draw a bezier curve of a defined colour between specified points
  void     drawBezier(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint16_t color);  
  void     drawBezierSegment(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint16_t color);

           // Draw a bitmap stored in SPIFFS to the TFT or a Sprite if a Sprite instance is included
  void     drawBmp(String filename, int16_t x, int16_t y, TFT_eSprite *_spr = nullptr);
//To do:  void     drawBmp(const char *filename, int16_t x, int16_t y, TFT_eSprite *_spr = nullptr);

           // Draw a Jpeg to the TFT, or to a Sprite if a Sprite instance is included
  void     drawJpeg(String filename, int16_t xpos, int16_t ypos, TFT_eSprite *_spr = nullptr);

           // Draw a Jpeg stored in a program memory array to the TFT
  void     drawJpeg(const uint8_t arrayname[], uint32_t array_size, int16_t xpos, int16_t ypos);

           // List information about a Jpeg file to the Serial port
  void     jpegInfo(String filename);
  void     jpegInfo(const uint8_t arrayname[], uint32_t array_size);

           // Draw a progress bar on the screen
  void     drawProgressBar(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t percent, uint16_t frameColor, uint16_t barColor);

           // Change the luminance of a defined colour
  uint16_t luminance(uint16_t color, uint8_t luminance);
  uint16_t luminance(uint8_t red, uint8_t green, uint8_t blue, uint8_t luminance);

           // Return a rainbow colour for a spectrum value 0-192
  uint16_t rainbowColor(uint8_t spectrum);

           // List files in the SPIFFS for ESP8266 or ESP32
  void     listSPIFFS(void);

  private:

  TFT_eSPI *_tft;

           // Support functions for the drawBMP() function
  uint16_t read16(fs::File &f);
  uint32_t read32(fs::File &f);

 protected:

};
