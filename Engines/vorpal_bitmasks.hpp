#define INF 10000000

#define U64 unsigned long long

namespace vorpal_bitmasks
{
    class MaskSet
    {
    public:
        U64 MASK[64] = {};
        U64 PAWN_ATTACKS[64][2] = {};

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
            for (int i = 0; i < 64; i++)
            {
                //MASK[i] = 1LL << i; // masks
            }
        }
    };
}; // namespace vorpal_bitmasks
