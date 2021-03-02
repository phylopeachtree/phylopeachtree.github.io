package peachtree.phy;

import java.util.ArrayList;
import java.util.List;


public class Tree {

	
	Node root;
	Node[] nodes;
	
	
	public Tree() {
		
	}
	
	
	public Tree(Node root) {
		this.setRoot(root);
	}
	
	
	public void setRoot(Node root) {
		this.root = root;
		this.initArray();
	}
	

	public Tree copy() {
		return new Tree(this.root.copy());
	}
	
	
	/**
	 * Post-order traversal
	 */
	public void initArray() {
		this.nodes = listNodes(root);
	}
	
	
	/**
	 * List of nodes, where first indices are leaves, then internal nodes, then root
	 * @return
	 */
	public Node[] getNodesAsArray() {
		return this.nodes;
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
	
	
	
}
