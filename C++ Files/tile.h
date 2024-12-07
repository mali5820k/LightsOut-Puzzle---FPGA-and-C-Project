#ifndef _TILE_H_INCLUDED
#define _TILE_H_INCLUDED

class Tile {
public:
	/*
	 * The position of this tile
	 */
	int startX, startY, endX, endY;
	int centerX;
	int centerY;
	int color;
	int associatedTileCount; // Number of associated tiles
	Tile* associatedTiles; // Max four


	Tile(int param1, int param2, int param3, int param4, int param5);
	~Tile();

	int isSelected(int x, int y);
};

#endif
