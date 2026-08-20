#ifndef SEG_H
#define SEG_H

#define SEG_BLANK 10

void Seg_Init(void);
void Seg_SetBuf(unsigned char pos, unsigned char dat);
void Seg_SetAll(const unsigned char *buf);
void Seg_Scan(unsigned char pos);
void Seg_Display(void);

#endif
