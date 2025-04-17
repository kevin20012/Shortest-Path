#ifndef ASTAR_H
#define ASTAR_H

#include "Dijkstra.h"
using std::string;

class astar :public dijkstra{
private:
	int* h;
public:
	astar(int** g, int nodeCount, int startNode, int goalNode);
	int loadH(string loadFile);
	void showResult();
	void run();
	~astar();
};

#endif
