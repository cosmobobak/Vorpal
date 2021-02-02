#include <iostream>

class IntegerHolder {
   public:
    int a;

    IntegerHolder(int x) {
        a = x;
    }
};

void add(IntegerHolder x) {
    x.a++;
}

auto main() -> int {
    IntegerHolder example = IntegerHolder(10);
    add(example);
    std::cout << example.a;
    return 0;
}

