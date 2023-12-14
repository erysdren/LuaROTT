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

#ifndef RT_DATADIR_H
#define RT_DATADIR_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "m_misc2.h"

void BuildDataDirList(void);
char *GetExeDir(void);
char *GetPrefDir(void);
char *GetRootDir(void);
void BuildDirs(void);
char *FindFileByName(const char *name);

/* directory to open file from */
enum {
	FILE_DIR_NONE, /* absolute path */
	FILE_DIR_EXEC, /* executable directory */
	FILE_DIR_ROOT, /* root data directory */
	FILE_DIR_PREF /* preferences directory */
};

/* file open type */
enum {
	FILE_OPEN_READ,
	FILE_OPEN_WRITE,
	FILE_OPEN_APPEND
};

/* open file (case insensitive) */
FILE *FileOpen(char *filename, int dir, int open);

/* check if file exists (case insensitive) */
bool FileExists(char *filename, int dir);

/* read size bytes from file */
size_t FileRead(void *buffer, size_t size, FILE *file);

/* write size bytes to file */
size_t FileWrite(void *buffer, size_t size, FILE *file);

/* print string to file */
int FilePrint(FILE *file, const char *format, ...);

/* close file */
void FileClose(FILE *file);

/* get full path to file at the specified dir */
char *FileGetPath(char *filename, int dir);

#endif
