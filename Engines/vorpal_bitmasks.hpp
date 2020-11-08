#include "vorpal_helpers.hpp"

#define INF 10000000

#define U64 unsigned long long

using namespace vorpal_helpers;

namespace vorpal_bitmasks
{
    class MaskSet
    {
    public:
        U64 MASK[64] = {};
        U64 PAWN_MOVES[64][2];
        U64 PAWN_ATTACKS[64][2];
        U64 KNIGHT_ATTACKS[8][64];
        U64 KING_ATTACKS[8][64];
        U64 RAYS[8][64];

        MaskSet()
        {
            for (int i = 0; i < 64; i++) // single-bit masks
            {
                MASK[i] = 1LL << i;
            }

            for (int i = 0; i < 64; i++) // pawn move masks
            {
                if (i < 8 || i > 55)
                {
                    PAWN_MOVES[i][0] = 0;
                    PAWN_MOVES[i][1] = 0;
                }
                else if (row(i) == 1 || row(i) == 6)
                {
                    PAWN_MOVES[i][0] |= 1LL << i + 8;
                    PAWN_MOVES[i][0] |= 1LL << i + 16;
                    PAWN_MOVES[i][1] |= 1LL << i - 8;
                    PAWN_MOVES[i][1] |= 1LL << i - 16;
                }
                else
                {
                    PAWN_MOVES[i][0] |= 1LL << i + 8;
                    PAWN_MOVES[i][1] |= 1LL << i - 8;
                }
            }

            for (int i = 0; i < 64; i++) // pawn attack masks
            {
                if (i < 8 || i > 55)
                {
                    PAWN_ATTACKS[i][0] = 0;
                    PAWN_ATTACKS[i][1] = 0;
                }
                else if (i % 8 == 7 || i % 8 == 0)
                {
                    if (i % 8 == 7)
                    {
                        PAWN_ATTACKS[i][0] |= 1LL << i + 7;
                        PAWN_ATTACKS[i][1] |= 1LL << i - 9;
                    }
                    else
                    {
                        PAWN_ATTACKS[i][0] |= 1LL << i + 9;
                        PAWN_ATTACKS[i][1] |= 1LL << i - 7;
                    }
                }
                else
                {
                    PAWN_ATTACKS[i][0] |= 1LL << i + 9;
                    PAWN_ATTACKS[i][0] |= 1LL << i + 7;
                    PAWN_ATTACKS[i][1] |= 1LL << i - 9;
                    PAWN_ATTACKS[i][1] |= 1LL << i - 7;
                }
            }

            for (int i = 0; i < 64; i++) // ray masks
            {
                for (int dir = 0; dir < 8; dir++)
                {
                    RAYS[dir][i] = ray_bitmask_generator(i, dir);
                }
            }

            for (int i = 0; i < 64; i++)
            {
                for (int dir = 0; dir < 8; dir++)
                {
                    KNIGHT_ATTACKS[dir][i] = knight_move_generator(i, dir);
                }
            }
            
            for (int i = 0; i < 64; i++)
            {
                for (int dir = 0; dir < 8; dir++)
                {
                    KING_ATTACKS[dir][i] = king_move_generator(i, dir);
                }
            }
        }

        auto union_bitmask(int piece, int index) -> U64
        {
            switch (piece)
            {
            case KNIGHT:
                return KNIGHT_ATTACKS[0][index] | KNIGHT_ATTACKS[1][index] | KNIGHT_ATTACKS[2][index] | KNIGHT_ATTACKS[3][index] | KNIGHT_ATTACKS[4][index] | KNIGHT_ATTACKS[5][index] | KNIGHT_ATTACKS[6][index] | KNIGHT_ATTACKS[7][index];
                break;
            case KING:
                return KING_ATTACKS[0][index] | KING_ATTACKS[1][index] | KING_ATTACKS[2][index] | KING_ATTACKS[3][index] | KING_ATTACKS[4][index] | KING_ATTACKS[5][index] | KING_ATTACKS[6][index] | KING_ATTACKS[7][index];
                break;
            case BISHOP:
                return RAYS[NORTH_EAST][index] | RAYS[NORTH_WEST][index] | RAYS[SOUTH_EAST][index] | RAYS[SOUTH_WEST][index];
                break;
            case ROOK:
                return RAYS[NORTH][index] | RAYS[WEST][index] | RAYS[EAST][index] | RAYS[SOUTH][index];
                break;
            case QUEEN:
                return RAYS[NORTH_EAST][index] | RAYS[NORTH_WEST][index] | RAYS[SOUTH_EAST][index] | RAYS[SOUTH_WEST][index] | RAYS[NORTH][index] | RAYS[WEST][index] | RAYS[EAST][index] | RAYS[SOUTH][index];
                break;
            default:
                return 0;
                break;
            }
        }
    };
}; // namespace vorpal_bitmasks
