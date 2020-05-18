/*
	MPU6050 Interfacing with Raspberry Pi
	http://www.electronicwings.com
	
	mpu6050_15
*/

#include <wiringPiI2C.h>
#include <stdlib.h>
#include <stdio.h>
#include <wiringPi.h>
#include <math.h>
#include <lcd101rpi.h>
#include <errno.h>
#include <stdint.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

#define Device_Address 0x68	/*Device Address/Identifier for MPU6050*/

#define PWR_MGMT_1   0x6B
#define SMPLRT_DIV   0x19
#define CONFIG       0x1A
#define GYRO_CONFIG  0x1B
#define INT_ENABLE   0x38
#define ACCEL_XOUT_H 0x3B
#define ACCEL_YOUT_H 0x3D
#define ACCEL_ZOUT_H 0x3F
#define GYRO_XOUT_H  0x43
#define GYRO_YOUT_H  0x45
#define GYRO_ZOUT_H  0x47

int fd=0;
int lcdHandle=0;
int lcdAddress = 0x27;

 
//
//	clear screen -- must be run in PUTTY
//
void cls()
{

// #include <stdio.h>

char FF[5]={27,'[','2','J'}; 		// the character to clear the terminal screen
char FG[6]={27,'[',1,1,'f'};    // move the curser to the upper left of the screen


printf("%s", FF);
printf("%s", FG);


}


void MPU6050_Init(){
	
	wiringPiI2CWriteReg8 (fd, SMPLRT_DIV, 0x07);	/* Write to sample rate register */
	wiringPiI2CWriteReg8 (fd, PWR_MGMT_1, 0x01);	/* Write to power management register */
	wiringPiI2CWriteReg8 (fd, CONFIG, 0);		/* Write to Configuration register */
	wiringPiI2CWriteReg8 (fd, GYRO_CONFIG, 24);	/* Write to Gyro Configuration register */
	wiringPiI2CWriteReg8 (fd, INT_ENABLE, 0x01);	/*Write to interrupt enable register */

	} 


short read_raw_data(int addr){
	short high_byte,low_byte,value;
	high_byte = wiringPiI2CReadReg8(fd, addr);
	low_byte = wiringPiI2CReadReg8(fd, addr+1);
	value = (high_byte << 8) | low_byte;
	return value;
}


void ms_delay(int val){
	int i,j;
	for(i=0;i<=val;i++)
		for(j=0;j<1200;j++);
}

 
int usage(char **args) {
  fprintf(stderr,"usage: %s [ -a hex_address ]\n", args[0]);
  return 0;
}

int commandLineOptions(int argc, char **args) {
	int c;


	while ((c = getopt(argc, args, "a:")) != -1)
		switch (c) {
		case 'a':
      if (strncmp(optarg,"0x",2)==0) {
  			sscanf(&optarg[2], "%x", &lcdAddress);
      } else {
  			sscanf(optarg, "%x", &lcdAddress);
      }
			break;
		case '?':
			if (optopt == 'a')
				fprintf(stderr, "Option -%c requires an argument.\n", optopt);
			else if (isprint(optopt))
				fprintf(stderr, "Unknown option `-%c'.\n", optopt);
			else
				fprintf(stderr, "Unknown option character \\x%x.\n", optopt);

			return usage(args);

		default:
			abort();
		}


	//	for (index = optind; index < argc; index++)
	//		printf("Non-option argument %s\n", args[index]);
	return 1;
}

 

int main(){
	
	float Acc_x,Acc_y,Acc_z;
	float Gyro_x,Gyro_y,Gyro_z;
	float Ax=0, Ay=0, Az=0;
	float Gx=0, Gy=0, Gz=0;
	double anglex=0;
	float dummyx=0;
	double angley=0;
	float dummyy=0;
	double avgx=0;
	double avgy=0;
	double avgz=0;
	int i=0;
	int N=200;
	int rs = 0;
	float truAnglex = 0;
	float anglexAdjust = 50.0;
	int lcdHandle = 0;
	char buffer[80];
//	int lcd_Device_Address = 0x27;
	
	fd = wiringPiI2CSetup(Device_Address);   /*Initializes I2C with device Address*/
	
	rs = wiringPiSetup();
	pinMode(1,OUTPUT);
	digitalWrite(1,LOW);
	delay(1000);
	if (rs != 0) 
		{
		fprintf (stderr, ": Unable to initialise wiringPi: %s\n", strerror (errno));
		return 1;
		}

	
	MPU6050_Init();		                 /* Initializes MPU6050 */

	lcdHandle=lcdSetup(lcdAddress);
	if (lcdHandle < 0) 
		{  
		fprintf(stderr, "lcdInit failed\n");  
		return 2; 
		}

	while(1)
	{
		avgx=0;		
		avgy=0;		
		avgz=0;
		
		cls();
		for(i=1; i<=N; i++)
		{
		/*Read raw value of Accelerometer and gyroscope from MPU6050*/
		Acc_x = read_raw_data(ACCEL_XOUT_H);
		Acc_y = read_raw_data(ACCEL_YOUT_H);
		Acc_z = read_raw_data(ACCEL_ZOUT_H);
		
		Gyro_x = read_raw_data(GYRO_XOUT_H);
		Gyro_y = read_raw_data(GYRO_YOUT_H);
		Gyro_z = read_raw_data(GYRO_ZOUT_H);
		
		/* Divide raw value by sensitivity scale factor */
		Ax = Acc_x/16384.0;
		Ay = Acc_y/16384.0;
		Az = Acc_z/16384.0;
		
		Gx = Gyro_x/131;
		Gy = Gyro_y/131;
		Gz = Gyro_z/131;
		
		avgx = avgx + Ax;
		avgy = avgy + Ay;
		avgz = avgz + Az;
		
		}
		
		avgx = avgx / N;		
		avgy = avgy / N;
		avgz = avgz / N;
		
		printf("\n Gx=%.3f °/s\tGy=%.3f °/s\tGz=%.3f °/s\tAx=%.3f g\tAy=%.3f g\tAz=%.3f g\n\n",Gx,Gy,Gz,Ax,Ay,Az);
		
		dummyx = avgz / avgx;
		anglex = atan(dummyx) * 180. / M_PI;
		dummyy = avgz / avgy;
		angley = atan(dummyy) * 180. / M_PI;
		printf("dummyx = %f \tanglex = %+3.1lf\tdummyx = %f \tangley = %+3.1lf\n", dummyx, anglex, dummyy, angley);
		truAnglex = anglex - anglexAdjust;
		printf("\nactual angle = %-f\n", truAnglex);
		sprintf(buffer, "ANGLE = %-3.1f", truAnglex);
		if(truAnglex > 20.)
			digitalWrite(1,HIGH);
		else
			digitalWrite(1,LOW);
//Position cursor on the first line in the first column 
		lcdPosition(lcdHandle, 0, 0);
		lcdPrintf(lcdHandle, "%s%c      ", buffer, 0xdf);
  
//  		lcdPosition(lcdHandle, 0, 1);
//  		lcdPrintf(lcdHandle, "45%c", 0xdf);		
		delay(100);
	}
	return 0;
}/*
	MPU6050 Interfacing with Raspberry Pi
	http://www.electronicwings.com
*/

