#include "tft_cal.h"

// Constructor
TFT_cal::TFT_cal(TFT_eSPI &tft, TFT_Touch &touch, HardwareSerial &Serial, uint16_t hres, uint16_t vres)
    : tft(tft), touch(touch), Serial(Serial), hres(hres), vres(vres)
{
}   

// Calibration routine
void TFT_cal::doTouchCalibration()
{
  int x1, y1;
  int x2, y2;
  int x3, y3;
  bool xyswap  = 0, xflip = 0, yflip = 0;

  Serial.println("TFT_Touch Calibration, follow TFT screen prompts..");
    
  // Reset the default calibration values
  touch.setCal(0, 4095, 0, 4095, 320, 240, 0);//, 0, 0);
  
  // Set TFT the screen to landscape orientation
  tft.setRotation(1);
  
  // Set Touch the screen to the same landscape orientation
  touch.setRotation(1);

  // Clear the screen
  tft.fillScreen(TFT_BLACK);

  // Show the screen prompt
  drawPrompt();

  drawCross(30, 30, TFT_RED);
  while (!touch.Pressed());
  delay(100);
  
  getCoord(); // This function assigns values to X_Raw and Y_Raw

  drawCross(30, 30, TFT_BLACK);
  Serial.print("First point : Raw x,y = ");
  Serial.print(X_Raw);
  Serial.print(",");
  Serial.println(Y_Raw);

  x1 = X_Raw;
  y1 = Y_Raw;

  drawCross(hres/2, vres/2, TFT_RED);
  delay(10);
  
  while (getCoord()); // This waits for the centre area to be touched
  
  drawCross(hres/2, vres/2, TFT_BLACK);
  Serial.print("Second point : Raw x,y = ");
  Serial.print(X_Raw);
  Serial.print(",");
  Serial.println(Y_Raw);

  drawCross(hres-30, vres-30, TFT_RED);
  
  while (!getCoord()); // This waits until the centre area is no longer pressed
  delay(10);           // Wait a little for touch bounces to stop after release
  
  getCoord();
  drawCross(hres-30, vres-30, TFT_BLACK);
  Serial.print("Third point : Raw x,y = ");
  Serial.print(X_Raw);
  Serial.print(",");
  Serial.println(Y_Raw);

  x2 = X_Raw;
  y2 = Y_Raw;

  drawCross(hres/2, vres/2, TFT_RED);
  delay(10);
  
  while (getCoord()); // This waits for the centre area to be touched
  
  drawCross(hres/2, vres/2, TFT_BLACK);
  Serial.print("Fourth point : Raw x,y = ");
  Serial.print(X_Raw);
  Serial.print(",");
  Serial.println(Y_Raw);

  drawCross(30, vres-30, TFT_RED);
  
  while (!getCoord()); // This waits until the centre area is no longer pressed
  delay(10);           // Wait a little for touch bounces to stop after release
  
  getCoord();
  drawCross(30, vres-30, TFT_BLACK);
  Serial.print("Fifth point : Raw x,y = ");
  Serial.print(X_Raw);
  Serial.print(",");
  Serial.println(Y_Raw);

  x3 = X_Raw;
  y3 = Y_Raw;

  int temp;
  if (abs(x1 - x3) > 1000) {
    xyswap = 1;
    temp = x1; x1 = y1; y1 = temp;
    temp = x2; x2 = y2; y2 = temp;
    temp = x3; x3 = y3; y3 = temp;
  }
  else xyswap = 0;
  
  int hmin = x1;// - (x2 - x1) * 3 / (hres/10 - 6);
  int hmax = x2;// + (x2 - x1) * 3 / (hres/10 - 6);

  int vmin = y1;// - (y2 - y1) * 3 / (vres/10 - 6);
  int vmax = y2;// + (y2 - y1) * 3 / (vres/10 - 6);

  Serial.println();
  Serial.println("  //This is the calibration line to use in your sketch");
  Serial.println("  //you can copy and paste into your sketch setup()");
  Serial.print("  touch.setCal(");
  Serial.print(hmin); Serial.print(", ");
  Serial.print(hmax); Serial.print(", ");
  Serial.print(vmin); Serial.print(", ");
  Serial.print(vmax); Serial.print(", ");
  Serial.print(hres); Serial.print(", ");
  Serial.print(vres); Serial.print(", ");
  Serial.print(xyswap); //Serial.print(", ");
  //Serial.print(xflip); Serial.print(", ");
  //Serial.print(yflip);
  Serial.println(");");

  Serial.println();
  Serial.println("Test the touch screen, green crosses appear at the touch coordinates!");
  Serial.println("Send any character from the serial monitor window to exit calibration");

  // These are the calibration settings the sketch has calculated to try out!
  touch.setCal(hmin, hmax, vmin, vmax, hres, vres, xyswap);//, xflip, yflip); // Raw xmin, xmax, ymin, ymax, 320, 240, XYswap, xflip, yflip
  
  // Keep TFT and Touch rotations the same, try values 0 to 3
  // Start with the current orientation
  // Receiving any serial character moves on to next orientation test
  tft.setRotation(1);
  touch.setRotation(1);
  doTouchTest();

/*
  tft.setRotation(2);
  touch.setRotation(2);
  doTouchTest();

  tft.setRotation(3);
  touch.setRotation(3);
  doTouchTest();

  tft.setRotation(0);
  touch.setRotation(0);
  doTouchTest();
*/
  Serial.println();  
}

void TFT_cal::doTouchTest(void)
{
  tft.fillScreen(TFT_BLACK);

  drawCross(30, 30, TFT_WHITE);

  drawCross(tft.width() - 30, tft.height() - 30, TFT_WHITE);

  int centre = tft.width()/2; // Get and work out x coord of screen centre

  String text;
  text+= "Screen rotation = ";
  text+= tft.getRotation();
  char buffer[30];
  text.toCharArray(buffer,30);
  
  tft.drawString(buffer, centre, 50, 2);

  tft.drawString("Touch anywhere on screen", centre, 70, 2);
  tft.drawString("to test settings", centre, 90, 2);

  tft.drawString("Send a character from the", centre, 120, 2);
  tft.drawString("IDE Serial Monitor to", centre, 140, 2);
  tft.drawString("continue!", centre, 160, 2);

  while (Serial.available()) Serial.read(); // Empty the serial buffer before we start
  
  while (!Serial.available()) {
    if (touch.Pressed()) // Note this function updates coordinates stored within library variables
    {
      /* Read the current X and Y axis as co-ordinates at the last touch time*/
      // The values returned were captured when Pressed() was called!
      int X_Coord = touch.X();
      int Y_Coord = touch.Y();

      drawCross(X_Coord, Y_Coord, TFT_GREEN);

      //delay(20);
      tft.setCursor(centre, 0,  2);
      tft.print("X = ");tft.print(X_Coord);tft.print("   ");
      tft.setCursor(centre, 20,  2);
      tft.print("Y = ");tft.print(Y_Coord);tft.print("   ");

    }
  }
}

void TFT_cal::drawPrompt(void)
{
  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  int centre = tft.width()/2; // Get and work out x coord of screen centre

  tft.drawString("CALIBRATION", centre, 20, 2);

  tft.drawString("Touch the red cross accurately", centre, 61, 2);
  tft.drawString("( using a cocktail stick works well! )", centre, 81, 2);
}

void TFT_cal::drawCross(int x, int y, unsigned int color)
{
  tft.drawLine(x - 5, y, x + 5, y, color);
  tft.drawLine(x, y - 5, x, y + 5, color);
}

bool TFT_cal::getCoord()
{
  bool Xwait = 1, Ywait = 1;
  int X_Temp1 = 9999, Y_Temp1 = 9999;
  int X_Temp2 = -1, Y_Temp2 = -1;
  X_Raw = -1;
  Y_Raw = -1;

  while (Xwait || Ywait) {
    if (touch.Pressed()) // Note this function updates coordinates stored within library variables
    {
      /* Read the current X and Y axis as co-ordinates at the last touch time*/
      // The values returned were captured when Pressed() was called!
      X_Temp1 = touch.RawX();
      Y_Temp1 = touch.RawY();
    }
    delay(5);
    if (touch.Pressed()) // Note this function updates coordinates stored within library variables
    {
      /* Read the current X and Y axis as co-ordinates at the last touch time*/
      // The values returned were captured when Pressed() was called!
      X_Temp2 = touch.RawX();
      Y_Temp2 = touch.RawY();
    }

    #define RAW_ERROR 10
    
    if ( (abs(X_Temp1 - X_Temp2) < RAW_ERROR) && Xwait ) {
      X_Raw = (X_Temp1 + X_Temp2) / 2;
      Xwait = 0;
    }
    if ( (abs(Y_Temp1 - Y_Temp2) < RAW_ERROR) && Ywait ) {
      Y_Raw = (Y_Temp1 + Y_Temp2) / 2;
      Ywait = 0;
    }
  }

  // Check if press is near middle third of screen
  if ((X_Raw > 1365) && (X_Raw < 2731) && (Y_Raw > 1365) && (Y_Raw < 2371)) return 0;
  
  // otherwise it is near edge for calibration points
  else return 1;
}