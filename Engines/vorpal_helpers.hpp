#include <iostream>
#include <string>
#include <cassert>

#include "vorpal_move.hpp"

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
        PAWN,
        KNIGHT,
        BISHOP,
        ROOK,
        QUEEN,
        KING,
        NO_PIECE
    };

    enum e_dirs
    {
        SOUTH_EAST,
        SOUTH,
        SOUTH_WEST,
        EAST,
        NORTH_WEST,
        NORTH,
        NORTH_EAST,
        WEST
    };

    enum e_color
    {
        WHITE,
        BLACK,
        NO_COLOR
    };

    struct s_searchTracker
    {
        bool myside;
        U8 depth;
        int history[128][128];
        vorpal_move::Move killers[1024][2];
        U64 nodes;
        S32 movetime;
        U64 q_nodes;
        unsigned long starttime;
    };

    //FUNCTIONS AND OVERLOADS

    std::ostream &operator<<(std::ostream &os, const vorpal_move::Move &obj)
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

    auto move_to_string(vorpal_move::Move move) -> std::string
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

    auto string(std::vector<vorpal_move::Move> v) -> std::string
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

    auto bitscan_forward(U64 bitboard) -> int // this function is just magic copied from here https://www.chessprogramming.org/BitScan#Divide_and_Conquer
    {
        unsigned int lsb;
        assert(bitboard != 0);
        bitboard &= -bitboard; // LS1B-Isolation
        lsb = (unsigned)bitboard | (unsigned)(bitboard >> 32);
        return (((((((((((unsigned)(bitboard >> 32) != 0) * 2) + ((lsb & 0xffff0000) != 0)) * 2) + ((lsb & 0xff00ff00) != 0)) * 2) + ((lsb & 0xf0f0f0f0) != 0)) * 2) + ((lsb & 0xcccccccc) != 0)) * 2) + ((lsb & 0xaaaaaaaa) != 0);
    }

    int bitscan_reverse(U64 bitboard) // this function is just magic copied from here https://www.chessprogramming.org/BitScan#Double_conversion
    {
        union
        {
            double d;
            struct
            {
                unsigned int mantissal : 32;
                unsigned int mantissah : 20;
                unsigned int exponent : 11;
                unsigned int sign : 1;
            };
        } ud;
        ud.d = (double)(bitboard & ~(bitboard >> 32)); // avoid rounding error
        return ud.exponent - 1023;
    }

    auto row(int index) -> int
    {
        return index / 8;
    }

    auto col(int index) -> int
    {
        return index % 8;
    }

    auto index(int row, int col) -> int
    {
        if (row > 7 || col > 7 || row < 0 || col < 0)
        {
            return 64;
        }
        return row * 8 + col;
    }

    auto set_bit(int index, U64 bitboard) -> U64
    {
        //return bitboard | M.MASK[index];
        return bitboard | 1LL << index;
    }

    auto row_bitmask_generator(int row) -> U64
    {
        U64 out = 0;
        for (int i = 0; i < 8; i++)
        {
            out = set_bit(index(row, i), out);
        }
        return out;
    }

    auto col_bitmask_generator(int col) -> U64
    {
        U64 out = 0;
        for (int i = 0; i < 8; i++)
        {
            out = set_bit(index(i, col), out);
        }
        return out;
    }

    auto ray_bitmask_generator(int square, int dir) -> U64 // slow as hell
    {
        int r = row(square);
        int c = col(square);
        int i = 1;
        U64 outputMask = 0;
        if (dir == SOUTH_EAST)
        {
            while (index(r + i, c + i) >= 0 && index(r + i, c + i) <= 63)
            {
                outputMask = set_bit(index(r + i, c + i), outputMask);
                i++;
            }
        }
        else if (dir == NORTH_WEST)
        {
            while (index(r - i, c - i) >= 0 && index(r - i, c - i) <= 63)
            {
                outputMask = set_bit(index(r - i, c - i), outputMask);
                i++;
            }
        }
        else if (dir == SOUTH_WEST)
        {
            while (index(r + i, c - i) >= 0 && index(r + i, c - i) <= 63)
            {
                outputMask = set_bit(index(r + i, c - i), outputMask);
                i++;
            }
        }
        else if (dir == NORTH_EAST)
        {
            while (index(r - i, c + i) >= 0 && index(r - i, c + i) <= 63)
            {
                outputMask = set_bit(index(r - i, c + i), outputMask);
                i++;
            }
        }
        else if (dir == NORTH)
        {
            while (index(r - i, c) >= 0 && index(r - i, c) <= 63)
            {
                outputMask = set_bit(index(r - i, c), outputMask);
                i++;
            }
        }
        else if (dir == SOUTH)
        {
            while (index(r + i, c) >= 0 && index(r + i, c) <= 63)
            {
                outputMask = set_bit(index(r + i, c), outputMask);
                i++;
            }
        }
        else if (dir == WEST)
        {
            while (index(r, c - i) >= 0 && index(r, c - i) <= 63)
            {
                outputMask = set_bit(index(r, c - i), outputMask);
                i++;
            }
        }
        else if (dir == EAST)
        {
            while (index(r, c + i) >= 0 && index(r, c + i) <= 63)
            {
                outputMask = set_bit(index(r, c + i), outputMask);
                i++;
            }
        }
        else
        {
            std::cout << "error in ray_bitmask_generator";
        }

        return outputMask;
    }

    auto knight_move_generator(int square, int dir) -> U64 // slow as hell
    {
        int offsets[] = {17, 15, 10, 6, -17, -15, -10, -6};
        int r = row(square);
        int c = col(square);
        U64 out = 0;
        if (square + offsets[dir] >= 0 && square + offsets[dir] <= 63)
        {
            out = 1LL << (square + offsets[dir]);
        }
        else
        {
            return 0;
        }
        
        if (r == 0 || r == 1)
        {
            out &= ~(row_bitmask_generator(7) | row_bitmask_generator(6));
        }
        else if (r == 7 || r == 6)
        {
            out &= ~(row_bitmask_generator(0) | row_bitmask_generator(1));
        }
        if (c == 0 || c == 1)
        {
            out &= ~(col_bitmask_generator(7) | col_bitmask_generator(6));
        }
        else if (c == 7 || c == 6)
        {
            out &= ~(col_bitmask_generator(0) | col_bitmask_generator(1));
        }
        return out;
    }

    auto king_move_generator(int square, int dir) -> U64 // slow as hell
    {
        int offsets[] = {9, 8, 7, 1, -9, -8, -7, -1};
        int r = row(square);
        int c = col(square);
        U64 out = 0;
        if (square + offsets[dir] >= 0 && square + offsets[dir] <= 63)
        {
            out = 1LL << (square + offsets[dir]);
        }
        else
        {
            return 0;
        }

        if (r == 0 || r == 1)
        {
            out &= ~(row_bitmask_generator(7) | row_bitmask_generator(6));
        }
        else if (r == 7 || r == 6)
        {
            out &= ~(row_bitmask_generator(0) | row_bitmask_generator(1));
        }
        if (c == 0 || c == 1)
        {
            out &= ~(col_bitmask_generator(7) | col_bitmask_generator(6));
        }
        else if (c == 7 || c == 6)
        {
            out &= ~(col_bitmask_generator(0) | col_bitmask_generator(1));
        }
        return out;
    }

    
}; // namespace vorpal_helpers
