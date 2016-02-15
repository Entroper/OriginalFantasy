/*****************************************************************************
* Original Fantasy
* Author: Michael Bennett
* Copyright 2007
*
* Monster.h
* Defines Monster, which describes the characteristics of a "species" of
* monster, and Enemy, which describes an actual instance of a monster in a
* battle.
*****************************************************************************/

#ifndef MONSTER_H
#define MONSTER_H

#include "Defs.h"
#include "Magic.h"

//Defines an entry in the monster table.
class Monster
{
public:

	int exp, gold; //reward for killing monster
	int hpMax; //monster's maximum hp
	int morale; //chance a monster will run
	int agility, defense, magdef; //defensive stats
	int hits, power, accuracy, critical; //offensive stats
	int initiative; //determines who goes first in battle
	int hitElem, hitStat; //physical attack elements and status effects
	int category; //what kind of monster (dragon, undead, etc.)
	int elemMod[NUM_ELEMS]; //damage modifiers for elemental attacks
	int elemRes, elemWeak; //resistances and weaknesses to elemental magic
	string name; //monster name

	int aiScript; //ai script describes how a monster behaves in battle
	int magChance, abilChance; //describes how often a monster casts magic or uses an ability
	int spells[8], abilities[4]; //a monster's available spells and abilities

};

//Defines an actual instance of a monster in a battle.
class Enemy : public Monster
{
public:
	int hp;
	int spellIndex, abilIndex;
};

#endif