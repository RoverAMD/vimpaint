#include "cext.h"

void VimPaintLog(const char* format, ...) {
    if (!format)
        return;
#ifdef DEBUG
    va_list vl;
    va_start(vl, format);
    vfprintf(stderr, format, vl);
    fprintf(stderr, "%c", '\n');
    va_end(vl);
#endif
    return;
}

char* VimPaintFixPath(const char* path) {
    if (!path)
        return NULL;
    char* buf = strdup(path);
    VimPaintFixPathInplace(buf);
    return buf;
}

void VimPaintFixPathInplace(char* path) {
    if (!path)
        return;
#ifdef _WIN32
    const char systemDelimiter = '\\';
#else
    const char systemDelimiter = '/';
#endif
    for (unsigned i = 0; i < strlen(path); i++) {
        if (path[i] == '/' || path[i] == '\\')
            path[i] = systemDelimiter;
    }
}

bool VimPaintExists(const char* path) {
    if (!path)
        return false;
    char* realPath = VimPaintFixPath(path);
    bool result = (access(realPath, F_OK) == 0);
    free(realPath);
    return result;
}

char* VimPaintDetectFont() {
    if (getenv("VIMPAINT_FONTPATH"))
        return strdup(getenv("VIMPAINT_FONTPATH"));
#ifdef __linux__
    char* potentialPath1 = strdup("/usr/share/fonts/truetype/SourceCodePro-Regular.ttf");
    if (VimPaintExists(potentialPath1))
        return potentialPath1;
    return NULL;
#endif
    return NULL;
}
