#include "keypad.h"

#include <Arduino.h>

Button btn_pending = Button::None;
unsigned char btn_pending_count = 0;
Button btn_active = Button::None;
unsigned long t_active = 0;
const int DEBOUNCE_MS = 30;
const unsigned char MIN_REPEAT_COUNT = 3;
bool new_button = false;

Button read_button() {
  int s = analogRead(A0);
  if (s < 50) return Button::Right;
  if (s < 178) return Button::Up;
  if (s < 350) return Button::Down;
  if (s < 550) return Button::Left;
  if (s < 830) return Button::Select;
  return Button::None;
}

void init_keypad() {
  Button btn = read_button();
  btn_pending = btn;
  btn_active = btn;
}

void update_keypad() {
  unsigned long t = millis();
  
  Button btn_current = read_button();
  if (btn_current != btn_pending) {
    btn_pending = btn_current;
    btn_pending_count = 0;
    t_active = t + DEBOUNCE_MS;
    return;
  }
  if (btn_current == btn_active) return; // The pending button is the active button; nothing to do
  if (btn_pending_count < MIN_REPEAT_COUNT) {
    btn_pending_count++;
    return;
  }
  if ((signed long)t - (signed long)t_active > 0) {
    btn_active = btn_pending;
    new_button = true;
  }
}

bool button_changed() {
  if (new_button) {
    new_button = false;
    return true;
  }
  return false;
}

Button get_button() {
  return btn_active;
}
