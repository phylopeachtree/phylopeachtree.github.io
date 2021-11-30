/*
 * ClusterTree.cpp
 *
 *  Created on: 13/07/2021
 *      Author: jdou557
 *      Modified from beast2 ClusterTree
 *      https://github.com/CompEvol/beast2/blob/master/src/beast/util/ClusterTree.java
 */

#include "ClusterTree.h"
#include "../aln/Alignment.h"
#include "../Utils.h"




ClusterTree::ClusterTree(Alignment* alignment, LinkType linkType){


	this->N_OPERATIONS_BEFORE_RETURN = 10000;
	this->finished = false;
	
	this->clusterNodes.clear();
	this->clusterID.clear();

	cout << "Making cluster tree" << endl;


	taxaNames = alignment->getNames();
	this->alignment = alignment;
	this->linkType = linkType;

	// Method
	distanceIsBranchLength = linkType == LinkType::neighborjoining;


	// Start building. Will finish later
	buildClusterer();




}


/*
 * Get the tree if it's ready
 */
Tree* ClusterTree::getTree(){
	
	

	if (!this->finished) {
		cout << "Tree is not ready yet" << endl;
		return nullptr;
	}
		
	int taxonCount = taxaNames.size();
		
	Node* root;
		
	// Pathological case
	if (taxonCount == 1) {
		root = new Node();
		root->setHeight(1);
		root->setNr(0);
	}
	
	else {
		
		// Get root
		for (int i = 0; i < taxonCount; i++) {
			if (this->clusterID.at(i).size() > 0) {
				root = clusterNodes.at(i)->toNode();
			}
		}
	}
	
	this->setRoot(root);
	
	
	// Ensure that last tip is at 0
	double latestTip = Utils::INFTY;
    for (int i = 0; i < this->nodes.size(); i++) {
    	latestTip = std::min(latestTip, this->nodes.at(i)->getHeight());
    }
    for (int i = 0; i < this->nodes.size(); i++) {
    	this->nodes.at(i)->setHeight(this->nodes.at(i)->getHeight() - latestTip);
    }


    this->initArray();
	
	return this;
	
}



string ClusterTree::getDefaultLinkType(){
	return "neighborjoining";
}

vector<string> ClusterTree::getDomain(){
	vector<string> domain = {"neighborjoining"};
	return domain;
}

LinkType ClusterTree::getLinkType(string val){


	if (val == "neighborjoining") return LinkType::neighborjoining;

	return LinkType::neighborjoining;


}


void ClusterTree::buildClusterer(){


    int taxonCount = taxaNames.size();


    if (taxonCount == 1) {
        // pathological case
        Node* node = new Node();
        node->setHeight(1);
        node->setNr(0);
		
		this->finished = true;
		
        return;
    }

    // use array of integer vectors to store cluster indices,
    // starting with one cluster per instance
    this->clusterID.resize(taxonCount);// = new ArrayList[taxonCount];
    for (int i = 0; i < taxonCount; i++) {
        this->clusterID.at(i).push_back(i);
    }
    // calculate distance matrix
    this->clusters = taxonCount;


    // used for keeping track of hierarchy
    this->clusterNodes.resize(taxonCount);

    
	/*
    if (linkType == LinkType::neighborjoining) {
        
    } else {
        doLinkClustering(clusters, clusterID, clusterNodes);
    }
	*/
	
	// Build the tree
	neighborJoining();



}


/*
 * Resume the clustering
 */
double ClusterTree::resumeClusterer(){
	

	
	int expectedNops = clusters*(clusters-1)/2.0 + clusters-1;
	const int n = taxaNames.size();
	int nOperations = 0;
	double d;


	int i;
	int j;
	for (i = this->distanceMatrix_i; i < clusters; i++) {
		
		this->dist.at(i).at(i) = 0;
		for (j = this->distanceMatrix_j; j < clusters; j++) {
			d = getDistance0(clusterID.at(i), clusterID.at(j));
			d = d / weightSum;
			this->dist.at(i).at(j) = d;
			this->dist.at(j).at(i) = d;
			nOperations ++;
			
			//cout << i << "," << j << ":" << d << endl;
			
			
			if (nOperations >= this->N_OPERATIONS_BEFORE_RETURN) {
				totalNumberOperations += nOperations;
				double progress = 1.0*totalNumberOperations/expectedNops;
				//cout << "Distance matrix returning " << progress << endl;
				this->distanceMatrix_i = i;
				this->distanceMatrix_j = j;
				return progress;
			}
		}
		
		this->distanceMatrix_j = i+2;
		
		
	}
	this->distanceMatrix_i = i;


	vector<double> separationSums(n);
	vector<double> separations(n);
	vector<int> nextActive(n);

	//calculate initial separation rows
	for (i = 0; i < n; i++) {
		double sum = 0;
		for (j = 0; j < n; j++) {
			sum += dist.at(i).at(j);
		}
		separationSums.at(i) = sum;
		separations.at(i) = sum / (clusters - 2);
		nextActive.at(i) = i + 1;
	}


	while (clusters > 2) {
		
		
		// find minimum
		int min1 = -1;
		int min2 = -1;
		double min = Utils::INFTY;
		{
			int i = 0;
			while (i < n) {
				double sep1 = separations.at(i);
				vector<double> row = dist.at(i);
				int j = nextActive[i];
				while (j < n) {
					const double sep2 = separations.at(j);
					const double val = row.at(j) - sep1 - sep2;
					if (val < min) {
						// new minimum
						min1 = i;
						min2 = j;
						min = val;
					}
					j = nextActive.at(j);
				}
				i = nextActive.at(i);
			}
		}
		// record distance
		const double minDistance = dist.at(min1).at(min2);
		clusters--;
		const double sep1 = separations.at(min1);
		const double sep2 = separations.at(min2);
		const double dist1 = (0.5 * minDistance) + (0.5 * (sep1 - sep2));
		const double dist2 = (0.5 * minDistance) + (0.5 * (sep2 - sep1));
		if (clusters > 2) {
			// update separations  & distance
			double newSeparationSum = 0;
			const double mutualDistance = dist[min1][min2];
			vector<double> row1 = dist.at(min1);
			vector<double> row2 = dist.at(min2);
			for (int i = 0; i < n; i++) {
				if (i == min1 || i == min2 || clusterID.at(i).size() == 0) {
					row1.at(i) = 0;
				} else {
					const double val1 = row1.at(i);
					const double val2 = row2.at(i);
					const double distance = (val1 + val2 - mutualDistance) / 2.0;
					newSeparationSum += distance;
					// update the separationsum of cluster i.
					separationSums.at(i) += (distance - val1 - val2);
					separations.at(i) = separationSums.at(i) / (clusters - 2);
					row1.at(i) = distance;
					dist.at(i).at(min1) = distance;
				}
			}
			separationSums.at(min1) = newSeparationSum;
			separations.at(min1) = newSeparationSum / (clusters - 2);
			separationSums.at(min2) = 0;
			merge(min1, min2, dist1, dist2, clusterID, clusterNodes);
			int prev = min2;
			// since min1 < min2 we havenActiveRows[0] >= 0, so the next loop should be save
			while (clusterID.at(prev).size() == 0) {
				prev--;
			}
			nextActive.at(prev) = nextActive.at(min2);
		} else {
			merge(min1, min2, dist1, dist2, clusterID, clusterNodes);
			break;
		}
	}



	for (int i = 0; i < n; i++) {
		if (clusterID.at(i).size() > 0) {
			for (int j = i + 1; j < n; j++) {
				if (clusterID.at(j).size() > 0) {
					const double dist1 = dist[i][j];
					if (clusterID.at(i).size() == 1) {
						merge(i, j, dist1, 0, clusterID, clusterNodes);
					} else if (clusterID.at(j).size() == 1) {
						merge(i, j, 0, dist1, clusterID, clusterNodes);
					} else {
						merge(i, j, dist1 / 2.0, dist1 / 2.0, clusterID, clusterNodes);
					}
					break;
				}
			}
		}
	}
	
	
	this->totalNumberOperations = -1;
	this->finished = true;
	return this->totalNumberOperations;

}




/*
 * Use neighbor joining algorithm for clustering
 * This is roughly based on the RapidNJ simple implementation and runs at O(n^3)
 */
void ClusterTree::neighborJoining(){


	const int n = taxaNames.size();
	
	
	
	// Weight sum
    this->weightSum = 0.0;
    for (int i = 0; i < alignment->getPatternCount(); i++) {
        this->weightSum += alignment->getPatternWeight(i);
	}
	
	// Create distance matrix
	this->dist.resize(clusters);
	for (int i = 0; i < clusters; i++) {
		this->dist.at(i).resize(clusters);
	}
	

	this->totalNumberOperations = 0;
	this->distanceMatrix_i = 0;
	this->distanceMatrix_j = 1;
	
	
	//while (this->totalNumberOperations != -1){
		//this->resumeClusterer();
	//}
	

	
}


/*
 * Perform clustering using a link method
 * This implementation uses a priority queue resulting in a O(n^2 log(n)) algorithm
 */
void ClusterTree::doLinkClustering(int clusters, vector<vector<int>>& clusterID, vector<NodeX*>& clusterNodes){

	/*
	 *
	 * TODO

    const int instances = taxaNames.size();
    const PriorityQueue<Tuple> queue = new PriorityQueue<>(clusters * clusters / 2, new TupleComparator());
    const double[][] distance0 = new double[clusters][clusters];
    for (int i = 0; i < clusters; i++) {
        distance0[i][i] = 0;
        for (int j = i + 1; j < clusters; j++) {

            distance0[i][j] = getDistance0(clusterID.at(i), clusterID.at(j));
            distance0[j][i] = distance0[i][j];
            queue.add(new Tuple(distance0[i][j], i, j, 1, 1));

        }
    }



    while (clusters > 1) {
        int min1 = -1;
        int min2 = -1;
        // use priority queue to find next best pair to cluster
        Tuple t;
        do {
            t = queue.poll();
        }
        while (t != nullptr && (clusterID.at(t.m_iCluster1()).size() != t.m_nClusterSize1() || clusterID.at(t.m_iCluster2()).size() != t.m_nClusterSize2()));
        min1 = t.m_iCluster1();
        min2 = t.m_iCluster2();
        merge(min1, min2, t.m_fDist()/2.0, t.m_fDist()/2.0, clusterID, clusterNodes);
        // merge  clusters

        //System.out.println("Merging " + min1 + " and " + min2);

        // update distances & queue
        for (int i = 0; i < instances; i++) {
            if (i != min1 && clusterID.at(i).size() != 0) {
                const int i1 = Math.min(min1, i);
                const int i2 = Math.max(min1, i);
                const double distance = getDistance(distance0, clusterID.at(i1), clusterID.at(i2));
                queue.add(new Tuple(distance, i1, i2, clusterID.at(i1).size(), clusterID.at(i2).size()));
            }
        }

        clusters--;
    }

    */

}

void ClusterTree::merge(int min1, int min2, double dist1, double dist2, vector<vector<int>>& clusterID, vector<NodeX*>& clusterNodes){

	if (min1 > min2) {
		const int h = min1;
		min1 = min2;
		min2 = h;
		const double f = dist1;
		dist1 = dist2;
		dist2 = f;
	}


	clusterID.at(min1).insert(clusterID.at(min1).end(), clusterID.at(min2).begin(), clusterID.at(min2).end());
	clusterID.at(min2).clear();

	//clusterID.at(min1).addAll(clusterID.at(min2));
	//clusterID.at(min2).removeAll(clusterID.at(min2));

	// track hierarchy
	NodeX* node = new NodeX(this->taxaNames);
	if (clusterNodes.at(min1) == nullptr) {
		node->m_iLeftInstance(min1);
	} else {
		node->m_left(clusterNodes.at(min1));
		clusterNodes.at(min1)->m_parent(node);
	}
	if (clusterNodes.at(min2) == nullptr) {
		node->m_iRightInstance(min2);
	} else {
		node->m_right(clusterNodes.at(min2));
		clusterNodes.at(min2)->m_parent(node);
	}
	if (this->distanceIsBranchLength) {
		node->setLength(dist1, dist2);
	} else {
		node->setHeight(dist1, dist2);
	}
	clusterNodes.at(min1) = node;

}

/*
 * calculate distance the first time when setting up the distance matrix
 */
double ClusterTree::getDistance0(vector<int> cluster1, vector<int> cluster2){
	return getDistance(cluster1.at(0), cluster2.at(0));
}


/*
 * Hamming distance
 */
double ClusterTree::getDistance(int taxon1, int taxon2){

    char state1, state2;

    int n = alignment->getPatternCount();

    double sumDistance = 0.0;
    //double sumWeight = 0.0;
	bool isNT = alignment->getIsNucleotide();

    vector<char> pattern;


    for (int i = 0; i < n; i++) {
        pattern = alignment->getPattern(i);

        state1 = pattern.at(taxon1);
		if (Alignment::isAmbiguousOrGap(state1, isNT)) continue;
		
		
        state2 = pattern.at(taxon2);
		if (Alignment::isAmbiguousOrGap(state2, isNT)) continue;
       

        if (state1 != state2) {
            sumDistance += alignment->getPatternWeight(i);
        }
        //sumWeight += weight;
    }


    return sumDistance;
}


/*
 * 1 - norm
 */
double ClusterTree::getDistance(vector<double> pattern1, vector<double> pattern2){
	double dist = 0;
	for (int i = 0; i < alignment->getPatternCount(); i++) {
		dist += alignment->getPatternWeight(i) * std::abs(pattern1[i] - pattern2[i]);
	}
	return dist;
}


/*
 * Calculate the distance between two clusters
 */
double ClusterTree::getDistance(vector<vector<double>> distance, vector<int> cluster1, vector<int> cluster2){


    double bestDist = Utils::INFTY;


    // Use if-else because cheerpj doesn't work as well with switch-case
    if (linkType == LinkType::single) {

    	// find single link distance aka minimum link, which is the closest distance between
        // any item in cluster1 and any item in cluster2
        bestDist = Utils::INFTY;
        for (int i = 0; i < cluster1.size(); i++) {
            const int i1 = cluster1.at(i);
            for (int j = 0; j < cluster2.size(); j++) {
                const int i2 = cluster2.at(j);
                const double dist = distance.at(i1).at(i2);
                if (bestDist > dist) {
                    bestDist = dist;
                }
            }
        }

    }else if (linkType == LinkType::complete || linkType == LinkType::adjcomplete) {

    	 // find complete link distance aka maximum link, which is the largest distance between
        // any item in cluster1 and any item in cluster2
        bestDist = 0;
        for (int i = 0; i < cluster1.size(); i++) {
            const int i1 = cluster1.at(i);
            for (int j = 0; j < cluster2.size(); j++) {
                const int i2 = cluster2.at(j);
                const double dist = distance.at(i1).at(i2);
                if (bestDist < dist) {
                    bestDist = dist;
                }
            }
        }

        if (linkType == LinkType::adjcomplete) {

            // calculate adjustment, which is the largest within cluster distance
            double maxDist = 0;
            for (int i = 0; i < cluster1.size(); i++) {
                const int i1 = cluster1.at(i);
                for (int j = i + 1; j < cluster1.size(); j++) {
                    const int i2 = cluster1.at(j);
                    const double dist = distance.at(i1).at(i2);
                    if (maxDist < dist) {
                        maxDist = dist;
                    }
                }
            }
            for (int i = 0; i < cluster2.size(); i++) {
                const int i1 = cluster2.at(i);
                for (int j = i + 1; j < cluster2.size(); j++) {
                    const int i2 = cluster2.at(j);
                    const double dist = distance.at(i1).at(i2);
                    if (maxDist < dist) {
                        maxDist = dist;
                    }
                }
            }
            bestDist -= maxDist;

        }

    }else if (linkType == LinkType::upgma) {


    	// finds average distance between the elements of the two clusters
        bestDist = 0;
        for (int i = 0; i < cluster1.size(); i++) {
            const int i1 = cluster1.at(i);
            for (int j = 0; j < cluster2.size(); j++) {
                const int i2 = cluster2.at(j);
                bestDist += distance.at(i1).at(i2);
            }
        }
        bestDist /= (cluster1.size() * cluster2.size());

    }else if (linkType == LinkType::mean) {


    	// calculates the mean distance of a merged cluster (akak Group-average agglomerative clustering)
        vector<int> merged;
        merged.insert(merged.end(), cluster1.begin(), cluster1.end());
        merged.insert(merged.end(), cluster2.begin(), cluster2.end());
        bestDist = 0;
        for (int i = 0; i < merged.size(); i++) {
            const int i1 = merged.at(i);
            for (int j = i + 1; j < merged.size(); j++) {
                const int i2 = merged.at(j);
                bestDist += distance.at(i1).at(i2);
            }
        }
        const int n = merged.size();
        bestDist /= (n * (n - 1.0) / 2.0);

    }else if (linkType == LinkType::centroid) {


			/*

    	// finds the distance of the centroids of the clusters
        const int patterns = alignment->getPatternCount();
        vector<double> centroid1(patterns);
        for (int i = 0; i < cluster1.size(); i++) {
            const int taxonIndex = cluster1.at(i);
            for (int j = 0; j < patterns; j++) {
                centroid1[j] += alignment->getPattern(taxonIndex, j);
            }
        }
        vector<double> centroid2(patterns);
        for (int i = 0; i < cluster2.size(); i++) {
            const int taxonIndex = cluster2.at(i);
            for (int j = 0; j < patterns; j++) {
                centroid2[j] += alignment->getPattern(taxonIndex, j);
            }
        }
        for (int j = 0; j < patterns; j++) {
            centroid1.at(j) /= cluster1.size();
            centroid2.at(j) /= cluster2.size();
        }
        bestDist = getDistance(centroid1, centroid2);
		
		*/

    }

    return bestDist;
}


