# LCD 16x02 Device Driver

This is a kernel level Linux Device driver to control a 16x2 character LCD (with HD44780 LCD controller) with 4 bit mode.

  - Provide API for app in userspace to comminicate with LCD
  - The LCD is interfaced with a micro-controller using GPIO pins.
  - Driver is written on the Raspberry Pi 3 B, the kernel version 4.19.75-v7+

# List of APIs

| Function | Description |
| ------ | ------ |
| __int _lcd_open_(void)__ | __open device file of LCD, return fd__ |
| __void _lcd_close_(int fd)__ | __close device file__ |
| __int _lcd_home_(void)__ | __Set cursor to home position__ |
| __void _lcd_clear_(void)__ | __Clear display and set cursor to home position__ |
| __int _lcd_put_char_(char c)__ | __Display character at current cursor position__ |
| __void _lcd_put_string_(const char *s)__ | __Display string at current cursor position__ |
| __void _lcd_gotoxy_(uint8_t x, uint8_t y)__ | __Set cursor to specified position__ |
| __void _lcd_scrollLeft_(void)__ | __scroll text to left__ |
| __void _lcd_scrollRight_(void)__ | __scroll text to right__ |
| __void _lcd_blink_(void)__ | __Set cursor to home position__ |
| __void _lcd_set_backlight_(uint8_t status)__ | __Set on/off backlight__ |
