/*
 * ClusterTree.h
 *
 *  Created on: 13/07/2021
 *      Author: jdou557
 */

#ifndef PHY_CLUSTERTREE_H_
#define PHY_CLUSTERTREE_H_


#include <iostream>
#include <vector>

#include "Tree.h"
#include "Node.h"
#include "NodeX.h"

using namespace std;


enum class LinkType {
	single, complete, upgma, mean, centroid, adjcomplete, neighborjoining
};


class ClusterTree : public Tree {
public:
	ClusterTree(Alignment* alignment, LinkType linkType);
	Node* buildClusterer();

	static string getDefaultLinkType();
	static vector<string> getDomain();
	static LinkType getLinkType(string val);

private:
	void neighborJoining(int clusters, vector<vector<int>>& clusterID, vector<NodeX*>& clusterNodes);
	void doLinkClustering(int clusters, vector<vector<int>>& clusterID, vector<NodeX*>& clusterNodes);
	void merge(int min1, int min2, double dist1, double dist2, vector<vector<int>>& clusterID, vector<NodeX*>& clusterNodes);
	double getDistance0(vector<int> cluster1, vector<int> cluster2);
	double getDistance(int taxon1, int taxon2);
	double getDistance(vector<double> pattern1, vector<double> pattern2);
	double getDistance(vector<vector<double>> distance, vector<int> cluster1, vector<int> cluster2);

	LinkType linkType;
	vector<string> taxaNames;
	bool distanceIsBranchLength;

};

#endif /* PHY_CLUSTERTREE_H_ */
