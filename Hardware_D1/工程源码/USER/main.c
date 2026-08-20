#include "stm32f10x.h"
#include "Board.h"
#include "Key.h"
#include "Motor.h"
#include "Encoder.h"
#include "Adc.h"
#include "usart.h"
#include "delay.h"

#define MOTOR_REPORT_PERIOD_MS 100U

static void Motor_ReportStatus(int32_t *position_a)
{
    int16_t delta_a = Encoder_ReadA();
    int32_t speed_a;

    *position_a += delta_a;
    speed_a = ((int32_t)delta_a * 1000L) /
              (int32_t)MOTOR_REPORT_PERIOD_MS;

    printf("Motor A Pos=%ld Speed=%ld count/s\r\n",
           (long)*position_a, (long)speed_a);
}

int main(void)
{
    /* 当前按键编号、速度档位和电机方向。 */
    unsigned char key;
    unsigned char speed = MOTOR_SPEED_STOP;
    unsigned char dir = MOTOR_DIR_FORWARD;
    int32_t position_a = 0L;

    /* 初始化系统时钟、按键、电机、编码器和电源电压采样。 */
    SystemInit();
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    delay_init();
    Board_Init();
    Motor_Init();
    Motor_SetDir(dir);
    Motor_SetSpeed(speed);
    Encoder_Init();
    Adc_Init();
    uart_init(115200U);
    printf("Motor monitor ready\r\n");

    while (1)
    {
        key = Key_Read(0);
        if (key == KEY1_PRESS) 
        {
            speed++;
            if (speed > MOTOR_SPEED_HIGH) speed = MOTOR_SPEED_STOP;
            Motor_SetSpeed(speed);
        }
        else if (key == KEY2_PRESS) /* Key2：正转模式。 */
        {
            dir = MOTOR_DIR_FORWARD;
            Motor_SetDir(dir);
        }
        else if (key == KEY3_PRESS) /* Key3：反转模式。 */
        {
            dir = MOTOR_DIR_REVERSE;
            Motor_SetDir(dir);
        }

        Motor_ReportStatus(&position_a);
        delay_ms(MOTOR_REPORT_PERIOD_MS);
    }
}
