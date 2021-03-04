package peachtree.phy;

import org.json.JSONArray;

import peachtree.aln.Alignment;
import peachtree.aln.Filtering;
import peachtree.aln.Sequence;
import peachtree.options.Scaling;

public class Tree {

	
	Node root;
	Node[] nodes;
	Alignment alignment;
	
	
	public Tree() {
		
	}
	
	
	public Tree(Node root, Alignment alignment) {
		this.alignment = alignment;
		this.setRoot(root);
	}
	
	
	public void setRoot(Node root) {
		this.root = root;
		this.initArray();
	}
	

	public Tree copy() {
		return new Tree(this.root.copy(), this.alignment);
	}
	
	
	/**
	 * Post-order traversal
	 */
	public void initArray() {
		this.nodes = listNodes(root);
		for (int i = 0; i < this.nodes.length; i ++) {
			this.nodes[i].setNr(i);
		}
	}
	
	
	/**
	 * List of nodes, where first indices are leaves, then internal nodes, then root
	 * @return
	 */
	public Node[] getNodesAsArray() {
		return this.nodes;
	}
	
	
	/**
	 * List of leaves i.e. the first N nodes
	 * @return
	 */
	public Node[] getLeavesAsArray() {
		Node[] leaves = new Node[this.getLeafNodeCount()];
		for (int i = 0; i < leaves.length; i ++) {
			leaves[i] = this.nodes[i];
		}
		return leaves;
	}
	
	
	/**
	 * Number of leaf nodes
	 * @return
	 */
	public int getLeafNodeCount(){
		return this.root.getLeafNodeCount();
	}
	
	
	/**
	 * To newick string
	 * @return
	 */
	public String toNewick() {
		final int[] dummy = new int[1];
		return this.root.toSortedNewick(dummy, false);
	}
	
	
	@Override
	public String toString() {
		return this.toNewick();
	}
	
	
	/**
	 * Reset the node ordering
	 */
	protected Node[] listNodes(Node theRoot) {
		
		Node[] toReturn = new Node[theRoot.getNodeCount()];
		
		// Post order traversal
		Node[] nodeArr = new Node[theRoot.getNodeCount()];
		Tree.getNodesPostOrder(theRoot, nodeArr, 0);
		
		// Leaves first, then internal, then root
		int index = 0;
		for (Node node : nodeArr) {
			if (node.isLeaf()) {
				toReturn[index] = node;
				index++;
			}
		}
		for (Node node : nodeArr) {
			if (!node.isLeaf() && !node.isRoot()) {
				toReturn[index] = node;
				index++;
			}
		}
		for (Node node : nodeArr) {
			if (!node.isLeaf() && node.isRoot()) {
				toReturn[index] = node;
				index++;
			}
		}
		
		return toReturn;
		
		
	}
	
	
	/**
	 * 
	 * @param node
	 * @param nodes
	 * @param pos
	 * @return
	 */
	 public static int getNodesPostOrder(final Node node, final Node[] nodes, int pos) {
		 if (!node.isLeaf()) {
			 for (final Node child : node.getChildren()) {
				 pos = getNodesPostOrder(child, nodes, pos);
			 }
		 }
		 nodes[pos] = node;
		 return pos + 1;
	 }


	 
	/**
	 * Get a json array of graphics
	 * @param scaling
	 * @param branchWidth
	 * @return
	 */
	public JSONArray getTreeGraphics(Scaling scaling, double branchWidth, Filtering filtering, boolean showTaxaOnTree) {
		
		JSONArray objs = new JSONArray();
		this.root.getGraphics(objs, filtering, scaling, branchWidth, showTaxaOnTree);
		return objs;
		
	}

	public double getHeight() {
		return this.root.getHeight();
	}

	
	
	
	
}











