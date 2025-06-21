/* Requires libraries:
 *   LiquidCrystal 1.0.7
 */

#include "countdown.h"
#include "display.h"
#include "door.h"
#include "keypad.h"
#include "state.h"
#include "ui.h"


void setup() {
  Serial.begin(115200);
  Serial.println("Initializing...");
  init_keypad();
  init_state();
  init_display();
  init_door();
  init_countdown();
  Serial.println("Initialization complete.");
}

void loop() {
  update_keypad();
  update_ui();
  update_countdown();
  update_door();
}
