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
	cout << "Term Project - Find Shortest Path Problem 12201898 ������" << endl;
	//--------���� �б�
	//�� ���ٿ��� ����� ������ ������ ���Ŀ� �������� ���´�.
	int nodeCount;
	int node1, node2, weight;
	int** w;

	fstream f(nodeFile, ios::in);
	if (!f) {
		cout << "������ �����ϴ�." << endl;
		return 0;
	}
	f >> nodeCount;
	//��尳�� x ��尳���� ������ adjacent ����� �����Ѵ�.
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
	//--------���ͽ�Ʈ��
	cout << "*******Running Dijkstra" << endl;
	dijkstra d(w, nodeCount, startNode, goalNode);
	d.run();
	d.showResult();
	cout << "*******Ended Dijkstra" << endl;
	////--------A* �˰���
	cout << "*******Running A* Algorithm" << endl;
	BFS(w, nodeCount, goalNode, hValueWeight); //make heuristic value for each node
	astar a(w, nodeCount, startNode, goalNode);
	a.loadH(hFunctionFile);
	a.run();
	a.showResult();
	cout << "*******Ended A* Algorithm" << endl;
	//--------GA �˰���
	cout << "*******Running GA Algorithm" << endl;
	GA g(w, nodeCount, startNode, goalNode, numGenome, generation, probCrossover, probMutation, crossoverSelect, mutationSelect, elitismNum, multiRand);
	g.run();
	//���� ��� �޸� ����
	for (int i = 0; i < nodeCount; i++) {
		delete[] w[i];
	}
	delete[] w;
}
