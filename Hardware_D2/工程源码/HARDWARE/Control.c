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
/* 电位器两端分别对应的目标位置，单位为编码器计数 count。 */
#define POSITION_MIN_COUNT   (-5000L)
#define POSITION_MAX_COUNT   5000L

/* 按钮依次切换：停止、低速、中速、高速，对应 PID/PWM 输出上限。 */
static const int32_t OutputLimitTable[4] =
{
    0L, 200L, 400L, 700L
};

static PositionPID_TypeDef PositionPID;             /* 单环位置 PID 的历史状态。 */
static volatile int32_t CurrentPosition = 0L;       /* 编码器累计得到的实际位置。 */
static volatile int32_t CurrentSpeed = 0L;          /* 最近 20 ms 的平均速度。 */
static volatile int32_t TargetPosition = 0L;        /* 电位器直接给出的位置终点。 */
static volatile int32_t PIDError = 0L;              /* SerialPlot 显示的位置误差。 */
static volatile int32_t PIDOutput = 0L;             /* 带符号 PWM，范围 -999~999。 */
static volatile int32_t OutputLimit = 0L;           /* 按钮选择的 PID/PWM 输出上限。 */
static volatile unsigned char ReportReady = 1U;    /* 置 1 后主循环读取一次快照。 */
static int32_t ReportDelta = 0L;                    /* 测速窗口内累计的编码器增量。 */
static unsigned char ReportTick = 0U;
static unsigned char SpeedLevel = 0U;

static int32_t Control_MapPotentiometer(int32_t adc_value)
{
    /* 将 ADC 的 0~4095 线性映射为 -5000~+5000 count。 */
    return POSITION_MIN_COUNT +
           (adc_value * (POSITION_MAX_COUNT - POSITION_MIN_COUNT)) / 4095L;
}

static int32_t Control_LimitOutput(int32_t output, int32_t limit)
{
    if (output > limit) return limit;
    if (output < -limit) return -limit;
    return output;
}

static void Control_UpdateMotor(void)
{
    OutputLimit = OutputLimitTable[SpeedLevel];
    PIDError = TargetPosition - CurrentPosition;

    /* 停止档关闭 PWM，并清除 PID 历史量，避免再次启动时积分突跳。 */
    if (OutputLimit == 0L)
    {
        PIDOutput = 0L;
        PositionPID_Reset(&PositionPID);
        Motor_Stop();
        return;
    }

    /*
     * 电位器目标位置直接送入单环位置 PID；按钮所选档位只限制
     * 控制量的最大绝对值，从而限制电机运动速度。
     */
    PIDOutput = (int32_t)PositionPID_Update(&PositionPID,
                                            TargetPosition,
                                            CurrentPosition);
    PIDOutput = Control_LimitOutput(PIDOutput, OutputLimit);
    Motor_SetDuty((int16_t)PIDOutput);
}

static void Control_Tick(void)
{
    int16_t encoder_delta;
    int32_t potentiometer;

    /* TIM4 每次读出并清零，因此本次读数就是最近 10 ms 的位置增量。 */
    encoder_delta = Encoder_ReadA();
    CurrentPosition += encoder_delta;
    ReportDelta += encoder_delta;

    /* ADC 驱动内部已进行 4 次平均，结果直接映射为 PID 目标位置。 */
    potentiometer = (int32_t)Adc_ReadPotentiometer();
    TargetPosition = Control_MapPotentiometer(potentiometer);

    /* 每次有效按下切换一个速度档，第四次重新回到停止档。 */
    if (Key_Read(0U) == KEY1_PRESS)
    {
        SpeedLevel++;
        if (SpeedLevel >= 4U) SpeedLevel = 0U;
    }

    Control_UpdateMotor();

    /* 每 20 ms 统计一次速度，并通知主循环输出 SerialPlot/OLED 数据。 */
    ReportTick++;
    if (ReportTick >= REPORT_TICKS)
    {
        ReportTick = 0U;
        CurrentSpeed = ReportDelta * (1000L /
                       (REPORT_TICKS * (1000L / CONTROL_FREQUENCY_HZ)));
        ReportDelta = 0L;
        ReportReady = 1U;
    }
}

void Control_Init(void)
{
    TIM_TimeBaseInitTypeDef timer;
    NVIC_InitTypeDef nvic;

    PositionPID_Init(&PositionPID);
    TargetPosition = Control_MapPotentiometer((int32_t)Adc_ReadPotentiometer());
    Motor_Stop();

    /* TIM2：72 MHz / (71+1) / (9999+1) = 100 Hz。 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    timer.TIM_Prescaler = 71U; /* 72 MHz / 72 = 1 MHz。 */
    timer.TIM_Period = 9999U;  /* 1 MHz / 10000 = 100 Hz。 */
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

    /*
     * 这些状态由 TIM2 中断更新。短暂关闭中断可保证主循环取得的是
     * 同一控制时刻的完整快照，而不是新旧数据混合。
     */
    __disable_irq();
    if (ReportReady != 0U)
    {
        status->position = CurrentPosition;
        status->speed = CurrentSpeed;
        status->pid_target_position = TargetPosition;
        status->pid_error = PIDError;
        status->pid_output = PIDOutput;
        status->target_position = TargetPosition;
        status->output_limit = OutputLimit;
        ReportReady = 0U;
        ready = 1U;
    }
    if (primask == 0U) __enable_irq();
    return ready;
}

void TIM2_IRQHandler(void)
{
    /* 100 Hz 定时中断是采样、按键扫描和位置 PID 的统一时间基准。 */
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
        Control_Tick();
    }
}
