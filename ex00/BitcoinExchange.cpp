#include "BitcoinExchange.hpp"

#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>

BitcoinExchange::BitcoinExchange() {}

BitcoinExchange::BitcoinExchange(const BitcoinExchange &other)
    : _rates(other._rates) {}

BitcoinExchange &BitcoinExchange::operator=(const BitcoinExchange &other) 
{
    if (this != &other)
        _rates = other._rates;
    return *this;
}

BitcoinExchange::~BitcoinExchange() {}

std::string BitcoinExchange::trim(const std::string &text) const 
{
    const std::string spaces = " \t\r\n";
    std::string::size_type first = text.find_first_not_of(spaces);
    if (first == std::string::npos)
        return "";
    std::string::size_type last = text.find_last_not_of(spaces);
    return text.substr(first, last - first + 1);
}

bool BitcoinExchange::checkDate(const std::string &date) const 
{
    if (date.size() != 10 || date[4] != '-' || date[7] != '-')
        return false;
    for (std::size_t i = 0; i < date.size(); ++i) {
        if (i == 4 || i == 7)
            continue;
        if (!std::isdigit(static_cast<unsigned char>(date[i])))
            return false;
    }

    int year = 0;
    int month = 0;
    int day = 0;
    std::istringstream(date.substr(0, 4)) >> year;
    std::istringstream(date.substr(5, 2)) >> month;
    std::istringstream(date.substr(8, 2)) >> day;
    if (year == 0 || month < 1 || month > 12)
        return false;

    const int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int maxDay = days[month - 1];
    if (month == 2 && (year % 400 == 0 || (year % 4 == 0 && year % 100 != 0)))
        maxDay = 29;
    return day >= 1 && day <= maxDay;
}

bool BitcoinExchange::readNumber(const std::string &text, double &number) const 
{
    std::string value = trim(text);
    if (value.empty())
        return false;
    std::istringstream input(value);
    input >> number;
    if (input.fail())
        return false;
    input >> std::ws;
    if (!input.eof())
        return false;
    // Reject NaN and infinities, while staying compatible with C++98.
    if (number != number || number > 1.0e308 || number < -1.0e308)
        return false;
    return true;
}

bool BitcoinExchange::loadData(const std::string &file) 
{
    std::ifstream input(file.c_str());
    if (!input.is_open()) {
        std::cerr << "Error: could not open database." << std::endl;
        return false;
    }
    _rates.clear();
    std::string line;
    while (std::getline(input, line)) {
        if (trim(line) == "date,exchange_rate")
            continue;
        std::string::size_type comma = line.find(',');
        if (comma == std::string::npos || line.find(',', comma + 1) != std::string::npos)
            continue;
        std::string date = trim(line.substr(0, comma));
        double rate = 0;
        if (checkDate(date) && readNumber(line.substr(comma + 1), rate) && rate >= 0)
            _rates[date] = rate;
    }
    if (_rates.empty()) {
        std::cerr << "Error: empty or invalid database." << std::endl;
        return false;
    }
    return true;
}

void BitcoinExchange::readLine(const std::string &line) const 
{
    std::string::size_type pipe = line.find('|');
    if (pipe == std::string::npos || line.find('|', pipe + 1) != std::string::npos) {
        std::cout << "Error: bad input => " << line << std::endl;
        return;
    }
    const std::string date = trim(line.substr(0, pipe));
    const std::string text = trim(line.substr(pipe + 1));
    double value = 0;
    if (!checkDate(date) || !readNumber(text, value)) {
        std::cout << "Error: bad input => " << line << std::endl;
        return;
    }
    if (value < 0) {
        std::cout << "Error: not a positive number." << std::endl;
        return;
    }
    if (value > 1000) {
        std::cout << "Error: too large a number." << std::endl;
        return;
    }

    std::map<std::string, double>::const_iterator it = _rates.upper_bound(date);
    if (it == _rates.begin()) {
        std::cout << "Error: no earlier date => " << date << std::endl;
        return;
    }
    --it;
    std::cout << date << " => " << text << " = " << value * it->second << std::endl;
}

bool BitcoinExchange::runFile(const std::string &file) const 
{
    std::ifstream input(file.c_str());
    if (!input.is_open()) {
        std::cerr << "Error: could not open file." << std::endl;
        return false;
    }
    std::string line;
    while (std::getline(input, line)) {
        if (trim(line) == "date | value")
            continue;
        readLine(line);
    }
    return true;
}
