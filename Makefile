.PHONY: all emscripten clean

include config.mk

all: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $(PROJECT)

%: %.c
	$(CC) -c $(CFLAGS) $(LIBS) -o $@ $<

# Emscripten test.
emscripten:
	emcc \
	--emrun \
	$(SRCS) \
	-O2 -msse -msse2 \
	-s USE_SDL=2 \
	-s USE_SDL_IMAGE=2 \
	-s SDL2_IMAGE_FORMATS='["png"]' \
	--preload-file res \
	-o html/index.html

clean:
	rm $(OBJS)
	rm $(PROJECT)
