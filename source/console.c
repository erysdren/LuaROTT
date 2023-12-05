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

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include "SDL.h"
#include "rt_def.h"
#include "rt_main.h"
#include "rt_str.h"
#include "rt_util.h"
#include "rt_build.h"
#include "rt_menu.h"
#include "rt_game.h"
#include "rt_ted.h"
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


boolean cvar_get_bool(const char *name)
{
	cvar_t *cvar;

	if ((cvar = cvar_retrieve(name)) == NULL)
		return false;
	if (cvar->type != CVAR_TYPE_BOOL)
		return false;

	return cvar->value.b;
}

int cvar_get_int(const char *name)
{
	cvar_t *cvar;

	if ((cvar = cvar_retrieve(name)) == NULL)
		return 0;
	if (cvar->type != CVAR_TYPE_INT)
		return 0;

	return cvar->value.i;
}

unsigned int cvar_get_uint(const char *name)
{
	cvar_t *cvar;

	if ((cvar = cvar_retrieve(name)) == NULL)
		return 0;
	if (cvar->type != CVAR_TYPE_UINT)
		return 0;

	return cvar->value.u;
}

fixed cvar_get_fixed(const char *name)
{
	cvar_t *cvar;

	if ((cvar = cvar_retrieve(name)) == NULL)
		return 0;
	if (cvar->type != CVAR_TYPE_FIXED)
		return 0;

	return cvar->value.x;
}

float cvar_get_float(const char *name)
{
	cvar_t *cvar;

	if ((cvar = cvar_retrieve(name)) == NULL)
		return 0;
	if (cvar->type != CVAR_TYPE_FLOAT)
		return 0;

	return cvar->value.f;
}

const char *cvar_get_string(const char *name)
{
	cvar_t *cvar;

	if ((cvar = cvar_retrieve(name)) == NULL)
		return NULL;
	if (cvar->type != CVAR_TYPE_STRING)
		return NULL;

	return cvar->value.s;
}

//****************************************************************************
//
// CVARLIB
//
//****************************************************************************

/* cvarlib array */
cvar_t _cvarlib[] = {
	CVAR_BOOL("test_bool", true),
	CVAR_INT("test_int", -3123),
	CVAR_UINT("test_uint", 62763746),
	CVAR_FIXED("test_fixed", (fixed)(13.7f * (1 << 16))),
	CVAR_FLOAT("test_float", 9193.64f),
	CVAR_STRING("test_string", "stringvalue")
};

/* register standard library of cvars */
void cvarlib_init(void)
{
	for (int i = 0; i < sizeof(_cvarlib) / sizeof(cvar_t); i++)
	{
		cvar_register(&_cvarlib[i]);
	}
}

/* shutdown cvarlib */
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

/* quit */
int _cmd_quit(int argc, char **argv)
{
	ShutDown();
	SDL_Quit();
	exit(0);
	return 0;
}

/* map */
int _cmd_map(int argc, char **argv)
{
	int episode = 0, level = 0, map = 0;
	int i;

	/* print current map */
	if (argc < 2)
	{
		console_printf("%s: E%dA%d - \"%s\" (%d)",
			ROTTMAPS,
			gamestate.episode,
			GetLevel(gamestate.episode, gamestate.mapon),
			LevelName,
			gamestate.mapon
		);
		return 0;
	}

	/* convert to lowercase */
	for (i = 0; i < strlen(argv[1]); i++)
		argv[1][i] = tolower(argv[1][i]);

	/* try to figure out map from string */
	sscanf(argv[1], "e%da%d", &episode, &level);

	/* bruteforce string to int */
	if (!episode || !level)
	{
		map = strtol(argv[1], NULL, 10);
		if (map >= 100 || map < 0)
		{
			console_printf("map %d is out of range", map);
			return 1;
		}
	}
	else
	{
		/* range checks */
		if (episode > 4 || (episode == 4 && level > 13) || (episode < 4 && level > 8))
		{
			console_printf("map E%dA%d is out of range", episode, level);
			return 1;
		}
		else
		{
			/* get map */
			map = ((episode - 1) * 8) + (level - 1);
		}
	}

	/* don't do it if we're already on this map */
	if (gamestate.mapon == map)
	{
		console_printf("map %d is already loaded", map);
		return 1;
	}

	/* force close console and do warp */
	console_should_close = true;
	playstate = ex_warped;
	gamestate.mapon = map;

	return 0;
}

/* help */
int _cmd_help(int argc, char **argv)
{
	cmd_t *cmd;

	if (argc < 2)
	{
		cmd = cmd_list;

		while (cmd != NULL)
		{
			/* print name and help text */
			console_printf("%s: %s", cmd->name, cmd->help);

			/* move down chain */
			cmd = cmd->next;
		}

		return 0;
	}

	/* find specific cvar */
	if ((cmd = cmd_retrieve(argv[1])) != NULL)
	{
		console_printf("%s: %s", cmd->name, cmd->help);
	}
	else
	{
		console_printf("no command named \"%s\"", argv[1]);
		return 1;
	}

	return 0;
}

/* find */
int _cmd_find(int argc, char **argv)
{
	int i;
	char *ptr;
	cmd_t *cmd;

	if (argc < 2)
	{
		console_printf("must specify search string");
		return 1;
	}

	/* iterate over cmds */
	cmd = cmd_list;
	while (cmd != NULL)
	{
		/* do text search */
		if ((ptr = strstr(cmd->name, argv[1])) != NULL)
			console_printf("%s: %s", cmd->name, cmd->help);

		/* next */
		cmd = cmd->next;
	}

	return 0;
}

/* cmdlib array */
cmd_t _cmdlib[] = {
	CMD("quit", "exit the game immediately", _cmd_quit),
	CMD("map", "load map by name", _cmd_map),
	CMD("help", "print help text", _cmd_help),
	CMD("find", "find command by name", _cmd_find)
};

/* register standard library of cmds */
void cmdlib_init(void)
{
	for (int i = 0; i < sizeof(_cmdlib) / sizeof(cmd_t); i++)
	{
		cmd_register(&_cmdlib[i]);
	}
}

/* shutdown cmdlib */
void cmdlib_quit(void)
{

}

//****************************************************************************
//
// CONSOLE
//
//****************************************************************************

/* local constants */
#define CONSOLE_BUFFER_SIZE (8192)

/* local variables */
static struct {
	char buf[CONSOLE_BUFFER_SIZE];
	char *bufptr;
	char *lines[CONSOLE_NUM_LINES];
	int num_lines;
} console;

/* global variables */
boolean console_should_close = false;

/* push up console lines buffer with new pointer */
static void console_push_line(char *ptr)
{
	/* add pointer to lines buffer */
	console.lines[console.num_lines++] = ptr;

	/* copy second half to first half if we're about to overflow */
	if (console.num_lines >= CONSOLE_NUM_LINES - 1)
	{
		void *first;
		void *second;
		int len;

		/* pointer to first half */
		first = console.lines;

		/* pointer to second half */
		second = &console.lines[(CONSOLE_NUM_LINES / 2) - 1];

		/* copy and set size */
		len = sizeof(char *) * (CONSOLE_NUM_LINES / 2);

		/* copy second half to first half */
		memcpy(first, second, len);

		/* zero out second half (minus one for input line) */
		second = &console.lines[CONSOLE_NUM_LINES / 2];
		len = sizeof(char *) * ((CONSOLE_NUM_LINES / 2) - 1);
		memset(second, 0, len);

		/* set num_lines */
		console.num_lines = CONSOLE_NUM_LINES / 2;
	}
}

/* push string info console */
static void console_push(char *src)
{
	int i;
	int len_src = strlen(src);

	/* bounds checks */
	if (console.bufptr + len_src + 1 > console.buf + CONSOLE_BUFFER_SIZE)
		console.bufptr = console.buf;

	/* add string to text buffer */
	sprintf(console.bufptr, "%s", src);

	/* add pointer to lines buffer */
	console_push_line(console.bufptr);

	/* advance text buffer pointer */
	console.bufptr += len_src + 1;

	/* check for newlines and push line again */
	for (i = 0; i < len_src; i++)
	{
		if (console.bufptr[i] == '\n')
		{
			console.bufptr[i] = '\0';
			console_push_line(&console.bufptr[i + 1]);
		}
	}
}

/* initialize developer console */
boolean console_init(void)
{
	/* initialize all memory to zero */
	memset(&console, 0, sizeof(console));

	/* set buffer pointer */
	console.bufptr = console.buf;

	return true;
}

/* shutdown console */
void console_quit(void)
{

}

/* draw console outline and current text buffer */
void console_draw(void)
{
	int i, x, y;

	/* clear region */
	EraseMenuBufRegion(CONSOLE_BOX_X, CONSOLE_BOX_Y, CONSOLE_BOX_W, CONSOLE_BOX_H);

	/* console area outline */
	DrawTMenuBufBox(CONSOLE_BOX_X, CONSOLE_BOX_Y, CONSOLE_BOX_W, CONSOLE_BOX_H);

	/* set font */
	CurrentFont = tinyfont;

	/* line start */
	x = CONSOLE_LINE_X;
	y = CONSOLE_LINE_Y;

	/* draw text */
	for (i = console.num_lines; i >= 0; i--)
	{
		/* handle null lines */
		if (console.lines[i] == NULL)
			continue;
		if (console.lines[i][0] == '\0')
			continue;

		/* gone offscreen */
		if (y < CONSOLE_BOX_Y)
			break;

		/* draw line */
		DrawMenuBufPropString(x, y, console.lines[i]);

		/* move up */
		y -= CONSOLE_LINE_H;
	}
}

/* print to console */
void console_printf(const char *s, ...)
{
	static char buf[256];
	va_list args;

	/* do vargs */
	va_start(args, s);
	vsnprintf(buf, sizeof(buf), s, args);
	va_end(args);

	/* print to standard output */
	printf("%s\n", buf);
	console_push(buf);
}

/* evaluate console command */
boolean console_evaluate(char *s)
{
	int argc;
	char **argv;
	cmd_t *cmd;
	cvar_t *cvar;

	/* print */
	console_printf("> %s", s);

	argv = US_Tokenize(s, &argc);

	if (!argv || !argc)
		return false;

	/* check for cmd */
	if ((cmd = cmd_retrieve(argv[0])) != NULL)
	{
		cmd->func(argc, argv);
		return true;
	}

	/* check cvar */
	if ((cvar = cvar_retrieve(argv[0])) != NULL)
	{
		/* user probably wants to set it */
		if (argv[1])
		{
			/* set value */
			switch (cvar->type)
			{
				case CVAR_TYPE_BOOL:
					if (strcasecmp(argv[1], "true") == 0)
						cvar->value.b = true;
					else if (strcasecmp(argv[1], "false") == 0)
						cvar->value.b = false;
					else if (strtol(argv[1], NULL, 10))
						cvar->value.b = true;
					else if (!strtol(argv[1], NULL, 10))
						cvar->value.b = false;
					else
						cvar->value.b = false;
					break;

				case CVAR_TYPE_INT:
					cvar->value.i = strtol(argv[1], NULL, 10);
					break;

				case CVAR_TYPE_UINT:
					cvar->value.u = strtoul(argv[1], NULL, 10);
					break;

				case CVAR_TYPE_FIXED:
					cvar->value.x = FIXED(strtof(argv[1], NULL));
					break;

				case CVAR_TYPE_FLOAT:
					cvar->value.f = strtof(argv[1], NULL);
					break;

				case CVAR_TYPE_STRING:
					Error("setting a string cvar is not yet supported");
					break;
			}
		}
		else
		{
			/* print value */
			switch (cvar->type)
			{
				case CVAR_TYPE_BOOL:
					if (cvar->value.b)
						console_printf("true");
					else
						console_printf("false");
					break;

				case CVAR_TYPE_INT:
					console_printf("%d", cvar->value.i);
					break;

				case CVAR_TYPE_UINT:
					console_printf("%u", cvar->value.u);
					break;

				case CVAR_TYPE_FIXED:
					console_printf("%0.4f", cvar->value.x * (1.0f / (float)(1 << 16)));
					break;

				case CVAR_TYPE_FLOAT:
					console_printf("%0.4f", cvar->value.f);
					break;

				case CVAR_TYPE_STRING:
					console_printf("\"%s\"", cvar->value.s);
					break;
			}
		}

		return true;
	}

	console_printf("\"%s\" is not a valid cmd or cvar", s);
	return false;
}
