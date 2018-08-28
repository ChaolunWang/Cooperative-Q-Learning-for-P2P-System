/*
 * network.cpp
 *	This class simulates the uderlying network for this study. Provide IP to node mapping.
 *  	The latency between nodes was set to be uniform distribution between 0 and 20ms by default
 *	Nodes are evenly distributed on the chord ring.
*/
#include "network.h"

network::network(int size, int nofNode)
{
	map<int,int> recorder;
	int ringSize=1<<ORDER;
	int counter=0;
	int nodeKey[nofNode];
	while(counter<nofNode)
	{
		int tempKey=rand()%ringSize;
		if(recorder.find(tempKey)==recorder.end())
		{
			recorder[tempKey]=1;
			nodeKey[counter]=tempKey;
			++counter;
		}
	}

	sort(nodeKey, nodeKey+nofNode);

	//bulid distance matrx;
	vector<vector<double> > distMatrix=buildDist(nofNode);

	//build network
	numberOfNode=nofNode;
	for(int i=0; i<NUMBER; ++i)
	{
		node temp;
		temp.key=nodeKey[i];
		//generate ip based on the value of index
		stringstream ss;
		ss<<i;
		ss>>temp.ip;

		temp.fingerTable=findFinger(nodeKey, i, distMatrix);
		updateMeanLate(temp);
		vector<vector<double> > tempTable;
		for(int j=0; j<temp.fingerTable.size(); ++j)
		{
			vector<double> tempVec;
			for(int k=0; k<temp.fingerTable.size(); ++k)
			{
				tempVec.push_back(0);
			}
			tempTable.push_back(tempVec);
		}
		temp.qtable=tempTable;
		visit[temp.ip]=temp;
	}
}

node network::request(string ip)
{
	return visit["ip"];
}



vector<vector<double> > network::buildDist(int size)                                 //build the distance matrix
{
	vector<vector<double> > result;
	for(int i=0; i<size; ++i)
	{
		double offset;
		if(rand()%100>4)						     // simulate the situation if there are nodes have bad connections
			offset=0;
		else
			offset=200;
		vector<double> temp;
		for(int j=0; j<size; ++j)
		{
			temp.push_back(rand()%20+offset);
		}
		result.push_back(temp);
	}
	return result;
}



