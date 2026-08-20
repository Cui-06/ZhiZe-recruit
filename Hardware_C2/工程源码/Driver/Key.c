#include "reg52.h"
#include "Key.h"

// 独立按键均为低电平按下
sbit KEY1 = P3^1;
sbit KEY2 = P3^0;
sbit KEY3 = P3^2;
sbit KEY4 = P3^3;

static void delay_10us(unsigned int ten_us)
{
    while (ten_us--);
}

unsigned char Key_Read(unsigned char mode)   //读取一次独立按键，返回被按下的按键编号
{
    static unsigned char key = 1;

    if (mode)
    {
        key = 1;  //清除按键锁定状态，允许重新检测按键
    }

    if (key == 1 && (KEY1 == 0 || KEY2 == 0 || KEY3 == 0 || KEY4 == 0))
    {
        delay_10us(1000);  //延时约 10 ms，完成按键消抖
        key = 0;  //暂时锁定按键，防止按键被长按时重复触发
        if (KEY1 == 0)
            return KEY1_PRESS;
        if (KEY2 == 0)
            return KEY2_PRESS;
        if (KEY3 == 0)
            return KEY3_PRESS;
        if (KEY4 == 0)
            return KEY4_PRESS;
    }
    else if (KEY1 == 1 && KEY2 == 1 && KEY3 == 1 && KEY4 == 1)
    {
        key = 1;
    }

    return KEY_UNPRESS;
}
