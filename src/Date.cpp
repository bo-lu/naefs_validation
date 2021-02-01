/**
 * @file	Date.h
 * @author  R.M. Suddaby, Bo Lu
 * @date    September 2012
 * @brief   Date: A utility class for manipulating dates.
 */

#include <sstream>
#include <string>
#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>
#include "Date.h"

using namespace std;

Date::Date() {
	// TODO Auto-generated constructor stub
	time_t now = time(0);
	struct tm *tm = localtime(&now);
	m_year = tm->tm_year + 1900;
	m_month = tm->tm_mon + 1;
	m_day = tm->tm_mday;
	m_hour = tm->tm_hour;
	m_day_of_year = tm->tm_yday + 1;

	m_isdst = false;

	if (tm->tm_isdst > 0) {
		m_isdst = true;
	}

	m_midnight = 0;
}

Date::Date(int year, int month, int day, int hour = 12)
{
	m_year = year;
	m_month = month;
	m_day = day;
	m_hour = hour;
	RecalcDayOfYear();

	m_isdst = IsDST();
}

Date::Date(string date)
{
    string yr;
    string mon;
    string d;
    bool isDashed;
    if (date.find("-") != std::string::npos)
        isDashed = true;
    else
        isDashed = false;

    if (isDashed == true)
    {
        yr = date.substr(0, 4);
        mon = date.substr(5, 2);
        d = date.substr(8, 2);
    }
    else
    {
        yr = date.substr(0, 4);
        mon = date.substr(4, 2);
        d = date.substr(6, 2);
    }

	m_year = atoi(yr.c_str());
	m_month = atoi(mon.c_str());
	m_day = atoi(d.c_str());
	m_hour = 12;
	RecalcDayOfYear();
	m_isdst = IsDST();
}

Date::~Date() {
	// TODO Auto-generated destructor stub
}

bool Date::Equals(Date other)
{
	return    (this->m_year == other.m_year)
			&& (this->m_month == other.m_month)
			&& (this->m_day == other.m_day);
}

bool Date::operator == (const Date &other) const
{
	return    (this->m_year == other.m_year)
	       && (this->m_month == other.m_month)
	       && (this->m_day == other.m_day);
}

bool Date::operator != (const Date &other) const
{
	return !(*this == other);
}

bool Date::IsDST() {
	return m_isdst;
}

int Date::Year()
{
	return m_year;
}

int Date::Month()
{
	return m_month;
}

int Date::Day()
{
	return m_day;
}

int Date::Hour()
{
	return m_hour;
}

int Date::DayOfYear() { return m_day_of_year; }

void Date::SetHour(int hour)
{
	m_hour = hour;
}

Date Date::AdjustDate(int adjust)
{
	time_t t;
	struct tm *tm;

	t = time(NULL);
	tm = localtime(&t);

	tm->tm_year = m_year - 1900;
	tm->tm_mon = m_month - 1;
	tm->tm_mday = m_day + adjust;
	tm->tm_hour = 12;
	t = mktime(tm);
	tm = localtime(&t);

	Date newDate(tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour);
	return newDate;
}

Date Date::AdjustDateByHour(int adjust)
{
	time_t t;
	struct tm *tm;

	t = time(NULL);
	tm = localtime(&t);

	tm->tm_year = m_year - 1900;
	tm->tm_mon = m_month - 1;
	tm->tm_mday = m_day;
	tm->tm_hour = m_hour + adjust;
	t = mktime(tm);
	tm = localtime(&t);
	Date newDate(tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour);
	return newDate;
}

void Date::RecalcDayOfYear() {
	time_t t;
	struct tm *tm;

	t = time(NULL);
	tm = localtime(&t);

	tm->tm_year = m_year - 1900;
	tm->tm_mon = m_month - 1;
	tm->tm_mday = m_day;
	tm->tm_hour = m_hour;
	t = mktime(tm);
	tm = localtime(&t);
	m_day_of_year = tm->tm_yday + 1;
	m_isdst = tm->tm_isdst > 0 ? true:false;
}

string Date::FormatDateString(string format)
{
	time_t t;
	struct tm *tm;

	t = time(NULL);
	tm = localtime(&t);

	tm->tm_year = m_year - 1900;
	tm->tm_mon = m_month - 1;
	tm->tm_mday = m_day;
	tm->tm_hour = m_hour;
	t = mktime(tm);
	tm = localtime(&t);


	if (m_hour == 0) {
		if (Midnight() == 24) {
			string::size_type pos = 0;
			while ((pos = format.find("%H", pos)) != string::npos) {
				format.replace(pos, format.size(), "24");
				pos++;
			}
			cout << "Format string is now " << format << endl;
		}
	}

	char datestring[200];
	strftime(datestring, sizeof(datestring) - 1, format.c_str(), tm);

	string date(datestring);
	return date;
}

string Date::DateString()
{
	stringstream ss;
	ss << setfill('0');
	ss << m_year << setw(2) << m_month << setw(2) << m_day;
	return ss.str();
}

string Date::DateStringDash()
{
	stringstream ss;
	ss << setfill('0');
	ss << m_year << "-" << setw(2) << m_month << "-" << setw(2) << m_day;
	return ss.str();
}

string Date::DateStringHour()
{
	// B.C. Midnight hack!!
	// Check to see if run time is 00 (midnight). If so make sure we use the right value
	// for midnight. For some bizarre reason, midnight is '24' in B.C.
//	int hour = m_hour;  // Just to make sure it's set to some value.
//	if (m_hour == 0) {
//		hour = Midnight();  // This will be set to 24 for B.C., 0 otherwise.
//	}

	stringstream ss;
	ss << setfill('0');
	ss << m_year << setw(2) << m_month << setw(2) << m_day << setw(2) << m_hour;
	return ss.str();
}

string Date::DateStringHourDash()
{
	stringstream ss;
	ss << setfill('0');
	ss << m_year << "-" << setw(2) << m_month << "-" << setw(2) << m_day << "T" << setw(2) << m_hour;
	return ss.str();
}
