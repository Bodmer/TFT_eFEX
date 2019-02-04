/*====================================================================================

    Test jpeg rendering with ESP32 and TFT_eSPI library using native jpeg decoder

  ==================================================================================*/

// The example images used to test this sketch can be found in the sketch
// Data folder, press Ctrl+K to see this folder. Use the IDE "Tools" menu
// "ESP32 Sketch Data Upload" option to upload the images to SPIFFS.

#include "jpeg_arrays.h"

//====================================================================================
//                                  Libraries
//====================================================================================
// Call up the SPIFFS FLASH filing system this is part of the ESP Core
#define FS_NO_GLOBALS
#include <FS.h>

#ifdef ESP32
#include "SPIFFS.h" // Needed for ESP32 only
#endif

#include <SPI.h>

// https://github.com/Bodmer/TFT_eSPI
#include <TFT_eSPI.h>                 // Hardware-specific library
TFT_eSPI tft = TFT_eSPI();            // Invoke custom library
//TFT_eSprite spr = TFT_eSprite(&tft);  // Declare Sprite object "spr" with pointer to "tft" object

// https://github.com/Bodmer/TFT_eFEX
#include <TFT_eFEX.h>              // Include the extension graphics functions library
TFT_eFEX  fex = TFT_eFEX(&tft);    // Create TFT_eFX object "efx" with pointer to "tft" object


//====================================================================================
//                                    Setup
//====================================================================================
void setup()
{
  Serial.begin(250000); // Used for messages and the C array generator

  delay(10);
  Serial.println("\nESP32 Jpeg decoder test!");

  tft.begin();

  tft.setRotation(0);  // 0 & 2 Portrait. 1 & 3 landscape

  tft.fillScreen(TFT_BLACK);

  if (!SPIFFS.begin()) {
    Serial.println("SPIFFS initialisation failed!");
    while (1) yield(); // Stay here twiddling thumbs waiting
  }
  Serial.println("\r\nInitialisation done.");

  fex.listSPIFFS(); // Lists the files so you can see what is in the SPIFFS

}

//====================================================================================
//                                    Loop
//====================================================================================
void loop()
{
  tft.fillScreen(random(0xFFFF));

  // Native ESP32 JPEG decoder build into TFT_eFEX library - has an extended set of parameters...
  // drawJpg(const uint8_t * jpg_data, size_t jpg_len, uint16_t x=0, uint16_t y=0, uint16_t maxWidth=0, uint16_t maxHeight=0, uint16_t offX=0, uint16_t offY=0, jpeg_div_t scale=JPEG_DIV_NONE);

uint32_t dt = millis();
 // Unscaled from array at 0,0 on screen
  fex.drawJpg(EagleEye, sizeof(EagleEye), 0, 0);
Serial.println(millis()-dt);

  // Power of 2 scale factors
  /*0  JPEG_DIV_NONE,
    1  JPEG_DIV_2,
    2  JPEG_DIV_4,
    3  JPEG_DIV_8,
    4  JPEG_DIV_MAX // = DIV_16 */

  uint8_t scale = (uint8_t)JPEG_DIV_2;


  // Scaled and cropped from array
  fex.drawJpg(EagleEye, sizeof(EagleEye), 0, 240, 120, 120, 60, 60, (jpeg_div_t)scale);


  delay(2000);
  tft.fillScreen(random(0xFFFF));

  // From image stored in SPIFFS
  fex.drawJpgFile(SPIFFS, "/Baboon.jpg", 0, 0);
  //tft.drawJpgFile(SPIFFS, "/Baboon.jpg", 0, 0), 120, 120, 60, 60, (jpeg_div_t)scale);

  delay(2000);
  tft.fillScreen(random(0xFFFF));

  // From image stored in SPIFFS
  fex.drawJpgFile(SPIFFS, "/Tiger.jpg", 0, 0);

  Serial.println();
  delay(2000);

  //while(1) yield(); // Stay here
}
//====================================================================================
