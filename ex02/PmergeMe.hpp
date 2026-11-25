#ifndef PMERGEME_HPP
#define PMERGEME_HPP

#include <vector>
#include <deque>
#include <cstddef>

class PmergeMe
{
    private:
        struct Node
        {
            int value;
            std::size_t id;
        };

        struct Pair
        {
            Node small;
            Node large;
        };

        std::vector<Node> _vector;
        std::deque<Node> _deque;

        int readNumber(const char *str) const;
        std::size_t nextGroup(std::size_t start, std::size_t end,
                            std::size_t count) const;
        void sortVector(std::vector<Node> &numbers);
        void sortDeque(std::deque<Node> &numbers);
        void insertVector(std::vector<Node> &chain, const Node &node,
                        std::size_t limit);
        void insertDeque(std::deque<Node> &chain, const Node &node,
                        std::size_t limit);
        void checkResult() const;
        void show(int argc, char **argv, double vectorTime,
                double dequeTime) const;

    public:
        PmergeMe();
        PmergeMe(const PmergeMe &other);
        PmergeMe &operator=(const PmergeMe &other);
        ~PmergeMe();

        void run(int argc, char **argv);
};

#endif
