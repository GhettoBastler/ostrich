engine: clean
	gcc src/engine.c -lSDL2 -lm -o bin/engine
clean:
	rm -rf bin/
	mkdir bin

run: engine
	./bin/engine
