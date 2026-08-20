#include "reg52.h"
#include "Uart.h"

#define UART_RX_SIZE 32

static volatile unsigned char Uart_RxBuf[UART_RX_SIZE];
static volatile unsigned char Uart_RxHead = 0;
static volatile unsigned char Uart_RxTail = 0;

void Uart_Init(void) //按照串口实验例程初始化串口
{
    TMOD |= 0x20;
    SCON = 0x50;
    PCON = 0x80;
    TH1 = 0xFA;
    TL1 = 0xFA;
    ES = 1;
    EA = 1;
    TR1 = 1;
    TI = 0;
    RI = 0;
    Uart_RxHead = 0;
    Uart_RxTail = 0;
}

void Uart_ISR(void) interrupt 4
{
    unsigned char next_head;

    if (RI)
    {
        RI = 0;
        next_head = Uart_RxHead + 1;
        if (next_head >= UART_RX_SIZE)
        {
            next_head = 0;
        }
        if (next_head != Uart_RxTail)
        {
            Uart_RxBuf[Uart_RxHead] = SBUF;
            Uart_RxHead = next_head;
        }
    }

    if (TI)
    {
        TI = 0;
    }
}

void Uart_SendByte(unsigned char dat)  //通过串口发送一个字节
{
    ES = 0;
    TI = 0;
    SBUF = dat;
    while (TI == 0);
    TI = 0;
    ES = 1;
}

void Uart_SendBytes(const unsigned char *dat)  //通过串口连续发送以 0 结尾的字节串
{
    while (*dat != 0)
    {
        Uart_SendByte(*dat);
        dat++;
    }
}

unsigned char Uart_ReadByte(unsigned char *dat)  //从串口环形缓冲区读取一个字节
{
    if (Uart_RxHead == Uart_RxTail)
    {
        return 0;
    }

    *dat = Uart_RxBuf[Uart_RxTail];
    Uart_RxTail++;
    if (Uart_RxTail >= UART_RX_SIZE)
    {
        Uart_RxTail = 0;
    }
    return 1;
}

void Uart_ClearRx(void)  //清空串口接收缓冲和硬件接收标志
{
    Uart_RxTail = Uart_RxHead;
    RI = 0;
}
