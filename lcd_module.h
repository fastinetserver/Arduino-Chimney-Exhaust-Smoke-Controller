//
// Created by Kos Ovechko on 12.10.22.
//

#ifndef CHIMNEY_EXHAUST_SMOKE_CONTROLLER_LCD_MODULE_H
#define CHIMNEY_EXHAUST_SMOKE_CONTROLLER_LCD_MODULE_H


#include <LiquidCrystal_I2C.h>

uint32_t SCREEN_TIMEOUT_MILLISECS=180000;

bool screen_backlight_on = false;
const String SPACE = "                ";
String lcd_lines[3] = {"", ""};

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display
//LiquidCrystal_I2C lcd(0x3F,16,2);

void lcd_line(byte line_num, String raw_text){
    String text = raw_text + SPACE;
    lcd_lines[line_num] = text.substring(0, 16);
    lcd.setCursor(0, line_num);
    lcd.print(lcd_lines[line_num]);
}

void lcd_line_center(byte line_num, String text){
    byte indent_length = max(0, (16 - text.length())/2);
    text = SPACE.substring(0, indent_length) + text;
    if (line_num < 2){
      lcd_line(line_num, text);
    } else {
      lcd_lines[line_num] = text.substring(0, 16);
    }
}

void lcd_show_3rd_line(){
   lcd_line(1, lcd_lines[2]);
}

void turn_lcd_backlight_on(){
    if (not(screen_backlight_on)){
        screen_backlight_on = true;
        lcd.backlight();
        return;
    }
}

//void turn_lcd_on(){
//  if (!screen_backlight_on){
//    screen_backlight_on = true;
//    lcd.backlight();
//    is_editing = false;
//    myEnc.write(0);
//  }
//}

#endif //CHIMNEY_EXHAUST_SMOKE_CONTROLLER_LCD_MODULE_H
