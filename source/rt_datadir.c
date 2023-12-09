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

#if 0
#ifndef PLATFORM_WINDOWS
	AddXdgDirs();
#endif
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

		free(path);

		if (probe != NULL)
		{
			return probe;
		}
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
