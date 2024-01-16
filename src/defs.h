#ifndef DEFS_H
#define DEFS_H

#include <cstdint>

typedef uint64_t Key;
typedef uint64_t BB;
typedef int8_t Square;
typedef int8_t Diagonal;
typedef int8_t AntiDiagonal;
typedef int8_t Direction;

typedef int8_t File;
typedef int8_t Rank;
typedef int8_t Piece;
typedef int8_t PieceType;
typedef bool Side;
typedef bool Color;

enum Format {
  BINARY,
  TEXT
};

enum GameResult {
  WIN  = 1,
  DRAW = 0,
  LOSS = -1
};

struct Result {
  int16_t score;
  int8_t wdl;
};

#endif
