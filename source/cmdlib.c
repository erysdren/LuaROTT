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
// CMDLIB.C
//
//****************************************************************************

#include "SDL.h"

#include "rt_def.h"
#include "rt_main.h"
#include "rt_cvar.h"
#include "cmdlib.h"

int _cmd_quit(int argc, char **argv)
{
	ShutDown();
	SDL_Quit();
	exit(0);
	return 0;
}

cmd_t _cmdlib[] = {
	CMD("quit", _cmd_quit)
};

void cmdlib_init(void)
{
	for (int i = 0; i < sizeof(_cmdlib) / sizeof(cmd_t); i++)
	{
		cmd_register(&_cmdlib[i]);
	}
}
