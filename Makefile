engine: clean
	gcc src/engine.c \
	src/camera.c \
	src/interpreter.c \
	src/primitives.c \
	src/render.c \
	src/transforms.c \
	src/ui.c \
	src/vect.c \
	src/utils.c \
	-lSDL2 -lm -o bin/engine
clean:
	rm -rf bin/
	mkdir bin

run: engine
	./bin/engine

profiling: clean
	gcc src/engine.c \
	src/camera.c \
	src/interpreter.c \
	src/primitives.c \
	src/render.c \
	src/transforms.c \
	src/ui.c \
	src/vect.c \
	src/utils.c \
	-lSDL2 -lm -o bin/engine_prof -pg

debug: clean
	gcc src/engine.c \
	src/camera.c \
	src/interpreter.c \
	src/primitives.c \
	src/render.c \
	src/transforms.c \
	src/ui.c \
	src/vect.c \
	src/utils.c \
	-lSDL2 -lm -o bin/engine_debug -g
