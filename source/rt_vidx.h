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

#ifndef _rt_vidx_public
#define _rt_vidx_public

//***************************************************************************
//
// Public header for RT_VIDX.C
//
//***************************************************************************

#include <stdbool.h>
#include <stdint.h>
#include "SDL.h"

//***************************************************************************
//
// DEFINES
//
//***************************************************************************

#define CANVAS_SCALE_MIN (1)
#define CANVAS_SCALE_MAX (4)
#define CANVAS_WIDTH (320)
#define CANVAS_HEIGHT (200)
#define CANVAS_BPP (8)
#define CANVAS_WIDTH_MIN (CANVAS_WIDTH * CANVAS_SCALE_MIN)
#define CANVAS_HEIGHT_MIN (CANVAS_HEIGHT * CANVAS_SCALE_MIN)
#define CANVAS_WIDTH_MAX (CANVAS_WIDTH * CANVAS_SCALE_MAX)
#define CANVAS_HEIGHT_MAX (CANVAS_HEIGHT * CANVAS_SCALE_MAX)

/* get or set overlay canvas pixel */
#define VX_OverlayCanvasPixel(x, y) ((uint8_t *)VX_OverlayCanvas->pixels)[y * VX_OverlayCanvas->pitch + x]

/* get or set world canvas pixel */
#define VX_WorldCanvasPixel(x, y) ((uint8_t *)VX_WorldCanvas->pixels)[y * VX_WorldCanvas->pitch + x]

//***************************************************************************
//
// TYPES
//
//***************************************************************************

typedef struct vxconfig_t {
	int CanvasWidth;
	int CanvasHeight;
	int WorldCanvasScale;
	bool WindowStretchAspect;
} vxconfig_t;

//***************************************************************************
//
// GLOBALS
//
//***************************************************************************

extern SDL_Surface *VX_WorldCanvas;
extern SDL_Surface *VX_OverlayCanvas;
extern vxconfig_t VX_Config;

//***************************************************************************
//
// PROTOTYPES
//
//***************************************************************************

/* initialize video subsystem */
void VX_Init(void);

/* shutdown video subsystem */
void VX_Shutdown(void);

/* flip all canvas changes to visible screen */
void VX_Flip(void);

/* set video palette */
void VX_SetPalette(uint8_t *palette);

/* retrieve current video palette */
void VX_GetPalette(uint8_t *palette);

/* fill video palette with color */
void VX_FillPalette(int red, int green, int blue);

/* clear video */
void VX_ClearVideo(uint8_t color);

#endif /* _rt_vidx_public */
