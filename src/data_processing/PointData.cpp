/**
 * @file	PointData.h
 * @author	Bo Lu
 * @date	May 2020
 * @brief	PointData: A wx point used for NAEFS Validation
 *          The PointData class is designed specifically for the naefs point data. It defines
 *          a map coordinate and associated values.
 */

#include "PointData.h"
#include <math.h>
#include <iostream>

using namespace std;

PointData::PointData() {
	// TODO Auto-generated constructor stub

}

PointData::~PointData() {
	// TODO Auto-generated destructor stub
}

PointData::PointData(double x, double y, double code, std::string name, std::string province, std::string country, std::string elevation) :
           m_x(x), m_y(y), m_code(code), m_name(name), m_province(province), m_country(country), m_elevation(elevation){}


void PointData::X(double x) { m_x = x; }
void PointData::Y(double y) { m_y = y; }

