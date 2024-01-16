#ifndef POSITION_H
#define POSITION_H

#include "bitboard.h"
#include "defs.h"
#include "piece.h"
#include "piecelist.h"
#include "positionmeta.h"

struct Position {
  PieceList m_pieces {};
  BB m_occupancy {};
  PositionMetaInformation m_meta {};
  Result m_result {};

  int get_piece_count() const {
    return bit_count(m_occupancy);
  }

  Piece get_piece(Square square) const {
    if (get_bit(m_occupancy, square)) {
      return m_pieces.get_piece(bit_count(m_occupancy, square));
    }
    return NO_PIECE;
  }
};

#endif
