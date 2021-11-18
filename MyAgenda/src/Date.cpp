#include <gtest/gtest.h>
#include "Date.hpp"
using namespace std;

/**
  * @brief default constructor
  */
Date::Date()
{
    m_year = 0;
    m_month = 0;
    m_day = 0;
    m_hour = 0;
    m_minute = 0;
}

/**
  * @brief constructor with arguments
  */
Date::Date(int t_year, int t_month, int t_day, int t_hour, int t_minute)
{
    this->m_year = t_year;
    this->m_month = t_month;
    this->m_day = t_day;
    this->m_hour = t_hour;
    this->m_minute = t_minute;
}

/**
  * @brief constructor with a string
  */
Date::Date(const std::string &dateString)
{
    Date t_date = Date::stringToDate(dateString);
    if (Date::isValid(t_date))
    {
        *this = t_date;
    }
    else
    {
        this->m_year = 0;
        this->m_month = 0;
        this->m_day = 0;
        this->m_hour = 0;
        this->m_minute = 0;
    }
}
/**
  * @brief return the year of a Date
  * @return   a integer indicate the year of a date
  */
int Date::getYear(void) const
{
    return m_year;
}

/**
  * @brief set the year of a date
  * @param a integer indicate the new year of a date
  */
void Date::setYear(const int t_year)
{
    this->m_year = t_year;
}

/**
  * @brief return the month of a Date
  * @return   a integer indicate the month of a date
  */
int Date::getMonth(void) const
{
    return this->m_month;
}

/**
  * @brief set the month of a date
  * @param a integer indicate the new month of a date
  */
void Date::setMonth(const int t_month)
{
    this->m_month = t_month;
}

/**
  * @brief return the day of a Date
  * @return   a integer indicate the day of a date
  */
int Date::getDay(void) const
{
    return this->m_day;
}

/**
  * @brief set the day of a date
  * @param a integer indicate the new day of a date
  */
void Date::setDay(const int t_day)
{
    this->m_day = t_day;
}

/**
  * @brief return the hour of a Date
  * @return   a integer indicate the hour of a date
  */
int Date::getHour(void) const
{
    return this->m_hour;
}

/**
  * @brief set the hour of a date
  * @param a integer indicate the new hour of a date
  */
void Date::setHour(const int t_hour)
{
    this->m_hour = t_hour;
}

/**
  * @brief return the minute of a Date
  * @return   a integer indicate the minute of a date
  */
int Date::getMinute(void) const
{
    return this->m_minute;
}

/**
  * @brief set the minute of a date
  * @param a integer indicate the new minute of a date
  */
void Date::setMinute(const int t_minute)
{
    this->m_minute = t_minute;
}

/**
  *   @brief check whether the date is valid or not
  *   @return the bool indicate valid or not
  */
bool Date::isValid(const Date &t_date)
{
    if (t_date.getYear() < 1000 || t_date.getYear() > 9999)
    {
        return false;
    }
    if (t_date.getMonth() < 1 || t_date.getMonth() > 12)
    {
        return false;
    }
    if (t_date.getDay() < 1 || t_date.getDay() > 31)
    {
        return false;
    }
    if (t_date.getHour() < 0 || t_date.getHour() > 23)
    {
        return false;
    }
    if (t_date.getMinute() < 0 || t_date.getMinute() > 59)
    {
        return false;
    }
    if (t_date.getMonth() == 2)
    {
        if ((t_date.getYear() % 4 == 0 && t_date.getYear()% 100 != 0) || (t_date.getYear() % 400 == 0))
        {
            if (t_date.getDay() > 29)
                return false;
        }
        else if (t_date.getDay() > 28)
        {
            return false;
        }
    }
    if (t_date.getMonth() == 4 || t_date.getMonth() == 6 || t_date.getMonth() == 9 || t_date.getMonth() == 11)
    {
        if (t_date.getDay() > 30)
        {
            return false;
        }
    }
    return true;
}

/**
  * @brief convert a string to date, if the format is not correct return
  * 0000-00-00/00:00
  * @return a date
  */
Date Date::stringToDate(const string &t_dateString)
{
    int v=0;
    if(t_dateString.size()!=16){
        v=1;
    }
    for (int i = 0; i < t_dateString.size(); ++i)
    {
        if(i==4||i==7||i==10||i==13){
            continue;
        }
        else if(t_dateString[i]<'0'||t_dateString[i]>'9'){
            v=1;
            break;
        }
    }
    if ((t_dateString[4] != '-') || (t_dateString[7]!= '-') || (t_dateString[10] != '/') || (t_dateString[13] != ':'))
    {
        v=1;
    }
    if(v==1){
        return Date();
    }
    stringstream ss;
    int year,month,day,hour,minute;
    char tmp;
    ss<<t_dateString;
    ss>>year>>tmp>>month>>tmp>>day>>tmp>>hour>>tmp>>minute;
    return Date(year,month,day,hour,minute);
}

/**
  * @brief convert a date to string, if the date is invalid return
  * 0000-00-00/00:00
  */
std::string Date::dateToString(const Date &t_date)
{
    if (isValid(t_date))
    {
        char tmp[17];
        snprintf(tmp, 17, "%04d-%02d-%02d/%02d:%02d", t_date.getYear(), t_date.getMonth(), t_date.getDay(), t_date.getHour(), t_date.getMinute());
        return string(tmp);
    }
    else
    {
        string s("0000-00-00/00:00");
        return s;
    }
}

/**
  *  @brief overload the assign operator
  */
Date &Date::operator=(const Date &t_date)
{
    this->m_year = t_date.getYear();
    this->m_month = t_date.getMonth();
    this->m_day = t_date.getDay();
    this->m_hour = t_date.getHour();
    this->m_minute = t_date.getMinute();
}

/**
  * @brief check whether the CurrentDate is equal to the t_date
  */
bool Date::operator==(const Date &t_date) const
{
    if (this->m_year == t_date.getYear())
    {
        if (this->m_month == t_date.getMonth())
        {
            if (this->m_day == t_date.getDay())
            {
                if (this->m_hour == t_date.getHour())
                {
                    if (this->m_minute == t_date.getMinute())
                    {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

/**
  * @brief check whether the CurrentDate is  greater than the t_date
  */
bool Date::operator>(const Date &t_date) const
{
    if (this->m_year > t_date.getYear()){return true;}
    else if(this->m_year == t_date.getYear())
    {
        if (this->m_month > t_date.getMonth()){return true;}
        else if (this->m_month == t_date.getMonth())
        {
            if (this->m_day > t_date.getDay()){return true;}
            else if (this->m_day == t_date.getDay())
            {
                if (this->m_hour > t_date.getHour()){return true;}
                else if (this->m_hour == t_date.getHour())
                {
                    if (this->m_minute > t_date.getMinute()){return true;}
                }
            }
        }
    }
    return false;
}

/**
  * @brief check whether the CurrentDate is  less than the t_date
  */
bool Date::operator<(const Date &t_date) const
{
    return !(*this>t_date||*this==t_date);
}

/**
  * @brief check whether the CurrentDate is  greater or equal than the t_date
  */
bool Date::operator>=(const Date &t_date) const
{

    return !(*this < t_date);
}

/**
  * @brief check whether the CurrentDate is  less than or equal to the t_date
  */
bool Date::operator<=(const Date &t_date) const
{
    return !(*this > t_date);
}