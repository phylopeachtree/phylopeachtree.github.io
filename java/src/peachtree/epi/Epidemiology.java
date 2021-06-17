package peachtree.epi;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

/**
 * Stores epidemiological information
 * @author jdou557
 *
 */
public class Epidemiology {
	
	
	public List<Case> cases;
	public List<String> annotations;
	
	

	public Epidemiology() {
		this.cases = new ArrayList<>();
		
	}
	
	
	
	/**
	 * Parse a file
	 * @param contents
	 */
	public void parseFile(String contents) throws Exception {
		
		
		this.cases = new ArrayList<>();
		
		// Split into lines
		contents = contents.replaceAll("\r", "");
		String[] lines = contents.split("\n");
		
		// Delimiter
		String delimiter = ",";
		
		// Header
		String[] variables = null;
		
		// Accession name
		int accessionIndex = -1;
		
		
		for (int lineNum = 0; lineNum < lines.length; lineNum++) {
		
			
			String line = lines[lineNum].trim();
			
			// Skip comment lines
			if (line.substring(0, 1).equals("#")) continue;

			
			
			// First line is header
			if (variables == null) {
				
				// What delimiter to use?
				variables = line.split(delimiter);
				
				if (variables.length == 1) {
					delimiter = "\t";
					variables = line.split(delimiter);
					if (variables.length == 1) {
						throw new Exception("Please use a comma or tab delimited file with at least 2 columns.");
					}
				}
				
				this.annotations = new ArrayList<>();
				
				// Ensure there is an accession column
				for (int i = 0; i < variables.length; i ++) {
					String var = variables[i];
					if (accessionIndex == -1 && var.equals("accession")) {
						accessionIndex = i;
					}else {
						if (this.annotations.contains(var)) {
							throw new Exception("Duplicate column name " + var);
						}
						this.annotations.add(var);
					}
				}
				
				if (accessionIndex == -1) {
					throw new Exception("Please ensure the header contains a variable named 'accession' which contains sample names.");
				}
				
				
				
			}
			
			
			// Contents line
			else {
				
				String[] lineContents = line.split(delimiter);
				
				// Right length?
				if (lineContents.length != variables.length) {
					throw new Exception("Line " + (lineNum+1) + " should contain " + variables.length + " elements when split on '" + delimiter + "'" +
								" but it contains " + lineContents.length);
				}
				
				
				// Parse values
				String accession = null;
				HashMap<String, String> annotations = new HashMap<>();
				for (int i = 0; i < variables.length; i ++) {
					String var = variables[i];
					String val = lineContents[i];
					
					if (i == accessionIndex) {
						accession = val;
					}else {
						annotations.put(var, val);
					}
					
				}
				Case person = new Case(accession, annotations);
				this.cases.add(person);
				
			}
			
			
			
		}
		
		
		
		
		
		System.out.println("Successfully parsed " + cases.size() + " cases and " + this.annotations.size() + " annotations");
		
		
		
	}
	
	
	/**
	 * List of column headers / annotations / variable names
	 * @return
	 */
	public List<String> getAnnotations() {
		return this.annotations;
	}
	
}
