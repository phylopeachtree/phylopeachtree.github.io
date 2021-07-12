/*
 * Scaling.h
 *
 *  Created on: 6/07/2021
 *      Author: jdou557
 */




#ifndef OPTIONS_SCALING_H_
#define OPTIONS_SCALING_H_



#include <iostream>
#include <vector>

using namespace std;

class Scaling {
public:
	Scaling(double canvasMinX, double canvasMaxX, double canvasMinY, double canvasMaxY);
	Scaling(double canvasMinX, double canvasMaxX, double canvasMinY, double canvasMaxY, double xmin, double xmax);
	void setScroll(double x, double y, double fullWidth, double fullHeight);
	double getScrollYLength();
	double getScrollXLength();
	void setRowHeight(double rowH);
	double getRowHeight();
	bool inRangeY(double rowPos);
	bool isAboveRangeY(double rowPos);
	double scaleY(double rowPos);
	bool inRangeX(double x);
	bool inRangeX(double x, double margin);
	bool isAboveRangeX(double x);
	bool isAboveRangeX(double x, double margin);
	double scaleX(double x);

	double getXmax();
	double getXmin();
	double getYmax();
	double getYmin();

	double getCanvasMinX();
	double getCanvasMaxX();
	double getCanvasMinY();
	double getCanvasMaxY();

	bool inView();

private:
	const double MIN_THUMB_SIZE = 20;
	double xmin = 0;
	double xmax = 1;
	double canvasMinX;
	double canvasMaxX;
	double canvasMinY;
	double canvasMaxY;
	double rowH = 0;
	double scrollX = 0;
	double scrollY = 0;
	double fullWidth = 0;
	double fullHeight = 0;

};

#endif /* OPTIONS_SCALING_H_ */
