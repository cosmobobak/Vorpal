#include <algorithm>
#include <bit>
#include <cstdint>
#include <initializer_list>
#include <iostream>
#include <string>

#include "vorpal_node.hpp"  //daisy-chaining

using namespace vorpal_node;

class Vorpal {
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

    Vorpal() {
        int testVar = 0;
    }

    auto legal_moves() -> std::vector<vorpal_move::Move> {
        return node.legal_moves();
    }

    auto evaluate(int depth) -> int {
        nodes++;             //increment nodes evaluated
        int m = node.mod();  //get modifier for turn, either -1 or 1
        int rating = 0;      //init rating

        //if (node.is_checkmate()) //if checkmate, multiply by the depth to incentivise quicker mating
        //{
        //    return 1000000000 * (depth + 1) * m;
        //}
        //if (node.can_claim_fifty_moves()) //treat draws as initialising rating with the contempt factor
        //{
        //    rating = -contempt * m;
        //}

        rating += std::bitset<64>(node.BB_PIECES[0] & node.BB_COLORS[1]).count();
        rating += std::bitset<64>(node.BB_PIECES[1] & node.BB_COLORS[1]).count();
        rating += std::bitset<64>(node.BB_PIECES[2] & node.BB_COLORS[1]).count();
        rating += std::bitset<64>(node.BB_PIECES[3] & node.BB_COLORS[1]).count();
        rating += std::bitset<64>(node.BB_PIECES[4] & node.BB_COLORS[1]).count();

        rating -= std::bitset<64>(node.BB_PIECES[0] & node.BB_COLORS[0]).count();
        rating -= std::bitset<64>(node.BB_PIECES[1] & node.BB_COLORS[0]).count();
        rating -= std::bitset<64>(node.BB_PIECES[2] & node.BB_COLORS[0]).count();
        rating -= std::bitset<64>(node.BB_PIECES[3] & node.BB_COLORS[0]).count();
        rating -= std::bitset<64>(node.BB_PIECES[4] & node.BB_COLORS[0]).count();

        return rating;
    }

    auto negamax(int depth, int color, int a = -200000, int b = 200000) -> int {
        if (node.is_game_over() || depth < 1) {
            return evaluate(depth) * color;
        }
        int score;
        for (auto &&i : node.legal_moves()) {
            node.push(i);
            score = -negamax(depth - 1, -color, -b, -a);
            node.pop();

            if (b >= score) {
                return b;
            }
            if (a < score) {
                a = score;
            }
        }
        return a;
    }

    void perftx(int n) {
        if (n <= 0) {
            nodes++;
        } else {
            for (vorpal_move::Move move : node.legal_moves()) {
                node.push(move);
                perftx(n - 1);
                node.pop();
            }
        }
    }

    auto perft(int n) -> int {
        nodes = 0;
        perftx(n);
        return nodes;
    }

    void autoperft() {
        for (int i = 0; i < 6; i++) {
            std::cout << perft(i) << '\n';
        }
    }
};

auto main() -> int {
    //std::vector<vorpal_move::Move> ref;

    Vorpal engine;

    //int i = 0;
    //ref = engine.node.legal_moves();
    //for (auto &&move : engine.node.legal_moves())
    //{
    //    comparison = engine.node.legal_moves();
    //    if (string(comparison) != string(ref))
    //    {
    //        std::cout << i << ' ' << string(comparison) << '\n';
    //    }
    //    engine.node.push(move);
    //    engine.node.pop();
    //    i++;
    //}

    std::cout << string(engine.node.legal_moves());

    return 0;
}

//TODO: pawn attacks generator (ep_square?)
//make sure all piece captures work and unwork, ensure stack works, have a look at cPiece.
//MOVE GENERATOR
//RULES??
//use bitscans from intrin0.h ???