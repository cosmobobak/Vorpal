#include <bitset>

#include "vorpal_bitmasks.hpp"

#define U64 unsigned long long

vorpal_bitmasks::MaskSet M;

namespace vorpal_node {
class Board {
   public:
    U64 BB_PIECES[6] = {
        0b0000000011111111000000000000000000000000000000001111111100000000,
        0b0100001000000000000000000000000000000000000000000000000001000010,
        0b0010010000000000000000000000000000000000000000000000000000100100,
        0b1000000100000000000000000000000000000000000000000000000010000001,
        0b0000100000000000000000000000000000000000000000000000000000001000,
        0b0001000000000000000000000000000000000000000000000000000000010000,
    };
    U64 BB_OCCUPIED = 0b1111111111111111000000000000000000000000000000001111111111111111;
    U64 BB_EMPTY = 0b0000000000000000111111111111111111111111111111110000000000000000;
    U64 BB_COLORS[2] = {
        0b0000000000000000000000000000000000000000000000001111111111111111,
        0b1111111111111111000000000000000000000000000000000000000000000000,
    };
    U64 EP_SQUARE = 0b0000000000000000000000000000000000000000000000000000000000000000;

    std::vector<vorpal_move::Move> stack;

    bool turn = WHITE;

    auto mod() -> int {
        if (turn) {
            return -1;
        } else {
            return 1;
        }
    }

    void show() {
        for (int i = 0; i < 64; i++) {
            std::cout << pieces[colored_piece_type_at(i)];
            std::cout << " ";
            if (i % 8 == 7) {
                std::cout << "\n";
            }
        }
    }

    auto get_square(U64 bb, int squareNum) -> bool {
        return (bb & M.MASK[squareNum]);
    }

    auto color_at(int i) -> int {
        if (get_square(BB_COLORS[WHITE], i)) {
            return WHITE;
        } else if (get_square(BB_COLORS[BLACK], i)) {
            return BLACK;
        } else {
            return NO_COLOR;
        }
    }

    auto move_from_uci(std::string notation) -> vorpal_move::Move {
        int f, t, p, cp;
        bool c;
        f = square_from_an(notation.substr(0, 2));
        t = square_from_an(notation.substr(2, 2));
        p = colored_piece_type_at(f);
        c = color_at(f);
        cp = colored_piece_type_at(t);
        return vorpal_move::Move(f, t, p, c, cp);
    }

    auto piece_type_at(int squareNum) -> int {
        //Gets the piece type at the given square.
        U64 mask = M.MASK[squareNum];

        if (!(BB_OCCUPIED & mask)) {
            return 12;  // Early return
        } else if (BB_PIECES[PAWN] & mask) {
            return PAWN;
        } else if (BB_PIECES[KNIGHT] & mask) {
            return KNIGHT;
        } else if (BB_PIECES[BISHOP] & mask) {
            return BISHOP;
        } else if (BB_PIECES[ROOK] & mask) {
            return ROOK;
        } else if (BB_PIECES[QUEEN] & mask) {
            return QUEEN;
        } else if (BB_PIECES[KING] & mask) {
            return KING;
        } else {
            return 42;
        }
    }

    auto colored_piece_type_at(int squareNum) -> int {
        //Gets the piece type at the given square.
        U64 mask = M.MASK[squareNum];
        int mod;
        if (BB_COLORS[0] & mask) {
            mod = 0;
        } else {
            mod = 6;
        }

        if (!(BB_OCCUPIED & mask)) {
            return 12;  // Early return
        } else if (BB_PIECES[PAWN] & mask) {
            return PAWN + mod;
        } else if (BB_PIECES[KNIGHT] & mask) {
            return KNIGHT + mod;
        } else if (BB_PIECES[BISHOP] & mask) {
            return BISHOP + mod;
        } else if (BB_PIECES[ROOK] & mask) {
            return ROOK + mod;
        } else if (BB_PIECES[QUEEN] & mask) {
            return QUEEN + mod;
        } else if (BB_PIECES[KING] & mask) {
            return KING + mod;
        } else {
            return 42;
        }
    }

    auto is_checkmate() -> bool  // TODO
    {
        return false;
    }

    auto can_claim_fifty_moves() -> bool  // TODO
    {
        return false;
    }

    auto is_insufficient_material() -> bool  // TODO
    {
        return false;
    }

    auto is_threefold() -> bool  // TODO
    {
        return false;
    }

    auto is_draw() -> bool  // TODO
    {
        return can_claim_fifty_moves() || is_insufficient_material() || is_threefold();
    }

    auto is_game_over() -> bool {
        return is_checkmate() || is_draw();
    }

    void push(vorpal_move::Move move)  //adapted from chessprogrammingwiki
    {
        //std::cout << *move << std::endl;
        U64 fromBB = M.MASK[move.from_square];
        U64 toBB = M.MASK[move.to_square];
        U64 fromToBB = fromBB ^ toBB;  // |+
        //std::cout << std::bitset<64>(fromToBB) << ' ' << move->piece << std::endl;
        BB_PIECES[move.piece % 6] ^= fromToBB;  // update piece bitboard
        BB_COLORS[move.color] ^= fromToBB;      // update white or black color bitboard
        if (move.iscapture) {
            BB_PIECES[move.cPiece] ^= toBB;  // reset the captured piece
            BB_PIECES[move.cColor] ^= toBB;  // update color bitboard by captured piece
        }
        BB_OCCUPIED ^= fromToBB;  // update occupied ...
        BB_EMPTY ^= fromToBB;     // ... and empty bitboard
        stack.push_back(move);
    }

    void pop() {
        vorpal_move::Move move = stack.back();
        U64 fromBB = M.MASK[move.from_square];
        U64 toBB = M.MASK[move.to_square];
        U64 fromToBB = fromBB ^ toBB;  // |+
        //std::cout << std::bitset<64>(fromToBB) << ' ' << move->piece << std::endl;
        BB_PIECES[move.piece % 6] ^= fromToBB;  // update piece bitboard
        BB_COLORS[move.color] ^= fromToBB;      // update white or black color bitboard
        if (move.iscapture) {
            BB_PIECES[move.cPiece] ^= toBB;  // reset the captured piece
            BB_PIECES[move.cColor] ^= toBB;  // update color bitboard by captured piece
        }
        BB_OCCUPIED ^= fromToBB;  // update occupied ...
        BB_EMPTY ^= fromToBB;     // ... and empty bitboard
        stack.pop_back();
    }

    /*auto slider_blockers(int square) -> U64  // TODO
        {
            U64 rooks_and_queens = BB_PIECES[ROOK] | BB_PIECES[QUEEN];
            U64 bishops_and_queens = BB_PIECES[BISHOP] | BB_PIECES[QUEEN];

            U64 snipers = ((BB_RANK_ATTACKS[square][0] & rooks_and_queens) |
                           (BB_FILE_ATTACKS[square][0] & rooks_and_queens) |
                           (BB_DIAG_ATTACKS[square][0] & bishops_and_queens));

            U64 blockers = 0;

            //for sniper in scan_reversed(snipers & self.occupied_co[not self.turn])
            //    b = between(king, sniper) & self.occupied;

            //    # Add to blockers if exactly one piece in-between.
            //    if b and BB_SQUARES[msb(b)] == b
            //        blockers |= b;

            return blockers & self.occupied_co[self.turn];
        }*/

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

    auto get_rook_moves_c(int square, U64 blockers) -> U64 {
        U64 attacks = 0ULL;

        // North
        attacks |= M.RAYS[NORTH][square];      // OR-on the northwest ray to the attacks accumulator
        if (M.RAYS[NORTH][square] & blockers)  // if there's a blocker on the northwest ray
        {
            int blockerIndex = bitscan_forward(M.RAYS[NORTH][square] & blockers);  //find blocker index
            attacks &= ~M.RAYS[NORTH][blockerIndex];                               // use AND to eliminate the ray past the blocker
        }

        // West
        attacks |= M.RAYS[EAST][square];
        if (M.RAYS[EAST][square] & blockers) {
            int blockerIndex = bitscan_forward(M.RAYS[WEST][square] & blockers);
            attacks &= ~M.RAYS[EAST][blockerIndex];
        }

        // South
        attacks |= M.RAYS[SOUTH][square];
        if (M.RAYS[SOUTH][square] & blockers) {
            int blockerIndex = bitscan_reverse(M.RAYS[SOUTH][square] & blockers);
            attacks &= ~M.RAYS[SOUTH][blockerIndex];
        }

        // East
        attacks |= M.RAYS[WEST][square];
        if (M.RAYS[WEST][square] & blockers) {
            int blockerIndex = bitscan_reverse(M.RAYS[EAST][square] & blockers);
            attacks &= ~M.RAYS[WEST][blockerIndex];
        }

        return attacks;
    }

    auto get_queen_moves_c(int square, U64 blockers) -> U64 {
        return get_bishop_moves_c(square, blockers) | get_rook_moves_c(square, blockers);
    }

    auto pseudo_legal_moves() -> std::vector<vorpal_move::Move>  // TODO
    {
        U64 our_pieces = BB_COLORS[turn];

        std::vector<vorpal_move::Move> moves;

        for (U64 from_bb : bitboard_split(BB_PIECES[PAWN] & our_pieces)) {  // generates the pawn moves
            int from_square = bitscan_forward(from_bb);
            for (U64 target_bb : bitboard_split(M.PAWN_MOVES[from_square][turn] & ~BB_OCCUPIED)) {  // handles pawn-forward moves
                int to_square = bitscan_forward(target_bb);
                moves.push_back(vorpal_move::Move(
                    from_square,
                    to_square,
                    PAWN,
                    turn,
                    1));  // setting cpiece here to 1 because a stack can be implemented to handle pop
            }
            for (U64 target_bb : bitboard_split(M.PAWN_ATTACKS[from_square][turn] & (BB_COLORS[!turn]) | EP_SQUARE)) {  // handles pawn captures
                int to_square = bitscan_forward(target_bb);
                moves.push_back(vorpal_move::Move(
                    from_square,
                    to_square,
                    PAWN,
                    turn));
            }
        }
        for (U64 from_bb : bitboard_split(BB_PIECES[KNIGHT] & our_pieces)) {
            int from_square = bitscan_forward(from_bb);
            for (U64 target_bb : bitboard_split(M.union_bitmask(KNIGHT, from_square) & ~BB_COLORS[turn])) {
                int to_square = bitscan_forward(target_bb);
                moves.push_back(vorpal_move::Move(
                    from_square,
                    to_square,
                    KNIGHT,
                    turn,
                    (get_square(BB_COLORS[~turn], to_square)) ? 1 : 12));  // setting cpiece here to 1 because a stack can be implemented to handle pop
            }
        }
        for (U64 from_bb : bitboard_split(BB_PIECES[KING] & our_pieces)) {
            int from_square = bitscan_forward(from_bb);
            for (U64 target_bb : bitboard_split(M.union_bitmask(KING, from_square) & ~BB_COLORS[turn])) {
                int to_square = bitscan_forward(target_bb);
                moves.push_back(vorpal_move::Move(
                    from_square,
                    to_square,
                    KING,
                    turn,
                    (get_square(BB_COLORS[!turn], to_square)) ? 1 : 12));  // setting cpiece here to 1 because a stack can be implemented to handle pop
            }
        }
        for (U64 from_bb : bitboard_split(BB_PIECES[BISHOP] & our_pieces)) {
            int from_square = bitscan_forward(from_bb);
            for (U64 target_bb : bitboard_split(get_bishop_moves_c(from_square, BB_OCCUPIED) & ~our_pieces)) {
                int to_square = bitscan_forward(target_bb);
                moves.push_back(vorpal_move::Move(
                    from_square,
                    to_square,
                    BISHOP,
                    turn,
                    (get_square(BB_COLORS[!turn], to_square)) ? 1 : 12));
            }
        }
        for (U64 from_bb : bitboard_split(BB_PIECES[ROOK] & our_pieces)) {
            int from_square = bitscan_forward(from_bb);
            for (U64 target_bb : bitboard_split(get_rook_moves_c(from_square, BB_OCCUPIED) & ~our_pieces)) {
                int to_square = bitscan_forward(target_bb);
                moves.push_back(vorpal_move::Move(
                    from_square,
                    to_square,
                    ROOK,
                    turn,
                    (get_square(BB_COLORS[!turn], to_square)) ? 1 : 12));
            }
        }
        for (U64 from_bb : bitboard_split(BB_PIECES[QUEEN] & our_pieces)) {
            int from_square = bitscan_forward(from_bb);
            for (U64 target_bb : bitboard_split(get_queen_moves_c(from_square, BB_OCCUPIED) & ~our_pieces)) {
                int to_square = bitscan_forward(target_bb);
                moves.push_back(vorpal_move::Move(
                    from_square,
                    to_square,
                    QUEEN,
                    turn,
                    (get_square(BB_COLORS[!turn], to_square)) ? 1 : 12));
            }
        }

        return moves;
    }

    auto legal_moves() -> std::vector<vorpal_move::Move>  // TODO
    {
        return pseudo_legal_moves();
    }

    auto bitboard_vector() -> std::vector<U64> {
        std::vector<U64> bbs = {BB_COLORS[0],
                                BB_COLORS[1],
                                BB_EMPTY,
                                BB_OCCUPIED,
                                BB_PIECES[0],
                                BB_PIECES[1],
                                BB_PIECES[2],
                                BB_PIECES[3],
                                BB_PIECES[4],
                                BB_PIECES[5]};
        return bbs;
    }
};
}  // namespace vorpal_node