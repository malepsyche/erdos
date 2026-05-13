#include <chrono>
#include <iostream>
#include <thread>

#include "spring/runtime.hpp"

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cerr << "usage: runtime_main <log_bin_path> \n";
    return 1;
  }

  const char* log_bin_path = argv[1];

  euclid::spring::Runtime<EVENT_CAPACITY,
                          EVENT_LOG_CAPACITY,
                          EVENT_LOG_BATCH_SIZE> runtime(log_bin_path);

  runtime.start();
  
  std::this_thread::sleep_for(std::chrono::seconds(5));
  
  runtime.stop();
  runtime.join();
  
  return 0;
}