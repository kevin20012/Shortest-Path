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
//���� ��
// ������
// 	: Sigma Scaling ����
// ���� ������ 
// : Roulette wheel selection ����
// 
//--------------Parameter---------------
//1. ��ü�� ũ�� : numGenome ����
//2. ���� Ȯ��
//3. �������� Ȯ��
//2. ���� ������ : PMX, OBX, PBX ���� 
//3. ���� ������ : SM, DM, IM, IVM, DIVM ����
//----------------------------------
private:
	int** weight;
	int startNode;
	int goalNode;
	vector<Genome> vecGenomes; //����ü��
	int lenGenome; //�Ѱ��� ����ü�� ����
	int numGenome; //�� ���뿡���� �� ����ü ����
	int generation; //����
	int currentShortestPath; //������� ���� ª�� �Ÿ�
	int currentLongestPath; //������� ���� �� �Ÿ�
	Genome shortestGenome; //������� ���� �Ÿ� �� ���� ª�� �Ÿ��� ������ ����ü
	double expectFitness; //�� ���뿡���� ��� ����ü�� �������� ��հ�
	int elitismNum; //����Ƽ���� �̿��Ҷ�, ����Ʈ�� ����
	float probCrossover; //0~1������ �� - ����Ȯ��
	float probMutation; //0~1������ �� - ���� Ȯ��
	string crossoverSelect; //���� ��� ���ϱ� ���� ����
	string mutationSelect; //���� ��� ���ϱ� ���� ����
	int multiRand; //MultiRand ���̸� �̿��Ҷ�, ��� ��带 �����ϰ� ������ ������
	double runningTime; //�ҿ�ð�
public:
	GA(int** g, int nodeCount, int startNode, int goalNode, int numGenome, int generation, double probCrossover, double probMutation, string crossoverSelect, string mutationSelect, int elitismNum, int multiRand);
	// fitness�� �������� �ؿ� ����� ��
	// ���۳�� -> ������� ������ �Ÿ� ��� �Լ�
	int calcDistance(Genome&);
	// fitness ���� �� �� Genome�� �й����ִ� �Լ�
	void recalcFitnessToAllGenome();
	// ���� ������ - Roulette �Լ�
	Genome& roulette();

	// ���� ������ - �θ� 2�� ���� -> �ڼ� 2�� ź��
	// PMX
	void crossoverPMX(Genome& p1, Genome& p2, Genome& o1, Genome& o2);
	// OBX
	void crossoverOBX(Genome& p1, Genome& p2, Genome& o1, Genome& o2);
	// PBX
	void crossoverPBX(Genome& p1, Genome& p2, Genome& o1, Genome& o2);

	//���� ������
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

	//���� �ʱ�ȭ �Լ�
	void reset();
	//���� ���� �Լ�
	void run();
};
#endif
