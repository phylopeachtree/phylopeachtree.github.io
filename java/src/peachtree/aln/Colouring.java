package peachtree.aln;

import java.util.Map;

/**
 * Alignment colouring scheme
 * @author Jordan
 *
 */
public abstract class Colouring {
	
	
	
	
	
	public abstract String getName();
	
	public abstract boolean isNucleotide();
	
	public abstract Map<String, String> getColourMap();
	
	public abstract String getDefaultCol();
	
	
	
	public String getColour(String symbol) {
		String col = this.getColourMap().get(symbol);
		if (col == null) return getDefaultCol();
		return col;
	}
	
	
	public String toJSON() {
		return "";
		//return this.getColourMap().get(symbol);
	}
	
	
	
	

}
