#ifndef BATTLEDEF_H
#define BATTLEDEF_H

/*
A battle entry in the NES ROM consists of:
byte 0: High order 4 bits (0xF0) = number of enemies.
		1 = 9 small, 2 = 4 large, 3 = 6 small + 2 large, 4 = fiend, 5 = chaos
		Low order 4 bits (0x0F) = which tileset to use for the enemy graphics
byte 1: 4 entries of 2 bits each defining which of the 4 enemy graphics from
		the selected tileset belongs to each enemy
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

enum BattleFormation
{
	FORM_SMALL = 1,
	FORM_LARGE = 2,
	FORM_MIXED = 3,
	FORM_FIEND = 4,
	FORM_CHAOS = 5
};

class BattleDef
{
public:

	BattleFormation formation;
	int tileset;
	int monsterPics[4];
	int monsters[4];
	int qtyMin[6], qtyMax[6];
	int monsterPalettes[4];
	int palettes[2];
	int surpriseChance;
	bool cannotRun;
};

#endif