LFLAGS	=	-lX11 -lGL -lGLU -lm -lpthread ./lib/*.o ./lib/*.a ./lib/*.so
CFLAGS	=	-Os -ggdb -Wall -Wextra -Wsign-conversion --std=c++11
IFLAGS	=	-I./ -I./inc
GPP	=	g++ $(IFLAGS) $(CFLAGS)

all: clean capture itoa loc line ppm convert iptocoord vectors setup project

itoa: ./src/itoa.cpp ./inc/itoa.h
	$(GPP) -o ./lib/itoa.o -c ./src/itoa.cpp

capture: ./src/capture.cpp ./inc/capture.h
	$(GPP) -o ./lib/capture.o -c ./src/capture.cpp

loc: ./src/loc.cpp ./inc/loc.h
	$(GPP) -o ./lib/loc.o -c ./src/loc.cpp

line: ./src/line.cpp ./inc/line.h
	$(GPP) -o ./lib/line.o -c ./src/line.cpp

ppm: ./src/ppm.cpp ./inc/ppm.h
	$(GPP) -o ./lib/ppm.o -c ./src/ppm.cpp

convert: ./src/convert.cpp ./inc/convert.h
	$(GPP) -o ./lib/convert.o -c ./src/convert.cpp

iptocoord: ./src/iptocoord.cpp ./inc/iptocoord.h
	$(GPP) -o ./lib/iptocoord.o -c ./src/iptocoord.cpp

vectors: ./src/vectors.cpp ./inc/vectors.h
	$(GPP) -o ./lib/vectors.o -c ./src/vectors.cpp

setup: ./src/setup.cpp ./inc/setup.h
	$(GPP) -o ./lib/setup.o -c ./src/setup.cpp

project: ./src/project.cpp
	$(GPP) -o ./bin/project ./src/project.cpp $(LFLAGS)
	ln -s ./bin/project ./

clean:
	rm -f ./bin/project
	rm -f ./project
	rm -f ./lib/*.o
	rm -f ./core
	rm -f ./bin/core
	rm -f ./img/map.ppm
	rm -f ./errors.log

