// # This file is part of the python-chess library.
// # Copyright (C) 2012-2020 Niklas Fiekas <niklas.fiekas@backscattering.de>
// #
// # This program is free software: you can redistribute it and/or modify
// # it under the terms of the GNU General Public License as published by
// # the Free Software Foundation, either version 3 of the License, or
// # (at your option) any later version.
// #
// # This program is distributed in the hope that it will be useful,
// # but WITHOUT ANY WARRANTY; without even the implied warranty of
// # MERCHANTABILITY || FITNESS FOR A PARTICULAR PURPOSE. See the
// # GNU General Public License for more details.
// #
// # You should have received a copy of the GNU General Public License
// # along with this program. If not, see <http://www.gnu.org/licenses/>.

// """
// A chess library with move generation and validation,
// Polyglot opening book probing, PGN reading and writing,
// Gaviota tablebase probing,
// Syzygy tablebase probing, and XBoard/UCI engine communication.
// """

// from __future__ import annotations

// __author__ = "Niklas Fiekas"

// __email__ = "niklas.fiekas@backscattering.de"

// __version__ = "1.3.3"

// import collections
// import copy
// import dataclasses
// import enum
// import math
// import re
// import itertools
// import typing

// from typing import ClassVar, Callable, Counter, Dict, Generic, Hashable, Iterable, Iterator, List, Mapping, Optional, SupportsInt, Tuple, Type, TypeVar, Union

// try:
//     from typing import Literal
//     _EnPassantSpec = Literal["legal", "fen", "xfen"]
// except ImportError:
//     # Before Python 3.8.
//     _EnPassantSpec = str  # type: ignore

#include <algorithm>
#include <array>
#include <bitset>
#include <cmath>
#include <iostream>
#include <map>
#include <numeric>
#include <regex>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include "pregenerated_chess_tables.hpp"

using Color = bool;
enum COLORS {
    WHITE,
    BLACK
};
std::array<bool, 2> COLORS = {true, false};
std::array<std::string, 2> COLOR_NAMES = {"black", "white"};

using PieceType = int;
enum PIECE_TYPES {
    PAWN = 1,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING
};
std::array<char, 7> PIECE_SYMBOLS = {' ', 'p', 'n', 'b', 'r', 'q', 'k'};
std::array<std::string, 7> PIECE_NAMES = {"NONE", "pawn", "knight", "bishop", "rook", "queen", "king"};

auto piece_symbol(PieceType piece_type) -> char {
    return PIECE_SYMBOLS[piece_type];
}

auto piece_name(PieceType piece_type) -> std::string {
    return PIECE_NAMES[piece_type];
}

std::map<char, std::string> UNICODE_PIECE_SYMBOLS = {
    {'R', "♖"},
    {'r', "♜"},
    {'N', "♘"},
    {'n', "♞"},
    {'B', "♗"},
    {'b', "♝"},
    {'Q', "♕"},
    {'q', "♛"},
    {'K', "♔"},
    {'k', "♚"},
    {'P', "♙"},
    {'p', "♟"},
};

std::array<char, 8> FILE_NAMES = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};

std::array<char, 8> RANK_NAMES = {'1', '2', '3', '4', '5', '6', '7', '8'};

constexpr auto STARTING_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
// """The FEN for the standard chess starting position."""

constexpr auto STARTING_BOARD_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
// """The board part of the FEN for the standard chess starting position."""

namespace Status {
constexpr int VALID = 0;
constexpr int NO_WHITE_KING = 1 << 0;
constexpr int NO_BLACK_KING = 1 << 1;
constexpr int TOO_MANY_KINGS = 1 << 2;
constexpr int TOO_MANY_WHITE_PAWNS = 1 << 3;
constexpr int TOO_MANY_BLACK_PAWNS = 1 << 4;
constexpr int PAWNS_ON_BACKRANK = 1 << 5;
constexpr int TOO_MANY_WHITE_PIECES = 1 << 6;
constexpr int TOO_MANY_BLACK_PIECES = 1 << 7;
constexpr int BAD_CASTLING_RIGHTS = 1 << 8;
constexpr int INVALID_EP_SQUARE = 1 << 9;
constexpr int OPPOSITE_CHECK = 1 << 10;
constexpr int EMPTY = 1 << 11;
constexpr int RACE_CHECK = 1 << 12;
constexpr int RACE_OVER = 1 << 13;
constexpr int RACE_MATERIAL = 1 << 14;
constexpr int TOO_MANY_CHECKERS = 1 << 15;
constexpr int IMPOSSIBLE_CHECK = 1 << 16;
};  // namespace Status

constexpr int STATUS_VALID = Status::VALID;
constexpr int STATUS_NO_WHITE_KING = Status::NO_WHITE_KING;
constexpr int STATUS_NO_BLACK_KING = Status::NO_BLACK_KING;
constexpr int STATUS_TOO_MANY_KINGS = Status::TOO_MANY_KINGS;
constexpr int STATUS_TOO_MANY_WHITE_PAWNS = Status::TOO_MANY_WHITE_PAWNS;
constexpr int STATUS_TOO_MANY_BLACK_PAWNS = Status::TOO_MANY_BLACK_PAWNS;
constexpr int STATUS_PAWNS_ON_BACKRANK = Status::PAWNS_ON_BACKRANK;
constexpr int STATUS_TOO_MANY_WHITE_PIECES = Status::TOO_MANY_WHITE_PIECES;
constexpr int STATUS_TOO_MANY_BLACK_PIECES = Status::TOO_MANY_BLACK_PIECES;
constexpr int STATUS_BAD_CASTLING_RIGHTS = Status::BAD_CASTLING_RIGHTS;
constexpr int STATUS_INVALID_EP_SQUARE = Status::INVALID_EP_SQUARE;
constexpr int STATUS_OPPOSITE_CHECK = Status::OPPOSITE_CHECK;
constexpr int STATUS_EMPTY = Status::EMPTY;
constexpr int STATUS_RACE_CHECK = Status::RACE_CHECK;
constexpr int STATUS_RACE_OVER = Status::RACE_OVER;
constexpr int STATUS_RACE_MATERIAL = Status::RACE_MATERIAL;
constexpr int STATUS_TOO_MANY_CHECKERS = Status::TOO_MANY_CHECKERS;
constexpr int STATUS_IMPOSSIBLE_CHECK = Status::IMPOSSIBLE_CHECK;

using Square = int;
enum eSQUARES {
    A1,
    B1,
    C1,
    D1,
    E1,
    F1,
    G1,
    H1,
    A2,
    B2,
    C2,
    D2,
    E2,
    F2,
    G2,
    H2,
    A3,
    B3,
    C3,
    D3,
    E3,
    F3,
    G3,
    H3,
    A4,
    B4,
    C4,
    D4,
    E4,
    F4,
    G4,
    H4,
    A5,
    B5,
    C5,
    D5,
    E5,
    F5,
    G5,
    H5,
    A6,
    B6,
    C6,
    D6,
    E6,
    F6,
    G6,
    H6,
    A7,
    B7,
    C7,
    D7,
    E7,
    F7,
    G7,
    H7,
    A8,
    B8,
    C8,
    D8,
    E8,
    F8,
    G8,
    H8,
};

auto _generate_square_names() -> std::array<std::array<char, 2>, 64> {
    std::array<std::array<char, 2>, 64> outarray;
    for (int f = 0; f < 8; f++)  // (auto &&f : FILE_NAMES)
    {
        for (int r = 0; r < 8; r++)  // (auto &&r : RANK_NAMES)
        {
            outarray[f * 8 + r][0] = FILE_NAMES[f];
            outarray[f * 8 + r][1] = RANK_NAMES[r];
        }
    }
    return outarray;
}
//constexpr std::array<std::array<char, 2>, 64> SQUARE_NAMES = _generate_square_names();
using SquareString = std::array<char, 3>;
constexpr std::array<SquareString, 64> SQUARE_NAMES = {"a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1", "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2", "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3", "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4", "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5", "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6", "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7", "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8"};

auto parse_square(SquareString name) -> Square  // not done
{
    // """
    // Gets the square index for the given square *name*
    // (e.g., ``a1`` returns ``0``).
    // :raises: :exc:`ValueError` if the square name is invalid.
    // """
    // return SQUARE_NAMES.index(name);
    for (int i = 0; i < 64; i++) {
        if (name[0] == SQUARE_NAMES[i][0] && name[1] == SQUARE_NAMES[i][1])
            return i;
    }
    return -1;
}

auto square_name(Square s) -> SquareString {
    // """Gets the name of the square, like ``a3``."""
    return SQUARE_NAMES[s];
}

auto square(int file_index, int rank_index) -> Square {
    // """Gets a square number by file and rank index."""
    return rank_index * 8 + file_index;
}

auto square_file(Square s) -> int {
    // """Gets the file index of the square where ``0`` is the a-file."""
    return s & 7;
}

auto square_rank(Square s) -> int {
    // """Gets the rank index of the square where ``0`` is the first rank."""
    return s >> 3;
}

auto square_distance(Square a, Square b) -> int {
    // """
    // Gets the distance (i.e., the number of king steps) from square *a* to *b*.
    // """
    return std::max(abs(square_file(a) - square_file(b)), abs(square_rank(a) - square_rank(b)));
}

auto square_mirror(Square s) -> Square {  //"""Mirrors the square vertically."""
    return s ^ 0x38;
}

std::array<int, 64> SQUARES = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63};
std::array<int, 64> SQUARES_180 = {56, 57, 58, 59, 60, 61, 62, 63, 48, 49, 50, 51, 52, 53, 54, 55, 40, 41, 42, 43, 44, 45, 46, 47, 32, 33, 34, 35, 36, 37, 38, 39, 24, 25, 26, 27, 28, 29, 30, 31, 16, 17, 18, 19, 20, 21, 22, 23, 8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7};

using Bitboard = unsigned long long;
constexpr Bitboard BB_EMPTY = 0ULL;
constexpr Bitboard BB_ALL = 0xffffffffffffffffULL;

constexpr Bitboard BB_A1 = 1ULL << 0;
constexpr Bitboard BB_B1 = 1ULL << 1;
constexpr Bitboard BB_C1 = 1ULL << 2;
constexpr Bitboard BB_D1 = 1ULL << 3;
constexpr Bitboard BB_E1 = 1ULL << 4;
constexpr Bitboard BB_F1 = 1ULL << 5;
constexpr Bitboard BB_G1 = 1ULL << 6;
constexpr Bitboard BB_H1 = 1ULL << 7;
constexpr Bitboard BB_A2 = 1ULL << 8;
constexpr Bitboard BB_B2 = 1ULL << 9;
constexpr Bitboard BB_C2 = 1ULL << 10;
constexpr Bitboard BB_D2 = 1ULL << 11;
constexpr Bitboard BB_E2 = 1ULL << 12;
constexpr Bitboard BB_F2 = 1ULL << 13;
constexpr Bitboard BB_G2 = 1ULL << 14;
constexpr Bitboard BB_H2 = 1ULL << 15;
constexpr Bitboard BB_A3 = 1ULL << 16;
constexpr Bitboard BB_B3 = 1ULL << 17;
constexpr Bitboard BB_C3 = 1ULL << 18;
constexpr Bitboard BB_D3 = 1ULL << 19;
constexpr Bitboard BB_E3 = 1ULL << 20;
constexpr Bitboard BB_F3 = 1ULL << 21;
constexpr Bitboard BB_G3 = 1ULL << 22;
constexpr Bitboard BB_H3 = 1ULL << 23;
constexpr Bitboard BB_A4 = 1ULL << 24;
constexpr Bitboard BB_B4 = 1ULL << 25;
constexpr Bitboard BB_C4 = 1ULL << 26;
constexpr Bitboard BB_D4 = 1ULL << 27;
constexpr Bitboard BB_E4 = 1ULL << 28;
constexpr Bitboard BB_F4 = 1ULL << 29;
constexpr Bitboard BB_G4 = 1ULL << 30;
constexpr Bitboard BB_H4 = 1ULL << 31;
constexpr Bitboard BB_A5 = 1ULL << 32;
constexpr Bitboard BB_B5 = 1ULL << 33;
constexpr Bitboard BB_C5 = 1ULL << 34;
constexpr Bitboard BB_D5 = 1ULL << 35;
constexpr Bitboard BB_E5 = 1ULL << 36;
constexpr Bitboard BB_F5 = 1ULL << 37;
constexpr Bitboard BB_G5 = 1ULL << 38;
constexpr Bitboard BB_H5 = 1ULL << 39;
constexpr Bitboard BB_A6 = 1ULL << 40;
constexpr Bitboard BB_B6 = 1ULL << 41;
constexpr Bitboard BB_C6 = 1ULL << 42;
constexpr Bitboard BB_D6 = 1ULL << 43;
constexpr Bitboard BB_E6 = 1ULL << 44;
constexpr Bitboard BB_F6 = 1ULL << 45;
constexpr Bitboard BB_G6 = 1ULL << 46;
constexpr Bitboard BB_H6 = 1ULL << 47;
constexpr Bitboard BB_A7 = 1ULL << 48;
constexpr Bitboard BB_B7 = 1ULL << 49;
constexpr Bitboard BB_C7 = 1ULL << 50;
constexpr Bitboard BB_D7 = 1ULL << 51;
constexpr Bitboard BB_E7 = 1ULL << 52;
constexpr Bitboard BB_F7 = 1ULL << 53;
constexpr Bitboard BB_G7 = 1ULL << 54;
constexpr Bitboard BB_H7 = 1ULL << 55;
constexpr Bitboard BB_A8 = 1ULL << 56;
constexpr Bitboard BB_B8 = 1ULL << 57;
constexpr Bitboard BB_C8 = 1ULL << 58;
constexpr Bitboard BB_D8 = 1ULL << 59;
constexpr Bitboard BB_E8 = 1ULL << 60;
constexpr Bitboard BB_F8 = 1ULL << 61;
constexpr Bitboard BB_G8 = 1ULL << 62;
constexpr Bitboard BB_H8 = 1ULL << 63;

constexpr Bitboard BB_CORNERS = BB_A1 | BB_H1 | BB_A8 | BB_H8;
constexpr Bitboard BB_CENTER = BB_D4 | BB_E4 | BB_D5 | BB_E5;

constexpr Bitboard BB_LIGHT_SQUARES = 0x55aa55aa55aa55aaULL;
constexpr Bitboard BB_DARK_SQUARES = 0xaa55aa55aa55aa55ULL;

const std::array<Bitboard, 64> BB_SQUARES = {
    BB_A1, BB_B1, BB_C1, BB_D1, BB_E1, BB_F1, BB_G1, BB_H1, BB_A2, BB_B2, BB_C2, BB_D2, BB_E2, BB_F2, BB_G2, BB_H2, BB_A3, BB_B3, BB_C3, BB_D3, BB_E3, BB_F3, BB_G3, BB_H3, BB_A4, BB_B4, BB_C4, BB_D4, BB_E4, BB_F4, BB_G4, BB_H4, BB_A5, BB_B5, BB_C5, BB_D5, BB_E5, BB_F5, BB_G5, BB_H5, BB_A6, BB_B6, BB_C6, BB_D6, BB_E6, BB_F6, BB_G6, BB_H6, BB_A7, BB_B7, BB_C7, BB_D7, BB_E7, BB_F7, BB_G7, BB_H7, BB_A8, BB_B8, BB_C8, BB_D8, BB_E8, BB_F8, BB_G8, BB_H8};

constexpr Bitboard BB_FILE_A = 0x0101010101010101ULL << 0;
constexpr Bitboard BB_FILE_B = 0x0101010101010101ULL << 1;
constexpr Bitboard BB_FILE_C = 0x0101010101010101ULL << 2;
constexpr Bitboard BB_FILE_D = 0x0101010101010101ULL << 3;
constexpr Bitboard BB_FILE_E = 0x0101010101010101ULL << 4;
constexpr Bitboard BB_FILE_F = 0x0101010101010101ULL << 5;
constexpr Bitboard BB_FILE_G = 0x0101010101010101ULL << 6;
constexpr Bitboard BB_FILE_H = 0x0101010101010101ULL << 7;

const std::array<Bitboard, 8> BB_FILES = {
    BB_FILE_A,
    BB_FILE_B,
    BB_FILE_C,
    BB_FILE_D,
    BB_FILE_E,
    BB_FILE_F,
    BB_FILE_G,
    BB_FILE_H};

constexpr Bitboard BB_RANK_1 = 0xffULL << (8 * 0);
constexpr Bitboard BB_RANK_2 = 0xffULL << (8 * 1);
constexpr Bitboard BB_RANK_3 = 0xffULL << (8 * 2);
constexpr Bitboard BB_RANK_4 = 0xffULL << (8 * 3);
constexpr Bitboard BB_RANK_5 = 0xffULL << (8 * 4);
constexpr Bitboard BB_RANK_6 = 0xffULL << (8 * 5);
constexpr Bitboard BB_RANK_7 = 0xffULL << (8 * 6);
constexpr Bitboard BB_RANK_8 = 0xffULL << (8 * 7);

const std::array<Bitboard, 8> BB_RANKS = {
    BB_RANK_1,
    BB_RANK_2,
    BB_RANK_3,
    BB_RANK_4,
    BB_RANK_5,
    BB_RANK_6,
    BB_RANK_7,
    BB_RANK_8};

constexpr Bitboard BB_BACKRANKS = BB_RANK_1 | BB_RANK_8;

/////////////////////////
const std::array<int, 64> index64 = {
    0, 1, 48, 2, 57, 49, 28, 3,
    61, 58, 50, 42, 38, 29, 17, 4,
    62, 55, 59, 36, 53, 51, 43, 22,
    45, 39, 33, 30, 24, 18, 12, 5,
    63, 47, 56, 27, 60, 41, 37, 16,
    54, 35, 52, 21, 44, 32, 23, 11,
    46, 26, 40, 15, 34, 20, 31, 10,
    25, 14, 19, 9, 13, 8, 7, 6};

auto lsb(Bitboard bitboard) -> int  // this function is just magic copied from here https://www.chessprogramming.org/BitScan#Divide_and_Conquer
{
    unsigned int lsb;
    //assert(bitboard != 0);
    bitboard &= -bitboard;  // LS1B-Isolation
    lsb = (unsigned)bitboard | (unsigned)(bitboard >> 32);
    return (((((((((((unsigned)(bitboard >> 32) != 0) * 2) + ((lsb & 0xffff0000) != 0)) * 2) + ((lsb & 0xff00ff00) != 0)) * 2) + ((lsb & 0xf0f0f0f0) != 0)) * 2) + ((lsb & 0xcccccccc) != 0)) * 2) + ((lsb & 0xaaaaaaaa) != 0);
}  // maybe try this later

// auto lsb(Bitboard bb) -> int
// {
//     const unsigned long long debruijn64 = C64(0x03f79d71b4cb0a89);
//     // assert(bb != 0);
//     return index64[((bb & -bb) * debruijn64) >> 58];
// }

auto msb(Bitboard bb) -> int  // this function is just magic copied from here https://www.chessprogramming.org/BitScan#Double_conversion
{
    union {
        double d;
        struct
        {
            unsigned int mantissal : 32;
            unsigned int mantissah : 20;
            unsigned int exponent : 11;
            unsigned int sign : 1;
        };
    } ud;
    ud.d = (double)(bb & ~(bb >> 32));  // avoid rounding error
    return ud.exponent - 1023;
}
//////////////////////

// auto scan_forward(bb: Bitboard) -> Iterator[Square]:
//     while bb:
//         r = bb & -bb
//         yield r.bit_length() - 1
//         bb ^= r

auto scan_forward(Bitboard bb) -> std::vector<int> {
    std::vector<int> bits;
    bits.reserve(32);  // because the board is unlikely to have >32 pieces on it
    for (; bb;) {
        bits.push_back(lsb(bb));
        bb &= bb - 1;  // clear the least significant bit set
    }
    return bits;
}

// auto msb(bb: Bitboard) -> int
//     return bb.bit_length() - 1

// auto scan_reversed(bb: Bitboard) -> Iterator[Square]
//     while bb:
//         r = bb.bit_length() - 1
//         yield r
//         bb ^= BB_SQUARES[r]

auto scan_reversed(Bitboard bb) -> std::vector<int> {
    std::vector<int> bits;
    bits.reserve(32);  // because the board is unlikely to have >32 pieces on it
    for (; bb;) {
        int i = msb(bb);
        bits.push_back(i);
        bb ^= 1L << i;
    }
    return bits;
}  // possibly borked due to MSB clearing

auto popcount(Bitboard bb) -> int {
    return std::bitset<64>(bb).count();
}

auto flip_vertical(Bitboard bb) -> Bitboard {
    // https://www.chessprogramming.org/Flipping_Mirroring_and_Rotating#FlipVertically
    bb = ((bb >> 8) & 0x00ff00ff00ff00ffULL) | ((bb & 0x00ff00ff00ff00ffULL) << 8);
    bb = ((bb >> 16) & 0x0000ffff0000ffffULL) | ((bb & 0x0000ffff0000ffffULL) << 16);
    bb = (bb >> 32) | ((bb & 0x00000000ffffffffULL) << 32);
    return bb;
}

auto flip_horizontal(Bitboard bb) -> Bitboard {
    // https://www.chessprogramming.org/Flipping_Mirroring_and_Rotating#MirrorHorizontally
    bb = ((bb >> 1) & 0x5555555555555555ULL) | ((bb & 0x5555555555555555ULL) << 1);
    bb = ((bb >> 2) & 0x3333333333333333ULL) | ((bb & 0x3333333333333333ULL) << 2);
    bb = ((bb >> 4) & 0x0f0f0f0f0f0f0f0fULL) | ((bb & 0x0f0f0f0f0f0f0f0fULL) << 4);
    return bb;
}
auto flip_diagonal(Bitboard bb) -> Bitboard {
    // https://www.chessprogramming.org/Flipping_Mirroring_and_Rotating#FlipabouttheDiagonal
    Bitboard t = (bb ^ (bb << 28)) & 0x0f0f0f0f00000000ULL;
    bb = bb ^ (t ^ (t >> 28));
    t = (bb ^ (bb << 14)) & 0x3333000033330000ULL;
    bb = bb ^ (t ^ (t >> 14));
    t = (bb ^ (bb << 7)) & 0x5500550055005500ULL;
    bb = bb ^ (t ^ (t >> 7));
    return bb;
}

auto flip_anti_diagonal(Bitboard bb) -> Bitboard {
    // https://www.chessprogramming.org/Flipping_Mirroring_and_Rotating#FlipabouttheAntidiagonal
    Bitboard t = bb ^ (bb << 36);
    bb = bb ^ ((t ^ (bb >> 36)) & 0xf0f0f0f00f0f0f0fULL);
    t = (bb ^ (bb << 18)) & 0xcccc0000cccc0000ULL;
    bb = bb ^ (t ^ (t >> 18));
    t = (bb ^ (bb << 9)) & 0xaa00aa00aa00aa00ULL;
    bb = bb ^ (t ^ (t >> 9));
    return bb;
}

auto shift_down(Bitboard b) -> Bitboard {
    return b >> 8;
}

auto shift_2_down(Bitboard b) -> Bitboard {
    return b >> 16;
}

auto shift_up(Bitboard b) -> Bitboard {
    return (b << 8) & BB_ALL;
}

auto shift_2_up(Bitboard b) -> Bitboard {
    return (b << 16) & BB_ALL;
}

auto shift_right(Bitboard b) -> Bitboard {
    return (b << 1) & ~BB_FILE_A & BB_ALL;
}

auto shift_2_right(Bitboard b) -> Bitboard {
    return (b << 2) & ~BB_FILE_A & ~BB_FILE_B & BB_ALL;
}

auto shift_left(Bitboard b) -> Bitboard {
    return (b >> 1) & ~BB_FILE_H;
}

auto shift_2_left(Bitboard b) -> Bitboard {
    return (b >> 2) & ~BB_FILE_G & ~BB_FILE_H;
}

auto shift_up_left(Bitboard b) -> Bitboard {
    return (b << 7) & ~BB_FILE_H & BB_ALL;
}

auto shift_up_right(Bitboard b) -> Bitboard {
    return (b << 9) & ~BB_FILE_A & BB_ALL;
}

auto shift_down_left(Bitboard b) -> Bitboard {
    return (b >> 9) & ~BB_FILE_H;
}

auto shift_down_right(Bitboard b) -> Bitboard {
    return (b >> 7) & ~BB_FILE_A;
}

auto _sliding_attacks(Square s, Bitboard occupied, std::vector<int> deltas) -> Bitboard {
    Bitboard attacks = BB_EMPTY;
    for (int delta : deltas) {
        Square sq = s;
        while (true) {
            sq += delta;
            if (!(0 <= sq < 64) || square_distance(sq, sq - delta) > 2)
                break;

            attacks |= BB_SQUARES[sq];

            if (occupied & BB_SQUARES[sq])
                break;
        }
    }
    return attacks;
}

auto _step_attacks(Square s, std::vector<int> deltas) -> Bitboard {
    return _sliding_attacks(s, BB_ALL, deltas);
}

auto _edges(Square s) -> Bitboard {
    return (((BB_RANK_1 | BB_RANK_8) & ~BB_RANKS[square_rank(s)]) |
            ((BB_FILE_A | BB_FILE_H) & ~BB_FILES[square_file(s)]));
}

auto _carry_rippler(Bitboard mask) -> std::vector<Bitboard> {
    std::vector<Bitboard> out;
    // Carry-Rippler trick to iterate subsets of mask.
    Bitboard subset = BB_EMPTY;
    while (true) {
        out.push_back(subset);
        subset = (subset - mask) & mask;
        if (!subset)
            break;
    }
    return out;
}

auto _attack_table(std::vector<int> &deltas) -> std::pair<std::vector<Bitboard>, std::vector<std::map<Bitboard, Bitboard>>> {
    std::vector<Bitboard> mask_table;
    std::vector<std::map<Bitboard, Bitboard>> attack_table;

    for (Square s : SQUARES) {
        std::map<Bitboard, Bitboard> attacks;
        Bitboard mask = _sliding_attacks(s, 0, deltas) & ~_edges(s);
        for (auto &&subset : _carry_rippler(mask)) {
            attacks[subset] = _sliding_attacks(s, subset, deltas);
        }
        attack_table.push_back(attacks);
        mask_table.push_back(mask);
    }

    return std::pair(mask_table, attack_table);
}

template <class T>
auto string(std::vector<T> v) -> std::string {
    std::string builder;
    builder.append("{ ");
    for (auto &&i : v) {
        builder.append(std::to_string(i));
        builder.append(", ");
    }
    builder.append("}");
    return builder;
}

int main(){
    std::vector<int> v1 = {-9, -7, 7, 9};
    std::vector<int> v2 = {-8, 8};
    std::vector<int> v3 = {-1, 1};

    auto diag = _attack_table(v1);
    // auto file = _attack_table(v2);
    // auto rank = _attack_table(v3);

    auto BB_DIAG_MASKS = diag.first;
    auto BB_DIAG_ATTACKS = diag.second;
    // auto BB_FILE_MASKS = file.first;
    // auto BB_FILE_ATTACKS = file.second;
    // auto BB_RANK_MASKS = rank.first;
    // auto BB_RANK_ATTACKS = rank.second;

    std::cout << string(BB_DIAG_MASKS);
}