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
bool VimPaintUILoadImage(VimPaintUI* uiObj, const char* path);
bool VimPaintUIBlit(const VimPaintUI* uiObj);
bool VimPaintUIHaveUnsavedChanges(const VimPaintUI* uiObj);
const char* VimPaintUIGetFilename(const VimPaintUI* uiObj);
bool VimPaintUISetFilename(VimPaintUI* uiObj, const char* fn);
bool VimPaintUISave(VimPaintUI* uiObj);
bool VimPaintUISetZoomCoefficent(VimPaintUI* uiObj, const unsigned vl);
bool VimPaintUIIncrementCursorPosition(VimPaintUI* uiObj, const int byX, const int byY);
bool VimPaintUISetPixel(VimPaintUI* uiObj, const int piX, const int piY);
bool VimPaintUISetCurrentPixel(VimPaintUI* uiObj);
bool VimPaintUIToggleBorder(VimPaintUI* uiObj);
void VimPaintUIRelease(VimPaintUI* uiObj);

#endif
