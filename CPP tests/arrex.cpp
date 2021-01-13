#include <iostream>
#include <array>

auto main() -> int
{
    int arr1[] = {1, 2, 3};
    for (int i = 0; i < 3; i++)
    {
        //std::cout << arr[i] << " ";
        std::cout << *(arr1 + i) << " ";
    }

    std::array<int, 3> arr2 = {1, 2, 3};
    for (int x : arr2)
    {
        std::cout << x << " ";
    }

    return 0;
}