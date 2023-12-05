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
// TYPEDEFS
//
//****************************************************************************

/* cvar type enum */
enum {
	CVAR_TYPE_BOOL,
	CVAR_TYPE_INT,
	CVAR_TYPE_UINT,
	CVAR_TYPE_FIXED,
	CVAR_TYPE_FLOAT,
	CVAR_TYPE_STRING
};

/* cvar value union */
typedef union cvar_value_t {
	boolean b;
	int i;
	unsigned int u;
	fixed x;
	float f;
	char *s;
} cvar_value_t;

enum {
	CVAR_FLAG_NONE = 0,
	CVAR_FLAG_SET = 1,
	CVAR_FLAG_PROTECTED = 2
};

/* cvar structure */
typedef struct cvar_t {
	/* name for searching */
	const char *name;

	/* value type identifier */
	int type;

	/* current value  */
	cvar_value_t val;

	/* default value */
	cvar_value_t def;

	/* flags */
	unsigned int flags;

	/* next in chain */
	struct cvar_t *next;
} cvar_t;

/* console command structure */
typedef struct cmd_t {
	/* name for searching */
	const char *name;

	/* help text */
	const char *help;

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
#define CVAR_BOOL(n, v, f) (cvar_t){.name = n, .type = CVAR_TYPE_BOOL, .val.b = v, .def.b = v, .flags = f, .next = NULL}
#define CVAR_INT(n, v, f) (cvar_t){.name = n, .type = CVAR_TYPE_INT, .val.i = v, .def.i = v, .flags = f, .next = NULL}
#define CVAR_UINT(n, v, f) (cvar_t){.name = n, .type = CVAR_TYPE_UINT, .val.u = v, .def.u = v, .flags = f, .next = NULL}
#define CVAR_FIXED(n, v, f) (cvar_t){.name = n, .type = CVAR_TYPE_FIXED, .val.x = v, .def.x = v, .flags = f, .next = NULL}
#define CVAR_FLOAT(n, v, f) (cvar_t){.name = n, .type = CVAR_TYPE_FLOAT, .val.f = v, .def.f = v, .flags = f, .next = NULL}
#define CVAR_STRING(n, v, f) (cvar_t){.name = n, .type = CVAR_TYPE_STRING, .val.s = v, .def.s = v, .flags = f, .next = NULL}

/* cmd creation macro */
#define CMD(n, h, f) (cmd_t){ .name = n, .help = h, .func = f, .next = NULL }

/* pixel information for drawable console */
#define CONSOLE_BOX_X (17)
#define CONSOLE_BOX_Y (13)
#define CONSOLE_BOX_W (254)
#define CONSOLE_BOX_H (134)

/* pixel information for drawable console font */
#define CONSOLE_FONT_W (3)
#define CONSOLE_FONT_H (6)

/* pixel information for drawable console input line */
#define CONSOLE_INPUT_X (CONSOLE_BOX_X + 1)
#define CONSOLE_INPUT_Y (CONSOLE_BOX_Y + CONSOLE_BOX_H - CONSOLE_FONT_H - 1)
#define CONSOLE_INPUT_W (CONSOLE_BOX_W - 2)
#define CONSOLE_INPUT_H (CONSOLE_FONT_H)

/* number of lines to be tracked is double the amount that can be displayed */
#define CONSOLE_NUM_LINES ((CONSOLE_BOX_H / CONSOLE_FONT_H - 1) * 2)

/* line position macros */
#define CONSOLE_LINE_X (CONSOLE_BOX_X + 1)
#define CONSOLE_LINE_Y (CONSOLE_INPUT_Y - 1 - CONSOLE_FONT_H)
#define CONSOLE_LINE_W (CONSOLE_BOX_W - 2)
#define CONSOLE_LINE_H (CONSOLE_FONT_H)

//****************************************************************************
//
// GLOBALS
//
//****************************************************************************

extern boolean console_should_close;

//****************************************************************************
//
// PROTOTYPES
//
//****************************************************************************

/* retrieve cvar from chain */
cvar_t *cvar_retrieve(const char *name);

/* add cvar to chain */
void cvar_register(cvar_t *cvar);

/* retrieve boolean from cvar */
boolean cvar_get_bool(const char *name);

/* retrieve int from cvar */
int cvar_get_int(const char *name);

/* retrieve unsigned int from cvar */
unsigned int cvar_get_uint(const char *name);

/* retrieve fixed from cvar */
fixed cvar_get_fixed(const char *name);

/* retrieve float from cvar */
float cvar_get_float(const char *name);

/* retrieve string from cvar */
const char *cvar_get_string(const char *name);

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

/* draw console outline and current text buffer */
void console_draw(void);

/* print to console */
void console_printf(const char *s, ...);

/* evaluate console command */
boolean console_evaluate(char *s);

#endif /* _console_public */
