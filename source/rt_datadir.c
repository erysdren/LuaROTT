/*
	Copyright (C) 1994-1995 Apogee Software, Ltd.
	Copyright (C) 2005-2014 Simon Howard
	Copyright (C) 2023 Fabian Greffrath

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

#include <stdarg.h>

#include "SDL_filesystem.h"

#include "m_misc2.h"
#include "rt_ted.h"
#include "rt_main.h"
#include "rt_util.h"
#include "rt_datadir.h"

#include "console.h"

static char *exe_dir = NULL;
static char *pref_dir = NULL;
static char *root_dir = NULL;

char *GetExeDir(void)
{
	if (!exe_dir)
		BuildDirs();
	return exe_dir;
}

char *GetPrefDir(void)
{
	if (!pref_dir)
		BuildDirs();
	return pref_dir;
}

char *GetRootDir(void)
{
	if (!root_dir)
		BuildDirs();
	return root_dir;
}

void BuildDirs(void)
{
	char *base = NULL;
	char *root = NULL;
	char *game = NULL;

	/* build exe dir */
	base = SDL_GetBasePath();
	if (base != NULL)
	{
		exe_dir = M_StringDuplicate(base);
		SDL_free(base);
	}
	else
	{
		base = M_DirName(_argv[0]);
		exe_dir = M_StringDuplicate(base);
	}

	/* ask sdl for pref path if user wants it */
	if (CheckParm("homedir"))
	{
		root = SDL_GetPrefPath("", PACKAGE_TARNAME);
	}

	/* sdl failed, or they don't wanna use homedir */
	if (root == NULL)
	{
		cvar_set("fs_root", "./");
	}
	else
	{
		/* set root cvar to pref dir */
		cvar_set("fs_root", root);
		SDL_free(root);
	}

	/* fs_game cvar is not set yet */
	if (!cvar_is_set("fs_game"))
	{
		/* TODO: this is lame */
		if (!ROTTMAPS)
			Error("Couldn't determine game, because no mapset is loaded!");

		/* get game dir from mapset filename */
		/* and make it lowercase */
		game = M_BaseNameExt(ROTTMAPS);
		M_ForceLowercase(game);

		/* set game cvar */
		cvar_set("fs_game", game);
		SDL_free(game);
	}

	/* build root path and create directory */
	root_dir =  M_StringJoin(cvar_get_string("fs_root"), PATH_SEP_STR, NULL);
	M_MakeDirectory(root_dir);

	/* build pref path and create directory */
	pref_dir = M_StringJoin(root_dir, cvar_get_string("fs_game"), PATH_SEP_STR, NULL);
	M_MakeDirectory(pref_dir);
}

#define MAX_DATADIRS 16
static char *datadirs[MAX_DATADIRS] = { 0 };
static int num_datadirs = 0;

static void AddDataDir(char *dir)
{
	if (num_datadirs < MAX_DATADIRS)
	{
		datadirs[num_datadirs++] = dir;
	}
}

void BuildDataDirList(void)
{
	/* already been setup */
	if (datadirs[0])
	{
		return;
	}

	/* build directories */
	BuildDirs();

	// current directory
	AddDataDir(".");

	// executable directory
	AddDataDir(GetExeDir());

	// root directory
	AddDataDir(GetRootDir());
}

char *FindFileByName(const char *name)
{
	char *path;
	char *probe;
	int i;

	// Absolute path?

	probe = M_FileCaseExists(name);
	if (probe != NULL)
	{
		return probe;
	}

	BuildDataDirList();

	for (i = 0; i < num_datadirs; i++)
	{
		path = M_StringJoin(datadirs[i], PATH_SEP_STR, name, NULL);

		probe = M_FileCaseExists(path);

		free(path);

		if (probe != NULL)
		{
			return probe;
		}
	}

	// File not found

	return NULL;
}

FILE *FileOpen(const char *filename, int dir, int open)
{
	char *path = NULL;
	FILE *file = NULL;

	/* sanity checks */
	if (!filename)
		Error("FileOpen(): Tried to open NULL filename!");

	/* dir type */
	switch (dir)
	{
		/* absolute path */
		case FILE_DIR_NONE:
			path = M_StringDuplicate(filename);
			break;

		/* executable directory */
		case FILE_DIR_EXEC:
			path = M_StringJoin(GetExeDir(), PATH_SEP_STR, filename);
			break;

		/* root data directory */
		case FILE_DIR_ROOT:
			path = M_StringJoin(GetRootDir(), PATH_SEP_STR, filename);
			break;

		/* preferences directory */
		case FILE_DIR_PREF:
			path = M_StringJoin(GetPrefDir(), PATH_SEP_STR, filename);
			break;

		/* invalid */
		default:
			Error("FileOpen(): Invalid dir type %d", dir);
	}

	if (!path)
		Error("FileOpen(): Failed to allocate path string");

	/* open type */
	switch (open)
	{
		/* read */
		case FILE_OPEN_READ:
			file = fopen(path, "rb");
			break;

		/* write */
		case FILE_OPEN_WRITE:
			file = fopen(path, "wb");
			break;

		/* append */
		case FILE_OPEN_APPEND:
			file = fopen(path, "ab");
			break;

		/* invalid */
		default:
			Error("FileOpen(): Invalid open type %d", open);
	}

	free(path);

	return file;
}

bool FileExists(const char *filename, int dir)
{
	FILE *file;

	/* sanity checks */
	if (!filename)
		Error("FileExists(): Tried to check NULL filename!");
	if (dir < FILE_DIR_NONE || dir > FILE_DIR_PREF)
		Error("FileExists(): Invalid dir type %d", dir);

	file = FileOpen(filename, dir, FILE_OPEN_READ);

	if (file == NULL)
		return false;

	FileClose(file);
	return true;
}

size_t FileRead(void *buffer, size_t size, FILE *file)
{
	/* sanity checks */
	if (!file)
		Error("Tried to read from NULL file handle!");
	if (!buffer)
		Error("Tried to read file to NULL pointer!");

	return fread(buffer, 1, size, file);
}

size_t FileWrite(void *buffer, size_t size, FILE *file)
{
	/* sanity checks */
	if (!file)
		Error("Tried to write to NULL file handle!");
	if (!buffer)
		Error("Tried to write NULL pointer to file!");

	return fwrite(buffer, 1, size, file);
}

int FilePrint(FILE *file, const char *format, ...)
{
	va_list args;
	int r;

	/* sanity checks */
	if (!file)
		Error("Tried to print to NULL file handle!");
	if (!format)
		Error("Tried to print NULL string to file!");

	va_start(args, format);
	r = vfprintf(file, format, args);
	va_end(args);

	return r;
}

void FileClose(FILE *file)
{
	/* sanity checks */
	if (!file)
		Error("Tried to close NULL file handle!");

	fclose(file);
}
