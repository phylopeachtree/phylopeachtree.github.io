package peachtree.aln;

import java.util.Calendar;
import java.util.Collection;
import java.util.HashMap;
import java.util.Map;

import org.json.JSONArray;
import org.json.JSONObject;


import peachtree.aln.colourings.Colouring;
import peachtree.options.Scaling;

public class Sequence {
	
	
	

	Taxon taxon;
	StringBuilder sequence;
	int[] sequenceArr;
	int seqLen;
	boolean isNucleotide;
	
	
	
	public Sequence(int id, String acc, StringBuilder seq) throws Exception {
		if (acc.substring(0, 1).equals(">")) acc = acc.substring(1);
		
		this.taxon = new Taxon(id, acc);
		this.sequenceArr = null;
		
		this.sequence = seq;
		
		// Is it nucleotide or amino acid?
		this.isNucleotide = true;
		seqLen = 0;
		for (int i = 0; i < seq.length(); i ++) {
			
			String c = seq.substring(i, i+1);
			if (c.equals("\r") || c.equals("\n")) continue;
			seqLen ++;
			
			
			boolean isNT = Alignment.nt_chars.containsKey(c);
			
			if (!isNT) {
				this.isNucleotide = false;
				break;
			}
			
		}
		
		
	}
	
	
	/**
	 * Prepare int array of the sequence
	 */
	public void prepareArray() throws Exception {
		//System.out.println("prepare array " + this.isNucleotide + " length " + this.sequence.length() + "/" + this.seqLen);
		//System.out.println("'" + this.sequence + "'");
		this.sequenceArr = new int[this.seqLen];
		for (int i = 0; i < this.seqLen; i ++) {
			String site = this.sequence.substring(i, i+1);
			Integer val;
			if (this.isNucleotide) {
				val = Alignment.nt_chars.get(site);
			}else {
				val = Alignment.alpha_chars.get(site);
			}
			//System.out.println("putting " + val + " at site " + i + " for symbol " + site);
			if (val == null) {
				throw new Exception("Unknown character " + site + " at site " + (i+1) + " in " + this.getAcc());
			}
			this.sequenceArr[i] = val;
		}
	}
	


	public StringBuilder getSeq() {
		return this.sequence;
	}
	
	
	public Taxon getTaxon() {
		return this.taxon;
	}
	
	public String getAcc() {
		return this.taxon.getName();
	}
	
	public int getID() {
		return this.taxon.getID();
	}
	
	public JSONObject toJSON() {
		JSONObject json = new JSONObject().put("acc",  this.getAcc()).put("seq", this.sequence);  
		return json;
	}

	public int getLength() {
		return this.seqLen;
	}

	public boolean isNucleotide() {
		return this.isNucleotide;
	}

	public void setIsNucleotide(boolean b) {
		this.isNucleotide = b;
	}

	
	
	/**
	 * Get taxon graphics
	 * @param scaling
	 * @param y
	 * @param height
	 * @param filtering
	 * @return
	 */
	public JSONArray getTaxonGraphics(Scaling scaling, int seqNum, int padding, Filtering filtering, double textSize, boolean showTaxonNumbers) {
		
		JSONArray arr = new JSONArray();
		
		// Do not plot beyond the edge
		if (!scaling.inRangeY(seqNum)) return arr;
		
		// Should this sequence be included?
		if (filtering != null && !filtering.includeTaxon(this.getTaxon())) return arr;
		
		double yc_scaled = scaling.scaleY(seqNum + 0.5);
		
		
		String numberPadding = showTaxonNumbers ? padRight((seqNum+1)+":", padding+2) : "";
		
		
		/// Plot accession
		JSONObject acc_json = new JSONObject();
		acc_json.put("ele", "text").put("x", scaling.scaleX(scaling.xmin())).put("y", yc_scaled);
		acc_json.put("text_anchor", "start"); 
		acc_json.put("value", numberPadding + this.getAcc());
		acc_json.put("title", this.getAcc());
		acc_json.put("font_size", textSize);
		arr.put(acc_json);
		
		
		return arr;
		
	}
	
	public static String padRight(String s, int n) {
	     return String.format("%-" + n + "s", s);  
	}

	
	
	/**
	 * Create JSON object of all elements in this sequence
	 * @param xmin
	 * @param xmax
	 * @param y
	 * @return
	 */
	public JSONArray getSequenceGraphics(Scaling scaling, int seqNum, double ntWidth, Colouring colouring, Filtering filtering, double textSize) {
		
		JSONArray arr = new JSONArray();
		
		// Do not plot beyond the edge
		if (!scaling.inRangeY(seqNum)) return arr;
		
		// Should this sequence be included?
		if (filtering != null && !filtering.includeTaxon(this.getTaxon())) return arr;
		
	
		
		double x = 0;
		
		double heightScaled = scaling.getRowHeight() + 1;
		//double x = scaling.xmin();
		

		
		double yc_rect_scaled = scaling.scaleY(seqNum);
		double yc_text_scaled = scaling.scaleY(seqNum + 0.5);
		
		
		// Add some xshift to the first nucleotide so it doesn't get clipped by left boundary
		Double xshift = null;
		
		
		// Plot sites
		JSONObject nt_bg, nt_font;
		for (int site : filtering.getSites()) {

			// Do not plot beyond the edge
			if (!scaling.inRangeX(x, ntWidth)) {
				x ++;
				continue;
			}
			if (scaling.isAboveRangeX(x, ntWidth)) break;
			
			String symbol = this.getSymbol(site);
			
			
			
			double xc = scaling.scaleX(x);
			if (xshift == null) {
				xshift = scaling.canvasMinX() - xc;
			}
			xc += xshift;
			
			
			
			
			// Background colour
			nt_bg = new JSONObject().put("ele", "rect").put("x", xc).put("y", yc_rect_scaled)
								.put("width", ntWidth+1).put("height", heightScaled);
			if (colouring != null) {
				nt_bg.put("fill", colouring.getColour(symbol));
			}
			//nt_bg.put("fill", "#008cba");
			//nt_bg.put("color", "white");
			
			
			
			arr.put(nt_bg);
			
			
			// Text
			xc = scaling.scaleX(x + 0.5) + xshift;
			nt_font = new JSONObject().put("ele", "text").put("x", xc).put("y", yc_text_scaled);
			nt_font.put("value", symbol);
			nt_font.put("text_anchor", "middle"); // Right alignment
			nt_font.put("title", "Site " + (site+1));
			nt_font.put("font_size", textSize);
			
			arr.put(nt_font);
			
			x ++;
			
			
		}
		
	
		
		
		return arr;
		
	}

	
	public int getSymbolInt(int site) {
		return this.sequenceArr[site];
	}
	

	public String getSymbol(int site) {
		int i = this.getSymbolInt(site);
		String val;
		if (this.isNucleotide) {
			val = Alignment.nt_ids[i];
		}else {
			val = Alignment.alpha_ids[i];
		}
		
		//System.out.println(site + "->" + i + "->" + val);
		
		if (val == null) return "?";
		return val;
	}
	
	
	
	

}







