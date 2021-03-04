package peachtree.aln.colourings;

import java.util.HashMap;
import java.util.Map;

import org.json.JSONArray;
import org.json.JSONObject;

/**
 * Alignment colouring scheme
 * @author Jordan
 *
 */
public abstract class Colouring {
	
	
	Map<String, String> colours = new  HashMap<String, String>();
	SiteColourFilter colFilter = SiteColourFilter.all;
	
	public abstract String getName();
	public abstract boolean isNucleotide();
	
	
	public String getDefaultCol() {
		return "white";
	}
	
	
	
	public String getColour(String symbol) {
		String col = this.colours.get(symbol);
		if (col == null) return getDefaultCol();
		return col;
	}
	
	
	public String toJSON() {
		JSONObject json = new JSONObject();
		for (String symbol : colours.keySet()) {
			String colour = colours.get(symbol);
			json.put(symbol, colour);
		}
		return json.toString();
		//return this.getColourMap().get(symbol);
	}
	
	@Override
	public String toString() {
		return this.getName();
	}
	
	
	/**
	 * Which sites should be coloured
	 * @param val
	 */
	public void setSiteColourFilter(SiteColourFilter val) {
		this.colFilter = val;
	}
	
	

}
