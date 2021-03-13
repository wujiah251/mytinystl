#include <vector>
#include <iostream>

void test_vector()
{
    std::vector<int> a{1, 2, 3};
    a.emplace_back(1);
    for (auto item : a)
        std::cout << item << ' ';
    std::cout << std::endl;
}