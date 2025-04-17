#include <iostream>
#include <queue>
#include <fstream>
#include "Dijkstra.h"
#include "Astar.h"
#include "GA.h"
#include "BFS.h"
#include "parameter.h"
using namespace std;

int main() {
	cout << "Term Project - Find Shortest Path Problem 12201898 박준형" << endl;
	//--------파일 읽기
	//맨 윗줄에는 노드의 개수가 나오고 이후에 간선들이 나온다.
	int nodeCount;
	int node1, node2, weight;
	int** w;

	fstream f(nodeFile, ios::in);
	if (!f) {
		cout << "파일이 없습니다." << endl;
		return 0;
	}
	f >> nodeCount;
	//노드개수 x 노드개수를 가지는 adjacent 행렬을 생성한다.
	w = new int* [nodeCount];
	for (int i = 0; i < nodeCount; i++) {
		w[i] = new int[nodeCount];
	}
	for (int i = 0; i < nodeCount; i++) {
		for (int j = 0; j < nodeCount; j++) {
			if (i == j) { 
				w[i][j] = 0; 
			}
			else {
				w[i][j] = INF;
			}
		}
	}
	while (f >> node1 >> node2 >> weight) {
		w[node1][node2] = weight;
	}
	//test
	//for (int i = 0; i < nodeCount; i++) {
	//	for (int j = 0; j < nodeCount; j++) {
	//		cout << w[i][j] << " ";
	//	}
	//	cout << endl;
	//}
	//--------다익스트라
	cout << "*******Running Dijkstra" << endl;
	dijkstra d(w, nodeCount, startNode, goalNode);
	d.run();
	d.showResult();
	cout << "*******Ended Dijkstra" << endl;
	////--------A* 알고리즘
	cout << "*******Running A* Algorithm" << endl;
	BFS(w, nodeCount, goalNode, hValueWeight); //make heuristic value for each node
	astar a(w, nodeCount, startNode, goalNode);
	a.loadH(hFunctionFile);
	a.run();
	a.showResult();
	cout << "*******Ended A* Algorithm" << endl;
	//--------GA 알고리즘
	cout << "*******Running GA Algorithm" << endl;
	GA g(w, nodeCount, startNode, goalNode, numGenome, generation, probCrossover, probMutation, crossoverSelect, mutationSelect, elitismNum, multiRand);
	g.run();
	//인접 행렬 메모리 해제
	for (int i = 0; i < nodeCount; i++) {
		delete[] w[i];
	}
	delete[] w;
}
