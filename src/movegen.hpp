#pragma once

#include "intrinsic_functions.hpp"
#include "names.hpp"

auto get_bishop_moves_c(const Square square, const U64 blockers, const MaskSet* masks) -> U64 {
    U64 attacks = 0;

    // North West
    // OR-on the northwest ray to the attacks accumulator
    attacks |= masks->RAYS[NORTH_WEST][square];
    // if there's a blocker on the northwest ray
    if (masks->RAYS[NORTH_WEST][square] & blockers) {
        // find blocker index
        int blockerIndex = bitscan_forward(masks->RAYS[NORTH_WEST][square] & blockers);
        // use AND to eliminate the ray past the blocker
        attacks &= ~masks->RAYS[NORTH_WEST][blockerIndex];
    }

    // North East
    // OR-on the northeast ray to the attacks accumulator
    attacks |= masks->RAYS[NORTH_EAST][square];
    if (masks->RAYS[NORTH_EAST][square] & blockers) {
        int blockerIndex = bitscan_forward(masks->RAYS[NORTH_EAST][square] & blockers);
        attacks &= ~masks->RAYS[NORTH_EAST][blockerIndex];
    }

    // South East
    // OR-on the southeast ray to the attacks accumulator
    attacks |= masks->RAYS[SOUTH_EAST][square];
    if (masks->RAYS[SOUTH_EAST][square] & blockers) {
        int blockerIndex = bitscan_reverse(masks->RAYS[SOUTH_EAST][square] & blockers);
        attacks &= ~masks->RAYS[SOUTH_EAST][blockerIndex];
    }

    // South West
    // OR-on the southwest ray to the attacks accumulator
    attacks |= masks->RAYS[SOUTH_WEST][square];
    if (masks->RAYS[SOUTH_WEST][square] & blockers) {
        int blockerIndex = bitscan_reverse(masks->RAYS[SOUTH_WEST][square] & blockers);
        attacks &= ~masks->RAYS[SOUTH_WEST][blockerIndex];
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

auto get_rook_moves_c(const Square square, const U64 blockers, const MaskSet* masks) -> U64 {
    U64 attacks = 0;

    // North West
    // OR-on the northwest ray to the attacks accumulator
    attacks |= masks->RAYS[NORTH][square];
    // if there's a blocker on the northwest ray
    if (masks->RAYS[NORTH][square] & blockers) {
        // find blocker index
        int blockerIndex = bitscan_forward(masks->RAYS[NORTH][square] & blockers);
        // use AND to eliminate the ray past the blocker
        attacks &= ~masks->RAYS[NORTH][blockerIndex];
    }

    // North East
    // OR-on the northeast ray to the attacks accumulator
    attacks |= masks->RAYS[WEST][square];
    if (masks->RAYS[WEST][square] & blockers) {
        int blockerIndex = bitscan_forward(masks->RAYS[WEST][square] & blockers);
        attacks &= ~masks->RAYS[WEST][blockerIndex];
    }

    // South East
    // OR-on the southeast ray to the attacks accumulator
    attacks |= masks->RAYS[SOUTH][square];
    if (masks->RAYS[SOUTH][square] & blockers) {
        int blockerIndex = bitscan_reverse(masks->RAYS[SOUTH][square] & blockers);
        attacks &= ~masks->RAYS[SOUTH][blockerIndex];
    }

    // South West
    // OR-on the southwest ray to the attacks accumulator
    attacks |= masks->RAYS[EAST][square];
    if (masks->RAYS[EAST][square] & blockers) {
        int blockerIndex = bitscan_reverse(masks->RAYS[EAST][square] & blockers);
        attacks &= ~masks->RAYS[EAST][blockerIndex];
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