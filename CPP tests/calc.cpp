#include <iostream>

char get_operator()
{
    char char_op;
    std::cout << "Enter the desired operator:\n";
    std::cin >> char_op;
    return char_op;
}

int get()
{
    
}

int dist(int time)
{
    return 16 * time * time;
}

int main()
{
    char o = get_operator();
    int x = get();
    int y = get();
    int d = calc(o, x, y);
    std::cout << "The object falls " << d << " feet.";
    return 0;
}