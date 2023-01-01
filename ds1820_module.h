//
// Created by Kos Ovechko on 12.10.22.
//

#ifndef CHIMNEY_EXHAUST_SMOKE_CONTROLLER_DS1820_MODULE_H
#define CHIMNEY_EXHAUST_SMOKE_CONTROLLER_DS1820_MODULE_H

#include <OneWire.h>
#include <DallasTemperature.h>
#include "pins_module.h"
#include "time_module.h"
#include "round_robin_eeprom_module.h"

const float DEFAULT_GOAL_TEMP = 30.0;
const float MIN_GOAL_TEMP = 30;
const float MAX_GOAL_TEMP = 80;
const float TEMP_THRESHOLD = 2.00;

#define MAX_TEMP_LIST_SIZE 5
#define MAX_TEN_MINS_TEMP_LIST_SIZE 6   // for 1 hour we need to make 6 records with 10 mins interval

byte ten_mins_temp_list_idx = 0;

int ten_mins_temp_list[MAX_TEN_MINS_TEMP_LIST_SIZE];

float celsius = NO_TEMP;

void setup_temp_module(){
    for (byte i = 0; i < MAX_TEN_MINS_TEMP_LIST_SIZE; i++){
        ten_mins_temp_list[i] = NO_TEMP;
    }
//    memset(ten_mins_temp_list, NO_TEMP, sizeof(ten_mins_temp_list));
    round_robin_init();
}

float read_raw_celsius(){
    OneWire oneWire(DS1820_DATA);
    DallasTemperature sensors(&oneWire);

    sensors.requestTemperatures();
    float celsius_raw = sensors.getTempCByIndex(0);

//    Serial.println("Celsius_raw: " + String(celsius_raw));
    return celsius_raw;
}

float read_celsius_avg(){
    static bool temp_list_initialized = false;
    static float temp_list[MAX_TEMP_LIST_SIZE];
    if (!temp_list_initialized){
        memset(temp_list, NO_TEMP, sizeof(temp_list));
        temp_list_initialized = true;
    }
    static byte celsius_list_idx;
    float temp = read_raw_celsius();
    float sum = 0;
    float avg = 0;
    if (temp > -1){
        // -127 is something wrong with the readings so we skip everything below -50 C
        temp_list[celsius_list_idx] = temp;
        celsius_list_idx = (celsius_list_idx + 1) % MAX_TEMP_LIST_SIZE;
    }
    for (byte i=0; i<MAX_TEMP_LIST_SIZE; i++){
        if (temp_list[i] > -1){
            sum = sum + temp_list[i];
            avg = sum / (i+1);
        }else{
            celsius = avg;
            return avg;  // If list is NOT full - we will produce a premature return
        }
    };
    celsius = avg;
    return avg;
};

int update_ten_mins_celsius_avg(float raw_temp){
    // Updates samples of 10 mins celsius avg and returns current HOURLY avg
    ten_mins_temp_list[ten_mins_temp_list_idx] = temp_float_to_int(raw_temp);;
    ten_mins_temp_list_idx = (ten_mins_temp_list_idx + 1) % MAX_TEN_MINS_TEMP_LIST_SIZE;

    long hourly_sum = 0;
    int hourly_avg = 0;
    byte records_cnt = 0;
    for (byte i=0; i<MAX_TEN_MINS_TEMP_LIST_SIZE; i++){
        if (ten_mins_temp_list[i] < 2500 && ten_mins_temp_list[i] != NO_TEMP ){  // 2500 stands for 250.0C, because multiplied by 10
            hourly_sum = hourly_sum + ten_mins_temp_list[i];
            records_cnt++;
        }
    };
    hourly_avg = hourly_sum / records_cnt;
    return hourly_avg;
}

int update_hourly_celsius_avg(int hourly_temp_to_add){
    // Updates samples of Hourly celsius avg and returns current Daily avg
    round_robin_write(hourly_temp_to_add);
    long daily_sum = 0;
    int daily_avg = 0;
    for (byte i=0; i<MAX_HOURLY_TEMP_LIST_SIZE; i++){
        if (hourly_temp_list[i].temp != NO_TEMP){
            daily_sum = daily_sum + hourly_temp_list[i].temp;
            daily_avg = daily_sum / (i+1);
        }else{
            return daily_avg;  // If list is NOT full - we will produce a premature return
        }
    };
    return daily_avg;
}

void update_all_temp_stats(){
    static uint32_t temp_tmr, ten_mins_temp_tmr, hourly_temp_tmr;
    if (millis() - temp_tmr > SECOND * 10){
        temp_tmr = millis();
        read_celsius_avg();
        if (millis() - ten_mins_temp_tmr > TEN_MINUTES){
            ten_mins_temp_tmr = millis();
            int hourly_avg = update_ten_mins_celsius_avg(celsius);
            if (millis() - hourly_temp_tmr > HOUR){
                hourly_temp_tmr = millis();
                update_hourly_celsius_avg(hourly_avg);
            }
        }
    }
}

#endif //CHIMNEY_EXHAUST_SMOKE_CONTROLLER_DS1820_MODULE_H
