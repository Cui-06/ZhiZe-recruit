#include "reg52.h"
#include "OLED.h"

#define OLED_ADDR 0x78
#define OLED_WIDTH 128
#define OLED_TEXT_COUNT 5
#define OLED_FONT_WIDTH 16
#define OLED_FONT_SPACE 2
#define OLED_TEXT_WIDTH (OLED_TEXT_COUNT * (OLED_FONT_WIDTH + OLED_FONT_SPACE))
#define OLED_TEXT_START_LEFT (OLED_WIDTH - 1)
#define OLED_SCROLL_STEP 1

sbit OLED_SCL = P1^2;
sbit OLED_SDA = P1^3;

static unsigned char OLED_Enable = 0;
static unsigned char OLED_LastEnable = 0;
static int OLED_TextLeft = OLED_TEXT_START_LEFT;

static unsigned char code OLED_TextFont[OLED_TEXT_COUNT][32] = {
    // 五个汉字的 16x16 点阵字模，显示“急救请避让”。
    {
        0x08, 0x18, 0x8C, 0x06, 0xAC, 0x00, 0xAA, 0x00,
        0xAB, 0x1E, 0xAB, 0x20, 0xAA, 0x21, 0xAA, 0x23,
        0xAA, 0x26, 0xAE, 0x20, 0xAA, 0x20, 0xA8, 0x30,
        0xA8, 0x1C, 0xF8, 0x03, 0x00, 0x0C, 0x00, 0x18
    },
    {
        0x00, 0x00, 0x1A, 0x26, 0x32, 0x23, 0x02, 0x31,
        0xFF, 0x1F, 0xA2, 0x01, 0x32, 0x03, 0x12, 0x32,
        0x70, 0x10, 0x7C, 0x18, 0x87, 0x0F, 0x04, 0x06,
        0x84, 0x0F, 0x7C, 0x18, 0x04, 0x30, 0x04, 0x00
    },
    {
        0x10, 0x00, 0x10, 0x10, 0xF1, 0x0F, 0x02, 0x08,
        0x10, 0x04, 0x15, 0x00, 0xD5, 0x1F, 0x55, 0x05,
        0x55, 0x05, 0x5F, 0x05, 0x55, 0x15, 0x55, 0x15,
        0x55, 0x15, 0xD5, 0x1F, 0x11, 0x00, 0x00, 0x00
    },
    {
        0x20, 0x30, 0x21, 0x18, 0xE2, 0x0F, 0x04, 0x12,
        0xC0, 0x21, 0x7F, 0x20, 0xD1, 0x2F, 0x51, 0x24,
        0x51, 0x24, 0xDF, 0x2F, 0x2A, 0x20, 0x32, 0x21,
        0xE3, 0x2F, 0x32, 0x21, 0x2A, 0x21, 0x22, 0x21
    },
    {
        0x20, 0x00, 0x20, 0x00, 0x21, 0x20, 0xE3, 0x1F,
        0x06, 0x10, 0x00, 0x08, 0x00, 0x24, 0x00, 0x20,
        0x00, 0x20, 0xFF, 0x3F, 0x10, 0x20, 0x10, 0x20,
        0x10, 0x20, 0x10, 0x20, 0x10, 0x20, 0x00, 0x20
    }
};

static void OLED_IIC_Delay(void) //
{
    unsigned char i;

    for (i = 0; i < 2; i++);
}

static void OLED_DelayMs(unsigned int ms)
{
    unsigned int i;
    unsigned char j;

    for (i = 0; i < ms; i++)
    {
        for (j = 0; j < 120; j++);
    }
}

static void OLED_IIC_Start(void)  //IIC 起始条件：SCL 为高时 SDA 从高变低
{
    OLED_SDA = 1;
    OLED_SCL = 1;
    OLED_IIC_Delay();
    OLED_SDA = 0;
    OLED_IIC_Delay();
    OLED_SCL = 0;
}

static void OLED_IIC_Stop(void)   //IIC 停止条件：SCL 为高时 SDA 从低变高
{
    OLED_SDA = 0;
    OLED_SCL = 1;
    OLED_IIC_Delay();
    OLED_SDA = 1;
}

static void OLED_IIC_WriteByte(unsigned char dat)  //通过软件 IIC 向 OLED 写入一个字节
{
    unsigned char i;

    for (i = 0; i < 8; i++)
    {
        OLED_SDA = (dat & 0x80) ? 1 : 0;
        OLED_SCL = 1;
        OLED_IIC_Delay();
        OLED_SCL = 0;
        dat <<= 1;
    }

    OLED_SDA = 1;
    OLED_SCL = 1;
    OLED_IIC_Delay();
    OLED_SCL = 0;
}

static void OLED_WriteCmd(unsigned char cmd) //向 OLED 写入一条控制命令
{
    OLED_IIC_Start();
    OLED_IIC_WriteByte(OLED_ADDR);
    OLED_IIC_WriteByte(0x00);
    OLED_IIC_WriteByte(cmd);
    OLED_IIC_Stop();
}

static void OLED_DataStreamStart(void) //开始向 OLED 连续写入显示数据
{
    OLED_IIC_Start();
    OLED_IIC_WriteByte(OLED_ADDR);
    OLED_IIC_WriteByte(0x40);
}

static void OLED_DataStreamEnd(void)  //结束 OLED 的连续显示数据传输
{
    OLED_IIC_Stop();
}

static void OLED_SetPos(unsigned char page, unsigned char col)  //设置 OLED 当前写入的页地址和列地址
{
    OLED_WriteCmd(0xB0 + page);
    OLED_WriteCmd(0x00 + (col & 0x0F));
    OLED_WriteCmd(0x10 + ((col >> 4) & 0x0F));
}


static void OLED_ClearPage(unsigned char page)   //清空 OLED 指定页的全部显示数据
{
    unsigned char i;

    OLED_SetPos(page, 0);
    OLED_DataStreamStart();
    for (i = 0; i < OLED_WIDTH; i++)
    {
        OLED_IIC_WriteByte(0x00);
    }
    OLED_DataStreamEnd();
}

static void OLED_Clear(void)  //清空 OLED 的全部 8 个显示页
{
    unsigned char page;

    for (page = 0; page < 8; page++)
    {
        OLED_ClearPage(page);
    }
}

static void OLED_DisplayOn(void)  //发送 OLED 开屏命令
{
    OLED_WriteCmd(0xAF);
}

static void OLED_DisplayOff(void)  //发送 OLED 关屏命令
{
    OLED_WriteCmd(0xAE);
}

static unsigned char OLED_GetTextColumn(unsigned int text_col, unsigned char page_part)  //根据文字列号和上下半页读取对应的点阵数据
{
    unsigned char char_index;
    unsigned char char_col;

    if (text_col >= OLED_TEXT_WIDTH)
    {
        return 0x00;
    }

    char_index = text_col / (OLED_FONT_WIDTH + OLED_FONT_SPACE);
    char_col = text_col % (OLED_FONT_WIDTH + OLED_FONT_SPACE);

    if (char_col >= OLED_FONT_WIDTH)
    {
        return 0x00;
    }

    return OLED_TextFont[char_index][char_col * 2 + page_part];
}

static void OLED_SetTextArea(void)  //设置 OLED 的垂直寻址模式和滚动显示区域
{
    OLED_WriteCmd(0x20);
    OLED_WriteCmd(0x01);
    OLED_WriteCmd(0x21);
    OLED_WriteCmd(0x00);
    OLED_WriteCmd(OLED_WIDTH - 1);
    OLED_WriteCmd(0x22);
    OLED_WriteCmd(0x03);
    OLED_WriteCmd(0x04);
}

static void OLED_ShowScrollText(void)  //刷新一帧滚动文字，并将文字向左移动一步
{
    unsigned char x;
    unsigned char upper;
    unsigned char lower;
    int text_col;

    // 按列重绘上下两页，使文字从右向左移动。 
    OLED_SetTextArea();
    OLED_DataStreamStart();
    for (x = 0; x < OLED_WIDTH; x++)
    {
        text_col = (int)x - OLED_TextLeft;
        if (text_col < 0 || text_col >= OLED_TEXT_WIDTH)
        {
            upper = 0x00;
            lower = 0x00;
        }
        else
        {
            upper = OLED_GetTextColumn((unsigned int)text_col, 0);
            lower = OLED_GetTextColumn((unsigned int)text_col, 1);
        }

        OLED_IIC_WriteByte(upper);
        OLED_IIC_WriteByte(lower);
    }
    OLED_DataStreamEnd();

    OLED_TextLeft -= OLED_SCROLL_STEP;
    if (OLED_TextLeft <= -OLED_TEXT_WIDTH)
    {
        OLED_TextLeft = OLED_TEXT_START_LEFT;
    }
}

void OLED_Init(void)  //初始化 OLED、清屏，并保持显示关闭。
{
    OLED_SCL = 1;
    OLED_SDA = 1;
    OLED_DelayMs(100);

    OLED_WriteCmd(0xAE);
    OLED_WriteCmd(0x20);
    OLED_WriteCmd(0x02);
    OLED_WriteCmd(0xB0);
    OLED_WriteCmd(0xC8);
    OLED_WriteCmd(0x00);
    OLED_WriteCmd(0x10);
    OLED_WriteCmd(0x40);
    OLED_WriteCmd(0x81);
    OLED_WriteCmd(0x7F);
    OLED_WriteCmd(0xA1);
    OLED_WriteCmd(0xA6);
    OLED_WriteCmd(0xA8);
    OLED_WriteCmd(0x3F);
    OLED_WriteCmd(0xA4);
    OLED_WriteCmd(0xD3);
    OLED_WriteCmd(0x00);
    OLED_WriteCmd(0xD5);
    OLED_WriteCmd(0x80);
    OLED_WriteCmd(0xD9);
    OLED_WriteCmd(0xF1);
    OLED_WriteCmd(0xDA);
    OLED_WriteCmd(0x12);
    OLED_WriteCmd(0xDB);
    OLED_WriteCmd(0x40);
    OLED_WriteCmd(0x8D);
    OLED_WriteCmd(0x14);

    OLED_Clear();
    OLED_DisplayOff();
}

void OLED_SetScrollEnable(unsigned char enable)  //设置 OLED 滚动显示功能的开关状态
{
    OLED_Enable = enable ? 1 : 0;
}

void OLED_Proc(void)  
{
    // 只在状态变化时开关屏，其余时间持续刷新滚动画面。 
    if (OLED_Enable != OLED_LastEnable)
    {
        OLED_LastEnable = OLED_Enable;
        OLED_TextLeft = OLED_TEXT_START_LEFT;
        if (OLED_Enable)
        {
            OLED_DisplayOn();
            OLED_ShowScrollText();
        }
        else
        {
            OLED_DisplayOff();
        }
    }

    if (OLED_Enable == 0)
    {
        return;
    }

    OLED_ShowScrollText();
}
