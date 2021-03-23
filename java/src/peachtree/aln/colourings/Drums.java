package peachtree.aln.colourings;

import java.util.HashMap;


/**
 * http://biomodel.uah.es/en/model4/dna/atgc.htm
 * @author Jordan
 *
 */
public class Drums extends Colouring {

	 
	public Drums() {
		
		
		 colours = new  HashMap<String, String>();
		 colours.put("A", "#5050ff");
		 colours.put("C", "#e00000");
		 colours.put("G", "#00c000");
		 colours.put("T", "#e6e600");
		 colours.put("U", "#e6e600");
		 //colours.put("N", "white");
		 
	}
	
	@Override
	public String getName() {
		return "DRuMS";
	}

	@Override
	public boolean isNucleotide() {
		return true;
	}


}
