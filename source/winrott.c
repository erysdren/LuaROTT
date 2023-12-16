/* Routines from winrott needed for the highres support for the SDL port */
#include <stdlib.h>
#include <string.h>
#include "WinRott.h"
#include "modexlib.h"

// typedef unsigned char byte;

int iGLOBAL_SCREENBWIDE;
int iG_SCREENWIDTH; // default screen width in bytes

int iGLOBAL_HEALTH_X;
int iGLOBAL_HEALTH_Y;
int iGLOBAL_AMMO_X;
int iGLOBAL_AMMO_Y;

int iGLOBAL_FOCALWIDTH;
double dGLOBAL_FPFOCALWIDTH;

double dTopYZANGLELIMIT;

int iG_X_center;
int iG_Y_center;

boolean iG_aimCross = 0;

extern int viewheight;
extern int viewwidth;

//----------------------------------------------------------------------
#define FINEANGLES 2048
void SetRottScreenRes(int Width, int Height)
{
	vidconfig.ScreenWidth = Width;
	vidconfig.ScreenHeight = Height;

	iGLOBAL_SCREENBWIDE = vidconfig.ScreenWidth * (96 / 320);
	iG_SCREENWIDTH = vidconfig.ScreenWidth * (96 / 320); // default screen width in bytes

	/* calculate based on screen resolution instead of hard-coding */
	iGLOBAL_HEALTH_X = 20;
	iGLOBAL_HEALTH_Y = vidconfig.ScreenHeight - 15;
	iGLOBAL_AMMO_X = vidconfig.ScreenWidth - 20;
	iGLOBAL_AMMO_Y = vidconfig.ScreenHeight - 15;

	if (vidconfig.ScreenWidth == 320)
	{
		iGLOBAL_FOCALWIDTH = 160;
		dGLOBAL_FPFOCALWIDTH = 160.0;

		dTopYZANGLELIMIT = (44 * FINEANGLES / 360);
	}
	if (vidconfig.ScreenWidth == 640)
	{
		iGLOBAL_FOCALWIDTH = 180;
		dGLOBAL_FPFOCALWIDTH = 180.0;

		dTopYZANGLELIMIT = (42 * FINEANGLES / 360);
	}
}

//----------------------------------------------------------------------
// luckey for me that I am not programmin a 386 or the next
// 4 function would never have worked. bna++
extern int viewsize;
void MoveScreenUpLeft()
{
	int startX, startY, startoffset;
	byte *Ycnt, *b;
	//   SetTextMode (  );
	b = (byte *)bufferofs;
	b += (((vidconfig.ScreenHeight - viewheight) / 2) * vidconfig.ScreenWidth) +
		 ((vidconfig.ScreenWidth - viewwidth) / 2);
	if (viewsize == 8)
	{
		b += 8 * vidconfig.ScreenWidth;
	}
	startX = 3; // take 3 pixels to the right
	startY = 3; // take 3 lines down
	startoffset = (startY * vidconfig.ScreenWidth) + startX;

	for (Ycnt = b; Ycnt < b + ((viewheight - startY) * vidconfig.ScreenWidth);
		 Ycnt += vidconfig.ScreenWidth)
	{
		memcpy(Ycnt, Ycnt + startoffset, viewwidth - startX);
	}
}
//----------------------------------------------------------------------
void MoveScreenDownLeft()
{
	int startX, startY, startoffset;
	byte *Ycnt, *b;
	//   SetTextMode (  );
	b = (byte *)bufferofs;
	b += (((vidconfig.ScreenHeight - viewheight) / 2) * vidconfig.ScreenWidth) +
		 ((vidconfig.ScreenWidth - viewwidth) / 2);
	if (viewsize == 8)
	{
		b += 8 * vidconfig.ScreenWidth;
	}
	startX = 3;									  // take 3 pixels to the right
	startY = 3;									  // take 3 lines down
	startoffset = (startY * vidconfig.ScreenWidth); //+startX;

	// Ycnt starts in botton of screen and copys lines upwards
	for (Ycnt = b + ((viewheight - startY - 1) * vidconfig.ScreenWidth); Ycnt > b;
		 Ycnt -= vidconfig.ScreenWidth)
	{
		memcpy(Ycnt + startoffset, Ycnt + startX, viewwidth - startX);
	}
}
//----------------------------------------------------------------------
void MoveScreenUpRight()
{
	int startX, startY, startoffset;
	byte *Ycnt, *b;
	//   SetTextMode (  );
	b = (byte *)bufferofs;

	b += (((vidconfig.ScreenHeight - viewheight) / 2) * vidconfig.ScreenWidth) +
		 ((vidconfig.ScreenWidth - viewwidth) / 2);
	if (viewsize == 8)
	{
		b += 8 * vidconfig.ScreenWidth;
	}
	startX = 3;									  // take 3 pixels to the right
	startY = 3;									  // take 3 lines down
	startoffset = (startY * vidconfig.ScreenWidth); //+startX;

	for (Ycnt = b; Ycnt < b + ((viewheight - startY) * vidconfig.ScreenWidth);
		 Ycnt += vidconfig.ScreenWidth)
	{
		memcpy(Ycnt + startX, Ycnt + startoffset, viewwidth - startX);
	}
}
//----------------------------------------------------------------------
void MoveScreenDownRight()
{
	int startX, startY, startoffset;
	byte *Ycnt, *b;
	//   SetTextMode (  );
	b = (byte *)bufferofs;

	b += (((vidconfig.ScreenHeight - viewheight) / 2) * vidconfig.ScreenWidth) +
		 ((vidconfig.ScreenWidth - viewwidth) / 2);
	if (viewsize == 8)
	{
		b += 8 * vidconfig.ScreenWidth;
	}
	startX = 3; // take 3 pixels to the right
	startY = 3; // take 3 lines down
	startoffset = (startY * vidconfig.ScreenWidth) + startX;

	// Ycnt starts in botton of screen and copys lines upwards
	for (Ycnt = b + ((viewheight - startY - 1) * vidconfig.ScreenWidth); Ycnt > b;
		 Ycnt -= vidconfig.ScreenWidth)
	{
		memcpy(Ycnt + startoffset, Ycnt, viewwidth - startX);
	}
}
