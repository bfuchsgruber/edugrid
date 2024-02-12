/*************************************************************************
 * @file edugrid_lcd.cpp
 * @date 2023/11/15
 *
 ************************************************************************/

/*************************************************************************
 * Include
 ************************************************************************/
#include <edugrid_lcd.h>

/*************************************************************************
 * Define
 ************************************************************************/

/*************************************************************************
 * Variable Definition
 ************************************************************************/
LiquidCrystal_I2C edugrid_lcd::lcd(LCD_I2C_ADDRESS, DIGITS_PER_ROW, ROWS);
String edugrid_lcd::lines_content[ROWS] = {""};
bool edugrid_lcd::lcd_inited = false;

/*************************************************************************
 * Function Definition
 ************************************************************************/


/** Start communication with LCD
 * 
 */
void edugrid_lcd::lcdInit()
{
    lcd.init();
    lcd.backlight();

    /* Write static text */
    lcdWriteText(0, 0, "PWM  ");
    lcdWriteText(0, 1, "IN   ");
    lcdWriteText(0, 2, "OUT  ");
    lcdWriteText(0, 3, "MPPT ");
    
    lcd_inited = true;

    Serial.println("| OK | LCD panel init");
}

/** Erase complete text on lcd
 * 
*/
void edugrid_lcd::lcdClear()
{
    lcd.clear();
}

/** Write a string to the lcd panel
 * @param digit Position where text starts
 * @param row Row of the lcd panel; row = [0;ROWS-1]
 * @param text String text to display *
 */
void edugrid_lcd::lcdWriteText(int digit, int row, String text) /* row = [0;ROWS-1] */
{
    if (!text.equals(lines_content[row])) /* text is new */
    {
        lines_content[row] = text; // store new text to memory
        lcd.setCursor(digit, row);
        // lcd.print("                    "); // clear line from digit
        lcd.print(text);
    }
    else /* text is already displayed */
    {
        /*Nothing to do here! */
    }
}

/** Get init state LCD 
 * @return bool true/false
*/
bool edugrid_lcd::getLcdInitState()
{
    return lcd_inited;
}