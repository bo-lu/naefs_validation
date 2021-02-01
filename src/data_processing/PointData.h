/**
 * @file	PointData.h
 * @author	Bo Lu
 * @date	May 2020
 * @brief	PointData: A wx point used for NAEFS Validation
 *          The PointData class is designed specifically for the naefs point data. It defines
 *          a map coordinate and associated values.
 */

#include <iostream>

#ifndef POINTDATA_H_
#define POINTDATA_H_


class PointData {
public:

	/**
	 * @brief Default Constructor
	 */
	PointData();

	PointData(double x, double y, double code, std::string name, std::string province, std::string country, std::string elevation);

	/**
	 * @brief Default destructor
	 */
	virtual ~PointData();

	void X(double x);
	void Y(double Y);

    int getCode() const { return m_code; }
    std::string getName() const { return m_name; }
    std::string getProvince() const { return m_province; }
    std::string getCountry() const { return m_country; }
    std::string getElevation() const { return m_elevation; }
    double getX() const { return m_x; }
    double getY() const { return m_y; }

    bool operator==(const PointData & obj2) const
    {///Used for comparison of m_code in .find()
        if(this->getCode() == obj2.getCode())
            return true;
        else
            return false;
    }

private:
	double m_x;		// The X coordinate
	double m_y;		// The Y coordinate
	int m_code;	// The value associated with this point
    std::string m_name;	        // The name associated with this point
	std::string m_province;	    // The province associated with this point
	std::string m_country;	    // The country associated with this point
	std::string m_elevation;	// The elevation associated with this point
	double m_distance;	        // A distance calculated relative to another point


};
#endif /* POINTDATA_H_ */

