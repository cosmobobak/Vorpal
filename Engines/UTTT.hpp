#include <array>
#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>

#include "accelerations.hpp"

namespace Board {
using Move = short;
class SubState {
   public:
    std::array<short, 2> position = {0b000000000, 0b000000000};
    // bool cached_death_state = false;

    void reset() {
        position[0] = 0b000000000;
        position[1] = 0b000000000;
    }

    inline auto union_bb() const -> short {
        return position[0] | position[1];
    }

    inline void play(const short i, const short turn) {
        // n ^ (1 << k) is a binary XOR where you flip the kth bit of n
        if (turn == 1) {
            position[0] |= (1 << i);
        } else {
            position[1] |= (1 << i);
        }
    }

    inline void unplay(const short prevmove, const short turn)  // do not unplay on root
    {
        if (turn == 1) {
            position[1] &= ~(1 << prevmove);
        } else {
            position[0] &= ~(1 << prevmove);
        }
    }

    inline auto pos_filled(const short i) const -> bool {
        return position[0] & (1 << i) || position[1] & (1 << i);
    }

    inline auto player_at(const short i) const -> bool  //only valid to use if pos_filled() returns true, true = x, false = y
    {
        return (position[0] & (1 << i));
    }

    inline auto is_full() const -> bool {
        return position[0] + position[1] == 0b111111111;
    }

    inline auto evaluate() const -> short {
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

    void show() const {
        for (int x = 0; x < 3; x++) {
            for (int y = 0; y < 3; y++) {
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

    inline auto is_board_dead() const -> bool {
        // if (cached_death_state) return true;
        if (is_full()) {
            // cached_death_state = true;
            return true;
        }
        if (evaluate() != 0) {
            // cached_death_state = true;
            return true;
        }
        return false;
    }

    auto get_square_as_char(const int square) const -> char {
        if (!pos_filled(square)) {
            return '.';
        } else {
            if (player_at(square)) {
                return 'X';
            } else {
                return 'O';
            }
        }
    }
};
}  // namespace Board

namespace UTTT {
using Move = short;
constexpr auto gameexpfactor = 5;
class State {
   public:
    std::array<Board::SubState, 9> metaposition;
    short forcingBoard;
    short turn;
    std::vector<short> movestack;     // reserve for the playout boards
    std::vector<short> forcingstack;  // reserve for the playout boards

    State() {
        movestack.reserve(16);
        forcingstack.reserve(16);
        for (Board::SubState &board : metaposition) {
            board = Board::SubState();
        }
        forcingBoard = -1;
        forcingstack.push_back(forcingBoard);
        turn = 1;
    }
    State(const UTTT::State &inputState) {
        metaposition = inputState.metaposition;
        forcingBoard = inputState.forcingBoard;
        turn = inputState.turn;
        forcingstack = inputState.forcingstack;
    }

    void show_debug() const {
        show();
        std::cout << "turn: " << turn << '\n';
        std::cout << "next forced board: " << forcingBoard << '\n';
        std::cout << "played moves: " << string(movestack) << '\n';
        std::cout << "previous forced boards: " << string(forcingstack) << '\n';
        int counter = 0;
        for (const auto &substate : metaposition)
        {
            std::cout << "substate " << counter << ":\n";
            substate.show();
        }
    }

    inline void mem_setup() {
        movestack.reserve(81);
        forcingstack.reserve(81);
    }

    void reset() {
        std::for_each(
            metaposition.begin(), 
            metaposition.end(), 
            [](Board::SubState &b) { b.reset(); }
        );
    }

    inline void play(const short i) {
        const short board = i / 9;
        const short square = i % 9;
        metaposition[board].play(square, turn);
        movestack.push_back(i);
        turn = -turn;
        forcingBoard = square;
        forcingstack.push_back(forcingBoard);
    }

    inline void unplay()  // do not unplay on root
    {
        const short prevmove = movestack.back();
        const short board = prevmove / 9;
        const short square = prevmove % 9;
        movestack.pop_back();
        metaposition[board].unplay(square, turn);
        turn = -turn;
        forcingstack.pop_back();
        forcingBoard = forcingstack.back();
    }

    inline auto board_won(const short board) const -> bool {
        return metaposition[board].evaluate() != 0;
    }

    inline auto board_over(const short board) const -> bool {
        return metaposition[board].is_board_dead();
    }

    inline auto winner_of_board(const short board) const -> bool  //only valid to use if pos_filled() returns true, true = x, false = y
    {
        return metaposition[board].evaluate() == 1;
    }

    inline auto is_full() -> bool {
        return std::all_of(metaposition.begin(), metaposition.end(), [](Board::SubState p) { return p.is_board_dead(); });
    }

    inline auto evaluate() const -> short {
        // check first diagonal
        if (board_over(0) && board_over(4) && board_over(8)) {
            if (winner_of_board(0) == winner_of_board(4) && winner_of_board(4) == winner_of_board(8)) {
                if (winner_of_board(0))
                    return 1;
                else
                    return -1;
            }
        }
        // check second diagonal
        if (board_over(2) && board_over(4) && board_over(6)) {
            if (winner_of_board(2) == winner_of_board(4) && winner_of_board(4) == winner_of_board(6)) {
                if (winner_of_board(2))
                    return 1;
                else
                    return -1;
            }
        }
        // check rows
        for (short i = 0; i < 3; i++) {
            if (board_over(i * 3) && board_over(i * 3 + 1) && board_over(i * 3 + 2)) {
                if (winner_of_board(i * 3) == winner_of_board(i * 3 + 1) && winner_of_board(i * 3 + 1) == winner_of_board(i * 3 + 2)) {
                    if (winner_of_board(i * 3))
                        return 1;
                    else
                        return -1;
                }
            }
        }
        // check columns
        for (short i = 0; i < 3; i++) {
            if (board_over(i) && board_over(i + 3) && board_over(i + 6)) {
                if (winner_of_board(i) == winner_of_board(i + 3) && winner_of_board(i + 3) == winner_of_board(i + 6)) {
                    if (winner_of_board(i))
                        return 1;
                    else
                        return -1;
                }
            }
        }
        short xwon = 0;
        short owon = 0;
        for (short i = 0; i < 9; i++) {
            if (board_over(i)) {
                if (winner_of_board(i)) {
                    xwon++;
                } else {
                    owon++;
                }
            } else {
                return 0;
            }
        }
        if (xwon > owon) {
            return 1;
        } else {
            return -1;
        }
    }

    inline void pass_turn() {
        turn = -turn;
    }

    inline auto is_game_over() -> bool {
        if (num_legal_moves() == 0)
            return true;
        return (evaluate() != 0);
    }

    void show() const {
        for (int x = 0; x < 3; x++) {
            for (int y = 0; y < 3; y++) {
                if (board_over(x * 3 + y)) {
                    if (winner_of_board(x * 3 + y))
                        std::cout << "X ";
                    else
                        std::cout << "0 ";
                } else
                    std::cout << ". ";
            }
            std::cout << "\n";
        }
        std::cout << "\n";
        int board, square;
        const std::array<int, 81> ordering = {
            0, 1, 2, 9, 10, 11, 18, 19, 20, 3, 4, 5, 12, 13, 14, 21, 22, 23, 6, 7, 8, 15, 16, 17, 24, 25, 26, 27, 28, 29, 36, 37, 38, 45, 46, 47, 30, 31, 32, 39, 40, 41, 48, 49, 50, 33, 34, 35, 42, 43, 44, 51, 52, 53, 54, 55, 56, 63, 64, 65, 72, 73, 74, 57, 58, 59, 66, 67, 68, 75, 76, 77, 60, 61, 62, 69, 70, 71, 78, 79, 80};
        int counter = 0;
        std::string linebreak = " |-----------------------|\n";
        for (const int i : ordering) {
            board = i / 9;
            square = i % 9;
            if (counter % 9 == 0 && i != 0)
                std::cout << " |\n";
            if (i == 0 || i == 27 || i == 54)
                std::cout << linebreak;
            if (counter % 3 == 0)
                std::cout << " |";
            std::cout << ' ' << metaposition[board].get_square_as_char(square);
            counter++;
        }
        std::cout << " |\n";
        std::cout << linebreak << "\n\n";
    }

    auto num_legal_moves() -> short {
        if (forcingBoard != -1)
            return 9 - popcount(metaposition[forcingBoard].union_bb());
        short cnt = 0;
        for (short i = 0; i < 9; i++) {
            if (!metaposition[i].is_board_dead())
                cnt += 9 - popcount(metaposition[i].union_bb());
        }
        return cnt;
    }

    // inline auto num_legal_moves() -> short {
    //     if (forcingBoard != -1)  // if we are being forced somewhere
    //         if (!metaposition[forcingBoard].is_board_dead())
    //             return 9 - popcount(metaposition[forcingBoard].union_bb());
       
    //     return std::accumulate(
    //         metaposition.begin(), 
    //         metaposition.end(), 
    //         0, 
    //         [](const int a, const Board::SubState &b) { 
    //             return a + (b.is_board_dead() ? 0 : 9 - popcount(b.union_bb())); 
    //         }
    //     );
    // }

    inline auto legal_moves() -> std::vector<Move> {
        std::vector<Move> moves;
        if (metaposition[forcingBoard].is_board_dead())
            forcingBoard = -1;
        if (forcingBoard == -1){
            moves.reserve(81);
            auto bcounter = 0;
            for (const auto &board : metaposition)
            {
                if (!board.is_board_dead()){
                    auto bb = ~board.union_bb() & 0b111111111;
                    for (; bb;) {
                        moves.push_back(bcounter * 9 + (lsb(bb)));
                        bb &= bb - 1;  // clear the least significant bit set
                    }
                }
                bcounter++;
            }
        } else {
            moves.reserve(9);
            auto bb = ~metaposition[forcingBoard].union_bb() & 0b111111111;
            for (; bb;) {
                moves.push_back(forcingBoard * 9 + (lsb(bb)));
                bb &= bb - 1;  // clear the least significant bit set
            }
        }
        return moves;
    }

    inline void random_play() {
        std::vector<Move> moves = legal_moves();
        play(moves[rand() % moves.size()]);
    }
};

bool operator==(const State &a, const State &b) {
    if (a.forcingBoard != b.forcingBoard)
        return false;
    if (a.turn != b.turn)
        return false;
    for (short i = 0; i < 9; i++) {
        if (a.metaposition[i].position[0] != b.metaposition[i].position[0])
            return false;
        if (a.metaposition[i].position[1] != b.metaposition[i].position[1])
            return false;
    }
    if (a.forcingstack != b.forcingstack)
        return false;
    return a.movestack == b.movestack;
}
}  // namespace UTTT

// int main()
// {
//     UTTT::State board = UTTT::State();
//     board.show();
//     while (!board.is_game_over())
//     {
//         board.random_play();
//         board.show();
//     }
//     return 0;
// }