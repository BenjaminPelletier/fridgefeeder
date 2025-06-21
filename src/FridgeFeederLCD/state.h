#pragma once

struct Settings {
  int days = 0;
  int hours = 0;
  int minutes = 0;
  int seconds = 10;
  bool paused = false;
  int rc_open = 1400;
  int rc_closed = 1600;
};
extern Settings state;

void init_state();

bool load_state();

void save_state();
