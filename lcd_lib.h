/*
 * Filename: lcd_lib.h
 * Date: 14/11/2019
 * Description: header file for LCD library use LCD Driver
 * Author: VietAnh Bui
 */
#ifndef _LCD_LIB_H_
#define _LCD_LIB_H_
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <stdlib.h>

#define DEVICE_NODE "/dev/lcd_1602"
#define NUM_CHAR_PER_LINE 16
#define NUM_LINE 2
/*********************GPIO Configure************************/

/**********************Data Structure***********************/
typedef struct _custom_char 
{
    unsigned char location;
    unsigned char *charmap;
}custom_character_t;

typedef struct _coordinate 
{
	unsigned char x;
	unsigned char y;
}coordinate_t;

typedef struct _display_control 
{
	unsigned char display;
	unsigned char cursor;
	unsigned char blink;
}control_display_t;

/* Define command for IOCTL*/
#define MAGICAL_NUM 248
#define CLEAR_DISPLAY            _IO(MAGICAL_NUM, 0)
#define GOTO_XY                 _IOW(MAGICAL_NUM, 1, coordinate_t *)
#define SET_DISPLAY             _IOW(MAGICAL_NUM, 2, control_display_t *)
#define PUT_CHAR                _IOW(MAGICAL_NUM, 3, unsigned char *)
#define SCROLL_LEFT  			 _IO(MAGICAL_NUM, 4)
#define SCROLL_RIGHT  			 _IO(MAGICAL_NUM, 5)
#define UPLOAD_CUSTOM_CHAR      _IOW(MAGICAL_NUM, 6, custom_character_t *)
#define INIT_LCD                 _IO(MAGICAL_NUM, 7)


/**********************Function Prototype ***********************/

/**
 * @brief Init LCD
 * Set cursor position to (0, 0), set enable cursor, enable blink
 * The order of the connection pins has been defined in the driver 
 * The feature that provides configuration of connector pins will be updated in later versions
 * @param void
 * @return void
 */
void lcd_init(void);
/**
 * @brief Open device file of LCD 
 * @param void
 * @return file descriptor of device file
 */
static int lcd_open_dev(void);
/**
 * @brief Close device file of LCD 
 * @param fd file descriptor
 * @return close device file
 */
static void lcd_close_dev(int fd);
/**
 * @brief Clear LCD display
 * Clear screen of LCD and set cursor position to (1, 0)
 * @param void
 * @return void
 */
void lcd_clear_display(void);
/**
 * @brief Set cursor goto line x, column y (LCD 16x02)
 * @param x line number.   (1 <= x <= 2)
 * @param y column number. (0 <= y <= 15) 
 * @return void
 */
void lcd_goto_xy(unsigned char x, unsigned char y);
/**
 * @brief Control Display
 * Set option to control display LCD
 * @param display to enable/disable display LCD. (ENABLE or DISABLE)
 * @param cursor to enable/disable cursor LCD. (ENABLE or DISABLE)
 * @param blink to enable/disable blink LCD. (ENABLE or DISABLE)
 * @return void
 */
void lcd_set_display(unsigned char display, unsigned char cursor, unsigned char blink);
/**
 * @brief Upload custom character to CGRAM (Character Generator RAM) in LCD
 * CGRAM in LCD can contain maximum data display for 8 character with font 5x8
 * @param location is address of custom character in CGRAM. (0-7)
 * @param charmap is pointer to array contain data display for custom character. You can visit link: https://maxpromer.github.io/LCD-Character-Creator/
 * @return void
 */
void lcd_upload_custom_char(unsigned char location, unsigned char *charmap);
/**
 * @brief Scroll both of line to left LCD
 * @param void
 * @return void
 */
void lcd_scroll_left(void);
/**
 * @brief Scroll both of line to right LCD
 * @param void
 * @return void
 */
void lcd_scroll_right(void);
/**
 * @brief Put character to current cursor in LCD
 * @param chr character
 * @return void
 */
void lcd_put_char(unsigned char chr);
/**
 * @brief Put string to screen
 * If length of string over 16, do not put to line 2.
 * So be careful when using this function.
 * @param str pointer to string
 * @return void
 */
void lcd_put_string(unsigned char *str);
/**
 * @brief Put string to screen, if line not enough to display, cursor move to line 2
 * @param str pointer to string
 * @return void
 */
void lcd_put_string_super(unsigned char *str);
#endif /*_LCD_LIB_H_*/