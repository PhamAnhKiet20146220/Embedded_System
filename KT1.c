// Bai KT1
Pham Anh Kiet - 20146220


// CODE
#include <wiringPi.h>
#include <softPwm.h> //thu vien tao xung PWM
#include <stdio.h> 

#define R 0
#define G 1
#define B 2
#define BT1 4
#define BT2 5

// khai bao bien
int a1,a2,a3;
int states = 0; // bien trang thai


// ham ngat nut nhat 1
void ngat1(void)
{
	if (digitalRead(BT1) == 1)
	{
			while(1){
			printf("nhap 3 gia tri theo thu tu r g b:");
			scanf("%d%d%d", &a1, &a2,&a3);
			softPwmWrite(R, a1);
			softPwmWrite(G, a2);
			softPwmWrite(B, a3);
			delay(2000);
		}
		}
	}
// ham ngat nut nhat 2	
void ngat2(void)
{
	if (digitalRead(BT2) == 1)
	{
		states = 2;
		}
	}

int main (void)
{
	//khai bao chan
	wiringPiSetup();
	pinMode(R,OUTPUT);
	pinMode(G,OUTPUT);
	pinMode(B,OUTPUT);
	
	pinMode(BT1,INPUT);
	pinMode(BT2,INPUT);
	
	//khai bao cap xung PWM mem
	softPwmCreate(R,0,100);
	softPwmCreate(G,0,100);
	softPwmCreate(B,0,100);
	
	// khai bao yeu cau ngat
	wiringPiISR(BT1, INT_EDGE_RISING, &ngat1);
	wiringPiISR(BT2, INT_EDGE_RISING, &ngat2);
	

	}



