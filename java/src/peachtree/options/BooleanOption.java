package peachtree.options;


import org.json.JSONObject;

public class BooleanOption extends Option {
	
	
	boolean value;
	
	public BooleanOption(String name, String section, String title, boolean value) {
		this.name = name;
		this.section = section;
		this.title = title; 
		this.value = value;
	}
	
	
	/**
	 * Set the value
	 * @return
	 */
	public boolean getVal() {
		return this.value;
	}
	
	/**
	 * Get the value
	 * @param value
	 */
	public void setVal(boolean value) {
		this.value = value;
	}

	@Override
	public JSONObject toJSON() {
		JSONObject json = new JSONObject().put("name", name).put("section", section).put("title", title)
				.put("value", value).put("type", this.getClass().getSimpleName());
		return json;
	}

}
