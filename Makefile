main: main.c game.c 
	gcc main.c game.c -o main -lraylib -lm -ggdb

web: main.c game.c 
	emcc -o build/index.html \
		main.c game.c \
		-Os -Wall \
		/home/taylor/vendor/raylib/src/libraylib.web.a \
		-I/home/taylor/vendor/raylib/src \
		-L/home/taylor/vendor/raylib/src/libraylib.web.a \
		-s USE_GLFW=3 \
		-s ASYNCIFY \
		--preload-file ./shaders/triangle.fs \
		--preload-file ./shaders/cube.fs \
		--shell-file ./shell.html \
		-DPLATFORM_WEB
clean:
	rm -f main

run:
	emrun build/index.html
