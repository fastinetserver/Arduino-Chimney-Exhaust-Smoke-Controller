//
// Created by Kos Ovechko on 12.10.22.
//

#ifndef CHIMNEY_EXHAUST_SMOKE_CONTROLLER_BLUETOOTH_MODULE_H
#define CHIMNEY_EXHAUST_SMOKE_CONTROLLER_BLUETOOTH_MODULE_H

#endif //CHIMNEY_EXHAUST_SMOKE_CONTROLLER_BLUETOOTH_MODULE_H

#include "Arduino.h"
#include <SoftwareSerial.h>
#include "pins_module.h"
#include "motor_module.h"
#include "encoder_ui_module.h"

SoftwareSerial BTSerial(BLUETOOTH_RX, BLUETOOTH_TX); // RX TX

String messageBuffer = "";
String message = "";
//settemp-30, 35, 40, 45, 50, 55, 60, 65, 70, auto-hi-temp-clean, manual-force-close, manual-force-open, auto-mode, manual-up, manual-down

void setup_bluetooth(){
    pinMode(BLUETOOTH_RX, INPUT);
    pinMode(BLUETOOTH_TX, OUTPUT);
    BTSerial.begin(9600);
}

void read_bluetooth(){
//  Serial.println("reading BT");
  String result = "";
//  if (BTSerial.available() > 0){
//      Serial.println("=== Have BT data");
//  }else{
//      Serial.println("--- NO BT data");
//  }
  while (BTSerial.available() > 0) {
    char data = (char) BTSerial.read();
    if (data == '\n' || data == '\r'){
      if (messageBuffer.length() > 0) {
        messageBuffer += data;
      }
    } else if (data!= ';') {   // skip separator
      messageBuffer += data;
    }
    if (data == ';'){
      message = messageBuffer;
      messageBuffer = "";
      if (message == "auto") {
          result = message;
          lcd_line_center(1, result);
          Serial.println(result);
          cur_motor_mode = MOTOR_MODE_AUTO;
      } else if (message == "clean") {
          result = message;
          lcd_line_center(1, result);
          Serial.println(result);
          cur_motor_mode = MOTOR_MODE_CLEAN;
      } else if (message == "info") {
          BTSerial.println("=====Info:====="); // send back to bluetooth terminal
          BTSerial.println(lcd_lines[0]); // send back to bluetooth terminal
          BTSerial.println(lcd_lines[1]); // send back to bluetooth terminal
          BTSerial.println("Mode: " + MOTOR_MODE_NAMES[cur_motor_mode]); // send back to bluetooth terminal
          result = message;
          lcd_line_center(1, result);
          Serial.println(result);
      } else if (message == "stats") {
          float daily_sum = 0;
          int daily_avg = 0;
          byte stats_count = 0;
          BTSerial.println("=====Hourly AVG Temperature:====="); // send back to bluetooth terminal
          for (byte i=0; i<MAX_HOURLY_TEMP_LIST_SIZE; i++){
              byte idx = (hourly_temp_list_idx + i) % MAX_HOURLY_TEMP_LIST_SIZE;
              if (hourly_temp_list[idx].temp != NO_TEMP){
                  BTSerial.println(String(24-i)+") "+String(temp_int_to_str(hourly_temp_list[idx].temp))); // send back to bluetooth terminal
                  daily_sum = daily_sum + hourly_temp_list[idx].temp;
                  stats_count++;
                  daily_avg = daily_sum / stats_count;
              }
          };
          BTSerial.println("Daily AVG Temperature: " + temp_int_to_str(daily_avg)); // send back to bluetooth terminal
          BTSerial.println("=====Last Hour 10mins Samples Temperature:====="); // send back to bluetooth terminal

          String last_hour_temp_samples_str = "";
          for (byte i=0; i<MAX_TEN_MINS_TEMP_LIST_SIZE; i++){
              int temp_to_show = ten_mins_temp_list[(ten_mins_temp_list_idx + i) % MAX_TEN_MINS_TEMP_LIST_SIZE];
              if (temp_to_show != NO_TEMP){
                  last_hour_temp_samples_str = last_hour_temp_samples_str + temp_int_to_str(temp_to_show) + "C, ";
              } else {
                  last_hour_temp_samples_str = last_hour_temp_samples_str + "N/A, ";
              }
          };
          BTSerial.println(last_hour_temp_samples_str); // send back to bluetooth terminal
          BTSerial.println(""); // send back to bluetooth terminal
          result = message;
          lcd_line_center(1, result);
          Serial.println(result);
      } else if (message == "close") {
          result = message;
          lcd_line_center(1, result);
          Serial.println(result);
          cur_motor_mode = MOTOR_MODE_MANUAL;
          motor_close();
      } else if (message == "open") {
          result = message;
          lcd_line_center(1, result);
          Serial.println(result);
          cur_motor_mode = MOTOR_MODE_MANUAL;
          motor_open();
      } else if (message == "up") {
          result = message;
          lcd_line_center(1, result);
          Serial.println(result);
          cur_motor_mode = MOTOR_MODE_MANUAL;
          motor_up();
      } else if (message == "down") {
          result = message;
          lcd_line_center(1, result);
          Serial.println(result);
          cur_motor_mode = MOTOR_MODE_MANUAL;
          motor_down();
      } else if (message.substring(0, 4) == "temp"){
          String temp_str = message.substring(4);
          int temp = temp_str.toInt();
          set_temp(temp);
      } else if (message.substring(0, 4) == "open"){
          String percent_str = message.substring(4);
          int target_percent = percent_str.toInt();
          target_percent = min(VALVE_MAX_OPEN_PERCENT, max(VALVE_MIN_OPEN_PERCENT, target_percent));  // Impose boundaries
          String result = "Set Open=" + String(target_percent) +"%";
          cur_motor_mode = MOTOR_MODE_MANUAL;
          lcd_line_center(1, result);
          Serial.println(result);
          motor_to_percent(target_percent);
      } else {
          result = F("Unknown command");
          lcd_line(1, result);
          Serial.println(result);
      }
      Serial.print(message); // send to serial monitor
      BTSerial.print("You sent " + message); // send back to bluetooth terminal
    }
  }
//  Serial.println("Reading BT DONE");
}