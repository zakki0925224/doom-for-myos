#include <stdio.h>
#include <syscalls.h>

#include "doomgeneric.h"

static uint32_t init_ticks_ms = 0;

void DG_Init()
{
    init_ticks_ms = (uint32_t)sys_uptime();
}

void DG_DrawFrame()
{
    printf("%d\n", ((char *)DG_ScreenBuffer)[0]);
}

void DG_SleepMs(uint32_t ms)
{
}

uint32_t DG_GetTicksMs()
{
    return (uint32_t)sys_uptime() - init_ticks_ms;
}

int DG_GetKey(int *pressed, unsigned char *doomKey)
{
    return 0;
}

void DG_SetWindowTitle(const char *title)
{
}

int main(int argc, char **argv)
{
    doomgeneric_Create(argc, argv);

    while (1)
    {
        doomgeneric_Tick();
    }

    return 0;
}
