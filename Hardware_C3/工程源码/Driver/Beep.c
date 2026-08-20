#include "reg52.h"
#include "Beep.h"

sbit BEEP = P2^5;

static volatile unsigned char Beep_Enable = 0;
static volatile unsigned char Beep_Count = 0;
static volatile unsigned int Beep_ToneTick = 0;
static volatile unsigned char Beep_TonePhase = 0;

void Beep_Init(void)  //初始化蜂鸣器，并使其保持关闭
{
    BEEP = 0;
    Beep_Enable = 0;
    Beep_Count = 0;
    Beep_ToneTick = 0;
    Beep_TonePhase = 0;
}

void Beep_SetEnable(unsigned char enable)  //开启或关闭蜂鸣器
{
    Beep_Enable = enable ? 1 : 0;
    if (Beep_Enable == 0)
    {
        BEEP = 0;
        Beep_Count = 0;
        Beep_ToneTick = 0;
        Beep_TonePhase = 0;
    }
}

void Beep_Tick(void)  
{
    if (Beep_Enable == 0)
    {
        BEEP = 0;
        return;
    }

    // 每 250 ms 交替切换两种频率，形成“滴露滴露”救援提示音
    Beep_ToneTick++;
    if (Beep_ToneTick >= 2500)
    {
        Beep_ToneTick = 0;
        Beep_TonePhase = !Beep_TonePhase;
        Beep_Count = 0;
    }

    Beep_Count++;
    if (Beep_Count >= (Beep_TonePhase ? 7 : 4))
    {
        Beep_Count = 0;
        BEEP = !BEEP;
    }
}
