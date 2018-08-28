#ifndef NODE
#define NODE

/*
 * node.cpp
 *	This class implement the chord node. Which need to keep track of its key, the ip address, the fingertable
 *  	and the meanLatency between the adjacent nodes. each finger contains a record of its key, the ip address
 *	the latency between the node and finger and the qvalue used in q learning algorithm.
*/
#include <string>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <sstream>
#include <algorithm>
#include <climits>

using namespace std;

const int ORDER=14;             //the size of chord ring will be 2^order
const int NUMBER=400;	        //the number of nodes inside chord ring

struct finger
{
	int key;			   //finger key
	string ip;                         //finger ip
	double latency;                    //latency from node to finger
	double qvalue;                     //q value of the action Q(node, finger)
};

struct node
{
	int key;                      	    //node key
	string ip;                          //node ip
	double meanLate;                    //mean latency used in server selection algorithm
	vector<finger> fingerTable;         //succinct list(the node index has been stored)
	vector<vector<double> > qtable;	    //this is the q table used for classic q learning
                                                                //for server selection, update the mean latency of the node
};
int findClosest(int arr[], int size, int key);              //find the closest available node on the chord ring
int findClosestInclude(int arr[], int size, int key);       //find the closest available node on the chord ring including itself
int distance(int a, int b, int size);                       //find the distance between two nodes on the chord ring
vector<finger> findFinger(int arr[], int ind, vector<vector<double> > dist);      	//find the nodes in finger table
void updateMeanLate(node& n);   							//find mean latency of node's finger
double findMaxQ(node& n);								//for my version of q learning, find maximum q value in the figure table
double findMaxQt(node& n, int cutoff);							//for classic version of q learning, find maximum q value in the q table
void updateQ(node& n, int action, int targetKey, double propagateQ);			//for my version of q learning, update q value in the figure table 
void updateQt(node& n, int action, int targetKey, double propagateQ, int cutoff); 	//for classic version of q learning, update q value in the q table
string itos(int i);
#endif
