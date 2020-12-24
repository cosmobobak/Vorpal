#include <iostream>
#include <algorithm>

enum turn
{
    CROSS,
    NOUGHT,
};

class MetaGlyph
{
public:
    int mods[2] = {-1, 1};
    int metanode[2] = {0b000000000, 0b000000000};
    int subnodes[9][2];
    int turn = CROSS;
    int nodes = 0;

    MetaGlyph()
    {
        for (int i = 0; i < 9; i++)
        {
            for (int j = 0; j < 2; j++)
            {
                subnodes[i][j] = 0b100100100;
            }
        }
    }

    void reset_nodes()
    {
        nodes = 0;
    }

    auto square_filled(int i, int bbpair[]) -> bool
    {
        if ((bbpair[0] | bbpair[1]) & (1L << i) != 0)
            return true;
        else return false;
    }

    auto player_at(int i, int bbpair[]) -> bool // only valid to use if square_filled() returns true
    {                                           // true = x, false = y
        if (bbpair[0] & (1L << i) != 0)
            return true;
        else return false;
    }

    auto board_filled() -> bool
    {
        for (int i = 0; i < 9; i++)
        {
            if (!square_filled(i, metanode))
            {
                return false;
            }
        }
        return true;
    }

    auto piecefinder(int i, int bbpair[])
    {
        if (square_filled(i, bbpair))
        {
            if (player_at(i, bbpair) == CROSS) return 'X';
            else return '0';
        }
        else return '.';
    }

    void show() //displays the full board
    {
        for (int run = 0; run < 9; run++)
        {
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    std::cout << piecefinder(j + (run % 3)*3, subnodes[i + run / 3]) << ' ';
                }
            }
            std::cout << '\n';
        }
    }

    void meta_show() //displays the top-level board
    {
        for (int x = 0; x < 3; x++)
        {
            for (int y = 0; y < 3; y++)
            {
                std::cout << piecefinder(x * 3 + y, metanode) << ' ';
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
            metanode[0] = metanode[0] ^ (1 << i);
            turn = -1;
        }
        else
        {
            metanode[1] = metanode[1] ^ (1 << i);
            turn = 1;
        }
    }

    void unplay(int i) //only valid directly after a move, do not unplay on root, or unplay twice in a row
    {
        if (turn == 1)
        {
            metanode[1] = metanode[1] ^ (1 << i);
            turn = -1;
        }
        else
        {
            metanode[0] = metanode[0] ^ (1 << i);
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

    auto evaluate(int n[]) -> int
    {
        nodes++; // increment nodes
        // check first diagonal
        if (square_filled(0, n) && square_filled(4, n) && square_filled(8, n))
        {
            if (player_at(0, n) == player_at(4, n) && player_at(4, n) == player_at(8, n))
            {
                if (player_at(0, n))
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
        if (square_filled(2, n) && square_filled(4, n) && square_filled(6, n))
        {
            if (player_at(2, n) == player_at(4, n) && player_at(4, n) == player_at(6, n))
            {
                if (player_at(2, n))
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
            if (square_filled(i * 3, n) && square_filled(i * 3 + 1, n) && square_filled(i * 3 + 2, n))
            {
                if (player_at(i * 3, n) == player_at(i * 3 + 1, n) && player_at(i * 3 + 1, n) == player_at(i * 3 + 2, n))
                {
                    if (player_at(i * 3, n))
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
            if (square_filled(i, n) && square_filled(i + 3, n) && square_filled(i + 6, n))
            {
                if (player_at(i, n) == player_at(i + 3, n) && player_at(i + 3, n) == player_at(i + 6, n))
                {
                    if (player_at(i, n))
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

    void print(std::string input, std::string end = "\n")
    {
        std::cout << input << end;
    }

    auto get_player_move() -> int
    {
        //int pos;
        //std::cin >> pos;
        //while (square_filled(pos))
        //{
        //    print("invalid move.");
        //    show();
        //    std::cin >> pos;
        //}
        //return pos;
    }

    auto is_game_over() -> bool
    {
        return evaluate(metanode) != 0 || board_filled();
    }

    void show_result()
    {
        int r;
        r = evaluate(metanode);
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

    auto get_first_player() -> bool
    {
        bool player;
        std::cout << "Is the human player going first? [1/0]"
                  << "\n";
        std::cin >> player;
        return player;
    }

    void run_game()
    {
        int i;
        show();
        if (get_first_player())
        {
            i = get_player_move();
            play(i);
            show();
        }
        while (!is_game_over())
        {
            //meta.engine_move();
            std::cout << "Nodes processed for move: " << nodes << "\n";
            reset_nodes();
            show();
            if (is_game_over())
            {
                break;
            }
            i = get_player_move();
            play(i);
            show();
        }
        show_result();
    }
};

int main()
{
    MetaGlyph meta;
    meta.show();
    meta.meta_show();
    return 0;
}