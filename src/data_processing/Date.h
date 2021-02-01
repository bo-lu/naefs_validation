/**
 * @file	Date.h
 * @author  R.M. Suddaby, Bo Lu
 * @date    September 2012
 * @brief   Date: A utility class for manipulating dates.
 */

#ifndef Date_H_
#define Date_H_

#include <string>
#include <ctime>

//! Date class
class Date {
public:
	//! Default constructor
	Date();
	//! Construct a Date from year, month, day and hour integers
	Date(int year, int month, int day, int hour);

	//! Construct a Date from a string with the format 'YYYYMMDD'
	Date(std::string date);
	virtual ~Date();

	//! Set the hour of this date object
	void SetHour(int hour);
	//! Adjust the date. A positive integer will generate a new future date, a negative for a date in the past
	Date AdjustDate(int adjust);
	//! Adjust the date by an hour value.
	Date AdjustDateByHour(int adjust);
	//! Get a default string representation of the date (YYYYMMDD)
	std::string DateString();
	//! Get a default string representation of the date with an hour (YYYYMMDDHH)
	std::string DateStringHour();
	//! Get a default string representation of the date with dashes (YYYY-MM-DD)
	std::string DateStringDash();
	//! Get a default string representation of the date with an hour with dashes and 'T' (YYYY-MM-DDTHH)
	std::string DateStringHourDash();

	//! Get a string representation of the date using the supplied format.
	std::string FormatDateString(std::string format);

	//! Get the year of the date
	int Year();
	//! Get the month of the date
	int Month();
	//! Get the day of the date
	int Day();
	//! Get the hour of the date
	int Hour();

	//! Get the daylight savings flag
	bool IsDST();

	//! Get the midnight value (either 0 or 24)
	int Midnight() { return m_midnight; }

	//! Set the midnight value
	void Midnight(int value) { m_midnight = value; }

	//! Get the day of year
	int DayOfYear();

	//! Check if this Date equals another
	bool Equals(Date date);
	//! Overloaded == operator
	bool operator==(const Date &other) const;
	//! Overloaded != operator
	bool operator!=(const Date &other) const;

private:

	void RecalcDayOfYear();
	int m_year;
	int m_month;
	int m_day;
	int m_hour;
	int m_day_of_year;
	bool m_isdst;
	int m_midnight;
};


#endif /* Date_H_ */
