#include "PmergeMe.hpp"
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <climits>
#include <ctime>
#include <algorithm>

PmergeMe::PmergeMe()
{
}

PmergeMe::PmergeMe(const PmergeMe &other)
    : _vector(other._vector), _deque(other._deque)
{
}

PmergeMe &PmergeMe::operator=(const PmergeMe &other)
{
    if (this != &other)
    {
        _vector = other._vector;
        _deque = other._deque;
    }
    return *this;
}

PmergeMe::~PmergeMe()
{
}

int PmergeMe::readNumber(const char *str) const
{
    int value = 0;

    if (*str == '+')
        ++str;
    if (*str == '\0')
        throw std::runtime_error("invalid number");
    while (*str)
    {
        if (*str < '0' || *str > '9')
            throw std::runtime_error("invalid number");
        int digit = *str - '0';
        if (value > (INT_MAX - digit) / 10)
            throw std::runtime_error("number too large");
        value = value * 10 + digit;
        ++str;
    }
    if (value == 0)
        throw std::runtime_error("number must be positive");
    return value;
}

// Jacobsthal lim: 1, 3, 5, 11, 21, 43...
std::size_t PmergeMe::nextGroup(std::size_t start, std::size_t end,
                                std::size_t count) const
{
    if (start > (count - end) / 2)
        return count;
    return end + 2 * start;
}

// Binary insertion: search only before the associated large value
void PmergeMe::insertVector(std::vector<Node> &chain, const Node &node,
                          std::size_t limit)
{
    std::size_t left = 0;
    std::size_t right = limit;

    while (left < right)
    {
        std::size_t mid = left + (right - left) / 2;
        if (chain[mid].value < node.value)
            left = mid + 1;
        else
            right = mid;
    }
    chain.insert(chain.begin() + left, node);
}

void PmergeMe::sortVector(std::vector<Node> &numbers)
{
    if (numbers.size() < 2)
        return;

    std::vector<Pair> pairs;
    std::vector<Node> large;
    std::vector<Node> chain;

    // comparaison chaq pair once and keep the smaller/larger elements
    for (std::size_t i = 0; i + 1 < numbers.size(); i += 2)
    {
        Pair pair = {numbers[i], numbers[i + 1]};
        if (pair.large.value < pair.small.value)
            std::swap(pair.small, pair.large);
        Node node = {pair.large.value, pairs.size()};
        pairs.push_back(pair);
        large.push_back(node);
    }

    // tri recursif  the larger values IDs identify the pairs
    sortVector(large);

    // Build chain: b1, a1, a2, .. an
    chain.push_back(pairs[large[0].id].small);
    for (std::size_t i = 0; i < large.size(); ++i)
        chain.push_back(pairs[large[i].id].large);

    // Insert the remaining small values in reverse Jacobsthal gp
    // unpaired value is treated as the last pendng value si il y en a
    std::size_t count = pairs.size() + numbers.size() % 2;
    std::size_t start = 1;
    std::size_t end = std::min(static_cast<std::size_t>(3), count);

    while (start < count)
    {
        for (std::size_t i = end; i > start; --i)
        {
            if (i > pairs.size())
                insertVector(chain, numbers.back(), chain.size());
            else
            {
                const Pair &pair = pairs[large[i - 1].id];
                std::size_t limit = 0;
                // Use ID not value caus duplicates ok
                while (chain[limit].id != pair.large.id)
                    ++limit;
                insertVector(chain, pair.small, limit);
            }
        }
        std::size_t next = nextGroup(start, end, count);
        start = end;
        end = next;
    }
    numbers.swap(chain);
}

// Binary insertion: search only before the associated large value
void PmergeMe::insertDeque(std::deque<Node> &chain, const Node &node,
                          std::size_t limit)
{
    std::size_t left = 0;
    std::size_t right = limit;

    while (left < right)
    {
        std::size_t mid = left + (right - left) / 2;
        if (chain[mid].value < node.value)
            left = mid + 1;
        else
            right = mid;
    }
    chain.insert(chain.begin() + left, node);
}

void PmergeMe::sortDeque(std::deque<Node> &numbers)
{
    if (numbers.size() < 2)
        return;

    std::deque<Pair> pairs;
    std::deque<Node> large;
    std::deque<Node> chain;

    // Compare each pair once and keep the smaller/larger elements
    for (std::size_t i = 0; i + 1 < numbers.size(); i += 2)
    {
        Pair pair = {numbers[i], numbers[i + 1]};
        if (pair.large.value < pair.small.value)
            std::swap(pair.small, pair.large);
        Node node = {pair.large.value, pairs.size()};
        pairs.push_back(pair);
        large.push_back(node);
    }

    // Recursively sort larger values. Their IDs identify the pairs
    sortDeque(large);

    // Build the main chain: b1, a1, a2, ..., an.
    chain.push_back(pairs[large[0].id].small);
    for (std::size_t i = 0; i < large.size(); ++i)
        chain.push_back(pairs[large[i].id].large);

    // Insert remaining small values in reverse Jacobsthal groups
    // The unpaired value is treated as the last pending value
    std::size_t count = pairs.size() + numbers.size() % 2;
    std::size_t start = 1;
    std::size_t end = std::min(static_cast<std::size_t>(3), count);

    while (start < count)
    {
        for (std::size_t i = end; i > start; --i)
        {
            if (i > pairs.size())
                insertDeque(chain, numbers.back(), chain.size());
            else
            {
                const Pair &pair = pairs[large[i - 1].id];
                std::size_t limit = 0;
                // Use the ID, not the value: duplicates are allowed
                while (chain[limit].id != pair.large.id)
                    ++limit;
                insertDeque(chain, pair.small, limit);
            }
        }
        std::size_t next = nextGroup(start, end, count);
        start = end;
        end = next;
    }
    numbers.swap(chain);
}

// Check both containers outside the timed sections
void PmergeMe::checkResult() const
{
    if (_vector.size() != _deque.size())
        throw std::runtime_error("sorting failed");
    for (std::size_t i = 0; i < _vector.size(); ++i)
    {
        if (_vector[i].value != _deque[i].value)
            throw std::runtime_error("sorting failed");
        if (i > 0 && _vector[i - 1].value > _vector[i].value)
            throw std::runtime_error("sorting failed");
    }
}

void PmergeMe::show(int argc, char **argv, double vectorTime,
                     double dequeTime) const
{
    std::cout << "Before:";
    for (int i = 1; i < argc; ++i)
        std::cout << " " << argv[i];
    std::cout << std::endl;

    std::cout << "After:";
    for (std::size_t i = 0; i < _vector.size(); ++i)
        std::cout << " " << _vector[i].value;
    std::cout << std::endl;

    std::cout << std::fixed << std::setprecision(3);
    std::cout << "Time to process a range of " << _vector.size()
              << " elements with std::vector : " << vectorTime
              << " us" << std::endl;
    std::cout << "Time to process a range of " << _deque.size()
              << " elements with std::deque : " << dequeTime
              << " us" << std::endl;
}

void PmergeMe::run(int argc, char **argv)
{
    if (argc < 2)
        throw std::runtime_error("no numbers provided");

    std::clock_t start = std::clock();
    _vector.clear();
    _vector.reserve(argc - 1);
    for (int i = 1; i < argc; ++i)
    {
        Node node = {readNumber(argv[i]), static_cast<std::size_t>(i - 1)};
        _vector.push_back(node);
    }
    sortVector(_vector);
    std::clock_t end = std::clock();
    double vectorTime = (end - start) * 1000000.0 / CLOCKS_PER_SEC;

    start = std::clock();
    _deque.clear();
    for (int i = 1; i < argc; ++i)
    {
        Node node = {readNumber(argv[i]), static_cast<std::size_t>(i - 1)};
        _deque.push_back(node);
    }
    sortDeque(_deque);
    end = std::clock();
    double dequeTime = (end - start) * 1000000.0 / CLOCKS_PER_SEC;

    checkResult();
    show(argc, argv, vectorTime, dequeTime);
}
