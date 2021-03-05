package peachtree.options;


import org.json.JSONObject;

public class BooleanOption extends Option {
	
	
	boolean value;
	
	public BooleanOption(String name, String section, String title, boolean value) {
		this(name, section, title, value, false);
	}
	
	
	public BooleanOption(String name, String section, String title, boolean value, boolean hide) {
		this.name = name;
		this.section = section;
		this.title = title; 
		this.value = value;
		this.hide = hide;
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
		JSONObject json = new JSONObject().put("value", value);
		super.modifyJSON(json);
		return json;
	}

}
