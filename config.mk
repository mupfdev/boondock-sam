TOOLCHAIN=
#TOOLCHAIN=i686-w64-mingw32
CC=$(TOOLCHAIN)-cc
PROJECT=boondock-sam
OUT=$(PROJECT)
#OUT=$(PROJECT).exe
LIBS=\
	-lSDL2 -lpthread\
	-lSDL2_image\
	-lSDL2_mixer\
	-lxml2 -lz -llzma -lm
CFLAGS=\
	-D_REENTRANT\
	-DSDL_MAIN_HANDLED\
	-DWANT_ZLIB\
	-L/usr/$(TOOLCHAIN)/lib\
	-I/usr/$(TOOLCHAIN)/include\
	-I/usr/$(TOOLCHAIN)/include/libxml2\
	-O2\
	-pedantic-errors\
	-std=c99\
	-Wall\
	-Werror\
	-Wextra
EMSCRIPTEN=\
	--emrun \
	$(SRCS) \
	emscripten/lib/libxml2.bc \
	-DWANT_ZLIB \
	-I./emscripten/include \
	-Os -msse -msse2 \
	-s USE_SDL=2 \
	-s USE_SDL_IMAGE=2 \
	-s SDL2_IMAGE_FORMATS='["png"]' \
	-s USE_ZLIB=1 \
	--preload-file windowed.ini \
	--preload-file res \
	--shell-file emscripten/shell.html\
	-o emscripten/index.html
SRCS=\
	$(wildcard src/*.c)\
	$(wildcard src/tmx/*.c)\
	$(wildcard src/inih/*.c)
OBJS=$(patsubst %.c, %.o, $(SRCS))
