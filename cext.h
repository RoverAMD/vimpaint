#ifndef CEXT_H
#define CEXT_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <ctype.h>

void VimPaintLog(const char* format, ...);

char* VimPaintFixPath(const char* path);
void VimPaintFixPathInplace(char* path);
bool VimPaintExists(const char* path);

char* VimPaintDetectFont(void);

#endif
