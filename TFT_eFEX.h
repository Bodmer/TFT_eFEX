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
  #include "rom/tjpgd.h" // For native ESP32 jpeg decoder
  #include "SPIFFS.h"    // ESP32 only
#endif

// Screen server setup

#define PIXEL_TIMEOUT 100     // 100ms Time-out between pixel requests
#define START_TIMEOUT 10000   // 10s Maximum time to wait at start transfer

#define BITS_PER_PIXEL 16     // 24 for RGB colour format, 16 for 565 colour format

// File names must be alpha-numeric characters (0-9, a-z, A-Z) or "/" underscore "_"
// other ascii characters are stripped out by client, including / generates
// sub-directories
#define DEFAULT_FILENAME "tft_screenshots/screenshot" // In case none is specified
#define FILE_TYPE "png"       // jpg, bmp, png, tif are valid

// Filename extension
// '#' = add incrementing number, '@' = add timestamp, '%' add millis() timestamp,
// '*' = add nothing
// '@' and '%' will generate new unique filenames, so beware of cluttering up your
// hard drive with lots of images! The PC client sketch is set to limit the number of
// saved images to 1000 and will then prompt for a restart.
#define FILE_EXT  '@'         

// Number of pixels to send in a burst (minimum of 1), no benefit above 8
// NPIXELS values and render times:
// NPIXELS 1 = use readPixel() = >5s and 16 bit pixels only
// NPIXELS >1 using rectRead() 2 = 1.75s, 4 = 1.68s, 8 = 1.67s
#define NPIXELS 1  // Must be integer division of both TFT width and TFT height

#ifdef ESP32 // For native jpeg decoder
typedef enum {
    JPEG_DIV_NONE,
    JPEG_DIV_2,
    JPEG_DIV_4,
    JPEG_DIV_8,
    JPEG_DIV_MAX
} jpeg_div_t;
#endif

// End of screens erver setup

class TFT_eFEX : public TFT_eSPI {

 public:

  TFT_eFEX(TFT_eSPI *tft);

           // Draw a bezier curve of a defined colour between specified points
  void     drawBezier(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint16_t color);  
  void     drawBezierSegment(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint16_t color);

           // Draw a bitmap stored in SPIFFS to the TFT or a Sprite if a Sprite instance is included
  void     drawBmp(String filename, int16_t x, int16_t y, TFT_eSprite *_spr = nullptr);
//To do:  void     drawBmp(const char *filename, int16_t x, int16_t y, TFT_eSprite *_spr = nullptr);

           // Draw a Jpeg to the TFT, or to a Sprite if a Sprite instance is included (uses JPEGDecoder library)
  void     drawJpeg(String filename, int16_t xpos, int16_t ypos, TFT_eSprite *_spr = nullptr);

           // Draw a Jpeg stored in a program memory array to the TFT (uses JPEGDecoder library)
  void     drawJpeg(const uint8_t arrayname[], uint32_t array_size, int16_t xpos, int16_t ypos);

           // List information about a Jpeg file to the Serial port (uses JPEGDecoder library)
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

           // Support Right To Left (RTL) character rendering
  void     setCursorRTL(int32_t cx, int32_t cy);
  void     drawStringRTL(const String& string);
  void     drawStringRTL(const char *string, int32_t *x, int32_t *y);
  void     drawStringLTR(const String& string);
  void     drawStringLTR(const char *string, int32_t *x, int32_t *y);

           // Screen server call with or without a filename for PC stored image file
  bool     screenServer(void);
  bool     screenServer(String filename);

#ifdef ESP32
           // Draw a jpeg stored in an array using the ESP32 native decoder, can crop and scale
  bool     drawJpg(const uint8_t * jpg_data, size_t jpg_len, uint16_t x=0, uint16_t y=0, uint16_t maxWidth=0, uint16_t maxHeight=0, uint16_t offX=0, uint16_t offY=0, jpeg_div_t scale=JPEG_DIV_NONE);
           // Draw a jpeg stored in a file using the ESP32 native decoder, can crop and scale (Tested with SPIFFS file)
  bool     drawJpgFile(fs::FS &fs, const char * path, uint16_t x=0, uint16_t y=0, uint16_t maxWidth=0, uint16_t maxHeight=0, uint16_t offX=0, uint16_t offY=0, jpeg_div_t scale=JPEG_DIV_NONE);
#endif

  private:

  TFT_eSPI *_tft;

           // Support functions for the drawBMP() function
  uint16_t read16(fs::File &f);
  uint32_t read32(fs::File &f);

  bool     serialScreenServer(String filename);
  void     sendParameters(String filename);
 protected:

int32_t rtl_cursorX = 0; // RTL cursor positions
int32_t rtl_cursorY = 0;

};
