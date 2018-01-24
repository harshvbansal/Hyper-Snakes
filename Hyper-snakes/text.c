#include "myLib.h"
#include "text.h"

void drawChar(int rows, int cols, char ch, u16 colsor)
{
	for(int r=0; r<8; r++) {
		for(int c=0; c<6; c++) {
			if(fontdata_6x8[((r*6) + c) + ch*48]) {
				Pixelate(rows+r, cols+c, colsor);
			}
		}
	}
}

void drawString(int rows, int cols, char*str, u16 colsor)
{
	while(*str) {
		drawChar(rows,cols,*str++,colsor);
		cols += 6;
	}
}
