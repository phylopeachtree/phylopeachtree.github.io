package peachtree.aln;

public class Taxon {

	int id;
	String name;
	boolean isSelected; // Is the taxon selected
	
	public Taxon(int id, String name) {
		this.id = id;
		this.name = name;
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


	
}
