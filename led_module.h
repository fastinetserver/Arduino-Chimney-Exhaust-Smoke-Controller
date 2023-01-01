//
// Created by Kos Ovechko on 12.10.22.
//

#ifndef CHIMNEY_EXHAUST_SMOKE_CONTROLLER_LED_MODULE_H
#define CHIMNEY_EXHAUST_SMOKE_CONTROLLER_LED_MODULE_H

#include "pins_module.h"
#include "lcd_module.h"

void setup_leds(){
    pinMode(RED_LED, OUTPUT);
    digitalWrite(RED_LED, LOW);
    pinMode(GREEN_LED, OUTPUT);
    digitalWrite(GREEN_LED, LOW);
    pinMode(BLUE_LED, OUTPUT);
    digitalWrite(BLUE_LED, LOW);
    pinMode(YELLOW_LED, OUTPUT);
    digitalWrite(YELLOW_LED, LOW);
}

void blink(byte LED, byte times){
    Serial.print(F("blink:"));
    Serial.println(LED);
    for (byte t=0; t<times; t++){
        digitalWrite(LED, HIGH);
        delay(30);
        digitalWrite(LED, LOW);
        delay(30);
    }
}

void leds_startup_blinking(){
    lcd_line(1, F("Blinking..."));
    blink(RED_LED, 3);
    blink(GREEN_LED, 3);
    blink(BLUE_LED, 3);
    blink(YELLOW_LED, 3);
}

#endif //CHIMNEY_EXHAUST_SMOKE_CONTROLLER_LED_MODULE_H
