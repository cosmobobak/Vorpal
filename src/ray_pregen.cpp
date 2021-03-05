#include "vorpal_helpers.hpp"
#include "names.hpp"

Direction operator++(Direction& x) {
    return x = (Direction)(std::underlying_type<Direction>::type(x) + 1);
}

Direction operator*(Direction c) {
    return c;
}

Direction begin(Direction r) {
    return Direction::First;
}

Direction end(Direction r) {
    Direction l = Direction::Last;
    return ++l;
}

Square operator++(Square& x) {
    return x = (Square)(std::underlying_type<Square>::type(x) + 1);
}

Square operator*(Square c) {
    return c;
}

Square begin(Square r) {
    return Square::SquareFirst;
}

Square end(Square r) {
    Square l = Square::SquareLast;
    return ++l;
}

int main(int argc, char const *argv[])
{
    for (const auto& dir : Direction()) {
        std::cout << "Direction " << dir << ": {";
        for (const auto& square : Square()) {
            std::cout << ray_bitmask_generator(square, dir) << ", ";
        }
        std::cout << "}\n\n";
    }

    return 0;
}
