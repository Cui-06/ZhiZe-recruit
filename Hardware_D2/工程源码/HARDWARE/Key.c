#include "Board.h"
#include "Key.h"

unsigned char Key_Read(unsigned char mode)
{
    /* key_lock=1 表示允许检测，0 表示本次按下已经触发。 */
    static unsigned char key_lock = 1U;
    static unsigned char last_level = 1U;
    static unsigned char stable_count = 0U;
    unsigned char level;

    if (mode != 0U)
    {
        /* 与 D1 接口一致：mode 非 0 时允许重新检测。 */
        key_lock = 1U;
    }

    level = ((GPIO_ReadInputData(KEY_PORT) & KEY_MASK) == KEY_MASK) ? 1U : 0U;

    /* 控制中断每 10 ms 调用一次，连续采到相同电平完成消抖。 */
    if (level == last_level)
    {
        if (stable_count < 2U) stable_count++;
    }
    else
    {
        last_level = level;
        stable_count = 0U;
    }

    if (stable_count >= 1U)
    {
        if (key_lock == 1U && level == 0U)
        {
            key_lock = 0U;
            return KEY1_PRESS;
        }

        if (level != 0U)
        {
            /* 按键释放后解锁，下一次按下才会再次触发。 */
            key_lock = 1U;
        }
    }

    return KEY_UNPRESS;
}
