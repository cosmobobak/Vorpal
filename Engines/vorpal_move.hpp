#define U64 unsigned long long
#define U32 unsigned __int32
#define U16 unsigned __int16
#define U8 unsigned __int8
#define S64 signed __int64
#define S32 signed __int32
#define S16 signed __int16
#define S8 signed __int8

namespace vorpal_move
{
    class Move
    {
    public:
        int from_square;
        int to_square;
        int piece;
        bool color;
        int cPiece;
        int cColor;
        bool iscapture;

        Move(int f = 0, int t = 0, int p = 1, bool c = 0, int cp = 12)
        {
            from_square = f;
            to_square = t;
            piece = p;
            color = c;
            cPiece = cp;
            cColor = !c;
            if (cp == 12)
            {
                iscapture = false;
            }
            else
            {
                iscapture = true;
            }
        }
    };
}; // namespace vorpal_move