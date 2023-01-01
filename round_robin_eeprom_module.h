//
// Created by mona on 14.10.22.
//

#ifndef CHIMNEY_EXHAUST_SMOKE_CONTROLLER_ROUND_ROBIN_EEPROM_MODULE_H
#define CHIMNEY_EXHAUST_SMOKE_CONTROLLER_ROUND_ROBIN_EEPROM_MODULE_H

struct HourStat{
    int temp;
};

const unsigned int ROUND_ROBIN_OFFSET = 200;
const unsigned int ROUND_ROBIN_SIZE = 240;
//const unsigned int ROUND_ROBIN_SIZE = 25;

#define MAX_HOURLY_TEMP_LIST_SIZE 24

HourStat hourly_temp_list[MAX_HOURLY_TEMP_LIST_SIZE];
const int NO_TEMP = -300;
byte hourly_temp_list_idx;

unsigned int round_robin_cursor = 0;

const HourStat EMPTY_HOUR_STAT = {
        temp: NO_TEMP,
};

int temp_float_to_int(float temp_as_float){
    return int(round(temp_as_float * 10));
}

float temp_int_to_float(int temp_as_int){
    return float(temp_as_int) / 10.0;
}

String temp_int_to_str(int temp_as_int){
//    return String(temp_as_int);
   String sign = temp_as_int < 0 ? "-":"";
   return sign + String(abs(temp_as_int)/10) + "." + String(abs(temp_as_int) % 10);
}

String temp_float_to_str(float temp_as_float){
   return String(temp_as_float);
}

void round_robin_write(int temp){
    HourStat hourStat = {
            temp: temp,
    };
    hourly_temp_list[hourly_temp_list_idx] = hourStat;
    hourly_temp_list_idx = (hourly_temp_list_idx + 1) % MAX_HOURLY_TEMP_LIST_SIZE;

   byte next_cursor = (round_robin_cursor + 1) % ROUND_ROBIN_SIZE;
   EEPROM.put(ROUND_ROBIN_OFFSET + next_cursor * sizeof(hourStat), EMPTY_HOUR_STAT);
   EEPROM.put(ROUND_ROBIN_OFFSET + round_robin_cursor * sizeof(hourStat), hourStat);
   round_robin_cursor = next_cursor;
}

void round_robin_read_last_24_hours(){
    unsigned int cursor = (ROUND_ROBIN_SIZE + round_robin_cursor - 24 - 1) % ROUND_ROBIN_SIZE;
    HourStat hourStat;
    for (byte hour=0; hour<24; hour++){
        cursor = (cursor + 1) % ROUND_ROBIN_SIZE;
        EEPROM.get(ROUND_ROBIN_OFFSET + cursor * sizeof(hourStat), hourly_temp_list[hour]);
    }
    hourly_temp_list_idx = 0;  // After we read the whole list put the idx back to 0
}

void round_robin_init(){
    Serial.println(F("Round_robin_init() Start"));
    HourStat hourStat;
    for (byte i=0; i<ROUND_ROBIN_SIZE; i++){
        EEPROM.get(ROUND_ROBIN_OFFSET + i * sizeof(hourStat), hourStat);
        if (hourStat.temp == NO_TEMP){
            round_robin_cursor = i;
            break;
        }
    }
    Serial.println(F("Round_robin_init() Middle"));
    round_robin_read_last_24_hours();
    Serial.println(F("Round_robin_init() Complete"));
}

#endif //CHIMNEY_EXHAUST_SMOKE_CONTROLLER_ROUND_ROBIN_EEPROM_MODULE_H
