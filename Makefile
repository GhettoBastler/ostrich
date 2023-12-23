engine: clean
	gcc src/*.c -lSDL2 -lm -o bin/engine
clean:
	rm -rf bin/
	mkdir bin

run: engine
	./bin/engine

profiling: clean
	gcc src/*.c -lSDL2 -lm -o bin/engine_prof -pg

polygon_test: clean
	gcc src/polygon_test.c src/primitives.c src/vect.c -lm -o bin/polygon_test
