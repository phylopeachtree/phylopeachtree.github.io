package peachtree.phy.util;

import java.util.Comparator;


/**
 * Comparator used by priority queue
 * https://github.com/CompEvol/beast2/blob/master/src/beast/util/ClusterTree.java
 */
public class TupleComparator implements Comparator<Tuple> {
	
	
	public TupleComparator() {
		
	}
	
    @Override
	public int compare(final Tuple o1, final Tuple o2) {
        if (o1.m_fDist < o2.m_fDist) {
            return -1;
        } else if (o1.m_fDist == o2.m_fDist) {
            return 0;
        }
        return 1;
    }
    
}