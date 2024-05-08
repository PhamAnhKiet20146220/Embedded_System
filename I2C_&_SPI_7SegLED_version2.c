#include <wiringPiI2C.h>
#include <wiringPi.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <wiringPiSPI.h>

unsigned char so[10] = {0x7E,0x30,0x6D,0x79,0x33,0x5B,0x5F,0x70,0x7F,0x7B};

unsigned char data1[2];


#define spi0 0
unsigned char data[2];
int32_t outX, outY;

int mpu;
#define Sample_rate 25
#define Config 		26
#define Gyro_config 27
#define Acc_config  28
#define Interrupt   56
#define PWR_Mana	107

#define Acc_x       59
#define Acc_y		61
#define Acc_z		63
#define INT_pin 	7

void send_data(unsigned char a, unsigned char b)
{
	data1[0] = a;
	data1[1] = b;
	wiringPiSPIDataRW(spi0, data1, 2);
	}
	



void initMpu(void){
	// 2ms do 1 lan
	wiringPiI2CWriteReg8(mpu, Sample_rate, 15);		//sample rate
	// Config k xung ngoai, tat bo loc so
	wiringPiI2CWriteReg8(mpu, Config, 0);	
	// khoang do :+-500
	wiringPiI2CWriteReg8(mpu, Gyro_config, 0x08);	//Gyro
	// khoang do gia toc +-8g, tra gia tri 4096
	wiringPiI2CWriteReg8(mpu, Acc_config, 0x10);	//ACC
	// mo interrupt, khi doc xong thay doi trang thai chan Int
	wiringPiI2CWriteReg8(mpu, Interrupt, 1);		//Interrupt
	// chon nguon xung Gyro X
	wiringPiI2CWriteReg8(mpu, PWR_Mana, 1);		//CLK source
}
// tao ham con doc 16bit
int16_t read_sensor(unsigned char sensor){
	int16_t high,low,data;
	high = wiringPiI2CReadReg8(mpu, sensor);
	low = wiringPiI2CReadReg8(mpu, sensor+1);
	data = (high << 8) | low;
	return data;
	}

// set up max7219	
void init7219 (void){
	//set decode mode: 0x09FF
	send_data(0x09,0x00);
	//set intensity: 0x0A09, 0x09=9
	send_data(0x0A, 9);
	//set scanslimit
	send_data(0x0B, 7);
	//set normal mode
	send_data(0x0C, 1); //no shut down
	send_data(0x0F, 0); //turn off display test
	}

int main(void){
	wiringPiSetup();
	// set up SPI
	wiringPiSPISetup(spi0, 8000000);
	// setup 7219
	init7219();
	// setup i2c interface
	mpu = wiringPiI2CSetup(0x68);
	// check connection
	if(wiringPiI2CReadReg8(mpu, 0x75)!= 0x68){
		printf("Connection fail. \n");
		exit(1);
	}
	// setup operational mode for mpu6050
	initMpu();
	// setup interrupt for INT pin
	pinMode(INT_pin, INPUT);
while(1)
{
	float Ax = (float)read_sensor(Acc_x)/4096.0;
	float Ay = (float)read_sensor(Acc_y)/4096.0;
	float Az = (float)read_sensor(Acc_z)/4096.0;
	// doc goc theo cong thuc
	float pitch = atan2(Ax, sqrt(pow(Ay,2)+pow(Az,2)))*180/M_PI;
	float roll = atan2(Ay, sqrt(pow(Ax,2)+pow(Az,2)))*180/M_PI;
	printf("\ngia tri pitch %f gia tri roll %f",pitch,roll);
	//printf("\n gia tri Ax %f, gia tri Ay %f, gia tri Az %f",Ax,Ay,Az);
	delay(1000);
	
	
	outX = fabs(roll)*10;
	int i = outX/100;
	int j = (outX/10)%10;
	int k = (outX)%10;
	
	outY = fabs(pitch)*10;
	int a = outY/100;
	int b = (outY/10)%10;
	int c = (outY)%10;
	
	

	
	send_data(8,so[i]);
	send_data(7,so[j] | 0x80);
	send_data(6,so[k]);
	send_data(4,so[a]);
	send_data(3,so[b] | 0x80);
	send_data(2,so[c]);
	send_data(5,0x63);
	send_data(1,0x63);
}
return 0;
}
