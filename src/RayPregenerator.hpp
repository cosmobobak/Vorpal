#include "names.hpp"
#include "vorpal_helpers.hpp"

using U64 = unsigned long long;

namespace RBP {
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
};  // namespace RBP