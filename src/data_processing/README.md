Code related to processing the 2014-2018 NAEFS data.

Modules:

1) Processing MSC locations.xml file to extract the Canadian stations locations (273). Note that the XML contains U.S. and Mexico points. We filtered the list by testing if the point is in "CA".

2) Processing the station NAEFS csv files in the \data folder. Between 2014 and 2015, filenames are in the format and location: \data\naefsYYYY\db_model_export_YYYYMMDD.csv. In 2016 to 2018, the filenames are formatted: db_model_export_YYYYMMDDa.csv

Each csv file contains 174720 lines: 273 stations * 16 lead days * 40 ensemble members.

Two weather stations were dropped from the analysis: an island far in Lake Superior and an island far off the east coast of Canada (St. Pierre and Miquelon).

3) Extract and calulate mean and median for temp, precip, rh, and ws: U.S. ensemble members, Canadian ensemble members, and Canadian+U.S. ensemble members.

4) Extract observed temp, precip, rh, and ws from CWFIS grids.

5) Calculate the FWI using CWFIS grid-extracted moisture codes from Feb. 28. Note: the same FWI.cpp code is used in the CWFIS Spatial Fire Management System (SFMS) program.


