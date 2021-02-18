package peachtree.aln;

import java.util.HashMap;
import java.util.Map;


public class NucleotideColouring extends Colouring {

	
	 Map<String, String> colours = new  HashMap<String, String>();
	 
	
	
	 
	public NucleotideColouring() {
		 
		 colours.put("A", "red");
		 colours.put("C", "blue");
		 colours.put("G", "green");
		 colours.put("T", "yellow");
		 colours.put("U", "yellow");
		 colours.put("N", "pink");
		 
	}
	
	@Override
	public String getName() {
		return "Simple nucleotide";
	}

	@Override
	public boolean isNucleotide() {
		return true;
	}

	
	@Override
	public Map<String, String> getColourMap() {
		return colours;
	}

	@Override
	public String getDefaultCol() {
		return "white";
	}
	
	
	

	
	
	
	

}
