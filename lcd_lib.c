#include "lcd_lib.h"

static int lcd_open_dev(void)
{
    int fd = open(DEVICE_NODE, O_RDWR);
    if(fd < 0)
    {
        printf("Cannot open the device file \n");
        exit(1);
    }
    return fd;
}
static void lcd_close_dev(int fd)
{
    close(fd);
}

void lcd_init(void)
{
    int fd = lcd_open_dev();
    ioctl(fd, INIT_LCD);
    lcd_close_dev(fd);
}

void lcd_clear_display(void)
{   
    int ret =0;
    int fd = lcd_open_dev();
    ret = ioctl(fd, CLEAR_DISPLAY);
    lcd_close_dev(fd);
    printf("%s display in char device\n", (ret < 0)?"Couldn't clear":"Cleared"); 
}

void lcd_goto_xy(unsigned char x, unsigned char y)
{
    coordinate_t location;
    location.x = x;
    location.y = y;
    int fd = lcd_open_dev();
    printf("Library: Cursor goto: %d, %d\n", location.x, location.y);
    ioctl(fd, GOTO_XY, &location);
    lcd_close_dev(fd);
}

void lcd_set_display(unsigned char display, unsigned char cursor, unsigned char blink)
{
    control_display_t control;
    control.display = display;
    control.cursor = cursor;
    control.blink = blink;
    int fd = lcd_open_dev();
    ioctl(fd, SET_DISPLAY, &control);
    lcd_close_dev(fd);
}

void lcd_upload_custom_char(unsigned char location, unsigned char *charmap)
{
    custom_character_t custom_char;
    custom_char.location = location;
    custom_char.charmap = charmap;
    int fd = lcd_open_dev();
    ioctl(fd, UPLOAD_CUSTOM_CHAR, &custom_char);
    lcd_close_dev(fd);
}

void lcd_scroll_left(void)
{
    int fd = lcd_open_dev();
    ioctl(fd, SCROLL_LEFT);
    lcd_close_dev(fd);
}

void lcd_scroll_right(void)
{
    int fd = lcd_open_dev();
    ioctl(fd, SCROLL_RIGHT);
    lcd_close_dev(fd);
}

void lcd_put_char(unsigned char chr)
{
    int fd = lcd_open_dev();
    ioctl(fd, PUT_CHAR, (unsigned char *)&chr);
    lcd_close_dev(fd);
}

void lcd_put_string(unsigned char *str)
{
    unsigned char *tmp = str;
    unsigned char counter = 0;
    int fd = lcd_open_dev();
    while (*(tmp) != '\0')
    {
        if (counter >= NUM_CHAR_PER_LINE)
        {
            break;
        }
        ioctl(fd, PUT_CHAR, tmp);
        tmp++;
        counter++;
    }
    lcd_close_dev(fd);
}

void lcd_put_string_super(unsigned char *str)
{
    int fd = lcd_open_dev();
    write(fd, str, strlen(str)+ 1); /* Print string to screen */
    lcd_close_dev(fd);    
}

int lcd_set_auto_scroll(unsigned char status)
{   
    /* Check Check the correctness of the parameter */
    if((ENABLE != status) && (DISABLE != status))
    {
        return -1;
    }
    int fd = lcd_open_dev();
    ioctl(fd, SET_AUTOSCROLL, &status);
    lcd_close_dev(fd);    
    return 0;
}