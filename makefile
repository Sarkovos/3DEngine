build:
	gcc -g -Wall -Werror -std=c99 ./src/*.c -lSDL2 -lm -o renderer

run:
	./renderer

debug:
	gdb ./renderer

clean:
	rm renderer