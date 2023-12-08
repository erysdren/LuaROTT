/*
Copyright (C) 1994-1995 Apogee Software, Ltd.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
#ifndef _rt_ted_private
#define _rt_ted_private

#define MAXPRECACHE 3500

#include "rt_actor.h"
#include "develop.h"

#define SHAREWARE_TAG 0x4d4b
#define REGISTERED_TAG 0x4344
#define RTL_VERSION (0x0101)
#define RXL_VERSION (0x0200)
#define COMMBAT_SIGNATURE ("RTC")
#define NORMAL_SIGNATURE ("RTL")
#define RANDROTT_SIGNATURE ("RTR")
#define EXTENDED_COMMBAT_SIGNATURE ("RXC")
#define EXTENDED_SIGNATURE ("RXL")
#define RTL_HEADER_OFFSET 8

typedef struct
{
	int lump;
	int cachelevel;
	int type; // To make precaching possible on big endian machines
} cachetype;

//========================================

typedef struct
{
	short RLEWtag;
	int headeroffsets[100];
	byte tileinfo[1];
} mapfiletype;

typedef struct
{
	int planestart[3];
	word planelength[3];
	word width, height;
	char name[16];
} maptype;

static inline boolean ActorIsPushWall(int xx, int yy)
{
	if (xx < 0 || xx >= MAPSIZE || yy < 0 || yy >= MAPSIZE)
		return false;
	if (actorat[xx][yy] == NULL)
		return false;
	if (((objtype *)actorat[xx][yy])->which == PWALL)
		return true;
	return false;
}

static inline boolean ActorIsWall(int xx, int yy)
{
	if (xx < 0 || xx >= MAPSIZE || yy < 0 || yy >= MAPSIZE)
		return false;
	if (actorat[xx][yy] == NULL)
		return false;
	if (((objtype *)actorat[xx][yy])->which == WALL)
		return true;
	return false;
}

static inline boolean ActorIsSpring(int xx, int yy)
{
	if (xx < 0 || xx >= MAPSIZE || yy < 0 || yy >= MAPSIZE)
		return false;
	if (actorat[xx][yy] == NULL)
		return false;
	if (((objtype *)actorat[xx][yy])->which != SPRITE && ((objtype *)actorat[xx][yy])->which != ACTOR)
		return false;
	if (((objtype *)actorat[xx][yy])->obclass == springobj)
		return true;
	return false;
}

static inline boolean StaticUndefined(int xx, int yy)
{
	if (xx < 0 || xx >= MAPSIZE || yy < 0 || yy >= MAPSIZE)
		return false;
	if (sprites[xx][yy] == NULL)
		return false;
	if (((statobj_t *)sprites[xx][yy])->z < -64)
		return true;
	return false;
}

#define PRECACHEASTRINGX 141
#define PRECACHEASTRINGY 8

#define PRECACHEESTRINGX 16
#define PRECACHEESTRINGY 8

#define PRECACHESTRINGX 16
#define PRECACHESTRINGY 144

#define PRECACHEBARX 28
#define PRECACHEBARY 178

#define PRECACHELED1X 9
#define PRECACHELED1Y 8

#define PRECACHELED2X 9
#define PRECACHELED2Y 12

#define MAXLEDS 57

#define MAXSILLYSTRINGS 32

#endif
