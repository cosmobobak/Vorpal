#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <bitset>

#define INF 10000000

#define U64 unsigned long long
#define U32 unsigned __int32
#define U16 unsigned __int16
#define U8 unsigned __int8
#define S64 signed __int64
#define S32 signed __int32
#define S16 signed __int16
#define S8 signed __int8

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

//CLASS DEFINITIONS BEGIN

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
    U64 MASK[64] = {};
    U64 PAWN_ATTACKS[64][2] = {};

    std::vector<Move> stack;

    bool turn = 1;

    Board()
    {
        for (int i = 0; i < 64; i++)
        {
            MASK[i] = 1LL << i; //MASKS
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
        return (bb & MASK[squareNum]);
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
        U64 mask = MASK[squareNum];

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
        U64 mask = MASK[squareNum];
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
        U64 fromBB = MASK[move->from_square];
        U64 toBB = MASK[move->to_square];
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

    //auto _attackers_mask(bool color, int square, U64 occupied) -> U64 //this finds the pieces attacking a specific square
    /*{
        U64 rank_pieces = BB_RANK_MASKS[square] & occupied;
        U64 file_pieces = BB_FILE_MASKS[square] & occupied;
        U64 diag_pieces = BB_DIAG_MASKS[square] & occupied;

        U64 queens_and_rooks = self.queens | self.rooks;
        U64 queens_and_bishops = self.queens | self.bishops;

        U64 attackers = ((BB_KING_ATTACKS[square] & self.kings) |
                         (BB_KNIGHT_ATTACKS[square] & self.knights) |
                         (BB_RANK_ATTACKS[square][rank_pieces] & queens_and_rooks) |
                         (BB_FILE_ATTACKS[square][file_pieces] & queens_and_rooks) |
                         (BB_DIAG_ATTACKS[square][diag_pieces] & queens_and_bishops) |
                         (BB_PAWN_ATTACKS[!color][square] & self.pawns));

        return attackers & self.occupied_co[color];
    }*/

    //auto attacks_mask(int piece) -> U64 //unfinished, only pawns in progress
    /*{
        U64 mask;
        if (piece == 0)
        {
        }
                return mask;
    }*/

    auto pseudo_legal_moves() -> std::vector<Move> //unfinished
    {
        U64 our_pieces = BB_COLORS[turn];
        U64 our_pawns = BB_PIECES[0] & our_pieces;
        U64 targets = BB_COLORS[(turn + 1) % 2];
        std::vector<Move> moveset;
        for (int sq = 0; sq < 64; sq++) //pawn-forward moves
        {
            if (get_square(our_pawns, sq) && !get_square(targets, sq))
            {
                moveset.push_back(Move(sq, sq + 8 * mod(), 0 + 6 * turn, false, 12));
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
        nodes++;
        int m = node.mod();
        int rating = 0;
        if (node.is_checkmate())
        {
            return 1000000000 * (depth + 1) * m;
        }
        if (node.can_claim_fifty_moves())
        {
            rating = -contempt * m;
        }

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
    //Vorpal engine;
    Board init;
    Board board;

    return 0;
}

//TODO: pawn attacks generator (ep_square?)
//
//MOVE GENERATOR
//RULES??