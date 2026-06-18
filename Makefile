main: main.c game.c 
	gcc main.c game.c -o main -lraylib -lm -ggdb

web: main.c game.c 
	emcc -o game.html main.c game.c -Os -Wall ./libraylib.a -I. -I./raylib.h -L. -L./libraylib.a -s USE_GLFW=3 --preload-file ./shaders/triangle.fs --preload-file ./shaders/cube.fs --shell-file minshell.html -DPLATFORM_WEB

clean:
	rm -f main
