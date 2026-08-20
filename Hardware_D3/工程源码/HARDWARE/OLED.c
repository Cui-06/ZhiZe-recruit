#include "Board.h"
#include "OLED.h"
#include "delay.h"

#define OLED_ADDR       0x78U /* SSD1306 的 8 位写地址，7 位地址为 0x3C。 */
#define OLED_WIDTH      128U
#define OLED_LINE_CHARS 21U   /* 21 个 6 像素宽字符可以覆盖 126 列。 */

static void OLED_IIC_Delay(void)
{
    delay_us(2U);
}

static void OLED_SCL_High(void)
{
    GPIO_SetBits(OLED_PORT, OLED_SCL_PIN);
}

static void OLED_SCL_Low(void)
{
    GPIO_ResetBits(OLED_PORT, OLED_SCL_PIN);
}

static void OLED_SDA_High(void)
{
    GPIO_SetBits(OLED_PORT, OLED_SDA_PIN);
}

static void OLED_SDA_Low(void)
{
    GPIO_ResetBits(OLED_PORT, OLED_SDA_PIN);
}

/* IIC 起始条件：SCL 为高时 SDA 从高变低。 */
static void OLED_IIC_Start(void)
{
    OLED_SDA_High();
    OLED_SCL_High();
    OLED_IIC_Delay();
    OLED_SDA_Low();
    OLED_IIC_Delay();
    OLED_SCL_Low();
}

/* IIC 停止条件：SCL 为高时 SDA 从低变高。 */
static void OLED_IIC_Stop(void)
{
    OLED_SDA_Low();
    OLED_SCL_High();
    OLED_IIC_Delay();
    OLED_SDA_High();
    OLED_IIC_Delay();
}

/* 发送一个字节；OLED 只写不读，忽略从机应答数据。 */
static void OLED_IIC_WriteByte(unsigned char data)
{
    unsigned char i;

    for (i = 0U; i < 8U; i++)
    {
        if ((data & 0x80U) != 0U)
        {
            OLED_SDA_High();
        }
        else
        {
            OLED_SDA_Low();
        }

        OLED_SCL_High();
        OLED_IIC_Delay();
        OLED_SCL_Low();
        OLED_IIC_Delay();
        data <<= 1;
    }

    /* 第 9 个时钟让出 SDA，完成 ACK 时序。 */
    OLED_SDA_High();
    OLED_SCL_High();
    OLED_IIC_Delay();
    OLED_SCL_Low();
}

static void OLED_WriteCommand(unsigned char command)
{
    OLED_IIC_Start();
    OLED_IIC_WriteByte(OLED_ADDR);
    OLED_IIC_WriteByte(0x00U);
    OLED_IIC_WriteByte(command);
    OLED_IIC_Stop();
}

static void OLED_DataStreamStart(void)
{
    OLED_IIC_Start();
    OLED_IIC_WriteByte(OLED_ADDR);
    OLED_IIC_WriteByte(0x40U);
}

static void OLED_DataStreamEnd(void)
{
    OLED_IIC_Stop();
}

static void OLED_SetPosition(unsigned char page, unsigned char column)
{
    OLED_WriteCommand((unsigned char)(0xB0U + page));
    OLED_WriteCommand((unsigned char)(column & 0x0FU));
    OLED_WriteCommand((unsigned char)(0x10U | ((column >> 4) & 0x0FU)));
}

static void OLED_Clear(void)
{
    unsigned char page;
    unsigned char column;

    for (page = 0U; page < 8U; page++)
    {
        OLED_SetPosition(page, 0U);
        OLED_DataStreamStart();
        for (column = 0U; column < OLED_WIDTH; column++)
        {
            OLED_IIC_WriteByte(0x00U);
        }
        OLED_DataStreamEnd();
    }
}

/* 返回当前显示字符的 5x7 点阵，每列一个字节。 */
static void OLED_GetGlyph(unsigned char character, unsigned char glyph[5])
{
    glyph[0] = 0x00U;
    glyph[1] = 0x00U;
    glyph[2] = 0x00U;
    glyph[3] = 0x00U;
    glyph[4] = 0x00U;

    switch (character)
    {
        case '0': glyph[0] = 0x3EU; glyph[1] = 0x51U; glyph[2] = 0x49U; glyph[3] = 0x45U; glyph[4] = 0x3EU; break;
        case '1': glyph[0] = 0x00U; glyph[1] = 0x42U; glyph[2] = 0x7FU; glyph[3] = 0x40U; glyph[4] = 0x00U; break;
        case '2': glyph[0] = 0x42U; glyph[1] = 0x61U; glyph[2] = 0x51U; glyph[3] = 0x49U; glyph[4] = 0x46U; break;
        case '3': glyph[0] = 0x21U; glyph[1] = 0x41U; glyph[2] = 0x45U; glyph[3] = 0x4BU; glyph[4] = 0x31U; break;
        case '4': glyph[0] = 0x18U; glyph[1] = 0x14U; glyph[2] = 0x12U; glyph[3] = 0x7FU; glyph[4] = 0x10U; break;
        case '5': glyph[0] = 0x27U; glyph[1] = 0x45U; glyph[2] = 0x45U; glyph[3] = 0x45U; glyph[4] = 0x39U; break;
        case '6': glyph[0] = 0x3CU; glyph[1] = 0x4AU; glyph[2] = 0x49U; glyph[3] = 0x49U; glyph[4] = 0x30U; break;
        case '7': glyph[0] = 0x01U; glyph[1] = 0x71U; glyph[2] = 0x09U; glyph[3] = 0x05U; glyph[4] = 0x03U; break;
        case '8': glyph[0] = 0x36U; glyph[1] = 0x49U; glyph[2] = 0x49U; glyph[3] = 0x49U; glyph[4] = 0x36U; break;
        case '9': glyph[0] = 0x06U; glyph[1] = 0x49U; glyph[2] = 0x49U; glyph[3] = 0x29U; glyph[4] = 0x1EU; break;
        case 'S': glyph[0] = 0x46U; glyph[1] = 0x49U; glyph[2] = 0x49U; glyph[3] = 0x49U; glyph[4] = 0x31U; break;
        case 'P': glyph[0] = 0x7FU; glyph[1] = 0x09U; glyph[2] = 0x09U; glyph[3] = 0x09U; glyph[4] = 0x06U; break;
        case 'D': glyph[0] = 0x7FU; glyph[1] = 0x41U; glyph[2] = 0x41U; glyph[3] = 0x22U; glyph[4] = 0x1CU; break;
        case 'I': glyph[0] = 0x00U; glyph[1] = 0x41U; glyph[2] = 0x7FU; glyph[3] = 0x41U; glyph[4] = 0x00U; break;
        case 'L': glyph[0] = 0x7FU; glyph[1] = 0x40U; glyph[2] = 0x40U; glyph[3] = 0x40U; glyph[4] = 0x40U; break;
        case 'M': glyph[0] = 0x7FU; glyph[1] = 0x02U; glyph[2] = 0x0CU; glyph[3] = 0x02U; glyph[4] = 0x7FU; break;
        case 'O': glyph[0] = 0x3EU; glyph[1] = 0x41U; glyph[2] = 0x41U; glyph[3] = 0x41U; glyph[4] = 0x3EU; break;
        case 'T': glyph[0] = 0x01U; glyph[1] = 0x01U; glyph[2] = 0x7FU; glyph[3] = 0x01U; glyph[4] = 0x01U; break;
        case ':': glyph[0] = 0x00U; glyph[1] = 0x36U; glyph[2] = 0x36U; glyph[3] = 0x00U; glyph[4] = 0x00U; break;
        case '+': glyph[0] = 0x08U; glyph[1] = 0x08U; glyph[2] = 0x3EU; glyph[3] = 0x08U; glyph[4] = 0x08U; break;
        case '-': glyph[0] = 0x08U; glyph[1] = 0x08U; glyph[2] = 0x08U; glyph[3] = 0x08U; glyph[4] = 0x08U; break;
        default: break;
    }
}

static void OLED_WriteCharData(unsigned char character)
{
    unsigned char glyph[5];
    unsigned char i;

    OLED_GetGlyph(character, glyph);
    for (i = 0U; i < 5U; i++)
    {
        OLED_IIC_WriteByte(glyph[i]);
    }
    OLED_IIC_WriteByte(0x00U);
}

static unsigned char OLED_WriteText(const char *text)
{
    unsigned char count = 0U;

    while (*text != '\0')
    {
        OLED_WriteCharData((unsigned char)*text++);
        count++;
    }
    return count;
}

static unsigned char OLED_WriteUnsigned(unsigned long value)
{
    unsigned char digits[10];
    unsigned char count = 0U;
    unsigned char i;

    if (value == 0UL)
    {
        OLED_WriteCharData('0');
        return 1U;
    }

    while (value != 0UL && count < 10U)
    {
        digits[count++] = (unsigned char)('0' + (value % 10UL));
        value /= 10UL;
    }

    for (i = count; i > 0U; i--)
    {
        OLED_WriteCharData(digits[i - 1U]);
    }
    return count;
}

static unsigned char OLED_WriteSigned(long value)
{
    unsigned char count;

    if (value < 0L)
    {
        OLED_WriteCharData('-');
        count = OLED_WriteUnsigned((unsigned long)(-value));
    }
    else
    {
        OLED_WriteCharData('+');
        count = OLED_WriteUnsigned((unsigned long)value);
    }
    return (unsigned char)(count + 1U);
}

static void OLED_WriteStatusLine(unsigned char page,
                                 const char *label,
                                 long value)
{
    unsigned char count;

    OLED_SetPosition(page, 0U);
    OLED_DataStreamStart();
    count = OLED_WriteText(label);
    count = (unsigned char)(count + OLED_WriteSigned(value));
    while (count < OLED_LINE_CHARS)
    {
        OLED_WriteCharData(' ');
        count++;
    }
    OLED_DataStreamEnd();
}

void OLED_Init(void)
{
    OLED_SDA_High();
    OLED_SCL_High();
    delay_ms(20U);

    OLED_WriteCommand(0xAEU); /* 关闭显示。 */
    OLED_WriteCommand(0x20U); /* 页寻址模式。 */
    OLED_WriteCommand(0x02U);
    OLED_WriteCommand(0xB0U);
    OLED_WriteCommand(0xC8U); /* COM 反向扫描，实现上下镜面翻转。 */
    OLED_WriteCommand(0x00U);
    OLED_WriteCommand(0x10U);
    OLED_WriteCommand(0x40U);
    OLED_WriteCommand(0x81U);
    OLED_WriteCommand(0x7FU);
    OLED_WriteCommand(0xA1U);
    OLED_WriteCommand(0xA6U);
    OLED_WriteCommand(0xA8U);
    OLED_WriteCommand(0x3FU);
    OLED_WriteCommand(0xA4U);
    OLED_WriteCommand(0xD3U);
    OLED_WriteCommand(0x00U);
    OLED_WriteCommand(0xD5U);
    OLED_WriteCommand(0x80U);
    OLED_WriteCommand(0xD9U);
    OLED_WriteCommand(0xF1U);
    OLED_WriteCommand(0xDAU);
    OLED_WriteCommand(0x12U);
    OLED_WriteCommand(0xDBU);
    OLED_WriteCommand(0x40U);
    OLED_WriteCommand(0x8DU);
    OLED_WriteCommand(0x14U);

    OLED_Clear();
    OLED_WriteCommand(0xAFU); /* 初始化后保持显示开启。 */
}

void OLED_ShowStatus(int32_t position,
                     int32_t speed,
                     int32_t target_position,
                     int32_t target_speed)
{
    OLED_WriteStatusLine(0U, "SPD:", (long)speed);
    OLED_WriteStatusLine(2U, "POS:", (long)position);
    OLED_WriteStatusLine(4U, "TSPD:", (long)target_speed);
    OLED_WriteStatusLine(6U, "TPOS:", (long)target_position);
}
