gcc -c cls.c
gcc *.o -lm -lwiringPi -lwiringPiDev -lwiringPiLCD101rpi main.c  -o demo
