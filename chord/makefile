# Compiler variables
CCFLAGS = -std=c++11

all: simulate

# Rule to link object code files to create executable file
simulate: simulate.o network.o node.o
	g++ $(CCFLAGS) -o simulate simulate.o network.o node.o

simulate.o: simulate.cpp
	g++ $(CCFLAGS) -c simulate.cpp
	
network.o: network.cpp network.h
	g++ $(CCFLAGS) -c network.cpp network.h

node.o: node.cpp node.h
	g++ $(CCFLAGS) -c node.cpp node.h

# Pseudo-target to remove object code and executale files
clean:
	-rm *.o simulate

