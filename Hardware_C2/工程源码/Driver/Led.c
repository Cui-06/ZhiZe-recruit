#include "reg52.h"
#include "Led.h"

sbit LED = P2^0;

void Led_Init(void)
{
    LED = 1;
}

void Led_Toggle(void)
{
    LED = !LED;
}
