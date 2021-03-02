#pragma once

#include <array>
#include <vector>

#include "move.hpp"
#include "movegen.hpp"
#include "names.hpp"

using U64 = unsigned long long;

class State {
    U64 occupied;
    U64 occupied_co[2];
    U64 pawns;
    U64 knights;
    U64 bishops;
    U64 rooks;
    U64 queens;
    U64 kings;
    U64 promoted;
    U64 ep_square;
    U64 castling_rights;  // make sure this goes when the king moves
    Colour turn;
    int movecount;
    int halfmove_clock; // resets on captures and pawn moves
    std::vector<Move> move_stack;

    State() {
        occupied = BB_RANK_2 | BB_RANK_7 | BB_BACKRANKS;
        occupied_co[WHITE] = BB_RANK_1 | BB_RANK_2;
        occupied_co[BLACK] = BB_RANK_7 | BB_RANK_8;
        pawns = BB_RANK_2 | BB_RANK_7;
        knights = BB_B1 | BB_G1 | BB_B8 | BB_G8;
        bishops = BB_C1 | BB_F1 | BB_C8 | BB_F8;
        rooks = BB_CORNERS;
        queens = BB_D1 | BB_D8;
        kings = BB_E1 | BB_E8;
        promoted = BB_EMPTY;
        ep_square = BB_EMPTY;
        castling_rights = BB_CORNERS;
        movecount = 0;
    }

    inline void pass_turn() {
        movecount++;
    }

    auto is_insufficient_material() const -> bool {
        return pawns == 0 && rooks == 0 && queens == 0 && (popcount(knights) <= 1 || ((popcount(bishops & BB_LIGHT_SQUARES) == 0 || popcount(bishops & BB_DARK_SQUARES) == 0) && knights == 0));
    }
    auto is_stalemate() const -> bool{
        return legal_moves().size() == 0 && !is_check();
    }
    auto is_fifty_moves() const -> bool {
        return halfmove_clock >= 50;
    }
    auto is_draw() const -> bool {
        return is_insufficient_material() || is_stalemate() || is_fifty_moves() || is_threefold();
    }
    auto is_game_over() const -> bool {
        return is_checkmate() || is_draw();
    }

    auto legal_moves() -> std::vector<Move> {
        U64 our_pieces = occupied_co[turn];
        std::vector<Move> moves;
        moves.reserve(32);
        // PAWN FORWARD GENERATION
        U64 our_pawns = our_pieces & pawns;
        int_fast8_t current_from_square;
        while (our_pawns) {
            current_from_square = bitscan_forward(our_pawns);
        }
    }
};