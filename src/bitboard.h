#ifndef BITBOARD_H
#define BITBOARD_H

#include "defs.h"

inline void set_bit(BB& number, Square index) {
  number |= (1ULL << index);
}

/**
 * get the bit
 * @param number    number to manipulate
 * @param index     index of bit starting at the LST
 * @return          the manipulated number
 */
inline bool get_bit(BB number, Square index) {
  return ((number >> index) & 1ULL) == 1;
}

/**
 * returns the amount of set bits in the given bitboard.
 * @param bb
 * @return
 */
inline int bit_count(BB bb) {
  return __builtin_popcountll(bb);
}

/**
 * counts the ones inside the bitboard before the given index
 */
inline int bit_count(BB bb, int pos) {
  BB mask = ((BB) 1 << pos) - 1;
  return bit_count(bb & mask);
}

/**
 *
 */
template<unsigned N, typename T = BB>
inline T mask() {
  return (T) (((T) 1 << N) - 1);
}

#endif
