#pragma once

enum class Button : unsigned char {
  None,
  Right,
  Up,
  Down,
  Left,
  Select,
};

void init_keypad();

void update_keypad();

bool button_changed();

Button get_button();
