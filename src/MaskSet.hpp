#pragma once

using U64 = unsigned long long;

#include "names.hpp"
#include "RayPregenerator.hpp"

class MaskSet {
   public:
    U64 PAWN_MOVES[2][64];
    U64 PAWN_ATTACKS[2][64];
    U64 KNIGHT_ATTACKS[64];
    U64 KING_ATTACKS[64];
    U64 RAYS[64][8];

    MaskSet() {
        for (int i = 0; i < 64; i++) {
            // if a pawn is on the backrank, it can't move.
            if ((1ULL << i) & BB_BACKRANKS) {
                PAWN_MOVES[WHITE][i] = 0;
                PAWN_MOVES[BLACK][i] = 0;
            } else if ((1ULL << i) & BB_RANK_2) {
                // on rank 2, the white pawns can double-move
                PAWN_MOVES[WHITE][i] |= 1LL << i + 8;
                PAWN_MOVES[WHITE][i] |= 1LL << i + 16;
                PAWN_MOVES[BLACK][i] |= 1LL << i - 8;
            } else if ((1ULL << i) & BB_RANK_7) {
                // on rank 7, the black pawns can double-move
                PAWN_MOVES[WHITE][i] |= 1LL << i + 8;
                PAWN_MOVES[BLACK][i] |= 1LL << i - 8;
                PAWN_MOVES[BLACK][i] |= 1LL << i - 16;
            } else {
                PAWN_MOVES[WHITE][i] |= 1LL << i + 8;
                PAWN_MOVES[BLACK][i] |= 1LL << i - 8;
            }
        }

        for (int i = 0; i < 64; i++) {
            PAWN_ATTACKS[WHITE][i] = BB_PAWN_ATTACKS[WHITE][i];
            PAWN_ATTACKS[BLACK][i] = BB_PAWN_ATTACKS[BLACK][i];
        }

        for (int i = 0; i < 64; i++) {
            for (int dir = 0; dir < 8; dir++) {
                RAYS[i][dir] = RBP::ray_bitmask_pregenerator(i, dir);
            }
        }

        for (int i = 0; i < 64; i++) KNIGHT_ATTACKS[i] = BB_KNIGHT_ATTACKS[i];
        for (int i = 0; i < 64; i++) KING_ATTACKS[i] = BB_KING_ATTACKS[i];
    }
};
