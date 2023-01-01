//
// Created by Kos Ovechko on 12.10.22.
//

#ifndef CHIMNEY_EXHAUST_SMOKE_CONTROLLER_ENCODER_UI_MODULE_H
#define CHIMNEY_EXHAUST_SMOKE_CONTROLLER_ENCODER_UI_MODULE_H

#include <Arduino.h>
#include <Encoder.h>
#include "GyverButton.h"
#include "motor_module.h"
#include "pins_module.h"
#include "settings_module.h"


uint32_t EDIT_TIMEOUT = SECOND*30;
uint32_t SCREEN_TIMEOUT = MINUTE*3;
uint32_t menu_active_timeout;

const byte DIRECTION_DEC_FAST = 0;
const byte DIRECTION_DEC = 1;
const byte DIRECTION_NONE = 2;
const byte DIRECTION_INC = 3;
const byte DIRECTION_INC_FAST = 4;

const byte MENU_OPEN = 10;
const byte MENU_OPEN_25 = 11;
const byte MENU_OPEN_30 = 12;
const byte MENU_OPEN_40 = 13;
const byte MENU_OPEN_50 = 14;
const byte MENU_OPEN_60 = 15;
const byte MENU_OPEN_70 = 16;
const byte MENU_OPEN_80 = 17;
const byte MENU_OPEN_90 = 18;
const byte MENU_OPEN_100 = 19;

const byte MENU_SET_TEMP = 20;
const byte MENU_SET_TEMP_30 = 21;
const byte MENU_SET_TEMP_35 = 22;
const byte MENU_SET_TEMP_40 = 23;
const byte MENU_SET_TEMP_45 = 24;
const byte MENU_SET_TEMP_50 = 25;
const byte MENU_SET_TEMP_55 = 26;
const byte MENU_SET_TEMP_60 = 27;
const byte MENU_SET_TEMP_65 = 28;
const byte MENU_SET_TEMP_70 = 29;

const byte MENU_CLEAN = 30;
//const unsigned int MENU_SET_AUTO = 40;
//const unsigned int MENU_SHOW_STATS = 50;

byte min_menu = MENU_OPEN;
byte max_menu = MENU_CLEAN;

Encoder myEnc(ENCODER_LEFT, ENCODER_RIGHT);

const long ENCODER_MIN_POS = 10000;
const long ENCODER_MIDDLE_POS = 30000;
const long ENCODER_MAX_POS = 50000;

byte cur_menu = MENU_OPEN;
byte menu_increment = 10;

GButton escBtn(BLACK_BTN, HIGH_PULL, NORM_OPEN);
GButton encoderBtn(ENCODER_BTN, HIGH_PULL, NORM_OPEN);

void set_temp(int raw_temp){  // Should be int here until we Impose boundaries
    byte temp = min(MAX_GOAL_TEMP, max(MIN_GOAL_TEMP, raw_temp));  // Impose boundaries
    settings.goal_temp = temp;
    String result = "Setting T=" + String(settings.goal_temp);
    show_stats();
    cur_motor_mode = MOTOR_MODE_AUTO;
    lcd_line_center(1, result);
    Serial.println(result);
    delay(2*SECOND);  // Delay to prevent EEPROM Wear out
    write_settings();
}

static void move_to_main_menu() {
    lcd_line(1, "");
    Serial.println(F("Esc Clicked!"));
    menu_increment = 10;
    cur_menu = int(cur_menu/10)*10;
    min_menu = MENU_OPEN;
    max_menu = MENU_CLEAN;
}

static void menu_exit(){
    move_to_main_menu();
    isMenuActive = false;
}

void menuId(byte cur_menu, byte value){
    encoderBtn.tick();
    escBtn.tick();
    byte id = int(cur_menu/10);
    String suffix = "";
    switch (id*10){
        case MENU_OPEN:
            if (cur_menu % 10 > 0) {
                suffix = " " + String(value) + "%";
                if (escBtn.isClick()){
                    move_to_main_menu();
                    break;
                }
                if (encoderBtn.isClick()) {
                    cur_menu = MENU_OPEN;
                    lcd_line(1, String(id) + ") Open" + suffix);
                    bool isMenuActive = false;
                    cur_motor_mode = MOTOR_MODE_MANUAL;
                    motor_to_percent(value);
                    menu_exit();
                    break;
                }
            } else if (escBtn.isClick()){
                menu_exit();
                break;
            } else if (encoderBtn.isClick()){
                menu_increment = 1;
                min_menu = MENU_OPEN_25;
                max_menu = MENU_OPEN_100;
            }
            lcd_line(1, String(id) + ") Open" + suffix);
            break;
        case MENU_SET_TEMP:
            if (cur_menu % 10 > 0) {
                suffix = " " + String(value) + "C";
                if (escBtn.isClick()){
                    move_to_main_menu();
                    break;
                }
                if (encoderBtn.isClick()) {
                    cur_menu = MENU_OPEN;
                    lcd_line(1, String(id) + ") Set Temp" + suffix);
                    bool isMenuActive = false;
                    set_temp(value);
                    menu_exit();
                    break;
                }
            } else if (escBtn.isClick()){
                menu_exit();
                break;
            } else if (encoderBtn.isClick()){
                menu_increment = 1;
                min_menu = MENU_SET_TEMP_30;
                max_menu = MENU_SET_TEMP_70;
            }
            lcd_line(1, String(id) + ") Set Temp" + suffix);
            break;
        case MENU_CLEAN:
            lcd_line(1, String(id) + ") Clean");
            break;
    }
}

setup_ui(){
    myEnc.write(ENCODER_MIDDLE_POS);
    escBtn.setDebounce(50);
    encoderBtn.setDebounce(50);
}

byte getEncoderDirection(){
    static long prev_encoder_pos = ENCODER_MIDDLE_POS;
    long encoder_pos = myEnc.read();

    if (encoder_pos < ENCODER_MIN_POS || encoder_pos > ENCODER_MAX_POS){
        encoder_pos = ENCODER_MIDDLE_POS;
        myEnc.write(encoder_pos);
//        Serial.println("encoder_pos: "+String(encoder_pos) +" Fixed");
    }
    int result = DIRECTION_NONE;
    static uint32_t encoder_tmr;
    if (millis() - encoder_tmr > 250) {
        encoder_tmr = millis();
        if (encoder_pos > prev_encoder_pos) {
            if (isMenuActive) result = DIRECTION_DEC;
            isMenuActive = true;
            menu_active_timeout = millis();
//            Serial.println("EncDirection: DEC");
        } else if (encoder_pos < prev_encoder_pos) {
            if (isMenuActive) result = DIRECTION_INC;
            isMenuActive = true;
            menu_active_timeout = millis();
//            Serial.println("EncDirection: INC");
        }
    }
    prev_encoder_pos = encoder_pos;
    return result;
}

void encoder_read_menu(){
    byte encoderDirection = getEncoderDirection();
    switch (encoderDirection) {
        case DIRECTION_DEC:
            cur_menu = cur_menu - menu_increment;
            break;
        case DIRECTION_INC:
            cur_menu = cur_menu + menu_increment;
            break;
        case DIRECTION_NONE:
            break;
    }
    cur_menu = max(min_menu, min(cur_menu, max_menu)); // Set menu boundaries;
//    Serial.println("cur_menu:" + String(cur_menu) + "  Active: "+ String(isMenuActive));
}

void run_encoder_ui(){
    encoder_read_menu();

//    if (millis() - menu_active_timeout > SECOND*30) {
//        menu_active_timeout = millis();
//        isMenuActive = false;
//    }

    uint32_t stats_tmr;
    if (millis() - stats_tmr > SECOND*10) {
        stats_tmr = millis();
        show_stats();
    }
    if (isMenuActive){
        switch (cur_menu){
            case MENU_OPEN:
                menuId(cur_menu, 40);  // value 40 will be disregarded but let's place some safe value here anyway
                break;
            case MENU_OPEN_25:
                menuId(cur_menu, 25);
                break;
            case MENU_OPEN_30:
                menuId(cur_menu, 30);
                break;
            case MENU_OPEN_40:
                menuId(cur_menu, 40);
                break;
            case MENU_OPEN_50:
                menuId(cur_menu, 50);
                break;
            case MENU_OPEN_60:
                menuId(cur_menu, 60);
                break;
            case MENU_OPEN_70:
                menuId(cur_menu, 70);
                break;
            case MENU_OPEN_80:
                menuId(cur_menu, 80);
                break;
            case MENU_OPEN_90:
                menuId(cur_menu, 90);
                break;
            case MENU_OPEN_100:
                menuId(cur_menu, 100);
                break;

            case MENU_SET_TEMP:
                menuId(cur_menu, 30);  // value 30 will be disregarded but let's place some safe value here anyway
                break;
            case MENU_SET_TEMP_30:
                menuId(cur_menu, 30);
                break;
            case MENU_SET_TEMP_35:
                menuId(cur_menu, 35);
                break;
            case MENU_SET_TEMP_40:
                menuId(cur_menu, 40);
                break;
            case MENU_SET_TEMP_45:
                menuId(cur_menu, 45);
                break;
            case MENU_SET_TEMP_50:
                menuId(cur_menu, 50);
                break;
            case MENU_SET_TEMP_55:
                menuId(cur_menu, 55);
                break;
            case MENU_SET_TEMP_60:
                menuId(cur_menu, 60);
                break;
            case MENU_SET_TEMP_65:
                menuId(cur_menu, 65);
                break;
            case MENU_SET_TEMP_70:
                menuId(cur_menu, 70);
                break;

            case MENU_CLEAN:
                menuId(cur_menu, 30);  // value 30 will be disregarded but let's place some safe value here anyway
                break;
        }
    } else {
        lcd_show_3rd_line();
    }
}

#endif //CHIMNEY_EXHAUST_SMOKE_CONTROLLER_ENCODER_UI_MODULE_H
