#ifndef GA_H
#define GA_H
#include <vector>
using std::vector;
using std::string;

class Genome {
public:
	vector<int> vecNodes;
	double fitness;
public:
	Genome();
	Genome(int numNodes, int startNode, int goalNode);
};

class GA { 
//고정 값
// 적응도
// 	: Sigma Scaling 고정
// 선택 연산자 
// : Roulette wheel selection 고정
// 
//--------------Parameter---------------
//1. 개체군 크기 : numGenome 수정
//2. 교배 확률
//3. 돌연변이 확률
//2. 교배 연산자 : PMX, OBX, PBX 선택 
//3. 변이 연산자 : SM, DM, IM, IVM, DIVM 선택
//----------------------------------
private:
	int** weight;
	int startNode;
	int goalNode;
	vector<Genome> vecGenomes; //염색체들
	int lenGenome; //한개의 염색체의 길이
	int numGenome; //한 세대에서의 총 염색체 개수
	int generation; //세대
	int currentShortestPath; //현재까지 가장 짧은 거리
	int currentLongestPath; //현재까지 가장 긴 거리
	Genome shortestGenome; //현재까지 구한 거리 중 가장 짧은 거리를 가지는 염색체
	double expectFitness; //한 세대에서의 모든 염색체의 적응도의 평균값
	int elitismNum; //엘리티즘을 이용할때, 엘리트의 개수
	float probCrossover; //0~1사이의 값 - 교배확률
	float probMutation; //0~1사이의 값 - 변이 확률
	string crossoverSelect; //교배 방법 정하기 위한 변수
	string mutationSelect; //변이 방법 정하기 위한 변수
	int multiRand; //MultiRand 변이를 이용할때, 몇개의 노드를 랜덤하게 변경할 것인지
	double runningTime; //소요시간
public:
	GA(int** g, int nodeCount, int startNode, int goalNode, int numGenome, int generation, double probCrossover, double probMutation, string crossoverSelect, string mutationSelect, int elitismNum, int multiRand);
	// fitness는 높을수록 해에 가까운 값
	// 시작노드 -> 최종노드 사이의 거리 계산 함수
	int calcDistance(Genome&);
	// fitness 재계산 후 각 Genome에 분배해주는 함수
	void recalcFitnessToAllGenome();
	// 선택 연산자 - Roulette 함수
	Genome& roulette();

	// 교배 연산자 - 부모 2개 선택 -> 자손 2개 탄생
	// PMX
	void crossoverPMX(Genome& p1, Genome& p2, Genome& o1, Genome& o2);
	// OBX
	void crossoverOBX(Genome& p1, Genome& p2, Genome& o1, Genome& o2);
	// PBX
	void crossoverPBX(Genome& p1, Genome& p2, Genome& o1, Genome& o2);

	//변이 연산자
	// SM
	void mutateSM(Genome&);
	// DM
	void mutateDM(Genome&);
	// IM
	void mutateIM(Genome&);
	// IVM
	void mutateIVM(Genome&);
	// DIVM
	void mutateDIVM(Genome&);
	//OneRand
	void mutateOneRand(Genome&);
	//MultiRand
	void mutateMultiRand(int num, Genome&);

	//변수 초기화 함수
	void reset();
	//최종 실행 함수
	void run();
};
#endif
