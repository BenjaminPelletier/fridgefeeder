#include "display.h"

#include <Arduino.h>
#include <LiquidCrystal.h>

#include "state.h"
#include "ui.h"

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

void init_display() {
  lcd.begin(16, 2);
  lcd.createChar(CHAR_DOWN_ARROW, downArrow);
  lcd.createChar(CHAR_PLAY_ICON, playIcon);
  lcd.createChar(CHAR_PAUSE_ICON, pauseIcon);
  lcd.clear();
  show_edit();
  show_time();
  show_status();
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
  EditMode edit_mode = get_edit_mode();
  lcd.setCursor(0, 0);
  if (edit_mode == EditMode::Days) {
    lcd.write(byte(CHAR_DOWN_ARROW));
    lcd.print("               ");
  } else if (edit_mode == EditMode::Hours) {
    lcd.print("   ");
    lcd.write(byte(CHAR_DOWN_ARROW));
    lcd.write(byte(CHAR_DOWN_ARROW));
    lcd.print("           ");
  } else if (edit_mode == EditMode::Minutes) {
    lcd.print("       ");
    lcd.write(byte(CHAR_DOWN_ARROW));
    lcd.write(byte(CHAR_DOWN_ARROW));
    lcd.print("       ");
  } else if (edit_mode == EditMode::Seconds) {
    lcd.print("           ");
    lcd.write(byte(CHAR_DOWN_ARROW));
    lcd.write(byte(CHAR_DOWN_ARROW));
    lcd.print("   ");
  } else if (edit_mode == EditMode::PlayPause) {
    lcd.print("               ");
    lcd.write(byte(CHAR_DOWN_ARROW));
  } else if (edit_mode == EditMode::Open) {
    lcd.print("Open:           ");
    lcd.setCursor(6, 0);
    lcd.print(state.rc_open);
  } else if (edit_mode == EditMode::Closed) {
    lcd.print("Closed:         ");
    lcd.setCursor(8, 0);
    lcd.print(state.rc_closed);
  } else if (edit_mode == EditMode::Save) {
    lcd.print("Save?           ");
  } else if (edit_mode == EditMode::Load) {
    lcd.print("Load?           ");
  } else {
    lcd.print("UNKNOWN EDITMODE");
  }
}

void show_message(const char* msg) {
  lcd.setCursor(0, 0);
  lcd.print(msg);
}
