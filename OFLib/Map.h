#ifndef MAP_H
#define MAP_H

#include "Tileset.h"

class Map
{
public:

	Map(string filename);
	~Map();

	void Draw(float centerX, float centerY);

private:

	int mapWidth, mapHeight;
	Tileset *insideTileset, *outsideTileset;
	int *tileIDs;
};

#endif