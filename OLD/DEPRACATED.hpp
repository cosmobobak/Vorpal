auto knight_move_pregenerator(int square, int dir) -> U64  // slow as hell
{
    int offsets[] = {17, 15, 10, 6, -17, -15, -10, -6};
    int r = row(square);
    int c = col(square);
    U64 out;
    if (square + offsets[dir] >= 0 && square + offsets[dir] <= 63) {
        out = 1ULL << (square + offsets[dir]);
    } else {
        return 0;
    }

    if (r == 0 || r == 1) {
        out &= ~(BB_RANK_7 | BB_RANK_8);
    } else if (r == 7 || r == 6) {
        out &= ~(BB_RANK_2 | BB_RANK_1);
    }
    if (c == 0 || c == 1) {
        out &= ~(BB_FILE_G | BB_FILE_H);
    } else if (c == 7 || c == 6) {
        out &= ~(BB_FILE_A | BB_FILE_B);
    }
    return out;
}

auto king_move_pregenerator(int square, int dir) -> U64  // slow as hell
{
    int offsets[] = {9, 8, 7, 1, -9, -8, -7, -1};
    int r = row(square);
    int c = col(square);
    U64 out;
    if (square + offsets[dir] >= 0 && square + offsets[dir] <= 63) {
        out = 1LL << (square + offsets[dir]);
    } else {
        return 0;
    }

    if (r == 0 || r == 1) {
        out &= ~(BB_RANK_7 | BB_RANK_8);
    } else if (r == 7 || r == 6) {
        out &= ~(BB_RANK_2 | BB_RANK_1);
    }
    if (c == 0 || c == 1) {
        out &= ~(BB_FILE_G | BB_FILE_H);
    } else if (c == 7 || c == 6) {
        out &= ~(BB_FILE_A | BB_FILE_B);
    }
    return out;
}

auto bitboard_split(U64 bb) -> std::vector<U64> {
    std::vector<U64> out;
    while (bb) {
        U64 ls1b = bb & -bb;  // isolate LS1B
        out.push_back(ls1b);
        bb &= bb - 1;  // reset LS1B
    }
    return out;
}