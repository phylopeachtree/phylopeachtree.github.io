package peachtree.epi;

import java.util.HashMap;
import java.util.Set;

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

	
	/**
	 * Get the value of this variable
	 * @param var
	 * @return
	 */
	public String getValue(String var) {
		return this.annotations.get(var);
	}


	/**
	 * Get this accession
	 * @return
	 */
	public String getAccession() {
		return this.accession;
	}


	public Set<String> getVariables() {
		return this.annotations.keySet();
	}
	
	

}
