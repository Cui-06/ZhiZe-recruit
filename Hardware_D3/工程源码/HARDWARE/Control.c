#include "Control.h"
#include "Adc.h"
#include "Key.h"
#include "Encoder.h"
#include "Motor.h"
#include "PID.h"

/* TIM2 每 10 ms 进入一次控制中断，即控制频率为 100 Hz。 */
#define CONTROL_FREQUENCY_HZ 100L
/* 每 2 个控制周期生成一次显示/串口数据，即 20 ms（50 Hz）。 */
#define REPORT_TICKS         2U
#define POSITION_MIN_COUNT   (-5000L)
#define POSITION_MAX_COUNT   5000L
#define SPEED_FILTER_SAMPLES 4U

/* 按钮选择目标速度档位，单位为 count/s。 */
static const int32_t TargetSpeedTable[4] =
{
    0L, 2000L, 4000L, 7000L
};

static PositionPID_TypeDef PositionPID;
static SpeedPID_TypeDef SpeedPID;
static volatile int32_t CurrentPosition = 0L;
static volatile int32_t CurrentSpeed = 0L;
static volatile int32_t TargetPosition = 0L;
static volatile int32_t TargetSpeed = 0L;
static volatile int32_t PIDOutput = 0L;
static volatile int32_t PositionError = 0L;
static volatile unsigned char ReportReady = 1U;
static int32_t PotentiometerValue = 0L;
static int32_t SpeedHistory[SPEED_FILTER_SAMPLES] = {0L, 0L, 0L, 0L};
static int32_t SpeedSum = 0L;
static unsigned char ReportTick = 0U;
static unsigned char SpeedLevel = 0U;
static unsigned char SpeedHistoryIndex = 0U;

static int32_t Control_MapPotentiometer(int32_t adc_value)
{
    /* 将 ADC 的 0~4095 线性映射为 -5000~+5000 count。 */
    return POSITION_MIN_COUNT +
           (adc_value * (POSITION_MAX_COUNT - POSITION_MIN_COUNT)) / 4095L;
}

static int32_t Control_LimitSigned(int32_t value, int32_t limit)
{
    if (value > limit) return limit;
    if (value < -limit) return -limit;
    return value;
}

static void Control_UpdateCascade(void)
{
    int32_t target_speed_limit = TargetSpeedTable[SpeedLevel];

    PositionError = TargetPosition - CurrentPosition;

    /* 停止档同时复位两级 PID，避免重新启动时残留积分造成冲击。 */
    if (target_speed_limit == 0L)
    {
        TargetSpeed = 0L;
        PIDOutput = 0L;
        PositionPID_Reset(&PositionPID);
        SpeedPID_Reset(&SpeedPID);
        Motor_Stop();
        return;
    }

    /* 外环：位置误差转换为目标速度，并受按钮档位限制。 */
    TargetSpeed = PositionPID_Update(&PositionPID,
                                     TargetPosition,
                                     CurrentPosition,
                                     CurrentSpeed);
    TargetSpeed = Control_LimitSigned(TargetSpeed, target_speed_limit);

    /* 内环：目标速度与实际速度比较，输出带符号 PWM。 */
    PIDOutput = (int32_t)SpeedPID_Update(&SpeedPID,
                                         TargetSpeed,
                                         CurrentSpeed);
    Motor_SetDuty((int16_t)PIDOutput);
}

static void Control_Tick(void)
{
    int16_t encoder_delta;
    int32_t raw_speed;

    /* TIM4 读出并清零，因此本次读数就是最近 10 ms 的位置增量。 */
    encoder_delta = Encoder_ReadA();
    CurrentPosition += encoder_delta;
    /* 10 ms 采样周期：增量乘 100 得到未经滤波的速度 count/s。 */
    raw_speed = (int32_t)encoder_delta * CONTROL_FREQUENCY_HZ;

    /*
     * 4 点滑动平均抑制编码器量化误差和电机干扰。
     * 相比直接使用单次增量，速度曲线更平滑且不会引入累计偏差。
     */
    SpeedSum -= SpeedHistory[SpeedHistoryIndex];
    SpeedHistory[SpeedHistoryIndex] = raw_speed;
    SpeedSum += raw_speed;
    SpeedHistoryIndex++;
    if (SpeedHistoryIndex >= SPEED_FILTER_SAMPLES) SpeedHistoryIndex = 0U;
    CurrentSpeed = SpeedSum / (int32_t)SPEED_FILTER_SAMPLES;

    /* ADC 驱动内部已进行 4 次平均，结果直接作为位置环目标。 */
    PotentiometerValue = (int32_t)Adc_ReadPotentiometer();
    TargetPosition = Control_MapPotentiometer(PotentiometerValue);

    /* 每次有效按下切换目标速度：停止、低速、中速、高速。 */
    if (Key_Read(0U) == KEY1_PRESS)
    {
        SpeedLevel++;
        if (SpeedLevel >= 4U) SpeedLevel = 0U;
    }

    Control_UpdateCascade();

    ReportTick++;
    if (ReportTick >= REPORT_TICKS)
    {
        ReportTick = 0U;
        ReportReady = 1U;
    }
}

void Control_Init(void)
{
    TIM_TimeBaseInitTypeDef timer;
    NVIC_InitTypeDef nvic;

    PositionPID_Init(&PositionPID);
    SpeedPID_Init(&SpeedPID);
    PotentiometerValue = (int32_t)Adc_ReadPotentiometer();
    TargetPosition = Control_MapPotentiometer(PotentiometerValue);
    Motor_Stop();

    /* TIM2：72 MHz / (71+1) / (9999+1) = 100 Hz。 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    timer.TIM_Prescaler = 71U;
    timer.TIM_Period = 9999U;
    timer.TIM_ClockDivision = TIM_CKD_DIV1;
    timer.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &timer);

    nvic.NVIC_IRQChannel = TIM2_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 1U;
    nvic.NVIC_IRQChannelSubPriority = 1U;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);

    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM2, ENABLE);
}

unsigned char Control_ReadStatus(ControlStatus_TypeDef *status)
{
    unsigned char ready = 0U;
    uint32_t primask = __get_PRIMASK();

    __disable_irq();
    if (ReportReady != 0U)
    {
        status->position = CurrentPosition;
        status->speed = CurrentSpeed;
        status->target_position = TargetPosition;
        status->target_speed = TargetSpeed;
        status->pid_output = PIDOutput;
        status->pid_error = PositionError;
        ReportReady = 0U;
        ready = 1U;
    }
    if (primask == 0U) __enable_irq();
    return ready;
}

void TIM2_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
        Control_Tick();
    }
}
