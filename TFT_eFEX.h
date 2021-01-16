/***************************************************************************************/
// The following class class adds additional functions to the TFT_eSPI library.
// The class inherits the generic drawing graphics functions from the TFT_eSPI class.

// Created by Bodmer 15/11/2018
// See license.txt in root folder of library
/***************************************************************************************/

#ifndef _TFT_eFEXH_
#define _TFT_eFEXH_

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
  void     drawJpeg(const uint8_t arrayname[], uint32_t array_size, int16_t xpos, int16_t ypos, TFT_eSprite *_spr = nullptr);

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
  void     drawStringRTLAr(const String& string);
  void     drawStringRTLAr(const char *string, int32_t *x, int32_t *y);
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

#define ISOLATED 1
#define INITIAL 2
#define MEDIAL 3
#define FINAL 4

const uint8_t CODECOUNT = 180;
const uint16_t codes[CODECOUNT][5] PROGMEM = {
  //'<isolated>', '<initial>', '<medial>', '<final>'
//#ARABIC LETTER HAMZA
  0x0621,0xFE80, 0, 0, 0,
//#ARABIC LETTER ALEF WITH MADDA ABOVE
  0x0622,0x0622, 0, 0, 0xFE82,
//#ARABIC LETTER ALEF WITH HAMZA ABOVE
  0x0623,0x0623, 0, 0, 0xFE84,
//#ARABIC LETTER WAW WITH HAMZA ABOVE
  0x0624,0x0624, 0, 0, 0xFE86,
//#ARABIC LETTER ALEF WITH HAMZA BELOW
  0x0625,0x0625, 0, 0, 0xFE88,
//#ARABIC LETTER YEH WITH HAMZA ABOVE
  0x0626,0x0626, 0xFE8B, 0xFE8C, 0xFE8A,
//#ARABIC LETTER ALEF
  0x0627,0x0627, 0, 0, 0xFE8E,
//#ARABIC LETTER BEH
  0x0628,0x0628, 0xFE91, 0xFE92, 0xFE90,
//#ARABIC LETTER TEH MARBUTA
  0x0629,0x0629, 0, 0, 0xFE94,
//#ARABIC LETTER TEH
  0x062A,0x062A, 0xFE97, 0xFE98, 0xFE96,
//#ARABIC LETTER THEH
  0x062B,0x062B, 0xFE9B, 0xFE9C, 0xFE9A,
//#ARABIC LETTER JEEM
  0x062C,0x062C, 0xFE9F, 0xFEA0, 0xFE9E,
//#ARABIC LETTER HAH
  0x062D,0xFEA1, 0xFEA3, 0xFEA4, 0xFEA2,
//#ARABIC LETTER KHAH
  0x062E,0x062E, 0xFEA7, 0xFEA8, 0xFEA6,
//#ARABIC LETTER DAL
  0x062F,0x062F, 0, 0, 0xFEAA,
//#ARABIC LETTER THAL
  0x0630,0x0630, 0, 0, 0xFEAC,
//#ARABIC LETTER REH
  0x0631,0x0631, 0, 0, 0xFEAE,
//#ARABIC LETTER ZAIN
  0x0632,0x0632, 0, 0, 0xFEB0,
//#ARABIC LETTER SEEN
  0x0633,0x0633, 0xFEB3, 0xFEB4, 0xFEB2,
//#ARABIC LETTER SHEEN
  0x0634,0x0634, 0xFEB7, 0xFEB8, 0xFEB6,
//#ARABIC LETTER SAD
  0x0635,0x0635, 0xFEBB, 0xFEBC, 0xFEBA,
//#ARABIC LETTER DAD
  0x0636,0x0636, 0xFEBF, 0xFEC0, 0xFEBE,
//#ARABIC LETTER TAH
  0x0637,0x0637, 0xFEC3, 0xFEC4, 0xFEC2,
//#ARABIC LETTER ZAH
  0x0638,0x0638, 0xFEC7, 0xFEC8, 0xFEC6,
//#ARABIC LETTER AIN
  0x0639,0x0639, 0xFECB, 0xFECC, 0xFECA,
//#ARABIC LETTER GHAIN
  0x063A,0x063A, 0xFECF, 0xFED0, 0xFECE,
//#ARABIC TATWEEL
  0x0640,0x0640, 0x0640, 0x0640, 0x0640,
//#ARABIC LETTER FEH
  0x0641,0x0641, 0xFED3, 0xFED4, 0xFED2,
//#ARABIC LETTER QAF
  0x0642,0x0642, 0xFED7, 0xFED8, 0xFED6,
//#ARABIC LETTER KAF
  0x0643,0x0643, 0xFEDB, 0xFEDC, 0xFEDA,
//#ARABIC LETTER LAM
  0x0644,0x0644, 0xFEDF, 0xFEE0, 0xFEDE,
//#ARABIC LETTER MEEM
  0x0645,0x0645, 0xFEE3, 0xFEE4, 0xFEE2,
//#ARABIC LETTER NOON
  0x0646,0x0646, 0xFEE7, 0xFEE8, 0xFEE6,
//#ARABIC LETTER HEH
  0x0647,0x0647, 0xFEEB, 0xFEEC, 0xFEEA,
//#ARABIC LETTER WAW
  0x0648,0x0648, 0, 0, 0xFEEE,
//#ARABIC LETTER(UIGHUR KAZAKH KIRGHIZ) ? ALEF MAKSURA
  0x0649,0x0649, 0xFBE8, 0xFBE9, 0xFEF0,
//#ARABIC LETTER YEH
  0x064A,0x064A, 0xFEF3, 0xFEF4, 0xFEF2,
//#ARABIC LETTER ALEF WASLA
  0x0671,0x0671, 0, 0, 0xFB51,
//#ARABIC LETTER U WITH HAMZA ABOVE
  0x0677,0x0677, 0, 0, 0,
//#ARABIC LETTER TTEH
  0x0679,0x0679, 0xFB68, 0xFB69, 0xFB67,
//#ARABIC LETTER TTEHEH
  0x067A,0x067A, 0xFB60, 0xFB61, 0xFB5F,
//#ARABIC LETTER BEEH
  0x067B,0x067B, 0xFB54, 0xFB55, 0xFB53,
//#ARABIC LETTER PEH
  0x067E,0x067E, 0xFB58, 0xFB59, 0xFB57,
//#ARABIC LETTER TEHEH
  0x067F,0x067F, 0xFB64, 0xFB65, 0xFB63,
//#ARABIC LETTER BEHEH
  0x0680,0x0680, 0xFB5C, 0xFB5D, 0xFB5B,
//#ARABIC LETTER NYEH
  0x0683,0x0683, 0xFB78, 0xFB79, 0xFB77,
//#ARABIC LETTER DYEH
  0x0684,0x0684, 0xFB74, 0xFB75, 0xFB73,
//#ARABIC LETTER TCHEH
  0x0686,0x0686, 0xFB7C, 0xFB7D, 0xFB7B,
//#ARABIC LETTER TCHEHEH
  0x0687,0x0687, 0xFB80, 0xFB81, 0xFB7F,
//#ARABIC LETTER DDAL
  0x0688,0x0688, 0, 0, 0xFB89,
//#ARABIC LETTER DAHAL
  0x068C,0x068C, 0, 0, 0xFB85,
//#ARABIC LETTER DDAHAL
  0x068D,0x068D, 0, 0, 0xFB83,
//#ARABIC LETTER DUL
  0x068E,0x068E, 0, 0, 0xFB87,
//#ARABIC LETTER RREH
  0x0691,0x0691, 0, 0, 0xFB8D,
//#ARABIC LETTER JEH
  0x0698,0x0698, 0, 0, 0xFB8B,
//#ARABIC LETTER VEH
  0x06A4,0x06A4, 0xFB6C, 0xFB6D, 0xFB6B,
//#ARABIC LETTER PEHEH
  0x06A6,0x06A6, 0xFB70, 0xFB71, 0xFB6F,
//#ARABIC LETTER KEHEH
  0x06A9,0x06A9, 0xFB90, 0xFB91, 0xFB8F,
//#ARABIC LETTER NG
  0x06AD,0x06AD, 0xFBD5, 0xFBD6, 0xFBD4,
//#ARABIC LETTER GAF
  0x06AF,0x06AF, 0xFB94, 0xFB95, 0xFB93,
//#ARABIC LETTER NGOEH
  0x06B1,0x06B1, 0xFB9C, 0xFB9D, 0xFB9B,
//#ARABIC LETTER GUEH
  0x06B3,0x06B3, 0xFB98, 0xFB99, 0xFB97,
//#ARABIC LETTER NOON GHUNNA
  0x06BA,0x06BA, 0, 0, 0xFB9F,
//#ARABIC LETTER RNOON
  0x06BB,0x06BB, 0xFBA2, 0xFBA3, 0xFBA1,
//#ARABIC LETTER HEH DOACHASHMEE
  0x06BE,0x06BE, 0xFBAC, 0xFBAD, 0xFBAB,
//#ARABIC LETTER HEH WITH YEH ABOVE
  0x06C0,0x06C0, 0, 0, 0xFBA5,
//#ARABIC LETTER HEH GOAL
  0x06C1,0x06C1, 0xFBA8, 0xFBA9, 0xFBA7,
//#ARABIC LETTER KIRGHIZ OE
  0x06C5,0x06C5, 0, 0, 0xFBE1,
//#ARABIC LETTER OE
  0x06C6,0x06C6, 0, 0, 0xFBDA,
//#ARABIC LETTER U
  0x06C7,0x06C7, 0, 0, 0xFBD8,
//#ARABIC LETTER YU
  0x06C8,0x06C8, 0, 0, 0xFBDC,
//#ARABIC LETTER KIRGHIZ YU
  0x06C9,0x06C9, 0, 0, 0xFBE3,
//#ARABIC LETTER VE
  0x06CB,0x06CB, 0, 0, 0xFBDF,
//#ARABIC LETTER FARSI YEH
  0x06CC,0x06CC, 0xFBFE, 0xFBFF, 0xFBFD,
//#ARABIC LETTER E
  0x06D0,0x06D0, 0xFBE6, 0xFBE7, 0xFBE5,
//#ARABIC LETTER YEH BARREE
  0x06D2,0x06D2, 0, 0, 0xFBAF,
//#ARABIC LETTER YEH BARREE WITH HAMZA ABOVE
  0x06D3,0x06D3, 0, 0, 0xFBB1,
//#Kurdish letter YEAH
  0x06ce,0xE004, 0xE005, 0xE006, 0xE004,
//#Kurdish letter Hamza same as arabic Teh without the point
  0x06d5,0x06d5, 0, 0, 0xE000,
};

};

#endif //ifndef _TFT_eFEXH_
