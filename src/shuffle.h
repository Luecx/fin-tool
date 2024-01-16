#ifndef SHUFFLE_H
#define SHUFFLE_H

#include <cstring>
#include <random>
#include <regex>
#include <vector>

#include "defs.h"
#include "reader.h"
#include "writer.h"

/**
 * shuffles all the files and writes num_files output files.
 * The output files will be generated using the out_format.
 * It assumes the out_format contains at least one "$" (dollar sign) which will be replaced
 * with a number ranging from 1 to num_files
 * @param files
 * @param out_format
 * @param num_files
 */
inline void mix_and_shuffle(std::vector<std::string>& files,
                            const std::string out_format,
                            const size_t num_files = 64) {
  std::vector<FILE*> outfiles {};
  std::vector<size_t> sizes {};

  outfiles.resize(num_files);
  sizes.resize(num_files);

  for (size_t i = 0; i < num_files; i++) {
    // replace the out_format's dollar signs with the index
    std::string file_name = std::regex_replace(out_format, std::regex("\\$"), std::to_string(i + 1));

    // open the file and store it in the outfiles
    FILE* f     = fopen(file_name.c_str(), "wb");
    outfiles[i] = f;
    sizes[i]    = 0;

    // write the header
    Header header {};
    fwrite(&header, sizeof(Header), 1, f);

    std::cout << "Created output file " << file_name << std::endl;
  }

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> distrib(1, num_files);

  // going through each file and writing the output files
  for (std::string s : files) {
    std::cout << "Reading from " << s << std::endl;

    DataSet ds = read<BINARY>(s);

    for (Position& p : ds.positions) {
      size_t idx = distrib(gen) - 1;

      fwrite(&p, sizeof(Position), 1, outfiles[idx]);
      sizes[idx]++;
    }
  }

  for (size_t i = 0; i < num_files; i++) {
    // correcting the size and closing the file
    // seek to the beginning
    fseek(outfiles[i], 0, SEEK_SET);
    // create new header and set position count
    Header header {};
    header.position_count = sizes[i];
    // overwrite the header at the start
    fwrite(&header, sizeof(Header), 1, outfiles[i]);
    // close
    fclose(outfiles[i]);
  }

  // final intra-file shuffling
  for (int i = 0; i < num_files; i++) {
    std::string file_name = std::regex_replace(out_format, std::regex("\\$"), std::to_string(i + 1));
    std::cout << "Shuffling " << file_name << std::endl;

    DataSet ds = read<BINARY>(file_name);
    ds.shuffle();
    write(file_name, ds);
  }
}

#endif
