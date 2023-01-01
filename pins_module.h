//
// Created by Kos Ovechko on 12.10.22.
//

#ifndef CHIMNEY_EXHAUST_SMOKE_CONTROLLER_PINS_MODULE_H
#define CHIMNEY_EXHAUST_SMOKE_CONTROLLER_PINS_MODULE_H

#define ENCODER_LEFT       2
#define ENCODER_BTN        3
#define ENCODER_RIGHT      4

#define BLACK_BTN          5
#define BLUETOOTH_TX       6
#define BLUETOOTH_RX       7

#define DS1820_DATA        9
#define MOTOR_SPEED_PIN    10
#define MOTOR_ENABLE_PIN   11

#define EXHAUST_VALVE_POT  A0
#define RED_LED            A1
#define GREEN_LED          A2
#define BLUE_LED           A3
#define YELLOW_LED         13

bool isMenuActive = false;

#endif //CHIMNEY_EXHAUST_SMOKE_CONTROLLER_PINS_MODULE_H
