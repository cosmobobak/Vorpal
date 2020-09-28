#include <iostream>
#include <algorithm>

/*class Square
{

}

class Move
{
public:
    int from_square;
    int to_square;
};*/

class Board
{
public:
    char pieces[13] = {'p', 'n', 'b', 'r', 'q', 'k', 'P', 'N', 'B', 'R', 'Q', 'K', '.'};
    unsigned long long BB_PIECES[6] = {
        0b0000000011111111000000000000000000000000000000001111111100000000,
        0b0100001000000000000000000000000000000000000000000000000001000010,
        0b0010010000000000000000000000000000000000000000000000000000100100,
        0b1000000100000000000000000000000000000000000000000000000010000001,
        0b0001000000000000000000000000000000000000000000000000000000001000,
        0b0000100000000000000000000000000000000000000000000000000000010000,
    };
    unsigned long long BB_OCCUPIED = 0b1111111111111111000000000000000000000000000000001111111111111111;
    unsigned long long BB_COLORS[2] = {
        0b1111111111111111000000000000000000000000000000000000000000000000,
        0b0000000000000000000000000000000000000000000000001111111111111111};
    unsigned long long MASK[64] = {};

    bool turn = 0;

    Board()
    {
        for (int i = 0; i < 64; i++)
        {
            MASK[i] = 1LL << i;
        }
    }

    void show()
    {
        for (int i = 0; i < 64; i++)
        {
            std::cout << pieces[piece_type_at(i)];
            std::cout << " ";
            if (i % 8 == 7)
            {
                std::cout << "\n";
            }
        }
    }

    bool get_square(unsigned long long bb, int squareNum)
    {
        return (bb & (1LL << squareNum));
    }

    int piece_type_at(int squareNum)
    {
        //Gets the piece type at the given square.
        unsigned long long mask = MASK[squareNum];
        int mod;
        if (BB_COLORS[1] & mask)
        {
            mod = 0;
        }else{
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
        else
        {
            return 5 + mod;
        }
    }

    bool is_checkmate()
    {
        return false;
    }

    bool can_claim_fifty_moves()
    {
        return false;
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
    int pieceValue[5] = {
        1000,
        3200,
        3330,
        5100,
        8800,
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

    int evaluate(int depth){
        nodes++;
        int mod;
        if (node.turn)
        {
            mod = 1;
        }else{
            mod = -1;
        }
        int rating = 0;
        if(node.is_checkmate()){
            return 1000000000*(depth+1)*mod;
        }
        if(node.can_claim_fifty_moves()){
            rating = -contempt*mod;
        }
        /*
        try:
            key, hash = self.pos_hash()
            reps = self.hashstack[hash]
        except KeyError:
            pass
        else:
            rating = -self.contempt*mod
        */

        //rating += sum([self.evaltable['p'][i] for i in self.node.pieces(chess.PAWN, chess.BLACK)])
        //rating -= sum([self.evaltable['P'][i] for i in self.node.pieces(chess.PAWN, chess.WHITE)])
        //rating += sum([self.evaltable['n'][i] for i in self.node.pieces(chess.KNIGHT, chess.BLACK)]) 
        //rating -= sum([self.evaltable['N'][i] for i in self.node.pieces(chess.KNIGHT, chess.WHITE)])
        //rating += sum([self.evaltable['b'][i] for i in self.node.pieces(chess.BISHOP, chess.BLACK)])
        //rating -= sum([self.evaltable['B'][i] for i in self.node.pieces(chess.BISHOP, chess.WHITE)])
        //rating += sum([self.evaltable['r'][i] for i in self.node.pieces(chess.ROOK, chess.BLACK)])
        //rating -= sum([self.evaltable['R'][i] for i in self.node.pieces(chess.ROOK, chess.WHITE)])
        //rating += sum([self.evaltable['q'][i] for i in self.node.pieces(chess.QUEEN, chess.BLACK)])
        //rating -= sum([self.evaltable['Q'][i] for i in self.node.pieces(chess.QUEEN, chess.WHITE)])
        //rating += sum([self.evaltable['k'][i] for i in self.node.pieces(chess.KING, chess.BLACK)])
        //rating -= sum([self.evaltable['K'][i] for i in self.node.pieces(chess.KING, chess.WHITE)])
        
        return rating;
    }

    int principal_variation_search(int depth, int colour, int a = -200000, int b = 200000)
    {
        
    }
};

int main()
{
    Vorpal engine;
    Board board;
    board.show();
    return 0;
}