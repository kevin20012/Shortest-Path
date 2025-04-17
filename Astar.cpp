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
	clock_t start, finish; //�ð� ������ ���� ����
	start = clock();
	priority_queue<pair<int, int>, std::vector<std::pair<int, int>>, Compare> pq; //ù��° ���ڸ� �������� �������� ���� {�Ÿ�, ����ȣ}�� push
	bool* visited = new bool[nodeCount]; //�湮 ó���� ���� �迭
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
					pq.push({ dist[iter->second]+h[iter->second], iter->second});
					p[iter->second] = curNode;
				}
			}
		}
	}
	finish = clock();
	runningTime = (double)(finish - start); //ms ����
}

int astar::loadH(string loadFile) { //node����xnode���� ũ���� �ŷ��Լ��� ���� h �迭�� �������.
	fstream f(loadFile, ios::in);
	if (!f) {
		cout << "������ �����ϴ�." << endl;
		return 0;
	}

	string temp;
	string token;
	getline(f, temp); //�� �� �б�
	stringstream sstream(temp);
	for (int i = 0; i < nodeCount; i++){ //������ �������� tokenize�Ͽ� h �迭 �����
		getline(sstream, token, ' '); 
		h[i] = stoi(token);
	}
	f.close();
	return 1;
}

void astar::showResult() {
	//log �����
	ofstream fout;
	fout.open(LogAstar);
	fout << "A* �˰���\n";
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
	cout << "�ҿ�ð� : " << runningTime << "ms" << endl;
	fout << "<RunningTime> " << runningTime << "\n";
	fout.close();
}

astar::~astar() {
	delete[] h;
}