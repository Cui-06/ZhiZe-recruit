#include "reg52.h"
#include "Motor.h"
#include "Beep.h"
#include "Seg.h"
#include "OLED.h"
#include "Uart.h"
#include "MatrixKey.h"
#include "Encoder.h"

#define PHONE_LEN 5
#define PHONE_KEY_NONE 0
#define PHONE_KEY_DIGIT_ZERO 16
#define PHONE_KEY_BACKSPACE 17
#define PHONE_KEY_DELETE_CONFIRM 18
#define PHONE_KEY_DIAL 19
#define COMM_IDLE 0
#define COMM_WAIT_POS 1
#define COMM_WAIT_OK 2
#define COMM_ACTIVE 3
#define RESCUE_TIME_TICKS 50000U
#define PHONE_BLINK_TICKS 3000U

static unsigned char Phone_Num[PHONE_LEN];
static unsigned char Phone_Count = 0;
static volatile unsigned char Phone_DeletePending = 0;
static unsigned char Phone_BlinkOn = 0;
static volatile unsigned int Phone_BlinkTick = 0;
static volatile unsigned char Phone_BlinkFlag = 0;
static unsigned char Comm_State = COMM_IDLE;
static unsigned char Comm_MatchIndex = 0;
static volatile unsigned char Rescue_Active = 0;
static volatile unsigned int Rescue_Tick = 0;

static unsigned char code MsgNeedHelp[] = {
    0xE9, 0x9C, 0x80, 0xE8, 0xA6, 0x81, 0xE6, 0x95,
    0x91, 0xE6, 0x8F, 0xB4, 0x0D, 0x0A, 0x00
};
static unsigned char code MsgReportPos[] = {
    0xE6, 0x8A, 0xA5, 0xE5, 0x91, 0x8A, 0xE4, 0xBD,
    0x8D, 0xE7, 0xBD, 0xAE, 0x00
};
static unsigned char code MsgPos01[] = {'0', '1', 0x0D, 0x0A, 0x00};
static unsigned char code MsgReceived[] = {
    0xE6, 0x94, 0xB6, 0xE5, 0x88, 0xB0, 0x00
};

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

    if (Rescue_Active)
    {
        if (Rescue_Tick < RESCUE_TIME_TICKS)
        {
            Rescue_Tick++;
        }
    }

    if (Phone_DeletePending)
    {
        if (Phone_BlinkTick < PHONE_BLINK_TICKS)
        {
            Phone_BlinkTick++;
        }
        else
        {
            Phone_BlinkTick = 0;
            Phone_BlinkFlag = 1;
        }
    }
    else
    {
        Phone_BlinkTick = 0;
        Phone_BlinkFlag = 0;
    }
}

static void Phone_UpdateDisplay(void)  //根据号码缓冲和删除状态更新 8 位数码管显示缓冲
{
    unsigned char i;

    for (i = 0; i < 8; i++)
    {
        Seg_SetBuf(i, SEG_BLANK);
    }

    for (i = 0; i < Phone_Count; i++)
    {
        if (Phone_DeletePending && i == Phone_Count - 1 && Phone_BlinkOn == 0)
        {
            Seg_SetBuf(i, SEG_BLANK);
        }
        else
        {
            Seg_SetBuf(i, Phone_Num[i]);
        }
    }
}

static void Phone_Clear(void)  //清空已输入号码，并关闭数码管显示
{
    Phone_Count = 0;
    Phone_DeletePending = 0;
    Phone_BlinkOn = 0;
    Phone_BlinkTick = 0;
    Phone_BlinkFlag = 0;
    Phone_UpdateDisplay();
}

static void Phone_InputDigit(unsigned char digit)  //将一个数字追加到号码中，最多允许输入 5 位
{
    if (Phone_Count >= PHONE_LEN)
    {
        return;
    }

    Phone_DeletePending = 0;
    Phone_Num[Phone_Count] = digit;
    Phone_Count++;
    Phone_BlinkOn = 0;
    Phone_BlinkTick = 0;
    Phone_BlinkFlag = 0;
    Phone_UpdateDisplay();
}

static void Phone_Backspace(void)  //选择号码最后一位作为待删除数字，并使其闪烁
{
    // S11 选择最后一位并使其闪烁，S12 确认删除 
    if (Phone_Count > 0)
    {
        Phone_DeletePending = 1;
        Phone_BlinkOn = 1;
    }
    Phone_BlinkTick = 0;
    Phone_BlinkFlag = 0;
    Phone_UpdateDisplay();
}

static void Phone_ConfirmDelete(void) //确认删除当前正在闪烁的最后一位数字
{
    if (Phone_DeletePending && Phone_Count > 0)
    {
        Phone_Count--;
    }
    Phone_DeletePending = 0;
    Phone_BlinkOn = 0;
    Phone_BlinkTick = 0;
    Phone_BlinkFlag = 0;
    Phone_UpdateDisplay();
}

static bit Phone_Is120(void)  ////判断当前输入的号码是否精确为 120
{
    return (Phone_Count == 3 &&
            Phone_Num[0] == 1 &&
            Phone_Num[1] == 2 &&
            Phone_Num[2] == 0);
}

static void Phone_BlinkProc(void)  //处理待删除数字的周期性闪烁
{
    if (Phone_BlinkFlag)
    {
        Phone_BlinkFlag = 0;
        if (Phone_DeletePending)
        {
            Phone_BlinkOn = !Phone_BlinkOn;
            Phone_UpdateDisplay();
        }
    }
}

static void Rescue_Stop(void)  //结束救援，停止电机和蜂鸣器并熄灭所有显示
{
    Rescue_Active = 0;
    Rescue_Tick = 0;
    Motor_SetEnable(0);
    Motor_SetSpeed(MOTOR_SPEED_STOP);
    Beep_SetEnable(0);
    OLED_SetScrollEnable(0);
    Phone_Clear();
    Comm_State = COMM_IDLE;
    Comm_MatchIndex = 0;
}

static void Rescue_Start(void)  //启动救援，同时开启电机、提示音和 OLED 滚动显示
{
    Motor_SetDir(MOTOR_DIR_FORWARD);
    Motor_SetEnable(1);
    Motor_SetSpeed(MOTOR_SPEED_HIGH);
    Beep_SetEnable(1);
    OLED_SetScrollEnable(1);
    Rescue_Tick = 0;
    Rescue_Active = 1;
    Comm_State = COMM_ACTIVE;
}

static void Phone_Dial(void)  
{
    Phone_DeletePending = 0;
    Phone_BlinkOn = 0;
    Phone_UpdateDisplay();

    // 只有号码精确为 120 时，才允许开始通信
    if (Phone_Is120())
    {
        Uart_ClearRx();
        Uart_SendBytes(MsgNeedHelp);
        Comm_State = COMM_WAIT_POS;
        Comm_MatchIndex = 0;
    }
}

static bit Comm_MatchByte(unsigned char rx, const unsigned char *target)  //逐字节匹配 PC 发来的目标消息
{
    if (rx == target[Comm_MatchIndex])
    {
        Comm_MatchIndex++;
        if (target[Comm_MatchIndex] == 0)
        {
            Comm_MatchIndex = 0;
            return 1;
        }
    }
    else
    {
        Comm_MatchIndex = (rx == target[0]) ? 1 : 0;
    }
    return 0;
}

static void Comm_Proc(void)  //执行串口通信状态机并根据 PC 回复推进救援流程
{
    unsigned char rx;

    // 一次处理当前缓冲区中的全部字节，避免连续中文回复滞留在接收缓冲区
    while (Uart_ReadByte(&rx))
    {
        // 通信状态：等待“报告位置” -> 发送“01” -> 等待“收到” -> 启动救援
        if (Comm_State == COMM_WAIT_POS)
        {
            if (Comm_MatchByte(rx, MsgReportPos))
            {
                Comm_State = COMM_WAIT_OK;
                Comm_MatchIndex = 0;
                Uart_SendBytes(MsgPos01);
            }
        }
        else if (Comm_State == COMM_WAIT_OK)
        {
            if (Comm_MatchByte(rx, MsgReceived))
            {
                Rescue_Start();
            }
        }
    }
}

static unsigned char MatrixKey_ToAction(unsigned char key)
{
    //S1-S9 输入 1-9，S10 输入 0，S11 选择删除，S12 确认删除，S16 拨号
    if (key >= 1 && key <= 9)
    {
        return key;
    }
    if (key == 10)
    {
        return PHONE_KEY_DIGIT_ZERO;
    }
    if (key == 11)
    {
        return PHONE_KEY_BACKSPACE;
    }
    if (key == 12)
    {
        return PHONE_KEY_DELETE_CONFIRM;
    }
    if (key == 16)
    {
        return PHONE_KEY_DIAL;
    }
    return PHONE_KEY_NONE;
}

static void MatrixKey_Proc(unsigned char key)  //执行数字输入、删除选择、确认删除或拨号操作
{
    unsigned char action = MatrixKey_ToAction(key);

    if (action <= 9 && action != PHONE_KEY_NONE)
    {
        Phone_InputDigit(action);
    }
    else if (action == PHONE_KEY_DIGIT_ZERO)
    {
        Phone_InputDigit(0);
    }
    else if (action == PHONE_KEY_BACKSPACE)
    {
        Phone_Backspace();
    }
    else if (action == PHONE_KEY_DELETE_CONFIRM)
    {
        Phone_ConfirmDelete();
    }
    else if (action == PHONE_KEY_DIAL)
    {
        Phone_Dial();
    }
}

void main(void) 
{
    unsigned char key = MATRIX_KEY_NONE;

    Seg_Init();
    Motor_Init();
    Beep_Init();
    OLED_Init();
    Uart_Init();
    Encoder_Init();
    Motor_SetDir(MOTOR_DIR_FORWARD);
    Motor_SetSpeed(MOTOR_SPEED_STOP);
    Beep_SetEnable(0);
    OLED_SetScrollEnable(0);
    Phone_Clear();
    Timer0Init();

    while (1)
    {
        Phone_BlinkProc();
        Seg_Display();
        OLED_Proc();
        Comm_Proc();

        if (Rescue_Active && Rescue_Tick >= RESCUE_TIME_TICKS)
        {
            Rescue_Stop();
        }

        if (Comm_State == COMM_IDLE)
        {
            key = MatrixKey_Read();
            if (key != MATRIX_KEY_NONE)
            {
                MatrixKey_Proc(key);
            }
        }
    }
}
