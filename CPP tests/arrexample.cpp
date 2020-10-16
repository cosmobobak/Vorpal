#include <iostream>
#include <vector>

auto main() -> int
{
    int array[] = {0, 1, 2, 3, 4};

    for (int i = 0; i < 5; i++)
    {
        std::cout << *(array + i) << '\n';
    }
    
    return 0;
}