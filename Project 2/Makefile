CXX = g++
CXXFLAGS = -g -Wall -std=c++17

all:		clean cache-sim

cache-sim:	cache-sim.o
	$(CXX) $(CXXFLAGS) cache-sim.o -o $@

clean:
	rm -f *~ *.o cache-sim
