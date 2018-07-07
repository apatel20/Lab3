//------------------------------------------
// DISPLAY API (Application Programming Interface)
// Also known as DISPLAY HAL (Hardware Abstraction Layer)
// HAL is a specific form of API that designs the interface with a certain hardware

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <ti/grlib/grlib.h>


#define MY_BLACK GRAPHICS_COLOR_BLACK
#define MY_WHITE GRAPHICS_COLOR_WHITE


void InitGraphics();
void LCDClearDisplay(int color);
void LCDDrawChar(unsigned row, unsigned col, int8_t c);
void PrintString(char *str, int row, int col);
void drawLines();
void drawOverR(unsigned x);
void drawOverL(unsigned x);
void drawOverObstacle(unsigned y, int obstacleX);
void drawIcon(int obstacleXPos);
void drawIconBack(int obstacleXPos);
void drawObstaclesBack(bool obstacle1, bool obstacle2, bool obstacle3, int xPos1, int yPos1, int xPos2, int yPos2, int xPos3, int yPos3);


#endif /* DISPLAY_H_ */
