#ifndef POSITIONMETA_H
#define POSITIONMETA_H

#include "bitboard.h"
#include "square.h"

struct PositionMetaInformation {
  uint8_t m_move_count;
  uint8_t m_fifty_move_rule;
  uint8_t m_castling_and_active_player {};
  Square m_en_passant_square {N_SQUARES};

  Color get_active_player() const {
    return get_bit(m_castling_and_active_player, 7);
  }

  void set_active_player(Color color) {
    if (color) {
      m_castling_and_active_player |= (1 << 7);
    } else {
      m_castling_and_active_player &= ~(1 << 7);
    }
  }

  Square get_en_passant_square() const {
    return m_en_passant_square;
  }

  void set_en_passant_square(Square ep_square) {
    m_en_passant_square = ep_square;
  }

  bool get_castling_right(Color player, Side side) const {
    return m_castling_and_active_player & (1 << (player * 2 + side));
  }

  void set_castling_right(Color player, Side side, bool value) {
    if (value)
      m_castling_and_active_player |= (1 << (player * 2 + side));
    else
      m_castling_and_active_player &= ~(1 << (player * 2 + side));
  }

  uint8_t get_fifty_move_rule() const {
    return m_fifty_move_rule;
  }
  void set_fifty_move_rule(uint8_t p_fifty_move_rule) {
    m_fifty_move_rule = p_fifty_move_rule;
  }

  uint8_t get_move_count() const {
    return m_move_count;
  }
  void set_move_count(uint8_t p_move_count) {
    m_move_count = p_move_count;
  }
};

#endif
