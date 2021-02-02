#include <iostream>

auto higher(int a, int b) -> int {
    return (a > b) ? a : b;
}

auto highest4(int a, int b, int c, int d) -> int {
    return higher(higher(a, b), higher(c, d));
}

auto highest10(int a, int b, int c, int d, int e, int f, int g, int h, int i, int j) -> int {
    return higher(higher(highest4(a, b, c, d), e), higher(highest4(f, g, h, i), j));
}

auto oldest(int a, int b, int c) -> int {
    int h = a;
    if (h < b) h = b;
    if (h < c) h = c;
    return h;
}

int main() {
    std::cout << highest4(5, 6, 7, 8) << '\n';
    std::cout << highest10(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
    return 0;
}
