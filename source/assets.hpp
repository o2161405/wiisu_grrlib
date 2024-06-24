#pragma once

#include <grrlib.h>

#include "wiisu_ttf.h"
#include "Background_png.h"
#include "settings_png.h"
#include "wiisu_png.h"
#include "Background_Lines_Big_png.h"
#include "Background_Lines_Medium_png.h"
#include "Background_Lines_Small_png.h"
#include "Background_Circles_png.h"
#include "SongSelectBar_png.h"
#include "SongBackground_png.h"
#include "hitcircle_png.h"
#include "approachcircle_png.h"


static GRRLIB_ttfFont* Font = GRRLIB_LoadTTF(wiisu_ttf, wiisu_ttf_size);

static GRRLIB_texImg* Background = GRRLIB_LoadTexture(Background_png);
static GRRLIB_texImg* Background_Lines_Big = GRRLIB_LoadTexture(Background_Lines_Big_png);
GRRLIB_SetHandle(Background_Lines_Big, Background_Lines_Big->w / 2, Background_Lines_Big->h / 2);
static GRRLIB_texImg* Background_Lines_Medium = GRRLIB_LoadTexture(Background_Lines_Medium_png);
GRRLIB_SetHandle(Background_Lines_Medium, Background_Lines_Medium->w / 2, Background_Lines_Medium->h / 2);
static GRRLIB_texImg* Background_Lines_Small = GRRLIB_LoadTexture(Background_Lines_Small_png);
GRRLIB_SetHandle(Background_Lines_Small, Background_Lines_Small->w / 2, Background_Lines_Small->h / 2);
static GRRLIB_texImg* Background_Circles = GRRLIB_LoadTexture(Background_Circles_png);
GRRLIB_SetHandle(Background_Circles, Background_Circles->w / 2, Background_Circles->h / 2);

static GRRLIB_texImg* SettingsImage = GRRLIB_LoadTexture(settings_png);
static GRRLIB_texImg* Wiisu = GRRLIB_LoadTexture(wiisu_png);
GRRLIB_SetHandle(Wiisu, Wiisu->w / 2, Wiisu->h / 2);

static GRRLIB_texImg* SongSelectBar = GRRLIB_LoadTexture(SongSelectBar_png);

static GRRLIB_texImg* SongBackground = GRRLIB_LoadTexture(SongBackground_png);

static GRRLIB_texImg* hitcircle = GRRLIB_LoadTexture(hitcircle_png);
GRRLIB_SetHandle(hitcircle, hitcircle->w / 2, hitcircle->h / 2);
static GRRLIB_texImg* approachcircle = GRRLIB_LoadTexture(approachcircle_png);
GRRLIB_SetHandle(approachcircle, approachcircle->w / 2, approachcircle->h / 2);