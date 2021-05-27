package peachtree.phy;

import java.math.BigDecimal;
import java.math.MathContext;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;

import org.json.JSONArray;
import org.json.JSONObject;

import peachtree.aln.Filtering;
import peachtree.aln.Taxon;
import peachtree.options.OptionsAPI;
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
    	
    	
    	StringBuilder str = new StringBuilder();
    	if (this.isLeaf()) {
    		str.append(this.getAcc());
    	}
    	else {
    		str.append("Click node to switch child ordering.");
    	}
    	str.append("\nheight=").append(OptionsAPI.sf(this.getHeight()));
    	if (this.annotations.isEmpty()) return str.toString();
    	str.append("\n");
    	int i = 0;
    	for (String key : this.annotations.keySet()) {
    		String value = this.annotations.get(key);
    		
    		
    		try {
	    		String[] bits = value.split(",");
	    		String roundedValue = "";
	    		for (int j = 0; j < bits.length; j ++) {
	    			double val = Double.parseDouble(bits[j]);
	    			val = OptionsAPI.sf(val);
	    			roundedValue += Double.toString(val);
	    			if (j < bits.length-1) roundedValue += ", ";
	    		}
	    		value = roundedValue;
    			
    		}catch (Exception e) {
    			
    			
    			
    			
    			
    		}
    		str.append(key).append("=").append(value);
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
	public Double getGraphics(boolean isRoot, JSONArray objs, Filtering filtering, Scaling scaling, double branchWidth, 
			boolean showTaxaOnTree, double yshift, double nodeRadius, String internalLabel, String leafLabel, double fontSize, int rounding,
			boolean transmissionTree) {
		
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
				Double ychild = child.getGraphics(false, objs, filtering, scaling, branchWidth, showTaxaOnTree, yshift, nodeRadius, internalLabel, leafLabel, fontSize, rounding, transmissionTree);
				if (ychild != null) {
					if (y == null) y = 0.0;
					y += ychild;
					if (ychild > maxY) maxY = ychild;
					if (ychild < minY) minY = ychild;
					nValidChildren ++;
				}
			}
			if (y != null) {
				if (transmissionTree) y = minY;
				else y = y / nValidChildren;
			}
			
			
		}
		
		
		
		// If this has y=null, then do not plot the subtree
		if (y == null) return y;
		
		double yscaled = scaling.scaleY(y);
		yscaled += yshift;
		boolean inrangeY = scaling.inRangeY(y);
		
		// Dashed line to taxa
		if (this.isLeaf() && showTaxaOnTree && this.getHeight() > 0) {
			
			// Only draw if this node is in y-range
			if (inrangeY) {
				
				
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
			if (scaling.inRangeY(minY) || scaling.inRangeY(maxY) || inrangeY) {
			
				
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
			if (inrangeY) {
			
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
		if (nodeRadius > 0 && inrangeY) {
			JSONObject node_json = new JSONObject();
			node_json.put("ele", "circle").put("cx", x2Scaled).put("cy", yscaled).put("r", nodeRadius).put("fill", "black");
			if (!this.isLeaf()) node_json.put("class", "node").put("i", this.nodeNr);
			if (!this.annotations.isEmpty())  node_json.put("title", this.getTidyMetaData());
			objs.put(node_json);
		}
		
		
		
		// Internal/leaf node annotations?
		if (inrangeY && fontSize > 0) {
			
			String label = this.isLeaf() ? leafLabel : internalLabel;
			if (label != null && this.annotations.containsKey(label)) {
			
				// Is it a double? If so then round it
				String val = this.annotations.get(label);
				try {
				    double d = Double.parseDouble(val);
				    BigDecimal bd = new BigDecimal(d);
				    bd = bd.round(new MathContext(rounding));
				    val = "" + bd.doubleValue();
				}
				catch (Exception e) {};
				
				
				
				// Plot annotation
				JSONObject label_json = new JSONObject();
				label_json.put("ele", "text").put("x", x2Scaled + nodeRadius).put("y", yscaled);
				label_json.put("text_anchor", "start"); 
				label_json.put("alignment_baseline", "middle"); 
				label_json.put("value", val);
				label_json.put("font_size", fontSize);
				objs.put(label_json);
			
			}
			
			
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
		char char1 = newick.charAt(0);
		boolean hasChildren = char1 == '(';
		

		// Get label
		//String[] labelSplit = labelAndAnnotations.split("\\[");
		//System.out.println(labelSplit[0] + " -> " + acc);
		if (!hasChildren) {
			
			// What is the node's name
			String[] bits = newick.toString().split(":");
			if (bits.length < 2 || bits[0].isEmpty()) {
				throw new Exception("Cannot identify label for newick string. Please use : delimiter");
			}
			
			String acc = bits[0].replaceAll("([(]|\\[.+)", "");
			this.setAcc(acc);
		}
		
		

		// Find the first closing bracket on the same level
		char char2;
		List<StringBuilder> childSubtrees = new ArrayList<>();
		int level = 0;
		int pos = 1;
		int childPos = 1;
		int annotationLevel = 0;
		int annotationPos = -1;
		double length = 0;
		while (pos < newick.length()) {
			
			
			char2 = newick.charAt(pos); 
			

			if (annotationLevel == 1 && char2 == ',') {
				
				// Parse annotation
				this.parseAnnotation(newick.substring(annotationPos, pos));
				annotationPos = pos+1;
				
			}
			
			// Parse node height
			else if (annotationLevel == 0 && level == 0 && char2 == ':') {
				String lengthStr = newick.substring(pos+1).replaceAll("(,|[(]|[)]).*", "");
				length = Double.parseDouble(lengthStr);
				if (!hasChildren) break;
			}
			
			// Grandchild subtree
			else if (annotationLevel == 0 && char2 == '(') {
				level++;
			}
			
			// New child of this node
			else if (annotationLevel == 0 && level == 0 && char2 == ',') {
				childSubtrees.add(new StringBuilder(newick.substring(childPos, pos)));
				childPos = pos+1;
			}
			
			// Child subtree has closed
			else if (annotationLevel == 0 && char2 == ')') {
				if (level == 0) {
					childSubtrees.add(new StringBuilder(newick.substring(childPos, pos+1)));
					childPos = pos+1;
					break;
				}
				level--;
			}
			
			// Annotation
			else if (char2 == '[') {
				annotationLevel++;
				annotationPos = pos+1;
			}
			else if (char2 == ']') {
				if (annotationLevel == 1) this.parseAnnotation(newick.substring(annotationPos, pos));
				annotationLevel--;
			}
			
			// Array annotation
			else if (char2 == '{') {
				annotationLevel++;
			}
			else if (char2 == '}') {
				annotationLevel--;
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
		
		int pos = annotation.indexOf("=");
		
		
		//String[] pair = annotation.split("=", 2);
		String key = annotation.substring(0, pos);
		String val = annotation.substring(pos+1);
		//if (pair.length == 2) {
			//String key = pair[0];
			key = key.replace("&", "");
			//String val = pair[1];
			val = val.replace("{", "").replace("}", "");
			this.annotations.put(key, val);
			//System.out.println(key + " =" + val);
		//}
			
		
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

	
	/**
	 * Populate the list with a unique list of annotations in the subtree
	 * @param annotations2
	 */
	public void getAllAnnotations(List<String> annotations) {
		
		for (String key : this.annotations.keySet()) {
			if (!annotations.contains(key)) annotations.add(key);
		}
		for (Node child : this.getChildren()) {
			child.getAllAnnotations(annotations);
		}
		
	}

	
	/**
	 * Rotate the ordering of the children
	 */
	public void rotateChildren() {
		
		if (this.getChildCount() == 0) return;
		
		Node lastChild = this.getChild(this.getChildCount()-1);
		this.children.add(0, lastChild);
		this.children.remove(this.getChildCount()-1);
		
		
	}
	

	
}
