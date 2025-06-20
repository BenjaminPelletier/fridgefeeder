/* Requires libraries:
 *   LiquidCrystal 1.0.7
 */

#include <EEPROM.h>
#include <LiquidCrystal.h>

const int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

byte downArrow[8] = {
  0b00100,
  0b00100,
  0b00100,
  0b00100,
  0b11111,
  0b01110,
  0b00100
};
unsigned char CHAR_DOWN_ARROW = 0;
byte playIcon[8] = {
  0b00000,
  0b01000,
  0b01100,
  0b01110,
  0b01100,
  0b01000,
  0b00000
};
unsigned char CHAR_PLAY_ICON = 1;
byte pauseIcon[8] = {
  0b00000,
  0b11011,
  0b11011,
  0b11011,
  0b11011,
  0b11011,
  0b00000
};
unsigned char CHAR_PAUSE_ICON = 2;

unsigned long t_countdown_last;

struct Settings {
  int days = 0;
  int hours = 0;
  int minutes = 0;
  int seconds = 10;
  bool paused = false;
  int rc_open = 1000;
  int rc_closed = 2000;
};
struct Settings state;
const unsigned char SETTINGS_INITIALIZED = 123;

const unsigned char PIN_SERVO = A5;
bool door_open = false;
unsigned long t_servo_last = 0;
const int SERVO_MS = 20;
const int RC_MIN = 900;
const int RC_MAX = 2100;

const unsigned char BTN_NONE = 0;
const unsigned char BTN_RIGHT = 1;
const unsigned char BTN_UP = 2;
const unsigned char BTN_DOWN = 3;
const unsigned char BTN_LEFT = 4;
const unsigned char BTN_SELECT = 5;
unsigned char btn_pending = BTN_NONE;
unsigned char btn_prev = BTN_NONE;
unsigned long debounce = 0;
const int DEBOUNCE_MS = 70;

const unsigned char EDIT_MODE_MIN = 0;
const unsigned char EDIT_MODE_DAYS = 0;
const unsigned char EDIT_MODE_HOURS = 1;
const unsigned char EDIT_MODE_MINUTES = 2;
const unsigned char EDIT_MODE_SECONDS = 3;
const unsigned char EDIT_MODE_PLAYPAUSE = 4;
const unsigned char EDIT_MODE_OPEN = 5;
const unsigned char EDIT_MODE_CLOSED = 6;
const unsigned char EDIT_MODE_SAVE = 7;
const unsigned char EDIT_MODE_LOAD = 8;
const unsigned char EDIT_MODE_MAX = 8;
unsigned char edit_mode = 0;

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

void show_time() {
  lcd.setCursor(0, 1);
  
  lcd.print(state.days);
  lcd.print("d ");
  
  if (state.hours < 10) lcd.print('0');
  lcd.print(state.hours);
  lcd.print("h ");

  if (state.minutes < 10) lcd.print('0');
  lcd.print(state.minutes);
  lcd.print("m ");

  if (state.seconds < 10) lcd.print('0');
  lcd.print(state.seconds);
  lcd.print("s");
}

void show_status() {
  lcd.setCursor(15, 1);
  lcd.write(state.paused ? byte(CHAR_PAUSE_ICON) : byte(CHAR_PLAY_ICON));
}

void show_edit() {
  lcd.setCursor(0, 0);
  if (edit_mode == EDIT_MODE_DAYS) {
    lcd.write(byte(CHAR_DOWN_ARROW));
    lcd.print("               ");
  } else if (edit_mode == EDIT_MODE_HOURS) {
    lcd.print("   ");
    lcd.write(byte(CHAR_DOWN_ARROW));
    lcd.write(byte(CHAR_DOWN_ARROW));
    lcd.print("           ");
  } else if (edit_mode == EDIT_MODE_MINUTES) {
    lcd.print("       ");
    lcd.write(byte(CHAR_DOWN_ARROW));
    lcd.write(byte(CHAR_DOWN_ARROW));
    lcd.print("       ");
  } else if (edit_mode == EDIT_MODE_SECONDS) {
    lcd.print("           ");
    lcd.write(byte(CHAR_DOWN_ARROW));
    lcd.write(byte(CHAR_DOWN_ARROW));
    lcd.print("   ");
  } else if (edit_mode == EDIT_MODE_PLAYPAUSE) {
    lcd.print("               ");
    lcd.write(byte(CHAR_DOWN_ARROW));
  } else if (edit_mode == EDIT_MODE_OPEN) {
    lcd.print("Open:           ");
    lcd.setCursor(6, 0);
    lcd.print(state.rc_open);
  } else if (edit_mode == EDIT_MODE_CLOSED) {
    lcd.print("Closed:         ");
    lcd.setCursor(8, 0);
    lcd.print(state.rc_closed);
  } else if (edit_mode == EDIT_MODE_SAVE) {
    lcd.print("Save?           ");
  } else if (edit_mode == EDIT_MODE_LOAD) {
    lcd.print("Load?           ");
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Initializing...");
  load_state();
  pinMode(PIN_SERVO, OUTPUT);
  digitalWrite(PIN_SERVO, LOW);
  lcd.begin(16, 2);
  lcd.createChar(CHAR_DOWN_ARROW, downArrow);
  lcd.createChar(CHAR_PLAY_ICON, playIcon);
  lcd.createChar(CHAR_PAUSE_ICON, pauseIcon);
  lcd.clear();
  show_edit();
  show_time();
  show_status();
  Serial.print("Initialization complete.");
  t_countdown_last = millis();
  t_servo_last = t_countdown_last;
}

unsigned char get_button() {
  int s = analogRead(A0);
  if (s < 50) return BTN_RIGHT;
  if (s < 178) return BTN_UP;
  if (s < 350) return BTN_DOWN;
  if (s < 550) return BTN_LEFT;
  if (s < 830) return BTN_SELECT;
  return BTN_NONE;
}

void process_button() {
  unsigned long t = millis();
  if (t < debounce) return;
  
  unsigned char btn_current = get_button();
  if (btn_current != btn_pending) {
    btn_pending = btn_current;
    return;
  }
  if (btn_current == btn_prev) return;
  debounce = t + DEBOUNCE_MS;
  btn_prev = btn_current;
  
  if (btn_current == BTN_NONE) return;

  bool skip_show_edit = false;

  if (btn_current == BTN_LEFT) {
    if (edit_mode > EDIT_MODE_MIN) {
      edit_mode--;
    } else {
      edit_mode = EDIT_MODE_MAX;
    }

  } else if (btn_current == BTN_RIGHT) {
    if (edit_mode < EDIT_MODE_MAX) {
      edit_mode++;
    } else {
      edit_mode = EDIT_MODE_MIN;
    }

  } else if (btn_current == BTN_UP) {
    if (edit_mode == EDIT_MODE_DAYS) {
      state.days++;
    } else if (edit_mode == EDIT_MODE_HOURS) {
      state.hours++;
    } else if (edit_mode == EDIT_MODE_MINUTES) {
      state.minutes++;
    } else if (edit_mode == EDIT_MODE_SECONDS) {
      state.seconds++;
    } else if (edit_mode == EDIT_MODE_PLAYPAUSE) {
      state.paused = !state.paused;
    } else if (edit_mode == EDIT_MODE_OPEN) {
      state.rc_open += 25;
      if (state.rc_open > RC_MAX) state.rc_open = RC_MAX;
    } else if (edit_mode == EDIT_MODE_CLOSED) {
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

  } else if (btn_current == BTN_DOWN) {
    if (edit_mode == EDIT_MODE_DAYS) {
      state.days--;
    } else if (edit_mode == EDIT_MODE_HOURS) {
      state.hours--;
    } else if (edit_mode == EDIT_MODE_MINUTES) {
      state.minutes--;
    } else if (edit_mode == EDIT_MODE_SECONDS) {
      state.seconds--;
    } else if (edit_mode == EDIT_MODE_PLAYPAUSE) {
      state.paused = !state.paused;
    } else if (edit_mode == EDIT_MODE_OPEN) {
      state.rc_open -= 25;
      if (state.rc_open < RC_MIN) state.rc_open = RC_MIN;
    } else if (edit_mode == EDIT_MODE_CLOSED) {
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

  } else if (btn_current == BTN_SELECT) {
    if (edit_mode == EDIT_MODE_DAYS) {
      state.days = 0;
    } else if (edit_mode == EDIT_MODE_HOURS) {
      state.hours = 0;
    } else if (edit_mode == EDIT_MODE_MINUTES) {
      state.minutes = 0;
    } else if (edit_mode == EDIT_MODE_SECONDS) {
      state.seconds = 0;
    } else if (edit_mode == EDIT_MODE_PLAYPAUSE) {
      state.paused = !state.paused;
    } else if (edit_mode == EDIT_MODE_OPEN || edit_mode == EDIT_MODE_CLOSED) {
      int temp = state.rc_open;
      state.rc_open = state.rc_closed;
      state.rc_closed = temp;
    } else if (edit_mode == EDIT_MODE_SAVE) {
      save_state();
      lcd.setCursor(0, 0);
      lcd.print("Saved.          ");
      skip_show_edit = true;
    } else if (edit_mode == EDIT_MODE_LOAD) {
      bool success = load_state();
      lcd.setCursor(0, 0);
      if (success) {
        lcd.print("Loaded.         ");
      } else {
        lcd.print("No saved state! ");
      }
      skip_show_edit = true;
    }
  }

  if (!skip_show_edit) {
    show_edit();
  }
  show_time();
  show_status();
}

void t_plus_one_second() {
  if (state.days == 0 && state.hours == 0 && state.minutes == 0 && state.seconds == 0) {
    door_open = true;
    return;
  }
  door_open = false;
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

void loop() {
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
  process_button();
  if (t1 > t_servo_last + SERVO_MS) {
    digitalWrite(PIN_SERVO, HIGH);
    delayMicroseconds(door_open ? state.rc_open : state.rc_closed);
    digitalWrite(PIN_SERVO, LOW);
    t_servo_last = t1;
  }
}
