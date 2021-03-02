#pragma once

#include "names.hpp"
#include "intrinsic_functions.hpp"

auto get_bishop_moves_c(int square, U64 blockers) -> U64 {
    U64 attacks = 0ULL;

    // North West
    attacks |= M.RAYS[NORTH_WEST][square];      // OR-on the northwest ray to the attacks accumulator
    if (M.RAYS[NORTH_WEST][square] & blockers)  // if there's a blocker on the northwest ray
    {
        int blockerIndex = bitscan_forward(M.RAYS[NORTH_WEST][square] & blockers);  //find blocker index
        attacks &= ~M.RAYS[NORTH_WEST][blockerIndex];                               // use AND to eliminate the ray past the blocker
    }

    // North East
    attacks |= M.RAYS[NORTH_EAST][square];
    if (M.RAYS[NORTH_EAST][square] & blockers) {
        int blockerIndex = bitscan_forward(M.RAYS[NORTH_EAST][square] & blockers);
        attacks &= ~M.RAYS[NORTH_EAST][blockerIndex];
    }

    // South East
    attacks |= M.RAYS[SOUTH_EAST][square];
    if (M.RAYS[SOUTH_EAST][square] & blockers) {
        int blockerIndex = bitscan_reverse(M.RAYS[SOUTH_EAST][square] & blockers);
        attacks &= ~M.RAYS[SOUTH_EAST][blockerIndex];
    }

    // South West
    attacks |= M.RAYS[SOUTH_WEST][square];
    if (M.RAYS[SOUTH_WEST][square] & blockers) {
        int blockerIndex = bitscan_reverse(M.RAYS[SOUTH_WEST][square] & blockers);
        attacks &= ~M.RAYS[SOUTH_WEST][blockerIndex];
    }

    return attacks;
}