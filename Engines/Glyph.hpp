#include <iostream>
#include <vector>

#include "accelerations.hpp"

namespace Glyph {
using Move = short;
constexpr auto gameexpfactor = 5;
class State {
   public:
    short position[2] = {0b000000000, 0b000000000};
    short turn = 1;
    std::vector<short> movestack;

    void mem_setup() {
        movestack.reserve(9);
    }

    void reset() {
        position[0] = 0b000000000;
        position[1] = 0b000000000;
    }

    void play(const short i) {
        // n ^ (1 << k) is a binary XOR where you flip the kth bit of n
        if (turn == 1)
            position[0] |= (1 << i);
        else
            position[1] |= (1 << i);
        turn = -turn;
        movestack.push_back(i);
    }

    void unplay()  // do not unplay on root
    {
        short prevmove = movestack.back();
        movestack.pop_back();
        if (turn == 1)
            position[1] &= ~(1 << prevmove);
        else
            position[0] &= ~(1 << prevmove);
        turn = -turn;
    }

    auto pos_filled(const short i) -> bool {
        if (((position[0] | position[1]) & (1L << i)) != 0)
            return true;
        else
            return false;
    }

    auto player_at(const short i) -> bool  //only valid to use if pos_filled() returns true, true = x, false = y
    {
        if ((position[0] & (1L << i)) != 0)
            return true;
        else
            return false;
    }

    auto is_full() -> bool {
        for (short i = 0; i < 9; i++) {
            if (!pos_filled(i))
                return false;
        }
        return true;
    }

    auto evaluate() -> short {
        // check first diagonal
        if (pos_filled(0) && pos_filled(4) && pos_filled(8)) {
            if (player_at(0) == player_at(4) && player_at(4) == player_at(8)) {
                if (player_at(0))
                    return 1;
                else
                    return -1;
            }
        }
        // check second diagonal
        if (pos_filled(2) && pos_filled(4) && pos_filled(6)) {
            if (player_at(2) == player_at(4) && player_at(4) == player_at(6)) {
                if (player_at(2))
                    return 1;
                else
                    return -1;
            }
        }
        // check rows
        for (short i = 0; i < 3; i++) {
            if (pos_filled(i * 3) && pos_filled(i * 3 + 1) && pos_filled(i * 3 + 2)) {
                if (player_at(i * 3) == player_at(i * 3 + 1) && player_at(i * 3 + 1) == player_at(i * 3 + 2)) {
                    if (player_at(i * 3))
                        return 1;
                    else
                        return -1;
                }
            }
        }
        // check columns
        for (short i = 0; i < 3; i++) {
            if (pos_filled(i) && pos_filled(i + 3) && pos_filled(i + 6)) {
                if (player_at(i) == player_at(i + 3) && player_at(i + 3) == player_at(i + 6)) {
                    if (player_at(i))
                        return 1;
                    else
                        return -1;
                }
            }
        }
        return 0;
    }

    void pass_turn() {
        turn = -turn;
    }

    void show() {
        for (short x = 0; x < 3; x++) {
            for (short y = 0; y < 3; y++) {
                if (pos_filled(x * 3 + y)) {
                    if (player_at(x * 3 + y))
                        std::cout << "X ";
                    else
                        std::cout << "0 ";
                } else
                    std::cout << ". ";
            }
            std::cout << "\n";
        }
        std::cout << "\n";
    }

    auto is_game_over() -> bool {
        return (evaluate() != 0) || is_full();
    }

    auto num_legal_moves() {
        return 9 - popcount(position[0] | position[1]);
    }

    auto legal_moves() -> std::vector<Move> {
        std::vector<Move> moves;
        moves.reserve(9);
        for (short i = 0; i < 9; i++) {
            if (!pos_filled(i))
                moves.push_back(i);
        }
        return moves;
    }

    void random_play() {
        std::vector<Move> moves = legal_moves();
        play(moves[rand() % moves.size()]);
    }

    auto heuristic_value() -> int {
        return 0;
    }
};

bool operator==(const State a, const State b) {
    if (a.turn != b.turn)
        return false;
    if (a.position[0] != b.position[0])
        return false;
    if (a.position[1] != b.position[1])
        return false;
    return a.movestack == b.movestack;
}
}  // namespace Glyph
