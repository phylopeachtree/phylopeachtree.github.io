package peachtree.options;

import java.util.List;

import org.json.JSONArray;
import org.json.JSONObject;


/**
 * Accepts a discrete value from a domain of values
 * @author jdou557
 *
 */
public class DiscreteOption extends Option {

	
	// Current value
	Object value;
	
	// Domain of values
	List<?> domain;
	
	
	public DiscreteOption(String name, String section, String title, Object value, List<?> domain) throws Exception {
		
		this.name = name;
		this.domain = domain;
		this.section = section;
		this.title = title;
		
		if (domain == null || domain.isEmpty()) {
			throw new Exception(this.getName() + " has no domain");
		}
		
		if (!this.setVal(value)) {
			throw new Exception(value + "is not in the domain of " + this.getName());
		}
		
	}
	
	
	/**
	 * Tries to set the value to val
	 * If 'val' is not in the domain, returns false
	 * @param val
	 * @return
	 */
	public boolean setVal(Object val) {
		
		
		this.value = val;
		return true;
		
		/*
		if (val == null) {
			this.value = val;
			return true;
		}
		
		// Validate
		for (Object d : domain) {
			if (d.equals(val)) {
				this.value = val;
				return true;
			}
		}
		
		return false;
		*/
	}
	
	
	/**
	 * Gets the value of this option
	 * @return
	 */
	public Object getVal() {
		return this.value;
	}
	
	public List<?> getDomain(){
		return this.domain;
	}
	
	
	
	@Override
	public JSONObject toJSON() {
		JSONObject json = new JSONObject().put("name", name).put("section", section).put("title", title)
				.put("value", value).put("type", this.getClass().getSimpleName());
		JSONArray domainArr = new JSONArray();
		for (Object val : domain) {
			if (val instanceof String) domainArr.put(val);
			else domainArr.put(val.toString());
		}
		json.put("domain", domainArr);
		return json;
	}

}
