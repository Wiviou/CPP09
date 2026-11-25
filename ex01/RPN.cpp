#include "RPN.hpp"
#include <sstream>
#include <stdexcept>
#include <limits>

RPN::RPN() {}

RPN::RPN(const RPN &other) : _numbers(other._numbers) {}

RPN &RPN::operator=(const RPN &other)
{
    if (this != &other)
        _numbers = other._numbers;
    return *this;
}

RPN::~RPN() {}

bool RPN::isOperator(const std::string &token) const
{
    return token.size() == 1 &&
        (token[0] == '+' || token[0] == '-' ||
         token[0] == '*' || token[0] == '/');
}

void RPN::calculate(char op)
{
    if (_numbers.size() < 2)
        throw std::runtime_error("Error");

    int b = _numbers.top();
    _numbers.pop();
    int a = _numbers.top();
    _numbers.pop();

    const int min = std::numeric_limits<int>::min();
    const int max = std::numeric_limits<int>::max();
    int result = 0;

    if (op == '+')
    {
        if ((b > 0 && a > max - b) || (b < 0 && a < min - b))
            throw std::runtime_error("Error");
        result = a + b;
    }
    else if (op == '-')
    {
        if ((b > 0 && a < min + b) || (b < 0 && a > max + b))
            throw std::runtime_error("Error");
        result = a - b;
    }
    else if (op == '*')
    {
        if ((a > 0 && b > 0 && a > max / b) ||
            (a > 0 && b < 0 && b < min / a) ||
            (a < 0 && b > 0 && a < min / b) ||
            (a < 0 && b < 0 && a < max / b))
            throw std::runtime_error("Error");
        result = a * b;
    }
    else if (op == '/')
    {
        if (b == 0 || (a == min && b == -1))
            throw std::runtime_error("Error");
        result = a / b;
    }
    _numbers.push(result);
}

int RPN::run(const std::string &expression)
{
    while (!_numbers.empty())
        _numbers.pop();

    std::istringstream stream(expression);
    std::string token;

    while (stream >> token)
    {
        if (token.size() == 1 && token[0] >= '0' && token[0] <= '9')
            _numbers.push(token[0] - '0');
        else if (isOperator(token))
            calculate(token[0]);
        else
            throw std::runtime_error("Error");
    }
    if (_numbers.size() != 1)
        throw std::runtime_error("Error");
    return _numbers.top();
}
