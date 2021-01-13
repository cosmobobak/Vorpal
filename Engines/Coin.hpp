#include <array>
#include <iostream>
#include <vector>

#include "accelerations.hpp"

namespace Coin {
constexpr std::array<int, 7> weights = {1, 2, 3, 4, 3, 2, 1};
constexpr std::array<int, 2> players = {-1, 1};
using Move = short;
constexpr auto gameexpfactor = 5;  // dubious
class State {
   public:
    std::array<std::array<int, 6>, 2> bbnode = {0};
    int turn = 1;
    int nodes = 0;
    std::vector<Move> movestack;

    State() {
        movestack.reserve(16);
    }

    void mem_setup() {
        movestack.reserve(7 * 6);
    }

    // void reset()
    // {
    //     for (int row = 0; row < 6; row++)
    //     {
    //         for (int col = 0; col < 7; col++)
    //         {
    //             node[row][col] = 0;
    //         }
    //     }
    // }

    auto union_bb(short r) -> short {
        return bbnode[0][r] | bbnode[1][r];
    }

    auto is_full() -> bool  //WORKING
    {
        return union_bb(0) == 0b1111111;
    }

    void show()  //WORKING
    {
        short row, col;
        for (row = 0; row < 6; ++row) {
            for (col = 0; col < 7; ++col) {
                if (pos_filled(row, col)) {
                    if (player_at(row, col))
                        std::cout << "X ";
                    else
                        std::cout << "O ";
                } else
                    std::cout << ". ";
            }
            std::cout << '\n';
        }
        std::cout << '\n';
    }

    auto pos_filled(short row, short col) -> bool {
        return bbnode[0][row] & (1 << col) || bbnode[1][row] & (1 << col);
    }

    auto pos_filled(short col) -> bool {
        return bbnode[0][0] & (1 << col) || bbnode[1][0] & (1 << col);
    }

    auto player_at(short row, short col) -> bool  //only valid to use if pos_filled() returns true, true = x, false = y
    {
        return (bbnode[0][row] & (1 << col));
    }

    auto num_legal_moves() -> short {
        return 7 - popcount(bbnode[0][0] | bbnode[1][0]);
    }

    auto legal_moves() -> std::vector<Move> {
        std::vector<Move> moves;
        moves.reserve(7);
        int toprow = bbnode[0][0] | bbnode[1][0];
        for (short col = 0; col < 7; col++) {
            if (!(toprow & (1 << col)))
                moves.push_back(col);
        }
        return moves;
    }

    void random_play() {
        std::vector<Move> moves = legal_moves();
        play(moves[rand() % moves.size()]);
    }

    inline void pass_turn() {
        turn = -turn;
    }

    void play(short col)  //WORKING
    {
        for (short row = 6; row; row--) {
            if (!pos_filled(row - 1, col)) {
                if (turn == 1)
                    bbnode[0][row - 1] |= (1 << col);
                else
                    bbnode[1][row - 1] |= (1 << col);
                break;
            }
        }
        pass_turn();
        movestack.push_back(col);
    }

    void unplay()  //WORKING
    {
        int col = movestack.back();
        movestack.pop_back();
        for (int row = 0; row < 6; row++) {
            if (pos_filled(row, col)) {
                if (turn == 1)
                    bbnode[1][row] &= ~(1 << col);
                else
                    bbnode[0][row] &= ~(1 << col);
                break;
            }
        }
        pass_turn();
    }

    auto horizontal_term() -> int {
        for (short row = 0; row < 6; row++) {
            for (short col = 0; col < 4; col++) {
                if (pos_filled(row, col) &&
                    pos_filled(row, col + 1) &&
                    pos_filled(row, col + 2) &&
                    pos_filled(row, col + 3)) {
                    if (player_at(row, col) == player_at(row, col + 1) &&
                        player_at(row, col + 1) == player_at(row, col + 2) &&
                        player_at(row, col + 2) == player_at(row, col + 3)) {
                        if (player_at(row, col))
                            return 1;
                        else
                            return -1;
                    }
                }
            }
        }
        return 0;
    }

    auto vertical_term() -> int {
        for (int row = 0; row < 3; row++) {
            for (int col = 0; col < 7; col++) {
                if (pos_filled(row, col) &&
                    pos_filled(row + 1, col) &&
                    pos_filled(row + 2, col) &&
                    pos_filled(row + 3, col)) {
                    if (player_at(row, col) == player_at(row + 1, col) &&
                        player_at(row + 1, col) == player_at(row + 2, col) &&
                        player_at(row + 2, col) == player_at(row + 3, col)) {
                        if (player_at(row, col))
                            return 1;
                        else
                            return -1;
                    }
                }
            }
        }
        return 0;
    }

    auto diagup_term() -> int {
        for (int row = 3; row < 6; row++) {
            for (int col = 0; col < 4; col++) {
                if (pos_filled(row, col) &&
                    pos_filled(row - 1, col + 1) &&
                    pos_filled(row - 2, col + 2) &&
                    pos_filled(row - 3, col + 3)) {
                    if (player_at(row, col) == player_at(row - 1, col + 1) &&
                        player_at(row - 1, col + 1) == player_at(row - 2, col + 2) &&
                        player_at(row - 2, col + 2) == player_at(row - 3, col + 3)) {
                        if (player_at(row, col))
                            return 1;
                        else
                            return -1;
                    }
                }
            }
        }
        return 0;
    }

    auto diagdown_term() -> int {
        for (int row = 0; row < 3; row++) {
            for (int col = 0; col < 4; col++) {
                if (pos_filled(row, col) &&
                    pos_filled(row + 1, col + 1) &&
                    pos_filled(row + 2, col + 2) &&
                    pos_filled(row + 3, col + 3)) {
                    if (player_at(row, col) == player_at(row + 1, col + 1) &&
                        player_at(row + 1, col + 1) == player_at(row + 2, col + 2) &&
                        player_at(row + 2, col + 2) == player_at(row + 3, col + 3)) {
                        if (player_at(row, col))
                            return 1;
                        else
                            return -1;
                    }
                }
            }
        }
        return 0;
    }

    auto evaluate() -> int {
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

    void show_result()  //WORKING
    {
        int r;
        r = evaluate();
        if (r == 0) {
            std::cout << "1/2-1/2" << '\n';
        } else if (r > 0) {
            std::cout << "1-0" << '\n';
        } else {
            std::cout << "0-1" << '\n';
        }
    }

    auto is_game_over() -> int {
        if (evaluate() != 0 || is_full() == true) {
            return true;
        } else {
            return false;
        }
    }

    // auto heuristic_value() -> int
    // {
    //     int val = 0;
    //     for (short row = 0; row < 6; row++)
    //     {
    //         for (short i = 0; i < 7; i++)
    //         {
    //             val += pos_filled(row, i) * (player_at(row, i) ? 1 : -1) * weights[i];
    //         }
    //     }
    //     return -(val * 10); // use some sort of central weighting approach
    // }
};

bool operator==(State a, State b) {
    if (a.turn != b.turn)
        return false;
    for (int row = 6; row; row--) {
        if (a.bbnode[0][row] != b.bbnode[0][row] || a.bbnode[1][row] != b.bbnode[1][row])
            return false;
    }
    return a.movestack == b.movestack;
}
}  // namespace Coin