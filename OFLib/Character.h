/*****************************************************************************
* Original Fantasy
* Author: Michael Bennett
* Copyright 2007
*
* Character.h
* Defines the characters that make up the player's party.
*****************************************************************************/

#ifndef CHARACTER_H
#define CHARACTER_H

#include "Defs.h"

//Character definition.
class Character
{
public:

	Job job;
	int hp, hpMax;
	int mp, mpMax;
	int exp, level;
	int strength, agility, intel, vitality, luck;
	Equipment eq;
	Spell *spells[SPELL_LEVELS][SPELLS_PER_LEVEL];

	int getPower();
	int getAccuracy();
	int getCritical();
	int getDefense();
	int getEvade();
};

//Defines an instance of a character in battle.
class Ally : public Character
{
public:
	int status;
	int initiative;


};

#endif