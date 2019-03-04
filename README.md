# TFT_eFEX

An Arduino IDE library that provides extra functions for the TFT_eSPI library.

The extra functions are:

           // Draw a bezier curve of a defined colour between specified points
  void     drawBezier(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint16_t color); 
 
  void     drawBezierSegment(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint16_t color);

           // Draw a bitmap (bmp file) stored in SPIFFS to the TFT or a Sprite if a Sprite instance is included
  void     drawBmp(String filename, int16_t x, int16_t y, TFT_eSprite *_spr = nullptr);

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

           // Return a rainbow colour for a spectrum value 0-191
  uint16_t rainbowColor(uint8_t spectrum);

           // List files in the SPIFFS for ESP8266 or ESP32
  void     listSPIFFS(void);

           // Screen server call with or without a filename for the resultant PC stored image
  bool     screenServer(void);

  bool     screenServer(String filename);

           // Support Right To Left (RTL) character rendering (see example)
  void     setCursorRTL(int32_t cx, int32_t cy);

  void     drawStringRTL(const String& string);

  void     drawStringRTL(const char *string, int32_t *x, int32_t *y);

  void     drawStringLTR(const String& string);

  void     drawStringLTR(const char *string, int32_t *x, int32_t *y);



**For ESP32 only (see "Jpeg_ESP32" example):**

           // Draw a jpeg stored in an array using the faster ESP32 native decoder, can crop and scale
  bool     drawJpg(const uint8_t * jpg_data, size_t jpg_len, uint16_t x=0, uint16_t y=0, uint16_t maxWidth=0, uint16_t maxHeight=0, uint16_t offX=0, uint16_t offY=0, jpeg_div_t scale=JPEG_DIV_NONE);

           // Draw a jpeg stored in a file using the faster ESP32 native decoder, can crop and scale
  bool     drawJpgFile(fs::FS &fs, const char * path, uint16_t x=0, uint16_t y=0, uint16_t maxWidth=0, uint16_t maxHeight=0, uint16_t offX=0, uint16_t offY=0, jpeg_div_t scale=JPEG_DIV_NONE);
