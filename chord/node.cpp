/*
 * node.cpp
 *	This class implement the chord node. Which need to keep track of its key, the ip address, the fingertable
 *  	and the meanLatency between the adjacent nodes. each finger contains a record of its key, the ip address
 *	the latency between the node and finger and the qvalue used in q learning algorithm.
*/
#include "node.h"

int findClosest(int arr[], int size, int key)        //find the closest available node on the chord ring
{
	for(int i=0; i<size; ++i)
	{
		if(arr[i]>key)
			return i;
	}
	return 0;
}

int findClosestInclude(int arr[], int size, int key)  //find the closest available node on the chord ring including itself
{
	for(int i=0; i<size; ++i)
	{
		if(arr[i]>=key)
			return i;
	}
	return 0;
}


int distance(int a, int b, int size)										//find the distance between two nodes on the chord ring
{
	if(a<=b)
		return b-a;
	return b-a+size;
}

vector<finger> findFinger(int arr[], int ind, vector<vector<double> > dist)        //find the nodes in finger table
{
	vector<int> indexList;
	vector<finger> result;
	int key=arr[ind];
	int ringSize=1<<ORDER;
	for(int i=0; i<ORDER; ++i)
	{
		int tempIndex=(key+(1<<i))%ringSize;
		//cout<<tempIndex<<'\n';
		indexList.push_back(findClosestInclude(arr, NUMBER, tempIndex));
	}
	for(int i=0; i<indexList.size(); ++i)
	{
		finger temp;
		temp.key=arr[indexList[i]];
		stringstream ss;
		ss<<indexList[i];
		ss>>temp.ip;
		temp.latency=dist[ind][indexList[i]];
		temp.qvalue=i;
		result.push_back(temp);
	}
	return result;
}

void updateMeanLate(node& n)                                                     //for server selection, update the mean latency of the node
{
	double sum=0;
	for(int i=0; i<n.fingerTable.size(); ++i)
		sum+=n.fingerTable[i].latency;
	n.meanLate=sum/n.fingerTable.size();
}

double findMaxQ(node& n)						//for my version of q learning, find maximum q value in the figure table
{
	double max=INT_MIN;
	for(int i=0; i<n.fingerTable.size(); ++i)
	{
		if(n.fingerTable[i].qvalue>=max)
			max=n.fingerTable[i].qvalue;
	}
	return max;
}

double findMaxQt(node& n, int cutoff)					//for classic version of q learning, find maximum q value in the q table
{
	double max=INT_MIN;
	for(int i=0; i<cutoff; ++i)
	{
		double tempq=n.qtable[cutoff-1][i]/*+(double)rand()/INT_MAX*/;
		if(tempq>=max)
			max=tempq;
	}
	return max;
}

void updateQ(node& n, int action, int targetKey, double propagateQ)	//for my version of q learning, update q value in the figure table 
{	
	double lr=0.9;			//learning rate 
	double lambda=1;		//decaying rate
	int size=1<<(ORDER);
	double loss=1.0*n.fingerTable[action].latency;
	double progress=(double)(distance(n.key, targetKey, size))/size;
	n.fingerTable[action].qvalue+=lr*(-loss/progress+lambda*propagateQ*(1-progress)-n.fingerTable[action].qvalue);
}

void updateQt(node& n, int action, int targetKey, double propagateQ, int cutoff) //for classic version of q learning, update q value in the q table
{
	double lr=0.9;			//learning rate 
	double lambda=0.8;		//decaying rate
	int size=1<<(ORDER);
	double loss=1.0*n.fingerTable[action].latency;
	n.qtable[cutoff-1][action]+=lr*(-loss+lambda*propagateQ-n.qtable[cutoff-1][action]);
}

string itos(int i)
{
	string result;
	stringstream ss;
	ss<<i;
	ss>>result;	
	return result;
}


