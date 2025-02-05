/*
 * VK2XMC ADF4351 Synthesizer V0.1
 *
 * This an open source alternative firmware for one the various ADF4351 synthesizers available on AliExpress and eBay.
 *
 * The ADF4351 driver is based on https://github.com/dfannin/adf4351/tree/master by David Fannin.
 
 * Some useful links:
 * https://doc-tft-espi.readthedocs.io/  TFT_eSPI documentation
 * https://github.com/stm32duino/Arduino_Core_STM32/tree/main STM32 core for Arduino
 * https://docs.arduino.cc/language-reference/keywords/ Arduino language reference
 *
 */

#include <Arduino.h>
#include "HardwareTimer.h"  // include the STM32 HardwareTimer library

#include "adf4351.h"


#include <TFT_eSPI.h>
//#include <TFT_eWidget.h>

// Define IO pins
#define LED_PIN PA4

// Create hardware driver objects
HardwareSerial Serial1(PA10, PA9);
TFT_eSPI tft = TFT_eSPI();
ADF4351  vfo(PB12, SPI_MODE0, 1000000UL , MSBFIRST) ;
HardwareTimer timer(TIM2);

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
#define XYSWAP 1 // 0 or 1

// This is the screen size for the raw to coordinate transformation
// width and height specified for landscape orientation
#define tftHRES 320 /* Default screen resulution for X axis */
#define tftVRES 240 /* Default screen resulution for Y axis */



#include <TFT_Touch.h>

// Create an instance of the touch screen library 
TFT_Touch touch = TFT_Touch(TOUCH_CS, TOUCH_CLK, TOUCH_DIN, TOUCH_DOUT);

#include "tft_cal.h"

#include "gui.h"
TFT_gui *gui;


#include <Vrekrer_scpi_parser.h>
SCPI_Parser scpi;

// Forward declarations
void scpi_init();

// put global variables here:
uint32_t set_f=50000000;
volatile uint32_t systemTick = 0;

// Timer callback function
void timerCallback() {
  // This code will execute every 10ms
  systemTick++;

  if (systemTick % 50 == 0) {
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
  }
}

// put your setup code here, to run once:
void setup() {
  
  // Initialize pins
  pinMode(LED_PIN, OUTPUT);

  // Intialize Serial
  Serial1.begin(115200);
  Serial1.println("VK2XMC ADF4351 Synthesizer V0.1");

  // Do touch calibration
  //TFT_cal tcal(tft, touch, Serial1, tftHRES, tftVRES);
  //tcal.doTouchCalibration();


  // Set the calibration values for the touch screen
  // These values are for the 3.5" TFT screen with the XPT2046 touch controller
  // For now, you can regenrate these by uncommenting the 2 lines above
  touch.setCal(3637, 691, 3323, 593, 320, 240, 1); // Raw xmin, xmax, ymin, ymax, width, height, XYswap
  touch.setRotation(1);

  // Create an instance of the GUI
  gui = new TFT_gui(tft, touch, Serial1);
  gui->setupGui();

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
    gui->updateStatus("ref freq set to 25 MHz", TFT_GREEN);
  } else {
    gui->updateStatus("ref freq set error", TFT_RED) ;
  }

  //initialize the ADF4351 
  vfo.init() ;
  //enable frequency output
  vfo.enable() ;

  vfo.setf_only(set_f, 11, 1);

  // Initialize Timer
  timer.setOverflow(100, HERTZ_FORMAT); // 10ms
  timer.attachInterrupt(timerCallback);
  timer.resume();

  // Initialize SCPI
  scpi_init();

}

//bool ledState = false;
// put your main code here, to run repeatedly:
void loop() {
  
  static uint32_t scanTime = millis();
  

  // Every 500ms 
  //if (systemTick % 50 == 0) {
  //  digitalWrite(LED_PIN, !digitalRead(LED_PIN)); // Toggle LED
  //  Serial1.print("System Tick: "); Serial1.println(systemTick);
  //}

  //digitalWrite(LED_PIN, HIGH);
  //delay(500);
  //digitalWrite(LED_PIN, LOW);
  //delay(500);

  //Serial1.println("Test Serial1");

  //vfo.setf_only(set_f, 11, 1);

  //if (millis() % 500 == 0){
  //  digitalWrite(LED_PIN, ledState); // Toggle LED
  //  ledState = !ledState;
  //  Serial1.print("Led "); Serial1.println(scanTime);
  //}

  if (millis() - scanTime >= 50) {
    scanTime = millis();

    gui->doGui();

    //gui->updateStatus(String("Tick: ") + String(scanTime, DEC));
        
  }

  scpi.ProcessInput(Serial1, "\n");
}

void Identify(SCPI_C commands, SCPI_P parameters, Stream& interface) {
  interface.println(F("VK2XMC,ADF4351 Sig Gen,#00," VREKRER_SCPI_VERSION));
  //*IDN? Suggested return string should be in the following format:
  // "<vendor>,<model>,<serial number>,<firmware>"
}

void scpiSetFrequency(SCPI_C commands, SCPI_P parameters, Stream& interface) {
  if (parameters.Size() == 1) {
    set_f = String(parameters[0]).toInt();
    vfo.setf_only(set_f, 11, 1);
    interface.println(F("OK"));
  } else {
    interface.println(F("ERROR"));
  }
  gui->updateStatus(String("Set Frequency: ") + String(set_f, DEC));
}

void scpiGetFrequency(SCPI_C commands, SCPI_P parameters, Stream& interface) {
  if (parameters.Size() == 0) {
    interface.println(set_f);
  } else {
    interface.println(F("ERROR"));
  }
}

void scpiSetPower(SCPI_C commands, SCPI_P parameters, Stream& interface) {
  if (parameters.Size() == 1) {
    vfo.pwrlevel = String(parameters[0]).toInt();
    vfo.R[4].setbf(3, 2, vfo.pwrlevel);            // Output power 0-3 (-4dBm to 5dBm, 3dB steps)
    vfo.writeRegisters();
    interface.println(F("OK"));
  } else {
    interface.println(F("ERROR"));
  }
}

void scpiGetPower(SCPI_C commands, SCPI_P parameters, Stream& interface) {
  if (parameters.Size() == 0) {
    interface.println(vfo.pwrlevel);
  } else {
    interface.println(F("ERROR"));
  }
}

void DoNothing(SCPI_C commands, SCPI_P parameters, Stream& interface) {
  if (parameters.Size() == 0) {
    //interface.println(F("OK"));
  } else {
    //interface.println(F("ERROR"));
  }
}

void scpi_init()
{
    scpi.RegisterCommand(F("*IDN?"), &Identify);

    scpi.SetCommandTreeBase(F("SOURce"));
    scpi.RegisterCommand(F(":FREQuency"), &scpiSetFrequency);
    scpi.RegisterCommand(F(":FREQuency?"), &scpiGetFrequency);
    scpi.RegisterCommand(F(":CW"), &scpiSetFrequency);
    scpi.RegisterCommand(F(":CW?"), &scpiSetFrequency);

    scpi.RegisterCommand(F(":POWer"), &scpiSetPower);
    scpi.RegisterCommand(F(":POWer?"), &scpiGetPower);

}
