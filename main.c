#include "ui.h"

enum VimPaintMenuSelection_e { VPM_NULL = 0, VPM_QUIT = 1, VPM_CREATE= 2 };
typedef enum VimPaintMenuSelection_e VimPaintMenuSelection;

enum VimPaintAdditionalTrigger_e { VPT_NULL = 0, VPT_CARECHANGES = 1, VPT_DONTCARECHANGES = 2};
typedef enum VimPaintAdditionalTrigger_e VimPaintAdditionalTrigger;

#define __drpattern \
                { \
                    finalNumber = defaultValue; \
                    TTF_CloseFont(fontPtr); \
                    free(ev); \
                    return defaultValue; \
                }

int VimPaintGetNumericInput(SDL_Window* wnd, const char* text, const int defaultValue, char** invalidlyPressedKeysPtr) {
    VimPaintLog("wnd = %p, text = \"%s\", defaultValue = %d\n", wnd, text, defaultValue);
    if (!wnd || !text)
        return defaultValue;
    SDL_Surface* tgt = SDL_GetWindowSurface(wnd);
    char* defaultFont = VimPaintDetectFont();
    VimPaintFixPathInplace(defaultFont);
    if (!defaultFont)
        return defaultValue;
    TTF_Font* fontPtr = TTF_OpenFont(defaultFont, 14);
    free(defaultFont);
    if (!fontPtr) {
        VimPaintLog(TTF_GetError());
        return defaultValue;
    }
    int width, height = 0;
    TTF_SizeUTF8(fontPtr, text, &width, &height);
    if (width >= tgt->w)
        width = tgt->w;
    SDL_Color boxColor = {86, 76, 76, 255};
    SDL_Rect rt = {0, 0, width, height};
    SDL_FillRect(tgt, &rt, SDL_MapRGBA(tgt->format, boxColor.r, boxColor.g, boxColor.b, boxColor.a));
    SDL_Color fontColor = {255, 255, 255, 255};
    SDL_Surface* sfc = TTF_RenderUTF8_Solid(fontPtr, text, fontColor);
    if (!sfc) {
        VimPaintLog(TTF_GetError());
        TTF_CloseFont(fontPtr);
        return defaultValue;
    }
    SDL_BlitSurface(sfc, NULL, tgt, &rt);
    SDL_UpdateWindowSurface(wnd);
    SDL_FreeSurface(sfc);
    int finalNumber = defaultValue;
    unsigned limit = 11;
    char* finalNumberRaw = calloc(limit, sizeof(char));
    char* lastKeyBuf = NULL;
    SDL_Event* ev = malloc(sizeof(SDL_Event));
    while (SDL_WaitEvent(ev) != 0) {
        if (ev->type == SDL_QUIT) {
            __drpattern
        } else if (ev->type == SDL_KEYDOWN) {
            SDL_Keycode kk = ev->key.keysym.sym;
            const char* kn = SDL_GetKeyName(kk);
            free(lastKeyBuf);
            lastKeyBuf = strdup(kn);
            if (strcmp(kn, "Escape") == 0) {
                __drpattern
            } else if (strcmp(kn, "Return") == 0) {
                if (strlen(finalNumberRaw) < 1)
                    __drpattern
                free(ev);
                break;
            } else if (strcmp(kn, "Backspace") == 0) {
                finalNumberRaw[strlen(finalNumberRaw) - 1] = '\0';
                finalNumber = atoi(finalNumberRaw);
            } else if (strlen(kn) >= 1 && isdigit(kn[0]) != 0) {
                finalNumberRaw[strlen(finalNumberRaw)] = kn[0];
                finalNumber = atoi(finalNumberRaw);
            } else
                __drpattern
            SDL_FillRect(tgt, &rt, SDL_MapRGBA(tgt->format, boxColor.r, boxColor.g, boxColor.b, boxColor.a));
            SDL_Surface* sfcTmp = TTF_RenderUTF8_Solid(fontPtr, finalNumberRaw, fontColor);
            SDL_BlitSurface(sfcTmp, NULL, tgt, &rt);
            SDL_FreeSurface(sfcTmp);
            SDL_UpdateWindowSurface(wnd);
        }
        free(ev);
        if (strlen(finalNumberRaw) >= (limit - 1))
            break;
        ev = malloc(sizeof(SDL_Event));
    }
    free(finalNumberRaw);
    if (invalidlyPressedKeysPtr) 
        (*invalidlyPressedKeysPtr) = lastKeyBuf;
    else
        free(lastKeyBuf);
    return finalNumber;
}

#undef __drpattern

void VimPaintDisplayMessage(SDL_Window* wnd, const char* text, const bool isError) {
    if (!wnd || !text || strlen(text) >= 255)
        return;
    SDL_Surface* tgt = SDL_GetWindowSurface(wnd);
    char* fontPath = VimPaintDetectFont();
    if (!fontPath)
        return;
    VimPaintFixPathInplace(fontPath);
    TTF_Font* fnt = TTF_OpenFont(fontPath, 13);
    if (!fnt) {
        VimPaintLog(TTF_GetError());
        free(fontPath);
        return;
    }
    free(fontPath);
    SDL_Color bgColor = {74, 152, 85, 255};
    if (isError) {
        bgColor.r = 152;
        bgColor.g = 74;
        bgColor.b = 113;
    }
    int widthOfText, heightOfText = 0;
    TTF_SizeUTF8(fnt, text, &widthOfText, &heightOfText);
    SDL_Surface* errorSurface = SDL_CreateRGBSurfaceWithFormat(0, widthOfText + 10, heightOfText + 40, 32, SDL_PIXELFORMAT_RGBA32);
    SDL_FillRect(errorSurface, NULL, SDL_MapRGBA(bgColor.r, bgColor.g, bgColor.b, bgColor.a));
    SDL_Surface* rdrText = TTF_RenderUTF8_Solid(fnt, text);
    if (!rdrText) {
        SDL_FreeSurface(errorSurface);
        TTF_CloseFont(fnt);
        return;
    }
    SDL_Rect textBounds = {4, 6, widthOfText, heightOfText};
    SDL_BlitSurface(rdrText, NULL, tgt, &textBounds);
    SDL_FreeSurface(rdrText);
    rdrText = TTF_RenderUTF8_Solid(fnt, "[OK]");
    int okWidth = (rdrText != NULL) ? rdrText->w : 0;
    int okHeight = (rdrText != NULL) ? rdrText->h : 0;
    textBounds = {(widthOfText + 10) / 2 - okWidth / 2, heightOfText + 35 - okHeight, okWidth, okHeight};
    SDL_BlitSurface(rdrText, NULL, errorSurface, &textBounds);
    TTF_CloseFont(fnt);
    SDL_FreeSurface(rdrText);
    int centeredX = (tgt->w / 2) - (errorSurface->w / 2);
    int centeredY = (tgt->h / 2) - (errorSurface->h / 2);
    SDL_Rect targetBlitRect = {centeredX, centeredY, errorSurface->w, errorSurface->h};
    SDL_BlitSurface(errorSurface, NULL, tgt, &targetBlitRect);
    SDL_FreeSurface(errorSurface);
    SDL_UpdateWindowSurface(wnd);
    SDL_Event* evTmp = malloc(sizeof(SDL_Event));
    while (SDL_WaitEvent(evTmp) != 0) {
        if (evTmp->type == SDL_QUIT)
            break;
        else if (evTmp->type == SDL_KEYDOWN) {
            const char* keyName = SDL_GetKeyName(evTmp->key.keysym.sym);
            if (strcmp(keyName, "Escape") == 0 || strcmp(keyName, "Space") == 0 || strcmp(keyName, "Return") == 0 || strcmp(keyName, "Backspace") == 0)
                break;
        }
        free(evTmp);
        evTmp = malloc(sizeof(SDL_Event));
    }
    free(evTmp);
}

VimPaintMenuSelection VimPaintStartMenuEventLoop(void) {
    SDL_Event* ev = malloc(sizeof(SDL_Event));
    VimPaintMenuSelection result = VPM_NULL;
    while (SDL_WaitEvent(ev) != 0) {
        if (ev->type == SDL_QUIT)
            result = VPM_QUIT;
        else if (ev->type == SDL_KEYDOWN) {
            SDL_Keycode pressedKey = ev->key.keysym.sym;
            const char* keyName = SDL_GetKeyName(pressedKey);
            VimPaintLog("User pressed: %s", keyName);
            if (strcmp(keyName, "Q") == 0 || strcmp(keyName, "Escape") == 0)
                result = VPM_QUIT;
            else if (strcmp(keyName, "S") == 0)
                result = VPM_CREATE;
        }
        free(ev);
        if (result != VPM_NULL)
            break;
        ev = malloc(sizeof(SDL_Event));
    }
    return result;
}

bool VimPaintCreateCanvasProcess(SDL_Window* associatedWindow, int* width, int* height) {
    if (!width || !height || !associatedWindow)
        return false;
    SDL_Surface* ts = SDL_GetWindowSurface(associatedWindow);
    int widthRaw = VimPaintGetNumericInput(associatedWindow, VPL_STR_CCONFWIDTH, ts->w, NULL);
    int heightRaw = VimPaintGetNumericInput(associatedWindow, VPL_STR_CCONFHEIGHT, ts->h, NULL);
    if (widthRaw < 2)
        widthRaw = 2;
    if (heightRaw < 1)
        heightRaw = 1;
    (*width) = widthRaw;
    (*height) = heightRaw;
    return true;
}

#define __drerrorout(msg) { \
                    free(userInputKeyID); \
                    VimPaintDisplayMessage(wnd, msg, true); \
                    return; }
                    
bool VimPaintAttemptSave(SDL_Window* wnd, VimPaintUI* uiObj) {
    if (!wnd || !uiObj)
        return false;
    
}

void VimPaintMenuEventLoop(SDL_Window* wnd, VimPaintUI* uiObj, const VimPaintAdditionalTrigger trigger) {
    char* userInputKeyID = NULL;
    VimPaintGetNumericInput(wnd, "What action?", -1, &userInputKeyID);
    if (!userInputKeyID || strcmp(userInputKeyID, "Escape") == 0) {
        free(userInputKeyID);
        VimPaintUIBlit(uiObj);
        SDL_UpdateWindowSurface(wnd);
        return;
    }
    bool ignoringChanges = (trigger == VPT_DONTCARECHANGES);
    if (strcmp(userInputKeyID, "!") == 0) {
        VimPaintMenuEventLoop(wnd, uiObj, VPT_DONTCARECHANGES);
        return;
    } else if (strcmp(userInputKeyID, "W") == 0) {
        VimPaintMenuEventLoop(wnd, uiObj, VPT_CARECHANGES);
        return;
    }
    if (strcmp(userInputKeyID, "Q") == 0) {
        if (VimPaintUIHaveUnsavedChanges(uiObj) && trigger == VPT_NULL)
            __drerrorout(VPL_STR_THEREAREUCHANGES);
        else if (trigger == VPT_CARECHANGES)
        VimPaintUIRelease(uiObj);
        SDL_DestroyWindow(wnd);
        TTF_Quit();
        SDL_Quit();
        exit(0);
    }
}

#undef __drerrorout

void VimPaintAdequateEventLoop(SDL_Window* wnd, VimPaintUI* uiObj) {
    VimPaintUIBlit(uiObj);
    SDL_UpdateWindowSurface(wnd);
    SDL_Event* ev = malloc(sizeof(SDL_Event));
    unsigned cachedZoom = 1;
    while (SDL_WaitEvent(ev) != 0) {
        if (ev->type == SDL_QUIT) {
            free(ev);
            return;
        } else if (ev->type == SDL_KEYDOWN) {
            const char* kn = SDL_GetKeyName(ev->key.keysym.sym);
            if (strcmp(kn, "Left") == 0)
                VimPaintUIIncrementCursorPosition(uiObj, -1, 0);
            else if (strcmp(kn, "Right") == 0)
                VimPaintUIIncrementCursorPosition(uiObj, 1, 0);
            else if (strcmp(kn, "Up") == 0)
                VimPaintUIIncrementCursorPosition(uiObj, 0, -1);
            else if (strcmp(kn, "Down") == 0)
                VimPaintUIIncrementCursorPosition(uiObj, 0, 1);
            else if (strcmp(kn, "B") == 0)
                VimPaintUIToggleBorder(uiObj);
            else if (strcmp(kn, "P") == 0 || strcmp(kn, "Space") == 0)
                VimPaintUISetCurrentPixel(uiObj);
            else if (strcmp(kn, "+") == 0 || strcmp(kn, "=") == 0) {
                if (cachedZoom <= 1)
                    cachedZoom *= 2;
                VimPaintUISetZoomCoefficent(uiObj, cachedZoom);
            else if (strcmp(kn, ":") == 0 || strcmp(kn, "Escape") == 0)
                VimPaintMenuEventLoop(wnd, uiObj, false);
            } else if (strcmp(kn, "-") == 0 || strcmp(kn, "_") == 0) {
                if (cachedZoom >= 2)
                    cachedZoom /= 2;
                VimPaintUISetZoomCoefficent(uiObj, cachedZoom);
            } else if (strcmp(kn, "Return") == 0) {
                VimPaintUISetCurrentPixel(uiObj);
                VimPaintUIIncrementCursorPosition(uiObj, 0, 1);
            }
            VimPaintUIBlit(uiObj);
            SDL_UpdateWindowSurface(wnd);
        }
        free(ev);
        ev = malloc(sizeof(SDL_Event));
    }
}

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return 1;
    }
    if (TTF_Init() != 0) {
        SDL_Quit();
        fprintf(stderr, "TTF_Init failed: %s\n", TTF_GetError());
        return 2;
    }
    SDL_Window* wnd = SDL_CreateWindow("VimPaint", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
    VimPaintUI* uiObj = VimPaintUICreate(SDL_GetWindowSurface(wnd));
    VimPaintUIDisplayWelcomeText(uiObj);
    SDL_UpdateWindowSurface(wnd);
    SDL_RaiseWindow(wnd);
    if (VimPaintStartMenuEventLoop() == VPM_QUIT) {
        VimPaintUIRelease(uiObj);
        TTF_Quit();
        SDL_Quit();
        return 0;
    } else {
        int canvasWidth, canvasHeight = 0;
        VimPaintCreateCanvasProcess(wnd, &canvasWidth, &canvasHeight);
        VimPaintUIInitializeInmemoryImage(uiObj, canvasWidth, canvasHeight);
    }
    VimPaintAdequateEventLoop(wnd, uiObj);
    return 0;
}
