#ifndef TILESET_H
#define TILESET_H

#include <map>
using namespace std;

class Tileset
{
public:
	
	Tileset(string path);

	unsigned int operator [](int tileID);

	int textureCount;

private:

	unsigned char *LoadBMPImage(string filename, int &width, int &height);

	map<int, unsigned int> textures;
};

#endif