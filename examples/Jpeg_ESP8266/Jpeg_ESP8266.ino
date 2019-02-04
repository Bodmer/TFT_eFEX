/*====================================================================================

           Test jpeg rendering with ESP8266 and JPEGDecoder library

  ==================================================================================*/

// The Jpeg library required can be found here:
// https://github.com/Bodmer/JPEGDecoder

// The example images used to test this sketch can be found in the sketch
// Data folder, press Ctrl+K to see this folder. Use the IDE "Tools" menu
// "ESP8266 Sketch Data Upload" option to upload the images to SPIFFS.

// An image is also stored as an array in a header file attached to this sketch
#include "jpeg_arrays.h"

//====================================================================================
//                                  Libraries
//====================================================================================
// Call up the SPIFFS FLASH filing system this is part of the ESP Core
#define FS_NO_GLOBALS
#include <FS.h>

#include <SPI.h>

// https://github.com/Bodmer/TFT_eSPI
#include <TFT_eSPI.h>                 // Hardware-specific library
TFT_eSPI tft = TFT_eSPI();            // Invoke custom library

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
  Serial.println("\nESP8266 Jpeg decoder test!");

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

  uint32_t dt = millis();

  // Image from the array
  fex.drawJpeg(EagleEye, sizeof(EagleEye), 0, 0);

  Serial.println(millis()-dt);

  delay(2000);
  tft.fillScreen(random(0xFFFF));

  // From image stored in SPIFFS
  fex.drawJpeg("/Baboon.jpg", 0, 0);

  delay(2000);
  tft.fillScreen(random(0xFFFF));

  // From image stored in SPIFFS
  fex.drawJpeg("/Tiger.jpg", 0, 0);

  Serial.println();
  delay(2000);

  //while(1) yield(); // Stay here
}
//====================================================================================
