// Created by tobozo 29/11/2019 as an example for the TFT_eFEX graphics extension library to TFT_eSPI

// https://github.com/Bodmer/TFT_eSPI
#include <TFT_eSPI.h>            // Include the graphics library (this includes the sprite functions)

// https://github.com/Bodmer/TFT_eFEX
#include <TFT_eFEX.h>             // Include the extension graphics functions library

TFT_eSPI tft = TFT_eSPI();       // Create object "tft"

TFT_eFEX  fex = TFT_eFEX(&tft);    // Create TFT_eFEX object "fex" with pointer to "tft" object


void randomColors( RGBColor &colorstart, RGBColor &colorend ) {
  colorstart = { 128, byte(random(128)), byte(random(128)) }; // create some random color
  colorend = { byte(random(128)+127), 255, byte(random(128)+127) }; // create some random color
  if( random(100) > 50 ) {
    swap_coord( colorstart, colorend );
  }
}


void fillGradiendHRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, RGBColor colorstart, RGBColor colorend ) {
  for( uint16_t h = 0; h < height; h++ ) {
    fex.drawGradientHLine( x, y+h, width, colorstart, colorend);
  }
}

void fillGradiendVRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, RGBColor colorstart, RGBColor colorend ) {
  for( uint16_t w = 0; w < width; w++ ) {
    fex.drawGradientVLine( x+w, y, height, colorstart, colorend);
  }
}



void fillGradiendDisc(uint16_t xpos, uint16_t ypos, byte r, RGBColor colorstart, RGBColor colorend ) {
  for( float angle=-PI;angle<PI;angle+=.01 ) {
    float x = sin( angle ) * r + xpos;
    float y = cos( angle ) * r + ypos;
    fex.drawGradientLine( xpos, ypos, x, y, colorstart, colorend );
  }
}


/* stolen from TFT_eSPI::fillCircle() */
void fillGradientCircle(int32_t x0, int32_t y0, int32_t r, RGBColor colorstart, RGBColor colorend) {

  int32_t  x  = 0;
  int32_t  dx = 1;
  int32_t  dy = r+r;
  int32_t  p  = -(r>>1);

  RGBColor _colorstart = fex.colorAt( x0 - r, y0 - r, x0 + r, y0 + r, x0 - r, y0, colorstart, colorend );
  RGBColor _colorend   = fex.colorAt( x0 - r, y0 - r, x0 + r, y0 + r, x0 + dy + 1, y0, colorstart, colorend );

  fex.drawGradientHLine(x0 - r, y0, dy+1, colorstart, colorend);
  bool swap;

  while(x<r){

    if(p>=0) {
      dy-=2;
      p-=dy;
      r--;
    }

    dx+=2;
    p+=dx;

    x++;

    fex.drawGradientHLine(x0 - r, y0 + x, 2 * r+1, colorstart, colorend);
    fex.drawGradientHLine(x0 - r, y0 - x, 2 * r+1, colorstart, colorend);
    fex.drawGradientHLine(x0 - x, y0 + r, 2 * x+1, colorstart, colorend);
    fex.drawGradientHLine(x0 - x, y0 - r, 2 * x+1, colorstart, colorend);

  }
}


// =========================================================================
void setup(void) {

  Serial.begin(250000);

  tft.init();

  tft.setRotation(1);

}

// =========================================================================
void loop() {

  RGBColor colorstart;
  RGBColor colorend;
  uint8_t margin = 50;
  uint16_t boxw = (tft.width()/2)-margin;
  uint16_t boxh = (tft.height()/2)-margin;
  uint16_t centerx = tft.width()/2;
  uint16_t centery = tft.height()/2;
  uint16_t xpos = 50;
  uint16_t ypos = 50;
  byte r = 25;

  tft.fillScreen(TFT_BLACK);

  randomColors( colorstart, colorend );

  for( uint16_t x=0; x<tft.width();x++ ) {
    fex.drawGradientLine( x, 0, 0, tft.height(), colorstart, colorend );
  }

  for( uint16_t y=0; y<tft.height();y++ ) {
    fex.drawGradientLine( tft.width()-1, y, 0, tft.height()-1, colorstart, colorend );
  }

  randomColors( colorstart, colorend );
  fillGradiendHRect( margin, margin, boxw, boxh, colorstart, colorend);
  fillGradiendVRect( centerx, centery, boxw, boxh, colorstart, colorend);

  randomColors( colorstart, colorend );
  fillGradiendHRect( margin, centery, boxw, boxh, colorend, colorstart);
  fillGradiendVRect( centerx, margin, boxw, boxh, colorend, colorstart);

  randomColors( colorstart, colorend );
  fillGradientCircle( centerx, centery, 50, colorend, colorstart );

  randomColors( colorstart, colorend );
  fillGradiendDisc( margin, margin, r, colorstart, colorend);

  randomColors( colorstart, colorend );
  fillGradiendDisc( tft.width()-margin, margin, r, colorstart, colorend);

  randomColors( colorstart, colorend );
  fillGradiendDisc( margin, tft.height()-margin, r, colorstart, colorend);

  randomColors( colorstart, colorend );
  fillGradiendDisc( tft.width()-margin, tft.height()-margin, r, colorstart, colorend);

  delay(2000);
}

