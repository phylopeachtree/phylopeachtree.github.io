package peachtree.phy;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;


public class Node {

	
	int nodeNr;
	double height;
	Node parent;
	List<Node> children;
	
	public Node() {
		this.nodeNr = 0;
		this.height = 0;
		this.parent = null;
		this.children = new ArrayList<>();
	}
	
	public Node(int index) {
		this.nodeNr = index;
		this.height = 0;
		this.parent = null;
		this.children = new ArrayList<>();
	}
	
	
	/**
	 * TODO
	 * Get the accession of this taxon or node number if not applicable
	 * @return
	 */
	public String getAcc() {
		return "" + this.nodeNr;
	}
	
	
	public int getNr() {
		return this.nodeNr;
	}
	
	public void setNr(int nr) {
		this.nodeNr = nr;
	}
	
	public double getHeight() {
		return this.height;
	}
	
	public void setHeight(double h) {
		this.height = h;
	}

	
	/**
	 * Set the parent of this node
	 * @param parent
	 */
	protected void setParent(Node parent) {
		this.parent = parent;
	}
	
	protected void removeParent() {
		this.parent = null;
	}
	
	/**
	 * The parent of this node
	 * @return
	 */
	public Node getParent() {
		return this.parent;
	}
	
	
	/**
	 * Add a child
	 * @param child
	 */
	public void addChild(Node child) {
		this.children.add(child);
		child.setParent(this);
	}
	
	
	
	
	
	
	/**
	 * Is it a leaf
	 * @return
	 */
	public boolean isLeaf() {
		return this.getNumChildren() == 0;
	}
	
	
	/**
	 * Is it the root
	 * @return
	 */
	public boolean isRoot() {
		return this.parent == null;
	}
	
	
	
	/**
	 * Get child
	 * @param i
	 * @return
	 */
	public Node getChild(int i) {
		return this.children.get(i);
	}
	
	
	/**
	 * Number of children
	 * @return
	 */
	public int getNumChildren() {
		return this.children.size();
	}
	
	
	public List<Node> getChildren(){
		return children;
	}
	

	/**
	 * Copy the node
	 * @return
	 */
	public Node copy() {
		Node clone = new Node(this.getNr());
		for (Node child : children) {
			clone.addChild(child.copy());
		}
		return clone;
	}
	
	
	/**
	 * Size of subtree
	 * @return
	 */
	public int getNodeCount() {
		int size = 1;
		if (!this.isLeaf()) {
			for (Node child : this.children) {
				size += child.getNodeCount();
			}
		}
		return size;
	}

	/**
	 * Number of leaves in subtree
	 * @return
	 */
	public int getLeafNodeCount() {
		if (this.isLeaf()) return 1;
		
		int size = 0;
		for (Node child : this.children) {
			size += child.getLeafNodeCount();
		}
		return size;
	}
	
	
	/**
	 * Number of immediate children
	 * @return
	 */
	public int getChildCount() {
		return this.children.size();
	}
	
	
	/**
	 * Length of this branch
	 * @return
	 */
	public double getLength() {
		if (this.isRoot()) return 0;
		return this.getParent().getHeight() - this.getHeight();
	}
	

	/**
	 * Print node as a newick
	 * Adapted from beast2.evolution.tree.Node
	 * @param maxNodeInClade
	 * @param printMetaData
	 * @return
	 */
    public String toSortedNewick(int[] maxNodeInClade, boolean printMetaData) {
        StringBuilder buf = new StringBuilder();

        if (!isLeaf()) {

            if (getChildCount() <= 2) {
                // Computationally cheap method for special case of <=2 children

                buf.append("(");
                String child1 = getChild(0).toSortedNewick(maxNodeInClade, printMetaData);
                int child1Index = maxNodeInClade[0];
                if (getChildCount() > 1) {
                    String child2 = getChild(1).toSortedNewick(maxNodeInClade, printMetaData);
                    int child2Index = maxNodeInClade[0];
                    if (child1Index > child2Index) {
                        buf.append(child2);
                        buf.append(",");
                        buf.append(child1);
                    } else {
                        buf.append(child1);
                        buf.append(",");
                        buf.append(child2);
                        maxNodeInClade[0] = child1Index;
                    }
                } else {
                    buf.append(child1);
                }
                buf.append(")");
                if (getAcc() != null) {
                    buf.append(nodeNr+1);
                }

            } else {
                // General method for >2 children

                String[] childStrings = new String[getChildCount()];
                int[] maxNodeNrs = new int[getChildCount()];
                Integer[] indices = new Integer[getChildCount()];
                for (int i = 0; i < getChildCount(); i++) {
                    childStrings[i] = getChild(i).toSortedNewick(maxNodeInClade, printMetaData);
                    maxNodeNrs[i] = maxNodeInClade[0];
                    indices[i] = i;
                }

                Arrays.sort(indices, (i1, i2) -> {
                    if (maxNodeNrs[i1] < maxNodeNrs[i2])
                        return -1;

                    if (maxNodeNrs[i1] > maxNodeNrs[i2])
                        return 1;

                    return 0;
                });

                maxNodeInClade[0] = maxNodeNrs[maxNodeNrs.length - 1];

                buf.append("(");
                for (int i = 0; i < indices.length; i++) {
                    if (i > 0)
                        buf.append(",");

                    buf.append(childStrings[indices[i]]);
                }

                buf.append(")");

                if (getAcc() != null) {
                    buf.append(nodeNr + 1);
                }
            }

        } else {
            maxNodeInClade[0] = nodeNr;
            buf.append(nodeNr + 1);
        }

        if (printMetaData) {
            //buf.append(getNewickMetaData());
        }

        buf.append(":");
        //if (printMetaData) buf.append(getNewickLengthMetaData());
        buf.append(getLength());

        return buf.toString();
    }
	
}
