#include <iostream>
#include <string>

int corners[9] = {0, 3, 6, 27, 30, 33, 54, 57, 60};

int iteration[9] = {0, 1, 2, 9, 10, 11, 18, 19, 20};

class Board
{
public:
    int matrix[81];
    Board()
    {
        for (int i = 0; i < 81; i++)
        {
            matrix[i] = -1;
        }
        int square;
        int value;
        show();
        std::cout << "enter square for known number, or -1 to exit: ";
        std::cin >> square;
        while (square != -1)
        {
            std::cout << "enter value: ";
            std::cin >> value;
            matrix[square] = value;
            std::cout << "enter square for known number, or -1 to exit: ";
            std::cin >> square;
        }
        show();
    }

    auto view_character(int c) -> std::string
    {
        if (c == -1)
        {
            return ".";
        }
        else
        {
            return std::to_string(c);
        }
    }

    void show()
    {
        for (int i = 0; i < 81; i++)
        {
            if (i % 3 == 2 && i % 9 != 8)
            {
                std::cout << view_character(matrix[i]) << " | ";
            }
            else
            {
                std::cout << view_character(matrix[i]) << ' ';
            }
            if (i % 9 == 8)
            {
                std::cout << "\n";
                if (i == 26 || i == 53)
                {
                    std::cout << "---------------------\n";
                }
            }
        }
    }

    auto in_square(int space) -> bool
    {
        int compressedRow = (space % 9) / 3;
        int compressedCol = (space / 9) / 3;
        int square = compressedRow + 3 * compressedCol;
        //std::cout << square;
        int corner = corners[square];
        for (int i = 0; i < 9; i++)
        {
            if (matrix[square] == matrix[iteration[i] + corner] && square != iteration[i] + corner)
            {
                return true;
            }
        }
        return false;
    }

    auto in_row(int space) -> bool
    {
        int row = space / 9;
        //std::cout << row;
        for (int col = 0; col < 9; col++)
        {
            if (matrix[space] == matrix[row * 9 + col] && space != row * 9 + col)
            {
                return true;
            }
        }
        return false;
    }

    auto in_col(int space) -> bool
    {
        int col = space % 9;
        //std::cout << col;
        for (int row = 0; row < 9; row++)
        {
            if (matrix[space] == matrix[row + 9 * col] && space != row + 9 * col)
            {
                return true;
            }
        }
        return false;
    }

    auto valid(int space) -> bool
    {
        if (in_row(space))
        {
            return false;
        }
        else if (in_col(space))
        {
            return false;
        }
        else if (in_square(space))
        {
            return false;
        }
        else
        {
            return true;
        }
    }

    void make(int s, int v)
    {
        matrix[s] = v;
    }

    void unmake(int s)
    {
        matrix[s] = -1;
    }

    auto solved() -> bool
    {
        for (int i = 0; i < 81; i++)
        {
            if (!valid(i))
            {
                return false;
            }
        }
        return true;
    }

    auto is_full(int s) -> bool
    {
        if (matrix[s] != -1)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    void solve(int begin)
    {
        for (int i = begin; i < 81; i++)
        {
            if (is_full(i))
            {
                continue;
            }
            else
            {
            }
        }
    }
};

auto main() -> int
{
    Board board = Board();
    while (board.solved() == false)
    {
    }
    return 0;
}