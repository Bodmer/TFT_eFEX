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
        if (x0 == x2 && y0 == y2) return;
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

    // retrieve infomration about the image
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
          uint16_t *cImg;
          int p = 0;
          cImg = pImg + win_w;
          for (int h = 1; h < win_h; h++)
          {
            p += mcu_w;
            for (int w = 0; w < win_w; w++)
            {
              *cImg = *(pImg + w + p);
              cImg++;
            }
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

    // retrieve infomration about the image
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
          uint16_t *cImg;
          int p = 0;
          cImg = pImg + win_w;
          for (int h = 1; h < win_h; h++)
          {
            p += mcu_w;
            for (int w = 0; w < win_w; w++)
            {
              *cImg = *(pImg + w + p);
              cImg++;
            }
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
  uint16_t r = (color & 0xF800) >> 8; r |= r >> 5;
  uint16_t g = (color & 0x07E0) >> 3; g |= g >> 6;
  uint16_t b = (color & 0x001F) << 3; b |= b >> 5;

  b = ((b * (uint16_t)luminance)/255) & 0x00F8;
  g = ((g * (uint16_t)luminance)/255) & 0x00FC;
  r = ((r * (uint16_t)luminance)/255) & 0x00F8;

  return (r << 8) | (g << 3) | b >> 3;
}


/***************************************************************************************
** Function name:           luminance
** Description:             return a 16 bit colour from an RGB with reduced luminance
***************************************************************************************/
uint16_t TFT_eFEX::luminance(uint8_t red, uint8_t green, uint8_t blue, uint8_t luminance)
{
  uint16_t b = (((uint16_t)blue  * (uint16_t)luminance)/255) & 0x00F8;
  uint16_t g = (((uint16_t)green * (uint16_t)luminance)/255) & 0x00FC;
  uint16_t r = (((uint16_t)red   * (uint16_t)luminance)/255) & 0x00F8;

  return (r << 8) | (g << 3) | b >> 3;
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

  static const char line[] PROGMEM =  "=====================================";
  Serial.println(FPSTR(line));
  Serial.println(F("  File name               Size"));
  Serial.println(FPSTR(line));

  while (dir.next()) {
    String fileName = dir.fileName();
    Serial.print(fileName);
    int spaces = 21 - fileName.length(); // Tabulate nicely
    while (spaces--) Serial.print(" ");

    fs::File f = dir.openFile("r");
    String fileSize = (String) f.size();
    spaces = 10 - fileSize.length(); // Tabulate nicely
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
  static const char line[] PROGMEM =  "======================================";

  Serial.println(FPSTR(line));
  Serial.println(F("  File name                Size"));
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
      spaces = 8 - fileSize.length(); // Tabulate nicely
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
** Function name:           
** Description:             
***************************************************************************************/