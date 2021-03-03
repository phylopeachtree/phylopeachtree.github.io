package peachtree.aln.colourings;

import java.util.HashMap;


/**
 * https://github.com/AliView/AliView/blob/master/src/main/java/aliview/color/DefaultColorScheme.java
 * @author Jordan
 *
 */
public class Aliview extends Colouring {

	

	
	 
	public Aliview() {
		
		
		 colours = new  HashMap<String, String>();
		 colours.put("A", "#5ADC5A");
		 colours.put("C", "#6464FA");
		 colours.put("G", "#5A5A5A");
		 colours.put("T", "#F58282");
		 colours.put("U", "#F58282");
		 
		 
	}
	
	@Override
	public String getName() {
		return "Aliview";
	}

	@Override
	public boolean isNucleotide() {
		return true;
	}
	



}

