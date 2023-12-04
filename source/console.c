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
// CONSOLE.C
//
//****************************************************************************

#include <stdarg.h>
#include "rt_def.h"
#include "console.h"

//****************************************************************************
//
// CVAR
//
//****************************************************************************

/* global cvar chain */
static cvar_t *cvar_list = NULL;

/* retrieve cvar from chain */
cvar_t *cvar_retrieve(const char *name)
{
	cvar_t *cvar = cvar_list;

	while (cvar != NULL)
	{
		if (strcasecmp(name, cvar->name) == 0)
			return cvar;

		/* move down chain */
		cvar = cvar->next;
	}

	return NULL;
}

/* add cvar to chain */
void cvar_register(cvar_t *cvar)
{
	/* don't add it if there's already one in the chain with the same name */
	if (cvar_retrieve(cvar->name) != NULL)
		return;

	if (cvar_list == NULL)
	{
		/* start chain */
		cvar_list = cvar;
	}
	else
	{
		/* add to chain */
		cvar->next = cvar_list;
		cvar_list = cvar;
	}
}

//****************************************************************************
//
// CVARLIB
//
//****************************************************************************

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

void cvarlib_quit(void)
{

}

//****************************************************************************
//
// CMD
//
//****************************************************************************

/* global cmd chain */
static cmd_t *cmd_list = NULL;

/* retrieve cmd from chain */
cmd_t *cmd_retrieve(const char *name)
{
	cmd_t *cmd = cmd_list;

	while (cmd != NULL)
	{
		if (strcasecmp(name, cmd->name) == 0)
			return cmd;

		/* move down chain */
		cmd = cmd->next;
	}

	return NULL;
}

/* add cmd to chain */
void cmd_register(cmd_t *cmd)
{
	/* don't add it if there's already one in the chain with the same name */
	if (cmd_retrieve(cmd->name) != NULL)
		return;

	if (cmd_list == NULL)
	{
		/* start chain */
		cmd_list = cmd;
	}
	else
	{
		/* add to chain */
		cmd->next = cmd_list;
		cmd_list = cmd;
	}
}

//****************************************************************************
//
// CMDLIB
//
//****************************************************************************

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

void cmdlib_quit(void)
{

}

//****************************************************************************
//
// CONSOLE
//
//****************************************************************************

/* print to console */
void console_printf(const char *s, ...)
{
	static char buffer[256];
	va_list args;

	/* do vargs */
	va_start(args, s);
	vsnprintf(buffer, 256, s, args);
	va_end(args);

	/* print to standard output */
	printf("%s\n", buffer);
}
