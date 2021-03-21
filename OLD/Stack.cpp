#include <iostream>
#include <algorithm>
#include <cmath>

class Move
{
public:
    int fromSquare;
    int toSquare;
    Move(int x, int y)
    { // Constructor
        fromSquare = x;
        toSquare = y;
    }
}

class Stack
{
public:
    long node[2] = {
        0b1111111111111111000000000000000000000000,
        0b0000000000000000000000001111111111111111};
    short turn = 1;
    
    int BB_OCCUPIED()
    {
        return node[0] | node[1];
    }

    void show() 
    {
        
    }

    void play(Move move)
    {
        
    }

    void unplay(int col)
    {
        for (int row = 0; row < 6; row++)
        {
            if (node[row][col] != '.')
            {
                node[row][col] = '.';
                break;
            }
        }
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
        int v, h, u, d;
        v = vertical_term();
        h = horizontal_term();
        u = diagup_term();
        d = diagdown_term();

        return v + h + u + d;
    }

    int negamax(int depth = 10, int colour = 1, int a = -2, int b = 2) 
    {
        if (evaluate() != 0 || is_full() == true || depth < 1)
        {
            return colour * evaluate();
        }
        int score;
        for (int col = 0; col < 7; col++)
        {
            if (node[0][col] == '.')
            {
                play(col);
                nodes++;
                score = -negamax(depth - 1, -colour, -b, -a);
                unplay(col);

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
        for (int i = 0; i < 7; i++)
        {
            if (arr[i] > max)
            {
                max = arr[i];
                index = i;
            }
        }
        return index;
    }

    int min_pos(float arr[]) 
    {
        float max, index;
        max = arr[0];
        index = 0;
        for (int i = 0; i < 7; i++)
        {
            if (arr[i] < max)
            {
                max = arr[i];
                index = i;
            }
        }
        return index;
    }

    void engine_move() 
    {
        int x, y, index;
        float scores[7] = {2, 2, 2, 2, 2, 2, 2};

        for (int col = 0; col < 7; col++)
        {
            if (node[0][col] == '.')
            {
                play(col);
                scores[col] = negamax(12, turn, -2, 2);
                unplay(col);
            }
        }
        for (float i = 0; i < 7; i++)
        {
            scores[static_cast<int>(i)] -= 1 - (abs(i - 3) + 3) / 6;
            std::cout << scores[static_cast<int>(i)] << " ";
        }
        std::cout << std::endl;
        play(min_pos(scores));
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

    bool is_game_over()
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
};

int main()
{
    Stack stack;
    stack.show();
    int col;
    while (!stack.is_game_over())
    {
        stack.engine_move();
        stack.show();
    }
    return 0;
}