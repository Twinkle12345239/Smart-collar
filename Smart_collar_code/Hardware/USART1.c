#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>

uint8_t USART1_RXFlag;
/**
  * @brief	惯导模块串口初始化
  * @param  USART1(PA9-TX、PA10-RX)、波特率9600、8位数据位、无校验位、停止位1位
  */
void USART1_IMU_Init(void)
{
	//1.开启RCC时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	
	//2.初始化GPIO口
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	//3.初始化USART1(接收发送模式)
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate=9600;
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStructure.USART_Parity=USART_Parity_No;
	USART_InitStructure.USART_StopBits=USART_StopBits_1;
	USART_InitStructure.USART_WordLength=USART_WordLength_8b;
	USART_Init(USART1,&USART_InitStructure);
	
	//4.使用中断
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	
	//5.设置优先级
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//优先级分组,抢占优先级，响应优先级
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel=USART1_IRQn;//中断通道
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;
	NVIC_Init(&NVIC_InitStructure);
	
	//6.使能USART1
	USART_Cmd(USART1,ENABLE);
}

/**
  * @brief	串口发送一个字节
  * @param  Byte要发送的字节
  */
void USART1_SendByte(uint8_t Byte)
{
	USART_SendData(USART1,Byte);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);//等待发送寄存器为空
}

/**
  * @brief	串口发送一个数组
  * @param  Array要发送的数组，Lenth发送数组的长度
  */
void USART1_SendArray(uint8_t *Array,uint16_t Lenth)
{
	uint16_t i=0;
	for(i=0;i<Lenth;i++)
	{
		USART1_SendByte(Array[i]);
	}
}

/**
  * @brief	发送字符串函数
  */
void USART1_SendString(char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i ++)//遍历字符数组（字符串），遇到字符串结束标志位后停止
	{
		USART1_SendByte(String[i]);		//依次调用Serial_SendByte发送每个字节数据
	}
}

/**
  * @brief	printf打印
  */
void USART1_Printf(char *format, ...)
{
	char String[100];				//定义字符数组
	va_list arg;					//定义可变参数列表数据类型的变量arg
	va_start(arg, format);			//从format开始，接收参数列表到arg变量
	vsprintf(String, format, arg);	//使用vsprintf打印格式化字符串和参数列表到字符数组中
	va_end(arg);					//结束变量arg
	USART1_SendString(String);		//串口发送字符数组（字符串）
}

/**
  * @brief	串口接收数据标志位函数
  * @retval 若接收到数据，函数返回1
  */
uint8_t USART1_GetRxFlag(void)
{
	if (USART1_RXFlag == 1)
	{
		USART1_RXFlag = 0;
		return 1;
	}
	return 0;
}
