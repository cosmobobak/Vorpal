#include <cassert>
#include <iostream>
#include <string>
#include <vector>

#include "MaskSet.hpp"
#include "engine.hpp"
#include "intrinsic_functions.hpp"
#include "move.hpp"
#include "movegen.hpp"
#include "names.hpp"
#include "rays.hpp"
#include "state.hpp"

#define INF 10000000000

using U64 = unsigned long long;
using U32 = unsigned __int32;
using U16 = unsigned __int16;
using U8 = unsigned __int8;
using S64 = signed __int64;
using S32 = signed __int32;
using S16 = signed __int16;
using S8 = signed __int8;

auto square_notation(Square index) -> std::string {
    // 0 => A8
    // 63 => H1
    char letters[8] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'};
    char numbers[8] = {'1', '2', '3', '4', '5', '6', '7', '8'};
    char first;
    char second;
    int modu = index % 8;
    int quot = index / 8;
    first = letters[modu];
    second = numbers[quot];
    return {first, second};
}

//FUNCTIONS AND OVERLOADS

auto square_from_an(std::string an_square) -> int {
    int a = 7 - (an_square[0] - 97);
    int b = an_square[1] - '0' - 1;
    return 63 - (a + 8 * b);
}

template <class T>
void print(T arr[], int len) {
    std::cout << "{ ";
    for (int i = 0; i < len; i++) {
        std::cout << arr[i] << ", ";
    }
    std::cout << "}" << std::endl;
}

template <class T>
auto string(std::vector<T> v) -> std::string {
    std::string builder;
    builder.append("{ ");
    for (auto &&i : v) {
        builder.append(std::to_string(i));
        builder.append(", ");
    }
    builder.append("}");
    return builder;
}

auto string(U64 bitboard) -> std::string {
    std::string builder;
    for (int i = 0; i < 64; i++) {
        if (bitboard & (1LL << i)) {
            builder.append("X");
        } else {
            builder.append(".");
        }
        builder.append(" ");
        if (i % 8 == 7) {
            builder.append("\n");
        }
    }
    return builder;
}
