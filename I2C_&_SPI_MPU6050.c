// Đọc giá trị của mpu và hiện số thập phân lên Max7219 (BKT2)
#include <wiringPiI2C.h>			//thư viện I2C
#include <wiringPi.h>				//Thư viện cơ bản wirringPi
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>					// thư viện toán tử (tang)
#include <wiringPiSPI.h>			// thư viện SPI
// chuỗi các số từ 1-9 để hiện ra màn hình
unsigned char so[10] = {0x7E,0x30,0x6D,0x79,0x33,0x5B,0x5F,0x70,0x7F,0x7B};
// chuỗi đẻ hiện ra chữ OPEN
unsigned char chu_O[4]={0x63,0x7E,0x67,0x4F,0x15};
// chuổi để hiện ra chứ CLOSE
unsigned char chu_C[4]={0x4E,0x30,0x7E,0x5A,0x4F};
// chuỗi để truyền dữ liệu
unsigned char data1[2];

// defice SPI
#define spi0 0
unsigned char data[2];							//?????????????????????????????
// tạo 2 biến 8-bit
int32_t outX, outY;
// tạo biến toàn cục 
int mpu;
int Open_;
int Chay_;
float a_x_prev = 0;  // Gia tốc theo trục x ở lần đọc trước
unsigned long t_prev = 0;  // Thời gian lần đọc trước
// defice thanh ghi
#define Sample_rate 25
#define Config 		26
#define Gyro_config 27
#define Acc_config  28
#define Interrupt   56
#define PWR_Mana	107

#define Acc_x       59
#define Acc_y		61
#define Acc_z		63
// defice chân INT
#define INT_pin 	7


// hàm cấu hình cho con mpu
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
// set up max7219	
void init7219 (void){
	//set decode mode: 0x09FF
	send_data(0x09,0x00);						// tắt decode mode
	//set intensity: 0x0A09, 0x09=9
	send_data(0x0A, 9);
	//set scanslimit
	send_data(0x0B, 7);
	//set normal mode
	send_data(0x0C, 1); //no shut down
	send_data(0x0F, 0); //turn off display test
}


// tạo hàm con đọc cảm biến mpu  (16bit)
int16_t read_sensor(unsigned char sensor){		//ngõ vào là giá trị sensor 
	// biến cục bộ để đọc giá trị high/low
	int16_t high,low,data;
	high = wiringPiI2CReadReg8(mpu, sensor);	// dọc giá trị mức cao
	low = wiringPiI2CReadReg8(mpu, sensor+1);	// giá trị mức thập bằng mức cao +1
	data = (high << 8) | low;					// dịch phải các bit mức cao và chèn vào bit mức thấp
	return data;								// trả giá trị ra là data
}


// hàm truyền data cho Max7219
void send_data(unsigned char a, unsigned char b){
	data1[0] = a;
	data1[1] = b;
	wiringPiSPIDataRW(spi0, data1, 2);
}
	

// Hàm open/close:
void LED_OC(void){
	for(int i=0;i<5;i++){
		if(Open_=1){
			send_data(i+1,chu_O[5-i]);
		}
		if(Open_=0){
			send_data(i+1,chu_C[5-i]);
		}
	}
}
//hàm nháy đèn
void Nhay_(void){
	for(int i=0;i<5;i++){
		send_data(i+1,chu_O[5-i]);
	}
	delay(100);
	for(int i=0;i<5;i++){
		send_data(i+1,0x00);
	}
	delay(100);
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


while(1){
	float Ax = (float)read_sensor(Acc_x)/4096.0;
	float Ay = (float)read_sensor(Acc_y)/4096.0;
	float Az = (float)read_sensor(Acc_z)/4096.0;
	// doc goc theo cong thuc
		float pitch = atan2(Ax, sqrt(pow(Ay,2)+pow(Az,2)))*180/M_PI;
		float roll = atan2(Ay, sqrt(pow(Ax,2)+pow(Az,2)))*180/M_PI;
		printf("\ngia tri pitch %f gia tri roll %f",pitch,roll);
		//printf("\n gia tri Ax %f, gia tri Ay %f, gia tri Az %f",Ax,Ay,Az);
		delay(1000);
	// đọc vận tốc 
	// Tính thời gian hiện tại
        unsigned long t_now = millis();
        // Tính khoảng thời gian giữa 2 lần đọc
        float dt = (t_now - t_prev) / 1000.0;  	//trong 1s
        // Tính vận tốc hiện tại
        float v_x = v_x_prev + (a_x + a_x_prev) / 2 * dt;
        // Lưu giữ giá trị a_x hiện tại và thời gian hiện tại để sử dụng cho lần đọc tiếp theo
        a_x_prev = a_x;
        t_prev = t_now;
        // Lưu giữ giá trị vận tốc hiện tại để sử dụng cho lần tính toán tiếp theo
        v_x_prev = v_x;

	//Đọc giá trị góc X:
		outX = fabs(roll)*10;
		int i = outX/100;
		int j = (outX/10)%10;
		int k = (outX)%10;
		// hiện số thập phân
		send_data(8,so[i]);
		send_data(7,so[j] | 0x80);
		send_data(6,so[k]);
	

	//đọc giá trị trục X 10 độ
		if (roll >10){
			Open_=1;
		}
		if (roll >1 & roll <10){
			Open_=0;
		}
		LED_OC();
	// Đo vận tốc trên đường thẳng với vận tốc 10 km/h
        if (v_x >= 2.78 & Open_=1) {
			Chay_=1;
			Nhay_();
		}
	}
return 0;
}
