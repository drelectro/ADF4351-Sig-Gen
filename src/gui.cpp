#include "gui.h"

#define BUTTON_W 100
#define BUTTON_H 50

#define R_BORDER 10

extern void enableOutput(bool state);
extern void setPowerLevel(uint8_t level);

// The eWidget library doesn't support CPP callbacks so we need to use a global function and globals to store the button instances
ButtonWidget* btnSetStartFreq;
//ButtonWidget* btnSetStopFreq;
//ButtonWidget* btnSetMode;
//ButtonWidget* btnSetPower;
ButtonWidget* btnEnableOutput;

void btnEnablePress()
{
  //Serial1.println("Enable button pressed");

  
  if (btnEnableOutput->justPressed()) {
    btnEnableOutput->drawSmoothButton(!btnEnableOutput->getState(), 3, TFT_BLACK, btnEnableOutput->getState() ? "RF OFF" : "RF ON");
    
    //Serial1.print("Button toggled: ");
    //if (btnEnableOutput->getState()) Serial1.println("ON");
    //else  Serial1.println("OFF");

    enableOutput(btnEnableOutput->getState());

    btnEnableOutput->setPressTime(millis());
  }

  // if button pressed for more than 1 sec...
  /*
  if (millis() - btnEnableOutput->getPressTime() >= 1000) {
    Serial1.println("Stop pressing my buttton.......");
  }
  else Serial1.println("Right button is being pressed");
  */
}

void btnStartFreqPress()
{
  if (btnSetStartFreq->justPressed()) {
    //btnSetStartFreq->drawSmoothButton(true);
    Serial1.println("Start Freq button pressed");
  }
  
}

// Constructor

TFT_gui::TFT_gui(TFT_eSPI &tft, TFT_Touch &touch, HardwareSerial &Serial)
    : tft(tft), touch(touch), Serial(Serial)
{
}

void TFT_gui::setupGui()
{
  // Initialize TFT  
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_SKYBLUE);
  tft.setTextColor(TFT_WHITE, TFT_SKYBLUE);
  tft.setFreeFont(FM9);

  // Draw the title bar
  tft.fillRect(0, 0, tft.width(), 20, TFT_RED);
  tft.setCursor(5, 14); // x,y
  tft.print("VK2XMC ADF4351 Sig gen V0.1");

  updateStatus("Initializing...");

  // Set up the GUI
  uint16_t btnx = (tft.width() - BUTTON_W - R_BORDER);
  uint16_t btny = 30;
/*
  btnL.initButtonUL(btnx, btny, BUTTON_W, BUTTON_H, TFT_WHITE, TFT_RED, TFT_BLACK, "Button", 1);
  //btnL.setPressAction(btnL_pressAction);
  //btnL.setReleaseAction(btnL_releaseAction);
  btnL.drawSmoothButton(false, 3, TFT_BLACK); // 3 is outline width, TFT_BLACK is the surrounding background colour for anti-aliasing

  btny += BUTTON_H + 10;
  btnR.initButtonUL(btnx, btny, BUTTON_W, BUTTON_H, TFT_WHITE, TFT_BLACK, TFT_GREEN, "OFF", 1);
  //btnR.setPressAction(btnR_pressAction);
  //btnR.setReleaseAction(btnR_releaseAction);
  btnR.drawSmoothButton(false, 3, TFT_BLACK); // 3 is outline width, TFT_BLACK is the surrounding background colour for anti-aliasing
*/

  // Create a new button and add it to the button vector

  buttons.push_back(new ButtonWidget(&tft));
  btnSetStartFreq = buttons.back();
  buttons.back()->initButtonUL(btnx, btny, BUTTON_W, BUTTON_H, TFT_WHITE, TFT_RED, TFT_BLACK, "Freq", 1);
  buttons.back()->setPressAction(btnStartFreqPress);
  buttons.back()->drawSmoothButton(false, 3, TFT_BLACK); // 3 is outline width, TFT_BLACK is the surrounding background colour for anti-aliasing

  //btny += BUTTON_H + 10;
  btny = 160;
  buttons.push_back(new ButtonWidget(&tft));
  btnEnableOutput = buttons.back();
  buttons.back()->initButtonUL(btnx, btny, BUTTON_W, BUTTON_H, TFT_WHITE, TFT_BLACK, TFT_GREEN, "RF ON", 1);
  buttons.back()->setPressAction(btnEnablePress);
  buttons.back()->drawSmoothButton(true, 3, TFT_BLACK); // 3 is outline width, TFT_BLACK is the surrounding background colour for anti-aliasing


  // Create a parameter set for the power level slider
  slider_t param;

  // Slider slot parameters
  param.slotWidth = 9;           // Note: ends of slot will be rounded and anti-aliased
  param.slotLength = 150;        // Length includes rounded ends
  param.slotColor = TFT_BLUE;    // Slot colour
  param.slotBgColor = TFT_SKYBLUE; // Slot background colour for anti-aliasing
  param.orientation = H_SLIDER;  // sets it "true" for horizontal

  // Slider control knob parameters (smooth rounded rectangle)
  param.knobWidth = 15;          // Always along x axis
  param.knobHeight = 25;         // Always along y axis
  param.knobRadius = 5;          // Corner radius
  param.knobColor = TFT_WHITE;   // Anti-aliased with slot backgound colour
  param.knobLineColor = TFT_RED; // Colour of marker line (set to same as knobColor for no line)

  // Slider range and movement speed
  param.sliderLT = 0;            // Left side for horizontal, top for vertical slider
  param.sliderRB = 3;            // Right side for horizontal, bottom for vertical slider
  param.startPosition = 3;       // Start position for control knob
  param.sliderDelay = 0;         // Microseconds per pixel movement delay (0 = no delay)

  // Create slider using parameters and plot at 0,0
  sliderKnob = new TFT_eSprite(&tft);
  pwrSlider = new SliderWidget(&tft, sliderKnob);
  pwrSlider->drawSlider(20, 165, param);
  pwrSlider->setSliderPosition(0);

}

void TFT_gui::doGui()
{
    uint16_t t_x = 9999, t_y = 9999; // To store the touch coordinates
    
    // Check if the screen is being touched
    bool pressed = touch.Pressed();
    if (pressed) {
      // Read the current X and Y axis as co-ordinates at the last touch time
      // The values returned were captured when Pressed() was called!
      t_x = touch.X();
      t_y = touch.Y();
      Serial.print("Touch X: "); Serial.print(t_x); Serial.print(" Y: "); Serial.println(t_y); 
    }
    // For each button in the vector, check if it has been pressed
    for (ButtonWidget* btn : buttons) {
      if (pressed) {
        if (btn->contains(t_x, t_y)) {
          btn->press(true);
          btn->pressAction();
          //Serial.println("Button pressed");
        }
      }
      else {
        btn->press(false);
        btn->releaseAction();
        //Serial.println("Button released");
      }
    }
    if (pwrSlider->checkTouch(t_x, t_y)) {
        Serial1.print("Slider 1 = "); Serial1.println(pwrSlider->getSliderPosition());
        setPowerLevel(pwrSlider->getSliderPosition());
    }
}

void TFT_gui::updateStatus(String status, uint32_t color)
{
  tft.fillRect(0, 220, tft.width(), 240, TFT_LIGHTGREY);
  tft.setTextColor(color);
  tft.setCursor(5, 235);
  tft.setFreeFont(FMB9);
  tft.print(status);
  tft.setFreeFont(FM9);
}

void TFT_gui::updateStartFreq(uint32_t freq)
{
  char s[16];

  sprintf(s, "%d.%03d", freq/1000000, (freq%1000000)/1000);

  tft.fillRect(15, 30, 110, 50, TFT_LIGHTGREY);
  tft.setTextColor(TFT_WHITE);
  tft.setCursor(20, 60);
  tft.setFreeFont(FSS12);
  tft.print(s);
  tft.setTextColor(TFT_BLACK);
  tft.setCursor(125, 60);
  tft.print(" MHz");

  tft.setFreeFont(FM9);
}



/*
// Create an array of button instances to use in for() loops
// This is more useful where large numbers of buttons are employed
ButtonWidget* btn[] = {&btnL , &btnR};;
uint8_t buttonCount = sizeof(btn) / sizeof(btn[0]);

void btnL_pressAction(void)
{
  Serial1.println("L");
  if (btnL.justPressed()) {
    Serial1.println("Left button just pressed");
    btnL.drawSmoothButton(true);
  }
}

void btnL_releaseAction(void)
{
  static uint32_t waitTime = 1000;
  if (btnL.justReleased()) {
    Serial1.println("Left button just released");
    btnL.drawSmoothButton(false);
    btnL.setReleaseTime(millis());
    waitTime = 10000;
  }
  else {
    if (millis() - btnL.getReleaseTime() >= waitTime) {
      waitTime = 1000;
      btnL.setReleaseTime(millis());
      btnL.drawSmoothButton(!btnL.getState());
    }
  }
}

void btnR_pressAction(void)
{
  Serial1.println("R");
  if (btnR.justPressed()) {
    btnR.drawSmoothButton(!btnR.getState(), 3, TFT_BLACK, btnR.getState() ? "OFF" : "ON");
    Serial1.print("Button toggled: ");
    if (btnR.getState()) Serial1.println("ON");
    else  Serial1.println("OFF");
    btnR.setPressTime(millis());
  }

  // if button pressed for more than 1 sec...
  if (millis() - btnR.getPressTime() >= 1000) {
    Serial1.println("Stop pressing my buttton.......");
  }
  else Serial1.println("Right button is being pressed");
}

void btnR_releaseAction(void)
{
  // Not action
}
*/