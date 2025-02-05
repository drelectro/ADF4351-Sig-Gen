#include "gui.h"

#define BUTTON_W 100
#define BUTTON_H 50

#define R_BORDER 10


void btnOnOffPress()
{
  Serial1.println("Button pressed");

  /*
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
  */
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
  uint16_t btny = 40;
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
  buttons.back()->initButtonUL(btnx, btny, BUTTON_W, BUTTON_H, TFT_WHITE, TFT_RED, TFT_BLACK, "Set F", 1);
  buttons.back()->drawSmoothButton(false, 3, TFT_BLACK); // 3 is outline width, TFT_BLACK is the surrounding background colour for anti-aliasing

  btny += BUTTON_H + 10;
  buttons.push_back(new ButtonWidget(&tft));
  buttons.back()->initButtonUL(btnx, btny, BUTTON_W, BUTTON_H, TFT_WHITE, TFT_BLACK, TFT_GREEN, "OFF", 1);
  buttons.back()->setPressAction(btnOnOffPress);
  buttons.back()->drawSmoothButton(false, 3, TFT_BLACK); // 3 is outline width, TFT_BLACK is the surrounding background colour for anti-aliasing

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
          //btn.pressAction();
          Serial.println("Button pressed");
        }
      }
      else {
        //btn->.releaseAction();
        //Serial.println("Button released");
      }
    }
    /*
    for (uint8_t b = 0; b < buttonCount; b++) {
      if (pressed) {
        if (btn[b]->contains(t_x, t_y)) {
          btn[b]->press(true);
          btn[b]->pressAction();
        }
      }
      else {
        btn[b]->press(false);
        btn[b]->releaseAction();
      }
    }
    */
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