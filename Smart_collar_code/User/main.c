#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "USART2.h"
#include "USART1.h"
#include "IMU.h"

uint8_t IMU_RxData,IMU_Rx_buff[11],Rx_Count=0;
uint8_t usart2_rxflag,data_flag=0,chart_flag=0;
int main(void)
{	
	USART2_Init();
	USART1_IMU_Init();
	
	Acc_Cali();//加速度计校准
	Gyr_Cali();//陀螺仪校准
	Angle_Cali();//角度校准
	Head_Cali();//航向校准(z轴)
	Time_Cali();//时间校准

	while (1)
	{
		if(data_flag==1 && chart_flag==0)
		{
			USART2_Printf("=== WTGAHRS3-TTL 数据  ===\r\n");
			USART2_Printf("GPS时间: 20%02d-%02d-%02d %02d:%02d:%02d\r\n",
						   Time_Data.Year, Time_Data.Month, Time_Data.Day,
						   Time_Data.Hour, Time_Data.Minute, Time_Data.Second);
			USART2_Printf("加速度：X=%.4fg | Y=%.4fg | Z=%.4fg \r\n", 
						   ACC_data.acc_x,ACC_data.acc_y,ACC_data.acc_z);
			USART2_Printf("角速度：X=%.4f°/s | Y=%.4f°/s | Z=%.4f°/s\r\n", 
						   Angle_v_data.angle_v_x,Angle_v_data.angle_v_y,Angle_v_data.angle_v_z);
			USART2_Printf("角度：Roll=%.2f° | Pitch=%.2f° | Yaw=%.2f°\r\n", 
						   Sensor_Data.Roll, Sensor_Data.Pitch, Sensor_Data.Yaw);
			USART2_Printf("经纬度：经度=%.6f° | 纬度=%.6f°\r\n", 
						   Sensor_Data.Lon, Sensor_Data.Lat);
			USART2_Printf("GPS：海拔=%.3fm | 航向角=%.3f° | 地速=%.3fkm/h \r\n", 
						   GPS_Data.GPS_Height,GPS_Data.GPS_Yaw,GPS_Data.GPS_V);
			USART2_Printf("==========================\r\n\r\n");
			Delay_ms(2000);
		}
		else if(chart_flag==1 && data_flag==0)
		{

		}
	}
}

void USART1_IRQHandler(void)
{
	if(USART_GetITStatus(USART1,USART_IT_RXNE)==SET)//若接收函数标志位为非空，则开始接收数据
	{
		IMU_RxData=USART_ReceiveData(USART1);
		
		uint8_t current_data_flag = data_flag;
        uint8_t current_chart_flag = chart_flag;
		
		if(current_data_flag==1)
		{
			if(Rx_Count==0 && IMU_RxData!=0x55)
			{
				return ;
			}
			IMU_Rx_buff[Rx_Count++]=IMU_RxData;
			if(Rx_Count>=11)
			{
				if(check_data(IMU_Rx_buff) == IMU_Rx_buff[10])
				{
					switch(IMU_Rx_buff[1])
					{
						case 0x50:
							Time_frame(IMU_Rx_buff);//时间
							break;
						case 0x51:
							ACC_frame(IMU_Rx_buff);//加速度
							break;
						case 0x52:
							Angle_v_frame(IMU_Rx_buff);//角速度
							break;					
						case 0x53:
							Angle_frame(IMU_Rx_buff);//角度
							break;
						case 0x57:
							LonLat_frame(IMU_Rx_buff);//经纬度
							break;
						case 0x58:
							GPS_frame(IMU_Rx_buff);//GPS数据
							break;
					}	
					USART1_RXFlag=1; 
				}
				Rx_Count=0;
			}
		}
		else if(current_chart_flag==1)
		{
			USART2_SendByte(IMU_RxData);
		}
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);//清除接收标志位
	}
}
void USART2_IRQHandler(void)
{
	if(USART_GetITStatus(USART2,USART_IT_RXNE)==SET)
	{
		usart2_rxflag=USART_ReceiveData(USART2);
		
		switch(usart2_rxflag)
        {
            case 0x01:  // 切换到数据模式
                    chart_flag = 0;
                    data_flag = 1;
                    Rx_Count = 0;  // 重要：重置帧计数器
                break;
                
            case 0x0A:  // 切换到图表模式
                    data_flag = 0;
                    chart_flag = 1;
                    Rx_Count = 0;  // 重置帧计数器
                break;
        }		
		USART_ClearITPendingBit(USART2,USART_IT_RXNE);//清除接收标志位
	}
}











