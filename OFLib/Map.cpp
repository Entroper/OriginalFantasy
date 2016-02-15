#include "Map.h"
#include <fstream>
#include <string>
#include <cmath>
#include <SDL/SDL_opengl.h>
using namespace std;

Map::Map(string filename)
{
	const string MAP_ROOT = "../Quests/FF1/Maps";

	ifstream mapFile(MAP_ROOT + "/" + filename, ios::in|ios::binary);
	
	mapWidth = 64;
	mapHeight = 64;
	tileIDs = new int[mapWidth*mapHeight];
	mapFile.read((char *)tileIDs, mapWidth*mapHeight*sizeof(int));

	outsideTileset = new Tileset("Castle");
	insideTileset = new Tileset("Castle (Rooms)");
}

Map::~Map()
{
	delete[] tileIDs;
	delete insideTileset;
	delete outsideTileset;
}

void Map::Draw(float centerX, float centerY)
{
	const float screenWidth = 16, screenHeight = 14;

	int leftX = floor(centerX - screenWidth/2.0);
	int lowerY = floor(centerY - screenHeight/2.0);
	int rightX = ceil(centerX + screenWidth/2.0);
	int upperY = ceil(centerY + screenHeight/2.0);

	for (int x = leftX; x <= rightX; x++)
	{
		for (int y = lowerY; y <= upperY; y++)
		{
			int xMod = x % mapWidth;
			int yMod = y % mapHeight;
			int i = xMod < 0 ? xMod + mapWidth  : xMod;
			int j = yMod < 0 ? yMod + mapHeight : yMod;

			int tileIndex = mapWidth*(mapHeight - 1 - j) + i;
			unsigned int texture = (*insideTileset)[tileIDs[tileIndex]];

			glBindTexture(GL_TEXTURE_2D, texture);
			glBegin(GL_QUADS);
				glTexCoord2i(0, 0);
				glVertex2i(x, y);
				glTexCoord2i(0, 1);
				glVertex2i(x, y + 1);
				glTexCoord2i(1, 1);
				glVertex2i(x + 1, y + 1);
				glTexCoord2i(1, 0);
				glVertex2i(x + 1, y);
			glEnd();
		}
	}
}