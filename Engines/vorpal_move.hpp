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

        Move(
            int from_square = 0, 
            int to_square = 0, 
            int piece = 1, 
            bool color = 0, 
            int cPiece = 12) : 
            from_square(from_square), 
            to_square(to_square), 
            piece(piece), 
            color(color), 
            cPiece(cPiece), 
            cColor(!color), 
            iscapture((cPiece == 12) ? false : true)
        {}
    };
}; // namespace vorpal_move