/*************************************************************************
 * @file edugrid_lcd.h
 * @date 2023/11/15
 *
 ************************************************************************/

#ifndef EDUGRID_LCD_H_
#define EDUGRID_LCD_H_

/*************************************************************************
 * Include
 ************************************************************************/
#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <edugrid_states.h>

/*************************************************************************
 * Define
 ************************************************************************/
#define LCD_I2C_ADDRESS (0x27)
#define DIGITS_PER_ROW (20)
#define ROWS (4)

/*************************************************************************
 * Class
 ************************************************************************/

/** 
 * Class with static members for all LCD panel tasks
*/
class edugrid_lcd
{
private:
    static LiquidCrystal_I2C lcd;
    static String lines_content[ROWS];
    static bool lcd_inited;
public:
    static void lcdInit ();
    static void lcdClear();
    static void lcdWriteText (int digit, int row, String text);
    static bool getLcdInitState();
protected:
};

#endif /* EDUGRID_LCD_H_ */