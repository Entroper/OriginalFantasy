/*****************************************************************************
* Original Fantasy
* Author: Michael Bennett
* Copyright 2007
*
* ROM.h
* Defines a class for handling input of a NES ROM image of the original
* Final Fantasy.
*****************************************************************************/

#ifndef ROM_H
#define ROM_H

#include "../OFLib/Monster.h"
#include "../OFLib/Items.h"
#include "../OFLib/Magic.h"
#include "../OFLib/BattleDef.h"

#include <fstream>
#include <vector>
#include <string>
using namespace std;

const string QUEST_ROOT = "../Quests/FF1";

//Data table definitions.
#define MONSTER_OFFSET 0x30530
#define MONSTER_SIZE 20
#define MONSTER_ENTRIES 128

#define WEAPON_OFFSET 0x30010
#define WEAPON_PERMS_OFFSET 0x3BF60
#define WEAPON_PRICE_OFFSET 0x37C48
#define WEAPON_SIZE 8
#define WEAPON_ENTRIES 40

#define ARMOR_OFFSET 0x30150
#define ARMOR_PERMS_OFFSET 0x3BFB0
#define ARMOR_PRICE_OFFSET 0x37C98
#define ARMOR_SIZE 4
#define ARMOR_ENTRIES 40

#define SPELL_OFFSET 0x301F0
#define SPELL_PRICE_OFFSET 0x37D70
#define SPELL_PERMS_OFFSET 0x3AD28
#define SPELL_SIZE 8
#define SPELL_ENTRIES 64
#define SPELL_PERMS_SIZE SPELL_ENTRIES/8
#define SPELL_PERMS_ENTRIES 12
#define ABIL_OFFSET 0x30400
#define ABIL_SIZE 8
#define ABIL_ENTRIES 26

#define BATTLE_OFFSET 0x2C410
#define BATTLE_SIZE 16
#define BATTLE_ENTRIES 128

#define BATTLE_TILESET_OFFSET 0x1C010
#define BATTLE_TILESET_ENTRIES 16 //16 sets of
#define BATTLE_TILESET_TILE_ENTRIES 128 //128 tiles
#define BATTLE_TILE_SIZE 16 //8x8, 2 bpp
#define BATTLE_PALETTE_OFFSET 0x30F30
#define BATTLE_PALETTE_ENTRIES 64
#define BATTLE_PALETTE_SIZE 4

#define FIEND_PATTERN_TABLE 0x2D2F0
#define FIEND_OVERLAY_TABLE 0x2D330
#define FIEND_PATTERN_SHIFT 0x50
#define FIEND_OVERLAY_SHIFT 0x50
#define CHAOS_PATTERN 0x2D430
#define CHAOS_OVERLAY 0x2D4D8
#define FIEND_BATTLE 0x73
#define CHAOS_BATTLE 0x7B

#define MAP_TILESET_OFFSET 0xC010
#define MAP_TILESET_ENTRIES 8
#define MAP_TILESET_TILE_ENTRIES 128
#define MAP_TILE_SIZE 16
#define MAP_TILESET_ASSIGNMENT_OFFSET 0x2CD0
#define MAP_ENTRIES 61
#define MAP_PALETTE_OFFSET 0x2010
#define MAP_PALETTE_ENTRIES MAP_ENTRIES*3 //Each map has three palettes: standard, inside-room, and sprites+controls.
#define MAP_PALETTE_SIZE 16
#define MAP_TILESET_PATTERN_OFFSET 0x1010
#define MAP_TILESET_PATTERN_ENTRIES 128
#define MAP_TILESET_PATTERN_SIZE 4
#define MAP_TILESET_PALETTE_ASSIGNMENT_OFFSET 0x410
#define MAP_OFFSET 0x10010 //The first 128 bytes are 2-byte pointers to the maps based from this offset.

#define NES_PALETTE_ENTRIES 64

#define MONSTER_TEXT_PTR_TABLE_OFFSET 0x2D4F0
#define MONSTER_TEXT_BASE 0x24010
#define ITEM_TEXT_PTR_TABLE_OFFSET 0x2B712
#define ITEM_TEXT_BASE 0x20010
#define WEAPON_TEXT_PTR_TABLE_OFFSET 0x2B748
#define WEAPON_TEXT_BASE 0x20010
#define ARMOR_TEXT_PTR_TABLE_OFFSET 0x2B798
#define ARMOR_TEXT_BASE 0x20010



enum MonsterPic
{
	MONPIC_NORMAL,
	MONPIC_LICH,
	MONPIC_KARY,
	MONPIC_KRAKEN,
	MONPIC_TIAMAT,
	MONPIC_CHAOS
};

struct UniqueTileset
{
	int tileset;
	char palette[MAP_PALETTE_SIZE];
	int paletteIndex;
};



//The actual workhorse.
class ROM
{
public:

	ROM(string filename);
	~ROM();

	void ExportFull();

	void LoadTextTables(string stdFilename, string DTEFilename);
	void LoadNESPalette(string filename);

	void LoadBattleGraphics();
	void LoadMapGraphics();

	vector<Monster> LoadMonsters();
	vector<BattleDef> LoadBattles();
	vector<Weapon> LoadWeapons();
	vector<Armor> LoadArmor();
	vector<Spell> LoadSpells();

	void DumpMonsterGraphics(string path);
	void DumpMapGraphics(string path);

	void DumpMonsterData(string filename);
	void DumpWeaponData(string filename);
	void DumpArmorData(string filename);
	void DumpSpellData(string filename);

	void DumpMapData(string path);

private:

	void BuildRGBMapTileSprite(unsigned char *&sprite, int tileset, int palnum, int tilenum);
	void BuildRGBMonsterSprite(unsigned char *&sprite, int tileset, int palnum, int picnum);
	void BuildRGBBossSprite(unsigned char *&sprite, int tileset, int palnum1, int palnum2, MonsterPic monpic);

	void WriteBMPImage(unsigned char *sprite, int width, int height, string filename);

	vector<UniqueTileset> FindMapTilesetMappings();
	vector<UniqueTileset> FindUniqueMapTilesets(vector<UniqueTileset> tilesetMappings);
	
	ifstream in;

	unsigned char NESpalette[NES_PALETTE_ENTRIES][3];
	
	unsigned char battlePalettes[BATTLE_PALETTE_ENTRIES][BATTLE_PALETTE_SIZE];
	unsigned char battleTilesets[BATTLE_TILESET_ENTRIES][BATTLE_TILESET_TILE_ENTRIES][BATTLE_TILE_SIZE];

	unsigned char mapPalettes[MAP_PALETTE_ENTRIES][MAP_PALETTE_SIZE];
	unsigned char mapTilesets[MAP_TILESET_ENTRIES][MAP_TILESET_TILE_ENTRIES][MAP_TILE_SIZE];
	unsigned char mapTilesetPatterns[MAP_TILESET_ENTRIES][MAP_TILESET_PATTERN_ENTRIES][MAP_TILESET_PATTERN_SIZE];
	unsigned char mapTilesetPaletteAssignments[MAP_TILESET_ENTRIES][MAP_TILESET_PATTERN_ENTRIES];

	vector<Monster> monsters;
	vector<BattleDef> battles;
	vector<Weapon> weapons;
	vector<Armor> armor;
	vector<Spell> spells;

	string textTable[256];
	string DTETable[256];
};



static char *TILESET_NAMES[] =
{
	"Town",
	"Castle",
	"Castle (Rooms)",
	"Temple of Fiends",
	"Temple of Fiends (Rooms)",
	"Earth Cave Upper",
	"Earth Cave Upper (Rooms)",
	"Volcano Upper",
	"Volcano Upper (Rooms)",
	"Ice Cave",
	"Ice Cave (Rooms)",
	"Islands",
	"Islands (Rooms)",
	"Waterfall",
	"Waterfall (Rooms)",
	"Dwarf Cave",
	"Dwarf Cave (Rooms)",
	"Matoya's Cave",
	"Matoya's Cave (Rooms)",
	"Sarda's Cave",
	"Sarda's Cave (Rooms)",
	"Marsh Cave Upper",
	"Marsh Cave Upper (Rooms)",
	"Mirage Tower Lower",
	"Mirage Tower Lower (Rooms)",
	"Marsh Cave Lower",
	"Marsh Cave Lower (Rooms)",
	"Earth Cave Lower",
	"Earth Cave Lower (Rooms)",
	"Volcano Lower",
	"Volcano Lower (Rooms)",
	"Mirage Tower Upper",
	"Mirage Tower Upper (Rooms)",
	"Sea Shrine Lower",
	"Sea Shrine Lower (Rooms)",
	"Sea Shrine Upper",
	"Sea Shrine Upper (Rooms)",
	"Sky Palace",
	"Sky Palace (Rooms)",
	"Temple of Fiends Revisited",
	"Temple of Fiends Revisited (Rooms)",
	"Titan's Tunnel",
	"Titan's Tunnel (Rooms)"
};

static char *MAP_NAMES[] =
{
	"Coneria","Provoka","Elfland","Melmond","Crescent Lake","Gaia","Onrac","Lefein","Coneria Castle 1F","Elfland Castle",
	"Northwest Castle","Castle of Ordeals 1F","Temple of Fiends","Earth Cave B1","Gurgu Volcano B1","Ice Cave B1","Cardia",
	"Bahamut's Room B1","Waterfall","Dwarf Cave","Matoya's Cave","Sarda's Cave","Marsh Cave B1","Mirage Tower 1F","Coneria Castle 2F",
	"Castle of Ordeal 2F","Castle of Ordeal 3F","Marsh Cave B2","Marsh Cave B3","Earth Cave B2","Earth Cave B3","Earth Cave B4","Earth Cave B5",
	"Gurgu Volcano B2","Gurgu Volcano B3","Gurgu Volcano B4","Gurgu Volcano B5","Ice Cave B2","Ice Cave B3","Bahamut's Room B2",
	"Mirage Tower 2F","Mirage Tower 3F","Sea Shrine B5","Sea Shrine B4","Sea Shrine B3","Sea Shrine B2","Sea Shrine B1",
	"Sky Palace 1F","Sky Palace 2F","Sky Palace 3F","Sky Palace 4F","Sky Palace 5F","Temple of Fiends 1F","Temple of Fiends 2F",
	"Temple of Fiends 3F","Temple of Fiends - Earth","Temple of Fiends - Fire","Temple of Fiends - Water","Temple of Fiends - Air",
	"Temple of Fiends - Chaos","Titan's Tunnel"
};


#endif