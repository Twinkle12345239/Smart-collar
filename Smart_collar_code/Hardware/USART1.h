#ifndef __USART1_H__
#define __USART1_H__

extern uint8_t USART1_RXFlag;
void USART1_IMU_Init(void);
void USART1_SendByte(uint8_t Byte);
void USART1_SendArray(uint8_t *Array,uint16_t Lenth);
void USART1_SendString(char *String);
void USART1_Printf(char *format, ...);
uint8_t USART1_GetRxFlag(void);

#endif
