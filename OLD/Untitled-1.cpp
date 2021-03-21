#include <iostream>

int raiseP(int base, int exp) {
    int acc = 1;
    for (int i = 0; i < exp; i++) {
        acc *= base;
    }
    return acc;
}

int char_to_int(char *c) {
    int i = *c - '0';
    if (i < 0 || i > 9) {
        return 0;
    }
    return i;
}

int timehandler_nd(char *start) {
    int arr[6];
    int counter = 0;
    for (int i = 0; i < 8; i++) {
        if (i != 2 && i != 5) {
            arr[counter] = char_to_int(start + i);
            counter++;
        }
    }
    int total = 0;
    for (int i = 0; i < 6; i++) {
        total += arr[5 - i] * raiseP(10, i);
    }
    return total;
}

// double mapper(int hour)
// {
//     double a = 0.8888889 * hour - 0.2222222 * raiseP(hour, 2) + 0.01234568 * raiseP(hour, 3);
//     return a;
// }

void generate_clock(int time) {
    int hour = time / 10000;
    int minute = (time - hour) / 100;
    int second = time - minute - hour;

    double hourX = 16 * sin(hAngle);
    double hourY = 16 * cos(hAngle);
}

int main() {
    // char timething[24] = "20-12-10 12:32:56";
    // std::cout << timehandler_nd(timething + 9);
    // generate_clock(30000);
    // generate_clock(90000);
    for (int i = 1; i < 25; i++) {
        std::cout << i << " " << i % 12 << "\n";
    }
}
