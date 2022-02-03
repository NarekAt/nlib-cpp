#ifndef NLIB_CONCURRENT_THREADPOOL_H_
#define NLIB_CONCURRENT_THREADPOOL_H_

#include <functional>
#include <future>
#include <thread>
#include <vector>

#include "nlib/concurrent/threads_joiner.h"
#include "nlib/concurrent/threadsafe_queue.h"

namespace nlib::concurrent {

// A thread pool to submit tasks to a pool of workers.
// The number of threads corresponds to the number of concurrent threads
// supported by hardware.
class ThreadPool {
 public:
  ThreadPool();
  ~ThreadPool();

  template <class FnType, class... ArgsType>
  auto AddTask(FnType&& fn, ArgsType&&... args)
      -> std::future<typename std::result_of<FnType(ArgsType...)>::type>;

  void ShutDown();

  // Get the id of the current thread.
  std::thread::id GetThreadId() const;

 private:
  void WorkerFn();

 private:
  std::atomic_bool done_;
  SyncQueue<std::function<void()>> work_queue_;
  std::vector<std::thread> workers_;
  ThreadsJoiner thread_joiner_;
};

ThreadPool::ThreadPool() : done_(false), thread_joiner_(workers_) {
  const unsigned int num_threads = std::thread::hardware_concurrency();

  try {
    for (unsigned int i = 0; i != num_threads; ++i) {
      workers_.push_back(std::thread(&ThreadPool::WorkerFn, this));
    }
  } catch (...) {
    done_ = true;
    throw;
  }
}

ThreadPool::~ThreadPool() { done_ = true; }

void ThreadPool::WorkerFn() {
  while (!done_) {
    std::function<void()> task;

    if (work_queue_.TryPop(&task)) {
      task();
    } else {
      // give another thread a chance to put some work
      // on the queue before it tries to take some off again the next time
      // around.
      std::this_thread::yield();
    }
  }
}

template <class FnType, class... ArgsType>
auto ThreadPool::AddTask(FnType&& fn, ArgsType&&... args)
    -> std::future<typename std::result_of<FnType(ArgsType...)>::type> {
  using return_t = typename std::result_of<FnType(ArgsType...)>::type;

  auto task = std::make_shared<std::packaged_task<return_t()>>(
      std::bind(std::forward<FnType>(fn), std::forward<ArgsType>(args)...));

  std::future<return_t> result_future(task->get_future());
  // The lambda is required because std::packaged_task<> instances are not
  // copyable, just movable, and std::function<> requires that the stored
  // function objects are copy-constructible.
  work_queue_.push([task]() { (*task)(); });
  return result_future;
}

std::thread::id ThreadPool::GetThreadId() const {
  return std::this_thread::get_id();
}

void ThreadPool::ShutDown() {}

}  // namespace nlib::concurrent

#endif