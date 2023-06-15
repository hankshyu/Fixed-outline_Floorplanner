CXX = g++
FLAGS = -std=c++11 
CFLAGS = -c
DEBUGFLAGS = -g 
all: iccad
debug: iccad_debug

# LINKFLAGS = -pedantic -Wall -fomit-frame-pointer -funroll-all-loops -O3
LINKFLAGS = 

iccad: main.o BStarTree.o Block.o Node.o
	$(CXX) $(LINKFLAGS) $^ -o $@
		
main.o: main.cpp 
	$(CXX) $(LINKFLAGS) $(CFLAGS) -DCOMPILETIME="\"`date`\"" $^ -o $@

BStarTree.o: BStarTree.cpp BStarTree.h
	$(CXX) $(LINKFLAGS) $(CFLAGS) BStarTree.cpp -o $@
			
Block.o: Block.cpp Block.h
	$(CXX) $(LINKFLAGS) $(CFLAGS) Block.cpp -o $@

Node.o: Node.cpp Node.h
	$(CXX) $(LINKFLAGS) $(CFLAGS) Node.cpp -o $@



iccad_debug: main_debug.o BStarTree_debug.o Block_debug.o Node_debug.o
	$(CXX) $(DEBUGFLAGS) $^ -o $@
		
main_debug.o: main.cpp 
	$(CXX) $(DEBUGFLAGS) $(CFLAGS) -DCOMPILETIME="\"`date`\"" $^ -o $@

BStarTree_debug.o: BStarTree.cpp BStarTree.h
	$(CXX) $(DEBUGFLAGS) $(CFLAGS) BStarTree.cpp -o $@
			
Block_debug.o: Block.cpp Block.h
	$(CXX) $(DEBUGFLAGS) $(CFLAGS) Block.cpp -o $@

Node_debug.o: Node.cpp Node.h
	$(CXX) $(DEBUGFLAGS) $(CFLAGS) Node.cpp -o $@


clean:
	rm -rf *.o *.gch iccad iccad
