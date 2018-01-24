#include "myLib.h"
#include <stdlib.h>

unsigned short *videoBuffer = (unsigned short *)0x6000000;

void Pixelate(int row, int col, unsigned short color)
{
	videoBuffer[OFFSET(row, col, 240)] = color;
}

void drawRectangle(int row, int col, int height, int width, volatile u16 color)
{
	for(int r=0; r<height; r++) {
		DMA[3].src = &color;
		DMA[3].dst = videoBuffer + OFFSET(row+r, col, 240);
		DMA[3].cnt = width | DMA_SOURCE_FIXED | DMA_ON;
	}
}

void delay(int n)
{
	volatile int x = 0;
	for(int i=0; i<50*n; i++)
	{
		x = x + 1;
	}
}

void waitForVblank()
{
	while(SCANLINECOUNTER > 160);
	while(SCANLINECOUNTER<160);
}

void drawBar(OBSTACLE1 a) {
	if (a.bar_size > 0) {
		if (a.orient == 0) {
			drawRectangle(a.pos, 0, a.bar_size, a.gap_pos, a.color);
			int startNext = a.gap_pos + a.gap_size;
			drawRectangle(a.pos, startNext, a.bar_size, 240 - startNext, a.color);
		} else {
			drawRectangle(0, a.pos, a.gap_pos, a.bar_size, a.color);
			int startNext = a.gap_pos + a.gap_size;
			drawRectangle(startNext, a.pos, 160 - a.gap_pos, a.bar_size, a.color);
		}

	}
}
void drawSnakey(volatile int row[], volatile int col[], u16 color) {
	
	for (int i = 0; i < 6; i++) {
		drawRectangle(row[i],col[i],SNAKESIZE,SNAKESIZE,color);
	}

}

void drawImage3(int r, int c, int width, int height, const u16* image) { 
	
		for (int i = 0; i <height; i++) {
			DMA[3].src = image + i*width;
			DMA[3].dst = videoBuffer + (r+i)*240 + c;
			DMA[3].cnt = width | DMA_ON;

		}

	}


	int collisionEnemy(volatile int playerRow[], volatile int playerCol[], ENEMY enemys[]) {
		int L1Y;
		int L1X;
		int R1Y;
		int R1X;
		int answer;
		for(int j = 0; j < ENEMYNUMBER; j++) {
			int L2Y = enemys[j].row;
			int L2X = enemys[j].col;
			int R2Y = enemys[j].row + ENEMYSIZE;
			int R2X = enemys[j].col + ENEMYSIZE;
			for (signed i = SNAKELENGTH - 1; i >= 0; i--) {
				L1Y = playerRow[i];
				L1X = playerCol[i];
				R1Y = playerRow[i] + SNAKESIZE;
				R1X = playerCol[i] + SNAKESIZE;
				collision(L1X,L1Y,R1X,R1Y,L2X,L2Y,R2X,R2Y, answer);
				if (answer == 1) {
					return 1;
				}
			}
		}
		return 0;
	}

	int collisionBars(volatile int playerRow[], volatile int playerCol[], OBSTACLE1 bars[]) {
		int L1Y;
		int L1X;
		int R1Y;
		int R1X;
		int answer;
		int L2Y;
		int L2X;
		int R2Y;
		int R2X;
		for (signed i = SNAKELENGTH - 1; i >= 0; i--) {
			L1Y = playerRow[i];
			L1X = playerCol[i];
			R1Y = playerRow[i] + SNAKESIZE;
			R1X = playerCol[i] + SNAKESIZE;

			L2Y = bars[0].pos;
			L2X = 0;
			R2Y = bars[0].pos + bars[0].bar_size;
			R2X = bars[0].gap_pos;

			collision(L1X,L1Y,R1X,R1Y,L2X,L2Y,R2X,R2Y,answer);
			if (answer == 1) {
				return 1;
			} else {
				L2Y = bars[0].pos;
				L2X = bars[0].gap_pos + bars[0].gap_size;
				R2Y = bars[0].pos + bars[0].bar_size;
				R2X = 240;
				collision(L1X,L1Y,R1X,R1Y,L2X,L2Y,R2X,R2Y,answer);
				if (answer == 1) {
					return 1;
				} else {
					L2Y = 0;
					L2X = bars[1].pos;
					R2Y = bars[1].gap_pos;
					R2X = bars[1].pos + bars[1].bar_size;
					collision(L1X,L1Y,R1X,R1Y,L2X,L2Y,R2X,R2Y,answer);
					if (answer == 1) {
						return 1;
					} else {
						L2Y = bars[1].gap_pos + bars[1].gap_size;
						L2X = bars[1].pos;
						R2Y = 160;
						R2X = bars[1].pos + bars[1].bar_size;
						collision(L1X,L1Y,R1X,R1Y,L2X,L2Y,R2X,R2Y,answer);
						if (answer == 1) {
							return 1;
						}
					}
				}
			}
		}
		return 0;
	}

	void moveBars(OBSTACLE1 bars[]) {
		for (int i = 0; i < 2; i++) {
			if (bars[i].orient == 0) {
				if (bars[i].bar_size == 0) {
					bars[i].gap_pos = rand()%(240 - bars[i].gap_size - 7) + 3;
					if (rand()%10 > 4) {
						bars[i].pos = 0;
						bars[i].direction = 1;
					} else {
						bars[i].pos = 160 - OBSTACLE1SPEED;
						bars[i].direction = -1;
					}
					bars[i].bar_size = OBSTACLE1SPEED;
				}


				bars[i].pos = bars[i].pos + OBSTACLE1SPEED*bars[i].direction;

				if (bars[i].pos + bars[i].bar_size > 159) {
					bars[i].bar_size = MAX(160 - bars[i].pos,0);
					bars[i].pos = 160 - bars[i].bar_size;
				} else if (bars[i].pos < 0) {
					bars[i].bar_size = MAX(bars[i].bar_size + bars[i].pos, 0);
					bars[i].pos = 0;
				} else if (bars[i].bar_size < OBSTACLE1SIZE) {
					bars[i].pos = bars[i].pos - OBSTACLE1SPEED*bars[i].direction;
					bars[i].bar_size = MAX(bars[i].bar_size + OBSTACLE1SPEED, OBSTACLE1SIZE);
				}

			} else {
				if (bars[i].bar_size == 0) {
					bars[i].gap_pos = rand()%(160 - bars[i].gap_size - 7) + 3;
					if (rand()%10 > 4) {
						bars[i].pos = 0;
						bars[i].direction = 1;
					} else {
						bars[i].pos = 238 - OBSTACLE1SPEED;
						bars[i].direction = -1;
					}
					bars[i].bar_size = OBSTACLE1SPEED;
				}


				bars[i].pos = bars[i].pos + OBSTACLE1SPEED*bars[i].direction;
				if (bars[i].pos + bars[i].bar_size > 239) {
					bars[i].bar_size = MAX(239 - bars[i].pos,0);
					bars[i].pos = 239- bars[i].bar_size;
				} else if (bars[i].pos < 0) {
					bars[i].bar_size = MAX(bars[i].bar_size + bars[i].pos, 0);
					bars[i].pos = 0;
				} else if (bars[i].bar_size < OBSTACLE1SIZE) {
					bars[i].pos = bars[i].pos - OBSTACLE1SPEED*bars[i].direction;
					bars[i].bar_size = MAX(bars[i].bar_size + OBSTACLE1SPEED, OBSTACLE1SIZE);
				}

			}
		}
	}

	void enemyPositionUpdate(ENEMY enemys[], ENEMY oldEnemy[]) {
		for (int i = 0; i < ENEMYNUMBER; i++) {
			if (enemys[i].exist == 0) {
				int randQuad = rand() % 4;
				int randRow = rand() % QUADROWSIZE;
				int randCol = rand() % QUADCOLSIZE;
				if (randQuad == 0) {
					enemys[i].row = randRow;
					enemys[i].col = randCol;
					enemys[i].rowDir = 1;
					enemys[i].colDir = 1;
					enemys[i].exist = 1;
				} else if (randQuad == 1) {
					enemys[i].row = randRow;
					enemys[i].col = 240 - QUADCOLSIZE + randCol;
					enemys[i].rowDir = 1;
					enemys[i].colDir = -1;
					enemys[i].exist = 1;
				} else if(randQuad == 2) {
					enemys[i].row = 160 - QUADROWSIZE + randRow;
					enemys[i].col = randCol;
					enemys[i].rowDir = -1;
					enemys[i].colDir = 1;
					enemys[i].exist = 1;
				} else if (randQuad == 3) {
					enemys[i].row = 160 - QUADROWSIZE + randRow;
					enemys[i].col = 240 - QUADCOLSIZE + randCol;
					enemys[i].rowDir = -1;
					enemys[i].colDir = -1;
					enemys[i].exist = 1;
				}

			}

			oldEnemy[i] = enemys[i];


			if (enemys[i].row > 159 || enemys[i].row < 0 || enemys[i].col < 0 || enemys[i].col > 239) {
				enemys[i].exist = 0;
			} else {
				enemys[i].row = enemys[i].row + ENEMYSPEED*enemys[i].rowDir;
				enemys[i].col = enemys[i].col + ENEMYSPEED*enemys[i].colDir;
			}
		}

	}

	void updateSnakePos(volatile int playerRow[], volatile int playerCol[], volatile int *last) {
		volatile unsigned int i;
		if (KEY_DOWN_NOW(BUTTON_UP) || KEY_DOWN_NOW(BUTTON_DOWN) || KEY_DOWN_NOW(BUTTON_LEFT) || KEY_DOWN_NOW(BUTTON_RIGHT)) {
			*last = 0;
		}

		if (KEY_DOWN_NOW(BUTTON_UP) || *last == 1) {
			for (i = 0; i < (SNAKELENGTH - SNAKESPEED); i++) {
				playerRow[i] = playerRow[i+SNAKESPEED];
				playerCol[i] = playerCol[i+SNAKESPEED];
			}
			for (i = 0; i < SNAKESPEED; i++) {
				playerRow[i + SNAKELENGTH - SNAKESPEED] = playerRow[SNAKELENGTH-1] - (i+1)*SNAKESIZE;
				playerCol[i + SNAKELENGTH - SNAKESPEED] = playerCol[SNAKELENGTH-1];
			}
			*last = 1;
		} else if (KEY_DOWN_NOW(BUTTON_DOWN) || *last == 2) {
			for (i = 0; i < (SNAKELENGTH - SNAKESPEED); i++) {
				playerRow[i] = playerRow[i+SNAKESPEED];
				playerCol[i] = playerCol[i+SNAKESPEED];
			}
			for (i = 0; i < SNAKESPEED; i++) {
				playerRow[i + SNAKELENGTH - SNAKESPEED] = playerRow[SNAKELENGTH-1] + (i+1)*SNAKESIZE;
				playerCol[i + SNAKELENGTH - SNAKESPEED] = playerCol[SNAKELENGTH-1];
			}
			*last = 2;
		} else if (KEY_DOWN_NOW(BUTTON_LEFT) || *last == 3) {
			for (i = 0; i < (SNAKELENGTH - SNAKESPEED); i++) {
				playerRow[i] = playerRow[i+SNAKESPEED];
				playerCol[i] = playerCol[i+SNAKESPEED];
			}
			for (i = 0; i < SNAKESPEED; i++) {
				playerRow[i + SNAKELENGTH - SNAKESPEED] = playerRow[SNAKELENGTH-1];
				playerCol[i + SNAKELENGTH - SNAKESPEED] = playerCol[SNAKELENGTH-1] - (i+1)*SNAKESIZE;
			}
			*last = 3;
		} else if (KEY_DOWN_NOW(BUTTON_RIGHT) || *last == 4) {
			for (i = 0; i < (SNAKELENGTH - SNAKESPEED); i++) {
				playerRow[i] = playerRow[i+SNAKESPEED];
				playerCol[i] = playerCol[i+SNAKESPEED];
			}
			for (i = 0; i < SNAKESPEED; i++) {
				playerRow[i + SNAKELENGTH - SNAKESPEED] = playerRow[SNAKELENGTH-1];
				playerCol[i + SNAKELENGTH - SNAKESPEED] = playerCol[SNAKELENGTH-1] + (i+1)*SNAKESIZE;
			}
			*last = 4;
		}
	}

	void drawOldEnemy(ENEMY enemys[], u16 color) {
		for (int i = 0; i < ENEMYNUMBER; i++) {
			drawRectangle(enemys[i].row,enemys[i].col,ENEMYSIZE, ENEMYSIZE, color);
		}
	}

	void newEnemyDraw(ENEMY enemys[], u16 color) {
		for (int i = 0; i < ENEMYNUMBER; i++) {
			if (enemys[i].exist == 1) {
				drawRectangle(enemys[i].row,enemys[i].col,ENEMYSIZE, ENEMYSIZE, color);
			}
		}
	}