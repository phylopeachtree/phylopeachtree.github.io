package peachtree.options;


/**
 * Scale x or y
 * @author Jordan
 *
 */
public class Scaling {

	
	double xmin;
	double xmax;
	double ymin;
	double ymax;
	double canvasMinX;
	double canvasMaxX;
	double canvasMinY;
	double canvasMaxY;

	
	
	public Scaling(double xmin, double xmax, double ymin, double ymax, 
					double canvasMinX, double canvasMaxX, double canvasMinY, double canvasMaxY) {
		
		this.xmin = xmin;
		this.xmax = xmax;
		this.ymin = ymin;
		this.ymax = ymax;
		this.canvasMinX = canvasMinX;
		this.canvasMaxX = canvasMaxX;
		this.canvasMinY = canvasMinY;
		this.canvasMaxY = canvasMaxY;
		
	}
	
	
	/**
	 * Scale X and round 
	 * @param x
	 * @return
	 */
	public double scaleX(double x) {
		x = (x - xmin) / (xmax - xmin) * (canvasMaxX - canvasMinX) + canvasMinX;
		return Math.round(x);
	}
	
	
	/**
	 * Scale Y and round 
	 * @param y
	 * @return
	 */
	public double scaleY(double y) {
		y = (y - ymin) / (ymax - ymin) * (canvasMaxY - canvasMinY) + canvasMinY;
		return Math.round(y);
	}


	public double ymax() {
		return ymax;
	}
	
	public double xmax() {
		return xmax;
	}
	
	public double ymin() {
		return ymin;
	}
	
	public double xmin() {
		return xmin;
	}
	
}
