#include "countdown.h"

#include <Arduino.h>

#include "display.h"
#include "door.h"
#include "state.h"

unsigned long t_countdown_last;

void init_countdown() {
  t_countdown_last = millis();
}

void t_plus_one_second() {
  if (state.days == 0 && state.hours == 0 && state.minutes == 0 && state.seconds == 0) {
    set_door(state.rc_open);
    return;
  }
  state.seconds--;
  if (state.seconds < 0) {
    state.seconds += 60;
    state.minutes -= 1;
  }
  if (state.minutes < 0) {
    state.minutes += 60;
    state.hours -= 1;
  }
  if (state.hours < 0) {
    state.hours += 24;
    state.days -= 1;
  }
  show_time();
}

void update_countdown() {
  unsigned long t1 = millis();
  if (t1 - t_countdown_last > 1000) {
    if (t1 - t_countdown_last > 2000) {
      // Wrap around
      t_countdown_last = t1;
    } else {
      t_countdown_last += 1000;
    }
    if (!state.paused) {
      t_plus_one_second();
    }
  }
}
