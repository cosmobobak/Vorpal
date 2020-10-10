#include <iostream>

int get_seconds()
{
    int count;
    std::cout << "Enter the time the object falls for:\n";
    std::cin >> count;
    return count;
}

int dist(int time)
{
    return 16 * time * time;
}

int main()
{
    int t = get_seconds();
    int d = dist(t);
    std::cout << "The object falls " << d << " feet.";
    return 0;
}