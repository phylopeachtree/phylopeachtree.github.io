package peachtree.aln;

import java.util.ArrayList;
import java.util.Calendar;
import java.util.Collection;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.json.JSONArray;
import org.json.JSONObject;

import peachtree.options.Scaling;

public class Alignment {
	
	
	List<Sequence> sequences;
	boolean isNucleotide;
	int alignmentLength;
	
	
	Colouring colouring;
	Filtering filtering;
	
	
	
	static public Map<String, Integer> nt_chars = null;
	static public String[] nt_ids;
	static public Map<String, Integer> alpha_chars = null;
	static public String[] alpha_ids;

	
	public Alignment(String fasta) throws Exception{
		
		
		if (fasta.isEmpty()) throw new Exception("Empty file contents");
		
		if (nt_chars == null) prepareAlignmentChars();
		
		this.sequences = new ArrayList<>();
		this.alignmentLength = -1;
		this.isNucleotide = true;

		// Parse fasta
		String[] lines = fasta.split("\n");
		String acc = "";
		boolean parsedSeq = true;
		int seqID = 0;
		
		StringBuilder seq = new StringBuilder();
		
		long timeOnSeqs = 0;
		long timeOnStrbuilding = 0;
		for (String line : lines) {
			
			//System.out.println(line);
			
			if (line.substring(0,1).equals(">")) {
				
				long t0 = Calendar.getInstance().getTimeInMillis();
				
				if (!parsedSeq) {
					throw new Exception("Cannot find sequence for " + acc);
				}
				
				
				// Parse the sequence
				if (seq.length() > 0) {
					
					
					Sequence sequence = new Sequence(seqID, acc, seq);
					if (this.sequences.isEmpty()) {
						this.alignmentLength = sequence.getLength();
						this.isNucleotide = sequence.isNucleotide();
					}else {
						
						// All sequences must have same length
						if (sequence.getLength() != alignmentLength) {
							throw new Exception("Cannot parse sequence " + sequence.getAcc() + " because of length mismatch (" + 
									sequence.getLength() + " != " + alignmentLength + ")");
						}
						
						// Is it still a nucleotide alignment?
						if (this.isNucleotide && !sequence.isNucleotide()) {
							this.isNucleotide = false;
							for (Sequence s : sequences) {
								s.setIsNucleotide(false);
							}
						}
						
					}
					
					sequences.add(sequence);
					seq = new StringBuilder();
				}
				
				acc = line;
				parsedSeq = false;
				
				long t1 = Calendar.getInstance().getTimeInMillis();
				timeOnSeqs += (t1-t0);
				
			}
			
			
			
			
			else {
				
				long t0 = Calendar.getInstance().getTimeInMillis();
				
				parsedSeq = true;
				seq.append(line.toUpperCase());
				
				long t1 = Calendar.getInstance().getTimeInMillis();
				timeOnStrbuilding += (t1-t0);
			}
			
		}
		
		
		
		if (this.sequences.isEmpty()) {
			throw new Exception("Cannot detect any sequences in the alignment. Are you sure this is .fasta?");
		}
		
		
		// Build all sequence arrays
		for (Sequence sequence : sequences) {
			sequence.prepareArray();
		}
		
		
		long t1 = Calendar.getInstance().getTimeInMillis();
		
		
		// Default filtering
		this.filtering = new Filtering(true, null, this);
		
		// Default colouring
		this.colouring = new NucleotideColouring();
		
		
		System.out.println("Parsed an alignment with " + this.alignmentLength + " sites and " + this.sequences.size() + " taxa");
		
		
		long timeOnFilteringT = Calendar.getInstance().getTimeInMillis() - t1;
		System.out.println(timeOnFilteringT + "ms on filtering");
		System.out.println(timeOnSeqs + "ms on seq");
		System.out.println(timeOnStrbuilding + "ms on str");
		
		
	}
	
	
	/**
	 * Alignment colouring
	 * @param colouring
	 */
	public void setColouring(Colouring colouring) {
		this.colouring = colouring;
	}
	
	
	/**
	 * Alignment taxon/site filtering
	 * @return
	 */
	public void setFiltering(Filtering filtering) {
		this.filtering = filtering;
	}
	
	public JSONObject toJSON() {
		
		// Array of sequences
		JSONArray arr = new JSONArray();
		for (int i = 0; i < this.sequences.size(); i ++) {
			Sequence seq = this.sequences.get(i);
			arr.put(seq.toJSON());
		}
		
		
		// Alignment
		JSONObject json = new JSONObject().put("alignment",  arr); 
		json.put("length", this.alignmentLength).put("ntaxa", this.sequences.size()).put("isNT", this.isNucleotide);
		return json;
		
		
	}



	/**
	 * Get graphics of the alignment
	 * @param xmin
	 * @param xmax
	 * @param ymin
	 * @param ymax
	 * @return
	 */
	public JSONArray getGraphics(Scaling scaling) {
		
		
		JSONArray objs = new JSONArray();
		
		double dy = (scaling.ymax() - scaling.ymin()) / this.sequences.size();
		double y = scaling.ymin();
		for (Sequence sequence : sequences) {
			
			
			//long start = Calendar.getInstance().getTimeInMillis();
			
			objs.putAll(sequence.getGraphics(scaling, y, dy, colouring, filtering));
			
			//long finish = Calendar.getInstance().getTimeInMillis();
			//System.out.println("converted sequence " + sequence.getAcc() + " to json (" + (finish - start) + "ms)");
			y += dy;
		}
		
		return objs;
		
		
	}


	public int getLength() {
		if (this.sequences == null) return 0;
		return this.alignmentLength;
	}


	public int getNtaxa() {
		if (this.sequences == null) return 0;
		return this.sequences.size();
	}


	public Sequence getSequence(int i) {
		if (this.sequences == null) return null;
		return this.sequences.get(i);
	}
	
	
	
	
	/**
	 * Prepare datatype maps
	 */
	public static void prepareAlignmentChars() {
		
		nt_chars = new HashMap<>();
		
		alpha_chars = new HashMap<>();
		
		// Nucleotide
		int i = -1;
		nt_chars.put("A", ++i);
		nt_chars.put("C", ++i);
		nt_chars.put("G", ++i);
		nt_chars.put("T", ++i);
		nt_chars.put("U", ++i);
		nt_chars.put("N", ++i);
		nt_chars.put("-", ++i);
		
		// Reverse mapping
		nt_ids = new String[nt_chars.size()];
		for (String key : nt_chars.keySet()) {
			int index = nt_chars.get(key);
			nt_ids[index] = key;
			//System.out.println(key + "->" + index);
		}
		
		
		// General case (including aa)
		i = -1;
		alpha_chars.put("A", ++i);
		alpha_chars.put("B", ++i);
		alpha_chars.put("C", ++i);
		alpha_chars.put("D", ++i);
		alpha_chars.put("E", ++i);
		alpha_chars.put("F", ++i);
		alpha_chars.put("G", ++i);
		alpha_chars.put("H", ++i);
		alpha_chars.put("I", ++i);
		alpha_chars.put("J", ++i);
		alpha_chars.put("K", ++i);
		alpha_chars.put("L", ++i);
		alpha_chars.put("M", ++i);
		alpha_chars.put("N", ++i);
		alpha_chars.put("O", ++i);
		alpha_chars.put("P", ++i);
		alpha_chars.put("Q", ++i);
		alpha_chars.put("R", ++i);
		alpha_chars.put("S", ++i);
		alpha_chars.put("T", ++i);
		alpha_chars.put("U", ++i);
		alpha_chars.put("V", ++i);
		alpha_chars.put("W", ++i);
		alpha_chars.put("X", ++i);
		alpha_chars.put("Y", ++i);
		alpha_chars.put("Z", ++i);
		alpha_chars.put("-", ++i);

		
		// Reverse mapping
		alpha_ids = new String[alpha_chars.size()];
		for (String key : alpha_chars.keySet()) {
			int index = alpha_chars.get(key);
			alpha_ids[index] = key;
			//System.out.println(key + "->" + index);
		}
		
		
		
	}
	
	

	

}

















