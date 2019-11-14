#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <stdlib.h>

#include "lcd_lib.h"
#define BUFFER_SIZE 50

/**************************Define Custom Character***********************/

unsigned char type0[8] = {  /* Arrow */
  0x04,
  0x0E,
  0x15,
  0x04,
  0x04,
  0x04,
  0x00,
  0x00
};
unsigned char type1[8] = { /* Face */
  0x1F,
  0x15,
  0x15,
  0x1F,
  0x11,
  0x0A,
  0x04,
  0x00
};
unsigned char type2[8] = { /* Heart*/
  0x00,
  0x0A,
  0x1F,
  0x1F,
  0x0E,
  0x04,
  0x00,
  0x00
};

unsigned char type3[8] = { /* Music*/
  0x01,
  0x03,
  0x05,
  0x09,
  0x0B,
  0x0B,
  0x18,
  0x18
};

unsigned char *type[4] = {type0, type1,type2,type3};

/*Ham kiem tra entry point write cua vchar driver*/
void write_data_chardev()
{
    char user_buf[BUFFER_SIZE];
    printf("\nEnter your message: ");
    scanf(" %[^\n]s", user_buf);
    lcd_put_string_super(user_buf);
}

void goto_xy(void)
{
    unsigned char x, y;
    printf("\nEnter position (line, column): ");
    scanf(" %d %d", &x, &y);
    lcd_goto_xy(x,y);

}
void set_display(void)
{
    unsigned char display, cursor, blink;
    printf("\nEnter your choice [display-cursor-blink] : ");
    scanf(" %d %d %d", (unsigned char *)&display, (unsigned char *)&cursor, (unsigned char *)&blink);
    lcd_set_display(display,cursor,blink);
}
void upload_custom_char(void)
{
    int i;
    unsigned char location;
    unsigned char *map = NULL;
    printf("\nEnter custom character type [0-7] ([x]: address [x], type[x]): ");
    scanf(" %d", &location);
    lcd_upload_custom_char(location, type[location]);
}
void put_char()
{
    unsigned char chr;
    printf("\nEnter character : ");
    scanf(" %c", &chr);
    lcd_put_char(chr);
}
void put_custom_char()
{
    unsigned char num;
    printf("\nEnter location of character [0-7] : ");
    scanf(" %d", (unsigned char *)&num);
    lcd_put_char(num);
}
void put_string()
{
    char user_buf[BUFFER_SIZE];
    printf("\nEnter your message: ");
    scanf(" %[^\n]s", user_buf);
    lcd_put_string(user_buf);
}
int main()
{
    int ret = 0;
    char option = 'q';
    int fd = -1;
    printf("Select below options: \n");

    printf("\tW (to write string (two line))\n");

    printf("\tC (to clear screen LCD)\n");

    printf("\tG (to goto XY)\n");

    printf("\tD (to control display)\n");

    printf("\tU (to upload custom character)\n");

    printf("\tp (to put a character)\n");

    printf("\to (to put custom character)\n");

    printf("\tP (to put string (one line))\n");

    printf("\tL (to scroll left)\n");

    printf("\tR (to scroll right)\n");

    printf("\tq (to quit the application)\n");
    while(1)
    {
        printf("Enter your options: ");
        scanf(" %c", &option);
        switch (option)
        {
            /* Write string (two line if can) */
            case 'W':
                write_data_chardev();
                break;
            /* Clear Screen LCD */
            case 'C':
                lcd_clear_display();
                break;
            /* Write data to device file */
            case 'G':
                goto_xy();
                break;
            /* Set Display */
            case 'D':
                set_display();
                break;
            /* Upload custom character */
            case 'U':
                upload_custom_char();
                break;
            /* Put a character */
            case 'p':
                put_char();
                break;
            case 'o':
                put_custom_char();
                break;                
            /* Put a string */
            case 'P':
                put_string();
                break;
            /* Scroll Left */
            case 'L':
                lcd_scroll_left();
                break;             
            /* Scroll right */   
            case 'R':
                lcd_scroll_right();
                break;                              
            /* Quit TEST APPLICATION */
            case 'q':
                if(fd > -1)
                printf("Quit the application. Goodbye!!!\n");
                return 0;
            default:
                printf("Invalid option %c \n", option);
                break;
        }
    }
}
