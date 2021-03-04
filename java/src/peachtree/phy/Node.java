package peachtree.phy;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import org.json.JSONArray;
import org.json.JSONObject;

import peachtree.aln.Filtering;
import peachtree.options.Scaling;


public class Node {

	
	int nodeNr;
	String acc;
	double height;
	Node parent;
	List<Node> children;
	
	public Node() {
		this.acc = "";
		this.nodeNr = 0;
		this.height = 0;
		this.parent = null;
		this.children = new ArrayList<>();
	}
	
	public Node(int index) {
		this.acc = "";
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
		if (this.acc.isEmpty()) return "" + (this.nodeNr+1);
		return this.acc;
	}
	
	public void setAcc(String acc) {
		this.acc = acc;
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
		clone.acc = this.acc;
		clone.height = this.height;
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
            //buf.append(getNewickMetaData());
        }

        buf.append(this.getAcc());
        buf.append(":");
        //if (printMetaData) buf.append(getNewickLengthMetaData());
        buf.append(getLength());

        return buf.toString();
    }

    
    
    /**
     * Get the svg graphics of this subtree
     * @param dy
     * @param filtering
     * @param branchWidth
     * @return y
     */
	public double getGraphics(JSONArray objs, Filtering filtering, Scaling scaling, double branchWidth, boolean showTaxaOnTree, double yshift) {
		
		//System.out.println("node height " + this.getHeight() + " max height " + scaling.xmax() + "/" + scaling.xmin());
		
		
		
		double x2 = this.getHeight();
		double x2Scaled = scaling.scaleX(x2);
		double maxY = Double.NEGATIVE_INFINITY;
		double minY = Double.POSITIVE_INFINITY;
		
		// If leaf, go with node ordering
		double y = 0;
		if (this.isLeaf()) {
			//y = scaling.getYPos(this.getNr()) + (0.5*scaling.getRowHeight()); //this.getNr() * dy + (0.5*dy);
			y = this.getNr() + 0.5;
		}
		
		
		// If parent node, y is midpoint between children
		else {
			
			
			
			for (Node child : this.getChildren()) {
				double ychild = child.getGraphics(objs, filtering, scaling, branchWidth, showTaxaOnTree, yshift);
				y += ychild;
				if (ychild > maxY) maxY = ychild;
				if (ychild < minY) minY = ychild;
			}
			y = y / this.getChildCount();
			
			
		}
		
		
		
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
		if (!this.isRoot()) {
			
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
		
		return y;
		
	}


	
}
