#include <ctype.h>
#include <stdio.h>
#include <syscalls.h>
#include <window.h>

#include "doomgeneric.h"
#include "doomkeys.h"

static uint32_t init_ticks_ms = 0;
static WindowDescriptor *wdesc = NULL;
static char before_input_key = '\0';
static char input_key;

#define KEYQUEUE_SIZE 16

static unsigned short s_keyQueue[KEYQUEUE_SIZE];
static unsigned int s_KeyQueueWriteIndex = 0;
static unsigned int s_KeyQueueReadIndex = 0;

static unsigned char convertToDoomKey(char key) {
    key = tolower(key);

    switch (key) {
        case '\n':
            return KEY_ENTER;
        case 'e':
            return KEY_ESCAPE;
        case 'a':
            return KEY_LEFTARROW;
        case 'd':
            return KEY_RIGHTARROW;
        case 'w':
            return KEY_UPARROW;
        case 's':
            return KEY_DOWNARROW;
        case ' ':
            return KEY_USE;
        case 'i':
            return KEY_FIRE;
        case 'o':
            return KEY_RSHIFT;
        default:
            return key;
    }
}

static void addKeyToQueue(int pressed, char key) {
    unsigned short keyData = (pressed << 8) | convertToDoomKey(key);

    s_keyQueue[s_KeyQueueWriteIndex] = keyData;
    s_KeyQueueWriteIndex++;
    s_KeyQueueWriteIndex %= KEYQUEUE_SIZE;
}

void DG_Init() {
    // tick
    init_ticks_ms = (uint32_t)sys_uptime();

    // window
    wdesc = create_window("DOOM", 0, 0, DOOMGENERIC_RESX + 8, DOOMGENERIC_RESY + 30);
    if (wdesc == NULL) {
        printf("Failed to create window\n");
        return;
    }

    if (add_image_to_window(wdesc, DOOMGENERIC_RESX, DOOMGENERIC_RESY, PIXEL_FORMAT_BGRA, (char *)DG_ScreenBuffer) == -1) {
        printf("Failed to add image to window\n");
        return;
    }
}

void DG_DrawFrame() {
    if (sys_read(FDN_STDIN, &input_key, 1) == -1)
        return;

    if (input_key == '\0')
        return;

    if (input_key == 0x03) {
        printf("Interrupt.\n");
        exit(0);
    }

    if (input_key != before_input_key && before_input_key != '\0')
        addKeyToQueue(0, before_input_key);

    addKeyToQueue(1, input_key);
    before_input_key = input_key;
}

void DG_SleepMs(uint32_t ms) {
    uint32_t start = DG_GetTicksMs();
    for (;;) {
        if (DG_GetTicksMs() - start >= ms)
            break;
    }
}

uint32_t DG_GetTicksMs() {
    return (uint32_t)sys_uptime() - init_ticks_ms;
}

int DG_GetKey(int *pressed, unsigned char *doomKey) {
    if (s_KeyQueueReadIndex == s_KeyQueueWriteIndex)
        return 0;

    unsigned short keyData = s_keyQueue[s_KeyQueueReadIndex];
    s_KeyQueueReadIndex++;
    s_KeyQueueReadIndex %= KEYQUEUE_SIZE;

    *pressed = keyData >> 8;
    *doomKey = keyData & 0xff;

    return 1;
}

void DG_SetWindowTitle(const char *title) {
}

int main(int argc, char **argv) {
    doomgeneric_Create(argc, argv);

    while (1)
        doomgeneric_Tick();

    return 0;
}
