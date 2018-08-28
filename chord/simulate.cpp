/*
* This is the simulator for the Chord ring peer-to-peer lookup algorithms, only the basic functions related to the research project are provided. A decentralized, scalable and efficient
* learning algorithm(cooperative q learning) has been developed and compared with the other three algorithm including greedy algorithm, server selection algorithm, and classic q learning.
* This code and the algorithm developed is the individual product of Chaolun Wang.
*/
#include <cmath>
#include <cstdlib>
#include <time.h>
#include <algorithm>
#include <limits.h>
#include <iomanip>
#include "node.h"
#include "network.h"


using namespace std;
const int SAMP=500;           //sampling period   
const int ITER=10000;	      //the total number of lookups performed by all the nodes inside chord ring

struct package                //return value of rpc
{
	string ip;				//the ip found by RPC
	double totalTime;			//the accumulative time on the RPC route
	double propagation;			//the largest Q vaule of the last node.
};


//find the next node for a hop. the candidate is selected from succinctList and finger table by different approaches. The return value is the index of the next node in finger table.
//flag==0: greedy approach; flag==1: server selection; flag==2: reinforcment learning; flag==3: reinforcement learning without updation; flag==4 classic q learning
int findNext(node& n, int targetKey, int flag);


//find the whole routing of lookup, output the total latency
//flag==0: greedy approach; flag==1: server selection; flag==2: reinforcment learning; flag==3: reinforcement learning without updation; flag==4 classic q learning
package route(network& net, node& n, int targetKey, int flag);


//calculate and record the moving average for each of the algorithm implemented.
void evaluate(vector<double>& record, int& iter, double& average, network& net, string ip, int targetKey, int flag, int sampling);


int main()
{
	//randomly generate a nodelist which maps on the index of chord ring
	srand(time(NULL));
	
	int ringSize=1<<(ORDER);
	network net(ringSize, NUMBER);

	//testing
	vector<double> greedy, serverSelect, cooperativeQ, classicQ;
	double aveg=0, aves=0, aveco=0, avecl=0;
	for(int i=0; i<ITER; ++i)
	{
		int randStart=rand()%NUMBER;
		int randEnd=rand()%ringSize;
		string randomIP=itos(randStart);
		//there will no interfere between different server finding methods when performing lookups.
		evaluate(greedy, i, aveg, net, randomIP, randEnd, 0, SAMP);
		evaluate(serverSelect, i, aves, net, randomIP, randEnd, 1, SAMP);
		evaluate(cooperativeQ, i, aveco, net, randomIP, randEnd, 2, SAMP);
		evaluate(classicQ, i, avecl, net, randomIP, randEnd, 4, SAMP);
	}
	cout<<"Comparison of the lookup latency for different lookup algorithms, with number of node: "<<NUMBER<<"  number of iteration: "<<ITER<<'\n';
	int w=20;
	cout << left << setw(w) <<"No. lookups:";
	cout << left << setw(w) <<"greedy approach:";
	cout << left << setw(w) <<"server selec:";
	cout << left << setw(w) <<"cooperative ql:";
	cout << left << setw(w) <<"classic ql"<<'\n';
	for(int i=0; i<greedy.size(); ++i)
	{
		cout << left << setw(w) << (i+1)*SAMP;
		cout << left << setw(w) << greedy[i];
		cout << left << setw(w) << serverSelect[i]; 
		cout << left << setw(w) << cooperativeQ[i]; 
		cout << left << setw(w) << classicQ[i];
		cout<<'\n';
	}

}

//find the next node for a hop. the candidate is selected from succinctList and finger table by different approaches. The return value is the index of the next node in finger table.
//flag==0: greedy approach; flag==1: server selection; flag==2: reinforcment learning; flag==3: reinforcement learning without updation; flag==4 classic q learning
int findNext(node& n, int targetKey, int flag)
{
	int size=1<<(ORDER);
	if(distance(n.key, targetKey, size)<distance(n.fingerTable[0].key, targetKey, size))
		return -1;

	int result;
	double min;

	//0: greedy approach: find the longest finger
	if(flag==0)
	{	
		min=INT_MAX;
		for(int i=0; i<n.fingerTable.size(); ++i)
		{
			if(distance(n.fingerTable[i].key, targetKey, size)<min)
			{
				result=i;
				min=distance(n.fingerTable[i].key, targetKey, size);
			}
		}
	}
	//1: server selection
	else if(flag==1)
	{
		//double N=(double)distance(n.key, n.succinct.back(), size)/size*numberSuccinct;
		double N=NUMBER;
		min=INT_MAX;
		int cutoff=0;
		for(int i=0; i<n.fingerTable.size(); ++i)
		{
			if(distance(n.key, targetKey, size)<distance(n.fingerTable[i].key, targetKey, size))
				break;
			++cutoff;
		}
		for(int i=0; i<cutoff; ++i)
		{
			double di=n.fingerTable[i].latency;
			double dmean=n.meanLate;
			double estimate=di+dmean*log2((double)distance(n.fingerTable[i].key, targetKey, size)/size*N);
			if(estimate<min)
			{
				result=i;
				min=estimate;
			}
		}
	}
	//2,3: cooperative q learning(my approach)
	else if(flag==2 || flag==3)
	{

		double noize=0.00;		//exploration rate, no need to explore, since different lookups will share experience between each other
		int cutoff=0;
		//deterimne feasiable actions
		for(int i=0; i<n.fingerTable.size(); ++i)
		{
			if(distance(n.key, targetKey, size)<distance(n.fingerTable[i].key, targetKey, size))
				break;
			++cutoff;
		}
		//selection
		double max=INT_MIN;
		if((double)rand()/INT_MAX>noize)
		{
			for(int i=0; i<cutoff; ++i)
			{
				double tempq=n.fingerTable[i].qvalue/*+(double)rand()/INT_MAX*/;
				if(tempq>=max)
				{
					result=i;
					max=tempq;
				}
			}
			//}
		}
		else
			result=rand()%cutoff;	
	}
	//4: classic q learning
	else if(flag==4)
	{

		double noize=0.02;		//exploration rate
		int cutoff=0;
		//deterimne feasiable actions
		for(int i=0; i<n.fingerTable.size(); ++i)
		{
			if(distance(n.key, targetKey, size)<distance(n.fingerTable[i].key, targetKey, size))
				break;
			++cutoff;
		}
		//selection
		double max=INT_MIN;
		if((double)rand()/INT_MAX>noize)
		{
			for(int i=0; i<cutoff; ++i)
			{
				double tempq=n.qtable[cutoff-1][i]/*+(double)rand()/INT_MAX*/;
				if(tempq>=max)
				{
					result=i;
					max=tempq;
				}
			}
		}
		else
			result=rand()%cutoff;	
	}
	return result;
}



//find the whole routing of lookup, output the total latency
//flag==0: greedy approach; flag==1: server selection; flag==2: reinforcment learning; flag==3: reinforcement learning without updation; flag==4 classic q learning
package route(network& net, node& n, int targetKey, int flag)
{
	//node next;
	package result;
	int size=1<<(ORDER);

	int fingerIndex=findNext(n, targetKey, flag);
	if(fingerIndex==-1)
	{
		
		result.ip=n.ip;
		result.totalTime=0;
		if(flag!=4)
			result.propagation=findMaxQ(n);
		else
			result.propagation=50;    //final reward for classic q learning
		return result;
	}
	string nextIp=n.fingerTable[fingerIndex].ip;
	
	int cutoff=0;
	//deterimne feasiable actions
	for(int i=0; i<n.fingerTable.size(); ++i)
	{
		if(distance(n.key, targetKey, size)<distance(n.fingerTable[i].key, targetKey, size))
			break;
		++cutoff;
	}
	//////////////////////////
	//begin recursive section:
	
	package receive=route(net, net.visit[nextIp], targetKey, flag);	//this is the only place need to refer to net.request. For the version with RPC, this should be an internet hop

	//end recursive section.
	/////////////////////////
	if(flag==4)
	{
		result.propagation=findMaxQt(n, cutoff);
		updateQt(n, fingerIndex, targetKey, receive.propagation, cutoff);
	}
	else
		result.propagation=findMaxQ(n);
	if(flag==2)
		updateQ(n, fingerIndex, targetKey, receive.propagation);
		
	result.ip=receive.ip;
	result.totalTime=receive.totalTime+n.fingerTable[fingerIndex].latency;
	

	return result;
}


//calculate and record the moving average for each of the algorithm implemented.
void evaluate(vector<double>& record, int& iter, double& average, network& net, string ip, int targetKey, int flag, int sampling)
{
	package ret=route(net, net.visit[ip], targetKey, flag);
	double latency=ret.totalTime;
	if((iter+1)%sampling==0)
	{
		record.push_back(average/sampling);
		average=0;
	}
	else
		average+=latency;
}
