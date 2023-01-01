//
// Created by Kos Ovechko on 12.10.22.
//

#ifndef CHIMNEY_EXHAUST_SMOKE_CONTROLLER_MOTOR_MODULE_H
#define CHIMNEY_EXHAUST_SMOKE_CONTROLLER_MOTOR_MODULE_H

#include "pins_module.h"
#include "time_module.h"
#include "motor_modes_module.h"
#include "stats_module.h"

byte MAX_SPEED = 255;   // Should not be more than 255, but i'll set lower value here
byte MIN_SPEED = 127;

uint32_t MOTOR_TIMEOUT = 20 * SECOND;

#define UP                 false
#define DOWN               true

byte raw_speed = MIN_SPEED;
bool direction;

void setup_motor_pins(){
    pinMode(MOTOR_ENABLE_PIN, OUTPUT);
    pinMode(MOTOR_SPEED_PIN, OUTPUT);
}

void motor_stop(){
    digitalWrite(MOTOR_ENABLE_PIN, LOW);
    analogWrite(MOTOR_SPEED_PIN, 0);   //PWM Speed Control, speed = 0 - Full stop
    digitalWrite(YELLOW_LED, LOW);
}

bool is_target_percent_reached(byte target_percent){
    if (direction == UP){
        Serial.print("UP Target percent: " + String(target_percent) + "Valve percent:" + String(valve_open_percent));
        return valve_open_percent <= target_percent;
    } else {
        Serial.print("DOWN Target percent: " + String(target_percent) + "Valve percent:" + String(valve_open_percent));
        return valve_open_percent >= target_percent;
    }
}

bool is_need_to_accelerate(byte target_percent){
    const byte DECELERATION_PERCENTS = 10;
    if (direction == UP){
        return valve_open_percent > target_percent + DECELERATION_PERCENTS;
    } else {
        return valve_open_percent + DECELERATION_PERCENTS < target_percent;
    }
}

void motor_to_percent(byte set_target_percent){
    Serial.println(F("motor_to_percent() Start"));
    motor_stop();
    target_percent = set_target_percent;
    if (valve_open_percent == target_percent){
        // nothing to do
        return;
    }
    digitalWrite(YELLOW_LED, HIGH);

    if (valve_open_percent < target_percent){
        // Open more - Move DOWN
        direction = DOWN;
    } else if (valve_open_percent > target_percent) {
        // Close more - Move UP
        direction = UP;
    }
    Serial.print(F("Got direction:"));
    Serial.println(direction == UP? "UP":"DOWN");

    if (valve_open_percent - target_percent >7 || target_percent <= 30 || valve_open_percent <= 30){
      // below 30 percent it is hard to move the valve so MAX_SPEED
      raw_speed = MAX_SPEED;
    } else {
      raw_speed = MIN_SPEED;
    }
//    raw_speed = MIN_SPEED;
    isMotorRunning = true;
    Serial.println(F("motor_to_percent() Finish"));
}

void motor_run(){
    static uint32_t motor_run_tmr;
    if (millis() - motor_run_tmr > 500) {
        motor_run_tmr = millis();
        Serial.println(F("motor_run() Start"));
        if (!isMotorRunning) {
            Serial.println(F("return motor_run() - isMotorRunning == false"));
            return;
        }
        Serial.print(F("motor_run() isMontorRunning: "));
        Serial.println(isMotorRunning);

        Serial.print(F("motor_run() B"));
        if (is_target_percent_reached(target_percent)) {

            Serial.println(F("Stopping motor"));
            Serial.print("Target percent: " + String(target_percent) + "Valve percent:" + String(valve_open_percent));

            motor_stop();
            digitalWrite(YELLOW_LED, LOW);
            isMotorRunning = false;
        } else {
            String action = direction == DOWN ? "Opening to " : "Closing to ";
            lcd_line_center(2, action + String(target_percent) + "%");
            if (is_need_to_accelerate(target_percent)) {
                raw_speed = min(MAX_SPEED, raw_speed + 5); //accelerate
                Serial.println(F("Accelerating..."));
            } else {
                raw_speed = max(MIN_SPEED, raw_speed - 5);  // decelerate
                Serial.println(F("Decelerating..."));
            }
            digitalWrite(MOTOR_ENABLE_PIN, direction);
            byte speed = raw_speed;  // if direction == DOWN
            if (direction == DOWN) {
                speed = 255 - raw_speed;
            }
            analogWrite(MOTOR_SPEED_PIN, speed);   //PWM Speed Control
            update_pot_pos();
//            for (byte i = 0; i < 10; i++) {
//                update_pot_pos();
//                delay(10);
//            }
            show_stats();
        }
    }
}

void motor_up(){
    String mode_name = MOTOR_MODE_NAMES[cur_motor_mode].substring(0, 1);
    lcd_line(2, mode_name + ">Close-> UP");
    byte rounded_valve_open_percent = int(round(float(valve_open_percent) / 5.0)) * 5;  // let's round percents to 5%
    byte target_percent = max(VALVE_MIN_OPEN_PERCENT, rounded_valve_open_percent - 5);
    motor_to_percent(target_percent);
    lcd_line(2, mode_name + ">Close-> UP OK");
}

void motor_down(){
    String mode_name = MOTOR_MODE_NAMES[cur_motor_mode].substring(0, 1);
    lcd_line(2, mode_name + ">Open-> DOWN");
    byte rounded_valve_open_percent = int(round(float(valve_open_percent) / 5.0)) * 5;  // let's round percents to 5%
    byte target_percent = min(VALVE_MAX_OPEN_PERCENT, rounded_valve_open_percent + 5);
    motor_to_percent(target_percent);
    lcd_line(2, mode_name + ">Open-> DOWN OK");
}

void motor_close(){
    String mode_name = MOTOR_MODE_NAMES[cur_motor_mode].substring(0, 1);
    lcd_line(2, mode_name + ">Full Close");
    motor_to_percent(VALVE_MIN_OPEN_PERCENT);
//    while(valve_open_percent >= VALVE_MIN_OPEN_PERCENT){
//        motor_go(UP, 100); // Close
//    }
    lcd_line(2, mode_name + ">Full Close OK");
}

void motor_open(){
    String mode_name = MOTOR_MODE_NAMES[cur_motor_mode].substring(0, 1);
    lcd_line(2, mode_name + ">Full Open");
    motor_to_percent(VALVE_MAX_OPEN_PERCENT);
//    while(valve_open_percent < VALVE_MAX_OPEN_PERCENT){
//        motor_go(DOWN, 100); // Open
//    }
    lcd_line(2, mode_name + ">Full Open OK");
}

void auto_motor(bool direction){
    static uint32_t motor_move_tmr;
    if (millis() - motor_move_tmr > MOTOR_TIMEOUT){
        if (direction == UP){
            motor_up();
        } else {
            motor_down();
        }
        motor_move_tmr = millis();
    }
}

#endif //CHIMNEY_EXHAUST_SMOKE_CONTROLLER_MOTOR_MODULE_H
