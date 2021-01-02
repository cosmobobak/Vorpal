#include <iostream>
#include <vector>
#include "accelerations.hpp"

int weights[] = {1, 2, 3, 4, 3, 2, 1};

namespace Coin
{

#define Move short

    class State
    {
    public:
        int8_t node[6][7] = {
            {0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0},
        };
        int turn = 1;
        int nodes = 0;
        int players[2] = {-1, 1};
        std::vector<Move> movestack;

        State()
        {
            movestack.reserve(16);
        }

        void mem_setup()
        {
            movestack.reserve(7 * 6);
        }

        void reset()
        {
            for (int row = 0; row < 6; row++)
            {
                for (int col = 0; col < 7; col++)
                {
                    node[row][col] = 0;
                }
            }
        }

        auto is_full() -> bool //WORKING
        {
            for (int col = 0; col < 7; col++)
            {
                if (node[0][col] == 0)
                    return false;
            }
            return true;
        }

        void show() //WORKING
        {
            int row, col;
            char symbols[] = {'X', '.', 'O'};
            for (row = 0; row < 6; ++row)
            {
                for (col = 0; col < 7; ++col)
                {
                    std::cout << symbols[node[row][col] + 1] << ' ';
                }
                std::cout << '\n';
            }
            std::cout << '\n';
        }

        auto num_legal_moves() -> short
        {
            short count = 0;
            for (short i = 0; i < 7; i++)
            {
                if (node[0][i] == 0)
                    count++;
            }
            return count;
        }

        auto legal_moves() -> std::vector<Move>
        {
            std::vector<Move> moves;
            moves.reserve(7);
            int ordering[] = {3, 4, 2, 5, 1, 6, 0};
            for (int col = 0; col < 7; col++)
            {
                if (node[0][ordering[col]] == 0)
                {
                    moves.push_back(ordering[col]);
                }
            }
            return moves;
        }

        void random_play()
        {
            Move col;
            col = rand() % 7;
            while (node[0][col] != 0)
            {
                col = rand() % 7;
            }
            play(col);
        }

        auto pos_filled(int col) -> bool
        {
            return (node[0][col] != 0);
        }

        void pass_turn()
        {
            turn = -turn;
        }

        void play(int col) //WORKING
        {
            for (int row = 0; row < 6; row++)
            {
                if (node[row][col] != 0)
                {
                    if (turn == 1)
                    {
                        node[row - 1][col] = players[0];
                        break;
                    }
                    else
                    {
                        node[row - 1][col] = players[1];
                        break;
                    }
                }
                else if (row == 5)
                {
                    if (turn == 1)
                    {
                        node[row][col] = players[0];
                    }
                    else
                    {
                        node[row][col] = players[1];
                    }
                }
            }
            turn = -turn;
            movestack.push_back(col);
        }

        void unplay() //WORKING
        {
            int col = movestack.back();
            movestack.pop_back();
            for (int row = 0; row < 6; row++)
            {
                if (node[row][col] != 0)
                {
                    node[row][col] = 0;
                    break;
                }
            }
            turn = -turn;
        }

        auto horizontal_term() -> int
        {
            for (int row = 0; row < 6; row++)
            {
                for (int col = 0; col < 4; col++)
                {
                    if (node[row][col] == node[row][col + 1] &&
                        node[row][col + 1] == node[row][col + 2] &&
                        node[row][col + 2] == node[row][col + 3])
                    {
                        if (node[row][col] == players[0])
                        {
                            return 1;
                        }
                        else if (node[row][col] == players[1])
                        {
                            return -1;
                        }
                    }
                }
            }
            return 0;
        }

        auto vertical_term() -> int
        {
            for (int row = 0; row < 3; row++)
            {
                for (int col = 0; col < 7; col++)
                {
                    if (node[row][col] == node[row + 1][col] &&
                        node[row + 1][col] == node[row + 2][col] &&
                        node[row + 2][col] == node[row + 3][col])
                    {
                        if (node[row][col] == players[0])
                        {
                            return 1;
                        }
                        else if (node[row][col] == players[1])
                        {
                            return -1;
                        }
                    }
                }
            }
            return 0;
        }

        auto diagup_term() -> int
        {
            for (int row = 3; row < 6; row++)
            {
                for (int col = 0; col < 4; col++)
                {
                    if (node[row][col] == node[row - 1][col + 1] &&
                        node[row - 1][col + 1] == node[row - 2][col + 2] &&
                        node[row - 2][col + 2] == node[row - 3][col + 3])
                    {
                        if (node[row][col] == players[0])
                        {
                            return 1;
                        }
                        else if (node[row][col] == players[1])
                        {
                            return -1;
                        }
                    }
                }
            }
            return 0;
        }

        auto diagdown_term() -> int
        {
            for (int row = 0; row < 3; row++)
            {
                for (int col = 0; col < 4; col++)
                {
                    if (node[row][col] == node[row + 1][col + 1] &&
                        node[row + 1][col + 1] == node[row + 2][col + 2] &&
                        node[row + 2][col + 2] == node[row + 3][col + 3])
                    {
                        if (node[row][col] == players[0])
                        {
                            return 1;
                        }
                        else if (node[row][col] == players[1])
                        {
                            return -1;
                        }
                    }
                }
            }
            return 0;
        }

        auto evaluate() -> int
        {
            int v, h, u, d;
            v = vertical_term();
            if (v)
                return v;
            h = horizontal_term();
            if (h)
                return h;
            u = diagup_term();
            if (u)
                return u;
            d = diagdown_term();
            if (d)
                return d;

            return 0;
        }

        void show_result() //WORKING
        {
            int r;
            r = evaluate();
            if (r == 0)
            {
                std::cout << "1/2-1/2" << '\n';
            }
            else if (r > 0)
            {
                std::cout << "1-0" << '\n';
            }
            else
            {
                std::cout << "0-1" << '\n';
            }
        }

        auto is_game_over() -> int
        {
            if (evaluate() != 0 || is_full() == true)
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        auto heuristic_value() -> int
        {
            int val = 0;
            for (short row = 0; row < 6; row++)
            {
                for (short i = 0; i < 7; i++)
                {
                    val += node[row][i] * weights[i];
                }
            }
            return -(val * 10); // use some sort of central weighting approach
        }
    };

    bool operator==(State a, State b)
    {
        if (a.turn != b.turn)
            return false;
        for (int row = 0; row < 6; row++)
        {
            for (int col = 0; col < 7; col++)
            {
                if (a.node[row][col] != b.node[row][col])
                    return false;
            }
        }
        return a.movestack == b.movestack;
    }
} // namespace Coin