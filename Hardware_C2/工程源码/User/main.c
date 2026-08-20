#include "reg52.h"
#include "Key.h"
#include "Led.h"
#include "Motor.h"
#include "Beep.h"
#include "Seg.h"
#include "OLED.h"

static void Timer0Init(void)
{
    TMOD &= 0xF0;
    TMOD |= 0x01;
    TH0 = 0xFF;
    TL0 = 0x38;
    TF0 = 0;
    ET0 = 1;
    EA = 1;
    TR0 = 1;
}

void Timer0Server(void) interrupt 1
{
    TF0 = 0;
    TH0 = 0xFF;
    TL0 = 0x38;
    Motor_Tick();
    Beep_Tick();
}

void main(void)
{
    unsigned char key = 0;
    unsigned char speed = MOTOR_SPEED_STOP;
    unsigned char dir = MOTOR_DIR_FORWARD;

    // 初始状态：电机停止、蜂鸣器关闭、LED 熄灭、数码管熄灭、OLED 熄灭。 
    Led_Init();
    Seg_Init();
    Motor_Init();
    Beep_Init();
    OLED_Init();
    Motor_SetDir(dir);
    Motor_SetSpeed(speed);
    Timer0Init();

    while (1)
    {
        key = Key_Read(0);
        if (key == KEY1_PRESS)  //KEY1 循环切换停止、低速、中速和高速
        {
            speed++;
            if (speed > MOTOR_SPEED_HIGH)
            {
                speed = MOTOR_SPEED_STOP;
            }
            Motor_SetSpeed(speed);
            // 高速时开启蜂鸣器和OLED滚动显示
            Beep_SetEnable(speed == MOTOR_SPEED_HIGH);
            OLED_SetScrollEnable(speed == MOTOR_SPEED_HIGH);
        }
        else if (key == KEY2_PRESS)  //KEY2 设置电机正转
        {
            dir = MOTOR_DIR_FORWARD;  
            Motor_SetDir(dir);
        }
        else if (key == KEY3_PRESS)   //KEY3 设置电机反转
        {
            dir = MOTOR_DIR_REVERSE;
            Motor_SetDir(dir);
        }

        OLED_Proc();
    }
}
