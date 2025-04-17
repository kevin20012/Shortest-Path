#include<iostream>
#include <queue>
#include <time.h>
#include <fstream>
#include "parameter.h"
#include "Dijkstra.h"
using namespace std;
using std::list;

dijkstra::dijkstra(int **g, int nodeCount, int startNode, int goalNode) {
	//2차원 배열 g(weight)로부터 node 리스트 동적할당
	node = new list<pair<int ,int>> [nodeCount];
	for (int i = 0; i < nodeCount; i++) {
		for (int j = 0; j < nodeCount; j++) {
			if (i!=j and g[i][j] != INF) {
				node[i].push_back({g[i][j], j});
			}
		}
	}
	
	//nodeCount 초기화
	this->nodeCount = nodeCount;
	//startNode 초기화
	this->startNode = startNode;
	//goalNode 초기화
	this->goalNode = goalNode;
	//dist 초기화
	dist = new int[nodeCount];
	for (int i = 0; i < nodeCount; i++) {
		dist[i] = INF;
	}
	//p 배열
	p = new int[nodeCount];
	for (int i = 0; i < nodeCount; i++) {
		p[i] = 0;
	}
	//runningTime 초기화
	runningTime = 0;
}

void dijkstra::run() {
	clock_t start, finish; //시간 측정을 위한 변수
	start = clock();
	priority_queue<pair<int, int>, std::vector<std::pair<int, int>>, Compare> pq; //첫번째 인자를 기준으로 내림차순 따라서 {거리, 노드번호}로 push
	bool* visited = new bool[nodeCount] {false, }; //방문 처리를 위한 배열
	pq.push({ 0, startNode }); //시작 노드를 pq에 push
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
		//goal에 도달했으면, 종료
		if (curNode == goalNode) {
			break;
		}

		for (list<pair<int, int>>::iterator iter = node[curNode].begin(); iter != node[curNode].end(); iter++) {
			if (visited[iter->second] != true) {
				if (dist[curNode] + (iter->first) < dist[iter->second]) { //갱신되는 노드가 있으면, 해당 노드와 거리를 pq에 push
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
	//log 남기기
	ofstream fout;
	fout.open(LogDijk);
	fout << "Dijkstra 알고리즘\n";
	//최종노드까지의 최단거리 보여주기
	fout << "<최단거리> " << dist[goalNode] << "\n";
	cout << startNode << "->" << goalNode << "의 최단거리 : " << dist[goalNode] << endl;
	//dist 배열 보여주기
	/*for (int i = 0; i < nodeCount; i++) {
		cout << dist[i] << " ";
	}
	cout << endl;*/
	//경로 보여주기
	int cur = goalNode;
	vector<int> pos;
	pos.push_back(cur);
	while (cur != startNode) {
		pos.insert(pos.begin(), p[cur]);
		cur = p[cur];
	}
	fout << "<최단 경로> ";
	for (int i = 0; i < pos.size(); i++) {
		cout << pos[i] << " ";
		fout << pos[i] << " ";
	}
	fout << "\n";
	cout << endl;
	//러닝타임 보여주기
	cout << "소요시간 : " << runningTime << "ms"  << endl;
	fout << "<RunningTime> " << runningTime << "\n";
	fout.close();
}

int dijkstra::getResult() {
	return dist[goalNode];
}

dijkstra::~dijkstra() {
	delete [] node;
}
