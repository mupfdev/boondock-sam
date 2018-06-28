.PHONY: all emscripten clean

include config.mk

all: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $(OUT)

%: %.c
	$(CC) -c $(CFLAGS) $(LIBS) -o $@ $<

emscripten:
	emcc \
	$(SRCS) \
	-Os -msse -msse2 \
	-s USE_SDL=2 \
	-s USE_SDL_IMAGE=2 \
	-s SDL2_IMAGE_FORMATS='["png"]' \
	--preload-file default.ini \
	--preload-file res \
	--separate-asm \
	--shell-file emscripten/shell.html \
	-o emscripten/index.html \

clean:
	rm $(OBJS)
	rm $(PROJECT)
