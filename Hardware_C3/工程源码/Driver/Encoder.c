#include "reg52.h"
#include "Encoder.h"

/* MG310 Hall encoder: A is connected to INT0, B is sampled for direction. */
sbit ENCODER_A = P3^2;
sbit ENCODER_B = P3^3;

static volatile signed int Encoder_Count = 0;

void Encoder_Init(void)
{
    /* Quasi-bidirectional inputs: write 1 to release the pins. */
    ENCODER_A = 1;
    ENCODER_B = 1;
    Encoder_Count = 0;
    IT0 = 1;
    IE0 = 0;
    EX0 = 1;
}

void Encoder_Int0(void) interrupt 0
{
    if (ENCODER_B)
    {
        Encoder_Count--;
    }
    else
    {
        Encoder_Count++;
    }
}

int Encoder_GetDelta(void)
{
    signed int count;

    EX0 = 0;
    count = Encoder_Count;
    Encoder_Count = 0;
    EX0 = 1;
    return count;
}
