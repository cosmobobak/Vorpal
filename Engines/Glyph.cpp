#include <iostream>
#include <algorithm>

class Glyph
{
public:
    int node[2] = {0b000000000, 0b000000000};
    int turn = 1;
    int nodes = 0;

    void reset_nodes()
    {
        nodes = 0;
    }

    bool pos_filled(int i)
    {
        if ((node[0] & (1L << i)) != 0)
        {
            return true;
        }
        else if ((node[1] & (1L << i)) != 0)
        {
            return true;
        }
        return false;
    }

    bool player_at(int i) //only valid to use if pos_filled() returns true
    {
        if ((node[0] & (1L << i)) != 0)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    bool is_full()
    {
        for (int i = 0; i < 9; i++)
        {
            if (!pos_filled(i))
            {
                return false;
            }
        }
        return true;
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
                    {
                        std::cout << "X ";
                    }
                    else
                    {
                        std::cout << "0 ";
                    }
                }
                else
                {
                    std::cout << ". ";
                }
            }
            std::cout << "\n";
        }
        std::cout << "\n";
    }

    void play(int i)
    {
        // n ^ (1 << k) is a binary XOR where you flip the kth bit of n
        if (turn == 1)
        {
            node[0] = node[0] ^ (1 << i);
            turn = -1;
        }
        else
        {
            node[1] = node[1] ^ (1 << i);
            turn = 1;
        }
    }

    void unplay(int i) //only valid directly after a move, do not unplay on root, or unplay twice in a row
    {
        if (turn == 1)
        {
            node[1] = node[1] ^ (1 << i);
            turn = -1;
        }
        else
        {
            node[0] = node[0] ^ (1 << i);
            turn = 1;
        }
    }

    void pass()
    {
        if (turn == 1)
        {
            turn = -1;
        }
        else
        {
            turn = 1;
        }
    }

    int evaluate()
    {
        nodes++; // increment nodes
        // check first diagonal
        if (pos_filled(0) && pos_filled(4) && pos_filled(8))
        {
            if (player_at(0) == player_at(4) && player_at(4) == player_at(8))
            {
                if (player_at(0))
                {
                    return 1;
                }
                else
                {
                    return -1;
                }
            }
        }
        // check second diagonal
        if (pos_filled(2) && pos_filled(4) && pos_filled(6))
        {
            if (player_at(2) == player_at(4) && player_at(4) == player_at(6))
            {
                if (player_at(2))
                {
                    return 1;
                }
                else
                {
                    return -1;
                }
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
                    {
                        return 1;
                    }
                    else
                    {
                        return -1;
                    }
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
                    {
                        return 1;
                    }
                    else
                    {
                        return -1;
                    }
                }
            }
        }
        return 0;
    }

    int negamax(int turn, int a = -2, int b = 2)
    {
        if (evaluate() != 0 || is_full() == true)
        {
            return turn * evaluate();
        }

        int score;
        for (int i = 0; i < 9; i++)
        {
            if (!pos_filled(i))
            {
                play(i);
                score = -negamax(-turn, -b, -a);
                unplay(i);

                if (score > b)
                {
                    return b;
                }
                if (score > a)
                {
                    a = score;
                }
            }
        }
        return a;
    }

    int max_pos(int arr[])
    {
        int max, index;
        max = arr[0];
        index = 0;
        for (int i = 0; i < 9; i++)
        {
            if (arr[i] > max)
            {
                max = arr[i];
                index = i;
            }
        }
        return index;
    }

    void engine_move()
    {
        int index;
        int scores[9] = {-2, -2, -2, -2, -2, -2, -2, -2, -2};

        for (int i = 0; i < 9; i++)
        {
            if (!pos_filled(i))
            {
                play(i);
                scores[i] = -negamax(turn, -2, 2);
                unplay(i);
            }
        }
        index = max_pos(scores);
        play(index);
    }

    void show_result()
    {
        int r;
        r = evaluate();
        if (r == 0)
        {
            std::cout << "1/2-1/2" << '\n';
        }
        else if (r == 1)
        {
            std::cout << "1-0" << '\n';
        }
        else
        {
            std::cout << "0-1" << '\n';
        }
    }
};

int main()
{
    Glyph glyph;
    int i;
    std::cin >> i;
    glyph.play(i);
    glyph.show();
    while (glyph.evaluate() == 0 && glyph.is_full() == false)
    {
        glyph.engine_move();
        std::cout << "Nodes processed for move: " << glyph.nodes << "\n";
        glyph.reset_nodes();
        glyph.show();
        if (glyph.evaluate() != 0 || glyph.is_full() == true)
        {
            break;
        }
        std::cin >> i;
        glyph.play(i);
        glyph.show();
    }
    glyph.show_result();
    return 0;
}