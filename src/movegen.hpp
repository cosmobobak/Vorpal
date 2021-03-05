#pragma once

#include "intrinsic_functions.hpp"
#include "names.hpp"

auto get_bishop_moves_c(int square, U64 blockers, U64** raymasks) -> U64 {
    U64 attacks = 0;

    // North West
    // OR-on the northwest ray to the attacks accumulator
    attacks |= raymasks[NORTH_WEST][square];
    // if there's a blocker on the northwest ray
    if (raymasks[NORTH_WEST][square] & blockers) {
        //find blocker index
        int blockerIndex = bitscan_forward(raymasks[NORTH_WEST][square] & blockers);
        // use AND to eliminate the ray past the blocker
        attacks &= ~raymasks[NORTH_WEST][blockerIndex];
    }

    // North East
    // OR-on the northeast ray to the attacks accumulator
    attacks |= raymasks[NORTH_EAST][square];
    if (raymasks[NORTH_EAST][square] & blockers) {
        int blockerIndex = bitscan_forward(raymasks[NORTH_EAST][square] & blockers);
        attacks &= ~raymasks[NORTH_EAST][blockerIndex];
    }

    // South East
    // OR-on the southeast ray to the attacks accumulator
    attacks |= raymasks[SOUTH_EAST][square];
    if (raymasks[SOUTH_EAST][square] & blockers) {
        int blockerIndex = bitscan_reverse(raymasks[SOUTH_EAST][square] & blockers);
        attacks &= ~raymasks[SOUTH_EAST][blockerIndex];
    }

    // South West
    // OR-on the southwest ray to the attacks accumulator
    attacks |= raymasks[SOUTH_WEST][square];
    if (raymasks[SOUTH_WEST][square] & blockers) {
        int blockerIndex = bitscan_reverse(raymasks[SOUTH_WEST][square] & blockers);
        attacks &= ~raymasks[SOUTH_WEST][blockerIndex];
    }

    return attacks;
}

// 0 0 0 0 0 0 0 0
// 1 0 0 0 0 0 0 0
// 0 1 0 0 0 0 0 0
// 0 0 1 0 0 0 0 0
// 0 0 0 1 0 0 0 0
// 0 0 0 0 0 0 0 0
// 0 0 0 0 0 0 0 0
// 0 0 0 0 0 0 0 0
// ^ FORWARD (right then down)

// 0 0 0 0 0 0 0 1
// 0 0 0 0 0 0 1 0
// 0 0 0 0 0 1 0 0
// 0 0 0 0 1 0 0 0
// 0 0 0 1 0 0 0 0
// 0 0 0 0 0 0 0 0
// 0 0 0 0 0 0 0 0
// 0 0 0 0 0 0 0 0
// ^ FORWARD (right then down)

// 0 0 0 0 0 0 0 0
// 0 0 0 0 0 0 0 0
// 0 0 0 0 0 0 0 0
// 0 0 0 0 0 0 0 0
// 0 0 0 1 0 0 0 0
// 0 0 0 0 1 0 0 0
// 0 0 0 0 0 1 0 0
// 0 0 0 0 0 0 1 0
// ^ REVERSE (left then up)

// 0 0 0 0 0 0 0 0
// 0 0 0 0 0 0 0 0
// 0 0 0 0 0 0 0 0
// 0 0 0 0 0 0 0 0
// 0 0 0 1 0 0 0 0
// 0 0 1 0 0 0 0 0
// 0 1 0 0 0 0 0 0
// 1 0 0 0 0 0 0 0
// ^ REVERSE (left then up)

auto get_rook_moves_c(int square, U64 blockers, U64** raymasks) -> U64 {
    U64 attacks = 0;

    // North West
    // OR-on the northwest ray to the attacks accumulator
    attacks |= raymasks[NORTH][square];
    // if there's a blocker on the northwest ray
    if (raymasks[NORTH][square] & blockers) {
        //find blocker index
        int blockerIndex = bitscan_forward(raymasks[NORTH][square] & blockers);
        // use AND to eliminate the ray past the blocker
        attacks &= ~raymasks[NORTH][blockerIndex];
    }

    // North East
    // OR-on the northeast ray to the attacks accumulator
    attacks |= raymasks[WEST][square];
    if (raymasks[WEST][square] & blockers) {
        int blockerIndex = bitscan_forward(raymasks[WEST][square] & blockers);
        attacks &= ~raymasks[WEST][blockerIndex];
    }

    // South East
    // OR-on the southeast ray to the attacks accumulator
    attacks |= raymasks[SOUTH][square];
    if (raymasks[SOUTH][square] & blockers) {
        int blockerIndex = bitscan_reverse(raymasks[SOUTH][square] & blockers);
        attacks &= ~raymasks[SOUTH][blockerIndex];
    }

    // South West
    // OR-on the southwest ray to the attacks accumulator
    attacks |= raymasks[EAST][square];
    if (raymasks[EAST][square] & blockers) {
        int blockerIndex = bitscan_reverse(raymasks[EAST][square] & blockers);
        attacks &= ~raymasks[EAST][blockerIndex];
    }

    return attacks;
}

// 0 0 0 1 0 0 0 0
// 0 0 0 1 0 0 0 0
// 0 0 0 1 0 0 0 0
// 0 0 0 1 0 0 0 0
// 0 0 0 1 0 0 0 0
// 0 0 0 0 0 0 0 0
// 0 0 0 0 0 0 0 0
// 0 0 0 0 0 0 0 0
// ^ FORWARD (right then down)

// 0 0 0 0 0 0 0 0
// 0 0 0 0 0 0 0 0
// 0 0 0 0 0 0 0 0
// 0 0 0 0 0 0 0 0
// 1 1 1 1 0 0 0 0
// 0 0 0 0 0 0 0 0
// 0 0 0 0 0 0 0 0
// 0 0 0 0 0 0 0 0
// ^ FORWARD (right then down)

// 0 0 0 0 0 0 0 0
// 0 0 0 0 0 0 0 0
// 0 0 0 0 0 0 0 0
// 0 0 0 0 0 0 0 0
// 0 0 0 1 0 0 0 0
// 0 0 0 1 0 0 0 0
// 0 0 0 1 0 0 0 0
// 0 0 0 1 0 0 0 0
// ^ REVERSE (left then up)

// 0 0 0 0 0 0 0 0
// 0 0 0 0 0 0 0 0
// 0 0 0 0 0 0 0 0
// 0 0 0 0 0 0 0 0
// 0 0 0 1 1 1 1 1
// 0 0 0 0 0 0 0 0
// 0 0 0 0 0 0 0 0
// 0 0 0 0 0 0 0 0
// ^ REVERSE (left then up)