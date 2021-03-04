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
    U64 pieces[6];
    U64 promoted;
    U64 ep_square;
    U64 castling_rights;  // make sure this goes when the king moves
    Colour turn;
    int movecount;
    int halfmove_clock;  // resets on captures and pawn moves
    std::vector<Move> move_stack;

    State() {
        occupied = BB_RANK_2 | BB_RANK_7 | BB_BACKRANKS;
        occupied_co[WHITE] = BB_RANK_1 | BB_RANK_2;
        occupied_co[BLACK] = BB_RANK_7 | BB_RANK_8;
        pieces[PAWN] = BB_RANK_2 | BB_RANK_7;
        pieces[KNIGHT] = BB_B1 | BB_G1 | BB_B8 | BB_G8;
        pieces[BISHOP] = BB_C1 | BB_F1 | BB_C8 | BB_F8;
        pieces[ROOK] = BB_CORNERS;
        pieces[QUEEN] = BB_D1 | BB_D8;
        pieces[KING] = BB_E1 | BB_E8;
        promoted = BB_EMPTY;
        ep_square = BB_EMPTY;
        castling_rights = BB_CORNERS;
        movecount = 0;
    }

    /////////////////////////////////////////////////////////////
    ////////////////////////// SETTERS //////////////////////////
    /////////////////////////////////////////////////////////////

    auto set_piece_at(Square square, Piece piece, Colour colour) {
        U64 adding_bb = 1ULL << square;
        occupied |= adding_bb;
        occupied_co[colour] |= adding_bb;
        pieces[piece] |= adding_bb;
    }

    /////////////////////////////////////////////////////////////
    /////////////////////// MOVE HANDLING ///////////////////////
    /////////////////////////////////////////////////////////////

    void push(Move move) {
        // TODO
    }

    void pop(Move move) {
        // TODO
    }

    void nullmove() {
        movecount++;
    }

    /////////////////////////////////////////////////////////////
    /////////////////////////// CHECK ///////////////////////////
    /////////////////////////////////////////////////////////////

    auto is_check() const -> bool {
        // find our king
        Square ourKingLocation = bitscan_forward(pieces[KING] & occupied_co[turn]);

        // bitmask of the opponent's pieces
        U64 theirPieces = occupied_co[!turn] & occupied;

        // if our king can attack one of their knights, like a knight,
        // then we are in check from that knight.
        bool knightCheck = BB_KNIGHT_ATTACKS[ourKingLocation] & (theirPieces & pieces[KNIGHT]);

        
        bool pawnCheck = BB_PAWN_ATTACKS[turn][ourKingLocation] & (theirPieces & pieces[PAWN]);
    }

    auto is_insufficient_material() const -> bool {
        // for a draw by insufficient_material, there must be no pawns, rooks, or queens
        bool baseRequirement = pieces[PAWN] == 0 && pieces[ROOK] == 0 && pieces[QUEEN] == 0;

        // after we have determined there are no pawns, rooks, or queens, draws occur when
        // 1. there are no bishops and 1 or fewer knights
        // 2. there are no knights and only light square bishops
        // 3. there are no knights and only dark square bishops
        bool knightDraw = popcount(pieces[KNIGHT]) <= 1 && pieces[BISHOP] == 0;
        bool lightBishopDraw = pieces[BISHOP] & BB_DARK_SQUARES == 0 && pieces[KNIGHT] == 0;
        bool darkBishopDraw = pieces[BISHOP] & BB_LIGHT_SQUARES == 0 && pieces[KNIGHT] == 0;
        // (for many of these, "popcount(bb) == 0" can be optimised to "bb == 0")

        return baseRequirement && (knightDraw || lightBishopDraw || darkBishopDraw);
    }
    auto is_stalemate() const -> bool {
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
        U64 our_pawns = our_pieces & pieces[PAWN];
        int_fast8_t current_from_square;
        while (our_pawns) {
            current_from_square = bitscan_forward(our_pawns);
        }
    }
};