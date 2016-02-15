/*****************************************************************************
* Original Fantasy
* Author: Michael Bennett
* Copyright 2007
*
* Defs.h
* Includes various classifications such as elemental attacks, status effects,
* and the like.
*****************************************************************************/

#ifndef DEFS_H
#define DEFS_H

//Character class definitions.
#define NUM_JOBS 12
enum Job
{
	JOB_FIGHTER = 0,
	JOB_THIEF = 1,
	JOB_BLACK_BELT = 2,
	JOB_RED_MAGE = 3,
	JOB_WHITE_MAGE = 4,
	JOB_BLACK_MAGE = 5,
	JOB_KNIGHT = 6,
	JOB_NINJA = 7,
	JOB_MASTER = 8,
	JOB_RED_WIZARD = 9,
	JOB_WHITE_WIZARD = 10,
	JOB_BLACK_WIZARD = 11
};

//Status effects.
#define NUM_STATS 8
enum Status
{
	STAT_DEATH = 0,
	STAT_STONE = 1,
	STAT_POISON = 2,
	STAT_DARK = 3,
	STAT_STUN = 4,
	STAT_SLEEP = 5,
	STAT_MUTE = 6,
	STAT_CONF = 7
};

//Elemental effects.
#define NUM_ELEMS 8
enum Element
{
	ELEM_STAT = 0,
	ELEM_POISON = 1,
	ELEM_DIMEN = 2,
	ELEM_DEATH = 3,
	ELEM_FIRE = 4,
	ELEM_ICE = 5,
	ELEM_LIT = 6,
	ELEM_EARTH = 7
};

//Enemy categories.
#define NUM_CATEGORIES 8
enum Category
{
	CAT_HUMANOID = 0,
	CAT_DRAGON = 1,
	CAT_GIANT = 2,
	CAT_UNDEAD = 3,
	CAT_WERE = 4,
	CAT_WATER = 5,
	CAT_MAGE = 6,
	CAT_REGEN = 7
};

#endif