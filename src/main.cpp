/*
 * VK2XMC ADF4351 Synthesizer V0.1
 *
 * This an open source alternative firmware for one the various ADF4351 synthesizers available on AliExpress and eBay.
 *
 * The ADF4351 driver is based on https://github.com/dfannin/adf4351/tree/master by David Fannin.
 
 * Some useful links:
 * https://doc-tft-espi.readthedocs.io/  TFT_eSPI documentation
 * https://github.com/stm32duino/Arduino_Core_STM32/tree/main STM32 core for Arduino
 *
 */

#include <Arduino.h>

#include "adf4351.h"

#include "Free_Fonts.h"
#include <TFT_eSPI.h>
#include <TFT_eWidget.h>

// Define IO pins
#define LED_PIN PA4

HardwareSerial Serial1(PA10, PA9);

TFT_eSPI tft = TFT_eSPI();

ADF4351  vfo(PB12, SPI_MODE0, 1000000UL , MSBFIRST) ;

// These are the pins used to interface between the 2046 touch controller and STM32
#define TOUCH_DOUT PB4  /* Data out pin (T_DO) of touch screen */
#define TOUCH_DIN  PB5  /* Data in pin (T_DIN) of touch screen */
#define TOUCH_CS   PB6  /* Chip select pin (T_CS) of touch screen */
#define TOUCH_CLK  PB3  /* Clock pin (T_CLK) of touch screen */

// These are the default min and maximum values, set to 0 and 4095 to test the screen
#define HMIN 0
#define HMAX 4095
#define VMIN 0
#define VMAX 4095
#define XYSWAP 0 // 0 or 1

// This is the screen size for the raw to coordinate transformation
// width and height specified for landscape orientation
#define HRES 320 /* Default screen resulution for X axis */
#define VRES 240 /* Default screen resulution for Y axis */

#define BUTTON_W 100
#define BUTTON_H 50

// GUI objects
ButtonWidget btnL = ButtonWidget(&tft);
ButtonWidget btnR = ButtonWidget(&tft);

#include <TFT_Touch.h>

/* Create an instance of the touch screen library */
TFT_Touch touch = TFT_Touch(TOUCH_CS, TOUCH_CLK, TOUCH_DIN, TOUCH_DOUT);

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
  tft.fillScreen(TFT_SKYBLUE);
  tft.setTextColor(TFT_WHITE, TFT_SKYBLUE);
  tft.setFreeFont(FM9);

  tft.setCursor(0, 10); // x,y
  tft.println("VK2XMC ADF4351 Sig gen V0.1");
  tft.println("Initializing...");

  //tft.setTextFont(1);
  //tft.println("Font 1");
  //tft.setTextFont(2);
  //tft.println("Font 2");
  //tft.setTextFont(4);
  //tft.println("Font 4");
  //tft.setTextFont(6);
  //tft.println("Font 6");

  //tft.setTextFont(7);
  //tft.setTextSize(1);
  //tft.println("123.456");

  //tft.setTextFont(8);
  //tft.println("Font 8");

  //tft.setFreeFont(FF2);
  //tft.println("Font FF2");

  //tft.setFreeFont(FF17);
  //tft.println("Font FF17");

/*
  tft.setFreeFont(TT1);
  tft.println("Font TT1");

  tft.setFreeFont(FMB9);
  tft.println("Font FMB9");

  tft.setFreeFont(FSS9);
  tft.println("Font FSS9");
*/

  tft.setFreeFont(FM9);

  // Set up the GUI
  uint16_t x = (tft.width() - BUTTON_W) / 2;
  uint16_t y = tft.height() / 2 - BUTTON_H - 10;
  btnL.initButtonUL(x, y, BUTTON_W, BUTTON_H, TFT_WHITE, TFT_RED, TFT_BLACK, "Button", 1);
  //btnL.setPressAction(btnL_pressAction);
  //btnL.setReleaseAction(btnL_releaseAction);
  btnL.drawSmoothButton(false, 3, TFT_BLACK); // 3 is outline width, TFT_BLACK is the surrounding background colour for anti-aliasing

  // Initialize Touch
  touch.setCal(HMIN, HMAX, VMIN, VMAX, HRES, VRES, XYSWAP); // Raw xmin, xmax, ymin, ymax, width, height
  touch.setRotation(1);

  // Initialize ADF4351
  vfo.pwrlevel = 0 ;      ///< sets to -4 dBm output
  vfo.RD2refdouble = 0 ;  ///< ref doubler off
  vfo.RD1Rdiv2 = 0 ;      ///< ref divider off
  vfo.ClkDiv = 150 ;
  vfo.BandSelClock = 80 ;
  vfo.RCounter = 1 ;      ///< R counter to 1 (no division)
  //vfo.ChanStep = steps[2] ;  ///< set to 10 kHz steps
  //vfo.ChanStep = steps[0] ;  ///< set to 1 Hz steps
  vfo.ChanStep = steps[11] ;  ///< set to 10 kHz steps

  // sets the reference frequency to 25 Mhz
  if ( vfo.setrf(25000000UL) ==  0 )
  {
    tft.println("ref freq set to 25 MHz") ;
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
  
  unsigned int X_Raw;
  unsigned int Y_Raw;

  digitalWrite(LED_PIN, HIGH);
  delay(500);
  digitalWrite(LED_PIN, LOW);
  delay(500);

  Serial1.println("Test Serial1");

  vfo.setf_only(set_f, 11, 1);

  if (touch.Pressed()) // Note this function updates coordinates stored within library variables
  {
    /* Read the current X and Y axis as raw co-ordinates at the last touch time*/
    // The values returned were captured when Pressed() was called!
    
    X_Raw = touch.RawX();
    Y_Raw = touch.RawY();

    /* Output the results to the serial port */
    Serial1.print("Raw x,y = ");
    Serial1.print(X_Raw);
    Serial1.print(",");
    Serial1.println(Y_Raw);
    delay(10);
  }
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}