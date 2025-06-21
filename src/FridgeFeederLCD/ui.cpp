#include "ui.h"

#include "display.h"
#include "door.h"
#include "keypad.h"
#include "state.h"

EditMode& operator++(EditMode& mode)
{
  if (mode == EditMode::Max) {
    mode = EditMode::Min;
  } else {
    mode = (EditMode)((unsigned char)mode + 1);
  }
  return mode;
}

EditMode& operator--(EditMode& mode)
{
  if (mode == EditMode::Min) {
    mode = EditMode::Max;
  } else {
    mode = (EditMode)((unsigned char)mode - 1);
  }
  return mode;
}

EditMode edit_mode = EditMode::PlayPause;

EditMode get_edit_mode() {
  return edit_mode;
}

void set_door_by_edit_mode() {
  if (edit_mode == EditMode::Open) {
    set_door(state.rc_open);
  } else if (edit_mode == EditMode::Closed) {
    set_door(state.rc_closed);
  } else {
    if (state.days == 0 && state.hours == 0 && state.minutes == 0 && state.seconds == 0) {
      set_door(state.rc_open);
    } else {
      set_door(state.rc_closed);
    }
  }
}

void update_ui() {
  if (!button_changed()) return;
  Button btn_current = get_button();
  if (btn_current == Button::None) return;
  
  bool skip_show_edit = false;

  if (btn_current == Button::Left) {
    edit_mode--;
    set_door_by_edit_mode();

  } else if (btn_current == Button::Right) {
    edit_mode++;
    set_door_by_edit_mode();

  } else if (btn_current == Button::Up) {
    if (edit_mode == EditMode::Days) {
      state.days++;
    } else if (edit_mode == EditMode::Hours) {
      state.hours++;
    } else if (edit_mode == EditMode::Minutes) {
      state.minutes++;
    } else if (edit_mode == EditMode::Seconds) {
      state.seconds++;
    } else if (edit_mode == EditMode::PlayPause) {
      state.paused = !state.paused;
    } else if (edit_mode == EditMode::Open) {
      state.rc_open += 25;
      if (state.rc_open > RC_MAX) state.rc_open = RC_MAX;
    } else if (edit_mode == EditMode::Closed) {
      state.rc_closed += 25;
      if (state.rc_closed > RC_MAX) state.rc_closed = RC_MAX;
    }
    while (state.seconds >= 60) {
      state.minutes++;
      state.seconds -= 60;
    }
    while (state.minutes >= 60) {
      state.hours++;
      state.minutes -= 60;
    }
    while (state.hours >= 24) {
      state.days++;
      state.hours -= 24;
    }
    if (state.days > 9) state.days = 9;
    set_door_by_edit_mode();

  } else if (btn_current == Button::Down) {
    if (edit_mode == EditMode::Days) {
      state.days--;
    } else if (edit_mode == EditMode::Hours) {
      state.hours--;
    } else if (edit_mode == EditMode::Minutes) {
      state.minutes--;
    } else if (edit_mode == EditMode::Seconds) {
      state.seconds--;
    } else if (edit_mode == EditMode::PlayPause) {
      state.paused = !state.paused;
    } else if (edit_mode == EditMode::Open) {
      state.rc_open -= 25;
      if (state.rc_open < RC_MIN) state.rc_open = RC_MIN;
    } else if (edit_mode == EditMode::Closed) {
      state.rc_closed -= 25;
      if (state.rc_closed < RC_MIN) state.rc_closed = RC_MIN;
    }
    while (state.seconds < 0) {
      state.minutes--;
      state.seconds += 60;
    }
    while (state.minutes < 0) {
      state.hours--;
      state.minutes += 60;
    }
    while (state.hours < 0) {
      state.days--;
      state.hours += 24;
    }
    if (state.days < 0) state.days = 0;
    set_door_by_edit_mode();

  } else if (btn_current == Button::Select) {
    if (edit_mode == EditMode::Days) {
      state.days = 0;
    } else if (edit_mode == EditMode::Hours) {
      state.hours = 0;
    } else if (edit_mode == EditMode::Minutes) {
      state.minutes = 0;
    } else if (edit_mode == EditMode::Seconds) {
      state.seconds = 0;
    } else if (edit_mode == EditMode::PlayPause) {
      state.paused = !state.paused;
    } else if (edit_mode == EditMode::Open || edit_mode == EditMode::Closed) {
      int temp = state.rc_open;
      state.rc_open = state.rc_closed;
      state.rc_closed = temp;
    } else if (edit_mode == EditMode::Save) {
      save_state();
      show_message("Saved.          ");
      skip_show_edit = true;
    } else if (edit_mode == EditMode::Load) {
      bool success = load_state();
      if (success) {
        show_message("Loaded.         ");
      } else {
        show_message("No saved state! ");
      }
      skip_show_edit = true;
    }
    set_door_by_edit_mode();
  }

  if (!skip_show_edit) {
    show_edit();
  }
  show_time();
  show_status();
}
