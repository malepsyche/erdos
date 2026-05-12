#include <chrono>
#include <thread>

#include "prism/runtime.hpp"

int main(int argc, char** argv) {
  const char* log_bin_path = argc > 1 ? argv[1] : "/tmp/pipeline_log.bin";
  const char* log_txt_path = argc > 2 ? argv[2] : "/tmp/pipeline_txt.bin";

  euclid::prism::Runtime runtime(log_bin_path, log_txt_path);

  runtime.start(); 

  return 0;
}