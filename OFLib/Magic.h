/*****************************************************************************
* Original Fantasy
* Author: Michael Bennett
* Copyright 2007
*
* Magic.h
* Defines the game's magic spells.
*****************************************************************************/

#ifndef MAGIC_H
#define MAGIC_H

#include <string>
using namespace std;



#define SPELL_LEVELS 8
#define SPELLS_PER_LEVEL 3

//Possible spell targets.
enum SpellTarget
{
	TARG_ALL_ENEMIES = 0,
	TARG_ONE_ENEMY = 1,
	TARG_SELF = 2,
	TARG_ALL_ALLIES = 3,
	TARG_ONE_ALLY = 4
};

//A magic spell.
class Spell
{
public:

	string name; //spell name
	int mp; //mp cost for the spell
	int price; //cost to buy the spell
	int learnMask; //who can learn this spell
	int target; //whom this spell can target
	int function; //which function to use
	int param; //parameter passed to the function (status inflicted, damage, etc.)
	int element; //which elements this spell is (fire/ice/lit/death/etc.)

};

#endif