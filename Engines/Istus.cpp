#include <iostream>
#include <vector>
#include <algorithm>
#include <ctime>
#include <climits>
#include <cmath>
#include <stdlib.h>
#include <Coin.hpp>
#include <Glyph.hpp>

using namespace Glyph;

class Istus
{
public:
    State node = State();
    int nodes = 0;

    void reset_nodes()
    {
        nodes = 0;
    }

    void print(std::string input, std::string end = "\n")
    {
        std::cout << input << end;
    }

    auto get_player_move() -> int
    {
        int pos;
        std::cin >> pos;
        while (node.pos_filled(pos))
        {
            print("invalid move.");
            node.show();
            std::cin >> pos;
        }
        return pos;
    }

    auto negamax(int turn, int a = -2, int b = 2) -> int
    {
        if (node.is_game_over())
        {
            nodes++;
            return turn * node.evaluate();
        }

        int score;
        for (auto &&i : node.legal_moves())
        {
            node.play(i);
            score = -negamax(-turn, -b, -a);
            node.unplay();

            if (score > b)
                return b;
            if (score > a)
                a = score;
        }
        return a;
    }

    void engine_move()
    {
        int index = 0;
        int best = -2;
        for (auto &&i : node.legal_moves())
        {
            node.play(i);
            int score = -negamax(node.turn, -2, 2);
            node.unplay();
            if (score > best)
            {
                best = score;
                index = i;
            }
        }
        node.play(index);
    }

    void show_result()
    {
        int r;
        r = node.evaluate();
        if (r == 0)
            std::cout << "1/2-1/2" << '\n';
        else if (r == 1)
            std::cout << "1-0" << '\n';
        else
            std::cout << "0-1" << '\n';
    }
};