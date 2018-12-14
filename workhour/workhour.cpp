#include <EEPROM.h>
#include "Arduino.h"
#include "workhour.h"

WorkHour::WorkHour(int relay_pin, int reset_pin, int led_pin) {
  _relay_pin = relay_pin;
  _reset_pin = reset_pin;
  _led_pin = led_pin;
}

void WorkHour::setup() {
  pinMode(_relay_pin, INPUT);
  pinMode(_reset_pin, INPUT);
  pinMode(_led_pin, OUTPUT);
  digitalWrite(_led_pin, HIGH);

  EEPROM.begin(512);
  this->_load();
}

void WorkHour::_load() {
  Serial.println("Read previous data from EEPROM.");
  unsigned long prev_data;
  EEPROM.get(0, prev_data);
  Serial.println("Continue from:"+String(prev_data));
  this->sum_time = prev_data;
}

void WorkHour::_save() {
  EEPROM.put(0, this->sum_time);
  EEPROM.commit();
}

void WorkHour::log_time() {
  int current_relay_status = !digitalRead(this->_relay_pin);
  if (this->saved_relay_status == 0 && current_relay_status == 1) {
     this->_session_start_time = millis();
     this->saved_relay_status = 1;
     digitalWrite(this->_led_pin, LOW);
  }
  else if (this->saved_relay_status == 1 && current_relay_status == 0) {
    unsigned long work_time = calc_work_time();
    this->saved_relay_status = 0;
    digitalWrite(this->_led_pin, HIGH);
    this->sum_time += work_time;
    Serial.println("sum:"+String(this->sum_time));
    this->_save();
  }
}

void WorkHour::check_reset() {
  if (!digitalRead(this->_reset_pin)) {
    Serial.print("RESET sum:");
    this->sum_time = 0;
    this->_save();
    this->saved_relay_status = 0;
    this->_blink(2);
    Serial.println(String(this->sum_time));
  }
}

unsigned long WorkHour::calc_work_time() {
  return (millis() - this->_session_start_time)/1000;
}

void WorkHour::_blink(int nn) {
  for (int ii = 0; ii < nn; ii++) {
    digitalWrite(this->_led_pin, LOW);
    delay(250);
    digitalWrite(this->_led_pin, HIGH);
    delay(250);
  }
}