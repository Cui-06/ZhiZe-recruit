#ifndef KEY_H
#define KEY_H

#define KEY1_PRESS  1U
#define KEY_UNPRESS 0U

/* 读取一次按键事件；mode 非 0 时清除按键锁定状态。 */
unsigned char Key_Read(unsigned char mode);

#endif
