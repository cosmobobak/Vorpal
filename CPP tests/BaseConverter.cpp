#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <vector>

const std::array<char, 36> SYMBOLS = {'0',
                                      '1',
                                      '2',
                                      '3',
                                      '4',
                                      '5',
                                      '6',
                                      '7',
                                      '8',
                                      '9',
                                      'A',
                                      'B',
                                      'C',
                                      'D',
                                      'E',
                                      'F',
                                      'G',
                                      'H',
                                      'I',
                                      'J',
                                      'K',
                                      'L',
                                      'M',
                                      'N',
                                      'O',
                                      'P',
                                      'Q',
                                      'R',
                                      'S',
                                      'T',
                                      'U',
                                      'V',
                                      'W',
                                      'X',
                                      'Y',
                                      'Z'};

auto hi_digit_position(int n, int base) {
    int count = 0;
    for (; n != 0; count++) {
        n /= base;
    }
    return count - 1;
}

auto construct_repr(int n, int base) -> std::vector<char> {
    int iterations = hi_digit_position(n, base);
    std::vector<char> out;
    for (int i = iterations + 1; i; i--) {
        out.push_back(SYMBOLS[n % base]);
        n /= base;
    }
    std::reverse(out.begin(), out.end());
    return out;
}

int main(int argc, char const *argv[]) {
    int number = atoi(argv[1]);
    int base = atoi(argv[2]);
    std::vector<char> input = construct_repr(number, base);
    std::string s(input.begin(), input.end());
    std::cout << s;
    std::cout << (int)'A';
    return 0;
}
