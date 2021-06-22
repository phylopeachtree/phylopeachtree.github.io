package peachtree.phy;

import java.util.ArrayList;
import java.util.Calendar;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;

import org.json.JSONArray;

import peachtree.aln.Alignment;
import peachtree.aln.Filtering;
import peachtree.aln.Taxon;
import peachtree.options.Scaling;

public class Tree {

	
	Node root;
	Node[] nodes;
	Alignment alignment;
	boolean parsedFromFile = false;
	
	public Tree() {
		
	}
	
	
	public Tree(Node root, Alignment alignment) {
		this.alignment = alignment;
		this.setRoot(root);
	}
	
	
	
	/**
	 * Was the tree parsed from a file?
	 * @return
	 */
	public boolean parsedFromFile() {
		return parsedFromFile;
	}
	
	
	/**
	 * Parse from nexus/newick
	 * If there is more than 1 tree, the last one will be parsed
	 * @param newick
	 * @throws Exception
	 */
	public void parseFromNexus(String nexus) throws Exception {
		
		
		
		this.parsedFromFile = true;
		
		String[] lines = nexus.split("\n");
		HashMap<String, String> translateMap = new HashMap<>();
		
		// Get the translate tokens
		boolean beganTrees = false;
		boolean beganTranslate = false;
		String[] splt;
		for (int lineNum = 0; lineNum < lines.length; lineNum++) {
			String line = lines[lineNum].trim();
			if (line.toLowerCase().contains("begin trees;")) {
				beganTrees = true;
			}
			else if (beganTrees && line.toLowerCase().equals(";")) {
				break;
			}
			else if (beganTrees && !beganTranslate && line.toLowerCase().contains("translate")) {
				beganTranslate = true;
			}
			else if (beganTranslate) {
				
				// Translate token
				splt = line.split(" ");
				if (splt.length < 2) {
					throw new Exception("Cannot parse translate line " + line + " because there are not 2 tokens split by a single space");
				}
				
				String id = splt[0];
				String label = "";
				for (int i = 1; i < splt.length; i ++) {
					label += splt[i];
					if (i < splt.length-1) label += " ";
				}
				
				// Remove trailing comma
				if (label.substring(label.length()-1).equals(",")) label = label.substring(0, label.length()-1); 
				
				// Remove " from start and end if there are spaces in the label
				if (splt.length > 2) {
					if (label.substring(0, 1).equals("\"") && label.substring(label.length()-1, label.length()).equals("\"")) {
						label = label.substring(1, label.length()-1);
					}
				}
				
				
				
				if (translateMap.containsKey(id)) {
					throw new Exception("Duplicate translate id detected " + id);
				}
				translateMap.put(id, label);
				
			}
		}
		
		
		
		// Get the last tree's newick
		StringBuilder newick = null;
		for (int lineNum = lines.length-1; lineNum >= 0; lineNum --) {
			String[] lineSplit = lines[lineNum].split("[=]", 2);
			if (lineNum == lines.length-2) {
				if (lineSplit[0].length() >= 4) System.out.println(lineSplit[0].toLowerCase().substring(0, 4));
			}
			if (lineSplit.length == 2 && lineSplit[0].length() >= 4 && lineSplit[0].toLowerCase().substring(0, 4).equals("tree")) {
				newick = new StringBuilder(lineSplit[1]);
				break;
			}
		}
		
		if (newick == null) {
			
			// Maybe it's just newick not nexus. Take first non empty line
			for (int lineNum = 0; lineNum < lines.length; lineNum ++) {
				if (!lines[lineNum].isEmpty()) {
					newick = new StringBuilder(lines[lineNum]);
					break;
				}
			}
			
			if (newick == null) throw new Exception("Cannot locate any newick trees in file");
		}
		
		
		
		
		// Remove trailing ; and all spaces
		newick = new StringBuilder(newick.toString().replace(";", "").replace(" ", ""));
		
		this.root = new Node();
		this.root.setHeight(0);
		this.root.parseFromNewick(newick);
		this.initArray();
		
		
		
		// Normalise so that the smallest leaf is at height 0
		double minimalHeight = Double.POSITIVE_INFINITY;
		for (Node node : this.getNodesAsArray()) minimalHeight = Math.min(minimalHeight, node.getHeight());
		for (Node node : this.getNodesAsArray()) node.setHeight(node.getHeight() - minimalHeight);
		
		
		// Apply label translation
		if (!translateMap.isEmpty()) {
			for (Node node : this.getNodesAsArray()) {
				String key = node.getAcc();
				if (translateMap.containsKey(key)) {
					node.setAcc(translateMap.get(key));
				}
			}
		}
		
		
	}
	

	public void setRoot(Node root) {
		this.root = root;
		this.initArray();
	}
	
	public Node getRoot() {
		return this.root;
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
		return this.root.toSortedNewick(dummy, true) + ")";
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
	public JSONArray getTreeGraphics(Scaling scaling, double branchWidth, Filtering filtering, boolean showTaxaOnTree, 
			double nodeRadius, String internalLabel, String leafLabel, double fontSize, int rounding, boolean transmissionTree) {
		
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
		
		Node subtree = filtering.getSubtreeRoot() != null ? filtering.getSubtreeRoot() : this.root;
		subtree.getGraphics(true, objs, filtering, scaling, branchWidth, showTaxaOnTree, yshift, nodeRadius, internalLabel, leafLabel, fontSize, rounding, transmissionTree);
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
		
		if (filtering == null) return;
		
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
		Node mrca = getMRCA(taxa);
		
		
		// Get all leaves in this subtree
		List<Node> leaves = new ArrayList<>();
		mrca.getLeafSet(leaves);
		
		
		// Return their taxa
		List<Taxon> clade = new ArrayList<>();
		for (Node leaf : leaves) clade.add(leaf.getTaxon());
		return clade;
		
	}
	
	
	/**
	 * Get the MRCA of a list of taxa
	 * @return
	 */
	public Node getMRCA(List<Taxon> taxa) {
		
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
		
		return mrca;
		
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


	/**
	 * Find all annotation variables and sort them alphabetically
	 * @return
	 */
	public List<String> getAllAnnotations() {
		List<String> annotations = new ArrayList<>();
		root.getAllAnnotations(annotations);
		Collections.sort(annotations);
		return annotations;
	}

	/**
	 * Rotate the subtree at the specified internal node
	 * @param index
	 */
	public void flipSubtree(int index) {
		
		
		// Find the node
		if (index < 0 || index >= this.nodes.length) return;
		Node node = this.nodes[index];
		if (node.isLeaf()) return;
		node.rotateChildren();
		
		
		this.initArray();
		
	}
	
	
}











