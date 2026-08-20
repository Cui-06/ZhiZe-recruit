#include "sys.h"

/* 等待中断，进入低功耗等待状态。 */
void WFI_SET(void)
{
    __ASM volatile("wfi");
}

/* 关闭和开启全局中断。 */
void INTX_DISABLE(void)
{
    __ASM volatile("cpsid i");
}

void INTX_ENABLE(void)
{
    __ASM volatile("cpsie i");
}

/* 设置 Cortex-M3 主堆栈指针，常用于 Bootloader 跳转。 */
__asm void MSR_MSP(u32 addr)
{
    MSR MSP, r0
    BX r14
}
