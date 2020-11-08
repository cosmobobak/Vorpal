#include <iostream>
#include <algorithm>
#include <cmath>
#include <vector>

class Coin
{
    public:
        char node[6][7] = {
            {'.', '.', '.', '.', '.', '.', '.'},
            {'.', '.', '.', '.', '.', '.', '.'},
            {'.', '.', '.', '.', '.', '.', '.'},
            {'.', '.', '.', '.', '.', '.', '.'},
            {'.', '.', '.', '.', '.', '.', '.'},
            {'.', '.', '.', '.', '.', '.', '.'},
        };
        int turn = 1;
        int nodes = 0;
        char players[2] = {'X', 'O'};

        void reset()
        {
            for (int row = 0; row < 6; row++)
            {
                for (int col = 0; col < 7; col++)
                {
                    node[row][col] = '.';
                }
            }
        }

        void fill()
        {
            for (int row = 0; row < 6; row++)
            {
                for (int col = 0; col < 7; col++)
                {
                    node[row][col] = 'F';
                }
            }
        }

        auto is_full() -> bool //WORKING
        {
            for (int row = 0; row < 6; row++){
                for (int col = 0; col < 7; col++){
                    if (node[row][col] == '.'){
                        return false;
                    }
                }
            }
            return true;
        }

        void show() //WORKING
        {
            int row, col;
            for (row = 0; row < 6; ++row){
                for (col = 0; col < 7; ++col){
                    std::cout << node[row][col] << ' ';
                }
                std::cout << '\n';
            }
            std::cout << '\n';
        }

        auto legal_moves() -> std::vector<int>
        {
            std::vector<int> moves;
            for (int col = 0; col < 7; col++)
            {
                if (node[0][col] != '.')
                {
                    moves.push_back(col);
                }
            }
            return moves;
        }

        void play(int col) //WORKING
        {
            for (int row = 0; row < 6; row++){
                if (node[row][col] != '.'){
                    if (turn == 1){
                        node[row - 1][col] = players[0];
                        turn = -1;
                        break;
                    }else{
                        node[row - 1][col] = players[1];
                        turn = 1;
                        break;
                    }
                }else if (row == 5){
                    if (turn == 1){
                        node[row][col] = players[0];
                        turn = -1;
                    }else{
                        node[row][col] = players[1];
                        turn = 1;
                    }
                }
            }
        }

        void unplay(int col) //WORKING
        {
            for (int row = 0; row < 6; row++){
                if (node[row][col] != '.'){
                    node[row][col] = '.';
                    break;
                }
            }
            if (turn == 1)
            {
                turn = -1;
            }else{
                turn = 1;
            }
        }

        auto horizontal_term() -> int
        {
            int score;
            score = 0;
            for (int row = 0; row < 6; row++){
                for (int col = 0; col < 4; col++){
                    if (node[row][col] == node[row][col + 1] && 
                        node[row][col + 1] == node[row][col + 2] && 
                        node[row][col + 2] == node[row][col + 3])
                    {
                        if(node[row][col] == players[0]){
                            score += 1;
                        }
                        else if (node[row][col] == players[1]){
                            score -= 1;
                        }
                    }
                }
            }
            return score;
        }

        auto vertical_term() -> int
        {
            int score;
            score = 0;
            for (int row = 0; row < 3; row++){
                for (int col = 0; col < 7; col++){
                    if (node[row][col] == node[row + 1][col] &&
                        node[row + 1][col] == node[row + 2][col] &&
                        node[row + 2][col] == node[row + 3][col])
                    {
                        if(node[row][col] == players[0]){
                            score += 1;
                        }
                        else if (node[row][col] == players[1]){
                            score -= 1;
                        }
                    }
                }
            }
            return score;
        }

        auto diagup_term() -> int
        {
            int score;
            score = 0;
            for (int row = 3; row < 6; row++){
                for (int col = 0; col < 4; col++){
                    if (node[row][col] == node[row - 1][col + 1] &&
                        node[row - 1][col + 1] == node[row - 2][col + 2] &&
                        node[row - 2][col + 2] == node[row - 3][col + 3])
                    {
                        if (node[row][col] == players[0]){
                            score += 1;
                        }
                        else if (node[row][col] == players[1]){
                            score -= 1;
                        }
                    }
                }
            }
            return score;
        }

        auto diagdown_term() -> int
        {
            int score;
            score = 0;
            for (int row = 0; row < 3; row++){
                for (int col = 0; col < 4; col++){
                    if (node[row][col] == node[row + 1][col + 1] &&
                        node[row + 1][col + 1] == node[row + 2][col + 2] &&
                        node[row + 2][col + 2] == node[row + 3][col + 3])
                    {
                        if (node[row][col] == players[0]){
                            score += 1;
                        }
                        else if (node[row][col] == players[1]){
                            score -= 1;
                        }
                    }
                }
            }
            return score;
        }

        auto evaluate() -> int
        {
            int v, h, u, d;
            v = vertical_term();
            h = horizontal_term();
            u = diagup_term();
            d = diagdown_term();

            return v + h + u + d;
        }

        auto rollout() -> int
        {
            
        }

        auto negamax(int depth = 10, int colour = 1, int a = -2, int b = 2) //WORKING
        {
            if(evaluate() != 0 || is_full() || depth < 1){
                return colour * evaluate();
            }
            int score;
            
            for (auto col : legal_moves())
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
            
            return a;
        }

        auto max_pos(int arr[]) -> int //WORKING
        {
            int max, index;
            max = arr[0];
            index = 0;
            for (int i = 0; i < 7; i++){
                if (arr[i] > max){
                    max = arr[i];
                    index = i;
                }
            }
            return index;
        }

        auto min_pos(float arr[]) -> int //WORKING
        {
            float max, index;
            max = arr[0];
            index = 0;
            for (int i = 0; i < 7; i++){
                if (arr[i] < max){
                    max = arr[i];
                    index = i;
                }
            }
            return index;
        }

        void engine_move() //WORKING
        {
            int x, y, index;
            float scores [7] = {2,2,2,2,2,2,2};
            
            for (int col = 0; col < 7; col++){
                if (node[0][col] == '.'){
                    play(col);
                    scores[col] = negamax(4, turn, -2, 2);
                    unplay(col);
                }
            }
            for (float i = 0; i < 7; i++)
            {
                scores[static_cast<int>(i)] -= 1-(abs(i-3)+3)/6;
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

        auto is_game_over() -> int
        {
            if (evaluate() != 0 || is_full() == true){
                return true;
            }else{
                return false;
            }
        }
};

int main()
{
    Coin coin;
    coin.show();
    int col;
    while (coin.is_game_over() == false)
    {
        coin.engine_move();
        coin.show();
    }
    return 0;
}