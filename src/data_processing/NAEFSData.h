/**
 * @file	NAEFSData.h
 * @author	Bo Lu
 * @date	May 2020
 * @brief	PointData: A structure used to store NAEFS data
 */

#include <iostream>
#include "Date.h"

#ifndef NAEFSDATA_H_
#define NAEFSDATA_H_

struct NAEFSDataStruct
{
    Date run_date;
    int model_id;
    int stn_id;
    Date forecast_date;

    double temp;
    double rh;
    double precip;
    double ws;
    double wd;

    double ffmc;
    double dmc;
    double dc;
    double bui;
    double isi;
    double fwi;
    double dsr;
};

#endif
