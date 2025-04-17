#include<iostream>
#include <queue>
#include <time.h>
#include <random>
#include <algorithm>
#include <cmath>
#include <fstream>
#include "GA.h"
#include "parameter.h"
using namespace std;

//Genome �Ѱ��� Ŭ���� -----------------------------------
Genome::Genome() { //����ü�� �����ϰ� ������ �ʰ�, �׳� �ΰ� ������ ��.
	fitness = 0;
}
Genome::Genome(int numNodes, int startNode, int goalNode) { //������ ��η� permutation�ϰ� ����ü�� ����.
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> dist(0, 2*numNodes); 
    //uniform ������ ����������.
    //0~numNodes-1 ������ ���� ���� ����ϰ�
    //�� �̻� ���� ������ -1�� �ִ� �뵵�� ����Ѵ�.

    int nextNode;
    for (int i = 0; i < numNodes-2; i++) {
        nextNode = dist(mt); //���� ����� �ĺ��� �������� �����Ѵ�.
        if (nextNode >= numNodes) { //numNodes�� ������, �ǳʶٴ� ���� -1�� �߰��Ѵ�. -1�� �ǳʶڴٴ� �ǹ̴�.
            //���� ���, 0, -1, -1, 6, -1, 5, 7 => 0, 6, 5, 7 ������ ��带 �̵��ϴ� �Ͱ� ����.
            vecNodes.push_back(-1);
            continue;
        }
        while (nextNode == startNode || nextNode == goalNode || std::find(vecNodes.begin(), vecNodes.end(), nextNode) != vecNodes.end()) { //���� vecNodes ���� �ߺ��Ǵ� Node�� �ִ��� �˻��Ѵ�. ���������� ��� �̴´�.
            nextNode = dist(mt);
        }
        if (nextNode >= numNodes) {
            vecNodes.push_back(-1);
            continue;
        }
        else {
            vecNodes.push_back(nextNode); //�ߺ��Ǵ� ���� ���� ���, vecNodes�� �����Ѵ�.
        }
        
    }
    //ó���� ���� ���۳��� ����带 �����Ѵ�.
    vecNodes.insert(vecNodes.begin(), startNode);
    vecNodes.push_back(goalNode);
}


//----------------------------------------------

//GA �˰��� Ŭ���� ------------------------------------
GA::GA(int** g, int nodeCount, int startNode, int goalNode, int numGenome, int generation, double probCrossover, double probMutation, string crossoverSelect, string mutationSelect, int elitismNum, int multiRand)
    :startNode(startNode), goalNode(goalNode), lenGenome(nodeCount), numGenome(numGenome), generation(generation), currentShortestPath(INF), currentLongestPath(0), expectFitness(0), probCrossover(probCrossover), probMutation(probMutation), elitismNum(elitismNum), multiRand(multiRand){
    //weight ����ġ �����Ҵ�
    weight = new int* [nodeCount];
    for (int i = 0; i < nodeCount; i++) {
        weight[i] = new int[nodeCount];
    }
    for (int i = 0; i < nodeCount; i++) { //�� ����
        for (int j = 0; j < nodeCount; j++) {
            weight[i][j] = g[i][j];
        }
    }
    //��ȿ�� üũ - crossoverParameter
    if (crossoverSelect == "PMX" || crossoverSelect == "OBX" || crossoverSelect == "PBX") {
        this->crossoverSelect = crossoverSelect;
    }
    else {
        cout << "wrong input!" << endl;
        exit(0);
    }
    //��ȿ�� üũ - mutationParameter
    if (mutationSelect == "SM" || mutationSelect == "DM" || mutationSelect == "IM" || mutationSelect == "IVM" || mutationSelect == "DIVM" || mutationSelect == "OneRand" || mutationSelect == "MultiRand") {
        this->mutationSelect = mutationSelect;
    }
    else {
        cout << "wrong input!" << endl;
        exit(0);
    }
}

int GA::calcDistance(Genome& g) {
    int sum = 0;
    vector<int> availableNodes;
    for (int i = 0; i < lenGenome; i++) {
        if (g.vecNodes[i] != -1) {
            availableNodes.push_back(g.vecNodes[i]);
        }
    }
    for (int i = 0; i < availableNodes.size()-1; i++) {
        if (weight[availableNodes[i]][availableNodes[i + 1]] == INF) {
            sum = INF;
            break;
        }
        sum += weight[availableNodes[i]][availableNodes[i + 1]];
    }

    return sum;
}

void GA::recalcFitnessToAllGenome() {
    //���� ������ ��� ����ü�� fitness �� ���ѵ� �����ϸ� �� ��й�
    int curDist = 0;
    double sum = 0.0;
    int* Dist = new int[numGenome];
    for (int i = 0; i < numGenome; i++) {
        curDist = calcDistance(vecGenomes[i]);

        Dist[i] = curDist;
        if (curDist <= currentShortestPath) {
            currentShortestPath = curDist;
            shortestGenome = vecGenomes[i];
        }
        if (curDist > currentLongestPath) {
            currentLongestPath = curDist;
        }
    }
    //fitness = currentLongestPath - �ڽ��� �Ÿ� (�̷��� �ϸ� �ڽ��� �Ÿ��� ���� ���� �������� Ŀ��)
    //fitness ��ձ��ϱ�
    for (int i = 0; i < numGenome; i++) {
        sum += 1.0/Dist[i];
    }
    expectFitness = sum/numGenome; //fitness �� ���

    for (int i = 0; i < numGenome; i++) {
        vecGenomes[i].fitness = (1.0 / Dist[i]) / sum;
    }
}

Genome& GA::roulette() { //GA ÷������ ����.
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist(0.0, 1.0); //uniform ������ �������� �Ͽ� 0~1 ������ �Ҽ��� �̴´�.
    
    double fSlice = dist(mt);
    double cfTotal = 0.0;

    int	SelectedGenome = 0;

    for (int i = 0; i < numGenome; i++)
    {

        cfTotal += vecGenomes[i].fitness;

        if (cfTotal > fSlice)
        {
            SelectedGenome = i;

            break;
        }
    }
    //cout << "��÷ : " << cfTotal<<"/"<<fSlice<<"->" << SelectedGenome << endl;
    return vecGenomes[SelectedGenome];
}

void GA::crossoverPMX(Genome& p1, Genome& p2, Genome& o1, Genome& o2) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist1(0.0, 1.0); //uniform ������ �������� �Ͽ� 0~1 ������ �Ҽ��� �̴´�.

    o1.vecNodes = p1.vecNodes;
    o2.vecNodes = p2.vecNodes;

    //���� Ȯ���� �����ص� Ȯ������ ���� ���;� �ش� Ȯ���̹Ƿ� �����ϰ� �� ũ�� ���� ���, �����Ѵ�.
    //���� �� �θ� ���� Genome�� ��쿡�� �����Ѵ�.
    if ((dist1(mt) > probCrossover) || (&p1 == &p2))
    {
        return;
    }
    vector<int> availableNode_o1;
    vector<int> availableNode_o2;
    int count = 0, count2=0;
    for (int i = 1; i < lenGenome-1; i++) { //����, ��ǥ��� ����
        if (o1.vecNodes[i] != -1) {
            availableNode_o1.push_back(o1.vecNodes[i]);
            count += 1;
        }
        if (o2.vecNodes[i] != -1) {
            availableNode_o2.push_back(o2.vecNodes[i]);
            count2++;
        }
    }
    int minSize = 0;
    if (availableNode_o1.size() <= availableNode_o2.size()) {
        minSize = availableNode_o1.size();
    }
    else {
        minSize = availableNode_o2.size();
    }
    //�켱 ������ �����ϰ� �����Ѵ�.
    std::uniform_int_distribution<int> dist2(0, minSize-1); // ù��°, ������ ��带 ���ܽ�Ű����� 1~������ 3��° ������ ���������Ѵ�.
    int beg = dist2(mt);
    std::uniform_int_distribution<int> dist3(beg, minSize-1);
    int end = dist3(mt);

    //�־��� ������ ���� o1, o2�� swap �Ѵ�.
    vector<int>::iterator posGene1, posGene2;
    int node1, node2;
    for (int i = beg; i <= end; i++) {
        node1 = availableNode_o1[i];
        node2 = availableNode_o2[i];
        if (node1 != node2) { //�� ��尡 ���� �������� swap�Ѵ�.
            //find and swap them in o1
            posGene1 = find(availableNode_o1.begin(), availableNode_o1.end(), node1);
            posGene2 = find(availableNode_o1.begin(), availableNode_o1.end(), node2);

            if (posGene2 == availableNode_o1.end()) { //ã�� ���� ���, �׳� �ش� ������ ��ü�Ѵ�,
                *posGene1 = node2;
            }
            else {
                std::swap(*posGene1, *posGene2);
            }

            //and in o2
            posGene1 = find(availableNode_o2.begin(), availableNode_o2.end(), node1);
            posGene2 = find(availableNode_o2.begin(), availableNode_o2.end(), node2);

            if (posGene1 == availableNode_o2.end()) { //ã�� ���� ���, �׳� �ش� ������ ��ü�Ѵ�,
                *posGene2 = node1;
            }
            else {
                std::swap(*posGene1, *posGene2);
            }
        }

        //�ٽ� o1,o2����
        int o1Count = 0, o2Count = 0;
        for (int i = 1; i < lenGenome-1; i++) {
            if (std::find(availableNode_o1.begin(), availableNode_o1.end(), o1.vecNodes[i]) != availableNode_o1.end()) {
                //cout << "o1Count : " << o1Count << "/" << availableNode_o1.size()-1 << endl;
                //cout << "o1Count: " << o1Count << endl;
                o1.vecNodes[i] = availableNode_o1[o1Count];
                o1Count++;
            }
            if (std::find(availableNode_o2.begin(), availableNode_o2.end(), o2.vecNodes[i]) != availableNode_o2.end()) {
                //cout << "o2Count : " << o2Count << "/" << availableNode_o2.size()-1 << endl;
                //cout << "o2Count: " << o2Count << endl;
                o2.vecNodes[i] = availableNode_o2[o2Count];
                o2Count++;
            }
        }
    }
}

void GA::crossoverOBX(Genome& p1, Genome& p2, Genome& o1, Genome& o2) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist1(0.0, 1.0); //uniform ������ �������� �Ͽ� 0~1 ������ �Ҽ��� �̴´�.

    o1.vecNodes = p1.vecNodes;
    o2.vecNodes = p2.vecNodes;

    //���� Ȯ���� �����ص� Ȯ������ ���� ���;� �ش� Ȯ���̹Ƿ� �����ϰ� �� ũ�� ���� ���, �����Ѵ�.
    //���� �� �θ� ���� Genome�� ��쿡�� �����Ѵ�.
    if ((dist1(mt) > probCrossover) || (&p1 == &p2))
    {
        return;
    }

    //������ ������ 1/3 ������ �ε����� �̾� �����Ѵ�. - �� ��쿡��, ù��°, ������ ���� �����Ѵ�.
    std::uniform_int_distribution<int> dist2(1, lenGenome-2);
    vector<int> index;
    int temp = 0;
    for (int i = 0; i < lenGenome/3; i++) {
        temp = dist2(mt); //���� ����� �ĺ��� �������� �����Ѵ�.
        while (std::find(index.begin(), index.end(), temp) != index.end()) { //���� vecNodes ���� �ߺ��Ǵ� Node�� �ִ��� �˻��Ѵ�. ���������� ��� �̴´�.
            temp = dist2(mt);
        }
        index.push_back(temp); //�ߺ��Ǵ� ���� ���� ���, vecNodes�� �����Ѵ�.
    }
    //������ ���
    vector<int> p1_value;
    vector<int> p2_value;
    for (int i = 0; i < index.size(); i++) {
        if (p1.vecNodes[index[i]] != -1) {
            p1_value.push_back(p1.vecNodes[index[i]]);
        }
        if (p2.vecNodes[index[i]] != -1) {
            p2_value.push_back(p2.vecNodes[index[i]]);
        }
    }
    //o1,o2 �� ���� ����
    vector<int>::iterator posO1, posO2;
    int o1Count = 0, o2Count = 0;
    for (int i = 0; i < lenGenome; i++) {
        if (std::find(p2_value.begin(), p2_value.end(), o1.vecNodes[i]) != p2_value.end()) {
            o1.vecNodes[i] = p2_value[o1Count];
            o1Count++;
        }
        if (std::find(p1_value.begin(), p1_value.end(), o2.vecNodes[i]) != p1_value.end()) {
            o2.vecNodes[i] = p1_value[o2Count];
            o2Count++;
        }
    }
}

void GA::crossoverPBX(Genome& p1, Genome& p2, Genome& o1, Genome& o2) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist1(0.0, 1.0); //uniform ������ �������� �Ͽ� 0~1 ������ �Ҽ��� �̴´�.

    o1.vecNodes = p1.vecNodes;
    o2.vecNodes = p2.vecNodes;

    //���� Ȯ���� �����ص� Ȯ������ ���� ���;� �ش� Ȯ���̹Ƿ� �����ϰ� �� ũ�� ���� ���, �����Ѵ�.
    //���� �� �θ� ���� Genome�� ��쿡�� �����Ѵ�.
    if ((dist1(mt) > probCrossover) || (&p1 == &p2))
    {
        return;
    }

    //������ ������ 1/3 ������ �ε����� �̾� �����Ѵ�. - �� ��쿡��, ù��°, ������ ���� �����Ѵ�.
    std::uniform_int_distribution<int> dist2(1, lenGenome - 2);
    vector<int> index;
    int temp = 0;
    for (int i = 0; i < lenGenome / 3; i++) {
        temp = dist2(mt); //���� ����� �ĺ��� �������� �����Ѵ�.
        while (std::find(index.begin(), index.end(), temp) != index.end()) { //���� vecNodes ���� �ߺ��Ǵ� Node�� �ִ��� �˻��Ѵ�. ���������� ��� �̴´�.
            temp = dist2(mt);
        }
        index.push_back(temp); //�ߺ��Ǵ� ���� ���� ���, vecNodes�� �����Ѵ�.
    }

    //������ ���
    vector<int> p1_value;
    vector<int> p2_value;
    for (int i = 0; i < index.size(); i++) {
        if (p1.vecNodes[index[i]] != -1) {
            p1_value.push_back(p1.vecNodes[index[i]]);
        }
        if (p2.vecNodes[index[i]] != -1) {
            p2_value.push_back(p2.vecNodes[index[i]]);
        }
    }

    //fix �� ��ҵ� ���� ������ ��ҵ��� �ߺ� ���� �ű��
    int o2Pos = 1, o1Pos = 1;
    for (int i = 1; i < lenGenome-1; i++) {
        if (std::find(index.begin(), index.end(), i) != index.end()) {
            continue;
        }
        else {
            for (int j = o2Pos; j < lenGenome-1; j++) {
                if (std::find(p1_value.begin(), p1_value.end(), p2.vecNodes[j]) == p1_value.end()) {
                    o1.vecNodes[i] = p2.vecNodes[j];
                    o2Pos = j+1;
                    break;
                }
            }
            for (int j = o1Pos; j < lenGenome-1; j++) {
                if (std::find(p2_value.begin(), p2_value.end(), p1.vecNodes[j]) == p2_value.end()) {
                    o2.vecNodes[i] = p1.vecNodes[j];
                    o1Pos = j + 1;
                    break;
                }
            }
        }
    }
}

void GA::mutateSM(Genome& g) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist1(0.0, 1.0); //uniform ������ �������� �Ͽ� 0~1 ������ �Ҽ��� �̴´�

    //���� Ȯ���� �����ص� Ȯ������ ���� ���;� �ش� Ȯ���̹Ƿ� �����ϰ� �� ũ�� ���� ���, �����Ѵ�.
    if ((dist1(mt) > probMutation))
    {
        return;
    }

    //�켱 ������ �����ϰ� �����Ѵ�.
    std::uniform_int_distribution<int> dist2(1, lenGenome - 3); // ù��°, ������ ��带 ���ܽ�Ű����� 1~������ 3��° ������ ���������Ѵ�.
    int beg = dist2(mt);

    std::uniform_int_distribution<int> dist3(beg, lenGenome - 2); //beg�������� ���������� 2��°������ �������� �Ѵ�.
    int end = dist3(mt);

    //�־��� ������ ���´�.
    vector<int> temp;
    for (int i = beg; i <= end; i++) {
        temp.push_back(g.vecNodes[i]);
    }
    vector<int> randIndex;
    std::uniform_int_distribution<int> dist4(0, temp.size()-1); 
    int rand;
    for (int i = 0; i < temp.size(); i++) {
        rand = dist4(mt);
        while (std::find(randIndex.begin(), randIndex.end(), rand) != randIndex.end()) {
            rand = dist4(mt);
        }
        randIndex.push_back(rand);
    }
    //���� ������ �ٽ� genome�� �����Ѵ�.
    for (int i = beg; i <= end; i++) {
        g.vecNodes[i] = randIndex[i-beg];
    }
}

void GA::mutateDM(Genome& g) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist1(0.0, 1.0); //uniform ������ �������� �Ͽ� 0~1 ������ �Ҽ��� �̴´�

    //���� Ȯ���� �����ص� Ȯ������ ���� ���;� �ش� Ȯ���̹Ƿ� �����ϰ� �� ũ�� ���� ���, �����Ѵ�.
    if ((dist1(mt) > probMutation))
    {
        return;
    }

    //�켱 ������ �����ϰ� �����Ѵ�.
    std::uniform_int_distribution<int> dist2(1, lenGenome - 3); // ù��°, ������ ��带 ���ܽ�Ű���� 1~������ 3��° ������ ���������Ѵ�.
    int beg = dist2(mt);

    std::uniform_int_distribution<int> dist3(beg, lenGenome - 2); //beg�������� ���������� 2��°������ �������� �Ѵ�.
    int end = dist3(mt);
    //������ ���� ����
    vector<int> temp;
    for (int i = beg; i <= end; i++) {
        temp.push_back(g.vecNodes[i]);
    }
    //������ �� �ϴ� vecNodes���� ����
    for (int i = 0; i < end-beg+1; i++) {
        g.vecNodes.erase(g.vecNodes.begin() + beg);
    }
    //�ű� ��ġ�� �ε����� �̴´�.
    std::uniform_int_distribution<int> dist4(1, g.vecNodes.size() - 2); // ù��°, ������ ��带 ���ܽ�Ű������ 1~������ 3��° ������ ���������Ѵ�.
    int moveTo = dist4(mt);
    //�ű��
    for (int i = 0; i < temp.size(); i++) {
        g.vecNodes.insert(g.vecNodes.begin() + moveTo, temp[i]);
    }

}

void GA::mutateIM(Genome& g) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist1(0.0, 1.0); //uniform ������ �������� �Ͽ� 0~1 ������ �Ҽ��� �̴´�

    //���� Ȯ���� �����ص� Ȯ������ ���� ���;� �ش� Ȯ���̹Ƿ� �����ϰ� �� ũ�� ���� ���, �����Ѵ�.
    if ((dist1(mt) > probMutation))
    {
        return;
    }

    //�Ű��� ��ġ�� �ε����� �̴´�.
    std::uniform_int_distribution<int> dist2(1, lenGenome -2); // ù��°, ������ ��带 ���ܽ�Ű����� 1~������ 3��° ������ ���������Ѵ�.
    int moveFrom = dist2(mt);

    //�ű� ��ġ�� �ε����� �̴´�.
    std::uniform_int_distribution<int> dist3(1, lenGenome - 2); // ù��°, ������ ��带 ���ܽ�Ű����� 1~������ 3��° ������ ���������Ѵ�.
    int moveTo = dist3(mt);

    //�ű��
    int temp = g.vecNodes[moveFrom];
    g.vecNodes.erase(g.vecNodes.begin() + moveFrom);
    g.vecNodes.insert(g.vecNodes.begin() + moveTo, temp);
}

void GA::mutateIVM(Genome& g) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist1(0.0, 1.0); //uniform ������ �������� �Ͽ� 0~1 ������ �Ҽ��� �̴´�

    //���� Ȯ���� �����ص� Ȯ������ ���� ���;� �ش� Ȯ���̹Ƿ� �����ϰ� �� ũ�� ���� ���, �����Ѵ�.
    if ((dist1(mt) > probMutation))
    {
        return;
    }

    //�켱 ������ �����ϰ� �����Ѵ�.
    std::uniform_int_distribution<int> dist2(1, lenGenome - 3); // ù��°, ������ ��带 ���ܽ�Ű����� 1~������ 3��° ������ ���������Ѵ�.
    int beg = dist2(mt);

    std::uniform_int_distribution<int> dist3(beg, lenGenome - 2); //beg�������� ���������� 2��°������ �������� �Ѵ�.
    int end = dist3(mt);

    //�־��� ������ ���´�.
    vector<int> inverse;
    for (int i = beg; i <= end; i++) {
        inverse.push_back(g.vecNodes[i]);
    }
    int temp;
    for (int i = 0; i < inverse.size()/2; i++) {
        temp = inverse[i];
        inverse[i] = inverse[inverse.size() - 1 - i];
        inverse[inverse.size() - 1 - i] = temp;
    }
    
    //���� ������ �ٽ� genome�� �����Ѵ�.
    for (int i = beg; i <= end; i++) {
        g.vecNodes[i] = inverse[i - beg];
    }
}

void GA::mutateDIVM(Genome& g) {
    mutateIVM(g);
    mutateDM(g);
}

void GA::mutateOneRand(Genome& g) { //�Ѱ��� ��ġ�� ��带 start, goal ��带 �����ϰ� ������ ��� �� �Ѱ��� ���� ������ �ٲ۴�.
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist1(0.0, 1.0); //uniform ������ �������� �Ͽ� 0~1 ������ �Ҽ��� �̴´�

    //���� Ȯ���� �����ص� Ȯ������ ���� ���;� �ش� Ȯ���̹Ƿ� �����ϰ� �� ũ�� ���� ���, �����Ѵ�.
    if ((dist1(mt) > probMutation))
    {
        return;
    }

    //�Ѱ��� ��带 �����ϵ� -1�� �ƴ� ���� �����Ѵ�.
    std::uniform_int_distribution<int> dist2(1, lenGenome - 2); // ù��°, ������ ��带 ���ܽ�Ű����� 1~������ 3��° ������ ���������Ѵ�.
    int pos = dist2(mt);

    vector<int> temp;
    for (int i = 0; i < lenGenome; i++) {
        if (g.vecNodes[i] != -1) {
            temp.push_back(g.vecNodes[i]);
        }
    }

    std::uniform_int_distribution<int> dist3(0, lenGenome - 1);
    int randNode = dist3(mt);
    while (std::find(temp.begin(), temp.end(), randNode) != temp.end()) { //���� vecNodes ���� �ߺ��Ǵ� Node�� �ִ��� �˻��Ѵ�. ���������� ��� �̴´�.
        randNode = dist3(mt);
    }
    
    g.vecNodes[pos] = randNode;

}

void GA::mutateMultiRand(int num, Genome& g) { //�Ѱ��� ��ġ�� ��带 start, goal ��带 �����ϰ� ������ ��� �� �Ѱ��� ���� ������ �ٲ۴�.
    for (int i = 0; i < num; i++) {
        mutateOneRand(g);
    }
}

void GA::reset() {
    //currentShortestPath = INF;
    currentLongestPath = 0;
    expectFitness = 0;
}

void GA::run() { 
    clock_t start, finish; //�ð� ������ ���� ����
    start = clock();
    ofstream fout;
    fout.open(LogGA);
    fout << "*********************parameter**********************\n";
    fout << "Genome ���� : " << numGenome << "\n";
    fout << "���� Ȯ�� : " << probCrossover << "\n";
    fout << "���� Ȯ�� : " << probMutation << "\n";
    fout << "���� ��� : " << crossoverSelect << "\n";
    fout << "���� ��� : " << mutationSelect << "\n";
    fout << "����Ƽ�� : " << elitismNum << "\n";
    fout << "*****************************************************\n";
    fout << "gen / shortest path / fitness���\n";
    //�Է¹��� ����ü�� ������ŭ ����
    for (int i = 0; i < numGenome; i++) {
        vecGenomes.push_back(Genome(lenGenome, startNode, goalNode));
    }
    for (int gen = 0; gen < generation; gen++) {
        reset();
        cout << "Gen " << gen << "���� ��..." << endl;
        fout << gen <<" ";
        recalcFitnessToAllGenome(); //��� ����ü�� fitness ��� �� �ο�
        cout << "current Shortest Path : " << currentShortestPath << endl;
        fout << currentShortestPath << " ";
        cout << "shortest Genome : ";
        for (int i = 0; i < lenGenome; i++) {
            if (shortestGenome.vecNodes[i] != -1) {
                cout << shortestGenome.vecNodes[i]<<" ";
            }
        }
        cout << endl;
        cout << "fitness ���(���� ���� ����) : " << expectFitness << endl;
        fout << expectFitness << "\n";

        vector<Genome> vecTempGnomes; //���� ���븦 ���� �ӽ� vector

        vector<int> elite;
        for (int i = 0; i < elitismNum; i++) { //����Ʈ���ǿ� ���� ���� ���� fitness�� ���� ����ü�� ellitismNum ���� ��ŭ ��.
            int bigFit = 0;
            int index = 0;
            for (int j = 0; j < numGenome; j++) {
                if (std::find(elite.begin(), elite.end(), j) == elite.end()) {
                    if (vecGenomes[j].fitness > bigFit) {
                        bigFit = vecGenomes[j].fitness;
                        index = j;
                    }
                }
            }
            elite.push_back(index);
        }
        for (int i = 0; i < elite.size(); i++) {
            vecTempGnomes.push_back(vecGenomes[elite[i]]);
        }

        while (vecTempGnomes.size() != numGenome) {
            Genome mom = roulette();
            Genome dad = roulette();

            Genome baby1, baby2;

            if (crossoverSelect == "PMX") {
                crossoverPMX(mom, dad, baby1, baby2);
            }
            else if (crossoverSelect == "OBX") {
                crossoverOBX(mom, dad, baby1, baby2);
            }
            else if (crossoverSelect == "PBX") {
                crossoverPBX(mom, dad, baby1, baby2);
            }

            if (mutationSelect == "SM") {
                mutateSM(baby1);
                mutateSM(baby2);
            }
            else if (mutationSelect == "DM") {
                mutateDM(baby1);
                mutateDM(baby2);
            }
            else if (mutationSelect == "IM") {
                mutateIM(baby1);
                mutateIM(baby2);
            }
            else if (mutationSelect == "IVM") {
                mutateIVM(baby1);
                mutateIVM(baby2);
            }
            else if (mutationSelect == "DIVM") {
                mutateDIVM(baby1);
                mutateDIVM(baby2);
            }
            else if (mutationSelect == "OneRand") {
                mutateOneRand(baby1);
                mutateOneRand(baby2);
            }
            else if (mutationSelect == "MultiRand") {
                mutateMultiRand(multiRand, baby1);
                mutateMultiRand(multiRand, baby2);
            }

            vecTempGnomes.push_back(baby1);
            vecTempGnomes.push_back(baby2);
        }
        vecGenomes = vecTempGnomes;
    }
    finish = clock();
    runningTime = (double)(finish - start);
    fout << "<runningTime> " << runningTime << "\n";
    fout.close();
}
//----------------------------------------------
