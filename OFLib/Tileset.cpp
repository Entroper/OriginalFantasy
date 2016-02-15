#include <SDL/SDL_opengl.h>
#include <sstream>
#include "Tileset.h"
#include "FlatFile.h"
using namespace std;

Tileset::Tileset(string path)
{
	const string TILESET_ROOT = "../Quests/FF1/Graphics/Maps";

	FlatFileReader tilesetFile(TILESET_ROOT + "/" + path + "/" + path + ".txt");
	textureCount = tilesetFile.lines.size();
	unsigned int *textureArray = new unsigned int[textureCount];
	glGenTextures(textureCount, textureArray);
	int textureIndex = 0;
	for (lineIterator line = tilesetFile.lines.begin(); line != tilesetFile.lines.end(); line++)
	{
		string tileFilename = (*line)[tilesetFile.headers["Filename"]];
		string tileIDString = (*line)[tilesetFile.headers["TileID"]];
		
		istringstream tileIDOSS(tileIDString);
		int tileID;
		tileIDOSS >> tileID;
		textures[tileID] = textureArray[textureIndex];

		int width, height;
		unsigned char *image = LoadBMPImage(TILESET_ROOT + "/" + path + "/" + tileFilename, width, height);

		glBindTexture(GL_TEXTURE_2D, textures[textureIndex]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, image);

		delete[] image;

		textureIndex++;
	}
}

unsigned int Tileset::operator [](int tileID)
{
	return textures[tileID];
}

unsigned char *Tileset::LoadBMPImage(string filename, int &width, int &height)
{
	ifstream bmpFile(filename.c_str(), ios::in|ios::binary);
	
	bmpFile.seekg(10, ios::beg);
	int pixelOffset;
	bmpFile.read((char *)&pixelOffset, sizeof(int));

	bmpFile.seekg(18, ios::beg);
	bmpFile.read((char *)&width, sizeof(int));
	bmpFile.read((char *)&height, sizeof(int));

	unsigned char *pixels = new unsigned char[width*height*3];
	bmpFile.seekg(pixelOffset, ios::beg);
	bmpFile.read((char *)pixels, width*height*3);

	return pixels;
}