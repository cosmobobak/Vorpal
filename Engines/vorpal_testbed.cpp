#include <iostream>
#include <algorithm>
#include <string>
#include <bit>
#include <cstdint>
#include <initializer_list>

#include "vorpal_node.hpp" //daisy-chaining

using namespace vorpal_node;

#define INF 10000000

auto main() -> int
{
    for (int origin = 0; origin < 64; origin++)
    {
        std::cout << vorpal_helpers::string(M.RAYS[vorpal_helpers::EAST][origin]) << '\n';
    }
    return 0;
}

//TODO: pawn attacks generator (ep_square?)
//
//MOVE GENERATOR
//RULES??