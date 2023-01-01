//
// Created by Kos Ovechko on 12.10.22.
//


#include "pins_module.h"
#include "lcd_module.h";
#include "led_module.h";
#include "bluetooth_module.h";
#include "ds1820_module.h";
#include "motor_module.h";
#include "settings_module.h";
#include "encoder_ui_module.h";

//To-Do: change settings with Encoder

//To-Do: change settings over bluetooth  clean, hourly-stats

uint32_t MOTOR_RELAX_TIMEOUT = 5 * MINUTE;

void setup() {
  Serial.begin(9600);
  Serial.println(F("Starting 1:"));

  setup_motor_pins();
  setup_leds();
  setup_ui();
  lcd.init();
  lcd_line(0, F("Starting:"));
  turn_lcd_backlight_on();

  leds_startup_blinking();
  read_settings();
  setup_bluetooth();

  lcd_line(1, F("Startup finished"));
  setup_temp_module();
  setup_stats();
  lcd_line(1, MOTOR_MODE_NAMES[cur_motor_mode]);
}

void auto_motor_mode(){
  if (cur_motor_mode != MOTOR_MODE_AUTO){
    return;
  }
  int new_valve_open_percent = 25;  // Default Value is to throttle
  if (celsius - float(settings.goal_temp) < -20) {
    new_valve_open_percent = 100;
  } else if (celsius - float(settings.goal_temp) < -15) {
    new_valve_open_percent = 80;
  } else if (celsius - float(settings.goal_temp) < -10) {
    new_valve_open_percent = 60;
  } else if (celsius - float(settings.goal_temp) < -5) {
    new_valve_open_percent = 45;
  } else  if (celsius - float(settings.goal_temp) < -2) {
    new_valve_open_percent = valve_open_percent + 5;
  } else  if (celsius - float(settings.goal_temp) < -1) {
    new_valve_open_percent = valve_open_percent + 2;
  } else  if (celsius - float(settings.goal_temp) < 1) {
    new_valve_open_percent = valve_open_percent;
  } else  if (celsius - float(settings.goal_temp) < 2) {
    new_valve_open_percent = valve_open_percent - 2;
  } else  if (celsius - float(settings.goal_temp) < 5) {
    new_valve_open_percent = min(25, valve_open_percent - 4);  // let's ensure when setting 25% we are not opening, but closing
  } else { // If >=5
    new_valve_open_percent = 22;    
  }

  new_valve_open_percent = max(VALVE_MIN_OPEN_PERCENT, min(VALVE_MAX_OPEN_PERCENT, new_valve_open_percent)); // Make sure we fit the boundaries
  
  if (abs(valve_open_percent - new_valve_open_percent) >= 2){
    motor_to_percent(new_valve_open_percent);
  }
}

void loop() {
  run_encoder_ui();
  read_bluetooth();

  update_all_temp_stats();

  static uint32_t pot_tmr;
  if (millis() - pot_tmr > (isMotorRunning ? 20: 5000)){
     update_pot_pos();
  }

  static uint32_t motor_relax_tmr = millis() - MOTOR_RELAX_TIMEOUT + 20 * SECOND; // Start in 20 seconds after boot
  if (millis() - motor_relax_tmr > MOTOR_RELAX_TIMEOUT){
    motor_relax_tmr = millis();
    auto_motor_mode();
  }
  motor_run();
}
