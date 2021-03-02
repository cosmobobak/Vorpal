#include <iostream>

#include "engine.hpp"
#include "intrinsic_functions.hpp"
#include "move.hpp"
#include "movegen.hpp"
#include "names.hpp"
#include "rays.hpp" 
#include "state.hpp"
#include "vorpal_helpers.hpp"
#include "vorpal_bitmasks.hpp"

int main(int argc, char const *argv[])
{
    MaskSet masks = MaskSet();
    State board = State(&masks);
    std::cout << "Vorpal running...";
    return 0;
}
