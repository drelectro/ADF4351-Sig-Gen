#include <Arduino.h>
#include <TFT_eSPI.h>
#include "adf4351.h"


// Define IO pins
#define LED_PIN PA4

HardwareSerial Serial1(PA10, PA9);

TFT_eSPI tft = TFT_eSPI();

ADF4351  vfo(PB12, SPI_MODE0, 1000000UL , MSBFIRST) ;

// put function declarations here:
//int myFunction(int, int);

// put global variables here:
uint32_t set_f=50000000;

// put your setup code here, to run once:
void setup() {
  
  // Initialize pins
  pinMode(LED_PIN, OUTPUT);

  // Intialize Serial
  Serial1.begin(115200);
  Serial1.println("VK2XMC ADF4351 Synthesizer V0.1");

  // Initialize TFT
  
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextFont(2);
  tft.setTextSize(1);
  tft.setCursor(0, 0);
  tft.println("VK2XMC ADF4351 Synthesizer V0.1");
  tft.println("Initializing...");

  /*
  tft.setTextFont(2);
  tft.println("Font 2");
  tft.setTextFont(4);
  tft.println("Font 4");
  tft.setTextFont(1);
  tft.println("Font 1");
  tft.setTextFont(7);
  tft.println("123.456");
  */

  // Initialize ADF4351
  //Setup ADF4351 defaults
  vfo.pwrlevel = 0 ; ///< sets to -4 dBm output
  vfo.RD2refdouble = 0 ; ///< ref doubler off
  vfo.RD1Rdiv2 = 0 ;   ///< ref divider off
  vfo.ClkDiv = 150 ;
  vfo.BandSelClock = 80 ;
  vfo.RCounter = 1 ;  ///< R counter to 1 (no division)
  //vfo.ChanStep = steps[2] ;  ///< set to 10 kHz steps
  //vfo.ChanStep = steps[0] ;  ///< set to 1 Hz steps
  vfo.ChanStep = steps[11] ;  ///< set to 1 Hz steps

  // sets the reference frequency to 10 Mhz
  if ( vfo.setrf(25000000UL) ==  0 )
  {
    tft.println("ref freq set to 25 Mhz") ;
  } else {
    tft.println("ref freq set error") ;
  }

  //initialize the chip
  vfo.init() ;
  //enable frequency output
  vfo.enable() ;

  vfo.setf_only(set_f, 11, 1);
}

// put your main code here, to run repeatedly:
void loop() {
  

  digitalWrite(LED_PIN, HIGH);
  delay(500);
  digitalWrite(LED_PIN, LOW);
  delay(500);

  Serial1.println("Test Serial1");
  //Serial.println("Test Serial");

  vfo.setf_only(set_f, 11, 1);
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}