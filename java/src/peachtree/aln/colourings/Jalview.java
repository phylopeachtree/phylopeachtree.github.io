package peachtree.aln.colourings;

import java.util.HashMap;
import java.util.Map;


public class Jalview extends Colouring {

	
	
	 
	
	
	 
	public Jalview() {
		
		
		 colours = new  HashMap<String, String>();
		 colours.put("A", "#64F73F");
		 colours.put("C", "#FFB340");
		 colours.put("G", "#EB413C");
		 colours.put("T", "#3C88EE");
		 colours.put("U", "#3C88EE");
		 //colours.put("N", "white");
		 
	}
	
	@Override
	public String getName() {
		return "Jalview";
	}

	@Override
	public boolean isNucleotide() {
		return true;
	}


	
	
	

	
	
	
	

}
