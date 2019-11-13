#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <stdlib.h>

#define DEVICE_NODE "/dev/char_dev_lcd"
/* Define command for IOCTL*/
#define MAGICAL_NUM 248
#define CLEAR_DISPLAY   _IO(MAGICAL_NUM, 0)
#define GOTO_XY         _IOW(MAGICAL_NUM, 1, unsigned char *)
#define SET_BLINK       _IOW(MAGICAL_NUM, 2, unsigned char *)
#define SET_CURSOR      _IOW(MAGICAL_NUM, 3, unsigned char *)


#define BUFFER_SIZE 50

/*Ham kiem tra entry point open cua vchar driver */
int open_lcd_dev(void)
{
    int fd = open(DEVICE_NODE, O_RDWR);
    if(fd < 0)
    {
        printf("Cannot open the device file \n");
        exit(1);
    }
    return fd;
}

/*Ham kiem tra entry point release cua vchar driver*/
void close_lcd_dev(int fd)
{
    close(fd);
}

/*Ham kiem tra entry point write cua vchar driver*/
void write_data_chardev()
{
    int ret = 0;
    char user_buf[BUFFER_SIZE];
    printf("\nEnter your message: ");
    scanf(" %[^\n]s", user_buf);
    int fd = open_lcd_dev();
    ret = write(fd, user_buf, strlen(user_buf)+ 1); /*Ghi chuoi ki tu, ke ca NULL o cuoi*/
    close_lcd_dev(fd);
    if(ret < 0)
    {
        printf("Could not write the message to %s\n", DEVICE_NODE);
    }
    else
    {
        printf("Wrote [%d] bytes to HW\n", ret);
    }
}

void clear_display(void)
{
    int ret = 0;
    int fd = open_lcd_dev();
    ret = ioctl(fd, CLEAR_DISPLAY);
    close_lcd_dev(fd);
    printf("%s display in char device\n", (ret < 0)?"Couldn't clear":"Cleared");
}
void goto_xy(void)
{
    unsigned char posi[2];
    printf("\nEnter line number: ");
    scanf(" %d", &posi[0]);
    printf("\nEnter column number: ");
    scanf(" %d", &posi[1]);

    int fd = open_lcd_dev();
    ioctl(fd, GOTO_XY, (unsigned char *)posi);
    close_lcd_dev(fd);
}
void set_blink(void)
{
    unsigned char isBlink = 0;
    char choose = 'n';
    printf("\nDo you want to enable blink (y/n): ");
    scanf(" %c", &choose);
    if(choose == 'y')
    {
        isBlink = 1;
    }
    else if(choose == 'n')
    {
        isBlink = 0;
    }
    else 
        return;
    int fd = open_lcd_dev();
    ioctl(fd,SET_BLINK,&isBlink);
    close_lcd_dev(fd);
}

void set_cursor(void)
{
    unsigned char isCursor = 0;
    char choose = 'n';
    printf("\nDo you want to enable Cursor (y/n): ");
    scanf(" %c", &choose);
    if(choose == 'y')
    {
        isCursor = 1;
    }
    else if(choose == 'n')
    {
        isCursor = 0;
    }
    else 
        return;
    int fd = open_lcd_dev();
    ioctl(fd,SET_BLINK,&isCursor);
    close_lcd_dev(fd);
}
int main()
{
    int ret = 0;
    char option = 'q';
    int fd = -1;
    printf("Select below options: \n");
    printf("\to (to open a LCD Device File)\n");
    printf("\tc (to close LCD Device File)\n");
    printf("\tw (to write data from device node)\n");

    printf("\tC (to clear LCD)\n");
    printf("\tg (to gotoxy)\n");
    printf("\tb (to set ON/OFF blink)\n");
    printf("\ts (to set ON/OFF cursor)\n");

    printf("\tq (to quit the application)\n");
    while(1)
    {
        printf("Enter your options: ");
        scanf(" %c", &option);
        switch (option)
        {
            /* Open device file */
            case 'o':
                if (fd < 0)
                {
                    fd = open_lcd_dev();
                }
                else
                    printf("%s has already opened\n", DEVICE_NODE);
                break;
            /* Close device file */
            case 'c':
                if(fd > -1)
                    close_lcd_dev(fd);
                else
                    printf("%s has not opened yet!\n", DEVICE_NODE);
                fd = -1;
                break;
            /* Write data to device file */
            case 'w':
                write_data_chardev();
                break;
            /* Clear screen LCD */
            case 'C':
                clear_display();
                break;
            /* Move cursor goto (xy) */
            case 'g':
                goto_xy();
                break;
            /* Set blink ON OFF */
            case 'b':
                set_blink();
                break;
            /* Set cursor ON OFF */
            case 's':
                set_cursor();
                break;
            /* Quit TEST APPLICATION */
            case 'q':
                if(fd > -1)
                    close_lcd_dev(fd);
                printf("Quit the application. Goodbye!!!\n");
                return 0;
            default:
                printf("Invalid option %c \n", option);
                break;
    
        }
    }
}
