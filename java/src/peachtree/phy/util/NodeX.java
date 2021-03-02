package peachtree.phy.util;

import java.text.DecimalFormat;
import java.text.DecimalFormatSymbols;
import java.util.List;
import java.util.Locale;

import peachtree.phy.Node;


/**
 * Class representing node in cluster hierarchy 
 * https://github.com/CompEvol/beast2/blob/master/src/beast/util/ClusterTree.java
 */
public class NodeX {
	
	final double EPSILON = 1e-10;
	
	List<String> taxaNames;

    NodeX m_left;
    NodeX m_right;
    NodeX m_parent;
    int m_iLeftInstance;
    int m_iRightInstance;
    double m_fLeftLength = 0;
    double m_fRightLength = 0;
    double m_fHeight = 0;
    
    
    public NodeX(List<String> taxaNames) {
    	this.taxaNames = taxaNames;
    }
    
    /**
     * Get and set methods
     */
    public void m_iLeftInstance(int val) {
    	this.m_iLeftInstance = val;
    }
    public int m_iLeftInstance() {
    	return this.m_iLeftInstance;
    }
    public void m_iRightInstance(int val) {
    	this.m_iRightInstance = val;
    }
    public int m_iRightInstance() {
    	return this.m_iRightInstance;
    }
    public void m_left(NodeX node) {
    	this.m_left = node;
    }
    public NodeX m_left() {
    	return this.m_left;
    }
    public void m_right(NodeX node) {
    	this.m_right = node;
    }
    public NodeX m_right() {
    	return this.m_right;
    }
    public void m_parent(NodeX node) {
    	this.m_parent = node;
    }
    public NodeX m_parent() {
    	return this.m_parent;
    }
    

    public void setHeight(double height1, double height2) {
        if (height1 < EPSILON) {
            height1 = EPSILON;
        }
        if (height2 < EPSILON) {
            height2 = EPSILON;
        }
        m_fHeight = height1;
        if (m_left == null) {
            m_fLeftLength = height1;
        } else {
            m_fLeftLength = height1 - m_left.m_fHeight;
        }
        if (m_right == null) {
            m_fRightLength = height2;
        } else {
            m_fRightLength = height2 - m_right.m_fHeight;
        }
    }

    public void setLength(double length1, double length2) {
        if (length1 < EPSILON) {
            length1 = EPSILON;
        }
        if (length2 < EPSILON) {
            length2 = EPSILON;
        }
        m_fLeftLength = length1;
        m_fRightLength = length2;
        m_fHeight = length1;
        if (m_left != null) {
            m_fHeight += m_left.m_fHeight;
        }
    }

    @Override
	public String toString() {
        final DecimalFormat myFormatter = new DecimalFormat("#.#####", new DecimalFormatSymbols(Locale.US));

        if (m_left == null) {
            if (m_right == null) {
                return "(" + taxaNames.get(m_iLeftInstance) + ":" + myFormatter.format(m_fLeftLength) + "," +
                        taxaNames.get(m_iRightInstance) + ":" + myFormatter.format(m_fRightLength) + ")";
            } else {
                return "(" + taxaNames.get(m_iLeftInstance) + ":" + myFormatter.format(m_fLeftLength) + "," +
                        m_right.toString() + ":" + myFormatter.format(m_fRightLength) + ")";
            }
        } else {
            if (m_right == null) {
                return "(" + m_left.toString() + ":" + myFormatter.format(m_fLeftLength) + "," +
                        taxaNames.get(m_iRightInstance) + ":" + myFormatter.format(m_fRightLength) + ")";
            } else {
                return "(" + m_left.toString() + ":" + myFormatter.format(m_fLeftLength) + "," + m_right.toString() + ":" + myFormatter.format(m_fRightLength) + ")";
            }
        }
    }

    public Node toNode() {
    	
        final Node node = new Node();
        node.setHeight(m_fHeight);
        if (m_left == null) {
        	
        	Node left = new Node();
        	node.addChild(left);
        	left.setNr(m_iLeftInstance);
        	left.setHeight(m_fHeight - m_fLeftLength);
            if (m_right == null) {
            	Node right = new Node();
                node.addChild(right);
                right.setNr(m_iRightInstance);
                right.setHeight(m_fHeight - m_fRightLength);
            } else {
                node.addChild(m_right.toNode());
            }
        } else {
            node.addChild(m_left.toNode());
            if (m_right == null) {
            	Node right = new Node();
                node.addChild(right);
                right.setNr(m_iRightInstance);
                right.setHeight(m_fHeight - m_fRightLength);
            } else {
            	node.addChild(m_right.toNode());
            }
        }
        if (node.getHeight() < node.getChild(0).getHeight() + EPSILON) {
            node.setHeight(node.getChild(0).getHeight() + EPSILON);
        }
        if (node.getHeight() < node.getChild(1).getHeight() + EPSILON) {
            node.setHeight(node.getChild(1).getHeight() + EPSILON);
        }

        //node.getChild(1).setParent(node);
        //node.getChild(0).setParent(node);
        return node;
    }

}
