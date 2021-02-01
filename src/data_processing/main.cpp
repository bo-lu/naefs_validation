#include <iostream>
#include <vector>
#include <boost/foreach.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/filesystem.hpp>
#include <boost/multi_array.hpp>
#include <algorithm>
#include <math.h>

#include <cpl_conv.h>

#include "Date.h"
#include "XMLDataDriver.h"
#include "NAEFSData.h"
#include "GridDataDriver.h"


bool ProcessDailyData (std::ifstream&, std::vector <NAEFSDataStruct> &, Date);
bool compareDateStationModel(NAEFSDataStruct &, NAEFSDataStruct &);
double calcPerc20(double data[20], double percentile);
double calcPerc40(double data[40], double percentile);
std::string findCodeValue(int code, std::vector<PointData> v_data, std::string key);

int main()
{
    std::cout << "Processing NAEFS Locations XML.." << std::endl;

    /** Begin Processing NAEFS Locations XML.. **/

    std::string xml_location = "C:\\cpp_projects\\config\\locations.xml";
    XMLDataDriver xmldata;
    std::vector<PointData> v_data;
    std::vector<PointData> v_country;
    std::vector<PointData>::iterator it;
    int counter = 0;
    std::ofstream outfile;

    xmldata.setParentName("location_list");
    xmldata.setChildName("location");
    xmldata.setLonString("longitude");
    xmldata.setLatString("latitude");

    if(xmldata.Open(xml_location) == true)
    {
        ///the getData function will extract all the data from the ECCC XML file and save it to v_data
        v_data = xmldata.getData("code", "file_desc", "province_state", "pays_country", "altitude");
        ///v_data, the vector of lat, lon and station 'code' and other info
    }

    std::string outFileLocn = "C:\\cpp_projects\\data\\locations.csv";

    outfile.open(outFileLocn);
    std::cout << "Writing to: " << outFileLocn << std::endl;
    outfile << "X,Y,Value,Name,Province,Country,Elevation" << std::endl;
    for (it = v_data.begin(); it != v_data.end(); ++it)
    {
        counter++;
        outfile << it->getX() << "," << it->getY() << "," << it->getCode() << "," << it->getName()
                << "," << it->getProvince() << "," << it->getCountry() << "," << it->getElevation() << "\n";
    }
    outfile.close();
    std::cout << "\nFound: " << counter << " NAEFS locations" << std::endl;


    std::cout << "Processing NAEFS Data.." << std::endl;

    /** End Processing NAEFS Locations XML.. **/

    /** Begin config for current run **/

    Date startDate("2016-04-01");
    //Date endDate("2018-10-01");
    Date endDate("2018-10-01");

    std::string data_locn = "C:\\cpp_projects\\data";

    std::ofstream outfilesummary;
    std::string outFileLocnSummary = "C:\\cpp_projects\\data\\summary.csv";
    outfilesummary.open(outFileLocnSummary);
    outfilesummary << "StationName,Lon,Lat,Province,Date,StationID,LeadTimeDays,ActTemp,ActPrecip,ActRH,ActWS,";
    outfilesummary << "AvgTempCan,MedianTempCan,AvgTempUS,MedianTempUS,";
    outfilesummary << "AvgPrecipCan,MedianPrecipCan,AvgPrecipUS,MedianPrecipUS,";
    outfilesummary << "AvgRHCan,MedianRHCan,AvgRHUS,MedianRHUS,";
    outfilesummary << "AvgWSCan,MedianWSCan,AvgWSUS,MedianWSUS,";
    outfilesummary << "MedianTempCanUS, MedianPrecipCanUS, MedianRHCanUS, MedianWSCanUS" << std::endl;

    /** End config for current run **/



    for (Date iterDate = startDate; iterDate != endDate; iterDate = iterDate.AdjustDate(+1))
    {///ingest all the naefs data from startDate to endDate
        std::vector <NAEFSDataStruct> naefsdata;
        if (iterDate.Month() >= 3 && iterDate.Month() < 11)
        {
            std::string dailyFile = data_locn + "\\naefs" + std::to_string(iterDate.Year()) + "\\db_model_export_" + iterDate.DateString() + ".csv";
            std::string dailyFilea = data_locn + "\\naefs" + std::to_string(iterDate.Year()) + "\\db_model_export_" + iterDate.DateString() + "a.csv";

            std::ifstream dailyFileStream;
            std::ifstream dailyFileStreama;
            dailyFileStream.open(dailyFile);
            dailyFileStreama.open(dailyFilea);

            if (dailyFileStream || dailyFileStreama)
            {
                if (dailyFileStream)
                {
                    std::cout << iterDate.DateStringHour() << "Reading file: " << dailyFile << std::endl;
                    //std::cout << "Opening: " << dailyFile << " for reading\n";
                    if(ProcessDailyData(dailyFileStream, naefsdata, iterDate))
                    {
                        std::cout << iterDate.DateStringHour() << "Processed: " << dailyFile << std::endl;
                    }
                }
                else if (dailyFileStreama)
                {
                    std::cout << iterDate.DateStringHour() << "Reading file: " << dailyFilea << std::endl;
                    //std::cout << "Opening: " << dailyFilea << " for reading\n";
                    if(ProcessDailyData(dailyFileStreama, naefsdata, iterDate))
                    {
                        std::cout << iterDate.DateStringHour() << "Processed: " << dailyFilea << std::endl;
                    }
                }
            }
            else
            {
                std::cout << "Could not open: " << dailyFile << " for reading\n";
                continue; ///skip this day
            }
            dailyFileStream.close();
            dailyFileStreama.close();

            if(naefsdata.size() != 174720)
            {
                std::cout << "Incorrect file length. Should be 16*273*40 = 174720";
            }

            std::sort(naefsdata.begin(), naefsdata.end(), compareDateStationModel); ///sort by forecast day, then by station id, and finally model id

            ///A programming options: 1) using multidimensional arrays, 2) multidimensional vectors, 3) single dimension vector with offset indexing, 4) boost::multi_array
            ///After reviewing the following discussion, it is probably easiest to use multidimensional arrays for both clarity and ease of use
            ///https://stackoverflow.com/questions/2679844/is-it-bad-practice-to-use-multi-dimensional-arrays-in-c-c

            typedef boost::multi_array<double, 3> array_type;

            array_type forecast_temperature(boost::extents[16][274][44]); ///forecast day (1-16), station_id (1-274), model id (1-43)
            array_type forecast_precip(boost::extents[16][274][44]);      ///forecast day (1-16), station_id (1-274), model id (1-43)
            array_type forecast_rh(boost::extents[16][274][44]);          ///forecast day (1-16), station_id (1-274), model id (1-43)
            array_type forecast_ws(boost::extents[16][274][44]);          ///forecast day (1-16), station_id (1-274), model id (1-43)

            array_type actual_temperature(boost::extents[16][274][44]);
            array_type actual_precip(boost::extents[16][274][44]);
            array_type actual_rh(boost::extents[16][274][44]);
            array_type actual_ws(boost::extents[16][274][44]);

            for(NAEFSDataStruct i : naefsdata)
            {///iterate through the daily naefs data
                int int_forecast_day = i.forecast_date.DayOfYear() - i.run_date.DayOfYear();

                if(int_forecast_day < 0 || int_forecast_day > 16)
                {
                    std::cout << "Error with calculating int_forecast_day" << i.forecast_date.DayOfYear() << "\t"  << i.run_date.DayOfYear() << "\t" << int_forecast_day << std::endl;
                    std::cout << "Error with calculating int_forecast_day" << i.forecast_date.DateStringHour() << "\t"  << i.run_date.DateStringHour() << "\t" << int_forecast_day << std::endl;
                    char ch;
                    std::cin.get(ch); // force pause of the program
                }
                if (i.stn_id > 274 || i.stn_id <= 0)
                {
                    std::cout << "Error with calculating i.stn_id";
                    char ch;
                    std::cin.get(ch); // force pause of the program
                }

                forecast_temperature[int_forecast_day][i.stn_id][i.model_id] = i.temp;
                forecast_precip[int_forecast_day][i.stn_id][i.model_id] = i.precip;
                forecast_rh[int_forecast_day][i.stn_id][i.model_id] = i.rh;
                forecast_ws[int_forecast_day][i.stn_id][i.model_id] = i.ws;
                //std::cout << int_forecast_day << "\t" << i.stn_id << "\t" << i.model_id << "\t" << forecast_temperature[int_forecast_day][i.stn_id][i.model_id] << std::endl;
            }
            naefsdata.clear();

            std::cout << "Naefs processed" << std::endl;

            double median_temperature_can[16][274]= {0}, median_precip_can[16][274]= {0}, median_rh_can[16][274]= {0}, median_ws_can[16][274]= {0};
            double median_temperature_us[16][274]= {0}, median_precip_us[16][274]= {0}, median_rh_us[16][274]= {0}, median_ws_us[16][274]= {0};

            double average_temperature_can[16][274]= {0}, average_precip_can[16][274]= {0}, average_rh_can[16][274]= {0}, average_ws_can[16][274]= {0};
            double average_temperature_us[16][274]= {0}, average_precip_us[16][274]= {0}, average_rh_us[16][274]= {0}, average_ws_us[16][274]= {0};

            double median_temperature_canus[16][274]= {0}, median_precip_canus[16][274]= {0}, median_rh_canus[16][274]= {0}, median_ws_canus[16][274]= {0};

            double temp_avg_temp_can = 0, temp_avg_precip_can = 0, temp_avg_rh_can = 0, temp_avg_ws_can = 0;
            double temp_avg_temp_us = 0, temp_avg_precip_us = 0, temp_avg_rh_us = 0, temp_avg_ws_us = 0;

            double temp_50_perc_temp_can[20] = {0}, precip_50_perc_temp_can[20] = {0}, ws_50_perc_temp_can[20] = {0}, rh_50_perc_temp_can[20]= {0};
            double temp_50_perc_temp_us[20] = {0}, precip_50_perc_temp_us[20] = {0}, ws_50_perc_temp_us[20] = {0}, rh_50_perc_temp_us[20]= {0};
            double temp_50_perc_temp_canus[40] = {0}, precip_50_perc_temp_canus[40] = {0}, ws_50_perc_temp_canus[40] = {0}, rh_50_perc_temp_canus[40]= {0};

            for (int i = 0; i < 16; ++i)
            {///let i be the forecasted day, 0(today) to 15
                for (int j = 1; j < 274; ++j)
                {///let j be the station id; 1-273
                    for (int k = 2; k < 22; ++k)
                    {///let k be the model id, 2-21 for CAN models, 24-43 for US models, 22 and 23 are empty
                        //std::cout << i << "\t" << j << "\t" << k << "\t" << forecast_temperature[i][j][k] << std::endl;
                        temp_avg_temp_can += forecast_temperature[i][j][k];
                        temp_avg_precip_can += forecast_precip[i][j][k];
                        temp_avg_rh_can += forecast_rh[i][j][k];
                        temp_avg_ws_can += forecast_ws[i][j][k];

                        temp_50_perc_temp_can[k-2] = forecast_temperature[i][j][k];
                        precip_50_perc_temp_can[k-2] = forecast_precip[i][j][k];
                        ws_50_perc_temp_can[k-2] = forecast_rh[i][j][k];
                        rh_50_perc_temp_can[k-2] = forecast_ws[i][j][k];

                        temp_50_perc_temp_canus[k-2] = forecast_temperature[i][j][k];
                        precip_50_perc_temp_canus[k-2] = forecast_precip[i][j][k];
                        ws_50_perc_temp_canus[k-2] = forecast_rh[i][j][k];
                        rh_50_perc_temp_canus[k-2] = forecast_ws[i][j][k];
                    }

                    ///std::cout << temp_avg_temp_can << "\t" << temp_avg_temp_can/20 << std::endl;
                    average_temperature_can[i][j] = temp_avg_temp_can / 20;
                    average_precip_can[i][j] = temp_avg_precip_can / 20;
                    average_rh_can[i][j] = temp_avg_rh_can / 20;
                    average_ws_can[i][j] = temp_avg_ws_can / 20;

                    median_temperature_can[i][j] = calcPerc20(temp_50_perc_temp_can, 0.5);
                    median_precip_can[i][j] = calcPerc20(precip_50_perc_temp_can, 0.5);
                    median_rh_can[i][j] = calcPerc20(ws_50_perc_temp_can, 0.5);
                    median_ws_can[i][j] = calcPerc20(rh_50_perc_temp_can, 0.5);

                    temp_avg_temp_can = 0; temp_avg_precip_can = 0; temp_avg_rh_can = 0; temp_avg_ws_can = 0;

                    for (int k = 24; k < 44; ++k) ///24-43
                    {
                        //std::cout << i << "\t" << j << "\t" << k << "\t" << forecast_temperature[i][j][k] << std::endl;
                        temp_50_perc_temp_us[k-24] = forecast_temperature[i][j][k];
                        precip_50_perc_temp_us[k-24] = forecast_precip[i][j][k];
                        ws_50_perc_temp_us[k-24] = forecast_rh[i][j][k];
                        rh_50_perc_temp_us[k-24] = forecast_ws[i][j][k];

                        temp_avg_temp_us += forecast_temperature[i][j][k];
                        temp_avg_precip_us += forecast_precip[i][j][k];
                        temp_avg_rh_us += forecast_rh[i][j][k];
                        temp_avg_ws_us += forecast_ws[i][j][k];

                        temp_50_perc_temp_canus[k-4] = forecast_temperature[i][j][k];
                        precip_50_perc_temp_canus[k-4] = forecast_precip[i][j][k];
                        ws_50_perc_temp_canus[k-4] = forecast_rh[i][j][k];
                        rh_50_perc_temp_canus[k-4] = forecast_ws[i][j][k];
                    }
                    average_temperature_us[i][j] = temp_avg_temp_us / 20;
                    average_precip_us[i][j] = temp_avg_precip_us / 20;
                    average_rh_us[i][j] = temp_avg_rh_us / 20;
                    average_ws_us[i][j] = temp_avg_ws_us / 20;
                    median_temperature_us[i][j] = calcPerc20(temp_50_perc_temp_us, 0.5);
                    median_precip_us[i][j] = calcPerc20(precip_50_perc_temp_us, 0.5);
                    median_rh_us[i][j] = calcPerc20(ws_50_perc_temp_us, 0.5);
                    median_ws_us[i][j] = calcPerc20(rh_50_perc_temp_us, 0.5);

                    median_temperature_canus[i][j] = calcPerc40(temp_50_perc_temp_canus, 0.5);
                    median_precip_canus[i][j] = calcPerc40(precip_50_perc_temp_canus, 0.5);
                    median_rh_canus[i][j] = calcPerc40(ws_50_perc_temp_canus, 0.5);
                    median_ws_canus[i][j] = calcPerc40(rh_50_perc_temp_canus, 0.5);

                    temp_avg_temp_us = 0; temp_avg_precip_us = 0; temp_avg_rh_us = 0; temp_avg_ws_us = 0; ///reset average to zero
                }
                //char ch;
                //std::cin.get(ch);
            }

            for (int i = 0; i <= 15; i++)
            {

                for (int j = 1; j <= 273; j++)
                {
                    std::string name = findCodeValue(j+10000, v_data, "name");
                    double lon = stod(findCodeValue(j+10000, v_data, "x"));
                    double lat = stod(findCodeValue(j+10000, v_data, "y"));
                    std::string province = findCodeValue(j+10000, v_data, "province");

                    GridDataDriver *griddata_temp = new GridDataDriver("C:\\cpp_projects\\data\\2018","temp",iterDate.AdjustDate(i).DateString());
                    GridDataDriver *griddata_precip = new GridDataDriver("C:\\cpp_projects\\data\\2018","precip",iterDate.AdjustDate(i).DateString());
                    GridDataDriver *griddata_rh = new GridDataDriver("C:\\cpp_projects\\data\\2018","rh",iterDate.AdjustDate(i).DateString());
                    GridDataDriver *griddata_ws = new GridDataDriver("C:\\cpp_projects\\data\\2018","ws",iterDate.AdjustDate(i).DateString());

                    double actual_temp=griddata_temp->getGridValue(lat,lon); ///std::cout << lat << "\t" << lon << "\t" << actual_temp << std::endl;
                    double actual_precip=griddata_precip->getGridValue(lat,lon);
                    double actual_rh=griddata_rh->getGridValue(lat,lon);
                    double actual_ws=griddata_ws->getGridValue(lat,lon);

                    griddata_temp->Close(); griddata_precip->Close(); griddata_rh->Close(); griddata_ws->Close();
                    delete griddata_temp; delete griddata_precip; delete griddata_rh; delete griddata_ws;

                    outfilesummary << name << "," << lon << "," << lat << "," << province << "," << iterDate.DateString() << "," << j << "," << i << ","
                                << actual_temp << "," << actual_precip << "," << actual_rh << "," << actual_ws << ","
                                << average_temperature_can[i][j] << "," << median_temperature_can[i][j] << "," << average_temperature_us[i][j] << "," << median_temperature_us[i][j] << ","
                                << average_precip_can[i][j] << "," << median_precip_can[i][j] << "," << average_precip_us[i][j] << "," << median_precip_us[i][j] << ","
                                << average_rh_can[i][j] << "," << median_rh_can[i][j] << "," << average_rh_us[i][j] << "," << median_rh_us[i][j] << ","
                                << average_ws_can[i][j] << "," << median_ws_can[i][j] << "," << average_ws_us[i][j] << "," << median_ws_us[i][j] << ","
                                << median_temperature_canus[i][j] << "," << median_precip_canus[i][j] << "," << median_rh_canus[i][j] << "," << median_ws_canus[i][j]
                                << std::endl;
                }
            }
        }
    }
    outfilesummary.close();
    return 0;
}

std::string findCodeValue(int code, std::vector<PointData> v_data, std::string key)
{
    std::string value = "";
    std::vector<PointData>::iterator it;
    it = std::find(v_data.begin(), v_data.end(), PointData(0,0,code,"","","",""));
    if(it != v_data.end())
    {
        if (key == "x")
            value = std::to_string(it->getX());
        else if (key == "y")
            value = std::to_string(it->getY());
        else if (key == "name")
            value = it->getName();
        else if (key == "province")
            value = it->getProvince();
    }
    else
        std::cout<<"Item with code: " << code << "not found"<<std::endl;
    return value;
}


bool ProcessDailyData (std::ifstream& inStream, std::vector <NAEFSDataStruct> &naefsdata, Date date)
{
    std::string line;
    NAEFSDataStruct data;

    //std::string t[12];

    data.run_date = date;

    std::getline(inStream, line); ///throw away headers

    while (std::getline(inStream, line) && inStream.good())
    {
        //std::cout << line;
        std::istringstream iss(line);

        std::vector <std::string> record;

        while (iss)
        {
            std::string s;
            if (!getline( iss, s, ',' ))
                break;
            record.push_back( s );
        }
        if (record.size() == 14)
        {
            record.push_back("");
        }

        data.model_id = std::stoi(record[0].substr(5,2));
        data.stn_id   = int(stoi(record[1])-10000);
        //std::cout << Date(record[2].substr(1,10)).DateString() << std::endl;
        data.forecast_date = Date(record[2].substr(1,10));
        data.temp     = atof(record[3].c_str());
        data.rh       = atof(record[4].c_str());
        data.precip   = atof(record[5].c_str());
        data.ws       = atof(record[6].c_str());
        data.wd       = atof(record[7].c_str());

        if(record[8] != "")
            data.ffmc = atof(record.at(8).c_str());
        else
            data.ffmc = 0;

        if(record[9] != "")
            data.dmc  = atof(record.at(9).c_str());
        else
            data.dmc = 0;

        if(record[10] != "")
            data.dc   = atof(record.at(10).c_str());
        else
            data.dc = 0;

        if(record[11] != "")
            data.bui  = atof(record.at(11).c_str());
        else
            data.bui = 0;

        if(record[12] != "")
            data.isi  = atof(record.at(12).c_str());
        else
            data.isi = 0;

        if(record[13] != "")
            data.fwi  = atof(record.at(13).c_str());
        else
            data.fwi = 0;

        if(record[14] != "")
            data.dsr  = atof(record.at(14).c_str());
        else
            data.dsr = 0;


        //char ch; std::cin.get(ch);

        //if (!(iss >> data.model_id >> data.stn_id >> data.forecast_date
        //          >> t[0] >> t[1] >> t[2] >> t[3] >> t[4] >> t[5]
        //          >> t[6] >> t[7] >> t[8] >> t[9] >> t[10] >> t[11]))
        //{
         //    std::cout << "error";
        //     break; /// error
        //};

        //std::cout << data.run_date.DateString() << "\t" << data.model_id << "\t" << data.stn_id << "\t" << data.forecast_date.DateString();
        //std::cout << "\t" << data.temp << "\t" << data.rh << "\t" << data.precip << "\t" << data.ws << "\t" << data.wd;
        //std::cout << "\t" << data.ffmc << "\t" << data.dmc << "\t" << data.dc << "\t" << data.bui << "\t" << data.isi;
        //std::cout << "\t" << data.fwi << "\t" << data.dsr;

    naefsdata.push_back(data);
    }
    inStream.close();

    //naefsdata.push_back(data);
    return true;
}

bool compareDateStationModel(NAEFSDataStruct &a, NAEFSDataStruct &b)
{
    if (a.forecast_date != b.forecast_date)
        return a.forecast_date.DateString() < b.forecast_date.DateString();
    if (a.stn_id != b.stn_id)
        return a.stn_id < b.stn_id;
    if (a.model_id != b.model_id)
        return a.model_id < b.model_id;

    return -10;///error
}

double calcPerc20(double data[20], double percentile)
{
    ///note: using the nearest rank method
    std::sort(data, data+20);
    ///for (size_t i = 0; i != 20; ++i)
        ///std::cout << data[i] << " ";

    ///0.9*20=18 which is the second last

    if(percentile == 0.5)
    {
        ///std::cout << std::endl << "median:" << (data[9] + data[10]) / 2.0 << std::endl;
        return (data[9] + data[10]) / 2.0;
    }
    else
    {
        ///std::cout << std::endl << "percentile(" << percentile << "): " << data[int(ceil(percentile*20))] << std::endl;
        return data[int(ceil(percentile*20))];
    }
}

double calcPerc40(double data[40], double percentile)
{
    ///note: using the nearest rank method
    std::sort(data, data+40);
    ///for (size_t i = 0; i != 40; ++i)
    ///    std::cout << data[i] << " ";

    ///0.9*40=36

    if(percentile == 0.5)
    {
        ///std::cout << std::endl << "median:" << (data[19] + data[20]) / 2.0 << std::endl;
        return (data[19] + data[20]) / 2.0;
    }
    else
    {
        ///std::cout << std::endl << "percentile(" << percentile << "): " << data[int(ceil(percentile*40))] << std::endl;
        return data[int(ceil(percentile*40))];
    }
}
