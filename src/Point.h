/**
 * @file	Point.h
 * @author	R.M. Suddaby
 * @date	Sept 2012
 * @brief	The Point class is a simple class representing a geographic point (lon,lat)
 */

#ifndef POINT_H_
#define POINT_H_

#include <string>


class Point {
private:
	double xval, yval;

public:
	//! Point constructor
	Point(double x = 0.0, double y = 0.0);

	//! Point destructor
	virtual ~Point() {}

	//! Returns the X value of the point
	double X();

	//! Returns the Y value of the point
	double Y();
};

#endif /* POINT_H_ */
