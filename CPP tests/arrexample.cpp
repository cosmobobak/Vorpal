#include <iostream>

auto average(float x, float y, float z) -> float
{
    return (x + y + z) / 3;
}

auto main() -> int
{
    std::cout << average(3, 4, 5);
    return 0;
}