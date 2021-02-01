/**
 * @file	Grid.h
 * @author  R.M. Suddaby, Bo Lu
 * @date    September 2012
 * @brief   Grid: a class representing a geo-referenced raster (grid).
 *          Represents a spatially referenced raster or grid. It is really just a wrapper for a
 *          number of GDAL functions intending to make the interface a little simpler.
 **/

#ifndef GRID_H_
#define GRID_H_

#include <string>
#include <gdal.h>

#include "Point.h"

class Grid {
public:

	//! Default constructor
	Grid();

	//! Constructs an Grid using the filename
	Grid(std::string filename);
	virtual ~Grid();

	//! Set the theme for this Grid
	void Theme(std::string theme_name);

	//! Get the theme for this Grid
	std::string Theme();

	//! Open the Grid for read only operations
	bool OpenRead();
	//! Open the Grid for write operations
	bool OpenWrite();

	//! Close an Grid and release resources
	void Close();

	//! Create a copy of an existing raster on disk
	Grid CreateCopy(std::string filename);

	//! Get the file name for this Grid
	std::string FileName();

	//! Return the number of columns in the raster
	int XSize();
	//! Return the number of rows in the raster
	int YSize();

	//! Return the upper left X value of the rasters map extent
	double ULX();
	//! Return the upper left Y value of the rasters map extent
	double ULY();
	//! Return the lower right X value of the rasters map extent
	double LRX();
	//! Return the lower right Y value of the rasters map extent
	double LRY();

	//! Return the value of the raster that indicates NODATA
	float NoDataValue();

	//! Set the nodata value for this raster
	void  SetNoDataValue(float nodatavalue);
	//! Write a value to the raster cell indicated by the indices i and j
	void WriteCell(int i, int j, float value);

	//! Return the value of a raster cell at index[i,j]
	float ValueAtCell(int i, int j);

	//! Return a scanline of raster data for a given row
	float *ReadScanline(int row);

	//! Write an entire scanline of raster data for a given row
	void WriteScanline(float *scanline, int row);

	//! Return the value of a raster at the coordinate represented by x and y
	float ValueAtXY(double x, double y, double *transform);
	GByte IntValueAtCell(int i, int j);
	GByte IntValueAtXY(double x, double y, double *transform);
	float ValueAtXY(double x, double y);

	//! Return a Point object representing the geographic coordinate at a given cell
	Point IndexToCoord(int i, int j);

	double m_transform[6];
private:
	std::string m_theme;
	std::string m_filename;
	GDALDatasetH m_ds;
	GDALRasterBandH m_rb;
//	double m_transform[6];
	int m_xsize;
	int m_ysize;
	double m_ulx, m_uly, m_lrx, m_lry;
	float m_nodata_value;
};
#endif /* Grid_H_ */
