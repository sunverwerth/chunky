LDFLAGS=-ldl -lglfw -lGL -lglut
CC=g++

bin/triangle:
	$(CC) src/*.c* $(LDFLAGS) -o bin/triangle
	
all: bin/triangle

run: all
	bin/triangle