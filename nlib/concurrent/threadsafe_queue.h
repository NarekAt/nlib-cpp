#ifndef NLIB_CONCURRENT_THREADSAFE_QUEUE_H
#define NLIB_CONCURRENT_THREADSAFE_QUEUE_H

#include <cassert>
#include <condition_variable>
#include <functional>
#include <future>
#include <queue>
#include <shared_mutex>

namespace nlib::concurrent {

// This threadsafe queue supports locking only. The basic thread safety is
// provided by protecting each member function with a lock on the mutex.
template <class T>
class SyncQueue {
 public:
  using container_type = std::queue<T>;
  using value_type = typename container_type::value_type;
  using size_type = typename container_type::size_type;

  SyncQueue() = default;

  void push(const value_type& val) {
    std::lock_guard<std::shared_timed_mutex> lock(mut_);
    queue_.push(val);
    data_ready_.notify_one();
  }

  void push(value_type&& val) {
    std::lock_guard<std::shared_timed_mutex> lock(mut_);
    queue_.push(std::move(val));
    data_ready_.notify_one();
  }

  bool empty() const {
    std::shared_lock<std::shared_timed_mutex> lock(mut_);
    return queue_.empty();
  }

  size_type size() const {
    std::shared_lock<std::shared_timed_mutex> lock(mut_);
    return queue_.size();
  }

  void WaitAndPop(value_type* p) {
    std::unique_lock<std::shared_timed_mutex> lock(mut_);
    data_ready_.wait(lock, [this] { return !queue_.empty(); });

    *p = std::move(queue_.front());
    queue_.pop();
  }

  bool TryPop(value_type* p) {
    std::lock_guard<std::shared_timed_mutex> lock(mut_);

    assert(p != nullptr);

    if (queue_.empty()) {
      return false;
    }

    *p = std::move(queue_.front());
    queue_.pop();
    return true;
  }

  bool Front(value_type* p) const {
    std::shared_lock<std::shared_timed_mutex> lock(mut_);
    if (queue_.empty()) {
      return false;
    }

    *p = std::move(queue_.front());
    return true;
  }

 private:
  mutable std::shared_timed_mutex mut_;
  container_type queue_;
  std::condition_variable data_ready_;
};

}  // namespace nlib::concurrent

#endif