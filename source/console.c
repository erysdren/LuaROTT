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
#include "SDL.h"
#include "rt_def.h"
#include "rt_main.h"
#include "rt_str.h"
#include "rt_build.h"
#include "rt_menu.h"
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

/* cmdlib array */
cmd_t _cmdlib[] = {
	CMD("quit", _cmd_quit)
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

/* push up console lines buffer with new pointer */
static void console_push_line(char *ptr)
{
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
	int i, line;

	/* clear region */
	EraseMenuBufRegion(CONSOLE_BOX_X, CONSOLE_BOX_Y, CONSOLE_BOX_W, CONSOLE_BOX_H);

	/* console area outline */
	DrawTMenuBufBox(CONSOLE_BOX_X, CONSOLE_BOX_Y, CONSOLE_BOX_W, CONSOLE_BOX_H);

	/* set start pos */
	line = CONSOLE_NUM_LINES;

	/* draw text */
	for (i = console.num_lines; i >= 0; i--)
	{
		/* handle null lines */
		if (console.lines[i] == NULL)
		{
			line -= 1;
			continue;
		}
		if (console.lines[i][0] == '\0')
		{
			line -= 1;
			continue;
		}

		/* gone offscreen */
		if (line < 0)
			break;

		/* draw line */
		DrawMenuBufPropString(CONSOLE_LINE_X(line), CONSOLE_LINE_Y(line), console.lines[i]);

		/* move y down */
		line -= 1;
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

	console_printf("\"%s\" is not a valid cmd or cvar");
	return false;
}
