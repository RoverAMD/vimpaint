#include "ui.h"

static const SDL_Color VimPaintLocalWhiteConst = {255, 255, 255 ,255};
static const SDL_Color VimPaintLocalSystemConst = {51, 51, 51, 255};
static const SDL_Color VimPaintLocalBlackConst = {0, 0, 0, 0};

struct VimPaintUI_s {
    char* outputFilename;
    SDL_Surface* target;
    SDL_Surface* image;
    SDL_Color currentColor;
    int currentX;
    int currentY;
    bool doBorder;
    unsigned zoomCoefficent;
    bool changesMade;s
};

void VimPaintClearSurface(SDL_Surface* target, const SDL_Color color) {
    if (!target)
        VimPaintLog("NULL surface queried for cleaning");
    else {
        SDL_Renderer* rdr = SDL_CreateSoftwareRenderer(target);
        SDL_SetRenderDrawColor(rdr, color.r, color.g, color.b, color.a);
        SDL_RenderClear(rdr);
        SDL_RenderPresent(rdr);
        SDL_DestroyRenderer(rdr);
    }
}

bool VimPaintUISetZoomCoefficent(VimPaintUI* uiObj, const unsigned vl) {
    if (!uiObj)
        return false;
    else if (vl < 1)
        return VimPaintUISetZoomCoefficent(uiObj, 1);
    uiObj->zoomCoefficent = vl;
    return true;
}

void VimPaintUIInitializeInmemoryImage(VimPaintUI* uiObj, const int width, const int height) {
    VimPaintLog("uiObj = %p, width = %d, height = %d", uiObj, width, height);
    if (!uiObj || width < 2 || height < 1) {
        VimPaintLog("Invalid parameters");
        return;
    }
    if (uiObj->image)
        SDL_FreeSurface(uiObj->image);
    uiObj->image = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, SDL_PIXELFORMAT_RGBA32);
    VimPaintClearSurface(uiObj->image, VimPaintLocalWhiteConst);
    uiObj->currentX = 0;
    uiObj->currentY = 0;
}

bool VimPaintScaledBlit(SDL_Surface* inmemTarget, SDL_Surface* realTarget, const int centeredX, const int centeredY, const int relativeX, const int relativeY, const unsigned zoomCoefficent) {
    VimPaintLog("inmemTarget = %p, realTarget = %p, zoomCoefficent = %u", inmemTarget, realTarget, zoomCoefficent);
    if (!inmemTarget || !realTarget) {
        VimPaintLog("Invalid args");
        return false;
    }
    SDL_Surface* zoomedInput = SDL_CreateRGBSurfaceWithFormat(0, inmemTarget->w * zoomCoefficent, inmemTarget->h * zoomCoefficent, 32, SDL_PIXELFORMAT_RGBA32);
    SDL_BlitScaled(inmemTarget, NULL, zoomedInput, NULL);
    SDL_Rect rectWhere = {centeredX, centeredY, realTarget->w, realTarget->h};
    int daX = relativeX - 7;
    int daY = relativeY - 7;
    if (daX <= 0 || zoomCoefficent < 2)
        daX = 0;
    if (daY <= 0 || zoomCoefficent < 2)
        daY = 0;
    SDL_Rect rectFrom = {daX, daY, inmemTarget->w, inmemTarget->h};
    bool result = (SDL_BlitSurface(zoomedInput, &rectFrom, realTarget, &rectWhere) == 0);
    SDL_FreeSurface(zoomedInput);
    return result;
}

bool VimPaintUIBlit(const VimPaintUI* uiObj) {
    if (!uiObj) {
        VimPaintLog("NULL uiObj");
        return false;
    }
    SDL_Surface* inmemTarget = NULL;
    VimPaintClearSurface(uiObj->target, VimPaintLocalSystemConst);
    int centeredX = (uiObj->target->w / 2) - (uiObj->image->w / 2);
    int centeredY = (uiObj->target->h / 2) - (uiObj->image->h / 2);
    if (centeredX < 0)
        centeredX = 0;
    if (centeredY < 0)
        centeredY = 0;
    inmemTarget = SDL_CreateRGBSurfaceWithFormat(0, uiObj->image->w, uiObj->image->h, 32, SDL_PIXELFORMAT_RGBA32);
    SDL_BlitSurface(uiObj->image, NULL, inmemTarget, NULL);
    SDL_Renderer* tempRenderer = SDL_CreateSoftwareRenderer(inmemTarget);
    SDL_SetRenderDrawColor(tempRenderer, 20, 101, 242, 255);
    SDL_RenderDrawPoint(tempRenderer, uiObj->currentX, uiObj->currentY);
    SDL_SetRenderDrawColor(tempRenderer, 30, 108, 66, 255);
    if (uiObj->doBorder) {
        SDL_RenderDrawPoint(tempRenderer, uiObj->currentX - 1, uiObj->currentY);
        SDL_RenderDrawPoint(tempRenderer, uiObj->currentX + 1, uiObj->currentY);
        SDL_RenderDrawPoint(tempRenderer, uiObj->currentX - 1, uiObj->currentY + 1);
        SDL_RenderDrawPoint(tempRenderer, uiObj->currentX, uiObj->currentY + 1);
        SDL_RenderDrawPoint(tempRenderer, uiObj->currentX + 1, uiObj->currentY + 1);
        SDL_RenderDrawPoint(tempRenderer, uiObj->currentX - 1, uiObj->currentY - 1);
        SDL_RenderDrawPoint(tempRenderer, uiObj->currentX, uiObj->currentY - 1);
        SDL_RenderDrawPoint(tempRenderer, uiObj->currentX + 1, uiObj->currentY - 1);
    }
    SDL_RenderPresent(tempRenderer);
    SDL_DestroyRenderer(tempRenderer);
    bool result = VimPaintScaledBlit(inmemTarget, uiObj->target, centeredX, centeredY, uiObj->currentX, uiObj->currentY, uiObj->zoomCoefficent);
    SDL_FreeSurface(inmemTarget);
    return result;
}

VimPaintUI* VimPaintUICreate(SDL_Surface* target) {
    if (!target) {
        VimPaintLog("VimPaint UI creation with NULL surface requested");
        return NULL;
    }
    VimPaintUI* uiObj = malloc(sizeof(VimPaintUI));
    uiObj->outputFilename = NULL;
    uiObj->target = target;
    uiObj->image = NULL;
    uiObj->doBorder = true;
    uiObj->currentX = 0;
    uiObj->changesMade = false;
    uiObj->currentY = 0;
    uiObj->currentColor = VimPaintLocalBlackConst;
    uiObj->zoomCoefficent = 1;
    VimPaintClearSurface(target, VimPaintLocalWhiteConst);
    return uiObj;
}

bool VimPaintUIIncrementCursorPosition(VimPaintUI* uiObj, const int byX, const int byY) {
    if (!uiObj)
        return false;
    uiObj->currentX += byX;
    uiObj->currentY += byY;
    if (uiObj->currentX <= 0 || uiObj->currentX >= uiObj->image->w)
        uiObj->currentX = 0;
    if (uiObj->currentY <= 0 || uiObj->currentY >= uiObj->image->h)
        uiObj->currentY = 0;
    VimPaintUIBlit(uiObj);
    return true;
}

bool VimPaintUIToggleBorder(VimPaintUI* uiObj) {
    if (!uiObj)
        return false;
    uiObj->doBorder = !uiObj->doBorder;
    return true;
}

bool VimPaintUISetPixel(VimPaintUI* uiObj, const int piX, const int piY) {
    VimPaintLog("uiObj = %p, piX = %d, piY = %d", uiObj, piX, piY);
    if (!uiObj || piX < 0 || piY < 0) {
        VimPaintLog("Invalid args");
        return false;
    }
    SDL_Renderer* tempRenderer = SDL_CreateSoftwareRenderer(uiObj->image);
    SDL_SetRenderDrawColor(tempRenderer, uiObj->currentColor.r, uiObj->currentColor.g, uiObj->currentColor.b, uiObj->currentColor.a);
    SDL_RenderDrawPoint(tempRenderer, piX, piY);
    SDL_DestroyRenderer(tempRenderer);
    uiObj->changesMade = true;
    return true;
}

bool VimPaintUISetCurrentPixel(VimPaintUI* uiObj) {
    if (!uiObj)
        return false;
    return VimPaintUISetPixel(uiObj, uiObj->currentX, uiObj->currentY);
}

bool VimPaintUIHaveUnsavedChanges(const VimPaintUI* uiObj) {
    if (!uiObj)
        return false;
    return uiObj->changesMade;
}

const char* VimPaintUIGetFilename(const VimPaintUI* uiObj) {
    if (!uiObj)
        return NULL;
    return uiObj->outputFilename;
}

bool VimPaintUISetFilename(VimPaintUI* uiObj, const char* fn) {
    if (!uiObj)
        return false;
    free(uiObj->outputFilename);
    uiObj->outputFilename = VimPaintFixPath(fn);
    return true;
}

bool VimPaintUISave(VimPaintUI* uiObj) {
    if (!uiObj || !uiObj->outputFilename)
        return false;
    bool result = (SDL_SaveBMP(uiObj->image, uiObj->outputFilename) == 0);
    uiObj->changesMade = !result;
    return result;
}s

void VimPaintRenderTextCentered(SDL_Surface* target, TTF_Font* font, const char* text, const int requiredY, const SDL_Color color) {
    if (!font || !target || !text || requiredY < 0) {
        VimPaintLog("Centered text renderer function specified with one of the invalid arguments: target = %p, font = %p, text = \"%s\", requiredY = %d", target, font, text, requiredY);
        return;
    }
    int width, height = 0;
    TTF_SizeUTF8(font, text, &width, &height);
    int x = (target->w / 2) - (width / 2);
    if (x < 0)
        x = 0;
    SDL_Surface* rdrText = TTF_RenderUTF8_Solid(font, text, color);
    if (!rdrText) {
        VimPaintLog(TTF_GetError());
        return;
    }
    SDL_Rect where = {x, requiredY, width, height};
    SDL_BlitSurface(rdrText, NULL, target, &where);
    SDL_FreeSurface(rdrText);
}

void VimPaintUIDisplayWelcomeText(const VimPaintUI* uiObj) {
    if (!uiObj)
        VimPaintLog("VimPaint menu print attempt with uiObj = NULL");
    else {
        VimPaintClearSurface(uiObj->target, VimPaintLocalSystemConst);
        const int fontSize = 13;
        char* fontPath = VimPaintDetectFont();
        if (!fontPath) {
            VimPaintLog("VimPaint cannot detect the font");
            return;
        }
        VimPaintFixPathInplace(fontPath);
        TTF_Font* font = TTF_OpenFont(fontPath, fontSize);
        if (!font) {
            VimPaintLog("Failed to open font: %s (font path = %s)", TTF_GetError(), fontPath);
            free(fontPath);
            return;
        }
        free(fontPath);
        int base = (uiObj->target->h / 2) - (fontSize * 2);
        if (base < 0)
            base = 0;
        SDL_Color welcomeTextColor = VimPaintLocalWhiteConst;
        VimPaintRenderTextCentered(uiObj->target, font, VPL_STR_WELCOME, base, welcomeTextColor);
        base += fontSize + 10;
        VimPaintRenderTextCentered(uiObj->target, font, VPL_STR_INSTRUCTIONCANVAS, base, welcomeTextColor);
        base += fontSize + 2;
        VimPaintRenderTextCentered(uiObj->target, font, VPL_STR_INSTRUCTIONQUIT, base, welcomeTextColor);
        TTF_CloseFont(font);
    }
}

void VimPaintUIRelease(VimPaintUI* uiObj) {
    if (!uiObj)
        return;
    SDL_FreeSurface(uiObj->image);
    free(uiObj->outputFilename);
    free(uiObj);
}
