#ifndef READER_H
#define READER_H

#include <sys/stat.h>

#include <cmath>
#include <filesystem>
#include <string>

#include "dataset.h"
#include "defs.h"
#include "fenparsing.h"
#include "position.h"

template<Format format>
inline DataSet read(const std::string& file, uint64_t count = -1) {
  constexpr uint64_t CHUNK_SIZE = (1 << 20);

  // open the file
  FILE* f;
  if (format == BINARY) {
    f = fopen(file.c_str(), "rb");
  } else if (format == TEXT) {
    f = fopen(file.c_str(), "r");
  }

  // create the dataset
  DataSet data_set {};

  // check if opening has worked
  if (f == nullptr) {
    std::cout << "could not open: " << file << std::endl;
    return DataSet {};
  }

  if (format == BINARY) {
    // read the header
    fread(&data_set.header, sizeof(Header), 1, f);

    // compute how much data to read
    auto data_to_read = std::min(count, data_set.header.position_count);
    data_set.positions.resize(data_to_read);
    int chunks = std::ceil(data_to_read / (float) CHUNK_SIZE);

    // actually load
    for (int c = 0; c < chunks; c++) {
      int start = c * CHUNK_SIZE;
      int end   = c * CHUNK_SIZE + CHUNK_SIZE;
      if (end > data_set.positions.size())
        end = data_set.positions.size();
      fread(&data_set.positions[start], sizeof(Position), end - start, f);
      printf("\r[Reading positions] Current count=%d", end);
      fflush(stdout);
    }
    std::cout << std::endl;
  } else if (format == TEXT) {
    int c = 0;
    char buffer[128];
    while (fgets(buffer, 128, f) && (c++) < count) {
      // Remove trailing newline
      buffer[strcspn(buffer, "\n")] = 0;
      if (c % CHUNK_SIZE == 0) {
        printf("\r[Reading positions] Current count=%d", c);
        fflush(stdout);
      }
      data_set.positions.push_back(parse_fen(std::string(buffer)));
    }
    printf("\r[Reading positions] Current count=%d", c - 1);
    fflush(stdout);

    std::cout << std::endl;
  }

  fclose(f);
  return data_set;
}

#endif
