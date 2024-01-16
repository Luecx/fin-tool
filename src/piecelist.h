#ifndef PIECELIST_H
#define PIECELIST_H

#include "defs.h"

#define MAX_PIECES_PER_BOARD (32)
#define PIECES_PER_BUCKET    (64 / 4)
#define MAX_BUCKETS          (MAX_PIECES_PER_BOARD / PIECES_PER_BUCKET)

/**
 * container storing the pieces on the board
 */
struct PieceList {
  BB m_piece_buckets[MAX_BUCKETS] {};

  Piece get_piece(const int index) const {
    // compute the bucket and the offset
    const int bucket = index / PIECES_PER_BUCKET;
    const int offset = index % PIECES_PER_BUCKET;

    // shift the content of the bucket and isolate the last 4 bits
    return (m_piece_buckets[bucket] >> (4 * offset)) & mask<4>();
  }

  void set_piece(const int index, const Piece piece) {
    // compute the bucket and the offset
    const int bucket = index / PIECES_PER_BUCKET;
    const int offset = index % PIECES_PER_BUCKET;

    // compute the shift
    const int shift = 4 * offset;

    // disable all in the correct place
    m_piece_buckets[bucket] &= ~(mask<4>() << shift);

    // set all in the correct place
    m_piece_buckets[bucket] |= ((BB) piece << shift);
  }
};

#endif
