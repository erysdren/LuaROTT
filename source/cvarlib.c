/*
Copyright (C) 1994-1995 Apogee Software, Ltd.
Copyright (C) 2002-2015 Steven Fuller, Ryan C. Gordon, John Hall, Dan Olson
Copyright (C) 2023 Fabian Greffrath
Copyright (C) 2023 erysdren (it/she/they)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

//****************************************************************************
//
// CVARLIB.C
//
//****************************************************************************

#include "rt_def.h"
#include "rt_main.h"
#include "console.h"
#include "cvarlib.h"

cvar_t _cvarlib[] = {
	CVAR_BOOL("test_bool", true),
	CVAR_INT("test_int", -3123),
	CVAR_UINT("test_uint", 62763746),
	CVAR_FIXED("test_fixed", (fixed)(13.7f * (1 << 16))),
	CVAR_FLOAT("test_float", 9193.64f),
	CVAR_STRING("test_string", "stringvalue")
};

void cvarlib_init(void)
{
	for (int i = 0; i < sizeof(_cvarlib) / sizeof(cvar_t); i++)
	{
		cvar_register(&_cvarlib[i]);
	}
}
