#include <iostream>
#include <queue>
#include <vector>
#include <list>
#include <utility>
#include <fstream>
#include "parameter.h"
#include "BFS.h"
using std::pair;
using std::vector;
using std::queue;
using std::list;
using std::ofstream;

/*
Astar 알고리즘을 사용하기위한 heuristic 함수를 만들기 위한 bfs입니다.
goalNode에서 시작해 나머지 노드를 bfs로 탐색할때 만들어지는 tree에서 같은 height를 가지는 노드는
같은 가중치를 부여하도록해 heuristic 값을 각 노드에 부여합니다.
그리고 해당 가중치를 hFunction.txt에 저장한뒤 이를 사용해 Astar 알고리즘을 동작시킵니다.
*/

void BFS(int**g, int nodeCount, int goalNode, int hValueW) {
	int hValueWeight = hValueW; //h값 가중치로 각 tree의 높이에 해당값이 곱해진 값이 h 값이 됩니다.

	list<int> * node = new list<int>[nodeCount];
	for (int i = 0; i < nodeCount; i++) {
		for (int j = 0; j < nodeCount; j++) {
			if (i != j and g[i][j] != INF) {
				node[i].push_back(j); //node번호
				if (std::find(node[j].begin(), node[j].end(), i) == node[j].end()) { //양방향 그래프로 만들기
					node[j].push_back(i);
				}
			}
		}
	}

	int* hValue = new int[nodeCount] {hValueWeight,};
	bool* visited = new bool[nodeCount] {false,};

	queue<pair<int, int>> queue; //{node번호, height}
	queue.push({goalNode, 1});
	visited[goalNode] = true;
	hValue[goalNode] = 1* hValueWeight;

	pair<int, int> cur;
	while (queue.size() != 0) {
		cur = queue.front();
		queue.pop();

		for (list<int>::iterator iter = node[cur.first].begin(); iter != node[cur.first].end(); iter++) {
			if (visited[*iter] != true) {
				queue.push({ *iter, cur.second+1});
				visited[*iter] = true;
				hValue[*iter] = (cur.second + 1) * hValueWeight;
			}
		}
	}
	ofstream fout;
	fout.open(hFunctionFile);
	for (int i = 0; i < nodeCount; i++) {
		fout << hValue[i];
		i != nodeCount - 1 ? fout << " " : fout << "\n";
	}
	fout.close();
}