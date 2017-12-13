LDFLAGS=--std=c++11 -ldl -lglfw -lGL -lglut
CC=g++

bin/triangle: $(wildcard, src/*.cpp);
	$(CC) src/*.cpp $(LDFLAGS) -o bin/triangle
	
all: bin/triangle

run: all
	bin/triangle

clean:
	rm bin/*