/*!
   @brief TFT Calibration class

   This class is used to calibrate the touch screen for the TFT display.
*/

#include <TFT_eSPI.h>
#include <TFT_Touch.h>
#include <HardwareSerial.h>

class TFT_cal
{
    public:
        TFT_cal(TFT_eSPI &tft, TFT_Touch &touch, HardwareSerial &Serial, uint16_t HRES, uint16_t VRES);
        void doTouchCalibration();
        void doTouchTest();
        
    private:
        TFT_eSPI &tft;
        TFT_Touch &touch;
        HardwareSerial &Serial;
        
        void drawPrompt();
        void drawCross(int x, int y, unsigned int color);
        bool getCoord();

        uint16_t hres;
        uint16_t vres;
        int X_Raw = 0;
        int Y_Raw = 0;

};
