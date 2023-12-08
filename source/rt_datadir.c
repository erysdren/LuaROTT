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

#include "SDL_filesystem.h"

#include "m_misc2.h"
#include "rt_util.h"
#include "rt_datadir.h"

#include "console.h"

static char *GetExeDir(void)
{
	static char *dir;

	if (dir == NULL)
	{
		char *result;

		result = SDL_GetBasePath();
		if (result != NULL)
		{
			dir = M_StringDuplicate(result);
			SDL_free(result);
		}
		else
		{
			result = M_DirName(_argv[0]);
			dir = M_StringDuplicate(result);
		}
	}

	return dir;
}

char *GetPrefDir(void)
{
	static char *dir;

	/* override with fs_userdir cvar */
	if (cvar_is_set("fs_userdir") && cvar_get_string("fs_userdir"))
		return (char *)cvar_get_string("fs_userdir");

	if (dir == NULL)
	{
		char *result;

#ifndef PLATFORM_WINDOWS
		result = SDL_GetPrefPath("", PACKAGE_TARNAME);
		if (result != NULL)
		{
			dir = M_StringDuplicate(result);
			SDL_free(result);
		}
		else
#endif
		{
			result = GetExeDir();
			dir = M_StringDuplicate(result);
		}

		M_MakeDirectory(dir);

#if !(SHAREWARE == 1)
		result = dir;
		dir = M_StringJoin(result, "darkwar", PATH_SEP_STR, NULL);
		free(result);

		M_MakeDirectory(dir);
#endif
	}

	return dir;
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

#ifndef PLATFORM_WINDOWS
static void AddDataPath(const char *path, const char *suffix)
{
	char *left, *p, *dup_path;

	dup_path = M_StringDuplicate(path);

	// Split into individual dirs within the list.
	left = dup_path;

	for (;;)
	{
		p = strchr(left, LIST_SEP_CHAR);
		if (p != NULL)
		{
			*p = '\0';

			AddDataDir(M_StringJoin(left, suffix, NULL));
			left = p + 1;
		}
		else
		{
			break;
		}
	}

	AddDataDir(M_StringJoin(left, suffix, NULL));

	free(dup_path);
}

static void AddXdgDirs(void)
{
	char *env, *tmp_env;

	env = getenv("XDG_DATA_HOME");
	tmp_env = NULL;

	if (env == NULL)
	{
		char *homedir = getenv("HOME");
		if (homedir == NULL)
		{
			homedir = "/";
		}

		tmp_env = M_StringJoin(homedir, "/.local/share", NULL);
		env = tmp_env;
	}

	AddDataDir(M_StringJoin(env, "/games/rott", NULL));
	free(tmp_env);

	env = getenv("XDG_DATA_DIRS");
	if (env == NULL)
	{
		env = "/usr/local/share:/usr/share";
	}

	AddDataPath(env, "/games/rott");
}
#endif

static void BuildDataDirList(void)
{
	/* override data dirs with cvar */
	if (cvar_is_set("fs_datadir") && cvar_get_string("fs_datadir"))
	{
		datadirs[0] = (char *)cvar_get_string("fs_datadir");
		num_datadirs = 1;
		return;
	}

	/* already been setup */
	if (datadirs[0])
	{
		return;
	}

	// current directory
	AddDataDir(".");

	// executable directory
	AddDataDir(GetExeDir());

	// preferences directory
	AddDataDir(GetPrefDir());

#ifdef DATADIR
	// build-time data directory
	AddDataDir(DATADIR);
#endif

#ifndef PLATFORM_WINDOWS
	AddXdgDirs();
#endif
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
		if (probe != NULL)
		{
			return probe;
		}

		free(path);
	}

	// File not found

	return NULL;
}

void PrintDataDirs(void)
{
	BuildDataDirList();

	for (int i = 0; i < num_datadirs; i++)
		printf("Data dir %d: %s\n", i, datadirs[i]);
}

/* returns TRUE if the given file is a valid ROTT level */
static boolean FileIsRTL(const char *_filename)
{
	boolean r = true;
	char magic[4];
	char *filename;

	/* get full path to file */
	filename = FindFileByName(_filename);
	if (!filename)
	{
		return false;
	}

	/* open file */
	FILE *file = fopen(filename, "rb");
	if (!file)
	{
		free(filename);
		return false;
	}

	/* read magic */
	fread(magic, 1, 4, file);

	/* test all magics */
	if (memcmp(magic, "RTL\0", 4) != 0 && memcmp(magic, "RTC\0", 4) != 0 &&
		memcmp(magic, "RXL\0", 4) != 0 && memcmp(magic, "RXC\0", 4) != 0 &&
		memcmp(magic, "RXC\0", 4) != 0)
		r = false;

	fclose(file);
	free(filename);

	return r;
}

/* returns TRUE if the given file is a valid WAD file */
static boolean FileIsWAD(const char *_filename)
{
	boolean r = true;
	char magic[4];
	char *filename;

	/* get full path to file */
	filename = FindFileByName(_filename);
	if (!filename)
	{
		return false;
	}

	/* open file */
	FILE *file = fopen(filename, "rb");
	if (!file)
	{
		free(filename);
		return false;
	}

	/* read magic */
	fread(magic, 1, 4, file);

	/* test all magics */
	if (memcmp(magic, "IWAD", 4) != 0 && memcmp(magic, "PWAD", 4) != 0)
		r = false;

	fclose(file);
	free(filename);

	return r;
}

/* PrintFilesByType */
#ifdef PLATFORM_WINDOWS

#include <windows.h>

void PrintFilesByType(int type)
{
	WIN32_FIND_DATA fdFile;
	HANDLE hFind = NULL;
	char *mask;
	int i;

	BuildDataDirList();

	for (i = 0; i < num_datadirs; i++)
	{
		/* make file mask string */
		mask = M_StringJoin(datadirs[i], "\\*.*", NULL);

		/* error */
		if ((hFind = FindFirstFile(mask, &fdFile)) == INVALID_HANDLE_VALUE)
		{
			free(mask);
			continue;
		}

		/* loop over found files */
		while (FindNextFile(hFind, &fdFile))
		{
			switch (type)
			{
				case FILE_TYPE_RTL:
					if (FileIsRTL(fdFile.cFileName))
						console_printf("%s", fdFile.cFileName);
					break;

				case FILE_TYPE_WAD:
					if (FileIsWAD(fdFile.cFileName))
						console_printf("%s", fdFile.cFileName);
					break;

				default:
					break;
			}

			if (FileIsRTL(fdFile.cFileName))
				console_printf("%s", fdFile.cFileName);
		}

		free(mask);
		FindClose(hFind);
	}
}

#else

#include <dirent.h>

void PrintFilesByType(int type)
{
	DIR *dir;
	struct dirent *entry;
	int i;

	BuildDataDirList();

	for (i = 0; i < num_datadirs; i++)
	{
		dir = opendir(datadirs[i]);
		if (!dir)
			continue;

		while ((entry = readdir(dir)) != NULL)
		{
			switch (type)
			{
				case FILE_TYPE_RTL:
					if (FileIsRTL(entry->d_name))
						console_printf("%s", entry->d_name);
					break;

				case FILE_TYPE_WAD:
					if (FileIsWAD(entry->d_name))
						console_printf("%s", entry->d_name);
					break;

				default:
					break;
			}
		}

		closedir(dir);
	}
}

#endif
