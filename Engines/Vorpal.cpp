#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <bit>
#include <bitset>
#include <cstdint>
#include <initializer_list>

#include "vorpal_helpers.hpp"
#include "vorpal_bitmasks.hpp"

using namespace vorpal_helpers;
using namespace vorpal_bitmasks;

#define INF 10000000

#define U64 unsigned long long
#define U32 unsigned __int32
#define U16 unsigned __int16
#define U8 unsigned __int8
#define S64 signed __int64
#define S32 signed __int32
#define S16 signed __int16
#define S8 signed __int8

//CLASS DEFINITIONS BEGIN

MaskSet M;

class Board
{
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
    const U64 BB_ALL = 0xffffffffffffffff;

    std::vector<Move> stack;

    bool turn = 1;

    Board()
    {
    }

    auto mod() -> int
    {
        if (turn)
        {
            return -1;
        }
        else
        {
            return 1;
        }
    }

    void show()
    {
        for (int i = 0; i < 64; i++)
        {
            std::cout << pieces[colored_piece_type_at(i)];
            std::cout << " ";
            if (i % 8 == 7)
            {
                std::cout << "\n";
            }
        }
    }

    auto get_square(U64 bb, int squareNum) -> bool
    {
        return (bb & M.MASK[squareNum]);
    }

    auto color_at(int i) -> int
    {
        if (get_square(BB_COLORS[0], i))
        {
            return 0;
        }
        else if (get_square(BB_COLORS[1], i))
        {
            return 1;
        }
        else
        {
            return 2;
        }
    }

    auto move_from_uci(std::string notation) -> Move
    {
        int f, t, p, cp;
        bool c;
        f = square_from_an(notation.substr(0, 2));
        t = square_from_an(notation.substr(2, 2));
        p = colored_piece_type_at(f);
        c = color_at(f);
        cp = colored_piece_type_at(t);
        return Move(f, t, p, c, cp);
    }

    auto piece_type_at(int squareNum) -> int
    {
        //Gets the piece type at the given square.
        U64 mask = M.MASK[squareNum];

        if (!(BB_OCCUPIED & mask))
        {
            return 12; // Early return
        }
        else if (BB_PIECES[0] & mask)
        {
            return 0;
        }
        else if (BB_PIECES[1] & mask)
        {
            return 1;
        }
        else if (BB_PIECES[2] & mask)
        {
            return 2;
        }
        else if (BB_PIECES[3] & mask)
        {
            return 3;
        }
        else if (BB_PIECES[4] & mask)
        {
            return 4;
        }
        else if (BB_PIECES[5] & mask)
        {
            return 5;
        }
        else
        {
            return 42;
        }
    }

    auto colored_piece_type_at(int squareNum) -> int
    {
        //Gets the piece type at the given square.
        U64 mask = M.MASK[squareNum];
        int mod;
        if (BB_COLORS[0] & mask)
        {
            mod = 0;
        }
        else
        {
            mod = 6;
        }

        if (!(BB_OCCUPIED & mask))
        {
            return 12; // Early return
        }
        else if (BB_PIECES[0] & mask)
        {
            return 0 + mod;
        }
        else if (BB_PIECES[1] & mask)
        {
            return 1 + mod;
        }
        else if (BB_PIECES[2] & mask)
        {
            return 2 + mod;
        }
        else if (BB_PIECES[3] & mask)
        {
            return 3 + mod;
        }
        else if (BB_PIECES[4] & mask)
        {
            return 4 + mod;
        }
        else if (BB_PIECES[5] & mask)
        {
            return 5 + mod;
        }
        else
        {
            return 42;
        }
    }

    auto is_checkmate() -> bool //unfinished
    {
        return false;
    }

    auto can_claim_fifty_moves() -> bool //unfinished
    {
        return false;
    }

    auto is_insufficient_material() -> bool //unfinished
    {
        return false;
    }

    auto is_threefold() -> bool //unfinished
    {
        return false;
    }

    auto is_draw() -> bool //unfinished
    {
        return can_claim_fifty_moves() || is_insufficient_material() || is_threefold();
    }

    auto is_game_over() -> bool
    {
        return is_checkmate() || is_draw();
    }

    void make(Move *move) //adapted from chessprogrammingwiki
    {
        std::cout << *move << std::endl;
        U64 fromBB = M.MASK[move->from_square];
        U64 toBB = M.MASK[move->to_square];
        U64 fromToBB = fromBB ^ toBB; // |+
        std::cout << std::bitset<64>(fromToBB) << ' ' << move->piece << std::endl;
        BB_PIECES[move->piece % 6] ^= fromToBB; // update piece bitboard
        BB_COLORS[move->color] ^= fromToBB;     // update white or black color bitboard
        if (move->iscapture)
        {
            BB_PIECES[move->cPiece] ^= toBB; // reset the captured piece
            BB_PIECES[move->cColor] ^= toBB; // update color bitboard by captured piece
        }
        BB_OCCUPIED ^= fromToBB; // update occupied ...
        BB_EMPTY ^= fromToBB;    // ... and empty bitboard
    }

    void unmake(Move *move)
    {
        //TODO
    }

    void play(Move *edge)
    {
        make(edge);
        stack.push_back(*edge);
    }

    U64 get_bishop_moves_c(int square, U64 blockers)
    {
        U64 attacks = 0ULL;

        // North West
        attacks |= RAYS[NORTH_WEST][square];
        if (RAYS[NORTH_WEST][square] & blockers)
        {
            int blockerIndex = bitscanForward(RAYS[NORTH_WEST][square] & blockers);
            attacks &= ~RAYS[NORTH_WEST][blockerIndex];
        }

        // North East
        attacks |= RAYS[NORTH_EAST][square];
        if (RAYS[NORTH_EAST][square] & blockers)
        {
            int blockerIndex = bitscanForward(RAYS[NORTH_EAST][square] & blockers);
            attacks &= ~RAYS[NORTH_EAST][blockerIndex];
        }

        // South East
        attacks |= RAYS[SOUTH_EAST][square];
        if (RAYS[SOUTH_EAST][square] & blockers)
        {
            int blockerIndex = bitscanReverse(RAYS[SOUTH_EAST][square] & blockers);
            attacks &= ~RAYS[SOUTH_EAST][blockerIndex];
        }

        // South West
        attacks |= RAYS[SOUTH_WEST][square];
        if (RAYS[SOUTH_WEST][square] & blockers)
        {
            int blockerIndex = bitscanReverse(RAYS[SOUTH_WEST][square] & blockers);
            attacks &= ~RAYS[SOUTH_WEST][blockerIndex];
        }

        return attacks;
    }

    auto pseudo_legal_moves() -> std::vector<Move> //unfinished
    {
        U64 our_pieces = BB_COLORS[turn];
        U64 our_pawns = BB_PIECES[0] & our_pieces;
        U64 targets = BB_COLORS[(turn + 1) % 2];
        std::vector<Move> moveset;
        for (int sq = 0; sq < 64; sq++) //pawn-forward moves
        {
            if (get_square(our_pawns, sq))
            {
                if (!get_square(targets, sq))
                {
                    moveset.push_back(Move(sq, sq + 8 * mod(), 0 + 6 * turn, false, 12));
                }
                if (M.PAWN_ATTACKS[sq][turn] & targets)
                {
                    //perform some sort of check for left or right pawn take
                }
            }
        }
        return moveset;
    }

    auto legal_moves() -> std::vector<Move> //unfinished
    {
        return pseudo_legal_moves();
    }
};

class Vorpal
{
public:
    int nodes = 0;
    Board node = Board();
    //int tableSize = SOME PRIME NUMBER;
    //Entry hashtable[tableSize];
    int timeLimit = 1;
    //int startTime = time();
    //include the advanced time control later
    bool human = false;
    bool useBook = false;
    bool inBook = true;
    bool variedBook = false;
    int contempt = 3000;
    bool oddeven = true;
    //Move best = Move();
    int pieceValue[6] = {
        1000,
        3200,
        3330,
        5100,
        8800,
        0,
    };
    int pieceSquareTable[12][64] = {
        {0, 0, 0, 0, 0, 0, 0, 0, 50, 50, 50, 50, 50, 50, 50, 50, 10, 10, 20, 30, 30, 20, 10, 10, 5, 5, 10, 25, 25, 10, 5, 5, 0, 0, 0, 20, 20, 0, 0, 0, 5, -5, -10, 0, 0, -10, -5, 5, 5, 10, 10, -20, -20, 10, 10, 5, 0, 0, 0, 0, 0, 0, 0, 0},
        {-50, -40, -30, -30, -30, -30, -40, -50, -40, -20, 0, 0, 0, 0, -20, -40, -30, 0, 10, 15, 15, 10, 0, -30, -30, 5, 15, 20, 20, 15, 5, -30, -30, 0, 15, 20, 20, 15, 0, -30, -30, 5, 10, 15, 15, 10, 5, -30, -40, -20, 0, 5, 5, 0, -20, -40, -50, -40, -30, -30, -30, -30, -40, -50},
        {-20, -10, -10, -10, -10, -10, -10, -20, -10, 0, 0, 0, 0, 0, 0, -10, -10, 0, 5, 10, 10, 5, 0, -10, -10, 5, 5, 10, 10, 5, 5, -10, -10, 0, 10, 10, 10, 10, 0, -10, -10, 10, 10, 10, 10, 10, 10, -10, -10, 5, 0, 0, 0, 0, 5, -10, -20, -10, -10, -10, -10, -10, -10, -20},
        {0, 0, 0, 0, 0, 0, 0, 0, 5, 10, 10, 10, 10, 10, 10, 5, -5, 0, 0, 0, 0, 0, 0, -5, -5, 0, 0, 0, 0, 0, 0, -5, -5, 0, 0, 0, 0, 0, 0, -5, -5, 0, 0, 0, 0, 0, 0, -5, -5, 0, 0, 0, 0, 0, 0, -5, 0, 0, 0, 5, 5, 0, 0, 0},
        {-20, -10, -10, -5, -5, -10, -10, -20, -10, 0, 0, 0, 0, 0, 0, -10, -10, 0, 5, 5, 5, 5, 0, -10, -5, 0, 5, 5, 5, 5, 0, -5, 0, 0, 5, 5, 5, 5, 0, -5, -10, 5, 5, 5, 5, 5, 0, -10, -10, 0, 5, 0, 0, 0, 0, -10, -20, -10, -10, -5, -5, -10, -10, -20},
        {-30, -40, -40, -50, -50, -40, -40, -30, -30, -40, -40, -50, -50, -40, -40, -30, -30, -40, -40, -50, -50, -40, -40, -30, -30, -40, -40, -50, -50, -40, -40, -30, -20, -30, -30, -40, -40, -30, -30, -20, -10, -20, -20, -20, -20, -20, -20, -10, 20, 20, 0, 0, 0, 0, 20, 20, 20, 30, 10, 0, 0, 10, 30, 20},
        {0, 0, 0, 0, 0, 0, 0, 0, 5, 10, 10, -20, -20, 10, 10, 5, 5, -5, -10, 0, 0, -10, -5, 5, 0, 0, 0, 20, 20, 0, 0, 0, 5, 5, 10, 25, 25, 10, 5, 5, 10, 10, 20, 30, 30, 20, 10, 10, 50, 50, 50, 50, 50, 50, 50, 50, 0, 0, 0, 0, 0, 0, 0, 0},
        {-50, -40, -30, -30, -30, -30, -40, -50, -40, -20, 0, 5, 5, 0, -20, -40, -30, 5, 10, 15, 15, 10, 5, -30, -30, 0, 15, 20, 20, 15, 0, -30, -30, 5, 15, 20, 20, 15, 5, -30, -30, 0, 10, 15, 15, 10, 0, -30, -40, -20, 0, 0, 0, 0, -20, -40, -50, -40, -30, -30, -30, -30, -40, -50},
        {-20, -10, -10, -10, -10, -10, -10, -20, -10, 5, 0, 0, 0, 0, 5, -10, -10, 10, 10, 10, 10, 10, 10, -10, -10, 0, 10, 10, 10, 10, 0, -10, -10, 5, 5, 10, 10, 5, 5, -10, -10, 0, 5, 10, 10, 5, 0, -10, -10, 0, 0, 0, 0, 0, 0, -10, -20, -10, -10, -10, -10, -10, -10, -20},
        {0, 0, 0, 5, 5, 0, 0, 0, -5, 0, 0, 0, 0, 0, 0, -5, -5, 0, 0, 0, 0, 0, 0, -5, -5, 0, 0, 0, 0, 0, 0, -5, -5, 0, 0, 0, 0, 0, 0, -5, -5, 0, 0, 0, 0, 0, 0, -5, 5, 10, 10, 10, 10, 10, 10, 5, 0, 0, 0, 0, 0, 0, 0, 0},
        {-20, -10, -10, -5, -5, -10, -10, -20, -10, 0, 0, 0, 0, 5, 0, -10, -10, 0, 5, 5, 5, 5, 5, -10, -5, 0, 5, 5, 5, 5, 0, 0, -5, 0, 5, 5, 5, 5, 0, -5, -10, 0, 5, 5, 5, 5, 0, -10, -10, 0, 0, 0, 0, 0, 0, -10, -20, -10, -10, -5, -5, -10, -10, -20},
        {20, 30, 10, 0, 0, 10, 30, 20, 20, 20, 0, 0, 0, 0, 20, 20, -10, -20, -20, -20, -20, -20, -20, -10, -20, -30, -30, -40, -40, -30, -30, -20, -30, -40, -40, -50, -50, -40, -40, -30, -30, -40, -40, -50, -50, -40, -40, -30, -30, -40, -40, -50, -50, -40, -40, -30, -30, -40, -40, -50, -50, -40, -40, -30},
    };

    Vorpal()
    {
        int testVar = 0;
    }

    auto evaluate(int depth) -> int
    {
        nodes++;            //increment nodes evaluated
        int m = node.mod(); //get modifier for turn, either -1 or 1
        int rating;         //init rating

        if (node.is_checkmate()) //if checkmate, multiply by the depth to incentivise quicker mating
        {
            return 1000000000 * (depth + 1) * m;
        }
        if (node.can_claim_fifty_moves()) //treat draws as initialising rating with the contempt factor
        {
            rating = -contempt * m;
        }

        rating += __builtin_popcount(node.BB_PIECES[0] & node.BB_COLORS[0]);
        rating += __builtin_popcount(node.BB_PIECES[1] & node.BB_COLORS[0]);
        rating += __builtin_popcount(node.BB_PIECES[2] & node.BB_COLORS[0]);
        rating += __builtin_popcount(node.BB_PIECES[3] & node.BB_COLORS[0]);
        rating += __builtin_popcount(node.BB_PIECES[4] & node.BB_COLORS[0]);
        rating -= __builtin_popcount(node.BB_PIECES[0] & node.BB_COLORS[1]);
        rating -= __builtin_popcount(node.BB_PIECES[1] & node.BB_COLORS[1]);
        rating -= __builtin_popcount(node.BB_PIECES[2] & node.BB_COLORS[1]);
        rating -= __builtin_popcount(node.BB_PIECES[3] & node.BB_COLORS[1]);
        rating -= __builtin_popcount(node.BB_PIECES[4] & node.BB_COLORS[1]);

        return rating;
    }

    auto negamax(int depth, int color, int a = -200000, int b = 200000) -> int
    {
        if (node.is_game_over() || depth < 1)
        {
            return evaluate(depth) * color;
        }
        int score;
        for (auto &&i : node.legal_moves())
        {
            node.make(&i);
            score = -negamax(depth - 1, -color, -b, -a);
            node.unmake(&i);

            //a = max(a, score)
            //alphabeta cutoff
        }
        return a;
    }
};

auto main() -> int
{
    Vorpal engine;
    Move arr[] = {engine.node.move_from_uci("e2e7")};
    engine.node.show();
    std::cout << engine.evaluate(1) << std::endl;
    engine.node.play(arr);
    engine.node.show();
    std::cout << engine.evaluate(1) << std::endl;
    return 0;
}

//TODO: pawn attacks generator (ep_square?)
//
//MOVE GENERATOR
//RULES??