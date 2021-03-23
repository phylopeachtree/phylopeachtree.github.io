package peachtree.phy.util;



/**
 * Used for priority queue for efficient retrieval of pair of clusters to merge
 * https://github.com/CompEvol/beast2/blob/master/src/beast/util/ClusterTree.java
 */
public class Tuple {
	
	 public Tuple(final double d, final int i, final int j, final int size1, final int size2) {
         m_fDist = d;
         m_iCluster1 = i;
         m_iCluster2 = j;
         m_nClusterSize1 = size1;
         m_nClusterSize2 = size2;
     }

     double m_fDist;
     int m_iCluster1;
     int m_iCluster2;
     int m_nClusterSize1;
     int m_nClusterSize2;
     
     
     
     // Get and set methods
     public void m_fDist(double val) {
    	 this.m_fDist = val;
     }
     public double m_fDist() {
    	 return this.m_fDist;
     }
     
     public void m_iCluster1(int val) {
    	 this.m_iCluster1 = val;
     }
     public int m_iCluster1() {
    	 return this.m_iCluster1;
     }
     
     public void m_iCluster2(int val) {
    	 this.m_iCluster2 = val;
     }
     public int m_iCluster2() {
    	 return this.m_iCluster2;
     }
     
     public void m_nClusterSize1(int val) {
    	 this.m_nClusterSize1 = val;
     }
     public int m_nClusterSize1() {
    	 return this.m_nClusterSize1;
     }

     
     public void m_nClusterSize2(int val) {
    	 this.m_nClusterSize2 = val;
     }
     public int m_nClusterSize2() {
    	 return this.m_nClusterSize2;
     }


}
