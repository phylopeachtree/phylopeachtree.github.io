package peachtree.epi;

import java.util.HashMap;

public class Case {
	
	
	String accession;
	HashMap<String, String> annotations;
	
	public Case(String accession, HashMap<String, String> annotations) {
		this.accession = accession;
		this.annotations = annotations;
		
		System.out.print("Accession " + accession);
		for (String var : annotations.keySet()) {
			System.out.print("\t" + var + "=" + annotations.get(var) + ".");
		}
		System.out.println();
		
	}

	
	
	public String getValue(String var) {
		return this.annotations.get(var);
	}
	
	

}
