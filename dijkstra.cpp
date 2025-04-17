#include<iostream>
#include <queue>
#include <time.h>
#include <fstream>
#include "parameter.h"
#include "Dijkstra.h"
using namespace std;
using std::list;

dijkstra::dijkstra(int **g, int nodeCount, int startNode, int goalNode) {
	//2���� �迭 g(weight)�κ��� node ����Ʈ �����Ҵ�
	node = new list<pair<int ,int>> [nodeCount];
	for (int i = 0; i < nodeCount; i++) {
		for (int j = 0; j < nodeCount; j++) {
			if (i!=j and g[i][j] != INF) {
				node[i].push_back({g[i][j], j});
			}
		}
	}
	
	//nodeCount �ʱ�ȭ
	this->nodeCount = nodeCount;
	//startNode �ʱ�ȭ
	this->startNode = startNode;
	//goalNode �ʱ�ȭ
	this->goalNode = goalNode;
	//dist �ʱ�ȭ
	dist = new int[nodeCount];
	for (int i = 0; i < nodeCount; i++) {
		dist[i] = INF;
	}
	//p �迭
	p = new int[nodeCount];
	for (int i = 0; i < nodeCount; i++) {
		p[i] = 0;
	}
	//runningTime �ʱ�ȭ
	runningTime = 0;
}

void dijkstra::run() {
	clock_t start, finish; //�ð� ������ ���� ����
	start = clock();
	priority_queue<pair<int, int>, std::vector<std::pair<int, int>>, Compare> pq; //ù��° ���ڸ� �������� �������� ���� {�Ÿ�, ����ȣ}�� push
	bool* visited = new bool[nodeCount] {false, }; //�湮 ó���� ���� �迭
	pq.push({ 0, startNode }); //���� ��带 pq�� push
	dist[startNode] = 0;

	pair<int, int> cur;
	int curNode;
	int curDist;
	while (!pq.empty()) {
		cur = pq.top();
		pq.pop();
		curNode = cur.second;
		if (visited[curNode] == true) {
			continue;
		}

		visited[curNode] = true;
		//goal�� ����������, ����
		if (curNode == goalNode) {
			break;
		}

		for (list<pair<int, int>>::iterator iter = node[curNode].begin(); iter != node[curNode].end(); iter++) {
			if (visited[iter->second] != true) {
				if (dist[curNode] + (iter->first) < dist[iter->second]) { //���ŵǴ� ��尡 ������, �ش� ���� �Ÿ��� pq�� push
					dist[iter->second] = dist[curNode] + (iter->first);
					pq.push({ dist[iter->second], iter->second });
					p[iter->second] = curNode;
				}
			}
		}
	}
	finish = clock();
	runningTime = (double)(finish - start);
}

void dijkstra::showResult() {
	//log �����
	ofstream fout;
	fout.open(LogDijk);
	fout << "Dijkstra �˰���\n";
	//������������ �ִܰŸ� �����ֱ�
	fout << "<�ִܰŸ�> " << dist[goalNode] << "\n";
	cout << startNode << "->" << goalNode << "�� �ִܰŸ� : " << dist[goalNode] << endl;
	//dist �迭 �����ֱ�
	/*for (int i = 0; i < nodeCount; i++) {
		cout << dist[i] << " ";
	}
	cout << endl;*/
	//��� �����ֱ�
	int cur = goalNode;
	vector<int> pos;
	pos.push_back(cur);
	while (cur != startNode) {
		pos.insert(pos.begin(), p[cur]);
		cur = p[cur];
	}
	fout << "<�ִ� ���> ";
	for (int i = 0; i < pos.size(); i++) {
		cout << pos[i] << " ";
		fout << pos[i] << " ";
	}
	fout << "\n";
	cout << endl;
	//����Ÿ�� �����ֱ�
	cout << "�ҿ�ð� : " << runningTime << "ms"  << endl;
	fout << "<RunningTime> " << runningTime << "\n";
	fout.close();
}

int dijkstra::getResult() {
	return dist[goalNode];
}

dijkstra::~dijkstra() {
	delete [] node;
}
