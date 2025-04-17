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

//Genome 한개의 클래스 -----------------------------------
Genome::Genome() { //염색체를 랜덤하게 만들지 않고, 그냥 두고 생성만 함.
	fitness = 0;
}
Genome::Genome(int numNodes, int startNode, int goalNode) { //무작위 경로로 permutation하게 염색체를 만듦.
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> dist(0, 2*numNodes); 
    //uniform 분포를 가지도록함.
    //0~numNodes-1 까지는 실제 노드로 사용하고
    //그 이상 값이 나오면 -1을 넣는 용도로 사용한다.

    int nextNode;
    for (int i = 0; i < numNodes-2; i++) {
        nextNode = dist(mt); //다음 노드의 후보를 무작위로 선택한다.
        if (nextNode >= numNodes) { //numNodes가 뽑히면, 건너뛰는 노드로 -1을 추가한다. -1은 건너뛴다는 의미다.
            //예를 들어, 0, -1, -1, 6, -1, 5, 7 => 0, 6, 5, 7 순으로 노드를 이동하는 것과 같다.
            vecNodes.push_back(-1);
            continue;
        }
        while (nextNode == startNode || nextNode == goalNode || std::find(vecNodes.begin(), vecNodes.end(), nextNode) != vecNodes.end()) { //현재 vecNodes 내에 중복되는 Node가 있는지 검사한다. 없을때까지 계속 뽑는다.
            nextNode = dist(mt);
        }
        if (nextNode >= numNodes) {
            vecNodes.push_back(-1);
            continue;
        }
        else {
            vecNodes.push_back(nextNode); //중복되는 것이 없을 경우, vecNodes에 삽입한다.
        }
        
    }
    //처음과 끝에 시작노드와 끝노드를 삽입한다.
    vecNodes.insert(vecNodes.begin(), startNode);
    vecNodes.push_back(goalNode);
}


//----------------------------------------------

//GA 알고리즘 클래스 ------------------------------------
GA::GA(int** g, int nodeCount, int startNode, int goalNode, int numGenome, int generation, double probCrossover, double probMutation, string crossoverSelect, string mutationSelect, int elitismNum, int multiRand)
    :startNode(startNode), goalNode(goalNode), lenGenome(nodeCount), numGenome(numGenome), generation(generation), currentShortestPath(INF), currentLongestPath(0), expectFitness(0), probCrossover(probCrossover), probMutation(probMutation), elitismNum(elitismNum), multiRand(multiRand){
    //weight 가중치 동적할당
    weight = new int* [nodeCount];
    for (int i = 0; i < nodeCount; i++) {
        weight[i] = new int[nodeCount];
    }
    for (int i = 0; i < nodeCount; i++) { //값 복사
        for (int j = 0; j < nodeCount; j++) {
            weight[i][j] = g[i][j];
        }
    }
    //유효성 체크 - crossoverParameter
    if (crossoverSelect == "PMX" || crossoverSelect == "OBX" || crossoverSelect == "PBX") {
        this->crossoverSelect = crossoverSelect;
    }
    else {
        cout << "wrong input!" << endl;
        exit(0);
    }
    //유효성 체크 - mutationParameter
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
    //현재 구해진 모든 염색체의 fitness 값 구한뒤 스케일링 후 재분배
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
    //fitness = currentLongestPath - 자신의 거리 (이렇게 하면 자신의 거리가 작을 수록 적응도가 커짐)
    //fitness 평균구하기
    for (int i = 0; i < numGenome; i++) {
        sum += 1.0/Dist[i];
    }
    expectFitness = sum/numGenome; //fitness 의 평균

    for (int i = 0; i < numGenome; i++) {
        vecGenomes[i].fitness = (1.0 / Dist[i]) / sum;
    }
}

Genome& GA::roulette() { //GA 첨부파일 참조.
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist(0.0, 1.0); //uniform 분포를 가지도록 하여 0~1 사이의 소수를 뽑는다.
    
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
    //cout << "당첨 : " << cfTotal<<"/"<<fSlice<<"->" << SelectedGenome << endl;
    return vecGenomes[SelectedGenome];
}

void GA::crossoverPMX(Genome& p1, Genome& p2, Genome& o1, Genome& o2) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist1(0.0, 1.0); //uniform 분포를 가지도록 하여 0~1 사이의 소수를 뽑는다.

    o1.vecNodes = p1.vecNodes;
    o2.vecNodes = p2.vecNodes;

    //랜덤 확률이 설정해둔 확률보다 적게 나와야 해당 확률이므로 진행하고 더 크게 나온 경우, 종료한다.
    //만약 두 부모가 같은 Genome인 경우에도 종료한다.
    if ((dist1(mt) > probCrossover) || (&p1 == &p2))
    {
        return;
    }
    vector<int> availableNode_o1;
    vector<int> availableNode_o2;
    int count = 0, count2=0;
    for (int i = 1; i < lenGenome-1; i++) { //시작, 목표노드 제외
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
    //우선 영역을 랜덤하게 선택한다.
    std::uniform_int_distribution<int> dist2(0, minSize-1); // 첫번째, 마지막 노드를 제외시키기우해 1~끝에서 3번째 노드까지 뽑히도록한다.
    int beg = dist2(mt);
    std::uniform_int_distribution<int> dist3(beg, minSize-1);
    int end = dist3(mt);

    //주어진 영역에 대해 o1, o2를 swap 한다.
    vector<int>::iterator posGene1, posGene2;
    int node1, node2;
    for (int i = beg; i <= end; i++) {
        node1 = availableNode_o1[i];
        node2 = availableNode_o2[i];
        if (node1 != node2) { //두 노드가 같지 않을때만 swap한다.
            //find and swap them in o1
            posGene1 = find(availableNode_o1.begin(), availableNode_o1.end(), node1);
            posGene2 = find(availableNode_o1.begin(), availableNode_o1.end(), node2);

            if (posGene2 == availableNode_o1.end()) { //찾지 못한 경우, 그냥 해당 값으로 교체한다,
                *posGene1 = node2;
            }
            else {
                std::swap(*posGene1, *posGene2);
            }

            //and in o2
            posGene1 = find(availableNode_o2.begin(), availableNode_o2.end(), node1);
            posGene2 = find(availableNode_o2.begin(), availableNode_o2.end(), node2);

            if (posGene1 == availableNode_o2.end()) { //찾지 못한 경우, 그냥 해당 값으로 교체한다,
                *posGene2 = node1;
            }
            else {
                std::swap(*posGene1, *posGene2);
            }
        }

        //다시 o1,o2수정
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
    std::uniform_real_distribution<double> dist1(0.0, 1.0); //uniform 분포를 가지도록 하여 0~1 사이의 소수를 뽑는다.

    o1.vecNodes = p1.vecNodes;
    o2.vecNodes = p2.vecNodes;

    //랜덤 확률이 설정해둔 확률보다 적게 나와야 해당 확률이므로 진행하고 더 크게 나온 경우, 종료한다.
    //만약 두 부모가 같은 Genome인 경우에도 종료한다.
    if ((dist1(mt) > probCrossover) || (&p1 == &p2))
    {
        return;
    }

    //유전자 개수의 1/3 정도의 인덱스를 뽑아 진행한다. - 이 경우에도, 첫번째, 마지막 노드는 제외한다.
    std::uniform_int_distribution<int> dist2(1, lenGenome-2);
    vector<int> index;
    int temp = 0;
    for (int i = 0; i < lenGenome/3; i++) {
        temp = dist2(mt); //다음 노드의 후보를 무작위로 선택한다.
        while (std::find(index.begin(), index.end(), temp) != index.end()) { //현재 vecNodes 내에 중복되는 Node가 있는지 검사한다. 없을때까지 계속 뽑는다.
            temp = dist2(mt);
        }
        index.push_back(temp); //중복되는 것이 없을 경우, vecNodes에 삽입한다.
    }
    //순서를 기억
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
    //o1,o2 에 순서 전이
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
    std::uniform_real_distribution<double> dist1(0.0, 1.0); //uniform 분포를 가지도록 하여 0~1 사이의 소수를 뽑는다.

    o1.vecNodes = p1.vecNodes;
    o2.vecNodes = p2.vecNodes;

    //랜덤 확률이 설정해둔 확률보다 적게 나와야 해당 확률이므로 진행하고 더 크게 나온 경우, 종료한다.
    //만약 두 부모가 같은 Genome인 경우에도 종료한다.
    if ((dist1(mt) > probCrossover) || (&p1 == &p2))
    {
        return;
    }

    //유전자 개수의 1/3 정도의 인덱스를 뽑아 진행한다. - 이 경우에도, 첫번째, 마지막 노드는 제외한다.
    std::uniform_int_distribution<int> dist2(1, lenGenome - 2);
    vector<int> index;
    int temp = 0;
    for (int i = 0; i < lenGenome / 3; i++) {
        temp = dist2(mt); //다음 노드의 후보를 무작위로 선택한다.
        while (std::find(index.begin(), index.end(), temp) != index.end()) { //현재 vecNodes 내에 중복되는 Node가 있는지 검사한다. 없을때까지 계속 뽑는다.
            temp = dist2(mt);
        }
        index.push_back(temp); //중복되는 것이 없을 경우, vecNodes에 삽입한다.
    }

    //순서를 기억
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

    //fix 된 요소들 외의 나머지 요소들을 중복 피해 옮기기
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
    std::uniform_real_distribution<double> dist1(0.0, 1.0); //uniform 분포를 가지도록 하여 0~1 사이의 소수를 뽑는다

    //랜덤 확률이 설정해둔 확률보다 적게 나와야 해당 확률이므로 진행하고 더 크게 나온 경우, 종료한다.
    if ((dist1(mt) > probMutation))
    {
        return;
    }

    //우선 영역을 랜덤하게 선택한다.
    std::uniform_int_distribution<int> dist2(1, lenGenome - 3); // 첫번째, 마지막 노드를 제외시키기우해 1~끝에서 3번째 노드까지 뽑히도록한다.
    int beg = dist2(mt);

    std::uniform_int_distribution<int> dist3(beg, lenGenome - 2); //beg에서부터 마지막에서 2번째노드까지 뽑히도록 한다.
    int end = dist3(mt);

    //주어진 영역을 섞는다.
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
    //섞인 영역을 다시 genome에 대입한다.
    for (int i = beg; i <= end; i++) {
        g.vecNodes[i] = randIndex[i-beg];
    }
}

void GA::mutateDM(Genome& g) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist1(0.0, 1.0); //uniform 분포를 가지도록 하여 0~1 사이의 소수를 뽑는다

    //랜덤 확률이 설정해둔 확률보다 적게 나와야 해당 확률이므로 진행하고 더 크게 나온 경우, 종료한다.
    if ((dist1(mt) > probMutation))
    {
        return;
    }

    //우선 영역을 랜덤하게 선택한다.
    std::uniform_int_distribution<int> dist2(1, lenGenome - 3); // 첫번째, 마지막 노드를 제외시키기위 1~끝에서 3번째 노드까지 뽑히도록한다.
    int beg = dist2(mt);

    std::uniform_int_distribution<int> dist3(beg, lenGenome - 2); //beg에서부터 마지막에서 2번째노드까지 뽑히도록 한다.
    int end = dist3(mt);
    //영역의 값을 저장
    vector<int> temp;
    for (int i = beg; i <= end; i++) {
        temp.push_back(g.vecNodes[i]);
    }
    //영역의 값 일단 vecNodes에서 삭제
    for (int i = 0; i < end-beg+1; i++) {
        g.vecNodes.erase(g.vecNodes.begin() + beg);
    }
    //옮길 위치의 인덱스를 뽑는다.
    std::uniform_int_distribution<int> dist4(1, g.vecNodes.size() - 2); // 첫번째, 마지막 노드를 제외시키기위해 1~끝에서 3번째 노드까지 뽑히도록한다.
    int moveTo = dist4(mt);
    //옮기기
    for (int i = 0; i < temp.size(); i++) {
        g.vecNodes.insert(g.vecNodes.begin() + moveTo, temp[i]);
    }

}

void GA::mutateIM(Genome& g) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist1(0.0, 1.0); //uniform 분포를 가지도록 하여 0~1 사이의 소수를 뽑는다

    //랜덤 확률이 설정해둔 확률보다 적게 나와야 해당 확률이므로 진행하고 더 크게 나온 경우, 종료한다.
    if ((dist1(mt) > probMutation))
    {
        return;
    }

    //옮겨질 위치의 인덱스를 뽑는다.
    std::uniform_int_distribution<int> dist2(1, lenGenome -2); // 첫번째, 마지막 노드를 제외시키기우해 1~끝에서 3번째 노드까지 뽑히도록한다.
    int moveFrom = dist2(mt);

    //옮길 위치의 인덱스를 뽑는다.
    std::uniform_int_distribution<int> dist3(1, lenGenome - 2); // 첫번째, 마지막 노드를 제외시키기우해 1~끝에서 3번째 노드까지 뽑히도록한다.
    int moveTo = dist3(mt);

    //옮기기
    int temp = g.vecNodes[moveFrom];
    g.vecNodes.erase(g.vecNodes.begin() + moveFrom);
    g.vecNodes.insert(g.vecNodes.begin() + moveTo, temp);
}

void GA::mutateIVM(Genome& g) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist1(0.0, 1.0); //uniform 분포를 가지도록 하여 0~1 사이의 소수를 뽑는다

    //랜덤 확률이 설정해둔 확률보다 적게 나와야 해당 확률이므로 진행하고 더 크게 나온 경우, 종료한다.
    if ((dist1(mt) > probMutation))
    {
        return;
    }

    //우선 영역을 랜덤하게 선택한다.
    std::uniform_int_distribution<int> dist2(1, lenGenome - 3); // 첫번째, 마지막 노드를 제외시키기우해 1~끝에서 3번째 노드까지 뽑히도록한다.
    int beg = dist2(mt);

    std::uniform_int_distribution<int> dist3(beg, lenGenome - 2); //beg에서부터 마지막에서 2번째노드까지 뽑히도록 한다.
    int end = dist3(mt);

    //주어진 영역을 섞는다.
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
    
    //섞인 영역을 다시 genome에 대입한다.
    for (int i = beg; i <= end; i++) {
        g.vecNodes[i] = inverse[i - beg];
    }
}

void GA::mutateDIVM(Genome& g) {
    mutateIVM(g);
    mutateDM(g);
}

void GA::mutateOneRand(Genome& g) { //한개의 위치의 노드를 start, goal 노드를 제외하고 나머지 노드 중 한개를 랜덤 선택해 바꾼다.
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist1(0.0, 1.0); //uniform 분포를 가지도록 하여 0~1 사이의 소수를 뽑는다

    //랜덤 확률이 설정해둔 확률보다 적게 나와야 해당 확률이므로 진행하고 더 크게 나온 경우, 종료한다.
    if ((dist1(mt) > probMutation))
    {
        return;
    }

    //한개의 노드를 선택하되 -1이 아닌 값을 선택한다.
    std::uniform_int_distribution<int> dist2(1, lenGenome - 2); // 첫번째, 마지막 노드를 제외시키기우해 1~끝에서 3번째 노드까지 뽑히도록한다.
    int pos = dist2(mt);

    vector<int> temp;
    for (int i = 0; i < lenGenome; i++) {
        if (g.vecNodes[i] != -1) {
            temp.push_back(g.vecNodes[i]);
        }
    }

    std::uniform_int_distribution<int> dist3(0, lenGenome - 1);
    int randNode = dist3(mt);
    while (std::find(temp.begin(), temp.end(), randNode) != temp.end()) { //현재 vecNodes 내에 중복되는 Node가 있는지 검사한다. 없을때까지 계속 뽑는다.
        randNode = dist3(mt);
    }
    
    g.vecNodes[pos] = randNode;

}

void GA::mutateMultiRand(int num, Genome& g) { //한개의 위치의 노드를 start, goal 노드를 제외하고 나머지 노드 중 한개를 랜덤 선택해 바꾼다.
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
    clock_t start, finish; //시간 측정을 위한 변수
    start = clock();
    ofstream fout;
    fout.open(LogGA);
    fout << "*********************parameter**********************\n";
    fout << "Genome 개수 : " << numGenome << "\n";
    fout << "교배 확률 : " << probCrossover << "\n";
    fout << "변이 확률 : " << probMutation << "\n";
    fout << "교배 방법 : " << crossoverSelect << "\n";
    fout << "변이 방법 : " << mutationSelect << "\n";
    fout << "엘리티즘 : " << elitismNum << "\n";
    fout << "*****************************************************\n";
    fout << "gen / shortest path / fitness평균\n";
    //입력받은 염색체의 개수만큼 생성
    for (int i = 0; i < numGenome; i++) {
        vecGenomes.push_back(Genome(lenGenome, startNode, goalNode));
    }
    for (int gen = 0; gen < generation; gen++) {
        reset();
        cout << "Gen " << gen << "진행 중..." << endl;
        fout << gen <<" ";
        recalcFitnessToAllGenome(); //모든 염색체에 fitness 계산 후 부여
        cout << "current Shortest Path : " << currentShortestPath << endl;
        fout << currentShortestPath << " ";
        cout << "shortest Genome : ";
        for (int i = 0; i < lenGenome; i++) {
            if (shortestGenome.vecNodes[i] != -1) {
                cout << shortestGenome.vecNodes[i]<<" ";
            }
        }
        cout << endl;
        cout << "fitness 평균(높을 수록 좋음) : " << expectFitness << endl;
        fout << expectFitness << "\n";

        vector<Genome> vecTempGnomes; //다음 세대를 위한 임시 vector

        vector<int> elite;
        for (int i = 0; i < elitismNum; i++) { //엘리트주의에 의해 가장 높은 fitness를 가진 염색체가 ellitismNum 개수 만큼 들어감.
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
