#ifndef DATASET_H
#define DATASET_H

#include <algorithm>
#include <random>
#include <vector>

#include "position.h"

struct Header {
  uint64_t position_count;

  char engine_1[128];
  char engine_2[128];
  char comments[1024];
};

struct DataSet {
  Header header {};
  std::vector<Position> positions {};

  void shuffle() {
    std::shuffle(positions.begin(), positions.end(), std::mt19937(std::random_device()()));
  }
};

#endif
