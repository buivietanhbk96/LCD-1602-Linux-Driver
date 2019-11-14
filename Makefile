EXTRA_CFLAGS =  -Wall
KDIR = /lib/modules/`uname -r`/build
obj-m        =  lcd_driver.o
all: ko app_test

ko:
	make -C $(KDIR) M=`pwd`

app_test: app_test.o lcd_lib.o
	gcc -o app_test app_test.o lcd_lib.o

app_test.o: app_test.c
	gcc -c app_test.c
lcd_lib.o: lcd_lib.c lcd_lib.h
	gcc -c lcd_lib.c
clean:
	make -C $(KDIR) M=`pwd` clean
	rm app_test