#include "ui.h"

enum VimPaintMenuSelection_e { VPM_NULL = 0, VPM_QUIT = 1, VPM_CREATE= 2 };
typedef enum VimPaintMenuSelection_e VimPaintMenuSelection;

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

void VimPaintAdequateEventLoop(SDL_Window* wnd, VimPaintUI* uiObj) {
    VimPaintUIBlit(uiObj);
    SDL_UpdateWindowSurface(wnd);
    SDL_Event* ev = malloc(sizeof(SDL_Event));
    while (SDL_WaitEvent(ev) != 0) {
        if (ev->type == SDL_QUIT) {
            free(ev);
            return;
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
