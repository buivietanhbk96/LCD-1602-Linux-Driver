/*
 * Filename: lcd_driver.h
 * Date: 12/11/2019
 * Description: header file for driver
 * Author: VietAnh Bui
 */
#ifndef _LCD_DRIVER_H_
#define _LCD_DRIVER_H_


#define ENABLE 1
#define DISABLE 0
/******************LCD Command****************************/
#define LCD_CLEARDISPLAY   0x01
#define LCD_RETURNHOME     0x02
#define LCD_ENTRYMODESET   0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_SHIFT    	   0x10
#define LCD_FUNCTIONSET    0x20
#define LCD_SETCGRAMADDR   0x40
#define LCD_SETDDRAMADDR   0x80

#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE  0x00
#define LCD_MOVERIGHT   0x04
#define LCD_MOVELEFT    0x00

#define LCD_DISPLAYON  0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON   0x02
#define LCD_CURSOROFF  0x00
#define LCD_BLINKON    0x01
#define LCD_BLINKOFF   0x00
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

/******************Define IOCTL Command ********************/
#define MAGICAL_NUMBER 248
#define LCD_CLEAR  					 _IO(MAGICAL_NUMBER, 0)
#define LCD_GOTOXY 					_IOW(MAGICAL_NUMBER, 1, axis_t *)
#define LCD_SET_DISPLAY  			_IOW(MAGICAL_NUMBER, 2, display_control_t *)
#define LCD_PUT_CHAR  				_IOW(MAGICAL_NUMBER, 3, unsigned char *)
#define LCD_SCROLL_LEFT  			 _IO(MAGICAL_NUMBER, 4)
#define LCD_SCROLL_RIGHT  			 _IO(MAGICAL_NUMBER, 5)
#define LCD_UPLOAD_CUSTOM_CHAR      _IOW(MAGICAL_NUMBER, 6, custom_char_t *)
#define LCD_INIT      				 _IO(MAGICAL_NUMBER, 7)

/**********************Data Structure***********************/
typedef struct custom_chr 
{
    unsigned char location;
    unsigned char *charmap;
}custom_char_t;

typedef struct axis 
{
	unsigned char x;
	unsigned char y;
}axis_t;

typedef struct display_control 
{
	unsigned char display;
	unsigned char cursor;
	unsigned char blink;
}display_control_t;

/******************Function Prototype**********************/

static int  lcd_pin_setup(unsigned int pin_number);
static void lcd_pin_setup_All( void );
static void lcd_pin_release(unsigned int pin_number);
static void lcd_pin_release_All( void );

static void lcd_nibble(char data);
static void lcd_send_command(char command);
static void lcd_send_data(char data);
static void lcd_initialize(void);
static int  lcd_print(char * msg, unsigned int lineNumber);
static void lcd_clearDisplay(void);
static void lcd_setLine(unsigned int line);

/* Set cursor goto line x, column y*/
static void lcd_gotoxy(axis_t *position);

/* Set ON/OFF Display/Cursor/Blink*/
static void lcd_set_display(display_control_t *display_option); 
/* Scroll to left*/
static void lcd_scroll_left(void);
/* Scroll to right*/
static void lcd_scroll_right(void);
/* Upload custom character to CGRAM ((Character Generator RAM) */
/* Only have 8 byte for custom character, location 0-7 */
static void lcd_create_char(custom_char_t *custom_chr);
#endif /*_LCD_DRIVER_H_*/
