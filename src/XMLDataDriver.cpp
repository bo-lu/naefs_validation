/**
 * @file    XMLDataDriver.cpp
 * @author  Bo Lu
 * @date    June 2017
 * @brief   Generic XML Data Driver. Used to parse an XML file format
 */


#include <boost/foreach.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/filesystem.hpp>

#include <iostream>
#include "XMLDataDriver.h"
#include "PointData.h"

using namespace std;
using namespace boost::filesystem;
using namespace boost;


XMLDataDriver::XMLDataDriver()
{
}

XMLDataDriver::~XMLDataDriver()
{
}

bool XMLDataDriver::Open(string xml_filename)
{
    /// Open the XML file
    std::ifstream input(xml_filename.c_str());

    if (input)
    {
        std::cout << "Reading: " << xml_filename << std::endl;
        boost::property_tree::read_xml(input, m_pt); ///m_pt is defined in the .h of type ptree
        return true;
    }
    else
    {
        std::cout << "Could not open: " << xml_filename << " for reading\n";
        return false;
    }
    //input.close();

}

std::vector<PointData>& XMLDataDriver::getData(string code, string name, string province, string country, string elevation)
{
    int count = 0;

    BOOST_FOREACH(ptree::value_type const &v, m_pt.get_child(m_parent_name))
    {
        if( v.first == m_child_name)
        {
            double lon = v.second.get<double>(m_lon); ///m_Xstring and m_Ystring are from the config file
            double lat = v.second.get<double>(m_lat);
            string m_code = v.second.get("<xmlattr>."+code, "");
            string m_name = v.second.get("<xmlattr>."+name, "");
            string m_province = v.second.get("<xmlattr>."+province, "");
            string m_country = v.second.get("<xmlattr>."+country, "");
            string m_elevation = v.second.get<std::string>(elevation);

            char* endptr = 0;
            strtod(m_code.c_str(), &endptr);

            if(*endptr != '\0' || endptr == m_code.c_str())
            {
                //std::cout << lon << "\t" << lat << "\t" << m_code << "\n";
            }
            else
            {
                int code_int = atoi(m_code.c_str());
                if (m_country == "CA")
                {
                    std::cout << lon << "\t" << lat << "\t" << code_int << "\t" << m_name
                              << "\t" << m_province << "\t" << m_country << "\t" << m_elevation << "\n";
                    m_data.push_back(PointData(lon, lat, code_int, m_name, m_province, m_country, m_elevation));
                    count++;
                }
            }
        }
    }

    return m_data;
}
