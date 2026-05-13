#include <chrono>
#include <iostream>
#include <thread>

#include "prism/runtime.hpp"

int main(int argc, char** argv) {
  if (argc < 3) {
    std::cerr << "usage: runtime_main <log_bin_path> <log_txt_path>\n";
    return 1;
  }

  const char* log_bin_path = argv[1];
  const char* log_txt_path = argv[2];

  euclid::prism::Runtime runtime(log_bin_path, log_txt_path);

  runtime.start(); 

  return 0;
}