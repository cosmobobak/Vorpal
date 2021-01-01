#include <iostream>
#include <vector>
#include <bitset>

auto popcount(short bb) -> short
{
    return std::bitset<16>(bb).count();
}

namespace Board
{
#define Move short

    class SubState
    {
    public:
        short position[2] = {0b000000000, 0b000000000};

        void reset()
        {
            position[0] = 0b000000000;
            position[1] = 0b000000000;
        }

        auto union_bb() -> short
        {
            return position[0] | position[1];
        }

        void play(short i, short turn)
        {
            // n ^ (1 << k) is a binary XOR where you flip the kth bit of n
            if (turn == 1)
            {
                position[0] |= (1 << i);
            }
            else
            {
                position[1] |= (1 << i);
            }
        }

        void unplay(short prevmove, short turn) // do not unplay on root
        {
            if (turn == 1)
            {
                position[1] &= ~(1 << prevmove);
            }
            else
            {
                position[0] &= ~(1 << prevmove);
            }
        }

        auto pos_filled(short i) -> bool
        {
            return position[0] & (1 << i) || position[1] & (1 << i);
        }

        auto player_at(short i) -> bool //only valid to use if pos_filled() returns true, true = x, false = y
        {
            return (position[0] & (1 << i));
        }

        auto is_full() -> bool
        {
            return position[0] + position[1] == 0b111111111;
        }

        auto evaluate() -> short
        {
            // check first diagonal
            if (pos_filled(0) && pos_filled(4) && pos_filled(8))
            {
                if (player_at(0) == player_at(4) && player_at(4) == player_at(8))
                {
                    if (player_at(0))
                        return 1;
                    else
                        return -1;
                }
            }
            // check second diagonal
            if (pos_filled(2) && pos_filled(4) && pos_filled(6))
            {
                if (player_at(2) == player_at(4) && player_at(4) == player_at(6))
                {
                    if (player_at(2))
                        return 1;
                    else
                        return -1;
                }
            }
            // check rows
            for (short i = 0; i < 3; i++)
            {
                if (pos_filled(i * 3) && pos_filled(i * 3 + 1) && pos_filled(i * 3 + 2))
                {
                    if (player_at(i * 3) == player_at(i * 3 + 1) && player_at(i * 3 + 1) == player_at(i * 3 + 2))
                    {
                        if (player_at(i * 3))
                            return 1;
                        else
                            return -1;
                    }
                }
            }
            // check columns
            for (short i = 0; i < 3; i++)
            {
                if (pos_filled(i) && pos_filled(i + 3) && pos_filled(i + 6))
                {
                    if (player_at(i) == player_at(i + 3) && player_at(i + 3) == player_at(i + 6))
                    {
                        if (player_at(i))
                            return 1;
                        else
                            return -1;
                    }
                }
            }
            return 0;
        }

        void show()
        {
            for (int x = 0; x < 3; x++)
            {
                for (int y = 0; y < 3; y++)
                {
                    if (pos_filled(x * 3 + y))
                    {
                        if (player_at(x * 3 + y))
                            std::cout << "X ";
                        else
                            std::cout << "0 ";
                    }
                    else
                        std::cout << ". ";
                }
                std::cout << "\n";
            }
            std::cout << "\n";
        }

        auto is_board_dead() -> bool
        {
            if (is_full())
            {
                return true;
            }
            return evaluate(); // cast to bool
        }

        auto get_square_as_char(int square) -> char
        {
            if (!pos_filled(square))
            {
                return '.';
            }
            else
            {
                if (player_at(square))
                {
                    return 'X';
                }
                else
                {
                    return 'O';
                }
            }
        }
    };
} // namespace Board

namespace UTTT
{
    //moves will range from 0->80 and be filtered by forcingSquare.

    class State
    {
    public:
        Board::SubState metaposition[9];
        short forcingBoard;
        short turn;
        std::vector<short> movestack;
        std::vector<short> forcingstack;

        State()
        {
            for (short i = 0; i < 9; i++)
            {
                metaposition[i] = Board::SubState();
            }
            forcingBoard = -1;
            forcingstack.push_back(forcingBoard);
            turn = 1;
        }
        State(UTTT::State *inputState)
        {
            for (short i = 0; i < 9; i++)
            {
                metaposition[i] = inputState->metaposition[i];
            }
            forcingBoard = inputState->forcingBoard;
            turn = inputState->turn;
        }

        void reset()
        {
            for (short i = 0; i < 9; i++)
            {
                metaposition[i].reset();
            }
        }

        void play(short i)
        {
            short board, square;
            board = i / 9;
            square = i % 9;
            metaposition[board].play(square, turn);
            movestack.push_back(i);
            turn = -turn;
            forcingBoard = square;
            forcingstack.push_back(forcingBoard);
        }

        void unplay() // do not unplay on root
        {
            short prevmove = movestack.back();
            short board, square;
            board = prevmove / 9;
            square = prevmove % 9;
            movestack.pop_back();
            metaposition[board].unplay(square, turn);
            turn = -turn;
            forcingstack.pop_back();
            forcingBoard = forcingstack.back();
        }

        auto board_won(short board) -> bool
        {
            return metaposition[board].evaluate() != 0;
        }

        auto board_over(short board) -> bool
        {
            return metaposition[board].is_board_dead();
        }

        auto winner_of_board(short board) -> bool //only valid to use if pos_filled() returns true, true = x, false = y
        {
            return metaposition[board].evaluate() == 1;
        }

        auto is_full() -> bool
        {
            for (short i = 0; i < 9; i++)
            {
                if (!board_over(i))
                    return false;
            }
            return true;
        }

        auto evaluate() -> short
        {
            // check first diagonal
            if (board_over(0) && board_over(4) && board_over(8))
            {
                if (winner_of_board(0) == winner_of_board(4) && winner_of_board(4) == winner_of_board(8))
                {
                    if (winner_of_board(0))
                        return 1;
                    else
                        return -1;
                }
            }
            // check second diagonal
            if (board_over(2) && board_over(4) && board_over(6))
            {
                if (winner_of_board(2) == winner_of_board(4) && winner_of_board(4) == winner_of_board(6))
                {
                    if (winner_of_board(2))
                        return 1;
                    else
                        return -1;
                }
            }
            // check rows
            for (short i = 0; i < 3; i++)
            {
                if (board_over(i * 3) && board_over(i * 3 + 1) && board_over(i * 3 + 2))
                {
                    if (winner_of_board(i * 3) == winner_of_board(i * 3 + 1) && winner_of_board(i * 3 + 1) == winner_of_board(i * 3 + 2))
                    {
                        if (winner_of_board(i * 3))
                            return 1;
                        else
                            return -1;
                    }
                }
            }
            // check columns
            for (short i = 0; i < 3; i++)
            {
                if (board_over(i) && board_over(i + 3) && board_over(i + 6))
                {
                    if (winner_of_board(i) == winner_of_board(i + 3) && winner_of_board(i + 3) == winner_of_board(i + 6))
                    {
                        if (winner_of_board(i))
                            return 1;
                        else
                            return -1;
                    }
                }
            }
            short xwon = 0;
            short owon = 0;
            for (short i = 0; i < 9; i++)
            {
                if (board_over(i))
                {
                    if (winner_of_board(i))
                    {
                        xwon++;
                    }
                    else
                    {
                        owon++;
                    }
                }
                else
                {
                    return 0;
                }
            }
            if (xwon > owon)
            {
                return 1;
            }
            else
            {
                return -1;
            }
        }

        void pass_turn()
        {
            turn = -turn;
        }

        auto is_game_over() -> bool
        {
            if (legal_moves().size() == 0)
            {
                return true;
            }
            return (evaluate() != 0);
        }

        void show()
        {
            for (int x = 0; x < 3; x++)
            {
                for (int y = 0; y < 3; y++)
                {
                    if (board_over(x * 3 + y))
                    {
                        if (winner_of_board(x * 3 + y))
                            std::cout << "X ";
                        else
                            std::cout << "0 ";
                    }
                    else
                        std::cout << ". ";
                }
                std::cout << "\n";
            }
            std::cout << "\n";
            int board, square;
            std::vector<int> ordering = {
                0, 1, 2, 9, 10, 11, 18, 19, 20, 3, 4, 5, 12, 13, 14, 21, 22, 23, 6, 7, 8, 15, 16, 17, 24, 25, 26, 27, 28, 29, 36, 37, 38, 45, 46, 47, 30, 31, 32, 39, 40, 41, 48, 49, 50, 33, 34, 35, 42, 43, 44, 51, 52, 53, 54, 55, 56, 63, 64, 65, 72, 73, 74, 57, 58, 59, 66, 67, 68, 75, 76, 77, 60, 61, 62, 69, 70, 71, 78, 79, 80};
            int counter = 0;
            std::string linebreak = " |-----------------------|\n";
            for (int i : ordering)
            {
                board = i / 9;
                square = i % 9;
                if (counter % 9 == 0 && i != 0)
                    std::cout << " |\n";
                if (i == 0 || i == 27 || i == 54)
                    std::cout << linebreak;
                if (counter % 3 == 0)
                    std::cout << " |";
                std::cout << ' ' << metaposition[board].get_square_as_char(square);
                counter++;
            }
            std::cout << " |\n";
            std::cout << linebreak << "\n\n";
        }

        auto num_legal_moves() -> short
        {
            if (forcingBoard != -1)
                return 9 - popcount(metaposition[forcingBoard].union_bb());
            
            short cnt = 0;
            for (short i = 0; i < 9; i++)
            {
                if (!metaposition[i].is_board_dead())
                    cnt += 9 - popcount(metaposition[i].union_bb());
            }
            return cnt;
        }

        auto legal_moves() -> std::vector<Move>
        {
            std::vector<Move> moves;
            // only allow the forcingBoard
            if (metaposition[forcingBoard].is_board_dead())
            {
                forcingBoard = -1;
            }

            for (short board = 0; board < 9; board++)
            {
                if ((board != forcingBoard && forcingBoard != -1) || metaposition[board].is_board_dead())
                {
                    continue;
                }
                for (short square = 0; square < 9; square++)
                {
                    if (!metaposition[board].pos_filled(square))
                    {
                        moves.push_back(board * 9 + square);
                    }
                }
            }
            return moves;
        }

        void random_play()
        {
            std::vector<Move> moves = legal_moves();
            play(moves[rand() % moves.size()]);
        }
    };

    bool operator==(State a, State b){
        if (a.forcingBoard != b.forcingBoard)
            return false;
        if (a.turn != b.turn)
            return false;
        for (short i = 0; i < 9; i++){
            if (a.metaposition[i].position[0] != b.metaposition[i].position[0])
                return false;
            if (a.metaposition[i].position[1] != b.metaposition[i].position[1])
                return false;
        }
        if (a.forcingstack != b.forcingstack){
            return false;
        }
        return a.movestack == b.movestack;
    }
} // namespace UTTT

// int main()
// {
//     UTTT::State board = UTTT::State();
//     board.show();
//     while (!board.is_game_over())
//     {
//         board.random_play();
//         board.show();
//     }
//     return 0;
// }