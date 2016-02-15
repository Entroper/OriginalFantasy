/*****************************************************************************
* Original Fantasy
* Author: Michael Bennett
* Copyright 2007
*****************************************************************************/

#include "ROM.h"
#include <sstream>
#include <iomanip>
#include <direct.h>
#include <iostream>

using namespace std;

//Constructors
ROM::ROM(string filename)
{
	in.open(filename.c_str(), ios::in|ios::binary);

	LoadTextTables("StandardTable.tbl", "DTETable.tbl");
	LoadNESPalette("FFHackster.pal");

	LoadBattleGraphics();
	LoadMapGraphics();

	monsters = LoadMonsters();
	battles = LoadBattles();
	weapons = LoadWeapons();
	armor = LoadArmor();
	spells = LoadSpells();
}

ROM::~ROM()
{
	in.close();
}



void ROM::ExportFull()
{
	_mkdir(QUEST_ROOT.c_str());
	_mkdir((QUEST_ROOT + "/Graphics").c_str());

	DumpMonsterGraphics(QUEST_ROOT + "/Graphics/Monsters");
	DumpMapGraphics(QUEST_ROOT + "/Graphics/Maps");

	DumpMonsterData(QUEST_ROOT + "/Monsters.txt");
	DumpWeaponData(QUEST_ROOT + "/Weapons.txt");
	DumpArmorData(QUEST_ROOT + "/Armor.txt");
	//DumpSpellData(QUEST_ROOT + "/Spells.txt");

	DumpMapData(QUEST_ROOT + "/Maps");
}



//Load a 64-color NES palette from a file.
void ROM::LoadNESPalette(string filename)
{
	ifstream palFile(filename.c_str(), ios::in|ios::binary);
	palFile.read((char *)NESpalette, NES_PALETTE_ENTRIES*3);
	palFile.close();
	
	//Reverse BGR to RGB format.
	for (int i = 0; i < NES_PALETTE_ENTRIES; i++)
	{
		unsigned char temp = NESpalette[i][0];
		NESpalette[i][0] = NESpalette[i][2];
		NESpalette[i][2] = temp;
	}
}



/****************************************************
* BATTLES
*****************************************************
Battles are complicated, especially the graphics.  Some terms:
The NES PALETTE consists of 64 colors.  I have used a list from the FF Hackster
tool to define standard 24-bit RGB representations for these colors.
A PALETTE used by the battle engine consists of 4 indices into the NES PALETTE.  4 bytes.
Each battle has two PALETTEs that can be used to draw enemies, one PALETTE for
the border graphics, and one for the background.
A TILE is an 8x8 image in 4 colors.  64 pixels, 2 bits each = 16 bytes.
A TILESET is a set of 128 tiles.  There are 16 battle TILESETs in the ROM, all
stored sequentially.  128 tiles = 2048 bytes.  All tilesets = 32 kB.
A PATTERN is a list of indices into a TILESET.  Read each of the TILEs specified,
and use the colors specified by the PALETTE to form the final enemy sprite.
There are only 4 main PATTERNs used by most enemies in the game.  The four fiends
and Chaos each have their own PATTERNs.  In fact, the bosses use four PALETTEs
each, so they need a PALETTE OVERLAY to define which PALETTE to use.  A PALETTE
OVERLAY is 2 bits for each 2x2 block of TILEs in the final image.  For the fiends,
this is 4x4, 16 entries, 4 bytes.  Chaos's OVERLAY is 7x6, 42 entries, 11 bytes.


A battle entry in the NES ROM consists of:
byte 0: High order 4 bits (0xF0) = number of enemies.
		1 = 9 small, 2 = 4 large, 3 = 6 small + 2 large, 4 = fiend, 5 = chaos
		Low order 4 bits (0x0F) = which tileset to use for the enemy graphics
byte 1: 4 entries of 2 bits each defining which of the 4 enemy graphics from
		the selected tileset belongs to each enemy.  This entire byte has its
		bits stored in reverse order (not just the entry order, the whole thing)
		for no reason I can fathom.
byte 2-5: 4 indices into the monster table defning which 4 enemies you will face
byte 6-9: The number of each enemy you may face.  High order 4 bits are the
		  minimum, low order 4 bits are the maximum for each enemy.
byte 10-11: Two indices into the palette table.  These two palettes are used
			for the enemy graphics in this battle.
byte 12: Chance of being surprised.  A 10 here means a 10% chance.
byte 13: High order 4 bits: one bit each to determine which palette is used by
		 each enemy.  Low order bit is set if you cannot run from the battle.
byte 14-15: Same as bytes 6 to 9, but for the "alternate" enemy formation.
*/

vector<BattleDef> ROM::LoadBattles()
{
	vector<BattleDef> battleList;
	unsigned char data[BATTLE_ENTRIES][BATTLE_SIZE];

	//Read in the battle data.
	in.seekg(BATTLE_OFFSET, ios::beg);
	in.read((char *)data, BATTLE_ENTRIES*BATTLE_SIZE);

	//Initialize the battles.
	for (int i = 0; i < BATTLE_ENTRIES; i++)
	{
		BattleDef battle;
		battle.formation = (BattleFormation)((data[i][0] & 0xF0) >> 4);
		battle.tileset = data[i][0] & 0x0F;

		unsigned char temp = data[i][1];
		battle.monsterPics[0] = ((temp & 0x01) << 1) + ((temp & 0x02) >> 1);
		battle.monsterPics[1] = ((temp & 0x04) >> 1) + ((temp & 0x08) >> 3);
		battle.monsterPics[2] = ((temp & 0x10) >> 3) + ((temp & 0x20) >> 5);
		battle.monsterPics[3] = ((temp & 0x40) >> 5) + ((temp & 0x80) >> 7); //confused yet?

		battle.monsters[0] = data[i][2];
		battle.monsters[1] = data[i][3];
		battle.monsters[2] = data[i][4];
		battle.monsters[3] = data[i][5];

		battle.qtyMax[0] = data[i][6] & 0x0F;
		battle.qtyMax[1] = data[i][7] & 0x0F;
		battle.qtyMax[2] = data[i][8] & 0x0F;
		battle.qtyMax[3] = data[i][9] & 0x0F;
		battle.qtyMax[4] = data[i][14] & 0x0F;
		battle.qtyMax[5] = data[i][15] & 0x0F;

		battle.qtyMin[0] = (data[i][6] & 0xF0) >> 4;
		battle.qtyMin[1] = (data[i][7] & 0xF0) >> 4;
		battle.qtyMin[2] = (data[i][8] & 0xF0) >> 4;
		battle.qtyMin[3] = (data[i][9] & 0xF0) >> 4;
		battle.qtyMin[4] = (data[i][14] & 0xF0) >> 4;
		battle.qtyMin[5] = (data[i][15] & 0xF0) >> 4;

		battle.palettes[0] = data[i][10];
		battle.palettes[1] = data[i][11];
		battle.surpriseChance = data[i][12];
		battle.monsterPalettes[0] = (data[i][13] & 0x80) >> 7;
		battle.monsterPalettes[1] = (data[i][13] & 0x40) >> 6;
		battle.monsterPalettes[2] = (data[i][13] & 0x20) >> 5;
		battle.monsterPalettes[3] = (data[i][13] & 0x10) >> 4;
		battle.cannotRun = data[i][13] & 1;

		//Add the initialized battle to the list.
		battleList.push_back(battle);
	}

	return battleList;
}

void ROM::LoadBattleGraphics()
{
	in.seekg(BATTLE_PALETTE_OFFSET, ios::beg);
	in.read((char *)battlePalettes, BATTLE_PALETTE_ENTRIES*BATTLE_PALETTE_SIZE);
	in.seekg(BATTLE_TILESET_OFFSET, ios::beg);
	in.read((char *)battleTilesets, BATTLE_TILESET_ENTRIES*BATTLE_TILESET_TILE_ENTRIES*BATTLE_TILE_SIZE);
}

//Build an RGB sprite from all the lookup info for a monster.
//Be sure to load the battle graphics first!
void ROM::BuildRGBMonsterSprite(unsigned char *&sprite, int tileset, int palnum, int picnum)
{
	//Determine if we're loading a small (4x4) or large (6x6) sprite.
	int size;
	if (picnum < 2)
		size = 4;
	else
		size = 6;
	
	//Determine the index of the first tile in the image.
	const unsigned char picindex[4] = { 0x12, 0x22, 0x32, 0x56 };
	int tilebase = picindex[picnum];

	//Load the 4-color palette and get the corresponding 24-bit values from the NES palette.
	unsigned char palette[BATTLE_PALETTE_SIZE][3];
	unsigned char NESpalIndex;
	for (int i = 0; i < BATTLE_PALETTE_SIZE; i++)
	{
		NESpalIndex = battlePalettes[palnum][i];
		for (int j = 0; j < 3; j++)
			palette[i][j] = NESpalette[NESpalIndex][j];
	}
	
	//Allocate the sprite.
	sprite = new unsigned char[size*8*size*8*3];

	//Fill the sprite.  This is ugly.
	//x and y are the tiles, i and j are pixels within each tile, l is the offset into the tile data
	int tilenum = tilebase;
	for (int y = 0; y < size; y++)
		for (int x = 0; x < size; x++)
		{
			//Fetch a tile, then fill in the block in the image.
			unsigned char *tile = battleTilesets[tileset][tilenum++];
			int l = 0;
			for (int i = 0; i < 8; i++)
			{
				//The tile is stored using bitplanes.  Tile[l] is the LSB for a row of 8 pixels.
				//Tile [l+8] is the MSB of the same row.
				unsigned char temp0 = tile[l];
				unsigned char temp1 = tile[l+8];
				l++;
				for (int j = 7; j >= 0; j--) //the MSB is the leftmost pixel
				{
					int pixel = (temp0 & 1) + ((temp1 & 1)<<1);
					temp0 >>= 1; temp1 >>= 1;
					for (int k = 0; k < 3; k++)
						sprite[3*(size*8*(8*y + i) + 8*x + j) + k] = palette[pixel][k];
				}
			}
		}
	//If you thought that was bad, wait until you see the boss graphics!
}

void ROM::BuildRGBBossSprite(unsigned char *&sprite, int tileset, int palnum1, int palnum2, MonsterPic monpic)
{
	//Determine the size of the boss (regular fiend or chaos).
	int sizeX, sizeY;
	if (monpic == MONPIC_CHAOS)
	{
		sizeX = 14;
		sizeY = 12;
	}
	else
	{
		sizeX = 8;
		sizeY = 8;
	}

	//Determine which battle definition the boss is in, and which pattern and overlay to use.
	//After much aggravation, I wussed out and hard coded these.
	const unsigned char ChaosOverlay[11] = {
		0xA9, 0xAA, 0xAA, 0xAA,	0xAA, 0x95, 0x95, 0x55,	0x55, 0x55, 0x50 };
	const unsigned char LichOverlay[4] = { 0x19, 0x15, 0x15, 0x15 };
	const unsigned char KaryOverlay[4] = { 0x65, 0x69, 0x55, 0x55 };
	const unsigned char KrakenOverlay[4] = { 0xAA, 0xAA, 0x56, 0x55 };
	const unsigned char TiamatOverlay[4] = { 0X6A, 0x5A, 0x56, 0x56 };
	const unsigned char * overlay;

	int picnum;
	switch (monpic)
	{
		case MONPIC_CHAOS:
			picnum = 0;
			overlay = ChaosOverlay;
			break;
		case MONPIC_LICH:
			picnum = 1;
			overlay = LichOverlay;
			break;
		case MONPIC_KARY:
			picnum = 0;
			overlay = KaryOverlay;
			break;
		case MONPIC_KRAKEN:
			picnum = 2;
			overlay = KrakenOverlay;
			break;
		case MONPIC_TIAMAT:
			picnum = 3;
			overlay = TiamatOverlay;
			break;
	}

	//Read in the boss' pattern.
	unsigned char *pattern = new unsigned char[sizeX*sizeY];
	if (monpic == MONPIC_CHAOS)
	{
		in.seekg(CHAOS_PATTERN, ios::beg);
		in.read((char *)pattern, sizeX*sizeY);
	}
	else
	{
		in.seekg(FIEND_PATTERN_TABLE + picnum*FIEND_PATTERN_SHIFT);
		in.read((char *)pattern, sizeX*sizeY);
	}

	//Load the 4-color palettes and get the corresponding 24-bit values from the NES palette.
	unsigned char palette[4][BATTLE_PALETTE_SIZE][3];
	unsigned char NESpalIndex;
	for (int i = 0; i < BATTLE_PALETTE_SIZE; i++)
	{
		NESpalIndex = battlePalettes[palnum1][i];
		for (int j = 0; j < 3; j++)
			palette[1][i][j] = NESpalette[NESpalIndex][j];

		NESpalIndex = battlePalettes[palnum2][i];
		for (int j = 0; j < 3; j++)
			palette[2][i][j] = NESpalette[NESpalIndex][j];
	}
	//Palettes 1 and 2 are loaded from the ROM, palettes 0 and 3 are blank (I know, it's dumb).
	for (int k = 0; k < 4; k+=3)
		for (int i = 0; i < BATTLE_PALETTE_SIZE; i++)
			for (int j = 0; j < 3; j++)
				palette[k][i][j] = 0;
	
	//Allocate the sprite.
	sprite = new unsigned char[sizeX*8*sizeY*8*3];

	//Fill the sprite.  This is REALLY ugly.
	//x and y are the tiles, i and j are pixels within each tile, l is the offset into the tile data
	int patternIndex = 0;
	for (int y = 0; y < sizeY; y++)
	{
		for (int x = 0; x < sizeX; x++)
		{
			//Determine which palette this tile is using.
			int palx = x/2, paly = y/2;
			int overlayIndex = paly*sizeX/2 + palx;
			int palnum = overlay[overlayIndex/4];
			palnum >>= 2*(3 - (overlayIndex%4));
			palnum &= 3;
			//Fetch the next tile from the pattern.
			int tilenum = pattern[patternIndex++];
			unsigned char *tile = battleTilesets[tileset][tilenum++];
			int l = 0;
			for (int i = 0; i < 8; i++)
			{
				//The tile is stored using bitplanes.  Tile[l] is the LSB for a row of 8 pixels.
				//Tile [l+8] is the MSB of the same row.
				unsigned char temp0 = tile[l];
				unsigned char temp1 = tile[l+8];
				l++;
				for (int j = 7; j >= 0; j--) //the MSB is the leftmost pixel
				{
					int pixel = (temp0 & 1) + ((temp1 & 1)<<1);
					temp0 >>= 1; temp1 >>= 1;
					for (int k = 0; k < 3; k++)
						sprite[3*(sizeX*8*(8*y + i) + 8*x + j) + k] = palette[palnum][pixel][k];
				}
			}
		}
	}
}

void ROM::WriteBMPImage(unsigned char *sprite, int width, int height, string filename)
{
	ofstream bmpFile(filename.c_str(), ios::out|ios::binary);
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

	//Write the rows in reverse order (from bottom to top).
	for (int row = height - 1; row >= 0; row--)
		bmpFile.write((char *)&(sprite[3*width*row]), 3*width);

	bmpFile.close();
}

void ROM::DumpMonsterGraphics(string path)
{
	_mkdir(path.c_str());

	//Get all the non-boss monster graphics.
	unsigned char *sprite;
	int small = 0; int large = 0;
	for (int i = 0; i < MONSTER_ENTRIES - 9; i++)
	{
		//Find an entry for the monster in one of the battles.
		int j, k;
		for (j = 0; j < BATTLE_ENTRIES; j++)
		{
			for (k = 0; k < 4; k++)
				if (battles[j].monsters[k] == i)
					break;
			if (k != 4)
				break;
		}

		if (j == BATTLE_ENTRIES)
			continue;

		//Get the graphic for the monster.
		int palnum = battles[j].monsterPalettes[k];
		int picnum = battles[j].monsterPics[k];
		int size = (picnum == 0 || picnum == 1) ? 32 : 48;

		BuildRGBMonsterSprite(sprite, battles[j].tileset, battles[j].palettes[palnum], battles[j].monsterPics[k]);
		WriteBMPImage(sprite, size, size, path + "/" + monsters[i].name + ".bmp");
		delete[] sprite;
	}

	//Get the boss graphics.  Yeah, I hardcoded the tileset and palette entries for these, too.
	BuildRGBBossSprite(sprite, 13, 54, 55, MONPIC_LICH);
	WriteBMPImage(sprite, 64, 64, path + "/" + "LICH.bmp");
	delete[] sprite;

	BuildRGBBossSprite(sprite, 13, 56, 57, MONPIC_KARY);
	WriteBMPImage(sprite, 64, 64, path + "/" + "KARY.bmp");
	delete[] sprite;

	BuildRGBBossSprite(sprite, 14, 58, 59, MONPIC_KRAKEN);
	WriteBMPImage(sprite, 64, 64, path + "/" + "KRAKEN.bmp");
	delete[] sprite;

	BuildRGBBossSprite(sprite, 14, 60, 61, MONPIC_TIAMAT);
	WriteBMPImage(sprite, 64, 64, path + "/" + "TIAMAT.bmp");
	delete[] sprite;

	BuildRGBBossSprite(sprite, 15, 62, 63, MONPIC_CHAOS);
	WriteBMPImage(sprite, 112, 96, path + "/" + "CHAOS.bmp");
	delete[] sprite;
}



/****************************************************
* MAPS
*****************************************************
Maps are, happily, somewhat simpler than the monster graphics.  The same
terminology applies, except that map tiles are 16x16, whereas the raw tiles in
the ROM data are still 8x8.  A MAP TILESET therefore requires a set of
PATTERNs into its ROM TILESET.  A PATTERN in this case is 4 indices into the
ROM TILESET, which define the 4 ROM TILEs that compose 1 MAP TILE.  Each MAP
TILESET contains 128 MAP TILEs formed from the 128 ROM TILEs available.
*/

void ROM::LoadMapGraphics()
{
	in.seekg(MAP_PALETTE_OFFSET, ios::beg);
	in.read((char *)mapPalettes, MAP_PALETTE_ENTRIES*MAP_PALETTE_SIZE);
	in.seekg(MAP_TILESET_OFFSET, ios::beg);
	in.read((char *)mapTilesets, MAP_TILESET_ENTRIES*MAP_TILESET_TILE_ENTRIES*MAP_TILE_SIZE);
	in.seekg(MAP_TILESET_PALETTE_ASSIGNMENT_OFFSET, ios::beg);
	in.read((char *)mapTilesetPaletteAssignments, MAP_TILESET_ENTRIES*MAP_TILESET_PATTERN_ENTRIES);
	
	//The tileset patterns are stored kinda funky.
	//I twiddle them here to make it easier when we actually dump the map graphics.
	in.seekg(MAP_TILESET_PATTERN_OFFSET, ios::beg);
	char temp[MAP_TILESET_ENTRIES*MAP_TILESET_PATTERN_ENTRIES*MAP_TILESET_PATTERN_SIZE];
	in.read(temp, MAP_TILESET_ENTRIES*MAP_TILESET_PATTERN_ENTRIES*MAP_TILESET_PATTERN_SIZE);
	for (int i = 0; i < MAP_TILESET_ENTRIES; i++)
	{
		for (int j = 0; j < MAP_TILESET_PATTERN_ENTRIES; j++)
		{
			for (int k = 0; k < MAP_TILESET_PATTERN_SIZE; k++)
			{
				//In the ROM, all the upper-left corners are listed first, then all the upper-right corners, etc.
				mapTilesetPatterns[i][j][k] = temp[i*MAP_TILESET_PATTERN_ENTRIES*MAP_TILESET_PATTERN_SIZE +
												   j +
												   k*MAP_TILESET_PATTERN_ENTRIES];
			}
		}
	}
}

void ROM::BuildRGBMapTileSprite(unsigned char *&sprite, int tileset, int palnum, int tilenum)
{
	//Load the 4-color palettes and get the corresponding 24-bit values from the NES palette.
	unsigned char palettes[4][4][3];
	unsigned char NESpalIndex;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			NESpalIndex = mapPalettes[palnum][4*i + j];
			for (int k = 0; k < 3; k++)
				palettes[i][j][k] = NESpalette[NESpalIndex][k];
		}
	}

	//Load the palette assignments for each of the 4 tiles.
	int paletteAssignments[2][2];
	unsigned char paletteAssignmentData = mapTilesetPaletteAssignments[tileset][tilenum];
	for (int i = 0; i < 4; i++)
	{
		paletteAssignments[i/2][i%2] = paletteAssignmentData & 3;
		paletteAssignmentData >>= 2;
	}

	//Get the 4 8x8 tiles used.
	unsigned char tiles[2][2][MAP_TILE_SIZE];
	for (int i = 0; i < MAP_TILESET_PATTERN_SIZE; i++)
	{
		int tileIndex = mapTilesetPatterns[tileset][tilenum][i];
		for (int j = 0; j < MAP_TILE_SIZE; j++)
		{
			tiles[i/2][i%2][j] = mapTilesets[tileset][tileIndex][j];
		}
	}

	//Allocate the sprite.
	sprite = new unsigned char[16*16*3];

	//Fill the sprite.  This is straightforward, but I'm afraid the array indexing will get unwieldy.
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			int paletteAssignment = paletteAssignments[i][j];
			for (int y = 0; y < 8; y++)
			{
				unsigned char temp0 = tiles[i][j][y];
				unsigned char temp1 = tiles[i][j][y+8];
				for (int x = 7; x >= 0; x--)
				{
					int pixel = (temp0 & 1) + ((temp1 & 1)<<1);
					temp0 >>= 1; temp1 >>= 1;
					for (int k = 0; k < 3; k++)
						sprite[3*(2*8*(8*i + y) + 8*j + x) + k] = palettes[paletteAssignment][pixel][k];
				}
			}
		}
	}
}

bool operator ==(const UniqueTileset &lhs, const UniqueTileset &rhs)
{
	for (int i = 0; i < MAP_PALETTE_SIZE; i++)
		if (lhs.palette[i] != rhs.palette[i])
			return false;

	return lhs.tileset == rhs.tileset;
}

vector<UniqueTileset> ROM::FindMapTilesetMappings()
{
	vector<UniqueTileset> tilesetMappings;
	
	//Read in the assignments.
	unsigned char tilesetAssignments[MAP_ENTRIES];
	in.seekg(MAP_TILESET_ASSIGNMENT_OFFSET);
	in.read((char *)tilesetAssignments, MAP_ENTRIES);

	//Find unique tileset/palette combinations.
	for (int i = 0; i < MAP_ENTRIES; i++)
	{
		UniqueTileset curr;
		curr.tileset = tilesetAssignments[i];
		for (int j = 0; j < MAP_PALETTE_SIZE; j++)
			curr.palette[j] = mapPalettes[3*i][j];
		curr.paletteIndex = 3*i;

		//See if it's already in the list.
		for (int j = 0; j < tilesetMappings.size(); j++)
		{
			if (curr == tilesetMappings[j])
			{
				curr.paletteIndex = tilesetMappings[j].paletteIndex;
				break;
			}
		}

		//Put it in the list.
		tilesetMappings.push_back(curr);

		//Do it again for the inside-room palette.
		for (int j = 0; j < MAP_PALETTE_SIZE; j++)
			curr.palette[j] = mapPalettes[3*i + 2][j];
		curr.paletteIndex = 3*i + 2;

		//See if it's already in the list.
		for (int j = 0; j < tilesetMappings.size(); j++)
		{
			if (curr == tilesetMappings[j])
			{
				curr.paletteIndex = tilesetMappings[j].paletteIndex;
				break;
			}
		}

		//Put it in the list.
		tilesetMappings.push_back(curr);
	}

	return tilesetMappings;
}

vector<UniqueTileset> ROM::FindUniqueMapTilesets(vector<UniqueTileset> tilesetMappings)
{
	vector<UniqueTileset> uniqueTilesets;
	for (int i = 0; i < tilesetMappings.size(); i++)
	{
		UniqueTileset curr = tilesetMappings[i];
		bool found = false;
		for (int j = 0; j < i; j++)
		{
			if (curr.tileset == tilesetMappings[j].tileset && curr.paletteIndex == tilesetMappings[j].paletteIndex)
			{
				found = true;
				break;
			}
		}
		if (!found)
			uniqueTilesets.push_back(curr);
	}

	return uniqueTilesets;
}

void ROM::DumpMapGraphics(string path)
{
	vector<UniqueTileset> mappings = FindMapTilesetMappings();
	vector<UniqueTileset> uniques = FindUniqueMapTilesets(mappings);

	_mkdir(path.c_str());

	for (int tilesetIndex = 0; tilesetIndex < uniques.size(); tilesetIndex++)
	{
		unsigned char *sprite;
		unsigned char bigsprite[(16*16)*(16*8)*3];

		string fullpath = path + "/" + TILESET_NAMES[tilesetIndex];
		_mkdir(fullpath.c_str());
		string tilesetFilename = fullpath + "/" + TILESET_NAMES[tilesetIndex] + ".txt";
		ofstream tilesetFile(tilesetFilename);
		tilesetFile << "TileID\tFilename" << endl;

		cout << TILESET_NAMES[tilesetIndex] << endl;
		for (int i = 0; i < MAP_TILESET_PATTERN_ENTRIES; i++)
		{
			ostringstream oss;
			oss << "tile" << setw(3) << setfill('0') << i << ".bmp";
			tilesetFile << i << "\t" << oss.str() << endl;
			string tileFilename = fullpath + "/" + oss.str();
			BuildRGBMapTileSprite(sprite, uniques[tilesetIndex].tileset, uniques[tilesetIndex].paletteIndex, i);
			WriteBMPImage(sprite, 16, 16, tileFilename);

			for (int y = 0; y < 16; y++)
				for (int x = 0; x < 16; x++)
					for (int c = 0; c < 3; c++)
						bigsprite[3*((16*16)*(16*(i/16) + y) + 16*(i%16) + x) + c] = sprite[3*(16*y + x) + c];
		}
		tilesetFile.close();

		string filename = path + "/" + TILESET_NAMES[tilesetIndex] + ".bmp";
		WriteBMPImage(bigsprite, 16*16, 16*8, filename);
	}
}

void ROM::DumpMapData(string path)
{
	_mkdir(path.c_str());

	//Get all the pointers to the map locations.
	unsigned short mapPointers[MAP_ENTRIES];
	in.seekg(MAP_OFFSET, ios::beg);
	in.read((char *)mapPointers, MAP_ENTRIES*sizeof(unsigned short));

	for (int mapIndex = 0; mapIndex < MAP_ENTRIES; mapIndex++)
	{
		//Open a file for the map.
		string filename = path + "/" + MAP_NAMES[mapIndex] + ".map";
		ofstream mapFile(filename.c_str(), ios::out|ios::binary);

		//Seek to the position of the map in the ROM.
		in.seekg(MAP_OFFSET + (int)mapPointers[mapIndex], ios::beg);

		//Begin decoding the RLE.
		unsigned char curr = 0;
		in.get((char&)curr);
		while (curr != 0xFF)
		{
			int runLength = 1;
			if (curr & 0x80) //The MSB determines if the next byte is a run length.
			{
				curr ^= 0x80; //Remove the MSB.
				unsigned char temp;
				in.get((char&)temp);
				runLength = temp;
				if (runLength == 0) //0 run length actually means 256.
					runLength = 256;
			}

			//Emit the run.
			int tileID = (int)curr;
			for (int i = 0; i < runLength; i++)
				mapFile.write((char *)&tileID, sizeof(int));

			in.get((char&)curr);
		}

		mapFile.close();
	}
}



/****************************************************
* LOADING DATA FROM NES ROM
*****************************************************
These functions simply read in basic data on things like monsters, spells,
items, etc., which are stored in simple tables in the ROM.  No complicated
mucking about with palettes and tile patterns here.
*/

void ROM::LoadTextTables(string stdFilename, string DTEFilename)
{
	ifstream tables;

	tables.open(stdFilename.c_str());
	while (!tables.eof())
	{
		char val[2];
		int index;
		val[1] = tables.get();
		val[0] = tables.get();
		if (val[0] >= '0' && val[0] <= '9')
			index = val[0] - '0';
		else
			index = val[0] - 'A' + 10;
		if (val[1] >= '0' && val[1] <= '9')
			index += (val[1] - '0')*16;
		else
			index += (val[1] - 'A' + 10)*16;
		tables.get(); //remove the '=' character
		if (!tables.eof())
			getline(tables, textTable[index]);
	}
	tables.close();

	tables.open(DTEFilename.c_str());
	while (!tables.eof())
	{
		char val[2];
		int index;
		val[1] = tables.get();
		val[0] = tables.get();
		if (val[0] >= '0' && val[0] <= '9')
			index = val[0] - '0';
		else
			index = val[0] - 'A' + 10;
		if (val[1] >= '0' && val[1] <= '9')
			index += (val[1] - '0')*16;
		else
			index += (val[1] - 'A' + 10)*16;
		tables.get(); //remove the '=' character
		if (!tables.eof())
			getline(tables, DTETable[index]);
	}
	tables.close();
}

//Read the monster data from the ROM.
vector<Monster> ROM::LoadMonsters()
{
	vector<Monster> monsterList;
	unsigned char data[MONSTER_ENTRIES][MONSTER_SIZE];
	
	//Read in the monster data.
	in.seekg(MONSTER_OFFSET, ios::beg);
	in.read((char *)data, MONSTER_ENTRIES*MONSTER_SIZE);

	//Read the monster text pointers table.
	unsigned short monTextPointers[MONSTER_ENTRIES];
	in.seekg(MONSTER_TEXT_PTR_TABLE_OFFSET, ios::beg);
	in.read((char *)monTextPointers, MONSTER_ENTRIES*sizeof(short));

	//Initialize the monsters.
	for (int i = 0; i < MONSTER_ENTRIES; i++)
	{
		Monster monster;
		monster.exp = data[i][0] + 256*data[i][1];
		monster.gold = data[i][2] + 256*data[i][3];
		monster.hpMax = data[i][4] + 256*data[i][5];
		monster.morale = data[i][6];
		monster.aiScript = data[i][7];
		monster.agility = data[i][8];
		monster.defense = data[i][9];
		monster.hits = data[i][10];
		monster.accuracy = data[i][11];
		monster.power = data[i][12];
		monster.critical = data[i][13];
		monster.hitElem = data[i][14];
		monster.hitStat = data[i][15];
		monster.category = data[i][16];
		monster.initiative = data[i][17];
		monster.magdef = 0;

		//Weaknesses and resistances are more complicated.
		monster.elemWeak = data[i][18];
		monster.elemRes = data[i][19];
		for (int j = 0; j < NUM_ELEMS; j++)
		{
			if (monster.elemRes & (1<<j))
				monster.elemMod[j] = 50;
			else if (monster.elemWeak & (1<<j))
				monster.elemMod[j] = -50;
			else
				monster.elemMod[j] = 0;
		}

		//Finally, lookup the name in the ROM.
		monster.name = "";
		int offset = MONSTER_TEXT_BASE + monTextPointers[i];
		in.seekg(offset, ios::beg);
		while (in.peek() != 0)
		{
			int temp = in.get();
			monster.name += textTable[temp];
		}

		//Add the initialized monster to the list.
		monsterList.push_back(monster);
	}

	return monsterList;
}

//Read the weapons data from the ROM.
vector<Weapon> ROM::LoadWeapons()
{
	vector<Weapon> weaponList;
	unsigned char data[WEAPON_ENTRIES][WEAPON_SIZE];
	unsigned short prices[WEAPON_ENTRIES];
	unsigned short perms[WEAPON_ENTRIES];

	//Read in all the data.
	in.seekg(WEAPON_OFFSET, ios::beg);
	in.read((char *)data, WEAPON_ENTRIES*WEAPON_SIZE);
	in.seekg(WEAPON_PRICE_OFFSET, ios::beg);
	in.read((char *)prices, 2*WEAPON_ENTRIES);
	in.seekg(WEAPON_PERMS_OFFSET, ios::beg);
	in.read((char *)perms, 2*WEAPON_ENTRIES);

	//Read the weapon text pointers table.
	unsigned short weaponTextPointers[WEAPON_ENTRIES];
	in.seekg(WEAPON_TEXT_PTR_TABLE_OFFSET, ios::beg);
	in.read((char *)weaponTextPointers, WEAPON_ENTRIES*sizeof(short));

	//Initialize the weapons.
	for (int i = 0; i < WEAPON_ENTRIES; i++)
	{
		Weapon weapon;
		weapon.accuracy = data[i][0];
		weapon.power = data[i][1];
		weapon.critical = data[i][2];
		weapon.spell = data[i][3];
		weapon.elemEffects = data[i][4];
		weapon.catEffects = data[i][5];
		weapon.statEffects = 0; //weapons just don't inflict statuses in FF1
		weapon.wearloc = (1<<WEAR_HAND);
		weapon.price = prices[i];
		weapon.equipMask = (~perms[i]) & 0x0FFF; //we only want the low-order 12 bits, negated

		//Finally, lookup the name in the ROM.
		weapon.name = "";
		int offset = WEAPON_TEXT_BASE + weaponTextPointers[i];
		in.seekg(offset, ios::beg);
		while (in.peek() != 0)
		{
			int temp = in.get();
			weapon.name += textTable[temp];
		}

		//Add the initialized weapon to the list.
		weaponList.push_back(weapon);
	}

	return weaponList;
}

//Read the armor data from the ROM.
vector<Armor> ROM::LoadArmor()
{
	vector<Armor> armorList;
	unsigned char data[ARMOR_ENTRIES][ARMOR_SIZE];
	unsigned short prices[ARMOR_ENTRIES];
	unsigned short perms[ARMOR_ENTRIES];

	//Read in all the data.
	in.seekg(ARMOR_OFFSET, ios::beg);
	in.read((char *)data, ARMOR_ENTRIES*ARMOR_SIZE);
	in.seekg(ARMOR_PRICE_OFFSET, ios::beg);
	in.read((char *)prices, 2*ARMOR_ENTRIES);
	in.seekg(ARMOR_PERMS_OFFSET, ios::beg);
	in.read((char *)perms, 2*ARMOR_ENTRIES);

	//Read the armor text pointers table.
	unsigned short armorTextPointers[ARMOR_ENTRIES];
	in.seekg(ARMOR_TEXT_PTR_TABLE_OFFSET, ios::beg);
	in.read((char *)armorTextPointers, ARMOR_ENTRIES*sizeof(short));

	//Initialize the armor.
	for (int i = 0; i < ARMOR_ENTRIES; i++)
	{
		Armor armor;
		armor.weight = data[i][0];
		armor.defense = data[i][1];
		armor.elemRes = data[i][2];
		for (int j = 0; j < NUM_ELEMS; j++)
		{
			if (armor.elemRes & (1<<j))
				armor.elemMod[j] = 50;
			else
				armor.elemMod[j] = 0;
		}
		armor.spell = data[i][3];
		armor.price = prices[i];
		armor.equipMask = (~perms[i]) & 0x0FFF;

		//Wear location is determined by order, strangely.
		if (i < 16)
			armor.wearloc = (1<<WEAR_BODY);
		else if (i < 25)
			armor.wearloc = (1<<WEAR_OFFHAND);
		else if (i < 32)
			armor.wearloc = (1<<WEAR_HEAD);
		else
			armor.wearloc = (1<<WEAR_GLOVE);

		//Finally, lookup the name in the ROM.
		armor.name = "";
		int offset = ARMOR_TEXT_BASE + armorTextPointers[i];
		in.seekg(offset, ios::beg);
		while (in.peek() != 0)
		{
			int temp = in.get();
			armor.name += textTable[temp];
		}

		//Add the initialized armor to the list.
		armorList.push_back(armor);
	}
	
	return armorList;
}

//Read the spell table from the ROM.
vector<Spell> ROM::LoadSpells()
{
	vector<Spell> spellList;
	unsigned char spells[SPELL_ENTRIES][SPELL_SIZE];
	unsigned short prices[SPELL_ENTRIES];
	unsigned char perms[SPELL_PERMS_ENTRIES][SPELL_PERMS_SIZE];
	unsigned char abils[ABIL_ENTRIES][ABIL_SIZE];

	//Read in all the data.
	in.seekg(SPELL_OFFSET, ios::beg);
	in.read((char *)spells, SPELL_ENTRIES*SPELL_SIZE);
	in.seekg(SPELL_PRICE_OFFSET, ios::beg);
	in.read((char *)prices, 2*SPELL_ENTRIES);
	in.seekg(SPELL_PERMS_OFFSET, ios::beg);
	in.read((char *)perms, SPELL_PERMS_ENTRIES*SPELL_PERMS_SIZE);
	in.seekg(ABIL_OFFSET, ios::beg);
	in.read((char *)abils, ABIL_ENTRIES*ABIL_SIZE);

	//Initialize the spells.
	for (int i = 0; i < SPELL_ENTRIES; i++)
	{
		Spell spell;
		spell.param = spells[i][1];
		spell.element = spells[i][2];
		spell.target = (1<<spells[i][3]);
		spell.function = spells[i][4];
		spell.price = prices[i];
		
		//Permissions are backwards for spells, so this is annoying.
		spell.learnMask = 0; //clear the mask
		//get the index into the permissions bitfield for the current spell
		int byte = (i>>3), bit = (i&7), bitmask = (1<<bit);
		for (int j = 0; j < SPELL_PERMS_ENTRIES; j++) //for each class
			spell.learnMask |= (perms[j][byte] & bitmask) ? 0 : (1<<j); //see if the class can learn the spell

		spellList.push_back(spell);
	}

	//Do the enemy abilities.
	for (int i = 0; i < ABIL_ENTRIES; i++)
	{
		Spell spell;
		spell.param = abils[i][1];
		spell.element = abils[i][2];
		spell.target = (1<<abils[i][3]);
		spell.function = abils[i][4];
		spell.price = 0;
		spell.learnMask = 0;

		spellList.push_back(spell);
	}

	return spellList;
}



/****************************************************
* WRITING DATA TO ORIGINAL FANTASY FORMAT
*****************************************************
*/

void ROM::DumpMonsterData(string filename)
{
	ofstream out(filename.c_str());
	out << "MonsterID\tName\tHP\tAttX\tAtt\tAcc\tCrit\tDef\tAgi\tMDef\tInit\tExp\tGold\t"
		<< "Morale\tType\tAttElem\tAttStat\tElemRes\tElemWeak\tAI\tSprite" << endl;

	for (int i = 0; i < MONSTER_ENTRIES; i++)
	{
		out << i << '\t';
		out << monsters[i].name << '\t';
		out << monsters[i].hpMax << '\t';
		out << monsters[i].hits << '\t';
		out << monsters[i].power << '\t';
		out << monsters[i].accuracy << '\t';
		out << monsters[i].critical << '\t';
		out << monsters[i].defense << '\t';
		out << monsters[i].agility << '\t';
		out << monsters[i].magdef << '\t';
		out << monsters[i].initiative << '\t';
		out << monsters[i].exp << '\t';
		out << monsters[i].gold << '\t';
		out << monsters[i].morale << '\t';
		out << monsters[i].category << '\t';
		out << monsters[i].hitElem << '\t';
		out << monsters[i].hitStat << '\t';
		out << monsters[i].elemRes << '\t';
		out << monsters[i].elemWeak << '\t';
		out << monsters[i].aiScript << '\t';
		out << monsters[i].name << ".bmp" << endl;
	}
}

void ROM::DumpWeaponData(string filename)
{
	ofstream out(filename.c_str());
	out << "WeaponID\tName\tPrice\tPower\tAcc\tCrit\tElem\tStat\tType\tEquip\tWear\tSpell" << endl;

	for (int i = 0; i < WEAPON_ENTRIES; i++)
	{
		out << i << '\t';
		out << weapons[i].name << '\t';
		out << weapons[i].price << '\t';
		out << weapons[i].power << '\t';
		out << weapons[i].accuracy << '\t';
		out << weapons[i].critical << '\t';
		out << weapons[i].elemEffects << '\t';
		out << weapons[i].statEffects << '\t';
		out << weapons[i].catEffects << '\t';
		out << weapons[i].equipMask << '\t';
		out << weapons[i].wearloc << '\t';
		out << weapons[i].spell << endl;
	}
}

void ROM::DumpArmorData(string filename)
{
	ofstream out(filename.c_str());
	out << "ArmorID\tName\tPrice\tDef\tWeight\tElem\tEquip\tWear\tSpell" << endl;

	for (int i = 0; i < ARMOR_ENTRIES; i++)
	{
		out << i << '\t';
		out << armor[i].name << '\t';
		out << armor[i].price << '\t';
		out << armor[i].defense << '\t';
		out << armor[i].weight << '\t';
		out << armor[i].elemRes << '\t';
		out << armor[i].equipMask << '\t';
		out << armor[i].wearloc << '\t';
		out << armor[i].spell << endl;
	}
}
