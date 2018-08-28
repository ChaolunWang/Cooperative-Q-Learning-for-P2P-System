#ifndef NETWORK
#define NETWORK

/*
 * network.cpp
 *	This class simulates the uderlying network for this study. Provide IP to node mapping.
 *  	The latency between nodes was set to be uniform distribution between 0 and 20ms by default
 *	Nodes are evenly distributed on the chord ring.
*/
#include <map>
#include <iostream>
#include <cstdlib>
#include <time.h>
#include <sstream>
#include "node.h"

using namespace std;

class network
{
	vector<vector<double> > buildDist(int size);

	public:
		network(int size, int nofNode);
		node request(string ip);
		map<string, node> visit;

	private:
		int numberOfNode;		
};



#endif
