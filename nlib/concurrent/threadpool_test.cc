#include "nlib/concurrent/threadpool.h"

#include <atomic>
#include <thread>

#include "gtest/gtest.h"

namespace nlib::concurrent {
namespace {

struct CounterFn {
  std::atomic_int counter_{0};

  void Increment() { ++counter_; }

  int GetCounter() { return counter_; }
};

TEST(ThreadPoolTest, SimpleTest) {
  for (int attempt = 0; attempt != 10; ++attempt) {
    CounterFn counter_fn;
    std::vector<std::future<void>> futures(5);

    auto func = [&]() { counter_fn.Increment(); };

    {
      ThreadPool pool;

      for (int i = 0; i != 5; ++i) {
        futures[i] = pool.AddTask(func);
      }

      // Wait for all 5 increments to happen.
      for (auto& f : futures) {
        f.get();
      }
    }

    EXPECT_EQ(counter_fn.GetCounter(), 5);
  }
}

}  // namespace
}  // namespace nlib::concurrent