package peachtree.phy;

import java.util.ArrayList;
import java.util.List;
import java.util.PriorityQueue;

import peachtree.aln.Alignment;
import peachtree.phy.util.LinkType;
import peachtree.phy.util.NodeX;
import peachtree.phy.util.Tuple;
import peachtree.phy.util.TupleComparator;

/**
 * Modified from beast2 ClusterTree
 * https://github.com/CompEvol/beast2/blob/master/src/beast/util/ClusterTree.java
 * @author Jordan
 *
 */
public class ClusterTree extends Tree {
	
	
	
	
	LinkType linkType;
	
	List<String> taxaNames;
	boolean distanceIsBranchLength;
	

	
	public ClusterTree(Alignment alignment, LinkType linkType) throws Exception {
		
		
		taxaNames = alignment.getNames();
		this.alignment = alignment;
		this.linkType = linkType;
		
		// Method
		distanceIsBranchLength = linkType == LinkType.neighborjoining;
		
		final Node root = buildClusterer();
		this.setRoot(root);
		
		// Ensure that last tip is at 0
		double latestTip = Double.POSITIVE_INFINITY;
        for (int i = 0; i < this.nodes.length; i++) {
        	latestTip = Math.min(latestTip, this.nodes[i].getHeight());
        }
        for (int i = 0; i < this.nodes.length; i++) {
        	this.nodes[i].setHeight(this.nodes[i].getHeight() - latestTip);
        }
        
        super.initArray();
		

	}

	
	public Node buildClusterer()  throws Exception {
		
        final int taxonCount = taxaNames.size();
        
        
        if (taxonCount == 1) {
            // pathological case
            final Node node = new Node();
            node.setHeight(1);
            node.setNr(0);
            return node;
        }

        // use array of integer vectors to store cluster indices,
        // starting with one cluster per instance
        final List<Integer>[] clusterID = new ArrayList[taxonCount];
        for (int i = 0; i < taxonCount; i++) {
            clusterID[i] = new ArrayList<>();
            clusterID[i].add(i);
        }
        // calculate distance matrix
        final int clusters = taxonCount;
        

        // used for keeping track of hierarchy
        final NodeX[] clusterNodes = new NodeX[taxonCount];

        // Build the tree
        if (linkType == LinkType.neighborjoining) {
            neighborJoining(clusters, clusterID, clusterNodes);
        } else {
            doLinkClustering(clusters, clusterID, clusterNodes);
        }
        
        

        // move all clusters in m_nClusterID array
        // & collect hierarchy
        for (int i = 0; i < taxonCount; i++) {
            if (clusterID[i].size() > 0) {
                return clusterNodes[i].toNode();
            }
        }
        return null;
    } // buildClusterer
	
	
	



    /**
     * use neighbor joining algorithm for clustering
     * This is roughly based on the RapidNJ simple implementation and runs at O(n^3)
     * More efficient implementations exist, see RapidNJ (or my GPU implementation :-))
     *
     * @param clusters
     * @param clusterID
     * @param clusterNodes
     */
    void neighborJoining(int clusters, final List<Integer>[] clusterID, final NodeX[] clusterNodes) throws Exception {
        final int n = taxaNames.size();

        final double[][] dist = new double[clusters][clusters];
        for (int i = 0; i < clusters; i++) {
            dist[i][i] = 0;
            for (int j = i + 1; j < clusters; j++) {
                dist[i][j] = getDistance0(clusterID[i], clusterID[j]);
                dist[j][i] = dist[i][j];
            }
        }

        final double[] separationSums = new double[n];
        final double[] separations = new double[n];
        final int[] nextActive = new int[n];

        //calculate initial separation rows
        for (int i = 0; i < n; i++) {
            double sum = 0;
            for (int j = 0; j < n; j++) {
                sum += dist[i][j];
            }
            separationSums[i] = sum;
            separations[i] = sum / (clusters - 2);
            nextActive[i] = i + 1;
        }

        while (clusters > 2) {
            // find minimum
            int min1 = -1;
            int min2 = -1;
            double min = Double.MAX_VALUE;
            {
                int i = 0;
                while (i < n) {
                    final double sep1 = separations[i];
                    final double[] row = dist[i];
                    int j = nextActive[i];
                    while (j < n) {
                        final double sep2 = separations[j];
                        final double val = row[j] - sep1 - sep2;
                        if (val < min) {
                            // new minimum
                            min1 = i;
                            min2 = j;
                            min = val;
                        }
                        j = nextActive[j];
                    }
                    i = nextActive[i];
                }
            }
            // record distance
            final double minDistance = dist[min1][min2];
            clusters--;
            final double sep1 = separations[min1];
            final double sep2 = separations[min2];
            final double dist1 = (0.5 * minDistance) + (0.5 * (sep1 - sep2));
            final double dist2 = (0.5 * minDistance) + (0.5 * (sep2 - sep1));
            if (clusters > 2) {
                // update separations  & distance
                double newSeparationSum = 0;
                final double mutualDistance = dist[min1][min2];
                final double[] row1 = dist[min1];
                final double[] row2 = dist[min2];
                for (int i = 0; i < n; i++) {
                    if (i == min1 || i == min2 || clusterID[i].size() == 0) {
                        row1[i] = 0;
                    } else {
                        final double val1 = row1[i];
                        final double val2 = row2[i];
                        final double distance = (val1 + val2 - mutualDistance) / 2.0;
                        newSeparationSum += distance;
                        // update the separationsum of cluster i.
                        separationSums[i] += (distance - val1 - val2);
                        separations[i] = separationSums[i] / (clusters - 2);
                        row1[i] = distance;
                        dist[i][min1] = distance;
                    }
                }
                separationSums[min1] = newSeparationSum;
                separations[min1] = newSeparationSum / (clusters - 2);
                separationSums[min2] = 0;
                merge(min1, min2, dist1, dist2, clusterID, clusterNodes);
                int prev = min2;
                // since min1 < min2 we havenActiveRows[0] >= 0, so the next loop should be save
                while (clusterID[prev].size() == 0) {
                    prev--;
                }
                nextActive[prev] = nextActive[min2];
            } else {
                merge(min1, min2, dist1, dist2, clusterID, clusterNodes);
                break;
            }
        }

        for (int i = 0; i < n; i++) {
            if (clusterID[i].size() > 0) {
                for (int j = i + 1; j < n; j++) {
                    if (clusterID[j].size() > 0) {
                        final double dist1 = dist[i][j];
                        if (clusterID[i].size() == 1) {
                            merge(i, j, dist1, 0, clusterID, clusterNodes);
                        } else if (clusterID[j].size() == 1) {
                            merge(i, j, 0, dist1, clusterID, clusterNodes);
                        } else {
                            merge(i, j, dist1 / 2.0, dist1 / 2.0, clusterID, clusterNodes);
                        }
                        break;
                    }
                }
            }
        }
    } 
    

    /**
     * Perform clustering using a link method
     * This implementation uses a priority queue resulting in a O(n^2 log(n)) algorithm
     *
     * @param clusters    number of clusters
     * @param clusterID
     * @param clusterNodes
     */
    void doLinkClustering(int clusters, final List<Integer>[] clusterID, final NodeX[] clusterNodes) {
    	
    	
    	
        final int instances = taxaNames.size();
        final PriorityQueue<Tuple> queue = new PriorityQueue<>(clusters * clusters / 2, new TupleComparator());
        final double[][] distance0 = new double[clusters][clusters];
        for (int i = 0; i < clusters; i++) {
            distance0[i][i] = 0;
            for (int j = i + 1; j < clusters; j++) {
            	
                distance0[i][j] = getDistance0(clusterID[i], clusterID[j]);
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
            while (t != null && (clusterID[t.m_iCluster1()].size() != t.m_nClusterSize1() || clusterID[t.m_iCluster2()].size() != t.m_nClusterSize2()));
            min1 = t.m_iCluster1();
            min2 = t.m_iCluster2();
            merge(min1, min2, t.m_fDist()/2.0, t.m_fDist()/2.0, clusterID, clusterNodes);
            // merge  clusters
            
            //System.out.println("Merging " + min1 + " and " + min2);

            // update distances & queue
            for (int i = 0; i < instances; i++) {
                if (i != min1 && clusterID[i].size() != 0) {
                    final int i1 = Math.min(min1, i);
                    final int i2 = Math.max(min1, i);
                    final double distance = getDistance(distance0, clusterID[i1], clusterID[i2]);
                    queue.add(new Tuple(distance, i1, i2, clusterID[i1].size(), clusterID[i2].size()));
                }
            }

            clusters--;
        }
    } 
    void merge(int min1, int min2, double dist1, double dist2, final List<Integer>[] clusterID, final NodeX[] clusterNodes) {
        if (min1 > min2) {
            final int h = min1;
            min1 = min2;
            min2 = h;
            final double f = dist1;
            dist1 = dist2;
            dist2 = f;
        }
        clusterID[min1].addAll(clusterID[min2]);
        //clusterID[min2].removeAllElements();
        clusterID[min2].removeAll(clusterID[min2]);

        // track hierarchy
        final NodeX node = new NodeX(this.taxaNames);
        if (clusterNodes[min1] == null) {
            node.m_iLeftInstance(min1);
        } else {
            node.m_left(clusterNodes[min1]);
            clusterNodes[min1].m_parent(node);
        }
        if (clusterNodes[min2] == null) {
            node.m_iRightInstance(min2);
        } else {
            node.m_right(clusterNodes[min2]);
            clusterNodes[min2].m_parent(node);
        }
        if (this.distanceIsBranchLength) {
            node.setLength(dist1, dist2);
        } else {
            node.setHeight(dist1, dist2);
        }
        clusterNodes[min1] = node;
    } // merge

    /**
     * calculate distance the first time when setting up the distance matrix *
     */
    double getDistance0(final List<Integer> cluster1, final List<Integer> cluster2) {
    	return distance(cluster1.get(0), cluster2.get(0));
    } 
    
    

    /**
     * Hamming distance
     * @param taxon1
     * @param taxon2
     * @return
     */
    double distance(final int taxon1, final int taxon2) {
    	
    	
    	
        int state1, state2;

        int n = alignment.getPatternCount();
        
        
        double distance, weight;
        double sumDistance = 0.0;
        double sumWeight = 0.0;
        

        int[] pattern;
        

        for (int i = 0; i < n; i++) {
            pattern = alignment.getPattern(i);
            
            state1 = pattern[taxon1];
            state2 = pattern[taxon2];
            
            weight = alignment.getPatternWeight(i);
            
            if (!Alignment.isAmbiguousOrGap(state1, alignment.isNucleotide()) && 
            	!Alignment.isAmbiguousOrGap(state2, alignment.isNucleotide()) &&
            		state1 != state2) {
                sumDistance += weight;
            }
            sumWeight += weight;
        }

        distance = sumDistance / sumWeight;

        return distance;
    	
    	
    } 

    // 1-norm
    double distance(final double[] pattern1, final double[] pattern2) {
        double dist = 0;
        for (int i = 0; i < alignment.getPatternCount(); i++) {
            dist += alignment.getPatternWeight(i) * Math.abs(pattern1[i] - pattern2[i]);
        }
        return dist; // / dataInput.get().getSiteCount();
    }


    /**
     * calculate the distance between two clusters
     *
     * @param cluster1 list of indices of instances in the first cluster
     * @param cluster2 dito for second cluster
     * @return distance between clusters based on link type
     */
    double getDistance(final double[][] distance, final List<Integer> cluster1, final List<Integer> cluster2) {
    	
    	
    	
        double bestDist = Double.MAX_VALUE;

        
        // Use if-else because cheerpj doesn't work as well with switch-case
        if (linkType == LinkType.single) {
        	
        	// find single link distance aka minimum link, which is the closest distance between
            // any item in cluster1 and any item in cluster2
            bestDist = Double.MAX_VALUE;
            for (int i = 0; i < cluster1.size(); i++) {
                final int i1 = cluster1.get(i);
                for (int j = 0; j < cluster2.size(); j++) {
                    final int i2 = cluster2.get(j);
                    final double dist = distance[i1][i2];
                    if (bestDist > dist) {
                        bestDist = dist;
                    }
                }
            }
        	
        }else if (linkType == LinkType.complete || linkType == LinkType.adjcomplete) {
        	
        	 // find complete link distance aka maximum link, which is the largest distance between
            // any item in cluster1 and any item in cluster2
            bestDist = 0;
            for (int i = 0; i < cluster1.size(); i++) {
                final int i1 = cluster1.get(i);
                for (int j = 0; j < cluster2.size(); j++) {
                    final int i2 = cluster2.get(j);
                    final double dist = distance[i1][i2];
                    if (bestDist < dist) {
                        bestDist = dist;
                    }
                }
            }
            
            if (linkType == LinkType.adjcomplete) {
            
	            // calculate adjustment, which is the largest within cluster distance
	            double maxDist = 0;
	            for (int i = 0; i < cluster1.size(); i++) {
	                final int i1 = cluster1.get(i);
	                for (int j = i + 1; j < cluster1.size(); j++) {
	                    final int i2 = cluster1.get(j);
	                    final double dist = distance[i1][i2];
	                    if (maxDist < dist) {
	                        maxDist = dist;
	                    }
	                }
	            }
	            for (int i = 0; i < cluster2.size(); i++) {
	                final int i1 = cluster2.get(i);
	                for (int j = i + 1; j < cluster2.size(); j++) {
	                    final int i2 = cluster2.get(j);
	                    final double dist = distance[i1][i2];
	                    if (maxDist < dist) {
	                        maxDist = dist;
	                    }
	                }
	            }
	            bestDist -= maxDist;
            
            }
        	
        }else if (linkType == LinkType.upgma) {
        	
        	
        	// finds average distance between the elements of the two clusters
            bestDist = 0;
            for (int i = 0; i < cluster1.size(); i++) {
                final int i1 = cluster1.get(i);
                for (int j = 0; j < cluster2.size(); j++) {
                    final int i2 = cluster2.get(j);
                    bestDist += distance[i1][i2];
                }
            }
            bestDist /= (cluster1.size() * cluster2.size());
        	
        }else if (linkType == LinkType.mean) {
        	
        	
        	// calculates the mean distance of a merged cluster (akak Group-average agglomerative clustering)
            final List<Integer> merged = new ArrayList<>();
            merged.addAll(cluster1);
            merged.addAll(cluster2);
            bestDist = 0;
            for (int i = 0; i < merged.size(); i++) {
                final int i1 = merged.get(i);
                for (int j = i + 1; j < merged.size(); j++) {
                    final int i2 = merged.get(j);
                    bestDist += distance[i1][i2];
                }
            }
            final int n = merged.size();
            bestDist /= (n * (n - 1.0) / 2.0);
        	
        }else if (linkType == LinkType.centroid) {
        	
        	
        	// finds the distance of the centroids of the clusters
            final int patterns = alignment.getPatternCount();
            final double[] centroid1 = new double[patterns];
            for (int i = 0; i < cluster1.size(); i++) {
                final int taxonIndex = cluster1.get(i);
                for (int j = 0; j < patterns; j++) {
                    centroid1[j] += alignment.getPattern(taxonIndex, j);
                }
            }
            final double[] centroid2 = new double[patterns];
            for (int i = 0; i < cluster2.size(); i++) {
                final int taxonIndex = cluster2.get(i);
                for (int j = 0; j < patterns; j++) {
                    centroid2[j] += alignment.getPattern(taxonIndex, j);
                }
            }
            for (int j = 0; j < patterns; j++) {
                centroid1[j] /= cluster1.size();
                centroid2[j] /= cluster2.size();
            }
            bestDist = distance(centroid1, centroid2);
        	
        }
        
        return bestDist;
        
    } 


    

    

    
   





}
