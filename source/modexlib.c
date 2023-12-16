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

#include <stdarg.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "SDL.h"

#include <stdlib.h>
#include <sys/stat.h>
#include "modexlib.h"
#include "rt_util.h"
#include "rt_net.h" // for GamePaused
#include "myprint.h"
#include "lumpy.h"

static void StretchMemPicture();
// GLOBAL VARIABLES

boolean StretchScreen = 0; // bná++
extern boolean iG_aimCross;
extern boolean sdl_fullscreen;
extern boolean sdl_mouse_grabbed;
extern int iG_X_center;
extern int iG_Y_center;
byte *iG_buf_center;

int linewidth;
int ylookup[800]; // Table of row offsets
int screensize;
byte *displayofs;
byte *bufferofs;
boolean graphicsmode = false;
byte *bufofsTopLimit;
byte *bufofsBottomLimit;

/* global video config */
vidconfig_t vidconfig = {
	640, /* WindowWidth */
	480, /* WindowHeight */
	320, /* ScreenBaseWidth */
	200, /* ScreenBaseHeight */
	320, /* ScreenWidth */
	200, /* ScreenHeight */
	1,   /* ScreenScale */
	true /* ScreenStretch */
};

/*
====================
=
= GraphicsMode
=
====================
*/

/* 320x200 */
SDL_Surface *BackSurfaceStretched = NULL;

/* 320x200 * vidconfig.ScreenScale */
SDL_Surface *BackSurface = NULL;

/* 320x200 * vidconfig.ScreenScale */
SDL_Surface *FrontSurface = NULL;

/* current pixel buffer */
byte *BackBuffer = NULL;

static SDL_Window *screen;
static SDL_Renderer *renderer;
static SDL_Texture *texture;
static SDL_Rect blit_rect = { 0 };

int VL_SaveBMP(const char *file)
{
	return SDL_SaveBMP(BackSurface, file);
}

void SetShowCursor(int show)
{
	SDL_SetRelativeMouseMode(!show);
	SDL_GetRelativeMouseState(NULL, NULL);
	sdl_mouse_grabbed = !show;
}

void GraphicsMode(void)
{
	uint32_t flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;
	uint32_t pixel_format;

	unsigned int rmask, gmask, bmask, amask;
	int bpp;

	if (SDL_InitSubSystem(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
	{
		Error("Could not initialize SDL\n");
	}

	if (sdl_fullscreen)
	{
		flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
	}

	screen = SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, vidconfig.WindowWidth, vidconfig.WindowHeight, flags);
	SDL_SetWindowMinimumSize(screen, vidconfig.WindowWidth, vidconfig.WindowHeight);
	SDL_SetWindowTitle(screen, PACKAGE_STRING);

	renderer = SDL_CreateRenderer(screen, -1, SDL_RENDERER_PRESENTVSYNC);
	if (!renderer)
	{
		renderer = SDL_CreateRenderer(screen, -1, SDL_RENDERER_SOFTWARE);
	}

	/* enable screen stretch */
	SetScreenStretch(vidconfig.ScreenStretch);

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);

	BackSurface = SDL_CreateRGBSurface(0, vidconfig.ScreenWidth, vidconfig.ScreenHeight, 8, 0, 0, 0, 0);
	SDL_FillRect(BackSurface, NULL, 0);

	pixel_format = SDL_GetWindowPixelFormat(screen);
	SDL_PixelFormatEnumToMasks(pixel_format, &bpp, &rmask, &gmask, &bmask, &amask);
	FrontSurface = SDL_CreateRGBSurface(0, vidconfig.ScreenWidth, vidconfig.ScreenHeight, bpp, rmask, gmask, bmask, amask);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
	texture = SDL_CreateTexture(renderer, pixel_format, SDL_TEXTUREACCESS_STREAMING, vidconfig.ScreenWidth, vidconfig.ScreenHeight);

	blit_rect.w = vidconfig.ScreenWidth;
	blit_rect.h = vidconfig.ScreenHeight;

	SetShowCursor(!sdl_fullscreen);
}

void ToggleFullScreen(void)
{
	unsigned int flags = 0;

	sdl_fullscreen = !sdl_fullscreen;

	if (sdl_fullscreen)
	{
		flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
	}

	SDL_SetWindowFullscreen(screen, flags);
	SetShowCursor(!sdl_fullscreen);
}

/*
====================
=
= SetTextMode
=
====================
*/
void SetTextMode(void)
{
	if (SDL_WasInit(SDL_INIT_VIDEO) == SDL_INIT_VIDEO)
	{
		if (BackSurface != NULL)
		{
			SDL_FreeSurface(BackSurface);

			BackSurface = NULL;
		}

		SDL_QuitSubSystem(SDL_INIT_VIDEO);
	}
}

/*
====================
=
= TurnOffTextCursor
=
====================
*/
void TurnOffTextCursor(void)
{
}

/*
====================
=
= WaitVBL
=
====================
*/
void WaitVBL(void)
{
	SDL_Delay(16667 / 1000);
}

/*
=======================
=
= VL_SetVGAPlaneMode
=
=======================
*/

void VL_SetVGAPlaneMode(void)
{
	int i, offset;

	GraphicsMode();

	//
	// set up lookup tables
	//
	// bna--   linewidth = 320;
	linewidth = vidconfig.ScreenWidth;

	offset = 0;

	for (i = 0; i < vidconfig.ScreenHeight; i++)
	{
		ylookup[i] = offset;
		offset += linewidth;
	}

	//    screensize=MAXSCREENHEIGHT*MAXSCREENWIDTH;
	screensize = vidconfig.ScreenHeight * vidconfig.ScreenWidth;

	BackBuffer = BackSurface->pixels;
	displayofs = BackBuffer;
	bufferofs = BackBuffer;

	iG_X_center = vidconfig.ScreenWidth / 2;
	iG_Y_center = (vidconfig.ScreenHeight / 2) + 10; //+10 = move aim down a bit

	//(iG_Y_center*vidconfig.ScreenWidth);//+iG_X_center;
	iG_buf_center = BackBuffer + (screensize / 2);

	bufofsTopLimit = BackBuffer + screensize - vidconfig.ScreenWidth;
	bufofsBottomLimit = BackBuffer + vidconfig.ScreenWidth;

	// start stretched
	EnableScreenStretch();
	XFlipPage();
}

/*
=======================
=
= VL_CopyPlanarPage
=
=======================
*/
void VL_CopyPlanarPage(byte *src, byte *dest)
{
	memcpy(dest, src, screensize);
}

/*
=======================
=
= VL_CopyPlanarPageToMemory
=
=======================
*/
void VL_CopyPlanarPageToMemory(byte *src, byte *dest)
{
	memcpy(dest, src, screensize);
}

/*
=================
=
= VL_ClearBuffer
=
= Fill the entire video buffer with a given color
=
=================
*/

void VL_ClearBuffer(byte *buf, byte color)
{
	memset((byte *)buf, color, screensize);
}

/*
=================
=
= VL_ClearVideo
=
= Fill the entire video buffer with a given color
=
=================
*/

void VL_ClearVideo(byte color)
{
	memset(BackSurface->pixels, color, vidconfig.ScreenWidth * vidconfig.ScreenHeight);
}

/*
=================
=
= VL_DePlaneVGA
=
=================
*/

void VL_DePlaneVGA(void)
{
}

/* C version of rt_vh_a.asm */

void VH_UpdateScreen(void)
{
	/* get current window size */
	SDL_GetWindowSize(screen, &vidconfig.WindowWidth, &vidconfig.WindowHeight);

	if (StretchScreen)
		StretchMemPicture();

	/* blit video buffer to screen */
	SDL_LowerBlit(BackSurface, &blit_rect, FrontSurface, &blit_rect);
	SDL_UpdateTexture(texture, NULL, FrontSurface->pixels, FrontSurface->pitch);
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
}

/*
=================
=
= XFlipPage
=
=================
*/

void XFlipPage(void)
{
	if (StretchScreen)
		StretchMemPicture();

	SDL_LowerBlit(BackSurface, &blit_rect, FrontSurface, &blit_rect);
	SDL_UpdateTexture(texture, NULL, FrontSurface->pixels, FrontSurface->pitch);
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
}

void EnableScreenStretch(void)
{
	if (vidconfig.ScreenWidth <= 320 || StretchScreen)
		return;

	if (BackSurfaceStretched == NULL)
	{
		/* should really be just 320x200, but there is code all over the
		   places which crashes then */
		BackSurfaceStretched = SDL_CreateRGBSurface(0, vidconfig.ScreenWidth, vidconfig.ScreenHeight, 8, 0, 0, 0, 0);
	}

	displayofs = (byte *)BackSurfaceStretched->pixels + (displayofs - (byte *)BackSurface->pixels);
	BackBuffer = BackSurfaceStretched->pixels;
	bufferofs = BackBuffer;
	StretchScreen = 1;
}

void DisableScreenStretch(void)
{
	if (vidconfig.ScreenWidth <= 320 || !StretchScreen)
		return;

	displayofs = (byte *)BackSurface->pixels + (displayofs - (byte *)BackSurfaceStretched->pixels);
	BackBuffer = BackSurface->pixels;
	bufferofs = BackBuffer;
	StretchScreen = 0;
}

// bna section -------------------------------------------
static void StretchMemPicture()
{
	SDL_Rect src;
	SDL_Rect dest;

	src.x = 0;
	src.y = 0;
	src.w = 320;
	src.h = 200;

	dest.x = 0;
	dest.y = 0;
	dest.w = vidconfig.ScreenWidth;
	dest.h = vidconfig.ScreenHeight;
	SDL_SoftStretch(BackSurfaceStretched, &src, BackSurface, &dest);
}

// bna section -------------------------------------------

/*
 *
 * erysdren
 *
 */

/*
 * 4:3 screen stretch
 */

void ToggleScreenStretch(void)
{
	SetScreenStretch(!vidconfig.ScreenStretch);
}

void SetScreenStretch(boolean to)
{
	int width, height;

	vidconfig.ScreenStretch = to;

	if (vidconfig.ScreenStretch)
	{
		width = vidconfig.ScreenWidth;
		height = vidconfig.ScreenHeight * 1.2f;
	}
	else
	{
		width = vidconfig.ScreenWidth;
		height = vidconfig.ScreenHeight;
	}

	SDL_RenderSetLogicalSize(renderer, width, height);
}

/*
 * cache pic_t to SDL_Surface
 */

/* [0] = pic_t */
/* [1] = SDL_Surface */
static int num_PicCache = 0;
static void *PicCache[4096][2];

SDL_Surface *GetCachedPic(pic_t *source)
{
	int i;

	for (i = 0; i < num_PicCache; i++)
	{
		if (source == PicCache[i][0])
			return PicCache[i][1];
	}

	return NULL;
}

SDL_Surface *CachePic(pic_t *source)
{
	SDL_Surface *surf;
	byte *ptr, *destline;
	int plane;
	int x, y;

	/* check if already cached */
	if ((surf = GetCachedPic(source)) != NULL)
		return surf;

	/* create surface */
	surf = SDL_CreateRGBSurface(0, source->width * 4, source->height, 8, 0, 0, 0, 0);

	/* set palette */
	SDL_SetPaletteColors(surf->format->palette, BackSurface->format->palette->colors, 0, 256);

	/* convert planes to linear pixel data */
	ptr = &source->data;
	for (plane = 0; plane < 4; plane++)
	{
		for (y = 0; y < source->height; y++)
		{
			destline = (byte *)surf->pixels + (y * surf->pitch);

			for (x = 0; x < source->width; x++)
			{
				destline[x * 4 + plane] = *ptr++;
			}
		}
	}

	/* add to cache */
	PicCache[num_PicCache][0] = source;
	PicCache[num_PicCache][1] = surf;
	num_PicCache++;

	return surf;
}

/*
 * draw pic_t on screen
 */

void DrawPic(pic_t *source, int x, int y, int scale)
{
	SDL_Surface *surf;
	SDL_Rect srcrect, dstrect;

	if ((surf = CachePic(source)) == NULL)
		Error("DrawPic(): Couldn't cache pic!");

	srcrect.x = 0;
	srcrect.y = 0;
	srcrect.w = surf->w;
	srcrect.h = surf->h;

	dstrect.x = x;
	dstrect.y = y;
	dstrect.w = surf->w * scale;
	dstrect.h = surf->h * scale;

	SDL_SoftStretch(surf, &srcrect, BackSurface, &dstrect);
}

/*
 * free pic cache
 */

void ShutdownPicCache(void)
{
	int i;

	for (i = 0; i < num_PicCache; i++)
		SDL_FreeSurface(PicCache[i][1]);

	num_PicCache = 0;
	memset(PicCache, 0, sizeof(PicCache));
}

