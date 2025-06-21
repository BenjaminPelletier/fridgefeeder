#include "state.h"

#include <EEPROM.h>

#include "keypad.h"

const unsigned char SETTINGS_INITIALIZED = 123;

Settings state;

void init_state() {
  Button btn = get_button();
  if (btn == Button::None) {
    load_state();
  }
}

bool load_state() {
  unsigned char settings_state = EEPROM.read(0);
  if (settings_state == SETTINGS_INITIALIZED) {
    EEPROM.get(1, state);
    return true;
  } else {
    return false;
  }
}

void save_state() {
  EEPROM.write(0, SETTINGS_INITIALIZED);
  EEPROM.put(1, state);
}
