/**
 * @file    XMLDataDriver.cpp
 * @author  Bo Lu
 * @date    June 2017
 * @brief   Generic XML Data Driver. Used to parse an XML file format
 */

#include <vector>
#include <string>
#include <sstream>
#include <iostream>

#ifdef _WIN32
#include <Windows.h>
#endif

#include "PointData.h"

#ifndef XMLDATADRIVER_H_
#define XMLDATADRIVER_H_

#include <boost/property_tree/ptree.hpp>

using namespace boost;
using property_tree::ptree;

class XMLDataDriver {

private:
    std::vector<PointData> m_data;
    std::string m_parent_name;
    std::string m_child_name;
    std::string m_lat;
    std::string m_lon;
    ptree m_pt;

public:
    XMLDataDriver();
    ~XMLDataDriver();
    bool Open(std::string);
    void setParentName(std::string pname) { m_parent_name = pname; }
    void setChildName(std::string cname) { m_child_name = cname; }
    void setLatString(std::string lat) { m_lat = lat; }
    void setLonString(std::string lon) { m_lon = lon; }
    std::vector<PointData>& getData(std::string, std::string, std::string, std::string, std::string);
};

#endif /* DATADRIVER_H_ */
