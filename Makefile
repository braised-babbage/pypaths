CC=c++
CFLAGS=-std=c++11 -O3
HEADERS = shortest_paths.hpp geometric_graph.hpp 

%.o: %.cpp $(HEADERS)
	$(CC) -c -o $@ $< $(CFLAGS)

test: shortest_paths.o geometric_graph.o test.o
	$(CC) -o test shortest_paths.o geometric_graph.o test.o
