/*****************************************************************************
* Original Fantasy
* Author: Michael Bennett
* Copyright 2007
*
* Items.h
* Defines items in the game, such as potions, weapons, armor, etc.
*****************************************************************************/

#ifndef ITEMS_H
#define ITEMS_H

#include "Defs.h"
#include <string>
using namespace std;



//Wear location determines where an item can be equipped.
enum Wearloc
{
	WEAR_HAND = 0,
	WEAR_OFFHAND = 1,
	WEAR_HEAD = 2,
	WEAR_BODY = 3,
	WEAR_GLOVE = 4,
	WEAR_RELIC = 5
};



//Base class for all items.
class Item
{
public:
	int price;
	int spell; //what spell does this item cast
	string name;
};

//Abstract base class for weapons and armor.
class Equippable : public Item
{
public:
	int equipMask; //who can wear this
	int wearloc; //where is this equipment worn
	
	virtual bool isWeapon() = 0;
	virtual bool isArmor() = 0;
};

//Weapons.
class Weapon : public Equippable
{
public:
	int power, accuracy, critical;
	int statEffects; //what status effects does this weapon inflict
	int catEffects; //what enemy types is this weapon strong against
	int elemEffects; //elemental weapon damage

	virtual bool isWeapon() { return true; }
	virtual bool isArmor() { return false; }
};

//Armor.
class Armor : public Equippable
{
public:
	int defense, weight;
	int elemRes; //protection from status-inflicting elemental spells
	int elemMod[NUM_ELEMS]; //elemental damage modification

	virtual bool isWeapon() { return false; }
	virtual bool isArmor() { return true; }
};



//Defines a set of equipment for a charater.
struct Equipment
{
	Equippable *left, *right;
	Armor *helm, *armor, *relic;
};



#endif