all: uart_cheak

.PHONY: clean

OBJ=BSerialPort.o BSerialPortManger.o proc.o 
CC=/home/gcc-linaro-4.9.4-2017.01-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-g++
CPPFLAGS= -std=c++11
CXXFLAGS=-lpthread

uart_cheak:$(OBJ)
	$(CC) $(CPPFLAGS) $(OBJ) -o $@  $(CXXFLAGS)

BSerialPort.o: BSerialPort.cpp BSerialPort.h
	$(CC) $(CPPFLAGS) -c BSerialPort.cpp

BSerialPortManger.o:BSerialPortManger.cpp
	$(CC) $(CPPFLAGS) -c BSerialPortManger.cpp 

proc.o:proc.cpp
	$(CC) $(CPPFLAGS) -c proc.cpp

clean: 
	-rm *.o uart_cheak