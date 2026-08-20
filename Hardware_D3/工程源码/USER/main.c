#include "stm32f10x.h"
#include "Board.h"
#include "Motor.h"
#include "Encoder.h"
#include "Adc.h"
#include "OLED.h"
#include "Control.h"
#include "usart.h"
#include "delay.h"

int main(void)
{
    ControlStatus_TypeDef status;
    unsigned char oled_refresh_count = 0U;

    /* 先初始化全部外设，最后启动 TIM2 控制中断。 */
    SystemInit();
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    delay_init();
    Board_Init();
    Motor_Init();
    Encoder_Init();
    Adc_Init();
    OLED_Init();
    uart_init(115200U);
    OLED_ShowStatus(0L, 0L, 0L, 0L);
    Control_Init();

    while (1)
    {
        /* 控制在 TIM2 中断中运行，主循环只负责低优先级的数据输出。 */
        if (Control_ReadStatus(&status) != 0U)
        {
            /*
             * SerialPlot 六列 CSV：目标位置、实际位置、目标速度、实际速度、
             * PWM 输出和位置误差，用于观察改进串级 PID 的跟随与到位质量。
             */
            printf("%ld,%ld,%ld,%ld,%ld,%ld\r\n",
                   (long)status.target_position,
                   (long)status.position,
                   (long)status.target_speed,
                   (long)status.speed,
                   (long)status.pid_output,
                   (long)status.pid_error);

            /* 串口约 50 Hz 输出；OLED 每 5 次再刷新，即约 10 Hz。 */
            oled_refresh_count++;
            if (oled_refresh_count >= 5U)
            {
                oled_refresh_count = 0U;
                OLED_ShowStatus(status.position,
                                status.speed,
                                status.target_position,
                                status.target_speed);
            }
        }
    }
}
