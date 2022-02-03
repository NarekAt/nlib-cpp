#ifndef NLIB_CONCURRENT_THREADS_JOINER_H_
#define NLIB_CONCURRENT_THREADS_JOINER_H_

#include <thread>
#include <vector>

namespace nlib::concurrent {

// A simple RAII mechanism to take care of joining a collection of threads.
class ThreadsJoiner {
 public:
  explicit ThreadsJoiner(std::vector<std::thread>& threads)
      : threads_(threads) {}

  ~ThreadsJoiner() {
    for (auto& t : threads_) {
      if (t.joinable()) {
        t.join();
      }
    }
  }

 private:
  std::vector<std::thread>& threads_;
};

}  // namespace nlib::concurrent

#endif