#include "stm32f10x.h"                  // Device header

#ifndef __IMU_H__
#define __IMU_H__

void Acc_Cali(void);//加速度计校准
void Gyr_Cali(void);//陀螺仪校准
void Angle_Cali(void);//角度校准
void Head_Cali(void);//航向校准(z轴)
void Time_Cali(void);//时间校准

typedef struct {
	//角度数据
	float Roll;//横滚角
	float Pitch;//俯仰角
	float Yaw;//偏航角
	//经纬度数据
	float Lon;//经度
	float Lat;//纬度
}IMU_Data;
extern IMU_Data Sensor_Data;
//typedef struct {
//	float Lon_xiaoshu;//经度
//	float Lat_xiaoshu;//纬度
//}IMU_Data_shuchu;
//extern IMU_Data_shuchu jiexi;
void Angle_frame(uint8_t *buff);//角度数据
void LonLat_frame(uint8_t *buff);//经纬度数据

typedef struct {
	//GPS海拔
	float GPS_Height;
	//GPS航向
	float GPS_Yaw;
	//GPS地速
	float GPS_V;
}GPS;
extern GPS GPS_Data;
void GPS_frame(uint8_t *buff);//GPS数据（海拔、地速、航向角）

typedef struct{
	//加速度
	float acc_x;
	float acc_y;
	float acc_z;
}ACC;
extern ACC ACC_data;
void ACC_frame(uint8_t *buff);

typedef struct{
	//角速度
	float angle_v_x;
	float angle_v_y;
	float angle_v_z;
}Angle_v;
extern Angle_v Angle_v_data;
void Angle_v_frame(uint8_t *buff);

typedef struct{
	uint8_t Year;
	uint8_t Month;
	uint8_t Day;
	uint8_t Hour;
	uint8_t Minute;
	uint8_t Second;
	uint16_t MS;
}Time;
extern Time Time_Data;
void Time_frame(uint8_t *buff);//时间

uint8_t check_data(uint8_t *buff);//数据和校验


#endif
