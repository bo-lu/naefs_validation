/**
 * @file	Grid.h
 * @author  R.M. Suddaby, Bo Lu
 * @date    September 2012
 * @brief   Grid: a class representing a geo-referenced raster (grid).
 *          Represents a spatially referenced raster or grid. It is really just a wrapper for a
 *          number of GDAL functions intending to make the interface a little simpler.
 **/

#include <iostream>
#include <gdal.h>
#include <cpl_conv.h>
#include <proj_api.h>
#include <cfloat>

#include "Grid.h"
#include "Point.h"

using namespace std;

Grid::Grid() {
	// TODO Auto-generated constructor stub
}

Grid::Grid(string filename)
{
	m_filename = filename;
}

Grid::~Grid() {
	// TODO Auto-generated destructor stub
}

void Grid::Theme(string theme) { m_theme = theme; }

string Grid::Theme() { return m_theme; }

bool Grid::OpenRead()
{


	m_ds = GDALOpen(m_filename.c_str(), GA_ReadOnly);

	if (m_ds == NULL)
	{
		return false;
	}

//	m_rb = m_ds->GetRasterBand(1);
	m_rb = GDALGetRasterBand(m_ds, 1);

//	m_ds->GetGeoTransform(m_transform);
	GDALGetGeoTransform(m_ds, m_transform);

//	m_xsize = m_ds->GetRasterXSize();
//	m_ysize = m_ds->GetRasterYSize();
	m_xsize = GDALGetRasterBandXSize(m_rb);
	m_ysize = GDALGetRasterBandYSize(m_rb);

	m_ulx = m_transform[0];
	m_uly = m_transform[3];
	m_lrx = m_transform[0] + m_transform[1] * m_xsize;
	m_lry = m_transform[3] + m_transform[5] * m_ysize;

//	m_nodata_value = m_rb->GetNoDataValue();
	m_nodata_value = GDALGetRasterNoDataValue(m_rb, NULL);
	return true;
}

bool Grid::OpenWrite()
{

//	m_ds = (GDALDataset *)GDALOpen(m_filename.c_str(), GA_Update);
	m_ds = GDALOpen(m_filename.c_str(), GA_Update);

	if (m_ds == NULL)
		return false;

//	m_rb = m_ds->GetRasterBand(1);
	m_rb = GDALGetRasterBand(m_ds, 1);
//	m_ds->GetGeoTransform(m_transform);
	GDALGetGeoTransform(m_ds, m_transform);

//	m_xsize = m_ds->GetRasterXSize();
//	m_ysize = m_ds->GetRasterYSize();
	m_xsize = GDALGetRasterBandXSize(m_rb);
	m_ysize = GDALGetRasterBandYSize(m_rb);

	m_ulx = m_transform[0];
	m_uly = m_transform[3];
	m_lrx = m_transform[0] + m_transform[1] * m_xsize;
	m_lry = m_transform[3] + m_transform[5] * m_ysize;

//	m_nodata_value = m_rb->GetNoDataValue();
	m_nodata_value = GDALGetRasterNoDataValue(m_rb, NULL);
	return true;
}

double Grid::ULX() { return m_ulx; }
double Grid::ULY() { return m_uly; }
double Grid::LRX() { return m_lrx; }
double Grid::LRY() { return m_lry; }

void Grid::Close()
{
	GDALClose(m_ds);
}

Grid Grid::CreateCopy(string fileName)
{
	Grid copy;
	GDALDriverH driver;
	GDALDataType gdt;
	float nodatavalue;

#ifdef SFMS_DEBUG
    //cout << "DEBUG: Sfms:Grid:CreateCopy: GDALAllRegister" << endl;
#endif // SFMS_DEBUG
	GDALAllRegister();

#ifdef SFMS_DEBUG
    //cout << "DEBUG: Sfms:Grid:CreateCopy: GDALOpen" << endl;
#endif // SFMS_DEBUG
	m_ds = GDALOpen(m_filename.c_str(), GA_ReadOnly);

#ifdef SFMS_DEBUG
    //cout << "DEBUG: Sfms:Grid:CreateCopy: GDALGetDatasetDriver" << endl;
#endif // SFMS_DEBUG
	driver = GDALGetDatasetDriver(m_ds);
#ifdef SFMS_DEBUG
    //cout << "DEBUG: Sfms:Grid:CreateCopy: GDALAllGetRasterBand" << endl;
#endif // SFMS_DEBUG
	m_rb = GDALGetRasterBand(m_ds, 1);
	gdt = GDALGetRasterDataType(m_rb);
	GDALGetGeoTransform(m_ds, m_transform);
	nodatavalue = GDALGetRasterNoDataValue(m_rb, NULL);
	m_xsize = GDALGetRasterBandXSize(m_rb);
	m_ysize = GDALGetRasterBandYSize(m_rb);
#ifdef SFMS_DEBUG
    //cout << "DEBUG: Sfms:Grid:CreateCopy: GDALCreate" << endl;
#endif // SFMS_DEBUG
	copy.m_ds = GDALCreate(driver, fileName.c_str(), m_xsize, m_ysize, 1, gdt, NULL);
	copy.m_filename = fileName;
	GDALSetProjection(copy.m_ds, GDALGetProjectionRef(m_ds));
	GDALSetGeoTransform(copy.m_ds, m_transform);
	copy.m_rb = GDALGetRasterBand(m_ds, 1);
	GDALSetRasterNoDataValue(copy.m_rb, nodatavalue);


	if (copy.m_ds != NULL)
		GDALClose(copy.m_ds);

	GDALClose(m_ds);

	return copy;
}

int Grid::XSize()
{
	return m_xsize;
}

int Grid::YSize()
{
	return m_ysize;
}

void Grid::SetNoDataValue(float nodatavalue) {
	GDALSetRasterNoDataValue(m_rb, nodatavalue);
}
float Grid::NoDataValue() { return m_nodata_value; }

float Grid::ValueAtCell(int i, int j)
{
	float value;
	//GDALDataType gdt;
	//gdt = GDALGetRasterDataType(m_rb);
	// Changed: Apr. 13, 2015
	// It appears that we can sample from grids of any data type using the
	// GDT_Float32 datatype. Conversion from native type to float32 happens
	// automatically. The calling program can do the conversion to other types as needed.
	if(GDALRasterIO(m_rb, GF_Read, i, j, 1, 1, &value, 1, 1, GDT_Float32, 0, 0) == 0);
	return value;
}

GByte Grid::IntValueAtCell(int i, int j)
{
	GByte value;

	if (m_rb == NULL) cout << "NULL\n";
	if(GDALRasterIO(m_rb, GF_Read, i, j, 1, 1, &value, 1, 1, GDT_Byte, 0, 0) == 0);
	return value;
}

GByte Grid::IntValueAtXY(double x, double y, double *transform)
{
	GByte value;
	int xOff, yOff;

	xOff = (int)(x - m_transform[0]) / m_transform[1];
	yOff = (int)(y - m_transform[3]) / m_transform[5];

	if(GDALRasterIO(m_rb, GF_Read, xOff, yOff, 1, 1, &value, 1, 1, GDT_Byte, 0, 0) == 0);
    return value;
}

float* Grid::ReadScanline(int row) {

	float *pScanline;

	pScanline = (float *)CPLMalloc(sizeof(float) * XSize());
	if(GDALRasterIO(m_rb, GF_Read, 0, row, XSize(), 1, pScanline, XSize(), 1, GDT_Float32, 0, 0) == 0);
	return pScanline;
}

void Grid::WriteScanline(float *scanline, int row) {

	if(GDALRasterIO(m_rb, GF_Write, 0, row, XSize(), 1, scanline, XSize(), 1, GDT_Float32, 0, 0) == 0);

}
void Grid::WriteCell(int i, int j, float value)
{
	if (m_rb == NULL) cout << "NULL";
	if(GDALRasterIO(m_rb, GF_Write, i, j, 1, 1, &value, 1, 1, GDT_Float32, 0, 0) == 0);
}

Point Grid::IndexToCoord(int i, int j)
{
	double x = m_transform[0] + i * m_transform[1] + j * m_transform[2];
	double y = m_transform[3] + i * m_transform[4] + j * m_transform[5];
	Point p(x, y);
	return p;
}

float Grid::ValueAtXY(double x, double y, double *transform)
{
	float value;
	int xOff, yOff;

	xOff = (int)(x - transform[0]) / transform[1];
	yOff = (int)(y - transform[3]) / transform[5];
	if(GDALRasterIO(m_rb, GF_Read, xOff, yOff, 1, 1, &value, 1, 1, GDT_Float32, 0, 0) == 0);
	return value;
}

float Grid::ValueAtXY(double x, double y)
{
	float value = -9999;
	int xOff, yOff;

	xOff = (int)(x - m_transform[0]) / m_transform[1];
	yOff = (int)(y - m_transform[3]) / m_transform[5];

    if(GDALRasterIO(m_rb, GF_Read, xOff, yOff, 1, 1, &value, 1, 1, GDT_Float32, 0, 0) == 0);

    if (value == -FLT_MAX) return -9999;
	return value;
}

string Grid::FileName() { return m_filename; }

