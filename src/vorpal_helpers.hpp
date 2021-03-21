#include <cassert>
#include <iostream>
#include <string>
#include <vector>

#include "engine.hpp"
#include "intrinsic_functions.hpp"
#include "move.hpp"
#include "movegen.hpp"
#include "names.hpp"
#include "rays.hpp"
#include "state.hpp"
#include "vorpal_bitmasks.hpp"

#define INF 10000000000

#define U64 unsigned long long
#define U32 unsigned __int32
#define U16 unsigned __int16
#define U8 unsigned __int8
#define S64 signed __int64
#define S32 signed __int32
#define S16 signed __int16
#define S8 signed __int8

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

auto row(int index) -> int {
    return index / 8;
}

auto col(int index) -> int {
    return index % 8;
}

auto index(int row, int col) -> int {
    if (row > 7 || col > 7 || row < 0 || col < 0) {
        return 64;
    }
    return row * 8 + col;
}

auto set_bit(int index, U64 &bitboard) -> U64 {
    bitboard |= 1ULL << index;
}

auto ray_bitmask_pregenerator(int square, int dir) -> U64 {
    int r = row(square);
    int c = col(square);
    U64 outputMask = 0;
    switch (dir) {
        case SOUTH_EAST:
            for (int i = 1; index(r + i, c + i) >= 0 && index(r + i, c + i) <= 63; i++) {
                set_bit(index(r + i, c + i), outputMask);
            }
            break;
        case NORTH_WEST:
            for (int i = 1; index(r - i, c - i) >= 0 && index(r - i, c - i) <= 63; i++) {
                set_bit(index(r - i, c - i), outputMask);
            }
            break;
        case SOUTH_WEST:
            for (int i = 1; index(r + i, c - i) >= 0 && index(r + i, c - i) <= 63; i++) {
                set_bit(index(r + i, c - i), outputMask);
            }
            break;
        case NORTH_EAST:
            for (int i = 1; index(r - i, c + i) >= 0 && index(r - i, c + i) <= 63; i++) {
                set_bit(index(r - i, c + i), outputMask);
            }
            break;
        case NORTH:
            for (int i = 1; index(r - i, c) >= 0 && index(r - i, c) <= 63; i++) {
                set_bit(index(r - i, c), outputMask);
            }
            break;
        case SOUTH:
            for (int i = 1; index(r + i, c) >= 0 && index(r + i, c) <= 63; i++) {
                set_bit(index(r + i, c), outputMask);
            }
            break;
        case WEST:
            for (int i = 1; index(r, c - i) >= 0 && index(r, c - i) <= 63; i++) {
                set_bit(index(r, c - i), outputMask);
            }
            break;
        case EAST:
            for (int i = 1; index(r, c + i) >= 0 && index(r, c + i) <= 63; i++) {
                set_bit(index(r, c + i), outputMask);
            }
            break;
        default:
            std::cout << "error in ray_bitmask_generator";
            break;
    }

    return outputMask;
}
