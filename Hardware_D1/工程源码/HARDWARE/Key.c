#include "Board.h"
#include "Key.h"
#include "delay.h"

unsigned char Key_Read(unsigned char mode)
{
    /* key_lock=1 表示允许检测，0 表示按键仍处于锁定状态。 */
    static unsigned char key_lock = 1U;
    uint16_t state;

    if (mode != 0U)
    {
        /* mode 非 0 时清除锁定，允许重新检测按键。 */
        key_lock = 1U;
    }

    state = GPIO_ReadInputData(KEY_PORT);
    if (key_lock == 1U && (state & KEY_MASK) != KEY_MASK)
    {
        /* 延时 10 ms 完成按键消抖。 */
        delay_ms(10);
        state = GPIO_ReadInputData(KEY_PORT);
        key_lock = 0U; /* 锁定本次按键，防止长按重复触发。 */
        if ((state & KEY1_PIN) == 0U) return KEY1_PRESS;
        if ((state & KEY2_PIN) == 0U) return KEY2_PRESS;
        if ((state & KEY3_PIN) == 0U) return KEY3_PRESS;
    }
    else if ((state & KEY_MASK) == KEY_MASK)
    {
        /* 所有按键释放后解锁，准备检测下一次按下。 */
        key_lock = 1U;
    }
    return KEY_UNPRESS;
}
