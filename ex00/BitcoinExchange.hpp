#ifndef BITCOINEXCHANGE_HPP
#define BITCOINEXCHANGE_HPP

#include <map>
#include <string>

class BitcoinExchange 
{
    private:
        std::map<std::string, double> _rates;

        bool checkDate(const std::string &date) const;
        bool readNumber(const std::string &text, double &number) const;
        std::string trim(const std::string &text) const;
        void readLine(const std::string &line) const;

    public:
        BitcoinExchange();
        BitcoinExchange(const BitcoinExchange &other);
        BitcoinExchange &operator=(const BitcoinExchange &other);
        ~BitcoinExchange();

        bool loadData(const std::string &file);
        bool runFile(const std::string &file) const;
};

#endif
