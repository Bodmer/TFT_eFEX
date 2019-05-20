/***************************************************************************************/
// The following class class adds additional functions to the TFT_eSPI library.
// The class inherits the generic drawing graphics functions from the TFT_eSPI class.

// Created by Bodmer 15/11/2018
// See license.txt in root folder of library
/***************************************************************************************/

#include "TFT_eFEX.h"


/***************************************************************************************
** Function name:           TFT_eFEX
** Description:             Class constructor
***************************************************************************************/
TFT_eFEX::TFT_eFEX(TFT_eSPI *tft)
{
  _tft = tft;  // Pointer to tft class so we can call member functions

}


/***************************************************************************************
** Function name:           drawQuadraticBezier
** Description:             Draw a bezier curve between points
***************************************************************************************/
// Plot any quadratic Bezier curve, no restrictions on point positions
// See source code http://members.chello.at/~easyfilter/bresenham.c
void TFT_eFEX::drawBezier(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint16_t color)
{
  int32_t x = x0 - x1, y = y0 - y1;
  double t = x0 - 2 * x1 + x2, r;

  if (x * (x2 - x1) > 0) {
    if (y * (y2 - y1) > 0)
      if (fabs((y0 - 2 * y1 + y2) / t * x) > abs(y)) {
        x0 = x2; x2 = x + x1; y0 = y2; y2 = y + y1;
      }
    t = (x0 - x1) / t;
    r = (1 - t) * ((1 - t) * y0 + 2.0 * t * y1) + t * t * y2;
    t = (x0 * x2 - x1 * x1) * t / (x0 - x1);
    x = floor(t + 0.5); y = floor(r + 0.5);
    r = (y1 - y0) * (t - x0) / (x1 - x0) + y0;
    drawBezierSegment(x0, y0, x, floor(r + 0.5), x, y, color);
    r = (y1 - y2) * (t - x2) / (x1 - x2) + y2;
    x0 = x1 = x; y0 = y; y1 = floor(r + 0.5);
  }
  if ((y0 - y1) * (y2 - y1) > 0) {
    t = y0 - 2 * y1 + y2; t = (y0 - y1) / t;
    r = (1 - t) * ((1 - t) * x0 + 2.0 * t * x1) + t * t * x2;
    t = (y0 * y2 - y1 * y1) * t / (y0 - y1);
    x = floor(r + 0.5); y = floor(t + 0.5);
    r = (x1 - x0) * (t - y0) / (y1 - y0) + x0;
    drawBezierSegment(x0, y0, floor(r + 0.5), y, x, y, color);
    r = (x1 - x2) * (t - y2) / (y1 - y2) + x2;
    x0 = x; x1 = floor(r + 0.5); y0 = y1 = y;
  }
  drawBezierSegment(x0, y0, x1, y1, x2, y2, color);
}


/***************************************************************************************
** Function name:           drawBezierSegment
** Description:             Draw a bezier segment curve between points
***************************************************************************************/

//  x0, y0 defines p0 etc.
//  coordinates for p0-p3 must be sequentially increasing or decreasing so
//  n0 <= n1 <= n2 or n0 >= n1 >= n2 where n is x or y, e.g.
//
//         p1 x           .x p2      p2 x.
//                   .                       .     x p1
//               .                               .
//            .                                     .
//          .                                         .
//        .                                             .
//      .                                                 .
//  p0 x                                                   x p0
//
void TFT_eFEX::drawBezierSegment(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint16_t color)
{
  // Check if coordinates are sequential (replaces assert)
  if (((x2 >= x1 && x1 >= x0) || (x2 <= x1 && x1 <= x0))
      && ((y2 >= y1 && y1 >= y0) || (y2 <= y1 && y1 <= y0)))
  {
    // Coordinates are sequential
    int32_t sx = x2 - x1, sy = y2 - y1;
    int32_t xx = x0 - x1, yy = y0 - y1, xy;
    float dx, dy, err, cur = xx * sy - yy * sx;

    if (sx * (int32_t)sx + sy * (int32_t)sy > xx * xx + yy * yy) {
      x2 = x0; x0 = sx + x1; y2 = y0; y0 = sy + y1; cur = -cur;
    }
    _tft->startWrite();
    if (cur != 0) {
      xx += sx; xx *= sx = x0 < x2 ? 1 : -1;
      yy += sy; yy *= sy = y0 < y2 ? 1 : -1;
      xy = 2 * xx * yy; xx *= xx; yy *= yy;
      if (cur * sx * sy < 0) {
        xx = -xx; yy = -yy; xy = -xy; cur = -cur;
      }
      dx = 4.0 * sy * cur * (x1 - x0) + xx - xy;
      dy = 4.0 * sx * cur * (y0 - y1) + yy - xy;
      xx += xx; yy += yy; err = dx + dy + xy;
      do {
        _tft->drawPixel(x0, y0, color);
        if (x0 == x2 && y0 == y2)
        {
          _tft->endWrite();
          return;
        }
        y1 = 2 * err < dx;
        if (2 * err > dy) {
          x0 += sx;
          dx -= xy;
          err += dy += yy;
        }
        if (    y1    ) {
          y0 += sy;
          dy -= xy;
          err += dx += xx;
        }
        yield();
      } while (dy < dx );
    }
    _tft->drawLine(x0, y0, x2, y2, color);
    _tft->endWrite();
  }
  else Serial.println("Bad coordinate set - non-sequential!");
}


/***************************************************************************************
** Function name:           drawBmp
** Description:             draw a bitmap stored in SPIFFS onto the TFT or in a Sprite
***************************************************************************************/
//void TFT_eFEX::drawBmp(const char *filename, int16_t x, int16_t y, TFT_eSprite *_spr) {
void TFT_eFEX::drawBmp(String filename, int16_t x, int16_t y, TFT_eSprite *_spr) {

  if ( (_spr == nullptr) && ((x >= _tft->width()) || (y >= _tft->height()))) return;

  fs::File bmpFS;

  // Note: ESP32 passes "open" test even if file does not exist, whereas ESP8266 returns NULL
  if ( !SPIFFS.exists(filename) )
  {
    Serial.println(F(" File not found")); // Can comment out if not needed
    return;
  }

  // Open requested file
  bmpFS = SPIFFS.open(filename, "r");

  if (!bmpFS)
  {
    Serial.print("Bitmap file not found");
    return;
  }

  uint32_t seekOffset;
  uint16_t w, h, row, col;
  uint8_t  r, g, b;

  //uint32_t startTime = millis();

  if (read16(bmpFS) == 0x4D42)
  {
    read32(bmpFS);
    read32(bmpFS);
    seekOffset = read32(bmpFS);
    read32(bmpFS);
    w = read32(bmpFS);
    h = read32(bmpFS);

    if ((read16(bmpFS) == 1) && (read16(bmpFS) == 24) && (read32(bmpFS) == 0))
    {
      y += h - 1;
      
      bool oldSwapBytes = _tft->getSwapBytes();

      _tft->setSwapBytes(_spr == nullptr);

      bmpFS.seek(seekOffset);

      uint16_t padding = (4 - ((w * 3) & 3)) & 3;
      uint8_t lineBuffer[w * 3 + padding];

      for (row = 0; row < h; row++) {
        
        bmpFS.read(lineBuffer, sizeof(lineBuffer));
        uint8_t*  bptr = lineBuffer;
        uint16_t* tptr = (uint16_t*)lineBuffer;
        // Convert 24 to 16 bit colours
        for (col = 0; col < w; col++)
        {
          b = *bptr++;
          g = *bptr++;
          r = *bptr++;
          *tptr++ = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
        }

        // Push the pixel row to screen, pushImage will crop the line if needed
        // y is decremented as the BMP image is drawn bottom up
        if (_spr == nullptr) _tft->pushImage(x, y--, w, 1, (uint16_t*)lineBuffer);
        else                 _spr->pushImage(x, y--, w, 1, (uint16_t*)lineBuffer);
      }

      _tft->setSwapBytes(oldSwapBytes); // Restore original setting

      //Serial.print("Loaded in "); Serial.print(millis() - startTime);
      //Serial.println(" ms");
    }
    else Serial.println("BMP format not recognised.");
  }
  bmpFS.close();
}

// These read 16 and 32-bit types from the file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t TFT_eFEX::read16(fs::File &f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t TFT_eFEX::read32(fs::File &f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}


/***************************************************************************************
** Function name:           drawJpeg
** Description:             draw a jpeg stored in SPIFFS onto the TFT
***************************************************************************************/
void TFT_eFEX::drawJpeg(String filename, int16_t xpos, int16_t ypos, TFT_eSprite *_spr) {

  if ( (_spr == nullptr) && ((xpos >= _tft->width()) || (ypos >= _tft->height()))) return;

  // Note: ESP32 passes "open" test even if file does not exist, whereas ESP8266 returns NULL
  if ( !SPIFFS.exists(filename) )
  {
    Serial.println(F(" Jpeg file not found")); // Can comment out if not needed
    return;
  }

  // Open the named file (the Jpeg decoder library will close it after rendering image)
  fs::File jpegFile = SPIFFS.open( filename, "r");    // File handle reference for SPIFFS
  //  File jpegFile = SD.open( filename, FILE_READ);  // or, file handle reference for SD library
 
  // Use one of the three following methods to initialise the decoder:
  //boolean decoded = JpegDec.decodeFsFile(jpegFile); // Pass a SPIFFS file handle to the decoder,
  //boolean decoded = JpegDec.decodeSdFile(jpegFile); // or pass the SD file handle to the decoder,
  boolean decoded = JpegDec.decodeFsFile(filename);  // or pass the filename (leading / distinguishes SPIFFS files)
                                   // Note: the filename can be a String or character array type
  if (decoded) {
    // render the image onto the screen at given coordinates

    // retrieve information about the image
    uint16_t  *pImg;
    uint16_t mcu_w = JpegDec.MCUWidth;
    uint16_t mcu_h = JpegDec.MCUHeight;
    int32_t  max_x = JpegDec.width;
    int32_t  max_y = JpegDec.height;

    // Jpeg images are draw as a set of image block (tiles) called Minimum Coding Units (MCUs)
    // Typically these MCUs are 16x16 pixel blocks
    // Determine the width and height of the right and bottom edge image blocks
    int32_t min_w = jpg_min(mcu_w, max_x % mcu_w);
    int32_t min_h = jpg_min(mcu_h, max_y % mcu_h);

    // save the current image block size
    int32_t win_w = mcu_w;
    int32_t win_h = mcu_h;

    // record the current time so we can measure how long it takes to draw an image
    uint32_t drawTime = millis();

    // Retrieve the width and height of the display/Sprite
    int32_t disp_w = _tft->width();
    int32_t disp_h = _tft->height();
    if (_spr != nullptr)
    {
      disp_w = _spr->width(); 
      disp_h = _spr->height();
    }

    bool oldSwapBytes = _tft->getSwapBytes();
    _tft->setSwapBytes(true);

    // save the coordinate of the right and bottom edges to assist image cropping
    // to the screen size
    max_x += xpos;
    max_y += ypos;

    // read each MCU block until there are no more

    while ( JpegDec.read())
    { // Normal byte order read
      // save a pointer to the image block
      pImg = JpegDec.pImage;

      // calculate where the image block should be drawn on the screen
      int mcu_x = JpegDec.MCUx * mcu_w + xpos;
      int mcu_y = JpegDec.MCUy * mcu_h + ypos;

      if ( mcu_y < disp_h )
      {
        // check if the image block size needs to be changed for the right edge
        if (mcu_x + mcu_w <= max_x) win_w = mcu_w;
        else win_w = min_w;

        // check if the image block size needs to be changed for the bottom edge
        if (mcu_y + mcu_h <= max_y) win_h = mcu_h;
        else win_h = min_h;

        // copy pixels into a smaller block
        if (win_w != mcu_w)
        {
          for (int h = 1; h < win_h; h++)
          {
            memcpy(pImg + h * win_w, pImg + h * mcu_w, win_w << 1);
          }
        }

      if (_spr == nullptr) _tft->pushImage(mcu_x, mcu_y, win_w, win_h, pImg);
      else _spr->pushImage(mcu_x, mcu_y, win_w, win_h, pImg);
      }
      else
      {
        JpegDec.abort();
      }
    }

    _tft->setSwapBytes(oldSwapBytes); // Restore original setting

    // calculate how long it took to draw the image
    drawTime = millis() - drawTime; // Calculate the time it took

    // print the results to the serial port
    //Serial.print  ("Total render time was    : "); Serial.print(drawTime); Serial.println(" ms");
    //Serial.println("=====================================");

  }
  else
  {
    Serial.println("Jpeg file format not supported!");
  }
}


/***************************************************************************************
** Function name:           drawJpeg
** Description:             draw a jpeg stored in FLASH onto the TFT
***************************************************************************************/
void TFT_eFEX::drawJpeg(const uint8_t arrayname[], uint32_t array_size, int16_t xpos, int16_t ypos) {

  boolean decoded = JpegDec.decodeArray(arrayname, array_size);

  if (decoded) {
    // render the image onto the screen at given coordinates

    // retrieve information about the image
    uint16_t  *pImg;
    uint16_t mcu_w = JpegDec.MCUWidth;
    uint16_t mcu_h = JpegDec.MCUHeight;
    int32_t  max_x = JpegDec.width;
    int32_t  max_y = JpegDec.height;

    // Jpeg images are draw as a set of image block (tiles) called Minimum Coding Units (MCUs)
    // Typically these MCUs are 16x16 pixel blocks
    // Determine the width and height of the right and bottom edge image blocks
    int32_t min_w = jpg_min(mcu_w, max_x % mcu_w);
    int32_t min_h = jpg_min(mcu_h, max_y % mcu_h);

    // save the current image block size
    int32_t win_w = mcu_w;
    int32_t win_h = mcu_h;

    // record the current time so we can measure how long it takes to draw an image
    uint32_t drawTime = millis();

    // Retrieve the width and height of the display/Sprite
    int32_t disp_w = _tft->width();
    int32_t disp_h = _tft->height();

    bool oldSwapBytes = _tft->getSwapBytes();
    _tft->setSwapBytes(true);

    // save the coordinate of the right and bottom edges to assist image cropping
    // to the screen size
    max_x += xpos;
    max_y += ypos;

    // read each MCU block until there are no more

    while ( JpegDec.read())
    { // Normal byte order read
      // save a pointer to the image block
      pImg = JpegDec.pImage;

      // calculate where the image block should be drawn on the screen
      int mcu_x = JpegDec.MCUx * mcu_w + xpos;
      int mcu_y = JpegDec.MCUy * mcu_h + ypos;

      if ( mcu_y < disp_h )
      {
        // check if the image block size needs to be changed for the right edge
        if (mcu_x + mcu_w <= max_x) win_w = mcu_w;
        else win_w = min_w;

        // check if the image block size needs to be changed for the bottom edge
        if (mcu_y + mcu_h <= max_y) win_h = mcu_h;
        else win_h = min_h;

        // copy pixels into a smaller block
        if (win_w != mcu_w)
        {
          for (int h = 1; h < win_h; h++)
          {
            memcpy(pImg + h * win_w, pImg + h * mcu_w, win_w << 1);
          }
        }

        _tft->pushImage(mcu_x, mcu_y, win_w, win_h, pImg);
      }
      else
      {
        JpegDec.abort();
      }
    }

    _tft->setSwapBytes(oldSwapBytes); // Restore original setting

    // calculate how long it took to draw the image
    drawTime = millis() - drawTime; // Calculate the time it took

    // print the results to the serial port
    //Serial.print  ("Total render time was    : "); Serial.print(drawTime); Serial.println(" ms");
    //Serial.println("=====================================");

  }
  else
  {
    Serial.println("Jpeg file format not supported!");
  }
}


/***************************************************************************************
** Function name:           jpegInfo
** Description:             Print information decoded from the Jpeg image
***************************************************************************************/
void TFT_eFEX::jpegInfo(String filename) {
  // Note: ESP32 passes "open" test even if file does not exist, whereas ESP8266 returns NULL
  if ( !SPIFFS.exists(filename) )
  {
    Serial.println(F(" Jpeg file not found")); // Can comment out if not needed
    return;
  }

  // Open the named file (the Jpeg decoder library abort will close it after reading image info)
  fs::File jpegFile = SPIFFS.open( filename, "r");    // File handle reference for SPIFFS
  //  File jpegFile = SD.open( filename, FILE_READ);  // or, file handle reference for SD library
 
  // Use one of the three following methods to initialise the decoder:
  //boolean decoded = JpegDec.decodeFsFile(jpegFile); // Pass a SPIFFS file handle to the decoder,
  //boolean decoded = JpegDec.decodeSdFile(jpegFile); // or pass the SD file handle to the decoder,
  boolean decoded = JpegDec.decodeFsFile(filename);  // or pass the filename (leading / distinguishes SPIFFS files)

  static const char line[] PROGMEM =  "===============";
  if (decoded) {
    Serial.println(FPSTR(line));
    Serial.println(F("JPEG image info"));
    Serial.println(FPSTR(line));
    Serial.print  (F("Width      :")); Serial.println(JpegDec.width);
    Serial.print  (F("Height     :")); Serial.println(JpegDec.height);
    Serial.print  (F("Components :")); Serial.println(JpegDec.comps);
    Serial.print  (F("MCU / row  :")); Serial.println(JpegDec.MCUSPerRow);
    Serial.print  (F("MCU / col  :")); Serial.println(JpegDec.MCUSPerCol);
    Serial.print  (F("Scan type  :")); Serial.println(JpegDec.scanType);
    Serial.print  (F("MCU width  :")); Serial.println(JpegDec.MCUWidth);
    Serial.print  (F("MCU height :")); Serial.println(JpegDec.MCUHeight);
    Serial.println(FPSTR(line));
    Serial.println("");
    JpegDec.abort();    // Close file and tidy up
  }
}


/***************************************************************************************
** Function name:           jpegInfo
** Description:             Print information decoded from the Jpeg image
***************************************************************************************/
void TFT_eFEX::jpegInfo(const uint8_t arrayname[], uint32_t array_size) {

  boolean decoded = JpegDec.decodeArray(arrayname, array_size);
  static const char line[] PROGMEM =  "===============";

  if (decoded) {
    Serial.println(FPSTR(line));
    Serial.println(F("JPEG image info"));
    Serial.println(FPSTR(line));
    Serial.print  (F("Width      :")); Serial.println(JpegDec.width);
    Serial.print  (F("Height     :")); Serial.println(JpegDec.height);
    Serial.print  (F("Components :")); Serial.println(JpegDec.comps);
    Serial.print  (F("MCU / row  :")); Serial.println(JpegDec.MCUSPerRow);
    Serial.print  (F("MCU / col  :")); Serial.println(JpegDec.MCUSPerCol);
    Serial.print  (F("Scan type  :")); Serial.println(JpegDec.scanType);
    Serial.print  (F("MCU width  :")); Serial.println(JpegDec.MCUWidth);
    Serial.print  (F("MCU height :")); Serial.println(JpegDec.MCUHeight);
    Serial.println(FPSTR(line));
    Serial.println("");
    JpegDec.abort();    // Close file and tidy up
  }
}


/***************************************************************************************
** Function name:           drawProgressBar
** Description:             Draw a progress bar - increasing percentage only
***************************************************************************************/
void TFT_eFEX::drawProgressBar(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t percent, uint16_t frameColor, uint16_t barColor) {
  if (percent == 0) {
    _tft->fillRoundRect(x, y, w, h, 3, TFT_BLACK);
  }
  uint8_t margin = 2;
  uint16_t barHeight = h - 2 * margin;
  uint16_t barWidth  = w - 2 * margin;
  _tft->drawRoundRect(x, y, w, h, 3, frameColor);
  _tft->fillRect(x + margin, y + margin, barWidth * percent / 100.0, barHeight, barColor);
}


/***************************************************************************************
** Function name:           luminance
** Description:             return a 16 bit colour with reduced luminance
***************************************************************************************/
uint16_t TFT_eFEX::luminance(uint16_t color, uint8_t luminance)
{
  // Extract rgb colours and stretch range to 0 - 255
  uint16_t r = (color & 0xF800) >> 8; r |= (r >> 5);
  uint16_t g = (color & 0x07E0) >> 3; g |= (g >> 6);
  uint16_t b = (color & 0x001F) << 3; b |= (b >> 5);

  b = ((b * (uint16_t)luminance + 255) >> 8) & 0x00F8;
  g = ((g * (uint16_t)luminance + 255) >> 8) & 0x00FC;
  r = ((r * (uint16_t)luminance + 255) >> 8) & 0x00F8;

  return (r << 8) | (g << 3) | (b >> 3);
}


/***************************************************************************************
** Function name:           luminance
** Description:             return a 16 bit colour from an RGB with reduced luminance
***************************************************************************************/
uint16_t TFT_eFEX::luminance(uint8_t red, uint8_t green, uint8_t blue, uint8_t luminance)
{
  uint16_t b = (((uint16_t)blue  * (uint16_t)luminance + 255) >> 8) & 0x00F8;
  uint16_t g = (((uint16_t)green * (uint16_t)luminance + 255) >> 8) & 0x00FC;
  uint16_t r = (((uint16_t)red   * (uint16_t)luminance + 255) >> 8) & 0x00F8;

  return (r << 8) | (g << 3) | (b >> 3);
}


/***************************************************************************************
** Function name:           rainbowColor
** Description:             Return a 16 bit rainbow colour
***************************************************************************************/

  // If 'spectrum' is in the range 0-159 it is converted to a spectrum colour
  // from 0 = red through to 127 = blue to 159 = violet
  // Extending the range to 0-191 adds a further violet to red band
 
uint16_t TFT_eFEX::rainbowColor(uint8_t spectrum)
{
  spectrum = spectrum%192;
  
  uint8_t red   = 0; // Red is the top 5 bits of a 16 bit colour spectrum
  uint8_t green = 0; // Green is the middle 6 bits, but only top 5 bits used here
  uint8_t blue  = 0; // Blue is the bottom 5 bits

  uint8_t sector = spectrum >> 5;
  uint8_t amplit = spectrum & 0x1F;

  switch (sector)
  {
    case 0:
      red   = 0x1F;
      green = amplit; // Green ramps up
      blue  = 0;
      break;
    case 1:
      red   = 0x1F - amplit; // Red ramps down
      green = 0x1F;
      blue  = 0;
      break;
    case 2:
      red   = 0;
      green = 0x1F;
      blue  = amplit; // Blue ramps up
      break;
    case 3:
      red   = 0;
      green = 0x1F - amplit; // Green ramps down
      blue  = 0x1F;
      break;
    case 4:
      red   = amplit; // Red ramps up
      green = 0;
      blue  = 0x1F;
      break;
    case 5:
      red   = 0x1F;
      green = 0;
      blue  = 0x1F - amplit; // Blue ramps down
      break;
  }

  return red << 11 | green << 6 | blue;
}


/***************************************************************************************
** Function name:           listSPIFFS
** Description:             Listing SPIFFS files
***************************************************************************************/
#ifdef ESP8266
void TFT_eFEX::listSPIFFS(void) {
  Serial.println(F("\r\nListing SPIFFS files:"));

  fs::Dir dir = SPIFFS.openDir("/"); // Root directory

  static const char line[] PROGMEM =  "=================================================";
  Serial.println(FPSTR(line));
  Serial.println(F("  File name                              Size"));
  Serial.println(FPSTR(line));

  while (dir.next()) {
    String fileName = dir.fileName();
    Serial.print(fileName);
    int spaces = 33 - fileName.length(); // Tabulate nicely
    if (spaces < 1) spaces = 1;
    while (spaces--) Serial.print(" ");

    fs::File f = dir.openFile("r");
    String fileSize = (String) f.size();
    spaces = 10 - fileSize.length(); // Tabulate nicely
    if (spaces < 1) spaces = 1;
    while (spaces--) Serial.print(" ");
    Serial.println(fileSize + " bytes");
  }

  Serial.println(FPSTR(line));
  Serial.println();
  delay(1000);
}

//====================================================================================

#elif defined ESP32
// TODO: add sub directories
void TFT_eFEX::listSPIFFS(void) {
  Serial.println(F("\r\nListing SPIFFS files:"));
  static const char line[] PROGMEM =  "=================================================";

  Serial.println(FPSTR(line));
  Serial.println(F("  File name                              Size"));
  Serial.println(FPSTR(line));

  fs::File root = SPIFFS.open("/");
  if (!root) {
    Serial.println(F("Failed to open directory"));
    return;
  }
  if (!root.isDirectory()) {
    Serial.println(F("Not a directory"));
    return;
  }

  fs::File file = root.openNextFile();
  while (file) {

    if (file.isDirectory()) {
      Serial.print("DIR : ");
      String fileName = file.name();
      Serial.print(fileName);
    } else {
      String fileName = file.name();
      Serial.print("  " + fileName);
      // File path can be 31 characters maximum in SPIFFS
      int spaces = 33 - fileName.length(); // Tabulate nicely
      if (spaces < 1) spaces = 1;
      while (spaces--) Serial.print(" ");
      String fileSize = (String) file.size();
      spaces = 10 - fileSize.length(); // Tabulate nicely
      if (spaces < 1) spaces = 1;
      while (spaces--) Serial.print(" ");
      Serial.println(fileSize + " bytes");
    }

    file = root.openNextFile();
  }

  Serial.println(FPSTR(line));
  Serial.println();
  delay(1000);
}

#else

void TFT_eFEX::listSPIFFS(void) {}

#endif


/***************************************************************************************
** Function name:           setCursorRTL
** Description:             Set the RTL cursor position
***************************************************************************************/
// Set the RTL cursor position
void TFT_eFEX::setCursorRTL(int32_t cx, int32_t cy)
{
  rtl_cursorX = cx;
  rtl_cursorY = cy;
}


/***************************************************************************************
** Function name:           drawStringRTL
** Description:             Draw the string RTL and update cursor coordinates
***************************************************************************************/
void TFT_eFEX::drawStringRTL(const String& string)
{
  int16_t len = string.length() + 2;
  char sbuffer[len];
  string.toCharArray(sbuffer, len);
  drawStringRTL(sbuffer, &rtl_cursorX, &rtl_cursorY);
}


/***************************************************************************************
** Function name:           drawStringRTL
** Description:             Draw the string RTL at defined coordinates
***************************************************************************************/
// Must call with variables, &x and &y
void TFT_eFEX::drawStringRTL(const char *string, int32_t *x, int32_t *y)
{
  int32_t poX = *x;
  int32_t poY = *y;

  int16_t len = strlen(string);

  if (_tft->fontLoaded)
  {
    uint8_t datum = _tft->getTextDatum();
    int16_t cx = _tft->getCursorX();
    int16_t cy = _tft->getCursorY();
    _tft->setTextDatum(TL_DATUM);
    uint16_t n = 0;
    while (n < len)
    {
      uint16_t uniCode = _tft->decodeUTF8((uint8_t*)string, &n, len - n);
      uint16_t gNum = 0;
      bool found = _tft->getUnicodeIndex(uniCode, &gNum);

      if (uniCode == 0x20)
      {
        poX -= _tft->gFont.spaceWidth;
      }
      else if (uniCode == '\n')
      {
        poX = _tft->width() - 1;
        poY += _tft->gFont.yAdvance;
      }

      if (found)
      {
        poX -= ( _tft->gWidth[gNum] + _tft->gdX[gNum] );

        if (poX < 0)
        {
          poX = _tft->width() - ( _tft->gWidth[gNum] + _tft->gdX[gNum] );
          poY += _tft->gFont.yAdvance;
          if (poY >= _tft->height()) poY = 0;
        }

        _tft->setCursor(poX, poY);
        _tft->drawGlyph(uniCode);
      }
    }

    _tft->setTextDatum(datum);
    _tft->setCursor(cx, cx);

    *x = poX;
    *y = poY;
  }
}


/***************************************************************************************
** Function name:           setCursorLTR
** Description:             Draw the string LTR and update cursor coordinates
***************************************************************************************/
void TFT_eFEX::drawStringLTR(const String& string)
{
  int16_t len = string.length() + 2;
  char sbuffer[len];
  string.toCharArray(sbuffer, len);
  drawStringLTR(sbuffer, &rtl_cursorX, &rtl_cursorY);
}


/***************************************************************************************
** Function name:           drawStringLTR
** Description:             Draw the string LTR at defined coordinates
***************************************************************************************/
void TFT_eFEX::drawStringLTR(const char *string, int32_t *x, int32_t *y)
{
  int32_t poX = *x;
  int32_t poY = *y;

  int16_t len = strlen(string);

  uint16_t pointCodes[len];
  uint16_t index = 0;

  if (_tft->fontLoaded)
  {
    uint8_t datum = _tft->getTextDatum();
    int16_t cx = _tft->getCursorX();
    int16_t cy = _tft->getCursorY();
    _tft->setTextDatum(TL_DATUM);
    uint16_t n = 0;

    while (n < len)
    {
      uint16_t uniCode = _tft->decodeUTF8((uint8_t*)string, &n, len - n);
      pointCodes[index++] = uniCode;
    }

    while (index--)
    {
      uint16_t gNum = 0;
      bool found = _tft->getUnicodeIndex(pointCodes[index], &gNum);

      if (pointCodes[index] == 0x20)
      {
        poX -= _tft->gFont.spaceWidth;
      }
      else if (pointCodes[index] == '\n')
      {
        poX = _tft->width() - 1;
        poY += _tft->gFont.yAdvance;
      }

      if (found)
      {
        poX -= ( _tft->gWidth[gNum] + _tft->gdX[gNum] );

        if (poX < 0)
        {
          poX = _tft->width() - ( _tft->gWidth[gNum] + _tft->gdX[gNum] );
          poY += _tft->gFont.yAdvance;
          if (poY >= _tft->height()) poY = 0;
        }

        _tft->setCursor(poX, poY);
        _tft->drawGlyph(pointCodes[index]);
      }
    }

    _tft->setTextDatum(datum);
    _tft->setCursor(cx, cx);

    *x = poX;
    *y = poY;
  }
}


//====================================================================================
//                           Screen server call with no filename
//====================================================================================
// Start a screen dump server (serial or network) - no filename specified
bool TFT_eFEX::screenServer(void)
{
  // With no filename the screenshot will be saved with a default name e.g. tft_screen_#.xxx
  // where # is a number 0-9 and xxx is a file type specified below
  return screenServer(DEFAULT_FILENAME);
}

//====================================================================================
//                           Screen server call with filename
//====================================================================================
// Start a screen dump server (serial or network) - filename specified
bool TFT_eFEX::screenServer(String filename)
{
  delay(0); // Equivalent to yield() for ESP8266;

  boolean result = serialScreenServer(filename); // Screenshot serial port server
  //boolean result = wifiScreenServer(filename);   // Screenshot WiFi UDP port server (WIP)

  delay(0); // Equivalent to yield()

  //Serial.println();
  //if (result) Serial.println(F("Screen dump passed :-)"));
  //else        Serial.println(F("Screen dump failed :-("));

  return result;
}

//====================================================================================
//                Serial server function that sends the data to the client
//====================================================================================
bool TFT_eFEX::serialScreenServer(String filename)
{
  // Precautionary receive buffer garbage flush for 50ms
  uint32_t clearTime = millis() + 50;
  while ( millis() < clearTime && Serial.read() >= 0) delay(0); // Equivalent to yield() for ESP8266;

  boolean wait = true;
  uint32_t lastCmdTime = millis();     // Initialise start of command time-out

  // Wait for the starting flag with a start time-out
  while (wait)
  {
    delay(0); // Equivalent to yield() for ESP8266;
    // Check serial buffer
    if (Serial.available() > 0) {
      // Read the command byte
      uint8_t cmd = Serial.read();
      // If it is 'S' (start command) then clear the serial buffer for 100ms and stop waiting
      if ( cmd == 'S' ) {
        // Precautionary receive buffer garbage flush for 50ms
        clearTime = millis() + 50;
        while ( millis() < clearTime && Serial.read() >= 0) delay(0); // Equivalent to yield() for ESP8266;

        wait = false;           // No need to wait anymore
        lastCmdTime = millis(); // Set last received command time

        // Send screen size etc using a simple header with delimiters for client checks
        sendParameters(filename);
      }
    }
    else
    {
      // Check for time-out
      if ( millis() > lastCmdTime + START_TIMEOUT) return false;
    }
  }

  uint8_t color[3 * NPIXELS]; // RGB and 565 format color buffer for N pixels

  // Send all the pixels on the whole screen
  for ( uint32_t y = 0; y < _tft->height(); y++)
  {
    // Increment x by NPIXELS as we send NPIXELS for every byte received
    for ( uint32_t x = 0; x < _tft->width(); x += NPIXELS)
    {
      delay(0); // Equivalent to yield() for ESP8266;

      // Wait here for serial data to arrive or a time-out elapses
      while ( Serial.available() == 0 )
      {
        if ( millis() > lastCmdTime + PIXEL_TIMEOUT) return false;
        delay(0); // Equivalent to yield() for ESP8266;
      }

      // Serial data must be available to get here, read 1 byte and
      // respond with N pixels, i.e. N x 3 RGB bytes or N x 2 565 format bytes
      if ( Serial.read() == 'X' ) {
        // X command byte means abort, so clear the buffer and return
        clearTime = millis() + 50;
        while ( millis() < clearTime && Serial.read() >= 0) delay(0); // Equivalent to yield() for ESP8266;
        return false;
      }
      // Save arrival time of the read command (for later time-out check)
      lastCmdTime = millis();

#if defined BITS_PER_PIXEL && BITS_PER_PIXEL >= 24 && NPIXELS > 1
      // Fetch N RGB pixels from x,y and put in buffer
      _tft->readRectRGB(x, y, NPIXELS, 1, color);
      // Send buffer to client
      Serial.write(color, 3 * NPIXELS); // Write all pixels in the buffer
#else
      // Fetch N 565 format pixels from x,y and put in buffer
      if (NPIXELS > 1) _tft->readRect(x, y, NPIXELS, 1, (uint16_t *)color);
      else
      {
        uint16_t c = _tft->readPixel(x, y);
        color[0] = c>>8;
        color[1] = c & 0xFF;  // Swap bytes
      }
      // Send buffer to client
      Serial.write(color, 2 * NPIXELS); // Write all pixels in the buffer
#endif
    }
  }

  Serial.flush(); // Make sure all pixel bytes have been despatched

  return true;
}

//====================================================================================
//    Send screen size etc using a simple header with delimiters for client checks
//====================================================================================
void TFT_eFEX::sendParameters(String filename)
{
  Serial.write('W'); // Width
  Serial.write(_tft->width()  >> 8);
  Serial.write(_tft->width()  & 0xFF);

  Serial.write('H'); // Height
  Serial.write(_tft->height() >> 8);
  Serial.write(_tft->height() & 0xFF);

  Serial.write('Y'); // Bits per pixel (16 or 24)
  if (NPIXELS > 1) Serial.write(BITS_PER_PIXEL);
  else Serial.write(16); // readPixel() only provides 16 bit values

  Serial.write('?'); // Filename next
  Serial.print(filename);

  Serial.write('.'); // End of filename marker

  Serial.write(FILE_EXT); // Filename extension identifier

  Serial.write(*FILE_TYPE); // First character defines file type j,b,p,t
}


/**************************************************************************/
//
//    Bodmer added native JPEG decoder functions from ESP32 rom utility
//    Source: https://github.com/espressif/WROVER_KIT_LCD
//
/**************************************************************************/
#ifdef ESP32

// Include these in header file
// #include "rom/tjpgd.h"
// #include "FS.h"

#define jpgColor(c) (((uint16_t)(((uint8_t*)(c))[0] & 0xF8) << 8) | \
                     ((uint16_t)(((uint8_t*)(c))[1] & 0xFC) << 3) | \
                     ((((uint8_t*)(c))[2] & 0xF8) >> 3))

#if ARDUHAL_LOG_LEVEL >= ARDUHAL_LOG_LEVEL_ERROR
const char * jd_errors[] = {
    "Succeeded",
    "Interrupted by output function",
    "Device error or wrong termination of input stream",
    "Insufficient memory pool for the image",
    "Insufficient stream input buffer",
    "Parameter error",
    "Data format error",
    "Right format but not supported",
    "Not supported JPEG standard"
};
#endif

// Struct to pass parameters to jpeg decoder
typedef struct {
        uint16_t x;
        uint16_t y;
        uint16_t maxWidth;
        uint16_t maxHeight;
        uint16_t offX;
        uint16_t offY;
        jpeg_div_t scale;
        const void * src;
        size_t len;
        size_t index;
        TFT_eSPI * tft;
        uint16_t outWidth;
        uint16_t outHeight;
} jpg_file_decoder_t;

/**************************************************************************/
//
//    JPEG decoder support function prototypes
//
/**************************************************************************/
static uint32_t jpgReadFile(JDEC *decoder, uint8_t *buf, uint32_t len);
static uint32_t jpgRead(JDEC *decoder, uint8_t *buf, uint32_t len);
static uint32_t jpgWrite(JDEC *decoder, void *bitmap, JRECT *rect);
static bool     jpgDecode(jpg_file_decoder_t * jpeg, uint32_t(* reader)(JDEC*,uint8_t *, uint32_t));


/**************************************************************************/
/*!
    @brief  Decode an array stored FLASH in memory (ESP32 only)
    @param    Array name
    @param    Size of array, use sizeof(array_name)
    @param    Display x coord to draw at
    @param    Display y coord to draw at
    @param    Optional: Unscaled jpeg maximum width in pixels
    @param    Optional: Unscaled jpeg maximum height in pixels
    @param    Optional: Unscaled jpeg start x coordinate offset in jpeg
    @param    Optional: Unscaled jpeg start y coordinate offset in jpeg
    @param    Optional: Scale factor 0-4 (type jpeg_div_t)
    @return   true if decoded, else false
*/
/**************************************************************************/
// e.g tft.drawJpg(EagleEye, sizeof(EagleEye), 0, 10);
// Where EagleEye is an array of bytes in PROGMEM:
// const uint8_t EagleEye[] PROGMEM = {...};
bool TFT_eFEX::drawJpg(const uint8_t * jpg_data, size_t jpg_len, uint16_t x, uint16_t y, uint16_t maxWidth, uint16_t maxHeight, uint16_t offX, uint16_t offY, jpeg_div_t scale){

    maxWidth = maxWidth>>(uint8_t)scale;
    maxHeight = maxHeight>>(uint8_t)scale;

    if((x + maxWidth) > width() || (y + maxHeight) > height()){
        log_e("Bad dimensions given");
        return false;
    }

    jpg_file_decoder_t jpeg;

    if(!maxWidth){
        maxWidth = width() - x;
    }
    if(!maxHeight){
        maxHeight = height() - y;
    }

    jpeg.src = jpg_data;
    jpeg.len = jpg_len;
    jpeg.index = 0;
    jpeg.x = x;
    jpeg.y = y;
    jpeg.maxWidth = maxWidth;
    jpeg.maxHeight = maxHeight;
    jpeg.offX = offX>>(uint8_t)scale;
    jpeg.offY = offY>>(uint8_t)scale;
    jpeg.scale = scale;
    jpeg.tft = this;

    return jpgDecode(&jpeg, jpgRead);
}

/**************************************************************************/
/*!
    @brief  Decode an array stored as a file (ESP32 only)
    @param    Filing system, e.g. SPIFFS
    @param    File name, precede with / for SPIFFS
    @param    Display x coord to draw at
    @param    Display y coord to draw at
    @param    Optional: Unscaled jpeg maximum width in pixels
    @param    Optional: Unscaled jpeg maximum height in pixels
    @param    Optional: Unscaled jpeg start x coordinate offset in jpeg
    @param    Optional: Unscaled jpeg start y coordinate offset in jpeg
    @param    Optional: Scale factor 0-4 (type jpeg_div_t)
    @return   true if decoded, else false
*/
/**************************************************************************/
// e.g. tft.drawJpgFile(SPIFFS, "/EagleEye.jpg", 0, 10);
bool TFT_eFEX::drawJpgFile(fs::FS &fs, const char * path, uint16_t x, uint16_t y, uint16_t maxWidth, uint16_t maxHeight, uint16_t offX, uint16_t offY, jpeg_div_t scale){

    maxWidth = maxWidth>>(uint8_t)scale;
    maxHeight = maxHeight>>(uint8_t)scale;

    if((x + maxWidth) > width() || (y + maxHeight) > height()){
        log_e("Bad dimensions given");
        return false;
    }

    fs::File file = fs.open(path, "r");
    if(!file){
        log_e("Failed to open file for reading");
        return false;
    }

    jpg_file_decoder_t jpeg;

    if(!maxWidth){
        maxWidth = width() - x;
    }
    if(!maxHeight){
        maxHeight = height() - y;
    }

    jpeg.src = &file;
    jpeg.len = file.size();
    jpeg.index = 0;
    jpeg.x = x;
    jpeg.y = y;
    jpeg.maxWidth = maxWidth;
    jpeg.maxHeight = maxHeight;
    jpeg.offX = offX>>(uint8_t)scale;
    jpeg.offY = offY>>(uint8_t)scale;
    jpeg.scale = scale;
    jpeg.tft = this;

    bool result = jpgDecode(&jpeg, jpgReadFile);

    file.close();
    return result;
}

/**************************************************************************/
//
//    JPEG decoder support functions
//
/**************************************************************************/
static uint32_t jpgReadFile(JDEC *decoder, uint8_t *buf, uint32_t len){
    jpg_file_decoder_t * jpeg = (jpg_file_decoder_t *)decoder->device;
    fs::File * file = (fs::File *)jpeg->src;
    if(buf){
        return file->read(buf, len);
    } else {
        file->seek(len, fs::SeekCur);
    }
    return len;
}

static uint32_t jpgRead(JDEC *decoder, uint8_t *buf, uint32_t len){
    jpg_file_decoder_t * jpeg = (jpg_file_decoder_t *)decoder->device;
    if(buf){
        memcpy(buf, (const uint8_t *)jpeg->src + jpeg->index, len);
    }
    jpeg->index += len;
    return len;
}

static uint32_t jpgWrite(JDEC *decoder, void *bitmap, JRECT *rect){
    jpg_file_decoder_t * jpeg = (jpg_file_decoder_t *)decoder->device;
    uint16_t x = rect->left;
    uint16_t y = rect->top;
    uint16_t w = rect->right + 1 - x;
    uint16_t h = rect->bottom + 1 - y;
    uint16_t oL = 0, oR = 0;
    uint8_t *data = (uint8_t *)bitmap;

    if(rect->right < jpeg->offX){
        return 1;
    }
    if(rect->left >= (jpeg->offX + jpeg->outWidth)){
        return 1;
    }
    if(rect->bottom < jpeg->offY){
        return 1;
    }
    if(rect->top >= (jpeg->offY + jpeg->outHeight)){
        return 1;
    }
    if(rect->top < jpeg->offY){
        uint16_t linesToSkip = jpeg->offY - rect->top;
        data += linesToSkip * w * 3;
        h -= linesToSkip;
        y += linesToSkip;
    }
    if(rect->bottom >= (jpeg->offY + jpeg->outHeight)){
        uint16_t linesToSkip = (rect->bottom + 1) - (jpeg->offY + jpeg->outHeight);
        h -= linesToSkip;
    }
    if(rect->left < jpeg->offX){
        oL = jpeg->offX - rect->left;
    }
    if(rect->right >= (jpeg->offX + jpeg->outWidth)){
        oR = (rect->right + 1) - (jpeg->offX + jpeg->outWidth);
    }

    uint16_t pixBuf[32];
    uint8_t pixIndex = 0;
    uint16_t line;

    jpeg->tft->startWrite();
    jpeg->tft->setAddrWindow(x - jpeg->offX + jpeg->x + oL, y - jpeg->offY + jpeg->y, w - (oL + oR), h);

    while(h--){
        data += 3 * oL;
        line = w - (oL + oR);
        while(line--){
            pixBuf[pixIndex++] = jpgColor(data);
            data += 3;
            if(pixIndex == 32){
                jpeg->tft->pushColors(pixBuf, 32);
                pixIndex = 0;
            }
        }
        data += 3 * oR;
    }
    if(pixIndex){
        jpeg->tft->pushColors(pixBuf, pixIndex);
    }
    jpeg->tft->endWrite();
    return 1;
}

static bool jpgDecode(jpg_file_decoder_t * jpeg, uint32_t(* reader)(JDEC*,uint8_t *, uint32_t)){
    static uint8_t work[3100];
    JDEC decoder;

    JRESULT jres = jd_prepare(&decoder, reader, work, 3100, jpeg);
    if(jres != JDR_OK){
        log_e("jd_prepare failed! %s", jd_errors[jres]);
        return false;
    }

    uint16_t jpgWidth = decoder.width / (1 << (uint8_t)(jpeg->scale));
    uint16_t jpgHeight = decoder.height / (1 << (uint8_t)(jpeg->scale));

    if(jpeg->offX >= jpgWidth || jpeg->offY >= jpgHeight){
        log_e("Offset Outside of JPEG size");
        return false;
    }

    size_t jpgMaxWidth = jpgWidth - jpeg->offX;
    size_t jpgMaxHeight = jpgHeight - jpeg->offY;

    jpeg->outWidth = (jpgMaxWidth > jpeg->maxWidth)?jpeg->maxWidth:jpgMaxWidth;
    jpeg->outHeight = (jpgMaxHeight > jpeg->maxHeight)?jpeg->maxHeight:jpgMaxHeight;

    jres = jd_decomp(&decoder, jpgWrite, (uint8_t)jpeg->scale);
    if(jres != JDR_OK){
        log_e("jd_decomp failed! %s", jd_errors[jres]);
        return false;
    }

    return true;
}

#endif // ESP32 only
