#include "vorpal_helpers.hpp"

#define INF 10000000

#define U64 unsigned long long

namespace vorpal_bitmasks
{
    class MaskSet
    {
    public:
        U64 MASK[64] = {};
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
                    RAYS[dir][i] = vorpal_helpers::ray_bitmask_generator(i, dir);
                }
            }

            for (int i = 0; i < 64; i++)
            {
                for (int dir = 0; dir < 8; dir++)
                {
                    KNIGHT_ATTACKS[dir][i] = vorpal_helpers::knight_move_generator(i, dir);
                }
            }
            
            for (int i = 0; i < 64; i++)
            {
                for (int dir = 0; dir < 8; dir++)
                {
                    KING_ATTACKS[dir][i] = vorpal_helpers::king_move_generator(i, dir);
                }
            }
        }

        auto union_bitmask(int piece, int index) -> U64
        {
            switch (piece)
            {
            case vorpal_helpers::KNIGHT:
                return KNIGHT_ATTACKS[0][index] | KNIGHT_ATTACKS[1][index] | KNIGHT_ATTACKS[2][index] | KNIGHT_ATTACKS[3][index] | KNIGHT_ATTACKS[4][index] | KNIGHT_ATTACKS[5][index] | KNIGHT_ATTACKS[6][index] | KNIGHT_ATTACKS[7][index];
                break;
            case vorpal_helpers::KING:
                return KING_ATTACKS[0][index] | KING_ATTACKS[1][index] | KING_ATTACKS[2][index] | KING_ATTACKS[3][index] | KING_ATTACKS[4][index] | KING_ATTACKS[5][index] | KING_ATTACKS[6][index] | KING_ATTACKS[7][index];
                break;
            case vorpal_helpers::BISHOP:
                return RAYS[vorpal_helpers::NORTH_EAST][index] | RAYS[vorpal_helpers::NORTH_WEST][index] | RAYS[vorpal_helpers::SOUTH_EAST][index] | RAYS[vorpal_helpers::SOUTH_WEST][index];
                break;
            case vorpal_helpers::ROOK:
                return RAYS[vorpal_helpers::NORTH][index] | RAYS[vorpal_helpers::WEST][index] | RAYS[vorpal_helpers::EAST][index] | RAYS[vorpal_helpers::SOUTH][index];
                break;
            case vorpal_helpers::QUEEN:
                return RAYS[vorpal_helpers::NORTH_EAST][index] | RAYS[vorpal_helpers::NORTH_WEST][index] | RAYS[vorpal_helpers::SOUTH_EAST][index] | RAYS[vorpal_helpers::SOUTH_WEST][index] | RAYS[vorpal_helpers::NORTH][index] | RAYS[vorpal_helpers::WEST][index] | RAYS[vorpal_helpers::EAST][index] | RAYS[vorpal_helpers::SOUTH][index];
                break;
            default:
                return 0;
                break;
            }
        }
    };
}; // namespace vorpal_bitmasks
