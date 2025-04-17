#ifndef DIJKSTRA_H
#define DIJKSTRA_H
#include <list>
#include <utility>
#include "parameter.h"
using std::list;
using std::pair;

struct Compare {
	bool operator()(const std::pair<int, int>& p1, const std::pair<int, int>& p2) {
		return p1.first > p2.first; // Min heap: return true if p1 > p2
	}
};

class dijkstra {
protected:
	list<pair<int, int>>* node;
	int nodeCount;
	int startNode;
	int goalNode;
	//info
	int* dist;
	int* p; //지나온 노드를 기억
	double runningTime;
public:
	dijkstra(int** g, int nodeCount, int startNode, int goalNode);
	void run();
	void showResult();
	int getResult();
	~dijkstra();
};

#endif
