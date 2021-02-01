/**
 * @file	GridDataDriver.h
 * @author	Bo Lu
 * @date	May 2020
 * @brief	GridDataDriver: A simple driver to help in extracting grid values from geotiffs
 */

#include <gdal_priv.h>
#include <cpl_conv.h>

#include <ogr_spatialref.h>

#include "ogrsf_frmts.h"
#include "GridDataDriver.h"

GridDataDriver::GridDataDriver(std::string raster_location, std::string theme, Date date)
{
    m_date = date.DateString();
    /// Create the file location string
    m_grid_locn = std::string(raster_location) + "/" + theme + m_date + ".tif";
    /// Load up the grid
    m_ras = Grid(m_grid_locn);
}

void GridDataDriver::Close()
{
	m_ras.Close();
}

double GridDataDriver::getGridValue(double lat, double lon)
{
	try
	{ ///Must be initialized once
	    GDALAllRegister();
	    OGRRegisterAll();
	}
	catch (const std::exception& e)
	{
	    std::cout << "GDAL could not initialize: " << e.what();
	    return -1;
	}

	try
	{
        OGRSpatialReferenceH hLatLong, hsfms_epsg;
        OGRCoordinateTransformationH hCT;
        double dfZ = 0.0;

        hLatLong   = OSRNewSpatialReference( NULL );
        hsfms_epsg = OSRNewSpatialReference( NULL );

        OSRImportFromEPSG(hLatLong, 4326 );
        OSRImportFromEPSG(hsfms_epsg, 3978 );
        hCT = OCTNewCoordinateTransformation( hLatLong, hsfms_epsg );

        if( hCT == NULL )
        {
            ;
        }

		if(!m_ras.OpenRead()) throw std::runtime_error ("FATAL: raster could not be opened for reading");

		OCTTransform( hCT, 1, &lon, &lat, &dfZ );
		//std::cout << lon << "\t" << lat << std::endl;

		if (m_ras.ValueAtXY(lon,lat) != -9999)
            return m_ras.ValueAtXY(lon,lat);
        else
            return -9999;
	}
	catch (const std::exception& e)
	{
        std::cout << "Error: " << e.what();
        return -9999;
	}

    return 0;
}
