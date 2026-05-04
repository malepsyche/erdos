#include <chrono>
#include <thread>

#include "spring/runtime.hpp"

int main(int argc, char** argv) {
  const char* file_path = argc > 1 ? argv[1] : "/tmp/pipeline_log.bin";

  euclid::spring::Runtime<EVENT_CAPACITY,
                          EVENT_LOG_CAPACITY,
                          EVENT_LOG_BATCH_SIZE> runtime(file_path);

  runtime.start();
  
  std::this_thread::sleep_for(std::chrono::seconds(5));
  
  runtime.stop();
  runtime.join();
  
  return 0;
}