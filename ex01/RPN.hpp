#ifndef RPN_HPP
#define RPN_HPP

#include <stack>
#include <list>
#include <string>

class RPN
{
    private:
        std::stack<int, std::list<int> > _numbers;

        bool isOperator(const std::string &token) const;
        void calculate(char op);

    public:
        RPN();
        RPN(const RPN &other);
        RPN &operator=(const RPN &other);
        ~RPN();

        int run(const std::string &expression);
};

#endif
