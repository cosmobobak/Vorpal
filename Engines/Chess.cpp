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

#include <iostream>
#include <vector>
#include <string>
#include <array>
#include <map>
#include <utility>
#include <cmath>
#include <algorithm>
#include <bitset>
#include <numeric>
#include <variant>
#include <regex>
#include "pregenerated_chess_tables.hpp"

using Color = bool;
enum COLORS
{
    WHITE,
    BLACK
};
std::array<bool, 2> COLORS = {true, false};
std::array<std::string, 2> COLOR_NAMES = {"black", "white"};

using PieceType = int;
enum PIECE_TYPES
{
    PAWN = 1,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING
};
std::array<char, 7> PIECE_SYMBOLS = {' ', 'p', 'n', 'b', 'r', 'q', 'k'};
std::array<std::string, 7> PIECE_NAMES = {"NONE", "pawn", "knight", "bishop", "rook", "queen", "king"};

auto piece_symbol(PieceType piece_type) -> char
{
    return PIECE_SYMBOLS[piece_type];
}

auto piece_name(PieceType piece_type) -> std::string
{
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

namespace Status
{
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
}; // namespace Status

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
enum eSQUARES{
    A1, B1, C1, D1, E1, F1, G1, H1,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A8, B8, C8, D8, E8, F8, G8, H8,
};

auto _generate_square_names() -> std::array<std::array<char, 2>, 64>
{
    std::array<std::array<char, 2>, 64> outarray;
    for (int f = 0; f < 8; f++) // (auto &&f : FILE_NAMES)
    {
        for (int r = 0; r < 8; r++) // (auto &&r : RANK_NAMES)
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

auto parse_square(SquareString name) -> Square // not done
{
    // """
    // Gets the square index for the given square *name*
    // (e.g., ``a1`` returns ``0``).
    // :raises: :exc:`ValueError` if the square name is invalid.
    // """
    // return SQUARE_NAMES.index(name);
    for (int i = 0; i < 64; i++)
    {
        if (name[0] == SQUARE_NAMES[i][0] && name[1] == SQUARE_NAMES[i][1])
            return i;
    }
}

auto square_name(Square s) -> SquareString
{
    // """Gets the name of the square, like ``a3``."""
    return SQUARE_NAMES[s];
}

auto square(int file_index, int rank_index) -> Square
{
    // """Gets a square number by file and rank index."""
    return rank_index * 8 + file_index;
}

auto square_file(Square s) -> int
{
    // """Gets the file index of the square where ``0`` is the a-file."""
    return s & 7;
}

auto square_rank(Square s) -> int
{
    // """Gets the rank index of the square where ``0`` is the first rank."""
    return s >> 3;
}

auto square_distance(Square a, Square b) -> int
{
    // """
    // Gets the distance (i.e., the number of king steps) from square *a* to *b*.
    // """
    return std::max(abs(square_file(a) - square_file(b)), abs(square_rank(a) - square_rank(b)));
}

auto square_mirror(Square s) -> Square
{ //"""Mirrors the square vertically."""
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

auto lsb(Bitboard bitboard) -> int // this function is just magic copied from here https://www.chessprogramming.org/BitScan#Divide_and_Conquer
{
    unsigned int lsb;
    //assert(bitboard != 0);
    bitboard &= -bitboard; // LS1B-Isolation
    lsb = (unsigned)bitboard | (unsigned)(bitboard >> 32);
    return (((((((((((unsigned)(bitboard >> 32) != 0) * 2) + ((lsb & 0xffff0000) != 0)) * 2) + ((lsb & 0xff00ff00) != 0)) * 2) + ((lsb & 0xf0f0f0f0) != 0)) * 2) + ((lsb & 0xcccccccc) != 0)) * 2) + ((lsb & 0xaaaaaaaa) != 0);
} // maybe try this later

// auto lsb(Bitboard bb) -> int
// {
//     const unsigned long long debruijn64 = C64(0x03f79d71b4cb0a89);
//     // assert(bb != 0);
//     return index64[((bb & -bb) * debruijn64) >> 58];
// }

auto msb(Bitboard bb) -> int // this function is just magic copied from here https://www.chessprogramming.org/BitScan#Double_conversion
{
    union
    {
        double d;
        struct
        {
            unsigned int mantissal : 32;
            unsigned int mantissah : 20;
            unsigned int exponent : 11;
            unsigned int sign : 1;
        };
    } ud;
    ud.d = (double)(bb & ~(bb >> 32)); // avoid rounding error
    return ud.exponent - 1023;
}
//////////////////////

// auto scan_forward(bb: Bitboard) -> Iterator[Square]:
//     while bb:
//         r = bb & -bb
//         yield r.bit_length() - 1
//         bb ^= r

auto scan_forward(Bitboard bb) -> std::vector<int>
{
    std::vector<int> bits;
    bits.reserve(32); // because the board is unlikely to have >32 pieces on it
    for (; bb;)
    {
        bits.push_back(lsb(bb));
        bb &= bb - 1; // clear the least significant bit set
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

auto scan_reversed(Bitboard bb) -> std::vector<int>
{
    std::vector<int> bits;
    bits.reserve(32); // because the board is unlikely to have >32 pieces on it
    for (; bb;)
    {
        int i = msb(bb);
        bits.push_back(i);
        bb ^= 1L << i;
    }
    return bits;
} // possibly borked due to MSB clearing

auto popcount(Bitboard bb) -> int
{
    return std::bitset<64>(bb).count();
}

auto flip_vertical(Bitboard bb) -> Bitboard
{
    // https://www.chessprogramming.org/Flipping_Mirroring_and_Rotating#FlipVertically
    bb = ((bb >> 8) & 0x00ff00ff00ff00ffULL) | ((bb & 0x00ff00ff00ff00ffULL) << 8);
    bb = ((bb >> 16) & 0x0000ffff0000ffffULL) | ((bb & 0x0000ffff0000ffffULL) << 16);
    bb = (bb >> 32) | ((bb & 0x00000000ffffffffULL) << 32);
    return bb;
}

auto flip_horizontal(Bitboard bb) -> Bitboard
{
    // https://www.chessprogramming.org/Flipping_Mirroring_and_Rotating#MirrorHorizontally
    bb = ((bb >> 1) & 0x5555555555555555ULL) | ((bb & 0x5555555555555555ULL) << 1);
    bb = ((bb >> 2) & 0x3333333333333333ULL) | ((bb & 0x3333333333333333ULL) << 2);
    bb = ((bb >> 4) & 0x0f0f0f0f0f0f0f0fULL) | ((bb & 0x0f0f0f0f0f0f0f0fULL) << 4);
    return bb;
}
auto flip_diagonal(Bitboard bb) -> Bitboard
{
    // https://www.chessprogramming.org/Flipping_Mirroring_and_Rotating#FlipabouttheDiagonal
    Bitboard t = (bb ^ (bb << 28)) & 0x0f0f0f0f00000000ULL;
    bb = bb ^ (t ^ (t >> 28));
    t = (bb ^ (bb << 14)) & 0x3333000033330000ULL;
    bb = bb ^ (t ^ (t >> 14));
    t = (bb ^ (bb << 7)) & 0x5500550055005500ULL;
    bb = bb ^ (t ^ (t >> 7));
    return bb;
}

auto flip_anti_diagonal(Bitboard bb) -> Bitboard
{
    // https://www.chessprogramming.org/Flipping_Mirroring_and_Rotating#FlipabouttheAntidiagonal
    Bitboard t = bb ^ (bb << 36);
    bb = bb ^ ((t ^ (bb >> 36)) & 0xf0f0f0f00f0f0f0fULL);
    t = (bb ^ (bb << 18)) & 0xcccc0000cccc0000ULL;
    bb = bb ^ (t ^ (t >> 18));
    t = (bb ^ (bb << 9)) & 0xaa00aa00aa00aa00ULL;
    bb = bb ^ (t ^ (t >> 9));
    return bb;
}

auto shift_down(Bitboard b) -> Bitboard
{
    return b >> 8;
}

auto shift_2_down(Bitboard b) -> Bitboard
{
    return b >> 16;
}

auto shift_up(Bitboard b) -> Bitboard
{
    return (b << 8) & BB_ALL;
}

auto shift_2_up(Bitboard b) -> Bitboard
{
    return (b << 16) & BB_ALL;
}

auto shift_right(Bitboard b) -> Bitboard
{
    return (b << 1) & ~BB_FILE_A & BB_ALL;
}

auto shift_2_right(Bitboard b) -> Bitboard
{
    return (b << 2) & ~BB_FILE_A & ~BB_FILE_B & BB_ALL;
}

auto shift_left(Bitboard b) -> Bitboard
{
    return (b >> 1) & ~BB_FILE_H;
}

auto shift_2_left(Bitboard b) -> Bitboard
{
    return (b >> 2) & ~BB_FILE_G & ~BB_FILE_H;
}

auto shift_up_left(Bitboard b) -> Bitboard
{
    return (b << 7) & ~BB_FILE_H & BB_ALL;
}

auto shift_up_right(Bitboard b) -> Bitboard
{
    return (b << 9) & ~BB_FILE_A & BB_ALL;
}

auto shift_down_left(Bitboard b) -> Bitboard
{
    return (b >> 9) & ~BB_FILE_H;
}

auto shift_down_right(Bitboard b) -> Bitboard
{
    return (b >> 7) & ~BB_FILE_A;
}

auto _sliding_attacks(Square s, Bitboard occupied, std::vector<int> deltas) -> Bitboard
{
    Bitboard attacks = BB_EMPTY;
    for (int delta : deltas)
    {
        Square sq = s;
        while (true)
        {
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

auto _step_attacks(Square s, std::vector<int> deltas) -> Bitboard
{
    return _sliding_attacks(s, BB_ALL, deltas);
}

auto _edges(Square s) -> Bitboard
{
    return (((BB_RANK_1 | BB_RANK_8) & ~BB_RANKS[square_rank(s)]) |
            ((BB_FILE_A | BB_FILE_H) & ~BB_FILES[square_file(s)]));
}

auto _carry_rippler(Bitboard mask) -> std::vector<Bitboard>
{
    std::vector<Bitboard> out;
    // Carry-Rippler trick to iterate subsets of mask.
    Bitboard subset = BB_EMPTY;
    while (true)
    {
        out.push_back(subset);
        subset = (subset - mask) & mask;
        if (!subset)
            break;
    }
    return out;
}

auto _attack_table(const std::vector<int> deltas) -> std::pair<std::vector<Bitboard>, std::vector<std::map<Bitboard, Bitboard>>>
{
    std::vector<Bitboard> mask_table;
    std::vector<std::map<Bitboard, Bitboard>> attack_table;

    for (Square s : SQUARES)
    {
        std::map<Bitboard, Bitboard> attacks;
        Bitboard mask = _sliding_attacks(s, 0, deltas) & ~_edges(s);
        for (auto &&subset : _carry_rippler(mask))
        {
            attacks[subset] = _sliding_attacks(s, subset, deltas);
        }
        attack_table.push_back(attacks);
        mask_table.push_back(mask);
    }

    return std::pair(mask_table, attack_table);
}

const std::vector<int> v1 = {-9, -7, 7, 9};
const std::vector<int> v2 = {-8, 8};
const std::vector<int> v3 = {-1, 1};

constexpr auto diag = _attack_table(v1);
constexpr auto file = _attack_table(v2);
constexpr auto rank = _attack_table(v3);

auto [BB_DIAG_MASKS, BB_DIAG_ATTACKS];
auto [BB_FILE_MASKS, BB_FILE_ATTACKS];
auto [BB_RANK_MASKS, BB_RANK_ATTACKS];

auto _rays() -> std::vector<std::vector<Bitboard>>
{
    std::vector<std::vector<Bitboard>> rays;
    Bitboard bb_a, bb_b;
    for (int a = 0; a < 64; a++) //a, bb_a in enumerate(BB_SQUARES)
    {
        bb_a = 1ULL << a;
        std::vector<Bitboard> rays_row;
        for (int b = 0; b < 64; b++) //b, bb_b in enumerate(BB_SQUARES):
        {
            bb_b = 1ULL << b;
            if (BB_DIAG_ATTACKS[a].at(0) & bb_b)
                rays_row.push_back((BB_DIAG_ATTACKS[a].at(0) & BB_DIAG_ATTACKS[b].at(0)) | bb_a | bb_b);
            else if (BB_RANK_ATTACKS[a].at(0) & bb_b)
                rays_row.push_back(BB_RANK_ATTACKS[a].at(0) | bb_a);
            else if (BB_FILE_ATTACKS[a].at(0) & bb_b)
                rays_row.push_back(BB_FILE_ATTACKS[a].at(0) | bb_a);
            else
                rays_row.push_back(BB_EMPTY);
        }
        rays.push_back(rays_row);
    }
    return rays;
}
const std::vector<std::vector<Bitboard>> BB_RAYS = _rays();

auto ray(Square a, Square b) -> Bitboard
{
    return BB_RAYS[a][b];
}

auto between(Square a, Square b) -> Bitboard
{
    Bitboard bb = BB_RAYS[a][b] & ((BB_ALL << a) ^ (BB_ALL << b));
    return bb & (bb - 1);
}

// SAN_REGEX = re.compile(r"^([NBKRQ])?([a-h])?([1-8])?[\-x]?([a-h][1-8])(=?[nbrqkNBRQK])?[\+#]?\Z")

// FEN_CASTLING_REGEX = re.compile(r"^(?:-|[KQABCDEFGH]{0,2}[kqabcdefgh]{0,2})\Z")

auto change_case(char c) -> char
{
    if (std::isupper(c))
        return std::tolower(c);
    else
        return std::toupper(c);
}

template <typename T, std::size_t SIZE>
auto get_index(std::array<T, SIZE> array, T target) -> int
{
    return std::find(array.begin(), array.end(), target) - array.begin();
}

auto get_index(std::array<std::array<char, 3>, 64> array, std::string target) -> int
{
    int index = 0;
    bool found = false;
    for (auto &&charray : array)
    {
        if (charray[0] == target[0] && charray[1] == target[1])
        {
            return index;
        }
        index++;
    }
    return 64;
}

class Piece
{
public:
    // """A piece with type and color."""

    PieceType piece_type;
    // """The piece type."""

    Color color;
    // """The piece color."""

    Piece(PieceType t, Color c)
    {
        piece_type = t;
        color = c;
    }

    auto get_symbol() -> char
    {
        // """
        // Gets the symbol ``P``, ``N``, ``B``, ``R``, ``Q`` || ``K`` for white
        // pieces || the lower-case variants for the black pieces.
        // """
        char s = piece_symbol(piece_type);
        return color ? std::toupper(s) : s;
    }
    auto unicode_symbol(bool invert_color = false) -> std::string
    {
        // """
        // Gets the Unicode character for the piece.
        // """
        char s = invert_color ? change_case(get_symbol()) : get_symbol();
        return UNICODE_PIECE_SYMBOLS.at(s);
    }
    auto __hash__() -> int
    {
        return piece_type + (color ? -1 : 5);
    }

    auto __repr__() -> std::string
    {
        std::string builder;
        builder += "Piece.from_symbol(";
        builder += get_symbol();
        builder += ")";
        return builder;
    }

    auto __str__() -> char
    {
        return get_symbol();
    }

    // auto _repr_svg_(self) -> str:
    //     import chess.svg
    //     return chess.svg.piece(self, size=45)

    static auto from_symbol(char s) -> Piece
    {
        // """
        // Creates a :class:`~chess.Piece` instance from a piece symbol.
        // :raises: :exc:`ValueError` if the symbol is invalid.
        // """
        return Piece(
            (PieceType)get_index(PIECE_SYMBOLS, (char)tolower(s)),
            isupper(s));
    }
};

//@dataclasses.dataclass(unsafe_hash=True)
class Move{
public:
    // """
    // Represents a move from a square to a square and possibly the promotion
    // piece type.
    // Drops and null moves are supported.
    // """

    Square from_square;
    // """The source square."""

    Square to_square;
    // """The target square."""

    std::optional<PieceType> promotion;
    // """The promotion piece type || ``None``."""

    std::optional<PieceType> drop;
    // """The drop piece type || ``None``."""

    Move(
        Square f, 
        Square t, 
        std::optional<PieceType> p = std::nullopt, 
        std::optional<PieceType> d = std::nullopt)
    {
        from_square = f;
        to_square = t;
        promotion = p;
        drop = d;
    }

    auto uci() -> std::string
    {
        // """
        // Gets a UCI string for the move.
        // For example, a move from a7 to a8 would be ``a7a8`` || ``a7a8q``
        // (if the latter is a promotion to a queen).
        // The UCI representation of a null move is ``0000``.
        // """
        if (drop){
            std::string builder;
            builder += (char)toupper(piece_symbol(drop.value()));
            builder += '@';
            builder += SQUARE_NAMES[to_square][0];
            builder += SQUARE_NAMES[to_square][1];
            return builder;
        }
        else if (promotion)
        {
            std::string builder;
            builder += SQUARE_NAMES[from_square][0];
            builder += SQUARE_NAMES[from_square][1];
            builder += SQUARE_NAMES[to_square][0];
            builder += SQUARE_NAMES[to_square][1];
            builder += piece_symbol(promotion.value());
            return builder;
        }
        else if (this->__bool__())
        {
            std::string builder;
            builder += SQUARE_NAMES[from_square][0];
            builder += SQUARE_NAMES[from_square][1];
            builder += SQUARE_NAMES[to_square][0];
            builder += SQUARE_NAMES[to_square][1];
            return builder;
        }
        else
            return "0000";
    }

    auto xboard() -> std::string
    {
        return this->__bool__() ? uci() : "@@@@";
    }

    auto __bool__() -> bool
    {
        return (from_square || to_square || promotion || drop);
    }

    auto __repr__() -> std::string{
        std::string builder;
        builder += "Move.from_uci(";
        builder += uci();
        builder += ")";
        return builder;
    }
    
    auto __str__() -> std::string
    {
        return uci();
    }
    
    static auto from_uci(std::string uci) -> Move
    {
        // """
        // Parses a UCI string.
        // :raises: :exc:`ValueError` if the UCI string is invalid.
        // """
        if (uci == "0000")
            return null();
        else if (uci.length() == 4 and '@' == uci[1])
        {
            std::optional<PieceType> _drop = (PieceType)get_index(PIECE_SYMBOLS, (char)tolower(uci[0]));
            Square s = get_index(SQUARE_NAMES, uci.substr(2));
            return Move(s, s, std::nullopt, _drop);
        }
        else if (4 <= uci.length() <= 5)
        {
            Square _from_square = get_index(SQUARE_NAMES, uci.substr(0, 2));
            Square _to_square = get_index(SQUARE_NAMES, uci.substr(2, 2));
            std::optional<PieceType> _promotion;
            if (uci.length() == 5)
                _promotion = get_index(PIECE_SYMBOLS, uci[4]);
            else
                _promotion = std::nullopt;
            if (_from_square == _to_square)
            {
                std::cout << "Exception: invalid uci (use 0000 for null moves):" << uci << '\n';
                std::terminate();
            }
            return Move(_from_square, _to_square, _promotion, std::nullopt);
        }
        else
            std::cout << "Exception: expected uci string to be of length 4 || 5:" << uci << '\n';
            std::terminate();
    }
    
    static auto null() -> Move
    {
        // """
        // Gets a null move.
        // A null move just passes the turn to the other side (and possibly
        // forfeits en passant capturing). Null moves evaluate to ``False`` in
        // boolean contexts.
        // >>> import chess
        // >>>
        // >>> bool(chess.Move.null())
        // False
        // """
        return Move(0, 0);
    }
};

// BaseBoardT = TypeVar("BaseBoardT", bound="BaseBoard")

// IntoSquareSet = Union[SupportsInt, Iterable[Square]]

using IntoSquareSet = std::variant<Bitboard, std::vector<Square>>;

class SquareSet
{
public:
    // """
    // A set of squares.

    // >>> import chess
    // >>>
    // >>> squares = chess.SquareSet([chess.A8, chess.A1])
    // >>> squares
    // SquareSet(0x0100_0000_0000_0001)

    // >>> squares = chess.SquareSet(chess.BB_A8 | chess.BB_RANK_1)
    // >>> squares
    // SquareSet(0x0100_0000_0000_00ff)

    // >>> print(squares)
    // 1 . . . . . . .
    // . . . . . . . .
    // . . . . . . . .
    // . . . . . . . .
    // . . . . . . . .
    // . . . . . . . .
    // . . . . . . . .
    // 1 1 1 1 1 1 1 1

    // >>> len(squares)
    // 9

    // >>> bool(squares)
    // True

    // >>> chess.B1 in squares
    // True

    // >>> for square in squares:
    // ...     # 0 -- chess.A1
    // ...     # 1 -- chess.B1
    // ...     # 2 -- chess.C1
    // ...     # 3 -- chess.D1
    // ...     # 4 -- chess.E1
    // ...     # 5 -- chess.F1
    // ...     # 6 -- chess.G1
    // ...     # 7 -- chess.H1
    // ...     # 56 -- chess.A8
    // ...     print(square)
    // ...
    // 0
    // 1
    // 2
    // 3
    // 4
    // 5
    // 6
    // 7
    // 56

    // >>> list(squares)
    // [0, 1, 2, 3, 4, 5, 6, 7, 56]

    // Square sets are internally represented by 64-bit integer masks of the
    // included squares. Bitwise operations can be used to compute unions,
    // intersections and shifts.

    // >>> int(squares)
    // 72057594037928191

    // Also supports common set operations like
    // :func:`~chess.SquareSet.issubset()`, :func:`~chess.SquareSet.issuperset()`,
    // :func:`~chess.SquareSet.union()`, :func:`~chess.SquareSet.intersection()`,
    // :func:`~chess.SquareSet.difference()`,
    // :func:`~chess.SquareSet.symmetric_difference()` and
    // :func:`~chess.SquareSet.copy()` as well as
    // :func:`~chess.SquareSet.update()`,
    // :func:`~chess.SquareSet.intersection_update()`,
    // :func:`~chess.SquareSet.difference_update()`,
    // :func:`~chess.SquareSet.symmetric_difference_update()` and
    // :func:`~chess.SquareSet.clear()`.
    // """

    Bitboard mask;

    SquareSet(IntoSquareSet squares = BB_EMPTY)
    {
        try
        {
            mask = std::get<Bitboard>(squares) & BB_ALL; // type: ignore
            return;
        }
        catch (const std::bad_variant_access &)
        {
            mask = 0;
        }

        // Try squares as an iterable. Not under except clause for nicer
        // backtraces.
        for (Square s : std::get<std::vector<Square>>(squares)) // type: ignore
        {
            add(s);
        }
    }
    auto __contains__(Square s) -> bool;
    auto __iter__() -> std::vector<Square>;
    auto __reversed__() -> std::vector<Square>;
    auto __len__() -> int;
    void add(Square s);
    void discard(Square s);
    auto isdisjoint(IntoSquareSet other) -> bool;
    auto issubset(IntoSquareSet other) -> bool;
    auto issuperset(IntoSquareSet other) -> bool;
    auto unify(IntoSquareSet other) -> SquareSet;
    auto intersection(IntoSquareSet other) -> SquareSet;
    auto difference(IntoSquareSet other) -> SquareSet;
    auto symmetric_difference(IntoSquareSet other) -> SquareSet;
    auto copy() -> SquareSet;
    void update(std::vector<IntoSquareSet> others);
    void intersection_update(std::vector<IntoSquareSet> others);
    void difference_update(IntoSquareSet other);
    void symmetric_difference_update(IntoSquareSet other);
    void remove(Square s);
    auto pop() -> Square;
    void clear();
    auto carry_rippler() -> std::vector<Bitboard>;
    auto mirror() -> SquareSet;
    auto tolist() -> std::array<bool, 64>;
    auto __bool__() -> bool;
    auto __invert__() -> SquareSet;
    auto __int__() -> Bitboard;
    auto __index__() -> Bitboard;
    auto __str__() -> std::string;
    static auto ray(Square a, Square b) -> SquareSet;
    static auto between(Square a, Square b) -> SquareSet;
    static auto from_square(Square a) -> SquareSet;
};

auto operator~(SquareSet s) -> SquareSet
{
    return SquareSet(~s.mask);
}
auto operator~(IntoSquareSet s) -> IntoSquareSet
{
    IntoSquareSet out;
    SquareSet interm = SquareSet(s);
    out = ~interm.mask;
    return out;
}

auto operator|(SquareSet self, IntoSquareSet other) -> SquareSet
{
    SquareSet r = SquareSet(other);
    r.mask |= self.mask;
    return r;
}

auto operator&(SquareSet self, IntoSquareSet other) -> SquareSet
{
    SquareSet r = SquareSet(other);
    r.mask &= self.mask;
    return r;
}

auto operator-(SquareSet self, IntoSquareSet other) -> SquareSet
{
    SquareSet r = SquareSet(other);
    r.mask = self.mask & ~r.mask;
    return r;
}

auto operator^(SquareSet self, IntoSquareSet other) -> SquareSet
{
    SquareSet r = SquareSet(other);
    r.mask ^= self.mask;
    return r;
}

void operator|=(SquareSet self, IntoSquareSet other)
{
    self.mask |= SquareSet(other).mask;
}

void operator&=(SquareSet self, IntoSquareSet other)
{
    self.mask &= SquareSet(other).mask;
}

auto operator-=(SquareSet self, IntoSquareSet other)
{
    self.mask &= ~SquareSet(other).mask;
}

auto operator^=(SquareSet self, IntoSquareSet other)
{
    self.mask ^= SquareSet(other).mask;
}

auto operator==(SquareSet a, SquareSet b) -> bool
{
    return a.mask == b.mask;
}
auto operator==(SquareSet a, IntoSquareSet b) -> bool
{
    return a.mask == SquareSet(b).mask;
}

auto operator<<(SquareSet a, int shift) -> SquareSet
{
    return SquareSet(a.mask << shift);
}

auto operator>>(SquareSet a, int shift) -> SquareSet
{
    return SquareSet(a.mask >> shift);
}

auto operator<<=(SquareSet a, int shift)
{
    a.mask <<= shift;
}

auto operator>>=(SquareSet a, int shift)
{
    a.mask >>= shift;
}

auto SquareSet::__contains__(Square s) -> bool
{
    return (BB_SQUARES[s] & mask);
}

auto SquareSet::__iter__() -> std::vector<Square>
{
    return scan_forward(mask);
}

auto SquareSet::__reversed__() -> std::vector<Square>
{
    return scan_reversed(mask);
}

auto SquareSet::__len__() -> int
{
    return popcount(mask);
}

// MutableSet

void SquareSet::add(Square s)
{
    // """Adds a square to the set."""
    mask |= BB_SQUARES[s];
}
void SquareSet::discard(Square s)
{
    // """Discards a square from the set."""
    mask &= ~BB_SQUARES[s];
}
// frozenset

auto SquareSet::isdisjoint(IntoSquareSet other) -> bool
{
    // """Tests if the square sets are disjoint."""
    return !(*this & other).__bool__();
}

auto SquareSet::issubset(IntoSquareSet other) -> bool
{
    // """Tests if this square set is a subset of another."""
    return !(~*this & other).__bool__();
}

auto SquareSet::issuperset(IntoSquareSet other) -> bool
{
    // """Tests if this square set is a superset of another."""
    return !(*this & ~other).__bool__();
}

auto SquareSet::unify(IntoSquareSet other) -> SquareSet
{
    return *this | other;
}

auto SquareSet::intersection(IntoSquareSet other) -> SquareSet
{
    return *this & other;
}

auto SquareSet::difference(IntoSquareSet other) -> SquareSet
{
    return *this - other;
}

auto SquareSet::symmetric_difference(IntoSquareSet other) -> SquareSet
{
    return *this ^ other;
}

auto SquareSet::copy() -> SquareSet
{
    return SquareSet(mask);
}
// set

void SquareSet::update(std::vector<IntoSquareSet> others)
{
    for (IntoSquareSet other : others)
        *this |= other;
}

void SquareSet::intersection_update(std::vector<IntoSquareSet> others)
{
    for (IntoSquareSet other : others)
        *this &= other;
}

void SquareSet::difference_update(IntoSquareSet other)
{
    *this -= other;
}

void SquareSet::symmetric_difference_update(IntoSquareSet other)
{
    *this ^= other;
}

void SquareSet::remove(Square s)
{
    // """
    // Removes a square from the set.

    // :raises: :exc:`KeyError` if the given *square* was not in the set.
    // """
    Bitboard fmask = BB_SQUARES[s];
    if (mask & fmask)
    {
        mask ^= fmask;
    }
    else
    {
        std::cout << "Exception: KeyError: " << s << '\n';
        std::terminate();
    }
}

auto SquareSet::pop() -> Square
{
    // """
    // Removes and returns a square from the set.

    // :raises: :exc:`KeyError` if the set is empty.
    // """
    if (!mask)
    {
        std::cout << "Exception: pop from empty SquareSet\n";
        std::terminate();
    }
    Square s = lsb(mask);
    mask &= (mask - 1);
    return s;
}

void SquareSet::clear()
{
    // """Removes all elements from this set."""
    mask = BB_EMPTY;
}

// #SquareSet

auto SquareSet::carry_rippler() -> std::vector<Bitboard>
{
    // """Iterator over the subsets of this set."""
    return _carry_rippler(mask);
}

auto SquareSet::mirror() -> SquareSet
{
    // """Returns a vertically mirrored copy of this square set."""
    return SquareSet(flip_vertical(mask));
}

auto SquareSet::tolist() -> std::array<bool, 64>
{
    // """Converts the set to a list of 64 bools."""
    std::array<bool, 64> result = {false};
    for (Square s : this->__iter__())
        result[s] = true;
    return result;
}

auto SquareSet::__bool__() -> bool
{
    return (bool)(mask);
}
// auto __eq__(other: object) -> bool:
//     try:
//         return self.mask == SquareSet(other).mask  # type: ignore
//     except (TypeError, ValueError):
//         return NotImplemented

auto SquareSet::__invert__() -> SquareSet
{
    return SquareSet(~mask & BB_ALL);
}
auto SquareSet::__int__() -> Bitboard
{
    return mask;
}
auto SquareSet::__index__() -> Bitboard
{
    return mask;
}

// auto __repr__() -> str
//     return f"SquareSet({self.mask:#021_x})"

auto SquareSet::__str__() -> std::string
{
    std::vector<std::string> builder;

    Bitboard fmask;
    for (Square s : SQUARES_180)
    {
        fmask = BB_SQUARES[s];
        builder.push_back(mask & fmask ? "1" : ".");

        if (!(mask & BB_FILE_H))
            builder.push_back(" ");
        else if (s != H1)
            builder.push_back("\n");
    }

    return std::accumulate(builder.begin(), builder.end(), std::string("")); // "".join(builder);
}

// auto _repr_svg_(self) -> str:
//     import chess.svg
//     return chess.svg.board(squares=self, size=390)

// // @classmethod
auto SquareSet::ray(Square a, Square b) -> SquareSet
{
    // """
    // All squares on the rank, file || diagonal with the two squares, if they
    // are aligned.

    // >>> import chess
    // >>>
    // >>> print(chess.SquareSet.ray(chess.E2, chess.B5))
    // . . . . . . . .
    // . . . . . . . .
    // 1 . . . . . . .
    // . 1 . . . . . .
    // . . 1 . . . . .
    // . . . 1 . . . .
    // . . . . 1 . . .
    // . . . . . 1 . .
    // """
    return SquareSet(ray(a, b));
}
// // @classmethod
auto SquareSet::between(Square a, Square b) -> SquareSet
{
    // """
    // All squares on the rank, file || diagonal between the two squares
    // (bounds not included), if they are aligned.

    // >>> import chess
    // >>>
    // >>> print(chess.SquareSet.between(chess.E2, chess.B5))
    // . . . . . . . .
    // . . . . . . . .
    // . . . . . . . .
    // . . . . . . . .
    // . . 1 . . . . .
    // . . . 1 . . . .
    // . . . . . . . .
    // . . . . . . . .
    // """
    return SquareSet(between(a, b));
}
// // @classmethod
auto SquareSet::from_square(Square square) -> SquareSet
{
    // """
    // Creates a :class:`~chess.SquareSet` from a single square.

    // >>> import chess
    // >>>
    // >>> chess.SquareSet.from_square(chess.A1) == chess.BB_A1
    // True
    // """
    return SquareSet(BB_SQUARES[square]);
}

auto split(const std::string &s, char seperator) -> std::vector<std::string>
{
    std::vector<std::string> output;
    output.reserve(8);
    std::string::size_type prev_pos = 0, pos = 0;
    while ((pos = s.find(seperator, pos)) != std::string::npos)
    {
        std::string substring(s.substr(prev_pos, pos - prev_pos));
        output.push_back(substring);
        prev_pos = ++pos;
    }
    output.push_back(s.substr(prev_pos, pos - prev_pos)); // Last word
    return output;
}

auto a_in_b(std::string a, std::string b) -> bool
{
    return (std::string(b).find(a) != std::string::npos);
}
auto a_in_b(char a, std::string b) -> bool
{
    return (std::string(b).find(std::string(1, a)) != std::string::npos);
}
auto a_in_b(char a, std::array<char, 7> b) -> bool
{
    return std::any_of(b.begin(), b.end(), [a](char c) { return c == a; });
}
auto a_in_b(std::string a, std::array<std::string, 7> b) -> bool
{
    return std::any_of(b.begin(), b.end(), [a](std::string s) { return s == a; });
}

class BaseBoard
{
public:
    // """
    // A board representing the position of chess pieces. See
    // :class:`~chess.Board` for a full board with move generation.
    // The board is initialized with the standard chess starting position, unless
    // otherwise specified in the optional *board_fen* argument. If *board_fen*
    // is ``None``, an empty board is created.
    // """
    Bitboard occupied;
    std::array<Bitboard, 2> occupied_co;
    Bitboard pawns;
    Bitboard knights;
    Bitboard bishops;
    Bitboard rooks;
    Bitboard queens;
    Bitboard kings;
    Bitboard promoted;

    BaseBoard(std::optional<std::string> board_fen = STARTING_BOARD_FEN)
    {
        occupied_co[0] = BB_EMPTY;
        occupied_co[1] = BB_EMPTY;

        if (!board_fen)
            _clear_board();
        else if (board_fen == STARTING_BOARD_FEN)
            _reset_board();
        else
            _set_board_fen(board_fen.value());
    }
    void _reset_board()
    {
        pawns = BB_RANK_2 | BB_RANK_7;
        knights = BB_B1 | BB_G1 | BB_B8 | BB_G8;
        bishops = BB_C1 | BB_F1 | BB_C8 | BB_F8;
        rooks = BB_CORNERS;
        queens = BB_D1 | BB_D8;
        kings = BB_E1 | BB_E8;

        promoted = BB_EMPTY;

        occupied_co[WHITE] = BB_RANK_1 | BB_RANK_2;
        occupied_co[BLACK] = BB_RANK_7 | BB_RANK_8;
        occupied = BB_RANK_1 | BB_RANK_2 | BB_RANK_7 | BB_RANK_8;
    }

    void reset_board()
    {
        // """Resets pieces to the starting position."""
        _reset_board();
    }

    void _clear_board()
    {
        pawns = BB_EMPTY;
        knights = BB_EMPTY;
        bishops = BB_EMPTY;
        rooks = BB_EMPTY;
        queens = BB_EMPTY;
        kings = BB_EMPTY;

        promoted = BB_EMPTY;

        occupied_co[WHITE] = BB_EMPTY;
        occupied_co[BLACK] = BB_EMPTY;
        occupied = BB_EMPTY;
    }
    void clear_board(){
        // """Clears the board."""
        _clear_board();
    }
    auto pieces_mask(PieceType piece_type, Color color) -> Bitboard
    {
        Bitboard bb;
        switch (piece_type)
        {
        case PAWN:
            bb = pawns;
            break;
        case KNIGHT:
            bb = knights;
            break;
        case BISHOP:
            bb = bishops;
            break;
        case ROOK:
            bb = rooks;
            break;
        case QUEEN:
            bb = queens;
            break;
        case KING:
            bb = kings;
            break;
        default:
            break;
        }
        return bb & occupied_co[color];
    }

    auto pieces(PieceType piece_type, Color color) -> SquareSet
    {
        // """
        // Gets pieces of the given type and color.
        // Returns a :class:`set of squares <chess.SquareSet>`.
        // """
        return SquareSet(pieces_mask(piece_type, color));
    }
    auto piece_at(Square s) -> std::optional<Piece>
    {
        // """Gets the :class:`piece <chess.Piece>` at the given square."""
        std::optional<int> piece_type = piece_type_at(s);
        if (piece_type){
            Bitboard mask = BB_SQUARES[s];
            Color color = (bool)(occupied_co[WHITE] & mask);
            return Piece(piece_type.value(), color);}
        else
            return std::nullopt;
    }

    auto piece_type_at(Square s) -> std::optional<PieceType>
    {
        // """Gets the piece type at the given square."""
        Bitboard mask = BB_SQUARES[s];

        if (!occupied & mask)
            return std::nullopt; // Early return
        else if (pawns & mask)
            return PAWN;
        else if (knights & mask)
            return KNIGHT;
        else if (bishops & mask)
            return BISHOP;
        else if (rooks & mask)
            return ROOK;
        else if (queens & mask)
            return QUEEN;
        else
            return KING;
    }

    auto color_at(Square s) -> std::optional<Color>
    {
        // """Gets the color of the piece at the given square."""
        Bitboard mask = BB_SQUARES[s];
        if (occupied_co[WHITE] & mask)
            return WHITE;
        else if (occupied_co[BLACK] & mask)
            return BLACK;
        else
            return std::nullopt;
    }
    auto king(Color color) -> std::optional<Color>
    {
        // """
        // Finds the king square of the given side. Returns ``None`` if there
        // is no king of that color.
        // In variants with king promotions, only non-promoted kings are
        // considered.
        // """
        Bitboard king_mask = occupied_co[color] & kings & ~promoted;
        if (king_mask){
            return msb(king_mask);
        }else{
            return std::nullopt;
        }
    }
    auto attacks_mask(Square s) -> Bitboard
    {
        Bitboard bb_square = BB_SQUARES[s];

        if (bb_square & pawns)
        {
            Color color = (bool)(bb_square & occupied_co[WHITE]);
            return BB_PAWN_ATTACKS[color][s];
        }
        else if (bb_square & knights)
            return BB_KNIGHT_ATTACKS[s];
        else if (bb_square & kings)
            return BB_KING_ATTACKS[s];
        else
        {
            Bitboard attacks = 0;
            if (bb_square & bishops || bb_square & queens)
                attacks = BB_DIAG_ATTACKS[s][BB_DIAG_MASKS[s] & occupied]; // .at() is probably the way to go, shame about performance
            if (bb_square & rooks || bb_square & queens)
                attacks |= (BB_RANK_ATTACKS[s][BB_RANK_MASKS[s] & occupied] |
                            BB_FILE_ATTACKS[s][BB_FILE_MASKS[s] & occupied]);
            return attacks;
        }
    }

    auto attacks(Square s) -> SquareSet
    {
        // """
        // Gets the set of attacked squares from the given square.
        // There will be no attacks if the square is empty. Pinned pieces are
        // still attacking other squares.
        // Returns a :class:`set of squares <chess.SquareSet>`.
        // """
        return SquareSet(attacks_mask(s));
    }
    auto _attackers_mask(Color color, Square s, Bitboard occupied) -> Bitboard
    {
        Bitboard rank_pieces = BB_RANK_MASKS[s] & occupied;
        Bitboard file_pieces = BB_FILE_MASKS[s] & occupied;
        Bitboard diag_pieces = BB_DIAG_MASKS[s] & occupied;

        Bitboard queens_and_rooks = queens | rooks;
        Bitboard queens_and_bishops = queens | bishops;

        Bitboard attackers = (
            (BB_KING_ATTACKS[s] & kings) |
            (BB_KNIGHT_ATTACKS[s] & knights) |
            (BB_RANK_ATTACKS[s][rank_pieces] & queens_and_rooks) |
            (BB_FILE_ATTACKS[s][file_pieces] & queens_and_rooks) |
            (BB_DIAG_ATTACKS[s][diag_pieces] & queens_and_bishops) |
            (BB_PAWN_ATTACKS[!color][s] & pawns));

        return attackers & occupied_co[color];
    }
    auto attackers_mask(Color color, Square s) -> Bitboard
    {
        return _attackers_mask(color, s, occupied);
    }
    auto is_attacked_by(Color color, Square s) -> bool
    {
        // """
        // Checks if the given side attacks the given square.
        // Pinned pieces still count as attackers. Pawns that can be captured
        // en passant are **not** considered attacked.
        // """
        return (bool)(attackers_mask(color, s));
    }
    auto attackers(Color color, Square s) -> SquareSet
    {
        // """
        // Gets the set of attackers of the given color for the given square.
        // Pinned pieces still count as attackers.
        // Returns a :class:`set of squares <chess.SquareSet>`.
        // """
        return SquareSet(attackers_mask(color, s));
    }
    auto pin_mask(Color color, Square s) -> Bitboard
    {
        std::optional<Color> kingc = king(color);
        if (!kingc)
            return BB_ALL;

        Bitboard square_mask = BB_SQUARES[s];
        std::array<std::vector<std::map<Bitboard, Bitboard>>, 3 > attacksl = {BB_FILE_ATTACKS, BB_RANK_ATTACKS, BB_DIAG_ATTACKS};
        std::array<Bitboard, 3> slidersl = {(rooks | queens), (rooks | queens), (bishops | queens)};

        std::vector<std::map<Bitboard, Bitboard>> attacks;
        Bitboard sliders, rays, snipers;
        for (int i = 0; i < 3; i++)
        {
            attacks = attacksl[i];
            sliders = slidersl[i];
            rays = attacks[kingc.value()][0];
            if (rays & square_mask)
            {
                snipers = rays & sliders & occupied_co[not color];
                for (int sniper : scan_reversed(snipers)){
                    if (between(sniper, kingc.value()) & (occupied | square_mask) == square_mask)
                        return ray(kingc.value(), sniper);}

                break;
            }
        }
        return BB_ALL;
    }
    auto pin(Color color, Square s) -> SquareSet
    {
        // """
        // Detects an absolute pin (and its direction) of the given square to
        // the king of the given color.
        // >>> import chess
        // >>>
        // >>> board = chess.Board("rnb1k2r/ppp2ppp/5n2/3q4/1b1P4/2N5/PP3PPP/R1BQKBNR w KQkq - 3 7")
        // >>> board.is_pinned(chess.WHITE, chess.C3)
        // True
        // >>> direction = board.pin(chess.WHITE, chess.C3)
        // >>> direction
        // SquareSet(0x0000_0001_0204_0810)
        // >>> print(direction)
        // . . . . . . . .
        // . . . . . . . .
        // . . . . . . . .
        // 1 . . . . . . .
        // . 1 . . . . . .
        // . . 1 . . . . .
        // . . . 1 . . . .
        // . . . . 1 . . .
        // Returns a :class:`set of squares <chess.SquareSet>` that mask the rank,
        // file || diagonal of the pin. If there is no pin, then a mask of the
        // entire board is returned.
        // """
        return SquareSet(pin_mask(color, s));
    }
    auto is_pinned(Color color, Square s) -> bool
    {
        // """
        // Detects if the given square is pinned to the king of the given color.
        // """
        return pin_mask(color, s) != BB_ALL;
    }
    auto _remove_piece_at(const Square s) -> std::optional<PieceType>
    {
        std::optional<PieceType> piece_type = piece_type_at(s);
        Bitboard mask = BB_SQUARES[s];

        if (piece_type == PAWN)
            pawns ^= mask;
        else if (piece_type == KNIGHT)
            knights ^= mask;
        else if (piece_type == BISHOP)
            bishops ^= mask;
        else if (piece_type == ROOK)
            rooks ^= mask;
        else if (piece_type == QUEEN)
            queens ^= mask;
        else if (piece_type == KING)
            kings ^= mask;
        else
            return std::nullopt;

        occupied ^= mask;
        occupied_co[WHITE] &= ~mask;
        occupied_co[BLACK] &= ~mask;

        promoted &= ~mask;

        return piece_type;
    }
    auto remove_piece_at(Square s) -> std::optional<Piece>
    {
        // """
        // Removes the piece from the given square. Returns the
        // :class:`~chess.Piece` || ``None`` if the square was already empty.
        // """
        Color color = (bool)(occupied_co[WHITE] & BB_SQUARES[s]);
        std::optional<PieceType> piece_type = _remove_piece_at(s);
        if (piece_type)
            return Piece(piece_type.value(), color); 
        else
            return std::nullopt;
    }
    void _set_piece_at(
        Square s, 
        PieceType piece_type, 
        Color color, 
        bool promoted = false
    )
    {
        _remove_piece_at(s);

        Bitboard mask = BB_SQUARES[s];

        if (piece_type == PAWN)
            pawns |= mask;
        else if (piece_type == KNIGHT)
            knights |= mask;
        else if (piece_type == BISHOP)
            bishops |= mask;
        else if (piece_type == ROOK)
            rooks |= mask;
        else if (piece_type == QUEEN)
            queens |= mask;
        else if (piece_type == KING)
            kings |= mask;
        else
            return;

        occupied ^= mask;
        occupied_co[color] ^= mask;

        if (promoted)
            promoted ^= mask;
    }
    void set_piece_at(Square s, std::optional<Piece> piece, bool promoted = false)
    {
        // """
        // Sets a piece at the given square.
        // An existing piece is replaced. Setting *piece* to ``None`` is
        // equivalent to :func:`~chess.Board.remove_piece_at()`.
        // """
        if (!piece)
            _remove_piece_at(s);
        else
            _set_piece_at(s, piece.value().piece_type, piece.value().color, promoted);
    }
    auto board_fen(std::optional<bool> fpromoted = false) -> std::string
    {
        // """
        // Gets the board FEN (e.g.,
        // ``rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR``).
        // """
        std::string builder;
        int empty = 0;
        std::optional<Piece> piece;
        for (Square s : SQUARES_180)
        {
            piece = piece_at(s);

            if (!piece)
                empty++;
            else
            {
                if (empty)
                {
                    char addon = '0' + empty;
                    builder += addon;
                    empty = 0;
                }
                builder += piece.value().get_symbol();
                if (fpromoted && BB_SQUARES[s] & promoted)
                    builder.append("~");
            }

            if (BB_SQUARES[s] & BB_FILE_H)
            {
                if (empty)
                {
                    char addon = '0' + empty;
                    builder += addon;
                    empty = 0;
                }

                if (s != H1)
                    builder.append("/");
            }
        }

        return std::accumulate(builder.begin(), builder.end(), std::string("")); // "".join(builder);
    }

    void _set_board_fen(std::string fen)
    {
        // # Compatibility with set_fen().
        fen = std::regex_replace(fen, std::regex("^ +| +$|( ) +"), "$1");
        if (fen.find(" ") != std::string::npos) // " " in fen
        {
            std::cout << "expected position part of fen, got multiple parts: " << fen;
            std::terminate();
        }

        // # Ensure the FEN is valid.
        std::vector<std::string> rows = split(fen, '/');
        if (rows.size() != 8)
        {
            std::cout << "expected 8 rows in position part of fen: " << fen << " but got " << rows.size();
            std::terminate();
        }

        // # Validate each row.
        for (std::string row : rows){
            int field_sum = 0;
            bool previous_was_digit = false;
            bool previous_was_piece = false;

            for (char c : row){
                if (a_in_b(c, "12345678")) // c in ["1", "2", "3", "4", "5", "6", "7", "8"]:
                {
                    if (previous_was_digit)
                    {
                        std::cout << "two subsequent digits in position part of fen: " << fen;
                        std::terminate();
                    }
                    field_sum += (c - '0'); // this is a trick to convert chars that represent digits to their number
                    previous_was_digit = true;
                    previous_was_piece = false;
                }
                else if (c == '~'){
                    if (!previous_was_piece)
                    {
                        std::cout << "'~' not after piece in position part of fen: " << fen;
                        std::terminate();
                    }
                    previous_was_digit = false;
                    previous_was_piece = false;
                }
                else if (a_in_b(tolower(c), PIECE_SYMBOLS))
                {
                    field_sum++;
                    previous_was_digit = false;
                    previous_was_piece = true;
                }
                else
                {
                    std::cout << "invalid character in position part of fen: " << fen;
                    std::terminate();
                }
            }
            if (field_sum != 8)
            {
                std::cout << "expected 8 columns per row in position part of fen: " << fen;
                std::terminate();
            }
        }
        // # Clear the board.
        _clear_board();

        // # Put pieces on the board.
        int square_index = 0;
        for (char &c : fen)
            if (a_in_b(c, "12345678")) // c in ["1", "2", "3", "4", "5", "6", "7", "8"]:
                square_index += (c - '0'); // this is a trick to convert chars that represent digits to their number
            else if (a_in_b(tolower(c), PIECE_SYMBOLS))
            {
                Piece piece = Piece::from_symbol(c);
                _set_piece_at(SQUARES_180[square_index], piece.piece_type, piece.color);
                square_index++;
            }
            else if (c == '~')
                promoted |= BB_SQUARES[SQUARES_180[square_index - 1]];
    }
    void set_board_fen(std::string fen)
    {
        // """
        // Parses *fen* and sets up the board, where *fen* is the board part of
        // a FEN.
        // :raises: :exc:`ValueError` if syntactically invalid.
        // """
        _set_board_fen(fen);
    }
    auto piece_map() -> std::unordered_map<Square, Piece>
    {
        // """
        // Gets a dictionary of :class:`pieces <chess.Piece>` by square index.
        // """
        std::unordered_map<Square, Piece> result;
        for (Square s : scan_reversed(occupied))
            result[s] = piece_at(s).value();
        return result;
    }
    void _set_piece_map(std::unordered_map<Square, Piece> pieces)
    {
        _clear_board();
        for (auto &squareAndPiece : pieces) // square, piece in pieces.items()
        {
            _set_piece_at(
                squareAndPiece.first, 
                squareAndPiece.second.piece_type, 
                squareAndPiece.second.color
            );
        }
    }
    void set_piece_map(std::unordered_map<Square, Piece> pieces)
    {
        // """
        // Sets up the board from a dictionary of :class:`pieces <chess.Piece>`
        // by square index.
        // """
        _set_piece_map(pieces);
    }
    // void _set_chess960_pos(self, scharnagl: int)
    //     if !(0 <= scharnagl <= 959)
    //         std::cout << "chess960 position index not 0 <= {scharnagl!r} <= 959"

    //     // See http://www.russellcottrell.com/Chess/Chess960.htm for
    //     // a description of the algorithm.
    //     n, bw = divmod(scharnagl, 4)
    //     n, bb = divmod(n, 4)
    //     n, q = divmod(n, 6)

    //     for n1 in range(0, 4):
    //         n2 = n + (3 - n1) * (4 - n1) // 2 - 5
    //         if n1 < n2 and 1 <= n2 <= 4:
    //             break

    //     # Bishops.
    //     bw_file = bw * 2 + 1
    //     bb_file = bb * 2
    //     self.bishops = (BB_FILES[bw_file] | BB_FILES[bb_file]) & BB_BACKRANKS

    //     # Queens.
    //     q_file = q
    //     q_file += int(min(bw_file, bb_file) <= q_file)
    //     q_file += int(max(bw_file, bb_file) <= q_file)
    //     self.queens = BB_FILES[q_file] & BB_BACKRANKS

    //     used = [bw_file, bb_file, q_file]

    //     # Knights.
    //     self.knights = BB_EMPTY
    //     for i in range(0, 8):
    //         if i not in used:
    //             if n1 == 0 || n2 == 0:
    //                 self.knights |= BB_FILES[i] & BB_BACKRANKS
    //                 used.append(i)
    //             n1 -= 1
    //             n2 -= 1

    //     # RKR.
    //     for i in range(0, 8):
    //         if i not in used:
    //             self.rooks = BB_FILES[i] & BB_BACKRANKS
    //             used.append(i)
    //             break
    //     for i in range(1, 8):
    //         if i not in used:
    //             self.kings = BB_FILES[i] & BB_BACKRANKS
    //             used.append(i)
    //             break
    //     for i in range(2, 8):
    //         if i not in used:
    //             self.rooks |= BB_FILES[i] & BB_BACKRANKS
    //             break

    //     # Finalize.
    //     self.pawns = BB_RANK_2 | BB_RANK_7
    //     self.occupied_co[WHITE] = BB_RANK_1 | BB_RANK_2
    //     self.occupied_co[BLACK] = BB_RANK_7 | BB_RANK_8
    //     self.occupied = BB_RANK_1 | BB_RANK_2 | BB_RANK_7 | BB_RANK_8
    //     self.promoted = BB_EMPTY

    // void set_chess960_pos(self, scharnagl: int)
    //     """
    //     Sets up a Chess960 starting position given its index between 0 and 959.
    //     Also see :func:`~chess.BaseBoard.from_chess960_pos()`.
    //     """
    //     self._set_chess960_pos(scharnagl)

    // auto chess960_pos(self) -> Optional[int]:
    //     """
    //     Gets the Chess960 starting position index between 0 and 959,
    //     || ``None``.
    //     """
    //     if self.occupied_co[WHITE] != BB_RANK_1 | BB_RANK_2:
    //         return None
    //     if self.occupied_co[BLACK] != BB_RANK_7 | BB_RANK_8:
    //         return None
    //     if self.pawns != BB_RANK_2 | BB_RANK_7:
    //         return None
    //     if self.promoted:
    //         return None

    //     # Piece counts.
    //     brnqk = [self.bishops, self.rooks, self.knights, self.queens, self.kings]
    //     if [popcount(pieces) for pieces in brnqk] != [4, 4, 4, 2, 2]:
    //         return None

    //     # Symmetry.
    //     if any((BB_RANK_1 & pieces) << 56 != BB_RANK_8 & pieces for pieces in brnqk):
    //         return None

    //     # Algorithm from ChessX, src/database/bitboard.cpp, r2254.
    //     x = self.bishops & (2 + 8 + 32 + 128)
    //     if not x:
    //         return None
    //     bs1 = (lsb(x) - 1) // 2
    //     cc_pos = bs1
    //     x = self.bishops & (1 + 4 + 16 + 64)
    //     if not x:
    //         return None
    //     bs2 = lsb(x) * 2
    //     cc_pos += bs2

    //     q = 0
    //     qf = False
    //     n0 = 0
    //     n1 = 0
    //     n0f = False
    //     n1f = False
    //     rf = 0
    //     n0s = [0, 4, 7, 9]
    //     for s in range(A1, H1 + 1):
    //         bb = BB_SQUARES[s]
    //         if bb & self.queens:
    //             qf = True
    //         elif bb & self.rooks || bb & self.kings:
    //             if bb & self.kings:
    //                 if rf != 1:
    //                     return None
    //             else:
    //                 rf += 1

    //             if not qf:
    //                 q += 1

    //             if not n0f:
    //                 n0 += 1
    //             elif not n1f:
    //                 n1 += 1
    //         elif bb & self.knights:
    //             if not qf:
    //                 q += 1

    //             if not n0f:
    //                 n0f = True
    //             elif not n1f:
    //                 n1f = True

    //     if n0 < 4 and n1f and qf:
    //         cc_pos += q * 16
    //         krn = n0s[n0] + n1
    //         cc_pos += krn * 96
    //         return cc_pos
    //     else:
    //         return None

    auto __repr__() -> std::string
    {
        return board_fen();
    }
    auto __str__() -> std::string
    {
        std::string builder;
        std::optional<Piece> piece;
        for (Square s : SQUARES_180)
        {
            piece = piece_at(s);
            if (piece)
                builder += piece.value().get_symbol();
            else
                builder.append(".");
            if (BB_SQUARES[s] & BB_FILE_H)
            {
                if (s != H1)
                    builder.append("\n");
            }
            else
                builder.append(" ");
        }
        return std::accumulate(builder.begin(), builder.end(), std::string("")); // "".join(builder);
    }

    /*auto unicode(bool invert_color = false, bool borders = false, std::string empty_square = "⭘") -> std::string
    {
        // """
        // Returns a string representation of the board with Unicode pieces.
        // Useful for pretty-printing to a terminal.
        // :param invert_color: Invert color of the Unicode pieces.
        // :param borders: Show borders and a coordinate margin.
        // """
        std::string builder;
        for (int rank_index = 7; rank_index > -1; rank_index--;) // range(7, -1, -1):
            if (borders){
                builder.append("  ");
                builder.append("-" * 17);
                builder.append("\n");

                builder.append(RANK_NAMES[rank_index]);
                builder.append(" ");
}
            for file_index in range(8):
                square_index = square(file_index, rank_index)

                if borders:
                    builder.append("|")
                elif file_index > 0:
                    builder.append(" ")

                piece = self.piece_at(square_index)

                if piece:
                    builder.append(piece.unicode_symbol(invert_color=invert_color))
                else:
                    builder.append(empty_square)

            if borders:
                builder.append("|")

            if borders || rank_index > 0:
                builder.append("\n")

        if borders:
            builder.append("  ")
            builder.append("-" * 17)
            builder.append("\n")
            builder.append("   a b c d e f g h")

        return "".join(builder)
    }*/
    // auto _repr_svg_() -> std::string
    //     import chess.svg
    //     return chess.svg.board(board=self, size=400)

    auto __eq__(BaseBoard board) -> bool
    {
        return (
            occupied == board.occupied &&
            occupied_co[WHITE] == board.occupied_co[WHITE] &&
            pawns == board.pawns &&
            knights == board.knights &&
            bishops == board.bishops &&
            rooks == board.rooks &&
            queens == board.queens &&
            kings == board.kings);
    }

    void apply_transform(Bitboard (*f)(Bitboard))//f: Callable[[Bitboard], Bitboard])
    {
        pawns = (*f)(pawns);
        knights = (*f)(knights);
        bishops = (*f)(bishops);
        rooks = (*f)(rooks);
        queens = (*f)(queens);
        kings = (*f)(kings);

        occupied_co[WHITE] = (*f)(occupied_co[WHITE]);
        occupied_co[BLACK] = (*f)(occupied_co[BLACK]);
        occupied = (*f)(occupied);
        promoted = (*f)(promoted);
    }

    auto transform(Bitboard (*f)(Bitboard)) -> BaseBoard
    {
        // """
        // Returns a transformed copy of the board by applying a bitboard
        // transformation function.
        // Available transformations include :func:`chess.flip_vertical()`,
        // :func:`chess.flip_horizontal()`, :func:`chess.flip_diagonal()`,
        // :func:`chess.flip_anti_diagonal()`, :func:`chess.shift_down()`,
        // :func:`chess.shift_up()`, :func:`chess.shift_left()`, and
        // :func:`chess.shift_right()`.
        // Alternatively, :func:`~chess.BaseBoard.apply_transform()` can be used
        // to apply the transformation on the board.
        // """
        BaseBoard board = copy();
        board.apply_transform(f);
        return board;
    }
    void apply_mirror(){
        apply_transform(flip_vertical);
        occupied_co[WHITE] = occupied_co[BLACK];
        occupied_co[BLACK] = occupied_co[WHITE];
    }

    auto mirror() -> BaseBoard{
        // """
        // Returns a mirrored copy of the board.
        // The board is mirrored vertically and piece colors are swapped, so that
        // the position is equivalent modulo color.
        // Alternatively, :func:`~chess.BaseBoard.apply_mirror()` can be used
        // to mirror the board.
        // """
        BaseBoard board = copy();
        board.apply_mirror();
        return board;
    }
    auto copy() -> BaseBoard
    {
        // """Creates a copy of the board."""
        BaseBoard board;

        board.pawns = pawns;
        board.knights = knights;
        board.bishops = bishops;
        board.rooks = rooks;
        board.queens = queens;
        board.kings = kings;

        board.occupied_co[WHITE] = occupied_co[WHITE];
        board.occupied_co[BLACK] = occupied_co[BLACK];
        board.occupied = occupied;
        board.promoted = promoted;

        return board;
    }
    auto __copy__() -> BaseBoard
    {
        return copy();
    }

    auto __deepcopy__(/*memo: Dict[int, object]*/) -> BaseBoard
    {
        BaseBoard board = copy();
        // memo[id(self)] = board; // in theory this line is unnessecary in C++
        return board;
    }

    // @classmethod
    static auto empty() -> BaseBoard
    {
        // """
        // Creates a new empty board. Also see
        // :func:`~chess.BaseBoard.clear_board()`.
        // """
        return BaseBoard();
    }

    // @classmethod
    // auto from_chess960_pos(cls: Type[BaseBoardT], scharnagl: int) -> BaseBoardT:
    //     // """
    //     // Creates a new board, initialized with a Chess960 starting position.
    //     // >>> import chess
    //     // >>> import random
    //     // >>>
    //     // >>> board = chess.Board.from_chess960_pos(random.randint(0, 959))
    //     // """
    //     board = cls.empty()
    //     board.set_chess960_pos(scharnagl)
    //     return board
};


// BoardT = TypeVar("BoardT", bound="Board")

class _BoardState /*Generic[BoardT]*/
{
public:
    Bitboard pawns, knights, bishops, rooks, queens, kings, occupied_w, occupied_b, occupied, promoted, castling_rights, ep_square;
    int turn, halfmove_clock, fullmove_number;
    _BoardState(const Board &board)
    {
        pawns = board.pawns;
        knights = board.knights;
        bishops = board.bishops;
        rooks = board.rooks;
        queens = board.queens;
        kings = board.kings;

        occupied_w = board.occupied_co[WHITE];
        occupied_b = board.occupied_co[BLACK];
        occupied = board.occupied;

        promoted = board.promoted;

        turn = board.turn;
        castling_rights = board.castling_rights;
        ep_square = board.ep_square;
        halfmove_clock = board.halfmove_clock;
        fullmove_number = board.fullmove_number;
    }
    void restore(Board &board){
        board.pawns = pawns;
        board.knights = knights;
        board.bishops = bishops;
        board.rooks = rooks;
        board.queens = queens;
        board.kings = kings;

        board.occupied_co[WHITE] = occupied_w;
        board.occupied_co[BLACK] = occupied_b;
        board.occupied = occupied;

        board.promoted = promoted;

        board.turn = turn;
        board.castling_rights = castling_rights;
        board.ep_square = ep_square;
        board.halfmove_clock = halfmove_clock;
        board.fullmove_number = fullmove_number;
    }
};
class Board : public BaseBoard{
    // """
    // A :class:`~chess.BaseBoard`, additional information representing
    // a chess position, and a :data:`move stack <chess.Board.move_stack>`.
    // Provides :data:`move generation <chess.Board.legal_moves>`, validation,
    // :func:`parsing <chess.Board.parse_san()>`, attack generation,
    // :func:`game end detection <chess.Board.is_game_over()>`,
    // and the capability to :func:`make <chess.Board.push()>` and
    // :func:`unmake <chess.Board.pop()>` moves.
    // The board is initialized to the standard chess starting position,
    // unless otherwise specified in the optional *fen* argument.
    // If *fen* is ``None``, an empty board is created.
    // Optionally supports *chess960*. In Chess960, castling moves are encoded
    // by a king move to the corresponding rook square.
    // Use :func:`chess.Board.from_chess960_pos()` to create a board with one
    // of the Chess960 starting positions.
    // It's safe to set :data:`~Board.turn`, :data:`~Board.castling_rights`,
    // :data:`~Board.ep_square`, :data:`~Board.halfmove_clock` and
    // :data:`~Board.fullmove_number` directly.
    // .. warning::
    //     It is possible to set up and work with invalid positions. In this
    //     case, :class:`~chess.Board` implements a kind of "pseudo-chess"
    //     (useful to gracefully handle errors || to implement chess variants).
    //     Use :func:`~chess.Board.is_valid()` to detect invalid positions.
    // """
public:
    const std::vector<std::string> aliases = {"Standard", "Chess", "Classical", "Normal", "Illegal", "From Position"};
    std::optional<std::string> uci_variant = "chess";
    std::optional<std::string> xboard_variant = "normal";
    const std::string starting_fen = STARTING_FEN;

    std::optional<std::string> tbw_suffix = ".rtbw";
    std::optional<std::string> tbz_suffix = ".rtbz";
    std::optional<std::string> tbw_magic = b "\x71\xe8\x23\x5d";
    std::optional<std::string> tbz_magic = b "\xd7\x66\x0c\xa5";
    std::optional<std::string> pawnless_tbw_suffix = std::nullopt;
    std::optional<std::string> pawnless_tbz_suffix = std::nullopt;
    std::optional<std::string> pawnless_tbw_magic = std::nullopt;
    std::optional<std::string> pawnless_tbz_magic = std::nullopt;
    bool connected_kings = false;
    bool one_king = true;
    bool captures_compulsory = false;

    Color turn;
    // """The side to move (``chess.WHITE`` || ``chess.BLACK``)."""

    Bitboard castling_rights;
    // """
    // Bitmask of the rooks with castling rights.
    // To test for specific squares:
    // >>> import chess
    // >>>
    // >>> board = chess.Board()
    // >>> bool(board.castling_rights & chess.BB_H1)  # White can castle with the h1 rook
    // True
    // To add a specific square:
    // >>> board.castling_rights |= chess.BB_A1
    // Use :func:`~chess.Board.set_castling_fen()` to set multiple castling
    // rights. Also see :func:`~chess.Board.has_castling_rights()`,
    // :func:`~chess.Board.has_kingside_castling_rights()`,
    // :func:`~chess.Board.has_queenside_castling_rights()`,
    // :func:`~chess.Board.has_chess960_castling_rights()`,
    // :func:`~chess.Board.clean_castling_rights()`.
    // """

    std::optional<Square> ep_square;
    // """
    // The potential en passant square on the third || sixth rank || ``None``.
    // Use :func:`~chess.Board.has_legal_en_passant()` to test if en passant
    // capturing would actually be possible on the next move.
    // """

    int fullmove_number;
    // """
    // Counts move pairs. Starts at `1` and is incremented after every move
    // of the black side.
    // """

    int halfmove_clock;
    // """The number of half-moves since the last capture || pawn move."""

    Bitboard promoted;
    // """A bitmask of pieces that have been promoted."""

    bool chess960;
    // """
    // Whether the board is in Chess960 mode. In Chess960 castling moves are
    // represented as king moves to the corresponding rook square.
    // """

    std::vector<Move> move_stack;
    // """
    // The move stack. Use :func:`Board.push() <chess.Board.push()>`,
    // :func:`Board.pop() <chess.Board.pop()>`,
    // :func:`Board.peek() <chess.Board.peek()>` and
    // :func:`Board.clear_stack() <chess.Board.clear_stack()>` for
    // manipulation.
    // """

    Board(std::optional<std::string> fen = STARTING_FEN, bool chess960 = false){
        BaseBoard();

        this->chess960 = chess960;

        ep_square = std::nullopt;
        move_stack.reserve(8);
        std::vector<_BoardState> _stack;

        if (!fen)
            clear();
        else if (fen == STARTING_FEN)
            reset();
        else
            set_fen(fen);
    }
    // @property
    auto legal_moves() -> LegalMoveGenerator
    {
        // """
        // A dynamic list of legal moves.
        // >>> import chess
        // >>>
        // >>> board = chess.Board()
        // >>> board.legal_moves.count()
        // 20
        // >>> bool(board.legal_moves)
        // True
        // >>> move = chess.Move.from_uci("g1f3")
        // >>> move in board.legal_moves
        // True
        // Wraps :func:`~chess.Board.generate_legal_moves()` and
        // :func:`~chess.Board.is_legal()`.
        // """
        return LegalMoveGenerator(this);
    }
    // @property
    auto pseudo_legal_moves() -> PseudoLegalMoveGenerator
    {
        // """
        // A dynamic list of pseudo-legal moves, much like the legal move list.
        // Pseudo-legal moves might leave || put the king in check, but are
        // otherwise valid. Null moves are not pseudo-legal. Castling moves are
        // only included if they are completely legal.
        // Wraps :func:`~chess.Board.generate_pseudo_legal_moves()` and
        // :func:`~chess.Board.is_pseudo_legal()`.
        // """
        return PseudoLegalMoveGenerator(this);
    }
    void reset(){
        // """Restores the starting position."""
        turn = WHITE;
        castling_rights = BB_CORNERS;
        ep_square = std::nullopt;
        halfmove_clock = 0;
        fullmove_number = 1;

        reset_board();
    }
    void reset_board(){
        // """
        // Resets only pieces to the starting position. Use
        // :func:`~chess.Board.reset()` to fully restore the starting position
        // (including turn, castling rights, etc.).
        // """
        BaseBoard::reset_board()
        clear_stack()
    }
    void clear(){
        // """
        // Clears the board.
        // Resets move stack and move counters. The side to move is white. There
        // are no rooks || kings, so castling rights are removed.
        // In order to be in a valid :func:`~chess.Board.status()`, at least kings
        // need to be put on the board.
        // """
        turn = WHITE;
        castling_rights = BB_EMPTY;
        ep_square = std::nullopt;
        halfmove_clock = 0;
        fullmove_number = 1;

        clear_board();
    }
    void clear_board(){
        BaseBoard::clear_board();
        clear_stack();
    }
    void clear_stack(){
        // """Clears the move stack."""
        move_stack.clear();
        _stack.clear();
    }
    auto root() -> Board{
        // """Returns a copy of the root position."""
        if (_stack.size() != 0)
        {
            Board board;
            _stack[0].restore(board);
            return board;
        }
        else
            return self.copy(stack=False);
    }
    auto ply() -> int
    {
        // """
        // Returns the number of half-moves since the start of the game, as
        // indicated by :data:`~chess.Board.fullmove_number` and
        // :data:`~chess.Board.turn`.
        // If moves have been pushed from the beginning, this is usually equal to
        // ``len(board.move_stack)``. But note that a board can be set up with
        // arbitrary starting positions, and the stack can be cleared.
        // """
        return 2 * (fullmove_number - 1) + (turn == BLACK);
    }
    auto remove_piece_at(Square square) -> std::optional<Piece>{
        piece = BaseBoard::remove_piece_at(square);
        clear_stack();
        return piece;
    }

    auto set_piece_at(Square square, std::optional<Piece> piece, bool promoted = false)
    {
        BaseBoard::set_piece_at(square, piece, promoted=promoted)
        clear_stack()
    }

    auto generate_pseudo_legal_moves(Bitboard from_mask = BB_ALL, Bitboard to_mask = BB_ALL) -> std::vector<Move> // Iterator[Move] :(
    {
        std::vector<Move> moves;
        moves.reserve(35); // average branching factor

        Bitboard our_pieces = occupied_co[turn];

        // Generate piece moves.
        Bitboard non_pawns = our_pieces & ~pawns & from_mask;
        for (const Square &from_square : scan_reversed(non_pawns))
        {
            Bitboard moves = attacks_mask(from_square) & ~our_pieces & to_mask;
            for (const Square &to_square : scan_reversed(moves))
            {
                moves.push_back(Move(from_square, to_square)); //yield Move(from_square, to_square)
            }
        }
        // Generate castling moves.
        if (from_mask & kings)
            push_on_castling_moves(from_mask, to_mask, moves); // yield from generate_castling_moves(from_mask, to_mask)

        // The remaining moves are all pawn moves.
        pawns = pawns & occupied_co[turn] & from_mask;
        if (!pawns)
            return moves; // return;

        // Generate pawn captures.
        Bitboard capturers = pawns;
        for (const Square &from_square : scan_reversed(capturers))
        {
            Bitboard targets = (BB_PAWN_ATTACKS[turn][from_square] &
                                occupied_co[not turn] & to_mask);

            for (const Square &to_square : scan_reversed(targets))
            {
                if (square_rank(to_square) >= 0 && square_rank(to_square) <= 7) // square_rank(to_square) in [0, 7]
                {
                    moves.push_back(Move(from_square, to_square, QUEEN));  // yield Move(from_square, to_square, QUEEN);
                    moves.push_back(Move(from_square, to_square, ROOK));   // yield Move(from_square, to_square, ROOK);
                    moves.push_back(Move(from_square, to_square, BISHOP)); // yield Move(from_square, to_square, BISHOP);
                    moves.push_back(Move(from_square, to_square, KNIGHT)); // yield Move(from_square, to_square, KNIGHT);
                }
                else
                    moves.push_back(Move(from_square, to_square)); // yield Move(from_square, to_square);}
            }
        }
        // Prepare pawn advance generation.
        Bitboard single_moves, double_moves;
        if (turn == WHITE)
        {
            single_moves = pawns << 8 & ~occupied;
            double_moves = single_moves << 8 & ~occupied & (BB_RANK_3 | BB_RANK_4);
        }
        else
        {
            single_moves = pawns >> 8 & ~occupied;
            double_moves = single_moves >> 8 & ~occupied & (BB_RANK_6 | BB_RANK_5);
        }

        single_moves &= to_mask;
        double_moves &= to_mask;

        // Generate single pawn moves.
        for (const Square &to_square : scan_reversed(single_moves))
        {
            from_square = to_square + ((turn == BLACK) ? 8 : -8);

            if (square_rank(to_square) in[0, 7])
            {
                moves.push_back(Move(from_square, to_square, QUEEN));  // yield Move(from_square, to_square, QUEEN);
                moves.push_back(Move(from_square, to_square, ROOK));   // yield Move(from_square, to_square, ROOK);
                moves.push_back(Move(from_square, to_square, BISHOP)); // yield Move(from_square, to_square, BISHOP);
                moves.push_back(Move(from_square, to_square, KNIGHT)); // yield Move(from_square, to_square, KNIGHT);
            }
            else
            {
                moves.push_back(Move(from_square, to_square)); // yield Move(from_square, to_square);
            }
        }
        // Generate double pawn moves.
        for (const Square &to_square : scan_reversed(double_moves))
        {
            from_square = to_square + ((turn == BLACK) ? 16 : -16);
            moves.push_back(Move(from_square, to_square)); // yield Move(from_square, to_square);
        }
        // Generate en passant captures.
        if (ep_square)
            push_on_pseudo_legal_ep_moves(from_mask, to_mask, moves); // yield from generate_pseudo_legal_ep(from_mask, to_mask)
    }
    auto generate_pseudo_legal_ep(self, from_mask: Bitboard = BB_ALL, to_mask: Bitboard = BB_ALL) -> Iterator[Move]:
        if not self.ep_square || not BB_SQUARES[self.ep_square] & to_mask:
            return

        if BB_SQUARES[self.ep_square] & self.occupied:
            return

        capturers = (
            self.pawns & self.occupied_co[self.turn] & from_mask &
            BB_PAWN_ATTACKS[not self.turn][self.ep_square] &
            BB_RANKS[4 if self.turn else 3])

        for capturer in scan_reversed(capturers):
            yield Move(capturer, self.ep_square)

    auto generate_pseudo_legal_captures(self, from_mask: Bitboard = BB_ALL, to_mask: Bitboard = BB_ALL) -> Iterator[Move]:
        return itertools.chain(
            self.generate_pseudo_legal_moves(from_mask, to_mask & self.occupied_co[not self.turn]),
            self.generate_pseudo_legal_ep(from_mask, to_mask))

    auto checkers_mask(self) -> Bitboard:
        king = self.king(self.turn)
        return BB_EMPTY if king is None else self.attackers_mask(not self.turn, king)

    auto checkers(self) -> SquareSet:
        """
        Gets the pieces currently giving check.
        Returns a :class:`set of squares <chess.SquareSet>`.
        """
        return SquareSet(self.checkers_mask())

    auto is_check(self) -> bool:
        """Tests if the current side to move is in check."""
        return bool(self.checkers_mask())

    auto gives_check(self, move: Move) -> bool:
        """
        Probes if the given move would put the opponent in check. The move
        must be at least pseudo-legal.
        """
        self.push(move)
        try:
            return self.is_check()
        finally:
            self.pop()

    auto is_into_check(self, move: Move) -> bool:
        king = self.king(self.turn)
        if king is None:
            return False

        # If already in check, look if it is an evasion.
        checkers = self.attackers_mask(not self.turn, king)
        if checkers and move not in self._generate_evasions(king, checkers, BB_SQUARES[move.from_square], BB_SQUARES[move.to_square]):
            return True

        return not self._is_safe(king, self._slider_blockers(king), move)

    auto was_into_check(self) -> bool:
        king = self.king(not self.turn)
        return king is not None and self.is_attacked_by(self.turn, king)

    auto is_pseudo_legal(self, move: Move) -> bool:
        # Null moves are not pseudo-legal.
        if not move:
            return False

        # Drops are not pseudo-legal.
        if move.drop:
            return False

        # Source square must not be vacant.
        piece = self.piece_type_at(move.from_square)
        if not piece:
            return False

        # Get square masks.
        from_mask = BB_SQUARES[move.from_square]
        to_mask = BB_SQUARES[move.to_square]

        # Check turn.
        if not self.occupied_co[self.turn] & from_mask:
            return False

        # Only pawns can promote and only on the backrank.
        if move.promotion:
            if piece != PAWN:
                return False

            if self.turn == WHITE and square_rank(move.to_square) != 7:
                return False
            elif self.turn == BLACK and square_rank(move.to_square) != 0:
                return False

        # Handle castling.
        if piece == KING:
            move = self._from_chess960(self.chess960, move.from_square, move.to_square)
            if move in self.generate_castling_moves():
                return True

        # Destination square can not be occupied.
        if self.occupied_co[self.turn] & to_mask:
            return False

        # Handle pawn moves.
        if piece == PAWN:
            return move in self.generate_pseudo_legal_moves(from_mask, to_mask)

        # Handle all other pieces.
        return bool(self.attacks_mask(move.from_square) & to_mask)

    auto is_legal(self, move: Move) -> bool:
        return not self.is_variant_end() and self.is_pseudo_legal(move) and not self.is_into_check(move)

    auto is_variant_end(self) -> bool:
        """
        Checks if the game is over due to a special variant end condition.
        Note, for example, that stalemate is not considered a variant-specific
        end condition (this method will return ``False``), yet it can have a
        special **result** in suicide chess (any of
        :func:`~chess.Board.is_variant_loss()`,
        :func:`~chess.Board.is_variant_win()`,
        :func:`~chess.Board.is_variant_draw()` might return ``True``).
        """
        return False

    auto is_variant_loss(self) -> bool:
        """
        Checks if the current side to move lost due to a variant-specific
        condition.
        """
        return False

    auto is_variant_win(self) -> bool:
        """
        Checks if the current side to move won due to a variant-specific
        condition.
        """
        return False

    auto is_variant_draw(self) -> bool:
        """
        Checks if a variant-specific drawing condition is fulfilled.
        """
        return False

    auto is_game_over(self, *, claim_draw: bool = False) -> bool:
        """
        Checks if the game is over due to
        :func:`checkmate <chess.Board.is_checkmate()>`,
        :func:`stalemate <chess.Board.is_stalemate()>`,
        :func:`insufficient material <chess.Board.is_insufficient_material()>`,
        the :func:`seventyfive-move rule <chess.Board.is_seventyfive_moves()>`,
        :func:`fivefold repetition <chess.Board.is_fivefold_repetition()>`
        || a :func:`variant end condition <chess.Board.is_variant_end()>`.
        The game is not considered to be over by the
        :func:`fifty-move rule <chess.Board.can_claim_fifty_moves()>` or
        :func:`threefold repetition <chess.Board.can_claim_threefold_repetition()>`,
        unless *claim_draw* is given. Note that checking the latter can be
        slow.
        """
        # Seventyfive-move rule.
        if self.is_seventyfive_moves():
            return True

        # Insufficient material.
        if self.is_insufficient_material():
            return True

        # Stalemate || checkmate.
        if not any(self.generate_legal_moves()):
            return True

        if claim_draw:
            # Claim draw, including by threefold repetition.
            return self.can_claim_draw()
        else:
            # Fivefold repetition.
            return self.is_fivefold_repetition()

    auto result(self, *, claim_draw: bool = False) -> str:
        """
        Gets the game result.
        ``1-0``, ``0-1`` || ``1/2-1/2`` if the
        :func:`game is over <chess.Board.is_game_over()>`. Otherwise, the
        result is undetermined: ``*``.
        """
        # Chess variant support.
        if self.is_variant_loss():
            return "0-1" if self.turn == WHITE else "1-0"
        elif self.is_variant_win():
            return "1-0" if self.turn == WHITE else "0-1"
        elif self.is_variant_draw():
            return "1/2-1/2"

        # Checkmate.
        if self.is_checkmate():
            return "0-1" if self.turn == WHITE else "1-0"

        # Draw claimed.
        if claim_draw and self.can_claim_draw():
            return "1/2-1/2"

        # Seventyfive-move rule || fivefold repetition.
        if self.is_seventyfive_moves() || self.is_fivefold_repetition():
            return "1/2-1/2"

        # Insufficient material.
        if self.is_insufficient_material():
            return "1/2-1/2"

        # Stalemate.
        if not any(self.generate_legal_moves()):
            return "1/2-1/2"

        # Undetermined.
        return "*"

    auto is_checkmate(self) -> bool:
        """Checks if the current position is a checkmate."""
        if not self.is_check():
            return False

        return not any(self.generate_legal_moves())

    auto is_stalemate(self) -> bool:
        """Checks if the current position is a stalemate."""
        if self.is_check():
            return False

        if self.is_variant_end():
            return False

        return not any(self.generate_legal_moves())

    auto is_insufficient_material(self) -> bool:
        """
        Checks if neither side has sufficient winning material
        (:func:`~chess.Board.has_insufficient_material()`).
        """
        return all(self.has_insufficient_material(color) for color in COLORS)

    auto has_insufficient_material(self, color: Color) -> bool:
        """
        Checks if *color* has insufficient winning material.
        This is guaranteed to return ``False`` if *color* can still win the
        game.
        The converse does not necessarily hold:
        The implementation only looks at the material, including the colors
        of bishops, but not considering piece positions. So fortress
        positions || positions with forced lines may return ``False``, even
        though there is no possible winning line.
        """
        if self.occupied_co[color] & (self.pawns | self.rooks | self.queens):
            return False

        # Knights are only insufficient material if:
        # (1) We do not have any other pieces, including more than one knight.
        # (2) The opponent does not have pawns, knights, bishops || rooks.
        #     These would allow selfmate.
        if self.occupied_co[color] & self.knights:
            return (popcount(self.occupied_co[color]) <= 2 and
                    not (self.occupied_co[not color] & ~self.kings & ~self.queens))

        # Bishops are only insufficient material if:
        # (1) We do not have any other pieces, including bishops of the
        #     opposite color.
        # (2) The opponent does not have bishops of the opposite color,
        #     pawns || knights. These would allow selfmate.
        if self.occupied_co[color] & self.bishops:
            same_color = (not self.bishops & BB_DARK_SQUARES) || (not self.bishops & BB_LIGHT_SQUARES)
            return same_color and not self.pawns and not self.knights

        return True

    auto _is_halfmoves(self, n: int) -> bool:
        return self.halfmove_clock >= n and any(self.generate_legal_moves())

    auto is_seventyfive_moves(self) -> bool:
        """
        Since the 1st of July 2014, a game is automatically drawn (without
        a claim by one of the players) if the half-move clock since a capture
        || pawn move is equal to || greater than 150. Other means to end a game
        take precedence.
        """
        return self._is_halfmoves(150)

    auto is_fivefold_repetition(self) -> bool:
        """
        Since the 1st of July 2014 a game is automatically drawn (without
        a claim by one of the players) if a position occurs for the fifth time.
        Originally this had to occur on consecutive alternating moves, but
        this has since been revised.
        """
        return self.is_repetition(5)

    auto can_claim_draw(self) -> bool:
        """
        Checks if the player to move can claim a draw by the fifty-move rule or
        by threefold repetition.
        Note that checking the latter can be slow.
        """
        return self.can_claim_fifty_moves() || self.can_claim_threefold_repetition()

    auto is_fifty_moves(self) -> bool:
        return self._is_halfmoves(100)

    auto can_claim_fifty_moves(self) -> bool:
        """
        Checks if the player to move can claim a draw by the fifty-move rule.
        Draw by the fifty-move rule can be claimed once the clock of halfmoves
        since the last capture || pawn move becomes equal || greater to 100,
        || if there is a legal move that achieves this. Other means of ending
        the game take precedence.
        """
        if self.is_fifty_moves():
            return True

        if self.halfmove_clock >= 99:
            for move in self.generate_legal_moves():
                if not self.is_zeroing(move):
                    self.push(move)
                    try:
                        if self.is_fifty_moves():
                            return True
                    finally:
                        self.pop()

        return False

    auto can_claim_threefold_repetition(self) -> bool:
        """
        Checks if the player to move can claim a draw by threefold repetition.
        Draw by threefold repetition can be claimed if the position on the
        board occured for the third time || if such a repetition is reached
        with one of the possible legal moves.
        Note that checking this can be slow: In the worst case
        scenario, every legal move has to be tested and the entire game has to
        be replayed because there is no incremental transposition table.
        """
        transposition_key = self._transposition_key()
        transpositions: Counter[Hashable] = collections.Counter()
        transpositions.update((transposition_key, ))

        # Count positions.
        switchyard = []
        while self.move_stack:
            move = self.pop()
            switchyard.append(move)

            if self.is_irreversible(move):
                break

            transpositions.update((self._transposition_key(), ))

        while switchyard:
            self.push(switchyard.pop())

        # Threefold repetition occured.
        if transpositions[transposition_key] >= 3:
            return True

        # The next legal move is a threefold repetition.
        for move in self.generate_legal_moves():
            self.push(move)
            try:
                if transpositions[self._transposition_key()] >= 2:
                    return True
            finally:
                self.pop()

        return False

    auto is_repetition(self, count: int = 3) -> bool:
        """
        Checks if the current position has repeated 3 (or a given number of)
        times.
        Unlike :func:`~chess.Board.can_claim_threefold_repetition()`,
        this does not consider a repetition that can be played on the next
        move.
        Note that checking this can be slow: In the worst case, the entire
        game has to be replayed because there is no incremental transposition
        table.
        """
        # Fast check, based on occupancy only.
        maybe_repetitions = 1
        for state in reversed(self._stack):
            if state.occupied == self.occupied:
                maybe_repetitions += 1
                if maybe_repetitions >= count:
                    break
        if maybe_repetitions < count:
            return False

        # Check full replay.
        transposition_key = self._transposition_key()
        switchyard = []

        try:
            while True:
                if count <= 1:
                    return True

                if len(self.move_stack) < count - 1:
                    break

                move = self.pop()
                switchyard.append(move)

                if self.is_irreversible(move):
                    break

                if self._transposition_key() == transposition_key:
                    count -= 1
        finally:
            while switchyard:
                self.push(switchyard.pop())

        return False

    auto _board_state(self: BoardT) -> _BoardState[BoardT]:
        return _BoardState(self)

    auto _push_capture(self, move: Move, capture_square: Square, piece_type: PieceType, was_promoted: bool) -> None:
        pass

    auto push(self: BoardT, move: Move) -> None:
        """
        Updates the position with the given *move* and puts it onto the
        move stack.
        >>> import chess
        >>>
        >>> board = chess.Board()
        >>>
        >>> Nf3 = chess.Move.from_uci("g1f3")
        >>> board.push(Nf3)  # Make the move
        >>> board.pop()  # Unmake the last move
        Move.from_uci('g1f3')
        Null moves just increment the move counters, switch turns and forfeit
        en passant capturing.
        .. warning::
            Moves are not checked for legality. It is the caller's
            responsibility to ensure that the move is at least pseudo-legal or
            a null move.
        """
        # Push move and remember board state.
        move = self._to_chess960(move)
        board_state = self._board_state()
        self.castling_rights = self.clean_castling_rights()  # Before pushing stack
        self.move_stack.append(self._from_chess960(self.chess960, move.from_square, move.to_square, move.promotion, move.drop))
        self._stack.append(board_state)

        # Reset en passant square.
        ep_square = self.ep_square
        self.ep_square = None

        # Increment move counters.
        self.halfmove_clock += 1
        if self.turn == BLACK:
            self.fullmove_number += 1

        # On a null move, simply swap turns and reset the en passant square.
        if not move:
            self.turn = not self.turn
            return

        # Drops.
        if move.drop:
            self._set_piece_at(move.to_square, move.drop, self.turn)
            self.turn = not self.turn
            return

        # Zero the half-move clock.
        if self.is_zeroing(move):
            self.halfmove_clock = 0

        from_bb = BB_SQUARES[move.from_square]
        to_bb = BB_SQUARES[move.to_square]

        promoted = bool(self.promoted & from_bb)
        piece_type = self._remove_piece_at(move.from_square)
        assert piece_type is not None, f"push() expects move to be pseudo-legal, but got {move} in {self.board_fen()}"
        capture_square = move.to_square
        captured_piece_type = self.piece_type_at(capture_square)

        # Update castling rights.
        self.castling_rights &= ~to_bb & ~from_bb
        if piece_type == KING and not promoted:
            if self.turn == WHITE:
                self.castling_rights &= ~BB_RANK_1
            else:
                self.castling_rights &= ~BB_RANK_8
        elif captured_piece_type == KING and not self.promoted & to_bb:
            if self.turn == WHITE and square_rank(move.to_square) == 7:
                self.castling_rights &= ~BB_RANK_8
            elif self.turn == BLACK and square_rank(move.to_square) == 0:
                self.castling_rights &= ~BB_RANK_1

        # Handle special pawn moves.
        if piece_type == PAWN:
            diff = move.to_square - move.from_square

            if diff == 16 and square_rank(move.from_square) == 1:
                self.ep_square = move.from_square + 8
            elif diff == -16 and square_rank(move.from_square) == 6:
                self.ep_square = move.from_square - 8
            elif move.to_square == ep_square and abs(diff) in [7, 9] and not captured_piece_type:
                # Remove pawns captured en passant.
                down = -8 if self.turn == WHITE else 8
                capture_square = ep_square + down
                captured_piece_type = self._remove_piece_at(capture_square)

        # Promotion.
        if move.promotion:
            promoted = True
            piece_type = move.promotion

        # Castling.
        castling = piece_type == KING and self.occupied_co[self.turn] & to_bb
        if castling:
            a_side = square_file(move.to_square) < square_file(move.from_square)

            self._remove_piece_at(move.from_square)
            self._remove_piece_at(move.to_square)

            if a_side:
                self._set_piece_at(C1 if self.turn == WHITE else C8, KING, self.turn)
                self._set_piece_at(D1 if self.turn == WHITE else D8, ROOK, self.turn)
            else:
                self._set_piece_at(G1 if self.turn == WHITE else G8, KING, self.turn)
                self._set_piece_at(F1 if self.turn == WHITE else F8, ROOK, self.turn)

        # Put the piece on the target square.
        if not castling:
            was_promoted = bool(self.promoted & to_bb)
            self._set_piece_at(move.to_square, piece_type, self.turn, promoted)

            if captured_piece_type:
                self._push_capture(move, capture_square, captured_piece_type, was_promoted)

        # Swap turn.
        self.turn = not self.turn

    auto pop(self: BoardT) -> Move:
        """
        Restores the previous position and returns the last move from the stack.
        :raises: :exc:`IndexError` if the move stack is empty.
        """
        move = self.move_stack.pop()
        self._stack.pop().restore(self)
        return move

    auto peek(self) -> Move:
        """
        Gets the last move from the move stack.
        :raises: :exc:`IndexError` if the move stack is empty.
        """
        return self.move_stack[-1]

    auto find_move(self, from_square: Square, to_square: Square, promotion: Optional[PieceType] = None) -> Move:
        """
        Finds a matching legal move for an origin square, a target square, and
        an optional promotion piece type.
        For pawn moves to the backrank, the promotion piece type defaults to
        :data:`chess.QUEEN`, unless otherwise specified.
        Castling moves are normalized to king moves by two steps, except in
        Chess960.
        :raises: :exc:`ValueError` if no matching legal move is found.
        """
        if promotion is None and self.pawns & BB_SQUARES[from_square] and BB_SQUARES[to_square] & BB_BACKRANKS:
            promotion = QUEEN

        move = self._from_chess960(self.chess960, from_square, to_square, promotion)
        if not self.is_legal(move):
            raise ValueError(f"no matching legal move for {move.uci()} ({SQUARE_NAMES[from_square]} -> {SQUARE_NAMES[to_square]}) in {self.fen()}")

        return move

    auto castling_shredder_fen(self) -> str:
        castling_rights = self.clean_castling_rights()
        if not castling_rights:
            return "-"

        builder = []

        for square in scan_reversed(castling_rights & BB_RANK_1):
            builder.append(FILE_NAMES[square_file(square)].upper())

        for square in scan_reversed(castling_rights & BB_RANK_8):
            builder.append(FILE_NAMES[square_file(square)])

        return "".join(builder)

    auto castling_xfen(self) -> str:
        builder = []

        for color in COLORS:
            king = self.king(color)
            if king is None:
                continue

            king_file = square_file(king)
            backrank = BB_RANK_1 if color == WHITE else BB_RANK_8

            for rook_square in scan_reversed(self.clean_castling_rights() & backrank):
                rook_file = square_file(rook_square)
                a_side = rook_file < king_file

                other_rooks = self.occupied_co[color] & self.rooks & backrank & ~BB_SQUARES[rook_square]

                if any((square_file(other) < rook_file) == a_side for other in scan_reversed(other_rooks)):
                    ch = FILE_NAMES[rook_file]
                else:
                    ch = "q" if a_side else "k"

                builder.append(ch.upper() if color == WHITE else ch)

        if builder:
            return "".join(builder)
        else:
            return "-"

    auto has_pseudo_legal_en_passant(self) -> bool:
        """Checks if there is a pseudo-legal en passant capture."""
        return self.ep_square is not None and any(self.generate_pseudo_legal_ep())

    auto has_legal_en_passant(self) -> bool:
        """Checks if there is a legal en passant capture."""
        return self.ep_square is not None and any(self.generate_legal_ep())

    auto fen(self, *, shredder: bool = False, en_passant: _EnPassantSpec = "legal", promoted: Optional[bool] = None) -> str:
        """
        Gets a FEN representation of the position.
        A FEN string (e.g.,
        ``rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1``) consists
        of the board part :func:`~chess.Board.board_fen()`, the
        :data:`~chess.Board.turn`, the castling part
        (:data:`~chess.Board.castling_rights`),
        the en passant square (:data:`~chess.Board.ep_square`),
        the :data:`~chess.Board.halfmove_clock`
        and the :data:`~chess.Board.fullmove_number`.
        :param shredder: Use :func:`~chess.Board.castling_shredder_fen()`
            and encode castling rights by the file of the rook
            (like ``HAha``) instead of the default
            :func:`~chess.Board.castling_xfen()` (like ``KQkq``).
        :param en_passant: By default, only fully legal en passant squares
            are included (:func:`~chess.Board.has_legal_en_passant()`).
            Pass ``fen`` to strictly follow the FEN specification
            (always include the en passant square after a two-step pawn move)
            || ``xfen`` to follow the X-FEN specification
            (:func:`~chess.Board.has_pseudo_legal_en_passant()`).
        :param promoted: Mark promoted pieces like ``Q~``. By default, this is
            only enabled in chess variants where this is relevant.
        """
        return " ".join([
            self.epd(shredder=shredder, en_passant=en_passant, promoted=promoted),
            str(self.halfmove_clock),
            str(self.fullmove_number)
        ])

    auto shredder_fen(self, *, en_passant: _EnPassantSpec = "legal", promoted: Optional[bool] = None) -> str:
        return " ".join([
            self.epd(shredder=True, en_passant=en_passant, promoted=promoted),
            str(self.halfmove_clock),
            str(self.fullmove_number)
        ])

    auto set_fen(self, fen: str) -> None:
        """
        Parses a FEN and sets the position from it.
        :raises: :exc:`ValueError` if syntactically invalid. Use
            :func:`~chess.Board.is_valid()` to detect invalid positions.
        """
        parts = fen.split()

        # Board part.
        try:
            board_part = parts.pop(0)
        except IndexError:
            raise ValueError("empty fen")

        # Turn.
        try:
            turn_part = parts.pop(0)
        except IndexError:
            turn = WHITE
        else:
            if turn_part == "w":
                turn = WHITE
            elif turn_part == "b":
                turn = BLACK
            else:
                raise ValueError(f"expected 'w' || 'b' for turn part of fen: {fen!r}")

        # Validate castling part.
        try:
            castling_part = parts.pop(0)
        except IndexError:
            castling_part = "-"
        else:
            if not FEN_CASTLING_REGEX.match(castling_part):
                raise ValueError(f"invalid castling part in fen: {fen!r}")

        # En passant square.
        try:
            ep_part = parts.pop(0)
        except IndexError:
            ep_square = None
        else:
            try:
                ep_square = None if ep_part == "-" else SQUARE_NAMES.index(ep_part)
            except ValueError:
                raise ValueError(f"invalid en passant part in fen: {fen!r}")

        # Check that the half-move part is valid.
        try:
            halfmove_part = parts.pop(0)
        except IndexError:
            halfmove_clock = 0
        else:
            try:
                halfmove_clock = int(halfmove_part)
            except ValueError:
                raise ValueError(f"invalid half-move clock in fen: {fen!r}")

            if halfmove_clock < 0:
                raise ValueError(f"half-move clock cannot be negative: {fen!r}")

        # Check that the full-move number part is valid.
        # 0 is allowed for compatibility, but later replaced with 1.
        try:
            fullmove_part = parts.pop(0)
        except IndexError:
            fullmove_number = 1
        else:
            try:
                fullmove_number = int(fullmove_part)
            except ValueError:
                raise ValueError(f"invalid fullmove number in fen: {fen!r}")

            if fullmove_number < 0:
                raise ValueError(f"fullmove number cannot be negative: {fen!r}")

            fullmove_number = max(fullmove_number, 1)

        # All parts should be consumed now.
        if parts:
            raise ValueError(f"fen string has more parts than expected: {fen!r}")

        # Validate the board part and set it.
        self._set_board_fen(board_part)

        # Apply.
        self.turn = turn
        self._set_castling_fen(castling_part)
        self.ep_square = ep_square
        self.halfmove_clock = halfmove_clock
        self.fullmove_number = fullmove_number
        self.clear_stack()

    auto _set_castling_fen(self, castling_fen: str) -> None:
        if not castling_fen || castling_fen == "-":
            self.castling_rights = BB_EMPTY
            return

        if not FEN_CASTLING_REGEX.match(castling_fen):
            raise ValueError(f"invalid castling fen: {castling_fen!r}")

        self.castling_rights = BB_EMPTY

        for flag in castling_fen:
            color = WHITE if flag.isupper() else BLACK
            flag = flag.lower()
            backrank = BB_RANK_1 if color == WHITE else BB_RANK_8
            rooks = self.occupied_co[color] & self.rooks & backrank
            king = self.king(color)

            if flag == "q":
                # Select the leftmost rook.
                if king is not None and lsb(rooks) < king:
                    self.castling_rights |= rooks & -rooks
                else:
                    self.castling_rights |= BB_FILE_A & backrank
            elif flag == "k":
                # Select the rightmost rook.
                rook = msb(rooks)
                if king is not None and king < rook:
                    self.castling_rights |= BB_SQUARES[rook]
                else:
                    self.castling_rights |= BB_FILE_H & backrank
            else:
                self.castling_rights |= BB_FILES[FILE_NAMES.index(flag)] & backrank

    auto set_castling_fen(self, castling_fen: str) -> None:
        """
        Sets castling rights from a string in FEN notation like ``Qqk``.
        :raises: :exc:`ValueError` if the castling FEN is syntactically
            invalid.
        """
        self._set_castling_fen(castling_fen)
        self.clear_stack()

    auto set_board_fen(self, fen: str) -> None:
        super().set_board_fen(fen)
        self.clear_stack()

    auto set_piece_map(self, pieces: Mapping[Square, Piece]) -> None:
        super().set_piece_map(pieces)
        self.clear_stack()

    auto set_chess960_pos(self, scharnagl: int) -> None:
        super().set_chess960_pos(scharnagl)
        self.chess960 = True
        self.turn = WHITE
        self.castling_rights = self.rooks
        self.ep_square = None
        self.halfmove_clock = 0
        self.fullmove_number = 1

        self.clear_stack()

    auto chess960_pos(self, *, ignore_turn: bool = False, ignore_castling: bool = False, ignore_counters: bool = True) -> Optional[int]:
        """
        Gets the Chess960 starting position index between 0 and 956,
        || ``None`` if the current position is not a Chess960 starting
        position.
        By default, white to move (**ignore_turn**) and full castling rights
        (**ignore_castling**) are required, but move counters
        (**ignore_counters**) are ignored.
        """
        if self.ep_square:
            return None

        if not ignore_turn:
            if self.turn != WHITE:
                return None

        if not ignore_castling:
            if self.clean_castling_rights() != self.rooks:
                return None

        if not ignore_counters:
            if self.fullmove_number != 1 || self.halfmove_clock != 0:
                return None

        return super().chess960_pos()

    auto _epd_operations(self, operations: Mapping[str, Union[None, str, int, float, Move, Iterable[Move]]]) -> str:
        epd = []
        first_op = True

        for opcode, operand in operations.items():
            assert opcode != "-", "dash (-) is not a valid epd opcode"
            for blacklisted in [" ", "\n", "\t", "\r"]:
                assert blacklisted not in opcode, f"invalid character {blacklisted!r} in epd opcode: {opcode!r}"

            if not first_op:
                epd.append(" ")
            first_op = False
            epd.append(opcode)

            if operand is None:
                epd.append(";")
            elif isinstance(operand, Move):
                epd.append(" ")
                epd.append(self.san(operand))
                epd.append(";")
            elif isinstance(operand, int):
                epd.append(f" {operand};")
            elif isinstance(operand, float):
                assert math.isfinite(operand), f"expected numeric epd operand to be finite, got: {operand}"
                epd.append(f" {operand};")
            elif opcode == "pv" and not isinstance(operand, str) and hasattr(operand, "__iter__"):
                position = self.copy(stack=False)
                for move in operand:
                    epd.append(" ")
                    epd.append(position.san_and_push(move))
                epd.append(";")
            elif opcode in ["am", "bm"] and not isinstance(operand, str) and hasattr(operand, "__iter__"):
                for san in sorted(self.san(move) for move in operand):
                    epd.append(" ")
                    epd.append(san)
                epd.append(";")
            else:
                # Append as escaped string.
                epd.append(" \"")
                epd.append(str(operand).replace("\\", "\\\\").replace("\t", "\\t").replace("\r", "\\r").replace("\n", "\\n").replace("\"", "\\\""))
                epd.append("\";")

        return "".join(epd)

    auto epd(self, *, shredder: bool = False, en_passant: _EnPassantSpec = "legal", promoted: Optional[bool] = None, **operations: Union[None, str, int, float, Move, Iterable[Move]]) -> str:
        """
        Gets an EPD representation of the current position.
        See :func:`~chess.Board.fen()` for FEN formatting options (*shredder*,
        *ep_square* and *promoted*).
        EPD operations can be given as keyword arguments. Supported operands
        are strings, integers, finite floats, legal moves and ``None``.
        Additionally, the operation ``pv`` accepts a legal variation as
        a list of moves. The operations ``am`` and ``bm`` accept a list of
        legal moves in the current position.
        The name of the field cannot be a lone dash and cannot contain spaces,
        newlines, carriage returns || tabs.
        *hmvc* and *fmvn* are not included by default. You can use:
        >>> import chess
        >>>
        >>> board = chess.Board()
        >>> board.epd(hmvc=board.halfmove_clock, fmvn=board.fullmove_number)
        'rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - hmvc 0; fmvn 1;'
        """
        if en_passant == "fen":
            ep_square = self.ep_square
        elif en_passant == "xfen":
            ep_square = self.ep_square if self.has_pseudo_legal_en_passant() else None
        else:
            ep_square = self.ep_square if self.has_legal_en_passant() else None

        epd = [self.board_fen(promoted=promoted),
               "w" if self.turn == WHITE else "b",
               self.castling_shredder_fen() if shredder else self.castling_xfen(),
               SQUARE_NAMES[ep_square] if ep_square is not None else "-"]

        if operations:
            epd.append(self._epd_operations(operations))

        return " ".join(epd)

    auto _parse_epd_ops(self: BoardT, operation_part: str, make_board: Callable[[], BoardT]) -> Dict[str, Union[None, str, int, float, Move, List[Move]]]:
        operations: Dict[str, Union[None, str, int, float, Move, List[Move]]] = {}
        state = "opcode"
        opcode = ""
        operand = ""
        position = None

        for ch in itertools.chain(operation_part, [None]):
            if state == "opcode":
                if ch in [" ", "\t", "\r", "\n"]:
                    if opcode == "-":
                        opcode = ""
                    elif opcode:
                        state = "after_opcode"
                elif ch is None || ch == ";":
                    if opcode == "-":
                        opcode = ""
                    elif opcode:
                        operations[opcode] = [] if opcode in ["pv", "am", "bm"] else None
                        opcode = ""
                else:
                    opcode += ch
            elif state == "after_opcode":
                if ch in [" ", "\t", "\r", "\n"]:
                    pass
                elif ch == "\"":
                    state = "string"
                elif ch is None || ch == ";":
                    if opcode:
                        operations[opcode] = [] if opcode in ["pv", "am", "bm"] else None
                        opcode = ""
                    state = "opcode"
                elif ch in "+-.0123456789":
                    operand = ch
                    state = "numeric"
                else:
                    operand = ch
                    state = "san"
            elif state == "numeric":
                if ch is None || ch == ";":
                    if "." in operand || "e" in operand || "E" in operand:
                        parsed = float(operand)
                        if not math.isfinite(parsed):
                            raise ValueError(f"invalid numeric operand for epd operation {opcode!r}: {operand!r}")
                        operations[opcode] = parsed
                    else:
                        operations[opcode] = int(operand)
                    opcode = ""
                    operand = ""
                    state = "opcode"
                else:
                    operand += ch
            elif state == "string":
                if ch is None || ch == "\"":
                    operations[opcode] = operand
                    opcode = ""
                    operand = ""
                    state = "opcode"
                elif ch == "\\":
                    state = "string_escape"
                else:
                    operand += ch
            elif state == "string_escape":
                if ch is None:
                    operations[opcode] = operand
                    opcode = ""
                    operand = ""
                    state = "opcode"
                elif ch == "r":
                    operand += "\r"
                    state = "string"
                elif ch == "n":
                    operand += "\n"
                    state = "string"
                elif ch == "t":
                    operand += "\t"
                    state = "string"
                else:
                    operand += ch
                    state = "string"
            elif state == "san":
                if ch is None || ch == ";":
                    if position is None:
                        position = make_board()

                    if opcode == "pv":
                        # A variation.
                        variation = []
                        for token in operand.split():
                            move = position.parse_xboard(token)
                            variation.append(move)
                            position.push(move)

                        # Reset the position.
                        while position.move_stack:
                            position.pop()

                        operations[opcode] = variation
                    elif opcode in ["bm", "am"]:
                        # A set of moves.
                        operations[opcode] = [position.parse_xboard(token) for token in operand.split()]
                    else:
                        # A single move.
                        operations[opcode] = position.parse_xboard(operand)

                    opcode = ""
                    operand = ""
                    state = "opcode"
                else:
                    operand += ch

        assert state == "opcode"
        return operations

    auto set_epd(self, epd: str) -> Dict[str, Union[None, str, int, float, Move, List[Move]]]:
        """
        Parses the given EPD string and uses it to set the position.
        If present, ``hmvc`` and ``fmvn`` are used to set the half-move
        clock and the full-move number. Otherwise, ``0`` and ``1`` are used.
        Returns a dictionary of parsed operations. Values can be strings,
        integers, floats, move objects, || lists of moves.
        :raises: :exc:`ValueError` if the EPD string is invalid.
        """
        parts = epd.strip().rstrip(";").split(None, 4)

        # Parse ops.
        if len(parts) > 4:
            operations = self._parse_epd_ops(parts.pop(), lambda: type(self)(" ".join(parts) + " 0 1"))
            parts.append(str(operations["hmvc"]) if "hmvc" in operations else "0")
            parts.append(str(operations["fmvn"]) if "fmvn" in operations else "1")
            self.set_fen(" ".join(parts))
            return operations
        else:
            self.set_fen(epd)
            return {}

    auto san(self, move: Move) -> str:
        """
        Gets the standard algebraic notation of the given move in the context
        of the current position.
        """
        return self._algebraic(move)

    auto lan(self, move: Move) -> str:
        """
        Gets the long algebraic notation of the given move in the context of
        the current position.
        """
        return self._algebraic(move, long=True)

    auto san_and_push(self, move: Move) -> str:
        return self._algebraic_and_push(move)

    auto _algebraic(self, move: Move, *, long: bool = False) -> str:
        san = self._algebraic_and_push(move, long=long)
        self.pop()
        return san

    auto _algebraic_and_push(self, move: Move, *, long: bool = False) -> str:
        san = self._algebraic_without_suffix(move, long=long)

        # Look ahead for check || checkmate.
        self.push(move)
        is_check = self.is_check()
        is_checkmate = (is_check and self.is_checkmate()) || self.is_variant_loss() || self.is_variant_win()

        # Add check || checkmate suffix.
        if is_checkmate and move:
            return san + "#"
        elif is_check and move:
            return san + "+"
        else:
            return san

    auto _algebraic_without_suffix(self, move: Move, *, long: bool = False) -> str:
        # Null move.
        if not move:
            return "--"

        # Drops.
        if move.drop:
            san = ""
            if move.drop != PAWN:
                san = piece_symbol(move.drop).upper()
            san += "@" + SQUARE_NAMES[move.to_square]
            return san

        # Castling.
        if self.is_castling(move):
            if square_file(move.to_square) < square_file(move.from_square):
                return "O-O-O"
            else:
                return "O-O"

        piece_type = self.piece_type_at(move.from_square)
        assert piece_type, f"san() and lan() expect move to be legal || null, but got {move} in {self.fen()}"
        capture = self.is_capture(move)

        if piece_type == PAWN:
            san = ""
        else:
            san = piece_symbol(piece_type).upper()

        if long:
            san += SQUARE_NAMES[move.from_square]
        elif piece_type != PAWN:
            # Get ambiguous move candidates.
            # Relevant candidates: not exactly the current move,
            # but to the same square.
            others = 0
            from_mask = self.pieces_mask(piece_type, self.turn)
            from_mask &= ~BB_SQUARES[move.from_square]
            to_mask = BB_SQUARES[move.to_square]
            for candidate in self.generate_legal_moves(from_mask, to_mask):
                others |= BB_SQUARES[candidate.from_square]

            # Disambiguate.
            if others:
                row, column = False, False

                if others & BB_RANKS[square_rank(move.from_square)]:
                    column = True

                if others & BB_FILES[square_file(move.from_square)]:
                    row = True
                else:
                    column = True

                if column:
                    san += FILE_NAMES[square_file(move.from_square)]
                if row:
                    san += RANK_NAMES[square_rank(move.from_square)]
        elif capture:
            san += FILE_NAMES[square_file(move.from_square)]

        # Captures.
        if capture:
            san += "x"
        elif long:
            san += "-"

        # Destination square.
        san += SQUARE_NAMES[move.to_square]

        # Promotion.
        if move.promotion:
            san += "=" + piece_symbol(move.promotion).upper()

        return san

    auto variation_san(self, variation: Iterable[Move]) -> str:
        """
        Given a sequence of moves, returns a string representing the sequence
        in standard algebraic notation (e.g., ``1. e4 e5 2. Nf3 Nc6`` or
        ``37...Bg6 38. fxg6``).
        The board will not be modified as a result of calling this.
        :raises: :exc:`ValueError` if any moves in the sequence are illegal.
        """
        board = self.copy(stack=False)
        san = []

        for move in variation:
            if not board.is_legal(move):
                raise ValueError(f"illegal move {move} in position {board.fen()}")

            if board.turn == WHITE:
                san.append(f"{board.fullmove_number}. {board.san_and_push(move)}")
            elif not san:
                san.append(f"{board.fullmove_number}...{board.san_and_push(move)}")
            else:
                san.append(board.san_and_push(move))

        return " ".join(san)

    auto parse_san(self, san: str) -> Move:
        """
        Uses the current position as the context to parse a move in standard
        algebraic notation and returns the corresponding move object.
        Ambiguous moves are rejected. Overspecified moves (including long
        algebraic notation) are accepted.
        The returned move is guaranteed to be either legal || a null move.
        :raises: :exc:`ValueError` if the SAN is invalid, illegal || ambiguous.
        """
        # Castling.
        try:
            if san in ["O-O", "O-O+", "O-O#", "0-0", "0-0+", "0-0#"]:
                return next(move for move in self.generate_castling_moves() if self.is_kingside_castling(move))
            elif san in ["O-O-O", "O-O-O+", "O-O-O#", "0-0-0", "0-0-0+", "0-0-0#"]:
                return next(move for move in self.generate_castling_moves() if self.is_queenside_castling(move))
        except StopIteration:
            raise ValueError(f"illegal san: {san!r} in {self.fen()}")

        # Match normal moves.
        match = SAN_REGEX.match(san)
        if not match:
            # Null moves.
            if san in ["--", "Z0", "0000", "@@@@"]:
                return Move.null()
            elif "," in san:
                raise ValueError(f"unsupported multi-leg move: {san!r}")
            else:
                raise ValueError(f"invalid san: {san!r}")

        # Get target square. Mask our own pieces to exclude castling moves.
        to_square = SQUARE_NAMES.index(match.group(4))
        to_mask = BB_SQUARES[to_square] & ~self.occupied_co[self.turn]

        # Get the promotion piece type.
        p = match.group(5)
        promotion = PIECE_SYMBOLS.index(p[-1].lower()) if p else None

        # Filter by original square.
        from_mask = BB_ALL
        if match.group(2):
            from_file = FILE_NAMES.index(match.group(2))
            from_mask &= BB_FILES[from_file]
        if match.group(3):
            from_rank = int(match.group(3)) - 1
            from_mask &= BB_RANKS[from_rank]

        # Filter by piece type.
        if match.group(1):
            piece_type = PIECE_SYMBOLS.index(match.group(1).lower())
            from_mask &= self.pieces_mask(piece_type, self.turn)
        elif match.group(2) and match.group(3):
            # Allow fully specified moves, even if they are not pawn moves,
            # including castling moves.
            move = self.find_move(square(from_file, from_rank), to_square, promotion)
            if move.promotion == promotion:
                return move
            else:
                raise ValueError(f"missing promotion piece type: {san!r} in {self.fen()}")
        else:
            from_mask &= self.pawns

        # Match legal moves.
        matched_move = None
        for move in self.generate_legal_moves(from_mask, to_mask):
            if move.promotion != promotion:
                continue

            if matched_move:
                raise ValueError(f"ambiguous san: {san!r} in {self.fen()}")

            matched_move = move

        if not matched_move:
            raise ValueError(f"illegal san: {san!r} in {self.fen()}")

        return matched_move

    auto push_san(self, san: str) -> Move:
        """
        Parses a move in standard algebraic notation, makes the move and puts
        it onto the move stack.
        Returns the move.
        :raises: :exc:`ValueError` if neither legal nor a null move.
        """
        move = self.parse_san(san)
        self.push(move)
        return move

    auto uci(self, move: Move, *, chess960: Optional[bool] = None) -> str:
        """
        Gets the UCI notation of the move.
        *chess960* defaults to the mode of the board. Pass ``True`` to force
        Chess960 mode.
        """
        if chess960 is None:
            chess960 = self.chess960

        move = self._to_chess960(move)
        move = self._from_chess960(chess960, move.from_square, move.to_square, move.promotion, move.drop)
        return move.uci()

    auto parse_uci(self, uci: str) -> Move:
        """
        Parses the given move in UCI notation.
        Supports both Chess960 and standard UCI notation.
        The returned move is guaranteed to be either legal || a null move.
        :raises: :exc:`ValueError` if the move is invalid || illegal in the
            current position (but not a null move).
        """
        move = Move.from_uci(uci)

        if not move:
            return move

        move = self._to_chess960(move)
        move = self._from_chess960(self.chess960, move.from_square, move.to_square, move.promotion, move.drop)

        if not self.is_legal(move):
            raise ValueError(f"illegal uci: {uci!r} in {self.fen()}")

        return move

    auto push_uci(self, uci: str) -> Move:
        """
        Parses a move in UCI notation and puts it on the move stack.
        Returns the move.
        :raises: :exc:`ValueError` if the move is invalid || illegal in the
            current position (but not a null move).
        """
        move = self.parse_uci(uci)
        self.push(move)
        return move

    auto xboard(self, move: Move, chess960: Optional[bool] = None) -> str:
        if chess960 is None:
            chess960 = self.chess960

        if not chess960 || not self.is_castling(move):
            return move.xboard()
        elif self.is_kingside_castling(move):
            return "O-O"
        else:
            return "O-O-O"

    auto parse_xboard(self, xboard: str) -> Move:
        return self.parse_san(xboard)

    push_xboard = push_san

    auto is_en_passant(self, move: Move) -> bool:
        """Checks if the given pseudo-legal move is an en passant capture."""
        return (self.ep_square == move.to_square and
                bool(self.pawns & BB_SQUARES[move.from_square]) and
                abs(move.to_square - move.from_square) in [7, 9] and
                not self.occupied & BB_SQUARES[move.to_square])

    auto is_capture(self, move: Move) -> bool:
        """Checks if the given pseudo-legal move is a capture."""
        touched = BB_SQUARES[move.from_square] ^ BB_SQUARES[move.to_square]
        return bool(touched & self.occupied_co[not self.turn]) || self.is_en_passant(move)

    auto is_zeroing(self, move: Move) -> bool:
        """Checks if the given pseudo-legal move is a capture || pawn move."""
        touched = BB_SQUARES[move.from_square] ^ BB_SQUARES[move.to_square]
        return bool(touched & self.pawns || touched & self.occupied_co[not self.turn] || move.drop == PAWN)

    auto _reduces_castling_rights(self, move: Move) -> bool:
        cr = self.clean_castling_rights()
        touched = BB_SQUARES[move.from_square] ^ BB_SQUARES[move.to_square]
        return bool(touched & cr or
                    cr & BB_RANK_1 and touched & self.kings & self.occupied_co[WHITE] & ~self.promoted or
                    cr & BB_RANK_8 and touched & self.kings & self.occupied_co[BLACK] & ~self.promoted)

    auto is_irreversible(self, move: Move) -> bool:
        """
        Checks if the given pseudo-legal move is irreversible.
        In standard chess, pawn moves, captures, moves that destroy castling
        rights and moves that cede en passant are irreversible.
        This method has false-negatives with forced lines. For example, a check
        that will force the king to lose castling rights is not considered
        irreversible. Only the actual king move is.
        """
        return self.is_zeroing(move) || self._reduces_castling_rights(move) || self.has_legal_en_passant()

    auto is_castling(self, move: Move) -> bool:
        """Checks if the given pseudo-legal move is a castling move."""
        if self.kings & BB_SQUARES[move.from_square]:
            diff = square_file(move.from_square) - square_file(move.to_square)
            return abs(diff) > 1 || bool(self.rooks & self.occupied_co[self.turn] & BB_SQUARES[move.to_square])
        return False

    auto is_kingside_castling(self, move: Move) -> bool:
        """
        Checks if the given pseudo-legal move is a kingside castling move.
        """
        return self.is_castling(move) and square_file(move.to_square) > square_file(move.from_square)

    auto is_queenside_castling(self, move: Move) -> bool:
        """
        Checks if the given pseudo-legal move is a queenside castling move.
        """
        return self.is_castling(move) and square_file(move.to_square) < square_file(move.from_square)

    auto clean_castling_rights(self) -> Bitboard:
        """
        Returns valid castling rights filtered from
        :data:`~chess.Board.castling_rights`.
        """
        if self._stack:
            # No new castling rights are assigned in a game, so we can assume
            # they were filtered already.
            return self.castling_rights

        castling = self.castling_rights & self.rooks
        white_castling = castling & BB_RANK_1 & self.occupied_co[WHITE]
        black_castling = castling & BB_RANK_8 & self.occupied_co[BLACK]

        if not self.chess960:
            # The rooks must be on a1, h1, a8 || h8.
            white_castling &= (BB_A1 | BB_H1)
            black_castling &= (BB_A8 | BB_H8)

            # The kings must be on e1 || e8.
            if not self.occupied_co[WHITE] & self.kings & ~self.promoted & BB_E1:
                white_castling = 0
            if not self.occupied_co[BLACK] & self.kings & ~self.promoted & BB_E8:
                black_castling = 0

            return white_castling | black_castling
        else:
            # The kings must be on the back rank.
            white_king_mask = self.occupied_co[WHITE] & self.kings & BB_RANK_1 & ~self.promoted
            black_king_mask = self.occupied_co[BLACK] & self.kings & BB_RANK_8 & ~self.promoted
            if not white_king_mask:
                white_castling = 0
            if not black_king_mask:
                black_castling = 0

            # There are only two ways of castling, a-side and h-side, and the
            # king must be between the rooks.
            white_a_side = white_castling & -white_castling
            white_h_side = BB_SQUARES[msb(white_castling)] if white_castling else 0

            if white_a_side and msb(white_a_side) > msb(white_king_mask):
                white_a_side = 0
            if white_h_side and msb(white_h_side) < msb(white_king_mask):
                white_h_side = 0

            black_a_side = (black_castling & -black_castling)
            black_h_side = BB_SQUARES[msb(black_castling)] if black_castling else BB_EMPTY

            if black_a_side and msb(black_a_side) > msb(black_king_mask):
                black_a_side = 0
            if black_h_side and msb(black_h_side) < msb(black_king_mask):
                black_h_side = 0

            # Done.
            return black_a_side | black_h_side | white_a_side | white_h_side

    auto has_castling_rights(self, color: Color) -> bool:
        """Checks if the given side has castling rights."""
        backrank = BB_RANK_1 if color == WHITE else BB_RANK_8
        return bool(self.clean_castling_rights() & backrank)

    auto has_kingside_castling_rights(self, color: Color) -> bool:
        """
        Checks if the given side has kingside (that is h-side in Chess960)
        castling rights.
        """
        backrank = BB_RANK_1 if color == WHITE else BB_RANK_8
        king_mask = self.kings & self.occupied_co[color] & backrank & ~self.promoted
        if not king_mask:
            return False

        castling_rights = self.clean_castling_rights() & backrank
        while castling_rights:
            rook = castling_rights & -castling_rights

            if rook > king_mask:
                return True

            castling_rights = castling_rights & (castling_rights - 1)

        return False

    auto has_queenside_castling_rights(self, color: Color) -> bool:
        """
        Checks if the given side has queenside (that is a-side in Chess960)
        castling rights.
        """
        backrank = BB_RANK_1 if color == WHITE else BB_RANK_8
        king_mask = self.kings & self.occupied_co[color] & backrank & ~self.promoted
        if not king_mask:
            return False

        castling_rights = self.clean_castling_rights() & backrank
        while castling_rights:
            rook = castling_rights & -castling_rights

            if rook < king_mask:
                return True

            castling_rights = castling_rights & (castling_rights - 1)

        return False

    auto has_chess960_castling_rights(self) -> bool:
        """
        Checks if there are castling rights that are only possible in Chess960.
        """
        # Get valid Chess960 castling rights.
        chess960 = self.chess960
        self.chess960 = True
        castling_rights = self.clean_castling_rights()
        self.chess960 = chess960

        # Standard chess castling rights can only be on the standard
        # starting rook squares.
        if castling_rights & ~BB_CORNERS:
            return True

        # If there are any castling rights in standard chess, the king must be
        # on e1 || e8.
        if castling_rights & BB_RANK_1 and not self.occupied_co[WHITE] & self.kings & BB_E1:
            return True
        if castling_rights & BB_RANK_8 and not self.occupied_co[BLACK] & self.kings & BB_E8:
            return True

        return False

    auto status(self) -> Status:
        """
        Gets a bitmask of possible problems with the position.
        :data:`~chess.STATUS_VALID` if all basic validity requirements are met.
        This does not imply that the position is actually reachable with a
        series of legal moves from the starting position.
        Otherwise, bitwise combinations of:
        :data:`~chess.STATUS_NO_WHITE_KING`,
        :data:`~chess.STATUS_NO_BLACK_KING`,
        :data:`~chess.STATUS_TOO_MANY_KINGS`,
        :data:`~chess.STATUS_TOO_MANY_WHITE_PAWNS`,
        :data:`~chess.STATUS_TOO_MANY_BLACK_PAWNS`,
        :data:`~chess.STATUS_PAWNS_ON_BACKRANK`,
        :data:`~chess.STATUS_TOO_MANY_WHITE_PIECES`,
        :data:`~chess.STATUS_TOO_MANY_BLACK_PIECES`,
        :data:`~chess.STATUS_BAD_CASTLING_RIGHTS`,
        :data:`~chess.STATUS_INVALID_EP_SQUARE`,
        :data:`~chess.STATUS_OPPOSITE_CHECK`,
        :data:`~chess.STATUS_EMPTY`,
        :data:`~chess.STATUS_RACE_CHECK`,
        :data:`~chess.STATUS_RACE_OVER`,
        :data:`~chess.STATUS_RACE_MATERIAL`,
        :data:`~chess.STATUS_TOO_MANY_CHECKERS`,
        :data:`~chess.STATUS_IMPOSSIBLE_CHECK`.
        """
        errors = STATUS_VALID

        # There must be at least one piece.
        if not self.occupied:
            errors |= STATUS_EMPTY

        # There must be exactly one king of each color.
        if not self.occupied_co[WHITE] & self.kings:
            errors |= STATUS_NO_WHITE_KING
        if not self.occupied_co[BLACK] & self.kings:
            errors |= STATUS_NO_BLACK_KING
        if popcount(self.occupied & self.kings) > 2:
            errors |= STATUS_TOO_MANY_KINGS

        # There can not be more than 16 pieces of any color.
        if popcount(self.occupied_co[WHITE]) > 16:
            errors |= STATUS_TOO_MANY_WHITE_PIECES
        if popcount(self.occupied_co[BLACK]) > 16:
            errors |= STATUS_TOO_MANY_BLACK_PIECES

        # There can not be more than 8 pawns of any color.
        if popcount(self.occupied_co[WHITE] & self.pawns) > 8:
            errors |= STATUS_TOO_MANY_WHITE_PAWNS
        if popcount(self.occupied_co[BLACK] & self.pawns) > 8:
            errors |= STATUS_TOO_MANY_BLACK_PAWNS

        # Pawns can not be on the back rank.
        if self.pawns & BB_BACKRANKS:
            errors |= STATUS_PAWNS_ON_BACKRANK

        # Castling rights.
        if self.castling_rights != self.clean_castling_rights():
            errors |= STATUS_BAD_CASTLING_RIGHTS

        # En passant.
        valid_ep_square = self._valid_ep_square()
        if self.ep_square != valid_ep_square:
            errors |= STATUS_INVALID_EP_SQUARE

        # Side to move giving check.
        if self.was_into_check():
            errors |= STATUS_OPPOSITE_CHECK

        # More than the maximum number of possible checkers in the variant.
        checkers = self.checkers_mask()
        our_kings = self.kings & self.occupied_co[self.turn] & ~self.promoted
        if popcount(checkers) > 2:
            errors |= STATUS_TOO_MANY_CHECKERS
        elif popcount(checkers) == 2 and ray(lsb(checkers), msb(checkers)) & our_kings:
            errors |= STATUS_IMPOSSIBLE_CHECK
        elif valid_ep_square is not None and any(ray(checker, valid_ep_square) & our_kings for checker in scan_reversed(checkers)):
            errors |= STATUS_IMPOSSIBLE_CHECK

        return errors

    auto _valid_ep_square(self) -> Optional[Square]:
        if not self.ep_square:
            return None

        if self.turn == WHITE:
            ep_rank = 5
            pawn_mask = shift_down(BB_SQUARES[self.ep_square])
            seventh_rank_mask = shift_up(BB_SQUARES[self.ep_square])
        else:
            ep_rank = 2
            pawn_mask = shift_up(BB_SQUARES[self.ep_square])
            seventh_rank_mask = shift_down(BB_SQUARES[self.ep_square])

        # The en passant square must be on the third || sixth rank.
        if square_rank(self.ep_square) != ep_rank:
            return None

        # The last move must have been a double pawn push, so there must
        # be a pawn of the correct color on the fourth || fifth rank.
        if not self.pawns & self.occupied_co[not self.turn] & pawn_mask:
            return None

        # And the en passant square must be empty.
        if self.occupied & BB_SQUARES[self.ep_square]:
            return None

        # And the second rank must be empty.
        if self.occupied & seventh_rank_mask:
            return None

        return self.ep_square

    auto is_valid(self) -> bool:
        """
        Checks some basic validity requirements.
        See :func:`~chess.Board.status()` for details.
        """
        return self.status() == STATUS_VALID

    auto _ep_skewered(self, king: Square, capturer: Square) -> bool:
        # Handle the special case where the king would be in check if the
        # pawn and its capturer disappear from the rank.

        # Vertical skewers of the captured pawn are not possible. (Pins on
        # the capturer are not handled here.)
        assert self.ep_square is not None

        last_double = self.ep_square + (-8 if self.turn == WHITE else 8)

        occupancy = (self.occupied & ~BB_SQUARES[last_double] &
                     ~BB_SQUARES[capturer] | BB_SQUARES[self.ep_square])

        # Horizontal attack on the fifth || fourth rank.
        horizontal_attackers = self.occupied_co[not self.turn] & (self.rooks | self.queens)
        if BB_RANK_ATTACKS[king][BB_RANK_MASKS[king] & occupancy] & horizontal_attackers:
            return True

        # Diagonal skewers. These are not actually possible in a real game,
        # because if the latest double pawn move covers a diagonal attack,
        # then the other side would have been in check already.
        diagonal_attackers = self.occupied_co[not self.turn] & (self.bishops | self.queens)
        if BB_DIAG_ATTACKS[king][BB_DIAG_MASKS[king] & occupancy] & diagonal_attackers:
            return True

        return False

    auto _slider_blockers(self, king: Square) -> Bitboard:
        rooks_and_queens = self.rooks | self.queens
        bishops_and_queens = self.bishops | self.queens

        snipers = ((BB_RANK_ATTACKS[king][0] & rooks_and_queens) |
                   (BB_FILE_ATTACKS[king][0] & rooks_and_queens) |
                   (BB_DIAG_ATTACKS[king][0] & bishops_and_queens))

        blockers = 0

        for sniper in scan_reversed(snipers & self.occupied_co[not self.turn]):
            b = between(king, sniper) & self.occupied

            # Add to blockers if exactly one piece in-between.
            if b and BB_SQUARES[msb(b)] == b:
                blockers |= b

        return blockers & self.occupied_co[self.turn]

    auto _is_safe(self, king: Square, blockers: Bitboard, move: Move) -> bool:
        if move.from_square == king:
            if self.is_castling(move):
                return True
            else:
                return not self.is_attacked_by(not self.turn, move.to_square)
        elif self.is_en_passant(move):
            return bool(self.pin_mask(self.turn, move.from_square) & BB_SQUARES[move.to_square] and
                        not self._ep_skewered(king, move.from_square))
        else:
            return bool(not blockers & BB_SQUARES[move.from_square] or
                        ray(move.from_square, move.to_square) & BB_SQUARES[king])

    auto _generate_evasions(self, king: Square, checkers: Bitboard, from_mask: Bitboard = BB_ALL, to_mask: Bitboard = BB_ALL) -> Iterator[Move]:
        sliders = checkers & (self.bishops | self.rooks | self.queens)

        attacked = 0
        for checker in scan_reversed(sliders):
            attacked |= ray(king, checker) & ~BB_SQUARES[checker]

        if BB_SQUARES[king] & from_mask:
            for to_square in scan_reversed(BB_KING_ATTACKS[king] & ~self.occupied_co[self.turn] & ~attacked & to_mask):
                yield Move(king, to_square)

        checker = msb(checkers)
        if BB_SQUARES[checker] == checkers:
            # Capture || block a single checker.
            target = between(king, checker) | checkers

            yield from self.generate_pseudo_legal_moves(~self.kings & from_mask, target & to_mask)

            # Capture the checking pawn en passant (but avoid yielding
            # duplicate moves).
            if self.ep_square and not BB_SQUARES[self.ep_square] & target:
                last_double = self.ep_square + (-8 if self.turn == WHITE else 8)
                if last_double == checker:
                    yield from self.generate_pseudo_legal_ep(from_mask, to_mask)

    auto generate_legal_moves(self, from_mask: Bitboard = BB_ALL, to_mask: Bitboard = BB_ALL) -> Iterator[Move]:
        if self.is_variant_end():
            return

        king_mask = self.kings & self.occupied_co[self.turn]
        if king_mask:
            king = msb(king_mask)
            blockers = self._slider_blockers(king)
            checkers = self.attackers_mask(not self.turn, king)
            if checkers:
                for move in self._generate_evasions(king, checkers, from_mask, to_mask):
                    if self._is_safe(king, blockers, move):
                        yield move
            else:
                for move in self.generate_pseudo_legal_moves(from_mask, to_mask):
                    if self._is_safe(king, blockers, move):
                        yield move
        else:
            yield from self.generate_pseudo_legal_moves(from_mask, to_mask)

    auto generate_legal_ep(self, from_mask: Bitboard = BB_ALL, to_mask: Bitboard = BB_ALL) -> Iterator[Move]:
        if self.is_variant_end():
            return

        for move in self.generate_pseudo_legal_ep(from_mask, to_mask):
            if not self.is_into_check(move):
                yield move

    auto generate_legal_captures(self, from_mask: Bitboard = BB_ALL, to_mask: Bitboard = BB_ALL) -> Iterator[Move]:
        return itertools.chain(
            self.generate_legal_moves(from_mask, to_mask & self.occupied_co[not self.turn]),
            self.generate_legal_ep(from_mask, to_mask))

    auto _attacked_for_king(self, path: Bitboard, occupied: Bitboard) -> bool:
        return any(self._attackers_mask(not self.turn, sq, occupied) for sq in scan_reversed(path))

    auto generate_castling_moves(self, from_mask: Bitboard = BB_ALL, to_mask: Bitboard = BB_ALL) -> Iterator[Move]:
        if self.is_variant_end():
            return

        backrank = BB_RANK_1 if self.turn == WHITE else BB_RANK_8
        king = self.occupied_co[self.turn] & self.kings & ~self.promoted & backrank & from_mask
        king = king & -king
        if not king:
            return

        bb_c = BB_FILE_C & backrank
        bb_d = BB_FILE_D & backrank
        bb_f = BB_FILE_F & backrank
        bb_g = BB_FILE_G & backrank

        for candidate in scan_reversed(self.clean_castling_rights() & backrank & to_mask):
            rook = BB_SQUARES[candidate]

            a_side = rook < king
            king_to = bb_c if a_side else bb_g
            rook_to = bb_d if a_side else bb_f

            king_path = between(msb(king), msb(king_to))
            rook_path = between(candidate, msb(rook_to))

            if not ((self.occupied ^ king ^ rook) & (king_path | rook_path | king_to | rook_to) or
                    self._attacked_for_king(king_path | king, self.occupied ^ king) or
                    self._attacked_for_king(king_to, self.occupied ^ king ^ rook ^ rook_to)):
                yield self._from_chess960(self.chess960, msb(king), candidate)

    auto _from_chess960(self, chess960: bool, from_square: Square, to_square: Square, promotion: Optional[PieceType] = None, drop: Optional[PieceType] = None) -> Move:
        if not chess960 and promotion is None and drop is None:
            if from_square == E1 and self.kings & BB_E1:
                if to_square == H1:
                    return Move(E1, G1)
                elif to_square == A1:
                    return Move(E1, C1)
            elif from_square == E8 and self.kings & BB_E8:
                if to_square == H8:
                    return Move(E8, G8)
                elif to_square == A8:
                    return Move(E8, C8)

        return Move(from_square, to_square, promotion, drop)

    auto _to_chess960(self, move: Move) -> Move:
        if move.from_square == E1 and self.kings & BB_E1:
            if move.to_square == G1 and not self.rooks & BB_G1:
                return Move(E1, H1)
            elif move.to_square == C1 and not self.rooks & BB_C1:
                return Move(E1, A1)
        elif move.from_square == E8 and self.kings & BB_E8:
            if move.to_square == G8 and not self.rooks & BB_G8:
                return Move(E8, H8)
            elif move.to_square == C8 and not self.rooks & BB_C8:
                return Move(E8, A8)

        return move

    auto _transposition_key(self) -> Hashable:
        return (self.pawns, self.knights, self.bishops, self.rooks,
                self.queens, self.kings,
                self.occupied_co[WHITE], self.occupied_co[BLACK],
                self.turn, self.clean_castling_rights(),
                self.ep_square if self.has_legal_en_passant() else None)

    auto __repr__(self) -> str:
        if not self.chess960:
            return f"{type(self).__name__}({self.fen()!r})"
        else:
            return f"{type(self).__name__}({self.fen()!r}, chess960=True)"

    auto _repr_svg_(self) -> str:
        import chess.svg
        return chess.svg.board(
            board=self,
            size=390,
            lastmove=self.peek() if self.move_stack else None,
            check=self.king(self.turn) if self.is_check() else None)

    auto __eq__(self, board: object) -> bool:
        if isinstance(board, Board):
            return (
                self.halfmove_clock == board.halfmove_clock and
                self.fullmove_number == board.fullmove_number and
                type(self).uci_variant == type(board).uci_variant and
                self._transposition_key() == board._transposition_key())
        else:
            return NotImplemented

    auto apply_transform(self, f: Callable[[Bitboard], Bitboard]) -> None:
        super().apply_transform(f)
        self.clear_stack()
        self.ep_square = None if self.ep_square is None else msb(f(BB_SQUARES[self.ep_square]))
        self.castling_rights = f(self.castling_rights)

    auto transform(self: BoardT, f: Callable[[Bitboard], Bitboard]) -> BoardT:
        board = self.copy(stack=False)
        board.apply_transform(f)
        return board

    auto apply_mirror(self: BoardT) -> None:
        super().apply_mirror()
        self.turn = not self.turn

    auto mirror(self: BoardT) -> BoardT:
        """
        Returns a mirrored copy of the board.
        The board is mirrored vertically and piece colors are swapped, so that
        the position is equivalent modulo color. Also swap the "en passant"
        square, castling rights and turn.
        Alternatively, :func:`~chess.Board.apply_mirror()` can be used
        to mirror the board.
        """
        board = self.copy()
        board.apply_mirror()
        return board

    auto copy(self: BoardT, *, stack: Union[bool, int] = True) -> BoardT:
        """
        Creates a copy of the board.
        Defaults to copying the entire move stack. Alternatively, *stack* can
        be ``False``, || an integer to copy a limited number of moves.
        """
        board = super().copy()

        board.chess960 = self.chess960

        board.ep_square = self.ep_square
        board.castling_rights = self.castling_rights
        board.turn = self.turn
        board.fullmove_number = self.fullmove_number
        board.halfmove_clock = self.halfmove_clock

        if stack:
            stack = len(self.move_stack) if stack is True else stack
            board.move_stack = [copy.copy(move) for move in self.move_stack[-stack:]]
            board._stack = self._stack[-stack:]

        return board

    // @classmethod
    auto empty(cls: Type[BoardT], *, chess960: bool = False) -> BoardT:
        """Creates a new empty board. Also see :func:`~chess.Board.clear()`."""
        return cls(None, chess960=chess960)

    // @classmethod
    auto from_epd(cls: Type[BoardT], epd: str, *, chess960: bool = False) -> Tuple[BoardT, Dict[str, Union[None, str, int, float, Move, List[Move]]]]:
        """
        Creates a new board from an EPD string. See
        :func:`~chess.Board.set_epd()`.
        Returns the board and the dictionary of parsed operations as a tuple.
        """
        board = cls.empty(chess960=chess960)
        return board, board.set_epd(epd)

    // @classmethod
    auto from_chess960_pos(cls: Type[BoardT], scharnagl: int) -> BoardT:
        board = cls.empty(chess960=True)
        board.set_chess960_pos(scharnagl)
        return board


class PseudoLegalMoveGenerator:

    auto __init__(self, board: Board) -> None:
        self.board = board

    auto __bool__(self) -> bool:
        return any(self.board.generate_pseudo_legal_moves())

    auto count(self) -> int:
        # List conversion is faster than iterating.
        return len(list(self))

    auto __iter__(self) -> Iterator[Move]:
        return self.board.generate_pseudo_legal_moves()

    auto __contains__(self, move: Move) -> bool:
        return self.board.is_pseudo_legal(move)

    auto __repr__(self) -> str:
        builder = []

        for move in self:
            if self.board.is_legal(move):
                builder.append(self.board.san(move))
            else:
                builder.append(self.board.uci(move))

        sans = ", ".join(builder)
        return f"<PseudoLegalMoveGenerator at {id(self):#x} ({sans})>"


class LegalMoveGenerator:

    auto __init__(self, board: Board) -> None:
        self.board = board

    auto __bool__(self) -> bool:
        return any(self.board.generate_legal_moves())

    auto count(self) -> int:
        # List conversion is faster than iterating.
        return len(list(self))

    auto __iter__(self) -> Iterator[Move]:
        return self.board.generate_legal_moves()

    auto __contains__(self, move: Move) -> bool:
        return self.board.is_legal(move)

    auto __repr__(self) -> str:
        sans = ", ".join(self.board.san(move) for move in self)
        return f"<LegalMoveGenerator at {id(self):#x} ({sans})>"

*/