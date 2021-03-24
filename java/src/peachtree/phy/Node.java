package peachtree.phy;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;

import org.json.JSONArray;
import org.json.JSONObject;

import peachtree.aln.Filtering;
import peachtree.aln.Taxon;
import peachtree.options.Scaling;


public class Node {

	
	int nodeNr;
	int filteredNodeNr;
	String acc;
	double height;
	Node parent;
	List<Node> children;
	Taxon taxon = null;
	HashMap<String, String> annotations = new HashMap<>();
	
	public Node() {
		this.acc = "";
		this.nodeNr = 0;
		this.filteredNodeNr = 0;
		this.height = 0;
		this.parent = null;
		this.children = new ArrayList<>();
	}
	
	public Node(int index) {
		this.acc = "";
		this.nodeNr = index;
		this.filteredNodeNr = index;
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
		if (this.acc.isEmpty()) return "" + (this.nodeNr+1);
		return this.acc;
	}
	
	public void setAcc(String acc) {
		this.acc = acc;
	}
	
	public void setTaxon(Taxon taxon) {
		this.taxon = taxon;
	}
	
	public Taxon getTaxon() {
		return this.taxon;
	}
	
	public int getNr() {
		return this.nodeNr;
	}
	
	public void setNr(int nr) {
		this.nodeNr = nr;
	}
	
	public void setFilteredNr(int nr) {
		this.filteredNodeNr = nr;
	}
	
	public int getFilteredNr() {
		return this.filteredNodeNr;
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
		clone.acc = this.acc;
		clone.height = this.height;
		clone.annotations = new HashMap<>();
		for (String key : this.annotations.keySet()) {
			clone.annotations.put(key, this.annotations.get(key));
		}
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
	 * Gets all leaves in this subtree and adds them to the list
	 */
	public void getLeafSet(List<Node> leaves){
		if (this.isLeaf()) leaves.add(this);
		for (Node child : this.children) {
			child.getLeafSet(leaves);
		}
		
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
	

	@Override
	public String toString() {
		return this.toSortedNewick(new int[1], false);
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
                //buf.append(this.getAcc());

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
                //buf.append(this.getAcc());
            }

        } else {
            maxNodeInClade[0] = nodeNr;
            //buf.append(nodeNr + 1);
        }

        if (printMetaData) {
            buf.append(getNewickMetaData());
        }

        buf.append(this.getAcc());
        buf.append(":");
        //if (printMetaData) buf.append(getNewickLengthMetaData());
        buf.append(getLength());

        return buf.toString();
    }

    
    
    /**
     * Meta data annotations for newick string
     * @return
     */
    private String getNewickMetaData() {
    	
    	if (this.annotations.isEmpty()) return "";
    	StringBuilder str = new StringBuilder("[");
    	int i = 0;
    	for (String key : this.annotations.keySet()) {
    		str.append(key).append("=").append(this.annotations.get(key));
    		if (i < this.annotations.size()-1) str.append(",");
    		i++;
    	}
    	str.append("]");
    	return str.toString();
    	
    }
    
    
    /**
     * Meta data annotations for displaying string
     * @return
     */
    private String getTidyMetaData() {
    	
    	if (this.annotations.isEmpty()) return "";
    	StringBuilder str = new StringBuilder();
    	int i = 0;
    	for (String key : this.annotations.keySet()) {
    		str.append(key).append("=").append(this.annotations.get(key));
    		if (i < this.annotations.size()-1) str.append("\n");
    		i++;
    	}
    	return str.toString();
    	
    }
    
    
    /**
     * Get the svg graphics of this subtree
     * @param dy
     * @param filtering
     * @param branchWidth
     * @return y
     */
	public Double getGraphics(boolean isRoot, JSONArray objs, Filtering filtering, Scaling scaling, double branchWidth, boolean showTaxaOnTree, double yshift, double nodeRadius) {
		
		//System.out.println("node height " + this.getHeight() + " max height " + scaling.xmax() + "/" + scaling.xmin());
		
		
		
		double x2 = this.getHeight();
		double x2Scaled = scaling.scaleX(x2);
		double maxY = Double.NEGATIVE_INFINITY;
		double minY = Double.POSITIVE_INFINITY;
		
		// If leaf, go with node ordering
		Double y = null;
		int nValidChildren = 0;
		if (this.isLeaf()) {
			if (!filtering.includeTaxon(this.getTaxon())) return null;
			y = this.getFilteredNr() + 0.5;
		}
		
		
		// If parent node, y is midpoint between children
		else {
			
			
			
			for (Node child : this.getChildren()) {
				Double ychild = child.getGraphics(false, objs, filtering, scaling, branchWidth, showTaxaOnTree, yshift, nodeRadius);
				if (ychild != null) {
					if (y == null) y = 0.0;
					y += ychild;
					if (ychild > maxY) maxY = ychild;
					if (ychild < minY) minY = ychild;
					nValidChildren ++;
				}
			}
			if (y != null) y = y / nValidChildren;
			
			
		}
		
		
		
		// If this has y=null, then do not plot the subtree
		if (y == null) return y;
		
		double yscaled = scaling.scaleY(y);
		yscaled += yshift;
		
		// Dashed line to taxa
		if (this.isLeaf() && showTaxaOnTree && this.getHeight() > 0) {
			
			// Only draw if this node is in y-range
			if (scaling.inRangeY(y)) {
				
				
				JSONObject dashed_json = new JSONObject();
				dashed_json.put("ele", "line").put("x1", x2Scaled).put("x2", scaling.canvasMaxX());
				dashed_json.put("y1", yscaled).put("y2", yscaled);
				dashed_json.put("stroke_width", branchWidth/2);
				dashed_json.put("stroke", "black");
				dashed_json.put("stroke_linecap", "round");
				dashed_json.put("stroke_dasharray", "4,7");
				objs.put(dashed_json);
				
			}
			
			
			
		}
		
		
	
		// Shoulder
		if (!this.isLeaf()) {
			
			
			// Only draw if this node is in y-range
			if (scaling.inRangeY(minY) || scaling.inRangeY(maxY) || scaling.inRangeY(y)) {
			
				
				JSONObject shoulder_json = new JSONObject();
				shoulder_json.put("ele", "line").put("x1", x2Scaled).put("x2", x2Scaled);
				shoulder_json.put("y1", scaling.scaleY(minY) + yshift).put("y2", scaling.scaleY(maxY) + yshift);
				shoulder_json.put("stroke_width", branchWidth);
				shoulder_json.put("stroke", "black");
				shoulder_json.put("stroke_linecap", "round");
				objs.put(shoulder_json);
			
			}
				
			
		}
		
		
		// Branch
		if (!isRoot) {
			
			// Only draw if this node is in y-range
			if (scaling.inRangeY(y)) {
			
				double x1 = this.getParent().getHeight();
				
				// Branch
				JSONObject branch_json = new JSONObject();
				branch_json.put("ele", "line").put("x1", scaling.scaleX(x1)).put("x2", x2Scaled);
				branch_json.put("y1", yscaled).put("y2", yscaled);
				branch_json.put("stroke_width", branchWidth);
				branch_json.put("stroke", "black");
				branch_json.put("stroke_linecap", "round");
				objs.put(branch_json);

				
			}
				
		
		}
		
		
		// Draw node and annotate it
		if (nodeRadius > 0 && scaling.inRangeY(y)) {
			JSONObject node_json = new JSONObject();
			node_json.put("ele", "circle").put("cx", x2Scaled).put("cy", yscaled).put("r", nodeRadius).put("fill", "black");
			if (!this.annotations.isEmpty())  node_json.put("title", this.getTidyMetaData());
			objs.put(node_json);
		}
		
		return y;
		
	}

	
	/**
	 * Parse newick. Assumes there is no ; on the end
	 * @param newick
	 * @throws Exception
	 */
	public void parseFromNewick(StringBuilder newick) throws Exception {
		
		
		this.annotations = new HashMap<>();
		
		
		// Is this a leaf?
		String char1 = newick.substring(0, 1);
		boolean hasChildren = char1.equals("(");
		
		
		// What is the node's name
		String[] bits = newick.toString().split(":");
		if (bits.length < 2 || bits[0].isEmpty()) {
			throw new Exception("Cannot identify label for newick string. Please use : delimiter");
		}
		

		// Get label
		String labelAndAnnotations = bits[0];
		String[] labelSplit = labelAndAnnotations.split("\\[");
		if (!hasChildren && labelSplit.length > 1) {
			this.setAcc(labelSplit[0]);
		}
		
		
	
		
		// Validate there is a terminal closing bracket
		if (hasChildren && !newick.substring(newick.length()-1, newick.length()).equals(")")) {
			//throw new Exception("Could not parse newick because there is a missing ')'.");
		}
		
		// Find the first closing bracket on the same level
		String char2;
		List<StringBuilder> childSubtrees = new ArrayList<>();
		int level = 0;
		int pos = 1;
		int childPos = 1;
		int annotationLevel = 0;
		int annotationPos = -1;
		double length = 0;
		while (pos < newick.length()) {
			
			
			char2 = newick.substring(pos, pos+1);
			
			// Annotation
			if (char2.equals("[")) {
				annotationLevel++;
				annotationPos = pos+1;
			}
			else if (char2.equals("]")) {
				if (annotationLevel == 1) this.parseAnnotation(newick.substring(annotationPos, pos));
				annotationLevel--;
			}
			
			// Array annotation
			else if (char2.equals("{")) {
				annotationLevel++;
			}
			else if (char2.equals("}")) {
				annotationLevel--;
			}
			
			else if (annotationLevel == 1 && char2.equals(",")) {
				
				// Parse annotation
				this.parseAnnotation(newick.substring(annotationPos, pos));
				annotationPos = pos+1;
				
			}
			
			// Parse node height
			else if (annotationLevel == 0 && level == 0 && char2.equals(":")) {
				String lengthStr = newick.substring(pos+1).split("(,|[(]|[)])")[0];
				length = Double.parseDouble(lengthStr);
				if (!hasChildren) break;
			}
			
			// Grandchild subtree
			else if (annotationLevel == 0 && char2.equals("(")) {
				level++;
			}
			
			// New child of this node
			else if (annotationLevel == 0 && char2.equals(",") && level == 0) {
				childSubtrees.add(new StringBuilder(newick.substring(childPos, pos)));
				childPos = pos+1;
			}
			
			// Child subtree has closed
			else if (annotationLevel == 0 && char2.equals(")")) {
				if (level == 0) {
					childSubtrees.add(new StringBuilder(newick.substring(childPos, pos+1)));
					childPos = pos+1;
					break;
				}
				level--;
			}
			
			pos ++;
		}
		
		
		// Validate
		if (hasChildren && childSubtrees.isEmpty()) {
			throw new Exception("Could not parse newick. Perhaps there is a missing ')'.");
		}
		
		
		// Set node height
		if (!this.isRoot()) {
			this.setHeight(this.parent.getHeight() - length);
			//System.out.println(label + " has tree " + newick + " and height " + this.getHeight());
		}
		
		
		
		// Parse children
		for (StringBuilder subtree : childSubtrees) {
			Node child = new Node();
			this.addChild(child);
			child.parseFromNewick(subtree);
		}
		
		
		
		
	}
	
	
	/**
	 * Parse the annotation
	 * @param annotation
	 */
	private void parseAnnotation(String annotation) {
		
		//System.out.println(annotation);
		
		String[] pair = annotation.split("=");
		if (pair.length == 2) {
			String key = pair[0];
			key = key.replace("&", "");
			String val = pair[1];
			this.annotations.put(key, val);
		}
			
		
	}

	
	/**
	 * Return the height of the youngest child in the subtree
	 * @return
	 */
	public double getYoungestChildHeight() {
		double minHeight = this.getHeight();
		for (Node child : this.getChildren()) {
			minHeight = Math.min(minHeight, child.getYoungestChildHeight());
		}
		return minHeight;
	}
	

	
}
