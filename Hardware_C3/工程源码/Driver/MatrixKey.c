#include "reg52.h"
#include "MatrixKey.h"

#define KEY_MATRIX_PORT P1

static void delay_10us(unsigned int ten_us)
{
    while (ten_us--);
}

static unsigned char MatrixKey_ScanRaw(void)  //逐列扫描矩阵键盘，并返回原始的 S1~S16 键值
{
    KEY_MATRIX_PORT = 0xf7;
    switch (KEY_MATRIX_PORT)
    {
        case 0x77: return 1;
        case 0xb7: return 5;
        case 0xd7: return 9;
        case 0xe7: return 13;
    }

    KEY_MATRIX_PORT = 0xfb;
    switch (KEY_MATRIX_PORT)
    {
        case 0x7b: return 2;
        case 0xbb: return 6;
        case 0xdb: return 10;
        case 0xeb: return 14;
    }

    KEY_MATRIX_PORT = 0xfd;
    switch (KEY_MATRIX_PORT)
    {
        case 0x7d: return 3;
        case 0xbd: return 7;
        case 0xdd: return 11;
        case 0xed: return 15;
    }

    KEY_MATRIX_PORT = 0xfe;
    switch (KEY_MATRIX_PORT)
    {
        case 0x7e: return 4;
        case 0xbe: return 8;
        case 0xde: return 12;
        case 0xee: return 16;
    }

    KEY_MATRIX_PORT = 0xff;
    return MATRIX_KEY_NONE;
}

unsigned char MatrixKey_Read(void)  //读取一次经过按下确认和松手检测的矩阵按键
{
    static unsigned char last_key = MATRIX_KEY_NONE;
    static unsigned char key_ready = 1;
    unsigned char key_value;

    key_value = MatrixKey_ScanRaw();
    if (key_value == MATRIX_KEY_NONE)
    {
        last_key = MATRIX_KEY_NONE;
        key_ready = 1;
        return MATRIX_KEY_NONE;
    }

    if (key_value == last_key)
    {
        if (key_ready)
        {
            key_ready = 0;
            return key_value;
        }
    }
    else
    {
        last_key = key_value;
    }

    return MATRIX_KEY_NONE;
}

unsigned char MatrixKey_RanksScan(void)  //使用行列逐列扫描方式读取矩阵按键，并等待按键松开
{
    unsigned char key_value = MATRIX_KEY_NONE;

    KEY_MATRIX_PORT = 0xf7;
    if (KEY_MATRIX_PORT != 0xf7)
    {
        delay_10us(1000);
        switch (KEY_MATRIX_PORT)
        {
            case 0x77: key_value = 1; break;
            case 0xb7: key_value = 5; break;
            case 0xd7: key_value = 9; break;
            case 0xe7: key_value = 13; break;
        }
    }
    while (KEY_MATRIX_PORT != 0xf7);

    KEY_MATRIX_PORT = 0xfb;
    if (KEY_MATRIX_PORT != 0xfb)
    {
        delay_10us(1000);
        switch (KEY_MATRIX_PORT)
        {
            case 0x7b: key_value = 2; break;
            case 0xbb: key_value = 6; break;
            case 0xdb: key_value = 10; break;
            case 0xeb: key_value = 14; break;
        }
    }
    while (KEY_MATRIX_PORT != 0xfb);

    KEY_MATRIX_PORT = 0xfd;
    if (KEY_MATRIX_PORT != 0xfd)
    {
        delay_10us(1000);
        switch (KEY_MATRIX_PORT)
        {
            case 0x7d: key_value = 3; break;
            case 0xbd: key_value = 7; break;
            case 0xdd: key_value = 11; break;
            case 0xed: key_value = 15; break;
        }
    }
    while (KEY_MATRIX_PORT != 0xfd);

    KEY_MATRIX_PORT = 0xfe;
    if (KEY_MATRIX_PORT != 0xfe)
    {
        delay_10us(1000);
        switch (KEY_MATRIX_PORT)
        {
            case 0x7e: key_value = 4; break;
            case 0xbe: key_value = 8; break;
            case 0xde: key_value = 12; break;
            case 0xee: key_value = 16; break;
        }
    }
    while (KEY_MATRIX_PORT != 0xfe);

    return key_value;
}

unsigned char MatrixKey_FlipScan(void)  //使用线翻转扫描方式读取矩阵按键
{
    static unsigned char key_value = MATRIX_KEY_NONE;

    KEY_MATRIX_PORT = 0x0f;
    if (KEY_MATRIX_PORT != 0x0f)
    {
        delay_10us(1000);
        if (KEY_MATRIX_PORT != 0x0f)
        {
            KEY_MATRIX_PORT = 0x0f;
            switch (KEY_MATRIX_PORT)
            {
                case 0x07: key_value = 1; break;
                case 0x0b: key_value = 2; break;
                case 0x0d: key_value = 3; break;
                case 0x0e: key_value = 4; break;
            }

            KEY_MATRIX_PORT = 0xf0;
            switch (KEY_MATRIX_PORT)
            {
                case 0x70: key_value = key_value; break;
                case 0xb0: key_value = key_value + 4; break;
                case 0xd0: key_value = key_value + 8; break;
                case 0xe0: key_value = key_value + 12; break;
            }
            while (KEY_MATRIX_PORT != 0xf0);
        }
    }
    else
    {
        key_value = MATRIX_KEY_NONE;
    }

    return key_value;
}
