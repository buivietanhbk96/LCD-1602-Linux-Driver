/*
 * Filename: lcd_driver.c
 * Date: 11/11/2019
 * Description: character driver for a LCD 16x02 device
 * Author: VietAnh Bui
 */
#include<linux/module.h> /* Define module_init(), module_exit()*/
#include<linux/fs.h> /*contain function allocate/free device number*/
#include<linux/device.h> /*contain function for creating device file*/
#include<linux/slab.h> /* cointain kmalloc & kfree*/
#include<linux/cdev.h> /*contain function work with cdev*/
#include<linux/uaccess.h> /*transport data between user vs kernel*/
#include<linux/ioctl.h> /* contain function services ioctl */
#include<linux/kernel.h>
#include <linux/init.h>
#include <linux/fcntl.h>
#include <linux/gpio.h>  /*linux gpio interface*/
#include <linux/kdev_t.h>

#include <linux/delay.h> /*delay*/
#include "lcd_driver.h"

#define DRIVER_AUTHOR "AnhBV-DiemPV"
#define DRIVER_VERSION "1.0"
#define DRIVER_DESC "The character device driver for LCD 16x02 HD44780"
// ********* Linux driver Constants ******************************************************************

#define MINOR_NUM_START		0   /* minor number starts from 0 */
#define MINOR_NUM_COUNT		1   /* the number of minor numbers required */

/************************Driver Structures*******************************/

#define CLASS_NAME  	"class_lcd"
#define DEVICE_NAME 	"lcd_1602"

struct _lcd_driver
{
    dev_t 		dev_number;	       /*dynamically allocated device major number*/
    struct class *lcd_class;	   /*class structure*/
	struct device *dev;
	struct cdev  *lcd_cdev;	       /*cdev structure*/
}lcd_driver;


/****************************Device Specific-START************************/

/*************Setup GPIO**********************/
static int lcd_pin_setup(unsigned int pin_number)
{
    int ret;
    PIN_DIRECTION gpio_direction = OUTPUT_PIN;
    
    /* Request GPIO allocation */
    ret = gpio_request(pin_number, "GPIO Pin Request");
    if(ret != 0)
    {
        printk(KERN_DEBUG "ERR: Failed to request gpio %d\n",pin_number);
        return ret;
    }
    /* Set GPIO export & disallow user space to change direction of GPIO */
    ret = gpio_export(pin_number, 0);
    if(ret != 0)
    {
        printk(KERN_DEBUG "ERR: Failed to export gpio %d\n",pin_number);
        return ret;
    }
    /* Set GPIO direction */
    ret = gpio_direction_output(pin_number, gpio_direction);
	if(ret != 0)
    {
        printk(KERN_DEBUG "ERR: Failed to set direction gpio %d\n",pin_number);
        return ret;
    }   

    /* Set init value of GPIO*/
    gpio_set_value(pin_number, 0);

    /* Return 0 if there is no error*/
    return 0;
}

/***********Setup All GPIO*******************/
static void lcd_pin_setup_All( void )
{
    lcd_pin_setup(LCD_RS_PIN_NUMBER);
    lcd_pin_setup(LCD_EN_PIN_NUMBER);

    lcd_pin_setup(LCD_D4_PIN_NUMBER);
    lcd_pin_setup(LCD_D5_PIN_NUMBER);
    lcd_pin_setup(LCD_D6_PIN_NUMBER);
    lcd_pin_setup(LCD_D7_PIN_NUMBER);
}

/***********Release a GPIO********************/
static void lcd_pin_release(unsigned int pin_number)
{
	gpio_unexport( pin_number);	// return GPIO pin
	gpio_free( pin_number);		// unexport GPIO pin
}

/***********Release All GPIO*******************/
static void lcd_pin_release_All( void )
{
    lcd_pin_release(LCD_RS_PIN_NUMBER);
    lcd_pin_release(LCD_EN_PIN_NUMBER);

    lcd_pin_release(LCD_D4_PIN_NUMBER);
    lcd_pin_release(LCD_D5_PIN_NUMBER);
    lcd_pin_release(LCD_D6_PIN_NUMBER);
    lcd_pin_release(LCD_D7_PIN_NUMBER);
}
/********************LCD Nibble************************/
static void lcd_nibble(char data)
{
	int db7_data = 0;
	int db6_data = 0;
	int db5_data = 0;
	int db4_data = 0;
	usleep_range(2000, 3000); /* added delay instead of busy checking */
	/*Get 4 bit Upper*/
	db7_data = ((data) & (0x01 << 7)) >> (7);
	db6_data = ((data) & (0x01 << 6)) >> (6);
	db5_data = ((data) & (0x01 << 5)) >> (5);
	db4_data = ((data) & (0x01 << 4)) >> (4);

	/*Set value to correspond GPIO*/
	gpio_set_value(LCD_D7_PIN_NUMBER, db7_data);
	gpio_set_value(LCD_D6_PIN_NUMBER, db6_data);
	gpio_set_value(LCD_D5_PIN_NUMBER, db5_data);
	gpio_set_value(LCD_D4_PIN_NUMBER, db4_data);

}
/***********LCD send command*******************
 * Only 4 upper bit was used */
static void lcd_send_command(char command)
{
	/*Send signal to Pin*/
	lcd_nibble(command & 0xF0);
	/*Set to command mode*/
	gpio_set_value(LCD_RS_PIN_NUMBER, RS_COMMAND_MODE);
	usleep_range(5, 10);
	/* Simulate falling edge triggered clock */
	gpio_set_value(LCD_EN_PIN_NUMBER, 1);
	usleep_range(5, 10);
	gpio_set_value(LCD_EN_PIN_NUMBER, 0);
}
/******************LCD send data************************************/
static void lcd_send_data(char data)
{
	/* Part 1.  Upper 4 bit data (from bit 7 to bit 4)*/
	lcd_nibble(data & 0xF0);
	/* Part 1. Set to data mode*/
	gpio_set_value(LCD_RS_PIN_NUMBER, RS_DATA_MODE);
	usleep_range(5, 10);

	/* Part 1. Simulate falling edge triggered clock */
	gpio_set_value(LCD_EN_PIN_NUMBER, 1);
	usleep_range(5, 10);
	gpio_set_value(LCD_EN_PIN_NUMBER, 0);	

	/* Part 2. Lower 4 bit data (from bit 3 to bit 0)*/
	lcd_nibble((data & 0x0F) << 4);
	/* Part 2. Set to data mode */
	gpio_set_value(LCD_RS_PIN_NUMBER, RS_DATA_MODE);
    usleep_range(5, 10);

	/* Part 2. Simulate falling edge triggered clock */
	gpio_set_value(LCD_EN_PIN_NUMBER, 1);
	usleep_range(5, 10);
	gpio_set_value(LCD_EN_PIN_NUMBER, 0);	
}
/************************LCD init******************************/
/* Initialize the LCD in 4 bit mode as described on the HD44780 LCD controller document.*/
static void lcd_initialize()
{
	usleep_range(41*1000, 50*1000);	// wait for more than 40 ms once the power is on

	lcd_send_command(0x30);		// Instruction 0011b (Function set)
	usleep_range(5*1000, 6*1000);	// wait for more than 4.1 ms

	lcd_send_command(0x30);		// Instruction 0011b (Function set)
	usleep_range(100,200);		// wait for more than 100 us

	lcd_send_command(0x30);		// Instruction 0011b (Function set)
	usleep_range(100,200);		// wait for more than 100 us

	lcd_send_command(0x20);		/* Instruction 0010b (Function set)*/ /*Set interface to be 4 bits long*/
	
    usleep_range(100, 200);     /* wait for more than 100 us */

    lcd_send_command(0x20); /* Instruction 0010b (Function set) */
    lcd_send_command(0x80); /* Instruction NF**b
					   Set N = 1, or 2-line display
					   Set F = 0, or 5x8 dot character font*/
    usleep_range(41 * 1000, 50 * 1000);

    /* Display off, Cursor Off, Blink Off*/
    lcd_send_command(0x00); // Instruction 0000b
    lcd_send_command(0x80); // Instruction 1000b
    usleep_range(100, 200);

    /* Display clear, return the cursor Home */
    lcd_send_command(0x00); // Instruction 0000b
    lcd_send_command(0x10); // Instruction 0001b
    usleep_range(100, 200);

    /* Entry mode set: Inc cursor to the right when writing, don't shift screen*/
    lcd_send_command(0x00); /* Instruction 0000b */
    lcd_send_command(0x60); /* Instruction 01(I/D)Sb -> 0110b
					   			Set I/D = 1, or increment or decrement DDRAM address by 1
					   			Set S = 0, or no display shift */
    usleep_range(100, 200);

    /* Initialization Completed, but set up default LCD setting here */

    /* Display On/off Control */
    lcd_send_command(0x00); /* Instruction 0000b*/
    lcd_send_command(0xF0); /* Instruction 1DCBb  
					   Set D= 1, or Display on
					   Set C= 1, or Cursor on
					   Set B= 1, or Blinking on
					*/
    usleep_range(100,200);
}
/**************Display string on LCD with line number*******************************/
static int lcd_print(char * msg, unsigned int lineNumber)
{
	unsigned int counter = 0;
	unsigned int lineNum = lineNumber;

	if(msg == NULL){
		printk(KERN_DEBUG "ERR: Empty data for lcd_print \n");
		return -1;
	}

	if( (lineNum != 1) && (lineNum != 2) ) { 
		printk( KERN_DEBUG "ERR: Invalid line number readjusted to 1 \n");
		lineNum = 1;
	}

    lcd_setLine(LCD_FIRST_LINE);
	if (lineNum == 1)
	{
		lcd_setLine(LCD_FIRST_LINE);
		while (*(msg) != '\0')
		{
			if (counter >= NUM_CHAR_PER_LINE)
			{
				lineNum = 2; // continue writing on the next line if the string is too long
				counter = 0;
				break;
			}
			lcd_send_data(*msg);
			msg++;
			counter++;
		}
	}
	if (lineNum == 2)
	{
		lcd_setLine(LCD_SECOND_LINE);
		while (*(msg) != '\0')
		{
			if (counter >= NUM_CHAR_PER_LINE)
			{
				break;
			}
			lcd_send_data(*msg);
			msg++;
			counter++;
		}
	}
	return 0;
}
/*************************Set line to display *********************************/
void lcd_setLine(unsigned int line)
{
	if(line == 1){
		lcd_send_command(0x80);	/* set position to LCD line 1*/
		lcd_send_command(0x00);
	}
	else if(line == 2){
		lcd_send_command(0xC0);
		lcd_send_command(0x00);
	}
	else{
		printk("ERR: Invalid line number. Select either 1 or 2 \n");
	}
}

/*************************LCD clean display*********************************/
static void lcd_clearDisplay(void)
{
    /*Send 0x01 command*/
	lcd_send_command( 0x00 ); /* upper 4 bits of command - 0000b */
	lcd_send_command( 0x10 ); /* lower 4 bits of command - 0001b */  

	printk(KERN_INFO "LCD Driver: display clear\n");
}
/*************************LCD scroll left*********************************/
static void lcd_scroll_left(void)
{
	unsigned char command = LCD_SHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT;
	lcd_send_command(command & 0xF0); 	  /* upper 4 bits of command */
	lcd_send_command((command & 0x0F) << 4); /* lower 4 bits of command */

	printk(KERN_INFO "LCD Driver: display clear\n");
}
/*************************LCD scroll right*********************************/
static void lcd_scroll_right(void)
{
	unsigned char command = LCD_SHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT;
	lcd_send_command(command & 0xF0); 	  /* upper 4 bits of command */
	lcd_send_command((command & 0x0F) << 4); /* lower 4 bits of command */

	printk(KERN_INFO "LCD Driver: display clear\n");
}
/************************LCD Set cursor to position (x,y)*********************/
static void lcd_gotoxy(axis_t *position)
{
	char command;

	if(1 == position->x){
		command = 0x80 + position->y;
		
		lcd_send_command(command & 0xF0); 	  /* upper 4 bits of command */
		lcd_send_command((command & 0x0F) << 4); /* lower 4 bits of command */
	}
	else if(2 == position->x){
		command = 0xC0 + position->y;

		lcd_send_command(command & 0xF0); 	  /* upper 4 bits of command */
		lcd_send_command((command & 0x0F) << 4 ); /* lower 4 bits of command */
	}
	else{
		printk("ERR: Invalid line number. Select either 1 or 2 \n");
	}	
}
/**********************LCD Set ON/OFF Display/Cursor/Blink********************************/
static void lcd_set_display(display_control_t *display_option)
{
	/* Display On/off Control */
	/* Instruction 1DCBb  - COMMAND CONTROL DISPLAY 0x08*/
	/* Set D= 1, or Display on
	   Set C= 1, or Cursor on
	   Set B= 1, or Blinking on
	*/
	unsigned char command = 0;
	command = LCD_DISPLAYCONTROL \
	          | (display_option->display ? LCD_DISPLAYON : LCD_DISPLAYOFF) \
			  | (display_option->cursor ? LCD_CURSORON  : LCD_CURSOROFF) \
			  | (display_option->blink ? LCD_BLINKON   : LCD_BLINKOFF);

	lcd_send_command(command & 0xF0);		  /* 4 Upper bit*/
	lcd_send_command((command & 0x0F) << 4 ); /* 4 Lower bit - contain display config */
}
/**********************LCD Upload custom character to CGRAM********************************/
static void lcd_create_char(custom_char_t *custom_chr)
{
	unsigned char command = 0x00, i;
	command = LCD_SETCGRAMADDR | ((custom_chr->location & 0x07) << 3); /* = LCD_SETCGRAMADDR + (location*8) => 5x8 character */
	lcd_send_command(command & 0xF0);		  /* 4 Upper bit */
	lcd_send_command((command & 0x0F) << 4 ); /* 4 Lower bit */
	usleep_range(60, 100);
	for(i = 0; i < 8; i++)
	{
		lcd_send_data(custom_chr->charmap[i]);
		usleep_range(60, 100);
	}
	lcd_clearDisplay();
}
/****************************Device Specific-END**************************/

/****************************OS Specific-START************************/
/****Function open entry point*****/
static int char_lcd_open(struct inode *inode, struct file *filp)
{
    printk(KERN_INFO "Open device file event\n");
    return 0;
}
/****Function release entry point*****/
static int char_lcd_release(struct inode *inode, struct file *filp)
{
    printk(KERN_INFO "Close event\n");
    return 0;
}
/****Function read entry point*****/

/****Function write entry point*****/
static ssize_t char_lcd_write(struct file *filp, const char __user *user_buf, size_t len, loff_t *off)
{
    char *kernel_buf = NULL;
    if(NULL == user_buf)
    {
        printk(KERN_DEBUG "ERR: Empty user buffer\n");
        return -ENOMEM;
    }
    printk(KERN_INFO "Handle write event from %lld, %zu bytes\n", *off, len);
    
    kernel_buf = kzalloc(len, GFP_KERNEL);
    if(copy_from_user(kernel_buf, user_buf, len))
    {
        return -EFAULT;
    }
    kernel_buf[len] = '\0';
    /*clear display*/
	lcd_clearDisplay();

	/*print on the first line by default*/
	lcd_print(kernel_buf, LCD_FIRST_LINE);
	kfree(kernel_buf);
	printk(KERN_INFO "LCD Driver: write()\n");
    return len;
}
/**************************Function ioctl entry point********************************/
static long char_lcd_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    int ret = 0;
    printk("Handle ioctl event (cmd: %u)\n",cmd);

    switch(cmd)
    {
        case LCD_CLEAR:
        {
			lcd_clearDisplay();
            break;
        }
        case LCD_GOTOXY:
        {
            axis_t position;
			if (copy_from_user(&position, (axis_t *)arg, sizeof(position)))
			{
				printk(KERN_DEBUG "ERR: Failed to copy from user space buffer \n");
				return -EFAULT;
			}
            lcd_gotoxy(&position);
            printk(KERN_INFO "Cursor go [%d][%d]\n", position.x,position.y);
            break;
        }
        case LCD_SET_DISPLAY:
        {
            display_control_t display;
			if (copy_from_user(&display, (display_control_t *)arg, sizeof(display)))
			{
				printk(KERN_DEBUG "ERR: Failed to copy from user space buffer \n");
				return -EFAULT;
			}			
            lcd_set_display(&display);
            printk(KERN_INFO "Control display: Display [%s], Cursor [%s], Blink [%s]\n", 
							(display.display)?"enable":"disable",
							(display.cursor)?"enable":"disable",
							(display.blink)?"enable":"disable");
            break;
        }
        case LCD_PUT_CHAR:
        {
            unsigned char character;
			if (copy_from_user(&character, (unsigned char *)arg, sizeof(character)))
			{
				printk(KERN_DEBUG "ERR: Failed to copy from user space buffer \n");
				return -EFAULT;
			}				
            lcd_send_data(character);
            printk(KERN_INFO "Put character: %c\n", character);
            break;
        }
		case LCD_SCROLL_LEFT:
		{
			lcd_scroll_left();
			break;
		}
		case LCD_SCROLL_RIGHT:
		{
			lcd_scroll_right();
			break;
		}		
		case LCD_UPLOAD_CUSTOM_CHAR:
		{
			custom_char_t custom_chr;
			if (copy_from_user(&custom_chr, (custom_char_t *)arg, sizeof(custom_chr)))
			{
				printk(KERN_DEBUG "ERR: Failed to copy from user space buffer \n");
				return -EFAULT;
			}				
			lcd_create_char(&custom_chr);
			break;
		}
		case LCD_INIT:
		{
			lcd_initialize();
			break;
		}
    }
    return ret;
}

/* file operation structure */
static struct file_operations lcd_fops =
{
	.owner = THIS_MODULE,
	.open  =  char_lcd_open,
	.release = char_lcd_release,
	.write   = char_lcd_write,
	.unlocked_ioctl = char_lcd_ioctl,
};

/****************************OS Specific-END**************************/


static int __init lcd_driver_init(void)
{

	/* dynamically allocate device major number */
	if( alloc_chrdev_region( &lcd_driver.dev_number, MINOR_NUM_START , MINOR_NUM_COUNT , DEVICE_NAME ) < 0) 
	{
		printk( KERN_DEBUG "ERR: Failed to allocate major number \n" );
		return -1;
	}

	/* create a class structure */
	lcd_driver.lcd_class = class_create(THIS_MODULE, CLASS_NAME );
	
	if( IS_ERR(lcd_driver.lcd_class) )
	{		
		unregister_chrdev_region( lcd_driver.dev_number, MINOR_NUM_COUNT );
		printk( KERN_DEBUG "ERR: Failed to create class structure \n" );
		
		return PTR_ERR( lcd_driver.lcd_class ) ;
	}
			
	/* create a device and registers it with sysfs */
	lcd_driver.dev = device_create( lcd_driver.lcd_class, NULL, lcd_driver.dev_number, NULL, DEVICE_NAME );
	
	if( IS_ERR(lcd_driver.dev) )
	{
		class_destroy( lcd_driver.lcd_class );
		unregister_chrdev_region( lcd_driver.dev_number, MINOR_NUM_COUNT );
		printk( KERN_DEBUG "ERR: Failed to create device structure \n" );		

		return PTR_ERR( lcd_driver.dev );
	}

	/* initialize a cdev structure */
	lcd_driver.lcd_cdev = cdev_alloc();
	cdev_init(lcd_driver.lcd_cdev, &lcd_fops);

	/* add a character device to the system */
	if( cdev_add(lcd_driver.lcd_cdev, lcd_driver.dev_number, MINOR_NUM_COUNT) < 0 )
	{
		device_destroy(lcd_driver.lcd_class, lcd_driver.dev_number);
		class_destroy(lcd_driver.lcd_class );
		unregister_chrdev_region(lcd_driver.dev_number, MINOR_NUM_COUNT);
		printk( KERN_DEBUG "ERR: Failed to add cdev \n" );		
		return -1;		
	}

	/*setup GPIO pins*/
	lcd_pin_setup_All();

	/* initialize LCD once */
	lcd_initialize();

    printk(KERN_INFO "Initialize lcd driver successfull!\n");
    return 0;
}

static void __exit lcd_driver_exit(void)
{
	// clear LCD display
	lcd_clearDisplay();

	// remove a cdev from the system
	cdev_del(lcd_driver.lcd_cdev);

	// remove device
	device_destroy( lcd_driver.lcd_class, lcd_driver.dev_number );

	// destroy class
	class_destroy( lcd_driver.lcd_class );	

	// deallocate device number
	unregister_chrdev_region(lcd_driver.dev_number, MINOR_NUM_COUNT );

	// releasse GPIO pins
	lcd_pin_release_All();

	printk(KERN_INFO "lcd Driver Exited. \n");
    printk(KERN_INFO "Goodbye!!!\n");
}

module_init(lcd_driver_init);
module_exit(lcd_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR); 
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_VERSION(DRIVER_VERSION);
