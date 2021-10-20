CXX = g++
CXXFLAGS = -g -Wall -std=c++17

all:		clean predictors

predictors:	predictors.o
	$(CXX) $(CXXFLAGS) predictors.o -o $@

clean:
	rm -f *~ *.o predictors
