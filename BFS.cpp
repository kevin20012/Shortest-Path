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
Astar �˰����� ����ϱ����� heuristic �Լ��� ����� ���� bfs�Դϴ�.
goalNode���� ������ ������ ��带 bfs�� Ž���Ҷ� ��������� tree���� ���� height�� ������ ����
���� ����ġ�� �ο��ϵ����� heuristic ���� �� ��忡 �ο��մϴ�.
�׸��� �ش� ����ġ�� hFunction.txt�� �����ѵ� �̸� ����� Astar �˰����� ���۽�ŵ�ϴ�.
*/

void BFS(int**g, int nodeCount, int goalNode, int hValueW) {
	int hValueWeight = hValueW; //h�� ����ġ�� �� tree�� ���̿� �ش簪�� ������ ���� h ���� �˴ϴ�.

	list<int> * node = new list<int>[nodeCount];
	for (int i = 0; i < nodeCount; i++) {
		for (int j = 0; j < nodeCount; j++) {
			if (i != j and g[i][j] != INF) {
				node[i].push_back(j); //node��ȣ
				if (std::find(node[j].begin(), node[j].end(), i) == node[j].end()) { //����� �׷����� �����
					node[j].push_back(i);
				}
			}
		}
	}

	int* hValue = new int[nodeCount] {hValueWeight,};
	bool* visited = new bool[nodeCount] {false,};

	queue<pair<int, int>> queue; //{node��ȣ, height}
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