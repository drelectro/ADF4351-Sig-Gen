/*!
   @brief ADF4351 Sig Gen touch screen gui class

   This class implements the touch GUI.
*/

#include <TFT_eSPI.h>
#include <TFT_Touch.h>
#include <HardwareSerial.h>
#include <TFT_eWidget.h>

#include <vector>

#include "Free_Fonts.h"

class TFT_gui
{
    public:
        TFT_gui(TFT_eSPI &tft, TFT_Touch &touch, HardwareSerial &Serial);
        void setupGui();
        void doGui();

        void updateStatus(String status, uint32_t color = TFT_WHITE);
        void updateStartFreq(uint32_t freq);
        
    private:
        TFT_eSPI &tft;
        TFT_Touch &touch;
        HardwareSerial &Serial;
        
        std::vector<ButtonWidget*> buttons;

        SliderWidget* pwrSlider;
        TFT_eSprite* sliderKnob;


};