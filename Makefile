CFLAGS=-std=c++11 -Wall -g 

export PKG_CONFIG_PATH=/usr/local/AID/pkgconfig


CFLAGS+= `pkg-config --cflags opencv` -fopenmp
LDFLAGS+= `pkg-config --libs opencv` -fopenmp

main: main.o gesture.o
	$(CXX) -O3 -o main main.o gesture.o $(LDFLAGS)

main.o: main.cpp
	$(CXX) -O3 -c main.cpp ${CFLAGS}

gesture.o: gesture.cpp
	$(CXX) -O3 -c gesture.cpp ${CFLAGS}
	
clean: 
	rm -f *.d *.o
	rm -f main

