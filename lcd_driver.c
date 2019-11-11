/*
 * Filename: lcd_driver.c
 * Date: 11/11/2019
 * Description: character driver for a LCD 16x02 device
 */
#include<linux/module.h> /* Define module_init(), module_exit()*/
#include<linux/fs.h> /*contain function allocate/free device number*/
#include<linux/device.h> /*contain function for creating device file*/
#include<linux/slab.h> /* cointain kmalloc & kfree*/
#include<linux/cdev.h> /*contain function work with cdev*/
#include<linux/uaccess.h> /*transport data between user vs kernel*/
#include<linux/ioctl.h> /* contain function services ioctl */

#define DRIVER_AUTHOR "AnhBV - DiemPV"
#define DRIVER_VERSION "1.0"
#define DRIVER_DESC "The character device driver for LCD 16x02 HD44780"

/************************Data structure define***************************/



/*************************************************************************/

/****************************Device Specific-START************************/
/****GPIO set mode*****/

/****GPIO output*****/

/****LCD send command*****/
/****LCD send data*****/
/****LCD init*****/
/****LCD clean display*****/
/****LCD gotoxy*****/
/****LCD visible cursor*****/

/****************************Device Specific-END**************************/

/****************************OS Specific-START************************/
/****Function open entry point*****/

/****Function release entry point*****/

/****Function read entry point*****/
/****Function write entry point*****/

/****Function ioctl entry point*****/

/****************************OS Specific-END**************************/


static int __init lcd_driver_init(void)
{
    printk(KERN_INFO "Initialize lcd driver successfull!\n");
    return 0;
}

static void __exit lcd_driver_exit(void)
{
    printk(KERN_INFO "Goodbye!!!\n");
}

module_init(lcd_driver_init);
module_exit(lcd_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR); 
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_VERSION(DRIVER_VERSION);