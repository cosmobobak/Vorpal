#include <iostream>
#include <string>

#define INF 10000000

#define U64 unsigned long long
#define U32 unsigned __int32
#define U16 unsigned __int16
#define U8 unsigned __int8
#define S64 signed __int64
#define S32 signed __int32
#define S16 signed __int16
#define S8 signed __int8

namespace vorpal_helpers
{

    char pieces[13] = {'p', 'n', 'b', 'r', 'q', 'k', 'P', 'N', 'B', 'R', 'Q', 'K', '.'};
    std::string pieceNames[13] = {
        "blackPawn",
        "blackKnight",
        "blackBishop",
        "blackRook",
        "blackQueen",
        "blackKing",
        "whitePawn",
        "whiteKnight",
        "whiteBishop",
        "whiteRook",
        "whiteQueen",
        "whiteKing",
        "emptySquare"};

    enum e_piece
    {
        KING,
        QUEEN,
        ROOK,
        BISHOP,
        KNIGHT,
        PAWN,
        PIECE_EMPTY
    };

    enum e_color
    {
        WHITE,
        BLACK,
        COLOR_EMPTY
    };

    //enum e_square { A1, B1, C1, D1, E1, F1, G1, H1, A2, B2, C2, D2, E2, F2, G2, H2, A3, B3, C3, D3, E3, F3, G3, H3, A4, B4, C4, D4, E4, F4, G4, H4, A5, B5, C5, D5, E5, F5, G5, H5, A6, B6, C6, D6, E6, F6, G6, H6, A7, B7, C7, D7, E7, F7, G7, H7, A8, B8, C8, D8, E8, F8, G8, H8 };

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

    struct s_searchTracker
    {
        bool myside;
        U8 depth;
        int history[128][128];
        Move killers[1024][2];
        U64 nodes;
        S32 movetime;
        U64 q_nodes;
        unsigned long starttime;
    };

    //FUNCTIONS AND OVERLOADS

    std::ostream &operator<<(std::ostream &os, const Move &obj)
    {
        os << "Move " << pieceNames[obj.piece] << " C:" << obj.color << " " << obj.from_square << "->" << obj.to_square << " C?:" << obj.iscapture << " CP:" << obj.cPiece << " CC:" << obj.cColor;
        return os;
    }

    auto square_from_an(std::string an_square) -> int
    {
        int a = 7 - (an_square[0] - 97);
        int b = an_square[1] - '0' - 1;
        return 63 - (a + 8 * b);
    }

    template <class T>
    void print_array(T arr[], int len)
    {
        std::cout << "{ ";
        for (size_t i = 0; i < len; i++)
        {
            std::cout << arr[i] << ", ";
        }
        std::cout << "}" << std::endl;
    }

    auto move_to_string(Move move) -> std::string
    {
        std::string builder;
        builder.append("Move ");
        builder.append(pieceNames[move.piece]);
        builder.append(" C:");
        builder.append(std::to_string(move.color));
        builder.append(" ");
        builder.append(std::to_string(move.from_square));
        builder.append("->");
        builder.append(std::to_string(move.to_square));
        builder.append(" C?:");
        builder.append(std::to_string(move.iscapture));
        builder.append(" CP:");
        builder.append(std::to_string(move.cPiece));
        builder.append(" CC:");
        builder.append(std::to_string(move.cColor));
        return builder;
    }

    template <class T>
    auto string(std::vector<T> v) -> std::string
    {
        std::string builder;
        builder.append("{ ");
        for (auto &&i : v)
        {
            builder.append(std::to_string(i));
            builder.append(", ");
        }
        builder.append("}");
        return builder;
    }

    auto string(std::vector<Move> v) -> std::string
    {
        std::string builder;
        builder.append("{\n");
        for (auto &&i : v)
        {
            builder.append(move_to_string(i));
            builder.append(",\n");
        }
        builder.append("}");
        return builder;
    }

    auto string(U64 bitboard) -> std::string
    {
        std::string builder;
        for (int i = 0; i < 64; i++)
        {
            if (bitboard & (1LL << i))
            {
                builder.append("X");
            }
            else
            {
                builder.append(".");
            }
            builder.append(" ");
            if (i % 8 == 7)
            {
                builder.append("\n");
            }
        }
        return builder;
    }
}; // namespace vorpal_helpers
