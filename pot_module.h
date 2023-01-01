//
// Created by Kos Ovechko on 12.10.22.
//

#ifndef CHIMNEY_EXHAUST_SMOKE_CONTROLLER_POT_MODULE_H
#define CHIMNEY_EXHAUST_SMOKE_CONTROLLER_POT_MODULE_H
#define MAX_POT_LIST_SIZE 5

const byte VALVE_MAX_OPEN_PERCENT = 100;  // Open
const byte VALVE_MIN_OPEN_PERCENT = 22;   // Closed

bool isMotorRunning = false;
byte target_percent = 100;

unsigned int pot_pos = 60;

int valve_open_percent = 0;

void update_pot_pos(){
    static bool pot_pos_list_initialized = false;
    static byte pot_pos_list[MAX_POT_LIST_SIZE];
    if (!pot_pos_list_initialized){
        memset(pot_pos_list, -1, sizeof(pot_pos_list));
        pot_pos_list_initialized = true;
    }
    static byte pot_pos_list_idx;
    float sum = 0;
    float cnt = 0;
    pot_pos_list[pot_pos_list_idx] = byte(analogRead(EXHAUST_VALVE_POT) / 4);
    pot_pos_list_idx = (pot_pos_list_idx + 1) % MAX_POT_LIST_SIZE;
    for (byte i=0; i<MAX_POT_LIST_SIZE; i++){
        if (pot_pos_list[i] > -1){
            sum = sum + float(pot_pos_list[i]);
            cnt++;
        }
    };
    pot_pos = sum / cnt;
}

float get_pot_angle(){
// return (175-pot_pos)/0.81; // this one is depreciated, returns angle in degrees instead of radians
    // Returns pot angle in radians
    float normalized_pot_pos = min(175.0, pot_pos);
//    Serial.println("normalized_pot_pos:"+String(normalized_pot_pos));
//    Serial.println("radians:" + String(float(175-normalized_pot_pos)/45));
    return (175.0-normalized_pot_pos)/45.0;
}

int get_valve_open_percent(){
    static float cur_percent = 0;
    float new_percent = cos(get_pot_angle())*100.00;
    if (new_percent >= 99){
        new_percent = 100;  // Consider everything beyond 99% to be 100%
    }
    if (abs(cur_percent - new_percent) >= 0.5) {  // This is to stabilize constantly jumping values of my pot
        cur_percent = new_percent;
    }
    if (abs(cur_percent - float(target_percent))<1.9 and !isMotorRunning){
        cur_percent = target_percent;
    }
    return round(cur_percent);
}

#endif //CHIMNEY_EXHAUST_SMOKE_CONTROLLER_POT_MODULE_H
