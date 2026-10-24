#include "stm32f10x.h"                  // Device header
#include "USART1.h"
#include "Delay.h"
uint8_t unlock_Cmd[5]={0xFF,0xAA,0x69,0x88,0xB5};//解锁指令
uint8_t Back_Cali_Cmd[5]={0xFF,0xAA,0x01,0x00,0x00};//退出校准指令
uint8_t Save_Cmd[5]={0xFF,0xAA,0x00,0x00,0x00};//保存指令
void Acc_Cali(void)//加速度计校准
{
	//1.加速度计校准指令数组，格式：FF AA 01 01 00
	uint8_t Acc_Cmd[5]={0xFF,0xAA,0x01,0x01,0x00};
	
	//2.校准流程
	USART1_SendArray(unlock_Cmd,5);//解锁
	Delay_ms(200);
	USART1_SendArray(Acc_Cmd,5);//校准
	Delay_s(4);
	USART1_SendArray(Back_Cali_Cmd,5);//退出校准
	Delay_ms(100);
	USART1_SendArray(Save_Cmd,5);//保存
}
void Gyr_Cali(void)//陀螺仪校准
{
	//1.关闭陀螺仪校准指令数组，格式：FF AA 61 01 00(陀螺仪自动校准中，00表示开启默认是开启的，01表示关闭陀螺仪自动校准)
	uint8_t Gyr_Cmd[5]={0xFF,0xAA,0x61,0x01,0x00};
	
	//2.校准流程
	USART1_SendArray(unlock_Cmd,5);//解锁
	Delay_ms(200);
	USART1_SendArray(Gyr_Cmd,5);//校准
	Delay_s(3);
	USART1_SendArray(Save_Cmd,5);//保存
}
void Angle_Cali(void)//角度校准
{
	//1.角度校准指令，格式：FF AA 01 08 00
	uint8_t Angle_Cmd[5]={0xFF,0xAA,0x01,0x08,0x00};
	
	//2.校准流程
	USART1_SendArray(unlock_Cmd,5);//解锁
	Delay_ms(200);
	USART1_SendArray(Angle_Cmd,5);//校准
	Delay_s(3);
	USART1_SendArray(Save_Cmd,5);//保存
}
void Head_Cali(void)//航向校准(z轴)
{
	//1.航向校准指令，格式：FF AA 01 04 00
	uint8_t Head_Cmd[5]={0xFF,0xAA,0x01,0x04,0x00};

	//2.校准流程
	USART1_SendArray(unlock_Cmd,5);//解锁
	Delay_ms(200);
	USART1_SendArray(Head_Cmd,5);//校准
	Delay_s(3);
	USART1_SendArray(Save_Cmd,5);//保存
}
void Time_Cali(void)//时间校准
{
	//1.时间校准指令
	uint8_t year_month_Cmd[5]={0xFF,0xAA,0x30,0x1A,0x02};
	uint8_t day_hour_Cmd[5]={0xFF,0xAA,0x31,0x02,0x11};
	uint8_t hour_minute_Cmd[5]={0xFF,0xAA,0x32,0x02,0x2B};
	uint8_t ms_Cmd[5]={0xFF,0xAA,0x33,0xDC,0x03};

	//2.校准流程
	USART1_SendArray(unlock_Cmd,5);//解锁
	USART1_SendArray(year_month_Cmd,5);
	USART1_SendArray(day_hour_Cmd,5);
	USART1_SendArray(hour_minute_Cmd,5);
	USART1_SendArray(ms_Cmd,5);
	USART1_SendArray(Save_Cmd,5);//保存
}

///////////////////角度和经纬度//////////////////////
typedef struct {
	//角度数据
	float Roll;//横滚角
	float Pitch;//俯仰角
	float Yaw;//航向角
	//经纬度数据
	float Lon;//经度
	float Lat;//纬度
}IMU_Data;
IMU_Data Sensor_Data;
void Angle_frame(uint8_t *buff)//角度数据
{
	int16_t Roll_Hex=(short)( ((short)buff[3]<<8) | buff[2]);
	int16_t Pitch_Hex=(short)( ((short)buff[5]<<8) | buff[4]);
	int16_t Yaw_Hex=(short)( ((short)buff[7]<<8) | buff[6]);

	Sensor_Data.Roll=Roll_Hex / 32768.0f * 180.0f;
	Sensor_Data.Pitch=Pitch_Hex / 32768.0f * 180.0f;
	Sensor_Data.Yaw=Yaw_Hex / 32768.0f * 180.0f;
}

////经纬度数据不准确了看小数部分
//typedef struct {
//	float Lon_xiaoshu;//经度小数
//	float Lat_xiaoshu;//纬度小数
//}IMU_Data_shuchu;
//IMU_Data_shuchu jiexi;

void LonLat_frame(uint8_t *buff)//经Lon纬Lat度数据
{
	//将原始数据存入
	uint32_t Lon_Hex=((uint32_t)buff[5]<<24) | (uint32_t)(buff[4]<<16) | (uint32_t)(buff[3]<<8) | buff[2];
	uint32_t Lat_Hex=((uint32_t)buff[9]<<24) | (uint32_t)(buff[8]<<16) | (uint32_t)(buff[7]<<8) | buff[6];
	
	int32_t Lon_Data=(int32_t)Lon_Hex;
	int32_t Lat_Data=(int32_t)Lat_Hex;
	//经纬度解析
	float Lon_du=Lon_Data / 10000000;
	float Lon_fen=(Lon_Data % 10000000) / 100000.0;
	Sensor_Data.Lon=Lon_du + Lon_fen / 60;
	
	float Lat_du=Lat_Data / 10000000;
	float Lat_fen=(Lat_Data % 10000000) / 100000.0;
	Sensor_Data.Lat=Lat_du + Lat_fen / 60;
}
////////////////////////////////////////////////////////

////////////////GPS数据（海拔、航向角、地速）//////////////////////
typedef struct {
	//GPS海拔
	float GPS_Height;
	//GPS航向
	float GPS_Yaw;
	//GPS地速
	float GPS_V;
}GPS;
GPS GPS_Data;
void GPS_frame(uint8_t *buff)
{
	int16_t GPS_Height_Hex=(short)( ((short)buff[3]<<8) | buff[2]);
	int16_t GPS_Yaw_Hex=(short)( ((short)buff[5]<<8) | buff[4]);
	int32_t GPS_V_Hex=((int32_t)buff[9]<<24) | (int32_t)(buff[8]<<16) | (int32_t)(buff[7]<<8) | buff[6];
	
	//数据解析
	GPS_Data.GPS_Height=GPS_Height_Hex / 10.0;
	GPS_Data.GPS_Yaw=GPS_Yaw_Hex / 100.0;
	GPS_Data.GPS_V=GPS_V_Hex / 1000.0;
}
////////////////////////////////////////////////////////

////////////////////加速度/////////////////////////
typedef struct{
	//加速度
	float acc_x;
	float acc_y;
	float acc_z;
}ACC;
ACC ACC_data;
void ACC_frame(uint8_t *buff)
{
	int16_t acc_x_Hex=(short)( ((short)buff[3]<<8) | buff[2]);
	int16_t acc_y_Hex=(short)( ((short)buff[5]<<8) | buff[4]);
	int16_t acc_z_Hex=(short)( ((short)buff[7]<<8) | buff[6]);

	ACC_data.acc_x=acc_x_Hex / 32768.0f * 16.0f;
	ACC_data.acc_y=acc_y_Hex / 32768.0f * 16.0f;
	ACC_data.acc_z=acc_z_Hex / 32768.0f * 16.0f;
}
///////////////////////////////////////////////////////

////////////////////角速度/////////////////////////
typedef struct{
	//角速度
	float angle_v_x;
	float angle_v_y;
	float angle_v_z;
}Angle_v;
Angle_v Angle_v_data;
void Angle_v_frame(uint8_t *buff)
{
	int16_t angle_v_x_Hex=(short)( ((short)buff[3]<<8) | buff[2]);
	int16_t angle_v_y_Hex=(short)( ((short)buff[5]<<8) | buff[4]);
	int16_t angle_v_z_Hex=(short)( ((short)buff[7]<<8) | buff[6]);

	Angle_v_data.angle_v_x=angle_v_x_Hex / 32768.0f * 2000.0f;
	Angle_v_data.angle_v_y=angle_v_y_Hex / 32768.0f * 2000.0f;
	Angle_v_data.angle_v_z=angle_v_z_Hex / 32768.0f * 2000.0f;
}
///////////////////////////////////////////////////////

/////////////////////////////时间/////////////////////////
typedef struct{
	uint8_t Year;
	uint8_t Month;
	uint8_t Day;
	uint8_t Hour;
	uint8_t Minute;
	uint8_t Second;
	uint16_t MS;
}Time;
Time Time_Data;
void Time_frame(uint8_t *buff)//时间
{
	Time_Data.Year=buff[2];
	Time_Data.Month=buff[3];
	Time_Data.Day=buff[4];
	Time_Data.Hour=buff[5];
	Time_Data.Minute=buff[6];
	Time_Data.Second=buff[7];
	Time_Data.MS=((uint16_t)buff[9]<<8) | buff[8];
}
///////////////////////////////////////////////////////

uint8_t check_data(uint8_t *buff)//数据和校验
{
	uint8_t sum=0,i=0;
	for(i=0;i<10;i++)
	{
		sum += buff[i];
	}
	return sum;
}

