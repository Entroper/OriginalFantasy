#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <exception>
#include <string>
#include <fstream>
#include "PerfTimer.h"
#include "mtxlib.h"
#include "../OFLib/Map.h"
using namespace std;

Map *myMap;
float centerX = 0, centerY = 0;
bool stop = false;
SDL_Surface *screen = NULL;

void SaveScreenshot()
{
	int width = 1920, height = 1200;
	unsigned char *pixels = new unsigned char[3*width*height];

	glReadBuffer(GL_FRONT);
	glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, pixels);

	ofstream bmpFile("screenshot.bmp", ios::out|ios::binary);
	int temp; short temps;

	temps = 19778; bmpFile.write((char *)&temps, sizeof(short)); //identifier
	temp = 14 + 40 + 3*width*height; bmpFile.write((char *)&temp, sizeof(int)); //size of file
	temp = 0; bmpFile.write((char *)&temp, sizeof(int)); //reserved
	temp = 14 + 40; bmpFile.write((char *)&temp, sizeof(int)); //offset of pixel data

	temp = 40; bmpFile.write((char *)&temp, sizeof(int)); //size of V3 BMP header
	temp = width; bmpFile.write((char *)&temp, sizeof(int)); //width and height of image
	temp = height; bmpFile.write((char *)&temp, sizeof(int));
	temps = 1; bmpFile.write((char *)&temps, sizeof(short)); //color planes (must be 1)
	temps = 24; bmpFile.write((char *)&temps, sizeof(short)); //color depth (24-bit)
	temp = 0; bmpFile.write((char *)&temp, sizeof(int)); //compression method (none)
	temp = 3*width*height; bmpFile.write((char *)&temp, sizeof(int)); //pixel data size
	temp = 0; bmpFile.write((char *)&temp, sizeof(int)); //pixels per meter (ignored)
	temp = 0; bmpFile.write((char *)&temp, sizeof(int));
	temp = 0; bmpFile.write((char *)&temp, sizeof(int)); //color palette size (no palette for 24-bit image)
	temp = 0; bmpFile.write((char *)&temp, sizeof(int)); //number of colors used (ignored)

	bmpFile.write((char *)pixels, 3*width*height);

	bmpFile.close();

	delete[] pixels;
}

void HandleEvent(const SDL_Event event)
{
	if (event.type == SDL_QUIT || event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
		stop = true;

	if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_F5)
		SaveScreenshot();

	if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_w)
		centerY += 1;
	if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_a)
		centerX -= 1;
	if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_s)
		centerY -= 1;
	if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_d)
		centerX += 1;

}

void Draw()
{
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-8.0, 8.0, -7.0, 7.0, 1.0, -1.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(-centerX, -centerY, 0.0);

	myMap->Draw(centerX, centerY);

	SDL_GL_SwapBuffers();
}

void GameLoop()
{
	SDL_Event event;

	PerfTimer timer;
	timer.Reset();
	timer.Start();

	while (!stop)
	{
		while (SDL_PollEvent(&event))
		{
			HandleEvent(event);
		}

		timer.Stop();
		float time = timer.GetDurationSeconds();
		timer.Reset();
		timer.Start();

		Draw();
	}
}

void InitGL()
{
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	int width = 1920, height = 1200;
	screen = SDL_SetVideoMode(width, height, 32, SDL_OPENGL|SDL_FULLSCREEN);
	if (screen == NULL)
	{
		printf("Unable to set video mode: %s\n", SDL_GetError());
		exit(1);
	}

	glViewport(275, 0, 1370, 1200);

	glEnable(GL_TEXTURE_2D);
}

void Initialize()
{
	if (SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO) < 0)
	{
		printf("Unable to init SDL: %s\n", SDL_GetError());
		exit(1);
	}
	atexit(SDL_Quit);

	InitGL();
}

void LoadMap()
{
	myMap = new Map("Elfland Castle.map");
}

int main(int argc, char **argv)
{
	Initialize();
	LoadMap();
	GameLoop();

	return 0;
}
