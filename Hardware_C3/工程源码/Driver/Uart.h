#ifndef UART_H
#define UART_H

void Uart_Init(void);
void Uart_SendByte(unsigned char dat);
void Uart_SendBytes(const unsigned char *dat);
unsigned char Uart_ReadByte(unsigned char *dat);
void Uart_ClearRx(void);

#endif
