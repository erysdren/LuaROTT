/*
Copyright (C) 1994-1995 Apogee Software, Ltd.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include "cin_glob.h"
#include "scriplib.h"
#include "rt_fixed.h"
#include "z_zone.h"
#include "w_wad.h"
#include "cin_main.h"
#include "modexlib.h"
#include "lumpy.h"

#include "f_scale.h"

int cin_iscale;
byte *cin_source;
int cin_texturemid;
int cin_ycenter;
int cin_yh;
int cin_yl;

/* f_scale.asm */

void R_DrawFilmColumn(byte *buf)
{
	// This is *NOT* 100% correct - DDOI
	int count;
	int frac, fracstep;
	byte *dest;

	count = (cin_yh + 1) - cin_yl;
	if (!count)
		return;

	dest = buf + ylookup[cin_yl];

	fracstep = cin_iscale;
	frac = cin_texturemid + (cin_yl - cin_ycenter) * fracstep;

	while (count--)
	{
		*dest = cin_source[(frac >> SFRACBITS)];
		dest += iGLOBAL_SCREENWIDTH;
		frac += fracstep;
	}
}

void DrawFilmPost(byte *buf, byte *src, int height)
{
	while (height--)
	{
		*buf = *src;

		src++;

		buf += linewidth;
	}
}
