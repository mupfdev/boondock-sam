PROJECT=boondock-sam

ifeq ($(OS),Windows_NT)
	OUT=$(PROJECT).exe
	TOOLCHAIN=i686-w64-mingw32
	CC=$(TOOLCHAIN)-cc
else
	OUT=$(PROJECT)
	TOOLCHAIN=local
	UNAME_S := $(shell uname -s)
endif

LIBS=\
	-lpthread\
	-lSDL2\
	-lSDL2_image\
	-lSDL2_mixer\
	-lxml2 -lz -llzma -licuuc -lm

CFLAGS=\
	-D_REENTRANT\
	-DSDL_MAIN_HANDLED\
	-DWANT_ZLIB\
	-isystem /usr/$(TOOLCHAIN)/include/libxml2\
	-O2\
	-pedantic-errors\
	-std=c99\
	-Wall\
	-Werror\
	-Wextra

SRCS=\
	$(wildcard src/*.c)\
	$(wildcard src/tmx/*.c)\
	$(wildcard src/inih/*.c)

OBJS=$(patsubst %.c, %.o, $(SRCS))
