/*****************************************************************//**
 * @file main_video_test.cpp
 *
 * @brief Basic test of 4 basic i/o cores
 *
 * @author p chu
 * @version v1.0: initial release
 *********************************************************************/

//#define _DEBUG
#include "chu_init.h"
#include "gpio_cores.h"
#include "vga_core.h"
#include "lfsr_core.h"
#include "tile.h"

//using namespace std;

// external core instantiation
GpoCore led(get_slot_addr(BRIDGE_BASE, S2_LED));
GpiCore sw(get_slot_addr(BRIDGE_BASE, S3_SW));
FrameCore frame(FRAME_BASE);
GpvCore bar(get_sprite_addr(BRIDGE_BASE, V7_BAR));
GpvCore gray(get_sprite_addr(BRIDGE_BASE, V6_GRAY));
SpriteCore ghost(get_sprite_addr(BRIDGE_BASE, V3_GHOST), 1024);
SpriteCore mouse(get_sprite_addr(BRIDGE_BASE, V1_MOUSE), 1024);
SpriteCore square(get_sprite_addr(BRIDGE_BASE, V5_USER5), 1024);
OsdCore osd(get_sprite_addr(BRIDGE_BASE, V2_OSD));
DebounceCore btn(get_slot_addr(BRIDGE_BASE, S7_BTN));
TimerCore timer(get_slot_addr(BRIDGE_BASE, S0_SYS_TIMER));
LfsrCore lfsr(get_slot_addr(BRIDGE_BASE, S4_USER));

// btnu : btn(0);
// btnr : btn(1);
// btnd : btn(2);
// btnl : btn(3);
// btnc : btn(4);

// Constants for btn index lookup
#define btnc  4
#define btnd  2
#define btnu  0
#define btnl  3
#define btnr  1
#define size(arr) (sizeof(arr)/sizeof(arr[0]))

int spriteOffset = 62; // 62 pixels (Sprite movement offset per-tile)
int messageLine = 1; // Row for printing out the message
int difficultyLevel = 1;

char message[] = "L# Time: ### second(s)";
int time = 0;
uint32_t data;
Tile* grid;
int selX;
int selY;
int xOffset = 6;
int yOffset = -5;
int currTile = 0;


/*
 * Function Prototypes
 */
void generateGrid();
void updateMessage(char* msg, int stringLength);
void showPattern(int sel);
void playWinningPattern();
void playLosingPattern();
void renderTile(Tile tile);
void selectTile();
void moveHighlighter(int x, int y);
void moveHorizontal(int dir);
void moveVertical(int dir);

int getButton(int buttonIndex);
int getSwitch(int switchIndex);
int checkIfWon();
int checkTime();

// Print out a white-line grid two lines at every vertical position
void generateGrid() {

	int screenX, screenY, gridWidth, gridHeight;
	int hStart, hEnd, vStart, vEnd;

	grid = (Tile*)malloc(sizeof(Tile)*36);

	screenX = 640;
	screenY = 480;
	gridWidth = 384;
	gridHeight = 384;

	// Center the grid
	hStart = (screenX - gridWidth)/2; // The offset from the left and right sides to center the grid
	hEnd = screenX - hStart;
	hStart -= 1;

	vStart = (screenY - gridHeight)/2; // The offset from the top and bottom sides to center the grid
	vEnd = screenY - vStart;
	vStart -= 1;
	//int lastJval = 0; // used for debugging loop
	// Print horizontal lines
	for(int j = vStart; j <= vEnd; j+= 64) {
		if((j+1) %2 == 0) {
			frame.plot_line(hStart, j, hEnd, j, 0);
			frame.plot_line(hStart, j+1, hEnd, j+1, 0);
		}
		//lastJval = j; // used for debugging loop
	}

	//int lastIval = 0; // used for debugging loop
	// Print vertical lines
	for(int i = hStart; i <= hEnd; i+= 64) {
		if((i+1) %2 == 0) {
			frame.plot_line(i, vStart, i, vEnd, 0);
			frame.plot_line(i+1, vStart, i+1, vEnd, 0);
		}
		//lastIval = i; //used for debugging loop
	}

	// Create tiles to add to the grid:
	int xStart, xEnd, yStart, yEnd;
	xStart = (hStart+2);
	yStart = (vStart+2);
	xEnd = xStart+62;
	yEnd = yStart+62;
	int counter = 0;

	for(int i = 0; i < 6; i++) {
		xStart = (hStart+2);
		xEnd = (xStart+62);
		for(int i = 0; i < 6; i++) {
			Tile newTile(xStart, yStart, xEnd, yEnd, 0);
			//newGrid[counter] = newTile;
			grid[counter] = newTile;
			//renderTile(newGrid[counter]);
			renderTile(grid[counter]);
			xStart += 64;
			xEnd += 64;
			counter++;
		}
		yStart += 64;
		yEnd += 64;
	}

	/*
	for(int i = 0; i < 36; i++) {
		// Set each tile's associated tiles:
		switch(i) {
			// Corner Tiles:
			case 0:
				grid[i].associatedTileCount = 2;
				grid[i].associatedTiles[0] = grid[i+1];
				grid[i].associatedTiles[1] = grid[i+6];
				break;


			case 30:
				grid[i].associatedTileCount = 2;
				grid[i].associatedTiles[0] = grid[i-6];
				grid[i].associatedTiles[1] = grid[i+1];
				break;


			case 35:
				grid[i].associatedTileCount = 2;
				grid[i].associatedTiles[0] = grid[i-6];
				grid[i].associatedTiles[1] = grid[i-1];
				break;

			case 5:
				grid[i].associatedTileCount = 2;
				grid[i].associatedTiles[0] = grid[i-1];
				grid[i].associatedTiles[1] = grid[i+6];
				break;

			// Left-side edge tiles
			case 6:
				grid[i].associatedTileCount = 3;
				grid[i].associatedTiles[0] = grid[i-6];
				grid[i].associatedTiles[1] = grid[i+1];
				grid[i].associatedTiles[1] = grid[i+6];
				break;

			case 12:
				grid[i].associatedTileCount = 3;
				grid[i].associatedTiles[0] = grid[i-6];
				grid[i].associatedTiles[1] = grid[i+1];
				grid[i].associatedTiles[1] = grid[i+6];
				break;

			case 18:
				grid[i].associatedTileCount = 3;
				grid[i].associatedTiles[0] = grid[i-6];
				grid[i].associatedTiles[1] = grid[i+1];
				grid[i].associatedTiles[1] = grid[i+6];
				break;

			case 24:
				grid[i].associatedTileCount = 3;
				grid[i].associatedTiles[0] = grid[i-6];
				grid[i].associatedTiles[1] = grid[i+1];
				grid[i].associatedTiles[1] = grid[i+6];
				break;

			// Right-side edge tiles
			case 11:
				grid[i].associatedTileCount = 3;
				grid[i].associatedTiles[0] = grid[i-6];
				grid[i].associatedTiles[1] = grid[i-1];
				grid[i].associatedTiles[1] = grid[i+6];
				break;

			case 17:
				grid[i].associatedTileCount = 3;
				grid[i].associatedTiles[0] = grid[i-6];
				grid[i].associatedTiles[1] = grid[i-1];
				grid[i].associatedTiles[1] = grid[i+6];
				break;

			case 23:
				grid[i].associatedTileCount = 3;
				grid[i].associatedTiles[0] = grid[i-6];
				grid[i].associatedTiles[1] = grid[i-1];
				grid[i].associatedTiles[1] = grid[i+6];
				break;

			case 29:
				grid[i].associatedTileCount = 3;
				grid[i].associatedTiles[0] = grid[i-6];
				grid[i].associatedTiles[1] = grid[i-1];
				grid[i].associatedTiles[1] = grid[i+6];
				break;

			// top-side edge tiles
			case 1:
				grid[i].associatedTileCount = 3;
				grid[i].associatedTiles[0] = grid[i-1];
				grid[i].associatedTiles[1] = grid[i+1];
				grid[i].associatedTiles[1] = grid[i+6];
				break;

			case 2:
				grid[i].associatedTileCount = 3;
				grid[i].associatedTiles[0] = grid[i-1];
				grid[i].associatedTiles[1] = grid[i+1];
				grid[i].associatedTiles[1] = grid[i+6];
				break;

			case 3:
				grid[i].associatedTileCount = 3;
				grid[i].associatedTiles[0] = grid[i-1];
				grid[i].associatedTiles[1] = grid[i+1];
				grid[i].associatedTiles[1] = grid[i+6];
				break;

			case 4:
				grid[i].associatedTileCount = 3;
				grid[i].associatedTiles[0] = grid[i-1];
				grid[i].associatedTiles[1] = grid[i+1];
				grid[i].associatedTiles[1] = grid[i+6];
				break;

			// bottom-side edge tiles
			case 31:
				grid[i].associatedTileCount = 3;
				grid[i].associatedTiles[0] = grid[i-1];
				grid[i].associatedTiles[1] = grid[i+1];
				grid[i].associatedTiles[1] = grid[i-6];
				break;

			case 32:
				grid[i].associatedTileCount = 3;
				grid[i].associatedTiles[0] = grid[i-1];
				grid[i].associatedTiles[1] = grid[i+1];
				grid[i].associatedTiles[1] = grid[i-6];
				break;

			case 33:
				grid[i].associatedTileCount = 3;
				grid[i].associatedTiles[0] = grid[i-1];
				grid[i].associatedTiles[1] = grid[i+1];
				grid[i].associatedTiles[1] = grid[i-6];
				break;

			case 34:
				grid[i].associatedTileCount = 3;
				grid[i].associatedTiles[0] = grid[i-1];
				grid[i].associatedTiles[1] = grid[i+1];
				grid[i].associatedTiles[1] = grid[i-6];
				break;

			// Anywhere else
			default:
				grid[i].associatedTileCount = 4;
				grid[i].associatedTiles[0] = grid[i-1];
				grid[i].associatedTiles[1] = grid[i+1];
				grid[i].associatedTiles[2] = grid[i+6];
				grid[i].associatedTiles[3] = grid[i-6];
				break;

		}
	}*/

}

void renderTile(Tile tile) {
	for(int y = tile.startY; y <= tile.endY; y++) {
		frame.plot_line(tile.startX, y, tile.endX, y, tile.color);
	}
}

// Writes the provided message to the screen at the line just above the 6x6 table
void updateMessage() {
	uart.disp("Entered updateMessage()");
	int posX = 144;

	char diffChar[1]; // 1-7
	char timeArr[3]; // Max amount of time will be 420 seconds

	uart.disp("Before itoa conversions\n");
	itoa(time, timeArr, 10); // Base 10 is the decimal value
	itoa(difficultyLevel, diffChar, 10);

	// Assign this to the appropriate indices in the message array
	// L 7 _ T i m e : _ #  #  # _  s  e  c  o  n  d  (  s  )
	// 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21

	uart.disp("Assigning message values at specific indices\n");
	message[1] 	= diffChar[0];
	message[9] 	= timeArr[0];
	message[10] = timeArr[1];
	message[11] = timeArr[2];

	uart.disp("Before for-loop\n");
	// Go to the Y position and start printing from the defined X position
	for(int i = posX; i < posX + 22; i++) {
		osd.wr_char(i, messageLine, message[i-posX]);
	}
}

void showPattern(int sel) {
	switch(sel) {
		case 0:
			// Set top-left grid to data pattern
			grid[0].color = bit_read(data, 0);
			renderTile(grid[0]);
			grid[1].color = bit_read(data, 1);
			renderTile(grid[1]);
			grid[2].color = bit_read(data, 2);
			renderTile(grid[2]);
			grid[6].color = bit_read(data, 3);
			renderTile(grid[6]);
			grid[7].color = bit_read(data, 4);
			renderTile(grid[7]);
			grid[8].color = bit_read(data, 5);
			renderTile(grid[8]);
			grid[12].color = bit_read(data, 6);
			renderTile(grid[12]);
			grid[13].color = bit_read(data, 7);
			renderTile(grid[13]);
			grid[14].color = bit_read(data, 8);
			renderTile(grid[14]);

			break;
		case 1:
			// Set top-right grid to data pattern
			grid[3].color = bit_read(data, 0);
			renderTile(grid[3]);
			grid[4].color = bit_read(data, 1);
			renderTile(grid[4]);
			grid[5].color = bit_read(data, 2);
			renderTile(grid[5]);
			grid[9].color = bit_read(data, 3);
			renderTile(grid[9]);
			grid[10].color = bit_read(data, 4);
			renderTile(grid[10]);
			grid[11].color = bit_read(data, 5);
			renderTile(grid[11]);
			grid[15].color = bit_read(data, 6);
			renderTile(grid[15]);
			grid[16].color = bit_read(data, 7);
			renderTile(grid[16]);
			grid[17].color = bit_read(data, 8);
			renderTile(grid[17]);

			break;

		case 2:
			// Set bottom-right grid to data pattern
			grid[21].color = bit_read(data, 0);
			renderTile(grid[21]);
			grid[22].color = bit_read(data, 1);
			renderTile(grid[22]);
			grid[23].color = bit_read(data, 2);
			renderTile(grid[23]);
			grid[27].color = bit_read(data, 3);
			renderTile(grid[27]);
			grid[28].color = bit_read(data, 4);
			renderTile(grid[28]);
			grid[29].color = bit_read(data, 5);
			renderTile(grid[29]);
			grid[33].color = bit_read(data, 6);
			renderTile(grid[33]);
			grid[34].color = bit_read(data, 7);
			renderTile(grid[34]);
			grid[35].color = bit_read(data, 8);
			renderTile(grid[35]);

			break;

		case 3:
			// Set bottom-left grid to data pattern
			grid[18].color = bit_read(data, 0);
			renderTile(grid[18]);
			grid[19].color = bit_read(data, 1);
			renderTile(grid[19]);
			grid[20].color = bit_read(data, 2);
			renderTile(grid[20]);
			grid[24].color = bit_read(data, 3);
			renderTile(grid[24]);
			grid[25].color = bit_read(data, 4);
			renderTile(grid[25]);
			grid[26].color = bit_read(data, 5);
			renderTile(grid[26]);
			grid[30].color = bit_read(data, 6);
			renderTile(grid[30]);
			grid[31].color = bit_read(data, 7);
			renderTile(grid[31]);
			grid[32].color = bit_read(data, 8);
			renderTile(grid[32]);

			break;
	}
}

void playWinningPattern() {
	uart.disp("Entered losing pattern\n");
	frame.clr_screen(0);
	for(int i = 0; i < 36; i++) {
		grid[i].color = 0;
	}
	uart.disp("Cleared the screen\n");
	for(int i = 0; i < 5; i++) {
		for(int j = 0; j < 36; j++) {
			grid[j].color = !grid[j].color;
			renderTile(grid[j]);
		}
	}
}

void playLosingPattern() {
	uart.disp("Entered losing pattern\n");
	frame.clr_screen(0);
	for(int i = 0; i < 36; i++) {
		grid[i].color = 0;
	}
	uart.disp("Cleared the screen\n");
	for(int i = 0; i < 5; i++) {
		uart.disp("In losing pattern for loop\n");
		grid[0].color = !grid[0].color;
		grid[7].color = !grid[7].color;
		grid[14].color = !grid[14].color;
		grid[21].color = !grid[21].color;
		grid[28].color = !grid[28].color;
		grid[35].color = !grid[35].color;

		grid[30].color = !grid[30].color;
		grid[25].color = !grid[25].color;
		grid[20].color = !grid[20].color;
		grid[15].color = !grid[15].color;
		grid[10].color = !grid[10].color;
		grid[5].color = !grid[5].color;

		renderTile(grid[0]);
		renderTile(grid[7]);
		renderTile(grid[14]);
		renderTile(grid[21]);
		renderTile(grid[28]);
		renderTile(grid[35]);

		renderTile(grid[30]);
		renderTile(grid[25]);
		renderTile(grid[20]);
		renderTile(grid[15]);
		renderTile(grid[10]);
		renderTile(grid[5]);
	}

}

// Gets the current button's input with debouncing
int getButton(int buttonIndex) {
	if(btn.read_db(buttonIndex)) {
		while(btn.read_db(buttonIndex));
		return 1;
	}
	else {
		return 0;
	}
}

// Gets the current switch input
int getSwitch(int switchIndex) {
	if(sw.read(switchIndex)) {
		return 1;
	}
	else {
		return 0;
	}
}

uint32_t getTimeLimit() {
	if(sw.read(15)) {
		// time unit is 30 seconds
		return 30;
	}
	else {
		// time unit is a minute (60 seconds)
		return 60;
	}
}

int checkIfWon() {
	for(int i = 0; i < 36; i++) {
		if(grid[i].color == 1) {
			return 0;
		}
	}
	return 1;
}

void selectTile() {
	grid[currTile].color = !grid[currTile].color;
	renderTile(grid[currTile]);
	// Set each tile's associated tiles to the opposite color:
	int i = currTile;
	switch(i) {
		// Corner Tiles:
		case 0:
			grid[i+1].color = !grid[i+1].color;
			grid[i+6].color = !grid[i+6].color;
			renderTile(grid[i+1]);
			renderTile(grid[i+6]);
			break;


		case 30:
			grid[i-6].color = !grid[i-6].color;
			grid[i+1].color = !grid[i+1].color;
			renderTile(grid[i-6]);
			renderTile(grid[i+1]);
			break;


		case 35:
			grid[i-6].color = !grid[i-6].color;
			grid[i-1].color = !grid[i-1].color;
			renderTile(grid[i-1]);
			renderTile(grid[i-6]);
			break;

		case 5:
			grid[i-1].color = !grid[i-1].color;
			grid[i+6].color = !grid[i+6].color;
			renderTile(grid[i-1]);
			renderTile(grid[i+6]);
			break;

		// Left-side edge tiles
		case 6:
			grid[i-6].color = !grid[i-6].color;
			grid[i+1].color = !grid[i+1].color;
			grid[i+6].color = !grid[i+6].color;
			renderTile(grid[i+1]);
			renderTile(grid[i+6]);
			renderTile(grid[i-6]);
			break;

		case 12:
			grid[i-6].color = !grid[i-6].color;
			grid[i+1].color = !grid[i+1].color;
			grid[i+6].color = !grid[i+6].color;
			renderTile(grid[i+1]);
			renderTile(grid[i+6]);
			renderTile(grid[i-6]);
			break;

		case 18:
			grid[i-6].color = !grid[i-6].color;
			grid[i+1].color = !grid[i+1].color;
			grid[i+6].color = !grid[i+6].color;
			renderTile(grid[i+1]);
			renderTile(grid[i+6]);
			renderTile(grid[i-6]);
			break;

		case 24:
			grid[i-6].color = !grid[i-6].color;
			grid[i+1].color = !grid[i+1].color;
			grid[i+6].color = !grid[i+6].color;
			renderTile(grid[i+1]);
			renderTile(grid[i+6]);
			renderTile(grid[i-6]);
			break;

		// Right-side edge tiles
		case 11:
			grid[i-6].color = !grid[i-6].color;
			grid[i-1].color = !grid[i-1].color;
			grid[i+6].color = !grid[i+6].color;
			renderTile(grid[i-1]);
			renderTile(grid[i+6]);
			renderTile(grid[i-6]);
			break;

		case 17:
			grid[i-6].color = !grid[i-6].color;
			grid[i-1].color = !grid[i-1].color;
			grid[i+6].color = !grid[i+6].color;
			renderTile(grid[i-1]);
			renderTile(grid[i+6]);
			renderTile(grid[i-6]);
			break;

		case 23:
			grid[i-6].color = !grid[i-6].color;
			grid[i-1].color = !grid[i-1].color;
			grid[i+6].color = !grid[i+6].color;
			renderTile(grid[i-1]);
			renderTile(grid[i+6]);
			renderTile(grid[i-6]);
			break;

		case 29:
			grid[i-6].color = !grid[i-6].color;
			grid[i-1].color = !grid[i-1].color;
			grid[i+6].color = !grid[i+6].color;
			renderTile(grid[i-1]);
			renderTile(grid[i+6]);
			renderTile(grid[i-6]);
			break;

		// top-side edge tiles
		case 1:
			grid[i-1].color = !grid[i-1].color;
			grid[i+1].color = !grid[i+1].color;
			grid[i+6].color = !grid[i+6].color;
			renderTile(grid[i-1]);
			renderTile(grid[i+1]);
			renderTile(grid[i+6]);
			break;

		case 2:
			grid[i-1].color = !grid[i-1].color;
			grid[i+1].color = !grid[i+1].color;
			grid[i+6].color = !grid[i+6].color;
			renderTile(grid[i-1]);
			renderTile(grid[i+1]);
			renderTile(grid[i+6]);
			break;

		case 3:
			grid[i-1].color = !grid[i-1].color;
			grid[i+1].color = !grid[i+1].color;
			grid[i+6].color = !grid[i+6].color;
			renderTile(grid[i-1]);
			renderTile(grid[i+1]);
			renderTile(grid[i+6]);
			break;

		case 4:
			grid[i-1].color = !grid[i-1].color;
			grid[i+1].color = !grid[i+1].color;
			grid[i+6].color = !grid[i+6].color;
			renderTile(grid[i-1]);
			renderTile(grid[i+1]);
			renderTile(grid[i+6]);
			break;

		// bottom-side edge tiles
		case 31:
			grid[i-1].color = !grid[i-1].color;
			grid[i+1].color = !grid[i+1].color;
			grid[i-6].color = !grid[i-6].color;
			renderTile(grid[i-1]);
			renderTile(grid[i+1]);
			renderTile(grid[i-6]);
			break;

		case 32:
			grid[i-1].color = !grid[i-1].color;
			grid[i+1].color = !grid[i+1].color;
			grid[i-6].color = !grid[i-6].color;
			renderTile(grid[i-1]);
			renderTile(grid[i+1]);
			renderTile(grid[i-6]);
			break;

		case 33:
			grid[i-1].color = !grid[i-1].color;
			grid[i+1].color = !grid[i+1].color;
			grid[i-6].color = !grid[i-6].color;
			renderTile(grid[i-1]);
			renderTile(grid[i+1]);
			renderTile(grid[i-6]);
			break;

		case 34:
			grid[i-1].color = !grid[i-1].color;
			grid[i+1].color = !grid[i+1].color;
			grid[i-6].color = !grid[i-6].color;
			renderTile(grid[i-1]);
			renderTile(grid[i+1]);
			renderTile(grid[i-6]);
			break;

		// Anywhere else
		default:
			grid[i-1].color = !grid[i-1].color;
			grid[i+1].color = !grid[i+1].color;
			grid[i+6].color = !grid[i+6].color;
			grid[i-6].color = !grid[i-6].color;

			renderTile(grid[i-1]);
			renderTile(grid[i+1]);
			renderTile(grid[i+6]);
			renderTile(grid[i-6]);
			break;

	}
}

void moveHorizontal(int dir) {
	// Right
	if(dir) {
		switch(currTile) {
			case 5:
				currTile = 0;
				moveHighlighter(grid[currTile].centerX, grid[currTile].centerY);
				break;
			case 11:
				currTile = 6;
				moveHighlighter(grid[currTile].centerX, grid[currTile].centerY);
				break;
			case 17:
				currTile = 12;
				moveHighlighter(grid[currTile].centerX, grid[currTile].centerY);
				break;
			case 23:
				currTile = 18;
				moveHighlighter(grid[currTile].centerX, grid[currTile].centerY);
				break;
			case 29:
				currTile = 24;
				moveHighlighter(grid[currTile].centerX, grid[currTile].centerY);
				break;
			case 35:
				currTile = 30;
				moveHighlighter(grid[currTile].centerX, grid[currTile].centerY);
				break;
			default:
				currTile += 1;
				moveHighlighter(grid[currTile].centerX, grid[currTile].centerY);
				break;
		}
	}
	// Left
	else {
		switch(currTile) {
			case 0:
				currTile = 5;
				moveHighlighter(grid[currTile].centerX, grid[currTile].centerY);
				break;
			case 6:
				currTile = 11;
				moveHighlighter(grid[currTile].centerX, grid[currTile].centerY);
				break;
			case 12:
				currTile = 17;
				moveHighlighter(grid[currTile].centerX, grid[currTile].centerY);
				break;
			case 18:
				currTile = 23;
				moveHighlighter(grid[currTile].centerX, grid[currTile].centerY);
				break;
			case 24:
				currTile = 29;
				moveHighlighter(grid[currTile].centerX, grid[currTile].centerY);
				break;
			case 30:
				currTile = 35;
				moveHighlighter(grid[currTile].centerX, grid[currTile].centerY);
				break;
			default:
				currTile -= 1;
				moveHighlighter(grid[currTile].centerX, grid[currTile].centerY);
				break;
		}
	}
}

void moveVertical(int dir) {
	// Up
	if(dir) {
		switch(currTile) {
			case 0:
				currTile = 30;
				moveHighlighter(grid[currTile].centerX, grid[currTile].centerY);
				break;
			case 1:
				currTile = 31;
				moveHighlighter(grid[currTile].centerX, grid[currTile].centerY);
				break;
			case 2:
				currTile = 32;
				moveHighlighter(grid[currTile].centerX, grid[currTile].centerY);
				break;
			case 3:
				currTile = 33;
				moveHighlighter(grid[currTile].centerX, grid[currTile].centerY);
				break;
			case 4:
				currTile = 34;
				moveHighlighter(grid[currTile].centerX, grid[currTile].centerY);
				break;
			case 5:
				currTile = 35;
				moveHighlighter(grid[currTile].centerX, grid[currTile].centerY);
				break;
			default:
				currTile -= 6;
				moveHighlighter(grid[currTile].centerX, grid[currTile].centerY);
				break;
		}
	}
	// Down
	else {
		switch(currTile) {
			case 30:
				currTile = 0;
				moveHighlighter(grid[currTile].centerX, grid[currTile].centerY);
				break;
			case 31:
				currTile = 1;
				moveHighlighter(grid[currTile].centerX, grid[currTile].centerY);
				break;
			case 32:
				currTile = 2;
				moveHighlighter(grid[currTile].centerX, grid[currTile].centerY);
				break;
			case 33:
				currTile = 3;
				moveHighlighter(grid[currTile].centerX, grid[currTile].centerY);
				break;
			case 34:
				currTile = 4;
				moveHighlighter(grid[currTile].centerX, grid[currTile].centerY);
				break;
			case 35:
				currTile = 5;
				moveHighlighter(grid[currTile].centerX, grid[currTile].centerY);
				break;
			default:
				currTile += 6;
				moveHighlighter(grid[currTile].centerX, grid[currTile].centerY);
				break;
		}
	}
}

void moveHighlighter(int x, int y) {
	square.move_xy(x-xOffset, y+yOffset);
	selX = x;
	selY = y;
}

/*
// Need to finish this function
void checkTime() {
	int currentTime;
	currentTime = timer.read_time();
	time = currentTime - temp;
	time /= 1000000; // conversion to seconds

}

*/


int main() {
	gray.bypass(1);
	ghost.bypass(1);
	mouse.bypass(1);
	bar.bypass(1);

	frame.bypass(0);
	osd.bypass(0);
	square.bypass(0); // My core

	lfsr.generate();

	setup:
	int setup = 0;

	while(1) {
		while(setup == 0) {
			data = lfsr.read();
			free(grid);
			frame.clr_screen(0);
			//while(!getButton(btnc));
			generateGrid();
			updateMessage();

			// Four button presses to get the
			while(!getButton(btnc));
			//Generate top-left quadrant
			data = lfsr.read();
			showPattern(0);

			while(!getButton(btnc));
			//Generate top-right quadrant
			data = lfsr.read();
			showPattern(1);

			while(!getButton(btnc));
			//Generate bottom-left quadrant
			data = lfsr.read();
			showPattern(2);

			while(!getButton(btnc));
			//Generate bottom-right quadrant
			data = lfsr.read();
			showPattern(3);
			// Place the highlight sprite at the top left tile by default
			moveHighlighter(grid[0].centerX, grid[0].centerY);
			currTile = 0;
			setup = 1;
		}
		// Setup the time:


		// Start the time
		//elapsedTime();


		// Play the game:
		//gameStart:
		while(setup == 1) {
			// Double Time Limit:
			//uin32_t getTimeLimit();

			// Forced win / Regular win
			int won = checkIfWon();
			if(won){
				playWinningPattern();

				if(difficultyLevel < 7) {
					difficultyLevel++;
				}
				goto setup;
			}
			if(getSwitch(0)){
				playWinningPattern();

				if(difficultyLevel < 7) {
					difficultyLevel++;
				}

				while(!getSwitch(0));
				goto setup;
			}

			// Forced loss
			if(getSwitch(1)) {
				playLosingPattern();

				while(!getSwitch(1));
				goto setup;
			}

			if(getButton(btnr)) {
				moveHorizontal(1);
			}

			if(getButton(btnl)) {
				moveHorizontal(0);
			}

			if(getButton(btnu)) {
				moveVertical(1);
			}

			if(getButton(btnd)) {
				moveVertical(0);
			}

			if(getButton(btnc)) {
				selectTile();
			}
		}

		sleep_ms(1000);
	}


}
