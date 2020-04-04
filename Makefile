CC ?= gcc
DEFINES = -DDEBUG
CFLAGS = -I. $(DEFINES) -std=gnu11 -Wall -Wextra `sdl2-config --cflags`
LIBS = `sdl2-config --libs` -lSDL2_ttf -lm
OUTPUT = vimpaint
TARGETS = cext.o ui.o main.o 

all: $(TARGETS) $(OUTPUT)

$(OUTPUT): $(TARGETS)
	$(CC) -o $(OUTPUT) $(TARGETS) $(LIBS)

$(TARGETS):
	$(CC) -c -o $@ $(CFLAGS) `basename $@ .o`.c
	
clean distclean:
	rm -rf $(TARGETS) $(OUTPUT)
