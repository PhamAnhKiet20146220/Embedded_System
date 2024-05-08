//Lop nhung sang T6:
Pham Anh Kiet 20146220
#include <wiringPiI2C.h> //them thu vien I2C
#include <wiringPi.h> //thu vien cua Pi
#include <stdio.h> //thu vien stdio
#include <stdint.h> //thu vien stdin
#include <stdlib.h> //thu vien tinh toan
#include <math.h> //thu vien tinh toan
#include <wiringPiSPI.h> //thu vien SPI

unsigned char so[10] = {0x7E,0x30,0x6D,0x79,0x33,0x5B,0x5F,0x70,0x7F,0x7B}; //so tu 0 - 9

unsigned char data1[2]; //gia tri gui vao SPI
unsigned char up[2]={0x3e,0x67}; //ki tu U va P
unsigned char dn[2]={0x7E,0x76}; //ki tu D va N
unsigned char ngang[2]={0x01,0x01}; // ki tu --



#define spi0 0 //define spi chan 0
unsigned char data[2]; //data sensor
int32_t outX, outY; //truc X va Y

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

void send_data(unsigned char a, unsigned char b) //ham gui gia tri SPI
{
	data1[0] = a;
	data1[1] = b;
	wiringPiSPIDataRW(spi0, data1, 2);
	}
	

void initMpu(void){
	//  tan so lay mau 200hz
	wiringPiI2CWriteReg8(mpu, Sample_rate,4);		//sample rate
	// mo digital lpf, tan so < 100hz
	wiringPiI2CWriteReg8(mpu, Config, 2 );	
	// khoang do :+-1000
	wiringPiI2CWriteReg8(mpu, Gyro_config, 0x10);	//Gyro
	// khoang do gia toc +-16g, tra gia tri 2048
	wiringPiI2CWriteReg8(mpu, Acc_config, 0x18);	//ACC
	// mo interrupt, khi doc xong thay doi trang thai chan Int
	wiringPiI2CWriteReg8(mpu, Interrupt, 1);		//Interrupt
	// chon nguon xung Gyro X
	wiringPiI2CWriteReg8(mpu, PWR_Mana, 1);		//CLK source
}
// tao ham con doc gia tri sensor 16bit
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
	float Ax = (float)read_sensor(Acc_x)/2048.0;
	float Ay = (float)read_sensor(Acc_y)/2048.0;
	float Az = (float)read_sensor(Acc_z)/2048.0;
	// doc goc theo cong thuc
	float pitch = atan2(Ax, sqrt(pow(Ay,2)+pow(Az,2)))*180/M_PI;
	float roll = atan2(Ay, sqrt(pow(Ax,2)+pow(Az,2)))*180/M_PI;
	delay(500);
	
//cau b va c
if (pitch>40 || pitch<-40) //ham chop tat led
{ 
	for(int y=1;y<9;y++){ //tat led
	send_data(y,0x00);}
	delay(100);

	
	if(pitch>10){
	printf("\n xe dang len doc ");
	send_data(8,up[0]);
	send_data(7,up[1]);
	}
	if(pitch<-10){
	printf("\n xe dang xuong doc ");
	send_data(8,dn[0]);
	send_data(7,dn[1]);
		}
	if(pitch>-10 && pitch<10)
	{
	printf("\n xe dang nam ngang ");
	send_data(8,ngang[0]);
	send_data(7,ngang[1]);
		}
send_data(6,0x00);	//tat 2 led 6 va 2
send_data(2,0x00);

	outX = fabs(pitch)*10;
	int i = outX/100;
	int j = (outX/10)%10;
	int k = (outX)%10;
	
	send_data(5,so[i]);
	send_data(4,so[j] | 0x80);
	send_data(3,so[k]);
	send_data(1,0x67); //chu P

	delay(100);
}

if (pitch<40 || pitch>-40) //bo khong chop tat led
{ 
	
	if(pitch>10){
	printf("\n xe dang len doc ");
	send_data(8,up[0]);
	send_data(7,up[1]);
	}
	if(pitch<-10){
	printf("\n xe dang xuong doc ");
	send_data(8,dn[0]);
	send_data(7,dn[1]);
		}
	if(pitch>-10 && pitch<10)
	{
	printf("\n xe dang nam ngang ");
	send_data(8,ngang[0]);
	send_data(7,ngang[1]);
		}
// tat 2 led 6 va 2
send_data(6,0x00);	
send_data(2,0x00);
//tinh gia tri float
	outX = fabs(pitch)*10;
	int i = outX/100;
	int j = (outX/10)%10;
	int k = (outX)%10;
	//gui gia tri float
	send_data(5,so[i]);
	send_data(4,so[j] | 0x80);
	send_data(3,so[k]);
	
send_data(1,0x67);

}
//ham tinh van toc



}
return 0;
}
