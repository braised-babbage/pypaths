CC=c++
CFLAGS=-std=c++11 -O3 -I/usr/local/Cellar/tclap/1.2.1/include
HEADERS = shortest_paths.hpp geometric_graph.hpp statistics.hpp

%.o: %.cpp $(HEADERS)
	$(CC) -c -o $@ $< $(CFLAGS)

test: shortest_paths.o geometric_graph.o statistics.o test.o
	$(CC) -o test shortest_paths.o geometric_graph.o statistics.o test.o

growth: shortest_paths.o geometric_graph.o statistics.o growth.o
	$(CC) -o growth shortest_paths.o geometric_graph.o statistics.o growth.o
