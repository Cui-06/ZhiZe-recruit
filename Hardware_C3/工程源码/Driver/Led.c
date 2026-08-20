#include "reg52.h"
#include "Led.h"

sbit LED = P2^0;

/* 函数作用：初始化 LED，并使其保持熄灭。 */
void Led_Init(void)
{
    LED = 1;
}

/* 函数作用：翻转 LED 当前的亮灭状态。 */
void Led_Toggle(void)
{
    LED = !LED;
}
