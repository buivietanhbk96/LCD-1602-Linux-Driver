/*
 * Filename: lcd_driver.h
 * Date: 12/11/2019
 * Description: header file for driver
 * Author: VietAnh Bui
 */
#ifndef _LCD_DRIVER_H_
#define _LCD_DRIVER_H_

/******************LCD Pin Configuration******************/

#define LCD_RS_PIN_NUMBER 26
#define LCD_EN_PIN_NUMBER 19

#define LCD_D4_PIN_NUMBER 13
#define LCD_D5_PIN_NUMBER 6
#define LCD_D6_PIN_NUMBER 5
#define LCD_D7_PIN_NUMBER 11



/********************LCD Instruction**********************/
#define RS_COMMAND_MODE   0    /* Select Instruction Register with RS pin*/
#define RS_DATA_MODE      1    /* Select Data Register with RS pin*/

#define LCD_FIRST_LINE    1
#define LCD_SECOND_LINE   2
#define NUM_CHAR_PER_LINE 16

/**********************GPIO Support ***********************/

typedef enum pin_dir
{
	INPUT_PIN  = 0,
	OUTPUT_PIN = 1
} PIN_DIRECTION;

/******************Function Prototype**********************/

static int  lcd_pin_setup(unsigned int pin_number);
static void lcd_pin_setup_All( void );
static void lcd_pin_release(unsigned int pin_number);
static void lcd_pin_release_All( void );


static void lcd_send_command(char command);
static void lcd_send_data(char data);
static void lcd_initialize(void);
static int  lcd_print(char * msg, unsigned int lineNumber);
static void lcd_clearDisplay(void);
static void lcd_setLinePosition(unsigned int line);
/*
static void lcd_print_WithPosition(char * msg, unsigned int lineNumber, unsigned int nthCharacter);
static void lcd_setPosition(unsigned int line, unsigned int nthCharacter);


static void lcd_cursor_on(void);
static void lcd_cursor_off(void);
*/
#endif /*_LCD_DRIVER_H_*/
