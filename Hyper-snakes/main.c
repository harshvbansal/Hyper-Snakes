#include "myLib.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "pics/startBackground.h"
#include "pics/gameOver.h"
#include "text.h"
#include "pics/foodPic.h"
int main()
{

	REG_DISPCTL = MODE3 | BG2_ENABLE;
	u16 bgcolor = BGCOLOR;

	int d = 250;
	int state = 0;


	//INITIALIZING THE OBSTACLE1S AND SCORES
	OBSTACLE1 bars[NUMOBSTACLE1];
	OBSTACLE1 oldbars[NUMOBSTACLE1];
	char buffer[40];
	char bufferOld[40];
	int bufferChange = 0;
	
	//INITIALIZING THE SNAKE
	volatile int playerRow[SNAKELENGTH];
	volatile int oldPlayerRow[SNAKELENGTH];
	volatile int playerCol[SNAKELENGTH];
	volatile int oldPlayerCol[SNAKELENGTH];
	volatile unsigned int i;
	

	//INITIALIZING THE FOOD;
	FOOD food;
	FOOD oldFood;
	food.hit = 0;
	food.row = rand()%(160-10)+5;
	food.col = rand()%(240-14)+7;
	food.count = 0;
	food.color = BLUE;
	oldFood = food;

	//INITALIZE THE ENEMY
	ENEMY enemys[ENEMYNUMBER];
	ENEMY enemysOld[ENEMYNUMBER];
	for (int i = 0; i < ENEMYNUMBER; i++) {
		enemys[i].row = 0;
		enemys[i].col = 0;
		enemys[i].rowDir = 1;
		enemys[i].colDir = 1;
		enemys[i].exist = 1;
		enemysOld[i] = enemys[i];
	}
	

	
	volatile int last = 4;
	while(1) {


		// If you RESET game, return to state 0.
		if (KEY_DOWN_NOW(BUTTON_SELECT)) {
			state = 0;
		}


		//If in state -1 (same as START state) and Press the START button, then go to state 1,
		// which is the beggining of the game;
		if (state == -1 && KEY_DOWN_NOW(BUTTON_START)) {
			state = 1;
		}


		//STATE 1 initalizes the start of the game. The start pos of the bars, the snake, the food and the
		//obstacles are set. Score set to 0 as well.
		if (state == 1) {

			d = 250;
			bars[0].pos = 0;
			bars[0].gap_size = 120;
			bars[0].gap_pos = 240/2 - bars[0].gap_size/2;
			bars[0].orient = 0;
			bars[0].color = GREEN;
			bars[0].bar_size = OBSTACLE1SIZE;
			bars[0].direction = 1;

			bars[1].pos = 0;
			bars[1].gap_size = 60;
			bars[1].gap_pos = 160/2 - bars[1].gap_size/2;
			bars[1].orient = 1;
			bars[1].color = GREEN;
			bars[1].bar_size = OBSTACLE1SIZE;
			bars[1].direction = 1;

			for (i = 0; i < SNAKELENGTH; i++)	{
				playerRow[i] = 79;
				oldPlayerRow[i] = 79;
			}

			for (i = 0; i < SNAKELENGTH; i++)	{
				playerCol[i] = 120 + i*SNAKESIZE;
				oldPlayerCol[i] = 120 + i*SNAKESIZE;
			}

			
			food.hit = 0;
			food.row = rand()%(160-10)+5;
			food.col = rand()%(240-14)+7;
			food.count = 0;
			food.color = BLUE;
			

			for (int i = 0; i < ENEMYNUMBER; i++) {
				enemys[i].row = 0;
				enemys[i].col = 0;
				enemys[i].rowDir = 1;
				enemys[i].colDir = 1;
				enemys[i].exist = 1;
				enemysOld[i] = enemys[i];
			}

			sprintf(buffer, "Score: %i", 0);
			bufferChange = 1;
			last = 4;
		}


		
		
		//STATE 2 is the state of the game while you're playing. So it updates the poss of all the
		//objects based on user input and randomly decided movement.
		if (state == 2) {


			//CHECK IF SNAKE COLLIDED WITH FOOD
			int L2Y = food.row;
			int L2X = food.col;
			int R2Y = food.row + FOODSIZE;
			int R2X = food.col + FOODSIZE;
			int cont = 1;
			for (int i = SNAKELENGTH - 1; i>=0 && cont == 1; i--) {
				int L1Y = playerRow[i];
				int L1X = playerCol[i];
				int R1Y = playerRow[i] + SNAKESIZE;
				int R1X = playerCol[i] + SNAKESIZE;
				int answer;
				collision(L1X,L1Y,R1X,R1Y,L2X,L2Y,R2X,R2Y,answer);
				if (answer == 1)  {
					oldFood.row = food.row;
					oldFood.col = food.col;
					food.hit = 0;
					food.row = rand()%(160-10)+5;
					food.col = rand()%(240-14)+7;
					food.count = food.count + 1;
					sprintf(bufferOld, buffer);
					sprintf(buffer, "Score: %i", food.count);
					bufferChange = 1;
					cont = 0;
					d = (d > 100) ? d - 2 : 100;
				}
			}




			//CHECK IF SNAKKE COLLIDED WITH OBSTACLE1S, OR WENT OFF SCREEN.
			//IF IT DID, THEN GO TO STATE 3 WHICH IS THE GAME OVER STATE
			if (playerRow[SNAKELENGTH - 1] > 159 || playerRow[SNAKELENGTH - 1] < -2 || playerCol[SNAKELENGTH - 1] > 239 || playerCol[SNAKELENGTH - 1] < 0) {
				state = 3;
				sprintf(buffer, "FINAL SCORE: %i", food.count);
			} else if (collisionEnemy(playerRow, playerCol, enemys) == 1) {
				state = 3;
				sprintf(buffer, "FINAL SCORE: %i", food.count);
			} else if (collisionBars(playerRow,playerCol,bars) == 1) {
				state = 3;
				sprintf(buffer, "FINAL SCORE: %i", food.count);
			} else {

				//UPDATE SNAKE pos BASED ON USER INPUT
				

				for (i = 0; i < SNAKELENGTH; i++) {
					oldPlayerCol[i] = playerCol[i];
					oldPlayerRow[i] = playerRow[i];
				}
				updateSnakePos(playerRow, playerCol, &last);
				//The variable 'last' makes sure that snake keeps moving in the same direction, until the
				//user presses some other direction key.

				

				//UPDATE THE OBSTACLE1S pos.
				oldbars[0] = bars[0];
				oldbars[1] = bars[1];
				oldbars[0].color = BGCOLOR;
				oldbars[1].color = BGCOLOR;
				moveBars(bars);
				


				//UPDATE ENEMY pos
				enemyPositionUpdate(enemys, enemysOld);


			}
		}


		delay(d);
		waitForVblank();
		//State 0 is start scene. So prints start backgorund. Goes to state -1 so that it
		// doesn't keep drawing the start screen again and again.
		if (state == 0) {
			drawImage3(0,0,240,160,startBackground);
			drawString(150, 50,"MADE BY: HARSH VARDHAN BANSAL", BLACK);
			state = -1;
		}

		//The initial state when just starting the game. Goes directly to State 2 which handles
		//the processes when the game is ongoing.
		if (state == 1) {
			DMA[3].src = &bgcolor;
			DMA[3].dst = videoBuffer;
			DMA[3].cnt = 38400 | DMA_ON | DMA_SOURCE_FIXED;
			state = 2;
		}

		//State when game is ongoing. First draws the previous locations of objects with background color. Then draws
		//the new poss of those objects
		if (state == 2) {	
			drawBar(oldbars[0]);
			drawBar(oldbars[1]);
			drawSnakey(oldPlayerRow, oldPlayerCol, BGCOLOR);
			drawRectangle(oldFood.row,oldFood.col,FOODSIZE,FOODSIZE,BGCOLOR);
			if (bufferChange == 1) {
				drawString(145,5,bufferOld, BGCOLOR);
				bufferChange = 0;
				
			}
			drawOldEnemy(enemysOld, BGCOLOR);

			drawBar(bars[0]);
			drawBar(bars[1]);
			drawImage3(food.row,food.col,7,7,foodPic);

			newEnemyDraw(enemys, ENEMYCOLOR);

			drawSnakey(playerRow,playerCol, SNAKECOLOR);
			drawString(145,5,buffer, WHITE);		
		}

		//When we reach this state it means game over.
		if (state == 3) {
			drawImage3(0,0,240,160,gameOver);
			drawString(50,75,buffer, WHITE);
			state = -2;
		}
		
		
	}
}

