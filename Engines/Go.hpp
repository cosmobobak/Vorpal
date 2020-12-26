#include <iostream>
#include <vector>

namespace Go
{

#define WIDTH 7
#define HEIGHT 7
#define Move int

    class State
    {
    public:
        int node[WIDTH * HEIGHT];
        int turn = 1;
        int nodes = 0;
        char players[2] = {'X', 'O'};
        std::vector<Move> movestack;

        void show() //WORKING
        {
            int row, col;
            for (row = 0; row < HEIGHT; ++row)
            {
                for (col = 0; col < WIDTH; ++col)
                {
                    char out;
                    switch (node[row * 7 + col])
                    {
                    case 0:
                        out = '🞡';
                        break;

                    case 1:
                        out = players[0];
                        break;

                    case -1:
                        out = players[1];
                        break;

                    default:
                        out = 'E';
                        break;
                    }
                    std::cout << out;
                }
                std::cout << '\n';
            }
            std::cout << '\n';
        }

        auto legal_moves() -> std::vector<int>
        {
            std::vector<Move> moves;
            moves.push_back(-1);
            for (int square = 0; square < WIDTH * HEIGHT; square++)
            {
                if (node[square] == 0)
                {
                    moves.push_back(square);
                }
            }
            return moves;
        }

        void random_play()
        {
            Move square;
            square = rand() % (WIDTH * HEIGHT);
            while (node[square] != 0)
            {
                square = rand() % (WIDTH * HEIGHT);
            }
            play(square);
        }

        void pass_turn()
        {
            turn = -turn;
        }

        void play(Move square)
        {
            if (square != -1)
            {
                node[square] = turn;
            }
            turn = -turn;
            movestack.push_back(square);
        }

        void unplay()
        {
            int square = movestack.back();
            node[square] = 0;
            turn = -turn;
            movestack.pop_back();
        }

        auto is_game_over() -> int
        {
            return ((movestack.end() - 1) == (movestack.end() - 2) && movestack.back() == -1);
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
    };
} // namespace Go