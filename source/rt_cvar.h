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

#ifndef _rt_cvar_public
#define _rt_cvar_public

//****************************************************************************
//
// Public header for RT_CVAR.C
//
//****************************************************************************

//****************************************************************************
//
// TYPEDEFS
//
//****************************************************************************

/* cvar type enum */
enum
{
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
		int i;
		unsigned int u;
		fixed x;
		float f;
		const char *s;
	} value;

	/* next in chain */
	struct cvar_t *next;
} cvar_t;

//****************************************************************************
//
// MACROS
//
//****************************************************************************

/* cvar creation macros */
#define CVAR_INT(n, v) \
	(cvar_t) \
	{ \
		.name = n, .type = CVAR_TYPE_INT, .value.i = v, .next = NULL \
	}
#define CVAR_UINT(n, v) \
	(cvar_t) \
	{ \
		.name = n, .type = CVAR_TYPE_UINT, .value.u = v, .next = NULL \
	}
#define CVAR_FIXED(n, v) \
	(cvar_t) \
	{ \
		.name = n, .type = CVAR_TYPE_FIXED, .value.x = v, .next = NULL \
	}
#define CVAR_FLOAT(n, v) \
	(cvar_t) \
	{ \
		.name = n, .type = CVAR_TYPE_FLOAT, .value.f = v, .next = NULL \
	}
#define CVAR_STRING(n, v) \
	(cvar_t) \
	{ \
		.name = n, .type = CVAR_TYPE_STRING, .value.s = v, .next = NULL \
	}

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

#endif
