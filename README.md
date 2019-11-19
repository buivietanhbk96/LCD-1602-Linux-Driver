# LCD 16x02 Device Driver

This is a kernel level Linux Device driver to control a 16x2 character LCD (with HD44780 LCD controller) with 4 bit mode.

  - Provide API for app in userspace to comminicate with LCD
  - The LCD is interfaced with a micro-controller using GPIO pins.
  - Driver is written on the Raspberry Pi 3 B, the kernel version 4.19.75-v7+

# List of APIs

| Function | Description |
| ------ | ------ |
| __void _lcd_init_(void)__ | __Init LCD, set default configure (display + cursor + blink on)__ |
| __void _lcd_clear_display_(void)__ | __Clear display and set cursor to home position__ |
| __void _lcd_put_char_(char c)__ | __Display character at current cursor position__ |
| __void _lcd_put_string_(const char *s)__ | __Display string at current cursor position__ |
| __void _lcd_put_string_super_(const char *s)__ | __Clear display and display on 2 line if string length > 16__ |
| __void _lcd_goto_xy_(unsigned char x, unsigned char y)__ | __Set cursor to specified position__ |
| __void _lcd_scroll_left_(void)__ | __scroll text to left__ |
| __void _lcd_scroll_right_(void)__ | __scroll 2 line text to right__ |
| __void _lcd_set_display_(unsigned char display, unsigned char cursor, unsigned char blink)__ | __Set cursor to home position__ |
| __void _lcd_upload_custom_char(unsigned char location, unsigned char *charmap)__ | __Upload data for custom char to CGRAM__ |
| __void _lcd_set_auto_scroll(unsigned char status)__ | __Set enable or disable auto move all the text one space to the left each time a letter is added__ |
