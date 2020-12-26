#include <iostream>
#include <vector>

namespace Glyph
{
    #define Move int
    
    class State
    {
    public:
        int position[2] = {0b000000000, 0b000000000};
        int turn = 1;
        std::vector<int> movestack;

        void reset()
        {
            position[0] = 0b000000000;
            position[1] = 0b000000000;
        }

        void play(int i)
        {
            // n ^ (1 << k) is a binary XOR where you flip the kth bit of n
            if (turn == 1)
            {
                position[0] |= (1 << i);
                turn = -1;
            }
            else
            {
                position[1] |= (1 << i);
                turn = 1;
            }
            movestack.push_back(i);
        }

        void unplay() // do not unplay on root
        {
            int prevmove = movestack.back();
            movestack.pop_back();
            if (turn == 1)
            {
                position[1] &= ~(1 << prevmove);
                turn = -1;
            }
            else
            {
                position[0] &= ~(1 << prevmove);
                turn = 1;
            }
        }

        auto pos_filled(int i) -> bool
        {
            if (((position[0] | position[1]) & (1L << i)) != 0)
                return true;
            else
                return false;
        }

        auto player_at(int i) -> bool //only valid to use if pos_filled() returns true, true = x, false = y
        {
            if ((position[0] & (1L << i)) != 0)
                return true;
            else
                return false;
        }

        auto is_full() -> bool
        {
            for (int i = 0; i < 9; i++)
            {
                if (!pos_filled(i))
                    return false;
            }
            return true;
        }

        auto evaluate() -> int
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
            for (int i = 0; i < 3; i++)
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
            for (int i = 0; i < 3; i++)
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

        auto rel_evaluate(int turnmod) -> int
        {
            return evaluate() * turnmod;
        }

        void pass_turn()
        {
            turn = -turn;
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

        auto is_game_over() -> bool
        {
            return (evaluate() != 0) || is_full();
        }

        auto legal_moves() -> std::vector<int>
        {
            std::vector<int> moves;
            for (int i = 0; i < 9; i++)
            {
                if (!pos_filled(i))
                    moves.push_back(i);
            }
            return moves;
        }

        void random_play()
        {
            int move;
            move = rand() % 9;
            while (pos_filled(move))
            {
                move = rand() % 9;
            }
            play(move);
        }
    };
} // namespace Glyph
