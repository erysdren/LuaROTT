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

#ifndef _console_public
#define _console_public

#include "rt_fixed.h"

//****************************************************************************
//
// Public header for CONSOLE.C
//
//****************************************************************************

//****************************************************************************
//
// PREPROCESSOR
//
//****************************************************************************

/* pixel information for drawable console */
#define CONSOLE_X (17)
#define CONSOLE_Y (13)
#define CONSOLE_W (254)
#define CONSOLE_H (133)

/* pixel information for drawable console font */
#define CONSOLE_FONT_W (3)
#define CONSOLE_FONT_H (6)

/* pixel information for drawable console input line */
#define CONSOLE_IN_X (CONSOLE_X + 1)
#define CONSOLE_IN_Y (CONSOLE_Y + CONSOLE_H - CONSOLE_FONT_H - 1)
#define CONSOLE_IN_W (CONSOLE_W - 2)
#define CONSOLE_IN_H (CONSOLE_FONT_H)

/* other console information */
#define CONSOLE_NUM_LINES (CONSOLE_H / CONSOLE_FONT_H - 1)

//****************************************************************************
//
// TYPEDEFS
//
//****************************************************************************

/* cvar type enum */
enum
{
	CVAR_TYPE_BOOL,
	CVAR_TYPE_INT,
	CVAR_TYPE_UINT,
	CVAR_TYPE_FIXED,
	CVAR_TYPE_FLOAT,
	CVAR_TYPE_STRING
};

/* cvar structure */
typedef struct cvar_t
{
	/* name for searching */
	const char *name;

	/* value type identifier */
	int type;

	/* value union */
	union {
		boolean b;
		int i;
		unsigned int u;
		fixed x;
		float f;
		const char *s;
	} value;

	/* next in chain */
	struct cvar_t *next;
} cvar_t;

/* console command structure */
typedef struct cmd_t
{
	/* name for searching */
	const char *name;

	/* function to call */
	int (*func)(int, char**);

	/* next in chain */
	struct cmd_t *next;

} cmd_t;

//****************************************************************************
//
// MACROS
//
//****************************************************************************

/* cvar creation macros */
#define CVAR_BOOL(n, v) (cvar_t){.name = n, .type = CVAR_TYPE_BOOL, .value.b = v, .next = NULL}
#define CVAR_INT(n, v) (cvar_t){.name = n, .type = CVAR_TYPE_INT, .value.i = v, .next = NULL}
#define CVAR_UINT(n, v) (cvar_t){.name = n, .type = CVAR_TYPE_UINT, .value.u = v, .next = NULL}
#define CVAR_FIXED(n, v) (cvar_t){.name = n, .type = CVAR_TYPE_FIXED, .value.x = v, .next = NULL}
#define CVAR_FLOAT(n, v) (cvar_t){.name = n, .type = CVAR_TYPE_FLOAT, .value.f = v, .next = NULL}
#define CVAR_STRING(n, v) (cvar_t){.name = n, .type = CVAR_TYPE_STRING, .value.s = v, .next = NULL}

/* cmd creation macro */
#define CMD(n, f) (cmd_t){ .name = n, .func = f, .next = NULL }

//****************************************************************************
//
// GLOBALS
//
//****************************************************************************

//****************************************************************************
//
// PROTOTYPES
//
//****************************************************************************

/* retrieve cvar from chain */
cvar_t *cvar_retrieve(const char *name);

/* add cvar to chain */
void cvar_register(cvar_t *cvar);

/* register standard library of cvars */
void cvarlib_init(void);

/* shutdown cvarlib */
void cvarlib_quit(void);

/* retrieve cmd from chain */
cmd_t *cmd_retrieve(const char *name);

/* add cmd to chain */
void cmd_register(cmd_t *cmd);

/* register standard library of cmds */
void cmdlib_init(void);

/* shutdown cmdlib */
void cmdlib_quit(void);

/* initialize developer console */
boolean console_init(void);

/* shutdown console */
void console_quit(void);

/* print to console */
void console_printf(const char *s, ...);

#endif /* _console_public */
