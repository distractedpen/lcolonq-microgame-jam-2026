RAYLIBWEB=./vendor/libraylib.web.a
RAYLIBSRC=./vendor/raylib/src
SHELLFILE=./vendor/raylib/src/minshell.html

main: main.c game.c 
	emcc -o build/index.html \
		main.c game.c \
		$(RAYLIBWEB) \
		-Os -Wall \
		-I$(RAYLIBSRC) \
		-L$(RAYLIBWEB) \
		-s USE_GLFW=3 \
		-s ASYNCIFY \
		-s ALLOW_MEMORY_GROWTH \
		--preload-file ./shaders/triangle.fs \
		--preload-file ./shaders/cube.fs \
		--preload-file ./sounds/block.ogg \
		--preload-file ./sounds/hit.ogg \
		--preload-file ./sounds/win.ogg \
		--preload-file ./sounds/lose.ogg \
		--shell-file $(SHELLFILE) \
		-DPLATFORM_WEB
clean:
	rm -f main

run:
	emrun build/index.html
