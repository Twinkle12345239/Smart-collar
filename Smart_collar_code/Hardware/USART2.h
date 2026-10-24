#ifndef __USART2_H__
#define __USART2_H__

#include <stdio.h>

void USART2_Init(void);
void USART2_SendByte(uint8_t Byte);
void USART2_SendArray(uint8_t *Array,uint16_t Lenth);
void USART2_SendString(char *String);
void USART2_Printf(char *format, ...);

#endif
