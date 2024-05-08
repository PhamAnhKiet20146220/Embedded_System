#include <wiringPiSPI.h>
#include <wiringPi.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <wiringPiI2C.h>
#include <math.h>

#define spi0   0
#define INT_pin 7
int mpu;
float temp;
int16_t temperature;

uint8_t buf[2];

void sendData(uint8_t address, uint8_t data){
    buf[0] = address;
    buf[1] = data;
    wiringPiSPIDataRW(spi0, buf, 2);
}

void InitSPI(void){
    // set decode mode: 0x09FF
    sendData(0x09,0xFF);
    // set intensity: 0x0A09
    sendData(0x0A, 9);
    // scan limit: 0x0B07
    sendData(0x0B, 7);
    // no shutdown, turn off display test
    sendData(0x0C, 1);
    sendData(0x0F, 0);
}

void InitMPU(void){
	wiringPiI2CWriteReg8(mpu, 0x19, 9);		//sample rate
	wiringPiI2CWriteReg8(mpu, 0x1A, 0x02);	//DLPF
	wiringPiI2CWriteReg8(mpu, 0x1B, 0x08);	//Gyro
	wiringPiI2CWriteReg8(mpu, 0x1C, 0x10);	//ACC
	wiringPiI2CWriteReg8(mpu, 0x38, 1);		//Interrupt
	wiringPiI2CWriteReg8(mpu, 0x6B, 1);		//CLK source
	}


/*void display_number(uint32_t num, uint8_t o){
    // count the number of digits
    uint8_t count=o;
    uint32_t n = num;
    while(n/10){
        count++;
        n = n/10;
    }
    // set scanlimit
    sendData(0x0B, count-1);
    // dislay number
    for(int i=2; i<count;i++){
        sendData(i+1,num%10);
        num = num/10;
    }
}*/
void display_float(float num, uint8_t dec){
    int32_t integerPart = num;
    int32_t fractionalPart = (num - integerPart) * pow(10,dec);
    int32_t number = integerPart*pow(10,dec) + fractionalPart;
    // count the number of digits
    uint8_t count=1;
    int32_t n = number;
    while(n/10){
        count++;
        n = n/10;
    }
        // set scanlimit
    sendData(0x0B, count-1);
    // dislay number
    for(int i=0; i<count;i++){
        if(i==dec)
            sendData(i+1,(number%10)|0x80); // turn on dot segment 
        else
            sendData(i+1,number%10);
        number = number/10;
    }

}

int16_t readReg16(uint8_t address){
	uint16_t high, low, value;
	high = wiringPiI2CReadReg8(mpu,address);
	low = wiringPiI2CReadReg8(mpu,address+1);
	value= (high<<8)|low;
	return value;
	}
	
void read(void){
	
	wiringPiI2CReadReg8(mpu, 0x3A);
	temperature=readReg16(0x41);
	}
	
int main(void){
	wiringPiSetup();
	
	
	// setup
	mpu = wiringPiI2CSetup(0x68);
	//check connection
	if(wiringPiI2CReadReg8(mpu, 0x75)!= 0x68){
		printf("Connection fail. \n");
		exit(1);
		}
    // setup SPI interface
    wiringPiSPISetup(spi0, 8000000);
    // set openrational mode for max7219
    InitSPI();
    // set openrational mode for MPU6050
    InitMPU();
 
    
    pinMode(INT_pin,INPUT);
    wiringPiISR(INT_pin, INT_EDGE_RISING, &read );
    temp=((float)temperature/340)+36.53;
	printf("Temp: %f\n",temp);
	display_float(temp,2);
    
	while(1){
		
		
		}
    
    return 0;
}
