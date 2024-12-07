#include "tile.h"

Tile::Tile(int startX, int startY, int endX, int endY, int color) {
	this->startX = startX;
	this->startY = startY;
	this->endX = endX;
	this->endY = endY;
	this->color = color;
	this->centerX = ((endX-startX)/2)+startX;
	this->centerY = ((endY-startY)/2)+startY;
}

Tile::~Tile() {}

int Tile::isSelected(int x, int y) {
	if(x == centerX && y == centerY) {
		return 1;
	}
	else {
		return 0;
	}
}
