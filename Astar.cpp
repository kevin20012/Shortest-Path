#include<iostream>
#include <queue>
#include <time.h>
#include <fstream>
#include <sstream>
#include <string>
#include "parameter.h"
#include "Astar.h"
using namespace std;

astar::astar(int** g, int nodeCount, int startNode, int goalNode)
	:dijkstra(g, nodeCount, startNode, goalNode) {
	h = new int [nodeCount];
}
void astar::run() {
	clock_t start, finish; //시간 측정을 위한 변수
	start = clock();
	priority_queue<pair<int, int>, std::vector<std::pair<int, int>>, Compare> pq; //첫번째 인자를 기준으로 내림차순 따라서 {거리, 노드번호}로 push
	bool* visited = new bool[nodeCount]; //방문 처리를 위한 배열
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
					pq.push({ dist[iter->second]+h[iter->second], iter->second});
					p[iter->second] = curNode;
				}
			}
		}
	}
	finish = clock();
	runningTime = (double)(finish - start); //ms 단위
}

int astar::loadH(string loadFile) { //node개수xnode개수 크기의 매력함수의 값을 h 배열로 만들어줌.
	fstream f(loadFile, ios::in);
	if (!f) {
		cout << "파일이 없습니다." << endl;
		return 0;
	}

	string temp;
	string token;
	getline(f, temp); //한 줄 읽기
	stringstream sstream(temp);
	for (int i = 0; i < nodeCount; i++){ //공백을 기준으로 tokenize하여 h 배열 만들기
		getline(sstream, token, ' '); 
		h[i] = stoi(token);
	}
	f.close();
	return 1;
}

void astar::showResult() {
	//log 남기기
	ofstream fout;
	fout.open(LogAstar);
	fout << "A* 알고리즘\n";
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
	cout << "소요시간 : " << runningTime << "ms" << endl;
	fout << "<RunningTime> " << runningTime << "\n";
	fout.close();
}

astar::~astar() {
	delete[] h;
}