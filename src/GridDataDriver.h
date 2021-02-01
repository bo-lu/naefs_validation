/**
 * @file	GridDataDriver.h
 * @author	Bo Lu
 * @date	May 2020
 * @brief	GridDataDriver: A simple driver to help in extracting grid values from geotiffs
 */

#include <iostream>
#include <stdexcept>
#include <iterator>

#include "Grid.h"
#include "Date.h"

#ifndef GRIDDATADRIVER_H_
#define GRIDDATADRIVER_H_

class GridDataDriver {
    private:
        std::string m_date;
        std::string m_grid_locn;
        Grid m_ras;


    public:
        GridDataDriver();
        void Close();
        //! Constructs using the location, theme, and date
        GridDataDriver(std::string raster_location, std::string theme, Date date);

        double getGridValue(double lat, double lon);

};

#endif /* GRIDDATADRIVER_H_ */
