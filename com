#!/bin/ksh

gcc -c cls.c
[ $? != 0 ] && exit

gcc *.o -lm -lwiringPi -lwiringPiDev -lwiringPiLCD101rpi main.c  -o demo
