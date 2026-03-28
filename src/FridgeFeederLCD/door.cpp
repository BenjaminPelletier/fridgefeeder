#include "door.h"

#include <Arduino.h>

#include "state.h"

const unsigned char PIN_SERVO = A5;
int door_value;
unsigned long t_servo_last = 0;
unsigned long servo_last = 0;
unsigned long servo_pulses = 0;
const int SERVO_MS = 20;
const int SERVO_CHANGE_PULSES = 100;

void init_door() {
  pinMode(PIN_SERVO, OUTPUT);
  digitalWrite(PIN_SERVO, LOW);
  t_servo_last = millis();
  door_value = state.rc_closed;
}

void set_door(int value) {
  if (value < RC_MIN) {
    door_value = RC_MIN;
  } else if (value > RC_MAX) {
    door_value = RC_MAX;
  } else {
    door_value = value;
  }
}

void update_door() {
  unsigned long t1 = millis();
  if (t1 > t_servo_last + SERVO_MS) {
    if (door_value == servo_last) {
      servo_pulses++;
    } else {
      servo_pulses = 0;
      servo_last = door_value;
    }
    if (servo_pulses < SERVO_CHANGE_PULSES) {
      digitalWrite(PIN_SERVO, HIGH);
      delayMicroseconds(door_value);
      digitalWrite(PIN_SERVO, LOW);
    }
    t_servo_last = t1;
  }
}
