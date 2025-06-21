#pragma once

enum class EditMode : unsigned char {
  Min = 0,
  Days = 0,
  Hours = 1,
  Minutes = 2,
  Seconds = 3,
  PlayPause = 4,
  Open = 5,
  Closed = 6,
  Save = 7,
  Load = 8,
  Max = 8,
};

EditMode& operator++(EditMode& orig);

EditMode& operator--(EditMode& orig);

void update_ui();

EditMode get_edit_mode();
