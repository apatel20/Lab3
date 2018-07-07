#include <ti/grlib/grlib.h>
#include "LcdDriver/Crystalfontz128x128_ST7735.h"
#include "LcdDriver/HAL_MSP_EXP432P401R_Crystalfontz128x128_ST7735.h"
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

Graphics_Context g_sContext;

extern tImage favicon38BPP_UNCOMP;
extern tImage unnamed_dvx_icon8BPP_UNCOMP;

#define LANE1 2
#define LANE2 18
#define LANE3 34
#define LANE4 50

void InitGraphics() {
    Crystalfontz128x128_Init();
    Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP);
    Graphics_initContext(&g_sContext,
                         &g_sCrystalfontz128x128,
                         &g_sCrystalfontz128x128_funcs);
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_GREEN);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    GrContextFontSet(&g_sContext, &g_sFontCmtt16);
    Graphics_clearDisplay(&g_sContext);
}

void drawLines()
{
    Graphics_drawLineV(&g_sContext, 0, 0, 127);
    Graphics_drawLineV(&g_sContext, 1, 0, 127);
    Graphics_drawLineV(&g_sContext, 66, 0, 127);
    Graphics_drawLineV(&g_sContext, 67, 0, 127);
    Graphics_drawImage(&g_sContext, &favicon38BPP_UNCOMP, 5, 111);
}

void LCDClearDisplay(int color) {
    Graphics_setBackgroundColor(&g_sContext, color);
    Graphics_clearDisplay(&g_sContext);
}


void LCDDrawChar(unsigned row, unsigned col, int8_t c) {
    Graphics_drawString(&g_sContext,
                        &c,
                        1,
                        8 * (col % 16),
                        16 * (row % 8),
                        OPAQUE_TEXT);
}

void PrintString(char *str, int row, int col) {
    int i;
    for (i = 0; str[i] != '\0'; i++) {
        LCDDrawChar(row,  col, str[i]);
        col++;
        if (col >= 16) {
            col = 0;
            row++;
            if (row >= 8) {
                row = 0;
            }
        }
    }
}

void drawOverR(unsigned x)
{
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_drawLineV(&g_sContext, x-1, 111, 127);
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_GREEN);
    Graphics_drawImage(&g_sContext, &favicon38BPP_UNCOMP, x, 111);
}

void drawOverL(unsigned x)
{
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_drawLineV(&g_sContext, x+1, 111, 127);
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_GREEN);
    Graphics_drawImage(&g_sContext, &favicon38BPP_UNCOMP, x, 111);
}

void drawIcon(int obstacleXPos)
{
    Graphics_drawImage(&g_sContext, &unnamed_dvx_icon8BPP_UNCOMP, obstacleXPos, 0);
}

void drawIconBack(int obstacleXPos)
{
    Graphics_drawImage(&g_sContext, &favicon38BPP_UNCOMP, obstacleXPos, 111);
}

void drawObstaclesBack(bool obstacle1, bool obstacle2, bool obstacle3, int xPos1, int yPos1, int xPos2, int yPos2, int xPos3, int yPos3)
{
    if (obstacle1)
    {
        Graphics_drawImage(&g_sContext, &unnamed_dvx_icon8BPP_UNCOMP, xPos1, yPos1);
    }
    if (obstacle2)
    {
        Graphics_drawImage(&g_sContext, &unnamed_dvx_icon8BPP_UNCOMP, xPos2, yPos2);
    }
    if (obstacle3)
    {
        Graphics_drawImage(&g_sContext, &unnamed_dvx_icon8BPP_UNCOMP, xPos3, yPos3);
    }
}


void drawOverObstacle(unsigned y, int obstacleX)
{
    if (obstacleX == LANE1)
    {
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
        Graphics_drawLineH(&g_sContext, 2, 17, y-1);
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_GREEN);
        Graphics_drawImage(&g_sContext, &unnamed_dvx_icon8BPP_UNCOMP, obstacleX, y);
    }
    else if (obstacleX == LANE2)
    {
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
        Graphics_drawLineH(&g_sContext, 18, 33, y);
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_GREEN);
        Graphics_drawImage(&g_sContext, &unnamed_dvx_icon8BPP_UNCOMP, obstacleX, y);
    }
    else if (obstacleX == LANE3)
    {
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
        Graphics_drawLineH(&g_sContext, 34, 49, y);
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_GREEN);
        Graphics_drawImage(&g_sContext, &unnamed_dvx_icon8BPP_UNCOMP, obstacleX, y);
    }
    else if (obstacleX == LANE4)
    {
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
        Graphics_drawLineH(&g_sContext, 50, 65, y);
        Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_GREEN);
        Graphics_drawImage(&g_sContext, &unnamed_dvx_icon8BPP_UNCOMP, obstacleX, y);
    }
}
