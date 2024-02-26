#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <tuple>
#include <vector>

#include "argparse.h"
#include "dataset.h"
#include "fenparsing.h"
#include "position.h"

using namespace std;
namespace fs = filesystem;

int main(int argc, char* argv[]) {
  argparse::ArgumentParser program("fin-tool");

  argparse::ArgumentParser counts_cmd("counts");
  counts_cmd.add_description("Get the count of positions in each file");
  counts_cmd.add_argument("files").help("Files to read").remaining();

  argparse::ArgumentParser convert_cmd("convert");
  convert_cmd.add_description(
    "Convert between fen and fin files. Input and output type determined based on output file name (.fin or .fens). "
    "Fen format should be {fen} [result] {search}.");
  convert_cmd.add_argument("-o", "--output").required().help("Output file name. Must contain either '.fin' or '.fens'");
  convert_cmd.add_argument("files").help("Files to convert").remaining();

  argparse::ArgumentParser combine_cmd("combine");
  combine_cmd.add_description("Combine many fin files into one.");
  combine_cmd.add_argument("-o", "--output").required().help("Output file name.");
  combine_cmd.add_argument("files").help("Files to combine").remaining();

  argparse::ArgumentParser shuffle_cmd("shuffle");
  shuffle_cmd.add_description("Shuffle fin files together.");
  shuffle_cmd.add_argument("-o", "--output").required().help("Output file name.");
  shuffle_cmd.add_argument("-t", "--tmp")
    .default_value("/tmp")
    .help("Temporary directory to write files into during shuffling");
  shuffle_cmd.add_argument("files").help("Files to shuffle").remaining();

  program.add_subparser(counts_cmd);
  program.add_subparser(convert_cmd);
  program.add_subparser(combine_cmd);
  program.add_subparser(shuffle_cmd);

  try {
    program.parse_args(argc, argv);
  } catch (const exception& err) {
    cerr << err.what() << endl;
    cerr << program;
    return EXIT_FAILURE;
  }

  /**
   * Counts
   */
  if (program.is_subcommand_used(counts_cmd)) {
    auto inputs = counts_cmd.get<vector<string>>("files");

    Header header {};

    for (const auto& input : inputs) {
      fs::path input_path(input);

      if (!fs::exists(input_path)) {
        cout << input_path << " does not exist, skipping!" << endl;
        continue;
      } else if (fs::is_directory(input_path)) {
        cout << input_path << " is a directory, skipping!" << endl;
        continue;
      }

      ifstream fin(input_path, ios::binary | ios::in);
      fin.read((char*) &header, sizeof(Header));
      fin.close();

      cout << input_path << ": " << header.position_count << endl;
    }
  }
  
  /**
   * Convert files from fen -> fin or fin -> fen
   */
  else if (program.is_subcommand_used(convert_cmd)) {
    auto output_name = convert_cmd.get("--output");
    auto inputs      = convert_cmd.get<vector<string>>("files");

    bool to_bin = (output_name.find(".fin") != string::npos || output_name.find(".bin") != string::npos);
    bool to_fen = (output_name.find(".fens") != string::npos);
    fs::path output_path(output_name);

    // fen -> fin
    if (to_bin) {
      Header out_header {};
      out_header.position_count = 0;

      bool exists = fs::exists(output_path);
      if (exists) {
        ifstream fin(output_path, ios::binary);
        fin.read((char*) &out_header, sizeof(Header));
        fin.close();

        cout << "Output file " << output_name << " exists with " << out_header.position_count << " positions. " << endl;

      } else {
        ofstream fout(output_path, ios::binary);
        fout.write((char*) &out_header, sizeof(Header));
        fout.close();

        cout << "Created new output file " << output_name << endl;
      }

      ofstream fout(output_path, ios::binary | ios::in | ios::out);
      fout.seekp(sizeof(Header) + sizeof(Position) * out_header.position_count);

      for (const auto& input : inputs) {
        fs::path input_path(input);

        if (!fs::exists(input_path)) {
          cout << input_path << " does not exist, skipping!" << endl;
          continue;
        } else if (fs::is_directory(input_path)) {
          cout << input_path << " is a directory, skipping!" << endl;
          continue;
        } else {
          cout << "Reading from " << input_path << endl;
        }

        ifstream fin(input_path);

        // TODO: Buffer the writes
        for (string fen; getline(fin, fen);) {
          auto pos = parse_fen(fen);
          fout.write((char*) &pos, sizeof(Position));
          out_header.position_count++;
        }

        fin.close();
      }

      fout.seekp(0);
      fout.write((char*) &out_header, sizeof(Header));
      fout.close();

      cout << "Successfully converted " << inputs.size() << " file(s) into " << output_name << " ("
           << out_header.position_count << " pos)" << endl;

      return EXIT_SUCCESS;
    } else if (to_fen) {
      cout << "Unimplemented..." << endl;
      return EXIT_FAILURE;
    } else {
      cerr << "Unable to determine input/output type." << endl;
      return EXIT_FAILURE;
    }

  }
  /**
   * Combine fins together
   */
  else if (program.is_subcommand_used(combine_cmd)) {
    auto output_name = combine_cmd.get("--output");
    auto inputs      = combine_cmd.get<vector<string>>("files");

    fs::path output_path(output_name);

    if (fs::exists(output_path)) {
      cerr << "Output file " << output_name << " already exists. Aborting to prevent accidental overwrite." << endl;
      return EXIT_FAILURE;
    }

    Header in_header {};
    Header out_header {};
    out_header.position_count = 0;

    ofstream fout(output_path, ios::binary | ios::out);
    fout.write((char*) &out_header, sizeof(Header));

    for (const auto& input : inputs) {
      fs::path input_path(input);

      if (!fs::exists(input_path)) {
        cout << input_path << " does not exist, skipping!" << endl;
        continue;
      } else if (fs::is_directory(input_path)) {
        cout << input_path << " is a directory, skipping!" << endl;
        continue;
      } else {
        cout << "Reading from " << input_path << endl;
      }

      ifstream fin(input_path, ios::binary | ios::in);
      fin.read((char*) &in_header, sizeof(Header));

      cout << "File contains " << in_header.position_count << " position(s)" << endl;
      out_header.position_count += in_header.position_count;

      fout << fin.rdbuf();
      fin.close();
    }

    fout.seekp(0);
    fout.write((char*) &out_header, sizeof(Header));
    fout.close();

    cout << "Successfully combined " << inputs.size() << " file(s) into " << output_name << " ("
         << out_header.position_count << " pos)" << endl;
    return EXIT_SUCCESS;
  }

  /**
   * Shuffle fin files together
   */
  else if (program.is_subcommand_used(shuffle_cmd)) {
    auto output_name  = shuffle_cmd.get("--output");
    auto tmp_dir_name = shuffle_cmd.get("--tmp");
    auto inputs       = shuffle_cmd.get<vector<string>>("files");

    auto inputs_it = inputs.begin();
    while (inputs_it != inputs.end()) {
      fs::path input_path(*inputs_it);

      if (!fs::exists(input_path) || fs::is_directory(input_path)) {
        cout << input_path << " is invalid, skipping it." << endl;
        inputs_it = inputs.erase(inputs_it);
      } else {
        inputs_it++;
      }
    }

    uint64_t total_positions = 0;
    for (const auto& input : inputs) {
      fs::path input_path(input);

      Header in_header {};
      fstream fin(input_path, ios::binary | ios::in);
      fin.read((char*) &in_header, sizeof(Header));
      total_positions += in_header.position_count;
      fin.close();
    }

    // Hardcoded to 4gb per temp file
    uint64_t total_files = ceil(total_positions / 134217728.0);

    cout << "Shuffling " << total_positions << " across " << inputs.size() << " file(s). Will use " << total_files
         << " temporary files during shuffling..." << endl;

    fs::path tmp_path(tmp_dir_name);
    vector<tuple<fs::path, ofstream, uint64_t>> tmp_files {};

    for (uint64_t i = 0; i < total_files; i++) {
      auto file_name     = "fin-tool-tmp-" + to_string(i);
      fs::path file_path = tmp_path / file_name;
      fs::create_directories(tmp_path);

      tmp_files.emplace_back(file_path, ofstream {file_path, ios::binary | ios::out}, (uint64_t) 0);
      cout << "Created temporary file " << file_path << endl;
    }

    for (auto& [_, fout, __] : tmp_files) {
      Header header {};
      fout.write((char*) &header, sizeof(Header));
    }

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distrib(1, total_files);

    for (const auto& input : inputs) {
      fs::path input_path(input);

      fstream fin(input_path, ios::binary | ios::in);
      Header in_header {};
      fin.read((char*) &in_header, sizeof(Header));

      cout << "Reading from " << input_path << " with " << in_header.position_count << " position(s)" << endl;

      for (uint64_t i = 0; i < in_header.position_count; i++) {
        Position pos {};
        fin.read((char*) &pos, sizeof(Position));

        size_t rand_idx        = distrib(gen) - 1;
        auto& [_, fout, count] = tmp_files[rand_idx];
        fout.write((char*) &pos, sizeof(Position));
        count++;
      }

      fin.close();
    }

    for (auto& [_, fout, __] : tmp_files)
      fout.close();

    fs::path output_path(output_name);
    ofstream fout(output_path, ios::binary | ios::out);

    cout << "Combining temporary files into " << output_path << endl;

    Header header {};
    header.position_count = total_positions;
    fout.write((char*) &header, sizeof(Header));

    for (auto& [file_path, _, count] : tmp_files) {
      ifstream fin(file_path, ios::binary | ios::in);
      fin.seekg(sizeof(Header));

      cout << "Loading " << file_path << " into memory for shuffling. Position count: " << count << endl;

      vector<Position> positions {};
      positions.resize(count);

      fin.read((char*) &positions[0], sizeof(Position) * count);
      fin.close();

      shuffle(positions.begin(), positions.end(), mt19937(random_device()()));

      cout << "Loading + shuffling complete, writing back to disk." << endl;

      fout.write((char*) &positions[0], sizeof(Position) * count);
      fs::remove(file_path);

      cout << "Copying complete. Deleted temporary file " << file_path << endl;
    }

    fout.close();
    cout << "Successfully shuffled " << inputs.size() << " file(s) with " << total_positions << " position(s) into "
         << output_name << endl;
  }
}
