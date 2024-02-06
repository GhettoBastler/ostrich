build: clean
	gcc src/engine.c \
	src/camera.c \
	src/interpreter.c \
	src/primitives.c \
	src/render.c \
	src/transforms.c \
	src/ui.c \
	src/vect.c \
	src/utils.c \
	-lSDL2 -lm -o bin/ostrich

clean:
	rm -rf bin/
	mkdir bin

run: build
	./bin/ostrich

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
	-lSDL2 -lm -o bin/ostric_prof -pg

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
	-lSDL2 -lm -o bin/ostric_debug -g
