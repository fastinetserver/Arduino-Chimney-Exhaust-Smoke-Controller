//
// Created by Kos Ovechko on 12.10.22.
//

#ifndef CHIMNEY_EXHAUST_SMOKE_CONTROLLER_STATS_MODULE_H
#define CHIMNEY_EXHAUST_SMOKE_CONTROLLER_STATS_MODULE_H

#include "pins_module.h"
#include "lcd_module.h"
#include "pot_module.h"
#include "motor_modes_module.h"
#include "settings_module.h"

void setup_stats(){
    Serial.println(F("Before read_celsius_avg()"));
    read_celsius_avg();
    for (byte i=0; i<MAX_POT_LIST_SIZE; i++) {
        update_pot_pos();
        delay(5);
    }
    read_celsius_avg();
    Serial.println(F("After read_celsius_avg()"));
}

void show_stats(){
    update_pot_pos();
    valve_open_percent = get_valve_open_percent();
    String temp_line = String(celsius) + "/" +String(int(settings.goal_temp)) + "C O:" + String(valve_open_percent)+"%";
    if (cur_motor_mode == MOTOR_MODE_AUTO || cur_motor_mode == MOTOR_MODE_CLEAN){
        if (settings.goal_temp - celsius > TEMP_THRESHOLD){
            // Temp is too LOW
            digitalWrite(RED_LED, LOW);
            digitalWrite(GREEN_LED, LOW);
            digitalWrite(BLUE_LED, HIGH);
        } else if (celsius - settings.goal_temp > TEMP_THRESHOLD){
            // Temp is too HIGH
            digitalWrite(RED_LED, HIGH);
            digitalWrite(GREEN_LED, LOW);
            digitalWrite(BLUE_LED, LOW);
        } else {
            // Temp is GOOD (within our THRESHOLD)
            digitalWrite(RED_LED, LOW);
            digitalWrite(GREEN_LED, HIGH);
            digitalWrite(BLUE_LED, LOW);
        }
    }
    static byte loop_bool = 0;
    if (cur_motor_mode == MOTOR_MODE_MANUAL){
      digitalWrite(RED_LED, loop_bool == 0);
      digitalWrite(GREEN_LED, loop_bool == 0);
      digitalWrite(BLUE_LED, loop_bool == 0);
      loop_bool = (loop_bool + 1) % 20;
    }
    lcd_line(0, temp_line);
}

#endif //CHIMNEY_EXHAUST_SMOKE_CONTROLLER_STATS_MODULE_H
