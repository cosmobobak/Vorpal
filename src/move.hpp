#pragma once

#include "names.hpp"

class Move {
    uint m_Move;  // or short or template type

    // first four bits are flags, next 6: from_square, last 6: to_square
    Move(uint from, uint to, uint flags) {
        m_Move = ((flags & 0b1111) << 12) | ((from & 0b111111) << 6) | (to & 0b111111);
    }

    // assignment just takes the integer
    void operator=(Move a) { m_Move = a.m_Move; }

    // returns the to_square from 0->63
    uint get_to() const { return m_Move & 0x3f; }
    // returns the from_square from 0->63
    uint get_from() const { return (m_Move >> 6) & 0x3f; }
    // returns the 4-bit flags
    uint get_flags() const { return (m_Move >> 12) & 0x0f; }

    // as the flags are the MSBs, and they order by promotion, capture, whatever, the internal integer works as a rudimentary ordering key
    uint get_sort_key() const { return m_Move; }

    // set the square that the move is going to
    void set_to(uint to) {
        m_Move &= ~0x3f;
        m_Move |= to & 0x3f;
    }

    // set the square that the move is coming from
    void set_from(uint from) {
        m_Move &= ~0xfc0;
        m_Move |= (from & 0x3f) << 6;
    }

    bool is_capture() const { return m_Move & CAPTURE_FLAG; }
    bool is_promotion() const { return m_Move & PROMOTION_FLAG; }

    bool operator==(Move a) const { return (m_Move & 0xffff) == (a.m_Move & 0xffff); }
    bool operator!=(Move a) const { return (m_Move & 0xffff) != (a.m_Move & 0xffff); }

    unsigned short as_short() const { return (unsigned short)m_Move; }
};

// | code | promotion | capture | special 1 | special 0 | kind of move
// |------|-----------|---------|-----------|-----------|----------------------
// | 0    | 0         | 0       | 0         | 0         | quiet moves
// | 1    | 0         | 0       | 0         | 1         | double pawn push
// | 2    | 0         | 0       | 1         | 0         | king castle
// | 3    | 0         | 0       | 1         | 1         | queen castle
// | 4    | 0         | 1       | 0         | 0         | captures
// | 5    | 0         | 1       | 0         | 1         | ep-capture
// | 8    | 1         | 0       | 0         | 0         | knight-promotion
// | 9    | 1         | 0       | 0         | 1         | bishop-promotion
// | 10   | 1         | 0       | 1         | 0         | rook-promotion
// | 11   | 1         | 0       | 1         | 1         | queen-promotion
// | 12   | 1         | 1       | 0         | 0         | knight-promo capture
// | 13   | 1         | 1       | 0         | 1         | bishop-promo capture
// | 14   | 1         | 1       | 1         | 0         | rook-promo capture
// | 15   | 1         | 1       | 1         | 1         | queen-promo capture
