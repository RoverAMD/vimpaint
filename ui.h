#ifndef UI_H
#define UI_H

#include "cext.h"
#include "localization.h"
#include <SDL.h>
#include <SDL_ttf.h>

struct VimPaintUI_s;
typedef struct VimPaintUI_s VimPaintUI;

VimPaintUI* VimPaintUICreate(SDL_Surface* target);
void VimPaintUIDisplayWelcomeText(const VimPaintUI* uiObj);
void VimPaintUIInitializeInmemoryImage(VimPaintUI* uiObj, const int width, const int height);
bool VimPaintUIBlit(const VimPaintUI* uiObj);
void VimPaintUIRelease(VimPaintUI* uiObj);

#endif
