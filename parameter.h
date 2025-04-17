#ifndef PARAMETER_H
#define PARAMETER_H
using std::string;
//************************Global Parameter
//Define INF
#define INF 9999999
//Setting node File
const string nodeFile = "./graph/1000_test5.txt";
//Setting hFunction File
const string hFunctionFile = "hFunction.txt";
//Setting Start, Goal Node
const int startNode = 0;
const int goalNode = 766;
//Setting log File
const string LogDijk = "./log/LOG_Dijk.txt";
const string LogAstar = "./log/LOG_Astar.txt";
const string LogGA = "./log/LOG_Ga.txt";

//************************Specific Parameter
//For Dikjstra, A*
const int hValueWeight = 3; //for A*, to make heuristic Value for each node


//For GA
const int numGenome = 100;
const int generation = 300;
const double probCrossover = 0.6;
const double probMutation = 0.8;
const string crossoverSelect = "PBX";  
const string mutationSelect = "MultiRand";
const int multiRand = 10;
const int elitismNum =0;

#endif