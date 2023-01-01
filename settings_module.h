//
// Created by Kos Ovechko on 12.10.22.
//

#ifndef CHIMNEY_EXHAUST_SMOKE_CONTROLLER_SETTINGS_MODULE_H
#define CHIMNEY_EXHAUST_SMOKE_CONTROLLER_SETTINGS_MODULE_H

#include <EEPROM.h>
#include "ds1820_module.h"
#include "lcd_module.h"
#include "stats_module.h"

const char INITIALIZED = 'I';

struct {
    byte goal_temp = DEFAULT_GOAL_TEMP;
} settings;

void write_settings(){
    lcd_line_center(1, "Saving Settings.");
    delay(1*SECOND);  // Delay to prevent EEPROM Wear out
    EEPROM.put(1, settings);
    delay(3*SECOND);  // Delay to prevent EEPROM Wear out
    lcd_line_center(1, "Settings Saved");
    delay(2*SECOND);  // Delay to prevent EEPROM Wear out
}

void read_settings(){
    if (EEPROM.read(0) != INITIALIZED){
        settings.goal_temp = DEFAULT_GOAL_TEMP;
        write_settings();
        EEPROM.write(0, INITIALIZED);
    } else {
        EEPROM.get(1, settings);
        if (settings.goal_temp < MIN_GOAL_TEMP || settings.goal_temp > MAX_GOAL_TEMP){
            settings.goal_temp = DEFAULT_GOAL_TEMP;
            write_settings();
        }
    }
}

#endif //CHIMNEY_EXHAUST_SMOKE_CONTROLLER_SETTINGS_MODULE_H
