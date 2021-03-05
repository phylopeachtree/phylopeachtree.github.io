package peachtree.phy;

import java.util.ArrayList;
import java.util.List;

import org.json.JSONArray;

import peachtree.aln.Alignment;
import peachtree.aln.Filtering;
import peachtree.aln.Sequence;
import peachtree.aln.Taxon;
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
		if (!scaling.inView()) return objs;
		
		// Calculate yshift to avoid clipping top margin
		// First, the top most leaf in the view must be found
		double yshift = 0;
		for (int leafNr = 0; leafNr < this.getLeafNodeCount(); leafNr ++) {
			int filteredNr = this.nodes[leafNr].getFilteredNr();
			if (filteredNr >= 0 && scaling.inRangeY(filteredNr)){
				yshift = scaling.canvasMinY() - scaling.scaleY(filteredNr);
				break;
			}
		}
		
		this.root.getGraphics(objs, filtering, scaling, branchWidth, showTaxaOnTree, yshift);
		return objs;
		
	}

	public double getHeight() {
		return this.root.getHeight();
	}

	
	/**
	 * Apply the taxon filtering 
	 * @param filtering
	 */
	public void applyFiltering(Filtering filtering) {
		
		int nr = 0;
		for (Node leaf : this.getLeavesAsArray()) {
			
			// Maintain a separate numbering system where filtered-out leaves are omitted
			if (filtering.focusing()) {
				if (leaf.getTaxon().isSelected()) {
					leaf.setFilteredNr(nr);
					nr ++;
				}else {
					leaf.setFilteredNr(-1);
				}
			}else {
				leaf.setFilteredNr(nr);
				nr ++;
			}
			
			
			
		}
		
	}


	
	/**
	 * Get all taxa which form a clade
	 * @param taxa
	 * @return
	 */
	public List<Taxon> getClade(List<Taxon> taxa) {
		
		
		// Find the mrca
		Node mrca = null;
		for (int i = 0; i < taxa.size(); i ++) {
			Node node1 = this.getNode(taxa.get(i));
			for (int j = i+1; j < taxa.size(); j ++) {
				Node node2 = this.getNode(taxa.get(j));
				Node ancestor = getMRCA(node1, node2);
				
				// The mrca of the set of taxa is the pairwise mrca with the greatest height
				if (mrca == null || ancestor.getHeight() > mrca.getHeight()){
					mrca = ancestor;
				}
					
			}
			
		}
		
		// Get all leaves in this subtree
		List<Node> leaves = new ArrayList<>();
		mrca.getLeafSet(leaves);
		
		
		// Return their taxa
		List<Taxon> clade = new ArrayList<>();
		for (Node leaf : leaves) clade.add(leaf.getTaxon());
		return clade;
		
	}

	
	/**
	 * Returns the leaf node associated with this taxon
	 * @param taxon
	 * @return
	 */
	public Node getNode(Taxon taxon) {
		for (Node leaf : this.getLeavesAsArray()) {
			if (leaf.getTaxon() == taxon) return leaf;
		}
		return null;
	}
	
	
	/**
	 * Gets thr MRCA of these two nodes
	 * @param taxon1
	 * @param taxon2
	 * @return
	 */
	public Node getMRCA(Node node1, Node node2) {
		
		// Trace from node1 to the root
		List<Node> trace1 = new ArrayList<>();
		Node node = node1;
		while (node != null) {
			trace1.add(node);
			node = node.getParent();
		} 
		
		// Find the earliest node on this path from node2 to the root
		node = node2;
		while(node != null) {
			if (trace1.contains(node)) return node;
			node = node.getParent();
		}
		
		return null;
	}
	
	
}











