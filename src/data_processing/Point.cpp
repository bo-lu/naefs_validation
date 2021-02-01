/**
 * @file	Point.h
 * @author	R.M. Suddaby
 * @date	Sept 2012
 * @brief	The Point class is a simple class representing a geographic point (lon,lat)
 */

#include "Point.h"

Point::Point(double x, double y) {
	xval = x;
	yval = y;
}

double Point::X() { return xval; }
double Point::Y() { return yval; }
