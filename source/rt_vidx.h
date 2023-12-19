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

/* get or set world canvas pixel */
#define VX_WorldCanvasPixel(x, y) ((uint8_t *)VX_WorldCanvas->pixels)[(y) * VX_WorldCanvas->pitch + (x)]

/* get or set hud canvas pixel */
#define VX_HudCanvasPixel(x, y) ((uint8_t *)VX_HudCanvas->pixels)[(y) * VX_HudCanvas->pitch + (x)]

/* get or set menu canvas pixel */
#define VX_MenuCanvasPixel(x, y) ((uint8_t *)VX_MenuCanvas->pixels)[(y) * VX_HudCanvas->pitch + (x)]

//***************************************************************************
//
// GLOBALS
//
//***************************************************************************

extern SDL_Surface *VX_WorldCanvas;
extern SDL_Surface *VX_HudCanvas;
extern SDL_Surface *VX_MenuCanvas;

//***************************************************************************
//
// PROTOTYPES
//
//***************************************************************************

/* initialize video subsystem */
void VX_Init(void);

/* shutdown video subsystem */
void VX_Shutdown(void);

/* restart video subsystem */
void VX_Restart(void);

/* flip all canvas changes to visible screen */
void VX_UpdateScreen(void);

/* set video palette */
void VX_SetPalette(uint8_t *palette);

/* retrieve current video palette */
void VX_GetPalette(uint8_t *palette);

/* fill video palette with color */
void VX_FillPalette(int red, int green, int blue);

/* clear video */
void VX_Clear(uint8_t color);

/* clear world canvas */
void VX_ClearWorldCanvas(uint8_t color);

/* clear hud canvas */
void VX_ClearHudCanvas(uint8_t color);

/* clear menu canvas */
void VX_ClearMenuCanvas(uint8_t color);

#endif /* _rt_vidx_public */
