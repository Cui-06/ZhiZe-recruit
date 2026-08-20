#include "reg52.h"
#include "Seg.h"

#define SMG_A_DP_PORT P0

// 数码管段选使用 P0，位选使用 P2.2~P2.4。 
sbit LSA = P2^2;
sbit LSB = P2^3;
sbit LSC = P2^4;

static unsigned char Seg_Buf[8] = {SEG_BLANK, SEG_BLANK, SEG_BLANK, SEG_BLANK,
                                   SEG_BLANK, SEG_BLANK, SEG_BLANK, SEG_BLANK};
static unsigned char code Seg_Code[11] = {
    0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07,0x7f, 0x6f,0x00};

static void delay_10us(unsigned int ten_us) 
{
    while (ten_us--);
}

void Seg_Init(void)   // 清空数码管显示缓冲并关闭数码管。
{
    unsigned char i;

    for (i = 0; i < 8; i++)
    {
        Seg_Buf[i] = SEG_BLANK;
    }
    LSC = 1;
    LSB = 1;
    LSA = 1;
    SMG_A_DP_PORT = 0x00;
}

void Seg_SetBuf(unsigned char pos, unsigned char dat)  //设置指定数码管位置的显示内容
{
    if (pos >= 8)
    {
        return;
    }
    Seg_Buf[pos] = (dat <= SEG_BLANK) ? dat : SEG_BLANK;
}

void Seg_SetAll(const unsigned char *buf)  //批量设置 8 位数码管的显示缓冲
{
    unsigned char i;

    for (i = 0; i < 8; i++)
    {
        Seg_SetBuf(i, buf[i]);
    }
}

void Seg_Scan(unsigned char pos)  //选择某一位数码管并输出对应段码选择某一位数码管并输出对应段码
{
    if (pos >= 8)
    {
        return;
    }

    switch (pos)
    {
        case 0: LSC = 1; LSB = 1; LSA = 1; break;
        case 1: LSC = 1; LSB = 1; LSA = 0; break;
        case 2: LSC = 1; LSB = 0; LSA = 1; break;
        case 3: LSC = 1; LSB = 0; LSA = 0; break;
        case 4: LSC = 0; LSB = 1; LSA = 1; break;
        case 5: LSC = 0; LSB = 1; LSA = 0; break;
        case 6: LSC = 0; LSB = 0; LSA = 1; break;
        case 7: LSC = 0; LSB = 0; LSA = 0; break;
    }

    SMG_A_DP_PORT = Seg_Code[Seg_Buf[pos]];
}

void Seg_Display(void)  //快速循环扫描 8 位数码管，形成稳定显示
{
    unsigned char i;

    for (i = 0; i < 8; i++)
    {
        Seg_Scan(i);
        delay_10us(100);
        SMG_A_DP_PORT = 0x00;
    }
}
