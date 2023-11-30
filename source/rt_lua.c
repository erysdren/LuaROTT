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
// RT_LUA.C
//
//****************************************************************************

#include "rt_def.h"
#include "rt_datadir.h"
#include "rt_util.h"

#include "rt_lua.h"

#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"

//****************************************************************************
//
// IO LIBRARY
//
//****************************************************************************

int _lua_io_print(lua_State *L)
{
	const char *s = luaL_checkstring(L, 1);
	if (s != NULL) printf("%s\n", s);
	return 0;
}

int _lua_io_error(lua_State *L)
{
	const char *s = luaL_checkstring(L, 1);
	if (s != NULL) Error("%s", s);
	return 0;
}

static const struct luaL_Reg _lua_io[] = {
	{"print", _lua_io_print},
	{"error", _lua_io_error},
	{NULL, NULL}
};

//****************************************************************************
//
// LOCAL VARIABLES
//
//****************************************************************************

/* lua state for menus */
static lua_State *lua_menu_state = NULL;

//****************************************************************************
//
// PUBLIC FUNCTIONS
//
//****************************************************************************

boolean lua_init(void)
{
	/* create lua state */
	lua_menu_state = luaL_newstate();
	if (lua_menu_state == NULL)
		return false;

	/* link libraries */
	luaL_newlib(lua_menu_state, _lua_io);
	lua_setglobal(lua_menu_state, "io");
	lua_settop(lua_menu_state, 0);

	return true;
}

void lua_quit(void)
{
	lua_close(lua_menu_state);
}

void lua_menu_add(const char *name)
{
	char *temp;
	char *filename;

	/* get lua path */
	temp = M_StringJoin(name, ".lua", NULL);
	filename = FindFileByName(temp);

	if (filename == NULL)
		Error("Invalid menu \"%s\" specified!", name);

	/* run file */
	luaL_loadfile(lua_menu_state, filename);
	if (lua_pcall(lua_menu_state, 0, LUA_MULTRET, 0))
	{
		Error("Lua Error: \"%s\"", lua_tostring(lua_menu_state, -1));
		return;
	}

	/* make module global */
	lua_setglobal(lua_menu_state, name);
	lua_settop(lua_menu_state, 0);

	/* run init function */
	lua_getglobal(lua_menu_state, name);
	lua_getfield(lua_menu_state, -1, "init");
	lua_call(lua_menu_state, 0, 0);

	/* free tempstrings */
	free(temp);
	free(filename);
}

void lua_menu_run_script(const char *filename)
{
	luaL_dofile(lua_menu_state, filename);
}

