#ifndef WRITER_H
#define WRITER_H

#include <cmath>
#include <string>

#include "dataset.h"
#include "position.h"

inline void write(const std::string& file, const DataSet& data_set, uint64_t count = -1) {
  constexpr uint64_t CHUNK_SIZE = (1 << 20);

  // open the file
  FILE* f = fopen(file.c_str(), "wb");
  if (f == nullptr) {
    return;
  }

  // write the data count
  auto data_to_write = std::min(count, data_set.positions.size());

  // copy the header and replace the data count
  Header header         = data_set.header;
  header.position_count = data_to_write;

  // write the header
  fwrite(&header, sizeof(Header), 1, f);

  // compute how much data to read
  int chunks = std::ceil(data_to_write / (float) CHUNK_SIZE);

  // actually write
  for (int c = 0; c < chunks; c++) {
    int start = c * CHUNK_SIZE;
    int end   = c * CHUNK_SIZE + CHUNK_SIZE;
    if (end > data_set.positions.size())
      end = data_set.positions.size();
    fwrite(&data_set.positions[start], sizeof(Position), end - start, f);
    printf("\r[Writing positions] Current count=%d", end);
    fflush(stdout);
  }
  std::cout << std::endl;

  fclose(f);
}
#endif
