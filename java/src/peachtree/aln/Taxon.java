package peachtree.aln;

public class Taxon {

	int id;
	String name;
	boolean isSelected; // Is the taxon selected
	boolean isHighlighted;
	
	public Taxon(int id, String name) {
		this.id = id;
		this.name = name;
		this.isHighlighted = false;
		this.isSelected = false;
	}
	
	public int getID() {
		return id;
	}
	
	public void setName(String name) {
		this.name = name;
	}
	
	public String getName() {
		return name;
	}
	
	public void toggleSelection() {
		this.isSelected = !this.isSelected;
	}
	
	public void isSelected(boolean val) {
		this.isSelected = val;
	}
	
	public boolean isSelected() {
		return this.isSelected;
	}
	
	
	public void toggleHighlighting() {
		this.isHighlighted = !this.isHighlighted;
	}
	
	
	public void isHighlighted(boolean val) {
		this.isHighlighted = val;
	}
	
	public boolean isHighlighted() {
		return this.isHighlighted;
	}
	

	
}
