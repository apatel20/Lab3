/*
 * You can add more of your own developed *.h files here.
 * However, you are not allowed to include driverlib, grlib, stdio or any other library in this file.
 */
#include <ADC_HAL.h>
#include <LED_HAL.h>
#include <Buttons_HAL.h>
#include <Timer_HAL.h>
#include <Display_HAL.h>

#define OPENING_WAIT 3000 // 3 seconds or 30000 ms
#define UP_THRESHOLD_HIGH 0x3000
#define UP_THRESHOLD_LOW 0x1000
#define DEBOUNCE_TIMING 100 //100 ms
#define WAIT_TIME 3300
#define MOVING_TIME 50
#define IMAGE_TOP 111
#define IMAGE_BOTTOM 127
#define BEGINNING 5
#define LANE1 2
#define LANE2 18
#define LANE3 34
#define LANE4 50

typedef enum {stable0, trans0To1, stable1, trans1To0} DebounceState_t;
typedef enum {play, instruct, score} menu;
typedef enum {center, up, down} joystick;
typedef enum {drive, reset, pause, obstacle, move, neutralGame} gameState;
typedef enum {playG, back2menu} pauseState;
typedef enum {run, instructions, scoreHist, neutral} screens;
static enum states {INCEPTION, OPENING, MENU} stateMenu = INCEPTION;

static unsigned x = BEGINNING;
int obstacle1y = 0;
int obstacle2y = 0;
int obstacle3y = 0;//global variables for obstacle position
int obstacle1x = 0;
int obstacle2x = 0;
int obstacle3x = 0;
int score1 = 0;
int score2 = 0;
int score3 = 0;
int newScore = 0;
int obstacleCounter = 0;
int arrowPosition = 2;
unsigned vx, vy;
bool showArrow = false;
bool obstacle1 = false;
bool obstacle2 = false;
bool obstacle3 = false;

static menu current = play;
static joystick position = center;
static gameState playing = move;
static joystick state = center;
static pauseState currentState = playG;
static screens cScreen = neutral;

char buf[2] = "00";

void startObstacle(int random)
{
    if (!obstacle1)//if obstacle one is not on screen
    {
        if (random == 0)//determine lane
        {
            obstacle1x = LANE1;
            obstacle1y = 0;//assign lane and draw
            obstacle1 = true;
            drawIcon(obstacle1x);
        }
        else if (random == 1)//determine lane 2
        {
            obstacle1x = LANE2;
            obstacle1y = 0;//assign lane and draw
            obstacle1 = true;
            drawIcon(obstacle1x);
        }
        else if (random == 2)//determine lane 3
        {
            obstacle1x = LANE3;
            obstacle1y = 0;
            obstacle1 = true;//assign lane and draw
            drawIcon(obstacle1x);
        }
        else if (random == 3) //determine lane 4
        {
            obstacle1x = LANE4;
            obstacle1y = 0;
            obstacle1 = true;//assign lane and draw
            drawIcon(obstacle1x);
        }
    }
    else if (!obstacle2)//repeat for obstacle 2
    {
        if (random == 0)
        {
            obstacle2x = LANE1;
            obstacle2y = 0;
            obstacle2 = true;
            drawIcon(obstacle2x);
        }
        else if (random == 1)
        {
            obstacle2x = LANE2;
            obstacle2y = 0;
            obstacle2 = true;
            drawIcon(obstacle2x);
        }
        else if (random == 2)
        {
            obstacle2x = LANE3;
            obstacle2y = 0;
            obstacle2 = true;
            drawIcon(obstacle2x);
        }
        else if (random == 3)
        {
            obstacle2x = LANE4;
            obstacle2y = 0;
            obstacle2 = true;
            drawIcon(obstacle2x);
        }
    }
    else if (!obstacle3)//repeat for obstacle 3
    {
        if (random == 0)
        {
            obstacle3x = LANE1;
            obstacle3y = 0;
            obstacle3 = true;
            drawIcon(obstacle3x);

        }
        else if (random == 1)
        {
            obstacle3x = LANE2;
            obstacle3y = 0;
            obstacle3 = true;
            drawIcon(obstacle3x);
        }
        else if (random == 2)
        {
            obstacle3x = LANE3;
            obstacle3y = 0;
            obstacle3 = true;
            drawIcon(obstacle3x);
        }
        else if (random == 3)
        {
            obstacle3x = LANE4;
            obstacle3y = 0;
            obstacle3 = true;
            drawIcon(obstacle3x);
        }
    }
}

void updateScoreHistory()
{
    int temp = 0;
    int temp2 = 0;

    if (newScore > score1)//check is newScore is greater than scores already stored
    {
        temp = score1;
        score1 = newScore;
    }
    else if (newScore > score2)
    {
        temp = score2;//swap
        score2 = newScore;
        score3 = temp2;
    }
    else if (newScore > score3)
    {
       score3 = newScore;
    }

    if (temp > score2 && temp != 0)
    {
        temp2 = score2;//swap
        score2 = temp;
    }
    if (temp > score3 && temp != 0)
    {
        score3 = temp;
    }
    if (temp2 > score3)
    {
        score3 = temp2;
    }
}

bool joyStickDebounce()
{
    static bool prevStatus = false;
    static DebounceState_t debounceState = stable0;
    static OneShotSWTimer_t timer;
    static bool initTimer = false;//debounces given to us

    if (!initTimer)
    {
        InitOneShotSWTimer(&timer, TIMER32_1_BASE, DEBOUNCE_TIMING);
        initTimer = true;
    }

    bool rawStatus = joystickPressed();
    bool curStatus = Debounce_Button(&debounceState, &timer, rawStatus);
    bool pushed = (!curStatus && prevStatus);
    prevStatus = curStatus;
    return pushed;
}


void checkForHigh()
{
    if (current == play)
    {
        current = score;
        PrintString(" ", 2, 1);
        arrowPosition = 6;//move arrow up
        if (showArrow)
        {
            PrintString(">", arrowPosition, 1);
        }
    }
    else if (current == instruct)
    {
        current = play;
        PrintString(" ", 4, 1);//move arrow up
        arrowPosition = 2;
        if (showArrow)
        {
            PrintString(">", arrowPosition, 1);
        }
    }
    else if (current == score)
    {
        current = instruct;
        PrintString(" ", 6, 1);
        arrowPosition = 4;//move arrow up
        if (showArrow)
        {
            PrintString(">", arrowPosition, 1);
        }
    }
}

void checkForLow()
{
    if (current == play)//verbatim for checkForHigh except arrow goes opposite direction
    {
        current = instruct;
        PrintString(" ", 2, 1);
        arrowPosition = 4;
        if (showArrow)
        {
            PrintString(">", arrowPosition, 1);
        }
    }
    else if (current == instruct)
    {
        current = score;
        PrintString(" ", 4, 1);
        arrowPosition = 6;
        if (showArrow)
        {
            PrintString(">", arrowPosition, 1);
        }
    }
    else if (current == score)
    {
        current = play;
        PrintString(" ", 6, 1);
        arrowPosition = 2;
        if (showArrow)
        {
            PrintString(">", arrowPosition, 1);
        }
    }
}

void DrawOpeningScreen()
{
    LCDClearDisplay(MY_BLACK);
    PrintString("Iron Man", 2, 2);//opening screen
    PrintString("by", 3, 2);
    PrintString("Ankit Patel", 4, 2);
    PrintString("Wait 3 sec", 6, 2);
}

void DrawInstructionScreen()
{
    LCDClearDisplay(MY_BLACK);
    PrintString("INSTRUCTIONS", 0, 2);
    PrintString("Direct the car", 2, 0);
    PrintString("to the right or", 3, 0);//instructions for playing game
    PrintString("left and avoid", 4, 0);
    PrintString("road obstacles!", 5, 0);
    PrintString("BTN1-MENU", 7, 6);
}

void joyStickPosition()
{
      switch(position)
      {
          case up://if joystick pushed up
              if (vy < UP_THRESHOLD_HIGH && vy > UP_THRESHOLD_LOW)
              {
                  position = center;
              }
              break;
          case center://if back at center
              if (vy > UP_THRESHOLD_HIGH)
              {
                  position = up;
                  checkForHigh();
              }
              else if (vy < UP_THRESHOLD_LOW)
              {
                  position = down;
                  checkForLow();
              }
                  break;
          case down://if pushed down
              if (vy < UP_THRESHOLD_HIGH && vy > UP_THRESHOLD_LOW)
              {
                  position = center;
              }
              break;
      }
}

void DrawMenuScreen()
{
    LCDClearDisplay(MY_BLACK);
    PrintString("GAME MENU", 0, 2);
    PrintString("Play Game", 2, 3);//menu screen
    PrintString("How to play", 4, 3);
    PrintString("Score History", 6, 3);
    PrintString(">", arrowPosition, 1);
}

void checkPause()
{
    switch (currentState)
    {
    case playG://if on continue
        currentState = back2menu;
        PrintString(" ", 2, 1);
        arrowPosition = 4;//go to menu
        showArrow = true;
        if (showArrow)
        {
            PrintString(">", arrowPosition, 1);
        }
        break;
    case back2menu://if on menu
        currentState = playG;
        PrintString(" ", 4, 1);
        arrowPosition = 2;//go to continue
        showArrow = true;
        if (showArrow)
        {
            PrintString(">", arrowPosition, 1);
        }
        break;
    }
}

void DrawPauseScreen()
{
    LCDClearDisplay(MY_BLACK);
    PrintString("GAME PAUSED", 0, 2);
    PrintString("Continue", 2, 3);//print pause screen
    PrintString("Menu", 4, 3);
    PrintString(">", arrowPosition, 1);
}

void joystickPressPause()
{
    if ((joystickPressed()) && (currentState == playG))//if pressed on continue
   {
       showArrow = false;
       loadGamePause();
       cScreen = run;
       playing = move;
   }
   else if ((joystickPressed()) && (currentState == back2menu))//if pressed on menu
   {
       showArrow = false;
       DrawMenuScreen();
       cScreen = neutral;
       playing = neutralGame;
   }
}

void choosePause()
{
    unsigned vx1,vy1;
    getSampleJoyStick(&vx1, &vy1);

    switch(state)
   {
       case up://if joystick pushed up
           if (vy1 < UP_THRESHOLD_HIGH && vy1 > UP_THRESHOLD_LOW)
           {
               state = center;
           }
           break;
       case center://if joystick back in center
           if (vy1 > UP_THRESHOLD_HIGH)
           {
               state = up;
               checkPause();
           }
           else if (vy1 < UP_THRESHOLD_LOW)
           {
               state = down;
               checkPause();
           }
               break;
       case down://if joystick pushed down
           if (vy1 < UP_THRESHOLD_HIGH && vy1 > UP_THRESHOLD_LOW)
           {
               state = center;
           }
           break;
   }
    joystickPressPause();//see where joystick is pressed
}

void DrawGameOverScreen()
{
    LCDClearDisplay(MY_BLACK);
    PrintString("GAME OVER", 0, 2);
    PrintString("SCORE: ", 2, 2);//draw game over screen with score
    PrintString(buf, 2, 8);
    //need to print score
}

void ScreensFSM()
{
    static OneShotSWTimer_t OST;
    InitOneShotSWTimer(&OST, TIMER32_1_BASE, OPENING_WAIT);

    // Set the default outputs
    bool drawOpening = false;
    bool drawMenu = false;

    switch (stateMenu)
    {
    case INCEPTION:
        StartOneShotSWTimer(&OST);

        // State transition
        stateMenu = OPENING;

        // The output(s) that are affected in this transition
        drawOpening = true;
        break;

    case OPENING:
        if (OneShotSWTimerExpired(&OST))
        {
            // State transition
            stateMenu = MENU;

            // The output(s) that are affected in this transition
            drawMenu = true;
        }
        break;

    case MENU:
        break;

    } // End of switch-case

    // Implement actions based on the outputs of the FSM
    if (drawOpening)
    {
        showArrow = false;
        DrawOpeningScreen();
    }

    if (drawMenu)
    {
        showArrow = true;
        arrowPosition = 2;
        DrawMenuScreen();
    }
}

void moveIconRight()
{
    if (x+15 == 65)//reach right wall
    {
        x = 50;
    }
    else
    {
        x++;
    }
}

void moveIconLeft()
{
    if (x == 2)//reach left wall
    {
        x = 2;
    }
    else
    {
        x--;
    }
}

void loadGamePause()
{
    showArrow = false;
    LCDClearDisplay(MY_BLACK);
    PrintString("SCORE", 0, 11);
    buf[0] = ((newScore/10)%10) + '0';
    buf[1] = ((newScore)%10) + '0';
    PrintString(buf, 1, 9);
    //actual score goes here
    PrintString("HIGH", 3, 12);
    PrintString("SCORE", 4, 11);//load back all pieces of game with pause
    char highS[2] = "XX";
    highS[0] = ((score1/10)%10) + '0';
    highS[1] = ((score1)%10) + '0';
    PrintString(highS, 5, 11);
    //highest score goes here
    drawLines();
    drawIconBack(x);
    drawObstaclesBack(obstacle1, obstacle2, obstacle3, obstacle1x, obstacle1y, obstacle2x, obstacle2y, obstacle3x, obstacle3y);
}

void loadGameScreen()
{
    LCDClearDisplay(MY_BLACK);
    PrintString("SCORE", 0, 11);
    buf[0] = ((newScore/10)%10) + '0';
    buf[1] = ((newScore)%10) + '0';
    PrintString(buf, 1, 11);
    //actual score goes here
    PrintString("HIGH", 3, 12);//load the game contents
    PrintString("SCORE", 4, 11);
    char highS[2] = "XX";
    highS[0] = ((score1/10)%10) + '0';
    highS[1] = ((score1)%10) + '0';
    PrintString(highS, 5, 11);
    //highest score goes here
    drawLines();
}

void playGame()
{
    unsigned vx, vy;
    static OneShotSWTimer_t OSTgame;
    InitOneShotSWTimer(&OSTgame, TIMER32_1_BASE, MOVING_TIME);
    getSampleJoyStick(&vx, &vy);

    switch (playing)
   {
       case drive://check for timer expiration
           if (OneShotSWTimerExpired(&OSTgame) && vx > UP_THRESHOLD_HIGH)
           {
               drawOverR(x);
               moveIconRight();
               playing = move;
           }
           if (OneShotSWTimerExpired(&OSTgame) && vx < UP_THRESHOLD_LOW)
           {
               drawOverL(x);
               moveIconLeft();
               playing = move;
           }
           if (Booster_Top_Button_Pushed())
           {
               playing = pause;
           }

           break;
       case reset://see paused states
           arrowPosition = 2;
           showArrow = true;
           choosePause();
           break;
       case pause:
           arrowPosition = 2;
           DrawPauseScreen();
           playing = reset;
           cScreen = neutral;
           break;
       case obstacle:
           break;
       case move:
           StartOneShotSWTimer(&OSTgame);//start 50 ms timer to move icon and obstacle
           playing = drive;
           break;
       case neutralGame:
           break;
   }
}

void loadScoreHistory()
{
    char ss1[2] = "XX";
    char ss2[2] = "XX";
    char ss3[2] = "XX";

    ss1[0] = ((score1/10)%10) + '0';
    ss1[1] = ((score1)%10) + '0';

    ss2[0] = ((score2/10)%10) + '0';//algorithm to print highest scores
    ss2[1] = ((score2)%10) + '0';

    ss3[0] = ((score3/10)%10) + '0';
    ss3[1] = ((score3)%10) + '0';

    LCDClearDisplay(MY_BLACK);
    PrintString("SCORES", 0, 2);
    PrintString("1. ", 2, 4);
    PrintString(ss1, 2, 7);
    PrintString("2. ", 4, 4);
    PrintString(ss2, 4, 7);
    PrintString("3. ", 6, 4);
    PrintString(ss3, 6, 7);
    PrintString("BTN1-MENU", 7, 6);
}

void updateScore()
{
    buf[0] = ((newScore/10)%10) + '0';
    buf[1] = ((newScore)%10) + '0';//updating score in game
    PrintString(buf, 1 , 11);
}

void initObstacles()
{
    unsigned x3, y3;
    int random = 0;
    int i = 0;

    for (i = 0; i < 2; i++)
    {
        getSampleJoyStick(&x3, &y3);
        int randBit = (x3%2) ^ (y3%2);
        random = (random << 1) + randBit;
    }
    startObstacle(random);
}

void moveObstacle()
{
    if (obstacle1)
    {
        if (obstacle1y == IMAGE_BOTTOM + 1)
        {
            obstacle1 = false;
            obstacle1x = 0;//moving obstacles
            newScore++;
            updateScore();
        }
        else
        {
            obstacle1y++;
            drawOverObstacle(obstacle1y, obstacle1x);
        }
    }

    if (obstacle2)
    {
        if (obstacle2y == IMAGE_BOTTOM + 1)
        {
            obstacle2 = false;
            obstacle2x = 0;
            newScore++;
            updateScore();
        }
        else
        {
            obstacle2y++;
            drawOverObstacle(obstacle2y, obstacle2x);
        }
    }

    if (obstacle3)
    {
        if (obstacle3y == IMAGE_BOTTOM + 1)
        {
            obstacle3 = false;
            obstacle3x = 0;
            newScore++;
            updateScore();
        }
        else
        {
            obstacle3y++;
            drawOverObstacle(obstacle3y, obstacle3x);
        }
    }
}

bool checkCollision()
{
    //comparing x's and y's
    if (obstacle1 && obstacle1x <= x+15 && obstacle1y <= IMAGE_BOTTOM && obstacle1x+15 >= x && obstacle1y+15 >= IMAGE_TOP)
    {
        showArrow = false;
        DrawGameOverScreen();//checking collision boxes for all 3 obstacles if obstacles are present
        playing = obstacle;
        return false;
    }
    else if (obstacle2 && obstacle2x <= x+15 && obstacle2y <= IMAGE_BOTTOM && obstacle2x+15 >= x && obstacle2y+15 >= IMAGE_TOP)
    {
        showArrow = false;
        DrawGameOverScreen();
        playing = obstacle;
        return false;
    }
    else if (obstacle3 && obstacle3x <= x+15 && obstacle3y <= IMAGE_BOTTOM && obstacle3x+15 >= x && obstacle3y+15 >= IMAGE_TOP)
    {
        showArrow = false;
        DrawGameOverScreen();
        playing = obstacle;
        return false;
    }
    else
    {
        showArrow = false;
        moveObstacle();
        return true;
    }
}

void checkForDebounce()
{
    if ((joystickPressed()) && (current == play) && stateMenu == MENU)// if on play game
   {
       showArrow = false;
       loadGameScreen();
       cScreen = run;
       playing = move;
   }
   else if ((joystickPressed()) && (current == instruct) && stateMenu == MENU)//if on instructions
   {
       showArrow = false;
       DrawInstructionScreen();
       cScreen = instructions;
   }
   else if ((joystickPressed()) && (current == score) && stateMenu == MENU)//if on score history
   {
       showArrow = false;
       loadScoreHistory();
       cScreen = scoreHist;
   }
}

void menuFSM()
{
    switch (cScreen)
    {
    case run:
        playGame();
        break;
    case instructions:
        if (Booster_Top_Button_Pushed())
        {
            showArrow = true;
            DrawMenuScreen();
            cScreen = neutral;
        }
        break;
    case scoreHist:
        if (Booster_Top_Button_Pushed())
        {
            showArrow = true;
            DrawMenuScreen();
            cScreen = neutral;
        }
        break;
    case neutral:
        break;
    }
    joyStickPosition();
    checkForDebounce();
}


int main(void) {
    WDT_A_hold(WDT_A_BASE);
    BSP_Clock_InitFastest();
    InitGraphics();
    InitHWTimers();
    InitButtons();
    InitLEDs();
    initADC_Multi();
    initJoyStick();
    startADC();

    static OneShotSWTimer_t OSTobstacle;
    static OneShotSWTimer_t OSTmoveob;
    static OneShotSWTimer_t OSTlose;
    InitOneShotSWTimer(&OSTlose, TIMER32_1_BASE, OPENING_WAIT);
    InitOneShotSWTimer(&OSTobstacle, TIMER32_1_BASE, WAIT_TIME);
    InitOneShotSWTimer(&OSTmoveob, TIMER32_1_BASE, MOVING_TIME);

    StartOneShotSWTimer(&OSTobstacle);
    while (1) {
        // Do not delete this statement. We will use this function to check if your program does not block on anything.
        if (Booster_Bottom_Button_Pushed())
            Toggle_Launchpad_Right_Red_LED();

        getSampleJoyStick(&vx, &vy);

       ScreensFSM();
       menuFSM();
       if (playing == obstacle)
       {
           StartOneShotSWTimer(&OSTlose);
           playing = neutralGame;
           cScreen = neutral;
       }
       if (OneShotSWTimerExpired(&OSTobstacle) && playing != obstacle && cScreen == run)
       {
           initObstacles();
           StartOneShotSWTimer(&OSTobstacle);
       }
       if (OneShotSWTimerExpired(&OSTmoveob) && playing != obstacle && cScreen == run)
       {
           if (checkCollision())
           {
               StartOneShotSWTimer(&OSTmoveob);
           }
       }
       if (OneShotSWTimerExpired(&OSTlose) && playing == neutralGame)
       {
           showArrow = true;
           DrawMenuScreen();
           current = play;
           position = center;
           playing = move;
           state = center;//reinitilaizing global variables to play again
           updateScoreHistory();
           newScore = 0;
           obstacle1y = 0;
           obstacle2y = 0;
           obstacle3y = 0;
           obstacle1x = 0;
           obstacle2x = 0;
           obstacle3x = 0;
           x = 2;
           obstacle1 = false;
           obstacle2 = false;
           obstacle3 = false;
       }
    }
}
