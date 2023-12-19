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

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "modexlib.h"
#include "rt_view.h"
#include "rt_vidx.h"

/* global variables */
SDL_Surface *VX_WorldCanvas = NULL;
SDL_Surface *VX_HudCanvas = NULL;
SDL_Surface *VX_MenuCanvas = NULL;

/* local variables */
static const int BaseWidth = 320;
static const int BaseHeight = 200;
static int RenderScale = 1;
static bool WindowStretchAspect = true;
static SDL_Window *Window = NULL;
static SDL_Renderer *Renderer = NULL;
static SDL_Texture *RenderTexture = NULL;
static SDL_Surface *RenderSurface = NULL;
static SDL_Color PaletteColors[256];

/* initialize video subsystem */
void VX_Init(void)
{
	unsigned int pixel_format, rmask, gmask, bmask, amask;
	int bpp;
	int i, offset;

	/* init sdl subsystems */
	SDL_InitSubSystem(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

	/* create world canvas */
	VX_WorldCanvas = SDL_CreateRGBSurface(0, BaseWidth * RenderScale, BaseHeight * RenderScale, 8, 0, 0, 0, 0);
	SDL_FillRect(VX_WorldCanvas, NULL, 0);

	/* create hud canvas */
	VX_HudCanvas = SDL_CreateRGBSurface(0, BaseWidth, BaseWidth, 8, 0, 0, 0, 0);
	SDL_SetColorKey(VX_HudCanvas, SDL_TRUE, 0xFF);
	SDL_FillRect(VX_HudCanvas, NULL, 0xFF);

	/* create menu canvas */
	VX_MenuCanvas = SDL_CreateRGBSurface(0, BaseWidth, BaseWidth, 8, 0, 0, 0, 0);
	SDL_SetColorKey(VX_MenuCanvas, SDL_TRUE, 0xFF);
	SDL_FillRect(VX_MenuCanvas, NULL, 0xFF);

	/* create window */
	Window = SDL_CreateWindow(PACKAGE_STRING, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, VX_WorldCanvas->w * 2, VX_WorldCanvas->h * 2 * 1.2f, SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
	SDL_SetWindowMinimumSize(Window, VX_WorldCanvas->w * 2, VX_WorldCanvas->h * 2 * 1.2f);

	/* setup stretch */
	SDL_RenderSetLogicalSize(Renderer, VX_WorldCanvas->w, VX_WorldCanvas->h * 1.2f);

	/* create renderer */
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
	Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_PRESENTVSYNC);
	SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 0xFF);
	SDL_RenderClear(Renderer);
	SDL_RenderPresent(Renderer);

	/* get pixel format from screen */
	pixel_format = SDL_GetWindowPixelFormat(Window);
	SDL_PixelFormatEnumToMasks(pixel_format, &bpp, &rmask, &gmask, &bmask, &amask);

	/* create render surface */
	RenderSurface = SDL_CreateRGBSurface(0, VX_WorldCanvas->w, VX_WorldCanvas->h, bpp, rmask, gmask, bmask, amask);

	/* create render texture */
	RenderTexture = SDL_CreateTexture(Renderer, pixel_format, SDL_TEXTUREACCESS_STREAMING, VX_WorldCanvas->w, VX_WorldCanvas->h);

	/* set up lookup tables */
	linewidth = VX_WorldCanvas->w;
	offset = 0;
	for (i = 0; i < VX_WorldCanvas->h; i++)
	{
		ylookup[i] = offset;
		offset += linewidth;
	}

	/* compatibility */
	pagestart = VX_WorldCanvas->pixels;
	displayofs = pagestart;
	bufferofs = pagestart;
}

/* shutdown video subsystem */
void VX_Shutdown(void)
{
	if (Window) SDL_DestroyWindow(Window);
	if (Renderer) SDL_DestroyRenderer(Renderer);
	if (RenderTexture) SDL_DestroyTexture(RenderTexture);
	if (RenderSurface) SDL_FreeSurface(RenderSurface);
	if (VX_WorldCanvas) SDL_FreeSurface(VX_WorldCanvas);
	if (VX_HudCanvas) SDL_FreeSurface(VX_HudCanvas);
	if (VX_MenuCanvas) SDL_FreeSurface(VX_MenuCanvas);
}

/* restart video subsystem */
void VX_Restart(void)
{
	VX_Shutdown();
	VX_Init();
}

/* flip all canvas changes to visible screen */
void VX_UpdateScreen(void)
{
	SDL_Rect src, dst;

	/* setup dst rectangle */
	dst.x = 0;
	dst.y = 0;
	dst.w = RenderSurface->w;
	dst.h = RenderSurface->h;

	/* blit world canvas */
	src.x = 0;
	src.y = 0;
	src.w = VX_WorldCanvas->w;
	src.h = VX_WorldCanvas->h;
	SDL_BlitSurface(VX_WorldCanvas, &src, RenderSurface, &dst);

	/* blit hud canvas */
	src.x = 0;
	src.y = 0;
	src.w = VX_HudCanvas->w;
	src.h = VX_HudCanvas->h;
	SDL_BlitSurface(VX_HudCanvas, &src, RenderSurface, &dst);

	/* blit menu canvas */
	src.x = 0;
	src.y = 0;
	src.w = VX_MenuCanvas->w;
	src.h = VX_MenuCanvas->h;
	SDL_BlitSurface(VX_MenuCanvas, &src, RenderSurface, &dst);

	/* update screen */
	SDL_UpdateTexture(RenderTexture, NULL, RenderSurface->pixels, RenderSurface->pitch);
	SDL_RenderClear(Renderer);
	SDL_RenderCopy(Renderer, RenderTexture, NULL, NULL);
	SDL_RenderPresent(Renderer);
}

/* set video palette */
void VX_SetPalette(uint8_t *palette)
{
	int i;

	for (i = 0; i < 256; i++)
	{
		PaletteColors[i].r = gammatable[(gammaindex << 6) + (*palette++)] << 2;
		PaletteColors[i].g = gammatable[(gammaindex << 6) + (*palette++)] << 2;
		PaletteColors[i].b = gammatable[(gammaindex << 6) + (*palette++)] << 2;
	}

	SDL_SetPaletteColors(VX_WorldCanvas->format->palette, PaletteColors, 0, 256);
	SDL_SetPaletteColors(VX_HudCanvas->format->palette, PaletteColors, 0, 256);
	SDL_SetPaletteColors(VX_MenuCanvas->format->palette, PaletteColors, 0, 256);
}

/* retrieve current video palette */
void VX_GetPalette(uint8_t *palette)
{
	int i;

	for (i = 0; i < 256; i++)
	{
		palette[i * 3] = PaletteColors[i].r >> 2;
		palette[i * 3 + 1] = PaletteColors[i].g >> 2;
		palette[i * 3 + 2] = PaletteColors[i].b >> 2;
	}
}

/* fill video palette with color */
void VX_FillPalette(int red, int green, int blue)
{
	int i;

	for (i = 0; i < 256; i++)
	{
		PaletteColors[i].r = red << 2;
		PaletteColors[i].g = green << 2;
		PaletteColors[i].b = blue << 2;
	}

	SDL_SetPaletteColors(VX_WorldCanvas->format->palette, PaletteColors, 0, 256);
	SDL_SetPaletteColors(VX_HudCanvas->format->palette, PaletteColors, 0, 256);
	SDL_SetPaletteColors(VX_MenuCanvas->format->palette, PaletteColors, 0, 256);
}

/* clear video */
void VX_Clear(uint8_t color)
{
	VX_ClearWorldCanvas(color);
	VX_ClearHudCanvas(0xFF);
	VX_ClearMenuCanvas(0xFF);
}

/* clear world canvas */
void VX_ClearWorldCanvas(uint8_t color)
{
	SDL_FillRect(VX_WorldCanvas, NULL, color);
}

/* clear hud canvas */
void VX_ClearHudCanvas(uint8_t color)
{
	SDL_FillRect(VX_HudCanvas, NULL, color);
}

/* clear menu canvas */
void VX_ClearMenuCanvas(uint8_t color)
{
	SDL_FillRect(VX_MenuCanvas, NULL, color);
}
