.PHONY: all emscripten clean

include config.mk

all: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $(OUT)

%: %.c
	$(CC) -c $(CFLAGS) $(LIBS) -o $@ $<

emscripten:
	emcc \
	$(EMSCRIPTEN)

clean:
	rm -f $(OBJS)
	rm -f $(OUT)
	rm -f emscripten/index.*
