package peachtree.aln.colourings;

import java.util.HashMap;

public class ClustalAminoColouring extends Colouring {

	// http://www.bioinformatics.nl/~berndb/aacolour.html
	public ClustalAminoColouring() {
		
		 colours = new  HashMap<String, String>();
		 colours.put("G", "orange");
		 colours.put("P", "orange");
		 colours.put("S", "orange");
		 colours.put("T", "orange");
		 
		 colours.put("H", "red");
		 colours.put("K", "red");
		 colours.put("R", "red");
		 
		 colours.put("F", "blue");
		 colours.put("W", "blue");
		 colours.put("Y", "blue");
		 
		 colours.put("I", "green");
		 colours.put("L", "green");
		 colours.put("M", "green");
		 colours.put("V", "green");
		 
	}
	
	
	@Override
	public String getName() {
		return "Clustal";
	}

	@Override
	public boolean isNucleotide() {
		return false;
	}




}
