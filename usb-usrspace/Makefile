 # # the compiler: gcc for C program, define as g++ for C++
 #  CC = gcc
 #  # compiler flags:
 #  #  -g    adds debugging information to the executable file
 #  #  -Wall turns on most, but not all, compiler warnings
 #  CFLAGS  = -g -Wall
 #  # the build target executable:
 #  TARGET = myusb-ioctrl
 #  all: $(TARGET)
 #       $(TARGET): $(TARGET).c $(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c
 #  clean: $(RM) $(TARGET)

CC=gcc
CFLAGS=-I.
DEPS = usbdev.h
OBJ = myusb-ioctrl.o myusb-ioctrl.o 
 %.o: %.c $(DEPS) $(CC) -c -o $@ $< $(CFLAGS)
myusb-ioctrl: $(OBJ) gcc -o $@ $^ $(CFLAGS)