#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <stdio.h>
#include <stdint.h>    		//int16_t
#include <math.h>     		//tang       //khi biên dịch:  gcc…..-lwiringPi –lm……

//define thanh ghi
#define Sample_rate  		 25
#define Config          		 26
#define Gyro_config 		 27
#define Acc_config 		 28
#define Interrupt 		 56
#define PWR_Managment	107

#define Acc_X			59
#define Acc_Y			61
#define Acc_Z			63
#define spi0    0                               	// define chân 0 là chân spi

int mpu;                                          	// biến cục bộ
unsigned char buf[2];                          // tạo một char có 2 mảng để send data
int16_t x,y;
//hàm con truyền data
void send_data(unsigned char address, unsigned char value)   // hàm gửi dữ liệu đến các vị trí cần 
{
    buf[0]= address;
    buf[1]= value;
    wiringPiSPIDataRW(spi0, buf, 2);                        // hiện ra dữ liệu đã gửi
}
// ham giot nuoc 
void dot(unit_t x, unit8_t y)
{
    send_data(y+1,(3<<x))
    send_data(y+2,(3<<x))
    send_data(y+1,(4<<x))
    send_data(y+2,(4<<x))
}

//hàm con để đọc giá trị sensor với 16-bit
int16_t read_sensor(unsigned char sensor)    	  // thông số đầu vào sẽ là biến sensor 
{
int16_t high, low, data;
high=wiringPiI2CReadReg8(mpu, sensor);
low=wiringPiI2CReadReg8(mpu, sensor +1);
data=(high<<8) | low;
return data;					// đầu ra để đưa vào chương trình chính sẽ là biến data
}

// hàm con cấu hình chế độ hoạt động
void Init_6050 (void)
{
// regitter
// Sample_rate   500Hz- cứ 2ms đo xong 1 mẫu 
wiringPiI2CWriteReg8(mpu, Sample_rate ,15);
// Config không sử dụng nguồn xung ngoài, tắt bộ lọc số
wiringPiI2CWriteReg8(mpu, Config, 0);
// Gyro_config khoản đo trong: +- 500
wiringPiI2CWriteReg8(mpu, Gyro_config, 0x08);
//khoản đo gia tốc acc: +- 8g                             	 // trả giá trị là 4096
wiringPiI2CWriteReg8(mpu, Acc_config, 0x10);
// mở interrupt của data ready: khi đo xong một mấu sẽ thay đổi trạng thái của chân int
wiringPiI2CWriteReg8(mpu, Interrupt, 1);
// chọn nguồn xung Gyro X
wiringPiI2CWriteReg8(mpu, PWR_Managment, 0x01);
}

  //hàm chính
int main(void)
{
//setup giao tieeps I2C
mpu=wiringPiI2CSetup(0x68);
// thiết lập chế độ đo MPU6050
Init_6050();
// đọc giá trị đo
//đọc trên trục x
//int16_t high, low, data;
//high=wiringPiI2CReadReg8(mpu, Acc_X);
//low=wiringPiI2CReadReg8(mpu, Acc_X +1);
//data=(high<<8) | low;
// tuy nhiên để code gọn hơn ta chuyển phần đọc dữ liệu này thành hàm con
while (1)            //cứ 1 giây thì đọc giá trị 1 lần
{
float Ax =(float)read_sensor(Acc_X)/4096.0;                                       // khởi tạo một biến số thực
float Ay =(float)read_sensor(Acc_Y)/4096.0;  
float Az =(float)read_sensor(Acc_Z)/4096.0;  

float pitch =atan2(Ax, sqrt(pow(Ay,2)+pow(Az,2)))* 180/M_PI;        //M_PI là số Pi
float roll =atan2(Ay, sqrt(pow(Ax,2)+pow(Az,2)))* 180/M_PI;
printf("\ngia tri pitch %f gia tri roll %f",pitch,roll);
	//printf("\n gia tri Ax %f, gia tri Ay %f, gia tri Az %f",Ax,Ay,Az);
x= roll/10
y= pitch/10
delay(1000); 
}   
return 0;
}
