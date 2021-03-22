#pragma once

#include <array>
#include <vector>

#include "move.hpp"
#include "movegen.hpp"
#include "names.hpp"
#include "MaskSet.hpp"

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

    MaskSet* masks;

    State(MaskSet* m = nullptr) {
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
        if (!m) {
            masks = new MaskSet();
        } else {
            masks = m;
        }
    }

    /////////////////////////////////////////////////////////////
    ////////////////////////// SETTERS //////////////////////////
    /////////////////////////////////////////////////////////////

    auto set_piece_at(Square square, Piece piece, Colour colour) {
        U64 adding_bb = 1ULL << square;
        occupied |= adding_bb;
        for (U64& bb : occupied_co) bb &= ~adding_bb;
        occupied_co[colour] |= adding_bb;
        for (U64& bb : pieces) bb &= ~adding_bb;
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
    ///////////////////////// PREDICATES ////////////////////////
    /////////////////////////////////////////////////////////////

    /////////////////////////// CHECK ///////////////////////////

    auto is_check() const -> bool {
        // find our king
        Square ourKingLocation = bitscan_forward(pieces[KING] & occupied_co[turn]);

        // bitmask of the opponent's pieces
        U64 theirPieces = occupied_co[!turn] & occupied;

        // ideally we can just reverse pawn attack generation (note "turn" is not negated)
        bool pawnCheck = BB_PAWN_ATTACKS[turn][ourKingLocation] & (theirPieces & pieces[PAWN]);

        // if our king can attack one of their knights, like a knight,
        // then we are in check from that knight.
        bool knightCheck = BB_KNIGHT_ATTACKS[ourKingLocation] & (theirPieces & pieces[KNIGHT]);

        // generate bitmasks for the diagonal attacks from the king
        U64 diaglines = get_bishop_moves_c(ourKingLocation, occupied, masks);
        // generate bitmasks for the rank & file attacks from the king
        U64 straightlines = get_rook_moves_c(ourKingLocation, occupied, masks);

        bool bishopCheck = diaglines & (theirPieces & pieces[BISHOP]);
        bool rookCheck = straightlines & (theirPieces & pieces[ROOK]);
        bool queenCheck = (diaglines | straightlines) & (theirPieces & pieces[QUEEN]);

        return pawnCheck || knightCheck || bishopCheck || rookCheck || queenCheck;
    }

    ///////////////////////// MATERIAL //////////////////////////

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

    ////////////////////////// CHECKMATE ////////////////////////

    auto is_checkmate() const -> bool {
        return is_check() && num_legal_moves() == 0;
    }

    /////////////////////////// SIMPLE //////////////////////////

    auto is_threefold() const -> bool {
        return false;  // TODO
    }
    auto is_stalemate() const -> bool {
        return num_legal_moves() == 0 && !is_check();
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

    /////////////////////////////////////////////////////////////
    ////////////////////// MOVE GENERATION //////////////////////
    /////////////////////////////////////////////////////////////

    auto num_legal_moves() const -> int {
        return 0;
    }

    void add_pawn_pushes(std::vector<Move>& movevec) {
        U64 our_pieces = occupied_co[turn];
        U64 our_pawns = our_pieces & pieces[PAWN];
        // the square that a move originates from
        Square from_square;
        // the square that a move targets
        Square to_square;
        // the opponent's pieces
        U64 targets;
        // generate pawn pushes
        while (our_pawns) {
            // find the current moved piece
            from_square = bitscan_forward(our_pawns);
            // find the intersection of legal pawn pushes and empty squares
            targets = ~occupied & (masks->PAWN_MOVES[turn][from_square]);
            while (targets) {
                // find a target square
                to_square = bitscan_forward(targets);
                if ((1ULL << to_square) & BB_BACKRANKS) {
                    // emplace_back constructs a move in the vector
                    movevec.emplace_back(
                        from_square,
                        to_square,
                        KNIGHT_PROMOTION_FLAG);
                    movevec.emplace_back(
                        from_square,
                        to_square,
                        BISHOP_PROMOTION_FLAG);
                    movevec.emplace_back(
                        from_square,
                        to_square,
                        ROOK_PROMOTION_FLAG);
                    movevec.emplace_back(
                        from_square,
                        to_square,
                        QUEEN_PROMOTION_FLAG);
                } else {
                    // double pawn push from second rank to middle rank else normal
                    if ((1ULL << from_square) & BB_SECOND_RANKS && (1ULL << to_square) & BB_MIDDLE_RANKS) {
                        movevec.emplace_back(
                            from_square,
                            to_square,
                            PAWN_DOUBLE_PUSH_FLAG);
                    } else {
                        movevec.emplace_back(
                            from_square,
                            to_square,
                            QUIET_MOVE_FLAG);
                    }
                }
                // clear the target push square for next run
                targets &= targets - 1;
            }
            // clear the pawn from_square for next run
            our_pawns &= our_pawns - 1;
        }
    }

    void add_pawn_captures(std::vector<Move>& movevec) {
        U64 our_pieces = occupied_co[turn];
        U64 our_pawns = our_pieces & pieces[PAWN];
        // the square that a move originates from
        uint_fast8_t from_square;
        // the square that a move targets
        uint_fast8_t to_square;
        // the opponent's pieces
        U64 targets;
        // generate pawn captures
        while (our_pawns) {
            // find the current moved piece
            from_square = bitscan_forward(our_pawns);
            // find the intersection of legal pawn attacks and the opponent's pieces
            targets = (occupied_co[!turn] | ep_square) & BB_PAWN_ATTACKS[turn][from_square];
            while (targets) {
                // find a target square
                to_square = bitscan_forward(targets);
                if ((1ULL << to_square) & BB_BACKRANKS) {
                    // emplace_back constructs a move in the vector
                    movevec.emplace_back(
                        from_square,
                        to_square,
                        KNIGHT_PROMOTION_FLAG | CAPTURE_FLAG);
                    movevec.emplace_back(
                        from_square,
                        to_square,
                        BISHOP_PROMOTION_FLAG | CAPTURE_FLAG);
                    movevec.emplace_back(
                        from_square,
                        to_square,
                        ROOK_PROMOTION_FLAG | CAPTURE_FLAG);
                    movevec.emplace_back(
                        from_square,
                        to_square,
                        QUEEN_PROMOTION_FLAG | CAPTURE_FLAG);
                } else {
                    // test for en passant
                    if ((1ULL << to_square) & ep_square) {
                        movevec.emplace_back(
                            from_square,
                            to_square,
                            EP_FLAG);
                    } else {
                        movevec.emplace_back(
                            from_square,
                            to_square,
                            CAPTURE_FLAG);
                    }
                }
                // clear the target push square for next run
                targets &= targets - 1;
            }
            // clear the pawn from_square for next run
            our_pawns &= our_pawns - 1;
        }
    }

    void add_knight_moves(std::vector<Move>& movevec) {
        U64 our_pieces = occupied_co[turn];
        U64 our_knights = our_pieces & pieces[KNIGHT];
        // the square that a move originates from
        uint_fast8_t from_square;
        // the square that a move targets
        uint_fast8_t to_square;
        // the opponent's pieces
        U64 capture_targets;
        // empty slots
        U64 quiet_targets;
        // generate knight moves
        while (our_knights) {
            // find the current moved piece
            from_square = bitscan_forward(our_knights);
            // find the intersection of knight attacks and the opponent's pieces
            capture_targets = occupied_co[!turn] & BB_KNIGHT_ATTACKS[from_square];
            while (capture_targets) {
                // find a target square
                to_square = bitscan_forward(capture_targets);
                // add move to moves
                movevec.emplace_back(
                    from_square,
                    to_square,
                    CAPTURE_FLAG);
                // clear the target push square for next run
                capture_targets &= capture_targets - 1;
            }
            // find the intersection of knight attacks and the empty spaces
            quiet_targets = ~occupied & BB_KNIGHT_ATTACKS[from_square];
            while (quiet_targets) {
                // find a target square
                to_square = bitscan_forward(quiet_targets);
                // add move to moves
                movevec.emplace_back(
                    from_square,
                    to_square,
                    QUIET_MOVE_FLAG);
                // clear the target push square for next run
                quiet_targets &= quiet_targets - 1;
            }
            // clear the pawn from_square for next run
            our_knights &= our_knights - 1;
        }
    }

    void add_king_moves(std::vector<Move>& movevec) {
        U64 our_pieces = occupied_co[turn];
        // the square that a move originates from (there's only one king)
        uint_fast8_t from_square = bitscan_forward(our_pieces & pieces[KING]);
        // the square that a move targets
        uint_fast8_t to_square;
        // the opponent's pieces
        U64 capture_targets;
        // empty slots
        U64 quiet_targets;
        // generate king moves
        // find the intersection of king attacks and the opponent's pieces
        capture_targets = occupied_co[!turn] & BB_KING_ATTACKS[from_square];
        while (capture_targets) {
            // find a target square
            to_square = bitscan_forward(capture_targets);
            // add move to moves
            movevec.emplace_back(
                from_square,
                to_square,
                CAPTURE_FLAG);
            // clear the target push square for next run
            capture_targets &= capture_targets - 1;
        }
        // find the intersection of king attacks and the empty spaces
        quiet_targets = ~occupied & BB_KING_ATTACKS[from_square];
        while (quiet_targets) {
            // find a target square
            to_square = bitscan_forward(quiet_targets);
            // add move to moves
            movevec.emplace_back(
                from_square,
                to_square,
                QUIET_MOVE_FLAG);
            // clear the target push square for next run
            quiet_targets &= quiet_targets - 1;
        }
        // generate castling moves
        if (castling_rights) {
            // TODO
        }
    }

    void add_bishop_moves(std::vector<Move>& movevec) {
        
    }

    auto pseudo_legal_moves() -> std::vector<Move> {
        std::vector<Move> moves;
        moves.reserve(32);
        add_pawn_pushes(moves);
        add_pawn_captures(moves);
        add_knight_moves(moves);
        add_king_moves(moves);
    }
};